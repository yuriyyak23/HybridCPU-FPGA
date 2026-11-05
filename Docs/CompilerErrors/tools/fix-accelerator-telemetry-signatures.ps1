$dir = 'C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Execution\ExternalAccelerators\Telemetry'
foreach ($name in @('AcceleratorTelemetry.h', 'AcceleratorTelemetry.cpp')) {
    $p = Join-Path $dir $name
    $s = [IO.File]::ReadAllText($p)
    $s = $s.Replace('std::optional<AcceleratorTokenState> &fromState', 'std::optional<AcceleratorTokenState> fromState')
    $s = $s.Replace('std::optional<AcceleratorTokenState> &toState', 'std::optional<AcceleratorTokenState> toState')
    $s = [regex]::Replace($s, '(RecordSubmit\([^\r\n]*?)std::optional<std::shared_ptr<AcceleratorGuardDecision>> &guardDecision', '$1std::optional<std::shared_ptr<AcceleratorGuardDecision>> guardDecision')
    if ($name -eq 'AcceleratorTelemetry.h') {
        $marker = 'namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Telemetry'+"`r`n"+'{'
        if (!$s.Contains($marker)) { throw 'Telemetry namespace boundary not found' }
        $s = $s.Replace($marker, $marker + "`r`n`tclass AcceleratorEvidenceRecord;`r`n`tclass AcceleratorTelemetrySnapshot;")
    }
    [IO.File]::WriteAllText($p, $s)
}
