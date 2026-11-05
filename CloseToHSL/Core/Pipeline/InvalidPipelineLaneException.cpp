#include "InvalidPipelineLaneException.h"

using namespace YAKSys_Hybrid_CPU::Core;

namespace YAKSys_Hybrid_CPU::Core::Pipeline
{

	std::wstring InvalidPipelineLaneException::getStageName() const
	{
		return StageName;
	}

	unsigned char InvalidPipelineLaneException::getLaneIndex() const
	{
		return LaneIndex;
	}

	std::optional<unsigned long long> InvalidPipelineLaneException::getBundlePc() const
	{
		return BundlePc;
	}

	std::optional<int> InvalidPipelineLaneException::getVtId() const
	{
		return VtId;
	}

	YAKSys_Hybrid_CPU::Core::ExecutionFaultCategory InvalidPipelineLaneException::getCategory() const
	{
		return ExecutionFaultCategory::InvalidPipelineLane;
	}

    InvalidPipelineLaneException::InvalidPipelineLaneException(const std::wstring& stageName, unsigned char laneIndex, std::optional<unsigned long long> bundlePc, std::optional<int> vtId)
        : std::runtime_error(std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t>().to_bytes(
            ExecutionFaultContract::FormatMessage(ExecutionFaultCategory::InvalidPipelineLane, FormatDetail(stageName, laneIndex, bundlePc, vtId)))),
          StageName(NormalizeStageName(stageName)), LaneIndex(laneIndex), BundlePc(bundlePc), VtId(vtId)
    {
        ExecutionFaultContract::Stamp(*this, getCategory());
    }
	std::wstring InvalidPipelineLaneException::FormatDetail(const std::wstring &stageName, unsigned char laneIndex, const std::optional<unsigned long long> &bundlePc, const std::optional<int> &vtId)
	{
		std::wstring normalizedStageName = NormalizeStageName(stageName);
		std::wstring context = L"";

		if (bundlePc)
		{
			context += std::format(L" BundlePc=0x{:X}.", bundlePc.value());
		}

		if (vtId)
		{
			context += std::format(L" VtId={}.", vtId.value());
		}

		return std::format(L"{} attempted to access invalid pipeline lane {}. Valid lane range is {}..{}.{}", normalizedStageName, laneIndex, MinLaneIndex, MaxLaneIndex, context);
	}

	std::wstring InvalidPipelineLaneException::NormalizeStageName(const std::wstring &stageName)
	{
		return StringHelper::isEmptyOrWhiteSpace(stageName) ? L"UnknownPipelineStage" : stageName;
	}

	unsigned char PipelineLaneResolver::ResolveLaneOrThrow(unsigned char laneIndex, const std::wstring &stageName, std::optional<unsigned long long> bundlePc, std::optional<int> vtId)
	{
		if (laneIndex <= InvalidPipelineLaneException::MaxLaneIndex)
		{
			return laneIndex;
		}

		throw InvalidPipelineLaneException(stageName, laneIndex, bundlePc, vtId);
	}
}
