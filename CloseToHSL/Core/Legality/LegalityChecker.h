#pragma once

#include "ILegalityChecker.h"
#include "../Pipeline/Safety/SafetyVerifier.Types.h"
#include "../Architecture/Registers/CSR/CsrAccessPolicy.h"
#include "IHazardState.h"
#include "IResourceState.h"
#include <optional>
#include <memory>

using namespace YAKSys_Hybrid_CPU::Arch;
using namespace YAKSys_Hybrid_CPU::Core::Pipeline::MicroOps;
using namespace YAKSys_Hybrid_CPU::Core::Registers;

namespace YAKSys_Hybrid_CPU::Core::Legality
{
	/// <summary>
	/// Default <see cref="ILegalityChecker"/> implementation for HybridCPU ISA v4.
	///
	/// Evaluation order (first failing check wins):
	/// <list type="number">
	///   <item>Privilege check: M-mode-only instruction in a lower privilege mode -&gt; <see cref="LegalityResult.PrivilegeFault"/>.</item>
	///   <item>Data-hazard check: Rs1 or Rs2 has a RAW hazard -&gt; <see cref="LegalityResult.Stall"/>.</item>
	///   <item>Structural-resource check: no execution unit available for the instruction class -&gt; <see cref="LegalityResult.Stall"/>.</item>
	///   <item>Otherwise: <see cref="LegalityResult.Legal"/>.</item>
	/// </list>
	///
	/// ARCHITECTURE RULE: This class is a pipeline-stage service.
	/// It must NOT be called from the decoder.
	/// </summary>
	class LegalityChecker final : public ILegalityChecker
	{
		/// <inheritdoc />
	public:
		LegalityResult Check(std::shared_ptr<InstructionIR> instruction, std::shared_ptr<IHazardState> hazards, std::shared_ptr<IResourceState> resources, PrivilegeLevel currentPrivilege) override;

	private:
		static bool RequiresMachineMode(unsigned short opCode);
	};
}
