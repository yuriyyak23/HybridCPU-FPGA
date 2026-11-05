#pragma once

#include "../Execution/Memory/Scratch/ScratchBankController.h"
#include <vector>
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{

	/// <summary>
	/// Reference-owned storage for the core-local StreamEngine/vector scratch
	/// buffers, buffer selector and bank-conflict controllers. This container is
	/// neither architectural state nor a timed-memory authority.
	/// </summary>
	class ScratchState final : public std::enable_shared_from_this<ScratchState>
	{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
// ORIGINAL LINE: internal byte[] ScratchA = null!;
	public:
		std::vector<unsigned char> ScratchA;
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
// ORIGINAL LINE: internal byte[] ScratchB = null!;
		std::vector<unsigned char> ScratchB;
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
// ORIGINAL LINE: internal byte[] ScratchDst = null!;
		std::vector<unsigned char> ScratchDst;
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
// ORIGINAL LINE: internal byte[] ScratchIndex = null!;
		std::vector<unsigned char> ScratchIndex;

// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
// ORIGINAL LINE: internal byte[] ScratchA_DB0 = null!;
		std::vector<unsigned char> ScratchA_DB0;
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
// ORIGINAL LINE: internal byte[] ScratchB_DB0 = null!;
		std::vector<unsigned char> ScratchB_DB0;
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
// ORIGINAL LINE: internal byte[] ScratchA_DB1 = null!;
		std::vector<unsigned char> ScratchA_DB1;
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
// ORIGINAL LINE: internal byte[] ScratchB_DB1 = null!;
		std::vector<unsigned char> ScratchB_DB1;
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
// ORIGINAL LINE: internal byte[] ScratchDst_DB0 = null!;
		std::vector<unsigned char> ScratchDst_DB0;
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
// ORIGINAL LINE: internal byte[] ScratchDst_DB1 = null!;
		std::vector<unsigned char> ScratchDst_DB1;

		ScratchBankController BankedScratchA;
		ScratchBankController BankedScratchB;
		ScratchBankController BankedScratchDst;

		int ActiveBufferSet = 0;
	};

}
