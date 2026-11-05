$p='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\errors.txt'
$backup=Join-Path (Split-Path $p) ('CompilerErrors\errors-before-dedup-' + (Get-Date -Format 'yyyyMMdd-HHmmss-fff') + '.txt')
Copy-Item -LiteralPath $p -Destination $backup
$lines=[IO.File]::ReadAllLines($p)
$seen=[Collections.Generic.HashSet[string]]::new([StringComparer]::Ordinal)
$result=[Collections.Generic.List[string]]::new()
foreach($line in $lines) { if($seen.Add($line)) { $result.Add($line) } }
[IO.File]::WriteAllLines($p,$result,[Text.UTF8Encoding]::new($true))
Write-Output "Original=$($lines.Count); Unique=$($result.Count); Removed=$($lines.Count-$result.Count); Backup=$backup"
