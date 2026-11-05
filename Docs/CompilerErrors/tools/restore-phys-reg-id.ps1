$root='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core'
$path=Join-Path $root 'Architecture\Registers\Architectural\RegisterIdentity.h'; $s=[IO.File]::ReadAllText($path)
$marker='class VtId final {'; if(-not $s.Contains($marker)){throw 'VtId marker missing'}
$type=@"
class PhysRegId final {
    unsigned short value_ = 0;
public:
    static constexpr int RegisterCount = 128;
    static constexpr unsigned short MinValue = 0, MaxValue = RegisterCount - 1;
    static const PhysRegId Zero;
    explicit PhysRegId(unsigned short value = 0) : value_(value) {
        if (!IsRepresentable(value)) throw std::out_of_range("PhysRegId");
    }
    unsigned short getValue() const noexcept { return value_; }
    unsigned short ToRawValue() const noexcept { return value_; }
    static bool IsRepresentable(int value) noexcept { return value >= 0 && value < RegisterCount; }
    static PhysRegId FromRawValue(unsigned short value) { return PhysRegId(value); }
    static PhysRegId Create(int value) {
        if (!IsRepresentable(value)) throw std::out_of_range("PhysRegId");
        return PhysRegId(static_cast<unsigned short>(value));
    }
    static bool TryCreate(int value, PhysRegId& out) noexcept {
        if (!IsRepresentable(value)) { out = PhysRegId(); return false; }
        out = PhysRegId(static_cast<unsigned short>(value)); return true;
    }
    std::wstring ToString() const { return L"p" + std::to_wstring(value_); }
    operator int() const noexcept { return value_; }
    explicit operator unsigned short() const noexcept { return value_; }
    bool operator==(const PhysRegId&) const = default;
};
inline const PhysRegId PhysRegId::Zero{};
"@
$s=$s.Replace($marker,$type+"`r`n"+$marker); [IO.File]::WriteAllText($path,$s,[Text.UTF8Encoding]::new($false))
$physical=Join-Path $root 'Architecture\Registers\Physical\PhysicalRegisterFile.h'; $s=[IO.File]::ReadAllText($physical); $m='#include "../CSR/CsrFile.h"'; if(-not $s.Contains($m)){throw 'Physical include marker missing'}
$s=$s.Replace($m,$m+"`r`n"+'#include "../Architectural/RegisterIdentity.h"')
$m='static constexpr int TotalPhysRegs = 128;'; if(-not $s.Contains($m)){throw 'Physical count missing'}
$s=$s.Replace($m,$m+"`r`n`t`tstatic_assert(TotalPhysRegs == PhysRegId::RegisterCount);")
[IO.File]::WriteAllText($physical,$s,[Text.UTF8Encoding]::new($false))
$permit=Join-Path $root 'Pipeline\Safety\SecureComputeIssuePermitOwner.h'; $s=[IO.File]::ReadAllText($permit); $m="namespace YAKSys_Hybrid_CPU::Core`r`n{"; if(-not $s.Contains($m)){throw 'Permit namespace missing'}; $s=$s.Replace($m,$m+"`r`n`tclass SecureComputeCanonicalAdmissionResult;"); [IO.File]::WriteAllText($permit,$s,[Text.UTF8Encoding]::new($false))
