#pragma once

#include "../Telemetry/AcceleratorTelemetry.h"
#include "../Tokens/AcceleratorToken.h"
#include "../Tokens/AcceleratorTokenStatusWord.h"
#include "../Capabilities/AcceleratorCapabilityRegistry.h"
#include <string>
#include <deque>
#include <stdexcept>
#include <optional>
#include <format>
#include <memory>
#include "../../../../../tangible_string_helper.h"

namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Tokens { class AcceleratorTokenAdmissionResult; }

using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Auth;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Capabilities;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Descriptors;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Telemetry;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Tokens;

namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Queues
{
	class AcceleratorQueueAdmissionRequest;
	class AcceleratorQueueAdmissionResult;
	class AcceleratorQueuedCommand;

	class IAcceleratorDevice
	{
	public:
		virtual std::wstring getDeviceId() const = 0;

		virtual bool isAvailable() const = 0;
	};

	class IAcceleratorCommandQueue
	{
	public:
		virtual int getCapacity() const = 0;

		virtual int getCount() const = 0;

		virtual std::shared_ptr<IAcceleratorDevice> getDevice() const = 0;

		virtual int getQueueFullRejectCount() const = 0;

		virtual int getDeviceBusyRejectCount() const = 0;

		virtual int getModelRejectCount() const = 0;

		virtual std::shared_ptr<AcceleratorQueueAdmissionResult> TryEnqueue(std::shared_ptr<AcceleratorQueueAdmissionRequest> request, std::optional<std::shared_ptr<AcceleratorGuardEvidence>> &currentGuardEvidence) = 0;

		virtual bool TryDequeueReady(std::optional<std::shared_ptr<AcceleratorGuardEvidence>> &currentGuardEvidence, std::optional<std::shared_ptr<AcceleratorQueuedCommand>> &command, std::shared_ptr<AcceleratorQueueAdmissionResult> &result) = 0;
	};

	class AcceleratorDevice final : public IAcceleratorDevice
	{
	private:
		const std::wstring DeviceId;
		bool IsAvailable = false;

	public:
		AcceleratorDevice(const std::wstring &deviceId, bool isAvailable = true);

		std::wstring getDeviceId() const override;

		bool isAvailable() const override;
		void setAvailable(bool value);
	};

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public sealed record AcceleratorQueueAdmissionRequest
//	{
//		public required AcceleratorCommandDescriptor Descriptor
//		{
//			get;
//			set;
//		}
//
//		public required AcceleratorCapabilityAcceptanceResult CapabilityAcceptance
//		{
//			get;
//			set;
//		}
//
//		public required AcceleratorTokenAdmissionResult TokenAdmission
//		{
//			get;
//			set;
//		}
//
//		// Placeholder evidence for this explicit queue model only; not a global
//		// CPU load/store conflict-manager acceptance signal.
//		public bool ConflictAccepted
//		{
//			get;
//			set;
//		} = true;
//
//		public string ConflictEvidenceMessage
//		{
//			get;
//			set;
//		} = "Phase 07 carries explicit placeholder conflict acceptance evidence; Phase 10 conflict manager is not implemented.";
//	}

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public sealed record AcceleratorQueuedCommand
//	{
//		internal AcceleratorQueuedCommand(ulong queueSequence, AcceleratorQueueAdmissionRequest request, AcceleratorToken token, AcceleratorGuardDecision queueGuardDecision)
//		{
//			QueueSequence = queueSequence;
//			Descriptor = request.Descriptor;
//			CapabilityAcceptance = request.CapabilityAcceptance;
//			TokenAdmission = request.TokenAdmission;
//			Token = token;
//			QueueGuardDecision = queueGuardDecision;
//			ConflictAccepted = request.ConflictAccepted;
//			ConflictEvidenceMessage = request.ConflictEvidenceMessage;
//		}
//
//		public ulong QueueSequence
//		{
//			get;
//		}
//
//		public AcceleratorCommandDescriptor Descriptor
//		{
//			get;
//		}
//
//		public AcceleratorCapabilityAcceptanceResult CapabilityAcceptance
//		{
//			get;
//		}
//
//		public AcceleratorTokenAdmissionResult TokenAdmission
//		{
//			get;
//		}
//
//		public AcceleratorToken Token
//		{
//			get;
//		}
//
//		public AcceleratorGuardDecision QueueGuardDecision
//		{
//			get;
//		}
//
//		public bool ConflictAccepted
//		{
//			get;
//		}
//
//		public string ConflictEvidenceMessage
//		{
//			get;
//		}
//
//		public bool CanPublishArchitecturalMemory
//		{
//			get
//			{
//				return false;
//			}
//		}
//
//		public bool CanPublishException
//		{
//			get
//			{
//				return false;
//			}
//		}
//	}

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public sealed record AcceleratorQueueAdmissionResult
//	{
//		private AcceleratorQueueAdmissionResult(bool isAccepted, System.Nullable<AcceleratorQueuedCommand> command, AcceleratorTokenFaultCode faultCode, System.Nullable<AcceleratorGuardDecision> guardDecision, int queueFullRejectCount, int deviceBusyRejectCount, int modelRejectCount, string message)
//		{
//			IsAccepted = isAccepted;
//			Command = command;
//			FaultCode = faultCode;
//			GuardDecision = guardDecision;
//			QueueFullRejectCount = queueFullRejectCount;
//			DeviceBusyRejectCount = deviceBusyRejectCount;
//			ModelRejectCount = modelRejectCount;
//			Message = message;
//		}
//
//		public bool IsAccepted
//		{
//			get;
//		}
//
//		public bool IsRejected
//		{
//			get
//			{
//				return !IsAccepted;
//			}
//		}
//
//		public System.Nullable<AcceleratorQueuedCommand> Command
//		{
//			get;
//		}
//
//		public AcceleratorTokenFaultCode FaultCode
//		{
//			get;
//		}
//
//		public System.Nullable<AcceleratorGuardDecision> GuardDecision
//		{
//			get;
//		}
//
//		public int QueueFullRejectCount
//		{
//			get;
//		}
//
//		public int DeviceBusyRejectCount
//		{
//			get;
//		}
//
//		public int ModelRejectCount
//		{
//			get;
//		}
//
//		public string Message
//		{
//			get;
//		}
//
//		public bool CanPublishArchitecturalMemory
//		{
//			get
//			{
//				return false;
//			}
//		}
//
//		public bool CanPublishException
//		{
//			get
//			{
//				return false;
//			}
//		}
//
//		public bool UserVisiblePublicationAllowed
//		{
//			get
//			{
//				return false;
//			}
//		}
//
//		public static AcceleratorQueueAdmissionResult Accepted(AcceleratorQueuedCommand command, AcceleratorGuardDecision guardDecision, int queueFullRejectCount, int deviceBusyRejectCount, int modelRejectCount)
//		{
//			ArgumentNullException.ThrowIfNull(command);
//			return new AcceleratorQueueAdmissionResult(isAccepted: true, command, AcceleratorTokenFaultCode.None, guardDecision, queueFullRejectCount, deviceBusyRejectCount, modelRejectCount, "L7-SDC queue admission accepted after descriptor, capability, token, owner/domain, epoch, capacity, and device availability checks.");
//		}
//
//		public static AcceleratorQueueAdmissionResult Reject(AcceleratorTokenFaultCode faultCode, System.Nullable<AcceleratorGuardDecision> guardDecision, int queueFullRejectCount, int deviceBusyRejectCount, int modelRejectCount, string message)
//		{
//			if (faultCode == AcceleratorTokenFaultCode.None)
//			{
//				throw new ArgumentException("Rejected L7-SDC queue admission requires a fault code.", nameof(faultCode));
//			}
//
//			return new AcceleratorQueueAdmissionResult(isAccepted: false, command: nullptr, faultCode, guardDecision, queueFullRejectCount, deviceBusyRejectCount, modelRejectCount, message);
//		}
//	}

	class AcceleratorQueueAdmissionRequest final {
	public:
		std::shared_ptr<AcceleratorCommandDescriptor> Descriptor;
		std::shared_ptr<AcceleratorCapabilityAcceptanceResult> CapabilityAcceptance;
		std::shared_ptr<AcceleratorTokenAdmissionResult> TokenAdmission;
		bool ConflictAccepted = true;
		std::wstring ConflictEvidenceMessage = L"Phase 07 carries explicit placeholder conflict acceptance evidence; Phase 10 conflict manager is not implemented.";
	};
	class AcceleratorQueuedCommand final {
	public:
		const unsigned long long QueueSequence;
		const std::shared_ptr<AcceleratorCommandDescriptor> Descriptor;
		const std::shared_ptr<AcceleratorCapabilityAcceptanceResult> CapabilityAcceptance;
		const std::shared_ptr<AcceleratorTokenAdmissionResult> TokenAdmission;
		const std::shared_ptr<AcceleratorToken> Token;
		const std::shared_ptr<AcceleratorGuardDecision> QueueGuardDecision;
		const bool ConflictAccepted;
		const std::wstring ConflictEvidenceMessage;
		AcceleratorQueuedCommand(unsigned long long sequence, const std::shared_ptr<AcceleratorQueueAdmissionRequest>& request, std::shared_ptr<AcceleratorToken> token, std::shared_ptr<AcceleratorGuardDecision> guard)
			: QueueSequence(sequence), Descriptor(request->Descriptor), CapabilityAcceptance(request->CapabilityAcceptance), TokenAdmission(request->TokenAdmission), Token(std::move(token)), QueueGuardDecision(std::move(guard)), ConflictAccepted(request->ConflictAccepted), ConflictEvidenceMessage(request->ConflictEvidenceMessage) {}
		bool CanPublishArchitecturalMemory() const noexcept { return false; }
		bool CanPublishException() const noexcept { return false; }
	};
	class AcceleratorQueueAdmissionResult final {
	public:
		const bool IsAccepted;
		const std::shared_ptr<AcceleratorQueuedCommand> Command;
		const AcceleratorTokenFaultCode FaultCode;
		const std::shared_ptr<AcceleratorGuardDecision> GuardDecision;
		const int QueueFullRejectCount, DeviceBusyRejectCount, ModelRejectCount;
		const std::wstring Message;
	private:
		AcceleratorQueueAdmissionResult(bool accepted, std::shared_ptr<AcceleratorQueuedCommand> command, AcceleratorTokenFaultCode fault, std::shared_ptr<AcceleratorGuardDecision> guard, int full, int busy, int model, std::wstring message)
			: IsAccepted(accepted), Command(std::move(command)), FaultCode(fault), GuardDecision(std::move(guard)), QueueFullRejectCount(full), DeviceBusyRejectCount(busy), ModelRejectCount(model), Message(std::move(message)) {}
	public:
		bool IsRejected() const noexcept { return !IsAccepted; }
		bool CanPublishArchitecturalMemory() const noexcept { return false; }
		bool CanPublishException() const noexcept { return false; }
		bool UserVisiblePublicationAllowed() const noexcept { return false; }
		static std::shared_ptr<AcceleratorQueueAdmissionResult> Accepted(std::shared_ptr<AcceleratorQueuedCommand> command, std::shared_ptr<AcceleratorGuardDecision> guard, int full, int busy, int model) {
			if (!command) throw std::invalid_argument("command");
			return std::shared_ptr<AcceleratorQueueAdmissionResult>(new AcceleratorQueueAdmissionResult(true, std::move(command), AcceleratorTokenFaultCode::None, std::move(guard), full, busy, model, L"L7-SDC queue admission accepted after descriptor, capability, token, owner/domain, epoch, capacity, and device availability checks."));
		}
		static std::shared_ptr<AcceleratorQueueAdmissionResult> Reject(AcceleratorTokenFaultCode fault, std::shared_ptr<AcceleratorGuardDecision> guard, int full, int busy, int model, const std::wstring& message) {
			if (fault == AcceleratorTokenFaultCode::None) throw std::invalid_argument("Rejected L7-SDC queue admission requires a fault code.");
			return std::shared_ptr<AcceleratorQueueAdmissionResult>(new AcceleratorQueueAdmissionResult(false, nullptr, fault, std::move(guard), full, busy, model, message));
		}
	};

	class AcceleratorCommandQueue final : public IAcceleratorCommandQueue
	{
	private:
		const int Capacity;
		const std::shared_ptr<IAcceleratorDevice> Device;
		int QueueFullRejectCount = 0;
		int DeviceBusyRejectCount = 0;
		int ModelRejectCount = 0;

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly Queue<AcceleratorQueuedCommand> _commands = new();
		std::deque<std::shared_ptr<AcceleratorQueuedCommand>> _commands = std::deque<std::shared_ptr<AcceleratorQueuedCommand>>();
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private readonly AcceleratorTelemetry? _telemetry;
		const std::shared_ptr<AcceleratorTelemetry> _telemetry;
		unsigned long long _nextQueueSequence = 1;

	public:
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public AcceleratorCommandQueue(int capacity, IAcceleratorDevice device, AcceleratorTelemetry? telemetry = null)
		AcceleratorCommandQueue(int capacity, std::shared_ptr<IAcceleratorDevice> device, std::shared_ptr<AcceleratorTelemetry> telemetry = nullptr);

		int getCapacity() const override;

		int getCount() const override;

		std::shared_ptr<IAcceleratorDevice> getDevice() const override;

		int getQueueFullRejectCount() const override;
	private:
		void setQueueFullRejectCount(int value);

	public:
		int getDeviceBusyRejectCount() const override;
	private:
		void setDeviceBusyRejectCount(int value);

	public:
		int getModelRejectCount() const override;
	private:
		void setModelRejectCount(int value);

	public:
		std::shared_ptr<AcceleratorQueueAdmissionResult> TryEnqueue(std::shared_ptr<AcceleratorQueueAdmissionRequest> request, std::optional<std::shared_ptr<AcceleratorGuardEvidence>> &currentGuardEvidence) override;

		bool TryDequeueReady(std::optional<std::shared_ptr<AcceleratorGuardEvidence>> &currentGuardEvidence, std::optional<std::shared_ptr<AcceleratorQueuedCommand>> &command, std::shared_ptr<AcceleratorQueueAdmissionResult> &result) override;

	private:
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private bool TryValidateAdmissionRequest(AcceleratorQueueAdmissionRequest request, System.Nullable<AcceleratorGuardEvidence> currentGuardEvidence, out AcceleratorToken? token, out AcceleratorGuardDecision guardDecision, out System.Nullable<AcceleratorQueueAdmissionResult> reject)
		bool TryValidateAdmissionRequest(std::shared_ptr<AcceleratorQueueAdmissionRequest> request, std::optional<std::shared_ptr<AcceleratorGuardEvidence>> &currentGuardEvidence, std::shared_ptr<AcceleratorToken> &token, std::shared_ptr<AcceleratorGuardDecision> &guardDecision, std::optional<std::shared_ptr<AcceleratorQueueAdmissionResult>> &reject);

		std::shared_ptr<AcceleratorQueueAdmissionResult> Reject(AcceleratorTokenFaultCode faultCode, std::optional<std::shared_ptr<AcceleratorGuardDecision>> guardDecision, const std::wstring &message);

		unsigned long long AllocateQueueSequence();

		static bool IsCapabilityGuardBacked(std::shared_ptr<AcceleratorCapabilityAcceptanceResult> capabilityAcceptance, std::shared_ptr<AcceleratorCommandDescriptor> descriptor, std::wstring &message);
	};

}
