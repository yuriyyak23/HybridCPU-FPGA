#include "AdmissionState.h"
#include <utility>

namespace YAKSys_Hybrid_CPU::Core
{
	AdmissionState::AdmissionState()
		: Preparation(RuntimeClusterAdmissionPreparation::CreateEmpty()),
		  CandidateView{},
		  DecisionDraft(RuntimeClusterAdmissionDecisionDraft::CreateEmpty()),
		  Handoff()
	{
	}

	FspSourcePreparation::FspSourcePreparation(std::shared_ptr<MicroOpScheduler> scheduler, unsigned long long bundlePc, unsigned long long sourceStateEpoch)
		: Scheduler(std::move(scheduler)), BundlePc(bundlePc), SourceStateEpoch(sourceStateEpoch)
	{
	}

	std::shared_ptr<MicroOpScheduler> FspSourcePreparation::getScheduler() const
	{
		return Scheduler;
	}

	unsigned long long FspSourcePreparation::getBundlePc() const
	{
		return BundlePc;
	}

	unsigned long long FspSourcePreparation::getSourceStateEpoch() const
	{
		return SourceStateEpoch;
	}

	std::vector<std::shared_ptr<MicroOp>>& FspSourcePreparation::getSmtCandidates()
	{
		return SmtCandidates;
	}

	std::vector<std::shared_ptr<MicroOp>>& FspSourcePreparation::getAssistCandidates()
	{
		return AssistCandidates;
	}

	void FspSourcePreparation::Withdraw()
	{
		for (int vt = 0; vt < getSmtCandidates().size(); vt++)
		{
			std::shared_ptr<MicroOp> candidate = std::dynamic_pointer_cast<MicroOp>(getSmtCandidates()[vt]);
			if (candidate != nullptr)
			{
				getScheduler()->WithdrawSmtCandidate(vt, candidate);
			}
			std::shared_ptr<MicroOp> assist = std::dynamic_pointer_cast<MicroOp>(getAssistCandidates()[vt]);
			if (assist != nullptr)
			{
				getScheduler()->WithdrawAssistCandidate(vt, assist);
			}
		}
		if (InterCoreNomination != nullptr)
		{
			InterCoreNomination->Withdraw();
		}
	}
}
