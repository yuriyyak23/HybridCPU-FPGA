#include "DecoderFeatureFlags.h"
#include "../Frontend/Decode/VliwDecoderV4Bridge/VliwDecoderV4.h"

namespace YAKSys_Hybrid_CPU::Core::Decoder
{

	std::shared_ptr<IDecoderFrontend> DecoderFeatureFlags::CreateDecoder()
	{
		return std::make_shared<VliwDecoderV4>();
	}
}
