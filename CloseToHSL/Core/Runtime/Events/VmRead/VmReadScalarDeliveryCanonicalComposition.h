#pragma once
#include "../../Governance/Virtualization/VmReadScalarDeliveryAcceptedPolicyResolver.h"

#include "../../../Pipeline/Safety/VirtualizationAdmissionCertificate.h"
#include "../../../Virtualization/Compatibility/Frontend/Retire/VmxRetireModel.h"
#include "../../Domains/Admission/Execution/YAKSys_Hybrid_CPU.Core.ExecutionDomainRuntime.h"
#include "../../Domains/Admission/Memory/YAKSys_Hybrid_CPU.Core.MemoryDomainRuntime.h"
#include "../../Capabilities/CompatibilityControls/CompatibilityControlPolicyOwner.h"
#include "../../../Virtualization/Compatibility/FrozenAbi/VmcsFieldAliases/VmcsField.h"

#include "../../Completion/Observation/DomainCompletionObservationOwner.h"
#include "../../../Virtualization/Compatibility/Frontend/Handlers/YAKSys_Hybrid_CPU.Core.VmxCompatibilityAdmissionService.h"
#include "../../Domains/Services/YAKSys_Hybrid_CPU.Core.DomainRuntimeContext.h"
#include "../../Domains/Authority/YAKSys_Hybrid_CPU.Core.RootAuthorityDescriptor.h"
#include "../../Domains/Descriptors/EvidencePolicy/YAKSys_Hybrid_CPU.Core.EvidencePolicyDescriptor.h"
#include "../../../Pipeline/Certificates/ReplayPhaseSubstrate.h"
#include <string>
#include <limits>
#include <stdexcept>
#include <any>
#include <optional>
#include <mutex>
#include <memory>
#include <atomic>

// C# TO C++ CONVERTER NOTE: Forward class declarations:
namespace YAKSys_Hybrid_CPU::Core { class VmReadScalarResultReceipt; }

using namespace YAKSys_Hybrid_CPU::Core::Vmcs::V2;

namespace YAKSys_Hybrid_CPU::Core
{

	enum class VmReadScalarDeliveryDecision : unsigned char
	{
		Prepared = 0,
		Disabled = 1,
		PolicyDenied = 2,
		AdmissionDenied = 3,
		FieldDenied = 4,
		DestinationDenied = 5,
		ProjectionDenied = 6,
		StaleReceipt = 7,
		DuplicateReceipt = 8,
		SourceDenied = 9
	};

struct VmReadScalarDeliveryResult {
    const VmReadScalarDeliveryDecision Decision; const std::shared_ptr<VmReadScalarResultReceipt> Receipt; const std::wstring Reason;
    const bool IsPrepared; static constexpr bool BackendExecutionAuthorized=false, UnderlyingVirtualizationMutationAuthorized=false, CompletionPublicationAuthorized=false, VmxRetireEffectAuthorized=false;
    VmReadScalarDeliveryResult(VmReadScalarDeliveryDecision decision, std::shared_ptr<VmReadScalarResultReceipt> receipt, std::wstring reason) : Decision(decision), Receipt(std::move(receipt)), Reason(std::move(reason)), IsPrepared(decision == VmReadScalarDeliveryDecision::Prepared && Receipt != nullptr) {}
};
	struct VmReadScalarDeliveryPolicyResolution;
	struct VmReadScalarDeliveryPolicyLookup;
	class VmxMicroOp;
	class IVmReadScalarResultReceiptOwner : public std::enable_shared_from_this<IVmReadScalarResultReceiptOwner>
	{
	public:
		virtual bool ValidateLive(std::shared_ptr<VmReadScalarResultReceipt> receipt, std::optional<unsigned long long> currentRestoreGeneration = std::nullopt) = 0;
		virtual bool ValidateConsumedBinding(std::shared_ptr<VmReadScalarResultReceipt> receipt, unsigned long long currentRestoreGeneration) = 0;
	};

	class VmReadScalarAttemptBinding final : public std::enable_shared_from_this<VmReadScalarAttemptBinding>
	{
	private:
		const std::shared_ptr<VirtualizationAdmissionCertificate> Certificate;

	public:
		VmReadScalarAttemptBinding(std::shared_ptr<VirtualizationAdmissionCertificate> certificate);

		std::shared_ptr<VirtualizationAdmissionCertificate> getCertificate() const;
		unsigned long long getAttemptId() const;
		unsigned long long getIssuerGeneration() const;
		unsigned long long getBundleIdentity() const;
		unsigned long long getReplayEpoch() const;
		unsigned long long getDomainTag() const;
		VmxOperationKind getOperation() const;
	};

	class VmReadScalarResultReceipt final : public std::enable_shared_from_this<VmReadScalarResultReceipt>
	{
	private:
		const std::wstring DecisionId;
		const std::any SourceOwner;
		const unsigned long long SourceEpoch;
		const std::shared_ptr<ExecutionDomainRuntime::SourceCapture> ExecutionDomainCapture;
		const std::shared_ptr<MemoryDomainRuntime::SourceCapture> MemoryDomainCapture;
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly System.Nullable<NeutralCompletionObservationSnapshot> CompletionObservationCapture;
		std::optional<std::shared_ptr<NeutralCompletionObservationSnapshot>> CompletionObservationCapture;
		const std::shared_ptr<CompatibilityControlPolicySnapshot> CompatibilityControlCapture;
		const unsigned long long ProfileGeneration;
		const unsigned long long AttemptId;
		const unsigned long long IssuerGeneration;
		const unsigned long long BundleIdentity;
		const unsigned long long ReplayEpoch;
		const unsigned long long RestoreGeneration;
		const unsigned long long DomainTag;
		const unsigned long long AddressSpaceTag;
		const std::shared_ptr<PrivilegedExecutionStateEpoch> DescriptorEpoch;
		const VmcsField Field;
		const unsigned char DestinationRegister;
		const unsigned long long Value;

		const std::shared_ptr<IVmReadScalarResultReceiptOwner> _owner;
		const std::shared_ptr<VmReadScalarAttemptBinding> _attempt;
		std::atomic<int> _consumed{0};

	public:
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal VmReadScalarResultReceipt(IVmReadScalarResultReceiptOwner owner, VmReadScalarAttemptBinding attempt, string decisionId, Object sourceOwner, ulong sourceEpoch, ExecutionDomainRuntime.SourceCapture? executionDomainCapture, ulong profileGeneration, ulong attemptId, ulong issuerGeneration, ulong bundleIdentity, ulong replayEpoch, ulong restoreGeneration, ulong domainTag, ulong addressSpaceTag, PrivilegedExecutionStateEpoch descriptorEpoch, VmcsField field, byte destinationRegister, ulong value) : this(owner, attempt, decisionId, sourceOwner, sourceEpoch, executionDomainCapture, memoryDomainCapture: null, profileGeneration, attemptId, issuerGeneration, bundleIdentity, replayEpoch, restoreGeneration, domainTag, addressSpaceTag, descriptorEpoch, field, destinationRegister, value)
		VmReadScalarResultReceipt(std::shared_ptr<IVmReadScalarResultReceiptOwner> owner, std::shared_ptr<VmReadScalarAttemptBinding> attempt, const std::wstring &decisionId, std::any sourceOwner, unsigned long long sourceEpoch, std::shared_ptr<ExecutionDomainRuntime::SourceCapture> executionDomainCapture, unsigned long long profileGeneration, unsigned long long attemptId, unsigned long long issuerGeneration, unsigned long long bundleIdentity, unsigned long long replayEpoch, unsigned long long restoreGeneration, unsigned long long domainTag, unsigned long long addressSpaceTag, std::shared_ptr<PrivilegedExecutionStateEpoch> descriptorEpoch, VmcsField field, unsigned char destinationRegister, unsigned long long value);

		VmReadScalarResultReceipt(std::shared_ptr<IVmReadScalarResultReceiptOwner> owner, std::shared_ptr<VmReadScalarAttemptBinding> attempt, const std::wstring &decisionId, std::any sourceOwner, unsigned long long sourceEpoch, std::shared_ptr<NeutralCompletionObservationSnapshot> completionObservationCapture, unsigned long long profileGeneration, unsigned long long attemptId, unsigned long long issuerGeneration, unsigned long long bundleIdentity, unsigned long long replayEpoch, unsigned long long restoreGeneration, unsigned long long domainTag, VmcsField field, unsigned char destinationRegister, unsigned long long value);

		VmReadScalarResultReceipt(std::shared_ptr<IVmReadScalarResultReceiptOwner> owner, std::shared_ptr<VmReadScalarAttemptBinding> attempt, const std::wstring &decisionId, std::any sourceOwner, unsigned long long sourceEpoch, std::shared_ptr<CompatibilityControlPolicySnapshot> compatibilityControlCapture, unsigned long long profileGeneration, unsigned long long attemptId, unsigned long long issuerGeneration, unsigned long long bundleIdentity, unsigned long long replayEpoch, unsigned long long restoreGeneration, unsigned long long domainTag, VmcsField field, unsigned char destinationRegister, unsigned long long value);

		VmReadScalarResultReceipt(std::shared_ptr<IVmReadScalarResultReceiptOwner> owner, std::shared_ptr<VmReadScalarAttemptBinding> attempt, const std::wstring &decisionId, std::any sourceOwner, unsigned long long sourceEpoch, std::shared_ptr<MemoryDomainRuntime::SourceCapture> memoryDomainCapture, unsigned long long profileGeneration, unsigned long long attemptId, unsigned long long issuerGeneration, unsigned long long bundleIdentity, unsigned long long replayEpoch, unsigned long long restoreGeneration, unsigned long long domainTag, unsigned long long addressSpaceTag, std::shared_ptr<PrivilegedExecutionStateEpoch> descriptorEpoch, VmcsField field, unsigned char destinationRegister, unsigned long long value);

	private:
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private VmReadScalarResultReceipt(IVmReadScalarResultReceiptOwner owner, VmReadScalarAttemptBinding attempt, string decisionId, Object sourceOwner, ulong sourceEpoch, ExecutionDomainRuntime.SourceCapture? executionDomainCapture, System.Nullable<MemoryDomainRuntime.SourceCapture> memoryDomainCapture, ulong profileGeneration, ulong attemptId, ulong issuerGeneration, ulong bundleIdentity, ulong replayEpoch, ulong restoreGeneration, ulong domainTag, ulong addressSpaceTag, PrivilegedExecutionStateEpoch descriptorEpoch, VmcsField field, byte destinationRegister, ulong value)
		VmReadScalarResultReceipt(std::shared_ptr<IVmReadScalarResultReceiptOwner> owner, std::shared_ptr<VmReadScalarAttemptBinding> attempt, const std::wstring &decisionId, std::any sourceOwner, unsigned long long sourceEpoch, std::shared_ptr<ExecutionDomainRuntime::SourceCapture> executionDomainCapture, std::shared_ptr<MemoryDomainRuntime::SourceCapture> memoryDomainCapture, std::optional<std::shared_ptr<NeutralCompletionObservationSnapshot>> completionObservationCapture, std::shared_ptr<CompatibilityControlPolicySnapshot> compatibilityControlCapture, unsigned long long profileGeneration, unsigned long long attemptId, unsigned long long issuerGeneration, unsigned long long bundleIdentity, unsigned long long replayEpoch, unsigned long long restoreGeneration, unsigned long long domainTag, unsigned long long addressSpaceTag, std::shared_ptr<PrivilegedExecutionStateEpoch> descriptorEpoch, VmcsField field, unsigned char destinationRegister, unsigned long long value);

	public:
		std::wstring getDecisionId() const;
		std::any getSourceOwner() const;
		unsigned long long getSourceEpoch() const;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal ExecutionDomainRuntime.SourceCapture? ExecutionDomainCapture {get;}
		std::shared_ptr<ExecutionDomainRuntime::SourceCapture> getExecutionDomainCapture() const;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal MemoryDomainRuntime.SourceCapture? MemoryDomainCapture {get;}
		std::shared_ptr<MemoryDomainRuntime::SourceCapture> getMemoryDomainCapture() const;
		std::optional<std::shared_ptr<NeutralCompletionObservationSnapshot>> getCompletionObservationCapture() const;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal CompatibilityControlPolicySnapshot? CompatibilityControlCapture {get;}
		std::shared_ptr<CompatibilityControlPolicySnapshot> getCompatibilityControlCapture() const;
		unsigned long long getProfileGeneration() const;
		unsigned long long getAttemptId() const;
		unsigned long long getIssuerGeneration() const;
		unsigned long long getBundleIdentity() const;
		unsigned long long getReplayEpoch() const;
		unsigned long long getRestoreGeneration() const;
		unsigned long long getDomainTag() const;
		unsigned long long getAddressSpaceTag() const;
		std::shared_ptr<PrivilegedExecutionStateEpoch> getDescriptorEpoch() const;
		VmcsField getField() const;
		unsigned char getDestinationRegister() const;
		unsigned long long getValue() const;
		bool isConsumed() const;
		bool getBackendExecutionAuthorized() const;
		bool getCompletionPublicationAuthorized() const;
		bool getVmxRetireEffectAuthorized() const;

		bool MatchesCarrier(std::shared_ptr<VmxMicroOp> carrier, unsigned long long fieldSelector);

		bool TryValidateSpeculative(std::optional<unsigned long long> currentRestoreGeneration = std::nullopt);

		// Completion projections require the exact still-current committed tuple.
		// Execution/MemoryDomain immutable captures have different lifetime contracts.
		bool MatchesCurrentCompletionObservation(std::shared_ptr<DomainCompletionObservationOwner> observationOwner, std::optional<unsigned long long> currentRestoreGeneration);

		bool TryConsumeAtRetire(unsigned long long currentRestoreGeneration);
	};

	class VmReadScalarDeliveryCanonicalComposition final : public IVmReadScalarResultReceiptOwner
	{
	private:
		const std::shared_ptr<VmReadScalarDeliveryPolicyResolution> PolicyResolution;

		mutable std::mutex _sync;
		const std::shared_ptr<VmxCompatibilityAdmissionService> _admission = std::make_shared<VmxCompatibilityAdmissionService>();
		const std::shared_ptr<DomainRuntimeContext> _context;
		const std::shared_ptr<RootAuthorityDescriptor> _root;
		const std::shared_ptr<EvidencePolicyDescriptor> _evidence;
		const bool _conformanceProven;
		std::shared_ptr<PrivilegedExecutionStateDescriptor> _descriptor;
		std::shared_ptr<PrivilegedExecutionStateEpoch> _currentEpoch;
		unsigned long long _restoreGeneration = 0;
		unsigned long long _generation = 1;
		bool _enabled = false;
		bool _hasReplayObservation = false;
		bool _replayActive = false;
		unsigned long long _replayEpoch = 0;
		unsigned long long _replayCachedPc = 0;
		unsigned long long _completedReplays = 0;

	public:
// C# TO C++ CONVERTER TASK: Throw expressions are not converted by C# to C++ Converter:
// ORIGINAL LINE: internal VmReadScalarDeliveryCanonicalComposition(DomainRuntimeContext context, RootAuthorityDescriptor root, EvidencePolicyDescriptor evidence, PrivilegedExecutionStateDescriptor descriptor, PrivilegedExecutionStateEpoch currentEpoch, ulong restoreGeneration, in VmReadScalarDeliveryPolicyLookup lookup, bool conformanceProven = true)
		VmReadScalarDeliveryCanonicalComposition(std::shared_ptr<DomainRuntimeContext> context, std::shared_ptr<RootAuthorityDescriptor> root, std::shared_ptr<EvidencePolicyDescriptor> evidence, std::shared_ptr<PrivilegedExecutionStateDescriptor> descriptor, std::shared_ptr<PrivilegedExecutionStateEpoch> currentEpoch, unsigned long long restoreGeneration, std::shared_ptr<VmReadScalarDeliveryPolicyLookup> lookup, bool conformanceProven = true);

		std::shared_ptr<VmReadScalarDeliveryPolicyResolution> getPolicyResolution() const;
		bool isEnabled() const;
		unsigned long long getGeneration() const;

		bool EnableExact();

		void Disable();

		void ReplaceAfterRestore(std::shared_ptr<PrivilegedExecutionStateDescriptor> descriptor, std::shared_ptr<PrivilegedExecutionStateEpoch> currentEpoch, unsigned long long restoreGeneration);

		void ObserveReplayPhase(ReplayPhaseContext const phase);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal VmReadScalarDeliveryResult Prepare(ReplayPhaseContext replayPhase, VmxMicroOp carrier, SafetyVerifier.VirtualizationAdmissionCertificate? e1, ulong fieldSelector, ulong restoreGeneration)
		std::shared_ptr<VmReadScalarDeliveryResult> Prepare(ReplayPhaseContext replayPhase, std::shared_ptr<VmxMicroOp> carrier, std::shared_ptr<VirtualizationAdmissionCertificate> e1, unsigned long long fieldSelector, unsigned long long restoreGeneration);

		bool ValidateLive(std::shared_ptr<VmReadScalarResultReceipt> receipt, std::optional<unsigned long long> currentRestoreGeneration = std::nullopt) override;

		bool ValidateConsumedBinding(std::shared_ptr<VmReadScalarResultReceipt> receipt, unsigned long long currentRestoreGeneration) override;

	private:
		bool ValidateBinding(std::shared_ptr<VmReadScalarResultReceipt> receipt, std::optional<unsigned long long> currentRestoreGeneration);

		void AdvanceGeneration();

		static std::shared_ptr<VmReadScalarDeliveryResult> Deny(VmReadScalarDeliveryDecision decision, const std::wstring &reason);
	};

}
