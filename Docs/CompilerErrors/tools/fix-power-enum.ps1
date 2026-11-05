$path=Join-Path (Split-Path -Parent (Split-Path -Parent $PSScriptRoot)) 'CloseToHSL\\Core\Architecture\Common\YAKSys_Hybrid_CPU.Processor.h'
$text=[IO.File]::ReadAllText($path)
$old='enum class CorePowerState : int'; $new='enum class CorePowerState : unsigned int'
if($text.Contains($old)){ $text=$text.Replace($old,$new); [IO.File]::WriteAllText($path,$text,[Text.UTF8Encoding]::new($false)); Write-Output 'Fixed CorePowerState' } elseif($text.Contains($new)){Write-Output 'Already fixed'} else{throw 'Enum declaration missing'}