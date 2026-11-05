#include "AcceleratorCommandQueue.h"
#include "../../../Pipeline/Safety/SafetyVerifier.Types.h"
#include "../Auth/AcceleratorOwnerDomainGuard.h"
#include "../Tokens/AcceleratorTokenStore.h"

using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Auth;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Capabilities;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Descriptors;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Telemetry;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Tokens;

namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Queues
{

	AcceleratorDevice::AcceleratorDevice(const std::wstring &deviceId, bool isAvailable) : DeviceId(deviceId), IsAvailable(isAvailable)
	{
		if (StringHelper::isEmptyOrWhiteSpace(deviceId))
		{
// C# TO C++ CONVERTER TASK: This exception's constructor requires only one argument:
// ORIGINAL LINE: throw new ArgumentException("Device id is required.", nameof(deviceId));
			throw std::invalid_argument("Device id is required.");
		}


	}

	std::wstring AcceleratorDevice::getDeviceId() const
	{
		return DeviceId;
	}

	bool AcceleratorDevice::isAvailable() const
	{
		return IsAvailable;
	}

	void AcceleratorDevice::setAvailable(bool value)
	{
		IsAvailable = value;
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public AcceleratorCommandQueue(int capacity, IAcceleratorDevice device, AcceleratorTelemetry? telemetry = null)
	AcceleratorCommandQueue::AcceleratorCommandQueue(int capacity, std::shared_ptr<IAcceleratorDevice> device, std::shared_ptr<AcceleratorTelemetry> telemetry) : Capacity(capacity), Device(device), _telemetry(telemetry)
	{
		if (capacity <= 0)
		{
			throw std::out_of_range("L7-SDC accelerator command queue capacity must be positive.");
		}

		if (!device) throw std::invalid_argument("device");
	}

	int AcceleratorCommandQueue::getCapacity() const
	{
		return Capacity;
	}

	int AcceleratorCommandQueue::getCount() const
	{
		return _commands.size();
	}

	std::shared_ptr<IAcceleratorDevice> AcceleratorCommandQueue::getDevice() const
	{
		return Device;
	}

	int AcceleratorCommandQueue::getQueueFullRejectCount() const
	{
		return QueueFullRejectCount;
	}

	void AcceleratorCommandQueue::setQueueFullRejectCount(int value)
	{
		QueueFullRejectCount = value;
	}

	int AcceleratorCommandQueue::getDeviceBusyRejectCount() const
	{
		return DeviceBusyRejectCount;
	}

	void AcceleratorCommandQueue::setDeviceBusyRejectCount(int value)
	{
		DeviceBusyRejectCount = value;
	}

	int AcceleratorCommandQueue::getModelRejectCount() const
	{
		return ModelRejectCount;
	}

	void AcceleratorCommandQueue::setModelRejectCount(int value)
	{
		ModelRejectCount = value;
	}

	std::shared_ptr<AcceleratorQueueAdmissionResult> AcceleratorCommandQueue::TryEnqueue(std::shared_ptr<AcceleratorQueueAdmissionRequest> request, std::optional<std::shared_ptr<AcceleratorGuardEvidence>> &currentGuardEvidence)
	{
		if (!request) throw std::invalid_argument("request");

		std::shared_ptr<AcceleratorToken> token;
		std::shared_ptr<AcceleratorGuardDecision> guardDecision;
		std::optional<std::shared_ptr<AcceleratorQueueAdmissionResult>> reject;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: if (!TryValidateAdmissionRequest(request, currentGuardEvidence, out AcceleratorToken? token, out AcceleratorGuardDecision guardDecision, out System.Nullable<AcceleratorQueueAdmissionResult> reject))
		if (!TryValidateAdmissionRequest(request, currentGuardEvidence, token, guardDecision, reject))
		{
	// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
	// ORIGINAL LINE: return reject!;
			return reject.value();
		}

		if (!getDevice()->isAvailable())
		{
			setDeviceBusyRejectCount(getDeviceBusyRejectCount() + 1);
			std::shared_ptr<AcceleratorQueueAdmissionResult> deviceBusy = Reject(AcceleratorTokenFaultCode::DeviceBusy, guardDecision, std::format(L"L7-SDC queue admission rejected because device '{}' is busy after guard-backed admission checks.", getDevice()->getDeviceId()));
			if (_telemetry != nullptr)
			{
				_telemetry->RecordDeviceBusyReject(deviceBusy->FaultCode, deviceBusy->Message);
			}
			return deviceBusy;
		}

		if (_commands.size() >= getCapacity())
		{
			setQueueFullRejectCount(getQueueFullRejectCount() + 1);
			std::shared_ptr<AcceleratorQueueAdmissionResult> queueFull = Reject(AcceleratorTokenFaultCode::QueueFull, guardDecision, L"L7-SDC queue admission rejected because the accelerator command queue is full after guard-backed admission checks.");
			if (_telemetry != nullptr)
			{
				_telemetry->RecordQueueFullReject(queueFull->FaultCode, queueFull->Message);
			}
			return queueFull;
		}

	// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
	// ORIGINAL LINE: if (token!.State == AcceleratorTokenState.Created)
		if (token->getState() == AcceleratorTokenState::Created)
		{
	// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
	// ORIGINAL LINE: AcceleratorTokenTransition validated = token.MarkValidated(currentGuardEvidence!);
			std::shared_ptr<AcceleratorTokenTransition> validated = token->MarkValidated(currentGuardEvidence.value_or(nullptr));
			if (validated->Rejected)
			{
				return Reject(validated->FaultCode, guardDecision, validated->Message);
			}
		}

		if (token->getState() != AcceleratorTokenState::Validated)
		{
			return Reject(AcceleratorTokenFaultCode::QueueAdmissionRejected, guardDecision, std::format(L"L7-SDC queue admission requires token Created or Validated, but token is {}.", static_cast<int>(token->getState())));
		}

	// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
	// ORIGINAL LINE: AcceleratorTokenTransition queued = token.MarkQueued(currentGuardEvidence!);
		std::shared_ptr<AcceleratorTokenTransition> queued = token->MarkQueued(currentGuardEvidence.value_or(nullptr));
		if (queued->Rejected)
		{
			return Reject(queued->FaultCode, guardDecision, queued->Message);
		}

		std::shared_ptr<AcceleratorQueuedCommand> queuedCommand = std::make_shared<AcceleratorQueuedCommand>(AllocateQueueSequence(), request, token, guardDecision);
		_commands.push_back(queuedCommand);
		return AcceleratorQueueAdmissionResult::Accepted(queuedCommand, guardDecision, getQueueFullRejectCount(), getDeviceBusyRejectCount(), getModelRejectCount());
	}

	bool AcceleratorCommandQueue::TryDequeueReady(std::optional<std::shared_ptr<AcceleratorGuardEvidence>> &currentGuardEvidence, std::optional<std::shared_ptr<AcceleratorQueuedCommand>> &command, std::shared_ptr<AcceleratorQueueAdmissionResult> &result)
	{
		command = std::nullopt;
		if (_commands.empty())
		{
			result = Reject(AcceleratorTokenFaultCode::BackendRejected, std::nullopt, L"L7-SDC queue has no ready command.");
			return false;
		}

		std::shared_ptr<AcceleratorQueuedCommand> next = _commands.front();
		std::shared_ptr<AcceleratorGuardDecision> guardDecision = AcceleratorOwnerDomainGuard::getDefault()->ValidateMappingEpoch(next->Token->getSubmitGuardDecision(), currentGuardEvidence);
		if (!guardDecision->IsAllowed)
		{
			result = Reject(AcceleratorTokenStore::MapGuardFault(guardDecision->Fault), guardDecision, L"L7-SDC queue dequeue requires fresh owner/domain and epoch authority. " + guardDecision->Message);
			return false;
		}

		command = next;
		_commands.pop_front();
		result = AcceleratorQueueAdmissionResult::Accepted(command.value(), guardDecision, getQueueFullRejectCount(), getDeviceBusyRejectCount(), getModelRejectCount());
		return true;
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private bool TryValidateAdmissionRequest(AcceleratorQueueAdmissionRequest request, System.Nullable<AcceleratorGuardEvidence> currentGuardEvidence, out AcceleratorToken? token, out AcceleratorGuardDecision guardDecision, out System.Nullable<AcceleratorQueueAdmissionResult> reject)
	bool AcceleratorCommandQueue::TryValidateAdmissionRequest(std::shared_ptr<AcceleratorQueueAdmissionRequest> request, std::optional<std::shared_ptr<AcceleratorGuardEvidence>> &currentGuardEvidence, std::shared_ptr<AcceleratorToken> &token, std::shared_ptr<AcceleratorGuardDecision> &guardDecision, std::optional<std::shared_ptr<AcceleratorQueueAdmissionResult>> &reject)
	{
		token = nullptr;
		guardDecision = nullptr;
		reject = std::nullopt;

		std::wstring descriptorMessage;
		if (!AcceleratorOwnerDomainGuard::getDefault()->IsDescriptorGuardBacked(request->Descriptor, descriptorMessage))
		{
			reject = Reject(AcceleratorTokenFaultCode::DescriptorNotGuardBacked, request->Descriptor->OwnerGuardDecision, descriptorMessage);
			return false;
		}

		std::wstring capabilityMessage;
		if (!IsCapabilityGuardBacked(request->CapabilityAcceptance, request->Descriptor, capabilityMessage))
		{
			reject = Reject(request->CapabilityAcceptance->isAccepted() ? AcceleratorTokenFaultCode::CapabilityNotAccepted : AcceleratorTokenFaultCode::CapabilityRejected, request->CapabilityAcceptance->getGuardDecision(), capabilityMessage);
			return false;
		}

		if (!request->TokenAdmission->IsAccepted || request->TokenAdmission->Token == nullptr)
		{
			reject = Reject(request->TokenAdmission->FaultCode == AcceleratorTokenFaultCode::None ? AcceleratorTokenFaultCode::QueueAdmissionRejected : request->TokenAdmission->FaultCode, request->TokenAdmission->GuardDecision, L"L7-SDC queue admission requires an accepted Phase 06 token admission result.");
			return false;
		}

		token = request->TokenAdmission->Token;
		if (token->getDescriptor() != request->Descriptor && !token->getDescriptor()->Equals(request->Descriptor))
		{
			reject = Reject(AcceleratorTokenFaultCode::QueueAdmissionRejected, token->getSubmitGuardDecision(), L"L7-SDC queue admission token descriptor does not match the guarded descriptor evidence.");
			return false;
		}

		if (token->getCapabilityAcceptance() != request->CapabilityAcceptance)
		{
			reject = Reject(AcceleratorTokenFaultCode::QueueAdmissionRejected, token->getSubmitGuardDecision(), L"L7-SDC queue admission token capability evidence does not match the accepted capability evidence.");
			return false;
		}

		if (!request->ConflictAccepted)
		{
			reject = Reject(AcceleratorTokenFaultCode::ConflictRejected, token->getSubmitGuardDecision(), StringHelper::isEmptyOrWhiteSpace(request->ConflictEvidenceMessage) ? L"L7-SDC Phase 07 placeholder conflict evidence rejected admission." : request->ConflictEvidenceMessage);
			return false;
		}

		guardDecision = AcceleratorOwnerDomainGuard::getDefault()->EnsureBeforeSubmit(request->Descriptor, currentGuardEvidence);
		if (!guardDecision->IsAllowed)
		{
			reject = Reject(AcceleratorTokenStore::MapGuardFault(guardDecision->Fault), guardDecision, L"L7-SDC queue admission requires fresh guarded submit authority. " + guardDecision->Message);
			return false;
		}

		std::shared_ptr<AcceleratorGuardDecision> tokenGuardDecision = AcceleratorOwnerDomainGuard::getDefault()->ValidateMappingEpoch(token->getSubmitGuardDecision(), currentGuardEvidence);
		if (!tokenGuardDecision->IsAllowed)
		{
			reject = Reject(AcceleratorTokenStore::MapGuardFault(tokenGuardDecision->Fault), tokenGuardDecision, L"L7-SDC queue admission requires token-bound owner/domain and epoch revalidation. " + tokenGuardDecision->Message);
			return false;
		}

		return true;
	}

	std::shared_ptr<AcceleratorQueueAdmissionResult> AcceleratorCommandQueue::Reject(AcceleratorTokenFaultCode faultCode, std::optional<std::shared_ptr<AcceleratorGuardDecision>> guardDecision, const std::wstring &message)
	{
		setModelRejectCount(getModelRejectCount() + 1);
		if (_telemetry != nullptr)
		{
			_telemetry->RecordGuardReject(guardDecision, message);
		}
		return AcceleratorQueueAdmissionResult::Reject(faultCode, guardDecision.value_or(nullptr), getQueueFullRejectCount(), getDeviceBusyRejectCount(), getModelRejectCount(), message);
	}

	unsigned long long AcceleratorCommandQueue::AllocateQueueSequence()
	{
		unsigned long long sequence = _nextQueueSequence++;
		if (_nextQueueSequence == 0)
		{
			_nextQueueSequence = 1;
		}

		return sequence;
	}

	bool AcceleratorCommandQueue::IsCapabilityGuardBacked(std::shared_ptr<AcceleratorCapabilityAcceptanceResult> capabilityAcceptance, std::shared_ptr<AcceleratorCommandDescriptor> descriptor, std::wstring &message)
	{
		if (!capabilityAcceptance->isAccepted())
		{
			message = StringHelper::isEmptyOrWhiteSpace(capabilityAcceptance->getRejectReason()) ? L"L7-SDC capability acceptance rejected before queue admission." : capabilityAcceptance->getRejectReason();
			return false;
		}

		std::shared_ptr<AcceleratorGuardDecision> guardDecision = capabilityAcceptance->getGuardDecision();
		if (!guardDecision->IsAllowed || (!guardDecision->Evidence || guardDecision->Evidence->Source != AcceleratorGuardEvidenceSource::GuardPlane) || guardDecision->LegalityDecision.getAuthoritySource() != LegalityAuthoritySource::GuardPlane || guardDecision->LegalityDecision.getAttemptedReplayCertificateReuse())
		{
			message = L"L7-SDC queue admission requires guard-backed capability acceptance; registry metadata is not authority.";
			return false;
		}

		if (guardDecision->DescriptorOwnerBinding == nullptr || !guardDecision->DescriptorOwnerBinding->Equals(descriptor->OwnerBinding))
		{
			message = L"L7-SDC queue admission capability guard does not match descriptor owner binding.";
			return false;
		}

		message = L"L7-SDC capability acceptance is guard-backed for queue admission.";
		return true;
	}
}
