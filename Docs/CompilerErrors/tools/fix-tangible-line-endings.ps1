$root=Join-Path (Split-Path -Parent (Split-Path -Parent $PSScriptRoot)) '2'
Get-ChildItem -LiteralPath $root -Filter 'tangible_*.h' -File | ForEach-Object {
  $bytes=[IO.File]::ReadAllBytes($_.FullName)
  $stream=[IO.MemoryStream]::new(); $count=0
  for($i=0;$i -lt $bytes.Length;$i++) {
    $stream.WriteByte($bytes[$i])
    if($bytes[$i] -eq 13 -and ($i+1 -eq $bytes.Length -or $bytes[$i+1] -ne 10)) { $stream.WriteByte(10); $count++ }
  }
  if($count -gt 0) { [IO.File]::WriteAllBytes($_.FullName,$stream.ToArray()); Write-Output "$($_.Name): $count line endings" }
  $stream.Dispose()
}