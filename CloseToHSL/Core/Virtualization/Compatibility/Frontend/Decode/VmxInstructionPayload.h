#pragma once
#include "../../FrozenAbi/OperandForms/VmxOperandForm.h"
#include "../../FrozenAbi/OperandForms/VmxInvalidationScope.h"
#include "../../FrozenAbi/FunctionLeaves/VmxFunctionLeaf.h"
#include "../../FrozenAbi/VmcsFieldAliases/VmxExitQualification.h"
#include "../../../../System/VmxRootDescriptorReference.h"
#include <memory>




// C# TO C++ CONVERTER TASK: The C# 'using static' statement  cannot be converted to C++:
//using static YAKSys_Hybrid_CPU.Processor.CPU_Core;


namespace YAKSys_Hybrid_CPU::Core
{
struct VmxInstructionPayload {
    const Vmx::VmxOperandForm OperandForm;
    const VmxInvalidationScope InvalidationScope;
    const VmxFunctionLeaf FunctionLeaf;
    const VmxRootDescriptorReference RootDescriptor;
    const VmxExitQualification ExitQualification;
    VmxInstructionPayload(Vmx::VmxOperandForm form, VmxInvalidationScope scope, VmxFunctionLeaf leaf, VmxRootDescriptorReference root, VmxExitQualification qualification) : OperandForm(form), InvalidationScope(scope), FunctionLeaf(leaf), RootDescriptor(root), ExitQualification(qualification) {}
    static const std::shared_ptr<VmxInstructionPayload> Empty;
    static std::shared_ptr<VmxInstructionPayload> FromDecodedRegisters(unsigned short opcode, unsigned char rd, unsigned char rs1, unsigned char rs2);
    static VmxInvalidationScope DecodeInvalidationScope(unsigned long long value);
    static VmxFunctionLeaf DecodeFunctionLeaf(unsigned long long value);
};
	class VmxV2InstructionCaps final
	{
	public:
		static constexpr unsigned long long VmPtrSt = 1ULL << 0;
		static constexpr unsigned long long VmCall = 1ULL << 1;
		static constexpr unsigned long long Invept = 1ULL << 2;
		static constexpr unsigned long long Invvpid = 1ULL << 3;
		static constexpr unsigned long long VmFunc = 1ULL << 4;
		static constexpr unsigned long long RootDescriptorOperand = 1ULL << 5;
		static constexpr unsigned long long VmSaveX = 1ULL << 6;
		static constexpr unsigned long long VmRestX = 1ULL << 7;
		static constexpr unsigned long long NestedVmx = 1ULL << 8;
	};

	class VmxV2ControlBits final
	{
	public:
		static constexpr unsigned long long VmFuncCapabilityQuery = 1ULL << 0;
		static constexpr unsigned long long VmFuncLane7CapabilityQuery = 1ULL << 1;
	};
}
