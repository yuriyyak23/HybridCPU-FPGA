#include "Rf06ExecutionContracts.h"
#include "CanonicalSecurityOperationIdentity.h"
#include "../../../NonRTL/Arch/Generated/GeneratedIsaShadowCatalog.g.h"
#include "../Architecture/Common/YAKSys_Hybrid_CPU.Processor.h"
#include "Rf06SpecializedCapabilityProjection.h"
#include "Rf08ScalarLoadContractProjection.h"
#include "../Pipeline/Scheduling/SlotLegality/SlotClassDefinitions.h"
#include "../Pipeline/Metadata/BundleMetadata.h"
#include <algorithm>
#include <cwctype>

using namespace HybridCPU_ISE::Arch;
using namespace YAKSys_Hybrid_CPU::Arch;
using namespace YAKSys_Hybrid_CPU::Arch::Generated;

namespace
{
	bool IsWhiteSpaceOnly(const std::wstring &value)
	{
		return std::all_of(value.begin(), value.end(), [](wchar_t character)
		{
			return std::iswspace(character) != 0;
		});
	}

	bool IsDefinedInstructionClass(YAKSys_Hybrid_CPU::Arch::InstructionClass value)
	{
		using YAKSys_Hybrid_CPU::Arch::InstructionClass;
		switch (value)
		{
		case InstructionClass::ScalarAlu:
		case InstructionClass::Memory:
		case InstructionClass::ControlFlow:
		case InstructionClass::Atomic:
		case InstructionClass::System:
		case InstructionClass::Csr:
		case InstructionClass::SmtVt:
		case InstructionClass::Vmx:
			return true;
		default:
			return false;
		}
	}
}

namespace YAKSys_Hybrid_CPU::Core::Decoder
{

	StaticMemoryAccessPlan::StaticMemoryAccessPlan(std::shared_ptr<CanonicalScalarLoadAddressPlan> scalarLoadPlan)
		: ScalarLoadPlan(std::move(scalarLoadPlan))
	{}

	std::shared_ptr<CanonicalScalarLoadAddressPlan> StaticMemoryAccessPlan::getScalarLoadPlan() const
	{
		return ScalarLoadPlan;
	}

	std::shared_ptr<GeneratedStaticBinding> StaticMemoryAccessPlan::getGeneratedBinding() const
	{
		return getScalarLoadPlan()->getGeneratedBinding();
	}

	MemoryAccessDirection StaticMemoryAccessPlan::getDirection() const
	{
		return MemoryAccessDirection::Read;
	}

	std::shared_ptr<StaticMemoryAccessPlan> StaticMemoryAccessPlan::UnresolvedScalarLoad(std::shared_ptr<CanonicalScalarLoadAddressPlan> scalarLoadPlan)
	{
		ArgumentNullException::ThrowIfNull(scalarLoadPlan);
		return std::shared_ptr<StaticMemoryAccessPlan>(new StaticMemoryAccessPlan(std::move(scalarLoadPlan)));
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private ExecutionContract(GeneratedStaticBinding generatedBinding, RuntimeExecutionProviderBinding runtimeProvider, InstructionClass instructionClass, SerializationClass serializationClass, ExecutionPlacement placement, string staticEffectContract, MemoryCapability memory, StaticMemoryAccessPlan? staticMemoryPlan, std::vector<int> readRegisters, std::vector<int> writeRegisters, ResourceBitset resourceMask, bool isStealable, bool isRetireVisible, bool isAssist)
	ExecutionContract::ExecutionContract(std::shared_ptr<GeneratedStaticBinding> generatedBinding, std::shared_ptr<RuntimeExecutionProviderBinding> runtimeProvider, YAKSys_Hybrid_CPU::Arch::InstructionClass instructionClass, YAKSys_Hybrid_CPU::Arch::SerializationClass serializationClass, std::shared_ptr<ExecutionPlacement> placement, const std::wstring &staticEffectContract, std::shared_ptr<MemoryCapability> memory, std::shared_ptr<StaticMemoryAccessPlan> staticMemoryPlan, std::vector<int> readRegisters, std::vector<int> writeRegisters, ResourceBitset resourceMask, bool isStealable, bool isRetireVisible, bool isAssist) : GeneratedBinding(std::move(generatedBinding)), RuntimeProvider(std::move(runtimeProvider)), InstructionClass(instructionClass), SerializationClass(serializationClass), Placement(std::move(placement)), StaticEffectContract(staticEffectContract), Memory(std::move(memory)), StaticMemoryPlan(std::move(staticMemoryPlan)), ReadRegisters(std::move(readRegisters)), WriteRegisters(std::move(writeRegisters)), ResourceMask(resourceMask), IsStealable(isStealable), IsRetireVisible(isRetireVisible), IsAssist(isAssist) {}

	std::shared_ptr<GeneratedStaticBinding> ExecutionContract::getGeneratedBinding() const
	{
		return GeneratedBinding;
	}

	std::shared_ptr<RuntimeExecutionProviderBinding> ExecutionContract::getRuntimeProvider() const
	{
		return RuntimeProvider;
	}

	YAKSys_Hybrid_CPU::Arch::InstructionClass ExecutionContract::getInstructionClass() const
	{
		return InstructionClass;
	}

	YAKSys_Hybrid_CPU::Arch::SerializationClass ExecutionContract::getSerializationClass() const
	{
		return SerializationClass;
	}

	std::shared_ptr<ExecutionPlacement> ExecutionContract::getPlacement() const
	{
		return Placement;
	}

	std::wstring ExecutionContract::getStaticEffectContract() const
	{
		return StaticEffectContract;
	}

	std::shared_ptr<MemoryCapability> ExecutionContract::getMemory() const
	{
		return Memory;
	}

	std::shared_ptr<StaticMemoryAccessPlan> ExecutionContract::getStaticMemoryPlan() const
	{
		return StaticMemoryPlan;
	}

	std::vector<int> ExecutionContract::getReadRegisters() const
	{
		return ReadRegisters;
	}

	std::vector<int> ExecutionContract::getWriteRegisters() const
	{
		return WriteRegisters;
	}

	YAKSys_Hybrid_CPU::Core::ResourceBitset ExecutionContract::getResourceMask() const
	{
		return ResourceMask;
	}

	bool ExecutionContract::isStealable() const
	{
		return IsStealable;
	}

	bool ExecutionContract::isRetireVisible() const
	{
		return IsRetireVisible;
	}

	bool ExecutionContract::isAssist() const
	{
		return IsAssist;
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public static ExecutionContract Create(GeneratedStaticBinding generatedBinding, RuntimeExecutionProviderBinding runtimeProvider, InstructionClass instructionClass, SerializationClass serializationClass, ExecutionPlacement placement, string staticEffectContract, MemoryCapability memory, IEnumerable<int>? readRegisters = null, IEnumerable<int>? writeRegisters = null, ResourceBitset resourceMask = default, bool isStealable = true, bool isRetireVisible = true, bool isAssist = false, System.Nullable<StaticMemoryAccessPlan> staticMemoryPlan = null)
	std::shared_ptr<ExecutionContract> ExecutionContract::Create(std::shared_ptr<GeneratedStaticBinding> generatedBinding, std::shared_ptr<RuntimeExecutionProviderBinding> runtimeProvider, YAKSys_Hybrid_CPU::Arch::InstructionClass instructionClass, YAKSys_Hybrid_CPU::Arch::SerializationClass serializationClass, std::shared_ptr<ExecutionPlacement> placement, const std::wstring &staticEffectContract, std::shared_ptr<MemoryCapability> memory, std::vector<int> readRegisters, std::vector<int> writeRegisters, ResourceBitset resourceMask, bool isStealable, bool isRetireVisible, bool isAssist, std::shared_ptr<StaticMemoryAccessPlan> staticMemoryPlan)
	{
		ArgumentNullException::ThrowIfNull(generatedBinding);
		ArgumentNullException::ThrowIfNull(runtimeProvider);
		ArgumentNullException::ThrowIfNull(placement);
		ArgumentNullException::ThrowIfNull(memory);
		if (IsWhiteSpaceOnly(staticEffectContract))
		{
			throw std::invalid_argument("staticEffectContract must not be empty or whitespace.");
		}
		if (CanonicalSecurityOperationIdentity::FromGeneratedBinding(generatedBinding) != CanonicalSecurityOperationKind::Ordinary)
		{
// C# TO C++ CONVERTER TASK: This exception's constructor requires only one argument:
// ORIGINAL LINE: throw new ArgumentException("Execution contract requires a recognized canonical generated operation binding.", nameof(generatedBinding));
			throw std::invalid_argument("Execution contract requires a recognized canonical generated operation binding.");
		}
		if (runtimeProvider->Id != generatedBinding->RuntimeExecutionProviderId)
		{
// C# TO C++ CONVERTER TASK: This exception's constructor requires only one argument:
// ORIGINAL LINE: throw new ArgumentException("Runtime provider binding does not match the generated descriptor binding.", nameof(runtimeProvider));
			throw std::invalid_argument("Runtime provider binding does not match the generated descriptor binding.");
		}

		// Scalar ALU operations have an exact register-local generated effect.
		// A recognized opcode cannot be repurposed as a memory or other effect.
		std::shared_ptr<GeneratedIsaDescriptor> descriptor;
		if (!GeneratedIsaCatalog::TryGetDescriptor(generatedBinding->Opcode, descriptor) || !descriptor)
		{
			throw std::invalid_argument("Execution contract requires a generated ISA descriptor.");
		}
		if (descriptor->StaticClass == InstructionClass::ScalarAlu && !IsMatrixTile(generatedBinding->Opcode) && (instructionClass != descriptor->StaticClass || serializationClass != descriptor->Serialization || staticEffectContract != descriptor->StaticEffectContract || memory->Kind != MemoryCapabilityKind::NoMemory || staticMemoryPlan != nullptr))
		{
			throw std::invalid_argument("Scalar ALU execution envelope must preserve its generated register-local effects.");
		}

		if (isAssist && isRetireVisible)
		{
// C# TO C++ CONVERTER TASK: This exception's constructor requires only one argument:
// ORIGINAL LINE: throw new ArgumentException("An assist capability cannot be retire-visible.", nameof(isRetireVisible));
			throw std::invalid_argument("An assist capability cannot be retire-visible.");
		}

		if (descriptor->StaticClass == InstructionClass::Csr || descriptor->StaticClass == InstructionClass::System || descriptor->StaticClass == InstructionClass::SmtVt || descriptor->StaticClass == InstructionClass::Vmx)
		{
	// C# TO C++ CONVERTER TASK: The following 'switch expression' was not converted by C# to C++ Converter:
	//				bool supportedEffect = staticEffectContract switch
	//				{
	//					"CsrOrSystemEffect" => descriptor.StaticEffectContract == "CsrOrSystemEffect",
	//					"SerializingEffect" => descriptor.Serialization != SerializationClass.Free &&
	//						runtimeProvider.PayloadSchema == Rf06SpecializedCapabilityProjection.SerializingPayloadSchema, "AcceleratorCommand" => IsAcceleratorCommand(generatedBinding.Opcode) &&
	//						runtimeProvider.PayloadSchema == Rf06SpecializedCapabilityProjection.AcceleratorPayloadSchema,
	//					_ => false,
	//				};
			const bool supportedEffect =
				(staticEffectContract == L"CsrOrSystemEffect" && descriptor->StaticEffectContract == L"CsrOrSystemEffect") ||
				(staticEffectContract == L"SerializingEffect" && descriptor->Serialization != SerializationClass::Free &&
				 runtimeProvider->PayloadSchema == Rf06SpecializedCapabilityProjection::SerializingPayloadSchema) ||
				(staticEffectContract == L"AcceleratorCommand" && IsAcceleratorCommand(generatedBinding->Opcode) &&
				 runtimeProvider->PayloadSchema == Rf06SpecializedCapabilityProjection::AcceleratorPayloadSchema);
			if (!supportedEffect || instructionClass != descriptor->StaticClass || serializationClass != descriptor->Serialization || isAssist || staticMemoryPlan != nullptr || memory->Kind != MemoryCapabilityKind::NoMemory)
			{
				throw std::invalid_argument("System operation has no handler for this effect envelope.");
			}
		}

		if (IsMatrixTile(generatedBinding->Opcode))
		{
			bool isLoad = generatedBinding->Opcode == Processor::CPU_Core::IsaOpcodeValues::MTILE_LOAD;
			bool isStore = generatedBinding->Opcode == Processor::CPU_Core::IsaOpcodeValues::MTILE_STORE;
			MemoryCapabilityKind expectedMemory = isLoad ? MemoryCapabilityKind::Load : isStore ? MemoryCapabilityKind::Store : MemoryCapabilityKind::NoMemory;
			std::wstring expectedEffect = isLoad || isStore ? L"MatrixTileMemoryCommit" : L"MatrixTileCommit";
			if (runtimeProvider->PayloadSchema != Rf06SpecializedCapabilityProjection::MatrixTilePayloadSchema || instructionClass != descriptor->StaticClass || serializationClass != descriptor->Serialization || staticEffectContract != expectedEffect || memory->Kind != expectedMemory || staticMemoryPlan != nullptr || isAssist)
			{
				throw std::invalid_argument("MatrixTile operation has no handler for this effect envelope.");
			}
		}

		if (descriptor->StaticClass == InstructionClass::ControlFlow && (instructionClass != descriptor->StaticClass || serializationClass != descriptor->Serialization || staticEffectContract != descriptor->StaticEffectContract || isAssist || memory->Kind != MemoryCapabilityKind::NoMemory || staticMemoryPlan != nullptr))
		{
			throw std::invalid_argument("Control-flow execution envelope must preserve its generated PC effect.");
		}

		if (IsScalarLoad(generatedBinding->Opcode))
		{
	// C# TO C++ CONVERTER TASK: The following 'switch expression' was not converted by C# to C++ Converter:
	//				bool supportedEnvelope = staticEffectContract switch
	//				{
	//					"RegisterWrite" => !isAssist && instructionClass == InstructionClass.Memory &&
	//						serializationClass == SerializationClass.Free &&
	//						((staticMemoryPlan is not null && memory.Kind == MemoryCapabilityKind.NoMemory &&
	//						  runtimeProvider.PayloadSchema == Rf08ScalarLoadContractProjection.PayloadSchema) ||
	//						 (staticMemoryPlan is null && memory.Kind == MemoryCapabilityKind.Load)), "AssistPrefetch" => isAssist && !isRetireVisible && staticMemoryPlan is null &&
	//						memory.Kind == MemoryCapabilityKind.Load &&
	//						runtimeProvider.PayloadSchema == Rf06SpecializedCapabilityProjection.AssistPayloadSchema,
	//					// Existing ordinary shadow comparison deliberately receives
	//					// mismatched capabilities so Stage A can classify the mismatch.
	//					// It is not a scalar-load execution/publication contract.
	//					"MemoryEffect" => !isAssist && staticMemoryPlan is null &&
	//						instructionClass == InstructionClass.Memory &&
	//						runtimeProvider.PayloadSchema == "memory-v1",
	//					_ => false,
	//				};
			const bool supportedEnvelope =
				(staticEffectContract == L"RegisterWrite" && !isAssist && instructionClass == InstructionClass::Memory &&
				 serializationClass == SerializationClass::Free &&
				 ((staticMemoryPlan != nullptr && memory->Kind == MemoryCapabilityKind::NoMemory &&
				   runtimeProvider->PayloadSchema == Rf08ScalarLoadContractProjection::PayloadSchema) ||
				  (staticMemoryPlan == nullptr && memory->Kind == MemoryCapabilityKind::Load))) ||
				(staticEffectContract == L"AssistPrefetch" && isAssist && !isRetireVisible && staticMemoryPlan == nullptr &&
				 memory->Kind == MemoryCapabilityKind::Load &&
				 runtimeProvider->PayloadSchema == Rf06SpecializedCapabilityProjection::AssistPayloadSchema) ||
				// Existing ordinary shadow comparison deliberately receives mismatched
				// capabilities so Stage A can classify the mismatch. It is not a scalar
				// load execution/publication contract.
				(staticEffectContract == L"MemoryEffect" && !isAssist && staticMemoryPlan == nullptr &&
				 instructionClass == InstructionClass::Memory && runtimeProvider->PayloadSchema == L"memory-v1");
			if (!supportedEnvelope)
			{
				throw std::invalid_argument("Scalar load has no handler for this effect envelope.");
			}
		}

		if (staticMemoryPlan != nullptr)
		{
			if (memory->Kind != MemoryCapabilityKind::NoMemory)
			{
// C# TO C++ CONVERTER TASK: This exception's constructor requires only one argument:
// ORIGINAL LINE: throw new ArgumentException("An unresolved static memory plan cannot be mixed with a concrete memory capability.", nameof(staticMemoryPlan));
				throw std::invalid_argument("An unresolved static memory plan cannot be mixed with a concrete memory capability.");
			}

			if (staticMemoryPlan->getGeneratedBinding() != generatedBinding)
			{
// C# TO C++ CONVERTER TASK: This exception's constructor requires only one argument:
// ORIGINAL LINE: throw new ArgumentException("Static memory plan must retain the exact generated binding instance of its execution contract.", nameof(staticMemoryPlan));
				throw std::invalid_argument("Static memory plan must retain the exact generated binding instance of its execution contract.");
			}
		}

		if (descriptor->StaticClass == InstructionClass::Atomic || (descriptor->StaticClass == InstructionClass::Memory && !IsScalarLoad(generatedBinding->Opcode) && !IsMatrixTile(generatedBinding->Opcode)))
		{
			unsigned int opcode = generatedBinding->Opcode;
			bool supportedEnvelope;
			if (opcode == Processor::CPU_Core::IsaOpcodeValues::DmaStreamCompute)
			{
				supportedEnvelope = runtimeProvider->PayloadSchema == Rf06SpecializedCapabilityProjection::DmaPayloadSchema && staticEffectContract == L"DmaCommit" && memory->Kind == MemoryCapabilityKind::NoMemory && placement->RequiredSlotClass == SlotClass::DmaStreamClass && placement->PinningKind == SlotPinningKind::HardPinned && placement->PinnedLaneId == 6;
			}
			else if ((opcode == Processor::CPU_Core::IsaOpcodeValues::SB || opcode == Processor::CPU_Core::IsaOpcodeValues::SH || opcode == Processor::CPU_Core::IsaOpcodeValues::SW || opcode == Processor::CPU_Core::IsaOpcodeValues::SD))
			{
				supportedEnvelope = staticEffectContract == L"StoreCommit" && memory->Kind == MemoryCapabilityKind::Store;
			}
			else if (descriptor->StaticClass == InstructionClass::Atomic)
			{
				MemoryCapabilityKind expectedMemory = (opcode == Processor::CPU_Core::IsaOpcodeValues::LR_W || opcode == Processor::CPU_Core::IsaOpcodeValues::LR_D) ? MemoryCapabilityKind::Load : (opcode == Processor::CPU_Core::IsaOpcodeValues::SC_W || opcode == Processor::CPU_Core::IsaOpcodeValues::SC_D) ? MemoryCapabilityKind::Store : MemoryCapabilityKind::Atomic;
				supportedEnvelope = staticEffectContract == L"AtomicCommit" && memory->Kind == expectedMemory;
			}
			else
			{
				// No typed execution-contract consumer for residual vector memory
				// operations is connected here. Ordinary legacy execution is separate.
				supportedEnvelope = false;
			}
			if (!supportedEnvelope || instructionClass != descriptor->StaticClass || serializationClass != descriptor->Serialization || isAssist || staticMemoryPlan != nullptr)
			{
				throw std::invalid_argument("Memory operation has no handler for this effect envelope.");
			}
		}

		if (!IsDefinedInstructionClass(descriptor->StaticClass))
		{
			throw std::invalid_argument("Generated operation class has no execution-contract handler.");
		}

		std::vector<int> frozenReads = FreezeRegisters(readRegisters, L"readRegisters");
		std::vector<int> frozenWrites = FreezeRegisters(writeRegisters, L"writeRegisters");
		return std::shared_ptr<ExecutionContract>(new ExecutionContract(std::move(generatedBinding), std::move(runtimeProvider), instructionClass, serializationClass, std::move(placement), StringHelper::trim(staticEffectContract), std::move(memory), std::move(staticMemoryPlan), std::move(frozenReads), std::move(frozenWrites), resourceMask, isStealable, isRetireVisible, isAssist));
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private static std::vector<int> FreezeRegisters(IEnumerable<int>? registers, string parameterName)
	std::vector<int> ExecutionContract::FreezeRegisters(const std::vector<int> &registers, const std::wstring &parameterName) { (void)parameterName; return FreezeRegisterIds(registers); }

	bool ExecutionContract::IsScalarLoad(unsigned int opcode)
	{
		return (opcode == Processor::CPU_Core::IsaOpcodeValues::LB || opcode == Processor::CPU_Core::IsaOpcodeValues::LBU || opcode == Processor::CPU_Core::IsaOpcodeValues::LH || opcode == Processor::CPU_Core::IsaOpcodeValues::LHU || opcode == Processor::CPU_Core::IsaOpcodeValues::LW || opcode == Processor::CPU_Core::IsaOpcodeValues::LWU || opcode == Processor::CPU_Core::IsaOpcodeValues::LD);
	}

	bool ExecutionContract::IsMatrixTile(unsigned int opcode)
	{
		return (opcode == Processor::CPU_Core::IsaOpcodeValues::MTILE_LOAD || opcode == Processor::CPU_Core::IsaOpcodeValues::MTILE_STORE || opcode == Processor::CPU_Core::IsaOpcodeValues::MTILE_MACC || opcode == Processor::CPU_Core::IsaOpcodeValues::MTRANSPOSE);
	}

	bool ExecutionContract::IsAcceleratorCommand(unsigned int opcode)
	{
		return (opcode == Processor::CPU_Core::IsaOpcodeValues::ACCEL_QUERY_CAPS || opcode == Processor::CPU_Core::IsaOpcodeValues::ACCEL_SUBMIT || opcode == Processor::CPU_Core::IsaOpcodeValues::ACCEL_POLL || opcode == Processor::CPU_Core::IsaOpcodeValues::ACCEL_WAIT || opcode == Processor::CPU_Core::IsaOpcodeValues::ACCEL_CANCEL || opcode == Processor::CPU_Core::IsaOpcodeValues::ACCEL_FENCE || opcode == Processor::CPU_Core::IsaOpcodeValues::ACCEL_STATUS);
	}





	ScheduledOperation::ScheduledOperation(std::shared_ptr<VliwOperationId> operationId, std::shared_ptr<AdmissionRecord> admission, int physicalLane)
		: OperationId(std::move(operationId)), Admission(std::move(admission)), PhysicalLane(physicalLane)
	{}

	std::shared_ptr<VliwOperationId> ScheduledOperation::getOperationId() const
	{
		return OperationId;
	}

	std::shared_ptr<AdmissionRecord> ScheduledOperation::getAdmission() const
	{
		return Admission;
	}

	int ScheduledOperation::getPhysicalLane() const
	{
		return PhysicalLane;
	}

	std::shared_ptr<RuntimeExecutionProviderBinding> ScheduledOperation::getRuntimeProvider() const
	{
		return getAdmission()->Contract->getRuntimeProvider();
	}

	std::shared_ptr<ScheduledOperation> ScheduledOperation::CreateAfterStageB(std::shared_ptr<AdmissionRecord> admission, unsigned long long workingBundleSequence, int workingSlotIndex, int physicalLane, std::shared_ptr<OperationAttemptIssuer> attemptIssuer)
	{
		ArgumentNullException::ThrowIfNull(admission);
		ArgumentNullException::ThrowIfNull(attemptIssuer);
		if (static_cast<unsigned int>(workingSlotIndex) >= BundleMetadata::BundleSlotCount)
		{
			throw ArgumentOutOfRangeException(L"workingSlotIndex");
		}

		if (static_cast<unsigned int>(physicalLane) >= BundleMetadata::BundleSlotCount)
		{
			throw ArgumentOutOfRangeException(L"physicalLane", physicalLane, L"A scheduled operation requires a selected physical lane.");
		}

		unsigned long long attempt = attemptIssuer->IssueNextAttempt();
		std::shared_ptr<VliwOperationId> operationId = VliwOperationId::Issue(admission->VirtualThreadId, workingBundleSequence, workingSlotIndex, attempt);
		return std::shared_ptr<ScheduledOperation>(new ScheduledOperation(std::move(operationId), std::move(admission), physicalLane));
	}
}
