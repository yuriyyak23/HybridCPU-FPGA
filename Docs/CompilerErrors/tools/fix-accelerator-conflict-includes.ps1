$p = 'C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Execution\ExternalAccelerators\Conflicts\ExternalAcceleratorConflictManager.cpp'
$s = [IO.File]::ReadAllText($p)
$s = $s.Replace('#include "ExternalAcceleratorConflictManager.h"', "#include `"ExternalAcceleratorConflictManager.h`"`r`n#include `"../Telemetry/AcceleratorTelemetry.h`"")
$s = $s.Replace('using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Commit;', '')
[IO.File]::WriteAllText($p, $s)
