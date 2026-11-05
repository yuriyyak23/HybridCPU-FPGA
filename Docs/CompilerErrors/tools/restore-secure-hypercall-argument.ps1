$p='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Runtime\Domains\SecureCompute\Descriptors\Hypercalls\SecureHypercallDescriptor.h'; $s=[IO.File]::ReadAllText($p); if(-not $s.Contains('struct SecureHypercallArgumentDescriptor final')) { $s=$s.Replace('#pragma once',"#pragma once`r`n#include `"../../Authority/SecureGrantHandle.h`""); $anchor='// C# TO C++ CONVERTER TASK:'; $def=@"
struct SecureHypercallArgumentDescriptor final {
    unsigned char Index = 0;
    SecureHypercallArgumentClass ArgumentClass = SecureHypercallArgumentClass::Immediate;
    unsigned long long SharedBufferId = 0;
    SecureGrantHandle Grant;
    bool requiresSharedBuffer() const noexcept { return ArgumentClass == SecureHypercallArgumentClass::ExplicitSharedBuffer; }
    bool isDeniedRawPrivatePointer() const noexcept { return ArgumentClass == SecureHypercallArgumentClass::RawPrivatePointerDenied; }
    bool operator==(const SecureHypercallArgumentDescriptor&) const = default;
};
"@; if(-not $s.Contains($anchor)){throw 'anchor missing'}; $s=$s.Replace($anchor,$def+"`r`n"+$anchor) }; [IO.File]::WriteAllText($p,$s)
$p='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Runtime\Domains\SecureCompute\Descriptors\Hypercalls\YAKSys_Hybrid_CPU.Core.SecureHypercallDescriptor.h'; $s=[IO.File]::ReadAllText($p); if(-not $s.Contains('#include "SecureHypercallDescriptor.h"')) { $s=$s.Replace('#pragma once',"#pragma once`r`n#include `"SecureHypercallDescriptor.h`"") }; [IO.File]::WriteAllText($p,$s)
