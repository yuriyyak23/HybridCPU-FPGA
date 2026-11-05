param([switch]$Apply)
$sourceRoot = Join-Path (Split-Path -Parent (Split-Path -Parent $PSScriptRoot)) '2'
$pattern = '\boverride\s+const(?=\s*[;{])'
$files = Get-ChildItem -LiteralPath $sourceRoot -Recurse -File | Where-Object { $_.Extension -in '.h','.hpp','.cpp' }
$count = 0
foreach ($file in $files) {
    $content = [IO.File]::ReadAllText($file.FullName)
    $matchesFound = [regex]::Matches($content, $pattern)
    if ($matchesFound.Count -eq 0) { continue }
    $count += $matchesFound.Count
    if ($Apply) {
        $updated = [regex]::Replace($content, $pattern, 'const override')
        [IO.File]::WriteAllText($file.FullName, $updated, [Text.UTF8Encoding]::new($false))
    }
    Write-Output "$($file.FullName): $($matchesFound.Count)"
}
Write-Output "Total: $count"