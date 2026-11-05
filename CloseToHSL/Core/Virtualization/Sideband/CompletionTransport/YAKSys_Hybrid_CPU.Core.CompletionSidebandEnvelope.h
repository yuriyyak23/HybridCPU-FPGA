#pragma once

#include "../../../Runtime/Completion/Records/YAKSys_Hybrid_CPU.Core.CompletionRecord.h"
#include "../../../Runtime/Domains/Descriptors/EvidencePolicy/EvidencePolicyDescriptor.h"
#include "../../../Runtime/Domains/Descriptors/EvidencePolicy/YAKSys_Hybrid_CPU.Core.EvidencePolicyDescriptor.h"
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{
	class CompletionSidebandEnvelope final : public std::enable_shared_from_this<CompletionSidebandEnvelope>
	{
	private:
		static inline const std::shared_ptr<CompletionSidebandEnvelope> Empty = std::make_shared<CompletionSidebandEnvelope>();
		const std::shared_ptr<CompletionRecord> Record;
		const unsigned long long RouteId;
		const unsigned long long Sequence;
		const bool IsHostOwnedEvidence;
		const EvidenceVisibilityClass VisibilityClass;

	public:
		CompletionSidebandEnvelope();

		CompletionSidebandEnvelope(std::shared_ptr<CompletionRecord> record, unsigned long long routeId, unsigned long long sequence, bool isHostOwnedEvidence, EvidenceVisibilityClass visibilityClass);

		static std::shared_ptr<CompletionSidebandEnvelope> getEmpty();

		std::shared_ptr<CompletionRecord> getRecord() const;

		unsigned long long getRouteId() const;

		unsigned long long getSequence() const;

		bool isHostOwnedEvidence() const;

		EvidenceVisibilityClass getVisibilityClass() const;

		bool getHasPayload() const;

		bool getRequiresHostHandling() const;

		bool CanExposeToGuest(std::shared_ptr<EvidencePolicyDescriptor> evidencePolicy);
	};
}
