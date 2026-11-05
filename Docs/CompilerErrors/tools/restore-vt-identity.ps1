$ErrorActionPreference='Stop'
$p='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Architecture\Registers\Architectural\RegisterIdentity.h'
$s=[IO.File]::ReadAllText($p)
$marker='inline const ArchRegId ArchRegId::Zero{};'
if (-not $s.Contains($marker) -or $s.Contains('class VtId final')) { throw 'Unexpected identity header state' }
$add=@"

class VtId final {
    unsigned char value_ = 0;
public:
    static constexpr int SmtWayCount = 4;
    static constexpr unsigned char MinValue = 0, MaxValue = SmtWayCount - 1;
    explicit VtId(unsigned char value = 0) : value_(value) {
        if (!IsRepresentable(value)) throw std::out_of_range("VtId");
    }
    unsigned char getValue() const noexcept { return value_; }
    unsigned char ToRawValue() const noexcept { return value_; }
    static bool IsRepresentable(int value) noexcept { return value >= 0 && value < SmtWayCount; }
    static VtId FromRawValue(unsigned char value) { return VtId(value); }
    static VtId Create(int value) {
        if (!IsRepresentable(value)) throw std::out_of_range("VtId");
        return VtId(static_cast<unsigned char>(value));
    }
    static bool TryCreate(int value, VtId& out) noexcept {
        if (!IsRepresentable(value)) { out = VtId(); return false; }
        out = VtId(static_cast<unsigned char>(value));
        return true;
    }
    std::wstring ToString() const { return L"vt" + std::to_wstring(value_); }
    operator int() const noexcept { return value_; }
    explicit operator unsigned char() const noexcept { return value_; }
    bool operator==(const VtId&) const = default;
};
"@
$s=$s.Replace($marker,$marker+$add)
[IO.File]::WriteAllText($p,$s,[Text.UTF8Encoding]::new($false))
