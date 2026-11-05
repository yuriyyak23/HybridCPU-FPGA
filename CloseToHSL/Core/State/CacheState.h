#pragma once

#include "../Architecture/Common/YAKSys_Hybrid_CPU.Processor.h"
#include <vector>
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{

	/// <summary>
	/// Reference-owned storage for CPU-local L1/L2 cache arrays and their existing
	/// query/replacement cursors. This container owns no timing, coherence, PTW or
	/// memory-publication authority.
	/// </summary>
	class CacheState final : public std::enable_shared_from_this<CacheState>
	{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
// ORIGINAL LINE: internal Processor.CPU_Core.Cache_VLIWBundle_Object[] L1VliwBundles = null!;
	public:
		std::vector<Processor::CPU_Core::Cache_VLIWBundle_Object> L1VliwBundles;
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
// ORIGINAL LINE: internal Processor.CPU_Core.Cache_Data_Object[] L1Data = null!;
		std::vector<Processor::CPU_Core::Cache_Data_Object> L1Data;
		std::vector<Processor::CPU_Core::Cache_VLIWBundle_Object> L2VliwBundles = std::vector<Processor::CPU_Core::Cache_VLIWBundle_Object>(65536);
		std::vector<Processor::CPU_Core::Cache_Data_Object> L2Data = std::vector<Processor::CPU_Core::Cache_Data_Object>(65536);

		unsigned long long MinimumL1Query = 0;
		unsigned long long MinimumL2Query = 0;
		unsigned long long CurrentVliwBundlePosition = 0;
		unsigned long long CurrentDataObjectPosition = 0;
	};

}
