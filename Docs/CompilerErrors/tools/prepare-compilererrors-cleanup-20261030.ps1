[CmdletBinding(SupportsShouldProcess = $true, ConfirmImpact = 'Low')]
param()

$ErrorActionPreference = 'Stop'
$root = (Resolve-Path -LiteralPath $PSScriptRoot).Path.TrimEnd('\')
$rootPrefix = $root + '\'
$reviewRoot = Join-Path $root '_cleanup-review-20261030'
$reviewRootPrefix = $reviewRoot.TrimEnd('\') + '\'

# These recent files document the successful solution launch and the transfer.
# The latest full-build log is kept as the newest compile snapshot.
$keepRelativePaths = @(
    'ActivityLog-shortpath-20261030.xml'
    'transfer-dpcpp-compilererrors-20261030.log'
    'codex-full-build-20261030-104.log'
)
$keepSet = [System.Collections.Generic.HashSet[string]]::new([System.StringComparer]::OrdinalIgnoreCase)
foreach ($path in $keepRelativePaths) { [void]$keepSet.Add($path) }

$logExtensions = @('.log', '.xml', '.err')
$temporaryExtensions = @(
    '.obj', '.exe', '.pdb', '.ilk', '.dll', '.lastbuildstate',
    '.tlog', '.cache', '.tmp', '.up2date'
)
$scriptExtensions = @(
    '.ps1', '.psm1', '.psd1', '.cmd', '.bat', '.js', '.mjs', '.cjs',
    '.py', '.pyw', '.sh', '.vbs', '.wsf', '.rb', '.pl', '.lua', '.r'
)

$candidates = [System.Collections.Generic.List[object]]::new()
$files = Get-ChildItem -LiteralPath $root -File -Recurse -Force
foreach ($file in $files) {
    $fullPath = $file.FullName
    if (-not $fullPath.StartsWith($rootPrefix, [System.StringComparison]::OrdinalIgnoreCase)) { continue }
    if ($fullPath.StartsWith($reviewRootPrefix, [System.StringComparison]::OrdinalIgnoreCase)) { continue }
    if ($scriptExtensions -contains $file.Extension.ToLowerInvariant()) { continue }

    $relativePath = $fullPath.Substring($rootPrefix.Length)
    if ($keepSet.Contains($relativePath)) { continue }

    $category = $null
    if ($logExtensions -contains $file.Extension.ToLowerInvariant()) {
        $category = 'Logs'
    } elseif (($temporaryExtensions -contains $file.Extension.ToLowerInvariant()) -or ($file.Name -ieq 'unsuccessfulbuild')) {
        $category = 'TemporaryBuildArtifacts'
    }

    if ($category) {
        $candidates.Add([pscustomobject]@{
            Source = $fullPath
            RelativePath = $relativePath
            Category = $category
            Length = $file.Length
            LastWriteTime = $file.LastWriteTime
        })
    }
}

if ($candidates.Count -eq 0) {
    Write-Output "No cleanup candidates found under: $root"
    exit 0
}

$moved = 0
$skipped = 0
$failed = 0
$movedBytes = [long]0
$manifest = [System.Collections.Generic.List[string]]::new()
$manifest.Add("Cleanup review folder: $reviewRoot")
$manifest.Add("Started: $(Get-Date -Format o)")
$manifest.Add("Candidate count: $($candidates.Count)")
$manifest.Add('Moved files preserve their original relative paths beneath Logs or TemporaryBuildArtifacts.')
$manifest.Add('Scripts and the explicitly retained recent logs were excluded.')

foreach ($candidate in $candidates) {
    $destination = Join-Path (Join-Path $reviewRoot $candidate.Category) $candidate.RelativePath
    $destinationParent = Split-Path -Parent $destination

    try {
        if ([System.IO.File]::Exists($destination)) {
            $skipped++
            $manifest.Add("SKIPPED_EXISTS`t$($candidate.RelativePath)`t$destination")
            continue
        }

        if ($PSCmdlet.ShouldProcess($candidate.Source, "Move to $destination")) {
            [System.IO.Directory]::CreateDirectory($destinationParent) | Out-Null
            # File.Move without overwrite throws if the destination already exists.
            [System.IO.File]::Move($candidate.Source, $destination)
            $moved++
            $movedBytes += $candidate.Length
            $manifest.Add("MOVED`t$($candidate.Length)`t$($candidate.RelativePath)`t$destination")
        } else {
            $manifest.Add("WHATIF`t$($candidate.RelativePath)`t$destination")
        }
    } catch {
        $failed++
        $manifest.Add("FAILED`t$($candidate.RelativePath)`t$($_.Exception.Message)")
    }
}

$manifest.Add("Completed: $(Get-Date -Format o)")
$manifest.Add("Moved: $moved")
$manifest.Add("Skipped because review copy already exists: $skipped")
$manifest.Add("Failed: $failed")
$manifest.Add("Moved bytes: $movedBytes")

if (-not $WhatIfPreference) {
    [System.IO.Directory]::CreateDirectory($reviewRoot) | Out-Null
    $stamp = Get-Date -Format 'yyyyMMdd-HHmmss-fff'
    $manifestPath = Join-Path $reviewRoot "manifest-$stamp.txt"
    $manifestStream = [System.IO.File]::Open(
        $manifestPath,
        [System.IO.FileMode]::CreateNew,
        [System.IO.FileAccess]::Write,
        [System.IO.FileShare]::None
    )
    try {
        $writer = [System.IO.StreamWriter]::new($manifestStream, [System.Text.UTF8Encoding]::new($false))
        foreach ($line in $manifest) { $writer.WriteLine($line) }
        $writer.Dispose()
    } finally {
        if ($manifestStream) { $manifestStream.Dispose() }
    }
    Write-Output "Review folder: $reviewRoot"
    Write-Output "Manifest: $manifestPath"
}

Write-Output "Candidates: $($candidates.Count); moved: $moved; skipped: $skipped; failed: $failed; moved GB: $([math]::Round($movedBytes / 1GB, 3))"
if ($failed -gt 0) { exit 1 }
