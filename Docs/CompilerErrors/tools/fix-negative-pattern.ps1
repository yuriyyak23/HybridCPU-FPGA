param([switch]$Apply)
$root=Join-Path (Split-Path -Parent (Split-Path -Parent $PSScriptRoot)) '2'
$const='(?:\d+|(?:::)?[A-Za-z_]\w*(?:::\w+)+)'
$pattern='(?<![\w.>])(?<var>[A-Za-z_]\w*)\s+is\s+not\s*\(\s*(?<items>'+$const+'(?:\s+or\s+'+$const+')+)\s*\)'
$count=0
foreach($file in (Get-ChildItem -LiteralPath $root -Recurse -File | Where-Object {$_.Extension -in '.cpp','.h','.hpp'})) {
 $text=[IO.File]::ReadAllText($file.FullName); $lines=[regex]::Split($text,'(?<=\n)'); $changed=$false
 for($i=0;$i -lt $lines.Length;$i++) {
  $line=$lines[$i]; if($line -match '^\s*//' -or $line.Contains('"') -or $line.Contains('//')) { continue }
  $hits=[regex]::Matches($line,$pattern); if($hits.Count -eq 0){continue}; $count+=$hits.Count
  if($Apply) { $lines[$i]=[regex]::Replace($line,$pattern,{param($m) $name=$m.Groups['var'].Value; $items=[regex]::Split($m.Groups['items'].Value,'\s+or\s+'); '(' + (($items | ForEach-Object {$name+' != '+$_}) -join ' && ') + ')' }); $changed=$true }
  Write-Output "$($file.FullName): $($hits.Count)"
 }
 if($Apply -and $changed) { [IO.File]::WriteAllText($file.FullName,($lines -join ''),[Text.UTF8Encoding]::new($false)) }
}
Write-Output "Total patterns: $count"