$path = 'C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Decoder\Rf09ReplayEntry.h'
$text = [IO.File]::ReadAllText($path)
$needle = "namespace YAKSys_Hybrid_CPU::Core::Decoder`r`n{"
if (-not $text.Contains($needle)) { throw 'Expected namespace not found.' }
$text = $text.Replace($needle, $needle + "`r`n`tclass CanonicalBundle;")
[IO.File]::WriteAllText($path, $text, [Text.UTF8Encoding]::new($false))
