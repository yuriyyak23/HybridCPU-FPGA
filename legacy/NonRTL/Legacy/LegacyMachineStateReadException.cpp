#include "LegacyMachineStateReadException.h"

namespace HybridCPU_ISE::Legacy
{

	unsigned long long LegacyMachineStateReadException::getAddress() const
	{
		return Address;
	}

	int LegacyMachineStateReadException::getLength() const
	{
		return Length;
	}

	long long LegacyMachineStateReadException::getMemoryLength() const
	{
		return MemoryLength;
	}

	LegacyMachineStateReadFailureKind LegacyMachineStateReadException::getFailureKind() const
	{
		return FailureKind;
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public LegacyMachineStateReadException(ulong address, int length, long memoryLength, LegacyMachineStateReadFailureKind failureKind, Exception? innerException = null) : base(BuildMessage(address, length, memoryLength, failureKind), innerException)
	LegacyMachineStateReadException::LegacyMachineStateReadException(unsigned long long address, int length, long long memoryLength, LegacyMachineStateReadFailureKind failureKind, std::runtime_error innerException) : InvalidOperationException(BuildMessage(address, length, memoryLength, failureKind), innerException)
	{
		Address = address;
		Length = length;
		MemoryLength = memoryLength;
		FailureKind = failureKind;
	}

	std::wstring LegacyMachineStateReadException::BuildMessage(unsigned long long address, int length, long long memoryLength, LegacyMachineStateReadFailureKind failureKind)
	{
		std::wstring rangeText = std::format("IOVA 0x{0:0>X} for {1:s} byte(s)", address, length);
		std::wstring memoryText = memoryLength >= 0 ? std::format("main memory length 0x{0:0>X}", memoryLength) : L"uninitialized main memory";

	// C# TO C++ CONVERTER TASK: The following 'switch expression' was not converted by C# to C++ Converter:
	//			return failureKind switch
	//			{
	//				LegacyMachineStateReadFailureKind.MainMemoryUnavailable => $"LegacyGlobal machine-state source cannot read {rangeText} because the runtime has no initialized main memory.",
	//				LegacyMachineStateReadFailureKind.AddressOutOfRange => $"LegacyGlobal machine-state source cannot read {rangeText} because the request starts beyond {memoryText}.",
	//				LegacyMachineStateReadFailureKind.PartialReadRequiresCompatibilityPadding => $"LegacyGlobal machine-state source requires explicit compatibility padding to read {rangeText} against {memoryText}. Use CreateLegacyGlobalCompat(...) when zero-filled tail bytes are intended.",
	//				_ => $"LegacyGlobal machine-state source failed to read {rangeText} from {memoryText}.",
	//			};
	}
}
