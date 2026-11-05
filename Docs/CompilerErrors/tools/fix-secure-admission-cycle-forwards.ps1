$path='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Pipeline\Safety\SafetyVerifier.SecureComputeAdmission.h'
$s=[IO.File]::ReadAllText($path);$marker="namespace YAKSys_Hybrid_CPU::Core`r`n{";if(-not $s.Contains($marker)){throw 'Namespace missing'}
$s=$s.Replace($marker,$marker+"`r`n`tclass SecureComputeMachinePermitConsumption;`r`n`tclass SecureComputeNoEffectRetirement;`r`n`tclass SecureComputeRegisterLocalRetirement;")
[IO.File]::WriteAllText($path,$s,[Text.UTF8Encoding]::new($false))
