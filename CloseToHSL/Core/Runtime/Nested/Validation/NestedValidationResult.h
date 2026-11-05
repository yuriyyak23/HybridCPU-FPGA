#pragma once
#include <memory>
#include <string>
#include <utility>

namespace YAKSys_Hybrid_CPU::Core::Nested
{

	enum class NestedValidationCode : unsigned char
	{
		Success = 0,
		ProjectionDenied = 1,
		CompatibilityProjectionFailed = 2
	};

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct NestedValidationResult(bool Succeeded, NestedValidationCode Code, string Message)
//	{
//		public static NestedValidationResult Success()
//		{
//			return new(true, NestedValidationCode.Success, string.Empty);
//		}
//
//		public static NestedValidationResult Fail(NestedValidationCode code, string message)
//		{
//			return new(false, code, message);
//		}
//	}

struct NestedValidationResult final
{
    bool Succeeded = false;
    NestedValidationCode Code = NestedValidationCode::Success;
    std::wstring Message;
    NestedValidationResult(bool succeeded, NestedValidationCode code, std::wstring message)
        : Succeeded(succeeded), Code(code), Message(std::move(message)) {}
    static std::shared_ptr<NestedValidationResult> Success() { return std::make_shared<NestedValidationResult>(true, NestedValidationCode::Success, L""); }
    static std::shared_ptr<NestedValidationResult> Fail(NestedValidationCode code, std::wstring message) { return std::make_shared<NestedValidationResult>(false, code, std::move(message)); }
};
}
