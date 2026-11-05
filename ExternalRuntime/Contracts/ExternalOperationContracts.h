#pragma once

#include "ExternalOperationIdentity.h"
#include <algorithm>
#include <cstdint>
#include <memory>
#include <optional>
#include <stdexcept>
#include <utility>
#include <vector>

namespace HybridCPU::ExternalRuntime::Contracts
{
	enum class ExternalRuntimeOutcome : unsigned char
	{
		Bound = 0, Succeeded = 1, Closed = 2, Unsupported = 3, Denied = 4,
		NotFound = 5, Stale = 6, Revoked = 7, Faulted = 8, Unknown = 9
	};

	enum class ExternalEffectClass : unsigned char { ReadOnly = 1, Idempotent = 2, NonIdempotent = 3 };
	enum class ExternalVisibilityRequirement : unsigned char { Explicit = 1, Coherent = 2, StagedOutput = 3 };
	enum class ExternalCancellationMode : unsigned char { Unsupported = 1, BestEffort = 2, ExactAcknowledgement = 3 };
	enum class ExternalReplayEffectClass : unsigned char
	{
		StagedReversibleUntilPublish = 1, SnapshotOrIdempotenceRequired = 2, IrreversibleBarrier = 3
	};
	enum class ExternalOperationStage : unsigned char
	{
		Prepared = 1, Admitted = 2, Submitted = 3, DeviceComplete = 4, Visible = 5,
		Published = 6, Released = 7, Failed = 8, Stale = 9
	};
	enum class ExternalOperationProviderPollStatus : unsigned char
	{
		Pending = 1, Receipt = 2, Unavailable = 3, Faulted = 4, Stale = 5
	};

	inline bool IsDefined(ExternalRuntimeOutcome value) noexcept;
	inline bool IsDefined(ExternalOperationStage value) noexcept;
	inline bool IsDefined(ExternalEffectClass value) noexcept;
	inline bool IsDefined(ExternalVisibilityRequirement value) noexcept;
	inline bool IsDefined(ExternalCancellationMode value) noexcept;
	inline bool IsDefined(ExternalReplayEffectClass value) noexcept;
	inline constexpr HybridCpuExternalContractVersion ExternalOperationContractVersion{1, 4, 0};
	inline void ValidateExternalOperationVersion(HybridCpuExternalContractVersion version)
	{
		if (version != ExternalOperationContractVersion)
			throw std::invalid_argument("Unsupported external-operation schema.");
	}

	class ExternalGenerationSet final
	{
	private:
		std::vector<Guid> tokens_;

	public:
		const HybridCpuExternalContractVersion ContractVersion;

		ExternalGenerationSet(HybridCpuExternalContractVersion contractVersion, std::vector<Guid> tokens)
			: tokens_(std::move(tokens)), ContractVersion(contractVersion)
		{
			ValidateExternalOperationVersion(contractVersion);
			if (tokens_.empty())
				throw std::invalid_argument("Snapshot must contain nonempty, unique opaque tokens.");
			std::sort(tokens_.begin(), tokens_.end());
			if (std::any_of(tokens_.begin(), tokens_.end(), [](const Guid& token) { return token == Guid::Empty; }) ||
				std::adjacent_find(tokens_.begin(), tokens_.end()) != tokens_.end())
				throw std::invalid_argument("Snapshot must contain nonempty, unique opaque tokens.");
		}

		bool Equals(const std::shared_ptr<ExternalGenerationSet>& other) const
		{
			return other && ContractVersion == other->ContractVersion && tokens_ == other->tokens_;
		}
		bool Equals(const ExternalGenerationSet& other) const
		{
			return ContractVersion == other.ContractVersion && tokens_ == other.tokens_;
		}
	};

	struct ExternalRequestCorrelation final
	{
		Guid Value;
		bool operator==(const ExternalRequestCorrelation&) const = default;
	};

	class ExternalOperationRequest final
	{
	public:
		const ExternalOperationIdentity Operation;
		const ExternalDomainLease Scope;
		const HybridCpuExternalContractVersion ContractVersion;
		const std::shared_ptr<ExternalGenerationSet> Generations;
		const ExternalRequestCorrelation Correlation;
		const ExternalEffectClass EffectClass;
		const ExternalVisibilityRequirement VisibilityRequirement;
		const ExternalCancellationMode CancellationMode;

		ExternalOperationRequest(ExternalOperationIdentity operation, ExternalDomainLease scope,
			HybridCpuExternalContractVersion contractVersion, std::shared_ptr<ExternalGenerationSet> generations,
			ExternalRequestCorrelation correlation, ExternalEffectClass effectClass,
			ExternalVisibilityRequirement visibilityRequirement, ExternalCancellationMode cancellationMode)
			: Operation(std::move(operation)), Scope(std::move(scope)), ContractVersion(contractVersion),
			  Generations(std::move(generations)), Correlation(std::move(correlation)), EffectClass(effectClass),
			  VisibilityRequirement(visibilityRequirement), CancellationMode(cancellationMode)
		{
			ValidateExternalOperationVersion(contractVersion);
			if (!Generations) throw std::invalid_argument("generations");
			if (Operation.Handle.Value == Guid::Empty || Operation.Generation.Value == 0)
				throw std::invalid_argument("Exact operation attempt required.");
			if (Scope.Handle.Value == Guid::Empty || Scope.Epoch.Value == 0)
				throw std::invalid_argument("Exact opaque lease scope required.");
			if (Correlation.Value == Guid::Empty) throw std::invalid_argument("Correlation required.");
			if (Generations->ContractVersion != contractVersion) throw std::invalid_argument("Snapshot version mismatch.");
			if (!IsDefined(effectClass) || !IsDefined(visibilityRequirement) || !IsDefined(cancellationMode))
				throw std::invalid_argument("Undefined external operation classification.");
		}
		ExternalOperationStage getStage() const noexcept { return ExternalOperationStage::Prepared; }
		bool Equals(const ExternalOperationRequest& other) const noexcept
		{
			return Operation == other.Operation && Scope == other.Scope && ContractVersion == other.ContractVersion &&
				Generations->Equals(*other.Generations) && Correlation == other.Correlation &&
				EffectClass == other.EffectClass && VisibilityRequirement == other.VisibilityRequirement &&
				CancellationMode == other.CancellationMode;
		}
		bool operator==(const ExternalOperationRequest& other) const noexcept { return Equals(other); }
	};

	class ExternalOperationReceipt
	{
	public:
		const std::shared_ptr<ExternalOperationRequest> Request;
		const ExternalOperationStage Stage;
		const ExternalRuntimeOutcome Outcome;
		virtual ~ExternalOperationReceipt() = default;

	protected:
		ExternalOperationReceipt(std::shared_ptr<ExternalOperationRequest> request, ExternalOperationStage stage,
			ExternalRuntimeOutcome outcome)
			: Request(std::move(request)), Stage(stage), Outcome(outcome)
		{
			if (!Request) throw std::invalid_argument("request");
			if (!IsDefined(stage)) throw std::invalid_argument("Undefined external operation stage.");
			if (!IsDefined(outcome)) throw std::invalid_argument("Undefined external runtime outcome.");
		}
	};

	class ExternalOperationAdmissionReceipt final : public ExternalOperationReceipt
	{
	public:
		ExternalOperationAdmissionReceipt(std::shared_ptr<ExternalOperationRequest> request, ExternalRuntimeOutcome outcome)
			: ExternalOperationReceipt(std::move(request), ExternalOperationStage::Admitted, outcome) {}
	};

	class ExternalOperationProgressReceipt final : public ExternalOperationReceipt
	{
	public:
		ExternalOperationProgressReceipt(std::shared_ptr<ExternalOperationRequest> request, ExternalOperationStage stage,
			ExternalRuntimeOutcome outcome)
			: ExternalOperationReceipt(std::move(request), stage, outcome)
		{
			if (stage != ExternalOperationStage::Submitted && stage != ExternalOperationStage::Visible)
				throw std::invalid_argument("Progress receipt stage must be Submitted or Visible.");
		}
	};

	class ExternalOperationCompletionReceipt final : public ExternalOperationReceipt
	{
	public:
		ExternalOperationCompletionReceipt(std::shared_ptr<ExternalOperationRequest> request, ExternalRuntimeOutcome outcome)
			: ExternalOperationReceipt(std::move(request), ExternalOperationStage::DeviceComplete, outcome) {}
	};

	class ExternalOperationPublicationEvidence final
	{
	public:
		const std::shared_ptr<ExternalOperationRequest> Request;
		const std::shared_ptr<ExternalGenerationSet> CurrentGenerations;
		const std::shared_ptr<ExternalOperationCompletionReceipt> Completion;
		const std::shared_ptr<ExternalOperationProgressReceipt> Visibility;

		ExternalOperationPublicationEvidence(std::shared_ptr<ExternalOperationRequest> request,
			std::shared_ptr<ExternalGenerationSet> currentGenerations,
			std::shared_ptr<ExternalOperationCompletionReceipt> completion,
			std::shared_ptr<ExternalOperationProgressReceipt> visibility)
			: Request(std::move(request)), CurrentGenerations(std::move(currentGenerations)),
			  Completion(std::move(completion)), Visibility(std::move(visibility))
		{
			if (!Request || !CurrentGenerations || !Completion || !Visibility)
				throw std::invalid_argument("Publication evidence requires all receipt and generation fields.");
			if (Visibility->Stage != ExternalOperationStage::Visible)
				throw std::invalid_argument("Visibility receipt required.");
		}
	};

	class ExternalOperationProviderPollResult final
	{
	public:
		const ExternalOperationProviderPollStatus Status;
		const std::shared_ptr<ExternalGenerationSet> CurrentGenerations;
		const std::shared_ptr<ExternalOperationReceipt> Receipt;

		ExternalOperationProviderPollResult(ExternalOperationProviderPollStatus status,
			std::shared_ptr<ExternalGenerationSet> currentGenerations,
			std::shared_ptr<ExternalOperationReceipt> receipt = nullptr);
	};

	inline ExternalOperationProviderPollResult::ExternalOperationProviderPollResult(
		ExternalOperationProviderPollStatus status, std::shared_ptr<ExternalGenerationSet> currentGenerations,
		std::shared_ptr<ExternalOperationReceipt> receipt)
		: Status(status), CurrentGenerations(std::move(currentGenerations)), Receipt(std::move(receipt))
	{
		if (!CurrentGenerations) throw std::invalid_argument("currentGenerations");
		if (Status != ExternalOperationProviderPollStatus::Pending && Status != ExternalOperationProviderPollStatus::Receipt &&
			Status != ExternalOperationProviderPollStatus::Unavailable && Status != ExternalOperationProviderPollStatus::Faulted &&
			Status != ExternalOperationProviderPollStatus::Stale)
			throw std::invalid_argument("Undefined external provider poll status.");
		if ((Status == ExternalOperationProviderPollStatus::Pending) != (Receipt == nullptr))
			throw std::invalid_argument("Only Pending observations omit a receipt.");
		if (Receipt && Receipt->Request && Receipt->Request->ContractVersion != CurrentGenerations->ContractVersion)
			throw std::invalid_argument("Receipt and current generations have incompatible versions.");
	}

	class ExternalOperationSemanticRequest final
	{
	public:
		const HybridCpuExternalContractVersion ContractVersion;
		const ExternalRequestCorrelation Correlation;
		const ExternalEffectClass EffectClass;
		const ExternalVisibilityRequirement VisibilityRequirement;
		const ExternalCancellationMode CancellationMode;
		const ExternalReplayEffectClass ReplayEffectClass;

		ExternalOperationSemanticRequest(HybridCpuExternalContractVersion contractVersion,
			ExternalRequestCorrelation correlation, ExternalEffectClass effectClass,
			ExternalVisibilityRequirement visibilityRequirement, ExternalCancellationMode cancellationMode,
			ExternalReplayEffectClass replayEffectClass)
			: ContractVersion(contractVersion), Correlation(std::move(correlation)), EffectClass(effectClass),
			  VisibilityRequirement(visibilityRequirement), CancellationMode(cancellationMode), ReplayEffectClass(replayEffectClass)
		{
			ValidateExternalOperationVersion(contractVersion);
			if (Correlation.Value == Guid::Empty) throw std::invalid_argument("Correlation required.");
			if (!IsDefined(effectClass) || !IsDefined(visibilityRequirement) || !IsDefined(cancellationMode) || !IsDefined(replayEffectClass))
				throw std::invalid_argument("Undefined external operation classification.");
		}
	};

	class IExternalOperationProvider
	{
	public:
		virtual ~IExternalOperationProvider() = default;
		virtual std::shared_ptr<ExternalOperationProviderPollResult> Admit(std::shared_ptr<ExternalOperationSemanticRequest> request) = 0;
		virtual std::shared_ptr<ExternalOperationProviderPollResult> Submit(std::shared_ptr<ExternalOperationRequest> request) = 0;
		virtual std::shared_ptr<ExternalOperationProviderPollResult> Poll(std::shared_ptr<ExternalOperationRequest> request) = 0;
		virtual std::shared_ptr<ExternalOperationProviderPollResult> Cancel(std::shared_ptr<ExternalOperationRequest> request) = 0;
	};

	enum class ExternalCpuGuardStatus : unsigned char { Allowed = 1, Rejected = 2, Stale = 3 };
	enum class ExternalOperationBindingRejectKind : unsigned char
	{
		None = 0, CpuGuardMissing = 1, CpuGuardRejected = 2, CpuGuardStale = 3,
		CpuGuardCorrelationMismatch = 4, AdmissionMissing = 5, AdmissionRejected = 6,
		AdmissionStale = 7, AdmissionCorrelationMismatch = 8, GenerationMismatch = 9
	};
	enum class ExternalOperationBindingStatus : unsigned char { SubmitEligible = 1, Rejected = 2, Stale = 3 };

	class ExternalOperationCpuGuardReceipt final
	{
	public:
		const std::shared_ptr<ExternalOperationRequest> Request;
		const ExternalCpuGuardStatus Status;
		ExternalOperationCpuGuardReceipt(std::shared_ptr<ExternalOperationRequest> request, ExternalCpuGuardStatus status)
			: Request(std::move(request)), Status(status)
		{
			if (!Request) throw std::invalid_argument("request");
			if (status != ExternalCpuGuardStatus::Allowed && status != ExternalCpuGuardStatus::Rejected && status != ExternalCpuGuardStatus::Stale)
				throw std::invalid_argument("Undefined CPU guard status.");
		}
	};

	class ExternalOperationBindingDecision final
	{
	public:
		const ExternalOperationBindingStatus Status;
		const ExternalOperationBindingRejectKind RejectKind;
		const bool IsSubmitEligible;
		bool getIsSubmitEligible() const noexcept { return Status == ExternalOperationBindingStatus::SubmitEligible; }
		static std::shared_ptr<ExternalOperationBindingDecision> Eligible()
		{
			return std::shared_ptr<ExternalOperationBindingDecision>(new ExternalOperationBindingDecision(ExternalOperationBindingStatus::SubmitEligible, ExternalOperationBindingRejectKind::None));
		}
		static std::shared_ptr<ExternalOperationBindingDecision> Reject(ExternalOperationBindingRejectKind kind)
		{
			return std::shared_ptr<ExternalOperationBindingDecision>(new ExternalOperationBindingDecision(ExternalOperationBindingStatus::Rejected, kind));
		}
		static std::shared_ptr<ExternalOperationBindingDecision> Stale(ExternalOperationBindingRejectKind kind)
		{
			return std::shared_ptr<ExternalOperationBindingDecision>(new ExternalOperationBindingDecision(ExternalOperationBindingStatus::Stale, kind));
		}
	private:
		ExternalOperationBindingDecision(ExternalOperationBindingStatus status, ExternalOperationBindingRejectKind kind)
			: Status(status), RejectKind(kind), IsSubmitEligible(status == ExternalOperationBindingStatus::SubmitEligible) {}
	};

	class ExternalOperationAdmissionBinding final
	{
	public:
		static std::shared_ptr<ExternalOperationBindingDecision> Evaluate(
			std::shared_ptr<ExternalOperationRequest> request,
			std::shared_ptr<ExternalGenerationSet> currentGenerations,
			std::shared_ptr<ExternalOperationCpuGuardReceipt> cpuGuard,
			std::shared_ptr<ExternalOperationAdmissionReceipt> admission)
		{
			if (!request) throw std::invalid_argument("request");
			if (!request->Generations->Equals(currentGenerations))
				return ExternalOperationBindingDecision::Stale(ExternalOperationBindingRejectKind::GenerationMismatch);
			if (!cpuGuard) return ExternalOperationBindingDecision::Reject(ExternalOperationBindingRejectKind::CpuGuardMissing);
			if (!cpuGuard->Request || *cpuGuard->Request != *request)
				return ExternalOperationBindingDecision::Stale(ExternalOperationBindingRejectKind::CpuGuardCorrelationMismatch);
			if (cpuGuard->Status == ExternalCpuGuardStatus::Stale)
				return ExternalOperationBindingDecision::Stale(ExternalOperationBindingRejectKind::CpuGuardStale);
			if (cpuGuard->Status != ExternalCpuGuardStatus::Allowed)
				return ExternalOperationBindingDecision::Reject(ExternalOperationBindingRejectKind::CpuGuardRejected);
			if (!admission) return ExternalOperationBindingDecision::Reject(ExternalOperationBindingRejectKind::AdmissionMissing);
			if (!admission->Request || *admission->Request != *request)
				return ExternalOperationBindingDecision::Stale(ExternalOperationBindingRejectKind::AdmissionCorrelationMismatch);
			if (admission->Outcome == ExternalRuntimeOutcome::Stale)
				return ExternalOperationBindingDecision::Stale(ExternalOperationBindingRejectKind::AdmissionStale);
			return admission->Outcome == ExternalRuntimeOutcome::Succeeded
				? ExternalOperationBindingDecision::Eligible()
				: ExternalOperationBindingDecision::Reject(ExternalOperationBindingRejectKind::AdmissionRejected);
		}
	};

	class ExternalOperationContract final
	{
	public:
		static inline const HybridCpuExternalContractVersion Version = ExternalOperationContractVersion;
		static void ValidateVersion(HybridCpuExternalContractVersion version)
		{
			ValidateExternalOperationVersion(version);
		}
		static ExternalOperationStage Revalidate(ExternalOperationStage current,
			const std::shared_ptr<ExternalOperationRequest>& request,
			const std::shared_ptr<ExternalGenerationSet>& currentGenerations)
		{
			if (!request) throw std::invalid_argument("request");
			if (!IsDefined(current)) return ExternalOperationStage::Failed;
			if (current == ExternalOperationStage::Failed || current == ExternalOperationStage::Stale || current == ExternalOperationStage::Released)
				return current;
			return request->Generations->Equals(currentGenerations) ? current : ExternalOperationStage::Stale;
		}
		static ExternalOperationStage Accept(ExternalOperationStage current,
			const std::shared_ptr<ExternalOperationRequest>& request,
			const std::shared_ptr<ExternalGenerationSet>& currentGenerations,
			const std::shared_ptr<ExternalOperationReceipt>& receipt)
		{
			const ExternalOperationStage checkedStage = Revalidate(current, request, currentGenerations);
			if (current == ExternalOperationStage::Released) return ExternalOperationStage::Failed;
			if (checkedStage != current || current == ExternalOperationStage::Failed || current == ExternalOperationStage::Stale)
				return checkedStage;
			if (!receipt) return ExternalOperationStage::Failed;
			if (!receipt->Request || *receipt->Request != *request) return ExternalOperationStage::Stale;
			if (receipt->Outcome == ExternalRuntimeOutcome::Stale) return ExternalOperationStage::Stale;
			const ExternalRuntimeOutcome expected = receipt->Stage == ExternalOperationStage::Released
				? ExternalRuntimeOutcome::Closed : ExternalRuntimeOutcome::Succeeded;
			if (receipt->Outcome != expected || static_cast<unsigned char>(receipt->Stage) != static_cast<unsigned char>(current) + 1)
				return ExternalOperationStage::Failed;
			return receipt->Stage;
		}
	};

	enum class ExternalOperationPublicationEligibility : unsigned char { Eligible = 1, Rejected = 2, Stale = 3 };
	class ExternalOperationPublicationGate final
	{
	public:
		static ExternalOperationPublicationEligibility Evaluate(
			const std::optional<std::shared_ptr<ExternalOperationPublicationEvidence>>& evidence)
		{
			if (!evidence || !*evidence) return ExternalOperationPublicationEligibility::Rejected;
			const auto& value = *evidence;
			if (!value->Request->Generations->Equals(value->CurrentGenerations))
				return ExternalOperationPublicationEligibility::Stale;
			if (ExternalOperationContract::Accept(ExternalOperationStage::Submitted, value->Request,
				value->CurrentGenerations, value->Completion) != ExternalOperationStage::DeviceComplete)
				return ExternalOperationPublicationEligibility::Rejected;
			if (ExternalOperationContract::Accept(ExternalOperationStage::DeviceComplete, value->Request,
				value->CurrentGenerations, value->Visibility) != ExternalOperationStage::Visible)
				return ExternalOperationPublicationEligibility::Rejected;
			return value->Request->VisibilityRequirement == ExternalVisibilityRequirement::StagedOutput &&
				value->Request->EffectClass != ExternalEffectClass::ReadOnly
				? ExternalOperationPublicationEligibility::Eligible : ExternalOperationPublicationEligibility::Rejected;
		}
	};

	inline bool IsDefined(ExternalRuntimeOutcome value) noexcept
	{
		return static_cast<unsigned char>(value) <= static_cast<unsigned char>(ExternalRuntimeOutcome::Unknown);
	}
	inline bool IsDefined(ExternalOperationStage value) noexcept
	{
		return static_cast<unsigned char>(value) >= static_cast<unsigned char>(ExternalOperationStage::Prepared) &&
			static_cast<unsigned char>(value) <= static_cast<unsigned char>(ExternalOperationStage::Stale);
	}
	inline bool IsDefined(ExternalEffectClass value) noexcept
	{
		return value == ExternalEffectClass::ReadOnly || value == ExternalEffectClass::Idempotent || value == ExternalEffectClass::NonIdempotent;
	}
	inline bool IsDefined(ExternalVisibilityRequirement value) noexcept
	{
		return value == ExternalVisibilityRequirement::Explicit || value == ExternalVisibilityRequirement::Coherent || value == ExternalVisibilityRequirement::StagedOutput;
	}
	inline bool IsDefined(ExternalCancellationMode value) noexcept
	{
		return value == ExternalCancellationMode::Unsupported || value == ExternalCancellationMode::BestEffort || value == ExternalCancellationMode::ExactAcknowledgement;
	}
	inline bool IsDefined(ExternalReplayEffectClass value) noexcept
	{
		return value == ExternalReplayEffectClass::StagedReversibleUntilPublish || value == ExternalReplayEffectClass::SnapshotOrIdempotenceRequired || value == ExternalReplayEffectClass::IrreversibleBarrier;
	}
}
