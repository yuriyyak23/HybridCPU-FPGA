$ErrorActionPreference='Stop'
$path='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Pipeline\MicroOps\Types\MicroOp.h'
$text=[IO.File]::ReadAllText($path)
$text=$text.Replace('#include "../../../Architecture/Common/YAKSys_Hybrid_CPU.Processor.h"' + "`r`n",'')
$text=$text.Replace('namespace YAKSys_Hybrid_CPU { class Processor; }','namespace YAKSys_Hybrid_CPU { class Processor { public: class CPU_Core; }; }')
$text=$text.Replace('retireRecords[static_cast<size_t>(retireRecordCount++)] = record;','std::construct_at(retireRecords.data() + retireRecordCount++, record);')
[IO.File]::WriteAllText($path,$text,[Text.UTF8Encoding]::new($false))
