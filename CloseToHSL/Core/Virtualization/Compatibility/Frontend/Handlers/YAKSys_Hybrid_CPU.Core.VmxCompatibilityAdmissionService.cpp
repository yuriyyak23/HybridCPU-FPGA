#include "YAKSys_Hybrid_CPU.Core.VmxCompatibilityAdmissionService.h"
#include "../Retire/VmxOperationKind.h"
#include "../Decode/VmxFrozenOpcodeValues.h"
#include "../../../../Runtime/Domains/Services/YAKSys_Hybrid_CPU.Core.DomainRuntimeContext.h"
#include "../../Generated/AliasMaps/CompatAliasMap.h"
#include "../../FrozenAbi/VmcsFieldAliases/VmcsField.h"
#include "../../../../Runtime/Domains/Services/YAKSys_Hybrid_CPU.Core.DomainRuntimeOperation.h"
#include "../../../../Runtime/Domains/Descriptors/EvidencePolicy/EvidencePolicyDescriptor.h"
#include "../../../../Runtime/Domains/Services/DomainRuntimeOperation.h"
#include "../../../../Runtime/Completion/Routing/TrapCompletionRoutePolicy.h"
#include "../../../../Runtime/Events/Traps/TrapPolicyBitmap.h"
#include "../Projection/Events/VmxTrapProjectionMapper.h"
#include "../../../../Runtime/Events/Hypercalls/HypercallBackendAdmissionPolicy.h"

using namespace YAKSys_Hybrid_CPU::Core::Vmcs::V2;

namespace YAKSys_Hybrid_CPU::Core
{

	VmxCompatibilityAdmissionService::VmxCompatibilityAdmissionService() : VmxCompatibilityAdmissionService(std::make_shared<VmxCompatDecodeBoundary>(), std::make_shared<VmxCompatProjectionService>(), std::make_shared<RuntimeBoundaryAdmissionService>(), std::make_shared<VmcsReadOnlyValueProjectionService>())
	{
	}

	VmxCompatibilityAdmissionService::VmxCompatibilityAdmissionService(std::shared_ptr<VmxCompatDecodeBoundary> decodeBoundary, std::shared_ptr<VmxCompatProjectionService> projectionService, std::shared_ptr<RuntimeBoundaryAdmissionService> runtimeAdmission, std::shared_ptr<VmcsReadOnlyValueProjectionService> readOnlyValueProjection) : _decodeBoundary(decodeBoundary), _projectionService(projectionService), _runtimeAdmission(runtimeAdmission), _readOnlyValueProjection(readOnlyValueProjection)
	{
	}

	std::shared_ptr<VmxCompatibilityVmReadAdmissionResult> VmxCompatibilityAdmissionService::AdmitVmReadProjection(std::shared_ptr<VmxCompatibilityVmReadAdmissionRequest> request) {
    auto decode = _decodeBoundary->Decode(std::make_shared<VmxCompatDecodeRequest>(VmxFrozenOpcodeValues::VMREAD, request->DestinationRegister, request->FieldSelectorRegister, request->ReservedRegister, request->DescriptorValidated, request->CapabilityValidated, request->SchedulingValidated, request->NoEmissionValidated));
    if (!decode->IsAllowed) return CreateResult(VmxCompatibilityVmReadAdmissionDecision::DecodeDenied,decode,nullptr,nullptr,nullptr,nullptr,0,decode->Reason);
    auto projection = _projectionService->ValidateProjection(std::make_shared<VmxCompatProjectionRequest>(CompatAliasSourceKind::Opcode,L"VMREAD",request->DescriptorValidated,request->ProjectionEvidenceValidated,false));
    if (!projection->getIsAllowed()) return CreateResult(VmxCompatibilityVmReadAdmissionDecision::ProjectionDenied,decode,projection,nullptr,nullptr,nullptr,0,projection->Reason);
    bool exact = request->FieldId == static_cast<unsigned short>(VmcsField::GuestCr0) || request->FieldId == static_cast<unsigned short>(VmcsField::GuestCr4);
    auto admission = _runtimeAdmission->Validate(std::make_shared<RuntimeBoundaryAdmissionRequest>(request->Context,request->RootAuthority,request->EvidencePolicy,DomainRuntimeOperation::FromCompatibilityFrontend(DomainRuntimeOperationKind::ReadCompatibilityProjection,false,true),exact ? DomainBoundaryDescriptor::ExecutionOnly : DomainBoundaryDescriptor::FullDomainRuntime,CapabilityBoundaryRequirement::None,EvidenceBoundaryRequirement::GuestVisible(EvidenceVisibilityClass::CompatibilityAlias)));
    if (!admission->IsAllowed) return CreateResult(VmxCompatibilityVmReadAdmissionDecision::RuntimeAdmissionDenied,decode,projection,admission,nullptr,nullptr,0,admission->Message);
    auto context=request->Context;
    auto value = _readOnlyValueProjection->Project(std::make_shared<VmcsReadOnlyValueProjectionRequest>(request->FieldId,admission,request->EvidencePolicy,request->DescriptorValidated,context ? context->getExecution() : nullptr,context ? context->getMemory() : nullptr,request->Completion,request->PrivilegedExecutionState,context ? context->getDomainTag() : 0,context ? context->getAddressSpaceTag() : 0,request->CurrentPrivilegedExecutionStateEpoch,request->PrivilegedExecutionStateConformanceProven));
    return CreateResult(value->isProjected() ? VmxCompatibilityVmReadAdmissionDecision::ReadOnlyValueProjected : VmxCompatibilityVmReadAdmissionDecision::ReadOnlyProjectionDenied,decode,projection,admission,value->Validation,value,value->Value,value->Reason);
}
std::shared_ptr<VmxCompatibilityVmReadAdmissionResult> VmxCompatibilityAdmissionService::CreateResult(VmxCompatibilityVmReadAdmissionDecision decision, std::shared_ptr<VmxCompatDecodeResult> decode, std::shared_ptr<VmxCompatProjectionResult> projection, std::shared_ptr<RuntimeBoundaryAdmissionResult> admission, std::shared_ptr<VmcsV2ValidationResult> validation, std::shared_ptr<VmcsReadOnlyValueProjectionResult> valueProjection, long long value, const std::wstring &reason) {
    return std::make_shared<VmxCompatibilityVmReadAdmissionResult>(decision,decode,projection,admission,validation,valueProjection,value,reason);
}
std::shared_ptr<VmxCompatibilityTrapAdmissionResult> VmxCompatibilityAdmissionService::AdmitVmCallTrapProjection(std::shared_ptr<VmxCompatibilityVmCallTrapAdmissionRequest> request) {
    auto trapRequest=TrapRequest::ForVmxOperation(VmxOperationKind::VmCall,VmxFrozenOpcodeValues::VMCALL,request->VtId,request->ExecutionDomainTag,request->AddressSpaceTag);
    auto decode=_decodeBoundary->Decode(std::make_shared<VmxCompatDecodeRequest>(VmxFrozenOpcodeValues::VMCALL,0,request->HypercallLeafRegister,request->DescriptorRegister,request->DescriptorValidated,request->CapabilityValidated,request->SchedulingValidated,request->NoEmissionValidated));
    if (!decode->IsAllowed) return CreateTrapResult(VmxCompatibilityTrapAdmissionDecision::DecodeDenied,decode,nullptr,nullptr,NeutralTrapResult::Denied(trapRequest),decode->Reason);
    auto projection=_projectionService->ValidateProjection(std::make_shared<VmxCompatProjectionRequest>(CompatAliasSourceKind::Opcode,L"VMCALL",request->DescriptorValidated,request->ProjectionEvidenceValidated,false));
    if (!projection->getIsAllowed()) return CreateTrapResult(VmxCompatibilityTrapAdmissionDecision::ProjectionDenied,decode,projection,nullptr,NeutralTrapResult::Denied(trapRequest),projection->Reason);
    auto admission=_runtimeAdmission->Validate(std::make_shared<RuntimeBoundaryAdmissionRequest>(request->Context,request->RootAuthority,request->EvidencePolicy,DomainRuntimeOperation::FromCompatibilityFrontend(DomainRuntimeOperationKind::ProjectCompatibilityTrap,false,true),DomainBoundaryDescriptor::FullDomainRuntime,CapabilityBoundaryRequirement::None,EvidenceBoundaryRequirement::GuestVisible(EvidenceVisibilityClass::CompatibilityAlias)));
    if (!admission->IsAllowed) return CreateTrapResult(VmxCompatibilityTrapAdmissionDecision::RuntimeAdmissionDenied,decode,projection,admission,NeutralTrapResult::Denied(trapRequest),admission->Message);
    std::shared_ptr<NeutralTrapResult> neutral; std::wstring reason;
    if (!TryEvaluateNeutralTrapPolicy(request->TrapPolicy,request->TrapBitmap,trapRequest,request->DomainValidated,neutral,reason)) return CreateTrapResult(VmxCompatibilityTrapAdmissionDecision::TrapPolicyDenied,decode,projection,admission,neutral,reason);
    return CreateTrapResult(VmxCompatibilityTrapAdmissionDecision::TrapProjectionDeniedBackend,decode,projection,admission,neutral,CreateHypercallBackendAdmission(request,admission,neutral));
}

	// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private static bool TryEvaluateNeutralTrapPolicy(TrapPolicyDescriptor? descriptor, System.Nullable<TrapPolicyBitmap> bitmap, TrapRequest request, bool domainValidated, out NeutralTrapResult result, out string reason)
	bool VmxCompatibilityAdmissionService::TryEvaluateNeutralTrapPolicy(std::shared_ptr<TrapPolicyDescriptor> descriptor, std::shared_ptr<TrapPolicyBitmap> bitmap, std::shared_ptr<TrapRequest> request, bool domainValidated, std::shared_ptr<NeutralTrapResult> &result, std::wstring &reason)
	{
		if (descriptor == nullptr)
		{
			result = NeutralTrapResult::Denied(request);
			reason = L"VMCALL trap projection requires a runtime-owned trap policy descriptor.";
			return false;
		}

		if (!descriptor->isRuntimeAuthoritative())
		{
			result = NeutralTrapResult::Denied(request);
			reason = L"Compatibility projection cannot own VMCALL trap policy evaluation.";
			return false;
		}

		if (descriptor->getRequiresValidatedDomain() && !domainValidated)
		{
			result = NeutralTrapResult::Denied(request);
			reason = L"VMCALL trap projection requires a validated domain descriptor.";
			return false;
		}

		if (!descriptor->AllowsClass(TrapPolicyClass::CompatibilityOperation))
		{
			result = NeutralTrapResult::Denied(request);
			reason = L"Runtime trap policy descriptor denies compatibility-operation traps.";
			return false;
		}

		result = bitmap == nullptr ? NeutralTrapResult::Continue(request) : bitmap->Evaluate(request);

		if (!result->ShouldTrap)
		{
			result = NeutralTrapResult::Denied(request);
			reason = L"VMCALL trap projection requires a neutral compatibility-operation intercept.";
			return false;
		}

		reason = L"";
		return true;
	}

	std::shared_ptr<VmxCompatibilityTrapAdmissionResult> VmxCompatibilityAdmissionService::CreateTrapResult(VmxCompatibilityTrapAdmissionDecision decision, std::shared_ptr<VmxCompatDecodeResult> decode, std::shared_ptr<VmxCompatProjectionResult> projection, std::shared_ptr<RuntimeBoundaryAdmissionResult> admission, std::shared_ptr<NeutralTrapResult> neutralResult, const std::wstring &reason)
	{
		return CreateTrapResult(decision, decode, projection, admission, neutralResult, HypercallBackendAdmissionResult::NotEvaluated(L"Hypercall backend admission was not evaluated before this deny point."), reason);
	}

	std::shared_ptr<VmxCompatibilityTrapAdmissionResult> VmxCompatibilityAdmissionService::CreateTrapResult(VmxCompatibilityTrapAdmissionDecision decision, std::shared_ptr<VmxCompatDecodeResult> decode, std::shared_ptr<VmxCompatProjectionResult> projection, std::shared_ptr<RuntimeBoundaryAdmissionResult> admission, std::shared_ptr<NeutralTrapResult> neutralResult, std::shared_ptr<HypercallBackendAdmissionResult> backendAdmission)
	{
		return CreateTrapResult(decision, decode, projection, admission, neutralResult, backendAdmission, std::wstring(L"VMCALL trap projection admitted; ") + backendAdmission->Reason);
	}

	std::shared_ptr<VmxCompatibilityTrapAdmissionResult> VmxCompatibilityAdmissionService::CreateTrapResult(VmxCompatibilityTrapAdmissionDecision decision, std::shared_ptr<VmxCompatDecodeResult> decode, std::shared_ptr<VmxCompatProjectionResult> projection, std::shared_ptr<RuntimeBoundaryAdmissionResult> admission, std::shared_ptr<NeutralTrapResult> neutralResult, std::shared_ptr<HypercallBackendAdmissionResult> backendAdmission, const std::wstring &reason)
	{
		auto routeAdmission = admission ? admission : std::make_shared<RuntimeBoundaryAdmissionResult>(RuntimeBoundaryAdmissionDecision::Allowed, L"", nullptr);
		std::shared_ptr<TrapCompletionRouteRequest> routeRequest = TrapCompletionRouteRequest::ProjectionOnlyDenied(neutralResult, routeAdmission, backendAdmission->IsAllowed);
		std::shared_ptr<TrapCompletionRouteResult> completionRoute = TrapCompletionRouteService::getDefault()->Authorize(routeRequest);
		std::shared_ptr<TrapCompletionPublicationFenceResult> publicationFence = TrapCompletionRouteService::getDefault()->EvaluateFence(routeRequest, completionRoute);

		return std::make_shared<VmxCompatibilityTrapAdmissionResult>(decision, decode, projection, admission, neutralResult, backendAdmission, VmxTrapProjectionMapper::getDefault()->Project(neutralResult), completionRoute, publicationFence, reason);
	}

	std::shared_ptr<HypercallBackendAdmissionResult> VmxCompatibilityAdmissionService::CreateHypercallBackendAdmission(std::shared_ptr<VmxCompatibilityVmCallTrapAdmissionRequest> request, std::shared_ptr<RuntimeBoundaryAdmissionResult> admission, std::shared_ptr<NeutralTrapResult> neutralResult)
	{
		return HypercallBackendAdmissionService::getDefault()->Admit(HypercallBackendAdmissionRequest::MissingNeutralOwner(neutralResult, admission, (request->Context ? request->Context->getCapabilities() : nullptr), request->EvidencePolicy, request->DomainValidated));
	}
}
