#include "YAKSys_Hybrid_CPU.Core.CompatibilityWriteNoEmissionContract.h"

namespace YAKSys_Hybrid_CPU::Core
{

	std::shared_ptr<CompatibilityWriteNoEmissionResult> CompatibilityWriteNoEmissionContract::ValidateVmxCapsWrite(std::shared_ptr<VmxCapsProjection> projection, VmxCapsWriteResult writeResult)
	{
		if (writeResult == VmxCapsWriteResult::Rejected)
		{
			return CompatibilityWriteNoEmissionResult::Allowed;
		}

		if (writeResult != VmxCapsWriteResult::CompatibilityNoEffect)
		{
			return CompatibilityWriteNoEmissionResult::Denied(CompatibilityWriteNoEmissionDecision::UnsupportedWriteResult, L"VmxCaps compatibility write produced an unsupported result.");
		}

		if (!projection->getCompatibilityWriteFenceEnabled())
		{
			return CompatibilityWriteNoEmissionResult::Denied(CompatibilityWriteNoEmissionDecision::MissingCompatibilityFence, L"Compatibility no-effect write requires an explicit compatibility write fence.");
		}

		if (!projection->getPublicationPolicy()->IsCompatibilityWriteArchitecturallyNoEffect())
		{
			return CompatibilityWriteNoEmissionResult::Denied(CompatibilityWriteNoEmissionDecision::MissingNoEffectPolicy, L"Compatibility no-effect write requires a no-effect publication policy.");
		}

		return CompatibilityWriteNoEmissionResult::Allowed;
	}

	std::shared_ptr<CompatibilityWriteNoEmissionResult> CompatibilityWriteNoEmissionContract::ValidateNoMutation(std::shared_ptr<VmxCapsProjection> projection, VmxCapsWriteResult writeResult, std::shared_ptr<CompatibilityWriteNoEmissionSnapshot> before, std::shared_ptr<CompatibilityWriteNoEmissionSnapshot> after)
	{
		std::shared_ptr<CompatibilityWriteNoEmissionResult> writePolicyResult = ValidateVmxCapsWrite(projection, writeResult);

		if (!writePolicyResult->IsAllowed)
		{
			return writePolicyResult;
		}

		if (before->DescriptorFingerprint != after->DescriptorFingerprint)
		{
			return CompatibilityWriteNoEmissionResult::Denied(CompatibilityWriteNoEmissionDecision::DescriptorMutated, L"Compatibility write mutated descriptor-owned state.");
		}

		if (before->GrantFingerprint != after->GrantFingerprint)
		{
			return CompatibilityWriteNoEmissionResult::Denied(CompatibilityWriteNoEmissionDecision::GrantsMutated, L"Compatibility write mutated capability grants.");
		}

		if (before->EvidenceFingerprint != after->EvidenceFingerprint)
		{
			return CompatibilityWriteNoEmissionResult::Denied(CompatibilityWriteNoEmissionDecision::EvidenceMutated, L"Compatibility write mutated evidence state.");
		}

		if (before->CompletionFingerprint != after->CompletionFingerprint)
		{
			return CompatibilityWriteNoEmissionResult::Denied(CompatibilityWriteNoEmissionDecision::CompletionMutated, L"Compatibility write mutated completion state.");
		}

		if (before->MemoryGenerationFingerprint != after->MemoryGenerationFingerprint)
		{
			return CompatibilityWriteNoEmissionResult::Denied(CompatibilityWriteNoEmissionDecision::MemoryGenerationMutated, L"Compatibility write mutated memory-domain generation.");
		}

		if (before->LaneStateFingerprint != after->LaneStateFingerprint)
		{
			return CompatibilityWriteNoEmissionResult::Denied(CompatibilityWriteNoEmissionDecision::LaneStateMutated, L"Compatibility write mutated lane-owned state.");
		}

		if (before->MigrationStateFingerprint != after->MigrationStateFingerprint)
		{
			return CompatibilityWriteNoEmissionResult::Denied(CompatibilityWriteNoEmissionDecision::MigrationStateMutated, L"Compatibility write mutated migration-owned state.");
		}

		return CompatibilityWriteNoEmissionResult::Allowed;
	}

	bool CompatibilityWriteNoEmissionContract::IsVmxCapsWriteSatisfied(std::shared_ptr<VmxCapsProjection> projection, VmxCapsWriteResult writeResult)
	{
		return ValidateVmxCapsWrite(projection, writeResult)->IsAllowed;
	}

	bool CompatibilityWriteNoEmissionContract::IsNoMutationSatisfied(std::shared_ptr<VmxCapsProjection> projection, VmxCapsWriteResult writeResult, std::shared_ptr<CompatibilityWriteNoEmissionSnapshot> before, std::shared_ptr<CompatibilityWriteNoEmissionSnapshot> after)
	{
		return ValidateNoMutation(projection, writeResult, before, after)->IsAllowed;
	}
}
