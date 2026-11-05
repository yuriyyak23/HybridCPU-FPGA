#include "VmxInstructionPayload.h"
#include "VmxFrozenOpcodeValues.h"
namespace YAKSys_Hybrid_CPU::Core {
using namespace VmxFrozenOpcodeValues;
using Vmx::VmxOperandForm;
const std::shared_ptr<VmxInstructionPayload> VmxInstructionPayload::Empty = std::make_shared<VmxInstructionPayload>(VmxOperandForm::None, VmxInvalidationScope::None, VmxFunctionLeaf::None, VmxRootDescriptorReference::CompatibilityDefault, VmxExitQualification());
VmxInvalidationScope VmxInstructionPayload::DecodeInvalidationScope(unsigned long long v) { switch(v) { case 1: return VmxInvalidationScope::SingleContext; case 2: return VmxInvalidationScope::AllContexts; case 3: return VmxInvalidationScope::SingleAddress; default: return VmxInvalidationScope::None; } }
VmxFunctionLeaf VmxInstructionPayload::DecodeFunctionLeaf(unsigned long long v) { switch(v) { case 1: return VmxFunctionLeaf::CapabilityQuery; case 7: return VmxFunctionLeaf::Lane7QueryCaps; case 8: return VmxFunctionLeaf::Lane7Submit; default: return VmxFunctionLeaf::None; } }
std::shared_ptr<VmxInstructionPayload> VmxInstructionPayload::FromDecodedRegisters(unsigned short opcode, unsigned char, unsigned char rs1, unsigned char rs2) {
    auto form=VmxOperandForm::None; auto scope=VmxInvalidationScope::None; auto leaf=VmxFunctionLeaf::None; auto root=VmxRootDescriptorReference::CompatibilityDefault; unsigned short qualLeaf=0; VmxInvalidationScope qualScope=VmxInvalidationScope::None; unsigned long long qualDescriptor=0;
    switch(opcode) {
    case VMXON: form=VmxOperandForm::ReservedIgnoredRegister; root=VmxRootDescriptorReference::FromOperand(rs1); break;
    case VMREAD: form=VmxOperandForm::FieldSelectorToRegister; break;
    case VMWRITE: form=VmxOperandForm::FieldSelectorAndValueRegisters; break;
    case VMCLEAR: case VMPTRLD: form=VmxOperandForm::VmcsPointerRegister; break;
    case VMPTRST: form=VmxOperandForm::CurrentVmcsPointerToRegister; break;
    case VMCALL: form=VmxOperandForm::HypercallLeafAndDescriptor; qualLeaf=rs1; qualDescriptor=rs2; break;
    case INVEPT: case INVVPID: form=VmxOperandForm::InvalidationScopeAndDescriptor; scope=DecodeInvalidationScope(rs1); qualScope=scope; qualDescriptor=rs2; break;
    case VMFUNC: form=VmxOperandForm::FunctionLeafAndDescriptor; leaf=DecodeFunctionLeaf(rs1); qualLeaf=rs1; qualDescriptor=rs2; break;
    case VMSAVEX: case VMRESTX: form=VmxOperandForm::ExtendedStateMaskAndDescriptor; qualLeaf=rs1; qualDescriptor=rs2; break;
    default: return Empty;
    }
    return std::make_shared<VmxInstructionPayload>(form,scope,leaf,root,VmxExitQualification(qualLeaf,qualScope,qualDescriptor));
}
}