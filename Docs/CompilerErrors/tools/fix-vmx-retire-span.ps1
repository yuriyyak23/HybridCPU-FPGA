$root = 'C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Pipeline\MicroOps\Types'
foreach ($file in @('MicroOp.IO.h', 'MicroOp.IO.cpp')) {
    $path = Join-Path $root $file
    $text = [IO.File]::ReadAllText($path)
    $old = 'Span<RetireRecord> retireRecords'
    if (-not $text.Contains($old)) { throw "Expected span not found: $file" }
    $text = $text.Replace($old, 'std::span<RetireRecord> retireRecords')
    if ($file -eq 'MicroOp.IO.h') {
        $text = $text.Replace('#include <vector>', '#include <vector>' + "`r`n" + '#include <span>')
    }
    [IO.File]::WriteAllText($path, $text, [Text.UTF8Encoding]::new($false))
}
