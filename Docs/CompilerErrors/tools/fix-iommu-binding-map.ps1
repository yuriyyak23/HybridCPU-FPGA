$p='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Runtime\Domains\Descriptors\IoDomain\IoVirtualizationBlock.cpp'; $s=[IO.File]::ReadAllText($p)
$s=$s.Replace('if (!effective->IsValid)','if (!effective || !effective->isValid())').Replace('_bindings[(effective->IoDomainTag, effective->DomainTag, effective->DeviceId)]','_bindings[std::make_tuple(effective->IoDomainTag, effective->DomainTag, effective->DeviceId)]')
$pat='(?s)bool IoVirtualizationBlock::UnbindDomain\(unsigned short ioDomainTag, unsigned long long domainTag, unsigned int deviceId\)\s*\{.*?\r?\n\t\}\s*(?=bool IoVirtualizationBlock::TryResolveBinding)'
$rep=@"
bool IoVirtualizationBlock::UnbindDomain(unsigned short ioDomainTag, unsigned long long domainTag, unsigned int deviceId)
    {
        auto it = _bindings.find(std::make_tuple(ioDomainTag, domainTag, deviceId));
        if (it == _bindings.end()) return false;
        auto binding = it->second;
        _bindings.erase(it);
        _backend->UnbindDomain(binding->IoDomainTag, binding->DomainId, binding->DeviceId);
        AdvanceOwnershipEpoch();
        return true;
    }

"@; $t=[regex]::Replace($s,$pat,$rep); if($t -eq $s){throw 'unbind not found'}; $s=$t
$pat='(?s)bool IoVirtualizationBlock::TryResolveBinding\(unsigned short ioDomainTag, unsigned long long domainTag, unsigned int deviceId, std::shared_ptr<IommuDomainBinding> &binding\)\s*\{.*?\r?\n\t\}\s*(?=bool IoVirtualizationBlock::TryResolveDescriptorBinding)'
$rep=@"
bool IoVirtualizationBlock::TryResolveBinding(unsigned short ioDomainTag, unsigned long long domainTag, unsigned int deviceId, std::shared_ptr<IommuDomainBinding> &binding)
    {
        auto it = _bindings.find(std::make_tuple(ioDomainTag, domainTag, deviceId));
        if (it == _bindings.end()) { binding.reset(); return false; }
        binding = it->second;
        return true;
    }

"@; $t=[regex]::Replace($s,$pat,$rep); if($t -eq $s){throw 'lookup not found'}; [IO.File]::WriteAllText($p,$t)
