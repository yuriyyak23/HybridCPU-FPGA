#pragma once

#include "../Architecture/Common/YAKSys_Hybrid_CPU.Processor.h"
#include "../Decoder/DecodedBundleDescriptor.h"
#include "../Decoder/ClusterIssuePreparation.h"
#include "../Architecture/Registers/Architectural/ArchContextState.h"
#include <optional>
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{
	/// <summary>
	/// Reference-owned containment for the current decode latch and bundle-scoped
	/// decode progress. Admission, scheduling and publication authority remain with
	/// their existing owners.
	/// </summary>
	class DecodeState final : public std::enable_shared_from_this<DecodeState>
	{
	public:
		DecodeState();

		Processor::CPU_Core::DecodeStage Decode;
		unsigned char PipelineBundleSlot = 0;
		DecodedBundleRuntimeState BundleRuntime;
		BundleProgressState BundleProgress;
		DecodedBundleDerivedIssuePlanState DerivedIssuePlan;
		unsigned long long BundleStateEpochCounter = 0;
		unsigned long long BundleStateVersionCounter = 0;
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
// ORIGINAL LINE: internal ClusterIssuePreparation ClusterPreparation = null!;
		std::shared_ptr<ClusterIssuePreparation> ClusterPreparation = nullptr;
		bool BundleDecodedAndPacked = false;
		// Pending refresh belongs to the existing foreground progress owner.
		// It is not a replay entry, scheduler admission or issued-attempt permit.
		std::optional<std::shared_ptr<Decoder::CanonicalDecodeContext>> SourceRefreshContext;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal Registers.ArchContextState? SourceRefreshArchitecturalOwner;
		std::shared_ptr<Registers::ArchContextState> SourceRefreshArchitecturalOwner;
	};
}
