#pragma once

#include "../../Domains/Descriptors/Observability/YAKSys_Hybrid_CPU.Core.ObservabilityDescriptor.h"
#include "../../Domains/Descriptors/EvidencePolicy/YAKSys_Hybrid_CPU.Core.EvidencePolicyDescriptor.h"
#include "../../Domains/Descriptors/EvidencePolicy/EvidencePolicyDescriptor.h"
#include "DebugTraceExportPolicy.h"
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{
	class DebugTraceExportPolicy final : public std::enable_shared_from_this<DebugTraceExportPolicy>
	{
	private:
		static inline const std::shared_ptr<DebugTraceExportPolicy> FailClosed = std::make_shared<DebugTraceExportPolicy>();
		const bool AllowGuestExport;
		const bool AllowHostLocalExport;
		const bool RedactHostOwnedEvidence;

	public:
		DebugTraceExportPolicy();

		DebugTraceExportPolicy(bool allowGuestExport, bool allowHostLocalExport, bool redactHostOwnedEvidence);

		static std::shared_ptr<DebugTraceExportPolicy> getFailClosed();

		bool getAllowGuestExport() const;

		bool getAllowHostLocalExport() const;

		bool getRedactHostOwnedEvidence() const;

		std::shared_ptr<DebugTraceExportResult> ValidateExport(std::shared_ptr<ObservabilityDescriptor> observability, std::shared_ptr<EvidencePolicyDescriptor> evidencePolicy, EvidenceVisibilityClass evidenceClass, bool guestVisibleExport);

	private:
		std::shared_ptr<DebugTraceExportResult> ValidateGuestExport(std::shared_ptr<ObservabilityDescriptor> observability, std::shared_ptr<EvidencePolicyDescriptor> evidencePolicy, EvidenceVisibilityClass evidenceClass);

		std::shared_ptr<DebugTraceExportResult> ValidateHostLocalExport(std::shared_ptr<ObservabilityDescriptor> observability, EvidenceVisibilityClass evidenceClass);
	};
}

