#pragma once

namespace YAKSys_Hybrid_CPU::Core::Registers { class ArchContextState; }
#include "../Commit/ArchitecturalCompletionCommitOwner.h"
#include "../../../../../ExternalRuntime/Contracts/ExternalSecureNoEffectExecution.h"
#include "../../../Pipeline/Safety/SecureComputeIssuePermitOwner.h"
#include "../../../Pipeline/Safety/YAKSys_Hybrid_CPU.Core.SafetyVerifier.h"
#include "../../../Architecture/Registers/Retire/RetireCoordinator.h"
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <algorithm>
#include <limits>
#include <any>
#include <optional>
#include <mutex>
#include "tangible_exception_helper.h"
#include <memory>
#include <utility>

// C# TO C++ CONVERTER NOTE: Forward class declarations:
namespace YAKSys_Hybrid_CPU::Core { class Acceptance; }

namespace HybridCPU::ExternalRuntime::Contracts { struct ExternalSecureDomainLease; }
using namespace HybridCPU::ExternalRuntime::Contracts;
using namespace YAKSys_Hybrid_CPU::Core::Registers;
using namespace YAKSys_Hybrid_CPU::Core::Registers::Retire;

namespace YAKSys_Hybrid_CPU::Core
{
	class SecureComputePreparedMachineAdmission;
	class SecureComputeMachineCertificate;
	class SafetyVerifier;

	/// <summary>Historical no-effect canonical retirement, never an ADD result or OS publication.</summary>
	class SecureComputeNoEffectRetirement final : public std::enable_shared_from_this<SecureComputeNoEffectRetirement>
	{
	private:
		const std::shared_ptr<ExternalSecureNoEffectSubmission> Submission;
		const unsigned long long AcceptanceSequence;
		const unsigned long long CanonicalOrderSequence;
		const unsigned long long CommitSequence;
		const unsigned long long RestoreGeneration;
		const unsigned long long Register3Value;

	public:
		SecureComputeNoEffectRetirement(std::shared_ptr<ExternalSecureNoEffectSubmission> submission, unsigned long long acceptanceSequence, std::shared_ptr<ArchitecturalCompletionReceiptBinding> binding, unsigned long long register3Value);
		std::shared_ptr<ExternalSecureNoEffectSubmission> getSubmission() const;
		unsigned long long getAcceptanceSequence() const;
		unsigned long long getCanonicalOrderSequence() const;
		unsigned long long getCommitSequence() const;
		unsigned long long getRestoreGeneration() const;
		unsigned long long getRegister3Value() const;
		int getEffectCount() const;
	};

	/// <summary>
	/// Bounded standalone completion window. It accepts original no-effect receipts
	/// before canonical retire, using the existing commit owner and RetireCoordinator.
	/// A window slot is not a hardware ROB identity or a semantic domain authority.
	/// </summary>
	class SecureComputeNoEffectCompletionOwner final : public std::enable_shared_from_this<SecureComputeNoEffectCompletionOwner>
	{
	private:
		const std::vector<std::shared_ptr<ArchContextState>> Contexts;
		const std::shared_ptr<ArchitecturalCompletionCommitOwner> CommitOwner;

	public:
		class Acceptance;
// C# TO C++ CONVERTER TASK: C++ has no equivalent to C# primary constructors:
// ORIGINAL LINE: internal sealed class Pending(SecureComputePreparedMachineAdmission prepared, ExternalSecureNoEffectSubmission submission, ulong domainScope, IExternalSecureNoEffectExecutionProvider provider)
		class Pending final : public std::enable_shared_from_this<Pending>
		{
		private:
			const std::shared_ptr<SecureComputePreparedMachineAdmission> Prepared;
			const std::shared_ptr<ExternalSecureNoEffectSubmission> Submission;
			const unsigned long long DomainScope;
			const std::shared_ptr<IExternalSecureNoEffectExecutionProvider> Provider;

		public:
			Pending(std::shared_ptr<SecureComputePreparedMachineAdmission> prepared, std::shared_ptr<ExternalSecureNoEffectSubmission> submission, unsigned long long domainScope, std::shared_ptr<IExternalSecureNoEffectExecutionProvider> provider) : Prepared(std::move(prepared)), Submission(std::move(submission)), DomainScope(domainScope), Provider(std::move(provider)) {}
			std::shared_ptr<SecureComputePreparedMachineAdmission> getPrepared() const;
			std::shared_ptr<ExternalSecureNoEffectSubmission> getSubmission() const;
			unsigned long long getDomainScope() const;
			std::shared_ptr<IExternalSecureNoEffectExecutionProvider> getProvider() const;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal Acceptance? Accepted;
			std::shared_ptr<Acceptance> Accepted;
		};

	public:
		class Acceptance;
// C# TO C++ CONVERTER TASK: C++ has no equivalent to C# primary constructors:
// ORIGINAL LINE: internal sealed class Acceptance(Pending pending, ExternalSecureNoEffectExecutionReceipt receipt, ulong sequence)
		class Acceptance final : public std::enable_shared_from_this<Acceptance>
		{
		private:
			const std::shared_ptr<YAKSys_Hybrid_CPU::Core::SecureComputeNoEffectCompletionOwner::Pending> Pending;
			const std::shared_ptr<ExternalSecureNoEffectExecutionReceipt> Receipt;
			const unsigned long long Sequence;

		public:
			Acceptance(std::shared_ptr<SecureComputeNoEffectCompletionOwner::Pending> pending, std::shared_ptr<ExternalSecureNoEffectExecutionReceipt> receipt, unsigned long long sequence) : Pending(std::move(pending)), Receipt(std::move(receipt)), Sequence(sequence) {}
			std::shared_ptr<YAKSys_Hybrid_CPU::Core::SecureComputeNoEffectCompletionOwner::Pending> getPending() const;
			std::shared_ptr<ExternalSecureNoEffectExecutionReceipt> getReceipt() const;
			unsigned long long getSequence() const;
		};

	private:
		const std::shared_ptr<SafetyVerifier> issuer;
		// Protects this owner's collections; issuer checks take the verifier's own gate.
		std::mutex gate;
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly HashSet<Pending> live = {};
		std::unordered_set<std::shared_ptr<Pending>> live = {};
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly Dictionary<Pending, ExternalSecureNoEffectExecutionReceipt> observed = {};
		std::unordered_map<std::shared_ptr<Pending>, std::shared_ptr<ExternalSecureNoEffectExecutionReceipt>> observed = std::unordered_map<std::shared_ptr<Pending>, std::shared_ptr<ExternalSecureNoEffectExecutionReceipt>>();
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly HashSet<Pending> invalidSources = {};
		std::unordered_set<std::shared_ptr<Pending>> invalidSources = {};
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly HashSet<SecureComputeMachineCertificate> issued = {};
		std::unordered_set<std::shared_ptr<SecureComputeMachineCertificate>> issued = {};
		// Numeric projection labels only, bound to the original neutral domain lease.
		// They satisfy the neutral observation ABI without minting semantic domains.
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly Dictionary<ExternalSecureDomainLease, ulong> domainScopes = {};
		std::vector<std::pair<ExternalSecureDomainLease, unsigned long long>> domainScopes;
		unsigned long long nextDomainScope = 1, nextGeneration = 1, nextAcceptance = 1;
	public:
		std::vector<std::shared_ptr<ArchContextState>> getContexts() const;
		std::shared_ptr<ArchitecturalCompletionCommitOwner> getCommitOwner() const;
	private:
		const std::shared_ptr<ArchitecturalCompletionCommitOwner::ProducerRegistration> producer;
		const std::shared_ptr<RetireCoordinator> retire;

	public:
		SecureComputeNoEffectCompletionOwner(std::shared_ptr<SafetyVerifier> issuer, std::recursive_mutex& gate);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal Pending? Begin(SecureComputePreparedMachineAdmission prepared, ExternalSecureExecutionAttempt attempt, ExternalSecureExecutionConsumptionReceipt consumption, IExternalSecureNoEffectExecutionProvider provider)
		std::shared_ptr<Pending> Begin(std::shared_ptr<SecureComputePreparedMachineAdmission> prepared, std::shared_ptr<ExternalSecureExecutionAttempt> attempt, std::shared_ptr<ExternalSecureExecutionConsumptionReceipt> consumption, std::shared_ptr<IExternalSecureNoEffectExecutionProvider> provider);

		std::optional<std::shared_ptr<ExternalSecureNoEffectExecutionResult>> ObserveOriginal(std::shared_ptr<Pending> pending);

	private:
		void InvalidateSource(std::shared_ptr<Pending> pending);

		bool Matches(std::shared_ptr<SecureComputePreparedMachineAdmission> prepared, std::shared_ptr<ExternalSecureExecutionAttempt> attempt);

	public:
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal Acceptance? Accept(Pending pending, ExternalSecureNoEffectExecutionReceipt receipt)
		std::shared_ptr<Acceptance> Accept(std::shared_ptr<Pending> pending, std::shared_ptr<ExternalSecureNoEffectExecutionReceipt> receipt);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal SecureComputeNoEffectRetirement? Retire(Pending pending, Acceptance acceptance)
		std::shared_ptr<SecureComputeNoEffectRetirement> Retire(std::shared_ptr<Pending> pending, std::shared_ptr<Acceptance> acceptance);

		void Cancel(std::shared_ptr<Pending> pending);

		void PruneInvalidated();

		void InvalidateAfterRestore();
	};

}





