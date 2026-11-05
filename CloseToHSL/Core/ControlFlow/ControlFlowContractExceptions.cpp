#include "ControlFlowContractExceptions.h"

namespace YAKSys_Hybrid_CPU::Core::ControlFlow
{

	ControlFlowContractException::ControlFlowContractException(const std::wstring &message) : InvalidOperationException(message)
	{
	}

	EntryPointDefinitionException::EntryPointDefinitionException(const std::wstring &operation, unsigned long long existingAddress, std::optional<unsigned long long> &requestedAddress, const std::wstring &message)
		: ControlFlowContractException(message), Operation(operation.empty() ? L"" : operation), ExistingAddress(existingAddress), RequestedAddress(requestedAddress)
	{
	}

	std::wstring EntryPointDefinitionException::getOperation() const
	{
		return Operation;
	}

	unsigned long long EntryPointDefinitionException::getExistingAddress() const
	{
		return ExistingAddress;
	}

	std::optional<unsigned long long> EntryPointDefinitionException::getRequestedAddress() const
	{
		return RequestedAddress;
	}

	UnsupportedEntryPointOperationException::UnsupportedEntryPointOperationException(const std::wstring &entryPointKind, const std::wstring &operation)
		: ControlFlowContractException((entryPointKind.empty() ? L"" : entryPointKind) + std::wstring(L" entry point does not support ") + (operation.empty() ? L"" : operation) + L"."),
		  EntryPointKind(entryPointKind.empty() ? L"" : entryPointKind), Operation(operation.empty() ? L"" : operation)
	{
	}

	std::wstring UnsupportedEntryPointOperationException::getEntryPointKind() const
	{
		return EntryPointKind;
	}

	std::wstring UnsupportedEntryPointOperationException::getOperation() const
	{
		return Operation;
	}

	ControlFlowStackUnderflowException::ControlFlowStackUnderflowException(const std::wstring &stackName)
		: ControlFlowContractException((stackName.empty() ? L"" : stackName) + std::wstring(L" control-flow stack is empty; returning PC=0 would alias a valid address.")), StackName(stackName.empty() ? L"" : stackName)
	{
	}

	std::wstring ControlFlowStackUnderflowException::getStackName() const
	{
		return StackName;
	}

	RelocationEncodingException::RelocationEncodingException(unsigned long long emissionCursor, unsigned long long requiredBackOffsetBytes)
		: ControlFlowContractException(L"Cannot encode legacy absolute relocation at cursor " + std::to_wstring(emissionCursor) + L"; the encoding requires a " + std::to_wstring(requiredBackOffsetBytes) + L" byte back-offset."),
		  EmissionCursor(emissionCursor), RequiredBackOffsetBytes(requiredBackOffsetBytes)
	{
	}

	unsigned long long RelocationEncodingException::getEmissionCursor() const
	{
		return EmissionCursor;
	}

	unsigned long long RelocationEncodingException::getRequiredBackOffsetBytes() const
	{
		return RequiredBackOffsetBytes;
	}

	UnsupportedRelocationApplicationException::UnsupportedRelocationApplicationException(RelocationEncodingKind encodingKind, unsigned char patchWidth)
		: ControlFlowContractException(L"Relocation application only supports LegacyAbsolute64 with a " + std::to_wstring(RelocationEntry::Absolute64PatchWidth) + L"-byte patch width. Received encoding " + std::to_wstring(static_cast<int>(encodingKind)) + L" with patch width " + std::to_wstring(static_cast<unsigned int>(patchWidth)) + L"."),
		  EncodingKind(encodingKind), PatchWidth(patchWidth)
	{
	}

	RelocationEncodingKind UnsupportedRelocationApplicationException::getEncodingKind() const
	{
		return EncodingKind;
	}

	unsigned char UnsupportedRelocationApplicationException::getPatchWidth() const
	{
		return PatchWidth;
	}

	RelocationPatchOutOfRangeException::RelocationPatchOutOfRangeException(unsigned long long imageBaseAddress, int imageLength, unsigned long long patchAddress, unsigned char patchWidth, const std::wstring &targetSymbol)
		: ControlFlowContractException(CreateMessage(imageBaseAddress, imageLength, patchAddress, patchWidth, targetSymbol)),
		  ImageBaseAddress(imageBaseAddress), ImageLength(imageLength), PatchAddress(patchAddress), PatchWidth(patchWidth), TargetSymbol(targetSymbol.empty() ? L"" : targetSymbol)
	{
	}

	unsigned long long RelocationPatchOutOfRangeException::getImageBaseAddress() const
	{
		return ImageBaseAddress;
	}

	int RelocationPatchOutOfRangeException::getImageLength() const
	{
		return ImageLength;
	}

	unsigned long long RelocationPatchOutOfRangeException::getPatchAddress() const
	{
		return PatchAddress;
	}

	unsigned char RelocationPatchOutOfRangeException::getPatchWidth() const
	{
		return PatchWidth;
	}

	std::wstring RelocationPatchOutOfRangeException::getTargetSymbol() const
	{
		return TargetSymbol;
	}

	std::wstring RelocationPatchOutOfRangeException::NormalizeTargetSymbol(const std::wstring &targetSymbol)
	{
		return StringHelper::isEmptyOrWhiteSpace(targetSymbol) ? L"<anonymous>" : targetSymbol;
	}

	std::wstring RelocationPatchOutOfRangeException::CreateMessage(unsigned long long imageBaseAddress, int imageLength, unsigned long long patchAddress, unsigned char patchWidth, const std::wstring &targetSymbol)
	{
		unsigned long long imageLengthUlong = static_cast<unsigned long long>(imageLength);
		std::wstring endAddressText = std::numeric_limits<unsigned long long>::max() - imageBaseAddress < imageLengthUlong ? L"overflow" : std::to_wstring(imageBaseAddress + imageLengthUlong);
		return L"Relocation patch for symbol '" + NormalizeTargetSymbol(targetSymbol) + L"' at " + std::to_wstring(patchAddress) + L" with width " + std::to_wstring(static_cast<unsigned int>(patchWidth)) + L" byte(s) falls outside emitted image [" + std::to_wstring(imageBaseAddress) + L", " + endAddressText + L").";
	}
}
