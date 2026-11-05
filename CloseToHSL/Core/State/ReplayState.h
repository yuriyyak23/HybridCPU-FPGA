#pragma once

#include "../Pipeline/Stages/Fetch/LoopBuffer.h"
#include "../Decoder/Rf09ReplaySemanticShadowLookup.h"
#include <memory>

using namespace YAKSys_Hybrid_CPU::Core::Decoder;

namespace YAKSys_Hybrid_CPU::Core
{

	/// <summary>
	/// Reference-owned containment for per-core loop replay and decode-context
	/// identity. It is not a pipeline image, rename checkpoint or rollback owner.
	/// </summary>
	class ReplayState final : public std::enable_shared_from_this<ReplayState>
	{
	public:
		LoopBuffer LoopBuffer;
		unsigned long long CodeGenerationEpoch = 0;
		unsigned long long ObservedRelevantMemoryEpoch = 0;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal ReplaySemanticShadowLookup? SemanticShadowLookup;
		std::shared_ptr<ReplaySemanticShadowLookup> SemanticShadowLookup;
	};

}
