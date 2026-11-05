#pragma once

#include "IDecoderFrontend.h"
#include <memory>

namespace YAKSys_Hybrid_CPU::Core::Decoder
{
	/// <summary>
	/// Decoder frontend factory for the live ISA v4 decode path.
	/// Production decode unconditionally routes through
	/// <see cref="VliwDecoderV4"/>.
	/// </summary>
	class DecoderFeatureFlags final
	{
		/// <summary>
		/// Convenience factory kept as a narrow seam for call sites that should not
		/// construct decoder implementations directly.
		/// </summary>
	public:
		static std::shared_ptr<IDecoderFrontend> CreateDecoder();
	};
}
