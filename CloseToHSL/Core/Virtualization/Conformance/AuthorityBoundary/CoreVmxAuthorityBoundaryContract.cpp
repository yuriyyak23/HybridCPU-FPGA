#include "CoreVmxAuthorityBoundaryContract.h"

namespace YAKSys_Hybrid_CPU::Core
{
	const std::vector<CoreVmxAuthorityMarker> &CoreVmxAuthorityBoundaryContract::getForbiddenMarkers() const
	{
		return Markers;
	}

	CoreVmxAuthorityBoundaryViolation CoreVmxAuthorityBoundaryContract::EvaluateLine(
		const std::wstring &relativePath, const std::wstring &line)
	{
		const std::wstring normalizedPath = NormalizePath(relativePath);
		for (const auto &marker : getForbiddenMarkers())
		{
			if (line.find(marker.Marker) == std::wstring::npos)
				continue;
			if (IsAllowedPath(normalizedPath, marker.AllowedRelativePaths))
				continue;
			return marker.Violation;
		}
		return CoreVmxAuthorityBoundaryViolation::None;
	}

	bool CoreVmxAuthorityBoundaryContract::IsAllowedLine(const std::wstring &relativePath, const std::wstring &line)
	{
		return EvaluateLine(relativePath, line) == CoreVmxAuthorityBoundaryViolation::None;
	}

	std::wstring CoreVmxAuthorityBoundaryContract::NormalizePath(const std::wstring &path)
	{
		std::wstring normalized = path;
		for (auto &character : normalized)
		{
			if (character == L'\\') character = L'/';
		}
		return normalized;
	}

	bool CoreVmxAuthorityBoundaryContract::IsAllowedPath(
		const std::wstring &normalizedPath, const std::wstring &allowedRelativePaths)
	{
		std::size_t start = 0;
		while (start < allowedRelativePaths.size())
		{
			const auto separator = allowedRelativePaths.find(L';', start);
			const auto end = separator == std::wstring::npos ? allowedRelativePaths.size() : separator;
			if (end > start && normalizedPath == allowedRelativePaths.substr(start, end - start))
				return true;
			if (separator == std::wstring::npos) break;
			start = separator + 1;
		}
		return false;
	}
}
