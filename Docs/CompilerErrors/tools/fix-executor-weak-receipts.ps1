$root='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Runtime\Events\Hypercalls'
$p=Join-Path $root 'DomainHypercallRuntimeExecutor.h'; $s=[IO.File]::ReadAllText($p); $s=$s.Replace('#include <mutex>',"#include <mutex>`r`n#include <map>")
$old='const std::shared_ptr<ConditionalWeakTable<std::shared_ptr<ExecutionReceipt>, std::shared_ptr<LiveReceipt>>> _liveReceipts = std::make_shared<ConditionalWeakTable<std::shared_ptr<ExecutionReceipt>, std::shared_ptr<LiveReceipt>>>();'
$new='std::map<std::weak_ptr<ExecutionReceipt>, std::shared_ptr<LiveReceipt>, std::owner_less<std::weak_ptr<ExecutionReceipt>>> _liveReceipts;'
if(-not $s.Contains($old)){throw 'table declaration not found'}; $s=$s.Replace($old,$new); [IO.File]::WriteAllText($p,$s)
$p=Join-Path $root 'DomainHypercallRuntimeExecutor.cpp'; $s=[IO.File]::ReadAllText($p)
$s=$s.Replace('_liveReceipts->Add(receipt, std::make_shared<LiveReceipt>(e2, restoreOwner, lifecycleGate));','_liveReceipts.emplace(receipt, std::make_shared<LiveReceipt>(e2, restoreOwner, lifecycleGate));')
$s=$s.Replace('!_liveReceipts->TryGetValue(receipt, live)', '((_liveReceipts.find(receipt) == _liveReceipts.end()) || !(live = _liveReceipts.find(receipt)->second))')
$s=$s.Replace('_liveReceipts->TryGetValue(receipt, live)', '((_liveReceipts.find(receipt) != _liveReceipts.end()) && (live = _liveReceipts.find(receipt)->second))')
$pat='(?s)std::shared_ptr<LiveReceipt> live = _liveReceipts->GetValue\(receipt, \[&\] \(std::any _\)\s*\{\s*throw InvalidOperationException\(\);\s*\}\);'
$s=[regex]::Replace($s,$pat,'std::shared_ptr<LiveReceipt> live = _liveReceipts.at(receipt);')
[IO.File]::WriteAllText($p,$s)
$p=Join-Path $root 'DomainHypercallRuntimeExecutor.cpp'; $s=[IO.File]::ReadAllText($p); $line='_liveReceipts.emplace(receipt, std::make_shared<LiveReceipt>(e2, restoreOwner, lifecycleGate));'; if(-not $s.Contains('it->first.expired()')) { $s=$s.Replace($line,"for (auto it = _liveReceipts.begin(); it != _liveReceipts.end();) { if (it->first.expired()) it = _liveReceipts.erase(it); else ++it; }`r`n                $line") }; [IO.File]::WriteAllText($p,$s)
