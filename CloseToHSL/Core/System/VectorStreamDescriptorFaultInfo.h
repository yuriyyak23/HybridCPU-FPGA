#pragma once
#include <utility>
#include <string>
#include "../Runtime/Lanes/VectorStream/State/VectorStreamState.h"
namespace YAKSys_Hybrid_CPU::Core
{

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct VectorStreamDescriptorFaultInfo(ushort ExecutionDomainTag, ushort AddressSpaceTag, ushort OwnerVirtualThreadId, StreamDescriptorFaultKind Kind, ulong GuestDescriptorAddress, uint DescriptorLength, ulong StreamReplayEpoch, ulong Sequence, string Message)
//	{
//		public bool IsFaulted
//		{
//			get
//			{
//				return Kind != StreamDescriptorFaultKind.None;
//			}
//		}
//
//		public ulong EncodeCompatibilityQualification()
//		{
//			return ((ulong)Kind & 0xFFUL) | (((ulong)OwnerVirtualThreadId & 0xFFFFUL) << 16) | (((ulong)AddressSpaceTag & 0xFFFFUL) << 32);
//		}
//
//		public static VectorStreamDescriptorFaultInfo None
//		{
//			get;
//		} = new(ExecutionDomainTag: 0, AddressSpaceTag: 0, OwnerVirtualThreadId: 0, StreamDescriptorFaultKind.None, GuestDescriptorAddress: 0, DescriptorLength: 0, StreamReplayEpoch: 0, Sequence: 0, Message: string.Empty);
//	}

struct VectorStreamDescriptorFaultInfo final {
    unsigned short ExecutionDomainTag, AddressSpaceTag, OwnerVirtualThreadId;
    StreamDescriptorFaultKind Kind; unsigned long long GuestDescriptorAddress; unsigned int DescriptorLength;
    unsigned long long StreamReplayEpoch, Sequence; std::wstring Message;
    bool IsFaulted;
    unsigned long long EncodeCompatibilityQualification() const { return (static_cast<unsigned long long>(Kind) & 0xFFULL) | ((static_cast<unsigned long long>(OwnerVirtualThreadId) & 0xFFFFULL) << 16) | ((static_cast<unsigned long long>(AddressSpaceTag) & 0xFFFFULL) << 32); }
    VectorStreamDescriptorFaultInfo(unsigned short domain, unsigned short addressSpace, unsigned short vt, StreamDescriptorFaultKind kind, unsigned long long address, unsigned int length, unsigned long long replayEpoch, unsigned long long sequence, std::wstring message)
        : ExecutionDomainTag(domain), AddressSpaceTag(addressSpace), OwnerVirtualThreadId(vt), Kind(kind), GuestDescriptorAddress(address), DescriptorLength(length), StreamReplayEpoch(replayEpoch), Sequence(sequence), Message(std::move(message)), IsFaulted(kind != StreamDescriptorFaultKind::None) {}
	static const VectorStreamDescriptorFaultInfo None;
};

inline const VectorStreamDescriptorFaultInfo VectorStreamDescriptorFaultInfo::None{0, 0, 0, StreamDescriptorFaultKind::None, 0, 0, 0, 0, L""};
}
