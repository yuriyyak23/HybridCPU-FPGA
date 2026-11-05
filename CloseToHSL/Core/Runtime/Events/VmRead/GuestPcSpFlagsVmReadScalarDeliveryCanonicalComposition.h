#pragma once
#include "../../Governance/Virtualization/GuestPcSpFlagsVmReadScalarDeliveryAcceptedPolicyResolver.h"

#include "VmReadScalarDeliveryCanonicalComposition.h"
#include "../../../Virtualization/Compatibility/Frontend/Handlers/YAKSys_Hybrid_CPU.Core.VmxCompatibilityAdmissionService.h"
#include "../../Domains/Admission/Execution/YAKSys_Hybrid_CPU.Core.ExecutionDomainRuntime.h"
#include "../../Domains/Authority/YAKSys_Hybrid_CPU.Core.RootAuthorityDescriptor.h"
#include "../../Domains/Descriptors/EvidencePolicy/YAKSys_Hybrid_CPU.Core.EvidencePolicyDescriptor.h"
#include "../../Domains/Services/YAKSys_Hybrid_CPU.Core.DomainRuntimeContext.h"
#include "../../../Pipeline/Certificates/ReplayPhaseSubstrate.h"
namespace YAKSys_Hybrid_CPU::Core { class VmxMicroOp; }
#include <string>
#include <limits>
#include <stdexcept>
#include <optional>
#include <mutex>
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{

	class GuestPcSpFlagsVmReadScalarDeliveryCanonicalComposition final : public IVmReadScalarResultReceiptOwner
	{
	private:
		const std::shared_ptr<GuestPcSpFlagsVmReadScalarDeliveryPolicyResolution> PolicyResolution;

		mutable std::mutex _sync;
		const std::shared_ptr<VmxCompatibilityAdmissionService> _admission = std::make_shared<VmxCompatibilityAdmissionService>();
		const std::shared_ptr<ExecutionDomainRuntime> _executionRuntime;
		const std::shared_ptr<RootAuthorityDescriptor> _root;
		const std::shared_ptr<EvidencePolicyDescriptor> _evidence;
		std::shared_ptr<DomainRuntimeContext> _context;
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
// ORIGINAL LINE: internal GuestPcSpFlagsVmReadScalarDeliveryCanonicalComposition(ExecutionDomainRuntime executionRuntime, DomainRuntimeContext context, RootAuthorityDescriptor root, EvidencePolicyDescriptor evidence, ulong restoreGeneration, in GuestPcSpFlagsVmReadScalarDeliveryPolicyLookup lookup)
		GuestPcSpFlagsVmReadScalarDeliveryCanonicalComposition(std::shared_ptr<ExecutionDomainRuntime> executionRuntime, std::shared_ptr<DomainRuntimeContext> context, std::shared_ptr<RootAuthorityDescriptor> root, std::shared_ptr<EvidencePolicyDescriptor> evidence, unsigned long long restoreGeneration, std::shared_ptr<GuestPcSpFlagsVmReadScalarDeliveryPolicyLookup> lookup);

		std::shared_ptr<GuestPcSpFlagsVmReadScalarDeliveryPolicyResolution> getPolicyResolution() const;
		bool isEnabled() const;
		unsigned long long getGeneration() const;

		bool EnableExact();

		void Disable();

		/// <summary>
		/// Publishes a replacement immutable context for future captures. This is not a
		/// profile-generation event: a receipt already issued from an atomic capture keeps
		/// its scalar value and does not re-read the replacement source at retire.
		/// </summary>
		bool RefreshSourceContext(std::shared_ptr<DomainRuntimeContext> context);

		void ReplaceAfterRestore(std::shared_ptr<DomainRuntimeContext> context, unsigned long long restoreGeneration);

		void ObserveReplayPhase(ReplayPhaseContext const phase);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal VmReadScalarDeliveryResult Prepare(ReplayPhaseContext replayPhase, VmxMicroOp carrier, VmReadScalarAttemptBinding? attempt, ulong fieldSelector, ulong restoreGeneration)
		std::shared_ptr<VmReadScalarDeliveryResult> Prepare(ReplayPhaseContext replayPhase, std::shared_ptr<VmxMicroOp> carrier, std::shared_ptr<VmReadScalarAttemptBinding> attempt, unsigned long long fieldSelector, unsigned long long restoreGeneration);

		bool ValidateLive(std::shared_ptr<VmReadScalarResultReceipt> receipt, std::optional<unsigned long long> currentRestoreGeneration = std::nullopt) override;

		bool ValidateConsumedBinding(std::shared_ptr<VmReadScalarResultReceipt> receipt, unsigned long long currentRestoreGeneration) override;

	private:
		bool ValidateBinding(std::shared_ptr<VmReadScalarResultReceipt> receipt, std::optional<unsigned long long> currentRestoreGeneration);

		bool HasCurrentSourceBinding(std::shared_ptr<DomainRuntimeContext> context);

		void AdvanceGeneration();

		static std::shared_ptr<VmReadScalarDeliveryResult> Deny(VmReadScalarDeliveryDecision decision, const std::wstring &reason);
	};

}
