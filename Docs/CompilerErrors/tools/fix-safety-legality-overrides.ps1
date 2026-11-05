$path='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Pipeline\Safety\YAKSys_Hybrid_CPU.Core.SafetyVerifier.h';$s=[IO.File]::ReadAllText($path)
foreach($name in @('ILegalityChecker_EvaluateInterCoreDomainGuard','ILegalityChecker_EvaluateSmtBoundaryGuard')){$s=[regex]::Replace($s,'('+[regex]::Escape($name)+'\([^\r\n]*\)) override;','$1;')}
$old='InterCoreDomainGuardDecision EvaluateInterCoreDomainGuard(std::shared_ptr<MicroOp> candidate, unsigned long long requestedDomainTag);';$new=$old.Replace(');',') override;');if(-not $s.Contains($old)){throw 'Inter-core declaration missing'};$s=$s.Replace($old,$new)
$old='LegalityDecision EvaluateSmtBoundaryGuard(PhaseCertificateTemplateKey4Way liveTemplateKey);';$new=$old.Replace(');',') override;');if(-not $s.Contains($old)){throw 'Boundary declaration missing'};$s=$s.Replace($old,$new)
[IO.File]::WriteAllText($path,$s,[Text.UTF8Encoding]::new($false))
