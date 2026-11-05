#include "AcceleratorFenceModel.h"
#include "../Tokens/AcceleratorTokenResults.h"
#include "../Tokens/AcceleratorObservationControlPolicies.h"
#include "../Tokens/AcceleratorTokenStatusWord.h"

using namespace HybridCPU::ExternalRuntime::Contracts;
using namespace YAKSys_Hybrid_CPU;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Auth;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Commit;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Conflicts;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Memory;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Tokens;

namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Fences
{

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private AcceleratorFenceScope(IReadOnlyList<AcceleratorTokenHandle> tokenHandles, bool commitCompletedTokens, bool commitConflictPlaceholderAccepted, AcceleratorFenceActiveTokenDisposition activeTokenDisposition, AcceleratorCancelPolicy activeCancelPolicy)
	AcceleratorFenceScope::AcceleratorFenceScope(const std::vector<std::shared_ptr<AcceleratorTokenHandle>> &tokenHandles, bool commitCompletedTokens, bool commitConflictPlaceholderAccepted, AcceleratorFenceActiveTokenDisposition activeTokenDisposition, std::shared_ptr<AcceleratorCancelPolicy> activeCancelPolicy) : TokenHandles(FreezeHandles(tokenHandles)), CommitCompletedTokens(commitCompletedTokens), CommitConflictPlaceholderAccepted(commitConflictPlaceholderAccepted), ActiveTokenDisposition(activeTokenDisposition), ActiveCancelPolicy(std::move(activeCancelPolicy)) {}

	const std::vector<std::shared_ptr<AcceleratorTokenHandle>> AcceleratorFenceScope::getTokenHandles() const
	{
		return TokenHandles;
	}

	bool AcceleratorFenceScope::getCommitCompletedTokens() const
	{
		return CommitCompletedTokens;
	}

	bool AcceleratorFenceScope::getCommitConflictPlaceholderAccepted() const
	{
		return CommitConflictPlaceholderAccepted;
	}

	AcceleratorFenceActiveTokenDisposition AcceleratorFenceScope::getActiveTokenDisposition() const
	{
		return ActiveTokenDisposition;
	}

	std::shared_ptr<AcceleratorCancelPolicy> AcceleratorFenceScope::getActiveCancelPolicy() const
	{
		return ActiveCancelPolicy;
	}

	std::shared_ptr<AcceleratorFenceScope> AcceleratorFenceScope::ForToken(std::shared_ptr<AcceleratorTokenHandle> tokenHandle, bool commitCompletedTokens, bool commitConflictPlaceholderAccepted, AcceleratorFenceActiveTokenDisposition activeTokenDisposition, std::optional<std::shared_ptr<AcceleratorCancelPolicy>> activeCancelPolicy)
	{
		return ForTokens({tokenHandle}, commitCompletedTokens, commitConflictPlaceholderAccepted, activeTokenDisposition, activeCancelPolicy);
	}

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: public static AcceleratorFenceScope ForTokens(IReadOnlyList<AcceleratorTokenHandle> tokenHandles, bool commitCompletedTokens = false, bool commitConflictPlaceholderAccepted = true, AcceleratorFenceActiveTokenDisposition activeTokenDisposition = AcceleratorFenceActiveTokenDisposition.Reject, System.Nullable<AcceleratorCancelPolicy> activeCancelPolicy = null)
	std::shared_ptr<AcceleratorFenceScope> AcceleratorFenceScope::ForTokens(const std::vector<std::shared_ptr<AcceleratorTokenHandle>> &tokenHandles, bool commitCompletedTokens, bool commitConflictPlaceholderAccepted, AcceleratorFenceActiveTokenDisposition activeTokenDisposition, std::optional<std::shared_ptr<AcceleratorCancelPolicy>> activeCancelPolicy)
	{
		const auto effectiveCancelPolicy = activeCancelPolicy.has_value()
			? activeCancelPolicy.value()
			: AcceleratorCancelPolicy::Cooperative;
		return std::shared_ptr<AcceleratorFenceScope>(new AcceleratorFenceScope(
			tokenHandles,
			commitCompletedTokens,
			commitConflictPlaceholderAccepted,
			activeTokenDisposition,
			effectiveCancelPolicy));
	}

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private static IReadOnlyList<AcceleratorTokenHandle> FreezeHandles(IReadOnlyList<AcceleratorTokenHandle> tokenHandles)
	const std::vector<std::shared_ptr<AcceleratorTokenHandle>> AcceleratorFenceScope::FreezeHandles(const std::vector<std::shared_ptr<AcceleratorTokenHandle>> &tokenHandles)
	{
		if (tokenHandles.empty())
		{
			return {};
		}

		return tokenHandles;
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public AcceleratorFenceResult TryFence(AcceleratorTokenStore tokenStore, AcceleratorFenceScope scope, System.Nullable<AcceleratorGuardEvidence> currentGuardEvidence, IAcceleratorStagingBuffer? stagingBuffer = null, System.Nullable<Processor.MainMemoryArea> mainMemory = null, System.Nullable<AcceleratorCommitCoordinator> commitCoordinator = null, System.Nullable<AcceleratorCommitInvalidationPlan> invalidationPlan = null, System.Nullable<ExternalAcceleratorConflictManager> conflictManager = null, System.Nullable<System.Func<AcceleratorToken, System.Nullable<ExternalOperationPublicationEvidence>>> externalPublicationEvidenceFactory = null)
	std::shared_ptr<AcceleratorFenceResult> AcceleratorFenceCoordinator::TryFence(std::shared_ptr<AcceleratorTokenStore> tokenStore, std::shared_ptr<AcceleratorFenceScope> scope, std::optional<std::shared_ptr<AcceleratorGuardEvidence>> &currentGuardEvidence, std::shared_ptr<IAcceleratorStagingBuffer> stagingBuffer, std::shared_ptr<Processor::MainMemoryArea> mainMemory, std::shared_ptr<AcceleratorCommitCoordinator> commitCoordinator, std::shared_ptr<AcceleratorCommitInvalidationPlan> invalidationPlan, std::shared_ptr<ExternalAcceleratorConflictManager> conflictManager, std::function<std::optional<std::shared_ptr<ExternalOperationPublicationEvidence>>(std::shared_ptr<AcceleratorToken>)> externalPublicationEvidenceFactory)
	{
		ArgumentNullException::ThrowIfNull(tokenStore);
		ArgumentNullException::ThrowIfNull(scope);

		std::vector<std::shared_ptr<AcceleratorTokenLookupResult>> tokenResults;
		std::vector<std::shared_ptr<AcceleratorCommitResult>> commitResults;
		commitCoordinator = (commitCoordinator != nullptr) ? commitCoordinator : std::make_shared<AcceleratorCommitCoordinator>();
		invalidationPlan = (invalidationPlan != nullptr) ? invalidationPlan : AcceleratorCommitInvalidationPlan::getNone();

		for (int index = 0; index < scope->getTokenHandles().size(); index++)
		{
			std::shared_ptr<AcceleratorTokenHandle> handle = scope->getTokenHandles()[index];
			std::shared_ptr<AcceleratorTokenLookupResult> lookup = tokenStore->TryLookup(handle, currentGuardEvidence, AcceleratorTokenLookupIntent::Fence);
			if (!lookup->IsAllowed || lookup->Token == nullptr)
			{
				tokenResults.push_back(lookup);
				continue;
			}

			std::shared_ptr<AcceleratorToken> token = lookup->Token;
			if (([&] { const auto codexPatternValue = token->getState(); return codexPatternValue == AcceleratorTokenState::DeviceComplete || codexPatternValue == AcceleratorTokenState::CommitPending; }()))
			{
				if (scope->getCommitCompletedTokens())
				{
					if (stagingBuffer == nullptr || mainMemory == nullptr)
					{
						tokenResults.push_back(AcceleratorTokenLookupResult::Rejected(AcceleratorTokenLookupIntent::Fence, token->getState(), AcceleratorTokenFaultCode::CommitCoordinatorRequired, L"ACCEL_FENCE requested commit of a completed token but no Phase 08 commit surfaces were supplied.", token, lookup->GuardDecision));
						continue;
					}

					std::optional<std::shared_ptr<ExternalOperationPublicationEvidence>> externalPublicationEvidence;
					const bool externalPublicationEvidenceRequired = static_cast<bool>(externalPublicationEvidenceFactory);
					if (externalPublicationEvidenceRequired)
					{
						externalPublicationEvidence = externalPublicationEvidenceFactory(token);
					}
					std::shared_ptr<AcceleratorCommitResult> commit = commitCoordinator->TryCommit(token, token->getDescriptor(), stagingBuffer, mainMemory, currentGuardEvidence, invalidationPlan, scope->getCommitConflictPlaceholderAccepted(), false, conflictManager, nullptr, externalPublicationEvidence, externalPublicationEvidenceRequired);
					commitResults.push_back(commit);
	// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
	// ORIGINAL LINE: tokenResults.Add( commit.Succeeded ? AcceleratorTokenLookupResult.Allowed( AcceleratorTokenLookupIntent.Fence, token, lookup.GuardDecision!.Value, "ACCEL_FENCE committed a completed token through the Phase 08 coordinator after fresh guard validation.") : AcceleratorTokenLookupResult.Rejected( AcceleratorTokenLookupIntent.Fence, token.State, commit.FaultCode == AcceleratorTokenFaultCode.None ? AcceleratorTokenFaultCode.FenceRejected : commit.FaultCode, commit.Message, token, lookup.GuardDecision));
					tokenResults.push_back(commit->Succeeded ? AcceleratorTokenLookupResult::Allowed(AcceleratorTokenLookupIntent::Fence, token, lookup->GuardDecision, L"ACCEL_FENCE committed a completed token through the Phase 08 coordinator after fresh guard validation.") : AcceleratorTokenLookupResult::Rejected(AcceleratorTokenLookupIntent::Fence, token->getState(), commit->getFaultCode() == AcceleratorTokenFaultCode::None ? AcceleratorTokenFaultCode::FenceRejected : commit->getFaultCode(), commit->Message, token, lookup->GuardDecision));
					continue;
				}

	// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
	// ORIGINAL LINE: tokenResults.Add( AcceleratorTokenLookupResult.Allowed( AcceleratorTokenLookupIntent.Fence, token, lookup.GuardDecision!.Value, "ACCEL_FENCE observed a completed token without commit publication by scope policy."));
				tokenResults.push_back(AcceleratorTokenLookupResult::Allowed(AcceleratorTokenLookupIntent::Fence, token, lookup->GuardDecision, L"ACCEL_FENCE observed a completed token without commit publication by scope policy."));
				continue;
			}

			if (([&] { const auto codexPatternValue = token->getState(); return codexPatternValue == AcceleratorTokenState::Created || codexPatternValue == AcceleratorTokenState::Validated || codexPatternValue == AcceleratorTokenState::Queued || codexPatternValue == AcceleratorTokenState::Running; }()))
			{
				tokenResults.push_back(ResolveActiveToken(tokenStore, token, handle, lookup, scope, currentGuardEvidence, conflictManager));
				continue;
			}

	// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
	// ORIGINAL LINE: tokenResults.Add( AcceleratorTokenLookupResult.Allowed( AcceleratorTokenLookupIntent.Fence, token, lookup.GuardDecision!.Value, "ACCEL_FENCE observed a terminal scoped token after fresh guard validation."));
			tokenResults.push_back(AcceleratorTokenLookupResult::Allowed(AcceleratorTokenLookupIntent::Fence, token, lookup->GuardDecision, L"ACCEL_FENCE observed a terminal scoped token after fresh guard validation."));
		}

		std::wstring message = CountRejected(tokenResults) + CountRejected(commitResults) == 0 ? L"ACCEL_FENCE serialized the scoped token set; any architectural memory publication used only the Phase 08 commit coordinator." : L"ACCEL_FENCE rejected at least one scoped token conservatively; no rejected token gained commit authority.";
		return AcceleratorFenceResult::Complete(tokenResults, commitResults, message);
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private static AcceleratorTokenLookupResult ResolveActiveToken(AcceleratorTokenStore tokenStore, AcceleratorToken token, AcceleratorTokenHandle handle, AcceleratorTokenLookupResult lookup, AcceleratorFenceScope scope, System.Nullable<AcceleratorGuardEvidence> currentGuardEvidence, ExternalAcceleratorConflictManager? conflictManager)
	std::shared_ptr<AcceleratorTokenLookupResult> AcceleratorFenceCoordinator::ResolveActiveToken(std::shared_ptr<AcceleratorTokenStore> tokenStore, std::shared_ptr<AcceleratorToken> token, std::shared_ptr<AcceleratorTokenHandle> handle, std::shared_ptr<AcceleratorTokenLookupResult> lookup, std::shared_ptr<AcceleratorFenceScope> scope, std::optional<std::shared_ptr<AcceleratorGuardEvidence>> &currentGuardEvidence, std::shared_ptr<ExternalAcceleratorConflictManager> conflictManager)
	{
		if (scope->getActiveTokenDisposition() == AcceleratorFenceActiveTokenDisposition::Reject)
		{
			if (conflictManager != nullptr)
			{
				std::shared_ptr<AcceleratorConflictDecision> boundary = conflictManager->NotifySerializingBoundary(token, currentGuardEvidence);
				return AcceleratorTokenLookupResult::Rejected(AcceleratorTokenLookupIntent::Fence, token->getState(), boundary->TokenFaultCode == AcceleratorTokenFaultCode::None ? AcceleratorTokenFaultCode::FenceRejected : boundary->TokenFaultCode, boundary->Message, token, (boundary->GuardDecision != nullptr) ? boundary->GuardDecision : lookup->GuardDecision);
			}

			return AcceleratorTokenLookupResult::Rejected(AcceleratorTokenLookupIntent::Fence, token->getState(), AcceleratorTokenFaultCode::FenceRejected, L"ACCEL_FENCE conservatively rejected an active scoped token; Phase 10 overlap tracking is not implemented.", token, lookup->GuardDecision);
		}

		if (scope->getActiveTokenDisposition() == AcceleratorFenceActiveTokenDisposition::Cancel)
		{
			std::shared_ptr<AcceleratorTokenLookupResult> cancel = tokenStore->TryCancel(handle, currentGuardEvidence, scope->getActiveCancelPolicy());
			if (cancel->IsAllowed && token->isTerminal())
			{
				if (conflictManager != nullptr)
				{
					conflictManager->ReleaseTokenFootprint(token, currentGuardEvidence);
				}
			}

			return cancel;
		}

	// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
	// ORIGINAL LINE: AcceleratorTokenTransition fault = token.MarkFaulted( AcceleratorTokenFaultCode.FenceRejected, currentGuardEvidence!);
		std::shared_ptr<AcceleratorTokenTransition> fault = token->MarkFaulted(AcceleratorTokenFaultCode::FenceRejected, *currentGuardEvidence);
		if (fault->Succeeded)
		{
			if (conflictManager != nullptr)
			{
				conflictManager->ReleaseTokenFootprint(token, currentGuardEvidence);
			}
		}

	// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
	// ORIGINAL LINE: return fault.Succeeded ? AcceleratorTokenLookupResult.AllowedWithStatus( AcceleratorTokenLookupIntent.Fence, token, lookup.GuardDecision!.Value, AcceleratorTokenStatusWord.FromToken(token), AcceleratorTokenFaultCode.FenceRejected, true, "ACCEL_FENCE faulted an active scoped token by explicit guarded policy; no commit authority was granted.") : AcceleratorTokenLookupResult.Rejected( AcceleratorTokenLookupIntent.Fence, token.State, fault.FaultCode, fault.Message, token, lookup.GuardDecision);
		return fault->Succeeded ? AcceleratorTokenLookupResult::AllowedWithStatus(AcceleratorTokenLookupIntent::Fence, token, lookup->GuardDecision, AcceleratorTokenStatusWord::FromToken(*token), AcceleratorTokenFaultCode::FenceRejected, true, L"ACCEL_FENCE faulted an active scoped token by explicit guarded policy; no commit authority was granted.") : AcceleratorTokenLookupResult::Rejected(AcceleratorTokenLookupIntent::Fence, token->getState(), fault->FaultCode, fault->Message, token, lookup->GuardDecision);
	}

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private static int CountRejected(IReadOnlyList<AcceleratorTokenLookupResult> results)
	int AcceleratorFenceCoordinator::CountRejected(const std::vector<std::shared_ptr<AcceleratorTokenLookupResult>> &results)
	{
		int count = 0;
		for (int index = 0; index < results.size(); index++)
		{
			if (results[index]->IsRejected)
			{
				count++;
			}
		}

		return count;
	}

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private static int CountRejected(IReadOnlyList<AcceleratorCommitResult> results)
	int AcceleratorFenceCoordinator::CountRejected(const std::vector<std::shared_ptr<AcceleratorCommitResult>> &results)
	{
		int count = 0;
		for (int index = 0; index < results.size(); index++)
		{
			if (results[index]->IsRejected)
			{
				count++;
			}
		}

		return count;
	}
}

