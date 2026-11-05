#include "Rf09ReplayEntry.h"
#include <codecvt>
#include <locale>
#include <span>
#include <string>
#include <utility>

namespace YAKSys_Hybrid_CPU::Core::Decoder
{

	ReplayEntry::ReplayEntry(SemanticInstructionKey semanticKey, std::shared_ptr<YAKSys_Hybrid_CPU::Core::Decoder::CanonicalBundle> canonicalBundle, std::shared_ptr<ReplayEntryValidationFingerprint> validationFingerprint)
		: SemanticKey(std::move(semanticKey)),
		  CanonicalBundle(std::move(canonicalBundle)),
		  ValidationFingerprint(std::move(validationFingerprint))
	{
	}

	SemanticInstructionKey ReplayEntry::getSemanticKey() const
	{
		return SemanticKey;
	}

	std::shared_ptr<YAKSys_Hybrid_CPU::Core::Decoder::CanonicalBundle> ReplayEntry::getCanonicalBundle() const
	{
		return CanonicalBundle;
	}

	std::shared_ptr<ReplayEntryValidationFingerprint> ReplayEntry::getValidationFingerprint() const
	{
		return ValidationFingerprint;
	}

	std::shared_ptr<ReplayEntry> ReplayEntry::Create(std::shared_ptr<YAKSys_Hybrid_CPU::Core::Decoder::CanonicalBundle> canonicalBundle)
	{
		ArgumentNullException::ThrowIfNull(canonicalBundle);
		if (!canonicalBundle->isReplayEligible())
		{
			throw InvalidOperationException(L"ReplayEntry requires an explicitly replay-eligible CanonicalDecodeContext.");
		}

		SemanticInstructionKey semanticKey = canonicalBundle->getSemanticKey();
		return std::shared_ptr<ReplayEntry>(new ReplayEntry(semanticKey, canonicalBundle, ComputeValidationFingerprint(canonicalBundle)));
	}

	bool ReplayEntry::HasValidFrozenContent()
	{
		return getValidationFingerprint()->isValid() && getSemanticKey().Equals(getCanonicalBundle()->getSemanticKey()) && (*getValidationFingerprint() == *ComputeValidationFingerprint(getCanonicalBundle()));
	}

	bool ReplayEntry::HasSameFrozenSemanticContent(std::shared_ptr<ReplayEntry> other)
	{
		ArgumentNullException::ThrowIfNull(other);
		const auto slots = getCanonicalBundle()->getSlots();
		const auto otherSlots = other->getCanonicalBundle()->getSlots();
		if (!getSemanticKey().Equals(other->getSemanticKey()) || !HasValidFrozenContent() || !other->HasValidFrozenContent() || !getCanonicalBundle()->getBundleSideband()->Equals(other->getCanonicalBundle()->getBundleSideband()) || slots.size() != otherSlots.size())
		{
			return false;
		}

		for (size_t slotIndex = 0; slotIndex < slots.size(); slotIndex++)
		{
			const auto &slot = slots[slotIndex];
			const auto &otherSlot = otherSlots[slotIndex];
			if (!slot || !otherSlot ||
				slot->SlotIndex != otherSlot->SlotIndex ||
				slot->IsOccupied != otherSlot->IsOccupied ||
				slot->Opcode != otherSlot->Opcode ||
				slot->InstructionClass != otherSlot->InstructionClass ||
				slot->SerializationClass != otherSlot->SerializationClass ||
				slot->Rd != otherSlot->Rd || slot->Rs1 != otherSlot->Rs1 ||
				slot->Rs2 != otherSlot->Rs2 || slot->Immediate != otherSlot->Immediate ||
				slot->CsrAddress != otherSlot->CsrAddress ||
				slot->AcquireOrdering != otherSlot->AcquireOrdering ||
				slot->ReleaseOrdering != otherSlot->ReleaseOrdering ||
				!slot->RawSlot || !otherSlot->RawSlot || !slot->RawSlot->Equals(otherSlot->RawSlot) ||
				!slot->InstructionPayload || !otherSlot->InstructionPayload || !slot->InstructionPayload->Equals(otherSlot->InstructionPayload) ||
				!slot->SlotSideband || !otherSlot->SlotSideband || !slot->SlotSideband->Equals(otherSlot->SlotSideband) ||
				(static_cast<bool>(slot->StaticBinding) != static_cast<bool>(otherSlot->StaticBinding)) ||
				(slot->StaticBinding && otherSlot->StaticBinding && *slot->StaticBinding != *otherSlot->StaticBinding) ||
				slot->ScalarLoadAddressPlan != otherSlot->ScalarLoadAddressPlan)
			{
				return false;
			}
		}

		return true;
	}

	std::shared_ptr<ReplayEntryValidationFingerprint> ReplayEntry::ComputeValidationFingerprint(std::shared_ptr<YAKSys_Hybrid_CPU::Core::Decoder::CanonicalBundle> canonicalBundle)
	{
		std::shared_ptr<StringBuilder> canonical = std::make_shared<StringBuilder>();
		SemanticInstructionKey semanticKey = canonicalBundle->getSemanticKey();
		Append(canonical, L"rf09-replay-entry-v1");
		Append(canonical, semanticKey.getRawBundleSha256());
		Append(canonical, semanticKey.getAnnotationsSha256());
		Append(canonical, semanticKey.getManifestVersion());
		Append(canonical, semanticKey.getManifestHash());
		Append(canonical, semanticKey.getExtensionConfigurationFingerprint());
		Append(canonical, semanticKey.getDecoderEpoch());
		Append(canonical, semanticKey.getDecoderVersion());
		Append(canonical, semanticKey.getPrivilegeContext());
		Append(canonical, semanticKey.getDomainIdentity());
		Append(canonical, semanticKey.getAddressSpaceIdentity());
		Append(canonical, semanticKey.getVectorConfigurationFingerprint());
		Append(canonical, std::to_wstring(semanticKey.getExecutableMemoryInvalidationEpoch()));
		Append(canonical, std::to_wstring(semanticKey.getCodeGenerationEpoch()));
		Append(canonical, semanticKey.isReplayEligible() ? L"1" : L"0");
		Append(canonical, canonicalBundle->getBundleSideband()->getKind());
		Append(canonical, canonicalBundle->getBundleSideband()->getContentSha256());

		const auto slots = canonicalBundle->getSlots();
		for (size_t slotIndex = 0; slotIndex < slots.size(); slotIndex++)
		{
			std::shared_ptr<CanonicalDecodedInstruction> slot = slots[slotIndex];
			Append(canonical, std::to_wstring(slotIndex));
			Append(canonical, slot->RawSlot->getKind());
			Append(canonical, slot->RawSlot->getContentSha256());
			Append(canonical, slot->InstructionPayload->getKind());
			Append(canonical, slot->InstructionPayload->getContentSha256());
			Append(canonical, slot->SlotSideband->getKind());
			Append(canonical, slot->SlotSideband->getContentSha256());
		}

		const std::wstring canonicalText = canonical->toString();
		const std::string utf8 = std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t>{}.to_bytes(canonicalText);
		const auto bytes = std::span<const unsigned char>(reinterpret_cast<const unsigned char*>(utf8.data()), utf8.size());
		return std::make_shared<ReplayEntryValidationFingerprint>(ReplayEntryValidationFingerprint::CurrentSchemaVersion, CanonicalPayloadSnapshot::Hash(bytes));
	}

	void ReplayEntry::Append(std::shared_ptr<StringBuilder> builder, const std::wstring &value)
	{
		builder->append(value.length());
		builder->append(L':');
		builder->append(value);
		builder->append(L'|');
	}
}
