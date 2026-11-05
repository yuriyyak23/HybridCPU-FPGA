$p='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Virtualization\Compatibility\Frontend\Decode\YAKSys_Hybrid_CPU.Core.VmxCompatDecodeBoundary.h'
$s=[IO.File]::ReadAllText($p); $anchor='class VmxCompatDecodeBoundary final'; if(-not $s.Contains($anchor)){throw 'anchor missing'}
$s=$s.Replace($anchor,"struct VmxCompatDecodeRequest;`r`n`tstruct VmxCompatDecodeResult;`r`n`t$anchor"); [IO.File]::WriteAllText($p,$s)
