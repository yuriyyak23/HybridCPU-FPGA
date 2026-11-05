#pragma once

#include "../../Architecture/Common/YAKSys_Hybrid_CPU.Processor.h"
#include "../../Decoder/CanonicalDecodedContracts.h"
#include "../../../../Frontend/Contracts/Contracts.h"
#include "../../Contracts/CompilerTransport/VliwBundleAnnotations.h"
#include "../../../../NonRTL/Arch/Compat/HybridCPU_ISE.Arch.VLIW_Instruction.h"
#include <string>
#include <vector>
#include <any>
#include <optional>
#include "tangible_exception_helper.h"
#include <memory>

using namespace HybridCpu::ShadowFrontend;
using namespace HybridCPU_ISE::Arch;
using namespace YAKSys_Hybrid_CPU::Core::Decoder;
// C# TO C++ CONVERTER TASK: The C# 'using static' statement  cannot be converted to C++:
//using static YAKSys_Hybrid_CPU.Processor.CPU_Core;

namespace YAKSys_Hybrid_CPU::Core
{

	// This adapter contains no opcode interpreter, runtime cache or issue authority.
	class ShadowSemanticProjection final
	{
	private:
		static std::shared_ptr<Snapshot> Snap(std::shared_ptr<CanonicalPayloadSnapshot> s);
	public:
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal static Snapshot Object(string kind, Object? value)
		static std::shared_ptr<Snapshot> Object(const std::wstring &kind, std::any value);
	private:
		static std::shared_ptr<CanonicalPayloadSnapshot> Native(std::shared_ptr<const Snapshot> s);

	public:
		static std::shared_ptr<DecodeResult> Project(std::shared_ptr<CanonicalBundle> bundle);

		static std::shared_ptr<DecodeResult> Illegal(std::shared_ptr<FrozenBytes> raw, std::shared_ptr<DecodeFailure> failure);

		static std::wstring SidebandIdentity(std::shared_ptr<VliwBundleAnnotations> annotations);

		static std::shared_ptr<CanonicalBundle> Restore(std::shared_ptr<DecodeResult> facts, std::shared_ptr<BundleRequest> placement, std::shared_ptr<CanonicalDecodeContext> context);

		static std::vector<VLIW_Instruction> ReadRaw(std::shared_ptr<FrozenBytes> bytes);
	};

}
