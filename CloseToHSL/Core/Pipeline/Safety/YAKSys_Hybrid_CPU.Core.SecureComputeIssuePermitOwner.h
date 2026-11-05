#pragma once

#include "YAKSys_Hybrid_CPU.Core.SafetyVerifier.h"
#include "SecureComputeIssuePermitOwner.h"
#include "SecureComputePlacedIntent.h"
#include "SecureComputeMemoryReadAdmission.h"
#include <unordered_set>
#include <algorithm>
#include <mutex>
#include <memory>
#include <utility>

using namespace HybridCPU::ExternalRuntime::Contracts;

using namespace HybridCPU_ISE::Arch;
using namespace YAKSys_Hybrid_CPU::Core::Decoder;
using namespace YAKSys_Hybrid_CPU::Core::Registers::Retire;

namespace YAKSys_Hybrid_CPU::Core
{
	class SafetyVerifier;
	class SecureComputeMemoryReadCertificate;
	class SecureComputeMemoryReadIssuePermit;
	/// <summary>
	/// Historical machine-local consumption fact, never an execution entitlement,
	/// backend receipt, completion acceptance or architectural commit. No token
	/// accepted by the permit owner is exposed through this observation.
	/// </summary>




	/// <summary>
	/// Registers and consumes only certificates retained by the issuing verifier.
	/// The gate belongs to that standalone machine scope; no provider callbacks or
	/// semantic authority run under it. Backend entitlement is a separate conjunct.
	/// </summary>
// C# TO C++ CONVERTER TASK: C++ has no equivalent to C# primary constructors:
// ORIGINAL LINE: internal sealed partial class SecureComputeIssuePermitOwner(SafetyVerifier issuer, Object gate)
	class SecureComputeIssuePermitOwner final : public std::enable_shared_from_this<SecureComputeIssuePermitOwner>
	{
	private:
		std::weak_ptr<SafetyVerifier> issuer;
		std::recursive_mutex& gate;
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly HashSet<SecureComputeIssuePermit> live = {};
		std::unordered_set<std::shared_ptr<SecureComputeIssuePermit>> live = {};
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly HashSet<SecureComputeIssuePermit> consumed = {};
		std::unordered_set<std::shared_ptr<SecureComputeIssuePermit>> consumed = {};

	public:
		SecureComputeIssuePermitOwner(std::shared_ptr<SafetyVerifier> owner, std::recursive_mutex& ownerGate) : issuer(std::move(owner)), gate(ownerGate) {}
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal SecureComputeIssuePermit? Register(SecureComputeMachineCertificate certificate)
		std::shared_ptr<SecureComputeIssuePermit> Register(std::shared_ptr<SecureComputeMachineCertificate> certificate);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal SecureComputeMachinePermitConsumption? Consume(SecureComputeIssuePermit permit, ExternalSecureRegisterLocalRequest request, SecureComputePlacedIntent placed, ExternalSecureExecutionReservation reservation)
		std::shared_ptr<SecureComputeMachinePermitConsumption> Consume(std::shared_ptr<SecureComputeIssuePermit> permit, std::shared_ptr<ExternalSecureRegisterLocalRequest> request, std::shared_ptr<SecureComputePlacedIntent> placed, std::shared_ptr<ExternalSecureExecutionReservation> reservation);

		void PruneInvalidated();

		bool HasConsumed(std::shared_ptr<SecureComputeIssuePermit> permit);


	private:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly HashSet<SecureComputeMemoryReadIssuePermit> memoryLive = {};
		std::unordered_set<std::shared_ptr<SecureComputeMemoryReadIssuePermit>> memoryLive = {};
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly HashSet<SecureComputeMemoryReadIssuePermit> memoryConsumed = {};
		std::unordered_set<std::shared_ptr<SecureComputeMemoryReadIssuePermit>> memoryConsumed = {};
	public:
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal SecureComputeMemoryReadIssuePermit? Register(SecureComputeMemoryReadCertificate certificate)
		std::shared_ptr<SecureComputeMemoryReadIssuePermit> Register(std::shared_ptr<SecureComputeMemoryReadCertificate> certificate);
		bool Consume(std::shared_ptr<SecureComputeMemoryReadIssuePermit> permit);
		bool HasConsumed(std::shared_ptr<SecureComputeMemoryReadIssuePermit> permit);
		void PruneMemoryRead();
	};
}
