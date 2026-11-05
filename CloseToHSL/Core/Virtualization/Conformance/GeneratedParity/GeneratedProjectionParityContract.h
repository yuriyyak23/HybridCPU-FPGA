#pragma once

#include "../../Sideband/DescriptorTransport/YAKSys_Hybrid_CPU.Core.DescriptorSidebandEnvelope.h"
#include <memory>
#include <utility>

namespace YAKSys_Hybrid_CPU::Core
{
	enum class GeneratedProjectionParityViolation : unsigned char
	{
		None = 0,
		SourceNotDescriptorOwned = 1,
		MissingGeneratedProjectionMarker = 2,
		SourceHashMismatch = 3,
		ProjectionArtifactHashMissing = 4,
		HostEvidenceProjectionAttempt = 5
	};

	struct GeneratedProjectionParityRequest final {
		std::shared_ptr<DescriptorSidebandEnvelope> SourceDescriptor;
		unsigned long long ProjectionSourceHash = 0;
		unsigned long long ProjectionArtifactHash = 0;
		bool IsGeneratedCompatibilityProjection = false;
		bool CarriesHostOwnedEvidence = false;

		GeneratedProjectionParityRequest(std::shared_ptr<DescriptorSidebandEnvelope> sourceDescriptor, unsigned long long projectionSourceHash, unsigned long long projectionArtifactHash, bool isGeneratedCompatibilityProjection, bool carriesHostOwnedEvidence)
			: SourceDescriptor(std::move(sourceDescriptor)), ProjectionSourceHash(projectionSourceHash), ProjectionArtifactHash(projectionArtifactHash), IsGeneratedCompatibilityProjection(isGeneratedCompatibilityProjection), CarriesHostOwnedEvidence(carriesHostOwnedEvidence) {}
	};
}
