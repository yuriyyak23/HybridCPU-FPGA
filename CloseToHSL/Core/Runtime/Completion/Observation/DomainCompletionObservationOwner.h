#pragma once


#include "../Commit/NeutralArchitecturalCompletionFacts.h"
#include <map>
#include <tuple>
#include <stdexcept>
#include <mutex>
#include <optional>
#include <string>
#include <utility>
#include "tangible_exception_helper.h"
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{
	class ArchitecturalCompletionCommitOwner;
	struct ArchitecturalCompletionReceiptBinding;
	struct CompletionObservationScope;
	struct NeutralCompletionObservationSnapshot;
	struct CompletionObservationResult;
	struct NeutralCompletionFieldResult;

	enum class CompletionObservationMigrationClass : unsigned char
	{
		RecomputedCompletion = 1
	};

	enum class CompletionObservationDecision : unsigned char
	{
		Observed = 0,
		DeniedInvalidScope = 1,
		DeniedAbsent = 2,
		DeniedInactiveOwner = 3
	};

	enum class NeutralCompletionObservationField : unsigned char
	{
		Reason = 0,
		Qualification = 1,
		GuestPhysicalAddress = 2,
		SecondStageTranslationViolationAuxiliary = 3
	};

	enum class NeutralCompletionFieldDecision : unsigned char
	{
		Present = 0,
		DeniedAbsent = 1,
		DeniedSemanticMismatch = 2,
		DeniedScope = 3,
		DeniedInactiveOwner = 4
	};

	struct CompletionObservationScope final
	{
		const unsigned long long DomainId;
		const int ContextId;
		const int VirtualThreadId;
		const bool IsValid;

		CompletionObservationScope(unsigned long long domainId, int contextId, int virtualThreadId)
			: DomainId(domainId), ContextId(contextId), VirtualThreadId(virtualThreadId),
		  IsValid(domainId != 0 && contextId > 0 && virtualThreadId >= 0 && virtualThreadId < 4)
		{
		}
	};

	struct NeutralCompletionObservationSnapshot final
	{
		const std::shared_ptr<CompletionObservationScope> Scope;
		const unsigned long long CompletionGeneration, CompletionIdentity, ProducerOwnerIdentity, ProducerOwnerEpoch;
		const unsigned long long AttemptId, EventId;
		const NeutralArchitecturalCompletionClass CompletionClass;
		const std::wstring CompletionDigest;
		const unsigned long long CanonicalOrderSequence, CommitSequence, RestoreGeneration;
		const std::shared_ptr<NeutralArchitecturalCompletionFacts> Facts;
		const NeutralTranslationProvenance TranslationProvenance;

		NeutralCompletionObservationSnapshot(std::shared_ptr<CompletionObservationScope> scope,
			unsigned long long completionGeneration, unsigned long long completionIdentity,
			unsigned long long producerOwnerIdentity, unsigned long long producerOwnerEpoch,
			unsigned long long attemptId, unsigned long long eventId,
			NeutralArchitecturalCompletionClass completionClass, std::wstring completionDigest,
			unsigned long long canonicalOrderSequence, unsigned long long commitSequence,
			unsigned long long restoreGeneration, std::shared_ptr<NeutralArchitecturalCompletionFacts> facts,
			NeutralTranslationProvenance translationProvenance = {})
			: Scope(std::move(scope)), CompletionGeneration(completionGeneration), CompletionIdentity(completionIdentity),
			  ProducerOwnerIdentity(producerOwnerIdentity), ProducerOwnerEpoch(producerOwnerEpoch), AttemptId(attemptId),
			  EventId(eventId), CompletionClass(completionClass), CompletionDigest(std::move(completionDigest)),
			  CanonicalOrderSequence(canonicalOrderSequence), CommitSequence(commitSequence), RestoreGeneration(restoreGeneration),
			  Facts(std::move(facts)), TranslationProvenance(translationProvenance)
		{
		}
	};

	struct CompletionObservationResult final
	{
		const CompletionObservationDecision Decision;
		const std::optional<std::shared_ptr<NeutralCompletionObservationSnapshot>> Snapshot;
		const bool IsObserved;

		CompletionObservationResult(CompletionObservationDecision decision,
			std::shared_ptr<NeutralCompletionObservationSnapshot> snapshot)
			: Decision(decision), Snapshot(snapshot ? std::optional<std::shared_ptr<NeutralCompletionObservationSnapshot>>(std::move(snapshot)) : std::nullopt),
		  IsObserved(decision == CompletionObservationDecision::Observed && Snapshot.has_value() && Snapshot.value() != nullptr)
		{
		}
	};

	struct NeutralCompletionFieldResult final
	{
		const NeutralCompletionFieldDecision Decision;
		const unsigned long long Value;
		const bool IsPresent;

		NeutralCompletionFieldResult(NeutralCompletionFieldDecision decision, unsigned long long value)
			: Decision(decision), Value(value), IsPresent(decision == NeutralCompletionFieldDecision::Present)
		{
		}
	};

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	internal readonly record struct CompletionObservationScope(ulong DomainId, int ContextId, int VirtualThreadId)
//	{
//		internal bool IsValid
//		{
//			get
//			{
//				return DomainId != 0 && ContextId > 0 && VirtualThreadId is >= 0 @and < Processor.CPU_Core.SmtWays;
//			}
//		}
//	}

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	internal readonly record struct NeutralCompletionObservationSnapshot(CompletionObservationScope Scope, ulong CompletionGeneration, ulong CompletionIdentity, ulong ProducerOwnerIdentity, ulong ProducerOwnerEpoch, ulong AttemptId, ulong EventId, NeutralArchitecturalCompletionClass CompletionClass, string CompletionDigest, ulong CanonicalOrderSequence, ulong CommitSequence, ulong RestoreGeneration, NeutralArchitecturalCompletionFacts Facts, NeutralTranslationProvenance TranslationProvenance = default);

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	internal readonly record struct CompletionObservationResult(CompletionObservationDecision Decision, System.Nullable<NeutralCompletionObservationSnapshot> Snapshot)
//	{
//		internal bool IsObserved
//		{
//			get
//			{
//				return Decision == CompletionObservationDecision.Observed && Snapshot.HasValue;
//			}
//		}
//	}

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	internal readonly record struct NeutralCompletionFieldResult(NeutralCompletionFieldDecision Decision, ulong Value)
//	{
//		internal bool IsPresent
//		{
//			get
//			{
//				return Decision == NeutralCompletionFieldDecision.Present;
//			}
//		}
//	}

	class CompletionGenerationAuthority final : public std::enable_shared_from_this<CompletionGenerationAuthority>
	{
	private:
		mutable std::mutex _gate;
		unsigned long long _current = 1;

	public:
		unsigned long long getCurrent() const;

		unsigned long long Advance();
	};

	/// <summary>
	/// Neutral, runtime-local, read-only observation of completions that have
	/// crossed the architectural commit boundary. It stores no compatibility
	/// projection fields or precomputed projection values.
	/// </summary>
	class DomainCompletionObservationOwner final : public std::enable_shared_from_this<DomainCompletionObservationOwner>
	{
	public:
		class CommitInstaller final : public std::enable_shared_from_this<CommitInstaller>
		{
		private:
			const std::shared_ptr<DomainCompletionObservationOwner> _owner;
			const std::shared_ptr<ArchitecturalCompletionCommitOwner> _issuer;

		public:
			CommitInstaller(std::shared_ptr<DomainCompletionObservationOwner> owner, std::shared_ptr<ArchitecturalCompletionCommitOwner> issuer);

			bool Matches(std::shared_ptr<DomainCompletionObservationOwner> owner, std::shared_ptr<ArchitecturalCompletionCommitOwner> issuer);
		};

	private:
		std::mutex _gate;
		const std::shared_ptr<CompletionGenerationAuthority> _generationAuthority;
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly Dictionary<CompletionObservationScope, NeutralCompletionObservationSnapshot> _snapshots = new();
		std::map<std::tuple<unsigned long long, int, int>, std::shared_ptr<NeutralCompletionObservationSnapshot>> _snapshots;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private ArchitecturalCompletionCommitOwner? _commitIssuer;
		std::shared_ptr<ArchitecturalCompletionCommitOwner> _commitIssuer;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private CommitInstaller? _liveInstaller;
		std::shared_ptr<CommitInstaller> _liveInstaller;
		bool _isActive = true;

	public:
// C# TO C++ CONVERTER TASK: Throw expressions are not converted by C# to C++ Converter:
// ORIGINAL LINE: internal DomainCompletionObservationOwner(CompletionGenerationAuthority generationAuthority)
		DomainCompletionObservationOwner(std::shared_ptr<CompletionGenerationAuthority> generationAuthority);

		CompletionObservationMigrationClass getMigrationClass() const;

		unsigned long long getCurrentGeneration() const;

		std::shared_ptr<CommitInstaller> RegisterCommitIssuer(std::shared_ptr<ArchitecturalCompletionCommitOwner> issuer);

		void InstallCommittedCompletion(std::shared_ptr<CommitInstaller> installer, std::shared_ptr<ArchitecturalCompletionCommitOwner> issuer, std::shared_ptr<ArchitecturalCompletionReceiptBinding> binding, std::shared_ptr<NeutralArchitecturalCompletionFacts> facts);

		std::shared_ptr<CompletionObservationResult> Observe(std::shared_ptr<CompletionObservationScope> scope);

		std::shared_ptr<NeutralCompletionFieldResult> ReadField(std::shared_ptr<CompletionObservationScope> scope, NeutralCompletionObservationField field);

		void Clear(std::shared_ptr<CompletionObservationScope> scope);

		void Rebind(std::shared_ptr<CompletionObservationScope> previousScope);

		void ClearAfterRestore();

		std::shared_ptr<DomainCompletionObservationOwner> ReplaceOwner(std::shared_ptr<ArchitecturalCompletionCommitOwner> issuer);

	private:
		void EnsureActive();

		static std::shared_ptr<NeutralCompletionFieldResult> ScalarResult(const NeutralScalarFact &fact);

		static std::shared_ptr<NeutralCompletionFieldResult> AddressResult(const NeutralAddressFact &fact, NeutralFaultAddressSemantic requiredSemantic);

		static std::shared_ptr<NeutralCompletionFieldResult> AuxiliaryResult(const NeutralAuxiliaryFact &fact, NeutralFaultAuxiliarySemantic requiredSemantic);
	};

}
