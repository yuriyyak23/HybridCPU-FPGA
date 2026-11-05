#include "SecureComputeIssuePermitOwner.h"

using namespace HybridCPU::ExternalRuntime::Contracts;

namespace YAKSys_Hybrid_CPU::Core
{

	SecureComputeMachinePermitConsumption::SecureComputeMachinePermitConsumption(std::shared_ptr<Guid> scope, unsigned long long generation, std::shared_ptr<SecureComputePlacedIntent> placed, std::shared_ptr<ExternalSecureExecutionReservation> reservation)
		: Scope(std::move(scope)), Generation(generation), PlacedIntent(std::move(placed)), Reservation(std::move(reservation))
	{
	}

	std::shared_ptr<Guid> SecureComputeMachinePermitConsumption::getScope() const
	{
		return Scope;
	}

	unsigned long long SecureComputeMachinePermitConsumption::getGeneration() const
	{
		return Generation;
	}

	std::shared_ptr<SecureComputePlacedIntent> SecureComputeMachinePermitConsumption::getPlacedIntent() const
	{
		return PlacedIntent;
	}

	std::shared_ptr<ExternalSecureExecutionReservation> SecureComputeMachinePermitConsumption::getReservation() const
	{
		return Reservation;
	}

	std::shared_ptr<SecureComputePlacedIntent> SecureComputeMachineCertificate::getPlaced() const
	{
		return Placed;
	}

	std::shared_ptr<ExternalSecureExecutionReservation> SecureComputeMachineCertificate::getReservation() const
	{
		return Reservation;
	}

	unsigned long long SecureComputeMachineCertificate::getGeneration() const
	{
		return Generation;
	}

	std::shared_ptr<SecureComputeLiveCoreTarget> SecureComputeMachineCertificate::getLiveCoreTarget() const
	{
		return LiveCoreTarget;
	}

	std::shared_ptr<SecureComputeMachineCertificate> SecureComputeIssuePermit::getCertificate() const
	{
		return Certificate;
	}

	std::shared_ptr<SecureComputeCanonicalAdmissionResult> SecureComputePreparedMachineAdmission::getResult() const
	{
		return Result;
	}

	std::shared_ptr<SecureComputeMachineCertificate> SecureComputePreparedMachineAdmission::getCertificate() const
	{
		return Certificate;
	}

	std::shared_ptr<SecureComputeIssuePermit> SecureComputePreparedMachineAdmission::getPermit() const
	{
		return Permit;
	}
}
