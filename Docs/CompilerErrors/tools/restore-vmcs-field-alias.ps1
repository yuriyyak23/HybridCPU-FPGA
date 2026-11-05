$base='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Virtualization\Compatibility\Generated\VmcsProjection'
$p=Join-Path $base 'VmcsFieldAliasProjection.h'; $s=[IO.File]::ReadAllText($p)
$s=$s.Replace('#pragma once',"#pragma once`r`n#include `"../../FrozenAbi/VmcsFieldAliases/VmcsField.h`"`r`n#include `"../../../../Runtime/Domains/Descriptors/EvidencePolicy/EvidencePolicyDescriptor.h`"`r`n#include <memory>`r`n#include <string>`r`n#include <utility>")
$i=$s.IndexOf("// C# TO C++ CONVERTER TASK: C# 'records'"); if($i -lt 0){throw 'anchor missing'}
$s=$s.Substring(0,$i)+@'
struct VmcsFieldAliasRequest {
    VmcsField Field{}; VmcsFieldAliasAccess Access{}; EvidenceVisibilityClass EvidenceClass{};
    bool GeneratedAliasDeclared{}, DescriptorValidated{}, AllowWrite{};
};
struct VmcsFieldAliasResult {
    const VmcsFieldAliasDecision Decision; const VmcsField Field; const std::wstring Reason; const bool IsAllowed;
    VmcsFieldAliasResult(VmcsFieldAliasDecision decision, VmcsField field, std::wstring reason) : Decision(decision), Field(field), Reason(std::move(reason)), IsAllowed(decision == VmcsFieldAliasDecision::Allowed) {}
    static std::shared_ptr<VmcsFieldAliasResult> Allowed(VmcsField field) { return std::make_shared<VmcsFieldAliasResult>(VmcsFieldAliasDecision::Allowed, field, L""); }
    static std::shared_ptr<VmcsFieldAliasResult> Denied(VmcsFieldAliasDecision decision, VmcsField field, std::wstring reason) { return std::make_shared<VmcsFieldAliasResult>(decision, field, std::move(reason)); }
};
}
'@
[IO.File]::WriteAllText($p,$s)
$p=Join-Path $base 'YAKSys_Hybrid_CPU.Core.VmcsFieldAliasProjection.h'; $s=[IO.File]::ReadAllText($p); $s=$s.Replace('#include <memory>',"#include `"VmcsFieldAliasProjection.h`"`r`n#include <memory>"); [IO.File]::WriteAllText($p,$s)
$p=Join-Path $base 'YAKSys_Hybrid_CPU.Core.VmcsFieldAliasProjection.cpp'; $s=[IO.File]::ReadAllText($p)
$enum=[IO.File]::ReadAllText((Join-Path $base '../../FrozenAbi/VmcsFieldAliases/VmcsField.h')); $names=[regex]::Matches($enum,'(?m)^\s*([A-Za-z][A-Za-z0-9_]*)\s*=\s*(?:0x[0-9A-Fa-f]+|[0-9]+)\s*,?\s*$') | ForEach-Object {$_.Groups[1].Value}; if($names.Count -lt 10){throw 'enum names missing'}
$cases=($names | ForEach-Object {"        case VmcsField::$($_): return true;"}) -join "`r`n"
$func="namespace { bool IsDefinedVmcsField(VmcsField field) { switch (field) {`r`n$cases`r`n        default: return false;`r`n    } } }`r`n"
$s=$s.Replace('std::shared_ptr<VmcsFieldAliasResult> VmcsFieldAliasProjection::ValidateAccess',"$func`r`n`tstd::shared_ptr<VmcsFieldAliasResult> VmcsFieldAliasProjection::ValidateAccess").Replace('Enum::IsDefined(typeof(VmcsField), request->Field)','IsDefinedVmcsField(request->Field)'); [IO.File]::WriteAllText($p,$s)