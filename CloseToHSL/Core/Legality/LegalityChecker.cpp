#include "LegalityChecker.h"
#include "../../../NonRTL/Arch/YAKSys_Hybrid_CPU.Arch.OpcodeRegistry.h"
#include "../../../NonRTL/Arch/OpcodeInfo.Types.h"

using namespace YAKSys_Hybrid_CPU::Arch;
using namespace YAKSys_Hybrid_CPU::Core::Pipeline::MicroOps;
using namespace YAKSys_Hybrid_CPU::Core::Registers;

namespace YAKSys_Hybrid_CPU::Core::Legality
{

	LegalityResult LegalityChecker::Check(std::shared_ptr<InstructionIR> instruction, std::shared_ptr<IHazardState> hazards, std::shared_ptr<IResourceState> resources, PrivilegeLevel currentPrivilege)
	{
		unsigned short opcode = instruction->CanonicalOpcode->Value;

		if (RequiresMachineMode(opcode) && currentPrivilege < PrivilegeLevel::Machine)
		{
			return LegalityResult::PrivilegeFault;
		}

		if (instruction->Rs1 > 0 && hazards->HasRawHazard(instruction->Rs1))
		{
			return LegalityResult::Stall;
		}

		if (instruction->Rs2 > 0 && hazards->HasRawHazard(instruction->Rs2))
		{
			return LegalityResult::Stall;
		}

		if (!resources->IsAvailable(instruction->Class))
		{
			return LegalityResult::Stall;
		}

		return LegalityResult::Legal;
	}

	bool LegalityChecker::RequiresMachineMode(unsigned short opCode)
	{
		std::optional<OpcodeInfo> info = OpcodeRegistry::GetInfo(opCode);
		return info && (static_cast<int>(info.value().Flags) & static_cast<int>(InstructionFlags::Privileged)) != 0;
	}
}
