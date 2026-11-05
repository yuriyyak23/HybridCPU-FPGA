#include "SecureComputePlacedIntent.h"

using namespace HybridCPU::ExternalRuntime::Contracts;
using namespace YAKSys_Hybrid_CPU::Core::Decoder;

namespace YAKSys_Hybrid_CPU::Core
{

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal SecureComputePlacedIntent(ExternalSecureRegisterLocalRequest request, ScheduledOperation scheduledOperation, PostStageBIssuedAttempt? issuedAttempt = null)
	SecureComputePlacedIntent::SecureComputePlacedIntent(std::shared_ptr<ExternalSecureRegisterLocalRequest> request, std::shared_ptr<YAKSys_Hybrid_CPU::Core::Decoder::ScheduledOperation> scheduledOperation, std::shared_ptr<PostStageBIssuedAttempt> issuedAttempt) : Request(request), ScheduledOperation(scheduledOperation), IssuedAttempt(issuedAttempt) {}

	std::shared_ptr<ExternalSecureRegisterLocalRequest> SecureComputePlacedIntent::getRequest() const
	{
		return Request;
	}

	std::shared_ptr<YAKSys_Hybrid_CPU::Core::Decoder::ScheduledOperation> SecureComputePlacedIntent::getScheduledOperation() const
	{
		return ScheduledOperation;
	}

	std::shared_ptr<PostStageBIssuedAttempt> SecureComputePlacedIntent::getIssuedAttempt() const
	{
		return IssuedAttempt;
	}
}
