#pragma once

#include "../Execution/ExecutionFaultContract.h"
#include <string>
#include <stdexcept>
#include <optional>
#include <format>
#include <memory>
#include "../../../tangible_string_helper.h"

using namespace YAKSys_Hybrid_CPU::Core;

namespace YAKSys_Hybrid_CPU::Core::Pipeline
{
	/// <summary>
	/// Thrown when production pipeline state addresses a scalar lane outside the
	/// architectural 8-wide issue window.
	/// </summary>
	class InvalidPipelineLaneException final : public std::runtime_error, public ExecutionFaultMetadata
	{
	private:
		const std::wstring StageName;
		const unsigned char LaneIndex;
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly System.Nullable<ulong> BundlePc;
		std::optional<unsigned long long> BundlePc;
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly System.Nullable<int> VtId;
		std::optional<int> VtId;

	public:
		static constexpr unsigned char MinLaneIndex = 0;
		static constexpr unsigned char MaxLaneIndex = 7;

		std::wstring getStageName() const;
		unsigned char getLaneIndex() const;
		std::optional<unsigned long long> getBundlePc() const;
		std::optional<int> getVtId() const;
		ExecutionFaultCategory getCategory() const;

		InvalidPipelineLaneException(const std::wstring &stageName, unsigned char laneIndex, std::optional<unsigned long long> bundlePc = std::nullopt, std::optional<int> vtId = std::nullopt);

	private:
		static std::wstring FormatDetail(const std::wstring &stageName, unsigned char laneIndex, const std::optional<unsigned long long> &bundlePc, const std::optional<int> &vtId);

		static std::wstring NormalizeStageName(const std::wstring &stageName);
	};

	class PipelineLaneResolver final
	{
	public:
		static unsigned char ResolveLaneOrThrow(unsigned char laneIndex, const std::wstring &stageName, std::optional<unsigned long long> bundlePc = std::nullopt, std::optional<int> vtId = std::nullopt);
	};
}
