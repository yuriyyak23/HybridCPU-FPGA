$ErrorActionPreference='Stop'
$p=Join-Path (Split-Path -Parent (Split-Path -Parent $PSScriptRoot)) 'Frontend\\Cache\SemanticBundleCache.cpp'
$s=Get-Content $p -Raw
$a=$s.IndexOf("`tlong long SemanticBundleCache::IdentityBytes("); $b=$s.IndexOf("`tstd::shared_ptr<RetainedGraphAccounting> SemanticBundleCache::RetainedGraph(",$a)
if($a -lt 0 -or $b -lt 0){throw 'IdentityBytes boundaries missing'}
$body=@"
    long long SemanticBundleCache::IdentityBytes(std::shared_ptr<SemanticIdentity> identity) {
        auto result = identity->getResult();
        long long size = result->getRaw()->getLength();
        auto sideband = result->getBundleSideband();
        if (sideband && *sideband) size += (*sideband)->Content->getLength();
        for (const auto& slot : result->getSlots()) {
            size += slot->RawSlot->Content->getLength() + slot->InstructionPayload->Content->getLength()
                  + slot->SlotSideband->Content->getLength() + slot->StaticBinding->Content->getLength()
                  + slot->ScalarLoadAddressPlan->Content->getLength();
        }
        return size;
    }

"@
$s=$s.Substring(0,$a)+$body+$s.Substring($b)
Set-Content $p $s -Encoding UTF8 -NoNewline
