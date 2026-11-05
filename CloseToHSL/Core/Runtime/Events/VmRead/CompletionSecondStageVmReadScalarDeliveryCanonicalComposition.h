#pragma once

#include "VmReadScalarDeliveryCanonicalComposition.h"
#include "../../Completion/Commit/ArchitecturalCompletionCommitOwner.h"
#include "../../../Virtualization/Compatibility/Frontend/Projection/Completion/NeutralCompletionSecondStageProjection.h"
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

	class CompletionSecondStageVmReadScalarDeliveryCanonicalComposition final : public IVmReadScalarResultReceiptOwner
	{
	private:
		const bool PolicyAccepted;

		std::mutex _sync;
		const std::shared_ptr<ArchitecturalCompletionCommitOwner> _completionOwner;
		const std::shared_ptr<NeutralCompletionSecondStageProjectionOwner> _projection;
		const bool _enabled;
		unsigned long long _generation = 1;

	public:
// C# TO C++ CONVERTER TASK: Throw expressions are not converted by C# to C++ Converter:
// ORIGINAL LINE: internal CompletionSecondStageVmReadScalarDeliveryCanonicalComposition(ArchitecturalCompletionCommitOwner completionOwner, ArchitecturalCompletionCommitOwner.ProducerRegistration exactProducer, bool enabled)
		CompletionSecondStageVmReadScalarDeliveryCanonicalComposition(std::shared_ptr<ArchitecturalCompletionCommitOwner> completionOwner, std::shared_ptr<ArchitecturalCompletionCommitOwner::ProducerRegistration> exactProducer, bool enabled);

		bool getPolicyAccepted() const;
		bool isEnabled() const;

		std::shared_ptr<VmReadScalarDeliveryResult> Prepare(ReplayPhaseContext replayPhase, std::shared_ptr<VmxMicroOp> carrier, std::shared_ptr<VmReadScalarAttemptBinding> attempt, unsigned long long fieldSelector, unsigned long long restoreGeneration);

		bool ValidateLive(std::shared_ptr<VmReadScalarResultReceipt> receipt, std::optional<unsigned long long> currentRestoreGeneration = std::nullopt) override;

		bool ValidateConsumedBinding(std::shared_ptr<VmReadScalarResultReceipt> receipt, unsigned long long currentRestoreGeneration) override;

	private:
		bool Validate(std::shared_ptr<VmReadScalarResultReceipt> receipt, std::optional<unsigned long long> currentRestoreGeneration);

		static std::shared_ptr<VmReadScalarDeliveryResult> Deny(VmReadScalarDeliveryDecision decision, const std::wstring &reason);
	};

}
