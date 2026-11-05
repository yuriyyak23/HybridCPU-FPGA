#pragma once

#include "MatrixTileState.h"
#include "../ISA/Instructions/NonVmx/Lanes00_03Vector/MatrixTile/MatrixTileExecuteCaptureAbi.h"
#include "../../Memory/Registers/YAKSys_Hybrid_CPU.Memory.StreamRegisterFile.h"
namespace YAKSys_Hybrid_CPU::CloseToHSL::Core::ISA::Instructions::NonVmx::Lanes00_03Vector::MatrixTile { class MatrixTileReplayRollbackJournal; }
#include "../Execution/DmaStreamCompute/DmaStreamComputeTokenStore.h"
#include "../Execution/ExternalAccelerators/ExternalAcceleratorRuntime.h"
#include <unordered_map>
#include <memory>

using namespace YAKSys_Hybrid_CPU::CloseToHSL::Core::ISA::Instructions::NonVmx::Lanes00_03Vector::MatrixTile;
using namespace YAKSys_Hybrid_CPU::Core::Execution::DmaStreamCompute;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators;
using namespace YAKSys_Hybrid_CPU::Memory;

namespace YAKSys_Hybrid_CPU::Core
{

	/// <summary>
	/// Namespace-only containment for references to distinct extension owners.
	/// It deliberately provides no common execution, commit or fallback ABI.
	/// </summary>
	class ExtensionState final : public std::enable_shared_from_this<ExtensionState>
	{
	public:
		std::shared_ptr<MatrixTileState> MatrixTile = std::make_shared<MatrixTileState>();
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal MatrixTileArchitecturalTileRegisterFile? MatrixTileRegisterFile;
		std::shared_ptr<MatrixTileArchitecturalTileRegisterFile> MatrixTileRegisterFile;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal StreamRegisterFile? MatrixTileStreamRegisterFile;
		std::shared_ptr<StreamRegisterFile> MatrixTileStreamRegisterFile;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal Dictionary<ulong, MatrixTileReplayRollbackJournal>? MatrixTileReplayJournals;
		std::unordered_map<unsigned long long, std::shared_ptr<MatrixTileReplayRollbackJournal>> MatrixTileReplayJournals;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal DmaStreamComputeTokenStore? DmaStreamComputeTokenStore;
		std::shared_ptr<DmaStreamComputeTokenStore> DmaStreamComputeTokenStore;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal ExternalAcceleratorRuntime? ExternalAcceleratorRuntime;
		std::shared_ptr<ExternalAcceleratorRuntime> ExternalAcceleratorRuntime;
	};

}
