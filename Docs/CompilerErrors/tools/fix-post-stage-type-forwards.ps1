$path='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Pipeline\Scheduling\PostStageBIssuedAttempt.h'
$s=[IO.File]::ReadAllText($path);$marker='namespace YAKSys_Hybrid_CPU::Core::Decoder { class ScheduledOperation; }'
if(-not $s.Contains($marker)){throw 'Forward marker missing'}
$s=$s.Replace($marker,$marker+"`r`n"+'namespace YAKSys_Hybrid_CPU::Core::Execution { class ExecutionRecord; }'+"`r`n"+'namespace YAKSys_Hybrid_CPU::Core::Registers::Retire { class ScalarRegisterWriteRetireEffect; }');[IO.File]::WriteAllText($path,$s,[Text.UTF8Encoding]::new($false))
