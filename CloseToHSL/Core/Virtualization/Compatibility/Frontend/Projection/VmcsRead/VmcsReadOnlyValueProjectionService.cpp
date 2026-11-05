#include "VmcsReadOnlyValueProjectionService.h"
#include "../../../../../Runtime/Domains/Descriptors/ExecutionDomain/YAKSys_Hybrid_CPU.Core.ExecutionDomainDescriptor.h"
#include "../../../../../Runtime/Domains/Descriptors/MemoryDomain/YAKSys_Hybrid_CPU.Core.MemoryDomainDescriptor.h"
#include "../../../../../Runtime/Domains/SecureCompute/Descriptors/ExecutionState/PrivilegedExecutionStateDescriptor.h"
#include "../../../Generated/VmcsProjection/VmcsFieldProjectionSchema.h"
#include "../../../../../Runtime/Domains/Descriptors/EvidencePolicy/YAKSys_Hybrid_CPU.Core.EvidencePolicyDescriptor.h"
#include "../../../Generated/VmcsProjection/VmcsFieldAliasProjection.h"
#include "../../../../../System/Vmcs/V2/VmcsV2FieldModel.h"

using namespace YAKSys_Hybrid_CPU::Core::Vmcs::V2;

namespace YAKSys_Hybrid_CPU::Core
{

	namespace { std::wstring FieldName(VmcsField value) { switch(value) {
case VmcsField::GuestPc: return L"GuestPc";
case VmcsField::GuestSp: return L"GuestSp";
case VmcsField::GuestFlags: return L"GuestFlags";
case VmcsField::GuestCr0: return L"GuestCr0";
case VmcsField::GuestCr3: return L"GuestCr3";
case VmcsField::GuestCr4: return L"GuestCr4";
case VmcsField::HostPc: return L"HostPc";
case VmcsField::HostSp: return L"HostSp";
case VmcsField::HostFlags: return L"HostFlags";
case VmcsField::HostCr0: return L"HostCr0";
case VmcsField::HostCr3: return L"HostCr3";
case VmcsField::PinBasedControls: return L"PinBasedControls";
case VmcsField::ProcBasedControls: return L"ProcBasedControls";
case VmcsField::ExitControls: return L"ExitControls";
case VmcsField::EntryControls: return L"EntryControls";
case VmcsField::EptPointer: return L"EptPointer";
case VmcsField::Vpid: return L"Vpid";
case VmcsField::SecondaryProcControls: return L"SecondaryProcControls";
case VmcsField::Cr3TargetCount: return L"Cr3TargetCount";
case VmcsField::ExitReason: return L"ExitReason";
case VmcsField::ExitQualification: return L"ExitQualification";
case VmcsField::GuestPhysicalAddress: return L"GuestPhysicalAddress";
case VmcsField::EptViolationQualification: return L"EptViolationQualification";
 default: return std::to_wstring(static_cast<int>(value)); } } 
std::wstring OwnerName(VmcsFieldProjectionOwner value) { switch(value) {
 case VmcsFieldProjectionOwner::ExecutionDomainDescriptor: return L"ExecutionDomainDescriptor";
 case VmcsFieldProjectionOwner::MemoryDomainDescriptor: return L"MemoryDomainDescriptor";
 case VmcsFieldProjectionOwner::CompletionRecord: return L"CompletionRecord";
 case VmcsFieldProjectionOwner::CompatibilityControlDescriptor: return L"CompatibilityControlDescriptor";
 default: return std::to_wstring(static_cast<int>(value)); } } }

	VmcsReadOnlyValueProjectionService::VmcsReadOnlyValueProjectionService() : VmcsReadOnlyValueProjectionService(std::make_shared<VmcsFieldAliasProjection>(), std::make_shared<CompletionProjectionService>(), std::make_shared<PrivilegedExecutionStateProjectionService>())
	{
	}

	VmcsReadOnlyValueProjectionService::VmcsReadOnlyValueProjectionService(std::shared_ptr<VmcsFieldAliasProjection> aliasProjection, std::shared_ptr<CompletionProjectionService> completionProjection) : VmcsReadOnlyValueProjectionService(aliasProjection, completionProjection, std::make_shared<PrivilegedExecutionStateProjectionService>())
	{
	}

// C# TO C++ CONVERTER TASK: Throw expressions are not converted by C# to C++ Converter:
// ORIGINAL LINE: public VmcsReadOnlyValueProjectionService(VmcsFieldAliasProjection aliasProjection, CompletionProjectionService completionProjection, PrivilegedExecutionStateProjectionService privilegedProjection)
	VmcsReadOnlyValueProjectionService::VmcsReadOnlyValueProjectionService(std::shared_ptr<VmcsFieldAliasProjection> aliasProjection, std::shared_ptr<CompletionProjectionService> completionProjection, std::shared_ptr<PrivilegedExecutionStateProjectionService> privilegedProjection) : _aliasProjection((aliasProjection != nullptr) ? aliasProjection : throw std::invalid_argument("aliasProjection")), _completionProjection((completionProjection != nullptr) ? completionProjection : throw std::invalid_argument("completionProjection")), _privilegedProjection((privilegedProjection != nullptr) ? privilegedProjection : throw std::invalid_argument("privilegedProjection"))
	{
	}

	std::shared_ptr<VmcsReadOnlyValueProjectionResult> VmcsReadOnlyValueProjectionService::Project(std::shared_ptr<VmcsReadOnlyValueProjectionRequest> request)
	{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: VmcsField field = unchecked((VmcsField)request.FieldId);
		VmcsField field = static_cast<VmcsField>(request->FieldId);

		if (!request->RuntimeAdmission->IsAllowed)
		{
			return Denied(VmcsReadOnlyValueProjectionDecision::RuntimeAdmissionDenied, field, nullptr, nullptr, request->FieldId, L"Generated read-only VMREAD value projection requires runtime boundary admission first.");
		}

		std::shared_ptr<VmcsFieldProjectionSchemaEntry> entry;
		if (!VmcsFieldProjectionSchema::TryGet(field, entry))
		{
			return Denied(VmcsReadOnlyValueProjectionDecision::UnknownFieldDenied, field, nullptr, nullptr, request->FieldId, L"VMREAD field is not part of the generated VMCS field projection schema.");
		}

		if (!VmcsFieldProjectionSchema::CanRead(entry))
		{
			return Denied(VmcsReadOnlyValueProjectionDecision::SchemaReadDenied, field, entry, nullptr, request->FieldId, L"Generated VMCS field projection schema denies read access for this field.");
		}

		if (entry->Owner == VmcsFieldProjectionOwner::CompatibilityControlDescriptor)
		{
			return Denied(VmcsReadOnlyValueProjectionDecision::CompatibilityControlValueProjectionDenied, field, entry, nullptr, request->FieldId, L"Compatibility-control VMREAD values are intentionally denied: the neutral CompatibilityControlDescriptor exposes fail-closed semantics, but no frozen VMX control-bit value projection contract is admitted in this slice.");
		}

// C# TO C++ CONVERTER TASK: The following 'is' operator pattern is not converted by C# to C++ Converter:
		if (entry->Owner != VmcsFieldProjectionOwner::CompletionRecord && entry->Owner != VmcsFieldProjectionOwner::ExecutionDomainDescriptor && entry->Owner != VmcsFieldProjectionOwner::MemoryDomainDescriptor)
		{
			return Denied(VmcsReadOnlyValueProjectionDecision::NeutralOwnerValueSourceMissing, field, entry, nullptr, request->FieldId, L"Generated VMREAD field '" + FieldName(entry->Field) + L"' owner '" + OwnerName(entry->Owner) + L"' has no admitted neutral read-only value source in this slice.");
		}

		std::shared_ptr<VmcsFieldAliasResult> alias = _aliasProjection->ValidateAccess(std::make_shared<VmcsFieldAliasRequest>(entry->Field, VmcsFieldAliasAccess::Read, entry->EvidenceClass, entry->IsGeneratedAlias, request->DescriptorValidated, false), (request->EvidencePolicy != nullptr) ? request->EvidencePolicy : EvidencePolicyDescriptor::getFailClosed());

		if (!alias->IsAllowed)
		{
			return Denied(VmcsReadOnlyValueProjectionDecision::AliasAccessDenied, field, entry, alias, request->FieldId, alias->Reason);
		}

	switch (entry->Owner) {
        case VmcsFieldProjectionOwner::CompletionRecord: return ProjectCompletionOwnedValue(request, field, entry, alias);
        case VmcsFieldProjectionOwner::ExecutionDomainDescriptor: return ProjectExecutionOwnedValue(request, field, entry, alias);
        case VmcsFieldProjectionOwner::MemoryDomainDescriptor: return ProjectMemoryOwnedValue(request, field, entry, alias);
        default: return Denied(VmcsReadOnlyValueProjectionDecision::NeutralOwnerValueSourceMissing, field, entry, alias, request->FieldId, L"Generated VMREAD field '" + FieldName(entry->Field) + L"' owner '" + OwnerName(entry->Owner) + L"' has no admitted neutral read-only value source in this slice.");
        }
	}

	std::shared_ptr<VmcsReadOnlyValueProjectionResult> VmcsReadOnlyValueProjectionService::ProjectCompletionOwnedValue(std::shared_ptr<VmcsReadOnlyValueProjectionRequest> request, VmcsField field, std::shared_ptr<VmcsFieldProjectionSchemaEntry> entry, std::shared_ptr<VmcsFieldAliasResult> alias)
	{
		if (request->Completion == nullptr)
		{
			return Denied(VmcsReadOnlyValueProjectionDecision::CompletionSourceMissing, field, entry, alias, request->FieldId, L"Completion-owned VMREAD value projection requires a neutral CompletionRecord source.");
		}

		if (!_completionProjection->CanProjectToVmx(request->Completion))
		{
			return Denied(VmcsReadOnlyValueProjectionDecision::CompletionSourceDenied, field, entry, alias, request->FieldId, L"CompletionRecord is not an admitted compatibility projection source for VMREAD value projection.");
		}

		std::shared_ptr<VmxCompletionProjection> projection = _completionProjection->ProjectToVmx(request->Completion);
		long long temp_var180;
		switch (entry->Field)
		{
			case VmcsField::ExitReason:
				temp_var180 = static_cast<long long>(projection->ExitReason);
				break;
			case VmcsField::ExitQualification:
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: temp_var180 = unchecked((long)projection.ExitQualification);
				temp_var180 = static_cast<long long>(projection->ExitQualification);
				break;
			case VmcsField::GuestPhysicalAddress:
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: temp_var180 = unchecked((long)projection.GuestPhysicalAddress);
				temp_var180 = static_cast<long long>(projection->GuestPhysicalAddress);
				break;
			case VmcsField::EptViolationQualification:
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: temp_var180 = unchecked((long)projection.EptViolationQualification);
				temp_var180 = static_cast<long long>(projection->EptViolationQualification);
				break;
			default:
				temp_var180 = 0;
				break;
		}
		long long value = temp_var180;

// C# TO C++ CONVERTER TASK: The following 'is' operator pattern is not converted by C# to C++ Converter:
		if (entry->Field != VmcsField::ExitReason && entry->Field != VmcsField::ExitQualification && entry->Field != VmcsField::GuestPhysicalAddress && entry->Field != VmcsField::EptViolationQualification)
		{
			return Denied(VmcsReadOnlyValueProjectionDecision::NeutralOwnerValueSourceMissing, field, entry, alias, request->FieldId, L"CompletionRecord does not define a VMREAD value projection for field '" + FieldName(entry->Field) + L"'.");
		}

		return std::make_shared<VmcsReadOnlyValueProjectionResult>(VmcsReadOnlyValueProjectionDecision::ReadOnlyValueProjected, field, entry, alias, VmcsV2ValidationResult::Success(request->FieldId), value, L"Generated read-only VMREAD value came from the neutral CompletionRecord owner.");
	}

	std::shared_ptr<VmcsReadOnlyValueProjectionResult> VmcsReadOnlyValueProjectionService::ProjectMemoryOwnedValue(std::shared_ptr<VmcsReadOnlyValueProjectionRequest> request, VmcsField field, std::shared_ptr<VmcsFieldProjectionSchemaEntry> entry, std::shared_ptr<VmcsFieldAliasResult> alias)
	{
		if (entry->Field == VmcsField::HostCr3)
		{
			return Denied(VmcsReadOnlyValueProjectionDecision::HostAddressSpaceOwnerMissing, field, entry, alias, request->FieldId, L"HostCr3 compatibility projection remains denied: no neutral host-address-space owner exposes a read-only address-space root source.");
		}

		if (request->Memory == nullptr)
		{
			return Denied(VmcsReadOnlyValueProjectionDecision::MemorySourceMissing, field, entry, alias, request->FieldId, L"Memory-owned VMREAD value projection requires a neutral MemoryDomainDescriptor source.");
		}

		std::shared_ptr<MemoryDomainReadOnlyTranslationView> view;
		std::wstring viewReason;
		if (!request->Memory->TryCreateReadOnlyTranslationView(view, viewReason))
		{
			return Denied(VmcsReadOnlyValueProjectionDecision::MemorySourceDenied, field, entry, alias, request->FieldId, viewReason);
		}

		if (entry->Field == VmcsField::Vpid && (!view->AddressSpaceTaggingEnabled || view->AddressSpaceTag == 0))
		{
			return Denied(VmcsReadOnlyValueProjectionDecision::MemorySourceDenied, field, entry, alias, request->FieldId, L"VPID compatibility projection requires neutral address-space tagging with a non-zero AddressSpaceTag.");
		}

		long long value;
		if (!TryProjectMemoryField(entry->Field, view, value))
		{
			return Denied(VmcsReadOnlyValueProjectionDecision::NeutralOwnerValueSourceMissing, field, entry, alias, request->FieldId, L"MemoryDomainDescriptor does not define a VMREAD value projection for field '" + FieldName(entry->Field) + L"'.");
		}

		return std::make_shared<VmcsReadOnlyValueProjectionResult>(VmcsReadOnlyValueProjectionDecision::ReadOnlyValueProjected, field, entry, alias, VmcsV2ValidationResult::Success(request->FieldId), value, L"Generated read-only VMREAD value came from the neutral MemoryDomainDescriptor translation view.");
	}

	std::shared_ptr<VmcsReadOnlyValueProjectionResult> VmcsReadOnlyValueProjectionService::ProjectExecutionOwnedValue(std::shared_ptr<VmcsReadOnlyValueProjectionRequest> request, VmcsField field, std::shared_ptr<VmcsFieldProjectionSchemaEntry> entry, std::shared_ptr<VmcsFieldAliasResult> alias)
	{
		if (entry->Field == VmcsField::HostPc || entry->Field == VmcsField::HostSp || entry->Field == VmcsField::HostFlags || entry->Field == VmcsField::HostCr0)
		{
			return Denied(VmcsReadOnlyValueProjectionDecision::HostExecutionStateOwnerMissing, field, entry, alias, request->FieldId, L"Host execution-state VMREAD projection remains denied: no neutral host-execution owner exposes read-only host PC/SP/flags/control-register sources.");
		}

		if (entry->Field == VmcsField::GuestCr0 || entry->Field == VmcsField::GuestCr4)
		{
			PrivilegedControlRegisterKind register_Keyword = entry->Field == VmcsField::GuestCr0 ? PrivilegedControlRegisterKind::GuestCr0 : PrivilegedControlRegisterKind::GuestCr4;
			std::shared_ptr<PrivilegedExecutionStateProjectionResult> privilegedProjection = _privilegedProjection->Project(std::make_shared<PrivilegedExecutionStateProjectionRequest>(register_Keyword, request->PrivilegedExecutionState, request->RuntimeDomainTag, request->RuntimeAddressSpaceTag, request->CurrentPrivilegedExecutionStateEpoch, alias->IsAllowed, request->PrivilegedExecutionState && request->PrivilegedExecutionState->MigrationClass == PrivilegedExecutionStateMigrationClass::RevalidatedAfterRestore, request->PrivilegedExecutionStateConformanceProven));

			if (!privilegedProjection->IsAllowed) {
                auto denied = Denied(VmcsReadOnlyValueProjectionDecision::PrivilegedExecutionStateProjectionDenied, field, entry, alias, request->FieldId, privilegedProjection->Reason);
                denied->PrivilegedProjection = privilegedProjection;
                return denied;
            }
            auto projected = std::make_shared<VmcsReadOnlyValueProjectionResult>(VmcsReadOnlyValueProjectionDecision::ReadOnlyValueProjected, field, entry, alias, VmcsV2ValidationResult::Success(request->FieldId), static_cast<long long>(privilegedProjection->Value), L"Guest control-register VMREAD value came from the neutral privileged execution-state owner.");
            projected->PrivilegedProjection = privilegedProjection;
            return projected;
		}

		if (request->Execution == nullptr)
		{
			return Denied(VmcsReadOnlyValueProjectionDecision::ExecutionSourceMissing, field, entry, alias, request->FieldId, L"Execution-owned VMREAD value projection requires a neutral ExecutionDomainDescriptor source.");
		}

		std::shared_ptr<ExecutionDomainReadOnlyStateView> view;
		std::wstring viewReason;
		if (!request->Execution->TryCreateReadOnlyStateView(view, viewReason))
		{
			return Denied(VmcsReadOnlyValueProjectionDecision::ExecutionSourceMissing, field, entry, alias, request->FieldId, viewReason);
		}

		long long value;
		std::wstring missingReason;
		if (!TryProjectExecutionField(entry->Field, view, value, missingReason))
		{
			return Denied(VmcsReadOnlyValueProjectionDecision::ExecutionSourceDenied, field, entry, alias, request->FieldId, missingReason);
		}

		return std::make_shared<VmcsReadOnlyValueProjectionResult>(VmcsReadOnlyValueProjectionDecision::ReadOnlyValueProjected, field, entry, alias, VmcsV2ValidationResult::Success(request->FieldId), value, L"Generated read-only VMREAD value came from the neutral ExecutionDomainDescriptor read-only state view.");
	}

	bool VmcsReadOnlyValueProjectionService::TryProjectExecutionField(VmcsField field, std::shared_ptr<ExecutionDomainReadOnlyStateView> view, long long &value, std::wstring &missingReason) {
        switch (field) {
        case VmcsField::GuestPc: if (view->HasMaterializedGuestPc) { value=static_cast<long long>(view->GuestPc); missingReason=L""; return true; } break;
        case VmcsField::GuestSp: if (view->HasMaterializedGuestSp) { value=static_cast<long long>(view->GuestSp); missingReason=L""; return true; } break;
        case VmcsField::GuestFlags: if (view->HasMaterializedGuestFlags) { value=static_cast<long long>(view->GuestFlags); missingReason=L""; return true; } break;
        default: value=0; missingReason=L"ExecutionDomainDescriptor does not define a VMREAD value projection for field '" + FieldName(field) + L"'."; return false;
        }
        value=0; missingReason=L"ExecutionDomainDescriptor read-only state view does not materialize field '" + FieldName(field) + L"'."; return false;
    }
    bool VmcsReadOnlyValueProjectionService::TryProjectMemoryField(VmcsField field, std::shared_ptr<MemoryDomainReadOnlyTranslationView> view, long long &value) {
        switch (field) {
        case VmcsField::GuestCr3: value=static_cast<long long>(view->AddressSpaceRoot); return true;
        case VmcsField::EptPointer: if (view->OwnsSecondStageTranslation) { value=static_cast<long long>(view->SecondStageRoot); return true; } break;
        case VmcsField::Vpid: value=view->AddressSpaceTag; return true;
        case VmcsField::Cr3TargetCount: value=view->AddressSpaceTargetCount; return true;
        default: break;
        }
        value=0; return false;
    }
	std::shared_ptr<VmcsReadOnlyValueProjectionResult> VmcsReadOnlyValueProjectionService::Denied(VmcsReadOnlyValueProjectionDecision decision, VmcsField field, std::shared_ptr<VmcsFieldProjectionSchemaEntry> entry, std::shared_ptr<VmcsFieldAliasResult> alias, unsigned short fieldId, const std::wstring &reason)
	{
		return std::make_shared<VmcsReadOnlyValueProjectionResult>(decision, field, entry, alias, VmcsV2ValidationResult::Fail(VmcsV2ValidationCode::AccessDenied, fieldId, reason), 0, reason);
	}
}
