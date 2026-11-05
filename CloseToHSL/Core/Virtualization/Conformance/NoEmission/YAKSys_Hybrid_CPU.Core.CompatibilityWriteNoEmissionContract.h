#pragma once

#include "../../Compatibility/Generated/CsrProjection/YAKSys_Hybrid_CPU.Core.VmxCapsProjection.h"
#include <memory>
#include <string>
#include <utility>

namespace YAKSys_Hybrid_CPU::Core
{
	enum class CompatibilityWriteNoEmissionDecision : unsigned char
	{
		Allowed = 0,
		UnsupportedWriteResult = 1,
		MissingCompatibilityFence = 2,
		MissingNoEffectPolicy = 3,
		DescriptorMutated = 4,
		GrantsMutated = 5,
		EvidenceMutated = 6,
		CompletionMutated = 7,
		MemoryGenerationMutated = 8,
		LaneStateMutated = 9,
		MigrationStateMutated = 10
	};

	struct CompatibilityWriteNoEmissionSnapshot final
	{
		const unsigned long long DescriptorFingerprint;
		const unsigned long long GrantFingerprint;
		const unsigned long long EvidenceFingerprint;
		const unsigned long long CompletionFingerprint;
		const unsigned long long MemoryGenerationFingerprint;
		const unsigned long long LaneStateFingerprint;
		const unsigned long long MigrationStateFingerprint;

		CompatibilityWriteNoEmissionSnapshot(unsigned long long descriptorFingerprint,
			unsigned long long grantFingerprint, unsigned long long evidenceFingerprint,
			unsigned long long completionFingerprint, unsigned long long memoryGenerationFingerprint,
			unsigned long long laneStateFingerprint, unsigned long long migrationStateFingerprint)
			: DescriptorFingerprint(descriptorFingerprint), GrantFingerprint(grantFingerprint),
			  EvidenceFingerprint(evidenceFingerprint), CompletionFingerprint(completionFingerprint),
			  MemoryGenerationFingerprint(memoryGenerationFingerprint), LaneStateFingerprint(laneStateFingerprint),
			  MigrationStateFingerprint(migrationStateFingerprint) {}
	};

	struct CompatibilityWriteNoEmissionResult final
	{
		const CompatibilityWriteNoEmissionDecision Decision;
		const std::wstring Reason;
		const bool IsAllowed;

		CompatibilityWriteNoEmissionResult(CompatibilityWriteNoEmissionDecision decision, std::wstring reason)
			: Decision(decision), Reason(std::move(reason)), IsAllowed(decision == CompatibilityWriteNoEmissionDecision::Allowed) {}

		static inline const std::shared_ptr<CompatibilityWriteNoEmissionResult> Allowed =
			std::make_shared<CompatibilityWriteNoEmissionResult>(CompatibilityWriteNoEmissionDecision::Allowed,
				L"Compatibility write has no emission path.");

		static std::shared_ptr<CompatibilityWriteNoEmissionResult> Denied(CompatibilityWriteNoEmissionDecision decision, std::wstring reason)
		{
			return std::make_shared<CompatibilityWriteNoEmissionResult>(decision, std::move(reason));
		}
	};

	class CompatibilityWriteNoEmissionContract final : public std::enable_shared_from_this<CompatibilityWriteNoEmissionContract>
	{
	public:
		std::shared_ptr<CompatibilityWriteNoEmissionResult> ValidateVmxCapsWrite(std::shared_ptr<VmxCapsProjection> projection, VmxCapsWriteResult writeResult);

		std::shared_ptr<CompatibilityWriteNoEmissionResult> ValidateNoMutation(std::shared_ptr<VmxCapsProjection> projection, VmxCapsWriteResult writeResult, std::shared_ptr<CompatibilityWriteNoEmissionSnapshot> before, std::shared_ptr<CompatibilityWriteNoEmissionSnapshot> after);

		bool IsVmxCapsWriteSatisfied(std::shared_ptr<VmxCapsProjection> projection, VmxCapsWriteResult writeResult);

		bool IsNoMutationSatisfied(std::shared_ptr<VmxCapsProjection> projection, VmxCapsWriteResult writeResult, std::shared_ptr<CompatibilityWriteNoEmissionSnapshot> before, std::shared_ptr<CompatibilityWriteNoEmissionSnapshot> after);
	};
}
