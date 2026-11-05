#pragma once

#include "../Telemetry/AcceleratorTelemetry.h"
#include "../../../Pipeline/MicroOps/Lane7Accelerator/SystemDeviceCommandMicroOp.h"
#include <memory>
#include <string>
#include <utility>

using namespace YAKSys_Hybrid_CPU::Core;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Telemetry;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Tokens;

namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Fences
{

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public sealed record AcceleratorLane7PressureResult
//	{
//		private AcceleratorLane7PressureResult(bool accepted, SystemDeviceCommandKind commandKind, int submitPollCount, int throttleRejectCount, AcceleratorTokenFaultCode faultCode, bool branchOrSystemProgressPreserved, string message)
//		{
//			Accepted = accepted;
//			CommandKind = commandKind;
//			SubmitPollCount = submitPollCount;
//			ThrottleRejectCount = throttleRejectCount;
//			FaultCode = faultCode;
//			BranchOrSystemProgressPreserved = branchOrSystemProgressPreserved;
//			Message = message;
//		}
//
//		public bool Accepted
//		{
//			get;
//		}
//
//		public bool Rejected
//		{
//			get
//			{
//				return !Accepted;
//			}
//		}
//
//		public SystemDeviceCommandKind CommandKind
//		{
//			get;
//		}
//
//		public int SubmitPollCount
//		{
//			get;
//		}
//
//		public int ThrottleRejectCount
//		{
//			get;
//		}
//
//		public AcceleratorTokenFaultCode FaultCode
//		{
//			get;
//		}
//
//		public bool BranchOrSystemProgressPreserved
//		{
//			get;
//		}
//
//		public string Message
//		{
//			get;
//		}
//
//		public static AcceleratorLane7PressureResult Allow(SystemDeviceCommandKind commandKind, int submitPollCount, int throttleRejectCount, bool branchOrSystemProgressPreserved, string message)
//		{
//			return new(accepted: true, commandKind, submitPollCount, throttleRejectCount, AcceleratorTokenFaultCode.None, branchOrSystemProgressPreserved, message);
//		}
//
//		public static AcceleratorLane7PressureResult Reject(SystemDeviceCommandKind commandKind, int submitPollCount, int throttleRejectCount, string message)
//		{
//			return new(accepted: false, commandKind, submitPollCount, throttleRejectCount, AcceleratorTokenFaultCode.Lane7PressureRejected, branchOrSystemProgressPreserved: true, message);
//		}
//	}

	class AcceleratorLane7PressureResult final {
	public:
		const bool Accepted;
		const SystemDeviceCommandKind CommandKind;
		const int SubmitPollCount;
		const int ThrottleRejectCount;
		const AcceleratorTokenFaultCode FaultCode;
		const bool BranchOrSystemProgressPreserved;
		const std::wstring Message;
		bool Rejected() const noexcept { return !Accepted; }

	private:
		AcceleratorLane7PressureResult(bool accepted, SystemDeviceCommandKind commandKind, int submitPollCount, int throttleRejectCount, AcceleratorTokenFaultCode faultCode, bool branchOrSystemProgressPreserved, std::wstring message)
			: Accepted(accepted), CommandKind(commandKind), SubmitPollCount(submitPollCount), ThrottleRejectCount(throttleRejectCount), FaultCode(faultCode), BranchOrSystemProgressPreserved(branchOrSystemProgressPreserved), Message(std::move(message)) {}

	public:
		static std::shared_ptr<AcceleratorLane7PressureResult> Allow(SystemDeviceCommandKind commandKind, int submitPollCount, int throttleRejectCount, bool branchOrSystemProgressPreserved, const std::wstring& message) {
			return std::shared_ptr<AcceleratorLane7PressureResult>(new AcceleratorLane7PressureResult(true, commandKind, submitPollCount, throttleRejectCount, AcceleratorTokenFaultCode::None, branchOrSystemProgressPreserved, message));
		}
		static std::shared_ptr<AcceleratorLane7PressureResult> Reject(SystemDeviceCommandKind commandKind, int submitPollCount, int throttleRejectCount, const std::wstring& message) {
			return std::shared_ptr<AcceleratorLane7PressureResult>(new AcceleratorLane7PressureResult(false, commandKind, submitPollCount, throttleRejectCount, AcceleratorTokenFaultCode::Lane7PressureRejected, true, message));
		}
	};

	class AcceleratorLane7PressureThrottle final : public std::enable_shared_from_this<AcceleratorLane7PressureThrottle>
	{
	private:
		const int MaxSubmitPollPerWindow;
		int SubmitPollCount = 0;
		int ThrottleRejectCount = 0;

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private readonly AcceleratorTelemetry? _telemetry;
		const std::shared_ptr<AcceleratorTelemetry> _telemetry;

	public:
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public AcceleratorLane7PressureThrottle(int maxSubmitPollPerWindow, AcceleratorTelemetry? telemetry = null)
		AcceleratorLane7PressureThrottle(int maxSubmitPollPerWindow, std::shared_ptr<AcceleratorTelemetry> telemetry = nullptr);

		int getMaxSubmitPollPerWindow() const;

		int getSubmitPollCount() const;
	private:
		void setSubmitPollCount(int value);

	public:
		int getThrottleRejectCount() const;
	private:
		void setThrottleRejectCount(int value);

	public:
		std::shared_ptr<AcceleratorLane7PressureResult> TryAdmit(SystemDeviceCommandKind commandKind);

		std::shared_ptr<AcceleratorLane7PressureResult> RecordBranchOrSystemProgress();

		void ResetWindow();
	};

}
