#pragma once

#include "../../Compatibility/Generated/VmcsProjection/VmcsFieldProjectionSchema.h"
#include "../../Sideband/DescriptorTransport/YAKSys_Hybrid_CPU.Core.DescriptorSidebandEnvelope.h"
#include <memory>
#include <utility>

namespace YAKSys_Hybrid_CPU::Core
{
	enum class GeneratedVmcsProjectionProvenanceViolation : unsigned char
	{
		None = 0,
		FieldSchemaParityFailed = 1,
		GeneratedParityFailed = 2,
		ArtifactHashMismatch = 3
	};

	struct GeneratedVmcsProjectionProvenanceRequest final {
		std::shared_ptr<VmcsFieldProjectionSchemaArtifact> FieldSchema;
		std::shared_ptr<DescriptorSidebandEnvelope> SourceDescriptor;
		unsigned long long ProjectionSourceHash = 0;
		unsigned long long ProjectionArtifactHash = 0;
		bool IsGeneratedCompatibilityProjection = false;
		bool CarriesHostOwnedEvidence = false;

		GeneratedVmcsProjectionProvenanceRequest(std::shared_ptr<VmcsFieldProjectionSchemaArtifact> fieldSchema, std::shared_ptr<DescriptorSidebandEnvelope> sourceDescriptor, unsigned long long projectionSourceHash, unsigned long long projectionArtifactHash, bool isGeneratedCompatibilityProjection, bool carriesHostOwnedEvidence)
			: FieldSchema(std::move(fieldSchema)), SourceDescriptor(std::move(sourceDescriptor)), ProjectionSourceHash(projectionSourceHash), ProjectionArtifactHash(projectionArtifactHash), IsGeneratedCompatibilityProjection(isGeneratedCompatibilityProjection), CarriesHostOwnedEvidence(carriesHostOwnedEvidence) {}
	};
}
