#pragma once

#include <memory>

namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Tokens
{

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct AcceleratorWaitPolicy(bool TimeoutExpired, bool MarkTokenTimedOutOnTimeout, ulong TimeoutTicks)
//	{
//		public static AcceleratorWaitPolicy ObserveOnly
//		{
//			get;
//		} = new(TimeoutExpired: false, MarkTokenTimedOutOnTimeout: false, TimeoutTicks: 0);
//
//		public static AcceleratorWaitPolicy TimedOutNoStateChange(ulong timeoutTicks = 0)
//		{
//			return new(TimeoutExpired: true, MarkTokenTimedOutOnTimeout: false, TimeoutTicks: timeoutTicks);
//		}
//
//		public static AcceleratorWaitPolicy TimedOutAndMarkToken(ulong timeoutTicks = 0)
//		{
//			return new(TimeoutExpired: true, MarkTokenTimedOutOnTimeout: true, TimeoutTicks: timeoutTicks);
//		}
//	}

	struct AcceleratorWaitPolicy final {
		const bool TimeoutExpired;
		const bool MarkTokenTimedOutOnTimeout;
		const unsigned long long TimeoutTicks;
		AcceleratorWaitPolicy(bool expired, bool mark, unsigned long long ticks) : TimeoutExpired(expired), MarkTokenTimedOutOnTimeout(mark), TimeoutTicks(ticks) {}
		static const std::shared_ptr<AcceleratorWaitPolicy> ObserveOnly;
		static std::shared_ptr<AcceleratorWaitPolicy> TimedOutNoStateChange(unsigned long long ticks = 0) { return std::make_shared<AcceleratorWaitPolicy>(true, false, ticks); }
		static std::shared_ptr<AcceleratorWaitPolicy> TimedOutAndMarkToken(unsigned long long ticks = 0) { return std::make_shared<AcceleratorWaitPolicy>(true, true, ticks); }
	};
	inline const std::shared_ptr<AcceleratorWaitPolicy> AcceleratorWaitPolicy::ObserveOnly = std::make_shared<AcceleratorWaitPolicy>(false, false, 0);
	enum class AcceleratorRunningCancelDisposition : unsigned char
	{
		CooperativeCancel = 0,
		Reject = 1,
		Fault = 2
	};

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct AcceleratorCancelPolicy(AcceleratorRunningCancelDisposition RunningDisposition)
//	{
//		public static AcceleratorCancelPolicy Cooperative
//		{
//			get;
//		} = new(AcceleratorRunningCancelDisposition.CooperativeCancel);
//
//		public static AcceleratorCancelPolicy RejectRunning
//		{
//			get;
//		} = new(AcceleratorRunningCancelDisposition.Reject);
//
//		public static AcceleratorCancelPolicy FaultRunning
//		{
//			get;
//		} = new(AcceleratorRunningCancelDisposition.Fault);
//	}

	struct AcceleratorCancelPolicy final
	{
		const AcceleratorRunningCancelDisposition RunningDisposition;

		explicit AcceleratorCancelPolicy(AcceleratorRunningCancelDisposition runningDisposition)
			: RunningDisposition(runningDisposition)
		{
		}

		bool operator==(const AcceleratorCancelPolicy &other) const
		{
			return RunningDisposition == other.RunningDisposition;
		}

		static const std::shared_ptr<AcceleratorCancelPolicy> Cooperative;
		static const std::shared_ptr<AcceleratorCancelPolicy> RejectRunning;
		static const std::shared_ptr<AcceleratorCancelPolicy> FaultRunning;
	};

	inline const std::shared_ptr<AcceleratorCancelPolicy> AcceleratorCancelPolicy::Cooperative =
		std::shared_ptr<AcceleratorCancelPolicy>(new AcceleratorCancelPolicy(AcceleratorRunningCancelDisposition::CooperativeCancel));
	inline const std::shared_ptr<AcceleratorCancelPolicy> AcceleratorCancelPolicy::RejectRunning =
		std::shared_ptr<AcceleratorCancelPolicy>(new AcceleratorCancelPolicy(AcceleratorRunningCancelDisposition::Reject));
	inline const std::shared_ptr<AcceleratorCancelPolicy> AcceleratorCancelPolicy::FaultRunning =
		std::shared_ptr<AcceleratorCancelPolicy>(new AcceleratorCancelPolicy(AcceleratorRunningCancelDisposition::Fault));
}
