$dir = 'C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Execution\ExternalAccelerators\Tokens'
$p = Join-Path $dir 'AcceleratorTokenHandle.h'
$s = [IO.File]::ReadAllText($p)
$s = $s.Replace('#pragma once', "#pragma once`r`n#include <memory>`r`n#include <stdexcept>`r`n#include <string>`r`n#include <format>")
$def = @'
	struct AcceleratorTokenHandle final {
		const unsigned long long Value;
		const bool IsValid;
		explicit AcceleratorTokenHandle(unsigned long long value = 0) : Value(value), IsValid(value != 0) {}
		static std::shared_ptr<AcceleratorTokenHandle> FromOpaqueValue(unsigned long long value) {
			if (!value) throw std::out_of_range("L7-SDC accelerator token handle value zero is invalid.");
			return std::make_shared<AcceleratorTokenHandle>(value);
		}
		static std::shared_ptr<AcceleratorTokenHandle> Invalid() { return std::make_shared<AcceleratorTokenHandle>(0); }
		std::wstring ToString() const { return IsValid ? std::format(L"0x{:016X}", Value) : L"invalid"; }
		bool operator==(const AcceleratorTokenHandle& other) const noexcept { return Value == other.Value; }
	};
'@
$pos = $s.IndexOf('{', $s.IndexOf('namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Tokens'))
if ($pos -lt 0 -or $s.Contains('struct AcceleratorTokenHandle final')) { throw 'Token handle boundary invalid' }
$s = $s.Insert($pos + 1, "`r`n" + $def + "`r`n")
[IO.File]::WriteAllText($p, $s)
$p = Join-Path $dir 'AcceleratorToken.h'
$s = [IO.File]::ReadAllText($p)
$s = $s.Replace('#include "../Capabilities/AcceleratorCapabilityRegistry.h"', '')
$s = $s.Replace('#include "../Telemetry/AcceleratorTelemetry.h"', '')
$s = $s.Replace('#include "AcceleratorTokenStatusWord.h"', "#include `"AcceleratorTokenStatusWord.h`"`r`n#include `"AcceleratorTokenHandle.h`"")
$decl = @'
namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Auth { struct AcceleratorMappingEpoch; struct AcceleratorIommuDomainEpoch; class AcceleratorGuardDecision; class AcceleratorGuardEvidence; }
namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Capabilities { class AcceleratorCapabilityAcceptanceResult; }
namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Descriptors { class AcceleratorCommandDescriptor; }
namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Telemetry { class AcceleratorTelemetry; }
namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Tokens { struct AcceleratorTokenStatusWord; class AcceleratorTokenTransition; }
'@
$s = $s.Replace('using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Auth;', $decl + "`r`nusing namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Auth;")
[IO.File]::WriteAllText($p, $s)
$p = Join-Path $dir 'AcceleratorToken.cpp'
$s = [IO.File]::ReadAllText($p)
$s = $s.Replace('#include "AcceleratorToken.h"', "#include `"AcceleratorToken.h`"`r`n#include `"../Capabilities/AcceleratorCapabilityRegistry.h`"`r`n#include `"../Telemetry/AcceleratorTelemetry.h`"")
[IO.File]::WriteAllText($p, $s)
