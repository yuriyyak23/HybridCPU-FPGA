#include "BurstPlanner.h"

namespace YAKSys_Hybrid_CPU::Execution
{

	BurstSegment::BurstSegment(unsigned long long address, int length) : Address(address), Length(length)
	{
	}

	std::vector<BurstSegment> BurstPlanner::Plan(unsigned long long baseAddr, unsigned long long totalBytes)
	{
		std::vector<BurstSegment> segments;
		unsigned long long processed = 0;

		while (processed < totalBytes)
		{
			unsigned long long addr = baseAddr + processed;

			// Calculate offset within current 4KB page
			unsigned long long pageOffset = addr & 0xFFF; // addr % 4096

			// Calculate bytes remaining in current page
			unsigned long long bytesInPage = PAGE_SIZE - pageOffset;

			// Calculate bytes remaining in transfer
			unsigned long long remaining = totalBytes - processed;

			// Segment size is minimum of bytes-in-page and remaining
			int chunk = static_cast<int>(std::min(bytesInPage, remaining));

			// Validate invariant: segment must not cross 4KB boundary
			assert((addr & 0xFFFFF000ULL) == ((addr + static_cast<unsigned long long>(chunk) - 1) & 0xFFFFF000ULL) || chunk == 0);

			// Validate: chunk must be positive and fit in remaining bytes
			assert(chunk > 0 && "Chunk size must be positive");
			assert(static_cast<unsigned long long>(chunk) <= remaining && "Chunk exceeds remaining bytes");

// C# TO C++ CONVERTER TASK: C++ does not have an equivalent to the C# 'yield' keyword:
			segments.emplace_back(addr, chunk);

			processed += static_cast<unsigned long long>(chunk);
		}
		return segments;
	}

	std::vector<BurstSegment> BurstPlanner::Plan2D(unsigned long long baseAddr, unsigned long long elementCount, int elementSize, unsigned int rowLength, unsigned short rowStride, unsigned short colStride)
	{
		std::vector<BurstSegment> segments;
		if (elementCount == 0 || rowLength == 0 || elementSize <= 0)
		{
// C# TO C++ CONVERTER TASK: C++ does not have an equivalent to the C# 'yield' keyword:
			return segments;
		}

		// Check if entire 2D region is contiguous (can treat as 1D)
		if (colStride == elementSize && rowStride == rowLength * elementSize)
		{
			// Fully contiguous - delegate to 1D planner
			unsigned long long totalBytes = elementCount * static_cast<unsigned long long>(elementSize);
			for (auto segment : Plan(baseAddr, totalBytes))
			{
// C# TO C++ CONVERTER TASK: C++ does not have an equivalent to the C# 'yield' keyword:
					segments.push_back(segment);
			}
// C# TO C++ CONVERTER TASK: C++ does not have an equivalent to the C# 'yield' keyword:
			return segments;
		}

		// Check if columns within each row are contiguous
		if (colStride == elementSize)
		{
			// Rows are contiguous but may have gaps between rows
			unsigned long long numRows = (elementCount + rowLength - 1) / rowLength;
			unsigned long long rowBytes = rowLength * static_cast<unsigned long long>(elementSize);

			for (unsigned long long row = 0; row < numRows; row++)
			{
				unsigned long long rowBase = baseAddr + (row * rowStride);
				unsigned long long elementsInRow = std::min(static_cast<unsigned long long>(rowLength), elementCount - (row * rowLength));
				unsigned long long bytesInRow = elementsInRow * static_cast<unsigned long long>(elementSize);

				// Plan bursts for this contiguous row
				for (auto segment : Plan(rowBase, bytesInRow))
				{
// C# TO C++ CONVERTER TASK: C++ does not have an equivalent to the C# 'yield' keyword:
						segments.push_back(segment);
				}
			}
		}
		else
		{
			// Non-contiguous access - return individual element addresses
			// Caller should handle element-by-element access
			for (unsigned long long i = 0; i < elementCount; i++)
			{
				unsigned long long row = i / rowLength;
				unsigned long long col = i % rowLength;
				unsigned long long addr = baseAddr + (row * rowStride) + (col * colStride);
// C# TO C++ CONVERTER TASK: C++ does not have an equivalent to the C# 'yield' keyword:
				segments.emplace_back(addr, elementSize);
			}
		}
		return segments;
	}

	std::vector<BurstSegment> BurstPlanner::PlanIndexed(unsigned long long baseAddr, int elementSize, std::vector<unsigned long long> &indices, bool indexIsByteOffset)
	{
		std::vector<BurstSegment> segments;
		if (indices.empty() || indices.empty() || elementSize <= 0)
		{
// C# TO C++ CONVERTER TASK: C++ does not have an equivalent to the C# 'yield' keyword:
			return segments;
		}

		// For small index counts, use element-by-element (sorting overhead not worth it)
		if (indices.size() < 4)
		{
			for (int i = 0; i < indices.size(); i++)
			{
				unsigned long long offset = indexIsByteOffset ? indices[i] : indices[i] * static_cast<unsigned long long>(elementSize);
				unsigned long long addr = baseAddr + offset;
// C# TO C++ CONVERTER TASK: C++ does not have an equivalent to the C# 'yield' keyword:
				segments.emplace_back(addr, elementSize);
			}
// C# TO C++ CONVERTER TASK: C++ does not have an equivalent to the C# 'yield' keyword:
			return segments;
		}

		// Copy and sort indices to find contiguous runs
		// Note: In a real implementation, we'd track original positions
		// For now, we just optimize the access pattern
		std::vector<unsigned long long> sortedIndices(indices.size());
		std::copy_n(indices.begin(), indices.size(), sortedIndices.begin());
		std::sort(sortedIndices.begin(), sortedIndices.end());

		// Identify contiguous runs and plan bursts
		int runStart = 0;
		for (int i = 1; i <= sortedIndices.size(); i++)
		{
			bool isEndOfRun = (i == sortedIndices.size()) || (sortedIndices[i] != sortedIndices[i - 1] + (indexIsByteOffset ? static_cast<unsigned long long>(elementSize) : 1));

			if (isEndOfRun)
			{
				int runLength = i - runStart;
				unsigned long long startIndex = sortedIndices[runStart];
				unsigned long long startOffset = indexIsByteOffset ? startIndex : startIndex * static_cast<unsigned long long>(elementSize);
				unsigned long long startAddr = baseAddr + startOffset;
				unsigned long long runBytes = static_cast<unsigned long long>(runLength * elementSize);

				if (runLength > 1)
				{
					// Contiguous run - use burst planning
					for (auto segment : Plan(startAddr, runBytes))
					{
// C# TO C++ CONVERTER TASK: C++ does not have an equivalent to the C# 'yield' keyword:
						segments.push_back(segment);
					}
				}
				else
				{
					// Single element
// C# TO C++ CONVERTER TASK: C++ does not have an equivalent to the C# 'yield' keyword:
					segments.emplace_back(startAddr, elementSize);
				}

				runStart = i;
			}
		}
		return segments;
	}

	BurstTiming BurstPlanner::CalculateBurstTiming(unsigned long long address, int size, bool isRead)
	{
		BurstTiming timing = BurstTiming();

		// AXI4 bus width (64 bytes typical for HBM/DDR)
		timing.BeatSize = 64;

		// Header phase (address + control)
		timing.HeaderCycles = 1; // AXI4 pipelined

		// Calculate alignment penalty
		int alignment = static_cast<int>(address % 64); // Assume 64-byte cache line
		if (alignment != 0)
		{
			timing.AlignmentPenalty = CalculateAlignmentPenalty(alignment, size);
		}

		// Data phase
		timing.DataBeats = (size + timing.BeatSize - 1) / timing.BeatSize;
		timing.DataCycles = timing.DataBeats;

		// Account for burst fragmentation across 4KB boundaries
		int fragmentCount = Count4KBFragments(address, size);
		if (fragmentCount > 1)
		{
			// Each fragment adds header + response overhead
			timing.HeaderCycles += (fragmentCount - 1);
			timing.ResponseCycles += (fragmentCount - 1);
		}

		// Response phase
		timing.ResponseCycles += 1; // AXI4 response

		// Total
		timing.TotalCycles = timing.HeaderCycles + timing.DataCycles + timing.ResponseCycles + timing.AlignmentPenalty;

		return timing;
	}

	int BurstPlanner::CalculateAlignmentPenalty(int alignment, int size)
	{
		// Unaligned access may require:
		// 1. Extra read-modify-write for partial cache lines
		// 2. Additional burst for crossing boundaries
		int penalty = 0;

		if (alignment != 0)
		{
			// Base penalty for unaligned access
			penalty = 2; // Read-modify-write overhead

			// Check if transfer crosses 64-byte boundary
			if (alignment + size > 64)
			{
				penalty += 2; // Boundary crossing penalty
			}
		}

		return penalty;
	}

	int BurstPlanner::Count4KBFragments(unsigned long long address, int size)
	{
		unsigned long long endAddress = address + static_cast<unsigned long long>(size);
		unsigned long long startPage = address / 4096;
		unsigned long long endPage = (endAddress - 1) / 4096;
		return static_cast<int>(endPage - startPage + 1);
	}

	int BurstPlanner::CalculateBurstSequenceLatency(std::vector<BurstSegment> &segments, bool isRead)
	{
		int totalCycles = 0;

		for (auto segment : segments)
		{
			BurstTiming timing = CalculateBurstTiming(segment.Address, segment.Length, isRead);
			totalCycles += timing.TotalCycles;
		}

		return totalCycles;
	}
}
