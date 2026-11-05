$dir = 'C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Execution\ExternalAccelerators\Descriptors'
foreach ($name in @('AcceleratorDescriptorParser.h', 'AcceleratorDescriptorParser.cpp')) {
    $p = Join-Path $dir $name
    $s = [IO.File]::ReadAllText($p)
    $s = $s.Replace('ReadOnlySpan<unsigned char>', 'std::span<const unsigned char>')
    $s = $s.Replace('std::optional<std::shared_ptr<AcceleratorDescriptorReference>> &descriptorReference', 'std::optional<std::shared_ptr<AcceleratorDescriptorReference>> descriptorReference')
    if ($name -eq 'AcceleratorDescriptorParser.h') {
        $s = $s.Replace('#include <vector>', "#include <vector>`r`n#include <span>")
        $needle = "`tclass AcceleratorDescriptorParser final"
        $s = $s.Replace($needle, "`tclass AcceleratorDescriptorStructuralReadResult;`r`n`tclass AcceleratorCarrierValidationResult;`r`n" + $needle)
        $s = $s.Replace('static bool TryAddRangeTableSpan(', "struct RangeTableSpan;`r`n`t`tstatic bool TryAddRangeTableSpan(")
    }
    [IO.File]::WriteAllText($p, $s)
}
