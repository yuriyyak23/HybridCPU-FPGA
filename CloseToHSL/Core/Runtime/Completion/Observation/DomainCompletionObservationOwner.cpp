#include "DomainCompletionObservationOwner.h"
#include "../Commit/ArchitecturalCompletionCommitOwner.h"

namespace YAKSys_Hybrid_CPU::Core
{

	unsigned long long CompletionGenerationAuthority::getCurrent() const
	{
		{
		std::scoped_lock<std::mutex> lock(_gate);
			return _current;
		}
	}

	unsigned long long CompletionGenerationAuthority::Advance()
	{
		{
			std::scoped_lock<std::mutex> lock(_gate);
//			unchecked
				_current++;
				if (_current == 0)
				{
					_current = 1;
				}
// C# TO C++ CONVERTER TASK: End of the original C# 'unchecked' block.
			return _current;
		}
	}

	DomainCompletionObservationOwner::CommitInstaller::CommitInstaller(std::shared_ptr<DomainCompletionObservationOwner> owner, std::shared_ptr<ArchitecturalCompletionCommitOwner> issuer) : _owner(owner), _issuer(issuer)
	{
	}

	bool DomainCompletionObservationOwner::CommitInstaller::Matches(std::shared_ptr<DomainCompletionObservationOwner> owner, std::shared_ptr<ArchitecturalCompletionCommitOwner> issuer)
	{
		return _owner == owner && _issuer == issuer;
	}

// C# TO C++ CONVERTER TASK: Throw expressions are not converted by C# to C++ Converter:
// ORIGINAL LINE: internal DomainCompletionObservationOwner(CompletionGenerationAuthority generationAuthority)
	DomainCompletionObservationOwner::DomainCompletionObservationOwner(std::shared_ptr<CompletionGenerationAuthority> generationAuthority) : _generationAuthority((generationAuthority != nullptr) ? generationAuthority : throw std::invalid_argument("generationAuthority"))
	{
	}

	CompletionObservationMigrationClass DomainCompletionObservationOwner::getMigrationClass() const
	{
		return CompletionObservationMigrationClass::RecomputedCompletion;
	}

	unsigned long long DomainCompletionObservationOwner::getCurrentGeneration() const
	{
		return _generationAuthority->getCurrent();
	}

	std::shared_ptr<DomainCompletionObservationOwner::CommitInstaller> DomainCompletionObservationOwner::RegisterCommitIssuer(std::shared_ptr<ArchitecturalCompletionCommitOwner> issuer)
	{
		ArgumentNullException::ThrowIfNull(issuer);
		{
			std::scoped_lock<std::mutex> lock(_gate);
			if (!_isActive || _commitIssuer != nullptr)
			{
				throw InvalidOperationException(L"Completion observation accepts exactly one live commit issuer.");
			}
			_commitIssuer = issuer;
			_liveInstaller = std::make_shared<CommitInstaller>(shared_from_this(), issuer);
			return _liveInstaller;
		}
	}

	void DomainCompletionObservationOwner::InstallCommittedCompletion(std::shared_ptr<CommitInstaller> installer, std::shared_ptr<ArchitecturalCompletionCommitOwner> issuer, std::shared_ptr<ArchitecturalCompletionReceiptBinding> binding, std::shared_ptr<NeutralArchitecturalCompletionFacts> facts)
	{
		{
			std::scoped_lock<std::mutex> lock(_gate);
			if (!_isActive || _liveInstaller == nullptr || _liveInstaller != installer || !installer->Matches(shared_from_this(), issuer) || _commitIssuer != issuer)
			{
				throw InvalidOperationException(L"Completion observation installation requires its exact live commit issuer capability.");
			}

			std::shared_ptr<CompletionObservationScope> scope = std::make_shared<CompletionObservationScope>(binding->DomainId, binding->ContextId, binding->VirtualThreadId);
			if (!scope->IsValid || binding->CompletionIdentity == 0 || binding->CommitSequence == 0 || binding->RestoreGeneration == 0 || facts->CompletionClass != binding->CompletionClass)
			{
				throw InvalidOperationException(L"Committed completion binding is not valid for neutral observation.");
			}

			unsigned long long generation = _generationAuthority->Advance();
			_snapshots[std::make_tuple(scope->DomainId, scope->ContextId, scope->VirtualThreadId)] = std::make_shared<NeutralCompletionObservationSnapshot>(scope, generation, binding->CompletionIdentity, binding->ProducerOwnerIdentity, binding->ProducerOwnerEpoch, binding->AttemptId, binding->EventId, binding->CompletionClass, binding->CompletionDigest, binding->CanonicalOrderSequence, binding->CommitSequence, binding->RestoreGeneration, facts, binding->TranslationProvenance);
		}
	}

	std::shared_ptr<CompletionObservationResult> DomainCompletionObservationOwner::Observe(std::shared_ptr<CompletionObservationScope> scope)
	{
		if (!scope || !scope->IsValid)
		{
			return std::make_shared<CompletionObservationResult>(CompletionObservationDecision::DeniedInvalidScope, nullptr);
		}

		{
			std::scoped_lock<std::mutex> lock(_gate);
			if (!_isActive)
			{
				return std::make_shared<CompletionObservationResult>(CompletionObservationDecision::DeniedInactiveOwner, nullptr);
			}
			const auto snapshot = _snapshots.find(std::make_tuple(scope->DomainId, scope->ContextId, scope->VirtualThreadId));
			if (snapshot == _snapshots.end() || !snapshot->second)
			{
				return std::make_shared<CompletionObservationResult>(CompletionObservationDecision::DeniedAbsent, nullptr);
			}
			return std::make_shared<CompletionObservationResult>(CompletionObservationDecision::Observed, snapshot->second);
		}
	}

	std::shared_ptr<NeutralCompletionFieldResult> DomainCompletionObservationOwner::ReadField(std::shared_ptr<CompletionObservationScope> scope, NeutralCompletionObservationField field)
	{
		std::shared_ptr<CompletionObservationResult> observation = Observe(scope);
		if (!observation || !observation->IsObserved || !observation->Snapshot || !observation->Snapshot.value())
		{
			const NeutralCompletionFieldDecision denied = observation && observation->Decision == CompletionObservationDecision::DeniedInactiveOwner
				? NeutralCompletionFieldDecision::DeniedInactiveOwner
				: observation && observation->Decision == CompletionObservationDecision::DeniedAbsent
				? NeutralCompletionFieldDecision::DeniedAbsent
				: NeutralCompletionFieldDecision::DeniedScope;
			return std::make_shared<NeutralCompletionFieldResult>(denied, 0);
		}

		const auto &facts = observation->Snapshot.value()->Facts;
		switch (field)
		{
		case NeutralCompletionObservationField::Reason:
			return facts->Reason.IsPresent
				? std::make_shared<NeutralCompletionFieldResult>(NeutralCompletionFieldDecision::Present, facts->Reason.Value)
				: std::make_shared<NeutralCompletionFieldResult>(NeutralCompletionFieldDecision::DeniedAbsent, 0);
		case NeutralCompletionObservationField::Qualification:
			return facts->Qualification.IsPresent
				? std::make_shared<NeutralCompletionFieldResult>(NeutralCompletionFieldDecision::Present, facts->Qualification.Value)
				: std::make_shared<NeutralCompletionFieldResult>(NeutralCompletionFieldDecision::DeniedAbsent, 0);
		case NeutralCompletionObservationField::GuestPhysicalAddress:
			if (!facts->FaultAddress.IsPresent) return std::make_shared<NeutralCompletionFieldResult>(NeutralCompletionFieldDecision::DeniedAbsent, 0);
			return facts->FaultAddress.Semantic == NeutralFaultAddressSemantic::GuestPhysicalAddress
				? std::make_shared<NeutralCompletionFieldResult>(NeutralCompletionFieldDecision::Present, facts->FaultAddress.Value)
				: std::make_shared<NeutralCompletionFieldResult>(NeutralCompletionFieldDecision::DeniedSemanticMismatch, 0);
		case NeutralCompletionObservationField::SecondStageTranslationViolationAuxiliary:
			if (!facts->FaultAuxiliary.IsPresent) return std::make_shared<NeutralCompletionFieldResult>(NeutralCompletionFieldDecision::DeniedAbsent, 0);
			return facts->FaultAuxiliary.Semantic == NeutralFaultAuxiliarySemantic::SecondStageTranslationViolation
				? std::make_shared<NeutralCompletionFieldResult>(NeutralCompletionFieldDecision::Present, facts->FaultAuxiliary.Value)
				: std::make_shared<NeutralCompletionFieldResult>(NeutralCompletionFieldDecision::DeniedSemanticMismatch, 0);
		default:
			return std::make_shared<NeutralCompletionFieldResult>(NeutralCompletionFieldDecision::DeniedSemanticMismatch, 0);
		}
	}

	void DomainCompletionObservationOwner::Clear(std::shared_ptr<CompletionObservationScope> scope)
	{
		if (!scope->IsValid)
		{
			throw ArgumentOutOfRangeException(L"scope");
		}
		{
			std::scoped_lock<std::mutex> lock(_gate);
			EnsureActive();
			_snapshots.erase(std::make_tuple(scope->DomainId, scope->ContextId, scope->VirtualThreadId));
			_generationAuthority->Advance();
		}
	}

	void DomainCompletionObservationOwner::Rebind(std::shared_ptr<CompletionObservationScope> previousScope)
	{
		Clear(previousScope);
	}

	void DomainCompletionObservationOwner::ClearAfterRestore()
	{
		{
			std::scoped_lock<std::mutex> lock(_gate);
			EnsureActive();
			_snapshots.clear();
			_generationAuthority->Advance();
		}
	}

	std::shared_ptr<DomainCompletionObservationOwner> DomainCompletionObservationOwner::ReplaceOwner(std::shared_ptr<ArchitecturalCompletionCommitOwner> issuer)
	{
		{
			std::scoped_lock<std::mutex> lock(_gate);
			EnsureActive();
			if (_commitIssuer != issuer)
			{
				throw InvalidOperationException(L"Only the exact commit issuer may replace observation ownership.");
			}
			_snapshots.clear();
			_isActive = false;
			_liveInstaller = nullptr;
			_generationAuthority->Advance();
		}

		return std::make_shared<DomainCompletionObservationOwner>(_generationAuthority);
	}

	void DomainCompletionObservationOwner::EnsureActive()
	{
		if (!_isActive)
		{
			throw InvalidOperationException(L"Completion observation owner was replaced.");
		}
	}

	std::shared_ptr<NeutralCompletionFieldResult> DomainCompletionObservationOwner::ScalarResult(const NeutralScalarFact &fact)
	{
		return fact.IsPresent
			? std::make_shared<NeutralCompletionFieldResult>(NeutralCompletionFieldDecision::Present, fact.Value)
			: std::make_shared<NeutralCompletionFieldResult>(NeutralCompletionFieldDecision::DeniedAbsent, 0);
	}

	std::shared_ptr<NeutralCompletionFieldResult> DomainCompletionObservationOwner::AddressResult(const NeutralAddressFact &fact, NeutralFaultAddressSemantic requiredSemantic)
	{
		if (!fact.IsPresent)
		{
			return std::make_shared<NeutralCompletionFieldResult>(NeutralCompletionFieldDecision::DeniedAbsent, 0);
		}
		return fact.Semantic == requiredSemantic
			? std::make_shared<NeutralCompletionFieldResult>(NeutralCompletionFieldDecision::Present, fact.Value)
			: std::make_shared<NeutralCompletionFieldResult>(NeutralCompletionFieldDecision::DeniedSemanticMismatch, 0);
	}

	std::shared_ptr<NeutralCompletionFieldResult> DomainCompletionObservationOwner::AuxiliaryResult(const NeutralAuxiliaryFact &fact, NeutralFaultAuxiliarySemantic requiredSemantic)
	{
		if (!fact.IsPresent)
		{
			return std::make_shared<NeutralCompletionFieldResult>(NeutralCompletionFieldDecision::DeniedAbsent, 0);
		}
		return fact.Semantic == requiredSemantic
			? std::make_shared<NeutralCompletionFieldResult>(NeutralCompletionFieldDecision::Present, fact.Value)
			: std::make_shared<NeutralCompletionFieldResult>(NeutralCompletionFieldDecision::DeniedSemanticMismatch, 0);
	}
}
