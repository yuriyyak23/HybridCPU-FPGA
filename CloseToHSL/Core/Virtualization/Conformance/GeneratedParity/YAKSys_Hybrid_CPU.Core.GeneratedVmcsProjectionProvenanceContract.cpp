#include "YAKSys_Hybrid_CPU.Core.GeneratedVmcsProjectionProvenanceContract.h"

namespace YAKSys_Hybrid_CPU::Core
{

	GeneratedVmcsProjectionProvenanceContract::GeneratedVmcsProjectionProvenanceContract() : GeneratedVmcsProjectionProvenanceContract(std::make_shared<VmcsFieldProjectionSchemaConformanceContract>(), std::make_shared<GeneratedProjectionParityContract>())
	{
	}

	GeneratedVmcsProjectionProvenanceContract::GeneratedVmcsProjectionProvenanceContract(std::shared_ptr<VmcsFieldProjectionSchemaConformanceContract> fieldSchema, std::shared_ptr<GeneratedProjectionParityContract> generatedParity) : _fieldSchema(fieldSchema), _generatedParity(generatedParity)
	{
	}

	GeneratedVmcsProjectionProvenanceViolation GeneratedVmcsProjectionProvenanceContract::Evaluate(std::shared_ptr<GeneratedVmcsProjectionProvenanceRequest> request)
	{
		if (request == nullptr || request->FieldSchema == nullptr || !_fieldSchema->IsSatisfied(request->FieldSchema))
		{
			return GeneratedVmcsProjectionProvenanceViolation::FieldSchemaParityFailed;
		}

		if (request->ProjectionArtifactHash != request->FieldSchema->ArtifactHash)
		{
			return GeneratedVmcsProjectionProvenanceViolation::ArtifactHashMismatch;
		}

		std::shared_ptr<GeneratedProjectionParityRequest> parity = std::make_shared<GeneratedProjectionParityRequest>(request->SourceDescriptor, request->ProjectionSourceHash, request->ProjectionArtifactHash, request->IsGeneratedCompatibilityProjection, request->CarriesHostOwnedEvidence);

		return _generatedParity->IsSatisfied(parity) ? GeneratedVmcsProjectionProvenanceViolation::None : GeneratedVmcsProjectionProvenanceViolation::GeneratedParityFailed;
	}

	bool GeneratedVmcsProjectionProvenanceContract::IsSatisfied(std::shared_ptr<GeneratedVmcsProjectionProvenanceRequest> request)
	{
		return Evaluate(request) == GeneratedVmcsProjectionProvenanceViolation::None;
	}
}
