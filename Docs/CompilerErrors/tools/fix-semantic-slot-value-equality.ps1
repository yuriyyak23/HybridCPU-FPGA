$ErrorActionPreference='Stop'
$p='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\Frontend\Contracts\SemanticRecords.h'
$s=[IO.File]::ReadAllText($p)
$m='    std::shared_ptr<const Snapshot> RawSlot, InstructionPayload, SlotSideband, StaticBinding, ScalarLoadAddressPlan;'
if(-not $s.Contains($m) -or $s.Contains('bool operator==(const SemanticSlot&')){throw 'Unexpected SemanticSlot state'}
$add=@(
'    bool operator==(const SemanticSlot& other) const {',
'        const auto equalSnapshot = [](const auto& a, const auto& b) { return (!a && !b) || (a && b && *a == *b); };',
'        return SlotIndex == other.SlotIndex && IsOccupied == other.IsOccupied && Opcode == other.Opcode &&',
'            InstructionClass == other.InstructionClass && SerializationClass == other.SerializationClass &&',
'            Rd == other.Rd && Rs1 == other.Rs1 && Rs2 == other.Rs2 && Immediate == other.Immediate &&',
'            CsrAddress == other.CsrAddress && AcquireOrdering == other.AcquireOrdering && ReleaseOrdering == other.ReleaseOrdering &&',
'            equalSnapshot(RawSlot, other.RawSlot) && equalSnapshot(InstructionPayload, other.InstructionPayload) &&',
'            equalSnapshot(SlotSideband, other.SlotSideband) && equalSnapshot(StaticBinding, other.StaticBinding) &&',
'            equalSnapshot(ScalarLoadAddressPlan, other.ScalarLoadAddressPlan);',
'    }') -join "`r`n"
$s=$s.Replace($m,$m+"`r`n"+$add)
[IO.File]::WriteAllText($p,$s,[Text.UTF8Encoding]::new($false))
