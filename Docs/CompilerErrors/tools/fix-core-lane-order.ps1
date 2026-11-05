$p='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Architecture\Common\YAKSys_Hybrid_CPU.Processor.h'
$s=[IO.File]::ReadAllText($p)
foreach($pair in @(@('ScalarExecuteLaneState','ExecuteStage'),@('ScalarMemoryLaneState','MemoryStage'))){
$pattern='(?ms)^\t\t\tclass '+$pair[0]+'\r?\n\t\t\t\{.*?^\t\t\t\};'; $m=[regex]::Match($s,$pattern); if(!$m.Success -or [regex]::Matches($s,$pattern).Count -ne 1){throw "Cannot identify $($pair[0])"}; $block=$m.Value; $s=$s.Remove($m.Index,$m.Length); $marker="`t`t`tclass "+$pair[1]+"`r`n"; $i=$s.IndexOf($marker); if($i -lt 0){throw "Cannot identify $($pair[1])"}; $s=$s.Insert($i,$block+"`r`n`r`n`t`tpublic:`r`n")
}
[IO.File]::WriteAllText($p,$s)