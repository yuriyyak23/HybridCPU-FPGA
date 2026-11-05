$p='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Virtualization\Compatibility\Frontend\Retire\VmxRetireModel.cpp'; $s=[IO.File]::ReadAllText($p)
$s=$s.Replace('#include "VmxRetireModel.h"',"#include `"VmxRetireModel.h`"`r`n#include `"../Projection/Events/TrapDecision.h`"`r`n#include `"../../../../Runtime/Completion/Records/TrapCompletionPublicationFence.h`"")
$pat='(?s)(VmxRetireEffect::VmxRetireEffect\([^\r\n]+\))\s*\{\s*Operation = operation;.*?RootDescriptor = rootDescriptor->IsValid \? rootDescriptor : VmxRootDescriptorReference::CompatibilityDefault;\s*\}'
if(-not [regex]::IsMatch($s,$pat)){throw 'constructor missing'}
$init='$1 : Operation(operation), IsFaulted(isFaulted), FailureReason(failureReason), ExitGuestContextOnRetire(exitGuestContextOnRetire), HasRegisterDestination(hasRegisterDestination), RegisterDestination(registerDestination), VmcsField(vmcsField), VmcsValue(vmcsValue), VmcsPointer(vmcsPointer), InvalidationScope(invalidationScope), DescriptorOperand(descriptorOperand), FunctionLeaf(functionLeaf), ExtendedStateMask(extendedStateMask), ExitQualification(exitQualification ? exitQualification : std::make_shared<VmxExitQualification>()), CompletionKind(completionKind), FailCode(failCode), AbortCode(abortCode), RootDescriptor(rootDescriptor && rootDescriptor->isValid() ? rootDescriptor : std::make_shared<VmxRootDescriptorReference>(VmxRootDescriptorReference::CompatibilityDefault)), ExitReason(exitReason), InterceptDecision(interceptDecision) {}'
$s=[regex]::Replace($s,$pat,$init)
$i=$s.IndexOf('VmxRetireEffect VmxRetireEffect::Fault('); if($i -lt 0){throw 'factory anchor missing'}
$factories=@'
VmxRetireEffect VmxRetireEffect::Fault(VmxOperationKind op, VmExitReason reason, VmFailCode fail) {
    return VmxRetireEffect(op,true,reason,false,false,0,VmcsField{},0,0,VmxInvalidationScope::None,0,VmxFunctionLeaf::None,VectorStreamSaveMask::None,nullptr,VmxCompletionKind::VmFailInvalid,nullptr,VmExitReason::None,nullptr,fail);
}
VmxRetireEffect VmxRetireEffect::Abort(VmxOperationKind op, VmAbortCode abort) {
    return VmxRetireEffect(op,true,VmExitReason::None,false,false,0,VmcsField{},0,0,VmxInvalidationScope::None,0,VmxFunctionLeaf::None,VectorStreamSaveMask::None,nullptr,VmxCompletionKind::VmAbort,nullptr,VmExitReason::None,nullptr,VmFailCode::None,abort);
}
VmxRetireEffect VmxRetireEffect::Control(VmxOperationKind op, bool exit) { return VmxRetireEffect(op,false,VmExitReason::None,exit,false,0,VmcsField{},0,0); }
VmxRetireEffect VmxRetireEffect::VmxOnRootDescriptor(std::shared_ptr<VmxRootDescriptorReference> root) {
    return VmxRetireEffect(VmxOperationKind::VmxOn,false,VmExitReason::None,false,false,0,VmcsField{},0,0,VmxInvalidationScope::None,0,VmxFunctionLeaf::None,VectorStreamSaveMask::None,nullptr,VmxCompletionKind::Success,root);
}
VmxRetireEffect VmxRetireEffect::VmcsRead(YAKSys_Hybrid_CPU::Core::VmcsField field, unsigned short destination, bool hasDestination) { return VmxRetireEffect(VmxOperationKind::VmRead,false,VmExitReason::None,false,hasDestination,destination,field,0,0); }
VmxRetireEffect VmxRetireEffect::VmPtrSt(unsigned short destination, bool hasDestination) { return VmxRetireEffect(VmxOperationKind::VmPtrSt,false,VmExitReason::None,false,hasDestination,destination,VmcsField{},0,0); }
VmxRetireEffect VmxRetireEffect::VmcsWrite(YAKSys_Hybrid_CPU::Core::VmcsField field, long long value) { return VmxRetireEffect(VmxOperationKind::VmWrite,false,VmExitReason::None,false,false,0,field,value,0); }
VmxRetireEffect VmxRetireEffect::VmcsPointerEffect(VmxOperationKind op, unsigned long long pointer) { return VmxRetireEffect(op,false,VmExitReason::None,false,false,0,VmcsField{},0,pointer); }
VmxRetireEffect VmxRetireEffect::VmCall(unsigned short leaf, unsigned long long descriptor, bool exit) {
    return VmxRetireEffect(VmxOperationKind::VmCall,false,VmExitReason::None,exit,false,0,VmcsField{},0,0,VmxInvalidationScope::None,descriptor,VmxFunctionLeaf::None,VectorStreamSaveMask::None,std::make_shared<VmxExitQualification>(leaf,VmxInvalidationScope::None,descriptor),exit ? VmxCompletionKind::VmExit : VmxCompletionKind::Success);
}
VmxRetireEffect VmxRetireEffect::Invalidation(VmxOperationKind op, VmxInvalidationScope scope, unsigned long long descriptor) {
    return VmxRetireEffect(op,false,VmExitReason::None,false,false,0,VmcsField{},0,0,scope,descriptor,VmxFunctionLeaf::None,VectorStreamSaveMask::None,std::make_shared<VmxExitQualification>(0,scope,descriptor));
}
VmxRetireEffect VmxRetireEffect::VmFunc(VmxFunctionLeaf leaf, unsigned long long descriptor, unsigned short destination, bool hasDestination, bool exit, VmExitReason reason) {
    return VmxRetireEffect(VmxOperationKind::VmFunc,false,VmExitReason::None,exit,hasDestination,destination,VmcsField{},0,0,VmxInvalidationScope::None,descriptor,leaf,VectorStreamSaveMask::None,std::make_shared<VmxExitQualification>(static_cast<unsigned short>(leaf),VmxInvalidationScope::None,descriptor),exit ? VmxCompletionKind::VmExit : VmxCompletionKind::Success,nullptr,exit ? reason : VmExitReason::None);
}
VmxRetireEffect VmxRetireEffect::ExtendedState(VmxOperationKind op, VectorStreamSaveMask mask, unsigned long long descriptor) {
    return VmxRetireEffect(op,false,VmExitReason::None,false,false,0,VmcsField{},0,0,VmxInvalidationScope::None,descriptor,VmxFunctionLeaf::None,mask,std::make_shared<VmxExitQualification>(static_cast<unsigned short>(mask),VmxInvalidationScope::None,descriptor));
}
VmxRetireEffect VmxRetireEffect::InterceptExit(std::shared_ptr<TrapDecision> decision, std::shared_ptr<TrapCompletionPublicationFenceResult> fence) {
    if (!fence->RetirePublicationAllowed) return Fault(VmxOperationKind::InterceptExit,VmExitReason::SecurityPolicyViolation);
    return VmxRetireEffect(VmxOperationKind::InterceptExit,false,VmExitReason::None,true,false,0,VmcsField{},0,0,VmxInvalidationScope::None,0,VmxFunctionLeaf::None,VectorStreamSaveMask::None,decision->ExitQualification,VmxCompletionKind::VmExit,nullptr,decision->ExitReason,decision);
}
}
'@
$s=$s.Substring(0,$i)+$factories; [IO.File]::WriteAllText($p,$s)