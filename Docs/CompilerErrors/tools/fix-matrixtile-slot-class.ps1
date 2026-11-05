$p='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\ISA\Instructions\NonVmx\Lanes00_03Vector\MatrixTile\MatrixTileResourceContour.cpp'
$s=[IO.File]::ReadAllText($p)
$pattern='(?s)(SlotClass MatrixTileResourceContour::ResolveSlotClass\(MatrixTileRuntimeResourceClass resourceClass\)\s*\{)\s*// C# TO C\+\+ CONVERTER TASK:.*?\r?\n\s*\}'
$replacement=@"
`$1
        switch (resourceClass)
        {
        case MatrixTileRuntimeResourceClass::MatrixTileMemory:
            return SlotClass::MatrixTileStreamClass;
        case MatrixTileRuntimeResourceClass::MatrixTileCompute:
            return SlotClass::AluClass;
        default:
            throw std::invalid_argument("A MatrixTile runtime resource class is required.");
        }
    }
"@
$t=[regex]::Replace($s,$pattern,$replacement)
if ($t -ne $s) { $s=$t } elseif (-not $s.Contains('throw std::invalid_argument("A MatrixTile runtime resource class is required.");')) { throw 'ResolveSlotClass pattern not found' }
if (-not $s.Contains('#include <stdexcept>')) { $s=$s.Replace('#include "MatrixTileResourceContour.h"',"#include `"MatrixTileResourceContour.h`"`r`n#include <stdexcept>") }
[IO.File]::WriteAllText($p,$s)
