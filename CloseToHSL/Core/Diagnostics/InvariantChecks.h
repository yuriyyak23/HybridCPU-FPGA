#pragma once

#include <string>
#include <vector>
#include <stdexcept>
#include <format>
#include <memory>
#include <exception>
#include <utility>

namespace HybridCPU_ISE::Core
{
	/// <summary>
	/// Custom exception for invariant violations (ref5.docx)
	/// </summary>
	class InvariantViolationException : public std::runtime_error, public std::enable_shared_from_this<InvariantViolationException>
	{
	private:
		std::exception_ptr _innerException;

	public:
		InvariantViolationException(const std::wstring &message);

		InvariantViolationException(const std::wstring &message, std::runtime_error innerException);

		std::exception_ptr getInnerException() const noexcept { return _innerException; }
	};

	/// <summary>
	/// Invariant checking utilities for verification
	/// Checks can be disabled via conditional compilation with TRACE_INVARIANTS symbol
	/// </summary>
	class InvariantChecks final
	{
		/// <summary>
		/// Check that masked-off elements in result vector remain unmodified
		/// </summary>
	public:
// C# TO C++ CONVERTER NOTE: The following .NET attribute has no direct equivalent in C++:
// ORIGINAL LINE: [Conditional("TRACE_INVARIANTS")] public static void CheckMaskedElementsPreserved(byte[] original, byte[] result, ulong predicateMask, int elementCount, int elementSize, bool maskAgnostic)
		static void CheckMaskedElementsPreserved(std::vector<unsigned char> &original, std::vector<unsigned char> &result, unsigned long long predicateMask, int elementCount, int elementSize, bool maskAgnostic);

		/// <summary>
		/// Check that burst transaction doesn't cross AXI page boundary
		/// </summary>
// C# TO C++ CONVERTER NOTE: The following .NET attribute has no direct equivalent in C++:
// ORIGINAL LINE: [Conditional("TRACE_INVARIANTS")] public static void CheckBurstBoundary(ulong address, int size, int axiBoundary = 4096)
		static void CheckBurstBoundary(unsigned long long address, int size, int axiBoundary = 4096);

		/// <summary>
		/// Check that strip-mining iteration processes exactly VL elements
		/// </summary>
// C# TO C++ CONVERTER NOTE: The following .NET attribute has no direct equivalent in C++:
// ORIGINAL LINE: [Conditional("TRACE_INVARIANTS")] public static void CheckStripMiningCount(int processedCount, uint vl, bool isFinalIteration)
		static void CheckStripMiningCount(int processedCount, unsigned int vl, bool isFinalIteration);

		/// <summary>
		/// Check that VL <= VLMAX constraint is satisfied
		/// </summary>
// C# TO C++ CONVERTER NOTE: The following .NET attribute has no direct equivalent in C++:
// ORIGINAL LINE: [Conditional("TRACE_INVARIANTS")] public static void CheckVLConstraint(uint vl, uint vlmax)
		static void CheckVLConstraint(unsigned int vl, unsigned int vlmax);

		/// <summary>
		/// Check that VSEW is valid power-of-2 between 8 and 64
		/// </summary>
// C# TO C++ CONVERTER NOTE: The following .NET attribute has no direct equivalent in C++:
// ORIGINAL LINE: [Conditional("TRACE_INVARIANTS")] public static void CheckVSEW(byte vsew)
		static void CheckVSEW(unsigned char vsew);

		/// <summary>
		/// Check that LMUL is valid (1, 2, 4, or 8)
		/// </summary>
// C# TO C++ CONVERTER NOTE: The following .NET attribute has no direct equivalent in C++:
// ORIGINAL LINE: [Conditional("TRACE_INVARIANTS")] public static void CheckLMUL(byte lmul)
		static void CheckLMUL(unsigned char lmul);

		/// <summary>
		/// Check that array index is within bounds
		/// </summary>
// C# TO C++ CONVERTER NOTE: The following .NET attribute has no direct equivalent in C++:
// ORIGINAL LINE: [Conditional("TRACE_INVARIANTS")] public static void CheckIndexBounds(int index, int arrayLength, string arrayName)
		static void CheckIndexBounds(int index, int arrayLength, const std::wstring &arrayName);

		/// <summary>
		/// Check that memory address is aligned
		/// </summary>
// C# TO C++ CONVERTER NOTE: The following .NET attribute has no direct equivalent in C++:
// ORIGINAL LINE: [Conditional("TRACE_INVARIANTS")] public static void CheckAlignment(ulong address, int alignment)
		static void CheckAlignment(unsigned long long address, int alignment);

		/// <summary>
		/// Check that predicate register index is valid (0-15)
		/// </summary>
// C# TO C++ CONVERTER NOTE: The following .NET attribute has no direct equivalent in C++:
// ORIGINAL LINE: [Conditional("TRACE_INVARIANTS")] public static void CheckPredicateRegisterIndex(byte index)
		static void CheckPredicateRegisterIndex(unsigned char index);

		/// <summary>
		/// Check that rounding mode is valid (0-4)
		/// </summary>
// C# TO C++ CONVERTER NOTE: The following .NET attribute has no direct equivalent in C++:
// ORIGINAL LINE: [Conditional("TRACE_INVARIANTS")] public static void CheckRoundingMode(byte mode)
		static void CheckRoundingMode(unsigned char mode);

		/// <summary>
		/// Check that exception mode is valid (0-2)
		/// </summary>
// C# TO C++ CONVERTER NOTE: The following .NET attribute has no direct equivalent in C++:
// ORIGINAL LINE: [Conditional("TRACE_INVARIANTS")] public static void CheckExceptionMode(byte mode)
		static void CheckExceptionMode(unsigned char mode);
	};
}
