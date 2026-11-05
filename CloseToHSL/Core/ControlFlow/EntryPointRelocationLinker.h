#pragma once

#include "ControlFlowContractExceptions.h"
#include <vector>
#include <memory>
#include <span>

namespace YAKSys_Hybrid_CPU::Core::ControlFlow
{
	/// <summary>
	/// Applies serialized entry-point relocations to an emitted bundle image.
	/// Keeps control-flow patching on an explicit linker-like surface instead of
	/// reopening direct MainMemory side effects in runtime helpers.
	/// </summary>
	class EntryPointRelocationLinker final
	{
	public:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: public static byte[] ApplyRelocations(byte[] programImage, ulong imageBaseAddress, IReadOnlyList<RelocationEntry> relocations)
		static std::vector<unsigned char> ApplyRelocations(std::vector<unsigned char> &programImage, unsigned long long imageBaseAddress, const std::vector<std::shared_ptr<RelocationEntry>> &relocations);

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: public static void ApplyRelocationsInPlace(byte[] programImage, ulong imageBaseAddress, IReadOnlyList<RelocationEntry> relocations)
		static void ApplyRelocationsInPlace(std::vector<unsigned char> &programImage, unsigned long long imageBaseAddress, const std::vector<std::shared_ptr<RelocationEntry>> &relocations);

	private:
		static void ApplyRelocation(std::span<unsigned char> programImage, unsigned long long imageBaseAddress, std::shared_ptr<RelocationEntry> relocation);

		static void ApplyLegacyAbsolute64(std::span<unsigned char> programImage, unsigned long long imageBaseAddress, std::shared_ptr<RelocationEntry> relocation);

		static int ResolvePatchOffset(int imageLength, unsigned long long imageBaseAddress, std::shared_ptr<RelocationEntry> relocation);

		static RelocationPatchOutOfRangeException CreatePatchOutOfRangeException(unsigned long long imageBaseAddress, int imageLength, std::shared_ptr<RelocationEntry> relocation);
	};
}
