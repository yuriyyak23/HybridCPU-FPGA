#pragma once

#include "../Architecture/Common/YAKSys_Hybrid_CPU.Processor.h"
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{

	/// <summary>
	/// Reference-owned storage for the current fetch latch, live frontend cursor
	/// and private fetch helpers. Decode progress, committed architectural PC,
	/// cache arrays/replacement state and admission are intentionally excluded.
	/// </summary>
	class FrontendState final : public std::enable_shared_from_this<FrontendState>
	{
	public:
		Processor::CPU_Core::FetchStage Fetch;
		std::shared_ptr<std::vector<unsigned char>> FetchVliwBuffer;
		Processor::CPU_Core::BranchPredictor BranchPredictor;
		unsigned long long ActiveLivePc = 0;
		bool HasMaterializedVliwFetchState = false;
	};

}
