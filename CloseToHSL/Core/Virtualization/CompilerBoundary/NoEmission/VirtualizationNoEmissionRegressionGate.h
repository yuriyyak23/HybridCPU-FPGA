#pragma once
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{

	enum class VirtualizationEmissionDecision : unsigned char
	{
		AllowedCompatibilityProjection = 0,
		DeniedDirectSubstrateEmission = 1,
		DeniedHostEvidenceEmission = 2,
		DeniedNativeTokenEmission = 3,
		DeniedUnvalidatedDescriptorEmission = 4
	};

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct VirtualizationEmissionRequest(bool IsCompatibilityFrontend, bool IsGeneratedProjection, bool DescriptorValidated, bool EmitsHostOwnedEvidence, bool EmitsNativeLaneToken);


struct VirtualizationEmissionRequest final
{
    bool IsCompatibilityFrontend = false;
    bool IsGeneratedProjection = false;
    bool DescriptorValidated = false;
    bool EmitsHostOwnedEvidence = false;
    bool EmitsNativeLaneToken = false;
};
}
