#include "RelocationEntry.h"
#include "ControlFlowContractExceptions.h"

namespace YAKSys_Hybrid_CPU::Core::ControlFlow
{
	std::shared_ptr<RelocationEntry> RelocationEntry::CreateLegacyAbsolute64(RelocationKind kind, unsigned long long emissionCursor, const std::wstring &targetSymbol, unsigned long long resolvedTargetAddress)
	{
		const auto patchAddress = EntryPointRelocationEncodingRules::ResolveLegacyAbsoluteTargetPatchAddress(emissionCursor);
		return std::make_shared<RelocationEntry>(RelocationEntry{kind, patchAddress, targetSymbol, Absolute64PatchWidth, patchAddress - (patchAddress % EntryPointRelocationEncodingRules::VliwBundleBytes), RelocationEncodingKind::LegacyAbsolute64, resolvedTargetAddress});
	}

	unsigned long long EntryPointRelocationEncodingRules::ResolveLegacyAbsoluteTargetPatchAddress(unsigned long long emissionCursor)
	{
		if (emissionCursor < LegacyAbsoluteTargetPatchBackOffsetBytes)
		{
			throw RelocationEncodingException(emissionCursor, LegacyAbsoluteTargetPatchBackOffsetBytes);
		}

		return emissionCursor - LegacyAbsoluteTargetPatchBackOffsetBytes;
	}
}
