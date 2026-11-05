$root=Join-Path (Split-Path -Parent (Split-Path -Parent $PSScriptRoot)) 'NonRTL\\Arch\Compat'
$h=Join-Path $root 'HybridCPU_ISE.Arch.VLIW_Instruction.h'; $c=Join-Path $root 'HybridCPU_ISE.Arch.VLIW_Instruction.cpp'
$ht=[IO.File]::ReadAllText($h); $ct=[IO.File]::ReadAllText($c)
if (!$ht.Contains('ReadOnlySpan<unsigned char>')) { Write-Output 'Already converted'; exit 0 }
$ht=$ht.Replace('#include <string>', '#include <span>' + "`r`n" + '#include <string>')
$ht=$ht.Replace('using namespace YAKSys_Hybrid_CPU::Arch;','').Replace('using namespace YAKSys_Hybrid_CPU::Core::Registers;','')
$ht=$ht.Replace('ReadOnlySpan<unsigned char>','std::span<const unsigned char>').Replace('Span<unsigned char>','std::span<unsigned char>')
$ct=$ct.Replace('#include "../InvalidOpcodeException.h"','#include "../InvalidOpcodeException.h"'+"`r`n"+'#include "tangible_exception_helper.h"'+"`r`n"+'#include <stdexcept>')
$ct=$ct.Replace('using namespace YAKSys_Hybrid_CPU::Core::Registers;','')
$ct=$ct.Replace('std::format("Instruction at slot {0:s}', 'std::format(L"Instruction at slot {}')
$ct=$ct.Replace('ArgumentOutOfRangeException(L"value", L"RowStride must be <= 8191 (13 bits)")','ArgumentOutOfRangeException(L"RowStride must be <= 8191 (13 bits) (value)")')
$ct=$ct.Replace('ArgumentOutOfRangeException(L"value", L"VirtualThreadId must be 0-3")','ArgumentOutOfRangeException(L"VirtualThreadId must be 0-3 (value)")')
$ct=$ct.Replace('ReadOnlySpan<unsigned char>','std::span<const unsigned char>').Replace('Span<unsigned char>','std::span<unsigned char>')
$pack=@'
        auto PackField = [](unsigned char regId, const wchar_t *paramName) -> unsigned long long {
            if (regId == NoArchReg) return NoReg;
            if (regId > 31) throw ArgumentOutOfRangeException(std::format(L"Architectural register id must be in [0, 31] or NoArchReg. ({})", paramName));
            return regId;
        };
        unsigned long long packed = 0;
'@
$ct=$ct.Replace('unsigned long long packed = 0;',$pack)
$unpack=@'
        auto TryDecodeField = [](unsigned short encoded, unsigned char &regId) -> bool {
            if (encoded == NoReg) { regId = NoArchReg; return true; }
            if (encoded <= 31) { regId = static_cast<unsigned char>(encoded); return true; }
            regId = 0; return false;
        };
        unsigned short rawRd = static_cast<unsigned short>(packedRegisters & 0xFFFF);
'@
$ct=$ct.Replace('unsigned short rawRd = static_cast<unsigned short>(packedRegisters & 0xFFFF);',$unpack)
$pattern='(?s)\tbool VLIW_Instruction::TryReadBytesCore\(std::span<const unsigned char> source, int offset\).*?(?=\r?\n\})'
$replacement=@'
    bool VLIW_Instruction::TryReadBytesCore(std::span<const unsigned char> source, int offset)
    {
        if (offset < 0) throw std::out_of_range("offset");
        const auto start = static_cast<std::size_t>(offset);
        if (start > source.size() || source.size() - start < 32) return false;
        auto read = [&](std::size_t at) {
            unsigned long long result = 0;
            for (std::size_t i = 0; i < 8; ++i) result |= static_cast<unsigned long long>(source[at + i]) << (i * 8);
            return result;
        };
        word0 = ValidateWord0ForProductionIngress(read(start));
        word1 = read(start + 8);
        word2 = read(start + 16);
        word3 = ValidateWord3ForProductionIngress(read(start + 24));
        return true;
    }

    bool VLIW_Instruction::TryWriteBytes(std::span<unsigned char> destination)
    {
        if (destination.size() < 32) return false;
        const unsigned long long words[] = {word0, word1, word2, word3};
        for (std::size_t j = 0; j < 4; ++j)
            for (std::size_t i = 0; i < 8; ++i)
                destination[j * 8 + i] = static_cast<unsigned char>(words[j] >> (i * 8));
        return true;
    }

    bool VLIW_Instruction::TryReadBytes(std::span<const unsigned char> source, int offset)
    {
        return TryReadBytesCore(source, offset);
    }
'@
if([regex]::Matches($ct,$pattern).Count -ne 1){throw 'Cannot identify serialization methods'}
$ct=[regex]::Replace($ct,$pattern,$replacement)
[IO.File]::WriteAllText($h,$ht,[Text.UTF8Encoding]::new($false));[IO.File]::WriteAllText($c,$ct,[Text.UTF8Encoding]::new($false));Write-Output 'Converted VLIW instruction'
