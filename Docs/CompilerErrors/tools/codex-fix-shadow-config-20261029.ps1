$ErrorActionPreference='Stop'
$p='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Architecture\Common\YAKSys_Hybrid_CPU.Processor.cpp'
$s=[IO.File]::ReadAllText($p)
$old='if (!Enum::IsDefined(mode))'
if (-not $s.Contains($old)) { throw 'Expected mode check missing' }
$s=$s.Replace($old,'if (mode != ShadowFrontendMode::Off && mode != ShadowFrontendMode::Observe && mode != ShadowFrontendMode::ColdWarm)')
$s=$s.Replace('throw ArgumentOutOfRangeException(L"mode");','throw std::out_of_range("mode");')
$s=$s.Replace('throw InvalidOperationException(L"Configure ShadowFrontend before starting execution or after disabling the pipeline.");','throw std::logic_error("Configure ShadowFrontend before starting execution or after disabling the pipeline.");')
[IO.File]::WriteAllText($p,$s,[Text.UTF8Encoding]::new($false))
