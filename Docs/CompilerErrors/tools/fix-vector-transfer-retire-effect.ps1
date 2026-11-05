$root='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core'
$h=Join-Path $root 'Pipeline\MicroOps\Vector\VectorTransferRetireEffect.h'; $c=Join-Path $root 'Pipeline\MicroOps\Vector\VectorTransferRetireEffect.cpp'; $processor=Join-Path $root 'Architecture\Common\YAKSys_Hybrid_CPU.Processor.cpp'
$hs=[IO.File]::ReadAllText($h); $cs=[IO.File]::ReadAllText($c); $ps=[IO.File]::ReadAllText($processor)
$hs=$hs.Replace('#include <vector>',"#include <vector>`r`n#include <span>").Replace('ReadOnlySpan<unsigned char>','std::span<const unsigned char>').Replace('ReadOnlyMemory<unsigned char>','std::vector<unsigned char>')
$start=$cs.IndexOf("`tVectorTransferRetireEffect::VectorTransferRetireEffect("); $end=$cs.IndexOf("`tunsigned int VectorTransferRetireEffect::getOpcode()",$start)
if($start -lt 0 -or $end -lt 0){throw 'Constructor not found'}
$ctor=@"
`tVectorTransferRetireEffect::VectorTransferRetireEffect(unsigned int opcode, unsigned long long sourceAddress, unsigned long long destinationAddress, unsigned long long elementCount, int elementSize, unsigned short stride, std::span<const unsigned char> packedData)
`t`t: Opcode(opcode), SourceAddress(sourceAddress), DestinationAddress(destinationAddress),
`t`t  ElementCount(elementCount), ElementSize(elementSize), Stride(stride),
`t`t  _packedData(packedData.begin(), packedData.end())
`t{
`t}

"@
$cs=$cs.Remove($start,$end-$start).Insert($start,$ctor).Replace('ReadOnlyMemory<unsigned char>','std::vector<unsigned char>').Replace('static_cast<std::vector<unsigned char>>(_packedData.Clone())','_packedData')
$old='vectorTransferEffect->getPackedData().Length'; if(-not $ps.Contains($old)){throw 'Expected caller missing'}
$ps=$ps.Replace($old,'vectorTransferEffect->getPackedData().size()')
[IO.File]::WriteAllText($h,$hs,[Text.UTF8Encoding]::new($false));[IO.File]::WriteAllText($c,$cs,[Text.UTF8Encoding]::new($false));[IO.File]::WriteAllText($processor,$ps,[Text.UTF8Encoding]::new($false))
