$dir = 'C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\ISA\Instructions\NonVmx\Lanes00_03Vector\MatrixTile'
$h = Join-Path $dir 'MatrixTileIrProjectionAndMaterializer.h'
$cpp = Join-Path $dir 'MatrixTileIrProjectionAndMaterializer.cpp'
$s = [IO.File]::ReadAllText($h)
$s = $s.Replace('#include "../../../../../Architecture/Common/YAKSys_Hybrid_CPU.Processor.h"', '#include "../../../../../Architecture/Common/InstructionOpcode.h"')
$s = $s.Replace('#include "../../../../../Pipeline/MicroOps/MatrixTile/MatrixTileMicroOps.h"', '')
$s = $s.Replace('namespace YAKSys_Hybrid_CPU::Core::Pipeline::MicroOps { struct VectorInstructionPayload; }', "namespace YAKSys_Hybrid_CPU::Core { class MatrixTileMicroOp; namespace Decoder { class CanonicalDecodedInstruction; class DecodedInstruction; } }`r`nnamespace YAKSys_Hybrid_CPU::Core::Pipeline::MicroOps { struct VectorInstructionPayload; class InstructionIR; }")
[IO.File]::WriteAllText($h, $s)
$s = [IO.File]::ReadAllText($cpp)
$s = $s.Replace('#include "MatrixTileIrProjectionAndMaterializer.h"', "#include `"MatrixTileIrProjectionAndMaterializer.h`"`r`n#include `"../../../../../Pipeline/MicroOps/MatrixTile/MatrixTileMicroOps.h`"")
[IO.File]::WriteAllText($cpp, $s)
