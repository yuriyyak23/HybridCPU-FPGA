$ErrorActionPreference='Stop'
$r='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Execution\Memory\Scratch'
$p=Join-Path $r 'ScratchBankController.h'; $s=[IO.File]::ReadAllText($p)
$s=$s.Replace('#include <vector>',"#include <vector>`r`n#include <span>").Replace('ReadOnlySpan<unsigned char>','std::span<const unsigned char>').Replace('Span<unsigned char>','std::span<unsigned char>')
$s=$s.Replace('inline void ReadElement','void ReadElement').Replace('inline void WriteElement','void WriteElement').Replace('inline int CheckConflict','int CheckConflict')
[IO.File]::WriteAllText($p,$s,[Text.UTF8Encoding]::new($false))
$p=Join-Path $r 'ScratchBankController.cpp'; $s=[IO.File]::ReadAllText($p)
$s=$s.Replace('#include "ScratchBankController.h"',"#include `"ScratchBankController.h`"`r`n#include <algorithm>`r`n#include <stdexcept>")
$s=$s.Replace('ReadOnlySpan<unsigned char>','std::span<const unsigned char>').Replace('Span<unsigned char>','std::span<unsigned char>')
$s=$s.Replace('inline void ScratchBankController::','void ScratchBankController::').Replace('inline int ScratchBankController::','int ScratchBankController::')
$start=$s.IndexOf("`tvoid ScratchBankController::ReadElement("); $end=$s.IndexOf("`tint ScratchBankController::CheckConflict(",$start)
if($start -lt 0 -or $end -lt 0){throw 'Expected read/write blocks missing'}
$methods=@(
'    void ScratchBankController::ReadElement(int elementIndex, int elementSize, std::span<unsigned char> dest)',
'    {',
'        if (elementIndex < 0 || elementSize < 0) throw std::out_of_range("Invalid scratch element.");',
'        const auto bank = static_cast<std::size_t>(elementIndex & (BANK_COUNT - 1));',
'        const auto offset = static_cast<std::size_t>(elementIndex >> 2) * static_cast<std::size_t>(elementSize);',
'        const auto size = static_cast<std::size_t>(elementSize);',
'        if (bank >= _banks.size() || offset > _banks[bank].size() || size > _banks[bank].size() - offset || dest.size() < size) throw std::out_of_range("Scratch read out of range.");',
'        std::copy_n(_banks[bank].begin() + offset, size, dest.begin());',
'        setTotalAccesses(getTotalAccesses() + 1);',
'    }',
'    void ScratchBankController::WriteElement(int elementIndex, int elementSize, std::span<const unsigned char> src)',
'    {',
'        if (elementIndex < 0 || elementSize < 0) throw std::out_of_range("Invalid scratch element.");',
'        const auto bank = static_cast<std::size_t>(elementIndex & (BANK_COUNT - 1));',
'        const auto offset = static_cast<std::size_t>(elementIndex >> 2) * static_cast<std::size_t>(elementSize);',
'        const auto size = static_cast<std::size_t>(elementSize);',
'        if (bank >= _banks.size() || offset > _banks[bank].size() || size > _banks[bank].size() - offset || src.size() > size) throw std::out_of_range("Scratch write out of range.");',
'        std::copy(src.begin(), src.end(), _banks[bank].begin() + offset);',
'        setTotalAccesses(getTotalAccesses() + 1);',
'    }',
'') -join "`r`n"
$s=$s.Substring(0,$start)+$methods+$s.Substring($end)
$s=$s.Replace('source.Length','source.size()').Replace('dest.Length','dest.size()').Replace('source.Slice(i * elementSize, elementSize)','source.subspan(i * elementSize, elementSize)').Replace('dest.Slice(i * elementSize, elementSize)','dest.subspan(i * elementSize, elementSize)')
[IO.File]::WriteAllText($p,$s,[Text.UTF8Encoding]::new($false))
