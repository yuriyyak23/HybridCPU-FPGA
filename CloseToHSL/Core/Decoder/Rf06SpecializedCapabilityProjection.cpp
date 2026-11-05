
#include "Rf06SpecializedCapabilityProjection.h"
#include "../Pipeline/MicroOps/Assist/AssistMicroOp.h"
#include "../Pipeline/MicroOps/Control/MicroOp.Control.h"
#include "../Execution/Memory/LoadStore/MemoryBankRouting.h"
#include "../Pipeline/Scheduling/SlotLegality/SlotClassDefinitions.h"
#include "../ISA/Instructions/NonVmx/Lanes00_03Vector/MatrixTile/MatrixTileIrProjectionAndMaterializer.h"
#include "../ISA/Instructions/NonVmx/Lanes00_03Vector/MatrixTile/MatrixTileResourceContour.h"
#include "../Architecture/Common/YAKSys_Hybrid_CPU.Processor.h"
#include <algorithm>
#include <optional>
#include <tuple>

using namespace YAKSys_Hybrid_CPU::Arch;
using namespace YAKSys_Hybrid_CPU::CloseToHSL::Core::ISA::Instructions::NonVmx::Lanes00_03Vector::MatrixTile;
using namespace YAKSys_Hybrid_CPU::Core::Execution::DmaStreamCompute;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Descriptors;
using namespace YAKSys_Hybrid_CPU::Core::Memory;
using namespace YAKSys_Hybrid_CPU::Core::Pipeline::MicroOps;

namespace YAKSys_Hybrid_CPU::Core::Decoder
{

	std::shared_ptr<Rf06ControlCapability> Rf06SpecializedCapabilityProjection::ProjectControl(std::shared_ptr<MicroOp> carrier, std::shared_ptr<GeneratedStaticBinding> binding)
	{
		ValidateBinding(carrier, binding);
		if (std::dynamic_pointer_cast<BranchMicroOp>(carrier) == nullptr || !carrier->isControlFlow() || carrier->getInstructionClass() != InstructionClass::ControlFlow)
		{
			throw InvalidOperationException(L"RF-06.5 control projection requires a BranchMicroOp control-flow carrier.");
		}

		std::shared_ptr<ExecutionContract> contract = CreateContract(carrier, binding, ControlPayloadSchema, L"PcWrite", MemoryCapability::None, false, carrier->isRetireVisible(), false);
		std::shared_ptr<BranchMicroOp> branch = std::static_pointer_cast<BranchMicroOp>(carrier);

		return std::make_shared<Rf06ControlCapability>(contract, branch->isConditional(), true, carrier->isStealable(), carrier->getPlacement().RequiredSlotClass, carrier->getPlacement().PinningKind, carrier->getPlacement().PinnedLaneId);
	}

	std::shared_ptr<Rf06SerializingCapability> Rf06SpecializedCapabilityProjection::ProjectSerializing(std::shared_ptr<MicroOp> carrier, std::shared_ptr<GeneratedStaticBinding> binding)
	{
		ValidateBinding(carrier, binding);
		if (carrier->isAssist() || carrier->isControlFlow() || carrier->getSerializationClass() == SerializationClass::Free)
		{
			throw InvalidOperationException(L"RF-06.5 serializing projection requires a non-assist, non-control carrier with an explicit serial boundary.");
		}

		std::shared_ptr<ExecutionContract> contract = CreateContract(carrier, binding, SerializingPayloadSchema, L"SerializingEffect", BuildMemoryCapability(carrier), false, carrier->isRetireVisible(), true);

		return std::make_shared<Rf06SerializingCapability>(contract, carrier->getInstructionClass(), carrier->getSerializationClass(), carrier->isControlFlow(), carrier->getHasSideEffects(), carrier->getPlacement().RequiredSlotClass, carrier->getPlacement().PinningKind, carrier->getPlacement().PinnedLaneId);
	}

	std::shared_ptr<Rf06AssistCapability> Rf06SpecializedCapabilityProjection::ProjectAssist(std::shared_ptr<AssistMicroOp> carrier, std::shared_ptr<GeneratedStaticBinding> binding)
	{
		ArgumentNullException::ThrowIfNull(carrier);
		ArgumentNullException::ThrowIfNull(binding);
		const auto &readRanges = carrier->getReadMemoryRanges();
		if (!carrier->isAssist() || carrier->isRetireVisible() || readRanges.size() != 1 || !carrier->getHasResolvedMemoryBankId())
		{
			throw InvalidOperationException(L"RF-06.5 assist projection requires one resolved read footprint and a non-retire-visible assist carrier.");
		}

		const auto &readRange = readRanges.front();
		FrozenMemoryRange footprint(readRange.Address, readRange.Length);
		MemoryBankId bank(carrier->getMemoryBankId());
		std::vector<FrozenMemoryRange> footprintRanges{footprint};
		std::shared_ptr<ExecutionContract> contract = CreateContract(carrier, binding, AssistPayloadSchema, L"AssistPrefetch", MemoryCapability::Create(MemoryCapabilityKind::Load, footprintRanges, bank), true, false, false);

		return std::make_shared<Rf06AssistCapability>(contract, carrier->getKind(), carrier->getExecutionMode(), carrier->getCarrierKind(), carrier->getDonorSource().getKind(), carrier->getCarrierVirtualThreadId(), carrier->getDonorVirtualThreadId(), carrier->getTargetVirtualThreadId(), carrier->getCarrierCoreId(), carrier->getTargetCoreId(), carrier->getPodId(), carrier->getPlacement().DomainTag, footprint, bank);
	}

	std::shared_ptr<Rf06DmaCapability> Rf06SpecializedCapabilityProjection::ProjectDma(std::shared_ptr<DmaStreamComputeMicroOp> carrier, std::shared_ptr<GeneratedStaticBinding> binding)
	{
		ArgumentNullException::ThrowIfNull(carrier);
		ArgumentNullException::ThrowIfNull(binding);
		if (carrier->getPlacement().RequiredSlotClass != SlotClass::DmaStreamClass || carrier->getPlacement().PinningKind != SlotPinningKind::HardPinned || carrier->getPlacement().PinnedLaneId != 6 || carrier->getReadMemoryRanges().empty() || carrier->getWriteMemoryRanges().empty())
		{
			throw InvalidOperationException(L"RF-06.5 DMA projection requires the existing lane6 hard-pinned carrier and both immutable footprints.");
		}

		std::shared_ptr<ExecutionContract> contract = CreateContract(carrier, binding, DmaPayloadSchema, L"DmaCommit", MemoryCapability::None, false, carrier->isRetireVisible(), false);

		const auto descriptorReference = carrier->getDescriptorReference();
		const auto descriptor = carrier->getDescriptor();
		const auto ownerBinding = carrier->getOwnerBinding();
		if (!descriptorReference || !descriptor || !ownerBinding)
		{
			throw InvalidOperationException(L"RF-06.5 DMA projection requires complete descriptor and owner metadata.");
		}
		return std::make_shared<Rf06DmaCapability>(contract, *descriptorReference, carrier->getDescriptorIdentityHash(), carrier->getCertificateInputHash(), carrier->getNormalizedFootprintHash(), carrier->getOperation(), carrier->getElementType(), carrier->getShape(), carrier->getRangeEncoding(), carrier->getPartialCompletionPolicy(), descriptor->AliasPolicy, ownerBinding->OwnerVirtualThreadId, ownerBinding->OwnerContextId, ownerBinding->OwnerCoreId, ownerBinding->OwnerPodId, ownerBinding->OwnerDomainTag, FreezeRanges(carrier->getReadMemoryRanges()), FreezeRanges(carrier->getWriteMemoryRanges()));
	}

	std::shared_ptr<Rf06AcceleratorCapability> Rf06SpecializedCapabilityProjection::ProjectAccelerator(std::shared_ptr<SystemDeviceCommandMicroOp> carrier, std::shared_ptr<GeneratedStaticBinding> binding)
	{
		ValidateBinding(carrier, binding);
		if (carrier->getPlacement().RequiredSlotClass != SlotClass::SystemSingleton || carrier->getPlacement().PinningKind != SlotPinningKind::HardPinned || carrier->getPlacement().PinnedLaneId != 7 || carrier->isStealable())
		{
			throw InvalidOperationException(L"RF-06.5 accelerator projection requires the non-stealable lane7 system-singleton carrier.");
		}

		std::shared_ptr<ExecutionContract> contract = CreateContract(carrier, binding, AcceleratorPayloadSchema, L"AcceleratorCommand", MemoryCapability::None, false, carrier->isRetireVisible(), true);

		const auto commandDescriptor = carrier->getCommandDescriptor();
		const auto descriptorReference = carrier->getCommandDescriptorReference();
		const bool hasDescriptor = commandDescriptor.has_value() && commandDescriptor.value() != nullptr;
		return std::make_shared<Rf06AcceleratorCapability>(contract, carrier->getCommandKind(), carrier->getDestinationRegister(), carrier->getTokenRegister(), hasDescriptor, descriptorReference, carrier->getPlacement().RequiredSlotClass, carrier->getPlacement().PinningKind, carrier->getPlacement().PinnedLaneId);
	}

	std::shared_ptr<Rf06MatrixTileCapability> Rf06SpecializedCapabilityProjection::ProjectMatrixTile(std::shared_ptr<MatrixTileMicroOp> carrier, std::shared_ptr<GeneratedStaticBinding> binding)
	{
		ValidateBinding(carrier, binding);
		if (carrier->getOperationKind() == MatrixTileProjectedOperationKind::Unspecified || carrier->getRuntimeResourceClass() == MatrixTileRuntimeResourceClass::None || carrier->getPlacement().RequiredSlotClass == SlotClass::Unclassified)
		{
			throw InvalidOperationException(L"RF-06.5 MatrixTile projection requires a legal typed MatrixTile carrier.");
		}

		const bool isMemoryOperation = carrier->getOperationKind() == MatrixTileProjectedOperationKind::Load ||
			carrier->getOperationKind() == MatrixTileProjectedOperationKind::Store;
		std::shared_ptr<ExecutionContract> contract = CreateContract(carrier, binding, MatrixTilePayloadSchema,
			isMemoryOperation ? L"MatrixTileMemoryCommit" : L"MatrixTileCommit",
			BuildMemoryCapability(carrier), false, carrier->isRetireVisible(), true);
		std::shared_ptr<MatrixTileMicroOpDependencyMetadata> dependency = carrier->getDependencyMetadata();
		if (!dependency)
		{
			throw InvalidOperationException(L"RF-06.5 MatrixTile projection requires dependency metadata.");
		}

		return std::make_shared<Rf06MatrixTileCapability>(contract, carrier->getOperationKind(), carrier->getRuntimeResourceClass(), dependency->ReadsTileState, dependency->WritesTileState, dependency->ReadsAccumulator, dependency->WritesAccumulator, dependency->HasTransposePolicyDependencyMetadata, carrier->getPlacement().RequiredSlotClass, carrier->getPlacement().PinningKind, carrier->getPlacement().PinnedLaneId);
	}

	std::shared_ptr<ExecutionContract> Rf06SpecializedCapabilityProjection::CreateContract(std::shared_ptr<MicroOp> carrier, std::shared_ptr<GeneratedStaticBinding> binding, const std::wstring &payloadSchema, const std::wstring &staticEffectContract, std::shared_ptr<MemoryCapability> memory, bool isAssist, bool isRetireVisible, bool validateOpcode)
	{
		ArgumentNullException::ThrowIfNull(carrier);
		ArgumentNullException::ThrowIfNull(binding);
		if (validateOpcode && binding->Opcode != carrier->getOpCode())
		{
			throw InvalidOperationException(L"RF-06.5 projection received a generated binding for a different materialized carrier.");
		}

		return ExecutionContract::Create(binding, std::make_shared<RuntimeExecutionProviderBinding>(binding->RuntimeExecutionProviderId, payloadSchema), carrier->getInstructionClass(), carrier->getSerializationClass(), ExecutionPlacement::Create(carrier->getPlacement().RequiredSlotClass, carrier->getPlacement().PinningKind, carrier->getPlacement().PinnedLaneId, carrier->getPlacement().DomainTag), staticEffectContract, memory, carrier->getReadRegisters(), carrier->getWriteRegisters(), carrier->getResourceMask(), carrier->isStealable(), isRetireVisible, isAssist);
	}

	void Rf06SpecializedCapabilityProjection::ValidateBinding(std::shared_ptr<MicroOp> carrier, std::shared_ptr<GeneratedStaticBinding> binding)
	{
		ArgumentNullException::ThrowIfNull(carrier);
		ArgumentNullException::ThrowIfNull(binding);
		if (binding->Opcode != carrier->getOpCode())
		{
			throw InvalidOperationException(L"RF-06.5 projection requires the exact generated binding carried by the canonical handoff.");
		}
	}

	std::shared_ptr<MemoryCapability> Rf06SpecializedCapabilityProjection::BuildMemoryCapability(std::shared_ptr<MicroOp> carrier)
	{
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: IReadOnlyList<(ulong Address, ulong Length)> reads = carrier.ReadMemoryRanges;
		const auto &reads = carrier->getReadMemoryRanges();
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: IReadOnlyList<(ulong Address, ulong Length)> writes = carrier.WriteMemoryRanges;
		const auto &writes = carrier->getWriteMemoryRanges();
		if (reads.empty() && writes.empty())
		{
			return MemoryCapability::None;
		}

		MemoryCapabilityKind kind = reads.size() > 0 && writes.size() > 0 ? MemoryCapabilityKind::Atomic : reads.size() > 0 ? MemoryCapabilityKind::Load : MemoryCapabilityKind::Store;
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: IReadOnlyList<(ulong Address, ulong Length)> selected = reads.Count > 0 ? reads : writes;
		const auto &selected = reads.empty() ? writes : reads;
		std::vector<FrozenMemoryRange> footprint = FreezeRanges(selected);
		int bankValue = MemoryBankRouting::ResolveSchedulerVisibleBankId(footprint.front().Address);
		if (!MemoryBankRouting::IsResolvedSchedulerVisibleBankId(bankValue))
		{
			throw InvalidOperationException(L"RF-06.5 memory-bearing family projection requires a resolved scheduler-visible bank.");
		}
		return MemoryCapability::Create(kind, footprint, MemoryBankId(bankValue));
	}

	std::vector<FrozenMemoryRange> Rf06SpecializedCapabilityProjection::FreezeRanges(const std::vector<MicroOpMemoryRange> &ranges)
	{
		std::vector<FrozenMemoryRange> frozen;
		frozen.reserve(ranges.size());
		for (const auto &range : ranges)
		{
			frozen.emplace_back(range.Address, range.Length);
		}
		std::stable_sort(frozen.begin(), frozen.end(), [](const auto &left, const auto &right)
		{
			return std::tie(left.Address, left.Length) < std::tie(right.Address, right.Length);
		});
		for (std::size_t index = 1; index < frozen.size(); index++)
		{
			if (frozen[index].Address <= frozen[index - 1].getLastAddress())
			{
				throw InvalidOperationException(L"RF-06.5 family footprint contains overlapping normalized ranges.");
			}
		}

		return frozen;
	}
}
