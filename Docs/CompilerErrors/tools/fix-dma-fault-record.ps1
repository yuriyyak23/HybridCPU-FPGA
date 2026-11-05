$p='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Runtime\IO\Dma\DmaDomainBinding.h'; $s=[IO.File]::ReadAllText($p).Replace('#pragma once','#pragma once'+"`r`n"+'#include <string>'+"`r`n"+'#include <utility>'); $idx=$s.IndexOf('// C# TO C++ CONVERTER TASK: C# ''records'' are not converted by C# to C++ Converter:'); if($idx -lt 0){throw 'Insertion marker missing'}
$block=@"
	struct DmaFault final {
		DmaFaultKind Kind = DmaFaultKind::None;
		DmaFaultDisposition Disposition = DmaFaultDisposition::None;
		unsigned long long FaultAddress = 0;
		bool IsWrite = false;
		std::wstring Message;
		bool isFaulted() const noexcept { return Kind != DmaFaultKind::None; }
		static DmaFault Abort(DmaFaultKind kind, unsigned long long address, bool isWrite, std::wstring message) { return {kind, DmaFaultDisposition::Abort, address, isWrite, std::move(message)}; }
		static DmaFault Replay(DmaFaultKind kind, unsigned long long address, bool isWrite, std::wstring message) { return {kind, DmaFaultDisposition::Replay, address, isWrite, std::move(message)}; }
	};

"@
$s=$s.Insert($idx,$block); [IO.File]::WriteAllText($p,$s)