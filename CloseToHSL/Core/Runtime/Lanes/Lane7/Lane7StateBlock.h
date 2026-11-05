#pragma once
#include "Lane7EvidenceTypes.h"
#include "../../../Execution/ExternalAccelerators/Descriptors/AcceleratorCommandDescriptor.h"
#include <string>
#include <memory>
#include "YAKSys_Hybrid_CPU.Core.Lane7VirtualToken.h"

using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Descriptors;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Tokens;

namespace YAKSys_Hybrid_CPU::Core
{
// C# TO C++ CONVERTER NOTE: The following .NET attribute has no direct equivalent in C++:
// ORIGINAL LINE: [Flags] public enum Lane7VirtualCapability : ulong
	enum class Lane7VirtualCapability : int
	{
		None = 0,
		QueryCaps = 1ULL << 0,
		Submit = 1ULL << 1,
		PollStatus = 1ULL << 2,
		Wait = 1ULL << 3,
		Cancel = 1ULL << 4,
		Fence = 1ULL << 5,
		MatMul = 1ULL << 16
	};

	enum class Lane7FaultKind : unsigned char
	{
		None = 0,
		VirtualizationDisabled = 1,
		OwnershipMismatch = 2,
		InvalidVirtualHandle = 3,
		InvalidVirtualToken = 4,
		CapabilityDenied = 5,
		QuotaExceeded = 6,
		FirstUseBindingRequired = 7,
		BackendUnavailable = 8,
		BackendRebindRequired = 9,
		SecuritySensitiveOperation = 10,
		CompletionRouteDenied = 11
	};

	enum class Lane7FaultDisposition : unsigned char
	{
		None = 0,
		Abort = 1,
		Replay = 2,
		RuntimeFallback = 3
	};

 struct Lane7Fault final {
  Lane7FaultKind Kind=Lane7FaultKind::None; Lane7FaultDisposition Disposition=Lane7FaultDisposition::None; std::wstring Message;
  bool IsFaulted() const { return Kind != Lane7FaultKind::None; }
  static std::shared_ptr<Lane7Fault> Abort(Lane7FaultKind k, std::wstring m) { return std::make_shared<Lane7Fault>(k,Lane7FaultDisposition::Abort,std::move(m)); }
  static std::shared_ptr<Lane7Fault> Replay(Lane7FaultKind k, std::wstring m) { return std::make_shared<Lane7Fault>(k,Lane7FaultDisposition::Replay,std::move(m)); }
  static std::shared_ptr<Lane7Fault> Fallback(Lane7FaultKind k, std::wstring m) { return std::make_shared<Lane7Fault>(k,Lane7FaultDisposition::RuntimeFallback,std::move(m)); }
  static const std::shared_ptr<Lane7Fault> None;
 };
 inline const std::shared_ptr<Lane7Fault> Lane7Fault::None = std::make_shared<Lane7Fault>();
 struct Lane7VirtualHandle final {
  unsigned short ExecutionDomainTag=0, OwnerVirtualThreadId=0; unsigned long long Value=0;
  Execution::ExternalAccelerators::Descriptors::AcceleratorDeviceId AcceleratorId{}; Lane7VirtualCapability Capabilities=Lane7VirtualCapability::None; unsigned long long HandleEpoch=0;
  bool IsValid() const { return ExecutionDomainTag != 0 && Value != 0 && Capabilities != Lane7VirtualCapability::None && HandleEpoch != 0; }
 };
// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct Lane7Fault(Lane7FaultKind Kind, Lane7FaultDisposition Disposition, string Message)
//	{
//		public bool IsFaulted
//		{
//			get
//			{
//				return Kind != Lane7FaultKind.None;
//			}
//		}
//
//		public static Lane7Fault None
//		{
//			get;
//		} = new(Lane7FaultKind.None, Lane7FaultDisposition.None, string.Empty);
//
//		public static Lane7Fault Abort(Lane7FaultKind kind, string message)
//		{
//			return new(kind, Lane7FaultDisposition.Abort, message);
//		}
//
//		public static Lane7Fault Replay(Lane7FaultKind kind, string message)
//		{
//			return new(kind, Lane7FaultDisposition.Replay, message);
//		}
//
//		public static Lane7Fault Fallback(Lane7FaultKind kind, string message)
//		{
//			return new(kind, Lane7FaultDisposition.RuntimeFallback, message);
//		}
//	}

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct Lane7QuotaPolicy(ushort MaxVirtualHandles, ushort MaxInflightTokens, ushort MaxSubmitPollPerWindow)
//	{
//		public static Lane7QuotaPolicy Default
//		{
//			get;
//		} = new(MaxVirtualHandles: 64, MaxInflightTokens: 64, MaxSubmitPollPerWindow: 8);
//
//		public Lane7QuotaPolicy Normalize()
//		{
//			return new(MaxVirtualHandles == 0 ? (ushort)1 : MaxVirtualHandles, MaxInflightTokens == 0 ? (ushort)1 : MaxInflightTokens, MaxSubmitPollPerWindow == 0 ? (ushort)1 : MaxSubmitPollPerWindow);
//		}
//	}

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct Lane7VirtualHandle(ushort ExecutionDomainTag, ushort OwnerVirtualThreadId, ulong Value, AcceleratorDeviceId AcceleratorId, Lane7VirtualCapability Capabilities, ulong HandleEpoch)
//	{
//		public bool IsValid
//		{
//			get
//			{
//				return ExecutionDomainTag != 0 && Value != 0 && Capabilities != Lane7VirtualCapability.None && HandleEpoch != 0;
//			}
//		}
//	}


// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct Lane7BackendBinding(ushort ExecutionDomainTag, ushort OwnerVirtualThreadId, ulong VirtualHandle, ulong BindingEpoch, ulong BackendGeneration, bool Available, bool RequiresRebind)
//	{
//		public bool IsUsable
//		{
//			get
//			{
//				return ExecutionDomainTag != 0 && VirtualHandle != 0 && BindingEpoch != 0 && Available && !RequiresRebind;
//			}
//		}
//	}

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct Lane7PressureSnapshot(ushort ExecutionDomainTag, ushort AddressSpaceTag, ushort OwnerVirtualThreadId, int InflightTokens, int MaxInflightTokens, int SubmitPollCount, int MaxSubmitPollPerWindow, bool QuotaExceeded, bool SchedulerPreemptionRecommended, ulong PressureEpoch);


}
