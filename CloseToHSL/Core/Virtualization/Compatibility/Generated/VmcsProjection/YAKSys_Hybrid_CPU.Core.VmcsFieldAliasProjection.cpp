#include "YAKSys_Hybrid_CPU.Core.VmcsFieldAliasProjection.h"
#include "../../FrozenAbi/VmcsFieldAliases/VmcsField.h"
#include "VmcsFieldAliasProjection.h"

namespace YAKSys_Hybrid_CPU::Core
{

	namespace { bool IsDefinedVmcsField(VmcsField field) { switch (field) {
        case VmcsField::GuestPc: return true;
        case VmcsField::GuestSp: return true;
        case VmcsField::GuestFlags: return true;
        case VmcsField::GuestCr0: return true;
        case VmcsField::GuestCr3: return true;
        case VmcsField::GuestCr4: return true;
        case VmcsField::HostPc: return true;
        case VmcsField::HostSp: return true;
        case VmcsField::HostFlags: return true;
        case VmcsField::HostCr0: return true;
        case VmcsField::HostCr3: return true;
        case VmcsField::PinBasedControls: return true;
        case VmcsField::ProcBasedControls: return true;
        case VmcsField::ExitControls: return true;
        case VmcsField::EntryControls: return true;
        case VmcsField::EptPointer: return true;
        case VmcsField::Vpid: return true;
        case VmcsField::SecondaryProcControls: return true;
        case VmcsField::Cr3TargetCount: return true;
        case VmcsField::ExitReason: return true;
        case VmcsField::ExitQualification: return true;
        case VmcsField::GuestPhysicalAddress: return true;
        case VmcsField::EptViolationQualification: return true;
        default: return false;
    } } }

	std::shared_ptr<VmcsFieldAliasResult> VmcsFieldAliasProjection::ValidateAccess(std::shared_ptr<VmcsFieldAliasRequest> request, std::shared_ptr<EvidencePolicyDescriptor> evidencePolicy)
	{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'typeof' operator:
		if (!IsDefinedVmcsField(request->Field))
		{
			return VmcsFieldAliasResult::Denied(VmcsFieldAliasDecision::UndefinedField, request->Field, L"VMCS field is not part of the frozen field alias ABI.");
		}

		if (!request->GeneratedAliasDeclared)
		{
			return VmcsFieldAliasResult::Denied(VmcsFieldAliasDecision::MissingGeneratedAlias, request->Field, L"VMCS field does not have a generated descriptor alias.");
		}

		if (!request->DescriptorValidated)
		{
			return VmcsFieldAliasResult::Denied(VmcsFieldAliasDecision::DescriptorValidationDenied, request->Field, L"VMCS field alias access requires descriptor validation.");
		}

		if (evidencePolicy->MustRecomputeAfterRestore(request->EvidenceClass))
		{
			return VmcsFieldAliasResult::Denied(VmcsFieldAliasDecision::HostOwnedEvidenceDenied, request->Field, L"VMCS field alias access cannot expose host-owned evidence.");
		}

		if (!evidencePolicy->CanExposeToGuest(request->EvidenceClass))
		{
			return VmcsFieldAliasResult::Denied(VmcsFieldAliasDecision::EvidencePolicyDenied, request->Field, L"Evidence policy does not permit guest-visible VMCS alias access.");
		}

		if (request->Access == VmcsFieldAliasAccess::Write)
		{
			return VmcsFieldAliasResult::Denied(VmcsFieldAliasDecision::WriteDenied, request->Field, L"VMCS field alias write authority was removed; compatibility projection is read-only.");
		}

		return VmcsFieldAliasResult::Allowed(request->Field);
	}
}
