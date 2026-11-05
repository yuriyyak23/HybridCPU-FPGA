$path=Join-Path (Split-Path -Parent (Split-Path -Parent $PSScriptRoot)) 'CloseToHSL\\Core\Contracts\CompilerContract.h'
$text=[IO.File]::ReadAllText($path)
$pattern='(?m)^([\t ]*)static inline const CompilerTypedSlotPolicy (\w+) = (CompilerTypedSlotPolicy\([^;]+\));'
$found=[regex]::Matches($text,$pattern)
if($found.Count -eq 0){ Write-Output 'Already fixed'; exit 0 }
if($found.Count -ne 3){throw "Expected 3 policies, found $($found.Count)"}
$defs=foreach($m in $found){ 'inline const CompilerTypedSlotPolicy CompilerTypedSlotPolicy::'+$m.Groups[2].Value+' = '+$m.Groups[3].Value+';' }
$text=[regex]::Replace($text,$pattern,{param($m) $m.Groups[1].Value+'static const CompilerTypedSlotPolicy '+$m.Groups[2].Value+';'})
$end=[regex]::Match($text,'\}\s*\z'); if(-not $end.Success){throw 'Namespace end missing'}
$text=$text.Insert($end.Index,([string]::Join("`r`n",$defs)+"`r`n"))
[IO.File]::WriteAllText($path,$text,[Text.UTF8Encoding]::new($false)); Write-Output 'Fixed three policies'