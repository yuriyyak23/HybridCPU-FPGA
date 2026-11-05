#include "VectorLegalityMatrix.h"

#include "YAKSys_Hybrid_CPU.Arch.OpcodeRegistry.h"

namespace YAKSys_Hybrid_CPU::Arch
{

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: public VectorLegalityMatrixRow(string familyName, IReadOnlyList<InstructionsEnum> opcodes, VectorContourLegalityStatus oneDimensional, VectorContourLegalityStatus indexedAddressing, VectorContourLegalityStatus twoDimensionalAddressing, VectorContourLegalityStatus masked, VectorContourLegalityStatus tailMaskPolicy, VectorContourLegalityStatus reduction, VectorContourLegalityStatus descriptorBacked, string runtimeEvidenceNote)
	VectorLegalityMatrixRow::VectorLegalityMatrixRow(const std::wstring &familyName, const std::vector<InstructionsEnum> &opcodes, VectorContourLegalityStatus oneDimensional, VectorContourLegalityStatus indexedAddressing, VectorContourLegalityStatus twoDimensionalAddressing, VectorContourLegalityStatus masked, VectorContourLegalityStatus tailMaskPolicy, VectorContourLegalityStatus reduction, VectorContourLegalityStatus descriptorBacked, const std::wstring &runtimeEvidenceNote)
		: FamilyName(StringHelper::isEmptyOrWhiteSpace(familyName) ? throw std::invalid_argument("Vector family name must be non-empty.") : familyName),
		  Opcodes(opcodes), OneDimensional(oneDimensional), IndexedAddressing(indexedAddressing), TwoDimensionalAddressing(twoDimensionalAddressing),
		  Masked(masked), TailMaskPolicy(tailMaskPolicy), Reduction(reduction), DescriptorBacked(descriptorBacked), RuntimeEvidenceNote(runtimeEvidenceNote)
	{
// C# TO C++ CONVERTER TASK: Throw expressions are not converted by C# to C++ Converter:
// C# TO C++ CONVERTER TASK: This exception's constructor requires only one argument:
// ORIGINAL LINE: FamilyName = string.IsNullOrWhiteSpace(familyName) ? throw new ArgumentException("Vector family name must be non-empty.", nameof(familyName)) : familyName;
	}

	std::wstring VectorLegalityMatrixRow::getFamilyName() const
	{
		return FamilyName;
	}

	const std::vector<InstructionsEnum> VectorLegalityMatrixRow::getOpcodes() const
	{
		return Opcodes;
	}

	VectorContourLegalityStatus VectorLegalityMatrixRow::getOneDimensional() const
	{
		return OneDimensional;
	}

	VectorContourLegalityStatus VectorLegalityMatrixRow::getIndexedAddressing() const
	{
		return IndexedAddressing;
	}

	VectorContourLegalityStatus VectorLegalityMatrixRow::getTwoDimensionalAddressing() const
	{
		return TwoDimensionalAddressing;
	}

	VectorContourLegalityStatus VectorLegalityMatrixRow::getMasked() const
	{
		return Masked;
	}

	VectorContourLegalityStatus VectorLegalityMatrixRow::getTailMaskPolicy() const
	{
		return TailMaskPolicy;
	}

	VectorContourLegalityStatus VectorLegalityMatrixRow::getReduction() const
	{
		return Reduction;
	}

	VectorContourLegalityStatus VectorLegalityMatrixRow::getDescriptorBacked() const
	{
		return DescriptorBacked;
	}

	std::wstring VectorLegalityMatrixRow::getRuntimeEvidenceNote() const
	{
		return RuntimeEvidenceNote;
	}

	VectorContourLegalityStatus VectorLegalityMatrixRow::GetContourStatus(VectorContourKind contour)
	{
		switch (contour)
		{
		case VectorContourKind::OneDimensional: return OneDimensional;
		case VectorContourKind::IndexedAddressing: return IndexedAddressing;
		case VectorContourKind::TwoDimensionalAddressing: return TwoDimensionalAddressing;
		case VectorContourKind::Masked: return Masked;
		case VectorContourKind::TailMaskPolicy: return TailMaskPolicy;
		case VectorContourKind::Reduction: return Reduction;
		case VectorContourKind::DescriptorBacked: return DescriptorBacked;
		default: throw ArgumentOutOfRangeException(L"contour", static_cast<unsigned char>(contour), L"Unknown vector contour kind.");
		}
	}

	const std::vector<std::shared_ptr<VectorLegalityMatrixRow>> VectorLegalityMatrix::getRows()
	{
		return s_rows;
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER NOTE: The following .NET attribute has no direct equivalent in C++:
// ORIGINAL LINE: public static bool TryGetRow(InstructionsEnum opcode, [NotNullWhen(true)] out VectorLegalityMatrixRow? row)
	bool VectorLegalityMatrix::TryGetRow(InstructionsEnum opcode, std::shared_ptr<VectorLegalityMatrixRow> &row)
	{
		auto entry = s_byOpcode.find(opcode);
		if (entry == s_byOpcode.end())
		{
			row.reset();
			return false;
		}

		row = entry->second;
		return true;
	}

	std::shared_ptr<VectorLegalityMatrixRow> VectorLegalityMatrix::GetRow(InstructionsEnum opcode)
	{
		std::shared_ptr<VectorLegalityMatrixRow> row;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: if (TryGetRow(opcode, out VectorLegalityMatrixRow? row))
		if (TryGetRow(opcode, row))
		{
			return row;
		}

		throw InvalidOperationException(std::format(L"Vector opcode {} has no runtime-owned vector legality matrix row.", OpcodeRegistry::GetMnemonicOrHex(static_cast<unsigned int>(opcode))));
	}

	bool VectorLegalityMatrix::TryGetAddressingStatus(InstructionsEnum opcode, bool indexed, bool is2D, VectorContourLegalityStatus &status)
	{
		std::shared_ptr<VectorLegalityMatrixRow> row;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: if (!TryGetRow(opcode, out VectorLegalityMatrixRow? row))
		if (!TryGetRow(opcode, row))
		{
			status = VectorContourLegalityStatus::FailClosed;
			return false;
		}

		status = ResolveAddressingStatus(row, indexed, is2D);
		return true;
	}

	VectorContourLegalityStatus VectorLegalityMatrix::GetAddressingStatus(InstructionsEnum opcode, bool indexed, bool is2D)
	{
		std::shared_ptr<VectorLegalityMatrixRow> row = GetRow(opcode);
		return ResolveAddressingStatus(row, indexed, is2D);
	}

	bool VectorLegalityMatrix::AllowsAddressingExecution(InstructionsEnum opcode, bool indexed, bool is2D)
	{
		return GetAddressingStatus(opcode, indexed, is2D) == VectorContourLegalityStatus::Executable;
	}

	VectorContourLegalityStatus VectorLegalityMatrix::ResolveAddressingStatus(std::shared_ptr<VectorLegalityMatrixRow> row, bool indexed, bool is2D)
	{
		if (indexed && is2D)
		{
			return CombineAddressingStatuses(row->getIndexedAddressing(), row->getTwoDimensionalAddressing());
		}

		if (indexed)
		{
			return row->getIndexedAddressing();
		}

		return is2D ? row->getTwoDimensionalAddressing() : row->getOneDimensional();
	}

	VectorContourLegalityStatus VectorLegalityMatrix::CombineAddressingStatuses(VectorContourLegalityStatus first, VectorContourLegalityStatus second)
	{
		if (first == VectorContourLegalityStatus::Executable && second == VectorContourLegalityStatus::Executable)
		{
			return VectorContourLegalityStatus::Executable;
		}

		if (first == VectorContourLegalityStatus::FailClosed || second == VectorContourLegalityStatus::FailClosed)
		{
			return VectorContourLegalityStatus::FailClosed;
		}

		if (first == VectorContourLegalityStatus::DescriptorOnly || second == VectorContourLegalityStatus::DescriptorOnly)
		{
			return VectorContourLegalityStatus::DescriptorOnly;
		}

		return VectorContourLegalityStatus::NotApplicable;
	}

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private static IReadOnlyDictionary<InstructionsEnum, VectorLegalityMatrixRow> BuildOpcodeIndex(IReadOnlyList<VectorLegalityMatrixRow> rows)
	const std::unordered_map<InstructionsEnum, std::shared_ptr<VectorLegalityMatrixRow>> VectorLegalityMatrix::BuildOpcodeIndex(const std::vector<std::shared_ptr<VectorLegalityMatrixRow>> &rows)
	{
		std::unordered_map<InstructionsEnum, std::shared_ptr<VectorLegalityMatrixRow>> byOpcode;
		for (auto row : rows)
		{
			for (auto opcode : row->getOpcodes())
			{
				byOpcode[opcode] = row;
			}
		}

		return byOpcode;
	}

	std::vector<std::shared_ptr<VectorLegalityMatrixRow>> VectorLegalityMatrix::BuildRows()
	{
		constexpr VectorContourLegalityStatus na = VectorContourLegalityStatus::NotApplicable;
		constexpr VectorContourLegalityStatus executable = VectorContourLegalityStatus::Executable;
		constexpr VectorContourLegalityStatus failClosed = VectorContourLegalityStatus::FailClosed;
		constexpr VectorContourLegalityStatus descriptorOnly = VectorContourLegalityStatus::DescriptorOnly;

		return std::vector<std::shared_ptr<VectorLegalityMatrixRow>> {Row(L"VectorConfigSystemSingleton", {InstructionsEnum::VSETVL, InstructionsEnum::VSETVLI, InstructionsEnum::VSETIVLI}, na, failClosed, failClosed, na, executable, na, na, L"Vector configuration is a lane7 system-singleton contour; indexed/2D vector payloads are not execution authority."), Row(L"VectorBinaryComputeCarrier", {InstructionsEnum::VADD, InstructionsEnum::VSUB, InstructionsEnum::VMUL, InstructionsEnum::VDIV, InstructionsEnum::VMOD, InstructionsEnum::VXOR, InstructionsEnum::VOR, InstructionsEnum::VAND, InstructionsEnum::VSLL, InstructionsEnum::VSRL, InstructionsEnum::VSRA, InstructionsEnum::VMIN, InstructionsEnum::VMAX, InstructionsEnum::VMINU, InstructionsEnum::VMAXU}, executable, failClosed, failClosed, executable, executable, na, na, L"Only the 1D in-place memory contour has MicroOp materialization and execution evidence."), Row(L"VectorUnaryComputeCarrier", {InstructionsEnum::VSQRT, InstructionsEnum::VNOT, InstructionsEnum::VREVERSE, InstructionsEnum::VPOPCNT, InstructionsEnum::VCLZ, InstructionsEnum::VCTZ, InstructionsEnum::VBREV8}, executable, failClosed, failClosed, executable, executable, na, na, L"Only the 1D unary vector contour has MicroOp materialization and execution evidence."), Row(L"VectorReductionScalarFootprintCarrier", {InstructionsEnum::VREDSUM, InstructionsEnum::VREDMAX, InstructionsEnum::VREDMIN, InstructionsEnum::VREDMAXU, InstructionsEnum::VREDMINU, InstructionsEnum::VREDAND, InstructionsEnum::VREDOR, InstructionsEnum::VREDXOR}, executable, failClosed, failClosed, executable, executable, executable, na, L"Reduction is executable only on the 1D scalar-footprint contour."), Row(L"VectorFmaDescriptorBacked", {InstructionsEnum::VFMADD, InstructionsEnum::VFMSUB, InstructionsEnum::VFNMADD, InstructionsEnum::VFNMSUB}, executable, failClosed, failClosed, executable, executable, na, executable, L"FMA execution requires the descriptor-backed 1D contour; descriptor-less, indexed, and 2D contours fail closed."), Row(L"VectorDotProductScalarFootprint", {InstructionsEnum::VDOT, InstructionsEnum::VDOTU, InstructionsEnum::VDOTF, InstructionsEnum::VDOT_FP8}, executable, failClosed, failClosed, executable, na, executable, failClosed, L"Dot-product execution is limited to the current 1D scalar-footprint ABI; indexed/2D and descriptor-backed ABI variants fail closed."), Row(L"VectorDotProductWideScalarFootprint", {InstructionsEnum::VDOT_WIDE}, executable, failClosed, failClosed, executable, na, executable, failClosed, L"VDOT.WIDE is executable only for the packed 1D FP16/BF16/FP8-to-FP32 and INT8/UINT8-to-32-bit scalar-footprint contours; indexed/2D, descriptor-backed, wider integer, block-scaled, and separate-destination variants fail closed."), Row(L"VectorTransferCarrier", {InstructionsEnum::VLOAD, InstructionsEnum::VSTORE}, executable, failClosed, failClosed, na, na, na, na, L"VLOAD/VSTORE publish only the 1D transfer-shape carrier; indexed/2D transfer contours fail closed."), Row(L"VectorPredicateMaskPublication", {InstructionsEnum::VMAND, InstructionsEnum::VMOR, InstructionsEnum::VMXOR, InstructionsEnum::VMNOT}, executable, failClosed, failClosed, na, na, na, na, L"Predicate-mask logical publication has no addressed vector-surface contour."), Row(L"VectorMaskPrefixPublication", {InstructionsEnum::VMSBF}, executable, failClosed, failClosed, na, executable, na, na, L"VMSBF is executable only as a 1D predicate-mask prefix publication contour; VMSIF/VMSOF and vector select/merge remain closed."), Row(L"VectorZeroExtendPublication", {InstructionsEnum::VZEXT}, executable, failClosed, failClosed, executable, executable, na, na, L"VZEXT is executable only as a packed 1D unsigned zero-extension memory-to-memory contour; other widen/narrow/convert forms remain closed."), Row(L"VectorScanPrefixPublication", {InstructionsEnum::VSCAN_SUM}, executable, failClosed, failClosed, executable, executable, na, na, L"VSCAN.SUM is executable only as a packed 1D integer inclusive-prefix sum contour; VSCAN.MIN/MAX, segment movement, indexed/2D, and matrix/tile remain closed."), Row(L"VectorPredicateMaskScalarResult", {InstructionsEnum::VPOPC}, executable, failClosed, failClosed, na, na, executable, na, L"VPOPC is executable only as a predicate-mask scalar-result contour."), Row(L"VectorComparisonPredicatePublication", {InstructionsEnum::VCMPEQ, InstructionsEnum::VCMPNE, InstructionsEnum::VCMPLT, InstructionsEnum::VCMPLE, InstructionsEnum::VCMPGT, InstructionsEnum::VCMPGE}, executable, failClosed, failClosed, executable, executable, na, na, L"Comparison publication is executable only for the 1D predicate-publication contour."), Row(L"VectorPredicativeMovement", {InstructionsEnum::VCOMPRESS, InstructionsEnum::VEXPAND}, executable, failClosed, failClosed, executable, executable, na, na, L"Predicative movement is executable only for the 1D single-surface contour."), Row(L"VectorPermutation", {InstructionsEnum::VPERMUTE, InstructionsEnum::VRGATHER}, executable, failClosed, failClosed, executable, executable, na, na, L"Permutation consumes an index vector on the 1D two-surface contour; it is not vector indexed-memory addressing."), Row(L"VectorPermute2Publication", {InstructionsEnum::VPERM2}, executable, failClosed, failClosed, executable, executable, na, na, L"VPERM2 is executable only for a packed 1D two-source two-lane immediate-controlled contour."), Row(L"VectorTransposePublication", {InstructionsEnum::VTRANSPOSE}, executable, failClosed, failClosed, executable, executable, na, failClosed, L"VTRANSPOSE is executable only for a packed 1D single-surface fixed 2x2 in-place transpose contour."), Row(L"VectorSlide", {InstructionsEnum::VSLIDEUP, InstructionsEnum::VSLIDEDOWN}, executable, failClosed, failClosed, executable, executable, na, na, L"Slide is executable only for the 1D single-surface contour."), Row(L"VectorSlideOnePublication", {InstructionsEnum::VSLIDE1UP, InstructionsEnum::VSLIDE1DOWN}, executable, failClosed, failClosed, executable, executable, na, na, L"VSLIDE1UP and VSLIDE1DOWN are executable only for packed 1D single-surface fixed-one-lane contours."), Row(L"VectorIndexedGatherMemory", {InstructionsEnum::VGATHER}, failClosed, executable, failClosed, executable, executable, na, executable, L"VGATHER publishes one executable 1D indexed-read carrier backed by Indexed2SrcDesc; VSCATTER, 2D, and indexed+2D remain closed."), Row(L"VectorIndexedScatterMemory", {InstructionsEnum::VSCATTER}, failClosed, executable, failClosed, executable, executable, na, executable, L"VSCATTER publishes one executable 1D indexed-write carrier backed by Indexed2SrcDesc; vector 2D and indexed+2D scatter remain closed."), Row(L"XMatrix", {InstructionsEnum::MTILE_LOAD, InstructionsEnum::MTILE_STORE, InstructionsEnum::MTILE_MACC, InstructionsEnum::MTRANSPOSE}, failClosed, failClosed, failClosed, failClosed, failClosed, failClosed, descriptorOnly, L"XMatrix Phase05 publishes runtime-owned descriptor-only legality rows for MTILE_LOAD, MTILE_STORE, MTILE_MACC, and MTRANSPOSE; descriptor-backed legality references the matrix/tile descriptor, memory/fault, accumulator, and transpose ABI, while decoder, IR, MicroOp, execution, compiler emission, and fallback paths remain closed.")};
	}

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private static VectorLegalityMatrixRow Row(string familyName, IReadOnlyList<InstructionsEnum> opcodes, VectorContourLegalityStatus oneDimensional, VectorContourLegalityStatus indexedAddressing, VectorContourLegalityStatus twoDimensionalAddressing, VectorContourLegalityStatus masked, VectorContourLegalityStatus tailMaskPolicy, VectorContourLegalityStatus reduction, VectorContourLegalityStatus descriptorBacked, string runtimeEvidenceNote)
	std::shared_ptr<VectorLegalityMatrixRow> VectorLegalityMatrix::Row(const std::wstring &familyName, const std::vector<InstructionsEnum> &opcodes, VectorContourLegalityStatus oneDimensional, VectorContourLegalityStatus indexedAddressing, VectorContourLegalityStatus twoDimensionalAddressing, VectorContourLegalityStatus masked, VectorContourLegalityStatus tailMaskPolicy, VectorContourLegalityStatus reduction, VectorContourLegalityStatus descriptorBacked, const std::wstring &runtimeEvidenceNote)
	{
		return std::make_shared<VectorLegalityMatrixRow>(familyName, opcodes, oneDimensional, indexedAddressing, twoDimensionalAddressing, masked, tailMaskPolicy, reduction, descriptorBacked, runtimeEvidenceNote);
	}
}
