#pragma once

#include "../../../Architecture/Registers/Retire/Rf081ScalarRegisterWriteRetireEffect.h"
#include "../Commit/ArchitecturalCompletionCommitOwner.h"
#include "../../../Pipeline/Safety/SecureComputeIssuePermitOwner.h"
#include "../../../Pipeline/Scheduling/PostStageBIssuedAttempt.h"
#include "../../../Architecture/Registers/Architectural/ArchContextState.h"
#include "../../../Architecture/Registers/Retire/RetireCoordinator.h"
#include "../../../Pipeline/Safety/YAKSys_Hybrid_CPU.Core.SafetyVerifier.h"
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <algorithm>
#include <limits>
#include <any>
#include <optional>
#include <mutex>
#include <memory>
#include <utility>

// C# TO C++ CONVERTER NOTE: Forward class declarations:
namespace YAKSys_Hybrid_CPU::Core { class Acceptance; }

namespace HybridCPU::ExternalRuntime::Contracts { class ExternalSecureRegisterLocalSubmission; class ExternalSecureRegisterLocalExecutionReceipt; class ExternalSecureRegisterLocalExecutionResult; class IExternalSecureRegisterLocalExecutionProvider; struct ExternalSecureDomainLease; class ExternalSecureExecutionAttempt; class ExternalSecureExecutionConsumptionReceipt; }
using namespace HybridCPU::ExternalRuntime::Contracts;
using namespace YAKSys_Hybrid_CPU::Core::Registers;
using namespace YAKSys_Hybrid_CPU::Core::Registers::Retire;

namespace YAKSys_Hybrid_CPU::Core
{
	class SecureComputePreparedMachineAdmission;
	class SecureComputeMachineCertificate;
	class SafetyVerifier;

	/// <summary>Historical canonical register-local ADD result, never an OS publication or live permit.</summary>
	class SecureComputeRegisterLocalRetirement final : public std::enable_shared_from_this<SecureComputeRegisterLocalRetirement>
	{
	private:
		const std::shared_ptr<ExternalSecureRegisterLocalSubmission> Submission;
		const unsigned long long AcceptanceSequence;
		const unsigned long long CanonicalOrderSequence;
		const unsigned long long CommitSequence;
		const unsigned long long RestoreGeneration;
		const unsigned long long Register3Value;
		const std::shared_ptr<ScalarRegisterWriteRetireEffect> Effect;

	public:
		SecureComputeRegisterLocalRetirement(std::shared_ptr<ExternalSecureRegisterLocalSubmission> submission, unsigned long long acceptanceSequence, std::shared_ptr<ArchitecturalCompletionReceiptBinding> binding, unsigned long long register3Value, std::shared_ptr<ScalarRegisterWriteRetireEffect> effect);
		std::shared_ptr<ExternalSecureRegisterLocalSubmission> getSubmission() const;
		unsigned long long getAcceptanceSequence() const;
		unsigned long long getCanonicalOrderSequence() const;
		unsigned long long getCommitSequence() const;
		unsigned long long getRestoreGeneration() const;
		unsigned long long getRegister3Value() const;
		int getEffectCount() const;
		std::shared_ptr<ScalarRegisterWriteRetireEffect> getEffect() const;
	};

	/// <summary>
	/// Bounded completion window. It accepts original bounded arithmetic receipts
	/// before canonical retire, using the existing commit owner and RetireCoordinator.
	/// State is per-attempt unless a live core retains its explicit target owners.
	/// A window slot is not a hardware ROB identity or a semantic domain authority.
	/// </summary>
	class SecureComputeRegisterLocalCompletionOwner final : public std::enable_shared_from_this<SecureComputeRegisterLocalCompletionOwner>
	{
	private:
		const std::shared_ptr<ArchitecturalCompletionCommitOwner> CommitOwner;

	public:
		class Acceptance;
// C# TO C++ CONVERTER TASK: C++ has no equivalent to C# primary constructors:
// ORIGINAL LINE: internal sealed class Pending(SecureComputePreparedMachineAdmission prepared, ExternalSecureRegisterLocalSubmission submission, ulong domainScope, IExternalSecureRegisterLocalExecutionProvider provider, PostStageBIssuedAttempt issuedAttempt)
		class Pending final : public std::enable_shared_from_this<Pending>
		{
		private:
			const std::shared_ptr<SecureComputePreparedMachineAdmission> Prepared;
			const std::shared_ptr<ExternalSecureRegisterLocalSubmission> Submission;
			const unsigned long long DomainScope;
			const std::shared_ptr<IExternalSecureRegisterLocalExecutionProvider> Provider;
			const std::shared_ptr<PostStageBIssuedAttempt> IssuedAttempt;
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly IReadOnlyList<ArchContextState> Contexts = (prepared.Certificate.LiveCoreTarget == null ? null : prepared.Certificate.LiveCoreTarget.Contexts) ?? Enumerable.Range(0, 4).Select(vt => new ArchContextState(32, vt)).ToArray();
			const std::vector<std::shared_ptr<ArchContextState>> Contexts;

		public:
			Pending(std::shared_ptr<SecureComputePreparedMachineAdmission> prepared, std::shared_ptr<ExternalSecureRegisterLocalSubmission> submission, unsigned long long domainScope, std::shared_ptr<IExternalSecureRegisterLocalExecutionProvider> provider, std::shared_ptr<PostStageBIssuedAttempt> issuedAttempt);
			std::shared_ptr<SecureComputePreparedMachineAdmission> getPrepared() const;
			std::shared_ptr<ExternalSecureRegisterLocalSubmission> getSubmission() const;
			unsigned long long getDomainScope() const;
			std::shared_ptr<IExternalSecureRegisterLocalExecutionProvider> getProvider() const;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal Acceptance? Accepted;
			std::shared_ptr<Acceptance> Accepted;
			std::shared_ptr<PostStageBIssuedAttempt> getIssuedAttempt() const;
			const std::vector<std::shared_ptr<ArchContextState>> getContexts() const;
		private:
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private RetireCoordinator? retire;
			mutable std::shared_ptr<RetireCoordinator> retire;
		public:
			std::shared_ptr<RetireCoordinator> getRetire() const;
		};

	public:
// C# TO C++ CONVERTER TASK: C++ has no equivalent to C# primary constructors:
// ORIGINAL LINE: internal sealed class Acceptance(Pending pending, ExternalSecureRegisterLocalExecutionReceipt receipt, ulong sequence)
		class Acceptance final : public std::enable_shared_from_this<Acceptance>
		{
		private:
			const std::shared_ptr<YAKSys_Hybrid_CPU::Core::SecureComputeRegisterLocalCompletionOwner::Pending> Pending;
			const std::shared_ptr<ExternalSecureRegisterLocalExecutionReceipt> Receipt;
			const unsigned long long Sequence;

		public:
			Acceptance(std::shared_ptr<SecureComputeRegisterLocalCompletionOwner::Pending> pending, std::shared_ptr<ExternalSecureRegisterLocalExecutionReceipt> receipt, unsigned long long sequence) : Pending(std::move(pending)), Receipt(std::move(receipt)), Sequence(sequence) {}
			std::shared_ptr<YAKSys_Hybrid_CPU::Core::SecureComputeRegisterLocalCompletionOwner::Pending> getPending() const;
			std::shared_ptr<ExternalSecureRegisterLocalExecutionReceipt> getReceipt() const;
			unsigned long long getSequence() const;
		};

	private:
		const std::shared_ptr<SafetyVerifier> issuer;
		std::recursive_mutex &gate;
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly HashSet<Pending> live = {};
		std::unordered_set<std::shared_ptr<Pending>> live = {};
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly Dictionary<Pending, ExternalSecureRegisterLocalExecutionReceipt> observed = {};
		std::unordered_map<std::shared_ptr<Pending>, std::shared_ptr<ExternalSecureRegisterLocalExecutionReceipt>> observed = std::unordered_map<std::shared_ptr<Pending>, std::shared_ptr<ExternalSecureRegisterLocalExecutionReceipt>>();
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
		std::vector<std::pair<std::shared_ptr<ExternalSecureDomainLease>, unsigned long long>> domainScopes;
		unsigned long long nextDomainScope = 1, nextGeneration = 1, nextAcceptance = 1;
	public:
		std::shared_ptr<ArchitecturalCompletionCommitOwner> getCommitOwner() const;
	private:
		const std::shared_ptr<ArchitecturalCompletionCommitOwner::ProducerRegistration> producer;

	public:
		SecureComputeRegisterLocalCompletionOwner(std::shared_ptr<SafetyVerifier> issuer, std::recursive_mutex &gate);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal Pending? Begin(SecureComputePreparedMachineAdmission prepared, ExternalSecureExecutionAttempt attempt, ExternalSecureExecutionConsumptionReceipt consumption, IExternalSecureRegisterLocalExecutionProvider provider)
		std::shared_ptr<Pending> Begin(std::shared_ptr<SecureComputePreparedMachineAdmission> prepared, std::shared_ptr<ExternalSecureExecutionAttempt> attempt, std::shared_ptr<ExternalSecureExecutionConsumptionReceipt> consumption, std::shared_ptr<IExternalSecureRegisterLocalExecutionProvider> provider);

		std::optional<std::shared_ptr<ExternalSecureRegisterLocalExecutionResult>> ObserveOriginal(std::shared_ptr<Pending> pending);

	private:
		void InvalidateSource(std::shared_ptr<Pending> pending);

		bool Matches(std::shared_ptr<SecureComputePreparedMachineAdmission> prepared, std::shared_ptr<ExternalSecureExecutionAttempt> attempt);

	public:
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal Acceptance? Accept(Pending pending, ExternalSecureRegisterLocalExecutionReceipt receipt)
		std::shared_ptr<Acceptance> Accept(std::shared_ptr<Pending> pending, std::shared_ptr<ExternalSecureRegisterLocalExecutionReceipt> receipt);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal SecureComputeRegisterLocalRetirement? Retire(Pending pending, Acceptance acceptance)
		std::shared_ptr<SecureComputeRegisterLocalRetirement> Retire(std::shared_ptr<Pending> pending, std::shared_ptr<Acceptance> acceptance);

		void Cancel(std::shared_ptr<Pending> pending);

		void PruneInvalidated();

		void InvalidateAfterRestore();
	};

}
