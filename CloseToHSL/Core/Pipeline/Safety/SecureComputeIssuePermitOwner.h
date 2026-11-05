#pragma once

#include "SecureComputePlacedIntent.h"
#include "SecureComputeLiveCoreTarget.h"
#include <memory>
#include <utility>

class Guid;
namespace HybridCPU::ExternalRuntime::Contracts { class ExternalSecureExecutionReservation; }
using namespace HybridCPU::ExternalRuntime::Contracts;

namespace YAKSys_Hybrid_CPU::Core
{
	class SecureComputeCanonicalAdmissionResult;

	/// <summary>
	/// Historical machine-local consumption fact, never an execution entitlement,
	/// backend receipt, completion acceptance or architectural commit. No token
	/// accepted by the permit owner is exposed through this observation.
	/// </summary>
	class SecureComputeMachinePermitConsumption final : public std::enable_shared_from_this<SecureComputeMachinePermitConsumption>
	{
	private:
		const std::shared_ptr<Guid> Scope;
		const unsigned long long Generation;
		const std::shared_ptr<SecureComputePlacedIntent> PlacedIntent;
		const std::shared_ptr<ExternalSecureExecutionReservation> Reservation;

	public:
		SecureComputeMachinePermitConsumption(std::shared_ptr<Guid> scope, unsigned long long generation, std::shared_ptr<SecureComputePlacedIntent> placed, std::shared_ptr<ExternalSecureExecutionReservation> reservation);

		std::shared_ptr<Guid> getScope() const;
		unsigned long long getGeneration() const;
		std::shared_ptr<SecureComputePlacedIntent> getPlacedIntent() const;
		std::shared_ptr<ExternalSecureExecutionReservation> getReservation() const;
	};

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER TASK: C++ has no equivalent to C# primary constructors:
// ORIGINAL LINE: internal sealed class SecureComputeMachineCertificate(SecureComputePlacedIntent placed, ExternalSecureExecutionReservation reservation, ulong generation, SecureComputeLiveCoreTarget? liveCoreTarget = null)
	class SecureComputeMachineCertificate final : public std::enable_shared_from_this<SecureComputeMachineCertificate>
	{
	private:
		const std::shared_ptr<SecureComputePlacedIntent> Placed;
		const std::shared_ptr<ExternalSecureExecutionReservation> Reservation;
		const unsigned long long Generation;
		const std::shared_ptr<SecureComputeLiveCoreTarget> LiveCoreTarget;

	public:
		SecureComputeMachineCertificate(std::shared_ptr<SecureComputePlacedIntent> placed, std::shared_ptr<ExternalSecureExecutionReservation> reservation, unsigned long long generation, std::shared_ptr<SecureComputeLiveCoreTarget> liveCoreTarget = nullptr) : Placed(std::move(placed)), Reservation(std::move(reservation)), Generation(generation), LiveCoreTarget(std::move(liveCoreTarget)) {}
		std::shared_ptr<SecureComputePlacedIntent> getPlaced() const;
		std::shared_ptr<ExternalSecureExecutionReservation> getReservation() const;
		unsigned long long getGeneration() const;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal SecureComputeLiveCoreTarget? LiveCoreTarget {get;} = liveCoreTarget;
		std::shared_ptr<SecureComputeLiveCoreTarget> getLiveCoreTarget() const;
	};

// C# TO C++ CONVERTER TASK: C++ has no equivalent to C# primary constructors:
// ORIGINAL LINE: internal sealed class SecureComputeIssuePermit(SecureComputeMachineCertificate certificate)
	class SecureComputeIssuePermit final : public std::enable_shared_from_this<SecureComputeIssuePermit>
	{
	private:
		const std::shared_ptr<SecureComputeMachineCertificate> Certificate;

	public:
		explicit SecureComputeIssuePermit(std::shared_ptr<SecureComputeMachineCertificate> certificate) : Certificate(std::move(certificate)) {}
		std::shared_ptr<SecureComputeMachineCertificate> getCertificate() const;
	};

	/// <summary>
	/// Registers and consumes only certificates retained by the issuing verifier.
	/// The gate belongs to that standalone machine scope; no provider callbacks or
	/// semantic authority run under it. Backend entitlement is a separate conjunct.
	/// </summary>

// C# TO C++ CONVERTER TASK: C++ has no equivalent to C# primary constructors:
// ORIGINAL LINE: internal sealed class SecureComputePreparedMachineAdmission(SecureComputeCanonicalAdmissionResult result, SecureComputeMachineCertificate certificate, SecureComputeIssuePermit permit)
	class SecureComputePreparedMachineAdmission final : public std::enable_shared_from_this<SecureComputePreparedMachineAdmission>
	{
	private:
		const std::shared_ptr<SecureComputeCanonicalAdmissionResult> Result;
		const std::shared_ptr<SecureComputeMachineCertificate> Certificate;
		const std::shared_ptr<SecureComputeIssuePermit> Permit;

	public:
		SecureComputePreparedMachineAdmission(std::shared_ptr<SecureComputeCanonicalAdmissionResult> result, std::shared_ptr<SecureComputeMachineCertificate> certificate, std::shared_ptr<SecureComputeIssuePermit> permit) : Result(std::move(result)), Certificate(std::move(certificate)), Permit(std::move(permit)) {}
		std::shared_ptr<SecureComputeCanonicalAdmissionResult> getResult() const;
		std::shared_ptr<SecureComputeMachineCertificate> getCertificate() const;
		std::shared_ptr<SecureComputeIssuePermit> getPermit() const;
	};

}
