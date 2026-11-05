#include "MatrixTileCompilerEmissionHandoffPackage.h"
#include "../../../../../../../NonRTL/Arch/InstructionSupportStatus.h"
#include "MatrixTileResourceContour.h"
#include "../../../../../Pipeline/Scheduling/SlotLegality/SlotClassDefinitions.h"

using namespace YAKSys_Hybrid_CPU::Arch;
using namespace YAKSys_Hybrid_CPU::Core;

namespace YAKSys_Hybrid_CPU::CloseToHSL::Core::ISA::Instructions::NonVmx::Lanes00_03Vector::MatrixTile
{

	std::vector<std::shared_ptr<MatrixTileCompilerEmissionHandoffRow>> MatrixTileCompilerEmissionHandoffPackage::getRows()
	{
		std::vector<std::shared_ptr<MatrixTileCompilerEmissionHandoffRow>> rows;
		rows.reserve(RowTable.size());
		for (const auto &row : RowTable)
			rows.push_back(std::make_shared<MatrixTileCompilerEmissionHandoffRow>(*row));
		return rows;
	}

	std::shared_ptr<MatrixTileCompilerEmissionHandoffRow> MatrixTileCompilerEmissionHandoffPackage::GetRow(const std::wstring &mnemonic)
	{
		for (const auto &row : RowTable)
		{
			if (row->Mnemonic == mnemonic)
			{
				return std::make_shared<MatrixTileCompilerEmissionHandoffRow>(*row);
			}
		}

		throw ArgumentOutOfRangeException(L"mnemonic", mnemonic, L"Unknown matrix/tile compiler handoff row.");
	}

	void MatrixTileCompilerEmissionHandoffPackage::RequireRuntimeExecutableAuthority(const std::wstring &mnemonic)
	{
		std::shared_ptr<MatrixTileCompilerEmissionHandoffRow> row = GetRow(mnemonic);
		if (!row->HasRuntimeExecutableAuthority || row->RuntimeStatus != IsaInstructionStatus::OptionalEnabled || row->RuntimeEvidence != RuntimeInstructionEvidence::ConformanceTested)
		{
			throw InvalidOperationException(std::format(L"{} does not have complete runtime executable authority.", row->Mnemonic));
		}
	}

	std::shared_ptr<MatrixTileCompilerEmissionHandoffRow> MatrixTileCompilerEmissionHandoffPackage::Create(const std::wstring &mnemonic, InstructionsEnum opcode, const std::wstring &runtimePublicationKind)
	{
		const MatrixTileRuntimeResourceClass resourceClass = MatrixTileResourceContour::Classify(static_cast<unsigned int>(opcode));
		const SlotClass slotClass = MatrixTileResourceContour::ResolveSlotClass(resourceClass);
		return std::make_shared<MatrixTileCompilerEmissionHandoffRow>(
			mnemonic, opcode, IsaInstructionStatus::OptionalEnabled,
			RuntimeInstructionEvidence::ConformanceTested, resourceClass, slotClass,
			SlotClassLaneMap::GetLaneMask(slotClass), runtimePublicationKind,
			true, true, true, true);
	}
}
