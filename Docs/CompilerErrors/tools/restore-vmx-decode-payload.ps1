$d='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Virtualization\Compatibility\Frontend\Decode'
$source=[IO.File]::ReadAllText('C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Architecture\Common\YAKSys_Hybrid_CPU.Processor.h')
$names=@('VMXON','VMXOFF','VMLAUNCH','VMRESUME','VMREAD','VMWRITE','VMCLEAR','VMPTRLD','VMPTRST','VMCALL','INVEPT','INVVPID','VMFUNC','VMSAVEX','VMRESTX')
$lines=foreach($name in $names){$m=[regex]::Match($source,'static constexpr unsigned short '+$name+'\s*=\s*(\d+)\s*;'); if(-not $m.Success){throw "opcode $name missing"}; "    inline constexpr unsigned short $name = $($m.Groups[1].Value);"}
[IO.File]::WriteAllText((Join-Path $d 'VmxFrozenOpcodeValues.h'),"#pragma once`r`nnamespace YAKSys_Hybrid_CPU::Core::VmxFrozenOpcodeValues {`r`n"+($lines -join "`r`n")+"`r`n}`r`n")
$p=Join-Path $d 'VmxInstructionPayload.h'; $s=[IO.File]::ReadAllText($p)
$s=$s.Replace('#pragma once',"#pragma once`r`n#include `"../../FrozenAbi/OperandForms/VmxOperandForm.h`"`r`n#include `"../../FrozenAbi/OperandForms/VmxInvalidationScope.h`"`r`n#include `"../../FrozenAbi/FunctionLeaves/VmxFunctionLeaf.h`"`r`n#include `"../../FrozenAbi/VmcsFieldAliases/VmxExitQualification.h`"`r`n#include `"../../../../System/VmxRootDescriptorReference.h`"`r`n#include <memory>")
$i=$s.IndexOf("// C# TO C++ CONVERTER TASK: C# 'records'"); $end=$s.IndexOf('class VmxV2InstructionCaps final',$i); if($i -lt 0 -or $end -lt 0){throw 'payload anchor missing'}
$def=@'
struct VmxInstructionPayload {
    const Vmx::VmxOperandForm OperandForm;
    const VmxInvalidationScope InvalidationScope;
    const VmxFunctionLeaf FunctionLeaf;
    const VmxRootDescriptorReference RootDescriptor;
    const VmxExitQualification ExitQualification;
    VmxInstructionPayload(Vmx::VmxOperandForm form, VmxInvalidationScope scope, VmxFunctionLeaf leaf, VmxRootDescriptorReference root, VmxExitQualification qualification) : OperandForm(form), InvalidationScope(scope), FunctionLeaf(leaf), RootDescriptor(root), ExitQualification(qualification) {}
    static const std::shared_ptr<VmxInstructionPayload> Empty;
    static std::shared_ptr<VmxInstructionPayload> FromDecodedRegisters(unsigned short opcode, unsigned char rd, unsigned char rs1, unsigned char rs2);
    static VmxInvalidationScope DecodeInvalidationScope(unsigned long long value);
    static VmxFunctionLeaf DecodeFunctionLeaf(unsigned long long value);
};
'@
$s=$s.Substring(0,$i)+$def+"`r`n`t"+$s.Substring($end); [IO.File]::WriteAllText($p,$s)
$p=Join-Path $d 'VmxInstructionPayload.cpp'; $s=@'
#include "VmxInstructionPayload.h"
#include "VmxFrozenOpcodeValues.h"
namespace YAKSys_Hybrid_CPU::Core {
using namespace VmxFrozenOpcodeValues;
using Vmx::VmxOperandForm;
const std::shared_ptr<VmxInstructionPayload> VmxInstructionPayload::Empty = std::make_shared<VmxInstructionPayload>(VmxOperandForm::None, VmxInvalidationScope::None, VmxFunctionLeaf::None, VmxRootDescriptorReference::CompatibilityDefault, VmxExitQualification());
VmxInvalidationScope VmxInstructionPayload::DecodeInvalidationScope(unsigned long long v) { switch(v) { case 1: return VmxInvalidationScope::SingleContext; case 2: return VmxInvalidationScope::AllContexts; case 3: return VmxInvalidationScope::SingleAddress; default: return VmxInvalidationScope::None; } }
VmxFunctionLeaf VmxInstructionPayload::DecodeFunctionLeaf(unsigned long long v) { switch(v) { case 1: return VmxFunctionLeaf::CapabilityQuery; case 7: return VmxFunctionLeaf::Lane7QueryCaps; case 8: return VmxFunctionLeaf::Lane7Submit; default: return VmxFunctionLeaf::None; } }
std::shared_ptr<VmxInstructionPayload> VmxInstructionPayload::FromDecodedRegisters(unsigned short opcode, unsigned char, unsigned char rs1, unsigned char rs2) {
    auto form=VmxOperandForm::None; auto scope=VmxInvalidationScope::None; auto leaf=VmxFunctionLeaf::None; auto root=VmxRootDescriptorReference::CompatibilityDefault; unsigned short qualLeaf=0; VmxInvalidationScope qualScope=VmxInvalidationScope::None; unsigned long long qualDescriptor=0;
    switch(opcode) {
    case VMXON: form=VmxOperandForm::ReservedIgnoredRegister; root=VmxRootDescriptorReference::FromOperand(rs1); break;
    case VMREAD: form=VmxOperandForm::FieldSelectorToRegister; break;
    case VMWRITE: form=VmxOperandForm::FieldSelectorAndValueRegisters; break;
    case VMCLEAR: case VMPTRLD: form=VmxOperandForm::VmcsPointerRegister; break;
    case VMPTRST: form=VmxOperandForm::CurrentVmcsPointerToRegister; break;
    case VMCALL: form=VmxOperandForm::HypercallLeafAndDescriptor; qualLeaf=rs1; qualDescriptor=rs2; break;
    case INVEPT: case INVVPID: form=VmxOperandForm::InvalidationScopeAndDescriptor; scope=DecodeInvalidationScope(rs1); qualScope=scope; qualDescriptor=rs2; break;
    case VMFUNC: form=VmxOperandForm::FunctionLeafAndDescriptor; leaf=DecodeFunctionLeaf(rs1); qualLeaf=rs1; qualDescriptor=rs2; break;
    case VMSAVEX: case VMRESTX: form=VmxOperandForm::ExtendedStateMaskAndDescriptor; qualLeaf=rs1; qualDescriptor=rs2; break;
    default: return Empty;
    }
    return std::make_shared<VmxInstructionPayload>(form,scope,leaf,root,VmxExitQualification(qualLeaf,qualScope,qualDescriptor));
}
}
'@
[IO.File]::WriteAllText($p,$s)
$p=Join-Path $d 'VmxCompatDecodeBoundary.h'; $s=[IO.File]::ReadAllText($p)
$s=$s.Replace('#include "../../../../Architecture/Common/YAKSys_Hybrid_CPU.Processor.h"',"#include `"VmxInstructionPayload.h`"`r`n#include <memory>`r`n#include <string>`r`n#include <utility>")
$i=$s.IndexOf("// C# TO C++ CONVERTER TASK: C# 'records'"); if($i -lt 0){throw 'decode record anchor missing'}
$s=$s.Substring(0,$i)+@'
struct VmxCompatDecodeRequest {
    const unsigned short Opcode; const unsigned char Rd, Rs1, Rs2; const bool DescriptorValidated, CapabilityValidated, SchedulingValidated, NoEmissionValidated;
    VmxCompatDecodeRequest(unsigned short opcode, unsigned char rd, unsigned char rs1, unsigned char rs2, bool descriptor, bool capability, bool scheduling, bool noEmission) : Opcode(opcode), Rd(rd), Rs1(rs1), Rs2(rs2), DescriptorValidated(descriptor), CapabilityValidated(capability), SchedulingValidated(scheduling), NoEmissionValidated(noEmission) {}
};
struct VmxCompatDecodeResult {
    const VmxCompatDecodeDecision Decision; const std::shared_ptr<VmxInstructionPayload> Payload; const std::wstring Reason; const bool IsAllowed;
    VmxCompatDecodeResult(VmxCompatDecodeDecision decision, std::shared_ptr<VmxInstructionPayload> payload, std::wstring reason) : Decision(decision), Payload(std::move(payload)), Reason(std::move(reason)), IsAllowed(decision == VmxCompatDecodeDecision::Allowed) {}
    static std::shared_ptr<VmxCompatDecodeResult> Allowed(std::shared_ptr<VmxInstructionPayload> payload) { return std::make_shared<VmxCompatDecodeResult>(VmxCompatDecodeDecision::Allowed, std::move(payload), L""); }
    static std::shared_ptr<VmxCompatDecodeResult> Denied(VmxCompatDecodeDecision decision, std::wstring reason) { return std::make_shared<VmxCompatDecodeResult>(decision, VmxInstructionPayload::Empty, std::move(reason)); }
};
}
'@
[IO.File]::WriteAllText($p,$s)
$p=Join-Path $d 'YAKSys_Hybrid_CPU.Core.VmxCompatDecodeBoundary.h'; $s=[IO.File]::ReadAllText($p).Replace('#include "../../../../Architecture/Common/YAKSys_Hybrid_CPU.Processor.h"','#include "VmxCompatDecodeBoundary.h"').Replace("struct VmxCompatDecodeRequest;`r`n`tstruct VmxCompatDecodeResult;`r`n`t",''); [IO.File]::WriteAllText($p,$s)
$p=Join-Path $d 'YAKSys_Hybrid_CPU.Core.VmxCompatDecodeBoundary.cpp'; $s=[IO.File]::ReadAllText($p)
$s=$s.Replace('#include "VmxCompatDecodeBoundary.h"',"#include `"VmxCompatDecodeBoundary.h`"`r`n#include `"VmxFrozenOpcodeValues.h`"").Replace('YAKSys_Hybrid_CPU::Processor::CPU_Core::IsaOpcodeValues::','VmxFrozenOpcodeValues::').Replace('IsaOpcodeValues::','VmxFrozenOpcodeValues::'); [IO.File]::WriteAllText($p,$s)