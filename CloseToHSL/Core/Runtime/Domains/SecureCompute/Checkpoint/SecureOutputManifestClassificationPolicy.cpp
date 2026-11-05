#include "SecureOutputManifestClassificationPolicy.h"
#include "YAKSys_Hybrid_CPU.Core.SecureCheckpointPayloadPolicy.h"

namespace YAKSys_Hybrid_CPU::Core
{

	std::shared_ptr<SecureOutputManifestClassificationPolicy> SecureOutputManifestClassificationPolicy::getFailClosed()
	{
		return FailClosed;
	}

	const std::vector<SecureOutputManifestEntryKind> SecureOutputManifestClassificationPolicy::getRequiredPositivePathEntries()
	{
		return RequiredPositivePathEntries;
	}

	std::shared_ptr<SecureOutputManifestClassificationResult> SecureOutputManifestClassificationPolicy::ClassifyEntry(std::shared_ptr<SecureOutputManifestEntry> entry, std::optional<std::shared_ptr<SecurePrivateMemorySealedPayloadContract>> privateMemoryContract)
	{
		if (!entry->OwnerPathClassified)
		{
			return Deny(SecureOutputManifestClassificationDecision::DeniedOwnerPathUnclassified, entry->Kind, SecureCheckpointPayloadDecision::Allowed, L"Secure output manifest entries require owner/path/reachability classification.");
		}

		SecureCheckpointPayloadDecision payloadDecision = ClassifyPayload(entry->PayloadClass, privateMemoryContract);
		if (payloadDecision != SecureCheckpointPayloadDecision::Allowed)
		{
			return Deny(MapPayloadDenial(entry->PayloadClass, payloadDecision), entry->Kind, payloadDecision, PayloadReason(payloadDecision));
		}

	// C# TO C++ CONVERTER TASK: The following 'switch expression' was not converted by C# to C++ Converter:
	//			return entry.Kind switch
	//			{
	//				SecureOutputManifestEntryKind.RequestState => ClassifyRequestState(entry),
	//
	//				SecureOutputManifestEntryKind.InternalBackendResult => ClassifiedManifestOnly(
	//						entry.Kind, payloadDecision, "Internal backend result is manifest coverage only; it is not checkpoint authority."), SecureOutputManifestEntryKind.InternalCompletionRecord => ClassifiedManifestOnly(
	//
	//						entry.Kind,
	//						payloadDecision,
	//						"Internal completion record is manifest coverage only; it is not checkpoint or restore authority."),
	//
	//				SecureOutputManifestEntryKind.GuestVisibleOutput =>
	//					ClassifyGuestVisibleOutput(entry, payloadDecision),
	//
	//				SecureOutputManifestEntryKind.RetireVisibleState =>
	//					ClassifyRetireVisibleState(entry, payloadDecision),
	//
	//				SecureOutputManifestEntryKind.RecomputedAfterRestoreState =>
	//					ClassifyRecomputedAfterRestore(entry, payloadDecision),
	//
	//				_ => Deny(
	//					SecureOutputManifestClassificationDecision.DeniedWrongPayloadClass,
	//					entry.Kind,
	//					payloadDecision,
	//					"Secure output manifest entry kind is not classified."),
	//			};
	}

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: public SecureOutputManifestClassificationResult ClassifyManifest(IReadOnlyList<SecureOutputManifestEntry> entries, System.Nullable<SecurePrivateMemorySealedPayloadContract> privateMemoryContract = null)
	std::shared_ptr<SecureOutputManifestClassificationResult> SecureOutputManifestClassificationPolicy::ClassifyManifest(const std::vector<std::shared_ptr<SecureOutputManifestEntry>> &entries, std::optional<std::shared_ptr<SecurePrivateMemorySealedPayloadContract>> privateMemoryContract)
	{
		for (auto required : RequiredEntries)
		{
			if (!ContainsEntry(entries, required))
			{
				return Deny(SecureOutputManifestClassificationDecision::DeniedMissingManifestEntry, required, SecureCheckpointPayloadDecision::Allowed, L"Secure output manifest is missing a required positive-path entry.");
			}
		}

		for (auto entry : entries)
		{
			std::shared_ptr<SecureOutputManifestClassificationResult> result = ClassifyEntry(entry, privateMemoryContract);
			if (result->IsDenied)
			{
				return result;
			}
		}

		return SecureOutputManifestClassificationResult::Classified(SecureOutputManifestClassificationDecision::CompleteManifestClassified, SecureOutputManifestEntryKind::RecomputedAfterRestoreState, SecureCheckpointPayloadDecision::Allowed, false, true, L"Secure output manifest coverage is complete for classification evidence only.");
	}

	std::shared_ptr<SecureOutputManifestClassificationResult> SecureOutputManifestClassificationPolicy::ClassifyRequestState(std::shared_ptr<SecureOutputManifestEntry> entry)
	{
		if (entry->PayloadClass != SecureCheckpointPayloadClass::SecurePolicyDescriptor && entry->PayloadClass != SecureCheckpointPayloadClass::GuestVisibleState)
		{
			return DenyWrongPayload(entry);
		}

		return SecureOutputManifestClassificationResult::Classified(SecureOutputManifestClassificationDecision::EntryClassifiedForManifestOnly, entry->Kind, SecureCheckpointPayloadDecision::Allowed, true, true, L"Request state is classified as descriptor or guest-visible state.");
	}

	std::shared_ptr<SecureOutputManifestClassificationResult> SecureOutputManifestClassificationPolicy::ClassifyGuestVisibleOutput(std::shared_ptr<SecureOutputManifestEntry> entry, SecureCheckpointPayloadDecision payloadDecision)
	{
		if (entry->PayloadClass != SecureCheckpointPayloadClass::GuestVisibleState && entry->PayloadClass != SecureCheckpointPayloadClass::SecureSharedMemory)
		{
			return DenyWrongPayload(entry);
		}

		return SecureOutputManifestClassificationResult::Classified(SecureOutputManifestClassificationDecision::EntryClassifiedForManifestOnly, entry->Kind, payloadDecision, true, true, L"Guest-visible output is classified as guest-visible/shared state only.");
	}

	std::shared_ptr<SecureOutputManifestClassificationResult> SecureOutputManifestClassificationPolicy::ClassifyRetireVisibleState(std::shared_ptr<SecureOutputManifestEntry> entry, SecureCheckpointPayloadDecision payloadDecision)
	{
		if (entry->PayloadClass != SecureCheckpointPayloadClass::GuestVisibleState)
		{
			return DenyWrongPayload(entry);
		}

		return SecureOutputManifestClassificationResult::Classified(SecureOutputManifestClassificationDecision::EntryClassifiedForManifestOnly, entry->Kind, payloadDecision, true, true, L"Retire-visible state is classified only as guest-visible architectural state.");
	}

	std::shared_ptr<SecureOutputManifestClassificationResult> SecureOutputManifestClassificationPolicy::ClassifyRecomputedAfterRestore(std::shared_ptr<SecureOutputManifestEntry> entry, SecureCheckpointPayloadDecision payloadDecision)
	{
		if (!entry->RestoreValidationProven)
		{
			return Deny(SecureOutputManifestClassificationDecision::DeniedRecomputedStateRestoreProof, entry->Kind, payloadDecision, L"Recomputed-after-restore state requires restore validation proof.");
		}

		return SecureOutputManifestClassificationResult::Classified(SecureOutputManifestClassificationDecision::EntryClassifiedForManifestOnly, entry->Kind, payloadDecision, false, true, L"Recomputed-after-restore state is classified as rebuild-only manifest coverage.");
	}

	std::shared_ptr<SecureOutputManifestClassificationResult> SecureOutputManifestClassificationPolicy::ClassifiedManifestOnly(SecureOutputManifestEntryKind entryKind, SecureCheckpointPayloadDecision payloadDecision, const std::wstring &reason)
	{
		return SecureOutputManifestClassificationResult::Classified(SecureOutputManifestClassificationDecision::EntryClassifiedForManifestOnly, entryKind, payloadDecision, false, true, reason);
	}

	SecureCheckpointPayloadDecision SecureOutputManifestClassificationPolicy::ClassifyPayload(SecureCheckpointPayloadClass payloadClass, const std::optional<std::shared_ptr<SecurePrivateMemorySealedPayloadContract>> &privateMemoryContract)
	{
		if (payloadClass == SecureCheckpointPayloadClass::SecurePrivateMemory)
		{
			return (privateMemoryContract.has_value() && privateMemoryContract.value() && privateMemoryContract.value()->IsComplete()) ? (std::make_shared<SecureCheckpointPayloadPolicy>(true))->Classify(payloadClass) : SecureCheckpointPayloadDecision::DeniedPrivateMemoryWithoutSealedPayload;
		}

		return SecureCheckpointPayloadPolicy::getFailClosed()->Classify(payloadClass);
	}

	SecureOutputManifestClassificationDecision SecureOutputManifestClassificationPolicy::MapPayloadDenial(SecureCheckpointPayloadClass payloadClass, SecureCheckpointPayloadDecision payloadDecision)
	{
	// C# TO C++ CONVERTER TASK: The following 'switch expression' was not converted by C# to C++ Converter:
	//			return payloadDecision switch
	//			{
	//				SecureCheckpointPayloadDecision.DeniedHostOwnedEvidence => SecureOutputManifestClassificationDecision.DeniedHostOwnedEvidence,
	//				SecureCheckpointPayloadDecision.DeniedDebugTraceAsGuestState => SecureOutputManifestClassificationDecision.DeniedDebugTraceAsGuestState,
	//				SecureCheckpointPayloadDecision.DeniedRawMeasurementSecret or SecureCheckpointPayloadDecision.DeniedRawSealingKey => SecureOutputManifestClassificationDecision.DeniedRawSecret,
	//ignore
	//				SecureCheckpointPayloadDecision.DeniedActiveHostPointer => SecureOutputManifestClassificationDecision.DeniedActiveHostPointer,
	//				SecureCheckpointPayloadDecision.DeniedPrivateMemoryWithoutSealedPayload => SecureOutputManifestClassificationDecision.DeniedPrivateMemoryContract,
	//				SecureCheckpointPayloadDecision.DeniedCompatibilityProjectionAuthority
	//					when payloadClass == SecureCheckpointPayloadClass.VmcsProjectionMetadata => SecureOutputManifestClassificationDecision.DeniedVmcsProjectionAuthority,
	//				SecureCheckpointPayloadDecision.DeniedCompatibilityProjectionAuthority =>
	//					SecureOutputManifestClassificationDecision.DeniedCompatibilityMetadataAuthority,
	//				_ => SecureOutputManifestClassificationDecision.DeniedWrongPayloadClass,
	//			};
	}

	std::wstring SecureOutputManifestClassificationPolicy::PayloadReason(SecureCheckpointPayloadDecision payloadDecision)
	{
		std::wstring temp_var154;
		switch (payloadDecision)
		{
			case SecureCheckpointPayloadDecision::DeniedHostOwnedEvidence:
				temp_var154 = L"Host-owned evidence, scheduler evidence, backend binding evidence and native tokens are not secure output manifest payloads.";
				break;
			case SecureCheckpointPayloadDecision::DeniedCompatibilityProjectionAuthority:
				temp_var154 = L"VMCS and compatibility projection metadata cannot be secure output manifest authority.";
				break;
			case SecureCheckpointPayloadDecision::DeniedDebugTraceAsGuestState:
				temp_var154 = L"Debug traces cannot be restored as secure guest state.";
				break;
			case SecureCheckpointPayloadDecision::DeniedRawMeasurementSecret:
			case SecureCheckpointPayloadDecision::DeniedRawSealingKey:
				temp_var154 = L"Raw measurement secrets and raw sealing keys are not manifest payloads.";
				break;
			case SecureCheckpointPayloadDecision::DeniedActiveHostPointer:
				temp_var154 = L"Active host pointers are not portable manifest payloads.";
				break;
			case SecureCheckpointPayloadDecision::DeniedPrivateMemoryWithoutSealedPayload:
				temp_var154 = L"Private memory requires a complete sealed/encrypted payload contract before classification.";
				break;
			default:
				temp_var154 = L"Secure output manifest payload class is not classified.";
				break;
		}
		return temp_var154;
	}

	std::shared_ptr<SecureOutputManifestClassificationResult> SecureOutputManifestClassificationPolicy::DenyWrongPayload(std::shared_ptr<SecureOutputManifestEntry> entry)
	{
		return Deny(SecureOutputManifestClassificationDecision::DeniedWrongPayloadClass, entry->Kind, SecureCheckpointPayloadDecision::Allowed, L"Secure output manifest entry uses the wrong payload class for its entry kind.");
	}

	std::shared_ptr<SecureOutputManifestClassificationResult> SecureOutputManifestClassificationPolicy::Deny(SecureOutputManifestClassificationDecision decision, SecureOutputManifestEntryKind entryKind, SecureCheckpointPayloadDecision payloadDecision, const std::wstring &reason)
	{
		return SecureOutputManifestClassificationResult::Denied(decision, entryKind, payloadDecision, reason);
	}

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private static bool ContainsEntry(IReadOnlyList<SecureOutputManifestEntry> entries, SecureOutputManifestEntryKind kind)
	bool SecureOutputManifestClassificationPolicy::ContainsEntry(const std::vector<std::shared_ptr<SecureOutputManifestEntry>> &entries, SecureOutputManifestEntryKind kind)
	{
		for (int index = 0; index < entries.size(); index++)
		{
			if (entries[index]->Kind == kind)
			{
				return true;
			}
		}

		return false;
	}
}
