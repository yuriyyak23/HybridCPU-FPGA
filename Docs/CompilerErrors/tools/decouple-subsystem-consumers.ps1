$ErrorActionPreference='Stop'
$root=Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
$headers=@('CloseToHSL\\Core\Pipeline\Assist\InterCore\YAKSys_Hybrid_CPU.Core.MicroOpScheduler.h','CloseToHSL\\Core\Pipeline\MicroOps\Vector\VectorMicroOps.Memory.h','CloseToHSL\\Core\Execution\StreamEngine\Classification\YAKSys_Hybrid_CPU.Execution.StreamEngine.h','CloseToHSL\\Core\Execution\StreamEngine\BurstIO\StreamEngine.BurstIO.h')
foreach($rel in $headers){
 $p=Join-Path $root $rel; $s=Get-Content $p -Raw
 $s=$s.Replace('#include "../../../../Memory/Subsystem/YAKSys_Hybrid_CPU.Memory.MemorySubsystem.h"','namespace YAKSys_Hybrid_CPU::Memory { class MemorySubsystem; class MemoryRequestToken; }').Replace('YAKSys_Hybrid_CPU::Memory::MemorySubsystem::MemoryRequestToken','YAKSys_Hybrid_CPU::Memory::MemoryRequestToken')
 Set-Content $p $s -Encoding UTF8 -NoNewline
 $cpp=[System.IO.Path]::ChangeExtension($p,'.cpp'); if(Test-Path $cpp){$c=Get-Content $cpp -Raw; if(!$c.Contains('#include "../../../../Memory/Subsystem/YAKSys_Hybrid_CPU.Memory.MemorySubsystem.h"')){$h=[System.IO.Path]::GetFileName($p);$c=$c.Replace("#include `"$h`"", "#include `"$h`"`r`n#include `"../../../../Memory/Subsystem/YAKSys_Hybrid_CPU.Memory.MemorySubsystem.h`"")};Set-Content $cpp $c -Encoding UTF8 -NoNewline}
}
