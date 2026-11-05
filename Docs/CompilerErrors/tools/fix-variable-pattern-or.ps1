param([switch]$Apply)
$root=Join-Path (Split-Path -Parent (Split-Path -Parent $PSScriptRoot)) '2'
$pattern='(?<![\w.>])(?<var>[A-Za-z_]\w*)\s*==\s*(?<first>[A-Za-z_]\w*(?:::\w+)+)(?<tail>(?:[ \t]+or[ \t]+[A-Za-z_]\w*(?:::\w+)+)+)'
$count=0
foreach($file in (Get-ChildItem -LiteralPath $root -Recurse -File | Where-Object {$_.Extension -in '.cpp','.h','.hpp'})) {
 $text=[IO.File]::ReadAllText($file.FullName); $lines=[regex]::Split($text,'(?<=\n)'); $changed=$false
 for($i=0;$i -lt $lines.Length;$i++) {
  $line=$lines[$i]; if($line -match '^\s*//' -or $line.Contains('"') -or $line.Contains('//') -or $line -match '\bis\b|\band_Keyword\b') { continue }
  $hits=[regex]::Matches($line,$pattern); if($hits.Count -eq 0){continue}; $count+=$hits.Count
  if($Apply) { $lines[$i]=[regex]::Replace($line,$pattern,{param($m) $name=$m.Groups['var'].Value; $items=@($m.Groups['first'].Value)+@([regex]::Matches($m.Groups['tail'].Value,'[A-Za-z_]\w*(?:::\w+)+') | ForEach-Object {$_.Value}); '(' + (($items | ForEach-Object {$name+' == '+$_}) -join ' || ') + ')' }); $changed=$true }
  Write-Output "$($file.FullName): $($hits.Count)"
 }
 if($Apply -and $changed) { [IO.File]::WriteAllText($file.FullName,($lines -join ''),[Text.UTF8Encoding]::new($false)) }
}
Write-Output "Total patterns: $count"