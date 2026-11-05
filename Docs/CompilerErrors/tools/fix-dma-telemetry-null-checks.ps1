$p = 'C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Execution\DmaStreamCompute\DmaStreamComputeTelemetry.cpp'
$s = [IO.File]::ReadAllText($p)
$s = $s.Replace('#include "DmaStreamComputeTelemetry.h"', "#include `"DmaStreamComputeTelemetry.h`"`r`n#include <stdexcept>")
$s = [regex]::Replace($s, 'ArgumentNullException::ThrowIfNull\((\w+)\);', { param($m) 'if (!' + $m.Groups[1].Value + ') throw std::invalid_argument("' + $m.Groups[1].Value + '");' })
[IO.File]::WriteAllText($p, $s)
