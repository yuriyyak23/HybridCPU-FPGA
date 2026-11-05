#pragma once

#include "../Pipeline/MicroOps/Types/MicroOp.h"
namespace YAKSys_Hybrid_CPU::Core { class AssistMicroOp; }
#include "../Pipeline/MicroOps/Lane6DmaStream/DmaStreamComputeMicroOp.h"
#include "../Pipeline/MicroOps/Lane7Accelerator/SystemDeviceCommandMicroOp.h"
#include "../Pipeline/MicroOps/MatrixTile/MatrixTileMicroOps.h"
#include "Rf06ExecutionContracts.h"
#include <string>
#include <vector>
#include <optional>
#include <utility>
#include "tangible_exception_helper.h"
#include <memory>

using namespace YAKSys_Hybrid_CPU::Arch;
using namespace YAKSys_Hybrid_CPU::CloseToHSL::Core::ISA::Instructions::NonVmx::Lanes00_03Vector::MatrixTile;
using namespace YAKSys_Hybrid_CPU::Core::Execution::DmaStreamCompute;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Descriptors;
using namespace YAKSys_Hybrid_CPU::Core::Memory;
using namespace YAKSys_Hybrid_CPU::Core::Pipeline::MicroOps;

namespace YAKSys_Hybrid_CPU::Core::Decoder
{

	/// <summary>
	/// RF-06.5 family identity. This is a static capability taxonomy, not an
	/// execution-state or retirement taxonomy.
	/// </summary>
	enum class Rf06CapabilityFamily : unsigned char
	{
		ControlFlow = 1,
		Serializing = 2,
		Assist = 3,
		Dma = 4,
		Accelerator = 5,
		MatrixTile = 6
	};

	// Immutable C++ counterparts of the RF-06 capability records.  Footprints
	// own value copies so callers cannot mutate the projected snapshot through
	// the source carrier's vectors.
	struct Rf06ControlCapability final
	{
		const std::shared_ptr<ExecutionContract> Contract;
		const bool IsConditional;
		const bool RedirectsProgramCounter;
		const bool IsStealable;
		const SlotClass RequiredSlotClass;
		const SlotPinningKind PinningKind;
		const unsigned char PinnedLaneId;

		Rf06ControlCapability(std::shared_ptr<ExecutionContract> contract, bool isConditional,
			bool redirectsProgramCounter, bool isStealable, SlotClass requiredSlotClass,
			SlotPinningKind pinningKind, unsigned char pinnedLaneId)
			: Contract(std::move(contract)), IsConditional(isConditional),
			  RedirectsProgramCounter(redirectsProgramCounter), IsStealable(isStealable),
			  RequiredSlotClass(requiredSlotClass), PinningKind(pinningKind), PinnedLaneId(pinnedLaneId) {}
	};

	struct Rf06SerializingCapability final
	{
		const std::shared_ptr<ExecutionContract> Contract;
		const InstructionClass InstructionClass;
		const SerializationClass SerializationClass;
		const bool IsControlFlow;
		const bool HasStaticSideEffects;
		const SlotClass RequiredSlotClass;
		const SlotPinningKind PinningKind;
		const unsigned char PinnedLaneId;

		Rf06SerializingCapability(std::shared_ptr<ExecutionContract> contract,
			YAKSys_Hybrid_CPU::Arch::InstructionClass instructionClass,
			YAKSys_Hybrid_CPU::Arch::SerializationClass serializationClass,
			bool isControlFlow, bool hasStaticSideEffects, SlotClass requiredSlotClass,
			SlotPinningKind pinningKind, unsigned char pinnedLaneId)
			: Contract(std::move(contract)), InstructionClass(instructionClass),
			  SerializationClass(serializationClass), IsControlFlow(isControlFlow),
			  HasStaticSideEffects(hasStaticSideEffects), RequiredSlotClass(requiredSlotClass),
			  PinningKind(pinningKind), PinnedLaneId(pinnedLaneId) {}
	};

	struct Rf06AssistCapability final
	{
		const std::shared_ptr<ExecutionContract> Contract;
		const AssistKind Kind;
		const AssistExecutionMode ExecutionMode;
		const AssistCarrierKind CarrierKind;
		const AssistDonorSourceKind DonorSourceKind;
		const int CarrierVirtualThreadId;
		const int DonorVirtualThreadId;
		const int TargetVirtualThreadId;
		const int CarrierCoreId;
		const int TargetCoreId;
		const unsigned short PodId;
		const unsigned long long DomainTag;
		const FrozenMemoryRange PrefetchFootprint;
		const MemoryBankId Bank;

		Rf06AssistCapability(std::shared_ptr<ExecutionContract> contract, AssistKind kind,
			AssistExecutionMode executionMode, AssistCarrierKind carrierKind,
			AssistDonorSourceKind donorSourceKind, int carrierVirtualThreadId,
			int donorVirtualThreadId, int targetVirtualThreadId, int carrierCoreId,
			int targetCoreId, unsigned short podId, unsigned long long domainTag,
			FrozenMemoryRange prefetchFootprint, MemoryBankId bank)
			: Contract(std::move(contract)), Kind(kind), ExecutionMode(executionMode),
			  CarrierKind(carrierKind), DonorSourceKind(donorSourceKind),
			  CarrierVirtualThreadId(carrierVirtualThreadId), DonorVirtualThreadId(donorVirtualThreadId),
			  TargetVirtualThreadId(targetVirtualThreadId), CarrierCoreId(carrierCoreId),
			  TargetCoreId(targetCoreId), PodId(podId), DomainTag(domainTag),
			  PrefetchFootprint(prefetchFootprint), Bank(bank) {}
	};

	struct Rf06DmaCapability final
	{
		const std::shared_ptr<ExecutionContract> Contract;
		const DmaStreamComputeDescriptorReference DescriptorReference;
		const unsigned long long DescriptorIdentityHash;
		const unsigned long long CertificateInputHash;
		const unsigned long long NormalizedFootprintHash;
		const DmaStreamComputeOperationKind Operation;
		const DmaStreamComputeElementType ElementType;
		const DmaStreamComputeShapeKind Shape;
		const DmaStreamComputeRangeEncoding RangeEncoding;
		const DmaStreamComputePartialCompletionPolicy PartialCompletionPolicy;
		const DmaStreamComputeAliasPolicy AliasPolicy;
		const unsigned short OwnerVirtualThreadId;
		const unsigned int OwnerContextId;
		const unsigned int OwnerCoreId;
		const unsigned int OwnerPodId;
		const unsigned long long OwnerDomainTag;
		const std::vector<FrozenMemoryRange> ReadFootprint;
		const std::vector<FrozenMemoryRange> WriteFootprint;

		Rf06DmaCapability(std::shared_ptr<ExecutionContract> contract,
			DmaStreamComputeDescriptorReference descriptorReference,
			unsigned long long descriptorIdentityHash, unsigned long long certificateInputHash,
			unsigned long long normalizedFootprintHash, DmaStreamComputeOperationKind operation,
			DmaStreamComputeElementType elementType, DmaStreamComputeShapeKind shape,
			DmaStreamComputeRangeEncoding rangeEncoding,
			DmaStreamComputePartialCompletionPolicy partialCompletionPolicy,
			DmaStreamComputeAliasPolicy aliasPolicy, unsigned short ownerVirtualThreadId,
			unsigned int ownerContextId, unsigned int ownerCoreId, unsigned int ownerPodId,
			unsigned long long ownerDomainTag, std::vector<FrozenMemoryRange> readFootprint,
			std::vector<FrozenMemoryRange> writeFootprint)
			: Contract(std::move(contract)), DescriptorReference(descriptorReference),
			  DescriptorIdentityHash(descriptorIdentityHash), CertificateInputHash(certificateInputHash),
			  NormalizedFootprintHash(normalizedFootprintHash), Operation(operation),
			  ElementType(elementType), Shape(shape), RangeEncoding(rangeEncoding),
			  PartialCompletionPolicy(partialCompletionPolicy), AliasPolicy(aliasPolicy),
			  OwnerVirtualThreadId(ownerVirtualThreadId), OwnerContextId(ownerContextId),
			  OwnerCoreId(ownerCoreId), OwnerPodId(ownerPodId), OwnerDomainTag(ownerDomainTag),
			  ReadFootprint(std::move(readFootprint)), WriteFootprint(std::move(writeFootprint)) {}
	};

	struct Rf06AcceleratorCapability final
	{
		const std::shared_ptr<ExecutionContract> Contract;
		const SystemDeviceCommandKind CommandKind;
		const unsigned short DestinationRegister;
		const unsigned short TokenRegister;
		const bool HasDescriptorSideband;
		const std::optional<std::shared_ptr<AcceleratorDescriptorReference>> DescriptorReference;
		const SlotClass RequiredSlotClass;
		const SlotPinningKind PinningKind;
		const unsigned char PinnedLaneId;

		Rf06AcceleratorCapability(std::shared_ptr<ExecutionContract> contract,
			SystemDeviceCommandKind commandKind, unsigned short destinationRegister,
			unsigned short tokenRegister, bool hasDescriptorSideband,
			std::optional<std::shared_ptr<AcceleratorDescriptorReference>> descriptorReference,
			SlotClass requiredSlotClass, SlotPinningKind pinningKind, unsigned char pinnedLaneId)
			: Contract(std::move(contract)), CommandKind(commandKind),
			  DestinationRegister(destinationRegister), TokenRegister(tokenRegister),
			  HasDescriptorSideband(hasDescriptorSideband), DescriptorReference(std::move(descriptorReference)),
			  RequiredSlotClass(requiredSlotClass), PinningKind(pinningKind), PinnedLaneId(pinnedLaneId) {}
	};

	struct Rf06MatrixTileCapability final
	{
		const std::shared_ptr<ExecutionContract> Contract;
		const MatrixTileProjectedOperationKind OperationKind;
		const MatrixTileRuntimeResourceClass RuntimeResourceClass;
		const bool ReadsTileState;
		const bool WritesTileState;
		const bool ReadsAccumulator;
		const bool WritesAccumulator;
		const bool HasTransposePolicyDependency;
		const SlotClass RequiredSlotClass;
		const SlotPinningKind PinningKind;
		const unsigned char PinnedLaneId;

		Rf06MatrixTileCapability(std::shared_ptr<ExecutionContract> contract,
			MatrixTileProjectedOperationKind operationKind, MatrixTileRuntimeResourceClass runtimeResourceClass,
			bool readsTileState, bool writesTileState, bool readsAccumulator, bool writesAccumulator,
			bool hasTransposePolicyDependency, SlotClass requiredSlotClass,
			SlotPinningKind pinningKind, unsigned char pinnedLaneId)
			: Contract(std::move(contract)), OperationKind(operationKind), RuntimeResourceClass(runtimeResourceClass),
			  ReadsTileState(readsTileState), WritesTileState(writesTileState),
			  ReadsAccumulator(readsAccumulator), WritesAccumulator(writesAccumulator),
			  HasTransposePolicyDependency(hasTransposePolicyDependency),
			  RequiredSlotClass(requiredSlotClass), PinningKind(pinningKind), PinnedLaneId(pinnedLaneId) {}
	};

	/// <summary>
	/// Immutable control-flow capability. Target resolution and branch outcome are
	/// deliberately absent; they belong to execution/retirement authorities.
	/// </summary>
// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	internal sealed record Rf06ControlCapability(ExecutionContract Contract, bool IsConditional, bool RedirectsProgramCounter, bool IsStealable, SlotClass RequiredSlotClass, SlotPinningKind PinningKind, byte PinnedLaneId);

	/// <summary>Immutable serializing-boundary capability.</summary>
// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	internal sealed record Rf06SerializingCapability(ExecutionContract Contract, InstructionClass InstructionClass, SerializationClass SerializationClass, bool IsControlFlow, bool HasStaticSideEffects, SlotClass RequiredSlotClass, SlotPinningKind PinningKind, byte PinnedLaneId);

	/// <summary>
	/// Immutable assist capability. Reserved line budgets, replay state, outcomes
	/// and completion are intentionally not copied from the legacy carrier.
	/// </summary>
// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	internal sealed record Rf06AssistCapability(ExecutionContract Contract, AssistKind Kind, AssistExecutionMode ExecutionMode, AssistCarrierKind CarrierKind, AssistDonorSourceKind DonorSourceKind, int CarrierVirtualThreadId, int DonorVirtualThreadId, int TargetVirtualThreadId, int CarrierCoreId, int TargetCoreId, ushort PodId, ulong DomainTag, FrozenMemoryRange PrefetchFootprint, MemoryBankId Bank);

	/// <summary>
	/// DMA descriptor capability. DMA has independent read/write footprints, so
	/// it is kept as a typed sideband beside the generic ExecutionContract rather
	/// than being misrepresented as a scalar Atomic memory operation.
	/// </summary>
// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	internal sealed record Rf06DmaCapability(ExecutionContract Contract, DmaStreamComputeDescriptorReference DescriptorReference, ulong DescriptorIdentityHash, ulong CertificateInputHash, ulong NormalizedFootprintHash, DmaStreamComputeOperationKind Operation, DmaStreamComputeElementType ElementType, DmaStreamComputeShapeKind Shape, DmaStreamComputeRangeEncoding RangeEncoding, DmaStreamComputePartialCompletionPolicy PartialCompletionPolicy, DmaStreamComputeAliasPolicy AliasPolicy, ushort OwnerVirtualThreadId, uint OwnerContextId, uint OwnerCoreId, uint OwnerPodId, ulong OwnerDomainTag, ImmutableArray<FrozenMemoryRange> ReadFootprint, ImmutableArray<FrozenMemoryRange> WriteFootprint);

	/// <summary>Immutable lane7 accelerator command capability.</summary>
// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	internal sealed record Rf06AcceleratorCapability(ExecutionContract Contract, SystemDeviceCommandKind CommandKind, ushort DestinationRegister, ushort TokenRegister, bool HasDescriptorSideband, System.Nullable<AcceleratorDescriptorReference> DescriptorReference, SlotClass RequiredSlotClass, SlotPinningKind PinningKind, byte PinnedLaneId);

	/// <summary>Immutable MatrixTile placement/dependency capability.</summary>
// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	internal sealed record Rf06MatrixTileCapability(ExecutionContract Contract, MatrixTileProjectedOperationKind OperationKind, MatrixTileRuntimeResourceClass RuntimeResourceClass, bool ReadsTileState, bool WritesTileState, bool ReadsAccumulator, bool WritesAccumulator, bool HasTransposePolicyDependency, SlotClass RequiredSlotClass, SlotPinningKind PinningKind, byte PinnedLaneId);

	/// <summary>
	/// Named RF-06.5 legacy boundary. It consumes an existing materialized carrier
	/// and a binding resolved by the canonical decode/materializer handoff. It does
	/// not create carriers, scheduler admission objects, working operation IDs or
	/// replay entries and
	/// does not resolve anything by opcode or registry.
	/// </summary>
	class Rf06SpecializedCapabilityProjection final
	{
	public:
		static inline const std::wstring ControlPayloadSchema = L"rf06.control-flow-v1";
		static inline const std::wstring SerializingPayloadSchema = L"rf06.serializing-v1";
		static inline const std::wstring AssistPayloadSchema = L"rf06.assist-v1";
		static inline const std::wstring DmaPayloadSchema = L"rf06.dma-v1";
		static inline const std::wstring AcceleratorPayloadSchema = L"rf06.accelerator-v1";
		static inline const std::wstring MatrixTilePayloadSchema = L"rf06.matrix-tile-v1";

		static std::shared_ptr<Rf06ControlCapability> ProjectControl(std::shared_ptr<MicroOp> carrier, std::shared_ptr<GeneratedStaticBinding> binding);

		static std::shared_ptr<Rf06SerializingCapability> ProjectSerializing(std::shared_ptr<MicroOp> carrier, std::shared_ptr<GeneratedStaticBinding> binding);

		static std::shared_ptr<Rf06AssistCapability> ProjectAssist(std::shared_ptr<AssistMicroOp> carrier, std::shared_ptr<GeneratedStaticBinding> binding);

		static std::shared_ptr<Rf06DmaCapability> ProjectDma(std::shared_ptr<DmaStreamComputeMicroOp> carrier, std::shared_ptr<GeneratedStaticBinding> binding);

		static std::shared_ptr<Rf06AcceleratorCapability> ProjectAccelerator(std::shared_ptr<SystemDeviceCommandMicroOp> carrier, std::shared_ptr<GeneratedStaticBinding> binding);

		static std::shared_ptr<Rf06MatrixTileCapability> ProjectMatrixTile(std::shared_ptr<MatrixTileMicroOp> carrier, std::shared_ptr<GeneratedStaticBinding> binding);

	private:
		static std::shared_ptr<ExecutionContract> CreateContract(std::shared_ptr<MicroOp> carrier, std::shared_ptr<GeneratedStaticBinding> binding, const std::wstring &payloadSchema, const std::wstring &staticEffectContract, std::shared_ptr<MemoryCapability> memory, bool isAssist, bool isRetireVisible, bool validateOpcode = true);

		static void ValidateBinding(std::shared_ptr<MicroOp> carrier, std::shared_ptr<GeneratedStaticBinding> binding);

		static std::shared_ptr<MemoryCapability> BuildMemoryCapability(std::shared_ptr<MicroOp> carrier);

		static std::vector<FrozenMemoryRange> FreezeRanges(const std::vector<MicroOpMemoryRange> &ranges);
	};

}
