$ErrorActionPreference='Stop'
$p='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Pipeline\MicroOps\Types\MicroOp.h'
$s=[IO.File]::ReadAllText($p)
$marker='        int getQueueIndex() const { return QueueIndex; }'
if (-not $s.Contains($marker) -or $s.Contains('SafetyMask128 getSafetyMask() const { return SafetyMask; }')) { throw 'Unexpected MicroOp.h state' }
$addition=@"
        virtual SafetyMask128 getSafetyMask() const { return SafetyMask; }
        virtual void setSafetyMask(SafetyMask128 value) { SafetyMask = value; }
        ArchLegalityMask getArchLegality() const { return ArchLegality; }
        BackendHazardMask getBackendHazard() const { return BackendHazard; }
        ResourceBitset getResourceMask() const { return ResourceMask; }
        void setResourceMask(ResourceBitset value) { ResourceMask = value; }
        ResourceBitset getOriginalResourceMask() const { return OriginalResourceMask; }
        void setOriginalResourceMask(ResourceBitset value) { OriginalResourceMask = value; }
"@
$s=$s.Replace($marker,$addition+"`r`n"+$marker)
[IO.File]::WriteAllText($p,$s,[Text.UTF8Encoding]::new($false))
