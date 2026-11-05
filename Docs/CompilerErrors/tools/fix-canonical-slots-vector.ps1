$root='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Decoder'
foreach($name in @('CanonicalDecodedContracts.h','CanonicalDecodedContracts.cpp')) {
 $p=Join-Path $root $name; $s=[IO.File]::ReadAllText($p)
 $s=$s.Replace('ImmutableArray<std::shared_ptr<CanonicalDecodedInstruction>>::Builder slots = ImmutableArray::CreateBuilder<std::shared_ptr<CanonicalDecodedInstruction>>(BundleMetadata::BundleSlotCount);','std::vector<std::shared_ptr<CanonicalDecodedInstruction>> slots; slots.reserve(BundleMetadata::BundleSlotCount);')
 $s=$s.Replace('std::shared_ptr<ImmutableArray<CanonicalDecodedInstruction>::Builder> slots','const std::vector<std::shared_ptr<CanonicalDecodedInstruction>> &slots')
 $s=$s.Replace('ImmutableArray<std::shared_ptr<CanonicalDecodedInstruction>>','std::vector<std::shared_ptr<CanonicalDecodedInstruction>>')
 $s=$s.Replace('slots.Add(', 'slots.push_back(').Replace('slots.MoveToImmutable()', 'std::move(slots)').Replace('slots->Count','slots.size()')
 $s=$s.Replace('getSlots().Length', 'getSlots().size()')
 [IO.File]::WriteAllText($p,$s)
}
$p=Join-Path $root 'CanonicalDecodedContracts.cpp'
$s=[IO.File]::ReadAllText($p)
$pattern='(?s)(CanonicalBundle::CanonicalBundle\(unsigned long long bundleAddress, unsigned long long bundleSerial, std::vector<std::shared_ptr<CanonicalDecodedInstruction>> slots, std::shared_ptr<CanonicalPayloadSnapshot> bundleSideband, SemanticInstructionKey semanticKey\))\s*\{\s*BundleAddress = bundleAddress;\s*BundleSerial = bundleSerial;\s*Slots = slots;\s*BundleSideband = bundleSideband;\s*SemanticKey = semanticKey;\s*\}'
$s=[regex]::Replace($s,$pattern,'$1 : BundleAddress(bundleAddress), BundleSerial(bundleSerial), Slots(std::move(slots)), BundleSideband(std::move(bundleSideband)), SemanticKey(std::move(semanticKey)) {}')
[IO.File]::WriteAllText($p,$s)
