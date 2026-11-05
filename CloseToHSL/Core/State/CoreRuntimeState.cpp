#include "CoreRuntimeState.h"

namespace YAKSys_Hybrid_CPU::Core
{

	CoreRuntimeState::CoreRuntimeState()
		: Telemetry(std::make_shared<TelemetryState>()),
		  Assist(std::make_shared<AssistState>()),
		  Scratch(std::make_shared<ScratchState>()),
		  Cache(std::make_shared<CacheState>()),
		  Resources(std::make_shared<ResourceState>()),
		  VirtualThreadControl(std::make_shared<VirtualThreadControlState>()),
		  LegacyCompatibility(std::make_shared<LegacyCompatibilityState>()),
		  Binding(std::make_shared<CoreBindingState>()),
		  Frontend(std::make_shared<FrontendState>()),
		  Decode(std::make_shared<DecodeState>()),
		  Admission(std::make_shared<AdmissionState>()),
		  Replay(std::make_shared<ReplayState>()),
		  Retire(std::make_shared<RetireState>()),
		  Architectural(std::make_shared<ArchitecturalState>()),
		  Scheduling(std::make_shared<SchedulingState>()),
		  Execution(std::make_shared<ExecutionState>()),
		  MemoryPipeline(std::make_shared<MemoryPipelineState>()),
		  CpuInstructionTranslation(std::make_shared<CpuInstructionTranslationState>()),
		  CompatibilityControlPolicy(std::make_shared<CompatibilityControlPolicyState>()),
		  Backend(std::make_shared<BackendState>()),
		  Extensions(std::make_shared<ExtensionState>())
	{
	}

	std::shared_ptr<TelemetryState> CoreRuntimeState::getTelemetry() const
	{
		return Telemetry;
	}

	std::shared_ptr<AssistState> CoreRuntimeState::getAssist() const
	{
		return Assist;
	}

	std::shared_ptr<ScratchState> CoreRuntimeState::getScratch() const
	{
		return Scratch;
	}

	std::shared_ptr<CacheState> CoreRuntimeState::getCache() const
	{
		return Cache;
	}

	std::shared_ptr<ResourceState> CoreRuntimeState::getResources() const
	{
		return Resources;
	}

	std::shared_ptr<VirtualThreadControlState> CoreRuntimeState::getVirtualThreadControl() const
	{
		return VirtualThreadControl;
	}

	std::shared_ptr<LegacyCompatibilityState> CoreRuntimeState::getLegacyCompatibility() const
	{
		return LegacyCompatibility;
	}

	std::shared_ptr<CoreBindingState> CoreRuntimeState::getBinding() const
	{
		return Binding;
	}

	std::shared_ptr<FrontendState> CoreRuntimeState::getFrontend() const
	{
		return Frontend;
	}

	std::shared_ptr<DecodeState> CoreRuntimeState::getDecode() const
	{
		return Decode;
	}

	std::shared_ptr<AdmissionState> CoreRuntimeState::getAdmission() const
	{
		return Admission;
	}

	std::shared_ptr<ReplayState> CoreRuntimeState::getReplay() const
	{
		return Replay;
	}

	std::shared_ptr<RetireState> CoreRuntimeState::getRetire() const
	{
		return Retire;
	}

	std::shared_ptr<ArchitecturalState> CoreRuntimeState::getArchitectural() const
	{
		return Architectural;
	}

	std::shared_ptr<SchedulingState> CoreRuntimeState::getScheduling() const
	{
		return Scheduling;
	}

	std::shared_ptr<ExecutionState> CoreRuntimeState::getExecution() const
	{
		return Execution;
	}

	std::shared_ptr<MemoryPipelineState> CoreRuntimeState::getMemoryPipeline() const
	{
		return MemoryPipeline;
	}

	std::shared_ptr<CpuInstructionTranslationState> CoreRuntimeState::getCpuInstructionTranslation() const
	{
		return CpuInstructionTranslation;
	}

	std::shared_ptr<CompatibilityControlPolicyState> CoreRuntimeState::getCompatibilityControlPolicy() const
	{
		return CompatibilityControlPolicy;
	}

	std::shared_ptr<BackendState> CoreRuntimeState::getBackend() const
	{
		return Backend;
	}

	std::shared_ptr<ExtensionState> CoreRuntimeState::getExtensions() const
	{
		return Extensions;
	}
}
