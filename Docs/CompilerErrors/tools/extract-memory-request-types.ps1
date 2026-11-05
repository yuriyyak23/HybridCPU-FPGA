$ErrorActionPreference='Stop'
$root=Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
$p=Join-Path $root 'CloseToHSL\\Memory\Subsystem\YAKSys_Hybrid_CPU.Memory.MemorySubsystem.h'; $s=Get-Content $p -Raw
function ExtractClass([string]$text,[string]$needle) {
 $a=$text.IndexOf($needle); if($a -lt 0){throw "Missing $needle"}; $brace=$text.IndexOf('{',$a); $depth=0
 for($i=$brace;$i -lt $text.Length;$i++){if($text[$i] -eq '{'){$depth++} elseif($text[$i] -eq '}'){$depth--;if($depth -eq 0){return @($a, ($i+2))}}}
 throw 'Unclosed class'
}
$t=ExtractClass $s "`t`tclass MemoryRequestToken :"; $token=$s.Substring($t[0],$t[1]-$t[0]); $s=$s.Substring(0,$t[0])+"`t`tusing MemoryRequestToken = YAKSys_Hybrid_CPU::Memory::MemoryRequestToken;"+$s.Substring($t[1])
$t=ExtractClass $s "`t`tclass BurstEventArgs :"; $event=$s.Substring($t[0],$t[1]-$t[0]); $s=$s.Substring(0,$t[0])+"`t`tusing BurstEventArgs = YAKSys_Hybrid_CPU::Memory::BurstEventArgs;"+$s.Substring($t[1])
$event = $event.Replace('class BurstEventArgs : public EventArgs', 'class BurstEventArgs : public std::enable_shared_from_this<BurstEventArgs>')
$event = [regex]::Replace($event, '(?s)\s*protected:\s*std::shared_ptr<BurstEventArgs> shared_from_this\(\)\s*\{.*?\}', '')
$header="#pragma once`r`n#include <memory>`r`n#include <string>`r`n#include <vector>`r`n#include `"../../../tangible_event.h`"`r`nnamespace YAKSys_Hybrid_CPU::Memory {`r`nclass PhysicalMemoryBankBinding;`r`n"+$event+"`r`n"+$token+"`r`n}`r`n"
Set-Content (Join-Path $root 'CloseToHSL\\Memory\Subsystem\MemoryRequestTypes.h') $header -Encoding UTF8 -NoNewline
$s=$s.Replace('#include "MemorySubsystem.Types.h"', "#include `"MemorySubsystem.Types.h`"`r`n#include `"MemoryRequestTypes.h`"")
Set-Content $p $s -Encoding UTF8 -NoNewline
$p=Join-Path $root 'CloseToHSL\\Core\Architecture\Common\YAKSys_Hybrid_CPU.Processor.h'; $s=Get-Content $p -Raw
$s=$s.Replace('#include "../../../Memory/Subsystem/YAKSys_Hybrid_CPU.Memory.MemorySubsystem.h"','namespace YAKSys_Hybrid_CPU::Memory { class MemorySubsystem; class MemoryRequestToken; class BurstEventArgs; }').Replace('YAKSys_Hybrid_CPU::Memory::MemorySubsystem::MemoryRequestToken','YAKSys_Hybrid_CPU::Memory::MemoryRequestToken').Replace('std::shared_ptr < YAKSys_Hybrid_CPU::Memory->MemorySubsystem.BurstEventArgs >','std::shared_ptr<YAKSys_Hybrid_CPU::Memory::BurstEventArgs>')
Set-Content $p $s -Encoding UTF8 -NoNewline
$p=Join-Path $root 'CloseToHSL\\Core\Architecture\Common\YAKSys_Hybrid_CPU.Processor.cpp'; $s=Get-Content $p -Raw
if (!$s.Contains('#include "../../../Memory/Subsystem/YAKSys_Hybrid_CPU.Memory.MemorySubsystem.h"')) {$s=$s.Replace('#include "YAKSys_Hybrid_CPU.Processor.h"', "#include `"YAKSys_Hybrid_CPU.Processor.h`"`r`n#include `"../../../Memory/Subsystem/YAKSys_Hybrid_CPU.Memory.MemorySubsystem.h`"")}
Set-Content $p $s -Encoding UTF8 -NoNewline
