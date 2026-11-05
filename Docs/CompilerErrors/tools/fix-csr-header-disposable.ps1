$path='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Architecture\Registers\CSR\CsrFile.h'
$s=[IO.File]::ReadAllText($path); $old='using namespace YAKSys_Hybrid_CPU::Arch;'; if(-not $s.Contains($old)){throw 'Expected using not found'}
$s=$s.Replace($old,'class Disposable;'); [IO.File]::WriteAllText($path,$s,[Text.UTF8Encoding]::new($false))
