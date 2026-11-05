#pragma once

#include <string>
#include <vector>
#include <memory>
#include <utility>
#include "../../../../../tangible_string_helper.h"

namespace YAKSys_Hybrid_CPU::Core
{

	enum class CoreVmxAuthorityBoundaryViolation : unsigned char
	{
		None = 0,
		DirectShadowVmcsAccess = 1,
		DirectVmxIommuHook = 2,
		DirectVmcsFieldAccess = 3,
		DirectVmcsTranslationSource = 4,
		DirectVmxCapsCsrSource = 5
	};

	struct CoreVmxAuthorityMarker final
	{
		std::wstring Marker;
		CoreVmxAuthorityBoundaryViolation Violation;
		std::wstring AllowedRelativePaths;

		CoreVmxAuthorityMarker(std::wstring marker, CoreVmxAuthorityBoundaryViolation violation,
			std::wstring allowedRelativePaths)
			: Marker(std::move(marker)), Violation(violation), AllowedRelativePaths(std::move(allowedRelativePaths))
		{
		}
	};

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct CoreVmxAuthorityMarker(string Marker, CoreVmxAuthorityBoundaryViolation Violation, string AllowedRelativePaths);

	class CoreVmxAuthorityBoundaryContract final : public std::enable_shared_from_this<CoreVmxAuthorityBoundaryContract>
	{
	private:
		static inline const std::wstring DeniedIommuAliasProjectionPath = L"Core/VMX/Compatibility/Adapters/IO/IommuVmxCompatibilityAliases.cs";

		static inline const std::vector<CoreVmxAuthorityMarker> Markers = {
			{L".ShadowVmcs.", CoreVmxAuthorityBoundaryViolation::DirectShadowVmcsAccess, L""},
			{L"IOMMU.", CoreVmxAuthorityBoundaryViolation::DirectVmxIommuHook, L""},
			{L"BindVmx", CoreVmxAuthorityBoundaryViolation::DirectVmxIommuHook, DeniedIommuAliasProjectionPath},
			{L"InvalidateVmx", CoreVmxAuthorityBoundaryViolation::DirectVmxIommuHook, DeniedIommuAliasProjectionPath},
			{L"ApplyVmx", CoreVmxAuthorityBoundaryViolation::DirectVmxIommuHook, DeniedIommuAliasProjectionPath},
			{L"UnbindVmx", CoreVmxAuthorityBoundaryViolation::DirectVmxIommuHook, DeniedIommuAliasProjectionPath},
			{L"ReadFieldValue(", CoreVmxAuthorityBoundaryViolation::DirectVmcsFieldAccess, L""},
			{L"WriteFieldValue(", CoreVmxAuthorityBoundaryViolation::DirectVmcsFieldAccess, L""},
			{L"MemoryTranslationControl.FromVmcs", CoreVmxAuthorityBoundaryViolation::DirectVmcsTranslationSource, L""},
			{L"CsrAddresses.VmxCaps", CoreVmxAuthorityBoundaryViolation::DirectVmxCapsCsrSource,
				L"Core/VMX/Compatibility/Generated/CsrProjection/VmxCapsProjection.cs;Core/VMX/Compatibility/FrozenAbi/CsrAliases/VmxCsrAliasSet.cs"}};

	public:
		const std::vector<CoreVmxAuthorityMarker> &getForbiddenMarkers() const;

		CoreVmxAuthorityBoundaryViolation EvaluateLine(const std::wstring &relativePath, const std::wstring &line);

		bool IsAllowedLine(const std::wstring &relativePath, const std::wstring &line);

	private:
		static std::wstring NormalizePath(const std::wstring &path);

		static bool IsAllowedPath(const std::wstring &normalizedPath, const std::wstring &allowedRelativePaths);
	};

}
