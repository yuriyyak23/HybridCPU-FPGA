#pragma once

namespace YAKSys_Hybrid_CPU::Core
{

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct MemoryDomainReadOnlyTranslationView(bool TranslationEnabled, bool AddressSpaceTaggingEnabled, bool OwnsSecondStageTranslation, ulong AddressSpaceRoot, ulong SecondStageRoot, ushort DomainTag, ushort AddressSpaceTag, ulong AddressSpaceGeneration, byte DefaultMemoryType, byte AddressSpaceTargetCount)
//	{
//		public static MemoryDomainReadOnlyTranslationView From(MemoryDomainTranslationControl control, bool ownsSecondStageTranslation)
//		{
//			return new(control.TranslationEnabled, control.AddressSpaceTaggingEnabled, ownsSecondStageTranslation, control.AddressSpaceRoot, control.SecondStageRoot, control.DomainTag, control.AddressSpaceTag, control.AddressSpaceGeneration, control.DefaultMemoryType, control.AddressSpaceTargetCount);
//		}
//	}

}

#include "MemoryDomainTranslationControl.h"
#include <memory>
#include <stdexcept>
namespace YAKSys_Hybrid_CPU::Core {
struct MemoryDomainReadOnlyTranslationView final {
    const bool TranslationEnabled, AddressSpaceTaggingEnabled, OwnsSecondStageTranslation;
    const unsigned long long AddressSpaceRoot, SecondStageRoot;
    const unsigned short DomainTag, AddressSpaceTag;
    const unsigned long long AddressSpaceGeneration;
    const unsigned char DefaultMemoryType, AddressSpaceTargetCount;
    static std::shared_ptr<MemoryDomainReadOnlyTranslationView> From(std::shared_ptr<MemoryDomainTranslationControl> control, bool owns) {
        if (!control) throw std::invalid_argument("control");
        return std::make_shared<MemoryDomainReadOnlyTranslationView>(MemoryDomainReadOnlyTranslationView{control->TranslationEnabled, control->AddressSpaceTaggingEnabled, owns, control->AddressSpaceRoot, control->SecondStageRoot, control->DomainTag, control->AddressSpaceTag, control->AddressSpaceGeneration, control->DefaultMemoryType, control->AddressSpaceTargetCount});
    }
};
}