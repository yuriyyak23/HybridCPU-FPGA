#include "AcceleratorLane7PressureModel.h"

using namespace YAKSys_Hybrid_CPU::Core;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Telemetry;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Tokens;

namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Fences
{

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public AcceleratorLane7PressureThrottle(int maxSubmitPollPerWindow, AcceleratorTelemetry? telemetry = null)
	AcceleratorLane7PressureThrottle::AcceleratorLane7PressureThrottle(int maxSubmitPollPerWindow, std::shared_ptr<AcceleratorTelemetry> telemetry) : MaxSubmitPollPerWindow(maxSubmitPollPerWindow), _telemetry(telemetry)
	{
		if (maxSubmitPollPerWindow <= 0)
		{
			throw ArgumentOutOfRangeException(L"maxSubmitPollPerWindow", maxSubmitPollPerWindow, L"L7-SDC lane7 pressure budget must be positive.");
		}
	}

	int AcceleratorLane7PressureThrottle::getMaxSubmitPollPerWindow() const
	{
		return MaxSubmitPollPerWindow;
	}

	int AcceleratorLane7PressureThrottle::getSubmitPollCount() const
	{
		return SubmitPollCount;
	}

	void AcceleratorLane7PressureThrottle::setSubmitPollCount(int value)
	{
		SubmitPollCount = value;
	}

	int AcceleratorLane7PressureThrottle::getThrottleRejectCount() const
	{
		return ThrottleRejectCount;
	}

	void AcceleratorLane7PressureThrottle::setThrottleRejectCount(int value)
	{
		ThrottleRejectCount = value;
	}

	std::shared_ptr<AcceleratorLane7PressureResult> AcceleratorLane7PressureThrottle::TryAdmit(SystemDeviceCommandKind commandKind)
	{
// C# TO C++ CONVERTER TASK: The following 'is' operator pattern is not converted by C# to C++ Converter:
		if ((commandKind != SystemDeviceCommandKind::Submit && commandKind != SystemDeviceCommandKind::Poll && commandKind != SystemDeviceCommandKind::Status))
		{
			return AcceleratorLane7PressureResult::Allow(commandKind, getSubmitPollCount(), getThrottleRejectCount(), true, L"Lane7 serializing/control progress is not consumed by the submit/poll storm budget.");
		}

		if (getSubmitPollCount() >= getMaxSubmitPollPerWindow())
		{
			setThrottleRejectCount(getThrottleRejectCount() + 1);
			std::shared_ptr<AcceleratorLane7PressureResult> result = AcceleratorLane7PressureResult::Reject(commandKind, getSubmitPollCount(), getThrottleRejectCount(), L"L7-SDC submit/poll/status storm exceeded the bounded lane7 pressure budget; command was rejected before authority-sensitive state changed.");
			if (_telemetry != nullptr)
			{
				_telemetry->RecordLane7SubmitPollThrottleReject(result->Message);
			}
			return result;
		}

		setSubmitPollCount(getSubmitPollCount() + 1);
		return AcceleratorLane7PressureResult::Allow(commandKind, getSubmitPollCount(), getThrottleRejectCount(), true, L"L7-SDC submit/poll/status command admitted within bounded lane7 pressure budget.");
	}

	std::shared_ptr<AcceleratorLane7PressureResult> AcceleratorLane7PressureThrottle::RecordBranchOrSystemProgress()
	{
		return AcceleratorLane7PressureResult::Allow(SystemDeviceCommandKind::Fence, getSubmitPollCount(), getThrottleRejectCount(), true, L"Lane7 branch/system progress evidence bypassed L7-SDC submit/poll storm throttling.");
	}

	void AcceleratorLane7PressureThrottle::ResetWindow()
	{
		setSubmitPollCount(0);
	}
}

