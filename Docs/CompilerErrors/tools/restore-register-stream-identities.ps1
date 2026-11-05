$r='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core'; foreach($spec in @(@('Architecture\Registers\Architectural\RegisterIdentity.h','ArchRegId',32,'YAKSys_Hybrid_CPU::Core::Registers'),@('Architecture\Common\StreamEngineId.h','StreamEngineId',4,'YAKSys_Hybrid_CPU::Core'))) { $p=Join-Path $r $spec[0]; $s=[IO.File]::ReadAllText($p); $name=$spec[1]; $count=$spec[2]; if($s -notmatch "class $name final") { $def=@"

#include <stdexcept>
namespace $($spec[3]) {
class $name final {
    unsigned char value_ = 0;
public:
    static constexpr int Count = $count;
    explicit $name(unsigned char value = 0) : value_(value) { if (value >= Count) throw std::out_of_range("$name"); }
    unsigned char getValue() const noexcept { return value_; }
    unsigned char ToRawValue() const noexcept { return value_; }
    static bool IsRepresentable(int value) noexcept { return value >= 0 && value < Count; }
    static $name FromRawValue(unsigned char value) { return $name(value); }
    static $name Create(int value) { if (!IsRepresentable(value)) throw std::out_of_range("$name"); return $name(static_cast<unsigned char>(value)); }
    static bool TryCreate(int value, $name &out) noexcept { if (!IsRepresentable(value)) { out = $name(); return false; } out = $name(static_cast<unsigned char>(value)); return true; }
    bool operator==(const $name&) const = default;
};
}
"@; $s += $def; [IO.File]::WriteAllText($p,$s) } }
