$dir = 'C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Execution\DmaStreamCompute'
$h = [IO.File]::ReadAllText((Join-Path $dir 'DmaStreamComputeTelemetry.h'))
$p = Join-Path $dir 'DmaStreamComputeTelemetry.cpp'
$s = [IO.File]::ReadAllText($p)
$start = $s.IndexOf("`tstd::shared_ptr<DmaStreamComputeTelemetrySnapshot> DmaStreamComputeTelemetryCounters::Snapshot()")
$end = $s.IndexOf("`tvoid DmaStreamComputeTelemetryCounters::RecordDescriptorParseAttempt()", $start)
if ($start -lt 0 -or $end -lt 0) { throw 'Snapshot boundary not found' }
$body = $s.Substring($start, $end - $start)
$match = [regex]::Match($body, 'return AnonymousType\(([^;]+)\);')
if (!$match.Success) { throw 'Snapshot argument list not found' }
$args = $match.Groups[1].Value -split ',\s*'
$recordStart = $h.IndexOf('struct DmaStreamComputeTelemetrySnapshot final {')
$recordEnd = $h.IndexOf('struct DmaStreamComputeReplayEvidenceComparison;', $recordStart)
if ($recordStart -lt 0 -or $recordEnd -lt 0) { throw 'Telemetry snapshot record not found' }
$block = $h.Substring($recordStart, $recordEnd - $recordStart)
$names = @([regex]::Matches($block, '(?m)^\s*[^\r\n]+\s+(\w+)\{\};') | ForEach-Object { $_.Groups[1].Value })
if ($args.Count -ne $names.Count) { throw "Field count mismatch: $($args.Count), $($names.Count)" }
$lines = for ($i=0; $i -lt $names.Count; $i++) { "`t`tresult->$($names[$i]) = $($args[$i]);" }
$newBody = "`tstd::shared_ptr<DmaStreamComputeTelemetrySnapshot> DmaStreamComputeTelemetryCounters::Snapshot()`r`n`t{`r`n`t`tauto result = std::make_shared<DmaStreamComputeTelemetrySnapshot>();`r`n" + ($lines -join "`r`n") + "`r`n`t`treturn result;`r`n`t}`r`n`r`n"
$s = $s.Substring(0, $start) + $newBody + $s.Substring($end)
[IO.File]::WriteAllText($p, $s)
