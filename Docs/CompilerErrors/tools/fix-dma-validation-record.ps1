$p = 'C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Execution\DmaStreamCompute\DmaStreamComputeValidationResult.h'
$s = [IO.File]::ReadAllText($p)
$s = $s.Replace('#pragma once', "#pragma once`r`n#include <memory>`r`n#include <stdexcept>`r`n#include <string>`r`n#include <utility>")
$insert = @'

	class DmaStreamComputeDescriptor;
	class DmaStreamComputeValidationResult final {
	public:
		const DmaStreamComputeValidationFault Fault;
		const std::shared_ptr<DmaStreamComputeDescriptor> Descriptor;
		const std::wstring Message;
		const bool IsValid;
		const bool IsDescriptorAbiAccepted;
	private:
		DmaStreamComputeValidationResult(DmaStreamComputeValidationFault fault, std::shared_ptr<DmaStreamComputeDescriptor> descriptor, std::wstring message)
			: Fault(fault), Descriptor(std::move(descriptor)), Message(std::move(message)), IsValid(Fault == DmaStreamComputeValidationFault::None && Descriptor != nullptr), IsDescriptorAbiAccepted(IsValid) {}
	public:
		static std::shared_ptr<DmaStreamComputeValidationResult> Valid(std::shared_ptr<DmaStreamComputeDescriptor> descriptor) {
			if (!descriptor) throw std::invalid_argument("descriptor");
			return std::shared_ptr<DmaStreamComputeValidationResult>(new DmaStreamComputeValidationResult(DmaStreamComputeValidationFault::None, std::move(descriptor), L"Descriptor accepted after owner/domain guard; execution remains disabled."));
		}
		static std::shared_ptr<DmaStreamComputeValidationResult> Fail(DmaStreamComputeValidationFault fault, const std::wstring& message) {
			if (fault == DmaStreamComputeValidationFault::None) throw std::invalid_argument("Use Valid for successful descriptor validation.");
			return std::shared_ptr<DmaStreamComputeValidationResult>(new DmaStreamComputeValidationResult(fault, nullptr, message));
		}
		std::shared_ptr<DmaStreamComputeDescriptor> RequireDescriptorForAdmission() const {
			if (IsDescriptorAbiAccepted && Descriptor) return Descriptor;
			throw std::logic_error("DmaStreamCompute descriptor is not admissible.");
		}
	};
'@
$pos = $s.LastIndexOf('}')
if ($pos -lt 0 -or $s.Contains('class DmaStreamComputeValidationResult final')) { throw 'Validation record boundary invalid or already converted' }
$s = $s.Insert($pos, $insert + "`r`n")
[IO.File]::WriteAllText($p, $s)
