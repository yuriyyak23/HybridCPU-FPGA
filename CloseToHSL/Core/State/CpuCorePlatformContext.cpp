#include "CpuCorePlatformContext.h"
#include "../Execution/Memory/AtomicMemory/YAKSys_Hybrid_CPU.Core.Memory.MainMemoryAtomicMemoryUnit.h"
#include "../Pipeline/Assist/InterCore/YAKSys_Hybrid_CPU.Core.MicroOpScheduler.h"

using namespace YAKSys_Hybrid_CPU::Core;
using namespace YAKSys_Hybrid_CPU::Core::Memory;

namespace YAKSys_Hybrid_CPU
{
	std::shared_ptr<Core::Memory::IAtomicMemoryUnit> Core::CpuCorePlatformContext::CreateAtomicMemoryUnit() const
	{
		if (!MainMemory)
		{
			throw std::invalid_argument("A configured CPU core requires a main-memory binding.");
		}
		return std::make_shared<Core::Memory::MainMemoryAtomicMemoryUnit>(MainMemory);
	}

	Core::CpuCorePlatformContext Core::CpuCorePlatformContext::CreateLegacy(std::shared_ptr<Processor::MainMemoryArea> mainMemory)
	{
		Core::CpuCorePlatformContext context;
		context.MainMemory = mainMemory ? std::move(mainMemory) : Processor::MainMemory;
		context.MemorySubsystem = Processor::Memory;
		context.InitialExecutionMode = Processor::CurrentProcessorMode;
		context.TrackGlobalExecutionMode = true;
		context.CpuInstructionTranslationPolicy = Core::Memory::CpuInstructionTranslationPolicy::getIdentity();
		context.IsConfigured = true;
		return context;
	}

	Core::CpuCorePlatformContext Core::CpuCorePlatformContext::CreateFixed(std::shared_ptr<Processor::MainMemoryArea> mainMemory, ProcessorMode executionMode, std::shared_ptr<IHybridCpuIseEcallBridgeV1> ecallBridge)
	{
		if (!mainMemory) throw std::invalid_argument("A fixed CPU core requires an explicit main-memory binding.");
		Core::CpuCorePlatformContext context;
		context.MainMemory = std::move(mainMemory);
		context.InitialExecutionMode = executionMode;
		context.TrackGlobalExecutionMode = false;
		context.CpuInstructionTranslationPolicy = Core::Memory::CpuInstructionTranslationPolicy::getIdentity();
		context.EcallBridge = std::move(ecallBridge);
		context.IsConfigured = true;
		return context;
	}

	Core::CpuCorePlatformContext Core::CpuCorePlatformContext::CreateSoftwareSecureFixed(std::shared_ptr<Processor::MainMemoryArea> mainMemory, ProcessorMode executionMode, std::shared_ptr<IHybridCpuIseEcallBridgeV1> ecallBridge)
	{
		Core::CpuCorePlatformContext context = CreateFixed(std::move(mainMemory), executionMode, std::move(ecallBridge));
		context.OwnedSoftwareSecureScheduler = std::make_shared<Core::MicroOpScheduler>();
		context.OwnedSoftwareSecureScheduler->setTypedSlotEnabled(true);
		return context;
	}
}
