#pragma once
#include <memory>
#include <string>

namespace YAKSys_Hybrid_CPU::Core::ControlFlow
{
	enum class RelocationKind
	{
		AbsoluteJump,
		AbsoluteCall,
		AbsoluteInterrupt
	};

	enum class RelocationEncodingKind
	{
		LegacyAbsolute64
	};

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct RelocationEntry(RelocationKind Kind, ulong PatchAddress, string TargetSymbol, byte PatchWidth, ulong BundlePc, RelocationEncodingKind EncodingKind, ulong ResolvedTargetAddress)
//	{
//		public const byte Absolute64PatchWidth = 8;
//
//		public static RelocationEntry CreateLegacyAbsolute64(RelocationKind kind, ulong emissionCursor, string targetSymbol, ulong resolvedTargetAddress)
//		{
//			ulong patchAddress = EntryPointRelocationEncodingRules.ResolveLegacyAbsoluteTargetPatchAddress(emissionCursor);
//
//			return new RelocationEntry(kind, patchAddress, targetSymbol ?? string.Empty, Absolute64PatchWidth, patchAddress - (patchAddress % EntryPointRelocationEncodingRules.VliwBundleBytes), RelocationEncodingKind.LegacyAbsolute64, resolvedTargetAddress);
//		}
//	}

		struct RelocationEntry final
	{
		static constexpr unsigned char Absolute64PatchWidth = 8;
		RelocationKind Kind;
		unsigned long long PatchAddress;
		std::wstring TargetSymbol;
		unsigned char PatchWidth;
		unsigned long long BundlePc;
		RelocationEncodingKind EncodingKind;
		unsigned long long ResolvedTargetAddress;
		static std::shared_ptr<RelocationEntry> CreateLegacyAbsolute64(RelocationKind kind, unsigned long long emissionCursor, const std::wstring& targetSymbol, unsigned long long resolvedTargetAddress);
	};
class EntryPointRelocationEncodingRules final
	{
	public:
		static constexpr unsigned long long VliwBundleBytes = 256;

		static constexpr unsigned long long LegacyAbsoluteTargetPatchBackOffsetBytes = 16;

		static unsigned long long ResolveLegacyAbsoluteTargetPatchAddress(unsigned long long emissionCursor);
	};
}
