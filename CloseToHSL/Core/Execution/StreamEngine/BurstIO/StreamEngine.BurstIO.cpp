#include "StreamEngine.BurstIO.h"
#include <span>
#include <type_traits>
#include <stdexcept>
#include "../../../../Memory/Subsystem/YAKSys_Hybrid_CPU.Memory.MemorySubsystem.h"
#include "BurstPlanner.h"
#include "../AddressGeneration/StreamEngine.AddressGen.h"

using namespace YAKSys_Hybrid_CPU::Memory;

namespace YAKSys_Hybrid_CPU::Execution
{
	namespace
	{
		template <typename T>
		T ReadLittleEndian(std::span<const unsigned char> bytes)
		{
			static_assert(std::is_unsigned_v<T>);
			if (bytes.size() < sizeof(T))
				throw std::out_of_range("Integer field is shorter than its encoded width.");

			T value = 0;
			for (std::size_t i = 0; i < sizeof(T); ++i)
				value |= static_cast<T>(bytes[i]) << (i * 8);
			return value;
		}

		template <typename T>
		std::span<T> CheckedSubspan(std::span<T> source, std::size_t offset, std::size_t count)
		{
			if (offset > source.size() || count > source.size() - offset)
				throw std::out_of_range("Burst buffer slice is outside the available span.");
			return source.subspan(offset, count);
		}
	}

	std::shared_ptr<Processor::MainMemoryArea> BurstIO::CaptureCurrentMainMemory()
	{
		return Processor::MainMemory;
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: [MethodImpl(MethodImplOptions.AggressiveInlining)] private static YAKSys_Hybrid_CPU.Memory.MemorySubsystem? ResolveActiveMemorySubsystem(YAKSys_Hybrid_CPU.Memory.MemorySubsystem? memSub)
	std::shared_ptr<YAKSys_Hybrid_CPU::Memory::MemorySubsystem> BurstIO::ResolveActiveMemorySubsystem(std::shared_ptr<YAKSys_Hybrid_CPU::Memory::MemorySubsystem> memSub)
	{
		return (memSub != nullptr) ? memSub : Processor::Memory;
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: [MethodImpl(MethodImplOptions.AggressiveInlining)] private static IBurstBackend GetBackend(YAKSys_Hybrid_CPU.Memory.MemorySubsystem? memSub)
	std::shared_ptr<IBurstBackend> BurstIO::GetBackend(std::shared_ptr<YAKSys_Hybrid_CPU::Memory::MemorySubsystem> memSub)
	{
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: YAKSys_Hybrid_CPU.Memory.MemorySubsystem? activeMemorySubsystem = ResolveActiveMemorySubsystem(memSub);
		std::shared_ptr<YAKSys_Hybrid_CPU::Memory::MemorySubsystem> activeMemorySubsystem = ResolveActiveMemorySubsystem(memSub);
		if (activeMemorySubsystem != nullptr)
		{
			return activeMemorySubsystem;
		}
		// Fallback to direct IOMMU backend for backwards compatibility
		return _backend;
	}

	bool BurstIO::HasExactBoundMainMemoryRange(std::shared_ptr<Processor::MainMemoryArea> mainMemory, unsigned long long address, int size)
	{
		if (size <= 0)
		{
			return false;
		}

		unsigned long long memoryLength = static_cast<unsigned long long>(mainMemory->getLength());
		unsigned long long requestSize = static_cast<unsigned long long>(size);
		return requestSize <= memoryLength && address <= memoryLength - requestSize;
	}

	bool BurstIO::TryReadBoundMainMemoryExact(std::shared_ptr<Processor::MainMemoryArea> mainMemory, unsigned long long address, std::span<unsigned char> buffer)
	{
		if (buffer.size() == 0)
		{
			return true;
		}

		return HasExactBoundMainMemoryRange(mainMemory, address, buffer.size()) && mainMemory->TryReadPhysicalRange(address, buffer);
	}

	bool BurstIO::TryWriteBoundMainMemoryExact(std::shared_ptr<Processor::MainMemoryArea> mainMemory, unsigned long long address, std::span<const unsigned char> buffer)
	{
		if (buffer.size() == 0)
		{
			return true;
		}

		return HasExactBoundMainMemoryRange(mainMemory, address, buffer.size()) && mainMemory->TryWritePhysicalRange(address, buffer);
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: [MethodImpl(MethodImplOptions.AggressiveInlining)] internal static bool TryReadThroughActiveBackend(ulong address, Span<byte> buffer, YAKSys_Hybrid_CPU.Memory.MemorySubsystem? memSub = null)
	bool BurstIO::TryReadThroughActiveBackend(unsigned long long address, std::span<unsigned char> buffer, std::shared_ptr<YAKSys_Hybrid_CPU::Memory::MemorySubsystem> memSub)
	{
		if (buffer.size() == 0)
		{
			return true;
		}

		return GetBackend(memSub)->Read(CPU_DEVICE_ID, address, buffer);
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: [MethodImpl(MethodImplOptions.AggressiveInlining)] private static bool TryConsumePrefetchedSrfChunk(ulong baseAddr, Span<byte> buffer, ulong elementCount, int elementSize, ushort stride, YAKSys_Hybrid_CPU.Memory.StreamRegisterFile? srf)
	bool BurstIO::TryConsumePrefetchedSrfChunk(unsigned long long baseAddr, std::span<unsigned char> buffer, unsigned long long elementCount, int elementSize, unsigned short stride, std::shared_ptr<YAKSys_Hybrid_CPU::Memory::StreamRegisterFile> srf)
	{
		if (stride != elementSize || elementCount == 0 || elementSize <= 0 || elementCount > std::numeric_limits<unsigned int>::max() || srf == nullptr)
		{
			return false;
		}

		unsigned long long totalBytes = elementCount * static_cast<unsigned long long>(elementSize);
		if (totalBytes == 0 || totalBytes > static_cast<unsigned long long>(buffer.size()) || totalBytes > std::numeric_limits<int>::max())
		{
			return false;
		}

		return srf->TryReadPrefetchedChunk(baseAddr, static_cast<unsigned char>(elementSize), static_cast<unsigned int>(elementCount), CheckedSubspan(buffer, 0, static_cast<int>(totalBytes)));
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: [MethodImpl(MethodImplOptions.AggressiveInlining)] private static void InvalidatePrefetchedSrfWindow(ulong address, ulong byteCount, YAKSys_Hybrid_CPU.Memory.StreamRegisterFile? srf)
	void BurstIO::InvalidatePrefetchedSrfWindow(unsigned long long address, unsigned long long byteCount, std::shared_ptr<YAKSys_Hybrid_CPU::Memory::StreamRegisterFile> srf)
	{
		if (byteCount == 0 || byteCount > std::numeric_limits<unsigned int>::max() || srf == nullptr)
		{
			return;
		}

		srf->InvalidateOverlappingRange(address, static_cast<unsigned int>(byteCount));
	}

	void BurstIO::ThrowIfUnsupportedZeroRowLength2DTransferContour(const std::wstring &operationName, unsigned long long elementCount, unsigned int rowLength)
	{
		if (elementCount == 0 || rowLength != 0)
		{
			return;
		}

		throw InvalidOperationException(std::format(L"{} reached a non-zero 2D transfer request with rowLength == 0. ", operationName) + L"Shared 2D vector/stream transfer helpers must fail closed instead of collapsing into hidden success/no-op when row geometry is non-representable.");
	}

	void BurstIO::ThrowIfIncompleteTransfer(const std::wstring &operationName, unsigned long long baseAddr, unsigned long long requestedElements, unsigned long long completedElements, int elementSize)
	{
		if (completedElements == requestedElements)
		{
			return;
		}

		throw InvalidOperationException(std::format(L"{} reached a non-zero vector/stream transfer request at base address 0x{:X} ", operationName, baseAddr) + std::format(L"but materialized only {} of {} element(s) with elementSize == {}. ", completedElements, requestedElements, elementSize) + L"Shared BurstIO helpers must fail closed instead of letting raw StreamEngine or authoritative vector carriers continue on partially materialized data, hidden partial success or stale scratch contents.");
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: [MethodImpl(MethodImplOptions.AggressiveInlining)] public static ulong BurstRead(ulong baseAddr, Span<byte> buffer, ulong elementCount, int elementSize, ushort stride, YAKSys_Hybrid_CPU.Memory.MemorySubsystem? memSub = null)
	unsigned long long BurstIO::BurstRead(unsigned long long baseAddr, std::span<unsigned char> buffer, unsigned long long elementCount, int elementSize, unsigned short stride, std::shared_ptr<YAKSys_Hybrid_CPU::Memory::MemorySubsystem> memSub)
	{
		if (elementCount == 0 || elementSize <= 0)
		{
			return 0;
		}

		// Default to packed stride if not specified
		if (stride == 0)
		{
			stride = static_cast<unsigned short>(elementSize);
		}

		// Calculate total transfer size
		unsigned long long totalBytes = elementCount * static_cast<unsigned long long>(elementSize);

		std::shared_ptr<StreamRegisterFile> srf = ResolveActiveMemorySubsystem(memSub) == nullptr ? nullptr : ResolveActiveMemorySubsystem(memSub)->getStreamRegisters();

		if (TryConsumePrefetchedSrfChunk(baseAddr, buffer, elementCount, elementSize, stride, srf))
		{
			return elementCount;
		}

		// For contiguous packed transfers, use BurstPlanner for page splitting
		if (stride == elementSize)
		{
			unsigned long long totalRead = 0;
			std::shared_ptr<IBurstBackend> backend = GetBackend(memSub);

			for (auto segment : BurstPlanner::Plan(baseAddr, totalBytes))
			{
				std::span<unsigned char> slice = CheckedSubspan(buffer, static_cast<int>(totalRead), segment.Length);

				if (!backend->Read(CPU_DEVICE_ID, segment.Address, slice))
				{
					ThrowIfIncompleteTransfer(L"BurstRead", baseAddr, elementCount, totalRead / static_cast<unsigned long long>(elementSize), elementSize);
				}

				totalRead += static_cast<unsigned long long>(segment.Length);
			}

			unsigned long long completedElements = totalRead / static_cast<unsigned long long>(elementSize);
			ThrowIfIncompleteTransfer(L"BurstRead", baseAddr, elementCount, completedElements, elementSize);
			return completedElements;
		}

		// For strided access, fall back to element-by-element reads
		// Future optimization: hardware scatter-gather DMA
		unsigned long long elementsRead = 0;
		unsigned long long bufferOffset = 0;
		std::shared_ptr<IBurstBackend> stridedBackend = GetBackend(memSub);

		for (unsigned long long i = 0; i < elementCount; i++)
		{
			unsigned long long elemAddr = baseAddr + (i * stride);
			std::span<unsigned char> elemSlice = CheckedSubspan(buffer, static_cast<int>(bufferOffset), elementSize);

			if (!stridedBackend->Read(CPU_DEVICE_ID, elemAddr, elemSlice))
			{
				ThrowIfIncompleteTransfer(L"BurstRead", baseAddr, elementCount, elementsRead, elementSize);
			}

			bufferOffset += static_cast<unsigned long long>(elementSize);
			elementsRead++;
		}

		return elementsRead;
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: [MethodImpl(MethodImplOptions.AggressiveInlining)] public static ulong BurstWrite(ulong baseAddr, ReadOnlySpan<byte> buffer, ulong elementCount, int elementSize, ushort stride, YAKSys_Hybrid_CPU.Memory.MemorySubsystem? memSub = null)
	unsigned long long BurstIO::BurstWrite(unsigned long long baseAddr, std::span<const unsigned char> buffer, unsigned long long elementCount, int elementSize, unsigned short stride, std::shared_ptr<YAKSys_Hybrid_CPU::Memory::MemorySubsystem> memSub)
	{
		if (elementCount == 0 || elementSize <= 0)
		{
			return 0;
		}

		// Default to packed stride if not specified
		if (stride == 0)
		{
			stride = static_cast<unsigned short>(elementSize);
		}

		// Calculate total transfer size
		unsigned long long totalBytes = elementCount * static_cast<unsigned long long>(elementSize);

		std::shared_ptr<StreamRegisterFile> srf = ResolveActiveMemorySubsystem(memSub) == nullptr ? nullptr : ResolveActiveMemorySubsystem(memSub)->getStreamRegisters();

		// For contiguous packed transfers, use BurstPlanner for page splitting
		if (stride == elementSize)
		{
			unsigned long long totalWritten = 0;
			std::shared_ptr<IBurstBackend> backend = GetBackend(memSub);

			for (auto segment : BurstPlanner::Plan(baseAddr, totalBytes))
			{
				std::span<const unsigned char> slice = CheckedSubspan(buffer, static_cast<int>(totalWritten), segment.Length);

				if (!backend->Write(CPU_DEVICE_ID, segment.Address, slice))
				{
					ThrowIfIncompleteTransfer(L"BurstWrite", baseAddr, elementCount, totalWritten / static_cast<unsigned long long>(elementSize), elementSize);
				}

				InvalidatePrefetchedSrfWindow(segment.Address, static_cast<unsigned long long>(segment.Length), srf);
				totalWritten += static_cast<unsigned long long>(segment.Length);
			}

			unsigned long long completedElements = totalWritten / static_cast<unsigned long long>(elementSize);
			ThrowIfIncompleteTransfer(L"BurstWrite", baseAddr, elementCount, completedElements, elementSize);
			return completedElements;
		}

		// For strided access, fall back to element-by-element writes
		unsigned long long elementsWritten = 0;
		unsigned long long bufferOffset = 0;
		std::shared_ptr<IBurstBackend> stridedBackend = GetBackend(memSub);

		for (unsigned long long i = 0; i < elementCount; i++)
		{
			unsigned long long elemAddr = baseAddr + (i * stride);
			std::span<const unsigned char> elemSlice = CheckedSubspan(buffer, static_cast<int>(bufferOffset), elementSize);

			if (!stridedBackend->Write(CPU_DEVICE_ID, elemAddr, elemSlice))
			{
				ThrowIfIncompleteTransfer(L"BurstWrite", baseAddr, elementCount, elementsWritten, elementSize);
			}

			InvalidatePrefetchedSrfWindow(elemAddr, static_cast<unsigned long long>(elementSize), srf);
			bufferOffset += static_cast<unsigned long long>(elementSize);
			elementsWritten++;
		}

		return elementsWritten;
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: [MethodImpl(MethodImplOptions.AggressiveInlining)] public static ulong BurstRead2D(ulong baseAddr, Span<byte> buffer, ulong elementCount, int elementSize, uint rowLength, ushort rowStride, ushort colStride, ulong startOffset = 0, YAKSys_Hybrid_CPU.Memory.MemorySubsystem? memSub = null)
	unsigned long long BurstIO::BurstRead2D(unsigned long long baseAddr, std::span<unsigned char> buffer, unsigned long long elementCount, int elementSize, unsigned int rowLength, unsigned short rowStride, unsigned short colStride, unsigned long long startOffset, std::shared_ptr<YAKSys_Hybrid_CPU::Memory::MemorySubsystem> memSub)
	{
		ThrowIfUnsupportedZeroRowLength2DTransferContour(L"BurstRead2D", elementCount, rowLength);

		if (elementCount == 0 || elementSize <= 0)
		{
			return 0;
		}

		std::shared_ptr<StreamRegisterFile> srf = ResolveActiveMemorySubsystem(memSub) == nullptr ? nullptr : ResolveActiveMemorySubsystem(memSub)->getStreamRegisters();
		std::shared_ptr<IBurstBackend> backend = GetBackend(memSub);

		// Check if we can use optimized burst planning
		if (colStride == elementSize)
		{
			// Columns within rows are contiguous - use burst planning
			unsigned long long totalRead = 0;

			for (auto segment : BurstPlanner::Plan2D(baseAddr + (startOffset * colStride), elementCount, elementSize, rowLength, rowStride, colStride))
			{
				std::span<unsigned char> slice = CheckedSubspan(buffer, static_cast<int>(totalRead), segment.Length);

				unsigned long long segmentElements = static_cast<unsigned long long>(segment.Length) / static_cast<unsigned long long>(elementSize);
				if (!TryConsumePrefetchedSrfChunk(segment.Address, slice, segmentElements, elementSize, static_cast<unsigned short>(elementSize), srf) && !backend->Read(CPU_DEVICE_ID, segment.Address, slice))
				{
					ThrowIfIncompleteTransfer(L"BurstRead2D", baseAddr, elementCount, totalRead / static_cast<unsigned long long>(elementSize), elementSize);
				}

				totalRead += static_cast<unsigned long long>(segment.Length);
			}

			unsigned long long completedElements = totalRead / static_cast<unsigned long long>(elementSize);
			ThrowIfIncompleteTransfer(L"BurstRead2D", baseAddr, elementCount, completedElements, elementSize);
			return completedElements;
		}

		// Fall back to element-by-element for non-contiguous columns
		unsigned long long elementsRead = 0;
		unsigned long long bufferOffset = 0;

		for (unsigned long long i = 0; i < elementCount; i++)
		{
			unsigned long long linearIdx = startOffset + i;
			unsigned long long addr = AddressGen::Gen2D(baseAddr, linearIdx, rowLength, rowStride, colStride);
			std::span<unsigned char> elemSlice = CheckedSubspan(buffer, static_cast<int>(bufferOffset), elementSize);

			if (!backend->Read(CPU_DEVICE_ID, addr, elemSlice))
			{
				ThrowIfIncompleteTransfer(L"BurstRead2D", baseAddr, elementCount, elementsRead, elementSize);
			}

			bufferOffset += static_cast<unsigned long long>(elementSize);
			elementsRead++;
		}

		return elementsRead;
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: [MethodImpl(MethodImplOptions.AggressiveInlining)] public static ulong BurstWrite2D(ulong baseAddr, ReadOnlySpan<byte> buffer, ulong elementCount, int elementSize, uint rowLength, ushort rowStride, ushort colStride, ulong startOffset = 0, YAKSys_Hybrid_CPU.Memory.MemorySubsystem? memSub = null)
	unsigned long long BurstIO::BurstWrite2D(unsigned long long baseAddr, std::span<const unsigned char> buffer, unsigned long long elementCount, int elementSize, unsigned int rowLength, unsigned short rowStride, unsigned short colStride, unsigned long long startOffset, std::shared_ptr<YAKSys_Hybrid_CPU::Memory::MemorySubsystem> memSub)
	{
		ThrowIfUnsupportedZeroRowLength2DTransferContour(L"BurstWrite2D", elementCount, rowLength);

		if (elementCount == 0 || elementSize <= 0)
		{
			return 0;
		}

		std::shared_ptr<StreamRegisterFile> srf = ResolveActiveMemorySubsystem(memSub) == nullptr ? nullptr : ResolveActiveMemorySubsystem(memSub)->getStreamRegisters();
		std::shared_ptr<IBurstBackend> backend = GetBackend(memSub);

		// Check if we can use optimized burst planning
		if (colStride == elementSize)
		{
			// Columns within rows are contiguous - use burst planning
			unsigned long long totalWritten = 0;

			for (auto segment : BurstPlanner::Plan2D(baseAddr + (startOffset * colStride), elementCount, elementSize, rowLength, rowStride, colStride))
			{
				std::span<const unsigned char> slice = CheckedSubspan(buffer, static_cast<int>(totalWritten), segment.Length);

				if (!backend->Write(CPU_DEVICE_ID, segment.Address, slice))
				{
					ThrowIfIncompleteTransfer(L"BurstWrite2D", baseAddr, elementCount, totalWritten / static_cast<unsigned long long>(elementSize), elementSize);
				}

				InvalidatePrefetchedSrfWindow(segment.Address, static_cast<unsigned long long>(segment.Length), srf);
				totalWritten += static_cast<unsigned long long>(segment.Length);
			}

			unsigned long long completedElements = totalWritten / static_cast<unsigned long long>(elementSize);
			ThrowIfIncompleteTransfer(L"BurstWrite2D", baseAddr, elementCount, completedElements, elementSize);
			return completedElements;
		}

		// Fall back to element-by-element for non-contiguous columns
		unsigned long long elementsWritten = 0;
		unsigned long long bufferOffset = 0;

		for (unsigned long long i = 0; i < elementCount; i++)
		{
			unsigned long long linearIdx = startOffset + i;
			unsigned long long addr = AddressGen::Gen2D(baseAddr, linearIdx, rowLength, rowStride, colStride);
			std::span<const unsigned char> elemSlice = CheckedSubspan(buffer, static_cast<int>(bufferOffset), elementSize);

			if (!backend->Write(CPU_DEVICE_ID, addr, elemSlice))
			{
				ThrowIfIncompleteTransfer(L"BurstWrite2D", baseAddr, elementCount, elementsWritten, elementSize);
			}

			InvalidatePrefetchedSrfWindow(addr, static_cast<unsigned long long>(elementSize), srf);
			bufferOffset += static_cast<unsigned long long>(elementSize);
			elementsWritten++;
		}

		return elementsWritten;
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: [MethodImpl(MethodImplOptions.AggressiveInlining)] public static ulong BurstGather(ulong baseAddr, Span<byte> buffer, ulong elementCount, int elementSize, ReadOnlySpan<byte> indices, int indexSize, byte indexIsByteOffset, YAKSys_Hybrid_CPU.Memory.MemorySubsystem? memSub = null)
	unsigned long long BurstIO::BurstGather(unsigned long long baseAddr, std::span<unsigned char> buffer, unsigned long long elementCount, int elementSize, std::span<const unsigned char> indices, int indexSize, unsigned char indexIsByteOffset, std::shared_ptr<YAKSys_Hybrid_CPU::Memory::MemorySubsystem> memSub)
	{
		if (elementCount == 0 || elementSize <= 0)
		{
			return 0;
		}

		std::shared_ptr<StreamRegisterFile> srf = ResolveActiveMemorySubsystem(memSub) == nullptr ? nullptr : ResolveActiveMemorySubsystem(memSub)->getStreamRegisters();
		std::shared_ptr<IBurstBackend> backend = GetBackend(memSub);

		// For larger index counts, try to optimize with burst planning
		if (elementCount >= 8)
		{
			// Extract indices into ulong array for analysis
			std::vector<unsigned long long> indexArray(elementCount);
			for (unsigned long long i = 0; i < elementCount; i++)
			{
				if (indexSize == 4)
				{
					indexArray[i] = ReadLittleEndian<unsigned int>(CheckedSubspan(indices, static_cast<int>(i * 4), 4));
				}
				else // indexSize == 8
				{
					indexArray[i] = ReadLittleEndian<unsigned long long>(CheckedSubspan(indices, static_cast<int>(i * 8), 8));
				}
			}

			// Use optimized burst planning for indexed access
			unsigned long long totalRead = 0;
			for (auto segment : BurstPlanner::PlanIndexed(baseAddr, elementSize, indexArray, indexIsByteOffset != 0))
			{
				std::span<unsigned char> slice = CheckedSubspan(buffer, static_cast<int>(totalRead), segment.Length);

				unsigned long long segmentElements = static_cast<unsigned long long>(segment.Length) / static_cast<unsigned long long>(elementSize);
				if (!TryConsumePrefetchedSrfChunk(segment.Address, slice, segmentElements, elementSize, static_cast<unsigned short>(elementSize), srf) && !backend->Read(CPU_DEVICE_ID, segment.Address, slice))
				{
					ThrowIfIncompleteTransfer(L"BurstGather", baseAddr, elementCount, totalRead / static_cast<unsigned long long>(elementSize), elementSize);
				}

				totalRead += static_cast<unsigned long long>(segment.Length);
			}

			unsigned long long completedElements = totalRead / static_cast<unsigned long long>(elementSize);
			ThrowIfIncompleteTransfer(L"BurstGather", baseAddr, elementCount, completedElements, elementSize);
			return completedElements;
		}

		// Fall back to element-by-element for small accesses
		unsigned long long elementsRead = 0;
		unsigned long long bufferOffset = 0;

		for (unsigned long long i = 0; i < elementCount; i++)
		{
			// Read index value
			unsigned long long index;
			if (indexSize == 4)
			{
				index = ReadLittleEndian<unsigned int>(CheckedSubspan(indices, static_cast<int>(i * 4), 4));
			}
			else // indexSize == 8
			{
				index = ReadLittleEndian<unsigned long long>(CheckedSubspan(indices, static_cast<int>(i * 8), 8));
			}

			// Calculate element address
			unsigned long long elemAddr;
			if (indexIsByteOffset != 0)
			{
				// Index is byte offset
				elemAddr = baseAddr + index;
			}
			else
			{
				// Index is element index
				elemAddr = baseAddr + (index * static_cast<unsigned long long>(elementSize));
			}

			// Read element
			std::span<unsigned char> elemSlice = CheckedSubspan(buffer, static_cast<int>(bufferOffset), elementSize);
			if (!backend->Read(CPU_DEVICE_ID, elemAddr, elemSlice))
			{
				ThrowIfIncompleteTransfer(L"BurstGather", baseAddr, elementCount, elementsRead, elementSize);
			}

			bufferOffset += static_cast<unsigned long long>(elementSize);
			elementsRead++;
		}

		return elementsRead;
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: [MethodImpl(MethodImplOptions.AggressiveInlining)] public static ulong BurstScatter(ulong baseAddr, ReadOnlySpan<byte> buffer, ulong elementCount, int elementSize, ReadOnlySpan<byte> indices, int indexSize, byte indexIsByteOffset, YAKSys_Hybrid_CPU.Memory.MemorySubsystem? memSub = null)
	unsigned long long BurstIO::BurstScatter(unsigned long long baseAddr, std::span<const unsigned char> buffer, unsigned long long elementCount, int elementSize, std::span<const unsigned char> indices, int indexSize, unsigned char indexIsByteOffset, std::shared_ptr<YAKSys_Hybrid_CPU::Memory::MemorySubsystem> memSub)
	{
		if (elementCount == 0 || elementSize <= 0)
		{
			return 0;
		}

		std::shared_ptr<StreamRegisterFile> srf = ResolveActiveMemorySubsystem(memSub) == nullptr ? nullptr : ResolveActiveMemorySubsystem(memSub)->getStreamRegisters();
		std::shared_ptr<IBurstBackend> backend = GetBackend(memSub);

		// For larger index counts, try to optimize with burst planning
		if (elementCount >= 8)
		{
			// Extract indices into ulong array for analysis
			std::vector<unsigned long long> indexArray(elementCount);
			for (unsigned long long i = 0; i < elementCount; i++)
			{
				if (indexSize == 4)
				{
					indexArray[i] = ReadLittleEndian<unsigned int>(CheckedSubspan(indices, static_cast<int>(i * 4), 4));
				}
				else // indexSize == 8
				{
					indexArray[i] = ReadLittleEndian<unsigned long long>(CheckedSubspan(indices, static_cast<int>(i * 8), 8));
				}
			}

			// Use optimized burst planning for indexed access
			unsigned long long totalWritten = 0;
			for (auto segment : BurstPlanner::PlanIndexed(baseAddr, elementSize, indexArray, indexIsByteOffset != 0))
			{
				std::span<const unsigned char> slice = CheckedSubspan(buffer, static_cast<int>(totalWritten), segment.Length);

				if (!backend->Write(CPU_DEVICE_ID, segment.Address, slice))
				{
					ThrowIfIncompleteTransfer(L"BurstScatter", baseAddr, elementCount, totalWritten / static_cast<unsigned long long>(elementSize), elementSize);
				}

				InvalidatePrefetchedSrfWindow(segment.Address, static_cast<unsigned long long>(segment.Length), srf);
				totalWritten += static_cast<unsigned long long>(segment.Length);
			}

			unsigned long long completedElements = totalWritten / static_cast<unsigned long long>(elementSize);
			ThrowIfIncompleteTransfer(L"BurstScatter", baseAddr, elementCount, completedElements, elementSize);
			return completedElements;
		}

		// Fall back to element-by-element for small accesses
		unsigned long long elementsWritten = 0;
		unsigned long long bufferOffset = 0;

		for (unsigned long long i = 0; i < elementCount; i++)
		{
			// Read index value
			unsigned long long index;
			if (indexSize == 4)
			{
				index = ReadLittleEndian<unsigned int>(CheckedSubspan(indices, static_cast<int>(i * 4), 4));
			}
			else // indexSize == 8
			{
				index = ReadLittleEndian<unsigned long long>(CheckedSubspan(indices, static_cast<int>(i * 8), 8));
			}

			// Calculate element address
			unsigned long long elemAddr;
			if (indexIsByteOffset != 0)
			{
				// Index is byte offset
				elemAddr = baseAddr + index;
			}
			else
			{
				// Index is element index
				elemAddr = baseAddr + (index * static_cast<unsigned long long>(elementSize));
			}

			// Write element
			std::span<const unsigned char> elemSlice = CheckedSubspan(buffer, static_cast<int>(bufferOffset), elementSize);
			if (!backend->Write(CPU_DEVICE_ID, elemAddr, elemSlice))
			{
				ThrowIfIncompleteTransfer(L"BurstScatter", baseAddr, elementCount, elementsWritten, elementSize);
			}

			InvalidatePrefetchedSrfWindow(elemAddr, static_cast<unsigned long long>(elementSize), srf);
			bufferOffset += static_cast<unsigned long long>(elementSize);
			elementsWritten++;
		}

		return elementsWritten;
	}
}
