$p='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Architecture\Common\YAKSys_Hybrid_CPU.Processor.h'
$s=[IO.File]::ReadAllText($p)
$s=$s.Replace('namespace YAKSys_Hybrid_CPU::Core { class VectorTransferMicroOp;', 'namespace YAKSys_Hybrid_CPU::Core::ControlFlow { struct RelocationEntry; }'+"`r`n"+'namespace YAKSys_Hybrid_CPU::Core { class LoadSegmentMicroOp; class VectorTransferMicroOp;')
$patterns=@('(?ms)^\t\t\tenum class PipelineStallKind : unsigned char\r?\n\t\t\t\{.*?^\t\t\t\};','(?ms)^\t\t\tclass PipelineControl\r?\n\t\t\t\{.*?^\t\t\t\};')
$blocks=@(); foreach($pattern in $patterns){$m=[regex]::Match($s,$pattern); if(!$m.Success -or [regex]::Matches($s,$pattern).Count -ne 1){throw 'Pipeline declaration not found uniquely'}; $blocks += $m.Value; $s=$s.Remove($m.Index,$m.Length)}
$marker='enum class VectorException : unsigned char;'; $idx=$s.IndexOf($marker); if($idx -lt 0){throw 'Core declaration marker missing'}; $idx += $marker.Length
$s=$s.Insert($idx,"`r`n`t`t`t"+'enum class PipelineStage;'+"`r`n`t`t`t"+'class ScalarExecuteLaneState;'+"`r`n`r`n"+($blocks -join "`r`n`r`n"))
[IO.File]::WriteAllText($p,$s)