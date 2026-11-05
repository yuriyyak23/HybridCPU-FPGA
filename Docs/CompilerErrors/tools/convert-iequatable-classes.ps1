$root=Join-Path (Split-Path -Parent (Split-Path -Parent $PSScriptRoot)) '2'
$pattern='(?m)^(?<indent>\s*)class\s+(?<name>\w+)(?<final>\s+final)?\s*:\s*public\s+IEquatable<(?<arg>std::shared_ptr<\w+>|\w+)>\s*$'
$totalClasses=0; $totalMethods=0
foreach($file in (Get-ChildItem -LiteralPath $root -Recurse -Filter '*.h' -File)) {
 $text=[IO.File]::ReadAllText($file.FullName); $matchesFound=[regex]::Matches($text,$pattern); if(!$matchesFound.Count){continue}
 foreach($m in $matchesFound){
  $name=$m.Groups['name'].Value; $arg=$m.Groups['arg'].Value
  if($arg -ne $name -and $arg -ne "std::shared_ptr<$name>"){throw "Unexpected equality argument $name / $arg"}
  $methodPattern='(?m)(\bbool\s+Equals\s*\(\s*'+[regex]::Escape($arg)+'\s+\w+\s*\)\s*(?:const\s+)?)override\s*;'
  $methodCount=[regex]::Matches($text,$methodPattern).Count
  if($methodCount -ne 1){throw "Expected one Equals override for $name in $($file.FullName), got $methodCount"}
  $text=[regex]::Replace($text,$methodPattern,'$1;'); $totalMethods++
 }
 $text=[regex]::Replace($text,$pattern,'${indent}class ${name}${final}')
 [IO.File]::WriteAllText($file.FullName,$text,[Text.UTF8Encoding]::new($false));$totalClasses+=$matchesFound.Count
 Write-Output "$($file.FullName): $($matchesFound.Count)"
}
Write-Output "Removed interfaces: $totalClasses; fixed Equals declarations: $totalMethods"