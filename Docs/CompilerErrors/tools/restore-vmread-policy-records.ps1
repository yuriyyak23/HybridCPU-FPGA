$ErrorActionPreference='Stop'
$root=Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
$base=Join-Path $root 'CloseToHSL\\Core\Runtime\Governance\Virtualization'
foreach($prefix in @('VmRead','MemoryOwnedVmRead','GuestPcSpFlagsVmRead')) {
 $p=Join-Path $base ($prefix+'ScalarDeliveryAcceptedPolicyResolver.h'); $s=Get-Content $p -Raw
 if($s.Contains("struct ${prefix}ScalarDeliveryPolicyLookup final")){continue}
 $s=$s.Replace('#include <string>', "#include <string>`r`n#include <vector>")
 $extra=if($prefix -eq 'VmRead'){''}else{'std::wstring ValueSource;'}
 $body=@"
    class AcceptedVmReadScalarDeliveryDecisionV2;
    enum class VirtualizationDecisionResultAbiV2 : unsigned char;
    enum class VirtualizationDecisionEffectClassV2 : unsigned char;
    enum class VirtualizationCapabilityRequirementV2 : unsigned char;
    enum class VirtualizationOperationMigrationPolicyV2 : unsigned char;
    struct ${prefix}ScalarDeliveryPolicyLookup final {
        std::wstring DecisionId, OperationNamespace, OperationId;
        std::vector<unsigned short> FieldIds;
        std::wstring SourceOwner;
        $extra
        VirtualizationDecisionResultAbiV2 ResultAbi{};
        VirtualizationDecisionEffectClassV2 EffectClass{};
        VirtualizationCapabilityRequirementV2 CapabilityRequirement{};
        VirtualizationOperationMigrationPolicyV2 MigrationPolicy{};
        bool Revoked = false;
    };
    struct ${prefix}ScalarDeliveryPolicyResolution final {
        std::shared_ptr<AcceptedVmReadScalarDeliveryDecisionV2> Policy;
        std::wstring Reason;
        bool isResolved() const noexcept { return Policy != nullptr; }
        bool getRuntimeAuthorityGranted() const noexcept { return false; }
        bool getSourceValueAvailable() const noexcept { return false; }
        bool getResultReceiptIssued() const noexcept { return false; }
    };

"@
 $marker="`tclass ${prefix}ScalarDeliveryAcceptedPolicyResolver final"
 if(!$s.Contains($marker)){throw "Resolver not found: $prefix"}; $s=$s.Replace($marker,$body+$marker); Set-Content $p $s -Encoding UTF8 -NoNewline
}
$dir=Join-Path $root 'CloseToHSL\\Core\Runtime\Events\VmRead'
foreach($prefix in @('VmRead','MemoryOwnedVmRead','GuestPcSpFlagsVmRead')) {
 $p=Join-Path $dir ($prefix+'ScalarDeliveryCanonicalComposition.h'); if(!(Test-Path $p)){continue}; $s=Get-Content $p -Raw
 $include='#include "../../Governance/Virtualization/'+$prefix+'ScalarDeliveryAcceptedPolicyResolver.h"'
 if(!$s.Contains($include)){$s=$s.Replace('#pragma once',"#pragma once`r`n"+$include)}
 Set-Content $p $s -Encoding UTF8 -NoNewline
}
