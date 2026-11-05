#pragma once
#include "VmxOperationKind.h"
#include "../../../../System/VmxRootDescriptorReference.h"

#include "../../FrozenAbi/VmcsFieldAliases/VmExitReason.h"
#include "../../FrozenAbi/VmcsFieldAliases/VmcsField.h"
#include "../../FrozenAbi/OperandForms/VmxInvalidationScope.h"
#include "../../FrozenAbi/FunctionLeaves/VmxFunctionLeaf.h"
#include "../../../../Runtime/Lanes/VectorStream/State/VectorStreamState.h"
#include "../Projection/VmxCompletion.h"
#include <memory>
#include <optional>

using namespace YAKSys_Hybrid_CPU::Core::Vmcs::V2;

namespace YAKSys_Hybrid_CPU::Core
{
    class TrapDecision;
	class VmxExitQualification;
	class TrapCompletionPublicationFenceResult;
	

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to the C# readonly struct:
// ORIGINAL LINE: public readonly struct VmxRetireEffect
	class VmxRetireEffect
	{
	private:
		const VmxOperationKind Operation;
		const bool IsFaulted;
		const VmExitReason FailureReason;
		const bool ExitGuestContextOnRetire;
		const bool HasRegisterDestination;
		const unsigned short RegisterDestination;
		const YAKSys_Hybrid_CPU::Core::VmcsField VmcsField;
		const long long VmcsValue;
		const unsigned long long VmcsPointer;
		const VmxInvalidationScope InvalidationScope;
		const unsigned long long DescriptorOperand;
		const VmxFunctionLeaf FunctionLeaf;
		const VectorStreamSaveMask ExtendedStateMask;
		const std::shared_ptr<VmxExitQualification> ExitQualification;
		const VmxCompletionKind CompletionKind;
		const VmFailCode FailCode;
		const VmAbortCode AbortCode;
		const std::shared_ptr<VmxRootDescriptorReference> RootDescriptor;
		const VmExitReason ExitReason;
		const std::shared_ptr<TrapDecision> InterceptDecision;

		VmxRetireEffect(VmxOperationKind operation, bool isFaulted, VmExitReason failureReason, bool exitGuestContextOnRetire, bool hasRegisterDestination, unsigned short registerDestination, YAKSys_Hybrid_CPU::Core::VmcsField vmcsField, long long vmcsValue, unsigned long long vmcsPointer, VmxInvalidationScope invalidationScope = VmxInvalidationScope::None, unsigned long long descriptorOperand = 0, VmxFunctionLeaf functionLeaf = VmxFunctionLeaf::None, VectorStreamSaveMask extendedStateMask = VectorStreamSaveMask::None, std::shared_ptr<VmxExitQualification> exitQualification = nullptr, VmxCompletionKind completionKind = VmxCompletionKind::Success, std::shared_ptr<VmxRootDescriptorReference> rootDescriptor = nullptr, VmExitReason exitReason = VmExitReason::None, std::shared_ptr<TrapDecision> interceptDecision = nullptr, VmFailCode failCode = VmFailCode::None, VmAbortCode abortCode = VmAbortCode::None);

	public:
		VmxOperationKind getOperation() const;

		bool isFaulted() const;

		VmExitReason getFailureReason() const;

		bool getExitGuestContextOnRetire() const;

		bool getHasRegisterDestination() const;

		unsigned short getRegisterDestination() const;

		YAKSys_Hybrid_CPU::Core::VmcsField getVmcsField() const;

		long long getVmcsValue() const;

		unsigned long long getVmcsPointer() const;

		VmxInvalidationScope getInvalidationScope() const;

		unsigned long long getDescriptorOperand() const;

		VmxFunctionLeaf getFunctionLeaf() const;

		VectorStreamSaveMask getExtendedStateMask() const;

		std::shared_ptr<VmxExitQualification> getExitQualification() const;

		VmxCompletionKind getCompletionKind() const;

		VmFailCode getFailCode() const;

		VmAbortCode getAbortCode() const;

		std::shared_ptr<VmxRootDescriptorReference> getRootDescriptor() const;

		VmExitReason getExitReason() const;

		std::shared_ptr<TrapDecision> getInterceptDecision() const;

		bool isValid() const;

		static VmxRetireEffect Fault(VmxOperationKind operation, VmExitReason failureReason = VmExitReason::None, VmFailCode failCode = VmFailCode::None);

		static VmxRetireEffect Abort(VmxOperationKind operation, VmAbortCode abortCode);

		static VmxRetireEffect Control(VmxOperationKind operation, bool exitGuestContextOnRetire = false);

		static VmxRetireEffect VmxOnRootDescriptor(std::shared_ptr<VmxRootDescriptorReference> rootDescriptor);

		static VmxRetireEffect VmcsRead(YAKSys_Hybrid_CPU::Core::VmcsField field, unsigned short destinationRegister, bool hasRegisterDestination);

		static VmxRetireEffect VmPtrSt(unsigned short destinationRegister, bool hasRegisterDestination);

		static VmxRetireEffect VmcsWrite(YAKSys_Hybrid_CPU::Core::VmcsField field, long long value);

		static VmxRetireEffect VmcsPointerEffect(VmxOperationKind operation, unsigned long long vmcsPointer);

		static VmxRetireEffect VmCall(unsigned short leaf, unsigned long long descriptor, bool exitGuestContextOnRetire);

		static VmxRetireEffect Invalidation(VmxOperationKind operation, VmxInvalidationScope scope, unsigned long long descriptor);

		static VmxRetireEffect VmFunc(VmxFunctionLeaf leaf, unsigned long long descriptor, unsigned short destinationRegister, bool hasRegisterDestination, bool exitGuestContextOnRetire, VmExitReason exitReason = VmExitReason::VmFunc);

		static VmxRetireEffect ExtendedState(VmxOperationKind operation, VectorStreamSaveMask mask, unsigned long long descriptor);

		static VmxRetireEffect InterceptExit(std::shared_ptr<TrapDecision> decision, std::shared_ptr<TrapCompletionPublicationFenceResult> publicationFence);

		VmxRetireEffect();
	};

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct VmxRetireOutcome(bool Faulted, VmExitReason FailureReason, bool HasRegisterWriteback, ushort RegisterDestination, ulong RegisterWritebackValue, System.Nullable<ulong> RedirectTargetPc, System.Nullable<ulong> RestoredStackPointer, bool FlushesPipeline)
//	{
//		public bool RedirectsControlFlow
//		{
//			get
//			{
//				return RedirectTargetPc.HasValue;
//			}
//		}
//
//		public static VmxRetireOutcome Fault(VmExitReason failureReason = VmExitReason.None)
//		{
//			return new(Faulted: true, FailureReason: failureReason, HasRegisterWriteback: false, RegisterDestination: 0, RegisterWritebackValue: 0, RedirectTargetPc: nullptr, RestoredStackPointer: nullptr, FlushesPipeline: false);
//		}
//
//		public static VmxRetireOutcome NoOp()
//		{
//			return new(Faulted: false, FailureReason: VmExitReason.None, HasRegisterWriteback: false, RegisterDestination: 0, RegisterWritebackValue: 0, RedirectTargetPc: nullptr, RestoredStackPointer: nullptr, FlushesPipeline: false);
//		}
//	}
	struct VmxRetireOutcome final
	{
		bool Faulted = false;
		VmExitReason FailureReason = VmExitReason::None;
		bool HasRegisterWriteback = false;
		unsigned short RegisterDestination = 0;
		unsigned long long RegisterWritebackValue = 0;
		std::optional<unsigned long long> RedirectTargetPc;
		std::optional<unsigned long long> RestoredStackPointer;
		bool FlushesPipeline = false;

		bool getRedirectsControlFlow() const { return RedirectTargetPc.has_value(); }

		static VmxRetireOutcome Fault(VmExitReason failureReason = VmExitReason::None)
		{
			VmxRetireOutcome outcome;
			outcome.Faulted = true;
			outcome.FailureReason = failureReason;
			return outcome;
		}

		static VmxRetireOutcome NoOp() { return {}; }
	};
}
