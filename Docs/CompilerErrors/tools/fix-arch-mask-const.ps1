$root=Join-Path (Split-Path -Parent (Split-Path -Parent $PSScriptRoot)) 'CloseToHSL\\Core\Pipeline\Certificates'
$h=Join-Path $root 'SafetyMask128.h'; $c=Join-Path $root 'SafetyMask128.cpp'; $ah=Join-Path $root 'ArchLegalityMask.h'; $ac=Join-Path $root 'ArchLegalityMask.cpp'
$ht=[IO.File]::ReadAllText($h); $ct=[IO.File]::ReadAllText($c); $aht=[IO.File]::ReadAllText($ah); $act=[IO.File]::ReadAllText($ac)
foreach($name in @('operator\s*\|\s*','operator\s*&\s*','operator\s*==\s*','operator\s*!=\s*','ConflictsWith','NoConflictsWith','Equals','GetHashCode','ToString')) {
 $ht=[regex]::Replace($ht, '(?m)('+ $name + '\([^\r\n]*?\))\s*;', '$1 const;')
 $ct=[regex]::Replace($ct, '(?m)(SafetyMask128::'+$name+'\([^\r\n]*?\))\s*\r?\n\s*\{', '$1 const' + "`r`n`t{")
}
foreach($type in @('ArchLegalityMask','BackendHazardMask')) {
 $pattern=$type+' m = dynamic_cast<'+$type+'>\(obj\);\s*return m != nullptr && Equals\(m\);'
 $act=[regex]::Replace($act,$pattern,('const auto *m = std::any_cast<'+$type+'>(&obj); return m != nullptr && Equals(*m);'))
 $act=$act.Replace('std::format("'+$type+'({0:s})", Mask)','std::format(L"'+$type+'({})", Mask.ToString())')
}
[IO.File]::WriteAllText($h,$ht,[Text.UTF8Encoding]::new($false));[IO.File]::WriteAllText($c,$ct,[Text.UTF8Encoding]::new($false));[IO.File]::WriteAllText($ac,$act,[Text.UTF8Encoding]::new($false));Write-Output 'Updated mask constness and conversion'
