$root='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Runtime\Migration\Checkpoint'
$p=Join-Path $root 'DomainHypercallLifecycleGate.h'; $s=[IO.File]::ReadAllText($p).Replace('#include <mutex>',"#include <mutex>`r`n#include <condition_variable>`r`n#include <chrono>"); $s=$s.Replace('std::mutex _sync;','std::mutex _sync; std::condition_variable _quiescent;').Replace('WaitForTransitionQuiescence(TimeSpan timeout)','WaitForTransitionQuiescence(std::chrono::milliseconds timeout)'); [IO.File]::WriteAllText($p,$s)
$p=Join-Path $root 'DomainHypercallLifecycleGate.cpp'; $s=[IO.File]::ReadAllText($p); $pat='(?s)bool DomainHypercallLifecycleGate::WaitForTransitionQuiescence\(TimeSpan timeout\)\s*\{.*?\r?\n\t\}\s*(?=bool DomainHypercallLifecycleGate::ResumeAfterValidatedRestore)'; $rep=@"
bool DomainHypercallLifecycleGate::WaitForTransitionQuiescence(std::chrono::milliseconds timeout)
    {
        constexpr auto infinite = std::chrono::milliseconds(-1);
        if (timeout < std::chrono::milliseconds::zero() && timeout != infinite)
            throw std::invalid_argument("timeout");
        std::unique_lock<std::mutex> lock(_sync);
        auto ready = [this] { return _transitionsInFlight == 0; };
        if (timeout == infinite) { _quiescent.wait(lock, ready); return true; }
        return _quiescent.wait_for(lock, timeout, ready);
    }

"@; $t=[regex]::Replace($s,$pat,$rep); if($t -eq $s){throw 'wait block missing'}; $s=$t.Replace('Monitor::PulseAll(_sync);','_quiescent.notify_all();'); [IO.File]::WriteAllText($p,$s)
$p=Join-Path $root 'DomainHypercallDrainLifecycleOwner.h'; $s=[IO.File]::ReadAllText($p).Replace('std::optional<TimeSpan> &transitionTimeout = std::nullopt','std::optional<std::chrono::milliseconds> transitionTimeout = std::nullopt'); [IO.File]::WriteAllText($p,$s)
$p=Join-Path $root 'DomainHypercallDrainLifecycleOwner.cpp'; $s=[IO.File]::ReadAllText($p).Replace('std::optional<TimeSpan> &transitionTimeout','std::optional<std::chrono::milliseconds> transitionTimeout').Replace('transitionTimeout ? transitionTimeout : TimeSpan::FromSeconds(30)','transitionTimeout.value_or(std::chrono::seconds(30))'); [IO.File]::WriteAllText($p,$s)
