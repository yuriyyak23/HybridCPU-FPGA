#pragma once

#include "TraceSink.h"
#include <string>
#include <unordered_map>
#include <vector>
#include <any>
#include <format>
#include <memory>

using namespace YAKSys_Hybrid_CPU;

namespace HybridCPU_ISE::Core
{
	/// <summary>
	/// Tracks instruction execution coverage and exception occurrence
	/// </summary>
	class InstructionCoverage : public std::enable_shared_from_this<InstructionCoverage>
	{
	private:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly Dictionary<uint, long> executionCount;
		std::unordered_map<unsigned int, long long> executionCount;
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly Dictionary<string, long> exceptionCount;
		std::unordered_map<std::wstring, long long> exceptionCount;
		long long totalInstructions = 0;
		long long totalExceptions = 0;

	public:
		InstructionCoverage();

		/// <summary>
		/// Record instruction execution
		/// </summary>
		void Record(unsigned int opcode);

		/// <summary>
		/// Record exception occurrence
		/// </summary>
		void RecordException(VectorExceptionFlags flags);

		/// <summary>
		/// Get execution count for specific opcode
		/// </summary>
		long long GetExecutionCount(unsigned int opcode);

		/// <summary>
		/// Get exception count for specific exception type
		/// </summary>
		long long GetExceptionCount(const std::wstring &exceptionType);

		/// <summary>
		/// Get total instructions executed
		/// </summary>
		long long getTotalInstructions() const;

		/// <summary>
		/// Get total exceptions occurred
		/// </summary>
		long long getTotalExceptions() const;

		/// <summary>
		/// Get count of unique opcodes executed
		/// </summary>
		int getUniqueOpcodes() const;

		/// <summary>
		/// Clear all coverage data
		/// </summary>
		void Clear();

		/// <summary>
		/// Dump coverage report to file
		/// </summary>
		void DumpCoverageReport(const std::wstring &path);

		/// <summary>
		/// Get coverage statistics as dictionary
		/// </summary>
		std::unordered_map<std::wstring, std::any> GetStatistics();
	};
}
