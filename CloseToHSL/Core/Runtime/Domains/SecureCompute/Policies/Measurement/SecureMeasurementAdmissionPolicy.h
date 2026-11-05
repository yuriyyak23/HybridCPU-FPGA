#pragma once

#include <memory>
#include <string>
#include <utility>

namespace YAKSys_Hybrid_CPU::Core
{
	enum class SecureMeasurementAdmissionDecision : unsigned char
	{
		AllowedMeasurement = 0,
		DeniedMeasurementNotRequired = 1,
		DeniedMissingMeasurement = 2,
		DeniedPendingMeasurement = 3,
		DeniedStaleMeasurement = 4,
		DeniedRevokedMeasurement = 5,
		DeniedUnmaterializedMeasurement = 6,
		DeniedPolicyDigestMismatch = 7,
		DeniedMemoryDigestMismatch = 8,
		DeniedDebugClassMismatch = 9,
		DeniedEvidenceVisibility = 10,
		DeniedPublicationFence = 11
	};

	struct SecureMeasurementAdmissionResult final {
		const SecureMeasurementAdmissionDecision Decision;
		const std::wstring Reason;
		const bool IsAllowed;

		SecureMeasurementAdmissionResult(SecureMeasurementAdmissionDecision decision, std::wstring reason)
			: Decision(decision), Reason(std::move(reason)), IsAllowed(decision == SecureMeasurementAdmissionDecision::AllowedMeasurement) {}

		static const std::shared_ptr<SecureMeasurementAdmissionResult> AllowedMeasurement;
		static std::shared_ptr<SecureMeasurementAdmissionResult> Denied(SecureMeasurementAdmissionDecision decision, std::wstring reason) {
			return std::make_shared<SecureMeasurementAdmissionResult>(decision, std::move(reason));
		}
	};

	inline const std::shared_ptr<SecureMeasurementAdmissionResult> SecureMeasurementAdmissionResult::AllowedMeasurement = std::make_shared<SecureMeasurementAdmissionResult>(SecureMeasurementAdmissionDecision::AllowedMeasurement, L"");
}
