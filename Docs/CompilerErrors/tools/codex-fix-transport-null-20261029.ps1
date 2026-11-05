$ErrorActionPreference='Stop'
$p='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Decoder\DecodedBundleTransportProjector.cpp'
$s=[IO.File]::ReadAllText($p)
$old='ArgumentNullException::ThrowIfNull(canonicalBundle);'
if (($s.Split([string[]]@($old),[StringSplitOptions]::None).Length - 1) -ne 2) { throw 'Expected two canonical null checks' }
$s=$s.Replace($old,'if (!canonicalBundle) throw std::invalid_argument("canonicalBundle must not be null.");')
[IO.File]::WriteAllText($p,$s,[Text.UTF8Encoding]::new($false))
