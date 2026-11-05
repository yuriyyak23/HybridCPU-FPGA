$dir='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Runtime\Completion\Records'
foreach($file in @('SecureComputeNoEffectCompletionOwner.h','SecureComputeRegisterLocalCompletionOwner.h')) {
 $path=Join-Path $dir $file;$s=[IO.File]::ReadAllText($path);$marker="namespace YAKSys_Hybrid_CPU::Core`r`n{";if(-not $s.Contains($marker)){throw "Missing marker $file"}
 $s=$s.Replace($marker,$marker+"`r`n`tclass SecureComputePreparedMachineAdmission;`r`n`tclass SecureComputeMachineCertificate;`r`n`tclass SafetyVerifier;");[IO.File]::WriteAllText($path,$s,[Text.UTF8Encoding]::new($false))
}
