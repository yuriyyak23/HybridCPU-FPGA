$p='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Diagnostics\TraceSink.cpp'
$s=[IO.File]::ReadAllText($p)
foreach($name in @('RegisterFile','VectorRegisters','PredicateRegisters','MemoryWrites','BankQueueDepths','ThreadReadyQueueDepths')){
$old="((evt.$name == nullptr ? nullptr : evt.$name`->Length) != nullptr) ? (evt.$name == nullptr ? nullptr : evt.$name`->Length) : 0"
$new="evt.$name ? static_cast<int>(evt.$name`->size()) : 0"
$s=$s.Replace($old,$new).Replace(": evt.$name)",": *evt.$name)")
}
[IO.File]::WriteAllText($p,$s)