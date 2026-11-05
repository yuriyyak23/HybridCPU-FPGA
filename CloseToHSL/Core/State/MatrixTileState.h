#pragma once

#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{

	/// <summary>
	/// Storage-only leaf for MatrixTile capture, stream-invalidation and replay
	/// identity scalars. MatrixTile retire and replay protocols retain authority.
	/// </summary>
	class MatrixTileState final : public std::enable_shared_from_this<MatrixTileState>
	{
	public:
		unsigned long long StreamInvalidationCount = 0;
		unsigned long long NextCaptureOrdinal = 0;
		unsigned long long NextReplayCheckpointOrdinal = 0;
		unsigned long long ReplayInvalidationEpoch = 0;
	};

}
