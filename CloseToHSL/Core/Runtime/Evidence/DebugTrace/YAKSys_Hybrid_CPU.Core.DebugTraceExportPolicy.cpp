#include "YAKSys_Hybrid_CPU.Core.DebugTraceExportPolicy.h"
#include "DebugTraceExportPolicy.h"

namespace YAKSys_Hybrid_CPU::Core
{

	DebugTraceExportPolicy::DebugTraceExportPolicy() : DebugTraceExportPolicy(false, false, true)
	{
	}

	DebugTraceExportPolicy::DebugTraceExportPolicy(bool allowGuestExport, bool allowHostLocalExport, bool redactHostOwnedEvidence)
		: AllowGuestExport(allowGuestExport),
		  AllowHostLocalExport(allowHostLocalExport),
		  RedactHostOwnedEvidence(redactHostOwnedEvidence)
	{
	}

	std::shared_ptr<DebugTraceExportPolicy> DebugTraceExportPolicy::getFailClosed()
	{
		return FailClosed;
	}

	bool DebugTraceExportPolicy::getAllowGuestExport() const
	{
		return AllowGuestExport;
	}

	bool DebugTraceExportPolicy::getAllowHostLocalExport() const
	{
		return AllowHostLocalExport;
	}

	bool DebugTraceExportPolicy::getRedactHostOwnedEvidence() const
	{
		return RedactHostOwnedEvidence;
	}

	std::shared_ptr<DebugTraceExportResult> DebugTraceExportPolicy::ValidateExport(std::shared_ptr<ObservabilityDescriptor> observability, std::shared_ptr<EvidencePolicyDescriptor> evidencePolicy, EvidenceVisibilityClass evidenceClass, bool guestVisibleExport)
	{
		return guestVisibleExport ? ValidateGuestExport(observability, evidencePolicy, evidenceClass) : ValidateHostLocalExport(observability, evidenceClass);
	}

	std::shared_ptr<DebugTraceExportResult> DebugTraceExportPolicy::ValidateGuestExport(std::shared_ptr<ObservabilityDescriptor> observability, std::shared_ptr<EvidencePolicyDescriptor> evidencePolicy, EvidenceVisibilityClass evidenceClass)
	{
		if (!getAllowGuestExport())
		{
			return DebugTraceExportResult::Denied(DebugTraceExportDecision::GuestExportDenied, L"Debug trace policy does not permit guest-visible export.");
		}

		if (getRedactHostOwnedEvidence() && observability->MustRedactForGuest(evidenceClass))
		{
			return DebugTraceExportResult::Denied(DebugTraceExportDecision::RedactionRequired, L"Debug trace evidence must be redacted before guest-visible export.");
		}

		if (!evidencePolicy->CanExposeToGuest(evidenceClass))
		{
			return DebugTraceExportResult::Denied(DebugTraceExportDecision::EvidencePolicyDenied, L"Evidence policy does not permit guest-visible export for this evidence class.");
		}

		if (!observability->CanPublishToGuest(evidenceClass))
		{
			return DebugTraceExportResult::Denied(DebugTraceExportDecision::GuestExportDenied, L"Observability policy does not permit guest-visible export for this evidence class.");
		}

		return DebugTraceExportResult::Allowed;
	}

	std::shared_ptr<DebugTraceExportResult> DebugTraceExportPolicy::ValidateHostLocalExport(std::shared_ptr<ObservabilityDescriptor> observability, EvidenceVisibilityClass evidenceClass)
	{
		if (!getAllowHostLocalExport())
		{
			return DebugTraceExportResult::Denied(DebugTraceExportDecision::HostLocalExportDenied, L"Debug trace policy does not permit host-local export.");
		}

		if (!observability->CanCaptureHostLocal(evidenceClass))
		{
			return DebugTraceExportResult::Denied(DebugTraceExportDecision::HostLocalExportDenied, L"Observability policy does not permit host-local export for this evidence class.");
		}

		return DebugTraceExportResult::Allowed;
	}
}

