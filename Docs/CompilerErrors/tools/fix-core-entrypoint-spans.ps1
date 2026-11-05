$root='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Architecture\Common\'
$h=Join-Path $root 'YAKSys_Hybrid_CPU.Processor.h'; $c=Join-Path $root 'YAKSys_Hybrid_CPU.Processor.cpp'
$s=[IO.File]::ReadAllText($h)
$pattern='(?ms)^\t\tclass EntryPoint\r?\n\t\t\{.*?^\t\t\};'
$m=[regex]::Match($s,$pattern); if(!$m.Success -or [regex]::Matches($s,$pattern).Count -ne 1){throw 'EntryPoint definition not found uniquely'}
$block=$m.Value
$block=$block.Replace('class EntryPoint'+"`r`n`t`t"+'{'+"`r`n`t`t"+'private:', 'class EntryPoint'+"`r`n`t`t"+'{'+"`r`n`t`t"+'public:'+"`r`n`t`t`t"+'enum class EntryPointType;'+"`r`n`t`t"+'private:')
$s=$s.Remove($m.Index,$m.Length)
$index=$s.IndexOf("`t`tclass CPU_Core final : public std::enable_shared_from_this<CPU_Core>")
if($index -lt 0){throw 'CPU_Core declaration not found'}
$s=$s.Insert($index,$block+"`r`n`r`n`tpublic:`r`n")
$s=$s.Replace('namespace YAKSys_Hybrid_CPU { class CPU_Core; }','namespace YAKSys_Hybrid_CPU { class CPU_Core; class ShadowFrontendCpuSnapshot; }')
$s=[regex]::Replace($s,'(?<![\w:])ReadOnlySpan<([^<>]+)>','std::span<const $1>')
$s=[regex]::Replace($s,'(?<![\w:])Span<([^<>]+)>','std::span<$1>')
[IO.File]::WriteAllText($h,$s)
$s=[IO.File]::ReadAllText($c); $s=[regex]::Replace($s,'(?<![\w:])ReadOnlySpan<([^<>]+)>','std::span<const $1>'); $s=[regex]::Replace($s,'(?<![\w:])Span<([^<>]+)>','std::span<$1>'); [IO.File]::WriteAllText($c,$s)