$ErrorActionPreference='Stop'
$p='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\ISA\Instructions\NonVmx\Lanes00_03Vector\MatrixTile\MatrixTileReplayRollbackAbi.h'
$s=[IO.File]::ReadAllText($p)
$m='#include <memory>'
if(-not $s.Contains($m) -or $s.Contains('class MatrixTileReplayIdentity;')){throw 'Unexpected header state'}
$s=$s.Replace($m,$m+"`r`n"+'#include "tangible_exception_helper.h"')
$s=$s.Replace('using namespace YAKSys_Hybrid_CPU::Core::Pipeline::MicroOps;', '')
$m='enum class MatrixTileReplayRollbackLifecycle : unsigned char'
$add=@('class MatrixTileReplayIdentity;','class MatrixTileRollbackOutcome;','class MatrixTileRetireOutcome;','class MatrixTileExecutionCaptureRecord;','class MatrixTileTileImage;','class MatrixTileCapturedMemoryWrite;','class MatrixTileMaterializedInstruction;','class MatrixTileCaptureIdentity;','class MatrixTileInstructionIrProjection;','') -join "`r`n"
$s=$s.Replace($m,$add+$m)
$pattern='(?s)\s*protected:\s*std::shared_ptr<MatrixTileReplayRollbackValidationException> shared_from_this\(\)\s*\{\s*return std::static_pointer_cast<MatrixTileReplayRollbackValidationException>\(InvalidOperationException::shared_from_this\(\)\);\s*\}'
if(([regex]::Matches($s,$pattern)).Count -ne 1){throw 'Expected invalid shared_from_this method not found'}
$s=[regex]::Replace($s,$pattern,'')
[IO.File]::WriteAllText($p,$s,[Text.UTF8Encoding]::new($false))
