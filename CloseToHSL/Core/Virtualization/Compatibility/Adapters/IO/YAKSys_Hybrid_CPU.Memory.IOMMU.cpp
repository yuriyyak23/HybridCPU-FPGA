#include "YAKSys_Hybrid_CPU.Memory.IOMMU.h"
#include "../../../../Runtime/IO/Dma/DmaDomainBinding.h"
#include "../../../../Runtime/IO/Dma/DmaTranslationResult.h"
#include "../../../../Runtime/Memory/Translation/YAKSys_Hybrid_CPU.Memory.NestedPageWalker.h"

using namespace HybridCPU_ISE::CloseToHSL::Memory::MMU;
using namespace YAKSys_Hybrid_CPU::Core;

namespace YAKSys_Hybrid_CPU::Memory
{

	bool IOMMU::getVmxCompatibilityIoAliasesAreReadOnlyDenied()
	{
		return true;
	}

	void IOMMU::InitializeVmxDmaState()
	{
	}

	std::shared_ptr<IommuDomainBinding> IOMMU::BindVmxDomain(std::shared_ptr<IommuDomainBinding> binding)
	{
		return nullptr;
	}

	bool IOMMU::UnbindVmxDomain(unsigned short vmid, unsigned int domainId, unsigned int deviceId)
	{
		return false;
	}

	bool IOMMU::TryGetVmxDomainBinding(unsigned short vmid, unsigned int domainId, unsigned int deviceId, std::shared_ptr<IommuDomainBinding> &binding)
	{
		binding = nullptr;
		return false;
	}

	bool IOMMU::TryTranslateVmxDma(std::shared_ptr<IommuDomainBinding> binding, unsigned long long ioVirtualAddress, unsigned long long accessSize, IOMMUAccessPermissions requestedPermissions, std::shared_ptr<DmaTranslationResult> &result)
	{
		result = nullptr;
		return false;
	}

	int IOMMU::CountVmxIotlbEntries()
	{
		return 0;
	}

	int IOMMU::InvalidateVmxIotlbAll()
	{
		return 0;
	}

	int IOMMU::InvalidateVmxIotlbByVmid(unsigned short vmid)
	{
		return 0;
	}

	int IOMMU::InvalidateVmxIotlbByDomain(unsigned short vmid, unsigned int domainId)
	{
		return 0;
	}

	int IOMMU::InvalidateVmxIotlbByDevice(unsigned short vmid, unsigned int domainId, unsigned int deviceId)
	{
		return 0;
	}

	int IOMMU::InvalidateVmxIotlbByEpoch(unsigned short vmid, unsigned int domainId, unsigned int deviceId, unsigned long long domainEpoch)
	{
		return 0;
	}

	bool IOMMU::TryAccountNptWriteProtectDirty(std::shared_ptr<NestedTranslationResult> translation, unsigned long long accessSize, std::shared_ptr<VmxDirtyLogStatus> &status)
	{
		status = nullptr;
		return false;
	}

	int IOMMU::ApplyVmxInvalidation(VmxInvalidationScope scope, unsigned long long descriptor, bool isEpt, bool epochWrapped)
	{
		return 0;
	}

	bool IOMMU::getNullDomainGuardEnabled()
	{
		return NullDomainGuardEnabled;
	}

	void IOMMU::setNullDomainGuardEnabled(bool value)
	{
		NullDomainGuardEnabled = value;
	}

	void IOMMU::Initialize()
	{
		PageDirectory = std::make_shared<std::vector<std::shared_ptr<std::vector<unsigned long long>>>>(PAGE_DIR_ENTRIES);
		_threadDomains = std::unordered_map<int, MemoryDomain>();
		_nextFreeAddress = 0x10000000ULL;
		_tlb.FlushAll();
		_ptw.Reset();
		InitializeIoDomainDmaState();
	}

	void IOMMU::RegisterDevice(unsigned long long deviceID)
	{
		// Device registration placeholder for future per-device page tables
	}

	void IOMMU::UnregisterDevice(unsigned long long deviceID)
	{
		// Simplified: in a full implementation, each device would have its own page table
		// For now, clearing is a no-op since we use a shared page table
	}

	bool IOMMU::Map(unsigned long long deviceID, unsigned long long ioVirtualAddress, unsigned long long physicalAddress, unsigned long long size, IOMMUAccessPermissions permissions)
	{
		if (size == 0)
		{
			return false;
		}
		if (PageDirectory == nullptr)
		{
			Initialize();
		}

		unsigned long long numPages = (size + PAGE_SIZE - 1) / PAGE_SIZE;
		unsigned long long permBits = 0;
		if ((static_cast<int>(permissions) & static_cast<int>(IOMMUAccessPermissions::Read)) != 0)
		{
			permBits |= PTE_READ_BIT;
		}
		if ((static_cast<int>(permissions) & static_cast<int>(IOMMUAccessPermissions::Write)) != 0)
		{
			permBits |= PTE_WRITE_BIT;
		}

		for (unsigned long long i = 0; i < numPages; i++)
		{
			unsigned long long currentVA = ioVirtualAddress + (i * PAGE_SIZE);
			unsigned long long currentPA = physicalAddress + (i * PAGE_SIZE);

			unsigned int dirIndex = static_cast<unsigned int>((currentVA >> 22) & 0x3FF);
			unsigned int tableIndex = static_cast<unsigned int>((currentVA >> 12) & 0x3FF);

			if ((*PageDirectory)[dirIndex] == nullptr)
			{
				(*PageDirectory)[dirIndex] = std::make_shared<std::vector<unsigned long long>>(PAGE_TABLE_ENTRIES);
			}

			// Encode PPN + permissions + present bit into 64-bit entry (TLB-style)
			unsigned long long entry = (currentPA & PTE_PPN_MASK) | permBits | PTE_PRESENT_BIT;
			(*(*PageDirectory)[dirIndex])[tableIndex] = entry;
		}

		InvalidateTranslationWarmStateOnMappingMutation();
		return true;
	}

	bool IOMMU::Unmap(unsigned long long deviceID, unsigned long long ioVirtualAddress, unsigned long long size)
	{
		if (size == 0 || PageDirectory == nullptr)
		{
			return false;
		}

		unsigned long long numPages = (size + PAGE_SIZE - 1) / PAGE_SIZE;
		bool anyUnmapped = false;

		for (unsigned long long i = 0; i < numPages; i++)
		{
			unsigned long long currentVA = ioVirtualAddress + (i * PAGE_SIZE);
			unsigned int dirIndex = static_cast<unsigned int>((currentVA >> 22) & 0x3FF);
			unsigned int tableIndex = static_cast<unsigned int>((currentVA >> 12) & 0x3FF);

			if ((*PageDirectory)[dirIndex] != nullptr && ((*(*PageDirectory)[dirIndex])[tableIndex] & PTE_PRESENT_BIT) != 0)
			{
				(*(*PageDirectory)[dirIndex])[tableIndex] = 0; // Clear entry
				anyUnmapped = true;
			}
		}

		if (anyUnmapped)
		{
			InvalidateTranslationWarmStateOnMappingMutation();
		}

		return anyUnmapped;
	}

	bool IOMMU::TranslateAndValidateAccess(unsigned long long deviceID, unsigned long long ioVirtualAddress, unsigned long long accessSize, IOMMUAccessPermissions requestedPermissions, unsigned long long &physicalAddress)
	{
		physicalAddress = 0;
		if (accessSize == 0 || PageDirectory == nullptr)
		{
			return false;
		}

		// === Fast path: TLB lookup (1 cycle in HW) ===
		unsigned char cachedPerms;
		if (_tlb.TryTranslate(ioVirtualAddress, static_cast<int>(deviceID), physicalAddress, cachedPerms))
		{
			if (!HasRequestedPermissions(cachedPerms, requestedPermissions))
			{
				return false;
			}

			return true;
		}

		// === Slow path: full 2-level page walk (MISS_PENALTY_CYCLES) ===
		unsigned char permBits;
		if (!TryResolveMappedPage(ioVirtualAddress, requestedPermissions, physicalAddress, permBits))
		{
			return false;
		}

		_tlb.Insert(ioVirtualAddress, physicalAddress, permBits, static_cast<int>(deviceID));

		return true;
	}

	std::pair<bool, std::wstring> IOMMU::CheckDomainAccess(int threadId, unsigned long long address, unsigned long long size, bool isWrite)
	{
		auto it = _threadDomains.find(threadId);
		if (it == _threadDomains.end()) return {false, std::format(L"Thread {} has no memory domain", threadId)};
		auto& domain = it->second;
		if (!domain.Contains(address, size)) return {false, std::format(L"Access 0x{:X}+{} outside domain [0x{:X}+{}]", address, size, domain.BaseAddress, domain.Size)};
		const auto required = isWrite ? MemoryDomainFlags::Write : MemoryDomainFlags::Read;
		if ((static_cast<int>(domain.Flags) & static_cast<int>(required)) == 0) return {false, isWrite ? L"Write access denied to read-only domain" : L"Read access denied to write-only domain"};
		return {true, L""};
	}
	bool IOMMU::TryWarmTranslationForAssistRange(unsigned long long deviceID, unsigned long long ioVirtualAddress, unsigned long long accessSize, IOMMUAccessPermissions requestedPermissions)
	{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: return TryWarmTranslationForAssistRange(unchecked((int)deviceID), ioVirtualAddress, accessSize, requestedPermissions, checkDomain: false);
		return TryWarmTranslationForAssistRange(static_cast<int>(deviceID), ioVirtualAddress, accessSize, requestedPermissions, false);
	}

	bool IOMMU::TryWarmTranslationForAssistRange(int threadId, unsigned long long ioVirtualAddress, unsigned long long accessSize, IOMMUAccessPermissions requestedPermissions, bool checkDomain)
	{
		if (accessSize == 0)
		{
			return true;
		}

		if (PageDirectory == nullptr)
		{
			return false;
		}

		if (checkDomain)
		{
// C# TO C++ CONVERTER TASK: C++ has no equivalent to C# deconstruction declarations:
			auto [allowed, reason] = CheckDomainAccess(threadId, ioVirtualAddress, accessSize, (static_cast<int>(requestedPermissions) & static_cast<int>(IOMMUAccessPermissions::Write)) != 0);

			if (!allowed)
			{
				return false;
			}
		}
		else if (getNullDomainGuardEnabled() && !_threadDomains.empty() && !_threadDomains.contains(threadId))
		{
			return false;
		}

		unsigned long long remaining = accessSize;
		unsigned long long currentVA = ioVirtualAddress;
		while (remaining > 0)
		{
			unsigned long long pageOffset = currentVA & 0xFFF;
			unsigned long long bytesInPage = PAGE_SIZE - pageOffset;
			unsigned long long chunkSize = std::min(bytesInPage, remaining);

			unsigned char cachedPerms;
			unsigned long long cachedPhysicalAddress;
			if (_tlb.TryTranslate(currentVA, threadId, cachedPhysicalAddress, cachedPerms))
			{
				if (!HasRequestedPermissions(cachedPerms, requestedPermissions))
				{
					return false;
				}
			}
			else
			{
				unsigned long long physicalAddress;
				unsigned char permissionBits;
				if (!TryResolveMappedPage(currentVA, requestedPermissions, physicalAddress, permissionBits))
				{
					return false;
				}

				_tlb.Insert(currentVA, physicalAddress, permissionBits, threadId);
			}

			currentVA += chunkSize;
			remaining -= chunkSize;
		}

		return true;
	}

	unsigned long long IOMMU::DMARead(unsigned long long deviceID, unsigned long long ioVirtualAddress, std::vector<unsigned char> &buffer, unsigned long long offset, unsigned long long count)
	{
		if (offset > buffer.size() || count > buffer.size() - static_cast<size_t>(offset)) return 0;
		std::shared_ptr<Processor::MainMemoryArea> mainMemory = CaptureCurrentMainMemory();
		unsigned long long physicalAddress;
		if (TranslateAndValidateAccess(deviceID, ioVirtualAddress, count, IOMMUAccessPermissions::Read, physicalAddress))
		{
			try
			{
				return TryReadCapturedMainMemory(mainMemory, physicalAddress, std::span<unsigned char>(buffer).subspan(static_cast<size_t>(offset), static_cast<size_t>(count))) ? count : 0;
			}
			catch (...)
			{
				return 0;
			}
		}
		return 0;
	}

	unsigned long long IOMMU::DMAWrite(unsigned long long deviceID, unsigned long long ioVirtualAddress, std::vector<unsigned char> &buffer, unsigned long long offset, unsigned long long count)
	{
		if (offset > buffer.size() || count > buffer.size() - static_cast<size_t>(offset)) return 0;
		std::shared_ptr<Processor::MainMemoryArea> mainMemory = CaptureCurrentMainMemory();
		unsigned long long physicalAddress;
		if (TranslateAndValidateAccess(deviceID, ioVirtualAddress, count, IOMMUAccessPermissions::Write, physicalAddress))
		{
			try
			{
				bool written = TryWriteCapturedMainMemory(mainMemory, physicalAddress, std::span<unsigned char>(buffer).subspan(static_cast<size_t>(offset), static_cast<size_t>(count)));
				if (!written)
				{
					return 0;
				}

				return count;
			}
			catch (...)
			{
			}
		}
		return 0;
	}

	bool IOMMU::ReadBurst(unsigned long long deviceID, unsigned long long ioVirtualAddress, std::span<unsigned char> buffer)
	{
		return ReadBurst(deviceID, ioVirtualAddress, buffer, nullptr);
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal static bool ReadBurst(ulong deviceID, ulong ioVirtualAddress, std::span<byte> buffer, Processor.MainMemoryArea? boundMemory)
	bool IOMMU::ReadBurst(unsigned long long deviceID, unsigned long long ioVirtualAddress, std::span<unsigned char> buffer, std::shared_ptr<Processor::MainMemoryArea> boundMemory)
	{
		// Phase §2: Null-domain guard — reject if the thread has no allocated domain
		if (getNullDomainGuardEnabled() && !_threadDomains.empty() && !_threadDomains.contains(static_cast<int>(deviceID)))
		{
			return false;
		}

		// Call thread-aware version with deviceID as threadID (backward compatibility)
		// For CPU operations (deviceID=0), this maps to thread 0
		return ReadBurstCore(static_cast<int>(deviceID), ioVirtualAddress, buffer, false, boundMemory);
	}

	bool IOMMU::ReadBurst(int threadId, unsigned long long ioVirtualAddress, std::span<unsigned char> buffer, bool checkDomain)
	{
		return ReadBurstCore(threadId, ioVirtualAddress, buffer, checkDomain, nullptr);
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private static bool ReadBurstCore(int threadId, ulong ioVirtualAddress, std::span<byte> buffer, bool checkDomain, Processor.MainMemoryArea? boundMemory)
	bool IOMMU::ReadBurstCore(int threadId, unsigned long long ioVirtualAddress, std::span<unsigned char> buffer, bool checkDomain, std::shared_ptr<Processor::MainMemoryArea> boundMemory)
	{
		if (buffer.size() == 0 || PageDirectory == nullptr)
		{
			return false;
		}
		std::shared_ptr<Processor::MainMemoryArea> mainMemory = (boundMemory != nullptr) ? boundMemory : CaptureCurrentMainMemory();
		if (boundMemory != nullptr)
		{
			(void)boundMemory->getReplayRelevantMutationEpoch();
		}

		// Phase 5: Check memory domain if enabled
		if (checkDomain)
		{
// C# TO C++ CONVERTER TASK: C++ has no equivalent to C# deconstruction declarations:
			auto [allowed, reason] = CheckDomainAccess(threadId, ioVirtualAddress, buffer.size(), false);
			if (!allowed)
			{
				// Domain violation - return false
				return false;
			}
		}

		int totalBytes = buffer.size();
		int offset = 0;
		unsigned long long currentVA = ioVirtualAddress;

		while (offset < totalBytes)
		{
			// Calculate bytes remaining in current page
			unsigned long long pageOffset = currentVA & 0xFFF; // Offset within 4KB page
			int bytesInPage = static_cast<int>(PAGE_SIZE - pageOffset);
			int chunkSize = std::min(bytesInPage, totalBytes - offset);

			// Translate current page
			unsigned long long physicalAddress;
			if (!TranslateAndValidateAccess(static_cast<unsigned long long>(threadId), currentVA, static_cast<unsigned long long>(chunkSize), IOMMUAccessPermissions::Read, physicalAddress))
			{
				return false;
			}

			// Read chunk from physical memory
			if (!TryReadCapturedMainMemory(mainMemory, physicalAddress, buffer.subspan(offset, chunkSize)))
			{
				return false;
			}

			// Move to next chunk
			offset += chunkSize;
			currentVA += static_cast<unsigned long long>(chunkSize);
		}

		return true;
	}

	bool IOMMU::WriteBurst(unsigned long long deviceID, unsigned long long ioVirtualAddress, std::span<const unsigned char> buffer)
	{
		return WriteBurst(deviceID, ioVirtualAddress, buffer, nullptr);
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal static bool WriteBurst(ulong deviceID, ulong ioVirtualAddress, std::span<const byte> buffer, Processor.MainMemoryArea? boundMemory)
	bool IOMMU::WriteBurst(unsigned long long deviceID, unsigned long long ioVirtualAddress, std::span<const unsigned char> buffer, std::shared_ptr<Processor::MainMemoryArea> boundMemory)
	{
		// Phase §2: Null-domain guard — reject if the thread has no allocated domain
		if (getNullDomainGuardEnabled() && !_threadDomains.empty() && !_threadDomains.contains(static_cast<int>(deviceID)))
		{
			return false;
		}

		// Call thread-aware version with deviceID as threadID (backward compatibility)
		// For CPU operations (deviceID=0), this maps to thread 0
		return WriteBurstCore(static_cast<int>(deviceID), ioVirtualAddress, buffer, false, boundMemory);
	}

	bool IOMMU::WriteBurst(int threadId, unsigned long long ioVirtualAddress, std::span<const unsigned char> buffer, bool checkDomain)
	{
		return WriteBurstCore(threadId, ioVirtualAddress, buffer, checkDomain, nullptr);
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private static bool WriteBurstCore(int threadId, ulong ioVirtualAddress, std::span<const byte> buffer, bool checkDomain, Processor.MainMemoryArea? boundMemory)
	bool IOMMU::WriteBurstCore(int threadId, unsigned long long ioVirtualAddress, std::span<const unsigned char> buffer, bool checkDomain, std::shared_ptr<Processor::MainMemoryArea> boundMemory)
	{
		if (buffer.size() == 0 || PageDirectory == nullptr)
		{
			return false;
		}
		std::shared_ptr<Processor::MainMemoryArea> mainMemory = (boundMemory != nullptr) ? boundMemory : CaptureCurrentMainMemory();
		if (boundMemory != nullptr)
		{
			(void)boundMemory->getReplayRelevantMutationEpoch();
		}

		// Phase 5: Check memory domain if enabled
		if (checkDomain)
		{
// C# TO C++ CONVERTER TASK: C++ has no equivalent to C# deconstruction declarations:
			auto [allowed, reason] = CheckDomainAccess(threadId, ioVirtualAddress, buffer.size(), true);
			if (!allowed)
			{
				// Domain violation - return false
				return false;
			}
		}

		int totalBytes = buffer.size();
		int offset = 0;
		unsigned long long currentVA = ioVirtualAddress;

		while (offset < totalBytes)
		{
			// Calculate bytes remaining in current page
			unsigned long long pageOffset = currentVA & 0xFFF; // Offset within 4KB page
			int bytesInPage = static_cast<int>(PAGE_SIZE - pageOffset);
			int chunkSize = std::min(bytesInPage, totalBytes - offset);

			// Translate current page
			unsigned long long physicalAddress;
			if (!TranslateAndValidateAccess(static_cast<unsigned long long>(threadId), currentVA, static_cast<unsigned long long>(chunkSize), IOMMUAccessPermissions::Write, physicalAddress))
			{
				return false;
			}

			// Write chunk to physical memory
			if (!TryWriteCapturedMainMemory(mainMemory, physicalAddress, buffer.subspan(offset, chunkSize)))
			{
				return false;
			}

			// Move to next chunk
			offset += chunkSize;
			currentVA += static_cast<unsigned long long>(chunkSize);
		}

		return true;
	}

	std::shared_ptr<Processor::MainMemoryArea> IOMMU::CaptureCurrentMainMemory()
	{
		return Processor::MainMemory;
	}

	bool IOMMU::TryReadCapturedMainMemory(std::shared_ptr<Processor::MainMemoryArea> mainMemory, unsigned long long physicalAddress, std::span<unsigned char> buffer)
	{
		return mainMemory->TryReadPhysicalRange(physicalAddress, buffer);
	}

	bool IOMMU::TryWriteCapturedMainMemory(std::shared_ptr<Processor::MainMemoryArea> mainMemory, unsigned long long physicalAddress, std::span<const unsigned char> buffer)
	{
		return mainMemory->TryWritePhysicalRange(physicalAddress, buffer);
	}

	bool IOMMU::SubmitPTWalk(unsigned long long virtualAddress, int threadId, int domainId, bool isWrite)
	{
		return _ptw.SubmitWalk(virtualAddress, threadId, domainId, isWrite);
	}

	PageTableWalker::WalkResult IOMMU::AdvancePTW()
	{
		PageTableWalker::WalkResult result = _ptw.AdvanceCycle(PageDirectory);

		// On successful walk, auto-fill TLB
		if (result.Done && !result.Faulted)
		{
			unsigned char permBits = result.Permissions;
			_tlb.Insert(result.VirtualAddress, result.PhysicalAddress, permBits, result.DomainId);
		}

		return result;
	}

	bool IOMMU::isPTWBusy()
	{
		return _ptw.isBusy();
	}

	int IOMMU::getPTWStalledThreadId()
	{
		return _ptw.getStalledThreadId();
	}

	bool IOMMU::AllocateDomain(int threadId, unsigned long long size, MemoryDomainFlags flags)
	{
		if (_threadDomains.empty())
		{
			Initialize();
		}

		// Check if thread already has a domain
	// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
	// ORIGINAL LINE: if (_threadDomains!.ContainsKey(threadId))
		if (_threadDomains.contains(threadId))
		{
			return false; // Domain already allocated
		}

		// Find free region
		unsigned long long baseAddr = FindFreeRegion(size);
		if (baseAddr == 0)
		{
			return false;
		}

		// Check no overlap with existing domains
		for (auto domain : _threadDomains)
		{
			if (domain.second.Overlaps(baseAddr, size))
			{
				return false;
			}
		}

		// Create domain
		MemoryDomain temp_var = MemoryDomain();
		temp_var.ThreadId = threadId;
		temp_var.BaseAddress = baseAddr;
		temp_var.Size = size;
		temp_var.Flags = flags;
		_threadDomains[threadId] = temp_var;

		return true;
	}

	unsigned long long IOMMU::FindFreeRegion(unsigned long long size)
	{
		// Simple bump allocator
		// In production, would need proper free list management
		unsigned long long candidate = _nextFreeAddress;

		// Align to page boundary
		if (candidate % PAGE_SIZE != 0)
		{
			candidate = (candidate / PAGE_SIZE + 1) * PAGE_SIZE;
		}

		_nextFreeAddress = candidate + size;

		// Check if we've exceeded reasonable address space (1GB for now)
		if (_nextFreeAddress > 0x40000000ULL)
		{
			return 0; // Out of address space
		}

		return candidate;
	}

	bool IOMMU::GetDomain(int threadId, MemoryDomain &domain)
	{
		domain = HybridCPU_ISE::CloseToHSL::Memory::MMU::MemoryDomain();

		if (_threadDomains.empty() || !_threadDomains.contains(threadId))
		{
			return false;
		}

		domain = _threadDomains[threadId];
		return true;
	}

	bool IOMMU::FreeDomain(int threadId)
	{
		if (_threadDomains.empty() || !_threadDomains.contains(threadId))
		{
			return false;
		}

		_threadDomains.erase(threadId);
		InvalidateTranslationWarmStateOnDomainChange(threadId);
		return true;
	}

	std::wstring IOMMU::GenerateIsolationProof()
	{
		if (_threadDomains.empty())
		{
			return L"; No domains allocated";
		}

		std::shared_ptr<StringBuilder> sb = std::make_shared<StringBuilder>();
		sb->appendLine(L"; SMT-LIB2 format proof of memory domain isolation");
		sb->appendLine(L"(set-logic QF_BV)");
		sb->appendLine();

		// Declare domain constants
		sb->appendLine(L"; Domain declarations");
		for (auto domain : _threadDomains)
		{
			sb->appendLine(std::format(L"(declare-const domain_{0}_base (_ BitVec 64))", domain.second.ThreadId));
			sb->appendLine(std::format(L"(declare-const domain_{0}_size (_ BitVec 64))", domain.second.ThreadId));
			sb->appendLine(std::format(L"(assert (= domain_{0}_base #x{1:016X}))", domain.second.ThreadId, domain.second.BaseAddress));
			sb->appendLine(std::format(L"(assert (= domain_{0}_size #x{1:016X}))", domain.second.ThreadId, domain.second.Size));
			sb->appendLine();
		}

		// Assert non-overlap for all pairs
		sb->appendLine(L"; Non-overlap assertions");
		std::vector<int> threadIds;
		threadIds.reserve(_threadDomains.size());
		for (const auto& [threadId, domain] : _threadDomains) threadIds.push_back(threadId);
		for (int i = 0; i < threadIds.size(); i++)
		{
			for (int j = i + 1; j < threadIds.size(); j++)
			{
				int tid1 = threadIds[i];
				int tid2 = threadIds[j];

				sb->appendLine(std::format(L"; Domains {0} and {1} do not overlap", tid1, tid2));
				sb->appendLine(L"(assert (or");
				sb->appendLine(std::format(L"  (bvuge domain_{0}_base (bvadd domain_{1}_base domain_{2}_size))", tid1, tid2, tid2));
				sb->appendLine(std::format(L"  (bvuge domain_{0}_base (bvadd domain_{1}_base domain_{2}_size))))", tid2, tid1, tid1));
				sb->appendLine();
			}
		}

		// Check satisfiability
		sb->appendLine(L"(check-sat)");
		sb->appendLine(L"; Expected result: sat (domains are mutually isolated)");

		return sb->toString();
	}

	bool IOMMU::VerifyDomainIsolation()
	{
		if (_threadDomains.empty())
		{
			return true; // No domains = trivially isolated
		}

		std::vector<MemoryDomain> domains;
		domains.reserve(_threadDomains.size());
		for (const auto& [threadId, domain] : _threadDomains) domains.push_back(domain);

		for (int i = 0; i < domains.size(); i++)
		{
			for (int j = i + 1; j < domains.size(); j++)
			{
				if (domains[i].Overlaps(domains[j].BaseAddress, domains[j].Size))
				{
					return false; // Overlap detected
				}
			}
		}

		return true; // All domains are isolated
	}

	bool IOMMU::HasRequestedPermissions(unsigned char permissionBits, IOMMUAccessPermissions requestedPermissions)
	{
		if ((static_cast<int>(requestedPermissions) & static_cast<int>(IOMMUAccessPermissions::Read)) != 0 && (permissionBits & 0x02) == 0)
		{
			return false;
		}

		if ((static_cast<int>(requestedPermissions) & static_cast<int>(IOMMUAccessPermissions::Write)) != 0 && (permissionBits & 0x04) == 0)
		{
			return false;
		}

		return true;
	}

	bool IOMMU::TryResolveMappedPage(unsigned long long ioVirtualAddress, IOMMUAccessPermissions requestedPermissions, unsigned long long &physicalAddress, unsigned char &permissionBits)
	{
		physicalAddress = 0;
		permissionBits = 0;

		if (PageDirectory == nullptr)
		{
			return false;
		}

		unsigned int dirIndex = static_cast<unsigned int>((ioVirtualAddress >> 22) & 0x3FF);
		unsigned int tableIndex = static_cast<unsigned int>((ioVirtualAddress >> 12) & 0x3FF);
		unsigned long long offset = ioVirtualAddress & 0xFFF;

		if ((*PageDirectory)[dirIndex] == nullptr)
		{
			return false;
		}

		unsigned long long entry = (*(*PageDirectory)[dirIndex])[tableIndex];
		if ((entry & PTE_PRESENT_BIT) == 0)
		{
			return false;
		}

		if ((entry & PTE_READ_BIT) != 0)
		{
			permissionBits |= 0x02;
		}
		if ((entry & PTE_WRITE_BIT) != 0)
		{
			permissionBits |= 0x04;
		}

		if (!HasRequestedPermissions(permissionBits, requestedPermissions))
		{
			permissionBits = 0;
			return false;
		}

		physicalAddress = (entry & PTE_PPN_MASK) + offset;
		return true;
	}

	void IOMMU::InvalidateTranslationWarmStateOnMappingMutation()
	{
		_tlb.FlushAll();
		InvalidateIotlbAll();
	}

	void IOMMU::InvalidateTranslationWarmStateOnDomainChange(int threadId)
	{
		_tlb.FlushDomain(threadId);
	}

	bool IOMMU::ResizeDomain(int threadId, unsigned long long newSize)
	{
		if (_threadDomains.empty() || !_threadDomains.contains(threadId))
		{
			return false; // Domain doesn't exist
		}

		MemoryDomain currentDomain = _threadDomains[threadId];

		// If shrinking, no overlap check needed
		if (newSize <= currentDomain.Size)
		{
			MemoryDomain temp_var = MemoryDomain();
			temp_var.ThreadId = threadId;
			temp_var.BaseAddress = currentDomain.BaseAddress;
			temp_var.Size = newSize;
			temp_var.Flags = currentDomain.Flags;
			_threadDomains[threadId] = temp_var;
			InvalidateTranslationWarmStateOnDomainChange(threadId);
			return true;
		}

		// Growing: check if new size would overlap with other domains
		unsigned long long growthSize = newSize - currentDomain.Size;
		unsigned long long newEnd = currentDomain.BaseAddress + newSize;

		for (auto kvp : _threadDomains)
		{
			if (kvp.first == threadId)
			{
				continue; // Skip self
			}

			MemoryDomain otherDomain = kvp.second;

			// Check if growing would overlap with this domain
			if (currentDomain.BaseAddress < (otherDomain.BaseAddress + otherDomain.Size) && newEnd > otherDomain.BaseAddress)
			{
				// Would overlap - try to relocate instead
				return RelocateDomain(threadId, newSize);
			}
		}

		// No overlap, can grow in place
		MemoryDomain temp_var2 = MemoryDomain();
		temp_var2.ThreadId = threadId;
		temp_var2.BaseAddress = currentDomain.BaseAddress;
		temp_var2.Size = newSize;
		temp_var2.Flags = currentDomain.Flags;
		_threadDomains[threadId] = temp_var2;

		InvalidateTranslationWarmStateOnDomainChange(threadId);
		return true;
	}

	bool IOMMU::RelocateDomain(int threadId, unsigned long long newSize)
	{
		if (_threadDomains.empty() || !_threadDomains.contains(threadId))
		{
			return false;
		}

		MemoryDomain currentDomain = _threadDomains[threadId];

		// Find new free region for relocated domain
		unsigned long long newBaseAddr = FindFreeRegion(newSize);
		if (newBaseAddr == 0)
		{
			return false;
		}

		// Check no overlap with existing domains (including the one we're relocating)
		for (auto kvp : _threadDomains)
		{
			if (kvp.first == threadId)
			{
				continue; // Skip domain being relocated
			}

			MemoryDomain otherDomain = kvp.second;
			if (otherDomain.Overlaps(newBaseAddr, newSize))
			{
				return false;
			}
		}

		// Relocate domain
		MemoryDomain temp_var = MemoryDomain();
		temp_var.ThreadId = threadId;
		temp_var.BaseAddress = newBaseAddr;
		temp_var.Size = newSize;
		temp_var.Flags = currentDomain.Flags;
		_threadDomains[threadId] = temp_var;

		InvalidateTranslationWarmStateOnDomainChange(threadId);
		return true;
	}

	unsigned long long IOMMU::AllocateSharedRegion(int sharedRegionId, unsigned long long size, std::vector<int> &threadIds, MemoryDomainFlags flags)
	{
		if (_threadDomains.empty())
		{
			Initialize();
		}

		// Enforce Shared flag
		if ((static_cast<int>(flags) & static_cast<int>(MemoryDomainFlags::Shared)) == 0)
		{
			return 0; // Must have Shared flag
		}

		// Find free region
		unsigned long long baseAddr = FindFreeRegion(size);
		if (baseAddr == 0)
		{
			return 0;
		}

		// Check no overlap with existing domains
	// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
	// ORIGINAL LINE: foreach (MemoryDomain domain in _threadDomains!.Values)
		for (auto domain : _threadDomains)
		{
			if (domain.second.Overlaps(baseAddr, size))
			{
				return 0;
			}
		}

		// Store shared region mapping for each thread
		// Note: In full implementation, would use separate _sharedRegions dictionary
		// For now, using negative thread IDs to distinguish shared regions
		int sharedThreadId = -(sharedRegionId + 1); // -1, -2, -3, etc.

		MemoryDomain temp_var = MemoryDomain();
		temp_var.ThreadId = sharedThreadId;
		temp_var.BaseAddress = baseAddr;
		temp_var.Size = size;
		temp_var.Flags = flags;
		_threadDomains[sharedThreadId] = temp_var;

		return baseAddr;
	}

	bool IOMMU::IsSharedMemory(unsigned long long address)
	{
		if (_threadDomains.empty())
		{
			return false;
		}

		for (auto kvp : _threadDomains)
		{
			// Shared regions have negative thread IDs
			if (kvp.first < 0)
			{
				MemoryDomain domain = kvp.second;
				if (domain.Contains(address, 1))
				{
					return true;
				}
			}
		}

		return false;
	}

	unsigned long long IOMMU::getIotlbMappingEpoch()
	{
		return _iotlbMappingEpoch;
	}

	unsigned long long IOMMU::getIotlbInvalidationEpoch()
	{
		return _iotlbInvalidationEpoch;
	}

	unsigned long long IOMMU::getIotlbHits()
	{
		return IotlbHits;
	}

	void IOMMU::setIotlbHits(unsigned long long value)
	{
		IotlbHits = value;
	}

	unsigned long long IOMMU::getIotlbMisses()
	{
		return IotlbMisses;
	}

	void IOMMU::setIotlbMisses(unsigned long long value)
	{
		IotlbMisses = value;
	}

	void IOMMU::InitializeIoDomainDmaState()
	{
		_ioDomainBindings.clear();
		_iotlb.clear();
		_nextIoDomainEpoch = 1;
		_iotlbMappingEpoch = 1;
		_iotlbInvalidationEpoch = 1;
		setIotlbHits(0);
		setIotlbMisses(0);
	}

	std::shared_ptr<IommuDomainBinding> IOMMU::BindIoDomain(std::shared_ptr<IommuDomainBinding> binding)
	{
		EnsureIoDomainDmaState();
		if (!binding) return nullptr;
		std::shared_ptr<IommuDomainBinding> effective = binding->DomainEpoch == 0 ? std::make_shared<IommuDomainBinding>(binding->WithEpoch(AllocateIoDomainEpoch())) : std::make_shared<IommuDomainBinding>(*binding);

		if (!effective->isValid())
		{
			return nullptr;
		}

		_ioDomainBindings[IoDomainKey{effective->IoDomainTag, effective->DomainId, effective->DeviceId}] = effective;
		InvalidateIotlbByDevice(effective->IoDomainTag, effective->DomainId, effective->DeviceId);
		return effective;
	}

	bool IOMMU::UnbindIoDomain(unsigned short ioDomainTag, unsigned int domainId, unsigned int deviceId)
	{
		EnsureIoDomainDmaState();
	// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
	// ORIGINAL LINE: bool removed = _ioDomainBindings!.Remove(new IoDomainKey(ioDomainTag, domainId, deviceId));
		bool removed = _ioDomainBindings.erase(IoDomainKey{ioDomainTag, domainId, deviceId});
		if (removed)
		{
			InvalidateIotlbByDevice(ioDomainTag, domainId, deviceId);
		}

		return removed;
	}

	bool IOMMU::TryGetIoDomainBinding(unsigned short ioDomainTag, unsigned int domainId, unsigned int deviceId, std::shared_ptr<IommuDomainBinding> &binding)
	{
		EnsureIoDomainDmaState();
	// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
	// ORIGINAL LINE: return _ioDomainBindings!.TryGetValue( new IoDomainKey(ioDomainTag, domainId, deviceId), out binding);
		auto it = _ioDomainBindings.find(IoDomainKey{ioDomainTag, domainId, deviceId});
		if (it == _ioDomainBindings.end()) { binding.reset(); return false; }
		binding = it->second;
		return true;
	}

	bool IOMMU::TryTranslateDma(std::shared_ptr<IommuDomainBinding> binding, unsigned long long ioVirtualAddress, unsigned long long accessSize, IOMMUAccessPermissions requestedPermissions, std::shared_ptr<DmaTranslationResult> &result)
	{
		EnsureIoDomainDmaState();

		std::shared_ptr<IommuDomainBinding> current;
		if (!binding || !binding->isValid() || !TryGetIoDomainBinding(binding->IoDomainTag, binding->DomainId, binding->DeviceId, current) || current->DomainTag != binding->DomainTag || current->DomainEpoch != binding->DomainEpoch)
		{
			result = DmaTranslationResult::Fail(ioVirtualAddress, accessSize, requestedPermissions, DmaFault::Abort(DmaFaultKind::MissingDomainBinding, ioVirtualAddress, (static_cast<int>(requestedPermissions) & static_cast<int>(IOMMUAccessPermissions::Write)) != 0, L"DMA translation requires an active I/O-domain tag/domain/device binding."));
			return false;
		}

		if (accessSize == 0 || ioVirtualAddress > std::numeric_limits<unsigned long long>::max() - (accessSize - 1))
		{
			result = DmaTranslationResult::Fail(ioVirtualAddress, accessSize, requestedPermissions, DmaFault::Abort(DmaFaultKind::DescriptorFault, ioVirtualAddress, (static_cast<int>(requestedPermissions) & static_cast<int>(IOMMUAccessPermissions::Write)) != 0, L"DMA descriptor range is empty or overflows the IOVA space."));
			return false;
		}

		bool isWrite = (static_cast<int>(requestedPermissions) & static_cast<int>(IOMMUAccessPermissions::Write)) != 0;
		if (!current->Allows(requestedPermissions))
		{
			result = DmaTranslationResult::Fail(ioVirtualAddress, accessSize, requestedPermissions, DmaFault::Abort(DmaFaultKind::PermissionFault, ioVirtualAddress, isWrite, L"DMA I/O-domain permissions reject the requested descriptor access."));
			return false;
		}

		std::shared_ptr<IotlbTag> tag = IotlbTag::Create(current, ioVirtualAddress, requestedPermissions, _iotlbMappingEpoch);
		unsigned long long offset = ioVirtualAddress & 0xFFFULL;
	// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
	// ORIGINAL LINE: if (_iotlb!.TryGetValue(tag, out IotlbEntry entry) && tag.Matches(current, ioVirtualAddress, requestedPermissions, _iotlbMappingEpoch))
		std::shared_ptr<IotlbEntry> entry;
		auto _iotlb_iterator = _iotlb.find(*tag);
		if (_iotlb_iterator != _iotlb.end() && tag->Matches(current, ioVirtualAddress, requestedPermissions, _iotlbMappingEpoch))
		{
			entry = _iotlb_iterator->second;
			setIotlbHits(getIotlbHits() + 1);
			unsigned long long hostPhysicalAddress = (entry->HostPhysicalPageNumber << 12) | offset;
			result = DmaTranslationResult::Success(true, ioVirtualAddress, hostPhysicalAddress, accessSize, requestedPermissions, tag);
			return true;
		}

		setIotlbMisses(getIotlbMisses() + 1);
		unsigned long long physicalAddress;
		if (!TranslateAndValidateAccess(current->DeviceId, ioVirtualAddress, accessSize, requestedPermissions, physicalAddress))
		{
			result = DmaTranslationResult::Fail(ioVirtualAddress, accessSize, requestedPermissions, DmaFault::Abort(DmaFaultKind::IommuTranslationFault, ioVirtualAddress, isWrite, L"DMA IOTLB miss could not be resolved by the IOMMU page tables."));
			return false;
		}

		unsigned char permissionBits = ToIotlbPermissionBits(requestedPermissions);
		_iotlb[*tag] = std::make_shared<IotlbEntry>(IotlbEntry{tag, physicalAddress >> 12, permissionBits});

		result = DmaTranslationResult::Success(false, ioVirtualAddress, physicalAddress, accessSize, requestedPermissions, tag);
		return true;
	}

	int IOMMU::CountIotlbEntries()
	{
		EnsureIoDomainDmaState();
	// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
	// ORIGINAL LINE: return _iotlb!.Count;
		return _iotlb.size();
	}

	int IOMMU::InvalidateIotlbAll()
	{
		EnsureIoDomainDmaState();
	// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
	// ORIGINAL LINE: int count = _iotlb!.Count;
		int count = _iotlb.size();
		_iotlb.clear();
		AdvanceIotlbInvalidationEpoch();
		return count;
	}

	int IOMMU::InvalidateIotlbByIoDomainTag(unsigned short ioDomainTag)
	{
		EnsureIoDomainDmaState();
		return InvalidateIotlbWhere([&] (std::shared_ptr<IotlbTag> tag)
		{
			return tag->IoDomainTag == ioDomainTag;
		});
	}

	int IOMMU::InvalidateIotlbByIoDomain(unsigned short ioDomainTag, unsigned int domainId)
	{
		EnsureIoDomainDmaState();
		return InvalidateIotlbWhere([&] (std::shared_ptr<IotlbTag> tag)
		{
			return tag->IoDomainTag == ioDomainTag && tag->DomainId == domainId;
		});
	}

	int IOMMU::InvalidateIotlbByDevice(unsigned short ioDomainTag, unsigned int domainId, unsigned int deviceId)
	{
		EnsureIoDomainDmaState();
		return InvalidateIotlbWhere([&] (std::shared_ptr<IotlbTag> tag)
		{
			return tag->IoDomainTag == ioDomainTag && tag->DomainId == domainId && tag->DeviceId == deviceId;
		});
	}

	int IOMMU::InvalidateIotlbByEpoch(unsigned short ioDomainTag, unsigned int domainId, unsigned int deviceId, unsigned long long domainEpoch)
	{
		EnsureIoDomainDmaState();
		return InvalidateIotlbWhere([&] (std::shared_ptr<IotlbTag> tag)
		{
			return tag->IoDomainTag == ioDomainTag && tag->DomainId == domainId && tag->DeviceId == deviceId && tag->DomainEpoch == domainEpoch;
		});
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public static bool TranslateGuestAccess(ulong deviceID, ulong guestVirtualAddress, ulong accessSize, NestedMemoryAccessType accessType, MemoryDomainTranslationControl domainControl, out ulong physicalAddress, out NestedTranslationResult translation, ulong secondStageEpoch = 0, ulong addressSpaceTagEpoch = 0, Processor.MainMemoryArea? mainMemory = null)
	bool IOMMU::TranslateGuestAccess(unsigned long long deviceID, unsigned long long guestVirtualAddress, unsigned long long accessSize, NestedMemoryAccessType accessType, std::shared_ptr<MemoryDomainTranslationControl> domainControl, unsigned long long &physicalAddress, std::shared_ptr<NestedTranslationResult> &translation, unsigned long long secondStageEpoch, unsigned long long addressSpaceTagEpoch, std::shared_ptr<Processor::MainMemoryArea> mainMemory)
	{
		physicalAddress = 0;
		if (accessSize == 0)
		{
			translation = NestedTranslationResult::GuestPageFault(guestVirtualAddress, 0, accessType, 0);
			return false;
		}

		if (!domainControl->TranslationEnabled)
		{
			IOMMUAccessPermissions requestedPermissions = ToIommuPermissions(accessType);
			bool singleStage = TranslateAndValidateAccess(deviceID, guestVirtualAddress, accessSize, requestedPermissions, physicalAddress);

			translation = singleStage ? NestedTranslationResult::SingleStage(guestVirtualAddress, physicalAddress, accessType, ToNestedPermissionBits(requestedPermissions)) : NestedTranslationResult::GuestPageFault(guestVirtualAddress, 0, accessType, 0);
			return singleStage;
		}

		if (!domainControl->isValid())
		{
			translation = NestedTranslationResult::SecondStageMisconfiguration(guestVirtualAddress, domainControl->SecondStageRoot, accessType, 0, false);
			return false;
		}

		std::shared_ptr<AddressSpaceId> addressSpace = std::make_shared<AddressSpaceId>(domainControl->ToAddressSpaceId(secondStageEpoch, addressSpaceTagEpoch));
		unsigned long long guestPhysicalAddress;
		unsigned char cachedPermissions;
		unsigned char cachedMemoryType;
		std::shared_ptr<NestedTlbTag> cachedTag;
		if (_tlb.TryTranslateNested(guestVirtualAddress, addressSpace, physicalAddress, guestPhysicalAddress, cachedPermissions, cachedMemoryType, cachedTag))
		{
			if (NestedPageWalker::HasAccessPermission(cachedPermissions, accessType))
			{
				translation = NestedTranslationResult::Success(guestVirtualAddress, guestPhysicalAddress, physicalAddress, accessType, cachedPermissions, cachedMemoryType, cachedTag);
				return true;
			}

			physicalAddress = 0;
			translation = NestedTranslationResult::SecondStageViolation(guestVirtualAddress, guestPhysicalAddress, accessType, 0, false);
			return false;
		}

		translation = NestedPageWalker::TranslateNestedDetailed(domainControl, guestVirtualAddress, accessType, secondStageEpoch, addressSpaceTagEpoch, mainMemory);

		if (!translation->Succeeded)
		{
			physicalAddress = 0;
			return false;
		}

		physicalAddress = translation->HostPhysicalAddress;
		_tlb.InsertNested(guestVirtualAddress, translation->GuestPhysicalAddress, translation->HostPhysicalAddress, translation->Permissions, translation->MemoryType, addressSpace);
		return true;
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public static bool TranslateAndValidateAccess(ulong deviceID, ulong ioVirtualAddress, ulong accessSize, IOMMUAccessPermissions requestedPermissions, MemoryDomainTranslationControl domainControl, out ulong physicalAddress, out NestedTranslationResult translation, ulong secondStageEpoch = 0, ulong addressSpaceTagEpoch = 0, Processor.MainMemoryArea? mainMemory = null)
	bool IOMMU::TranslateAndValidateAccess(unsigned long long deviceID, unsigned long long ioVirtualAddress, unsigned long long accessSize, IOMMUAccessPermissions requestedPermissions, std::shared_ptr<MemoryDomainTranslationControl> domainControl, unsigned long long &physicalAddress, std::shared_ptr<NestedTranslationResult> &translation, unsigned long long secondStageEpoch, unsigned long long addressSpaceTagEpoch, std::shared_ptr<Processor::MainMemoryArea> mainMemory)
	{
		NestedMemoryAccessType accessType = (static_cast<int>(requestedPermissions) & static_cast<int>(IOMMUAccessPermissions::Write)) != 0 ? NestedMemoryAccessType::Write : NestedMemoryAccessType::Read;

		return TranslateGuestAccess(deviceID, ioVirtualAddress, accessSize, accessType, domainControl, physicalAddress, translation, secondStageEpoch, addressSpaceTagEpoch, mainMemory);
	}

	int IOMMU::ApplyIoDomainInvalidation(TranslationInvalidationScope scope, unsigned long long descriptor, bool isSecondStageRoot, bool epochWrapped)
	{
		if (epochWrapped || scope == TranslationInvalidationScope::Global)
		{
			InvalidateIotlbAll();
			return _tlb.FlushNestedAll();
		}

		if (scope == TranslationInvalidationScope::AddressSpace)
		{
			if (!isSecondStageRoot)
			{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: InvalidateIotlbByIoDomainTag(unchecked((ushort)descriptor));
				InvalidateIotlbByIoDomainTag(static_cast<unsigned short>(descriptor));
			}

// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: return isSecondStageRoot ? _tlb.FlushNestedBySecondStageRoot(descriptor) : _tlb.FlushNestedByAddressSpaceTag(unchecked((ushort)descriptor));
			return isSecondStageRoot ? _tlb.FlushNestedBySecondStageRoot(descriptor) : _tlb.FlushNestedByAddressSpaceTag(static_cast<unsigned short>(descriptor));
		}

		if (scope == TranslationInvalidationScope::Address)
		{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: return isSecondStageRoot ? _tlb.FlushNestedBySecondStageRoot(descriptor) : _tlb.FlushNestedByAddressSpaceTag(unchecked((ushort)descriptor));
			return isSecondStageRoot ? _tlb.FlushNestedBySecondStageRoot(descriptor) : _tlb.FlushNestedByAddressSpaceTag(static_cast<unsigned short>(descriptor));
		}

		return 0;
	}

	int IOMMU::CountNestedTlbEntries()
	{
		return _tlb.CountNestedEntries();
	}

	void IOMMU::EnsureIoDomainDmaState()
	{
		// The C++ maps are initialized at construction, including when empty.
	}

	unsigned long long IOMMU::AllocateIoDomainEpoch()
	{
		unsigned long long epoch = _nextIoDomainEpoch++;
		if (_nextIoDomainEpoch == 0)
		{
			_nextIoDomainEpoch = 1;
		}

		return epoch == 0 ? 1 : epoch;
	}

	int IOMMU::InvalidateIotlbWhere(std::function<bool(std::shared_ptr<IotlbTag>)> predicate)
	{
		std::vector<IotlbTag> doomed;
	// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
	// ORIGINAL LINE: foreach (IotlbTag tag in _iotlb!.Keys)
		for (auto tag : _iotlb)
		{
			if (predicate(std::make_shared<IotlbTag>(tag.first)))
			{
				doomed.push_back(tag.first);
			}
		}

		for (int i = 0; i < doomed.size(); i++)
		{
			_iotlb.erase(doomed[i]);
		}

		if (doomed.size() != 0)
		{
			AdvanceIotlbInvalidationEpoch();
		}

		return doomed.size();
	}

	void IOMMU::AdvanceIotlbInvalidationEpoch()
	{
//		unchecked
			_iotlbInvalidationEpoch++;
			_iotlbMappingEpoch++;
			if (_iotlbInvalidationEpoch == 0)
			{
				_iotlbInvalidationEpoch = 1;
			}

			if (_iotlbMappingEpoch == 0)
			{
				_iotlbMappingEpoch = 1;
			}
// C# TO C++ CONVERTER TASK: End of the original C# 'unchecked' block.
	}

	unsigned char IOMMU::ToIotlbPermissionBits(IOMMUAccessPermissions permissions)
	{
		unsigned char bits = 0;
		if ((static_cast<int>(permissions) & static_cast<int>(IOMMUAccessPermissions::Read)) != 0)
		{
			bits |= 0x02;
		}

		if ((static_cast<int>(permissions) & static_cast<int>(IOMMUAccessPermissions::Write)) != 0)
		{
			bits |= 0x04;
		}

		return bits;
	}

	IOMMUAccessPermissions IOMMU::ToIommuPermissions(NestedMemoryAccessType accessType)
	{
		return accessType == NestedMemoryAccessType::Write ? IOMMUAccessPermissions::Write : IOMMUAccessPermissions::Read;
	}

	unsigned char IOMMU::ToNestedPermissionBits(IOMMUAccessPermissions permissions)
	{
		unsigned char bits = 0;
		if ((static_cast<int>(permissions) & static_cast<int>(IOMMUAccessPermissions::Read)) != 0)
		{
			bits |= NestedPageWalker::ReadPermission;
		}

		if ((static_cast<int>(permissions) & static_cast<int>(IOMMUAccessPermissions::Write)) != 0)
		{
			bits |= NestedPageWalker::WritePermission;
		}

		return bits;
	}

	int IOMMU::ApplyTranslationInvalidation(TranslationInvalidationScope scope, unsigned long long descriptor, unsigned long long guestPhysicalAddress, bool isSecondStageRoot, bool epochWrapped)
	{
		return ApplyIoDomainInvalidation(scope, descriptor, isSecondStageRoot, epochWrapped);
	}
}
