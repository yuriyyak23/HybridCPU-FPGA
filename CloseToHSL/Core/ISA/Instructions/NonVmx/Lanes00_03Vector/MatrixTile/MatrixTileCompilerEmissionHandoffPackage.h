#pragma once

#include "../../../../../Architecture/Common/YAKSys_Hybrid_CPU.Processor.h"
#include "../../../../../../../NonRTL/Arch/InstructionSupportStatus.h"
#include "MatrixTileResourceContour.h"
#include "../../../../../Pipeline/Scheduling/SlotLegality/SlotClassDefinitions.h"
#include <string>
#include <vector>
#include <format>
#include <memory>
#include <utility>

using namespace YAKSys_Hybrid_CPU::Arch;
using namespace YAKSys_Hybrid_CPU::Core;
// C# TO C++ CONVERTER TASK: The C# 'using static' statement  cannot be converted to C++:
//using static YAKSys_Hybrid_CPU.Processor.CPU_Core;

namespace YAKSys_Hybrid_CPU::CloseToHSL::Core::ISA::Instructions::NonVmx::Lanes00_03Vector::MatrixTile
{

	struct MatrixTileCompilerEmissionHandoffRow final
	{
		const std::wstring Mnemonic;
		const InstructionsEnum Opcode;
		const IsaInstructionStatus RuntimeStatus;
		const RuntimeInstructionEvidence RuntimeEvidence;
		const MatrixTileRuntimeResourceClass RuntimeResourceClass;
		const SlotClass RequiredSlotClass;
		const unsigned char PhysicalLaneMask;
		const std::wstring RuntimePublicationKind;
		const bool HasRuntimeExecutableAuthority;
		const bool RequiresDedicatedCompilerImplementation;
		const bool HasCurrentCompilerHelper;
		const bool HasCurrentCompilerEmission;

		MatrixTileCompilerEmissionHandoffRow(
			std::wstring mnemonic, InstructionsEnum opcode, IsaInstructionStatus runtimeStatus,
			RuntimeInstructionEvidence runtimeEvidence, MatrixTileRuntimeResourceClass runtimeResourceClass,
			SlotClass requiredSlotClass, unsigned char physicalLaneMask,
			std::wstring runtimePublicationKind, bool hasRuntimeExecutableAuthority,
			bool requiresDedicatedCompilerImplementation, bool hasCurrentCompilerHelper,
			bool hasCurrentCompilerEmission)
			: Mnemonic(std::move(mnemonic)), Opcode(opcode), RuntimeStatus(runtimeStatus),
			  RuntimeEvidence(runtimeEvidence), RuntimeResourceClass(runtimeResourceClass),
			  RequiredSlotClass(requiredSlotClass), PhysicalLaneMask(physicalLaneMask),
			  RuntimePublicationKind(std::move(runtimePublicationKind)),
			  HasRuntimeExecutableAuthority(hasRuntimeExecutableAuthority),
			  RequiresDedicatedCompilerImplementation(requiresDedicatedCompilerImplementation),
			  HasCurrentCompilerHelper(hasCurrentCompilerHelper),
			  HasCurrentCompilerEmission(hasCurrentCompilerEmission)
		{
		}

		bool operator==(const MatrixTileCompilerEmissionHandoffRow &) const = default;
	};

	class MatrixTileCompilerEmissionHandoffPackage final
	{
	public:
		static inline const std::wstring HandoffDecision = L"ClosedPhase14PositiveCompilerEmissionHandoffPackage";
		static inline const std::wstring StatusCatalogDecision = L"ClosedOptionalEnabledConformanceTestedStatusCatalogPromotion";
		static inline const std::wstring RuntimeAuthorityDecision = L"RuntimeOwnedLegalityAndRetirePublicationRemainFinalIsaAuthority";
		static inline const std::wstring CompilerBoundaryDecision = L"RuntimeHandoffConsumedByExistingPositiveCompilerImplementationCompilerCodeUnchangedByPhase14";
		static inline const std::wstring NoFallbackDecision = L"CompilerMustTargetTypedMatrixTileRuntimePathWithoutFallbackOrHiddenLowering";

		static constexpr bool HasPositiveStatusCatalogPromotion = true;
		static constexpr bool HasPositiveCompilerEmissionHandoffPackage = true;
		static constexpr bool RuntimeAuthorityReadyForSeparateCompilerImplementation = true;
		static constexpr bool CurrentCompilerImplementationExists = true;
		static constexpr bool CurrentCompilerHelperExists = true;
		static constexpr bool CurrentCompilerEmissionExists = true;
		static constexpr bool ModifiesCompilerCode = false;
		static constexpr bool AllowsCompilerToOverrideRuntimeLegality = false;
		static constexpr bool AllowsFallbackOrHiddenLowering = false;

	private:
		static std::shared_ptr<MatrixTileCompilerEmissionHandoffRow> Create(const std::wstring &mnemonic, InstructionsEnum opcode, const std::wstring &runtimePublicationKind);

		static inline const std::vector<std::shared_ptr<MatrixTileCompilerEmissionHandoffRow>> RowTable = {Create(L"MTILE_LOAD", InstructionsEnum::MTILE_LOAD, L"RetireOwnedTilePublication"), Create(L"MTILE_STORE", InstructionsEnum::MTILE_STORE, L"RetireOwnedAllOrNoneMemoryCommit"), Create(L"MTILE_MACC", InstructionsEnum::MTILE_MACC, L"RetireOwnedAccumulatorPublication"), Create(L"MTRANSPOSE", InstructionsEnum::MTRANSPOSE, L"RetireOwnedDestinationTilePublication")};

	public:
		static std::vector<std::shared_ptr<MatrixTileCompilerEmissionHandoffRow>> getRows();

		static std::shared_ptr<MatrixTileCompilerEmissionHandoffRow> GetRow(const std::wstring &mnemonic);

		static void RequireRuntimeExecutableAuthority(const std::wstring &mnemonic);

	};

}
