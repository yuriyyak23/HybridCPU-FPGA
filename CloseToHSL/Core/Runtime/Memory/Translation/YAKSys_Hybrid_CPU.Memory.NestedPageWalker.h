#pragma once

#include "NestedTranslationResult.h"
#include "../../../Architecture/Common/YAKSys_Hybrid_CPU.Processor.h"
#include <memory>

using namespace YAKSys_Hybrid_CPU::Core;


namespace YAKSys_Hybrid_CPU::Memory
{
	/// <summary>
	/// Runtime page walker for composed address-space and second-stage translation.
	/// </summary>
	class NestedPageWalker final
	{
	public:
		static constexpr int NESTED_WALK_LATENCY_CYCLES = 32;

		static constexpr unsigned char ReadPermission = 0x02;
		static constexpr unsigned char WritePermission = 0x04;
		static constexpr unsigned char ExecutePermission = 0x08;

	private:
		static constexpr unsigned long long PPN_MASK = 0xFFFFFFFFFFFFF000ULL;
		static constexpr unsigned long long PRESENT_BIT = 0x1ULL;
		static constexpr unsigned long long READ_BIT = 0x2ULL;
		static constexpr unsigned long long WRITE_BIT = 0x4ULL;
		static constexpr unsigned long long EXECUTE_BIT = 0x8ULL;
		static constexpr unsigned long long RESERVED_MISCONFIG_BIT = 1ULL << 63;

	public:
		static bool HasAccessPermission(unsigned char permissions, NestedMemoryAccessType accessType);

	private:
		static bool WalkGuestPageTable(std::shared_ptr<MemoryDomainTranslationControl> control, unsigned long long guestVirtualAddress, NestedMemoryAccessType accessType, std::shared_ptr<Processor::MainMemoryArea> resolvedMemory, unsigned long long &guestPhysicalAddress, unsigned char &permissions, std::shared_ptr<NestedTranslationResult> &fault);

		static bool WalkSecondStage(std::shared_ptr<MemoryDomainTranslationControl> control, unsigned long long guestPhysicalAddress, unsigned long long guestVirtualAddress, NestedMemoryAccessType accessType, bool causedByPageWalk, int pageWalkLevel, std::shared_ptr<Processor::MainMemoryArea> resolvedMemory, unsigned long long &hostPhysicalAddress, unsigned char &permissions, unsigned char &memoryType, std::shared_ptr<NestedTranslationResult> &fault);

		static unsigned char DecodeGuestPermissions(unsigned long long entry);

		static unsigned char DecodeSecondStagePermissions(unsigned long long entry);

		static bool IsMisconfiguredEntry(unsigned long long entry);


	public:
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public static bool TranslateNested(ulong addressSpaceRoot, ulong secondStageRoot, ulong guestVirtualAddress, out ulong hostPhysicalAddress, out byte permissions, Processor.MainMemoryArea? mainMemory = null)
		static bool TranslateNested(unsigned long long addressSpaceRoot, unsigned long long secondStageRoot, unsigned long long guestVirtualAddress, unsigned long long &hostPhysicalAddress, unsigned char &permissions, std::shared_ptr<Processor::MainMemoryArea> mainMemory = nullptr);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public static NestedTranslationResult TranslateNestedDetailed(MemoryDomainTranslationControl control, ulong guestVirtualAddress, NestedMemoryAccessType accessType, ulong secondStageEpoch, ulong addressSpaceTagEpoch, Processor.MainMemoryArea? mainMemory = null)
		static std::shared_ptr<NestedTranslationResult> TranslateNestedDetailed(std::shared_ptr<MemoryDomainTranslationControl> control, unsigned long long guestVirtualAddress, NestedMemoryAccessType accessType, unsigned long long secondStageEpoch, unsigned long long addressSpaceTagEpoch, std::shared_ptr<Processor::MainMemoryArea> mainMemory = nullptr);
	};
}
