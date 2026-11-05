$cpp=[IO.File]::ReadAllText('C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Virtualization\Compatibility\Frontend\Decode\VmxFrozenOpcodeValues.h')
$cs=[IO.File]::ReadAllText('C:\Users\Yuriy\Desktop\HybridCPU ISE\HybridCPU_ISE\NonRTL\Arch\Generated\GeneratedIsaOpcodeValues.g.cs')
$items=[regex]::Matches($cpp,'constexpr unsigned short (\w+) = (\d+);'); if($items.Count -ne 15){throw 'unexpected opcode count'}
foreach($item in $items){$name=$item.Groups[1].Value; $value=$item.Groups[2].Value; if(-not [regex]::IsMatch($cs,"const ushort $name\s*=\s*$value\s*;")){throw "mismatch $name"}}
'All 15 opcodes match C# generated constants.'
