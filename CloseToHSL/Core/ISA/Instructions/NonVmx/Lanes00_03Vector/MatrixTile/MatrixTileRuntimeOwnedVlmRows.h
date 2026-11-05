#pragma once

#include "../../../../../Architecture/Common/YAKSys_Hybrid_CPU.Processor.h"
#include "MatrixTileAccumulatorAndTransposePolicyAbi.h"
#include "../../../../../../../NonRTL/Arch/VectorLegalityMatrix.h"
#include <string>
#include <vector>
#include <functional>
#include <memory>

using namespace YAKSys_Hybrid_CPU::Arch;
// C# TO C++ CONVERTER TASK: The C# 'using static' statement  cannot be converted to C++:
//using static YAKSys_Hybrid_CPU.Processor.CPU_Core;

namespace YAKSys_Hybrid_CPU::CloseToHSL::Core::ISA::Instructions::NonVmx::Lanes00_03Vector::MatrixTile
{
	struct MatrixTileMemoryShapeContract;

	enum class MatrixTileVlmLegalityFaultKind : unsigned char
	{
		None = 0,
		NonMatrixTileOpcode = 1,
		MissingRuntimeOwnedVlmRow = 2,
		NonDescriptorBackedContour = 3,
		MemoryShapeFault = 4,
		MaccSemanticFault = 5,
		TransposeSemanticFault = 6
	};

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct MatrixTileVlmLegalityValidationResult(bool IsLegal, InstructionsEnum Opcode, MatrixTileVlmLegalityFaultKind FaultKind, VectorContourLegalityStatus DescriptorBackedStatus, bool OpensDecoderAdmission, bool OpensExecution, bool UsesFallbackPath)
//	{
//		public static MatrixTileVlmLegalityValidationResult Legal(InstructionsEnum opcode)
//		{
//			return new MatrixTileVlmLegalityValidationResult(IsLegal: true, opcode, MatrixTileVlmLegalityFaultKind.None, VectorContourLegalityStatus.DescriptorOnly, OpensDecoderAdmission: false, OpensExecution: false, UsesFallbackPath: false);
//		}
//
//		public static MatrixTileVlmLegalityValidationResult Fault(InstructionsEnum opcode, MatrixTileVlmLegalityFaultKind faultKind)
//		{
//			return new MatrixTileVlmLegalityValidationResult(IsLegal: false, opcode, faultKind, VectorContourLegalityStatus.FailClosed, OpensDecoderAdmission: false, OpensExecution: false, UsesFallbackPath: false);
//		}
//	}

	struct MatrixTileVlmLegalityValidationResult final
	{
		bool IsLegal = false;
		InstructionsEnum Opcode = static_cast<InstructionsEnum>(0);
		MatrixTileVlmLegalityFaultKind FaultKind = MatrixTileVlmLegalityFaultKind::None;
		VectorContourLegalityStatus DescriptorBackedStatus = VectorContourLegalityStatus::NotApplicable;
		bool OpensDecoderAdmission = false;
		bool OpensExecution = false;
		bool UsesFallbackPath = false;

		MatrixTileVlmLegalityValidationResult() = default;
		MatrixTileVlmLegalityValidationResult(bool isLegal, InstructionsEnum opcode, MatrixTileVlmLegalityFaultKind faultKind,
			VectorContourLegalityStatus descriptorBackedStatus, bool opensDecoderAdmission, bool opensExecution, bool usesFallbackPath)
			: IsLegal(isLegal), Opcode(opcode), FaultKind(faultKind), DescriptorBackedStatus(descriptorBackedStatus),
			  OpensDecoderAdmission(opensDecoderAdmission), OpensExecution(opensExecution), UsesFallbackPath(usesFallbackPath)
		{
		}

		bool IsValid() const noexcept
		{
			return IsLegal && FaultKind == MatrixTileVlmLegalityFaultKind::None &&
				DescriptorBackedStatus == VectorContourLegalityStatus::DescriptorOnly &&
				!OpensDecoderAdmission && !OpensExecution && !UsesFallbackPath;
		}

		static std::shared_ptr<MatrixTileVlmLegalityValidationResult> Legal(InstructionsEnum opcode)
		{
			return std::make_shared<MatrixTileVlmLegalityValidationResult>(true, opcode,
				MatrixTileVlmLegalityFaultKind::None, VectorContourLegalityStatus::DescriptorOnly,
				false, false, false);
		}

		static std::shared_ptr<MatrixTileVlmLegalityValidationResult> Fault(InstructionsEnum opcode, MatrixTileVlmLegalityFaultKind faultKind)
		{
			return std::make_shared<MatrixTileVlmLegalityValidationResult>(false, opcode, faultKind,
				VectorContourLegalityStatus::FailClosed, false, false, false);
		}
	};

	class MatrixTileRuntimeOwnedVlmRows final
	{
	private:
		static inline const std::vector<InstructionsEnum> Opcodes = {InstructionsEnum::MTILE_LOAD, InstructionsEnum::MTILE_STORE, InstructionsEnum::MTILE_MACC, InstructionsEnum::MTRANSPOSE};

	public:
		static inline const std::wstring MatrixTileVlmFamily = L"XMatrix";
		static inline const std::wstring RuntimeOwnedVlmRowsDecision = L"ClosedRuntimeOwnedMatrixTileVlmRows";
		static inline const std::wstring FeatureGateDecision = L"XMatrixExtensionLegalityGateSelected";
		static inline const std::wstring ElementWidthDecision = L"MatrixTileElementWidths1_2_4_8Legal";
		static inline const std::wstring TileShapeDecision = L"CanonicalTileDescriptorRowsColumnsStrideContourSelected";
		static inline const std::wstring MemoryLayoutDecision = L"RowMajorTileMemoryLayoutContourSelected";
		static inline const std::wstring ReservedDisabledDecision = L"ReservedDisabledMatrixTileVlmContoursFailClosed";
		static inline const std::wstring DescriptorBackedDecision = L"DescriptorBackedMatrixTileContourIsDescriptorOnly";
		static inline const std::wstring MetadataNonAuthorityDecision = L"ClassifierAndOptionalDisabledMetadataAreNotVlmAuthority";
		static inline const std::wstring DecoderAdmissionDecision = L"DecoderAdmissionOwnedByPhase06DecoderEncoderAbiNotVlmRows";
		static inline const std::wstring FallbackDecision = L"NoScalarVectorDscLane7VmxOrBackendFallbackAuthority";

		static constexpr bool HasFeatureGate = true;
		static constexpr bool HasElementWidthLegality = true;
		static constexpr bool HasTileShapeLegality = true;
		static constexpr bool HasMemoryLayoutLegality = true;
		static constexpr bool HasReservedDisabledRows = true;
		static constexpr bool KeepsClassifierMetadataNonAuthority = true;
		static constexpr bool KeepsOptionalDisabledStatusNonAuthority = true;
		static constexpr bool KeepsDecoderAdmissionBlocked = true;
		static constexpr bool KeepsCompilerEmissionIndependent = true;
		static constexpr bool KeepsCompilerHandoffBlocked = true;
		static constexpr bool OpensDecoderAdmission = false;
		static constexpr bool OpensExecution = false;
		static constexpr bool UsesFallbackPath = false;

		static std::vector<InstructionsEnum> getOpcodes();

		static bool getHasRuntimeOwnedVlmRows();

		static bool getHasMatrixTileVlmFamily();

		static bool getHasDescriptorBackedOnlyContour();

		static bool getKeepsNonDescriptorContoursFailClosed();

		static bool getKeepsExecutableContoursClosed();

		static bool IsMatrixTileOpcode(InstructionsEnum opcode);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public static bool TryGetRuntimeOwnedRow(InstructionsEnum opcode, out VectorLegalityMatrixRow? row)
		static bool TryGetRuntimeOwnedRow(InstructionsEnum opcode, std::shared_ptr<VectorLegalityMatrixRow> &row);

		static std::shared_ptr<MatrixTileVlmLegalityValidationResult> ValidateLoad(std::shared_ptr<MatrixTileMemoryShapeContract> contract);

		static std::shared_ptr<MatrixTileVlmLegalityValidationResult> ValidateStore(std::shared_ptr<MatrixTileMemoryShapeContract> contract);

		static std::shared_ptr<MatrixTileVlmLegalityValidationResult> ValidateMacc(std::shared_ptr<MatrixTileMaccSemanticContract> contract);

		static std::shared_ptr<MatrixTileVlmLegalityValidationResult> ValidateTranspose(std::shared_ptr<MatrixTileTransposeSemanticContract> contract);

	private:
		static bool IsRuntimeOwnedMatrixTileRow(std::shared_ptr<VectorLegalityMatrixRow> row);

		static bool AllRowsSatisfy(std::function<bool(std::shared_ptr<VectorLegalityMatrixRow>)> predicate);
	};

}
