#include "VmxRetireModel.h"
#include "../Projection/Events/TrapDecision.h"
#include "../../../../Runtime/Completion/Records/TrapCompletionPublicationFence.h"

using namespace YAKSys_Hybrid_CPU::Core::Vmcs::V2;

namespace YAKSys_Hybrid_CPU::Core
{

	VmxRetireEffect::VmxRetireEffect() : Operation(VmxOperationKind::None), IsFaulted(false), FailureReason(VmExitReason::None), ExitGuestContextOnRetire(false), HasRegisterDestination(false), RegisterDestination(0), VmcsField(YAKSys_Hybrid_CPU::Core::VmcsField{}), VmcsValue(0), VmcsPointer(0), InvalidationScope(VmxInvalidationScope::None), DescriptorOperand(0), FunctionLeaf(VmxFunctionLeaf::None), ExtendedStateMask(VectorStreamSaveMask::None), ExitQualification(std::make_shared<VmxExitQualification>()), CompletionKind(VmxCompletionKind{}), FailCode(VmFailCode::None), AbortCode(VmAbortCode::None), RootDescriptor(std::make_shared<VmxRootDescriptorReference>()), ExitReason(VmExitReason::None), InterceptDecision(nullptr) {}

	VmxRetireEffect::VmxRetireEffect(VmxOperationKind operation, bool isFaulted, VmExitReason failureReason, bool exitGuestContextOnRetire, bool hasRegisterDestination, unsigned short registerDestination, YAKSys_Hybrid_CPU::Core::VmcsField vmcsField, long long vmcsValue, unsigned long long vmcsPointer, VmxInvalidationScope invalidationScope, unsigned long long descriptorOperand, VmxFunctionLeaf functionLeaf, VectorStreamSaveMask extendedStateMask, std::shared_ptr<VmxExitQualification> exitQualification, VmxCompletionKind completionKind, std::shared_ptr<VmxRootDescriptorReference> rootDescriptor, VmExitReason exitReason, std::shared_ptr<TrapDecision> interceptDecision, VmFailCode failCode, VmAbortCode abortCode) : Operation(operation), IsFaulted(isFaulted), FailureReason(failureReason), ExitGuestContextOnRetire(exitGuestContextOnRetire), HasRegisterDestination(hasRegisterDestination), RegisterDestination(registerDestination), VmcsField(vmcsField), VmcsValue(vmcsValue), VmcsPointer(vmcsPointer), InvalidationScope(invalidationScope), DescriptorOperand(descriptorOperand), FunctionLeaf(functionLeaf), ExtendedStateMask(extendedStateMask), ExitQualification(exitQualification ? exitQualification : std::make_shared<VmxExitQualification>()), CompletionKind(completionKind), FailCode(failCode), AbortCode(abortCode), RootDescriptor(rootDescriptor && rootDescriptor->isValid() ? rootDescriptor : std::make_shared<VmxRootDescriptorReference>(VmxRootDescriptorReference::CompatibilityDefault)), ExitReason(exitReason), InterceptDecision(interceptDecision) {}

	VmxOperationKind VmxRetireEffect::getOperation() const
	{
		return Operation;
	}

	bool VmxRetireEffect::isFaulted() const
	{
		return IsFaulted;
	}

	VmExitReason VmxRetireEffect::getFailureReason() const
	{
		return FailureReason;
	}

	bool VmxRetireEffect::getExitGuestContextOnRetire() const
	{
		return ExitGuestContextOnRetire;
	}

	bool VmxRetireEffect::getHasRegisterDestination() const
	{
		return HasRegisterDestination;
	}

	unsigned short VmxRetireEffect::getRegisterDestination() const
	{
		return RegisterDestination;
	}

	YAKSys_Hybrid_CPU::Core::VmcsField VmxRetireEffect::getVmcsField() const
	{
		return VmcsField;
	}

	long long VmxRetireEffect::getVmcsValue() const
	{
		return VmcsValue;
	}

	unsigned long long VmxRetireEffect::getVmcsPointer() const
	{
		return VmcsPointer;
	}

	VmxInvalidationScope VmxRetireEffect::getInvalidationScope() const
	{
		return InvalidationScope;
	}

	unsigned long long VmxRetireEffect::getDescriptorOperand() const
	{
		return DescriptorOperand;
	}

	VmxFunctionLeaf VmxRetireEffect::getFunctionLeaf() const
	{
		return FunctionLeaf;
	}

	VectorStreamSaveMask VmxRetireEffect::getExtendedStateMask() const
	{
		return ExtendedStateMask;
	}

	std::shared_ptr<VmxExitQualification> VmxRetireEffect::getExitQualification() const
	{
		return ExitQualification;
	}

	VmxCompletionKind VmxRetireEffect::getCompletionKind() const
	{
		return CompletionKind;
	}

	VmFailCode VmxRetireEffect::getFailCode() const
	{
		return FailCode;
	}

	VmAbortCode VmxRetireEffect::getAbortCode() const
	{
		return AbortCode;
	}

	std::shared_ptr<VmxRootDescriptorReference> VmxRetireEffect::getRootDescriptor() const
	{
		return RootDescriptor;
	}

	VmExitReason VmxRetireEffect::getExitReason() const
	{
		return ExitReason;
	}

	std::shared_ptr<TrapDecision> VmxRetireEffect::getInterceptDecision() const
	{
		return InterceptDecision;
	}

	bool VmxRetireEffect::isValid() const
	{
		return getOperation() != VmxOperationKind::None;
	}

	VmxRetireEffect VmxRetireEffect::Fault(VmxOperationKind op, VmExitReason reason, VmFailCode fail) {
    return VmxRetireEffect(op,true,reason,false,false,0,YAKSys_Hybrid_CPU::Core::VmcsField{},0,0,VmxInvalidationScope::None,0,VmxFunctionLeaf::None,VectorStreamSaveMask::None,nullptr,VmxCompletionKind::VmFailInvalid,nullptr,VmExitReason::None,nullptr,fail);
}
VmxRetireEffect VmxRetireEffect::Abort(VmxOperationKind op, VmAbortCode abort) {
    return VmxRetireEffect(op,true,VmExitReason::None,false,false,0,YAKSys_Hybrid_CPU::Core::VmcsField{},0,0,VmxInvalidationScope::None,0,VmxFunctionLeaf::None,VectorStreamSaveMask::None,nullptr,VmxCompletionKind::VmAbort,nullptr,VmExitReason::None,nullptr,VmFailCode::None,abort);
}
VmxRetireEffect VmxRetireEffect::Control(VmxOperationKind op, bool exit) { return VmxRetireEffect(op,false,VmExitReason::None,exit,false,0,YAKSys_Hybrid_CPU::Core::VmcsField{},0,0); }
VmxRetireEffect VmxRetireEffect::VmxOnRootDescriptor(std::shared_ptr<VmxRootDescriptorReference> root) {
    return VmxRetireEffect(VmxOperationKind::VmxOn,false,VmExitReason::None,false,false,0,YAKSys_Hybrid_CPU::Core::VmcsField{},0,0,VmxInvalidationScope::None,0,VmxFunctionLeaf::None,VectorStreamSaveMask::None,nullptr,VmxCompletionKind::Success,root);
}
VmxRetireEffect VmxRetireEffect::VmcsRead(YAKSys_Hybrid_CPU::Core::VmcsField field, unsigned short destination, bool hasDestination) { return VmxRetireEffect(VmxOperationKind::VmRead,false,VmExitReason::None,false,hasDestination,destination,field,0,0); }
VmxRetireEffect VmxRetireEffect::VmPtrSt(unsigned short destination, bool hasDestination) { return VmxRetireEffect(VmxOperationKind::VmPtrSt,false,VmExitReason::None,false,hasDestination,destination,YAKSys_Hybrid_CPU::Core::VmcsField{},0,0); }
VmxRetireEffect VmxRetireEffect::VmcsWrite(YAKSys_Hybrid_CPU::Core::VmcsField field, long long value) { return VmxRetireEffect(VmxOperationKind::VmWrite,false,VmExitReason::None,false,false,0,field,value,0); }
VmxRetireEffect VmxRetireEffect::VmcsPointerEffect(VmxOperationKind op, unsigned long long pointer) { return VmxRetireEffect(op,false,VmExitReason::None,false,false,0,YAKSys_Hybrid_CPU::Core::VmcsField{},0,pointer); }
VmxRetireEffect VmxRetireEffect::VmCall(unsigned short leaf, unsigned long long descriptor, bool exit) {
    return VmxRetireEffect(VmxOperationKind::VmCall,false,VmExitReason::None,exit,false,0,YAKSys_Hybrid_CPU::Core::VmcsField{},0,0,VmxInvalidationScope::None,descriptor,VmxFunctionLeaf::None,VectorStreamSaveMask::None,std::make_shared<VmxExitQualification>(leaf,VmxInvalidationScope::None,descriptor),exit ? VmxCompletionKind::VmExit : VmxCompletionKind::Success);
}
VmxRetireEffect VmxRetireEffect::Invalidation(VmxOperationKind op, VmxInvalidationScope scope, unsigned long long descriptor) {
    return VmxRetireEffect(op,false,VmExitReason::None,false,false,0,YAKSys_Hybrid_CPU::Core::VmcsField{},0,0,scope,descriptor,VmxFunctionLeaf::None,VectorStreamSaveMask::None,std::make_shared<VmxExitQualification>(0,scope,descriptor));
}
VmxRetireEffect VmxRetireEffect::VmFunc(VmxFunctionLeaf leaf, unsigned long long descriptor, unsigned short destination, bool hasDestination, bool exit, VmExitReason reason) {
    return VmxRetireEffect(VmxOperationKind::VmFunc,false,VmExitReason::None,exit,hasDestination,destination,YAKSys_Hybrid_CPU::Core::VmcsField{},0,0,VmxInvalidationScope::None,descriptor,leaf,VectorStreamSaveMask::None,std::make_shared<VmxExitQualification>(static_cast<unsigned short>(leaf),VmxInvalidationScope::None,descriptor),exit ? VmxCompletionKind::VmExit : VmxCompletionKind::Success,nullptr,exit ? reason : VmExitReason::None);
}
VmxRetireEffect VmxRetireEffect::ExtendedState(VmxOperationKind op, VectorStreamSaveMask mask, unsigned long long descriptor) {
    return VmxRetireEffect(op,false,VmExitReason::None,false,false,0,YAKSys_Hybrid_CPU::Core::VmcsField{},0,0,VmxInvalidationScope::None,descriptor,VmxFunctionLeaf::None,mask,std::make_shared<VmxExitQualification>(static_cast<unsigned short>(mask),VmxInvalidationScope::None,descriptor));
}
VmxRetireEffect VmxRetireEffect::InterceptExit(std::shared_ptr<TrapDecision> decision, std::shared_ptr<TrapCompletionPublicationFenceResult> fence) {
    if (!fence->RetirePublicationAllowed) return Fault(VmxOperationKind::InterceptExit,VmExitReason::SecurityPolicyViolation);
    return VmxRetireEffect(VmxOperationKind::InterceptExit,false,VmExitReason::None,true,false,0,YAKSys_Hybrid_CPU::Core::VmcsField{},0,0,VmxInvalidationScope::None,0,VmxFunctionLeaf::None,VectorStreamSaveMask::None,decision->ExitQualification,VmxCompletionKind::VmExit,nullptr,decision->ExitReason,decision);
}
}