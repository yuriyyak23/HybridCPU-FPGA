#pragma once

#include "TelemetryState.h"
#include "AssistState.h"
#include "ScratchState.h"
#include "CacheState.h"
#include "ResourceState.h"
#include "VirtualThreadControlState.h"
#include "../../../Legacy/CloseToHSL/Core/State/LegacyCompatibilityState.h"
#include "CoreBindingState.h"
#include "FrontendState.h"
#include "DecodeState.h"
#include "AdmissionState.h"
#include "ReplayState.h"
#include "RetireState.h"
#include "ArchitecturalState.h"
#include "SchedulingState.h"
#include "ExecutionState.h"
#include "MemoryPipelineState.h"
#include "CpuInstructionTranslationState.h"
#include "CompatibilityControlPolicyState.h"
#include "BackendState.h"
#include "ExtensionState.h"
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{

	/// <summary>
	/// Stable reference identity and containment root for one constructed CPU core.
	/// Containment does not transfer execution, commit, rollback or publication
	/// authority from the existing state owners.
	/// </summary>
	class CoreRuntimeState final : public std::enable_shared_from_this<CoreRuntimeState>
	{
	private:
		const std::shared_ptr<TelemetryState> Telemetry;
		const std::shared_ptr<AssistState> Assist;
		const std::shared_ptr<ScratchState> Scratch;
		const std::shared_ptr<CacheState> Cache;
		const std::shared_ptr<ResourceState> Resources;
		const std::shared_ptr<VirtualThreadControlState> VirtualThreadControl;
		const std::shared_ptr<LegacyCompatibilityState> LegacyCompatibility;
		const std::shared_ptr<CoreBindingState> Binding;
		const std::shared_ptr<FrontendState> Frontend;
		const std::shared_ptr<DecodeState> Decode;
		const std::shared_ptr<AdmissionState> Admission;
		const std::shared_ptr<ReplayState> Replay;
		const std::shared_ptr<RetireState> Retire;
		const std::shared_ptr<ArchitecturalState> Architectural;
		const std::shared_ptr<SchedulingState> Scheduling;
		const std::shared_ptr<ExecutionState> Execution;
		const std::shared_ptr<MemoryPipelineState> MemoryPipeline;
		const std::shared_ptr<CpuInstructionTranslationState> CpuInstructionTranslation;
		const std::shared_ptr<CompatibilityControlPolicyState> CompatibilityControlPolicy;
		const std::shared_ptr<BackendState> Backend;
		const std::shared_ptr<ExtensionState> Extensions;

	public:
		CoreRuntimeState();

		std::shared_ptr<TelemetryState> getTelemetry() const;

		std::shared_ptr<AssistState> getAssist() const;

		std::shared_ptr<ScratchState> getScratch() const;

		std::shared_ptr<CacheState> getCache() const;

		std::shared_ptr<ResourceState> getResources() const;

		std::shared_ptr<VirtualThreadControlState> getVirtualThreadControl() const;

		std::shared_ptr<LegacyCompatibilityState> getLegacyCompatibility() const;

		std::shared_ptr<CoreBindingState> getBinding() const;

		std::shared_ptr<FrontendState> getFrontend() const;

		std::shared_ptr<DecodeState> getDecode() const;

		std::shared_ptr<AdmissionState> getAdmission() const;

		std::shared_ptr<ReplayState> getReplay() const;

		std::shared_ptr<RetireState> getRetire() const;

		std::shared_ptr<ArchitecturalState> getArchitectural() const;

		std::shared_ptr<SchedulingState> getScheduling() const;

		std::shared_ptr<ExecutionState> getExecution() const;

		std::shared_ptr<MemoryPipelineState> getMemoryPipeline() const;

		std::shared_ptr<CpuInstructionTranslationState> getCpuInstructionTranslation() const;

		std::shared_ptr<CompatibilityControlPolicyState> getCompatibilityControlPolicy() const;

		std::shared_ptr<BackendState> getBackend() const;

		std::shared_ptr<ExtensionState> getExtensions() const;
	};

}
