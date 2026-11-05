#pragma once

#include "../../../Pipeline/Safety/SafetyVerifier.Types.h"
#include "../Descriptors/AcceleratorCommandDescriptor.h"
#include <stdexcept>
#include <string>
#include <optional>
#include <format>
#include "tangible_exception_helper.h"
#include <memory>
#include "../../../../../tangible_string_helper.h"

namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Descriptors { struct AcceleratorOwnerBinding; class AcceleratorCommandDescriptor; }
namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Auth { class AcceleratorInvalidOwnerCompletionResult; class AcceleratorGuardDecision; class AcceleratorGuardEvidence; struct AcceleratorMappingEpoch; }

using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Descriptors;

namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Auth
{

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct AcceleratorDomainTag(ulong Value);

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct AcceleratorMappingEpoch(ulong Value);

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct AcceleratorIommuDomainEpoch(ulong Value);

	enum class AcceleratorGuardEvidenceSource : unsigned char
	{
		GuardPlane = 0,
		RawVirtualThreadIdHint = 1,
		TokenHandle = 2,
		Telemetry = 3,
		ReplayCertificateIdentity = 4,
		RegistryMetadata = 5
	};

	enum class AcceleratorGuardSurface : unsigned char
	{
		DescriptorAcceptance = 0,
		CapabilityAcceptance = 1,
		SubmitAdmission = 2,
		DeviceExecution = 3,
		Commit = 4,
		ExceptionPublication = 5,
		MappingEpochValidation = 6
	};

	enum class AcceleratorGuardFault : unsigned char
	{
		None = 0,
		MissingGuardEvidence = 1,
		RejectedGuard = 2,
		EvidenceSourceNotAuthority = 3,
		OwnerMismatch = 4,
		DomainMismatch = 5,
		DescriptorOwnerBindingMismatch = 6,
		MappingEpochDrift = 7,
		IommuDomainEpochDrift = 8,
		CapabilityGuardMissing = 9,
		InvalidOwnerCompletion = 10
	};

	enum class AcceleratorInvalidOwnerCompletionDisposition : unsigned char
	{
		None = 0,
		Faulted = 1,
		Abandoned = 2
	};

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public sealed record AcceleratorGuardEvidence
//	{
//		public required AcceleratorGuardEvidenceSource Source
//		{
//			get;
//			set;
//		}
//
//		public required ushort OwnerVirtualThreadId
//		{
//			get;
//			set;
//		}
//
//		public required uint OwnerContextId
//		{
//			get;
//			set;
//		}
//
//		public required uint OwnerCoreId
//		{
//			get;
//			set;
//		}
//
//		public required uint OwnerPodId
//		{
//			get;
//			set;
//		}
//
//		public required AcceleratorDomainTag DomainTag
//		{
//			get;
//			set;
//		}
//
//		public required ulong ActiveDomainCertificate
//		{
//			get;
//			set;
//		}
//
//		public AcceleratorMappingEpoch MappingEpoch
//		{
//			get;
//			set;
//		}
//
//		public AcceleratorIommuDomainEpoch IommuDomainEpoch
//		{
//			get;
//			set;
//		}
//
//		public ulong EvidenceIdentity
//		{
//			get;
//			set;
//		}
//
//		public System.Nullable<string> RegistryAcceleratorId
//		{
//			get;
//			set;
//		}
//
//		public static AcceleratorGuardEvidence FromGuardPlane(AcceleratorOwnerBinding ownerBinding, ulong activeDomainCertificate, AcceleratorMappingEpoch mappingEpoch = default, AcceleratorIommuDomainEpoch iommuDomainEpoch = default)
//		{
//			ArgumentNullException.ThrowIfNull(ownerBinding);
//			return new AcceleratorGuardEvidence { Source = AcceleratorGuardEvidenceSource.GuardPlane, OwnerVirtualThreadId = ownerBinding.OwnerVirtualThreadId, OwnerContextId = ownerBinding.OwnerContextId, OwnerCoreId = ownerBinding.OwnerCoreId, OwnerPodId = ownerBinding.OwnerPodId, DomainTag = new AcceleratorDomainTag(ownerBinding.DomainTag), ActiveDomainCertificate = activeDomainCertificate, MappingEpoch = mappingEpoch, IommuDomainEpoch = iommuDomainEpoch };
//		}
//
//		public static AcceleratorGuardEvidence FromEvidencePlane(AcceleratorGuardEvidenceSource source, AcceleratorOwnerBinding ownerBinding, ulong evidenceIdentity = 0, System.Nullable<string> registryAcceleratorId = nullptr)
//		{
//			if (source == AcceleratorGuardEvidenceSource.GuardPlane)
//			{
//				throw new ArgumentException("Use FromGuardPlane for guard-plane evidence.", nameof(source));
//			}
//
//			AcceleratorGuardEvidence evidence = FromGuardPlane(ownerBinding, activeDomainCertificate: ownerBinding.DomainTag);
//			return evidence with { Source = source, EvidenceIdentity = evidenceIdentity, RegistryAcceleratorId = registryAcceleratorId };
//		}
//
//		public static AcceleratorGuardEvidence FromRawVirtualThreadIdHint(ushort rawVirtualThreadId, AcceleratorOwnerBinding ownerBinding)
//		{
//			return FromEvidencePlane(AcceleratorGuardEvidenceSource.RawVirtualThreadIdHint, ownerBinding) with { OwnerVirtualThreadId = rawVirtualThreadId };
//		}
//	}

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct AcceleratorGuardDecision
//	{
//		private AcceleratorGuardDecision(AcceleratorGuardSurface surface, AcceleratorGuardFault fault, System.Nullable<AcceleratorOwnerBinding> descriptorOwnerBinding, System.Nullable<AcceleratorGuardEvidence> evidence, LegalityDecision legalityDecision, string message)
//		{
//			Surface = surface;
//			Fault = fault;
//			DescriptorOwnerBinding = descriptorOwnerBinding;
//			Evidence = evidence;
//			LegalityDecision = legalityDecision;
//			Message = message;
//		}
//
//		public AcceleratorGuardSurface Surface
//		{
//			get;
//		}
//
//		public AcceleratorGuardFault Fault
//		{
//			get;
//		}
//
//		public System.Nullable<AcceleratorOwnerBinding> DescriptorOwnerBinding
//		{
//			get;
//		}
//
//		public System.Nullable<AcceleratorGuardEvidence> Evidence
//		{
//			get;
//		}
//
//		public LegalityDecision LegalityDecision
//		{
//			get;
//		}
//
//		public string Message
//		{
//			get;
//		}
//
//		public AcceleratorMappingEpoch MappingEpoch
//		{
//			get
//			{
//				return (Evidence == nullptr ? nullptr : Evidence.MappingEpoch) ?? default;
//			}
//		}
//
//		public AcceleratorIommuDomainEpoch IommuDomainEpoch
//		{
//			get
//			{
//				return (Evidence == nullptr ? nullptr : Evidence.IommuDomainEpoch) ?? default;
//			}
//		}
//
//		public bool IsAllowed
//		{
//			get
//			{
//				return Fault == AcceleratorGuardFault.None && LegalityDecision.IsAllowed && (Evidence == nullptr ? nullptr : Evidence.Source) == AcceleratorGuardEvidenceSource.GuardPlane;
//			}
//		}
//
//		public static AcceleratorGuardDecision Allow(AcceleratorGuardSurface surface, AcceleratorOwnerBinding descriptorOwnerBinding, AcceleratorGuardEvidence evidence, string message)
//		{
//			ArgumentNullException.ThrowIfNull(descriptorOwnerBinding);
//			ArgumentNullException.ThrowIfNull(evidence);
//			return new AcceleratorGuardDecision(surface, AcceleratorGuardFault.None, descriptorOwnerBinding, evidence, LegalityDecision.Allow(LegalityAuthoritySource.GuardPlane, attemptedReplayCertificateReuse: false), message);
//		}
//
//		public static AcceleratorGuardDecision Reject(AcceleratorGuardSurface surface, AcceleratorGuardFault fault, System.Nullable<AcceleratorOwnerBinding> descriptorOwnerBinding, System.Nullable<AcceleratorGuardEvidence> evidence, RejectKind rejectKind, string message)
//		{
//			if (fault == AcceleratorGuardFault.None)
//			{
//				throw new ArgumentException("Use Allow for successful guard decisions.", nameof(fault));
//			}
//
//			bool attemptedReplayCertificateAuthority = (evidence == nullptr ? nullptr : evidence.Source) == AcceleratorGuardEvidenceSource.ReplayCertificateIdentity;
//			return new AcceleratorGuardDecision(surface, fault, descriptorOwnerBinding, evidence, LegalityDecision.Reject(rejectKind, CertificateRejectDetail.None, LegalityAuthoritySource.GuardPlane, attemptedReplayCertificateAuthority), message);
//		}
//	}

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public sealed record AcceleratorInvalidOwnerCompletionResult
//	{
//		private AcceleratorInvalidOwnerCompletionResult(AcceleratorGuardDecision guardDecision, AcceleratorInvalidOwnerCompletionDisposition disposition, bool privilegedDiagnosticRecorded, string message)
//		{
//			GuardDecision = guardDecision;
//			Disposition = disposition;
//			PrivilegedDiagnosticRecorded = privilegedDiagnosticRecorded;
//			Message = message;
//		}
//
//		public AcceleratorGuardDecision GuardDecision
//		{
//			get;
//		}
//
//		public AcceleratorInvalidOwnerCompletionDisposition Disposition
//		{
//			get;
//		}
//
//		public bool UserVisiblePublicationAllowed
//		{
//			get
//			{
//				return false;
//			}
//		}
//
//		public bool PrivilegedDiagnosticRecorded
//		{
//			get;
//		}
//
//		public string Message
//		{
//			get;
//		}
//
//		public static AcceleratorInvalidOwnerCompletionResult GuardStillValid(AcceleratorGuardDecision guardDecision)
//		{
//			return new AcceleratorInvalidOwnerCompletionResult(guardDecision, AcceleratorInvalidOwnerCompletionDisposition.None, privilegedDiagnosticRecorded: false, "Owner/domain guard is still valid; Phase 05 still has no user-visible commit publication path.");
//		}
//
//		public static AcceleratorInvalidOwnerCompletionResult InvalidOwner(AcceleratorGuardDecision guardDecision, bool privilegedDiagnosticRecorded)
//		{
//			AcceleratorInvalidOwnerCompletionDisposition disposition = guardDecision.Fault is AcceleratorGuardFault.OwnerMismatch or AcceleratorGuardFault.DomainMismatch or AcceleratorGuardFault.MappingEpochDrift or AcceleratorGuardFault.IommuDomainEpochDrift ? AcceleratorInvalidOwnerCompletionDisposition.Abandoned : AcceleratorInvalidOwnerCompletionDisposition.Faulted;
//
//			return new AcceleratorInvalidOwnerCompletionResult(guardDecision, disposition, privilegedDiagnosticRecorded, "Invalid-owner completion is model evidence only; user-visible architectural publication is forbidden.");
//		}
//	}

	struct AcceleratorDomainTag final { unsigned long long Value = 0; bool operator==(const AcceleratorDomainTag&) const = default; };
	struct AcceleratorMappingEpoch final { unsigned long long Value = 0; bool operator==(const AcceleratorMappingEpoch&) const = default; };
	struct AcceleratorIommuDomainEpoch final { unsigned long long Value = 0; bool operator==(const AcceleratorIommuDomainEpoch&) const = default; };
	class AcceleratorGuardEvidence final {
	public:
		AcceleratorGuardEvidenceSource Source{};
		unsigned short OwnerVirtualThreadId = 0;
		unsigned int OwnerContextId = 0, OwnerCoreId = 0, OwnerPodId = 0;
		AcceleratorDomainTag DomainTag{};
		unsigned long long ActiveDomainCertificate = 0;
		AcceleratorMappingEpoch MappingEpoch{};
		AcceleratorIommuDomainEpoch IommuDomainEpoch{};
		unsigned long long EvidenceIdentity = 0;
		std::optional<std::wstring> RegistryAcceleratorId;
		static std::shared_ptr<AcceleratorGuardEvidence> FromGuardPlane(const std::shared_ptr<AcceleratorOwnerBinding>& owner, unsigned long long certificate, AcceleratorMappingEpoch mapping = {}, AcceleratorIommuDomainEpoch iommu = {}) {
			if (!owner) throw std::invalid_argument("ownerBinding");
			auto e = std::make_shared<AcceleratorGuardEvidence>();
			e->Source = AcceleratorGuardEvidenceSource::GuardPlane; e->OwnerVirtualThreadId = owner->OwnerVirtualThreadId;
			e->OwnerContextId = owner->OwnerContextId; e->OwnerCoreId = owner->OwnerCoreId; e->OwnerPodId = owner->OwnerPodId;
			e->DomainTag = {owner->DomainTag}; e->ActiveDomainCertificate = certificate; e->MappingEpoch = mapping; e->IommuDomainEpoch = iommu; return e;
		}
		static std::shared_ptr<AcceleratorGuardEvidence> FromEvidencePlane(AcceleratorGuardEvidenceSource source, const std::shared_ptr<AcceleratorOwnerBinding>& owner, unsigned long long identity = 0, std::optional<std::wstring> registry = std::nullopt) {
			if (source == AcceleratorGuardEvidenceSource::GuardPlane || !owner) throw std::invalid_argument("source or ownerBinding");
			auto e = FromGuardPlane(owner, owner->DomainTag); e->Source = source; e->EvidenceIdentity = identity; e->RegistryAcceleratorId = std::move(registry); return e;
		}
		static std::shared_ptr<AcceleratorGuardEvidence> FromRawVirtualThreadIdHint(unsigned short vt, const std::shared_ptr<AcceleratorOwnerBinding>& owner) {
			auto e = FromEvidencePlane(AcceleratorGuardEvidenceSource::RawVirtualThreadIdHint, owner); e->OwnerVirtualThreadId = vt; return e;
		}
	};
	class AcceleratorGuardDecision final {
	public:
		const AcceleratorGuardSurface Surface;
		const AcceleratorGuardFault Fault;
		const std::shared_ptr<AcceleratorOwnerBinding> DescriptorOwnerBinding;
		const std::shared_ptr<AcceleratorGuardEvidence> Evidence;
		const YAKSys_Hybrid_CPU::Core::LegalityDecision LegalityDecision;
		const std::wstring Message;
		const bool IsAllowed;
	private:
		AcceleratorGuardDecision(AcceleratorGuardSurface surface, AcceleratorGuardFault fault, std::shared_ptr<AcceleratorOwnerBinding> owner, std::shared_ptr<AcceleratorGuardEvidence> evidence, YAKSys_Hybrid_CPU::Core::LegalityDecision legality, std::wstring message)
			: Surface(surface), Fault(fault), DescriptorOwnerBinding(std::move(owner)), Evidence(std::move(evidence)), LegalityDecision(legality), Message(std::move(message)), IsAllowed(fault == AcceleratorGuardFault::None && LegalityDecision.isAllowed() && Evidence && Evidence->Source == AcceleratorGuardEvidenceSource::GuardPlane) {}
	public:
		AcceleratorMappingEpoch getMappingEpoch() const { return Evidence ? Evidence->MappingEpoch : AcceleratorMappingEpoch{}; }
		AcceleratorIommuDomainEpoch getIommuDomainEpoch() const { return Evidence ? Evidence->IommuDomainEpoch : AcceleratorIommuDomainEpoch{}; }
		static std::shared_ptr<AcceleratorGuardDecision> Allow(AcceleratorGuardSurface surface, std::shared_ptr<AcceleratorOwnerBinding> owner, std::shared_ptr<AcceleratorGuardEvidence> evidence, const std::wstring& message) {
			if (!owner || !evidence) throw std::invalid_argument("guard owner or evidence");
			return std::shared_ptr<AcceleratorGuardDecision>(new AcceleratorGuardDecision(surface, AcceleratorGuardFault::None, std::move(owner), std::move(evidence), YAKSys_Hybrid_CPU::Core::LegalityDecision::Allow(LegalityAuthoritySource::GuardPlane, false), message));
		}
		static std::shared_ptr<AcceleratorGuardDecision> Reject(AcceleratorGuardSurface surface, AcceleratorGuardFault fault, std::shared_ptr<AcceleratorOwnerBinding> owner, std::shared_ptr<AcceleratorGuardEvidence> evidence, RejectKind reject, const std::wstring& message) {
			if (fault == AcceleratorGuardFault::None) throw std::invalid_argument("Use Allow for successful guard decisions.");
			const bool replay = evidence && evidence->Source == AcceleratorGuardEvidenceSource::ReplayCertificateIdentity;
			return std::shared_ptr<AcceleratorGuardDecision>(new AcceleratorGuardDecision(surface, fault, std::move(owner), std::move(evidence), YAKSys_Hybrid_CPU::Core::LegalityDecision::Reject(reject, CertificateRejectDetail::None, LegalityAuthoritySource::GuardPlane, replay), message));
		}
	};

	class AcceleratorInvalidOwnerCompletionResult final {
	public:
		const std::shared_ptr<AcceleratorGuardDecision> GuardDecision;
		const AcceleratorInvalidOwnerCompletionDisposition Disposition;
		const bool UserVisiblePublicationAllowed = false;
		const bool PrivilegedDiagnosticRecorded;
		const std::wstring Message;
		AcceleratorInvalidOwnerCompletionResult(std::shared_ptr<AcceleratorGuardDecision> decision, AcceleratorInvalidOwnerCompletionDisposition disposition, bool diagnostic, std::wstring message)
			: GuardDecision(std::move(decision)), Disposition(disposition), PrivilegedDiagnosticRecorded(diagnostic), Message(std::move(message)) {}
		static std::shared_ptr<AcceleratorInvalidOwnerCompletionResult> GuardStillValid(std::shared_ptr<AcceleratorGuardDecision> decision) {
			return std::make_shared<AcceleratorInvalidOwnerCompletionResult>(std::move(decision), AcceleratorInvalidOwnerCompletionDisposition::None, false, L"Owner/domain guard is still valid; Phase 05 still has no user-visible commit publication path.");
		}
		static std::shared_ptr<AcceleratorInvalidOwnerCompletionResult> InvalidOwner(std::shared_ptr<AcceleratorGuardDecision> decision, bool diagnostic) {
			if (!decision) throw std::invalid_argument("guardDecision");
			auto fault = decision->Fault;
			auto disposition = fault == AcceleratorGuardFault::OwnerMismatch || fault == AcceleratorGuardFault::DomainMismatch || fault == AcceleratorGuardFault::MappingEpochDrift || fault == AcceleratorGuardFault::IommuDomainEpochDrift ? AcceleratorInvalidOwnerCompletionDisposition::Abandoned : AcceleratorInvalidOwnerCompletionDisposition::Faulted;
			return std::make_shared<AcceleratorInvalidOwnerCompletionResult>(std::move(decision), disposition, diagnostic, L"Invalid-owner completion is model evidence only; user-visible architectural publication is forbidden.");
		}
	};
	class AcceleratorOwnerDomainGuard final : public std::enable_shared_from_this<AcceleratorOwnerDomainGuard>
	{
	private:
		static inline const std::shared_ptr<AcceleratorOwnerDomainGuard> Default = std::make_shared<AcceleratorOwnerDomainGuard>();

	public:
		static std::shared_ptr<AcceleratorOwnerDomainGuard> getDefault();

		std::shared_ptr<AcceleratorGuardDecision> EnsureBeforeDescriptorAcceptance(std::shared_ptr<AcceleratorOwnerBinding> descriptorOwnerBinding, std::optional<std::shared_ptr<AcceleratorGuardEvidence>> &guardEvidence);

		std::shared_ptr<AcceleratorGuardDecision> EnsureBeforeCapabilityAcceptance(std::shared_ptr<AcceleratorOwnerBinding> descriptorOwnerBinding, std::optional<std::shared_ptr<AcceleratorGuardEvidence>> &guardEvidence);

		std::shared_ptr<AcceleratorGuardDecision> EnsureBeforeCapabilityAcceptance(std::shared_ptr<AcceleratorOwnerBinding> descriptorOwnerBinding, std::shared_ptr<AcceleratorGuardDecision> descriptorGuardDecision);

		std::shared_ptr<AcceleratorGuardDecision> EnsureBeforeSubmit(std::shared_ptr<AcceleratorCommandDescriptor> descriptor, std::optional<std::shared_ptr<AcceleratorGuardEvidence>> &guardEvidence);

		std::shared_ptr<AcceleratorGuardDecision> EnsureBeforeDeviceExecution(std::shared_ptr<AcceleratorCommandDescriptor> descriptor, std::optional<std::shared_ptr<AcceleratorGuardEvidence>> &guardEvidence);

		std::shared_ptr<AcceleratorGuardDecision> EnsureBeforeCommit(std::shared_ptr<AcceleratorCommandDescriptor> descriptor, std::optional<std::shared_ptr<AcceleratorGuardEvidence>> &guardEvidence);

		std::shared_ptr<AcceleratorGuardDecision> EnsureBeforeExceptionPublication(std::shared_ptr<AcceleratorCommandDescriptor> descriptor, std::optional<std::shared_ptr<AcceleratorGuardEvidence>> &guardEvidence);

		std::shared_ptr<AcceleratorMappingEpoch> CaptureMappingEpoch(std::shared_ptr<AcceleratorGuardEvidence> guardEvidence);

		std::shared_ptr<AcceleratorGuardDecision> ValidateMappingEpoch(std::shared_ptr<AcceleratorGuardDecision> acceptedGuardDecision, std::optional<std::shared_ptr<AcceleratorGuardEvidence>> &currentGuardEvidence);

		std::shared_ptr<AcceleratorInvalidOwnerCompletionResult> MarkAbandonedOnInvalidOwner(std::shared_ptr<AcceleratorCommandDescriptor> descriptor, std::optional<std::shared_ptr<AcceleratorGuardEvidence>> &completionEvidence, bool recordPrivilegedDiagnostic = true);

		bool IsDescriptorGuardBacked(std::shared_ptr<AcceleratorCommandDescriptor> descriptor, std::wstring &message);

	private:
		static std::shared_ptr<AcceleratorGuardDecision> EvaluateGuard(AcceleratorGuardSurface surface, std::shared_ptr<AcceleratorOwnerBinding> descriptorOwnerBinding, const std::optional<std::shared_ptr<AcceleratorGuardEvidence>> &guardEvidence, const std::optional<std::shared_ptr<AcceleratorGuardDecision>> &priorAcceptedDecision, bool validateEpochs);

		static bool IsDomainCoveredByCertificate(unsigned long long ownerDomainTag, unsigned long long activeDomainCertificate);

		static std::shared_ptr<AcceleratorGuardDecision> Reject(AcceleratorGuardSurface surface, AcceleratorGuardFault fault, std::shared_ptr<AcceleratorOwnerBinding> descriptorOwnerBinding, const std::optional<std::shared_ptr<AcceleratorGuardEvidence>> &guardEvidence, RejectKind rejectKind, const std::wstring &message);
	};

}

