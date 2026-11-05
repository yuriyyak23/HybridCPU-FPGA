#pragma once

#include <string>
#include <stdexcept>
#include <any>
#include <format>
#include <memory>
#include "../../../tangible_string_helper.h"
#include "../../../tangible_exception_helper.h"

class InvalidOperationException;
class ArgumentOutOfRangeException;

namespace YAKSys_Hybrid_CPU::Core
{
	/// <summary>
	/// Stable fail-closed taxonomy for execution/retire contract violations that must
	/// remain observable across leaf throws and boundary wrappers.
	/// </summary>
	enum class ExecutionFaultCategory : unsigned char
	{
		UnsupportedExecutionSurface = 1,
		InvalidInternalOp = 2,
		UnsupportedVectorElementType = 3,
		ReferenceModelValidation = 4,
		InvalidPipelineLane = 5,
		InvalidVirtualThread = 6
	};

	/// <summary>
	/// Canonical message/category/inner-exception contract for fail-closed execution faults.
	/// Leaf faults publish a prefixed message with no inner exception; wrappers preserve the
	/// original fault as <see cref="Exception.InnerException"/> and restamp the same category.
	/// </summary>
	class ExecutionFaultContract final
	{
	private:
		static inline const std::wstring CategoryDataKey = L"HybridCPU.ExecutionFaultCategory";
		static inline const std::wstring PrefixLead = L"[FailClosed/";

	public:
		static std::wstring GetMessagePrefix(ExecutionFaultCategory category);

		static std::wstring FormatMessage(ExecutionFaultCategory category, const std::wstring &detail);

		static void Stamp(std::exception& exception, ExecutionFaultCategory category);
		template<class T> static void Stamp(const std::shared_ptr<T>& exception, ExecutionFaultCategory category) { if (!exception) throw std::invalid_argument("exception"); Stamp(*exception, category); }

		static ExecutionFaultCategory GetCategory(const std::exception& exception);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public static bool TryGetCategory(Exception? exception, out ExecutionFaultCategory category)
		static bool TryGetCategory(const std::exception& exception, ExecutionFaultCategory &category);

		static InvalidOperationException CreateUnsupportedVectorElementTypeException(const std::wstring &detail);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public static ArgumentOutOfRangeException CreateReferenceModelValidationException(string paramName, Object? actualValue, string detail)
		static ArgumentOutOfRangeException CreateReferenceModelValidationException(const std::wstring &paramName, std::any actualValue, const std::wstring &detail);

		static InvalidOperationException CreateWrappedException(ExecutionFaultCategory category, const std::wstring &detail, std::exception_ptr innerException);

	private:
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private static bool TryGetDirectCategory(Exception? exception, out ExecutionFaultCategory category)
		static bool TryGetDirectCategory(const std::exception& exception, ExecutionFaultCategory &category);

		static bool TryParseCategoryFromMessagePrefix(const std::wstring &message, ExecutionFaultCategory &category);
	};
}
