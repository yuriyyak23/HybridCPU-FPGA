$ErrorActionPreference='Stop'
$r='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core'
$p=Join-Path $r 'Architecture\BinaryFormat\SlotEncoding\SlotId.h'; $s=[IO.File]::ReadAllText($p)
if($s.Contains('class SlotId final')){throw 'SlotId already exists'}
$s += @"

#include "../../../Pipeline/Metadata/BundleWidth.h"
#include <stdexcept>
#include <string>
namespace YAKSys_Hybrid_CPU::Core {
class SlotId final {
    unsigned char value_ = 0;
public:
    static constexpr int SlotCount = BundleSlotCount;
    static constexpr unsigned char MinValue = 0, MaxValue = SlotCount - 1;
    static const SlotId Zero;
    explicit SlotId(unsigned char value = 0) : value_(value) {
        if (!IsRepresentable(value)) throw std::out_of_range("SlotId");
    }
    unsigned char getValue() const noexcept { return value_; }
    unsigned char ToRawValue() const noexcept { return value_; }
    static bool IsRepresentable(int value) noexcept { return value >= 0 && value < SlotCount; }
    static SlotId FromRawValue(unsigned char value) { return SlotId(value); }
    static SlotId Create(int value) {
        if (!IsRepresentable(value)) throw std::out_of_range("SlotId");
        return SlotId(static_cast<unsigned char>(value));
    }
    static bool TryCreate(int value, SlotId& out) noexcept {
        if (!IsRepresentable(value)) { out = SlotId(); return false; }
        out = SlotId(static_cast<unsigned char>(value));
        return true;
    }
    std::wstring ToString() const { return L"slot" + std::to_wstring(value_); }
    operator int() const noexcept { return value_; }
    explicit operator unsigned char() const noexcept { return value_; }
    bool operator==(const SlotId&) const = default;
};
inline const SlotId SlotId::Zero{};
}
"@
[IO.File]::WriteAllText($p,$s,[Text.UTF8Encoding]::new($false))
$p=Join-Path $r 'Pipeline\Scheduling\PostStageBIssuedAttempt.h'; $s=[IO.File]::ReadAllText($p); $m='#include "../MicroOps/Types/MicroOp.h"'
if(-not $s.Contains($m)){throw 'Missing MicroOp include'}
$s=$s.Replace($m,$m+"`r`n"+'#include "../../Architecture/BinaryFormat/SlotEncoding/SlotId.h"')
[IO.File]::WriteAllText($p,$s,[Text.UTF8Encoding]::new($false))
