$ErrorActionPreference='Stop'
$p='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Pipeline\MicroOps\Types\MicroOp.h'
$s=[IO.File]::ReadAllText($p)
$marker='        void SetHardPinnedPlacement(SlotClass requiredSlotClass, unsigned char pinnedLaneId)'
if (-not $s.Contains($marker) -or $s.Contains('virtual bool TryGetPrimaryWriteBackResult(')) { throw 'Unexpected MicroOp.h state' }
$fragment=@"
    public:
        virtual bool TryGetPrimaryWriteBackResult(unsigned long long& value)
        {
            value = 0;
            return false;
        }
        virtual void CapturePrimaryWriteBackResult(unsigned long long) {}
        virtual std::wstring GetDescription() = 0;
        virtual void RefreshWriteMetadata() {}
    protected:
"@
$s=$s.Replace($marker,$fragment+"`r`n"+$marker)
[IO.File]::WriteAllText($p,$s,[Text.UTF8Encoding]::new($false))
