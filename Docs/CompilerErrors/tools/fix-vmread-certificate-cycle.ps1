$d='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Runtime\Events\VmRead'
foreach($name in @('VmReadScalarDeliveryCanonicalComposition.h','VmReadScalarDeliveryCanonicalComposition.cpp')) {
 $p=Join-Path $d $name; $s=[IO.File]::ReadAllText($p).Replace('SafetyVerifier::VirtualizationAdmissionCertificate','VirtualizationAdmissionCertificate')
 if($name -like '*.h') {
  $s=$s.Replace('#include "../../../Pipeline/Safety/YAKSys_Hybrid_CPU.Core.SafetyVerifier.h"','#include "../../../Pipeline/Safety/VirtualizationAdmissionCertificate.h"').Replace('#include "../../../Pipeline/MicroOps/Types/MicroOp.IO.h"','')
  $s=$s.Replace('class IVmReadScalarResultReceiptOwner', "struct VmReadScalarDeliveryPolicyResolution;`r`n`tstruct VmReadScalarDeliveryPolicyLookup;`r`n`tclass VmxMicroOp;`r`n`tclass IVmReadScalarResultReceiptOwner")
  $i=$s.IndexOf("// C# TO C++ CONVERTER WARNING: Nullable reference types"); $end=$s.IndexOf('struct VmReadScalarDeliveryPolicyResolution;',$i); if($i -lt 0 -or $end -lt 0){throw 'result anchor missing'}
  $def=@'
struct VmReadScalarDeliveryResult {
    const VmReadScalarDeliveryDecision Decision; const std::shared_ptr<VmReadScalarResultReceipt> Receipt; const std::wstring Reason;
    const bool IsPrepared; static constexpr bool BackendExecutionAuthorized=false, UnderlyingVirtualizationMutationAuthorized=false, CompletionPublicationAuthorized=false, VmxRetireEffectAuthorized=false;
    VmReadScalarDeliveryResult(VmReadScalarDeliveryDecision decision, std::shared_ptr<VmReadScalarResultReceipt> receipt, std::wstring reason) : Decision(decision), Receipt(std::move(receipt)), Reason(std::move(reason)), IsPrepared(decision == VmReadScalarDeliveryDecision::Prepared && Receipt != nullptr) {}
};
'@
  $s=$s.Substring(0,$i)+$def+"`r`n`t"+$s.Substring($end)
 } else { $s=$s.Replace('#include "VmReadScalarDeliveryCanonicalComposition.h"', "#include `"VmReadScalarDeliveryCanonicalComposition.h`"`r`n#include `"../../../Pipeline/MicroOps/Types/MicroOp.IO.h`"`r`n#include `"../../../Pipeline/Safety/YAKSys_Hybrid_CPU.Core.SafetyVerifier.h`"") }
 [IO.File]::WriteAllText($p,$s)
}