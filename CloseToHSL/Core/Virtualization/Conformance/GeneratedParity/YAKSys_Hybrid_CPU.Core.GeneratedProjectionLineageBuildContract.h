#pragma once

#include <string>
#include <vector>
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{
	class GeneratedProjectionLineageBuildContract final : public std::enable_shared_from_this<GeneratedProjectionLineageBuildContract>
	{
	public:
		static inline const std::wstring BuildPropertyName = L"EnableVmxProjectionLineageCheck";
		static inline const std::wstring BuildTargetName = L"VerifyVmxProjectionLineage";
		static inline const std::wstring BuildTargetPhase = L"CoreCompile";
		static inline const std::wstring VerifierScriptPath = L"tools/VMXProjectionLineage/VerifyProjectionLineage.ps1";
		static inline const std::wstring VmcsFieldProjectionSchemaPath = L"docs/VMXRefactoring/schemas/vmcs-field-projection-schema.v1.json";
		static inline const std::wstring CompatAliasSchemaPath = L"docs/VMXRefactoring/schemas/compat-alias-schema.v1.json";
		static inline const std::wstring VmxCapsCapabilityBitSchemaPath = L"docs/VMXRefactoring/schemas/vmxcaps-capability-bit-schema.v1.json";
		static inline const std::wstring CompatSpecArtifactSchemaPath = L"docs/VMXRefactoring/schemas/compat-spec-artifact-schema.v1.json";
		static inline const std::wstring VmcsFieldProjectionSourcePath = L"CloseToHSL/Core/Virtualization/Compatibility/Generated/VmcsProjection/VmcsFieldProjectionSchema.cs";
		static inline const std::wstring CompatAliasSourcePath = L"CloseToHSL/Core/Virtualization/Compatibility/Generated/AliasMaps/CompatAliasMap.cs";
		static inline const std::wstring VmxCapsCapabilityBitSourcePath = L"CloseToHSL/Core/Virtualization/Compatibility/Generated/CapabilityProjection/CapabilityDescriptorSetSchema.cs";
		static inline const std::wstring CompatSpecArtifactSourcePath = L"CloseToHSL/Core/Virtualization/Compatibility/Generated/SpecArtifacts/CompatSpecArtifactSet.cs";

	private:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private static readonly IReadOnlyList<string> RequiredVerifierFunctions = new[] { "New-VmcsFieldProjectionSchemaSource", "New-CompatAliasMapSource", "New-VmxCapsCapabilityBitSchemaSource", "New-CompatSpecArtifactSetSource", "Assert-GeneratedSourceMatches", "Assert-RequiredSchemaSwitches"};
		static const std::vector<std::wstring> RequiredVerifierFunctions;
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private static readonly IReadOnlyList<string> RequiredGeneratedInputs = new[] { VmcsFieldProjectionSchemaPath, CompatAliasSchemaPath, VmxCapsCapabilityBitSchemaPath, CompatSpecArtifactSchemaPath};
		static const std::vector<std::wstring> RequiredGeneratedInputs;
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private static readonly IReadOnlyList<string> RequiredGeneratedOutputs = new[] { VmcsFieldProjectionSourcePath, CompatAliasSourcePath, VmxCapsCapabilityBitSourcePath, CompatSpecArtifactSourcePath};
		static const std::vector<std::wstring> RequiredGeneratedOutputs;
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private static readonly IReadOnlyList<string> RequiredDriftFailureMarkers = new[] { "Generated projection drift detected", "Regenerated expectation:"};
		static const std::vector<std::wstring> RequiredDriftFailureMarkers;
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private static readonly IReadOnlyList<string> ForbiddenRuntimeOwnerMarkers = new[] { "VmcsProjectionRuntimeManager", "VmcsV2Runtime", "VmcsV2RuntimeManager", "VmcsManagerAdapter"};
		static const std::vector<std::wstring> ForbiddenRuntimeOwnerMarkers;

	public:
		static const std::vector<std::wstring> getRequiredVerifierFunctions();

		static const std::vector<std::wstring> getRequiredGeneratedInputs();

		static const std::vector<std::wstring> getRequiredGeneratedOutputs();

		static const std::vector<std::wstring> getRequiredDriftFailureMarkers();

		static const std::vector<std::wstring> getForbiddenRuntimeOwnerMarkers();

		bool getUsesBuildTimeTarget() const;

		bool getRegeneratesVmcsFieldProjectionSchema() const;

		bool getRegeneratesCompatAliasMap() const;

		bool getRegeneratesVmxCapsCapabilityBitSchema() const;

		bool getRegeneratesCompatSpecArtifactManifest() const;

		bool getFailsBuildOnGeneratedDrift() const;

		bool getLeavesRuntimeAuthorityUnchanged() const;

		bool getDoesNotCreateRuntimeOwner() const;

		bool IsSatisfied();
	};
}
