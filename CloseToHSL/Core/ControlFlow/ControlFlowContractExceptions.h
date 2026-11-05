#pragma once

#include "RelocationEntry.h"
#include <string>
#include <limits>
#include <optional>
#include <format>
#include <memory>
#include "../../../tangible_exception_helper.h"
#include "../../../tangible_string_helper.h"

namespace YAKSys_Hybrid_CPU::Core::ControlFlow
{
	class ControlFlowContractException : public InvalidOperationException, public std::enable_shared_from_this<ControlFlowContractException>
	{
	protected:
		ControlFlowContractException(const std::wstring &message);

	protected:
		std::shared_ptr<ControlFlowContractException> shared_from_this()
		{
			return std::static_pointer_cast<ControlFlowContractException>(std::enable_shared_from_this<ControlFlowContractException>::shared_from_this());
		}
	};

	class EntryPointDefinitionException final : public ControlFlowContractException
	{
	private:
		const std::wstring Operation;
		const unsigned long long ExistingAddress;
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly System.Nullable<ulong> RequestedAddress;
		std::optional<unsigned long long> RequestedAddress;

	public:
		EntryPointDefinitionException(const std::wstring &operation, unsigned long long existingAddress, std::optional<unsigned long long> &requestedAddress, const std::wstring &message);

		std::wstring getOperation() const;

		unsigned long long getExistingAddress() const;

		std::optional<unsigned long long> getRequestedAddress() const;

	protected:
		std::shared_ptr<EntryPointDefinitionException> shared_from_this()
		{
			return std::static_pointer_cast<EntryPointDefinitionException>(ControlFlowContractException::shared_from_this());
		}
	};

	class UnsupportedEntryPointOperationException final : public ControlFlowContractException
	{
	private:
		const std::wstring EntryPointKind;
		const std::wstring Operation;

	public:
		UnsupportedEntryPointOperationException(const std::wstring &entryPointKind, const std::wstring &operation);

		std::wstring getEntryPointKind() const;

		std::wstring getOperation() const;

	protected:
		std::shared_ptr<UnsupportedEntryPointOperationException> shared_from_this()
		{
			return std::static_pointer_cast<UnsupportedEntryPointOperationException>(ControlFlowContractException::shared_from_this());
		}
	};

	class ControlFlowStackUnderflowException final : public ControlFlowContractException
	{
	private:
		const std::wstring StackName;

	public:
		ControlFlowStackUnderflowException(const std::wstring &stackName);

		std::wstring getStackName() const;

	protected:
		std::shared_ptr<ControlFlowStackUnderflowException> shared_from_this()
		{
			return std::static_pointer_cast<ControlFlowStackUnderflowException>(ControlFlowContractException::shared_from_this());
		}
	};

	class RelocationEncodingException final : public ControlFlowContractException
	{
	private:
		const unsigned long long EmissionCursor;
		const unsigned long long RequiredBackOffsetBytes;

	public:
		RelocationEncodingException(unsigned long long emissionCursor, unsigned long long requiredBackOffsetBytes);

		unsigned long long getEmissionCursor() const;

		unsigned long long getRequiredBackOffsetBytes() const;

	protected:
		std::shared_ptr<RelocationEncodingException> shared_from_this()
		{
			return std::static_pointer_cast<RelocationEncodingException>(ControlFlowContractException::shared_from_this());
		}
	};

	class UnsupportedRelocationApplicationException final : public ControlFlowContractException
	{
	private:
		const RelocationEncodingKind EncodingKind;
		const unsigned char PatchWidth;

	public:
		UnsupportedRelocationApplicationException(RelocationEncodingKind encodingKind, unsigned char patchWidth);

		RelocationEncodingKind getEncodingKind() const;

		unsigned char getPatchWidth() const;

	protected:
		std::shared_ptr<UnsupportedRelocationApplicationException> shared_from_this()
		{
			return std::static_pointer_cast<UnsupportedRelocationApplicationException>(ControlFlowContractException::shared_from_this());
		}
	};

	class RelocationPatchOutOfRangeException final : public ControlFlowContractException
	{
	private:
		const unsigned long long ImageBaseAddress;
		const int ImageLength;
		const unsigned long long PatchAddress;
		const unsigned char PatchWidth;
		const std::wstring TargetSymbol;

	public:
		RelocationPatchOutOfRangeException(unsigned long long imageBaseAddress, int imageLength, unsigned long long patchAddress, unsigned char patchWidth, const std::wstring &targetSymbol);

		unsigned long long getImageBaseAddress() const;

		int getImageLength() const;

		unsigned long long getPatchAddress() const;

		unsigned char getPatchWidth() const;

		std::wstring getTargetSymbol() const;

	private:
		static std::wstring NormalizeTargetSymbol(const std::wstring &targetSymbol);

		static std::wstring CreateMessage(unsigned long long imageBaseAddress, int imageLength, unsigned long long patchAddress, unsigned char patchWidth, const std::wstring &targetSymbol);

	protected:
		std::shared_ptr<RelocationPatchOutOfRangeException> shared_from_this()
		{
			return std::static_pointer_cast<RelocationPatchOutOfRangeException>(ControlFlowContractException::shared_from_this());
		}
	};
}

