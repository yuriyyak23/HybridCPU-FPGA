#pragma once

#include "../../CloseToHSL/Core/Architecture/Common/YAKSys_Hybrid_CPU.Processor.h"
#include <string>
#include <unordered_map>
#include <vector>
#include <stdexcept>
#include <format>
#include <memory>
#include "../../tangible_string_helper.h"

// C# TO C++ CONVERTER TASK: The C# 'using static' statement  cannot be converted to C++:
//using static YAKSys_Hybrid_CPU.Processor.CPU_Core;

namespace YAKSys_Hybrid_CPU::Arch
{
	enum class VectorContourKind : unsigned char
	{
		OneDimensional = 0,
		IndexedAddressing = 1,
		TwoDimensionalAddressing = 2,
		Masked = 3,
		TailMaskPolicy = 4,
		Reduction = 5,
		DescriptorBacked = 6
	};

	enum class VectorContourLegalityStatus : unsigned char
	{
		NotApplicable = 0,
		Executable = 1,
		FailClosed = 2,
		DescriptorOnly = 3
	};

	class VectorLegalityMatrixRow final : public std::enable_shared_from_this<VectorLegalityMatrixRow>
	{
	private:
		const std::wstring FamilyName;
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly IReadOnlyList<InstructionsEnum> Opcodes;
		const std::vector<InstructionsEnum> Opcodes;
		const VectorContourLegalityStatus OneDimensional;
		const VectorContourLegalityStatus IndexedAddressing;
		const VectorContourLegalityStatus TwoDimensionalAddressing;
		const VectorContourLegalityStatus Masked;
		const VectorContourLegalityStatus TailMaskPolicy;
		const VectorContourLegalityStatus Reduction;
		const VectorContourLegalityStatus DescriptorBacked;
		const std::wstring RuntimeEvidenceNote;

	public:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: public VectorLegalityMatrixRow(string familyName, IReadOnlyList<InstructionsEnum> opcodes, VectorContourLegalityStatus oneDimensional, VectorContourLegalityStatus indexedAddressing, VectorContourLegalityStatus twoDimensionalAddressing, VectorContourLegalityStatus masked, VectorContourLegalityStatus tailMaskPolicy, VectorContourLegalityStatus reduction, VectorContourLegalityStatus descriptorBacked, string runtimeEvidenceNote)
		VectorLegalityMatrixRow(const std::wstring &familyName, const std::vector<InstructionsEnum> &opcodes, VectorContourLegalityStatus oneDimensional, VectorContourLegalityStatus indexedAddressing, VectorContourLegalityStatus twoDimensionalAddressing, VectorContourLegalityStatus masked, VectorContourLegalityStatus tailMaskPolicy, VectorContourLegalityStatus reduction, VectorContourLegalityStatus descriptorBacked, const std::wstring &runtimeEvidenceNote);

		std::wstring getFamilyName() const;

		const std::vector<InstructionsEnum> getOpcodes() const;

		VectorContourLegalityStatus getOneDimensional() const;

		VectorContourLegalityStatus getIndexedAddressing() const;

		VectorContourLegalityStatus getTwoDimensionalAddressing() const;

		VectorContourLegalityStatus getMasked() const;

		VectorContourLegalityStatus getTailMaskPolicy() const;

		VectorContourLegalityStatus getReduction() const;

		VectorContourLegalityStatus getDescriptorBacked() const;

		std::wstring getRuntimeEvidenceNote() const;

		VectorContourLegalityStatus GetContourStatus(VectorContourKind contour);
	};

	/// <summary>
	/// Runtime-owned vector contour evidence matrix. This table is declaration/evidence
	/// for current runtime legality; opcode metadata, parser acceptance, and sideband
	/// descriptors do not override it.
	/// </summary>
	class VectorLegalityMatrix final
	{
	private:
		// Declare table builders before inline static members initialize from them.
		static std::vector<std::shared_ptr<VectorLegalityMatrixRow>> BuildRows();
		static const std::unordered_map<InstructionsEnum, std::shared_ptr<VectorLegalityMatrixRow>> BuildOpcodeIndex(const std::vector<std::shared_ptr<VectorLegalityMatrixRow>> &rows);

		static inline const std::vector<std::shared_ptr<VectorLegalityMatrixRow>> s_rows = BuildRows();
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private static readonly IReadOnlyDictionary<InstructionsEnum, VectorLegalityMatrixRow> s_byOpcode = BuildOpcodeIndex(s_rows);
		static inline const std::unordered_map<InstructionsEnum, std::shared_ptr<VectorLegalityMatrixRow>> s_byOpcode = BuildOpcodeIndex(s_rows);

	public:
		static const std::vector<std::shared_ptr<VectorLegalityMatrixRow>> getRows();

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER NOTE: The following .NET attribute has no direct equivalent in C++:
// ORIGINAL LINE: public static bool TryGetRow(InstructionsEnum opcode, [NotNullWhen(true)] out VectorLegalityMatrixRow? row)
		static bool TryGetRow(InstructionsEnum opcode, std::shared_ptr<VectorLegalityMatrixRow> &row);

		static std::shared_ptr<VectorLegalityMatrixRow> GetRow(InstructionsEnum opcode);

		static bool TryGetAddressingStatus(InstructionsEnum opcode, bool indexed, bool is2D, VectorContourLegalityStatus &status);

		static VectorContourLegalityStatus GetAddressingStatus(InstructionsEnum opcode, bool indexed, bool is2D);

		static bool AllowsAddressingExecution(InstructionsEnum opcode, bool indexed, bool is2D);

	private:
		static VectorContourLegalityStatus ResolveAddressingStatus(std::shared_ptr<VectorLegalityMatrixRow> row, bool indexed, bool is2D);

		static VectorContourLegalityStatus CombineAddressingStatuses(VectorContourLegalityStatus first, VectorContourLegalityStatus second);

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private static IReadOnlyDictionary<InstructionsEnum, VectorLegalityMatrixRow> BuildOpcodeIndex(IReadOnlyList<VectorLegalityMatrixRow> rows)
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private static VectorLegalityMatrixRow Row(string familyName, IReadOnlyList<InstructionsEnum> opcodes, VectorContourLegalityStatus oneDimensional, VectorContourLegalityStatus indexedAddressing, VectorContourLegalityStatus twoDimensionalAddressing, VectorContourLegalityStatus masked, VectorContourLegalityStatus tailMaskPolicy, VectorContourLegalityStatus reduction, VectorContourLegalityStatus descriptorBacked, string runtimeEvidenceNote)
		static std::shared_ptr<VectorLegalityMatrixRow> Row(const std::wstring &familyName, const std::vector<InstructionsEnum> &opcodes, VectorContourLegalityStatus oneDimensional, VectorContourLegalityStatus indexedAddressing, VectorContourLegalityStatus twoDimensionalAddressing, VectorContourLegalityStatus masked, VectorContourLegalityStatus tailMaskPolicy, VectorContourLegalityStatus reduction, VectorContourLegalityStatus descriptorBacked, const std::wstring &runtimeEvidenceNote);
	};
}
