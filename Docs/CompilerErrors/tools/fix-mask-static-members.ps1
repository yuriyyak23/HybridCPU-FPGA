$root=Join-Path (Split-Path -Parent (Split-Path -Parent $PSScriptRoot)) 'CloseToHSL\\Core\Pipeline'
$files=@('Safety\ResourceBitset.h','Certificates\SafetyMask128.h','Certificates\ArchLegalityMask.h')
foreach($rel in $files) {
  $path=Join-Path $root $rel; $text=[IO.File]::ReadAllText($path)
  $pattern='(?m)^(\s*)static inline const (ResourceBitset|SafetyMask128|ArchLegalityMask|BackendHazardMask) (Zero|All) = ([^;]+);'
  $matchesFound=[regex]::Matches($text,$pattern)
  if($matchesFound.Count -eq 0) { Write-Output "$rel already fixed"; continue }
  $definitions=foreach($m in $matchesFound){ 'inline const ' + $m.Groups[2].Value + ' ' + $m.Groups[2].Value + '::' + $m.Groups[3].Value + ' = ' + $m.Groups[4].Value + ';' }
  $text=[regex]::Replace($text,$pattern, { param($m) $m.Groups[1].Value + 'static const ' + $m.Groups[2].Value + ' ' + $m.Groups[3].Value + ';' })
  $end=[regex]::Match($text,'\}\s*\z'); if(-not $end.Success){throw "namespace end missing in $path"}
  $text=$text.Insert($end.Index, ([string]::Join("`r`n",$definitions) + "`r`n"))
  if($rel -eq 'Safety\ResourceBitset.h') { $text=$text.Replace('inline ResourceBitset(unsigned long long','ResourceBitset(unsigned long long') }
  [IO.File]::WriteAllText($path,$text,[Text.UTF8Encoding]::new($false)); Write-Output "$rel fixed $($matchesFound.Count)"
}
$cpp=Join-Path $root 'Safety\ResourceBitset.cpp'; $text=[IO.File]::ReadAllText($cpp); $text=$text.Replace('inline ResourceBitset::ResourceBitset(', 'ResourceBitset::ResourceBitset('); [IO.File]::WriteAllText($cpp,$text,[Text.UTF8Encoding]::new($false))