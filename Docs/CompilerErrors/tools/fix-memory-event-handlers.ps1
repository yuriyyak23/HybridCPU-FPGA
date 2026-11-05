$root='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Memory'
foreach($entry in @(@('DMA\DMAController.h','TransferCompletedEventArgs'),@('Subsystem\YAKSys_Hybrid_CPU.Memory.MemorySubsystem.h','BurstEventArgs'))) {
$p=Join-Path $root $entry[0]; $s=[IO.File]::ReadAllText($p)
$s=$s.Replace(('EventHandler<'+$entry[1]+'>'),('std::function<void(void*, std::shared_ptr<'+$entry[1]+'>)>'))
if(-not $s.Contains('#include <functional>')) { $s=$s.Replace('#pragma once',"#pragma once`r`n#include <functional>") }
[IO.File]::WriteAllText($p,$s)
}
