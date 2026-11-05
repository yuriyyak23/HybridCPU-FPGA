#include "AcceleratorExceptionPublication.h"
#include "../Auth/AcceleratorOwnerDomainGuard.h"
#include "AcceleratorTokenStore.h"

using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Auth;

namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Tokens
{

	std::shared_ptr<AcceleratorFaultPublicationResult> AcceleratorExceptionPublication::TryPublish(std::shared_ptr<AcceleratorToken> token, AcceleratorTokenFaultCode faultCode, std::optional<std::shared_ptr<AcceleratorGuardEvidence>> &currentGuardEvidence, bool recordPrivilegedDiagnostic)
	{
		ArgumentNullException::ThrowIfNull(token);
		if (faultCode == AcceleratorTokenFaultCode::None)
		{
			return AcceleratorFaultPublicationResult::Rejected(token, AcceleratorTokenFaultCode::FaultPublicationRejected, nullptr, L"L7-SDC fault publication requires a non-None token fault code.");
		}

		std::shared_ptr<AcceleratorGuardDecision> tokenGuardDecision = AcceleratorOwnerDomainGuard::getDefault()->ValidateMappingEpoch(token->getSubmitGuardDecision(), currentGuardEvidence);
		if (!tokenGuardDecision->IsAllowed)
		{
			return AcceleratorFaultPublicationResult::Blocked(token, AcceleratorTokenStore::MapGuardFault(tokenGuardDecision->Fault), tokenGuardDecision, recordPrivilegedDiagnostic, L"L7-SDC fault publication blocked user-visible status because token owner/domain or epochs are no longer valid. " + tokenGuardDecision->Message);
		}

		std::shared_ptr<AcceleratorGuardDecision> publicationGuardDecision = AcceleratorOwnerDomainGuard::getDefault()->EnsureBeforeExceptionPublication(token->getDescriptor(), currentGuardEvidence);
		if (!publicationGuardDecision->IsAllowed)
		{
			return AcceleratorFaultPublicationResult::Blocked(token, AcceleratorTokenStore::MapGuardFault(publicationGuardDecision->Fault), publicationGuardDecision, recordPrivilegedDiagnostic, L"L7-SDC fault publication blocked user-visible status because exception publication authority was rejected. " + publicationGuardDecision->Message);
		}

		if (tokenGuardDecision->DescriptorOwnerBinding == nullptr || publicationGuardDecision->DescriptorOwnerBinding == nullptr || !tokenGuardDecision->DescriptorOwnerBinding->Equals(publicationGuardDecision->DescriptorOwnerBinding))
		{
			return AcceleratorFaultPublicationResult::Rejected(token, AcceleratorTokenFaultCode::FaultPublicationRejected, publicationGuardDecision, L"L7-SDC fault publication token guard and exception guard bind different owners.");
		}

		if (token->getState() == AcceleratorTokenState::Faulted)
		{
			return AcceleratorFaultPublicationResult::Published(token, token->getFaultCode() == AcceleratorTokenFaultCode::None ? faultCode : token->getFaultCode(), publicationGuardDecision, L"L7-SDC fault status was published only after fresh exception-publication guard authority.");
		}

		if (token->isTerminal())
		{
			return AcceleratorFaultPublicationResult::Rejected(token, AcceleratorTokenFaultCode::TerminalState, publicationGuardDecision, std::format(L"L7-SDC fault publication cannot rewrite terminal token state {}.", static_cast<int>(token->getState())));
		}

	// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
	// ORIGINAL LINE: AcceleratorTokenTransition fault = token.MarkFaulted( faultCode, currentGuardEvidence!);
		std::shared_ptr<AcceleratorTokenTransition> fault = token->MarkFaulted(faultCode, *currentGuardEvidence);
		return fault->Succeeded ? AcceleratorFaultPublicationResult::Published(token, faultCode, publicationGuardDecision, L"L7-SDC fault was marked and published only after fresh owner/domain, mapping, and exception-publication guard authority.") : AcceleratorFaultPublicationResult::Rejected(token, fault->FaultCode, publicationGuardDecision, fault->Message);
	}
}
