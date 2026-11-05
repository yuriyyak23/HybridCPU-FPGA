#pragma once

namespace HybridCPU::ExternalRuntime::Contracts { class ExternalSecureRegisterLocalRequest; }
namespace YAKSys_Hybrid_CPU::Core::Decoder { class ScheduledOperation; }
namespace YAKSys_Hybrid_CPU::Core { class PostStageBIssuedAttempt; }
#include <memory>

using namespace HybridCPU::ExternalRuntime::Contracts;
using namespace YAKSys_Hybrid_CPU::Core::Decoder;

namespace YAKSys_Hybrid_CPU::Core
{

	/// <summary>
	/// Immutable correlation for a standalone software placement attempt. Created
	/// only after the existing typed scheduler selected a lane. This is neither a
	/// current core certificate nor a permit, execution receipt, or commit record.
	/// No mutable MicroOp or architectural sink escapes the admission composition.
	/// </summary>
	class SecureComputePlacedIntent final : public std::enable_shared_from_this<SecureComputePlacedIntent>
	{
	private:
		const std::shared_ptr<ExternalSecureRegisterLocalRequest> Request;
		const std::shared_ptr<YAKSys_Hybrid_CPU::Core::Decoder::ScheduledOperation> ScheduledOperation;
		const std::shared_ptr<PostStageBIssuedAttempt> IssuedAttempt;

	public:
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal SecureComputePlacedIntent(ExternalSecureRegisterLocalRequest request, ScheduledOperation scheduledOperation, PostStageBIssuedAttempt? issuedAttempt = null)
		SecureComputePlacedIntent(std::shared_ptr<ExternalSecureRegisterLocalRequest> request, std::shared_ptr<YAKSys_Hybrid_CPU::Core::Decoder::ScheduledOperation> scheduledOperation, std::shared_ptr<PostStageBIssuedAttempt> issuedAttempt = nullptr);

		std::shared_ptr<ExternalSecureRegisterLocalRequest> getRequest() const;
		std::shared_ptr<YAKSys_Hybrid_CPU::Core::Decoder::ScheduledOperation> getScheduledOperation() const;
		// The live execution carrier remains private to machine owners. Public
		// placement observations must not expose mutable ExecutionRecord state.
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal PostStageBIssuedAttempt? IssuedAttempt {get;}
		std::shared_ptr<PostStageBIssuedAttempt> getIssuedAttempt() const;
	};

}
