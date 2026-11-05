$r='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core'; $p=Join-Path $r 'System\VmxRootDescriptorReference.h'; $s=[IO.File]::ReadAllText($p); if(-not $s.Contains('struct VmxRootDescriptorReference final')) { $i=$s.LastIndexOf('}'); $s=$s.Insert($i,@"
struct VmxRootDescriptorReference final {
    unsigned long long Address = 0;
    bool IsLegacyDefault = false;
    bool isExplicit() const noexcept { return !IsLegacyDefault && Address != 0; }
    bool isValid() const noexcept { return IsLegacyDefault || Address != 0; }
    static VmxRootDescriptorReference FromOperand(unsigned long long address) noexcept { return address == 0 ? VmxRootDescriptorReference{0, true} : VmxRootDescriptorReference{address, false}; }
    static const VmxRootDescriptorReference CompatibilityDefault;
    static const VmxRootDescriptorReference LegacyDefault;
    bool operator==(const VmxRootDescriptorReference&) const = default;
};
inline const VmxRootDescriptorReference VmxRootDescriptorReference::CompatibilityDefault{0, true};
inline const VmxRootDescriptorReference VmxRootDescriptorReference::LegacyDefault = VmxRootDescriptorReference::CompatibilityDefault;
"@+"`r`n") }; [IO.File]::WriteAllText($p,$s)
$p=Join-Path $r 'Virtualization\Compatibility\Frontend\Retire\VmxRetireModel.h'; $s=[IO.File]::ReadAllText($p); if(-not $s.Contains('#include "../../../../System/VmxRootDescriptorReference.h"')) {$s=$s.Replace('#pragma once',"#pragma once`r`n#include `"../../../../System/VmxRootDescriptorReference.h`"")}; if(-not $s.Contains('class TrapDecision;')) {$a='namespace YAKSys_Hybrid_CPU::Core'+"`r`n"+'{'; $s=$s.Replace($a,$a+"`r`n    class TrapDecision;")}; [IO.File]::WriteAllText($p,$s)
