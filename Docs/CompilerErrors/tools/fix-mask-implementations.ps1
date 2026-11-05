$root=Join-Path (Split-Path -Parent (Split-Path -Parent $PSScriptRoot)) 'CloseToHSL\\Core\Pipeline'
foreach ($item in @(@{Type='ResourceBitset';Path='Safety\ResourceBitset'}, @{Type='SafetyMask128';Path='Certificates\SafetyMask128'})) {
    $h=Join-Path $root ($item.Path+'.h'); $cpp=Join-Path $root ($item.Path+'.cpp'); $ht=[IO.File]::ReadAllText($h); $ct=[IO.File]::ReadAllText($cpp); $type=$item.Type
    $old="$type other = dynamic_cast<$type>(obj);`r`n`t`treturn other != nullptr && Equals(other);"
    $oldLf=$old.Replace("`r`n","`n")
    $new="const auto *other = std::any_cast<$type>(&obj);`r`n`t`treturn other != nullptr && Equals(*other);"
    if($ct.Contains($old)){$ct=$ct.Replace($old,$new)} elseif($ct.Contains($oldLf)){$ct=$ct.Replace($oldLf,$new)} elseif(!$ct.Contains('std::any_cast<'+$type+'>')){throw "Equals body missing: $type"}
    $ct=$ct.Replace('return HashCode::Combine(Low, High);','return static_cast<int>((Low ^ (Low >> 32) ^ High ^ (High >> 32)) & 0x7FFFFFFFULL);')
    $ct=$ct.Replace('std::format("'+$type+'(Low=0x{0:0>.16X}, High=0x{1:0>.16X})", Low, High)','std::format(L"'+$type+'(Low=0x{:016X}, High=0x{:016X})", Low, High)')
    if($type -eq 'ResourceBitset') {
        $ht=[regex]::Replace($ht,'(?m)^(\s*)inline (?=(?:explicit |unsigned |ResourceBitset |bool ))','$1')
        $ct=[regex]::Replace($ct,'(?m)^(\s*)inline (?=[^\r\n]*ResourceBitset::)','$1')
    }
    [IO.File]::WriteAllText($h,$ht,[Text.UTF8Encoding]::new($false));[IO.File]::WriteAllText($cpp,$ct,[Text.UTF8Encoding]::new($false));Write-Output "Fixed $type"
}
