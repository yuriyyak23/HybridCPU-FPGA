$root='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Architecture\Common\'
$h=Join-Path $root 'YAKSys_Hybrid_CPU.Processor.h'; $c=Join-Path $root 'YAKSys_Hybrid_CPU.Processor.cpp'
$hs=[IO.File]::ReadAllText($h); $cs=[IO.File]::ReadAllText($c)
$hs=$hs.Replace('std::function<unsigned char[]?(unsigned long long, int)> CaptureMatrixTileMemoryReader()', 'std::function<std::optional<std::vector<unsigned char>>(unsigned long long, int)> CaptureMatrixTileMemoryReader()')
$old='(?s)\tstd::function<unsigned char\[\]\?\(unsigned long long, int\)> Processor::CPU_Core::CaptureMatrixTileMemoryReader\(\)\s*\{.*?\r?\n\t\}\r?\n(?=\r?\n\tbool Processor::CPU_Core::TryCommitRetiredMatrixTileStoreAllOrNone)'
$new=@"
	std::function<std::optional<std::vector<unsigned char>>(unsigned long long, int)> Processor::CPU_Core::CaptureMatrixTileMemoryReader()
	{
		auto boundMemory = GetBoundMainMemory();
		return [boundMemory](unsigned long long address, int length) -> std::optional<std::vector<unsigned char>>
		{
			if (length <= 0) return std::nullopt;
			const auto memoryLength = static_cast<unsigned long long>(boundMemory->getLength());
			const auto requestSize = static_cast<unsigned long long>(length);
			if (requestSize > memoryLength || address > memoryLength - requestSize) return std::nullopt;
			std::vector<unsigned char> buffer(static_cast<size_t>(length));
			if (!boundMemory->TryReadPhysicalRange(address, buffer)) return std::nullopt;
			return buffer;
		};
	}
"@
if(([regex]::Matches($cs,$old)).Count -ne 1){throw 'Reader implementation not found uniquely'}
$cs=[regex]::Replace($cs,$old,($new+"`r`n"))
$hs=$hs.Replace('inline int[]? std::shared_ptr<> GetBoundMemorySubsystemBankQueueDepths();','inline std::shared_ptr<std::vector<int>> GetBoundMemorySubsystemBankQueueDepths();')
$cs=$cs.Replace('inline int[]? std::shared_ptr<> Processor::CPU_Core::GetBoundMemorySubsystemBankQueueDepths()', 'inline std::shared_ptr<std::vector<int>> Processor::CPU_Core::GetBoundMemorySubsystemBankQueueDepths()').Replace('return (GetBoundMemorySubsystem() == nullptr ? nullptr : GetBoundMemorySubsystem()->GetBankQueueDepthsSnapshot());', 'auto memory = GetBoundMemorySubsystem();'+"`r`n`t`t"+'return memory ? std::make_shared<std::vector<int>>(memory->GetBankQueueDepthsSnapshot()) : nullptr;')
[IO.File]::WriteAllText($h,$hs); [IO.File]::WriteAllText($c,$cs)