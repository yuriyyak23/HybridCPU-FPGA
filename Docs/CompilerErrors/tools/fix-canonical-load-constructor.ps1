$p='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Decoder\CanonicalDecodedContracts.cpp'
$s=[IO.File]::ReadAllText($p)
$pattern='(?s)CanonicalScalarLoadAddressPlan::CanonicalScalarLoadAddressPlan\(std::shared_ptr<GeneratedStaticBinding> generatedBinding, unsigned char destinationRegisterId, unsigned char baseRegisterId, long long signedDisplacement, unsigned char accessSize\)\s*\{\s*GeneratedBinding = generatedBinding;\s*DestinationRegisterId = destinationRegisterId;\s*BaseRegisterId = baseRegisterId;\s*SignedDisplacement = signedDisplacement;\s*AccessSize = accessSize;\s*\}'
$replacement=@"
CanonicalScalarLoadAddressPlan::CanonicalScalarLoadAddressPlan(std::shared_ptr<GeneratedStaticBinding> generatedBinding, unsigned char destinationRegisterId, unsigned char baseRegisterId, long long signedDisplacement, unsigned char accessSize)
        : GeneratedBinding(std::move(generatedBinding)), DestinationRegisterId(destinationRegisterId),
          BaseRegisterId(baseRegisterId), SignedDisplacement(signedDisplacement), AccessSize(accessSize)
    {
    }
"@
$t=[regex]::Replace($s,$pattern,$replacement)
if ($t -eq $s -and -not $s.Contains('GeneratedBinding(std::move(generatedBinding))')) { throw 'Constructor pattern missing' }
$s=$t
$s=$s.Replace('std::optional<std::shared_ptr<GeneratedStaticBinding>> binding = canonical->StaticBinding;', 'std::shared_ptr<GeneratedStaticBinding> binding = canonical->StaticBinding;')
$s=$s.Replace('binding.Opcode != canonical->Opcode', 'binding->Opcode != canonical->Opcode')
$s=$s.Replace('plan = std::make_shared<CanonicalScalarLoadAddressPlan>(binding, canonical->Rd, canonical->Rs1, canonical->Immediate, accessSize);','plan = std::shared_ptr<CanonicalScalarLoadAddressPlan>(new CanonicalScalarLoadAddressPlan(binding, canonical->Rd, canonical->Rs1, canonical->Immediate, accessSize));')
if (-not $s.Contains('#include <utility>')) { $s=$s.Replace('#include "CanonicalDecodedContracts.h"',"#include `"CanonicalDecodedContracts.h`"`r`n#include <utility>") }
[IO.File]::WriteAllText($p,$s)
