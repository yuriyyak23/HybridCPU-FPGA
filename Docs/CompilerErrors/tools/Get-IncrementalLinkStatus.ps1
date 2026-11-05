[CmdletBinding()]
param(
    [int]$ProcessId = 60444,
    [int]$Tail = 30,
    [int]$RefreshSeconds = 0
)

$ErrorActionPreference = 'Stop'
$projectRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
$artifactRoot = $PSScriptRoot
$logPath = Join-Path $artifactRoot 'independent-incremental-link.log'
$exitPath = Join-Path $artifactRoot 'independent-incremental-link.exit.txt'
$exePath = Join-Path $artifactRoot 'independent-incremental-link.exe'

function Show-LinkStatus {
    $process = Get-Process -Id $ProcessId -ErrorAction SilentlyContinue
    $exitCode = if (Test-Path -LiteralPath $exitPath) {
        (Get-Content -LiteralPath $exitPath -Raw).Trim()
    } else { $null }

    if ($process) {
        $elapsed = (Get-Date) - $process.StartTime
        [pscustomobject]@{
            State = 'Running'
            PID = $process.Id
            Process = $process.ProcessName
            Elapsed = $elapsed.ToString('hh\:mm\:ss')
            CPUSeconds = [math]::Round($process.CPU, 1)
            ExitCode = $null
        } | Format-List
    } elseif ($null -ne $exitCode) {
        $state = if ($exitCode -eq '0') { 'Completed successfully' } else { "Failed (exit $exitCode)" }
        [pscustomobject]@{
            State = $state
            PID = $ProcessId
            Process = 'icx-cl'
            Elapsed = $null
            CPUSeconds = $null
            ExitCode = $exitCode
        } | Format-List
    } else {
        "State       : Process not found; no exit-code file yet (it may not have started or may have terminated unexpectedly)."
        "PID         : $ProcessId"
    }

    if (Test-Path -LiteralPath $logPath) {
        $log = Get-Item -LiteralPath $logPath
        "Log         : $($log.FullName) ($($log.Length) bytes; updated $($log.LastWriteTime))"
        if ($log.Length -gt 0) {
            'Recent linker output:'
            Get-Content -LiteralPath $logPath -Tail $Tail
        } else {
            'Recent linker output: (log is currently empty)'
        }
    }

    if (Test-Path -LiteralPath $exePath) {
        $output = Get-Item -LiteralPath $exePath
        "Output      : $($output.FullName) ($($output.Length) bytes; updated $($output.LastWriteTime))"
    }
}

if ($RefreshSeconds -gt 0) {
    do {
        Clear-Host
        "HybridCPU_FPGA incremental-link status — $(Get-Date -Format 'yyyy-MM-dd HH:mm:ss')"
        Show-LinkStatus
        Start-Sleep -Seconds $RefreshSeconds
    } while (Get-Process -Id $ProcessId -ErrorAction SilentlyContinue)
    Clear-Host
}

Show-LinkStatus
