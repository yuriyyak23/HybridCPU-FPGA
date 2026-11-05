#pragma once
#include <string>
#include <utility>
#include "../Retire/VmxRetireModel.h"
#include "VmxCompletion.h"
#include "../../../../Runtime/Nested/Validation/NestedValidationResult.h"

using namespace YAKSys_Hybrid_CPU::Core::Nested;
using namespace YAKSys_Hybrid_CPU::Core::Vmcs::V2;

namespace YAKSys_Hybrid_CPU::Core
{

	enum class VmxFrontendResultKind : unsigned char
	{
		Success = 0,
		VmFailValid = 1,
		VmFailInvalid = 2,
		VmExit = 3,
		VmAbort = 4
	};

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct VmxFrontendResult(VmxFrontendResultKind Kind, VmxCompletionKind CompletionKind, VmFailCode FailCode, VmAbortCode AbortCode, string Reason)
//	{
//		public bool Succeeded
//		{
//			get
//			{
//				return Kind == VmxFrontendResultKind.Success;
//			}
//		}
//
//		public static VmxFrontendResult Success
//		{
//			get;
//		} = new(VmxFrontendResultKind.Success, VmxCompletionKind.Success, VmFailCode.None, VmAbortCode.None, string.Empty);
//
//		public static VmxFrontendResult FailValid(VmFailCode failCode, string reason)
//		{
//			return new(VmxFrontendResultKind.VmFailValid, VmxCompletionKind.VmFailValid, failCode, VmAbortCode.None, reason);
//		}
//
//		public static VmxFrontendResult FailInvalid(VmFailCode failCode, string reason)
//		{
//			return new(VmxFrontendResultKind.VmFailInvalid, VmxCompletionKind.VmFailInvalid, failCode, VmAbortCode.None, reason);
//		}
//	}


struct VmxFrontendResult final
{
    VmxFrontendResultKind Kind{};
    VmxCompletionKind CompletionKind{};
    VmFailCode FailCode{};
    VmAbortCode AbortCode{};
    std::wstring Reason;
    bool Succeeded = false;
    VmxFrontendResult(VmxFrontendResultKind kind, VmxCompletionKind completion, VmFailCode fail, VmAbortCode abort, std::wstring reason)
        : Kind(kind), CompletionKind(completion), FailCode(fail), AbortCode(abort), Reason(std::move(reason)), Succeeded(kind == VmxFrontendResultKind::Success) {}
    static VmxFrontendResult Success() { return { VmxFrontendResultKind::Success, VmxCompletionKind::Success, VmFailCode::None, VmAbortCode::None, L"" }; }
    static VmxFrontendResult FailValid(VmFailCode code, std::wstring reason) { return { VmxFrontendResultKind::VmFailValid, VmxCompletionKind::VmFailValid, code, VmAbortCode::None, std::move(reason) }; }
    static VmxFrontendResult FailInvalid(VmFailCode code, std::wstring reason) { return { VmxFrontendResultKind::VmFailInvalid, VmxCompletionKind::VmFailInvalid, code, VmAbortCode::None, std::move(reason) }; }
};
}
