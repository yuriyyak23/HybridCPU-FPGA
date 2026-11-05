#include "BundleCertificate.h"
#include "../MicroOps/Types/MicroOp.h"

namespace YAKSys_Hybrid_CPU::Core
{

	BundleResourceProof::BundleResourceProof()
	{
		Cycle = 0;
		BundleHash = 0;
		AllowedMemoryRangeStart = 0;
		AllowedMemoryRangeEnd = 0;
		ThreadMask = 0;
		Signature.clear();
		Timestamp = std::chrono::system_clock::now();
		IsValid = false;
		VerificationStatus = L"Not verified";
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: public static uint CalculateBundleHash(System.Collections.Generic.IReadOnlyList<MicroOp?> bundle)
	unsigned int BundleResourceProof::CalculateBundleHash(const std::vector<std::shared_ptr<MicroOp>> &bundle)
	{
		if (bundle.empty() || bundle.empty())
		{
			return 0;
		}

		// FNV-1a constants
		constexpr unsigned int FNV_OFFSET_BASIS = 2166136261;
		constexpr unsigned int FNV_PRIME = 16777619;

		unsigned int hash = FNV_OFFSET_BASIS;

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: foreach (MicroOp? op in bundle)
		for (auto op : bundle)
		{
			if (op == nullptr)
			{
				continue;
			}

			// Hash OpCode
			unsigned int opCode = op->getOpCode();
			hash ^= opCode & 0xFF;
			hash *= FNV_PRIME;
			hash ^= (opCode >> 8) & 0xFF;
			hash *= FNV_PRIME;
			hash ^= (opCode >> 16) & 0xFF;
			hash *= FNV_PRIME;
			hash ^= (opCode >> 24) & 0xFF;
			hash *= FNV_PRIME;

			// Hash OwnerThreadId
			hash ^= static_cast<unsigned int>(op->getOwnerThreadId());
			hash *= FNV_PRIME;
		}

		return hash;
	}

	bool BundleResourceProof::VerifySignature()
	{
		// Simulated signature verification only.

		if (Signature.empty() || Signature.empty())
		{
			return false;
		}

		// For ISE: simple length check indicates presence of signature
		return Signature.size() >= 32; // Minimum signature length
	}

	std::wstring BundleResourceProof::ToString()
	{
		return std::format(L"BundleProof[Cycle={}, Hash=0x{:08X}, MemRange=0x{:X}-0x{:X}, ThreadMask=0x{:X}, Valid={}]", Cycle, BundleHash, AllowedMemoryRangeStart, AllowedMemoryRangeEnd, ThreadMask, IsValid ? L"True" : L"False");
	}
}
