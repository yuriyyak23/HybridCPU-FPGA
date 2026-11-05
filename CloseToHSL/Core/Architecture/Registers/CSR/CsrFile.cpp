#include "CsrFile.h"
#include "../../../../../Arch/CsrAddresses.h"
#include "../../Common/YAKSys_Hybrid_CPU.Processor.h"

using namespace YAKSys_Hybrid_CPU::Arch;

namespace YAKSys_Hybrid_CPU::Core::Registers
{

	unsigned short CsrPrivilegeFaultException::getCsrAddress() const
	{
		return CsrAddress;
	}

	PrivilegeLevel CsrPrivilegeFaultException::getRequiredLevel() const
	{
		return RequiredLevel;
	}

	PrivilegeLevel CsrPrivilegeFaultException::getActualLevel() const
	{
		return ActualLevel;
	}

	bool CsrPrivilegeFaultException::isWrite() const
	{
		return IsWrite;
	}

	CsrPrivilegeFaultException::CsrPrivilegeFaultException(unsigned short csrAddress, PrivilegeLevel required, PrivilegeLevel actual, bool isWrite)
		: std::runtime_error(std::format("CSR privilege fault: {} to 0x{:03X} requires {}, current level is {}",
			isWrite ? "write" : "read", csrAddress, static_cast<int>(required), static_cast<int>(actual))),
		  CsrAddress(csrAddress), RequiredLevel(required), ActualLevel(actual), IsWrite(isWrite)
	{
	}

	unsigned short CsrUnknownAddressException::getCsrAddress() const
	{
		return CsrAddress;
	}

	CsrUnknownAddressException::CsrUnknownAddressException(unsigned short csrAddress)
		: std::runtime_error(std::format("Unknown CSR address: 0x{:03X}", csrAddress)), CsrAddress(csrAddress)
	{
	}

	CsrFile::CsrFile() : CsrFile(nullptr)
	{
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal CsrFile(System.Func<CanonicalStateAccessKind, IDisposable>? reserveAccess)
	CsrFile::CsrFile(std::function<std::shared_ptr<Disposable>(CanonicalStateAccessKind)> reserveAccess) : _reserveAccess(reserveAccess)
	{
		RegisterDefaults();
	}

	unsigned long long CsrFile::Read(unsigned short address, PrivilegeLevel privilege)
	{
// C# TO C++ CONVERTER NOTE: The following 'using' block is replaced by its C++ equivalent:
// ORIGINAL LINE: using var access = _reserveAccess == null ? null : _reserveAccess.Invoke(CanonicalStateAccessKind.Read);
		{
			auto access = _reserveAccess == nullptr ? nullptr : _reserveAccess(CanonicalStateAccessKind::Read);
			CsrAccessPolicy policy;
			std::unordered_map<unsigned short, CsrAccessPolicy>::const_iterator _policies_iterator = _policies.find(address);
			if (_policies_iterator == _policies.end())
			{
				throw CsrUnknownAddressException(address);
			}
			else
			{
				policy = _policies_iterator->second;
			}
        
			CheckReadAccess(address, policy, privilege);
        
			std::unordered_map<unsigned short, unsigned long long>::const_iterator _values_iterator = _values.find(address);
			return _values_iterator != _values.end() ? _values_iterator->second : 0ULL;
		}
	}

	void CsrFile::Write(unsigned short address, unsigned long long value, PrivilegeLevel privilege)
	{
// C# TO C++ CONVERTER NOTE: The following 'using' block is replaced by its C++ equivalent:
// ORIGINAL LINE: using var access = _reserveAccess == null ? null : _reserveAccess.Invoke(CanonicalStateAccessKind.Write);
		{
			auto access = _reserveAccess == nullptr ? nullptr : _reserveAccess(CanonicalStateAccessKind::Write);
			CsrAccessPolicy policy;
			std::unordered_map<unsigned short, CsrAccessPolicy>::const_iterator _policies_iterator = _policies.find(address);
			if (_policies_iterator == _policies.end())
			{
				throw CsrUnknownAddressException(address);
			}
			else
			{
				policy = _policies_iterator->second;
			}
        
			CheckWriteAccess(address, policy, privilege);
        
			_values[address] = value;
		}
	}

	void CsrFile::HardwareWrite(unsigned short address, unsigned long long value)
	{
// C# TO C++ CONVERTER NOTE: The following 'using' block is replaced by its C++ equivalent:
// ORIGINAL LINE: using var access = _reserveAccess == null ? null : _reserveAccess.Invoke(CanonicalStateAccessKind.Write);
		{
			auto access = _reserveAccess == nullptr ? nullptr : _reserveAccess(CanonicalStateAccessKind::Write);
			if (!_policies.contains(address))
			{
				throw CsrUnknownAddressException(address);
			}
        
			_values[address] = value;
		}
	}

	unsigned long long CsrFile::DirectRead(unsigned short address)
	{
// C# TO C++ CONVERTER NOTE: The following 'using' block is replaced by its C++ equivalent:
// ORIGINAL LINE: using var access = _reserveAccess == null ? null : _reserveAccess.Invoke(CanonicalStateAccessKind.Read);
		{
			auto access = _reserveAccess == nullptr ? nullptr : _reserveAccess(CanonicalStateAccessKind::Read);
			std::unordered_map<unsigned short, unsigned long long>::const_iterator _values_iterator = _values.find(address);
			return _values_iterator != _values.end() ? _values_iterator->second : 0ULL;
		}
	}

	bool CsrFile::IsRegistered(unsigned short address)
	{
		return _policies.contains(address);
	}

	void CsrFile::CheckReadAccess(unsigned short address, CsrAccessPolicy policy, PrivilegeLevel privilege)
	{
		// All policies allow read from Machine mode.
		if (privilege == PrivilegeLevel::Machine)
		{
			return;
		}

		switch (policy)
		{
			case CsrAccessPolicy::ReadOnly:
			case CsrAccessPolicy::HardwareUpdated:
			case CsrAccessPolicy::SupervisorReadWrite:
				// Readable from any mode (S and U can read).
				return;

			case CsrAccessPolicy::MachineReadWrite:
				// M-mode only — U-mode and S-mode reads are denied.
				if (privilege < PrivilegeLevel::Machine)
				{
					throw CsrPrivilegeFaultException(address, PrivilegeLevel::Machine, privilege, false);
				}
				return;
		}
	}

	void CsrFile::CheckWriteAccess(unsigned short address, CsrAccessPolicy policy, PrivilegeLevel privilege)
	{
		switch (policy)
		{
			case CsrAccessPolicy::ReadOnly:
				throw CsrPrivilegeFaultException(address, PrivilegeLevel::Machine, privilege, true);

			case CsrAccessPolicy::HardwareUpdated:
				// Software writes are forbidden — must use HardwareWrite.
				throw CsrPrivilegeFaultException(address, PrivilegeLevel::Machine, privilege, true);

			case CsrAccessPolicy::MachineReadWrite:
				if (privilege < PrivilegeLevel::Machine)
				{
					throw CsrPrivilegeFaultException(address, PrivilegeLevel::Machine, privilege, true);
				}
				return;

			case CsrAccessPolicy::SupervisorReadWrite:
				if (privilege < PrivilegeLevel::Supervisor)
				{
					throw CsrPrivilegeFaultException(address, PrivilegeLevel::Supervisor, privilege, true);
				}
				return;
		}
	}

	void CsrFile::RegisterDefaults()
	{
		// ── Machine-mode privilege/system (M-mode R/W) ────────────────
		Register(CsrAddresses::Mstatus, CsrAccessPolicy::MachineReadWrite);
		Register(CsrAddresses::Misa, CsrAccessPolicy::ReadOnly);
		Register(CsrAddresses::Mie, CsrAccessPolicy::MachineReadWrite);
		Register(CsrAddresses::Mtvec, CsrAccessPolicy::MachineReadWrite);
		Register(CsrAddresses::Mscratch, CsrAccessPolicy::MachineReadWrite);
		Register(CsrAddresses::Mepc, CsrAccessPolicy::MachineReadWrite);
		Register(CsrAddresses::Mcause, CsrAccessPolicy::MachineReadWrite);
		Register(CsrAddresses::Mtval, CsrAccessPolicy::MachineReadWrite);
		Register(CsrAddresses::Mip, CsrAccessPolicy::MachineReadWrite);
		Register(CsrAddresses::MpowerState, CsrAccessPolicy::MachineReadWrite, static_cast<unsigned long long>(Processor::CPU_Core::CorePowerState::C0_Active));
		Register(CsrAddresses::MperfLevel, CsrAccessPolicy::MachineReadWrite, static_cast<unsigned long long>(Processor::CPU_Core::CorePowerState::P0_MaxPerformance));

		// ── Supervisor-mode (S-mode R/W) ──────────────────────────────
		Register(CsrAddresses::Sstatus, CsrAccessPolicy::SupervisorReadWrite);
		Register(CsrAddresses::Sie, CsrAccessPolicy::SupervisorReadWrite);
		Register(CsrAddresses::Stvec, CsrAccessPolicy::SupervisorReadWrite);
		Register(CsrAddresses::Sscratch, CsrAccessPolicy::SupervisorReadWrite);
		Register(CsrAddresses::Sepc, CsrAccessPolicy::SupervisorReadWrite);
		Register(CsrAddresses::Scause, CsrAccessPolicy::SupervisorReadWrite);
		Register(CsrAddresses::Stval, CsrAccessPolicy::SupervisorReadWrite);
		Register(CsrAddresses::Sip, CsrAccessPolicy::SupervisorReadWrite);

		// ── VT / SMT (hardware-updated, read-only for software) ───────
		Register(CsrAddresses::VtId, CsrAccessPolicy::HardwareUpdated);
		Register(CsrAddresses::VtMask, CsrAccessPolicy::HardwareUpdated);
		Register(CsrAddresses::VtStatus, CsrAccessPolicy::HardwareUpdated);
		Register(CsrAddresses::VtCause, CsrAccessPolicy::HardwareUpdated);

		// ── Capability / Configuration (read-only discovery) ──────────
		Register(CsrAddresses::IsaCaps, CsrAccessPolicy::ReadOnly);
		Register(CsrAddresses::LaneCaps, CsrAccessPolicy::ReadOnly);
		Register(CsrAddresses::BundleCaps, CsrAccessPolicy::ReadOnly);
		Register(CsrAddresses::SafetyCaps, CsrAccessPolicy::ReadOnly);
		Register(CsrAddresses::MemCaps, CsrAccessPolicy::ReadOnly);

		// ── VMX (M-mode R/W — VmxEnable also updated by VMXON/VMXOFF) ─
		Register(CsrAddresses::VmxEnable, CsrAccessPolicy::MachineReadWrite);
		Register(CsrAddresses::VmxCaps, CsrAccessPolicy::ReadOnly);
		Register(CsrAddresses::VmxControl, CsrAccessPolicy::MachineReadWrite);
		Register(CsrAddresses::VmxExitReason, CsrAccessPolicy::MachineReadWrite);
		Register(CsrAddresses::VmxExitQual, CsrAccessPolicy::MachineReadWrite);

		// ── Performance counters (read-only from software, updated by HW)
		Register(CsrAddresses::Cycle, CsrAccessPolicy::HardwareUpdated);
		Register(CsrAddresses::BundleRet, CsrAccessPolicy::HardwareUpdated);
		Register(CsrAddresses::InstrRet, CsrAccessPolicy::HardwareUpdated);
		Register(CsrAddresses::VmExitCnt, CsrAccessPolicy::HardwareUpdated);
		Register(CsrAddresses::BarrierCnt, CsrAccessPolicy::HardwareUpdated);
		Register(CsrAddresses::StealCnt, CsrAccessPolicy::HardwareUpdated);
		Register(CsrAddresses::ReplayCnt, CsrAccessPolicy::HardwareUpdated);
	}

	void CsrFile::Register(unsigned short address, CsrAccessPolicy policy, unsigned long long defaultValue)
	{
		_policies[address] = policy;
		_values[address] = defaultValue;
	}
}
