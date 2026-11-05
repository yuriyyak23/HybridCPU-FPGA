$ErrorActionPreference='Stop'
$r='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\Frontend\Contracts'
$p=Join-Path $r 'SemanticRecords.h'; if(Test-Path -LiteralPath $p){throw 'Header exists'}
$source=@(
'#pragma once',
'#include "FrozenBytes.h"',
'#include <cstdint>',
'#include <memory>',
'#include <optional>',
'#include <string>',
'#include <utility>',
'namespace HybridCpu::ShadowFrontend {',
'struct Snapshot final {',
'    const std::wstring Kind;',
'    const std::shared_ptr<const FrozenBytes> Content;',
'    Snapshot(std::wstring kind, std::shared_ptr<const FrozenBytes> content) : Kind(std::move(kind)), Content(std::move(content)) {}',
'    bool operator==(const Snapshot& other) const {',
'        if (Kind != other.Kind || bool(Content) != bool(other.Content)) return false;',
'        return !Content || (Content->getSpan().size() == other.Content->getSpan().size() && std::equal(Content->getSpan().begin(), Content->getSpan().end(), other.Content->getSpan().begin()));',
'    }',
'};',
'struct TypedDecodeFailure final {',
'    std::uint16_t Code; int SlotIndex; std::wstring Field, RawHash, Message;',
'    bool operator==(const TypedDecodeFailure&) const = default;',
'};',
'struct SemanticSlot final {',
'    int SlotIndex; bool IsOccupied; std::uint32_t Opcode;',
'    std::optional<int> InstructionClass, SerializationClass;',
'    std::uint8_t Rd, Rs1, Rs2; std::int64_t Immediate;',
'    std::optional<std::uint16_t> CsrAddress;',
'    bool AcquireOrdering, ReleaseOrdering;',
'    std::shared_ptr<const Snapshot> RawSlot, InstructionPayload, SlotSideband, StaticBinding, ScalarLoadAddressPlan;',
'};',
'}',
'') -join "`r`n"
$source=$source.Replace('#include <any>','#include <any>')
$source=$source.Replace('#include <cstdint>',"#include <algorithm>`r`n#include <cstdint>")
[IO.File]::WriteAllText($p,$source,[Text.UTF8Encoding]::new($false))
$p=Join-Path $r 'Contracts.h'; $s=[IO.File]::ReadAllText($p); $m='#include "RequestTypes.h"'; if(-not $s.Contains($m)){throw 'Expected include missing'}
$s=$s.Replace($m,$m+"`r`n"+'#include "SemanticRecords.h"'); [IO.File]::WriteAllText($p,$s,[Text.UTF8Encoding]::new($false))
