$p = 'C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Execution\ExternalAccelerators\Capabilities\AcceleratorCapabilityRegistry.cpp'
$s = [IO.File]::ReadAllText($p)
$s = $s.Replace('#include "AcceleratorCapabilityRegistry.h"', "#include `"AcceleratorCapabilityRegistry.h`"`r`n#include `"../Telemetry/AcceleratorTelemetry.h`"")
$s = $s.Replace('ArgumentNullException::ThrowIfNull(provider);', 'if (!provider) throw std::invalid_argument("provider");')
$s = $s.Replace('ArgumentNullException::ThrowIfNull(capabilities);', '')
$s = $s.Replace('ArgumentNullException::ThrowIfNull(descriptor);', 'if (!descriptor) throw std::invalid_argument("descriptor");')
$s = $s.Replace('!_descriptors.TryAdd(descriptor->getAcceleratorId(), descriptor)', '!_descriptors.emplace(descriptor->getAcceleratorId(), descriptor).second')
$s = $s.Replace('throw InvalidOperationException(std::format("Accelerator capability metadata for ''{0:s}'' is already registered.", descriptor->getAcceleratorId()));', 'throw std::logic_error("Accelerator capability metadata is already registered.");')
$old = @'
		std::unordered_map<std::wstring, AcceleratorCapabilityDescriptor*>::const_iterator _descriptors_iterator = _descriptors.find(acceleratorId);
		auto temp_var = _descriptors_iterator != _descriptors.end();
		descriptor = _descriptors_iterator->second;
		return temp_var;
'@
$new = @'
		auto it = _descriptors.find(acceleratorId);
		if (it == _descriptors.end()) return false;
		descriptor = it->second;
		return true;
'@
$old = $old -replace "`r?`n", "`r`n"
$new = $new -replace "`r?`n", "`r`n"
if (!$s.Contains($old)) { throw 'Lookup body not found' }
$s = $s.Replace($old, $new)
$s = $s.Replace('return _descriptors.Values->ToArray();', "std::vector<std::shared_ptr<AcceleratorCapabilityDescriptor>> result;`r`n`t`tresult.reserve(_descriptors.size());`r`n`t`tfor (const auto& [id, descriptor] : _descriptors) result.push_back(descriptor);`r`n`t`treturn result;")
[IO.File]::WriteAllText($p, $s)
