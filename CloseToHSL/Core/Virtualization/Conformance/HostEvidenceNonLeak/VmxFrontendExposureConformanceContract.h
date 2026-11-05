#pragma once
#include <memory>
#include "../../../Runtime/Domains/Descriptors/EvidencePolicy/EvidencePolicyDescriptor.h"
#include "../../../Runtime/Migration/Format/MigrationDescriptor.h"

namespace YAKSys_Hybrid_CPU::Core
{

	enum class VmxFrontendExposureViolation : unsigned char
	{
		None = 0,
		HostEvidenceVisible = 1,
		NativeTokenVisible = 2,
		BackendBindingVisible = 3,
		SchedulerEvidenceVisible = 4,
		MigrationOnlyStateVisible = 5,
		LanePrivateStateVisible = 6,
		NonGeneratedProjection = 7,
		MissingAccessPolicy = 8
	};

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct VmxFrontendExposureRequest(EvidenceVisibilityClass EvidenceClass, MigrationPayloadClass PayloadClass, bool IsGuestVisible, bool ExposesNativeToken, bool ExposesBackendHandle, bool ExposesSchedulerEvidence, bool ExposesMigrationOnlyState, bool ExposesLanePrivateState, bool IsGeneratedProjection, bool HasAccessPolicy);


struct VmxFrontendExposureRequest final
{
    EvidenceVisibilityClass EvidenceClass{};
    MigrationPayloadClass PayloadClass{};
    bool IsGuestVisible = false;
    bool ExposesNativeToken = false;
    bool ExposesBackendHandle = false;
    bool ExposesSchedulerEvidence = false;
    bool ExposesMigrationOnlyState = false;
    bool ExposesLanePrivateState = false;
    bool IsGeneratedProjection = false;
    bool HasAccessPolicy = false;
};
}
