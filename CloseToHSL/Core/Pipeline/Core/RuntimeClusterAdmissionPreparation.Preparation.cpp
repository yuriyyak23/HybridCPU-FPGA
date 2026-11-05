#include "RuntimeClusterAdmissionPreparation.Preparation.h"

namespace YAKSys_Hybrid_CPU::Core
{

	RuntimeClusterAdmissionPreparation::RuntimeClusterAdmissionPreparation(YAKSys_Hybrid_CPU::Core::DecodeMode decodeMode, unsigned char preparedScalarMask, unsigned char fallbackDiagnosticsMask, int preparedScalarCount, int auxiliaryReservationCount, bool shouldConsiderClusterAdmission, bool suggestsFallbackDiagnostics)
	{
		DecodeMode = decodeMode;
		PreparedScalarMask = preparedScalarMask;
		FallbackDiagnosticsMask = fallbackDiagnosticsMask;
		PreparedScalarCount = preparedScalarCount;
		AuxiliaryReservationCount = auxiliaryReservationCount;
		ShouldConsiderClusterAdmission = shouldConsiderClusterAdmission;
		SuggestsFallbackDiagnostics = suggestsFallbackDiagnostics;
	}

	YAKSys_Hybrid_CPU::Core::DecodeMode RuntimeClusterAdmissionPreparation::getDecodeMode() const
	{
		return DecodeMode;
	}

	unsigned char RuntimeClusterAdmissionPreparation::getPreparedScalarMask() const
	{
		return PreparedScalarMask;
	}

	unsigned char RuntimeClusterAdmissionPreparation::getFallbackDiagnosticsMask() const
	{
		return FallbackDiagnosticsMask;
	}

	int RuntimeClusterAdmissionPreparation::getPreparedScalarCount() const
	{
		return PreparedScalarCount;
	}

	int RuntimeClusterAdmissionPreparation::getAuxiliaryReservationCount() const
	{
		return AuxiliaryReservationCount;
	}

	bool RuntimeClusterAdmissionPreparation::getShouldConsiderClusterAdmission() const
	{
		return ShouldConsiderClusterAdmission;
	}

	bool RuntimeClusterAdmissionPreparation::getSuggestsFallbackDiagnostics() const
	{
		return SuggestsFallbackDiagnostics;
	}

	RuntimeClusterAdmissionPreparation RuntimeClusterAdmissionPreparation::Create(std::shared_ptr<ClusterIssuePreparation> clusterPreparation)
	{
		return RuntimeClusterAdmissionPreparation(clusterPreparation->getDecodeMode(), clusterPreparation->getScalarClusterGroup()->getPreparedMask(), clusterPreparation->getFallbackDiagnosticsMask(), clusterPreparation->getScalarClusterGroup()->getCount(), clusterPreparation->getAuxiliaryReservations().size(), clusterPreparation->getHasScalarClusterCandidate(), clusterPreparation->getSuggestFallbackDiagnostics());
	}

	RuntimeClusterAdmissionPreparation RuntimeClusterAdmissionPreparation::CreateEmpty()
	{
		return RuntimeClusterAdmissionPreparation(DecodeMode::ReferenceSequentialMode, 0, 0, 0, 0, false, false);
	}
}
