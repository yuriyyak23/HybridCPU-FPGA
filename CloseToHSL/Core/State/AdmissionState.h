#pragma once

#include "../Pipeline/Core/RuntimeClusterAdmissionPreparation.Preparation.h"
#include "../Pipeline/Core/RuntimeClusterAdmissionPreparation.CandidateView.h"
#include "../Pipeline/Core/RuntimeClusterAdmissionPreparation.DecisionDraft.h"
#include "../Pipeline/Core/RuntimeClusterAdmissionPreparation.Handoff.h"
#include "../Pipeline/Assist/InterCore/YAKSys_Hybrid_CPU.Core.MicroOpScheduler.h"
#include "../Pipeline/MicroOps/Types/MicroOp.h"
#include "../Architecture/Common/YAKSys_Hybrid_CPU.Processor.h"
#include <vector>
#include <memory>

// C# TO C++ CONVERTER NOTE: Forward class declarations:
namespace YAKSys_Hybrid_CPU::Core { class FspSourcePreparation; }

namespace YAKSys_Hybrid_CPU::Core
{

	/// <summary>
	/// Reference-owned containment for the current decode-coupled admission chain.
	/// It does not own scheduler legality, execution or publication authority.
	/// </summary>
	class AdmissionState final : public std::enable_shared_from_this<AdmissionState>
	{
	public:
		AdmissionState();

		RuntimeClusterAdmissionPreparation Preparation;
		RuntimeClusterAdmissionCandidateView CandidateView;
		RuntimeClusterAdmissionDecisionDraft DecisionDraft;
		RuntimeClusterAdmissionHandoff Handoff;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal FspSourcePreparation? SourcePreparation;
		std::shared_ptr<FspSourcePreparation> SourcePreparation;
	};

	// Cleanup provenance for nominations published by the existing CPU producer.
	// Neither this record nor its source stamp grants scheduling/issue authority.
	class FspSourcePreparation final : public std::enable_shared_from_this<FspSourcePreparation>
	{
	private:
		const std::shared_ptr<MicroOpScheduler> Scheduler;
		const unsigned long long BundlePc;
		const unsigned long long SourceStateEpoch;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private readonly MicroOp[] SmtCandidates = new MicroOp?[Processor.CPU_Core.SmtWays];
		std::vector<std::shared_ptr<MicroOp>> SmtCandidates = std::vector<std::shared_ptr<MicroOp>>(Processor::CPU_Core::SmtWays);
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private readonly MicroOp[] AssistCandidates = new MicroOp?[Processor.CPU_Core.SmtWays];
		std::vector<std::shared_ptr<MicroOp>> AssistCandidates = std::vector<std::shared_ptr<MicroOp>>(Processor::CPU_Core::SmtWays);

	public:
		FspSourcePreparation(std::shared_ptr<MicroOpScheduler> scheduler, unsigned long long bundlePc, unsigned long long sourceStateEpoch);

		std::shared_ptr<MicroOpScheduler> getScheduler() const;
		unsigned long long getBundlePc() const;
		unsigned long long getSourceStateEpoch() const;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal MicroOp?[] SmtCandidates {get;} = new MicroOp?[Processor.CPU_Core.SmtWays];
		std::vector<std::shared_ptr<MicroOp>>& getSmtCandidates();
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal MicroOp?[] AssistCandidates {get;} = new MicroOp?[Processor.CPU_Core.SmtWays];
		std::vector<std::shared_ptr<MicroOp>>& getAssistCandidates();
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal MicroOpScheduler.InterCoreAssistNomination? InterCoreNomination;
		std::shared_ptr<MicroOpScheduler::InterCoreAssistNomination> InterCoreNomination;

		void Withdraw();
	};

}
