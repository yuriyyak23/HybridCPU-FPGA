#pragma once

#include "../../Domains/Admission/Memory/YAKSys_Hybrid_CPU.Core.MemoryDomainRuntime.h"
#include "../../Domains/Descriptors/MemoryDomain/YAKSys_Hybrid_CPU.Core.MemoryDomainDescriptor.h"
#include "../../../Architecture/Common/YAKSys_Hybrid_CPU.Processor.h"
#include "../../Completion/Commit/ArchitecturalCompletionCommitOwner.h"
#include "CpuInstructionTranslationRegions.h"
#include <vector>
#include <limits>
#include <optional>
#include <cstdint>
#include <stdexcept>
#include <any>
#include <mutex>
#include <map>
#include <format>
#include "tangible_exception_helper.h"
#include <memory>

// C# TO C++ CONVERTER NOTE: Forward class declarations:
namespace YAKSys_Hybrid_CPU::Core::Memory { class PhysicalReadBinding; }
namespace HybridCPU_ISE::CloseToHSL::Memory::Timing { class MemoryCycleController; }

namespace YAKSys_Hybrid_CPU::Core::Memory
{

	enum class CpuInstructionTranslationMode : unsigned char
	{
		Identity = 0,
		BoundedRegions = 1,
		TwoStageBoundedRegions = 2
	};

	enum class CpuInstructionTranslationAccessKind : unsigned char
	{
		None = 0,
		InstructionFetch = 1,
		ScalarLoad = 2,
		ScalarStore = 3
	};

	enum class CpuInstructionTranslationFaultReason : unsigned char
	{
		None = 0,
		UnmappedAddress = 1,
		AccessDenied = 2,
		AddressOverflow = 3,
		OwnerScopeMismatch = 4,
		SecondStageViolation = 5,
		SecondStageMisconfiguration = 6,
		SecondStageSourceStale = 7
	};

	enum class CpuInstructionTranslationFaultStage : unsigned char
	{
		StageOne = 1,
		SecondStage = 2
	};

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct CpuInstructionTranslationScope(ulong DomainId, int ContextId, int VirtualThreadId)
//	{
//		public bool IsValid
//		{
//			get
//			{
//				return DomainId != 0 && ContextId > 0 && VirtualThreadId is >= 0 @and < Processor.CPU_Core.SmtWays;
//			}
//		}
//	}

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct CpuInstructionTranslationRegion(ulong VirtualBase, ulong PhysicalBase, ulong Length, bool Readable, bool Writable, bool Executable)
//	{
//		internal bool IsValid
//		{
//			get
//			{
//				return Length != 0 && VirtualBase <= ulong.MaxValue - (Length - 1) && PhysicalBase <= ulong.MaxValue - (Length - 1) && (Readable || Writable || Executable);
//			}
//		}
//	}

	/// <summary>
	/// Explicit stage-one VA-to-GPA mapping. This type is deliberately distinct
	/// from <see cref="CpuInstructionTranslationRegion"/> so a Phase 54 physical
	/// result can never be reinterpreted as a guest-physical address.
	/// </summary>
// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct CpuInstructionStageOneRegion(ulong VirtualBase, ulong GuestPhysicalBase, ulong Length, bool Readable, bool Writable, bool Executable)
//	{
//		internal bool IsValid
//		{
//			get
//			{
//				return Length != 0 && VirtualBase <= ulong.MaxValue - (Length - 1) && GuestPhysicalBase <= ulong.MaxValue - (Length - 1) && (Readable || Writable || Executable);
//			}
//		}
//	}

    struct CpuInstructionTranslationOperationIdentity final {
        std::uint64_t AttemptId, EventId, MemoryOperationId;
        bool isValid() const { return AttemptId != 0 && EventId != 0 && MemoryOperationId != 0; }
        bool operator==(const CpuInstructionTranslationOperationIdentity&) const = default;
    };
    struct CpuInstructionTranslationRequest final {
        CpuInstructionTranslationScope Scope; CpuInstructionTranslationAccessKind AccessKind;
        std::uint64_t VirtualAddress; std::uint16_t AccessSize; CpuInstructionTranslationOperationIdentity OperationIdentity;
        bool hasOperationIdentity() const { return AccessKind != CpuInstructionTranslationAccessKind::None && AccessSize != 0 && OperationIdentity.isValid(); }
        bool isValid() const { return Scope.isValid() && hasOperationIdentity(); }
        bool operator==(const CpuInstructionTranslationRequest&) const = default;
    };
    struct CpuInstructionTranslationFault final {
        CpuInstructionTranslationFaultReason Reason; CpuInstructionTranslationRequest Request;
        std::uint64_t OwnerEpoch, Qualification, Auxiliary;
        CpuInstructionTranslationFaultStage FaultStage = CpuInstructionTranslationFaultStage::StageOne;
        bool HasGuestPhysicalAddress = false;
        std::uint64_t GuestPhysicalAddress = 0, MemoryDomainOwnerEpoch = 0, AddressSpaceGeneration = 0, AddressSpaceIdentity = 0;
        bool isValid() const { return Reason != CpuInstructionTranslationFaultReason::None && Request.isValid() && OwnerEpoch != 0 && (FaultStage != CpuInstructionTranslationFaultStage::SecondStage || (HasGuestPhysicalAddress && MemoryDomainOwnerEpoch != 0 && AddressSpaceGeneration != 0 && AddressSpaceIdentity != 0)); }
        bool operator==(const CpuInstructionTranslationFault&) const = default;
    };
    struct CpuInstructionTranslationResult final {
        bool Succeeded; std::uint64_t PhysicalAddress, OwnerEpoch;
        std::optional<CpuInstructionTranslationFault> Fault;
        bool HasGuestPhysicalAddress = false; std::uint64_t GuestPhysicalAddress = 0;
        bool operator==(const CpuInstructionTranslationResult&) const = default;
        static CpuInstructionTranslationResult Success(std::uint64_t address, std::uint64_t epoch) { return {true, address, epoch, std::nullopt}; }
        static CpuInstructionTranslationResult Failed(const CpuInstructionTranslationFault& fault) { return {false, 0, fault.OwnerEpoch, fault}; }
    };
	/// <summary>
	/// Immutable CPU-owned address-translation policy. Identity is the production
	/// default; bounded translation is enabled only by explicitly supplying a
	/// mapped policy when constructing a core platform context.
	/// </summary>
	class CpuInstructionTranslationPolicy final : public std::enable_shared_from_this<CpuInstructionTranslationPolicy>
	{
	private:
		const CpuInstructionTranslationMode Mode;
		const std::shared_ptr<CpuInstructionTranslationScope> Scope;
		const std::vector<std::shared_ptr<CpuInstructionTranslationRegion>> Regions;
		const std::vector<std::shared_ptr<CpuInstructionStageOneRegion>> StageOneRegions;
		const std::shared_ptr<Core::MemoryDomainRuntime::CpuSecondStageTranslationBinding> SecondStageBinding;
		

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private CpuInstructionTranslationPolicy(CpuInstructionTranslationMode mode, CpuInstructionTranslationScope scope, ImmutableArray<CpuInstructionTranslationRegion> regions, ImmutableArray<CpuInstructionStageOneRegion> stageOneRegions = default, Core.MemoryDomainRuntime.CpuSecondStageTranslationBinding? secondStageBinding = null)
		CpuInstructionTranslationPolicy(CpuInstructionTranslationMode mode, std::shared_ptr<CpuInstructionTranslationScope> scope, std::vector<std::shared_ptr<CpuInstructionTranslationRegion>> regions, std::vector<std::shared_ptr<CpuInstructionStageOneRegion>> stageOneRegions = {}, std::shared_ptr<Core::MemoryDomainRuntime::CpuSecondStageTranslationBinding> secondStageBinding = nullptr);

	public:
		CpuInstructionTranslationMode getMode() const;

		std::shared_ptr<CpuInstructionTranslationScope> getScope() const;

		std::vector<std::shared_ptr<CpuInstructionTranslationRegion>> getRegions() const;

		std::vector<std::shared_ptr<CpuInstructionStageOneRegion>> getStageOneRegions() const;

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal Core.MemoryDomainRuntime.CpuSecondStageTranslationBinding? SecondStageBinding {get;}
		std::shared_ptr<Core::MemoryDomainRuntime::CpuSecondStageTranslationBinding> getSecondStageBinding() const;

		static std::shared_ptr<CpuInstructionTranslationPolicy> getIdentity();

		static std::shared_ptr<CpuInstructionTranslationPolicy> CreateBoundedRegions(std::shared_ptr<CpuInstructionTranslationScope> scope, std::vector<std::shared_ptr<CpuInstructionTranslationRegion>> &regions);

		static std::shared_ptr<CpuInstructionTranslationPolicy> CreateTwoStageBoundedRegions(std::shared_ptr<CpuInstructionTranslationScope> scope, std::vector<std::shared_ptr<CpuInstructionStageOneRegion>> &stageOneRegions, std::shared_ptr<Core::MemoryDomainRuntime> memoryDomainRuntime, std::shared_ptr<Core::MemoryDomainDescriptor> memoryDomainDescriptor, unsigned long long runtimeAddressSpaceIdentity);
	};

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct CpuInstructionTranslationOperationIdentity(ulong AttemptId, ulong EventId, ulong MemoryOperationId)
//	{
//		public bool IsValid
//		{
//			get
//			{
//				return AttemptId != 0 && EventId != 0 && MemoryOperationId != 0;
//			}
//		}
//	}

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct CpuInstructionTranslationRequest(CpuInstructionTranslationScope Scope, CpuInstructionTranslationAccessKind AccessKind, ulong VirtualAddress, ushort AccessSize, CpuInstructionTranslationOperationIdentity OperationIdentity)
//	{
//		internal bool HasOperationIdentity
//		{
//			get
//			{
//				return AccessKind != CpuInstructionTranslationAccessKind.None && AccessSize != 0 && OperationIdentity.IsValid;
//			}
//		}
//
//		public bool IsValid
//		{
//			get
//			{
//				return Scope.IsValid && HasOperationIdentity;
//			}
//		}
//	}

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct CpuInstructionTranslationFault(CpuInstructionTranslationFaultReason Reason, CpuInstructionTranslationRequest Request, ulong OwnerEpoch, ulong Qualification, ulong Auxiliary, CpuInstructionTranslationFaultStage FaultStage = CpuInstructionTranslationFaultStage.StageOne, bool HasGuestPhysicalAddress = false, ulong GuestPhysicalAddress = 0, ulong MemoryDomainOwnerEpoch = 0, ulong AddressSpaceGeneration = 0, ulong AddressSpaceIdentity = 0)
//	{
//		public bool IsValid
//		{
//			get
//			{
//				return Reason != CpuInstructionTranslationFaultReason.None && Request.IsValid && OwnerEpoch != 0 && (FaultStage != CpuInstructionTranslationFaultStage.SecondStage || (HasGuestPhysicalAddress && MemoryDomainOwnerEpoch != 0 && AddressSpaceGeneration != 0 && AddressSpaceIdentity != 0));
//			}
//		}
//	}

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct CpuInstructionTranslationResult(bool Succeeded, ulong PhysicalAddress, ulong OwnerEpoch, System.Nullable<CpuInstructionTranslationFault> Fault, bool HasGuestPhysicalAddress = false, ulong GuestPhysicalAddress = 0)
//	{
//		public bool operator==(const CpuInstructionTranslationResult&) const = default;
//        static CpuInstructionTranslationResult Success(ulong physicalAddress, ulong ownerEpoch)
//		{
//			return new(true, physicalAddress, ownerEpoch, nullptr);
//		}
//
//		public static CpuInstructionTranslationResult Failed(in CpuInstructionTranslationFault fault)
//		{
//			return new(false, 0, fault.OwnerEpoch, fault);
//		}
//	}

	class CpuInstructionTranslationFaultException final : public std::runtime_error, public std::enable_shared_from_this<CpuInstructionTranslationFaultException>
	{
	private:
		const std::shared_ptr<CpuInstructionTranslationFault> Fault;

	public:
		CpuInstructionTranslationFaultException(std::shared_ptr<CpuInstructionTranslationFault> fault);

		std::shared_ptr<CpuInstructionTranslationFault> getFault() const;
	};

	class CpuInstructionTranslationSourceStaleException final : public std::runtime_error, public std::enable_shared_from_this<CpuInstructionTranslationSourceStaleException>
	{
	private:
		const std::shared_ptr<CpuInstructionTranslationFault> Fault;

	public:
		CpuInstructionTranslationSourceStaleException(std::shared_ptr<CpuInstructionTranslationFault> fault);

		std::shared_ptr<CpuInstructionTranslationFault> getFault() const;
	};

	class CpuInstructionTranslationOwner;
	class PhysicalReadBinding final : public std::enable_shared_from_this<PhysicalReadBinding>
		{
		private:
			const std::shared_ptr<CpuInstructionTranslationOwner> owner;
			std::mutex gate;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private Object? controller;
			std::shared_ptr<HybridCPU_ISE::CloseToHSL::Memory::Timing::MemoryCycleController> controller;
			unsigned long long requestId = 0;
			int phase = 0;
		public:
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal PhysicalReadBinding(CpuInstructionTranslationOwner owner, Object? controller = null)
			PhysicalReadBinding(std::shared_ptr<CpuInstructionTranslationOwner> owner, std::shared_ptr<HybridCPU_ISE::CloseToHSL::Memory::Timing::MemoryCycleController> controller = nullptr);
			bool TryClaim(std::shared_ptr<HybridCPU_ISE::CloseToHSL::Memory::Timing::MemoryCycleController> target, unsigned long long id, unsigned long long address, int size);
			void Close(std::shared_ptr<HybridCPU_ISE::CloseToHSL::Memory::Timing::MemoryCycleController> target, unsigned long long id);
			void RevokeUnclaimed();
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal bool Read(Processor.MainMemoryArea? backing, ulong address, byte[] destination)
			bool Read(std::shared_ptr<Processor::MainMemoryArea> backing, unsigned long long address, std::vector<unsigned char> &destination);
		};

	class CpuInstructionTranslationOwner final : public std::enable_shared_from_this<CpuInstructionTranslationOwner>
	{
        friend class PhysicalReadBinding;
	private:
		const unsigned long long OwnerEpoch;

		        struct PhysicalReadRequest { CpuInstructionTranslationRequest Request; unsigned long long Address; };
        std::mutex physicalReadsGate;
        std::map<std::weak_ptr<PhysicalReadBinding>, PhysicalReadRequest, std::owner_less<std::weak_ptr<PhysicalReadBinding>>> physicalReads;
// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//		private sealed record PhysicalReadRequest(CpuInstructionTranslationRequest Request, ulong Address);
	public:
		using PhysicalReadBinding = YAKSys_Hybrid_CPU::Core::Memory::PhysicalReadBinding;

	public:
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal PhysicalReadBinding? BindPhysicalRead(in CpuInstructionTranslationRequest request, in CpuInstructionTranslationResult result, System.Nullable<Object> controller)
		std::shared_ptr<PhysicalReadBinding> BindPhysicalRead(std::shared_ptr<CpuInstructionTranslationRequest> request, std::shared_ptr<CpuInstructionTranslationResult> result, std::shared_ptr<HybridCPU_ISE::CloseToHSL::Memory::Timing::MemoryCycleController> controller);

	private:
		bool HasPhysicalRead(std::shared_ptr<PhysicalReadBinding> binding, unsigned long long address, int size);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private bool ReadPhysical(PhysicalReadBinding binding, Processor.MainMemoryArea? backing, ulong address, byte[] destination)
		bool ReadPhysical(std::shared_ptr<PhysicalReadBinding> binding, std::shared_ptr<Processor::MainMemoryArea> backing, unsigned long long address, std::vector<unsigned char> &destination);
		const std::shared_ptr<CpuInstructionTranslationPolicy> _policy;
		const std::shared_ptr<Processor::MainMemoryArea> _physicalMemory;
		unsigned long long _nextFetchIdentity = 0;
		unsigned long long _nextFallbackMemoryIdentity = 0;

	public:
// C# TO C++ CONVERTER TASK: Throw expressions are not converted by C# to C++ Converter:
// ORIGINAL LINE: internal CpuInstructionTranslationOwner(CpuInstructionTranslationPolicy policy, Processor.MainMemoryArea physicalMemory)
		CpuInstructionTranslationOwner(std::shared_ptr<CpuInstructionTranslationPolicy> policy, std::shared_ptr<Processor::MainMemoryArea> physicalMemory);

		unsigned long long getOwnerEpoch() const;

		bool isIdentity() const;

		// Cache eligibility only. Execute permission still comes from Translate
		// on the original complete instruction-fetch request at each boundary.
		bool getHasImmutableInstructionMapping() const;

		std::shared_ptr<CpuInstructionTranslationScope> getConfiguredScope() const;

		void BindCompletionLifecycle(std::shared_ptr<Core::ArchitecturalCompletionCommitOwner> completionOwner);

		std::shared_ptr<CpuInstructionTranslationOperationIdentity> IssueFetchIdentity(unsigned long long virtualAddress);

		std::shared_ptr<CpuInstructionTranslationOperationIdentity> IssueFallbackMemoryIdentity(CpuInstructionTranslationAccessKind accessKind, unsigned long long virtualAddress);

		std::shared_ptr<CpuInstructionTranslationResult> Translate(std::shared_ptr<CpuInstructionTranslationRequest> request);

	private:
		std::shared_ptr<CpuInstructionTranslationResult> TranslateTwoStage(std::shared_ptr<CpuInstructionTranslationRequest> request, unsigned long long requestEnd);

		std::shared_ptr<CpuInstructionTranslationResult> SecondStageFault(CpuInstructionTranslationFaultReason reason, std::shared_ptr<CpuInstructionTranslationRequest> request, unsigned long long guestPhysicalAddress, std::shared_ptr<Core::CpuSecondStageTranslationSnapshot> snapshot, unsigned long long auxiliary);

		static bool Allows(std::shared_ptr<CpuInstructionStageOneRegion> region, CpuInstructionTranslationAccessKind accessKind);

		std::shared_ptr<CpuInstructionTranslationResult> Fault(CpuInstructionTranslationFaultReason reason, std::shared_ptr<CpuInstructionTranslationRequest> request);

	public:
		static unsigned long long CreateMemoryOperationIdentity(unsigned long long attemptId, unsigned long long eventId, CpuInstructionTranslationAccessKind accessKind, unsigned long long virtualAddress);

	private:
		static unsigned long long MixNonZero(unsigned long long left, unsigned long long right);

		static unsigned long long RotateLeft(unsigned long long value, int count);
	};

}
