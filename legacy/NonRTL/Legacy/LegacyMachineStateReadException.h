#pragma once

#include <string>
#include <stdexcept>
#include <format>
#include <memory>

namespace HybridCPU_ISE::Legacy
{
	enum class LegacyMachineStateReadFailureKind
	{
		MainMemoryUnavailable = 0,
		AddressOutOfRange = 1,
		PartialReadRequiresCompatibilityPadding = 2,
		ReadFault = 3
	};

	class LegacyMachineStateReadException final : public InvalidOperationException
	{
	private:
		const unsigned long long Address;
		const int Length;
		const long long MemoryLength;
		const LegacyMachineStateReadFailureKind FailureKind;

	public:
		unsigned long long getAddress() const;
		int getLength() const;
		long long getMemoryLength() const;
		LegacyMachineStateReadFailureKind getFailureKind() const;

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public LegacyMachineStateReadException(ulong address, int length, long memoryLength, LegacyMachineStateReadFailureKind failureKind, Exception? innerException = null) : base(BuildMessage(address, length, memoryLength, failureKind), innerException)
		LegacyMachineStateReadException(unsigned long long address, int length, long long memoryLength, LegacyMachineStateReadFailureKind failureKind, std::runtime_error innerException = std::runtime_error());

	private:
		static std::wstring BuildMessage(unsigned long long address, int length, long long memoryLength, LegacyMachineStateReadFailureKind failureKind);

	protected:
		std::shared_ptr<LegacyMachineStateReadException> shared_from_this()
		{
			return std::static_pointer_cast<LegacyMachineStateReadException>(InvalidOperationException::shared_from_this());
		}
	};
}
