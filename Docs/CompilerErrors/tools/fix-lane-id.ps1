$p=Join-Path (Split-Path -Parent (Split-Path -Parent $PSScriptRoot)) 'CloseToHSL\\Core\Pipeline\Scheduling\LaneId.h'
$t=[IO.File]::ReadAllText($p)
if($t.Contains('class LaneId final')){Write-Output 'Already fixed';exit}
$code=@"
#include <stdexcept>
#include <string>
namespace YAKSys_Hybrid_CPU::Core {
class LaneId final {
    unsigned char value_ = 0;
public:
    static constexpr int LaneCount = 8;
    static constexpr unsigned char MinValue = 0, MaxValue = 7;
    LaneId() = default;
    explicit LaneId(unsigned char value) : value_(value) { if(value > MaxValue) throw std::out_of_range("Physical lane id must be in [0, 7]."); }
    static bool IsRepresentable(int value) { return value >= 0 && value < LaneCount; }
    static LaneId FromRawValue(unsigned char value) { return LaneId(value); }
    unsigned char ToRawValue() const { return value_; }
    unsigned char getValue() const { return value_; }
    static LaneId Create(int value) { if(!IsRepresentable(value)) throw std::out_of_range("Physical lane id must be in [0, 7]."); return LaneId(static_cast<unsigned char>(value)); }
    static bool TryCreate(int value, LaneId &out) { if(!IsRepresentable(value)){out=LaneId();return false;} out=LaneId(static_cast<unsigned char>(value));return true; }
    std::wstring ToString() const { return L"lane" + std::to_wstring(value_); }
    operator int() const { return value_; }
    explicit operator unsigned char() const { return value_; }
    bool operator==(const LaneId &) const = default;
    static const LaneId Zero;
};
inline const LaneId LaneId::Zero{};
}
"@
$t=$t+"`r`n"+$code;[IO.File]::WriteAllText($p,$t,[Text.UTF8Encoding]::new($false));Write-Output 'Restored LaneId'