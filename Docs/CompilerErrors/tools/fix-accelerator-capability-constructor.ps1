$dir = 'C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Execution\ExternalAccelerators\Capabilities'
foreach ($name in @('AcceleratorCapabilityDescriptor.h','AcceleratorCapabilityDescriptor.cpp')) {
    $p = Join-Path $dir $name
    $s = [IO.File]::ReadAllText($p)
    $s = $s.Replace('std::optional<std::shared_ptr<AcceleratorDescriptorTaxonomyKey>> &taxonomyKey', 'std::optional<std::shared_ptr<AcceleratorDescriptorTaxonomyKey>> taxonomyKey')
    $s = $s.Replace('std::optional<AcceleratorDescriptorTaxonomyStatus> &taxonomyStatus', 'std::optional<AcceleratorDescriptorTaxonomyStatus> taxonomyStatus')
    if ($name -eq 'AcceleratorCapabilityDescriptor.h') {
        $s = $s.Replace('const std::shared_ptr<AcceleratorDescriptorTaxonomyKey> TaxonomyKey;', 'std::shared_ptr<AcceleratorDescriptorTaxonomyKey> TaxonomyKey;')
        $s = $s.Replace('const AcceleratorDescriptorTaxonomyStatus TaxonomyStatus;', 'AcceleratorDescriptorTaxonomyStatus TaxonomyStatus;')
        $s = $s.Replace('const std::shared_ptr<AcceleratorDescriptorTaxonomyEntry> TaxonomyEntry;', 'std::shared_ptr<AcceleratorDescriptorTaxonomyEntry> TaxonomyEntry;')
    } else {
        $s = $s.Replace('std::optional<AcceleratorDescriptorTaxonomyStatus> taxonomyStatus)' + "`r`n`t{", 'std::optional<AcceleratorDescriptorTaxonomyStatus> taxonomyStatus)' + "`r`n`t`t: AcceleratorId(acceleratorId), DisplayName(displayName), CapabilityVersion(capabilityVersion), Operations(operations), ResourceModel(resourceModel), AdoptionMode(adoptionMode), CompatibilityMode(compatibilityMode) `r`n`t{")
        $s = $s.Replace('ArgumentNullException::ThrowIfNull(operations);', '')
        $s = $s.Replace('ArgumentNullException::ThrowIfNull(resourceModel);', 'if (!resourceModel) throw std::invalid_argument("resourceModel");')
        $s = $s.Replace('taxonomyKey ? taxonomyKey : AcceleratorDescriptorTaxonomyCatalog::getCurrentMatMul()->getKey()', 'taxonomyKey.value_or(AcceleratorDescriptorTaxonomyCatalog::getCurrentMatMul()->getKey())')
        $s = $s.Replace('taxonomyStatus ? taxonomyStatus : taxonomyEntry->getStatus()', 'taxonomyStatus.value_or(taxonomyEntry->getStatus())')
        foreach ($line in @('AcceleratorId = acceleratorId;', 'DisplayName = displayName;', 'CapabilityVersion = capabilityVersion;', 'Operations = operations.ToArray();', 'ResourceModel = resourceModel;', 'AdoptionMode = adoptionMode;', 'CompatibilityMode = compatibilityMode;')) { $s = $s.Replace($line, '') }
    }
    [IO.File]::WriteAllText($p, $s)
}
