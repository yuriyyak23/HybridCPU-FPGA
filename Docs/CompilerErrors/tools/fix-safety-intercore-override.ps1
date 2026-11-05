$path='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Pipeline\Safety\YAKSys_Hybrid_CPU.Core.SafetyVerifier.h';$s=[IO.File]::ReadAllText($path)
$pat='(?m)^(\s*LegalityDecision EvaluateInterCoreLegality\([^\r\n]*\));';if(-not [regex]::IsMatch($s,$pat)){throw 'Target method missing'};$s=[regex]::Replace($s,$pat,'$1 override;')
$pat='(?m)^(\s*LegalityDecision ILegalityChecker_EvaluateInterCoreLegality\([^\r\n]*\)) override;';if(-not [regex]::IsMatch($s,$pat)){throw 'Wrapper missing'};$s=[regex]::Replace($s,$pat,'$1;')
[IO.File]::WriteAllText($path,$s,[Text.UTF8Encoding]::new($false))
