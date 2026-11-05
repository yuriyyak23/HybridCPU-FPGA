$root=Join-Path (Split-Path -Parent (Split-Path -Parent $PSScriptRoot)) 'CloseToHSL\\Core\Decoder'
$h=Join-Path $root 'CanonicalDecodedContracts.h'; $cpp=Join-Path $root 'CanonicalDecodedContracts.cpp'
$ht=[IO.File]::ReadAllText($h); $ct=[IO.File]::ReadAllText($cpp)
if (!$ht.Contains('#include "CanonicalDecodeContext.h"')) {
 $ht=$ht.Replace('#pragma once', '#pragma once' + "`r`n" + '#include "CanonicalDecodeContext.h"')
 $ht=$ht.Replace('enum class DecodeFailureCode : int','enum class DecodeFailureCode : unsigned short')
}
if (!$ct.Contains('CanonicalDecodeContext::Unbound =')) {
 $marker="namespace YAKSys_Hybrid_CPU::Core::Decoder`r`n{"
 $definition=@'

    const std::shared_ptr<CanonicalDecodeContext> CanonicalDecodeContext::Unbound = [] {
        auto context = std::make_shared<CanonicalDecodeContext>();
        context->ManifestVersion = GeneratedIsaCatalog::ManifestVersion;
        context->ManifestHash = GeneratedIsaCatalog::ManifestSha256;
        context->ExtensionConfigurationFingerprint = L"unbound";
        context->DecoderEpoch = L"rf04-shadow";
        context->DecoderVersion = L"VliwDecoderV4-legacy";
        context->PrivilegeContext = L"unbound";
        context->DomainIdentity = L"unbound";
        context->AddressSpaceIdentity = L"unbound";
        context->VectorConfigurationFingerprint = L"unbound";
        return context;
    }();
'@
 if (!$ct.Contains($marker)) { $marker="namespace YAKSys_Hybrid_CPU::Core::Decoder`n{" }
 if (!$ct.Contains($marker)) {throw 'Namespace not found'}
 $ct=$ct.Replace($marker,$marker+$definition)
}
[IO.File]::WriteAllText($h,$ht,[Text.UTF8Encoding]::new($false)); [IO.File]::WriteAllText($cpp,$ct,[Text.UTF8Encoding]::new($false)); Write-Output 'Restored context declaration and Unbound definition'
