$dir = 'C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Execution\ExternalAccelerators\Tokens'
foreach ($name in @('AcceleratorTokenStore.h', 'AcceleratorTokenStore.cpp')) {
    $p = Join-Path $dir $name
    $s = [IO.File]::ReadAllText($p)
    $s = $s.Replace('std::optional<std::shared_ptr<ExternalAcceleratorFeatureSwitch>> &featureSwitch', 'std::optional<std::shared_ptr<ExternalAcceleratorFeatureSwitch>> featureSwitch')
    if ($name -eq 'AcceleratorTokenStore.h') {
        $s = $s.Replace('#include "AcceleratorToken.h"', "#include `"AcceleratorToken.h`"`r`n#include `"../ExternalAcceleratorFeatureSwitch.h`"")
        $needle = 'namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Tokens'+"`r`n"+'{'
        if (!$s.Contains($needle)) { throw 'Token store namespace boundary not found' }
        $s = $s.Replace($needle, $needle + "`r`n`tclass AcceleratorTokenAdmissionResult; class AcceleratorTokenLookupResult; class AcceleratorWaitPolicy; class AcceleratorCancelPolicy;")
    } else {
        $s = $s.Replace('featureSwitch ? featureSwitch : ExternalAcceleratorFeatureSwitch->Enabled', 'featureSwitch.value_or(ExternalAcceleratorFeatureSwitch::Enabled)')
    }
    [IO.File]::WriteAllText($p, $s)
}
