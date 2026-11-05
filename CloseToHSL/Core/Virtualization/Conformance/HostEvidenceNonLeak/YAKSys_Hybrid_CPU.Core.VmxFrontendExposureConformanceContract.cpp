#include "YAKSys_Hybrid_CPU.Core.VmxFrontendExposureConformanceContract.h"
#include "../../../Runtime/Migration/Format/MigrationDescriptor.h"

namespace YAKSys_Hybrid_CPU::Core
{

	VmxFrontendExposureViolation VmxFrontendExposureConformanceContract::Evaluate(std::shared_ptr<VmxFrontendExposureRequest> request)
	{
		if (!request->IsGeneratedProjection)
		{
			return VmxFrontendExposureViolation::NonGeneratedProjection;
		}

		if (!request->HasAccessPolicy)
		{
			return VmxFrontendExposureViolation::MissingAccessPolicy;
		}

		if (!request->IsGuestVisible)
		{
			return VmxFrontendExposureViolation::None;
		}

		if (IsHostOwnedEvidence(request->EvidenceClass) || request->PayloadClass == MigrationPayloadClass::HostOwnedRuntimeEvidence)
		{
			return VmxFrontendExposureViolation::HostEvidenceVisible;
		}

		if (request->ExposesNativeToken || request->EvidenceClass == EvidenceVisibilityClass::NativeTokenEvidence || request->PayloadClass == MigrationPayloadClass::NativeTokenEvidence)
		{
			return VmxFrontendExposureViolation::NativeTokenVisible;
		}

		if (request->ExposesBackendHandle || request->EvidenceClass == EvidenceVisibilityClass::BackendBindingEvidence || request->PayloadClass == MigrationPayloadClass::BackendBindingEvidence)
		{
			return VmxFrontendExposureViolation::BackendBindingVisible;
		}

		if (request->ExposesSchedulerEvidence || request->EvidenceClass == EvidenceVisibilityClass::SchedulerEvidence || request->PayloadClass == MigrationPayloadClass::SchedulerEvidence)
		{
			return VmxFrontendExposureViolation::SchedulerEvidenceVisible;
		}

		if (request->ExposesMigrationOnlyState || request->PayloadClass == MigrationPayloadClass::CompatibilityProjectionMetadata)
		{
			return VmxFrontendExposureViolation::MigrationOnlyStateVisible;
		}

		return request->ExposesLanePrivateState ? VmxFrontendExposureViolation::LanePrivateStateVisible : VmxFrontendExposureViolation::None;
	}

	bool VmxFrontendExposureConformanceContract::IsSatisfied(std::shared_ptr<VmxFrontendExposureRequest> request)
	{
		return Evaluate(request) == VmxFrontendExposureViolation::None;
	}

	bool VmxFrontendExposureConformanceContract::IsHostOwnedEvidence(EvidenceVisibilityClass evidenceClass)
	{
		return (evidenceClass == EvidenceVisibilityClass::HostOwnedRuntimeEvidence || evidenceClass == EvidenceVisibilityClass::SchedulerEvidence || evidenceClass == EvidenceVisibilityClass::BackendBindingEvidence || evidenceClass == EvidenceVisibilityClass::NativeTokenEvidence);
	}
}
