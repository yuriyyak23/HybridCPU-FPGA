#include "Rf08ScalarLoadContractProjection.h"
#include "CanonicalDecodedContracts.h"
#include "../Pipeline/MicroOps/Types/MicroOp.h"
#include "../Pipeline/Safety/ResourceBitset.h"
#include "../Pipeline/Scheduling/SlotLegality/SlotClassDefinitions.h"
#include "../../../NonRTL/Arch/InstructionClass.h"
#include "../../../NonRTL/Arch/SerializationClass.h"

using namespace HybridCPU_ISE::Arch;
using namespace YAKSys_Hybrid_CPU::Arch;
using namespace YAKSys_Hybrid_CPU::Core::Pipeline::MicroOps;
using namespace YAKSys_Hybrid_CPU::Core::Registers;

namespace YAKSys_Hybrid_CPU::Core::Decoder
{

	std::shared_ptr<ExecutionContract> Rf08ScalarLoadContractProjection::CreateContract(std::shared_ptr<CanonicalDecodedInstruction> canonical)
	{
		ArgumentNullException::ThrowIfNull(canonical);
		std::shared_ptr<CanonicalScalarLoadAddressPlan> plan = canonical->ScalarLoadAddressPlan;
		if (!plan)
		{
			throw InvalidOperationException(L"RF-08 scalar-load contract projection requires the canonical unresolved address plan.");
		}
		plan->EnsureMatches(canonical);

		std::shared_ptr<GeneratedStaticBinding> binding = canonical->StaticBinding;
		if (!binding)
		{
			throw InvalidOperationException(L"RF-08 scalar-load contract projection requires the canonical generated binding.");
		}
		if (binding != plan->getGeneratedBinding() || canonical->InstructionClass != InstructionClass::Memory || canonical->SerializationClass != SerializationClass::Free)
		{
			throw InvalidOperationException(L"RF-08 scalar-load contract projection received a mismatched canonical load plan or binding.");
		}

		ResourceBitset resourceMask = ResourceMaskBuilder::ForLoad();
		ArchRegId baseRegisterId;
		resourceMask = resourceMask | (ArchRegId::TryCreate(plan->getBaseRegisterId(), baseRegisterId) ? ResourceMaskBuilder::ForArchitecturalRegisterRead(std::make_shared<ArchRegId>(baseRegisterId)) : ResourceMaskBuilder::ForRegisterRead(plan->getBaseRegisterId()));
		ArchRegId destinationRegisterId;
		resourceMask = resourceMask | (ArchRegId::TryCreate(plan->getDestinationRegisterId(), destinationRegisterId) ? ResourceMaskBuilder::ForArchitecturalRegisterWrite(std::make_shared<ArchRegId>(destinationRegisterId)) : ResourceMaskBuilder::ForRegisterWrite(plan->getDestinationRegisterId()));

		return ExecutionContract::Create(binding, std::make_shared<RuntimeExecutionProviderBinding>(binding->RuntimeExecutionProviderId, PayloadSchema), InstructionClass::Memory, SerializationClass::Free, ExecutionPlacement::Create(SlotClass::LsuClass, SlotPinningKind::ClassFlexible), L"RegisterWrite", MemoryCapability::None, {plan->getBaseRegisterId()}, {plan->getDestinationRegisterId()}, resourceMask, true, true, false, StaticMemoryAccessPlan::UnresolvedScalarLoad(plan));
	}
}
