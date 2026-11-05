#pragma once

#include <limits>
#include "../AddressSpaces/AddressSpaceId.h"
#include <memory>

using namespace YAKSys_Hybrid_CPU::Memory;

namespace YAKSys_Hybrid_CPU::Core
{

	enum class RuntimeEpochAdvanceDecision : unsigned char
	{
		Advanced = 0,
		DeniedWraparound = 1
	};

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct RuntimeEpochAdvanceResult(RuntimeEpochAdvanceDecision Decision, ulong Epoch, bool RequiresDomainFlush)
//	{
//		public bool Succeeded
//		{
//			get
//			{
//				return Decision == RuntimeEpochAdvanceDecision.Advanced;
//			}
//		}
//
//		public static RuntimeEpochAdvanceResult Advanced(ulong epoch)
//		{
//			return new(RuntimeEpochAdvanceDecision.Advanced, epoch, RequiresDomainFlush: false);
//		}
//
//		public static RuntimeEpochAdvanceResult DeniedWraparound(ulong currentEpoch)
//		{
//			return new(RuntimeEpochAdvanceDecision.DeniedWraparound, currentEpoch, RequiresDomainFlush: true);
//		}
//	}

	struct RuntimeEpochAdvanceResult final {
    RuntimeEpochAdvanceDecision Decision; unsigned long long Epoch; bool RequiresDomainFlush;
    bool isSucceeded() const noexcept { return Decision == RuntimeEpochAdvanceDecision::Advanced; }
    static std::shared_ptr<RuntimeEpochAdvanceResult> Advanced(unsigned long long epoch) { return std::make_shared<RuntimeEpochAdvanceResult>(RuntimeEpochAdvanceResult{RuntimeEpochAdvanceDecision::Advanced, epoch, false}); }
    static std::shared_ptr<RuntimeEpochAdvanceResult> DeniedWraparound(unsigned long long epoch) { return std::make_shared<RuntimeEpochAdvanceResult>(RuntimeEpochAdvanceResult{RuntimeEpochAdvanceDecision::DeniedWraparound, epoch, true}); }
};

	class MemoryDomainTranslationEpoch final
	{
	public:
		static std::shared_ptr<RuntimeEpochAdvanceResult> TryAdvanceFailClosed(unsigned long long current);
	};

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct MemoryDomainTranslationControl(bool TranslationEnabled, bool AddressSpaceTaggingEnabled, ulong AddressSpaceRoot, ulong SecondStageRoot, ushort DomainTag, ushort AddressSpaceTag, ulong AddressSpaceGeneration, byte DefaultMemoryType, byte AddressSpaceTargetCount = 0)
//	{
//		public const byte WriteBackMemoryType = 6;
//		public const byte MaxAddressSpaceTargetCount = 4;
//
//		public bool IsValid
//		{
//			get
//			{
//				return !TranslationEnabled || ((AddressSpaceRoot & 0xFFFUL) == 0 && (SecondStageRoot & 0xFFFUL) == 0 && (!AddressSpaceTaggingEnabled || AddressSpaceTag != 0) && AddressSpaceTargetCount <= MaxAddressSpaceTargetCount);
//			}
//		}
//
//		public static MemoryDomainTranslationControl Disabled
//		{
//			get;
//		} = new(TranslationEnabled: false, AddressSpaceTaggingEnabled: false, AddressSpaceRoot: 0, SecondStageRoot: 0, DomainTag: 0, AddressSpaceTag: 0, AddressSpaceGeneration: 0, DefaultMemoryType: WriteBackMemoryType, AddressSpaceTargetCount: 0);
//
//		public AddressSpaceId ToAddressSpaceId(ulong secondStageEpoch, ulong addressSpaceTagEpoch)
//		{
//			return new(DomainTag, AddressSpaceTaggingEnabled ? AddressSpaceTag : (ushort)0, SecondStageRoot, secondStageEpoch, addressSpaceTagEpoch, AddressSpaceGeneration);
//		}
//	}

struct MemoryDomainTranslationControl final {
    bool TranslationEnabled = false, AddressSpaceTaggingEnabled = false;
    unsigned long long AddressSpaceRoot = 0, SecondStageRoot = 0;
    unsigned short DomainTag = 0, AddressSpaceTag = 0;
    unsigned long long AddressSpaceGeneration = 0;
    unsigned char DefaultMemoryType = 6, AddressSpaceTargetCount = 0;
    static constexpr unsigned char WriteBackMemoryType = 6, MaxAddressSpaceTargetCount = 4;
    bool isValid() const noexcept { return !TranslationEnabled || ((AddressSpaceRoot & 0xfffULL) == 0 && (SecondStageRoot & 0xfffULL) == 0 && (!AddressSpaceTaggingEnabled || AddressSpaceTag != 0) && AddressSpaceTargetCount <= MaxAddressSpaceTargetCount); }
    YAKSys_Hybrid_CPU::Memory::AddressSpaceId ToAddressSpaceId(unsigned long long secondStageEpoch, unsigned long long addressSpaceTagEpoch) const noexcept { return {DomainTag, static_cast<unsigned short>(AddressSpaceTaggingEnabled ? AddressSpaceTag : 0), SecondStageRoot, secondStageEpoch, addressSpaceTagEpoch, AddressSpaceGeneration}; }
    static const std::shared_ptr<MemoryDomainTranslationControl> Disabled;
};
inline const std::shared_ptr<MemoryDomainTranslationControl> MemoryDomainTranslationControl::Disabled = std::make_shared<MemoryDomainTranslationControl>();
}
