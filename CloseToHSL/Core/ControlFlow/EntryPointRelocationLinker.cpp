#include "EntryPointRelocationLinker.h"
#include "RelocationEntry.h"
#include <cstring>
#include <limits>

namespace YAKSys_Hybrid_CPU::Core::ControlFlow
{

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: public static byte[] ApplyRelocations(byte[] programImage, ulong imageBaseAddress, IReadOnlyList<RelocationEntry> relocations)
	std::vector<unsigned char> EntryPointRelocationLinker::ApplyRelocations(std::vector<unsigned char> &programImage, unsigned long long imageBaseAddress, const std::vector<std::shared_ptr<RelocationEntry>> &relocations)
	{
		std::vector<unsigned char> linkedImage = programImage;
		ApplyRelocationsInPlace(linkedImage, imageBaseAddress, relocations);
		return linkedImage;
	}

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: public static void ApplyRelocationsInPlace(byte[] programImage, ulong imageBaseAddress, IReadOnlyList<RelocationEntry> relocations)
	void EntryPointRelocationLinker::ApplyRelocationsInPlace(std::vector<unsigned char> &programImage, unsigned long long imageBaseAddress, const std::vector<std::shared_ptr<RelocationEntry>> &relocations)
	{
		std::span<unsigned char> imageBytes(programImage);
		for (int index = 0; index < relocations.size(); index++)
		{
			ApplyRelocation(imageBytes, imageBaseAddress, relocations[index]);
		}
	}

	void EntryPointRelocationLinker::ApplyRelocation(std::span<unsigned char> programImage, unsigned long long imageBaseAddress, std::shared_ptr<RelocationEntry> relocation)
	{
		switch (relocation->EncodingKind)
		{
			case RelocationEncodingKind::LegacyAbsolute64:
				ApplyLegacyAbsolute64(programImage, imageBaseAddress, relocation);
				return;

			default:
				throw UnsupportedRelocationApplicationException(relocation->EncodingKind, relocation->PatchWidth);
		}
	}

	void EntryPointRelocationLinker::ApplyLegacyAbsolute64(std::span<unsigned char> programImage, unsigned long long imageBaseAddress, std::shared_ptr<RelocationEntry> relocation)
	{
		if (relocation->PatchWidth != RelocationEntry::Absolute64PatchWidth)
		{
			throw UnsupportedRelocationApplicationException(relocation->EncodingKind, relocation->PatchWidth);
		}

		int patchOffset = ResolvePatchOffset(static_cast<int>(programImage.size()), imageBaseAddress, relocation);
		const unsigned long long targetAddress = relocation->ResolvedTargetAddress;
		std::memcpy(programImage.data() + patchOffset, &targetAddress, RelocationEntry::Absolute64PatchWidth);
	}

	int EntryPointRelocationLinker::ResolvePatchOffset(int imageLength, unsigned long long imageBaseAddress, std::shared_ptr<RelocationEntry> relocation)
	{
		if (relocation->PatchAddress < imageBaseAddress)
		{
			throw CreatePatchOutOfRangeException(imageBaseAddress, imageLength, relocation);
		}

		unsigned long long patchOffset = relocation->PatchAddress - imageBaseAddress;
		if (patchOffset > std::numeric_limits<unsigned long long>::max() - relocation->PatchWidth)
		{
			throw CreatePatchOutOfRangeException(imageBaseAddress, imageLength, relocation);
		}
		unsigned long long patchEndExclusive = patchOffset + relocation->PatchWidth;
		if (patchEndExclusive > static_cast<unsigned long long>(imageLength))
		{
			throw CreatePatchOutOfRangeException(imageBaseAddress, imageLength, relocation);
		}

// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'checked' in this context:
// ORIGINAL LINE: return checked((int)patchOffset);
		return static_cast<int>(patchOffset);
	}

	RelocationPatchOutOfRangeException EntryPointRelocationLinker::CreatePatchOutOfRangeException(unsigned long long imageBaseAddress, int imageLength, std::shared_ptr<RelocationEntry> relocation)
	{
		return RelocationPatchOutOfRangeException(imageBaseAddress, imageLength, relocation->PatchAddress, relocation->PatchWidth, relocation->TargetSymbol);
	}
}
