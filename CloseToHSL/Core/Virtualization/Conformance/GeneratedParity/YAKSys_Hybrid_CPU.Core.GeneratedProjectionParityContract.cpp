#include "YAKSys_Hybrid_CPU.Core.GeneratedProjectionParityContract.h"

namespace YAKSys_Hybrid_CPU::Core
{

	GeneratedProjectionParityViolation GeneratedProjectionParityContract::Evaluate(std::shared_ptr<GeneratedProjectionParityRequest> request)
	{
		if (request == nullptr || request->SourceDescriptor == nullptr || !request->SourceDescriptor->getHasDescriptor() || !request->SourceDescriptor->isValidated())
		{
			return GeneratedProjectionParityViolation::SourceNotDescriptorOwned;
		}

		if (request->CarriesHostOwnedEvidence)
		{
			return GeneratedProjectionParityViolation::HostEvidenceProjectionAttempt;
		}

		if (!request->IsGeneratedCompatibilityProjection)
		{
			return GeneratedProjectionParityViolation::MissingGeneratedProjectionMarker;
		}

		if (request->ProjectionSourceHash != request->SourceDescriptor->getDescriptorHash())
		{
			return GeneratedProjectionParityViolation::SourceHashMismatch;
		}

		if (request->ProjectionArtifactHash == 0)
		{
			return GeneratedProjectionParityViolation::ProjectionArtifactHashMissing;
		}

		return GeneratedProjectionParityViolation::None;
	}

	bool GeneratedProjectionParityContract::IsSatisfied(std::shared_ptr<GeneratedProjectionParityRequest> request)
	{
		return Evaluate(request) == GeneratedProjectionParityViolation::None;
	}
}
