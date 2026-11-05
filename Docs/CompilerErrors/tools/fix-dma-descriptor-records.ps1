$p = 'C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Execution\DmaStreamCompute\DmaStreamComputeDescriptor.h'
$s = [IO.File]::ReadAllText($p)
$s = $s.Replace('#include "DmaStreamComputeOwnerBinding.h"', "#include `"DmaStreamComputeOwnerBinding.h`"`r`n#include `"../../Pipeline/Safety/SafetyVerifier.Types.h`"`r`n#include <memory>`r`n#include <string>`r`n#include <vector>`r`n#include <stdexcept>`r`n#include <utility>")
$def = @'

	struct DmaStreamComputeDescriptorReference final {
		unsigned long long DescriptorAddress = 0;
		unsigned int DescriptorSize = 0;
		unsigned long long DescriptorIdentityHash = 0;
		bool operator==(const DmaStreamComputeDescriptorReference&) const = default;
	};
	class DmaStreamComputeMemoryRange final {
	public:
		unsigned long long Address = 0, Length = 0;
		bool operator==(const DmaStreamComputeMemoryRange&) const = default;
	};
	class DmaStreamComputeOwnerGuardDecision final {
	public:
		std::shared_ptr<DmaStreamComputeOwnerBinding> DescriptorOwnerBinding;
		DmaStreamComputeOwnerGuardContext RuntimeOwnerContext;
		YAKSys_Hybrid_CPU::Core::LegalityDecision LegalityDecision;
		std::wstring Message;
		bool IsAllowed;
		DmaStreamComputeOwnerGuardDecision(std::shared_ptr<DmaStreamComputeOwnerBinding> binding, DmaStreamComputeOwnerGuardContext context, YAKSys_Hybrid_CPU::Core::LegalityDecision decision, std::wstring message)
			: DescriptorOwnerBinding(std::move(binding)), RuntimeOwnerContext(context), LegalityDecision(decision), Message(std::move(message)), IsAllowed(LegalityDecision.isAllowed()) {}
		static std::shared_ptr<DmaStreamComputeOwnerGuardDecision> Allow(std::shared_ptr<DmaStreamComputeOwnerBinding> binding, DmaStreamComputeOwnerGuardContext context, const std::wstring& message) {
			if (!binding) throw std::invalid_argument("descriptorOwnerBinding");
			return std::make_shared<DmaStreamComputeOwnerGuardDecision>(std::move(binding), context, YAKSys_Hybrid_CPU::Core::LegalityDecision::Allow(LegalityAuthoritySource::GuardPlane, false), message);
		}
		static std::shared_ptr<DmaStreamComputeOwnerGuardDecision> Reject(std::shared_ptr<DmaStreamComputeOwnerBinding> binding, DmaStreamComputeOwnerGuardContext context, RejectKind reject, const std::wstring& message) {
			if (!binding) throw std::invalid_argument("descriptorOwnerBinding");
			return std::make_shared<DmaStreamComputeOwnerGuardDecision>(std::move(binding), context, YAKSys_Hybrid_CPU::Core::LegalityDecision::Reject(reject, CertificateRejectDetail::None, LegalityAuthoritySource::GuardPlane, false), message);
		}
	};
	class VmxDmaDescriptorValidationEvidence;
	class DmaStreamComputeDescriptor final {
	public:
		static constexpr unsigned int CanonicalLane6DeviceId = 6;
		DmaStreamComputeDescriptorReference DescriptorReference{};
		unsigned short AbiVersion = 0, HeaderSize = 0;
		unsigned int TotalSize = 0;
		unsigned long long DescriptorIdentityHash = 0, CertificateInputHash = 0;
		DmaStreamComputeOperationKind Operation{};
		DmaStreamComputeElementType ElementType{};
		DmaStreamComputeShapeKind Shape{};
		DmaStreamComputeRangeEncoding RangeEncoding{};
		DmaStreamComputePartialCompletionPolicy PartialCompletionPolicy{};
		std::shared_ptr<DmaStreamComputeOwnerBinding> OwnerBinding;
		std::shared_ptr<DmaStreamComputeOwnerGuardDecision> OwnerGuardDecision;
		std::vector<DmaStreamComputeMemoryRange> ReadMemoryRanges, NormalizedReadMemoryRanges, WriteMemoryRanges, NormalizedWriteMemoryRanges;
		DmaStreamComputeAliasPolicy AliasPolicy{};
		unsigned long long NormalizedFootprintHash = 0;
		std::shared_ptr<VmxDmaDescriptorValidationEvidence> VmxValidationEvidence;
	};
'@
$pos = $s.LastIndexOf('}')
if ($pos -lt 0 -or $s.Contains('class DmaStreamComputeDescriptor final')) { throw 'Descriptor record boundary invalid or already converted' }
$s = $s.Insert($pos, $def + "`r`n")
[IO.File]::WriteAllText($p, $s)
