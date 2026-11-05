#pragma once

#include "../../../../../Memory/MMU/IOMMU.h"
#include "../../FrozenAbi/OperandForms/VmxInvalidationScope.h"
#include "../../../../../Memory/MMU/TLB.h"
#include "../../../../../Memory/MMU/IotlbTag.h"
#include "../../../../../Memory/MMU/PageTableWalker.h"
#include "../../../../../Memory/MMU/MemoryDomain.h"
#include "../../../../Architecture/Common/YAKSys_Hybrid_CPU.Processor.h"
#include "../../../../Runtime/Memory/Translation/NestedTranslationResult.h"
#include "../../../../Runtime/IO/Dma/DmaDomainBinding.h"
#include "../../../../Runtime/Memory/Invalidation/TranslationInvalidationService.h"
#include <string>
#include <unordered_map>
#include <map>
#include <tuple>
#include <vector>
#include <utility>
#include <span>
#include <cmath>
#include <limits>
#include <functional>
#include <format>
#include <memory>
#include "../../../../../../tangible_stringbuilder.h"

// Description: Frozen VMX I/O aliases retained as denied compatibility vocabulary only.
using namespace HybridCPU_ISE::CloseToHSL::Memory::MMU;
using namespace YAKSys_Hybrid_CPU::Core;


// Description: Generic host-side I/O-domain binding, DMA translation, and IOTLB state for IOMMU.

// Description: Generic host-side translation invalidation entrypoint used after runtime memory-domain policy has admitted a request.

namespace YAKSys_Hybrid_CPU::Core { struct VmxDirtyLogStatus; }
namespace YAKSys_Hybrid_CPU::Memory
{
	struct DmaTranslationResult; struct NestedTranslationResult;
	/// <summary>
	/// Access permissions for IOMMU mappings.
	/// </summary>

	/// <summary>
	/// Input-Output Memory Management Unit (IOMMU).
	/// Provides address translation and access control for DMA devices.
	///
	/// Design principles:
	/// - Two-level page table (1024 directories x 1024 pages)
	/// - 4KB page size (matching typical system MMU)
	/// - TLB-style entry format: PPN | Permissions | Present
	/// - HLS-compatible: fixed-size arrays, no Dictionary
	///
	/// Phase 5 additions:
	/// - SIP-style memory domains for thread isolation
	/// - Domain allocation and enforcement
	/// - Formal isolation proof generation
	///
	/// Memory model:
	/// - Devices access memory via IO Virtual Addresses (IOVA)
	/// - IOMMU translates IOVA → Physical Address
	/// - Enforces read/write permissions per mapping
	/// - Per-thread domains ensure Singularity-style isolation
	/// </summary>
	class IOMMU final
	{
	private:
		static inline bool NullDomainGuardEnabled = false;
		static inline unsigned long long IotlbHits = 0;
		static inline unsigned long long IotlbMisses = 0;

	public:
		static bool getVmxCompatibilityIoAliasesAreReadOnlyDenied();

		static void InitializeVmxDmaState();

		static std::shared_ptr<IommuDomainBinding> BindVmxDomain(std::shared_ptr<IommuDomainBinding> binding);

		static bool UnbindVmxDomain(unsigned short vmid, unsigned int domainId, unsigned int deviceId);

		static bool TryGetVmxDomainBinding(unsigned short vmid, unsigned int domainId, unsigned int deviceId, std::shared_ptr<IommuDomainBinding> &binding);

		static bool TryTranslateVmxDma(std::shared_ptr<IommuDomainBinding> binding, unsigned long long ioVirtualAddress, unsigned long long accessSize, IOMMUAccessPermissions requestedPermissions, std::shared_ptr<DmaTranslationResult> &result);

		static int CountVmxIotlbEntries();

		static int InvalidateVmxIotlbAll();

		static int InvalidateVmxIotlbByVmid(unsigned short vmid);

		static int InvalidateVmxIotlbByDomain(unsigned short vmid, unsigned int domainId);

		static int InvalidateVmxIotlbByDevice(unsigned short vmid, unsigned int domainId, unsigned int deviceId);

		static int InvalidateVmxIotlbByEpoch(unsigned short vmid, unsigned int domainId, unsigned int deviceId, unsigned long long domainEpoch);

		static bool TryAccountNptWriteProtectDirty(std::shared_ptr<NestedTranslationResult> translation, unsigned long long accessSize, std::shared_ptr<VmxDirtyLogStatus> &status);

		static int ApplyVmxInvalidation(VmxInvalidationScope scope, unsigned long long descriptor, bool isEpt, bool epochWrapped = false);


		// Two-level page table: 1024 directories x 1024 pages
	private:
		static inline std::shared_ptr<std::vector<std::shared_ptr<std::vector<unsigned long long>>>> PageDirectory;
		static constexpr unsigned long long PAGE_SIZE = 4096;
		static constexpr int PAGE_DIR_ENTRIES = 1024;
		static constexpr int PAGE_TABLE_ENTRIES = 1024;

		// Page table entry bits: [63:12] Physical Page Number | [3:2] Permissions | [0] Present
		static constexpr unsigned long long PTE_PRESENT_BIT = 0x1ULL;
		static constexpr unsigned long long PTE_READ_BIT = 0x2ULL; // Corresponds to IOMMUAccessPermissions.Read (1) shifted left 1
		static constexpr unsigned long long PTE_WRITE_BIT = 0x4ULL; // Corresponds to IOMMUAccessPermissions.Write (2) shifted left 1
		static constexpr unsigned long long PTE_PPN_MASK = 0xFFFFFFFFFFFFF000ULL;

		// HLS-compatible TLB cache (16-entry CAM, single-cycle hit)
		static inline TLB _tlb;

		// Hardware Page Table Walker (Plan 09) — autonomous FSM for TLB miss handling
		static inline PageTableWalker _ptw;

		// Phase 5: SIP memory domains for thread isolation
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private static Dictionary<int, MemoryDomain>? _threadDomains;
		static inline std::unordered_map<int, MemoryDomain> _threadDomains;
		static inline unsigned long long _nextFreeAddress = 0x10000000ULL; // Start allocating from 256MB

		/// <summary>
		/// Phase §2: Null-domain guard. When enabled, burst operations from threads
		/// without an allocated memory domain are rejected even if checkDomain=false.
		/// This prevents uninitialized or null-domain threads from direct (unmasked)
		/// bypass to user-level memory, closing a side-channel vector.
		/// HLS: single AND gate on the burst transaction arbiter.
		/// </summary>
	public:
		static bool getNullDomainGuardEnabled();
		static void setNullDomainGuardEnabled(bool value);

		static void Initialize();

		static void RegisterDevice(unsigned long long deviceID);

		static void UnregisterDevice(unsigned long long deviceID);

		/// <summary>
		/// Maps IO virtual addresses to physical addresses using two-level page table.
		/// </summary>
		static bool Map(unsigned long long deviceID, unsigned long long ioVirtualAddress, unsigned long long physicalAddress, unsigned long long size, IOMMUAccessPermissions permissions);

		/// <summary>
		/// Unmaps IO virtual address range from the page table.
		/// </summary>
		static bool Unmap(unsigned long long deviceID, unsigned long long ioVirtualAddress, unsigned long long size);

		/// <summary>
		/// Translates IOVA to physical address.
		/// Fast path: TLB lookup (1 cycle in HW).
		/// Slow path: 2-level page walk (MISS_PENALTY_CYCLES).
		/// </summary>
		static bool TranslateAndValidateAccess(unsigned long long deviceID, unsigned long long ioVirtualAddress, unsigned long long accessSize, IOMMUAccessPermissions requestedPermissions, unsigned long long &physicalAddress);

		/// <summary>
		/// Warm translation state for an assist/replay-discardable range without
		/// materializing an architectural memory access.
		/// </summary>
		static bool TryWarmTranslationForAssistRange(unsigned long long deviceID, unsigned long long ioVirtualAddress, unsigned long long accessSize, IOMMUAccessPermissions requestedPermissions);

		/// <summary>
		/// Thread-aware translation warm path.
		/// Domain/permission failures stay suppressed on the warm path.
		/// </summary>
		static bool TryWarmTranslationForAssistRange(int threadId, unsigned long long ioVirtualAddress, unsigned long long accessSize, IOMMUAccessPermissions requestedPermissions, bool checkDomain);

		/// <summary>
		/// DMA Read through IOMMU (device reads from system memory).
		/// </summary>
		static unsigned long long DMARead(unsigned long long deviceID, unsigned long long ioVirtualAddress, std::vector<unsigned char> &buffer, unsigned long long offset, unsigned long long count);

		/// <summary>
		/// DMA Write through IOMMU (device writes to system memory).
		/// </summary>
		static unsigned long long DMAWrite(unsigned long long deviceID, unsigned long long ioVirtualAddress, std::vector<unsigned char> &buffer, unsigned long long offset, unsigned long long count);

		/// <summary>
		/// Burst Read - HLS-compatible batch memory read for vector operations.
		/// Handles multi-page bursts by splitting at 4KB boundaries.
		///
		/// Architecture contract:
		/// - Splits burst into page-aligned chunks
		/// - Translates each chunk separately via page table walk
		/// - Ensures no single read operation crosses page boundary
		/// - Returns false if any page translation fails
		/// </summary>
		static bool ReadBurst(unsigned long long deviceID, unsigned long long ioVirtualAddress, std::span<unsigned char> buffer);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal static bool ReadBurst(ulong deviceID, ulong ioVirtualAddress, std::span<byte> buffer, Processor.MainMemoryArea? boundMemory)
		static bool ReadBurst(unsigned long long deviceID, unsigned long long ioVirtualAddress, std::span<unsigned char> buffer, std::shared_ptr<Processor::MainMemoryArea> boundMemory);

		/// <summary>
		/// Burst Read with thread domain checking (Phase 5).
		/// Thread-aware version that validates memory access against thread's domain.
		///
		/// Architecture contract:
		/// - Validates entire burst range is within thread's memory domain
		/// - Splits burst into page-aligned chunks
		/// - Translates each chunk separately via page table walk
		/// - Ensures no single read operation crosses page boundary
		/// - Returns false if domain check or page translation fails
		/// </summary>
		/// <param name="threadId">Hardware thread ID (0-15) performing the read</param>
		/// <param name="ioVirtualAddress">Starting virtual address</param>
		/// <param name="buffer">Destination buffer for read data</param>
		/// <param name="checkDomain">Whether to enforce domain boundary checking</param>
		/// <returns>True if read succeeded, false on domain violation or translation failure</returns>
		static bool ReadBurst(int threadId, unsigned long long ioVirtualAddress, std::span<unsigned char> buffer, bool checkDomain = true);

	private:
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private static bool ReadBurstCore(int threadId, ulong ioVirtualAddress, std::span<byte> buffer, bool checkDomain, Processor.MainMemoryArea? boundMemory)
		static bool ReadBurstCore(int threadId, unsigned long long ioVirtualAddress, std::span<unsigned char> buffer, bool checkDomain, std::shared_ptr<Processor::MainMemoryArea> boundMemory);

		/// <summary>
		/// Burst Write - HLS-compatible batch memory write for vector operations.
		/// Handles multi-page bursts by splitting at 4KB boundaries.
		///
		/// Architecture contract:
		/// - Splits burst into page-aligned chunks
		/// - Translates each chunk separately via page table walk
		/// - Ensures no single write operation crosses page boundary
		/// - Returns false if any page translation fails
		/// </summary>
	public:
		static bool WriteBurst(unsigned long long deviceID, unsigned long long ioVirtualAddress, std::span<const unsigned char> buffer);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal static bool WriteBurst(ulong deviceID, ulong ioVirtualAddress, std::span<const byte> buffer, Processor.MainMemoryArea? boundMemory)
		static bool WriteBurst(unsigned long long deviceID, unsigned long long ioVirtualAddress, std::span<const unsigned char> buffer, std::shared_ptr<Processor::MainMemoryArea> boundMemory);

		/// <summary>
		/// Burst Write with thread domain checking (Phase 5).
		/// Thread-aware version that validates memory access against thread's domain.
		///
		/// Architecture contract:
		/// - Validates entire burst range is within thread's memory domain
		/// - Splits burst into page-aligned chunks
		/// - Translates each chunk separately via page table walk
		/// - Ensures no single write operation crosses page boundary
		/// - Returns false if domain check or page translation fails
		/// </summary>
		/// <param name="threadId">Hardware thread ID (0-15) performing the write</param>
		/// <param name="ioVirtualAddress">Starting virtual address</param>
		/// <param name="buffer">Source buffer with data to write</param>
		/// <param name="checkDomain">Whether to enforce domain boundary checking</param>
		/// <returns>True if write succeeded, false on domain violation or translation failure</returns>
		static bool WriteBurst(int threadId, unsigned long long ioVirtualAddress, std::span<const unsigned char> buffer, bool checkDomain = true);

	private:
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private static bool WriteBurstCore(int threadId, ulong ioVirtualAddress, std::span<const byte> buffer, bool checkDomain, Processor.MainMemoryArea? boundMemory)
		static bool WriteBurstCore(int threadId, unsigned long long ioVirtualAddress, std::span<const unsigned char> buffer, bool checkDomain, std::shared_ptr<Processor::MainMemoryArea> boundMemory);

		static std::shared_ptr<Processor::MainMemoryArea> CaptureCurrentMainMemory();

		static bool TryReadCapturedMainMemory(std::shared_ptr<Processor::MainMemoryArea> mainMemory, unsigned long long physicalAddress, std::span<unsigned char> buffer);

		static bool TryWriteCapturedMainMemory(std::shared_ptr<Processor::MainMemoryArea> mainMemory, unsigned long long physicalAddress, std::span<const unsigned char> buffer);

		// ========== Hardware PTW Integration (Plan 09) ==========

		/// <summary>
		/// Submit a TLB miss to the hardware PTW for asynchronous walk.
		/// Called internally by <see cref="TranslateAndValidateAccess"/> slow path
		/// or externally when a streaming/burst operation encounters a TLB miss.
		///
		/// <para>The requesting thread should be stalled (STALL_PTW_WALK state)
		/// until <see cref="AdvancePTW"/> returns a completed walk result.</para>
		/// </summary>
		/// <param name="virtualAddress">Virtual address that missed TLB.</param>
		/// <param name="threadId">Hardware thread ID (0–15).</param>
		/// <param name="domainId">Domain / ASID for page table root selection.</param>
		/// <param name="isWrite">True if original access was a write.</param>
		/// <returns>True if accepted (started or queued), false if PTW queue full.</returns>
	public:
		static bool SubmitPTWalk(unsigned long long virtualAddress, int threadId, int domainId, bool isWrite);

		/// <summary>
		/// Advance the hardware PTW FSM by one cycle and process results.
		/// Called from MemorySubsystem.AdvanceCycles() once per clock tick.
		///
		/// <para>On successful walk: auto-fills TLB with the translated mapping.
		/// On fault: returns fault indication for the core FSM to handle.</para>
		/// </summary>
		/// <returns>Walk result for this cycle (check <c>Done</c> flag).</returns>
		static PageTableWalker::WalkResult AdvancePTW();

		/// <summary>True if PTW is currently busy with a walk.</summary>
		static bool isPTWBusy();

		/// <summary>Thread ID stalled by PTW, or -1 if idle.</summary>
		static int getPTWStalledThreadId();

		/// <summary>PTW walk statistics: (completed, faulted, totalCycles).</summary>
// C# TO C++ CONVERTER TASK: Methods returning tuples are not converted by C# to C++ Converter:
//		public static (ulong Completed, ulong Faulted, ulong TotalCycles) GetPTWStatistics()
//		{
//			return (_ptw.WalksCompleted, _ptw.WalksFaulted, _ptw.TotalWalkCycles);
//		}

		// ========== Phase 5: SIP Memory Domain Management ==========

		/// <summary>
		/// Allocate memory domain for thread (SIP-style) (Phase 5)
		/// </summary>
		static bool AllocateDomain(int threadId, unsigned long long size, MemoryDomainFlags flags);

		/// <summary>
		/// Find free memory region of requested size (Phase 5)
		/// </summary>
	private:
		static unsigned long long FindFreeRegion(unsigned long long size);

		/// <summary>
		/// Verify memory access respects domain boundaries (Phase 5)
		/// </summary>
// C# TO C++ CONVERTER TASK: Methods returning tuples are not converted by C# to C++ Converter:
		static std::pair<bool, std::wstring> CheckDomainAccess(int threadId, unsigned long long address, unsigned long long size, bool isWrite);
//		public static (bool Allowed, string Reason) CheckDomainAccess(int threadId, ulong address, ulong size, bool isWrite)
//		{
//			if (_threadDomains == nullptr || !_threadDomains.ContainsKey(threadId))
//			{
//				return (false, string.Format("Thread {0} has no memory domain", threadId));
//			}
//
//			MemoryDomain domain = _threadDomains[threadId];
//
//			if (!domain.Contains(address, size))
//			{
//				return (false, string.Format("Access 0x{0:X}+{1} outside domain [0x{2:X}+{3}]", address, size, domain.BaseAddress, domain.Size));
//			}
//
//			if (isWrite && !domain.Flags.HasFlag(MemoryDomainFlags.Write))
//			{
//				return (false, string.Format("Write access denied to read-only domain"));
//			}
//
//			if (!isWrite && !domain.Flags.HasFlag(MemoryDomainFlags.Read))
//			{
//				return (false, string.Format("Read access denied to write-only domain"));
//			}
//
//			return (true, "");
//		}

		/// <summary>
		/// Get memory domain for thread (Phase 5)
		/// </summary>
	public:
		static bool GetDomain(int threadId, MemoryDomain &domain);

		/// <summary>
		/// Free memory domain for thread (Phase 5)
		/// </summary>
		static bool FreeDomain(int threadId);

		/// <summary>
		/// Generate formal proof of domain isolation (Phase 5)
		/// Exports SMT-LIB2 format proof that can be checked by Z3 or other SMT solvers
		/// </summary>
		static std::wstring GenerateIsolationProof();

		/// <summary>
		/// Verify all domains are non-overlapping (Phase 5)
		/// </summary>
		static bool VerifyDomainIsolation();

	private:
		static bool HasRequestedPermissions(unsigned char permissionBits, IOMMUAccessPermissions requestedPermissions);

		static bool TryResolveMappedPage(unsigned long long ioVirtualAddress, IOMMUAccessPermissions requestedPermissions, unsigned long long &physicalAddress, unsigned char &permissionBits);

		static void InvalidateTranslationWarmStateOnMappingMutation();

		static void InvalidateTranslationWarmStateOnDomainChange(int threadId);

		/// <summary>
		/// Get domain statistics (Phase 5)
		/// </summary>
// C# TO C++ CONVERTER TASK: Methods returning tuples are not converted by C# to C++ Converter:
//		public static (int TotalDomains, ulong TotalAllocated, ulong LargestDomain) GetDomainStatistics()
//		{
//			if (_threadDomains == nullptr || _threadDomains.Count == 0)
//			{
//				return (0, 0, 0);
//			}
//
//			int count = _threadDomains.Count;
//			ulong total = 0;
//			ulong largest = 0;
//
//			foreach (MemoryDomain domain in _threadDomains.Values)
//			{
//				total += domain.Size;
//				if (domain.Size > largest)
//				{
//					largest = domain.Size;
//				}
//			}
//
//			return (count, total, largest);
//		}

		/// <summary>
		/// Resize existing memory domain for thread (Phase 5 extension)
		/// This allows dynamic adjustment of domain sizes for different thread workloads.
		///
		/// Important notes:
		/// - Cannot resize if new size causes overlap with other domains
		/// - Preserves existing flags and base address when shrinking
		/// - May relocate domain if growing requires more space
		/// - All existing mappings remain valid after resize (caller must handle remapping)
		/// </summary>
		/// <param name="threadId">Thread ID of domain to resize</param>
		/// <param name="newSize">New size in bytes (must be page-aligned)</param>
		/// <returns>True if resize succeeded, false if failed (overlap or no domain)</returns>
	public:
		static bool ResizeDomain(int threadId, unsigned long long newSize);

		/// <summary>
		/// Relocate domain to new address space with new size (Phase 5 extension)
		/// Used internally by ResizeDomain when growing in place isn't possible.
		/// </summary>
	private:
		static bool RelocateDomain(int threadId, unsigned long long newSize);

		/// <summary>
		/// Allocate shared memory region accessible by multiple threads (Phase 5 extension)
		/// Shared regions require explicit synchronization (mutex/locks) by the caller.
		///
		/// Architecture notes:
		/// - Shared regions are separate from per-thread domains
		/// - All threads accessing shared region must use same flags
		/// - No automatic synchronization - caller must implement locking
		/// - Singularity-style: shared memory is explicit and controlled
		/// </summary>
		/// <param name="sharedRegionId">Unique ID for shared region (user-defined)</param>
		/// <param name="size">Size in bytes</param>
		/// <param name="threadIds">Array of thread IDs that can access this region</param>
		/// <param name="flags">Access flags (must include Shared flag)</param>
		/// <returns>Base address of shared region, or 0 if allocation failed</returns>
	public:
		static unsigned long long AllocateSharedRegion(int sharedRegionId, unsigned long long size, std::vector<int> &threadIds, MemoryDomainFlags flags);

		/// <summary>
		/// Check if address is in shared memory region (Phase 5 extension)
		/// </summary>
		static bool IsSharedMemory(unsigned long long address);


// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//		private readonly record struct IoDomainKey(ushort IoDomainTag, uint DomainId, uint DeviceId);

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//		private readonly record struct IotlbEntry(IotlbTag Tag, ulong HostPhysicalPageNumber, byte PermissionBits);

	private:
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private static Dictionary<IoDomainKey, IommuDomainBinding>? _ioDomainBindings;
		using IoDomainKey = std::tuple<unsigned short, unsigned int, unsigned int>;
		struct IotlbEntry { std::shared_ptr<IotlbTag> Tag; unsigned long long HostPhysicalPageNumber; unsigned char PermissionBits; };
		static inline std::map<IoDomainKey, std::shared_ptr<IommuDomainBinding>> _ioDomainBindings;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private static Dictionary<IotlbTag, IotlbEntry>? _iotlb;
		static inline std::map<IotlbTag, std::shared_ptr<IotlbEntry>> _iotlb;
		static inline unsigned long long _nextIoDomainEpoch = 1;
		static inline unsigned long long _iotlbMappingEpoch = 1;
		static inline unsigned long long _iotlbInvalidationEpoch = 1;

	public:
		static unsigned long long getIotlbMappingEpoch();

		static unsigned long long getIotlbInvalidationEpoch();

		static unsigned long long getIotlbHits();
	private:
		static void setIotlbHits(unsigned long long value);

	public:
		static unsigned long long getIotlbMisses();
	private:
		static void setIotlbMisses(unsigned long long value);

	public:
		static void InitializeIoDomainDmaState();

		static std::shared_ptr<IommuDomainBinding> BindIoDomain(std::shared_ptr<IommuDomainBinding> binding);

		static bool UnbindIoDomain(unsigned short ioDomainTag, unsigned int domainId, unsigned int deviceId);

		static bool TryGetIoDomainBinding(unsigned short ioDomainTag, unsigned int domainId, unsigned int deviceId, std::shared_ptr<IommuDomainBinding> &binding);

		static bool TryTranslateDma(std::shared_ptr<IommuDomainBinding> binding, unsigned long long ioVirtualAddress, unsigned long long accessSize, IOMMUAccessPermissions requestedPermissions, std::shared_ptr<DmaTranslationResult> &result);

		static int CountIotlbEntries();

		static int InvalidateIotlbAll();

		static int InvalidateIotlbByIoDomainTag(unsigned short ioDomainTag);

		static int InvalidateIotlbByIoDomain(unsigned short ioDomainTag, unsigned int domainId);

		static int InvalidateIotlbByDevice(unsigned short ioDomainTag, unsigned int domainId, unsigned int deviceId);

		static int InvalidateIotlbByEpoch(unsigned short ioDomainTag, unsigned int domainId, unsigned int deviceId, unsigned long long domainEpoch);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public static bool TranslateGuestAccess(ulong deviceID, ulong guestVirtualAddress, ulong accessSize, NestedMemoryAccessType accessType, MemoryDomainTranslationControl domainControl, out ulong physicalAddress, out NestedTranslationResult translation, ulong secondStageEpoch = 0, ulong addressSpaceTagEpoch = 0, Processor.MainMemoryArea? mainMemory = null)
		static bool TranslateGuestAccess(unsigned long long deviceID, unsigned long long guestVirtualAddress, unsigned long long accessSize, NestedMemoryAccessType accessType, std::shared_ptr<MemoryDomainTranslationControl> domainControl, unsigned long long &physicalAddress, std::shared_ptr<NestedTranslationResult> &translation, unsigned long long secondStageEpoch = 0, unsigned long long addressSpaceTagEpoch = 0, std::shared_ptr<Processor::MainMemoryArea> mainMemory = nullptr);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public static bool TranslateAndValidateAccess(ulong deviceID, ulong ioVirtualAddress, ulong accessSize, IOMMUAccessPermissions requestedPermissions, MemoryDomainTranslationControl domainControl, out ulong physicalAddress, out NestedTranslationResult translation, ulong secondStageEpoch = 0, ulong addressSpaceTagEpoch = 0, Processor.MainMemoryArea? mainMemory = null)
		static bool TranslateAndValidateAccess(unsigned long long deviceID, unsigned long long ioVirtualAddress, unsigned long long accessSize, IOMMUAccessPermissions requestedPermissions, std::shared_ptr<MemoryDomainTranslationControl> domainControl, unsigned long long &physicalAddress, std::shared_ptr<NestedTranslationResult> &translation, unsigned long long secondStageEpoch = 0, unsigned long long addressSpaceTagEpoch = 0, std::shared_ptr<Processor::MainMemoryArea> mainMemory = nullptr);

		static int ApplyIoDomainInvalidation(TranslationInvalidationScope scope, unsigned long long descriptor, bool isSecondStageRoot, bool epochWrapped = false);

		static int CountNestedTlbEntries();

	private:
		static void EnsureIoDomainDmaState();

		static unsigned long long AllocateIoDomainEpoch();

		static int InvalidateIotlbWhere(std::function<bool(std::shared_ptr<IotlbTag>)> predicate);

		static void AdvanceIotlbInvalidationEpoch();

		static unsigned char ToIotlbPermissionBits(IOMMUAccessPermissions permissions);

		static IOMMUAccessPermissions ToIommuPermissions(NestedMemoryAccessType accessType);

		static unsigned char ToNestedPermissionBits(IOMMUAccessPermissions permissions);


	public:
		static int ApplyTranslationInvalidation(TranslationInvalidationScope scope, unsigned long long descriptor, unsigned long long guestPhysicalAddress, bool isSecondStageRoot, bool epochWrapped = false);
	};
}
