#include "InvariantChecks.h"

#include <codecvt>
#include <locale>

namespace HybridCPU_ISE::Core
{

	InvariantViolationException::InvariantViolationException(const std::wstring &message)
		: std::runtime_error(std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t>().to_bytes(message)), _innerException(nullptr)
	{
	}

	InvariantViolationException::InvariantViolationException(const std::wstring &message, std::runtime_error innerException)
		: std::runtime_error(std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t>().to_bytes(message)), _innerException(std::make_exception_ptr(std::move(innerException)))
	{
	}

	void InvariantChecks::CheckMaskedElementsPreserved(std::vector<unsigned char> &original, std::vector<unsigned char> &result, unsigned long long predicateMask, int elementCount, int elementSize, bool maskAgnostic)
	{
		if (maskAgnostic)
		{
			return; // Agnostic mode allows modifications
		}

		if (predicateMask == 0xFFFFFFFFFFFFFFFFULL)
		{
			return; // All lanes active, no masked elements
		}

		if (original.empty() || result.empty())
		{
			return;
		}

		for (int i = 0; i < elementCount && i < 64; i++)
		{
			bool laneActive = (predicateMask & (1ULL << i)) != 0;
			if (!laneActive)
			{
				// Check that this element wasn't modified
				int offset = i * elementSize;
				if (offset + elementSize <= original.size() && offset + elementSize <= result.size())
				{
					for (int b = 0; b < elementSize; b++)
					{
						if (original[offset + b] != result[offset + b])
						{
							throw InvariantViolationException(std::format(
								L"Invariant violation: Masked-off element {} was modified. Original[{}]={}, Result[{}]={}",
								i, offset + b, static_cast<unsigned int>(original[offset + b]), offset + b,
								static_cast<unsigned int>(result[offset + b])));
						}
					}
				}
			}
		}
	}

	void InvariantChecks::CheckBurstBoundary(unsigned long long address, int size, int axiBoundary)
	{
		unsigned long long startPage = address / static_cast<unsigned long long>(axiBoundary);
		unsigned long long endPage = (address + static_cast<unsigned long long>(size) - 1) / static_cast<unsigned long long>(axiBoundary);

		if (startPage != endPage)
		{
			throw InvariantViolationException(std::format(
				L"Invariant violation: Burst transaction crosses AXI boundary. Address=0x{:X}, Size={}, Boundary={}. StartPage={}, EndPage={}",
				address, size, axiBoundary, startPage, endPage));
		}
	}

	void InvariantChecks::CheckStripMiningCount(int processedCount, unsigned int vl, bool isFinalIteration)
	{
		if (isFinalIteration)
		{
			// Final iteration may process fewer elements
			if (processedCount > vl)
			{
				throw InvariantViolationException(std::format(
					L"Invariant violation: Final strip-mining iteration processed {} elements, but VL={}",
					processedCount, vl));
			}
		}
		else
		{
			// Non-final iterations must process exactly VL elements
			if (processedCount != vl)
			{
				throw InvariantViolationException(std::format(
					L"Invariant violation: Strip-mining iteration processed {} elements, expected VL={}",
					processedCount, vl));
			}
		}
	}

	void InvariantChecks::CheckVLConstraint(unsigned int vl, unsigned int vlmax)
	{
		if (vl > vlmax)
		{
			throw InvariantViolationException(std::format(L"Invariant violation: VL={} exceeds VLMAX={}", vl, vlmax));
		}
	}

	void InvariantChecks::CheckVSEW(unsigned char vsew)
	{
		if (vsew != 8 && vsew != 16 && vsew != 32 && vsew != 64)
		{
			throw InvariantViolationException(std::format(
				L"Invariant violation: VSEW={} is not a valid value (must be 8, 16, 32, or 64)",
				static_cast<unsigned int>(vsew)));
		}
	}

	void InvariantChecks::CheckLMUL(unsigned char lmul)
	{
		if (lmul != 1 && lmul != 2 && lmul != 4 && lmul != 8)
		{
			throw InvariantViolationException(std::format(
				L"Invariant violation: LMUL={} is not a valid value (must be 1, 2, 4, or 8)",
				static_cast<unsigned int>(lmul)));
		}
	}

	void InvariantChecks::CheckIndexBounds(int index, int arrayLength, const std::wstring &arrayName)
	{
		if (index < 0 || index >= arrayLength)
		{
			throw InvariantViolationException(std::format(
				L"Invariant violation: Index {} is out of bounds for {} (length={})",
				index, arrayName, arrayLength));
		}
	}

	void InvariantChecks::CheckAlignment(unsigned long long address, int alignment)
	{
		if ((address % static_cast<unsigned long long>(alignment)) != 0)
		{
			throw InvariantViolationException(std::format(
				L"Invariant violation: Address 0x{:X} is not aligned to {} bytes", address, alignment));
		}
	}

	void InvariantChecks::CheckPredicateRegisterIndex(unsigned char index)
	{
		if (index >= 16)
		{
			throw InvariantViolationException(std::format(
				L"Invariant violation: Predicate register index {} is invalid (must be 0-15)",
				static_cast<unsigned int>(index)));
		}
	}

	void InvariantChecks::CheckRoundingMode(unsigned char mode)
	{
		if (mode > 4)
		{
			throw InvariantViolationException(std::format(
				L"Invariant violation: Rounding mode {} is invalid (must be 0-4)",
				static_cast<unsigned int>(mode)));
		}
	}

	void InvariantChecks::CheckExceptionMode(unsigned char mode)
	{
		if (mode > 2)
		{
			throw InvariantViolationException(std::format(
				L"Invariant violation: Exception mode {} is invalid (must be 0-2)",
				static_cast<unsigned int>(mode)));
		}
	}
}
