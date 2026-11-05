$p = 'C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Execution\ExternalAccelerators\ExternalAcceleratorFeatureSwitch.h'
$s = [IO.File]::ReadAllText($p)
$s = $s.Replace('#pragma once', "#pragma once`r`n#include <memory>")
$def = @'
	struct ExternalAcceleratorFeatureSwitch final {
		bool SubmitAdmissionEnabled = true;
		bool BackendExecutionEnabled = true;
		bool ParserAndCarrierValidationEnabled() const noexcept { return true; }
		bool GuardValidationEnabled() const noexcept { return true; }
		bool TokenObservationEnabled() const noexcept { return true; }
		bool TelemetryEvidenceEnabled() const noexcept { return true; }
		static const std::shared_ptr<ExternalAcceleratorFeatureSwitch> Enabled;
		static const std::shared_ptr<ExternalAcceleratorFeatureSwitch> SubmitAndBackendExecutionDisabled;
		static const std::shared_ptr<ExternalAcceleratorFeatureSwitch> BackendExecutionDisabled;
	};
	inline const std::shared_ptr<ExternalAcceleratorFeatureSwitch> ExternalAcceleratorFeatureSwitch::Enabled = std::make_shared<ExternalAcceleratorFeatureSwitch>();
	inline const std::shared_ptr<ExternalAcceleratorFeatureSwitch> ExternalAcceleratorFeatureSwitch::SubmitAndBackendExecutionDisabled = std::make_shared<ExternalAcceleratorFeatureSwitch>(false, false);
	inline const std::shared_ptr<ExternalAcceleratorFeatureSwitch> ExternalAcceleratorFeatureSwitch::BackendExecutionDisabled = std::make_shared<ExternalAcceleratorFeatureSwitch>(true, false);
'@
$pos = $s.LastIndexOf('}')
if ($pos -lt 0 -or $s.Contains('struct ExternalAcceleratorFeatureSwitch final')) { throw 'Feature switch boundary invalid' }
$s = $s.Insert($pos, $def + "`r`n")
[IO.File]::WriteAllText($p, $s)
