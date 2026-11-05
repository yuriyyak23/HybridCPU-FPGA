$base='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Runtime\Events\Traps'
$p=Join-Path $base 'TrapRequest.h'; $s=[IO.File]::ReadAllText($p); $s=$s.Replace('#pragma once',"#pragma once`r`n#include <limits>")
$i=$s.IndexOf("// C# TO C++ CONVERTER TASK: C# 'records'"); if($i -lt 0){throw 'range anchor missing'}
$s=$s.Substring(0,$i)+@'
struct MemoryTrapRange {
    const unsigned long long BaseAddress, Length; const TrapAccessMask AccessMask; const bool IsValid;
    MemoryTrapRange(unsigned long long base, unsigned long long length, TrapAccessMask access) : BaseAddress(base), Length(length), AccessMask(access), IsValid(length != 0 && access != TrapAccessMask::None) {}
    static unsigned long long SaturatingEnd(unsigned long long address, unsigned long long length) { if (!length) length=1; auto end=address+length; return end < address ? std::numeric_limits<unsigned long long>::max() : end; }
    static TrapAccessMask ToMask(TrapAccessType access) { switch (access) { case TrapAccessType::Read: return TrapAccessMask::Read; case TrapAccessType::Write: return TrapAccessMask::Write; case TrapAccessType::Execute: return TrapAccessMask::Execute; case TrapAccessType::CompatibilityOperation: return TrapAccessMask::CompatibilityOperation; case TrapAccessType::LaneOperation: return TrapAccessMask::LaneOperation; case TrapAccessType::TimerExpiry: return TrapAccessMask::TimerExpiry; default: return TrapAccessMask::None; } }
    bool Matches(unsigned long long address, unsigned long long length, TrapAccessType access) const {
        if (!IsValid || (static_cast<unsigned char>(AccessMask) & static_cast<unsigned char>(ToMask(access))) == 0) return false;
        return address < SaturatingEnd(BaseAddress, Length) && SaturatingEnd(address, length) > BaseAddress;
    }
};
}
'@
[IO.File]::WriteAllText($p,$s)
$p=Join-Path $base 'YAKSys_Hybrid_CPU.Core.TrapPolicyBitmap.h'; $s=[IO.File]::ReadAllText($p).Replace('#include "../../../Virtualization/Compatibility/Frontend/Retire/VmxRetireModel.h"','#include "NeutralTrapResult.h"'); [IO.File]::WriteAllText($p,$s)
$p=Join-Path $base 'YAKSys_Hybrid_CPU.Core.TrapPolicyBitmap.cpp'; $s=[IO.File]::ReadAllText($p)
$s=$s.Replace('_instructionOpcodes.insert(opcode))','_instructionOpcodes.insert(opcode).second)').Replace('_csrAddresses.insert(csrAddress))','_csrAddresses.insert(csrAddress).second)').Replace('_laneOperations.insert(key))','_laneOperations.insert(key).second)').Replace('operation == 0 or >= 64','operation == 0 || operation >= 64').Replace('std::find(_laneOperations.begin(), _laneOperations.end(), LaneOperationKey(laneId, opcode) != _laneOperations.end())','_laneOperations.find(LaneOperationKey(laneId, opcode)) != _laneOperations.end()')
$i=$s.IndexOf("// C# TO C++ CONVERTER TASK: The following 'switch expression'"); $end=$s.IndexOf("`n`t}",$i); if($i -lt 0 -or $end -lt 0){throw 'evaluate anchor missing'}
$def=@'
switch (request->TargetKind) {
        case TrapTargetKind::InstructionOpcode:
            if ((_enabledClasses & static_cast<unsigned long long>(TrapPolicyClass::Instruction)) && _instructionOpcodes.contains(static_cast<unsigned short>(request->Target))) return NeutralTrapResult::Trap(request, NeutralTrapResultKind::InstructionIntercept); break;
        case TrapTargetKind::CsrAddress:
            if ((_enabledClasses & static_cast<unsigned long long>(TrapPolicyClass::Csr)) && _csrAddresses.contains(static_cast<unsigned short>(request->Target))) return NeutralTrapResult::Trap(request, NeutralTrapResultKind::CsrIntercept); break;
        case TrapTargetKind::CompatibilityOperation:
            if ((_enabledClasses & static_cast<unsigned long long>(TrapPolicyClass::CompatibilityOperation)) && InterceptsCompatibilityOperation(static_cast<unsigned char>(request->Target))) return NeutralTrapResult::Trap(request, NeutralTrapResultKind::CompatibilityOperationIntercept); break;
        case TrapTargetKind::MemoryRange:
            if ((_enabledClasses & static_cast<unsigned long long>(TrapPolicyClass::Memory)) && InterceptsMemory(request->Target, request->Auxiliary, request->AccessType)) return NeutralTrapResult::Trap(request, NeutralTrapResultKind::MemoryIntercept); break;
        case TrapTargetKind::LaneOperation:
            if ((_enabledClasses & static_cast<unsigned long long>(TrapPolicyClass::LaneOperation)) && InterceptsLaneOperation(static_cast<unsigned char>(request->Target), static_cast<unsigned short>(request->Auxiliary))) return NeutralTrapResult::Trap(request, NeutralTrapResultKind::LaneOperationIntercept); break;
        default: break;
        }
        return NeutralTrapResult::Continue(request);
'@
$s=$s.Substring(0,$i)+$def+$s.Substring($end); [IO.File]::WriteAllText($p,$s)