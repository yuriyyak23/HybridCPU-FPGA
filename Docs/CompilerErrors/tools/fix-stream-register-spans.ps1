$ErrorActionPreference='Stop'
$p='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Memory\Registers\YAKSys_Hybrid_CPU.Memory.StreamRegisterFile.cpp'
$s=[IO.File]::ReadAllText($p)
if(-not $s.Contains('memory.AsSpan(') -or -not $s.Contains('RegisterEntry reg = registers[regIndex];')){throw 'Unexpected source state'}
$s=$s.Replace('#include "YAKSys_Hybrid_CPU.Memory.StreamRegisterFile.h"',"#include `"YAKSys_Hybrid_CPU.Memory.StreamRegisterFile.h`"`r`n#include <algorithm>")
$s=$s.Replace('rejectKind = AssistStreamRegisterRejectKind::None;' + "`r`n`t`t" + 'if (TryFindAssistReuseRegister(sourceAddr, elementSize, 1, _))','rejectKind = AssistStreamRegisterRejectKind::None;' + "`r`n`t`t" + 'int unusedRegisterIndex = 0;' + "`r`n`t`t" + 'if (TryFindAssistReuseRegister(sourceAddr, elementSize, 1, unusedRegisterIndex))')
$s=$s.Replace('TrySelectAssistRegisterSlot(policy, _, rejectKind)', 'TrySelectAssistRegisterSlot(policy, unusedRegisterIndex, rejectKind)')
$s=[regex]::Replace($s,'RegisterEntry reg = registers\[([^\]]+)\];','RegisterEntry& reg = registers[$1];')
$s=$s.Replace('memory.AsSpan(static_cast<int>(reg.SourceAddress), static_cast<int>(bytesToLoad))','std::span<const unsigned char>(memory).subspan(static_cast<std::size_t>(reg.SourceAddress), bytesToLoad)')
$s=$s.Replace('sourceData.Length','sourceData.size()').Replace('destination.Length','destination.size()').Replace('source.Length','source.size()')
$s=$s.Replace('sourceData.Slice(0, static_cast<int>(bytesToLoad)).CopyTo(reg.Data);','std::copy_n(sourceData.begin(), bytesToLoad, reg.Data.begin());')
$s=$s.Replace('reg.Data.AsSpan(0, static_cast<int>(byteCount))->CopyTo(destination);','std::copy_n(reg.Data.begin(), byteCount, destination.begin());')
$s=$s.Replace('source.Slice(0, static_cast<int>(byteCount)).CopyTo(reg.Data);','std::copy_n(source.begin(), byteCount, reg.Data.begin());')
$s=$s.Replace('_ = InvalidateOverlappingRangeAndCount(address, byteCount);','(void)InvalidateOverlappingRangeAndCount(address, byteCount);')
[IO.File]::WriteAllText($p,$s,[Text.UTF8Encoding]::new($false))
