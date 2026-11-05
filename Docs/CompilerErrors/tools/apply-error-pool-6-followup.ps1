$ErrorActionPreference = 'Stop'
$root = 'C:\Users\Yuriy\Desktop\HybridCPU_FPGA'

function Replace-Once([string]$RelativePath, [string]$Old, [string]$New) {
    $path = Join-Path $root $RelativePath
    $content = [System.IO.File]::ReadAllText($path)
    if ($content.Contains($New)) { return }
    $count = ([regex]::Matches($content, [regex]::Escape($Old))).Count
    if ($count -ne 1) { throw "Expected one match in $RelativePath, found $count" }
    [System.IO.File]::WriteAllText($path, $content.Replace($Old, $New), [System.Text.UTF8Encoding]::new($false))
}

function Replace-Constructor([string]$RelativePath, [string]$Start, [string]$End, [string]$Replacement) {
    $path = Join-Path $root $RelativePath
    $content = [System.IO.File]::ReadAllText($path)
    $startIndex = $content.IndexOf($Start, [StringComparison]::Ordinal)
    if ($startIndex -lt 0) {
        if ($content.Contains($Replacement)) { return }
        throw "Constructor start not found in $RelativePath"
    }
    $endIndex = $content.IndexOf($End, $startIndex, [StringComparison]::Ordinal)
    if ($endIndex -lt 0) { throw "Constructor boundary not found in $RelativePath" }
    $updated = $content.Substring(0, $startIndex) + $Replacement + "`r`n`r`n`t" + $content.Substring($endIndex)
    [System.IO.File]::WriteAllText($path, $updated, [System.Text.UTF8Encoding]::new($false))
}

$contract = 'CloseToHSL\\Core\Runtime\Domains\SecureCompute\Descriptors\Hypercalls\SecureHypercallBackendContract.h'
Replace-Once $contract 'SecureGrantHandle::None' 'std::make_shared<SecureGrantHandle>()'
Replace-Once $contract 'nullptr, nullptr, nullptr, SecureRevocationEpoch::Unmaterialized, nullptr, std::make_shared<SecureGrantHandle>()' 'std::make_shared<SecureHypercallDecodedLeaf>(), std::make_shared<SecureComputeServiceId>(), std::make_shared<SecureBackendOwnerId>(), SecureRevocationEpoch::Unmaterialized, std::make_shared<SecureHypercallContractVersion>(), std::make_shared<SecureGrantHandle>()'

$contractCppApi = 'CloseToHSL\\Core\Runtime\Domains\SecureCompute\Descriptors\Hypercalls\SecureHypercallBackendContract.cpp'
Replace-Once $contractCppApi 'getOwnerEpoch()->IsMaterialized' 'getOwnerEpoch()->isMaterialized()'
Replace-Once $contractCppApi 'getRequiredGrant()->IsMaterialized' 'getRequiredGrant()->isMaterialized()'
$migrationCpp = 'CloseToHSL\\Core\Runtime\Domains\SecureCompute\Policies\Migration\YAKSys_Hybrid_CPU.Core.SecureMigrationAdmissionPolicy.cpp'
Replace-Once $migrationCpp 'migration->getPolicyEpoch()->IsMaterialized' 'migration->getPolicyEpoch()->isMaterialized()'
Replace-Once $migrationCpp 'expectedPolicyEpoch->IsMaterialized' 'expectedPolicyEpoch->isMaterialized()'

$admission = 'CloseToHSL\\Core\Runtime\Domains\SecureCompute\Policies\Backend\SecureHypercallBackendContractAdmissionPolicy.cpp'
Replace-Once $admission 'if ((contract == nullptr ? nullptr : contract->isMaterialized()) != true)' 'if (contract == nullptr || !contract->isMaterialized())'
Replace-Once $admission 'if (!request->EvidenceEpoch->Equals(request->CurrentEpoch))' 'if (!request->EvidenceEpoch || !request->CurrentEpoch || !request->EvidenceEpoch->Equals(request->CurrentEpoch))'
Replace-Once $admission 'argument->Grant.MatchesEpoch(request->CurrentEpoch)' 'request->CurrentEpoch && argument->Grant.MatchesEpoch(*request->CurrentEpoch)'
Replace-Once $admission '(request->IoPolicy == nullptr ? nullptr : request->IoPolicy.TryFindCurrentSharedBuffer(argument->Value, request->ValidatedDomainTag, request->CurrentEpoch, buffer)) != true || argument->Length == 0 || argument->Length > buffer->Length || (!request->CurrentEpoch || !argument->Grant.MatchesEpoch(*request->CurrentEpoch))' '!request->IoPolicy || !request->IoPolicy->TryFindCurrentSharedBuffer(argument->Value, request->ValidatedDomainTag, request->CurrentEpoch, buffer) || argument->Length == 0 || !buffer || argument->Length > buffer->Length || !request->CurrentEpoch || !argument->Grant.MatchesEpoch(*request->CurrentEpoch)'
Replace-Once $admission 'if (!request->Owner.value()->Epoch->Equals(contract->getOwnerEpoch()) || !request->Owner.value()->Epoch->Equals(request->CurrentEpoch))' 'if (!request->Owner.value()->Epoch || !request->CurrentEpoch || !request->Owner.value()->Epoch->Equals(contract->getOwnerEpoch()) || !request->Owner.value()->Epoch->Equals(request->CurrentEpoch))'

$publication = 'CloseToHSL\\Core\Runtime\Domains\SecureCompute\Publication\SecureCompletionRetirePublicationAuthorityPolicy.cpp'
Replace-Once $publication 'if (!owner || !owner.value()->Materialized || !owner.value()->HasIdentity)' 'if (!owner || !owner.value() || !owner.value()->Materialized || !owner.value()->HasIdentity)'
$ownerAdmission = 'CloseToHSL\\Core\Runtime\Domains\SecureCompute\Policies\Backend\YAKSys_Hybrid_CPU.Core.SecureBackendOwnerAdmissionPolicy.cpp'
Replace-Once $ownerAdmission 'if (!owner || !owner.value()->Materialized || !owner.value()->HasIdentity)' 'if (!owner || !owner.value() || !owner.value()->Materialized || !owner.value()->HasIdentity)'

$epochPath = Join-Path $root 'CloseToHSL\\Core\Runtime\Domains\SecureCompute\Authority\SecureRevocationEpoch.h'
$epochText = [System.IO.File]::ReadAllText($epochPath)
$brokenEpochTail = '}`r`n    bool Equals(const std::shared_ptr<SecureRevocationEpoch>& other) const noexcept { return other && Current == other->Current; }'
if ($epochText.Contains($brokenEpochTail)) {
    $epochText = $epochText.Replace($brokenEpochTail, '}')
    [System.IO.File]::WriteAllText($epochPath, $epochText, [System.Text.UTF8Encoding]::new($false))
}

$registryHeader = 'CloseToHSL\\Core\Runtime\Domains\SecureCompute\Descriptors\Hypercalls\SecureHypercallBackendOwnerAbiRegistry.h'
Replace-Once $registryHeader 'std::optional<std::shared_ptr<SecureRevocationEpoch>> &epoch = std::nullopt' 'std::optional<std::shared_ptr<SecureRevocationEpoch>> epoch = std::nullopt'
$registry = 'CloseToHSL\\Core\Runtime\Domains\SecureCompute\Descriptors\Hypercalls\SecureHypercallBackendOwnerAbiRegistry.cpp'
Replace-Once $registry 'return new(TransportOpcodeValue);' 'return std::make_shared<SecureHypercallTransportOpcode>(TransportOpcodeValue);'
Replace-Once $registry 'return new(DecodedLeafValue);' 'return std::make_shared<SecureHypercallDecodedLeaf>(DecodedLeafValue);'
Replace-Once $registry 'return new(ServiceIdValue);' 'return std::make_shared<SecureComputeServiceId>(ServiceIdValue);'
Replace-Once $registry 'return new(OwnerIdValue);' 'return std::make_shared<SecureBackendOwnerId>(OwnerIdValue);'
Replace-Once $registry 'return new(OwnerEpochValue);' 'return std::make_shared<SecureRevocationEpoch>(OwnerEpochValue);'
Replace-Once $registry 'return new(ContractVersionMajor, ContractVersionMinor);' 'return std::make_shared<SecureHypercallContractVersion>(ContractVersionMajor, ContractVersionMinor);'
Replace-Once $registry 'return new(SecureGrantHandleKind::HypercallPolicy, RequiredGrantLocalIdValue, RequiredGrantProvenanceHashValue, OwnerEpochValue);' 'return std::make_shared<SecureGrantHandle>(SecureGrantHandle{ SecureGrantHandleKind::HypercallPolicy, RequiredGrantLocalIdValue, RequiredGrantProvenanceHashValue, OwnerEpochValue });'
Replace-Once $registry 'return new(getDecodedLeaf(), getServiceId(), getOwnerId(), getOwnerEpoch(), getContractVersion(), getRequiredGrant(), SecureHypercallReplayPolicy::DenyReplay, SecureHypercallCancellationPolicy::DenyBeforeExecution, SecureHypercallRequestMigrationClass::NonMigratableInFlight, SecureHypercallResultMigrationClass::NoResultBeforeExecution);' 'return std::make_shared<SecureHypercallBackendContractDescriptor>(getDecodedLeaf(), getServiceId(), getOwnerId(), getOwnerEpoch(), getContractVersion(), getRequiredGrant(), SecureHypercallReplayPolicy::DenyReplay, SecureHypercallCancellationPolicy::DenyBeforeExecution, SecureHypercallRequestMigrationClass::NonMigratableInFlight, SecureHypercallResultMigrationClass::NoResultBeforeExecution);'
Replace-Once $registry 'std::shared_ptr<SecureBackendOwnerDescriptor> SecureHypercallBackendOwnerAbiRegistry::CreateOwnerDescriptor(SecureBackendOwnerSource source, std::optional<std::shared_ptr<SecureRevocationEpoch>> &epoch,' 'std::shared_ptr<SecureBackendOwnerDescriptor> SecureHypercallBackendOwnerAbiRegistry::CreateOwnerDescriptor(SecureBackendOwnerSource source, std::optional<std::shared_ptr<SecureRevocationEpoch>> epoch,'
Replace-Once $registry 'return new(OwnerIdValue, source, OwnerPolicyDigestValue, OwnerProofDigestValue, epoch ? epoch : getOwnerEpoch(), Materialized: true, grantProofValidated, evidenceProofValidated, completionFenceValidated, retireFenceValidated, negativeTestsPresent);' 'std::shared_ptr<SecureRevocationEpoch> ownerEpoch = epoch && epoch.value() ? epoch.value() : getOwnerEpoch(); return std::make_shared<SecureBackendOwnerDescriptor>(OwnerIdValue, source, OwnerPolicyDigestValue, OwnerProofDigestValue, ownerEpoch, true, grantProofValidated, evidenceProofValidated, completionFenceValidated, retireFenceValidated, negativeTestsPresent);'

$contractCpp = 'CloseToHSL\\Core\Runtime\Domains\SecureCompute\Descriptors\Hypercalls\SecureHypercallBackendContract.cpp'
$ctorStart = 'SecureHypercallBackendContractDescriptor::SecureHypercallBackendContractDescriptor('
$ctorEnd = 'std::shared_ptr<SecureHypercallBackendContractDescriptor> SecureHypercallBackendContractDescriptor::getUnresolved()'
$newCtor = @'
SecureHypercallBackendContractDescriptor::SecureHypercallBackendContractDescriptor(std::shared_ptr<SecureHypercallDecodedLeaf> decodedLeaf, std::shared_ptr<SecureComputeServiceId> serviceId, std::shared_ptr<SecureBackendOwnerId> ownerId, std::shared_ptr<SecureRevocationEpoch> ownerEpoch, std::shared_ptr<SecureHypercallContractVersion> version, std::shared_ptr<SecureGrantHandle> requiredGrant, SecureHypercallReplayPolicy replayPolicy, SecureHypercallCancellationPolicy cancellationPolicy, SecureHypercallRequestMigrationClass requestMigrationClass, SecureHypercallResultMigrationClass resultMigrationClass)
    : DecodedLeaf(std::move(decodedLeaf)), ServiceId(std::move(serviceId)), OwnerId(std::move(ownerId)),
      OwnerEpoch(ownerEpoch ? std::move(ownerEpoch) : SecureRevocationEpoch::Unmaterialized),
      Version(std::move(version)), RequiredGrant(std::move(requiredGrant)), ReplayPolicy(replayPolicy),
      CancellationPolicy(cancellationPolicy), RequestMigrationClass(requestMigrationClass), ResultMigrationClass(resultMigrationClass)
{
}
'@
Replace-Constructor $contractCpp $ctorStart $ctorEnd $newCtor

Write-Output 'Pool 6 dependent hypercall conversions applied. No build or compilation commands were run.'
