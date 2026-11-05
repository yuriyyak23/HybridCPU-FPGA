#include "SecureComputeMemoryReadContractProjection.h"
#include "../../Decoder/Rf08ScalarLoadContractProjection.h"
#include "../MicroOps/Types/MicroOp.h"

using namespace YAKSys_Hybrid_CPU::Core::Decoder;

namespace YAKSys_Hybrid_CPU::Core
{

	std::shared_ptr<ExecutionContract> SecureComputeMemoryReadContractProjection::Resolve(std::shared_ptr<CanonicalDecodedInstruction> canonical, std::shared_ptr<MemoryCapability> memory, int ownerThread)
	{
		// RF-08 remains the immutable unresolved source; this consumer owns resolution.
		auto source = Rf08ScalarLoadContractProjection::CreateContract(canonical);
	// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
	// ORIGINAL LINE: var plan = canonical.ScalarLoadAddressPlan!;
		auto plan = canonical->ScalarLoadAddressPlan;
		if (!plan || !memory || memory->Kind != MemoryCapabilityKind::Load ||
			memory->Direction != MemoryAccessDirection::Read || !memory->Bank.has_value() ||
			memory->Footprint.size() != 1 || memory->Footprint[0].Length != plan->getAccessSize())
		{
			throw InvalidOperationException(L"Exact resolved scalar read footprint required.");
		}
		auto binding = source->getGeneratedBinding();
		return ExecutionContract::Create(binding,
			std::make_shared<RuntimeExecutionProviderBinding>(binding->RuntimeExecutionProviderId, L"secure-readonly-source-load-v1"),
			source->getInstructionClass(), source->getSerializationClass(), source->getPlacement(), L"RegisterWrite",
			memory, source->getReadRegisters(), source->getWriteRegisters(),
			source->getResourceMask() | ResourceMaskBuilder::ForMemoryDomain(ownerThread), true, true, false, nullptr);
	}
}
