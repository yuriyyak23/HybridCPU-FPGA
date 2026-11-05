$ErrorActionPreference='Stop'
$r='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Decoder'
foreach($f in @('DecodedBundleDescriptor.h','DecodedBundleDescriptor.cpp')){
 $p=Join-Path $r $f; $s=[IO.File]::ReadAllText($p); $m='std::optional<DecodedBundleTransformHistory> &transformHistory'
 if(-not $s.Contains($m)){throw "Expected optional missing in $f"}
 $s=$s.Replace($m,'const std::optional<DecodedBundleTransformHistory> &transformHistory'); [IO.File]::WriteAllText($p,$s,[Text.UTF8Encoding]::new($false))
}
