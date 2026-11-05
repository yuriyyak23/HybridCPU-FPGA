#pragma once

#include "SecureCheckpointPayloadPolicy.h"
#include "../Policies/Migration/SecureMigrationAdmissionPolicy.h"
#include <string>
#include <vector>
#include <optional>
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{

	enum class SecureOutputManifestEntryKind : unsigned char
	{
		RequestState = 0,
		InternalBackendResult = 1,
		InternalCompletionRecord = 2,
		GuestVisibleOutput = 3,
		RetireVisibleState = 4,
		RecomputedAfterRestoreState = 5
	};

	enum class SecureOutputManifestClassificationDecision : unsigned char
	{
		EntryClassifiedForManifestOnly = 0,
		CompleteManifestClassified = 1,
		DeniedMissingManifestEntry = 2,
		DeniedOwnerPathUnclassified = 3,
		DeniedWrongPayloadClass = 4,
		DeniedPrivateMemoryContract = 5,
		DeniedHostOwnedEvidence = 6,
		DeniedDebugTraceAsGuestState = 7,
		DeniedVmcsProjectionAuthority = 8,
		DeniedCompatibilityMetadataAuthority = 9,
		DeniedRawSecret = 10,
		DeniedActiveHostPointer = 11,
		DeniedRecomputedStateRestoreProof = 12
	};

	struct SecureOutputManifestEntry final
	{
		SecureOutputManifestEntryKind Kind;
		SecureCheckpointPayloadClass PayloadClass;
		bool OwnerPathClassified;
		bool RestoreValidationProven;

		SecureOutputManifestEntry(SecureOutputManifestEntryKind kind, SecureCheckpointPayloadClass payloadClass, bool ownerPathClassified, bool restoreValidationProven)
			: Kind(kind), PayloadClass(payloadClass), OwnerPathClassified(ownerPathClassified), RestoreValidationProven(restoreValidationProven) {}

		static std::shared_ptr<SecureOutputManifestEntry> RequestState() { return std::make_shared<SecureOutputManifestEntry>(SecureOutputManifestEntryKind::RequestState, SecureCheckpointPayloadClass::SecurePolicyDescriptor, true, true); }
		static std::shared_ptr<SecureOutputManifestEntry> InternalBackendResult() { return std::make_shared<SecureOutputManifestEntry>(SecureOutputManifestEntryKind::InternalBackendResult, SecureCheckpointPayloadClass::SecurePolicyDescriptor, true, true); }
		static std::shared_ptr<SecureOutputManifestEntry> InternalCompletionRecord() { return std::make_shared<SecureOutputManifestEntry>(SecureOutputManifestEntryKind::InternalCompletionRecord, SecureCheckpointPayloadClass::SecurePolicyDescriptor, true, true); }
		static std::shared_ptr<SecureOutputManifestEntry> GuestVisibleOutput() { return std::make_shared<SecureOutputManifestEntry>(SecureOutputManifestEntryKind::GuestVisibleOutput, SecureCheckpointPayloadClass::GuestVisibleState, true, true); }
		static std::shared_ptr<SecureOutputManifestEntry> RetireVisibleState() { return std::make_shared<SecureOutputManifestEntry>(SecureOutputManifestEntryKind::RetireVisibleState, SecureCheckpointPayloadClass::GuestVisibleState, true, true); }
		static std::shared_ptr<SecureOutputManifestEntry> RecomputedAfterRestoreState() { return std::make_shared<SecureOutputManifestEntry>(SecureOutputManifestEntryKind::RecomputedAfterRestoreState, SecureCheckpointPayloadClass::SecurePolicyDescriptor, true, true); }
	};

	struct SecureOutputManifestClassificationResult final
	{
		SecureOutputManifestClassificationDecision Decision;
		SecureOutputManifestEntryKind EntryKind;
		SecureCheckpointPayloadDecision PayloadDecision;
		bool ManifestClassified;
		bool CheckpointPayloadIncluded;
		bool RestoreRevalidationRequired;
		bool CreatesRuntimeAuthority;
		bool CreatesCompletionPublicationAuthority;
		bool CreatesRetirePublicationAuthority;
		std::wstring Reason;
		bool IsDenied;

		SecureOutputManifestClassificationResult(SecureOutputManifestClassificationDecision decision, SecureOutputManifestEntryKind entryKind, SecureCheckpointPayloadDecision payloadDecision, bool classified, bool checkpointIncluded, bool restoreRequired, bool runtimeAuthority, bool completionAuthority, bool retireAuthority, const std::wstring& reason)
			: Decision(decision), EntryKind(entryKind), PayloadDecision(payloadDecision), ManifestClassified(classified), CheckpointPayloadIncluded(checkpointIncluded), RestoreRevalidationRequired(restoreRequired), CreatesRuntimeAuthority(runtimeAuthority), CreatesCompletionPublicationAuthority(completionAuthority), CreatesRetirePublicationAuthority(retireAuthority), Reason(reason), IsDenied(!classified) {}

		bool CreatesAnyRuntimeOrPublicationAuthority() const noexcept { return CreatesRuntimeAuthority || CreatesCompletionPublicationAuthority || CreatesRetirePublicationAuthority; }
		static std::shared_ptr<SecureOutputManifestClassificationResult> Classified(SecureOutputManifestClassificationDecision decision, SecureOutputManifestEntryKind kind, SecureCheckpointPayloadDecision payload, bool checkpointIncluded, bool restoreRequired, const std::wstring& reason)
		{
			return std::make_shared<SecureOutputManifestClassificationResult>(decision, kind, payload, true, checkpointIncluded, restoreRequired, false, false, false, reason);
		}
		static std::shared_ptr<SecureOutputManifestClassificationResult> Denied(SecureOutputManifestClassificationDecision decision, SecureOutputManifestEntryKind kind, SecureCheckpointPayloadDecision payload, const std::wstring& reason)
		{
			return std::make_shared<SecureOutputManifestClassificationResult>(decision, kind, payload, false, false, false, false, false, false, reason);
		}
	};
// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct SecureOutputManifestEntry(SecureOutputManifestEntryKind Kind, SecureCheckpointPayloadClass PayloadClass, bool OwnerPathClassified, bool RestoreValidationProven)
//	{
//		public static SecureOutputManifestEntry RequestState()
//		{
//			return new(SecureOutputManifestEntryKind.RequestState, SecureCheckpointPayloadClass.SecurePolicyDescriptor, OwnerPathClassified: true, RestoreValidationProven: true);
//		}
//
//		public static SecureOutputManifestEntry InternalBackendResult()
//		{
//			return new(SecureOutputManifestEntryKind.InternalBackendResult, SecureCheckpointPayloadClass.SecurePolicyDescriptor, OwnerPathClassified: true, RestoreValidationProven: true);
//		}
//
//		public static SecureOutputManifestEntry InternalCompletionRecord()
//		{
//			return new(SecureOutputManifestEntryKind.InternalCompletionRecord, SecureCheckpointPayloadClass.SecurePolicyDescriptor, OwnerPathClassified: true, RestoreValidationProven: true);
//		}
//
//		public static SecureOutputManifestEntry GuestVisibleOutput()
//		{
//			return new(SecureOutputManifestEntryKind.GuestVisibleOutput, SecureCheckpointPayloadClass.GuestVisibleState, OwnerPathClassified: true, RestoreValidationProven: true);
//		}
//
//		public static SecureOutputManifestEntry RetireVisibleState()
//		{
//			return new(SecureOutputManifestEntryKind.RetireVisibleState, SecureCheckpointPayloadClass.GuestVisibleState, OwnerPathClassified: true, RestoreValidationProven: true);
//		}
//
//		public static SecureOutputManifestEntry RecomputedAfterRestoreState()
//		{
//			return new(SecureOutputManifestEntryKind.RecomputedAfterRestoreState, SecureCheckpointPayloadClass.SecurePolicyDescriptor, OwnerPathClassified: true, RestoreValidationProven: true);
//		}
//	}

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct SecureOutputManifestClassificationResult(SecureOutputManifestClassificationDecision Decision, SecureOutputManifestEntryKind EntryKind, SecureCheckpointPayloadDecision PayloadDecision, bool ManifestClassified, bool CheckpointPayloadIncluded, bool RestoreRevalidationRequired, bool CreatesRuntimeAuthority, bool CreatesCompletionPublicationAuthority, bool CreatesRetirePublicationAuthority, string Reason)
//	{
//		public bool IsDenied
//		{
//			get
//			{
//				return !ManifestClassified;
//			}
//		}
//
//		public bool CreatesAnyRuntimeOrPublicationAuthority
//		{
//			get
//			{
//				return CreatesRuntimeAuthority || CreatesCompletionPublicationAuthority || CreatesRetirePublicationAuthority;
//			}
//		}
//
//		public static SecureOutputManifestClassificationResult Classified(SecureOutputManifestClassificationDecision decision, SecureOutputManifestEntryKind entryKind, SecureCheckpointPayloadDecision payloadDecision, bool checkpointPayloadIncluded, bool restoreRevalidationRequired, string reason)
//		{
//			return new(decision, entryKind, payloadDecision, ManifestClassified: true, checkpointPayloadIncluded, restoreRevalidationRequired, CreatesRuntimeAuthority: false, CreatesCompletionPublicationAuthority: false, CreatesRetirePublicationAuthority: false, reason);
//		}
//
//		public static SecureOutputManifestClassificationResult Denied(SecureOutputManifestClassificationDecision decision, SecureOutputManifestEntryKind entryKind, SecureCheckpointPayloadDecision payloadDecision, string reason)
//		{
//			return new(decision, entryKind, payloadDecision, ManifestClassified: false, CheckpointPayloadIncluded: false, RestoreRevalidationRequired: false, CreatesRuntimeAuthority: false, CreatesCompletionPublicationAuthority: false, CreatesRetirePublicationAuthority: false, reason);
//		}
//	}

	class SecureOutputManifestClassificationPolicy final : public std::enable_shared_from_this<SecureOutputManifestClassificationPolicy>
	{
	private:
		static inline const std::shared_ptr<SecureOutputManifestClassificationPolicy> FailClosed = std::make_shared<SecureOutputManifestClassificationPolicy>();
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private static readonly IReadOnlyList<SecureOutputManifestEntryKind> RequiredPositivePathEntries = Array.AsReadOnly(RequiredEntries);
		static inline const std::vector<SecureOutputManifestEntryKind> RequiredEntries = {SecureOutputManifestEntryKind::RequestState, SecureOutputManifestEntryKind::InternalBackendResult, SecureOutputManifestEntryKind::InternalCompletionRecord, SecureOutputManifestEntryKind::GuestVisibleOutput, SecureOutputManifestEntryKind::RetireVisibleState, SecureOutputManifestEntryKind::RecomputedAfterRestoreState};
		static inline const std::vector<SecureOutputManifestEntryKind> RequiredPositivePathEntries = RequiredEntries;

	public:
		static std::shared_ptr<SecureOutputManifestClassificationPolicy> getFailClosed();

		static const std::vector<SecureOutputManifestEntryKind> getRequiredPositivePathEntries();

		std::shared_ptr<SecureOutputManifestClassificationResult> ClassifyEntry(std::shared_ptr<SecureOutputManifestEntry> entry, std::optional<std::shared_ptr<SecurePrivateMemorySealedPayloadContract>> privateMemoryContract = std::nullopt);

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: public SecureOutputManifestClassificationResult ClassifyManifest(IReadOnlyList<SecureOutputManifestEntry> entries, System.Nullable<SecurePrivateMemorySealedPayloadContract> privateMemoryContract = null)
		std::shared_ptr<SecureOutputManifestClassificationResult> ClassifyManifest(const std::vector<std::shared_ptr<SecureOutputManifestEntry>> &entries, std::optional<std::shared_ptr<SecurePrivateMemorySealedPayloadContract>> privateMemoryContract = std::nullopt);

	private:
		static std::shared_ptr<SecureOutputManifestClassificationResult> ClassifyRequestState(std::shared_ptr<SecureOutputManifestEntry> entry);

		static std::shared_ptr<SecureOutputManifestClassificationResult> ClassifyGuestVisibleOutput(std::shared_ptr<SecureOutputManifestEntry> entry, SecureCheckpointPayloadDecision payloadDecision);

		static std::shared_ptr<SecureOutputManifestClassificationResult> ClassifyRetireVisibleState(std::shared_ptr<SecureOutputManifestEntry> entry, SecureCheckpointPayloadDecision payloadDecision);

		static std::shared_ptr<SecureOutputManifestClassificationResult> ClassifyRecomputedAfterRestore(std::shared_ptr<SecureOutputManifestEntry> entry, SecureCheckpointPayloadDecision payloadDecision);

		static std::shared_ptr<SecureOutputManifestClassificationResult> ClassifiedManifestOnly(SecureOutputManifestEntryKind entryKind, SecureCheckpointPayloadDecision payloadDecision, const std::wstring &reason);

		static SecureCheckpointPayloadDecision ClassifyPayload(SecureCheckpointPayloadClass payloadClass, const std::optional<std::shared_ptr<SecurePrivateMemorySealedPayloadContract>> &privateMemoryContract);

		static SecureOutputManifestClassificationDecision MapPayloadDenial(SecureCheckpointPayloadClass payloadClass, SecureCheckpointPayloadDecision payloadDecision);

		static std::wstring PayloadReason(SecureCheckpointPayloadDecision payloadDecision);

		static std::shared_ptr<SecureOutputManifestClassificationResult> DenyWrongPayload(std::shared_ptr<SecureOutputManifestEntry> entry);

		static std::shared_ptr<SecureOutputManifestClassificationResult> Deny(SecureOutputManifestClassificationDecision decision, SecureOutputManifestEntryKind entryKind, SecureCheckpointPayloadDecision payloadDecision, const std::wstring &reason);

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private static bool ContainsEntry(IReadOnlyList<SecureOutputManifestEntry> entries, SecureOutputManifestEntryKind kind)
		static bool ContainsEntry(const std::vector<std::shared_ptr<SecureOutputManifestEntry>> &entries, SecureOutputManifestEntryKind kind);
	};

}
