$ErrorActionPreference='Stop'
$p='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Pipeline\MicroOps\Types\MicroOp.h'
$s=[IO.File]::ReadAllText($p)
$m='        void SetHardPinnedPlacement(SlotClass requiredSlotClass, unsigned char pinnedLaneId)'
if(-not $s.Contains($m) -or $s.Contains('static bool TryNormalizeFlatArchRegId(')){throw 'Unexpected MicroOp.h state'}
$addition=@(
'        static bool TryNormalizeFlatArchRegId(unsigned short rawRegId, int& archRegId)',
'        {',
'            ArchRegId regId;',
'            if (!ArchRegId::TryCreate(rawRegId, regId)) { archRegId = 0; return false; }',
'            archRegId = regId.getValue();',
'            return true;',
'        }',
'') -join "`r`n"
$s=$s.Replace($m,$addition+$m)
[IO.File]::WriteAllText($p,$s,[Text.UTF8Encoding]::new($false))
