#pragma once

#include "../../../Pipeline/Safety/SecureComputeMemoryReadAdmission.h"
#include "../../../../../ExternalRuntime/Contracts/ExternalSecureMemoryRead.h"
#include "../../../Architecture/Registers/Architectural/ArchContextState.h"
#include "../../../Architecture/Registers/Retire/RetireCoordinator.h"
#include "../../../Pipeline/Safety/YAKSys_Hybrid_CPU.Core.SafetyVerifier.h"
#include "../Commit/ArchitecturalCompletionCommitOwner.h"
#include "../Observation/DomainCompletionObservationOwner.h"
#include "../../Events/Hypercalls/VirtualizationOperandSnapshot.h"
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <algorithm>
#include <limits>
#include <any>
#include <mutex>
#include "tangible_exception_helper.h"
#include <memory>
#include <utility>

// C# TO C++ CONVERTER NOTE: Forward class declarations:
namespace YAKSys_Hybrid_CPU::Core { class Acceptance; }
namespace YAKSys_Hybrid_CPU::Core { class SecureComputeMemoryReadRetirement; }

namespace HybridCPU::ExternalRuntime::Contracts { struct ExternalSecureDomainLease; class ExternalSecureMemoryReadReceipt; class ExternalSecureMemoryReadSubmission; }
using namespace HybridCPU::ExternalRuntime::Contracts;
using namespace YAKSys_Hybrid_CPU::Core::Registers;
using namespace YAKSys_Hybrid_CPU::Core::Registers::Retire;

namespace YAKSys_Hybrid_CPU::Core
{

	class SecureComputeMemoryReadCompletionOwner final : public std::enable_shared_from_this<SecureComputeMemoryReadCompletionOwner>
	{
	public:
		class Acceptance;
// C# TO C++ CONVERTER TASK: C++ has no equivalent to C# primary constructors:
// ORIGINAL LINE: internal sealed class Pending(SecureComputeMemoryReadCertificate certificate, SecureComputeMemoryReadIssuePermit permit, ExternalSecureMemoryReadSubmission submission, int slot, ulong generation, ulong domainScope)
		class Pending final : public std::enable_shared_from_this<Pending>
		{
		public:
			Pending(std::shared_ptr<SecureComputeMemoryReadCertificate> certificate, std::shared_ptr<SecureComputeMemoryReadIssuePermit> permit, std::shared_ptr<ExternalSecureMemoryReadSubmission> submission, int slot, unsigned long long generation, unsigned long long domainScope) : Certificate(std::move(certificate)), Permit(std::move(permit)), Submission(std::move(submission)), Slot(slot), Generation(generation), DomainScope(domainScope), Contexts([]{ std::vector<std::shared_ptr<ArchContextState>> contexts; for (int vt=0; vt<4; ++vt) contexts.push_back(std::make_shared<ArchContextState>(32, vt)); return contexts; }()) {}
			const std::shared_ptr<SecureComputeMemoryReadCertificate> Certificate;
			const std::shared_ptr<SecureComputeMemoryReadIssuePermit> Permit;
			const std::shared_ptr<ExternalSecureMemoryReadSubmission> Submission;
			const int Slot;
			const unsigned long long Generation, DomainScope;
			const std::vector<std::shared_ptr<ArchContextState>> Contexts;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal Acceptance? Accepted;
			std::shared_ptr<Acceptance> Accepted;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal RetireCoordinator? Retire;
			std::shared_ptr<RetireCoordinator> Retire;
		};
	public:
// C# TO C++ CONVERTER TASK: C++ has no equivalent to C# primary constructors:
// ORIGINAL LINE: internal sealed class Acceptance(Pending pending, ExternalSecureMemoryReadReceipt receipt, ulong sequence)
		class Acceptance final : public std::enable_shared_from_this<Acceptance>
		{
		public:
			Acceptance(std::shared_ptr<SecureComputeMemoryReadCompletionOwner::Pending> pending, std::shared_ptr<ExternalSecureMemoryReadReceipt> receipt, unsigned long long sequence) : Pending(std::move(pending)), Receipt(std::move(receipt)), Sequence(sequence) {}
			const std::shared_ptr<SecureComputeMemoryReadCompletionOwner::Pending> Pending;
			const std::shared_ptr<ExternalSecureMemoryReadReceipt> Receipt;
			const unsigned long long Sequence;
		};
	private:
		const std::shared_ptr<SafetyVerifier> issuer;
		std::mutex gate;
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly HashSet<Pending> live = {};
		std::unordered_set<std::shared_ptr<Pending>> live = {};
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly HashSet<SecureComputeMemoryReadCertificate> issued = {};
		std::unordered_set<std::shared_ptr<SecureComputeMemoryReadCertificate>> issued = {};
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly Dictionary<ExternalSecureDomainLease, ulong> domains = {};
		std::vector<std::pair<ExternalSecureDomainLease, unsigned long long>> domains;
		unsigned long long nextGeneration = 1, nextAcceptance = 1, nextDomain = 1;
		const std::shared_ptr<ArchitecturalCompletionCommitOwner> commit = std::make_shared<ArchitecturalCompletionCommitOwner>(std::make_shared<VirtualizationRestoreGenerationOwner>(), std::make_shared<DomainCompletionObservationOwner>(std::make_shared<CompletionGenerationAuthority>()));
		const std::shared_ptr<ArchitecturalCompletionCommitOwner::ProducerRegistration> producer;
	public:
		SecureComputeMemoryReadCompletionOwner(std::shared_ptr<SafetyVerifier> issuer, std::recursive_mutex& issuerGate);
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal Pending? Begin(SecureComputeMemoryReadCertificate certificate, SecureComputeMemoryReadIssuePermit permit, ExternalSecureMemoryReadSubmission submission)
		std::shared_ptr<Pending> Begin(std::shared_ptr<SecureComputeMemoryReadCertificate> certificate, std::shared_ptr<SecureComputeMemoryReadIssuePermit> permit, std::shared_ptr<ExternalSecureMemoryReadSubmission> submission);
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal SecureComputeMemoryReadRetirement? AcceptAndRetire(Pending pending, ExternalSecureMemoryReadReceipt receipt)
		std::shared_ptr<SecureComputeMemoryReadRetirement> AcceptAndRetire(std::shared_ptr<Pending> pending, std::shared_ptr<ExternalSecureMemoryReadReceipt> receipt);
	private:
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private SecureComputeMemoryReadRetirement? Retire(Pending pending, Acceptance acceptance)
		std::shared_ptr<SecureComputeMemoryReadRetirement> Retire(std::shared_ptr<Pending> pending, std::shared_ptr<Acceptance> acceptance);
	public:
		void Cancel(std::shared_ptr<Pending> pending);
	};

}


