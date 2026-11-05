#include "YAKSys_Hybrid_CPU.Core.SecureCompletionPublicationFence.h"

namespace YAKSys_Hybrid_CPU::Core
{

	SecureCompletionPublicationFence::SecureCompletionPublicationFence() : SecureCompletionPublicationFence(SecureCompletionFenceState::Missing, SecureRetirePublicationRule::Denied)
	{
	}

	SecureCompletionPublicationFence::SecureCompletionPublicationFence(SecureCompletionFenceState state, SecureRetirePublicationRule retireRule)
		: State(state), RetireRule(retireRule)
	{
	}

	std::shared_ptr<SecureCompletionPublicationFence> SecureCompletionPublicationFence::getDenied()
	{
		return Denied;
	}

	SecureCompletionFenceState SecureCompletionPublicationFence::getState() const
	{
		return State;
	}

	SecureRetirePublicationRule SecureCompletionPublicationFence::getRetireRule() const
	{
		return RetireRule;
	}

	bool SecureCompletionPublicationFence::getCanPublishCompletion() const
	{
		return ([&] { const auto codexPatternValue = getState(); return codexPatternValue == SecureCompletionFenceState::CompletionAllowed || codexPatternValue == SecureCompletionFenceState::RetireAllowed; }());
	}

	bool SecureCompletionPublicationFence::getCanPublishRetire() const
	{
		return getState() == SecureCompletionFenceState::RetireAllowed && getRetireRule() == SecureRetirePublicationRule::ExplicitRetireFenceRequired;
	}
}
