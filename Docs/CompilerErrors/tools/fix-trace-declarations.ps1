$p='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Diagnostics\TraceSink.h'
$s=[IO.File]::ReadAllText($p)
$start=$s.IndexOf("`t/// <summary>`r`n`t/// Vector exception flags for tracing")
$end=$s.IndexOf("`t/// <summary>`r`n`t/// Memory delta",$start)
$insert=$s.IndexOf("`t/// <summary>`r`n`t/// Single execution event for tracing")
if($start -lt 0 -or $end -lt 0 -or $insert -lt 0){throw 'Trace declarations not found'}
$block=$s.Substring($start,$end-$start); $s=$s.Remove($start,$end-$start); $s=$s.Insert($insert,$block)
$s=$s.Replace('namespace HybridCPU_ISE::Core'+"`r`n"+'{','namespace HybridCPU_ISE::Core'+"`r`n"+'{'+"`r`n`t"+'class BinaryWriter;')
foreach($entry in @(@('unsigned long long[]? RegisterFile','std::shared_ptr<std::vector<unsigned long long>> RegisterFile'),@('unsigned long long[]? VectorRegisters','std::shared_ptr<std::vector<unsigned long long>> VectorRegisters'),@('unsigned short[]? PredicateRegisters','std::shared_ptr<std::vector<unsigned short>> PredicateRegisters'),@('MemoryDelta[]? MemoryWrites','std::shared_ptr<std::vector<MemoryDelta>> MemoryWrites'),@('int[]? BankQueueDepths','std::shared_ptr<std::vector<int>> BankQueueDepths'),@('int[]? ThreadReadyQueueDepths','std::shared_ptr<std::vector<int>> ThreadReadyQueueDepths'))){$s=$s.Replace($entry[0],$entry[1])}
foreach($name in @('DecodedBundleStateOwnerKind','DecodedBundleStateKind','DecodedBundleStateOrigin','AssistInvalidationReason')){$s=$s.Replace("$name $name = static_cast<$name>(0);", "YAKSys_Hybrid_CPU::Core::$name $name = static_cast<YAKSys_Hybrid_CPU::Core::$name>(0);")}
[IO.File]::WriteAllText($p,$s)