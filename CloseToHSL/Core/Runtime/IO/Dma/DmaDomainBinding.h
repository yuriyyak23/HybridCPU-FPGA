#pragma once
#include <string>
#include <utility>

namespace HybridCPU_ISE::CloseToHSL::Memory::MMU { enum class IOMMUAccessPermissions : int; }
using namespace HybridCPU_ISE::CloseToHSL::Memory::MMU;

namespace YAKSys_Hybrid_CPU::Memory
{
	struct IommuDomainBinding final {
    unsigned short IoDomainTag = 0; unsigned int DomainId = 0; unsigned long long DomainTag = 0; unsigned int DeviceId = 0;
    IOMMUAccessPermissions Permissions = static_cast<IOMMUAccessPermissions>(0);
    unsigned long long DomainEpoch = 0; bool NonCoherent = false; bool RequiresFence = false;
    bool isValid() const noexcept { return IoDomainTag != 0 && DomainId != 0 && DomainTag != 0 && DeviceId != 0 && static_cast<int>(Permissions) != 0 && DomainEpoch != 0; }
    bool Allows(IOMMUAccessPermissions requested) const noexcept { auto r = static_cast<int>(requested); return r != 0 && (static_cast<int>(Permissions) & r) == r; }
    IommuDomainBinding WithEpoch(unsigned long long epoch) const { auto copy = *this; copy.DomainEpoch = epoch ? epoch : 1; return copy; }
    static IommuDomainBinding Create(unsigned short ioTag, unsigned int domainId, unsigned long long domainTag, unsigned int deviceId, IOMMUAccessPermissions permissions, unsigned long long epoch = 1, bool nonCoherent = false, bool requiresFence = false) { return {ioTag, domainId, domainTag, deviceId, permissions, epoch ? epoch : 1, nonCoherent, requiresFence}; }
    bool operator==(const IommuDomainBinding&) const = default;
};
enum class DmaFaultKind : unsigned char
	{
		None = 0,
		MissingDomainBinding = 1,
		DeviceDomainMismatch = 2,
		DescriptorFault = 3,
		PermissionFault = 4,
		IommuTranslationFault = 5,
		NonCoherentFenceRequired = 6,
		QueueOwnershipFault = 7,
		Lane6QueuePressure = 8,
		ValidationEvidenceMissing = 9,
		ValidationEvidenceStale = 10,
		EpochExhausted = 11,
		ResourceIdentityExhausted = 12
	};

	enum class DmaFaultDisposition : unsigned char
	{
		None = 0,
		Abort = 1,
		Replay = 2
	};

	struct DmaFault final {
		DmaFaultKind Kind = DmaFaultKind::None;
		DmaFaultDisposition Disposition = DmaFaultDisposition::None;
		unsigned long long FaultAddress = 0;
		bool IsWrite = false;
		std::wstring Message;
		bool isFaulted() const noexcept { return Kind != DmaFaultKind::None; }
		static DmaFault Abort(DmaFaultKind kind, unsigned long long address, bool isWrite, std::wstring message) { return {kind, DmaFaultDisposition::Abort, address, isWrite, std::move(message)}; }
		static DmaFault Replay(DmaFaultKind kind, unsigned long long address, bool isWrite, std::wstring message) { return {kind, DmaFaultDisposition::Replay, address, isWrite, std::move(message)}; }
	};
// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct IommuDomainBinding(ushort IoDomainTag, uint DomainId, ulong DomainTag, uint DeviceId, IOMMUAccessPermissions Permissions, ulong DomainEpoch, bool NonCoherent, bool RequiresFence)
//	{
//		public bool IsValid
//		{
//			get
//			{
//				return IoDomainTag != 0 && DomainId != 0 && DomainTag != 0 && DeviceId != 0 && Permissions != IOMMUAccessPermissions.None && DomainEpoch != 0;
//			}
//		}
//
//		public bool Allows(IOMMUAccessPermissions requested)
//		{
//			return requested != IOMMUAccessPermissions.None && (Permissions & requested) == requested;
//		}
//
//		public IommuDomainBinding WithEpoch(ulong epoch)
//		{
//			return this with { DomainEpoch = epoch == 0 ? 1 : epoch };
//		}
//
//		public static IommuDomainBinding Create(ushort ioDomainTag, uint domainId, ulong domainTag, uint deviceId, IOMMUAccessPermissions permissions, ulong domainEpoch = 1, bool nonCoherent = false, bool requiresFence = false)
//		{
//			return new(ioDomainTag, domainId, domainTag, deviceId, permissions, domainEpoch == 0 ? 1 : domainEpoch, nonCoherent, requiresFence);
//		}
//	}

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct DmaFault(DmaFaultKind Kind, DmaFaultDisposition Disposition, ulong FaultAddress, bool IsWrite, string Message)
//	{
//		public bool IsFaulted
//		{
//			get
//			{
//				return Kind != DmaFaultKind.None;
//			}
//		}
//
//		public static DmaFault None
//		{
//			get;
//		} = new(DmaFaultKind.None, DmaFaultDisposition.None, 0, false, string.Empty);
//
//		public static DmaFault Abort(DmaFaultKind kind, ulong faultAddress, bool isWrite, string message)
//		{
//			return new(kind, DmaFaultDisposition.Abort, faultAddress, isWrite, message);
//		}
//
//		public static DmaFault Replay(DmaFaultKind kind, ulong faultAddress, bool isWrite, string message)
//		{
//			return new(kind, DmaFaultDisposition.Replay, faultAddress, isWrite, message);
//		}
//	}

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct DmaTranslationResult(bool Succeeded, bool IotlbHit, ulong IoVirtualAddress, ulong HostPhysicalAddress, ulong AccessSize, IOMMUAccessPermissions Permissions, IotlbTag Tag, DmaFault Fault)
//	{
//		public static DmaTranslationResult Success(bool iotlbHit, ulong ioVirtualAddress, ulong hostPhysicalAddress, ulong accessSize, IOMMUAccessPermissions permissions, IotlbTag tag)
//		{
//			return new(true, iotlbHit, ioVirtualAddress, hostPhysicalAddress, accessSize, permissions, tag, DmaFault.None);
//		}
//
//		public static DmaTranslationResult Fail(ulong ioVirtualAddress, ulong accessSize, IOMMUAccessPermissions permissions, DmaFault fault)
//		{
//			return new(false, false, ioVirtualAddress, 0, accessSize, permissions, default, fault);
//		}
//	}
}
