$root='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Memory\DMA\'; $p=Join-Path $root 'DmaChannelId.h'; $s=[IO.File]::ReadAllText($p); $s=$s.Replace('#pragma once','#pragma once'+"`r`n"+'#include <stdexcept>'); $idx=$s.LastIndexOf("`r`n}"); if($idx -lt 0){throw 'Namespace end not found'}
$code=@"
	class DmaChannelId final {
		unsigned char value_ = 0;
	public:
		static constexpr unsigned char MinValue = 0;
		static constexpr unsigned char MaxValue = 7;
		DmaChannelId() = default;
		explicit DmaChannelId(unsigned char value) : value_(value) { if (!IsRepresentable(value)) throw std::out_of_range("DMA channel must be in range [0, 7]"); }
		static constexpr bool IsRepresentable(unsigned char value) noexcept { return value <= MaxValue; }
		static DmaChannelId Create(unsigned char value) { return DmaChannelId(value); }
		static bool TryCreate(unsigned char value, DmaChannelId& result) { if (!IsRepresentable(value)) { result = DmaChannelId(); return false; } result = DmaChannelId(value); return true; }
		static DmaChannelId FromRawValue(unsigned char value) { return Create(value); }
		unsigned char ToRawValue() const noexcept { return value_; }
		operator unsigned char() const noexcept { return value_; }
	};
"@
$s=$s.Insert($idx,"`r`n"+$code); [IO.File]::WriteAllText($p,$s)
$p=Join-Path $root 'DMAController.h'; $s=[IO.File]::ReadAllText($p).Replace('#include "../../Core/Architecture/Common/YAKSys_Hybrid_CPU.Processor.h"','#include "../../Core/Architecture/Common/YAKSys_Hybrid_CPU.Processor.h"'+"`r`n"+'#include "DmaChannelId.h"'+"`r`n"+'#include <span>').Replace('std::shared_ptr<DmaChannelId> channel','DmaChannelId channel').Replace('ReadOnlySpan<unsigned char>','std::span<const unsigned char>').Replace('Span<unsigned char>','std::span<unsigned char>').Replace('std::optional<TransferCompletionCallback> &callback = std::nullopt','const std::optional<TransferCompletionCallback> &callback = std::nullopt'); [IO.File]::WriteAllText($p,$s)
$p=Join-Path $root 'DMAController.cpp'; $s=[IO.File]::ReadAllText($p).Replace('std::shared_ptr<DmaChannelId> channel','DmaChannelId channel').Replace('ReadOnlySpan<unsigned char>','std::span<const unsigned char>').Replace('Span<unsigned char>','std::span<unsigned char>').Replace('std::optional<TransferCompletionCallback> &callback)','const std::optional<TransferCompletionCallback> &callback)'); [IO.File]::WriteAllText($p,$s)
$p='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\tangible_event.h'; $s=[IO.File]::ReadAllText($p).Replace('#include <functional>','#include <functional>'+"`r`n"+'#include <memory>').Replace('class EventArgs { public: virtual ~EventArgs() = default; };','class EventArgs : public std::enable_shared_from_this<EventArgs> { public: virtual ~EventArgs() = default; };'); [IO.File]::WriteAllText($p,$s)