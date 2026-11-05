#include "InstructionCoverage.h"

#include <algorithm>
#include <chrono>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <sstream>

using namespace YAKSys_Hybrid_CPU;

namespace HybridCPU_ISE::Core
{
	namespace
	{
		std::wstring CurrentUtcTimestamp()
		{
			const auto now = std::chrono::system_clock::now();
			const std::time_t value = std::chrono::system_clock::to_time_t(now);
			std::tm utc{};
			gmtime_s(&utc, &value);
			std::wostringstream result;
			result << std::put_time(&utc, L"%Y-%m-%d %H:%M:%S");
			return result.str();
		}

		std::wstring FormatCount(long long value)
		{
			std::wostringstream result;
			result << std::fixed << std::setprecision(0) << value;
			return result.str();
		}

		void WriteLine(std::wofstream& writer, const std::wstring& line = {})
		{
			writer << line << L'\n';
		}
	}

	InstructionCoverage::InstructionCoverage()
		: executionCount(),
		  exceptionCount({
			  {L"Overflow", 0},
			  {L"Underflow", 0},
			  {L"DivByZero", 0},
			  {L"InvalidOp", 0},
			  {L"Inexact", 0}}),
		  totalInstructions(0),
		  totalExceptions(0)
	{
	}

	void InstructionCoverage::Record(unsigned int opcode)
	{
		++executionCount[opcode];
		++totalInstructions;
	}

	void InstructionCoverage::RecordException(VectorExceptionFlags flags)
	{
		if (flags.Overflow) { ++exceptionCount[L"Overflow"]; ++totalExceptions; }
		if (flags.Underflow) { ++exceptionCount[L"Underflow"]; ++totalExceptions; }
		if (flags.DivByZero) { ++exceptionCount[L"DivByZero"]; ++totalExceptions; }
		if (flags.InvalidOp) { ++exceptionCount[L"InvalidOp"]; ++totalExceptions; }
		if (flags.Inexact) { ++exceptionCount[L"Inexact"]; ++totalExceptions; }
	}

	long long InstructionCoverage::GetExecutionCount(unsigned int opcode)
	{
		const auto found = executionCount.find(opcode);
		return found == executionCount.end() ? 0 : found->second;
	}

	long long InstructionCoverage::GetExceptionCount(const std::wstring& exceptionType)
	{
		const auto found = exceptionCount.find(exceptionType);
		return found == exceptionCount.end() ? 0 : found->second;
	}

	long long InstructionCoverage::getTotalInstructions() const
	{
		return totalInstructions;
	}

	long long InstructionCoverage::getTotalExceptions() const
	{
		return totalExceptions;
	}

	int InstructionCoverage::getUniqueOpcodes() const
	{
		return static_cast<int>(executionCount.size());
	}

	void InstructionCoverage::Clear()
	{
		executionCount.clear();
		for (auto& [key, count] : exceptionCount)
		{
			count = 0;
		}
		totalInstructions = 0;
		totalExceptions = 0;
	}

	void InstructionCoverage::DumpCoverageReport(const std::wstring& path)
	{
		std::wofstream writer{std::filesystem::path(path)};

		WriteLine(writer, L"=== HybridCPU ISE Instruction Coverage Report ===");
		WriteLine(writer, std::format(L"Generated: {} UTC", CurrentUtcTimestamp()));
		WriteLine(writer);

		WriteLine(writer, L"--- Summary ---");
		WriteLine(writer, std::format(L"Total Instructions Executed: {}", FormatCount(totalInstructions)));
		WriteLine(writer, std::format(L"Unique Opcodes Executed: {}", getUniqueOpcodes()));
		WriteLine(writer, std::format(L"Total Exceptions: {}", FormatCount(totalExceptions)));
		WriteLine(writer);

		WriteLine(writer, L"--- Instruction Execution Counts ---");
		WriteLine(writer, std::format(L"{:<30} {:>15} {:>12}", L"Opcode", L"Count", L"Percentage"));
		WriteLine(writer, std::wstring(60, L'-'));

		std::vector<std::pair<unsigned int, long long>> sortedOpcodes(executionCount.begin(), executionCount.end());
		std::stable_sort(sortedOpcodes.begin(), sortedOpcodes.end(), [](const auto& left, const auto& right)
		{
			return left.second > right.second;
		});
		for (const auto& kvp : sortedOpcodes)
		{
			const double percentage = totalInstructions > 0 ? (kvp.second * 100.0 / totalInstructions) : 0.0;
			WriteLine(writer, std::format(L"{:>30} {:>15} {:>11.2f}%", kvp.first, FormatCount(kvp.second), percentage));
		}

		WriteLine(writer);
		WriteLine(writer, L"--- Exception Counts ---");
		WriteLine(writer, std::format(L"{:<30} {:>15} {:>12}", L"Exception Type", L"Count", L"Percentage"));
		WriteLine(writer, std::wstring(60, L'-'));

		std::vector<std::pair<std::wstring, long long>> sortedExceptions(exceptionCount.begin(), exceptionCount.end());
		std::stable_sort(sortedExceptions.begin(), sortedExceptions.end(), [](const auto& left, const auto& right)
		{
			return left.second > right.second;
		});
		for (const auto& kvp : sortedExceptions)
		{
			const double percentage = totalExceptions > 0 ? (kvp.second * 100.0 / totalExceptions) : 0.0;
			WriteLine(writer, std::format(L"{:<30} {:>15} {:>11.2f}%", kvp.first, FormatCount(kvp.second), percentage));
		}

		WriteLine(writer);
		WriteLine(writer, L"--- ISA Coverage Analysis ---");
		constexpr int totalOpcodes = 256;
		const double coveragePercent = getUniqueOpcodes() * 100.0 / totalOpcodes;
		WriteLine(writer, std::format(L"Total Opcodes in ISA: {}", totalOpcodes));
		WriteLine(writer, std::format(L"Opcodes Executed: {}", getUniqueOpcodes()));
		WriteLine(writer, std::format(L"ISA Coverage: {:.2f}%", coveragePercent));

		WriteLine(writer);
		WriteLine(writer, L"--- Unexecuted Opcodes ---");
		std::vector<unsigned int> unexecutedOpcodes;
		for (unsigned int opcode = 0; opcode < 256; ++opcode)
		{
			if (!executionCount.contains(opcode))
			{
				unexecutedOpcodes.push_back(opcode);
			}
		}

		if (!unexecutedOpcodes.empty())
		{
			for (const auto opcode : unexecutedOpcodes)
			{
				WriteLine(writer, std::format(L"  Opcode {}", opcode));
			}
		}
		else
		{
			WriteLine(writer, L"  (All opcodes executed)");
		}

		WriteLine(writer);
		WriteLine(writer, L"=== End of Report ===");
	}

	std::unordered_map<std::wstring, std::any> InstructionCoverage::GetStatistics()
	{
		constexpr int totalOpcodes = 256;
		const double coveragePercent = getUniqueOpcodes() * 100.0 / totalOpcodes;
		return {
			{L"TotalInstructions", totalInstructions},
			{L"UniqueOpcodes", getUniqueOpcodes()},
			{L"TotalOpcodes", totalOpcodes},
			{L"CoveragePercent", coveragePercent},
			{L"TotalExceptions", totalExceptions},
			{L"ExceptionCounts", exceptionCount}
		};
	}
}