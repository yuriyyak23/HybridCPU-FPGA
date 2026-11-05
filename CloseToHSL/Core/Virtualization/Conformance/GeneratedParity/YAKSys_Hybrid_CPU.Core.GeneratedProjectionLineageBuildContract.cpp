#include "YAKSys_Hybrid_CPU.Core.GeneratedProjectionLineageBuildContract.h"

namespace YAKSys_Hybrid_CPU::Core
{
	const std::vector<std::wstring> GeneratedProjectionLineageBuildContract::RequiredVerifierFunctions = {L"New-VmcsFieldProjectionSchemaSource", L"New-CompatAliasMapSource", L"New-VmxCapsCapabilityBitSchemaSource", L"New-CompatSpecArtifactSetSource", L"Assert-GeneratedSourceMatches", L"Assert-RequiredSchemaSwitches"};
	const std::vector<std::wstring> GeneratedProjectionLineageBuildContract::RequiredGeneratedInputs = {L"docs/VMXRefactoring/schemas/vmcs-field-projection-schema.v1.json", L"docs/VMXRefactoring/schemas/compat-alias-schema.v1.json", L"docs/VMXRefactoring/schemas/vmxcaps-capability-bit-schema.v1.json", L"docs/VMXRefactoring/schemas/compat-spec-artifact-schema.v1.json"};
	const std::vector<std::wstring> GeneratedProjectionLineageBuildContract::RequiredGeneratedOutputs = {L"CloseToHSL/Core/Virtualization/Compatibility/Generated/VmcsProjection/VmcsFieldProjectionSchema.cs", L"CloseToHSL/Core/Virtualization/Compatibility/Generated/AliasMaps/CompatAliasMap.cs", L"CloseToHSL/Core/Virtualization/Compatibility/Generated/CapabilityProjection/CapabilityDescriptorSetSchema.cs", L"CloseToHSL/Core/Virtualization/Compatibility/Generated/SpecArtifacts/CompatSpecArtifactSet.cs"};
	const std::vector<std::wstring> GeneratedProjectionLineageBuildContract::RequiredDriftFailureMarkers = {L"Generated projection drift detected", L"Regenerated expectation:"};
	const std::vector<std::wstring> GeneratedProjectionLineageBuildContract::ForbiddenRuntimeOwnerMarkers = {L"VmcsProjectionRuntimeManager", L"VmcsV2Runtime", L"VmcsV2RuntimeManager", L"VmcsManagerAdapter"};

	const std::vector<std::wstring> GeneratedProjectionLineageBuildContract::getRequiredVerifierFunctions()
	{
		return RequiredVerifierFunctions;
	}

	const std::vector<std::wstring> GeneratedProjectionLineageBuildContract::getRequiredGeneratedInputs()
	{
		return RequiredGeneratedInputs;
	}

	const std::vector<std::wstring> GeneratedProjectionLineageBuildContract::getRequiredGeneratedOutputs()
	{
		return RequiredGeneratedOutputs;
	}

	const std::vector<std::wstring> GeneratedProjectionLineageBuildContract::getRequiredDriftFailureMarkers()
	{
		return RequiredDriftFailureMarkers;
	}

	const std::vector<std::wstring> GeneratedProjectionLineageBuildContract::getForbiddenRuntimeOwnerMarkers()
	{
		return ForbiddenRuntimeOwnerMarkers;
	}

	bool GeneratedProjectionLineageBuildContract::getUsesBuildTimeTarget() const
	{
		return true;
	}

	bool GeneratedProjectionLineageBuildContract::getRegeneratesVmcsFieldProjectionSchema() const
	{
		return true;
	}

	bool GeneratedProjectionLineageBuildContract::getRegeneratesCompatAliasMap() const
	{
		return true;
	}

	bool GeneratedProjectionLineageBuildContract::getRegeneratesVmxCapsCapabilityBitSchema() const
	{
		return true;
	}

	bool GeneratedProjectionLineageBuildContract::getRegeneratesCompatSpecArtifactManifest() const
	{
		return true;
	}

	bool GeneratedProjectionLineageBuildContract::getFailsBuildOnGeneratedDrift() const
	{
		return true;
	}

	bool GeneratedProjectionLineageBuildContract::getLeavesRuntimeAuthorityUnchanged() const
	{
		return true;
	}

	bool GeneratedProjectionLineageBuildContract::getDoesNotCreateRuntimeOwner() const
	{
		return true;
	}

	bool GeneratedProjectionLineageBuildContract::IsSatisfied()
	{
		return getUsesBuildTimeTarget() && getRegeneratesVmcsFieldProjectionSchema() && getRegeneratesCompatAliasMap() && getRegeneratesVmxCapsCapabilityBitSchema() && getRegeneratesCompatSpecArtifactManifest() && getFailsBuildOnGeneratedDrift() && getLeavesRuntimeAuthorityUnchanged() && getDoesNotCreateRuntimeOwner();
	}
}
