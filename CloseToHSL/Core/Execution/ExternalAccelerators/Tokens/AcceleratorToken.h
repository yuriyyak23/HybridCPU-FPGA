#pragma once



#include "AcceleratorTokenStatusWord.h"
#include "AcceleratorTokenHandle.h"
#include <string>
#include <stdexcept>
#include <functional>
#include <format>
#include <memory>

namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Auth { struct AcceleratorMappingEpoch; struct AcceleratorIommuDomainEpoch; class AcceleratorGuardDecision; class AcceleratorGuardEvidence; }
namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Capabilities { class AcceleratorCapabilityAcceptanceResult; }
namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Descriptors { class AcceleratorCommandDescriptor; }
namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Telemetry { class AcceleratorTelemetry; }
namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Tokens { struct AcceleratorTokenStatusWord; class AcceleratorTokenTransition; }
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Auth;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Capabilities;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Descriptors;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Telemetry;

namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Tokens
{

	class AcceleratorToken final : public std::enable_shared_from_this<AcceleratorToken>
	{
	private:
		const unsigned long long TokenId;
		const std::shared_ptr<AcceleratorTokenHandle> Handle;
		const std::shared_ptr<AcceleratorCommandDescriptor> Descriptor;
		const std::shared_ptr<AcceleratorCapabilityAcceptanceResult> CapabilityAcceptance;
		const std::shared_ptr<AcceleratorGuardDecision> SubmitGuardDecision;
		const std::shared_ptr<AcceleratorTelemetry> Telemetry;
		AcceleratorTokenState State = static_cast<AcceleratorTokenState>(0);
		AcceleratorTokenFaultCode FaultCode = static_cast<AcceleratorTokenFaultCode>(0);
		unsigned int StatusSequence = 0;

	public:
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal AcceleratorToken(ulong tokenId, AcceleratorTokenHandle handle, AcceleratorCommandDescriptor descriptor, AcceleratorCapabilityAcceptanceResult capabilityAcceptance, AcceleratorGuardDecision submitGuardDecision, AcceleratorTelemetry? telemetry = null)
		AcceleratorToken(unsigned long long tokenId, std::shared_ptr<AcceleratorTokenHandle> handle, std::shared_ptr<AcceleratorCommandDescriptor> descriptor, std::shared_ptr<AcceleratorCapabilityAcceptanceResult> capabilityAcceptance, std::shared_ptr<AcceleratorGuardDecision> submitGuardDecision, std::shared_ptr<AcceleratorTelemetry> telemetry = nullptr);

		unsigned long long getTokenId() const;

		std::shared_ptr<AcceleratorTokenHandle> getHandle() const;

		std::shared_ptr<AcceleratorCommandDescriptor> getDescriptor() const;

		std::shared_ptr<AcceleratorCapabilityAcceptanceResult> getCapabilityAcceptance() const;

		std::shared_ptr<AcceleratorGuardDecision> getSubmitGuardDecision() const;

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal AcceleratorTelemetry? Telemetry {get;}
		std::shared_ptr<AcceleratorTelemetry> getTelemetry() const;

		AcceleratorTokenState getState() const;
	private:
		void setState(AcceleratorTokenState value);

	public:
		AcceleratorTokenFaultCode getFaultCode() const;
	private:
		void setFaultCode(AcceleratorTokenFaultCode value);

	public:
		unsigned int getStatusSequence() const;
	private:
		void setStatusSequence(unsigned int value);

	public:
		std::shared_ptr<AcceleratorMappingEpoch> getMappingEpoch() const;

		std::shared_ptr<AcceleratorIommuDomainEpoch> getIommuDomainEpoch() const;

		bool isTerminal() const;

		bool getHasBackendExecution() const;

		bool getHasQueueExecution() const;

		bool getHasStagedWrites() const;

		bool getHasArchitecturalCommit() const;

		bool getUserVisiblePublicationAllowed() const;

		std::shared_ptr<AcceleratorTokenStatusWord> GetStatusWord();

		std::shared_ptr<AcceleratorTokenTransition> MarkValidated(std::shared_ptr<AcceleratorGuardEvidence> currentGuardEvidence);

		std::shared_ptr<AcceleratorTokenTransition> MarkQueued(std::shared_ptr<AcceleratorGuardEvidence> currentGuardEvidence);

		std::shared_ptr<AcceleratorTokenTransition> MarkRunning(std::shared_ptr<AcceleratorGuardEvidence> currentGuardEvidence);

		std::shared_ptr<AcceleratorTokenTransition> MarkDeviceComplete(std::shared_ptr<AcceleratorGuardEvidence> currentGuardEvidence);

		std::shared_ptr<AcceleratorTokenTransition> MarkCommitPending(std::shared_ptr<AcceleratorGuardEvidence> currentGuardEvidence);

		std::shared_ptr<AcceleratorTokenTransition> MarkCommitted(std::shared_ptr<AcceleratorGuardEvidence> currentGuardEvidence);

		std::shared_ptr<AcceleratorTokenTransition> MarkCommitPendingFromCommitCoordinator(std::shared_ptr<AcceleratorGuardEvidence> currentGuardEvidence);

		std::shared_ptr<AcceleratorTokenTransition> MarkCommittedFromCommitCoordinator(std::shared_ptr<AcceleratorGuardEvidence> currentGuardEvidence);

		std::shared_ptr<AcceleratorTokenTransition> MarkFaulted(AcceleratorTokenFaultCode faultCode, std::shared_ptr<AcceleratorGuardEvidence> currentGuardEvidence);

		std::shared_ptr<AcceleratorTokenTransition> MarkCanceled(std::shared_ptr<AcceleratorGuardEvidence> currentGuardEvidence);

		std::shared_ptr<AcceleratorTokenTransition> MarkTimedOut(std::shared_ptr<AcceleratorGuardEvidence> currentGuardEvidence);

		std::shared_ptr<AcceleratorTokenTransition> MarkAbandoned(std::shared_ptr<AcceleratorGuardEvidence> currentGuardEvidence);

	private:
		std::shared_ptr<AcceleratorTokenTransition> TryTerminalTransition(AcceleratorTokenState targetState, AcceleratorTokenFaultCode terminalFaultCode, std::shared_ptr<AcceleratorGuardEvidence> currentGuardEvidence, const std::wstring &message);

		std::shared_ptr<AcceleratorTokenTransition> TryTransition(AcceleratorTokenState targetState, std::shared_ptr<AcceleratorGuardEvidence> currentGuardEvidence, std::function<bool(AcceleratorTokenState)> isLegalSource, const std::wstring &successMessage, AcceleratorTokenFaultCode targetFaultCode = AcceleratorTokenFaultCode::None);

		std::shared_ptr<AcceleratorTokenTransition> RejectGuardedTransition(AcceleratorTokenState targetState, std::shared_ptr<AcceleratorGuardDecision> guardDecision, const std::wstring &surfaceMessage);

		std::shared_ptr<AcceleratorGuardDecision> ValidateCurrentGuard(std::shared_ptr<AcceleratorGuardEvidence> currentGuardEvidence);

		void AdvanceStatusSequence();
	};

}
