#include "YAKSys_Hybrid_CPU.Core.CompatAbiFreezeContract.h"
#include "../../../../../Arch/CsrAddresses.h"
#include "../../Compatibility/FrozenAbi/VmcsFieldAliases/VmcsField.h"
#include <array>
#include <span>

using namespace YAKSys_Hybrid_CPU::Arch;

namespace YAKSys_Hybrid_CPU::Core
{

	std::span<const unsigned short> CompatAbiFreezeContract::getFrozenCsrAliases()
	{
		static constexpr std::array<unsigned short, 5> aliases{ CsrAddresses::VmxEnable, CsrAddresses::VmxCaps, CsrAddresses::VmxControl, CsrAddresses::VmxExitReason, CsrAddresses::VmxExitQual };
		return aliases;
	}

	bool CompatAbiFreezeContract::IsFrozenOpcode(unsigned short opcode)
	{
		return (opcode >= FrozenCoreOpcodeFirst && opcode <= FrozenCoreOpcodeLast) || opcode == FrozenPointerStoreOpcode || opcode == FrozenCallOpcode || (opcode >= FrozenExtensionOpcodeFirst && opcode <= FrozenExtensionOpcodeLast);
	}

	bool CompatAbiFreezeContract::IsFrozenCsrAlias(unsigned short csrAddress)
	{
		for (auto frozen : getFrozenCsrAliases())
		{
			if (csrAddress == frozen)
			{
				return true;
			}
		}

		return false;
	}

	bool CompatAbiFreezeContract::IsFrozenVmcsFieldAlias(unsigned short fieldId)
	{
		switch (static_cast<VmcsField>(fieldId))
		{
		case VmcsField::GuestPc: case VmcsField::GuestSp: case VmcsField::GuestFlags: case VmcsField::GuestCr0: case VmcsField::GuestCr3: case VmcsField::GuestCr4:
		case VmcsField::HostPc: case VmcsField::HostSp: case VmcsField::HostFlags: case VmcsField::HostCr0: case VmcsField::HostCr3:
		case VmcsField::PinBasedControls: case VmcsField::ProcBasedControls: case VmcsField::ExitControls: case VmcsField::EntryControls:
		case VmcsField::EptPointer: case VmcsField::Vpid: case VmcsField::SecondaryProcControls: case VmcsField::Cr3TargetCount:
		case VmcsField::ExitReason: case VmcsField::ExitQualification: case VmcsField::GuestPhysicalAddress: case VmcsField::EptViolationQualification:
			return true;
		default:
			return false;
		}
	}

	CompatAbiFreezeViolation CompatAbiFreezeContract::ValidateOpcode(unsigned short opcode)
	{
		return IsFrozenOpcode(opcode) ? CompatAbiFreezeViolation::None : CompatAbiFreezeViolation::OpcodeOutsideFrozenSet;
	}

	CompatAbiFreezeViolation CompatAbiFreezeContract::ValidateCsrAlias(unsigned short csrAddress)
	{
		return IsFrozenCsrAlias(csrAddress) ? CompatAbiFreezeViolation::None : CompatAbiFreezeViolation::CsrOutsideFrozenAliasSet;
	}

	CompatAbiFreezeViolation CompatAbiFreezeContract::ValidateVmcsFieldAlias(unsigned short fieldId)
	{
		return IsFrozenVmcsFieldAlias(fieldId) ? CompatAbiFreezeViolation::None : CompatAbiFreezeViolation::VmcsFieldOutsideFrozenAliasSet;
	}
}
