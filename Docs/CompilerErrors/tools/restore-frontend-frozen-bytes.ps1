$ErrorActionPreference='Stop'
$r='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\Frontend\Contracts'; $p=Join-Path $r 'FrozenBytes.h'
if(Test-Path -LiteralPath $p){throw 'FrozenBytes header already exists'}
$lines=@(
'#pragma once',
'#include <any>',
'#include <memory>',
'#include <span>',
'#include <vector>',
'namespace HybridCpu::ShadowFrontend {',
'class FrozenBytes final {',
'    const std::vector<unsigned char> bytes;',
'public:',
'    explicit FrozenBytes(std::span<const unsigned char> value) : bytes(value.begin(), value.end()) {}',
'    int getLength() const { return static_cast<int>(bytes.size()); }',
'    std::vector<unsigned char> Copy() const { return bytes; }',
'    std::span<const unsigned char> getSpan() const noexcept { return bytes; }',
'    bool Equals(const std::shared_ptr<FrozenBytes>& other) const { return other && bytes == other->bytes; }',
'    bool Equals(const std::any& obj) const {',
'        const auto value = std::any_cast<std::shared_ptr<FrozenBytes>>(&obj);',
'        return value && Equals(*value);',
'    }',
'    int GetHashCode() const { return getLength(); }',
'};',
'}',
'') -join "`r`n"
[IO.File]::WriteAllText($p,$lines,[Text.UTF8Encoding]::new($false))
$p=Join-Path $r 'Contracts.h'; $s=[IO.File]::ReadAllText($p)
$start=$s.IndexOf("`tclass FrozenBytes final"); $end=$s.IndexOf('// C# TO C++ CONVERTER TASK:', $start)
if($start -lt 0 -or $end -lt 0){throw 'Expected FrozenBytes block missing'}
$s=$s.Substring(0,$start)+$s.Substring($end)
$s=$s.Replace('#pragma once',"#pragma once`r`n`r`n#include `"FrozenBytes.h`"")
[IO.File]::WriteAllText($p,$s,[Text.UTF8Encoding]::new($false))
$p=Join-Path $r 'Contracts.cpp'; $s=[IO.File]::ReadAllText($p); $start=$s.IndexOf("`tFrozenBytes::FrozenBytes("); $end=$s.IndexOf("`tDecodeResult::DecodeResult(",$start)
if($start -lt 0 -or $end -lt 0){throw 'Expected FrozenBytes definitions missing'}
$s=$s.Substring(0,$start)+$s.Substring($end); [IO.File]::WriteAllText($p,$s,[Text.UTF8Encoding]::new($false))
