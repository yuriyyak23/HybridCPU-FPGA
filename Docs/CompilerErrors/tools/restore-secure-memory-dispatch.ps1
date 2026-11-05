$p='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Runtime\Domains\SecureCompute\Policies\Memory\YAKSys_Hybrid_CPU.Core.SecureMemoryAdmissionPolicy.cpp'; $s=[IO.File]::ReadAllText($p)
$anchor="// C# TO C++ CONVERTER TASK: The following 'switch expression' was not converted by C# to C++ Converter:"; $i=$s.IndexOf($anchor); $end=$s.IndexOf("`n`t}",$i); if($i -lt 0 -or $end -lt 0){throw 'switch anchor missing'}
$replacement=@'
switch (request->Kind) {
        case SecureMemoryAccessKind::RuntimeTouch: return AdmitRuntimeTouch(region);
        case SecureMemoryAccessKind::HostRead: return AdmitHostRead(region);
        case SecureMemoryAccessKind::DmaRead:
        case SecureMemoryAccessKind::DmaWrite: return AdmitDma(descriptor, secureIoPolicy, region, request);
        case SecureMemoryAccessKind::Measurement: return AdmitMeasurement(region);
        default: return Deny(SecureMemoryAdmissionDecision::DeniedRegionNotFound, L"Secure memory access kind is not recognized by the secure memory policy.");
        }
'@
$s=$s.Substring(0,$i)+$replacement+$s.Substring($end); [IO.File]::WriteAllText($p,$s)