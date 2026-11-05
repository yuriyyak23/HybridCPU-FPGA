#pragma once
#include "../Runtime/Lanes/VectorStream/State/VectorStreamState.h"
namespace YAKSys_Hybrid_CPU::Core
{

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct VectorStreamExceptionInfo(ushort ExecutionDomainTag, ushort AddressSpaceTag, ushort OwnerVirtualThreadId, uint ExceptionMask, uint ExceptionPriority, byte HighestExceptionIndex, ulong FaultingPc, ulong FaultingLane, uint FaultingOpcode, VectorExceptionAction Action, ulong Sequence)
//	{
//		public bool IsValid
//		{
//			get
//			{
//				return ExecutionDomainTag != 0 && HighestExceptionIndex < 5;
//			}
//		}
//
//		public ulong EncodeCompatibilityQualification()
//		{
//			return ((ulong)HighestExceptionIndex & 0x7UL) | (((ulong)Action & 0x3UL) << 8) | (((ulong)OwnerVirtualThreadId & 0xFFFFUL) << 16) | (((ulong)AddressSpaceTag & 0xFFFFUL) << 32);
//		}
//
//		public bool RequiresCompatibilityExitProjection
//		{
//			get
//			{
//				return Action == VectorExceptionAction.ReflectAsCompatibilityExit;
//			}
//		}
//	}

struct VectorStreamExceptionInfo final {
    unsigned short ExecutionDomainTag, AddressSpaceTag, OwnerVirtualThreadId;
    unsigned int ExceptionMask, ExceptionPriority; unsigned char HighestExceptionIndex;
    unsigned long long FaultingPc, FaultingLane; unsigned int FaultingOpcode;
    VectorExceptionAction Action; unsigned long long Sequence;
    bool IsValid;
    bool RequiresCompatibilityExitProjection;
    unsigned long long EncodeCompatibilityQualification() const { return (static_cast<unsigned long long>(HighestExceptionIndex) & 0x7ULL) | ((static_cast<unsigned long long>(Action) & 0x3ULL) << 8) | ((static_cast<unsigned long long>(OwnerVirtualThreadId) & 0xFFFFULL) << 16) | ((static_cast<unsigned long long>(AddressSpaceTag) & 0xFFFFULL) << 32); }
    VectorStreamExceptionInfo(unsigned short domain, unsigned short addressSpace, unsigned short vt, unsigned int exceptionMask, unsigned int exceptionPriority, unsigned char highestIndex, unsigned long long faultingPc, unsigned long long faultingLane, unsigned int opcode, VectorExceptionAction action, unsigned long long sequence)
        : ExecutionDomainTag(domain), AddressSpaceTag(addressSpace), OwnerVirtualThreadId(vt), ExceptionMask(exceptionMask), ExceptionPriority(exceptionPriority), HighestExceptionIndex(highestIndex), FaultingPc(faultingPc), FaultingLane(faultingLane), FaultingOpcode(opcode), Action(action), Sequence(sequence), IsValid(domain != 0 && highestIndex < 5), RequiresCompatibilityExitProjection(action == VectorExceptionAction::ReflectAsCompatibilityExit) {}
};
}
