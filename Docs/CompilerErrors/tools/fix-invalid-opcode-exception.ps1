$root=Join-Path (Split-Path -Parent (Split-Path -Parent $PSScriptRoot)) 'NonRTL\\Arch'
$h=Join-Path $root 'InvalidOpcodeException.h'; $cpp=Join-Path $root 'InvalidOpcodeException.cpp'
$ht=[IO.File]::ReadAllText($h); $ct=[IO.File]::ReadAllText($cpp)
if ($ct.Contains(': Exception(message)')) {
    $ht=$ht.Replace('#include <stdexcept>', '#include <stdexcept>' + "`r`n" + '#include <exception>')
    $ht=$ht.Replace('const bool IsProhibited;', 'const bool IsProhibited;' + "`r`n" + '        const std::exception_ptr InnerException;')
    $ht=$ht.Replace('bool isProhibited() const;', 'bool isProhibited() const;' + "`r`n" + '        std::exception_ptr getInnerException() const noexcept;')
    $ct=$ct.Replace('#include "InvalidOpcodeException.h"', '#include "InvalidOpcodeException.h"' + "`r`n" + '#include <codecvt>' + "`r`n" + '#include <locale>')
    $pattern='(?s)\tInvalidOpcodeException::InvalidOpcodeException\(const std::wstring &message, const std::wstring &opcodeIdentifier, int slotIndex, bool isProhibited\).*?(?=\r?\n\})'
    $replacement=@'
    namespace {
        std::string ToUtf8(const std::wstring &text) {
            return std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t>().to_bytes(text);
        }
    }
    std::exception_ptr InvalidOpcodeException::getInnerException() const noexcept { return InnerException; }
    InvalidOpcodeException::InvalidOpcodeException(const std::wstring &message, const std::wstring &opcodeIdentifier, int slotIndex, bool isProhibited)
        : std::runtime_error(ToUtf8(message)), OpcodeIdentifier(opcodeIdentifier), SlotIndex(slotIndex), IsProhibited(isProhibited), InnerException(nullptr) {}
    InvalidOpcodeException::InvalidOpcodeException(const std::wstring &message)
        : std::runtime_error(ToUtf8(message)), OpcodeIdentifier(), SlotIndex(-1), IsProhibited(false), InnerException(nullptr) {}
    InvalidOpcodeException::InvalidOpcodeException(const std::wstring &message, std::runtime_error inner)
        : std::runtime_error(ToUtf8(message)), OpcodeIdentifier(), SlotIndex(-1), IsProhibited(false), InnerException(std::make_exception_ptr(inner)) {}
'@
    if([regex]::Matches($ct,$pattern).Count -ne 1){throw 'Unexpected InvalidOpcodeException constructors'}
    $ct=[regex]::Replace($ct,$pattern,$replacement)
    [IO.File]::WriteAllText($h,$ht,[Text.UTF8Encoding]::new($false));[IO.File]::WriteAllText($cpp,$ct,[Text.UTF8Encoding]::new($false));Write-Output 'Converted InvalidOpcodeException'
} else {Write-Output 'Already converted'}
