#include "InstructionSupportStatus.h"
#include "IsaV4Surface.h"
#include "YAKSys_Hybrid_CPU.Arch.OpcodeRegistry.h"
#include <algorithm>

namespace YAKSys_Hybrid_CPU::Arch
{

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: internal InstructionSupportEvidenceReport(IReadOnlyList<InstructionSupportStatus> rows)
	InstructionSupportEvidenceReport::InstructionSupportEvidenceReport(const std::vector<std::shared_ptr<InstructionSupportStatus>> &rows)
		: Rows(rows)
	{
	}

	int InstructionSupportEvidenceReport::getVersion() const
	{
		return SchemaVersion;
	}

	std::wstring InstructionSupportEvidenceReport::getKind() const
	{
		return L"instruction-support-declaration-evidence";
	}

	bool InstructionSupportEvidenceReport::isRuntimeAuthority() const
	{
		return false;
	}

	const std::vector<std::shared_ptr<InstructionSupportStatus>> InstructionSupportEvidenceReport::getRows() const
	{
		return Rows;
	}

	const std::vector<std::wstring> InstructionSupportStatusCatalog::getMandatoryInteger64RepairMnemonics()
	{
		return s_mandatoryInteger64RepairMnemonics;
	}

	const std::vector<std::shared_ptr<InstructionSupportStatus>> InstructionSupportStatusCatalog::getExplicitStatuses()
	{
		return s_explicitStatuses;
	}

	std::shared_ptr<InstructionSupportEvidenceReport> InstructionSupportStatusCatalog::getGeneratedEvidenceReport()
	{
		return s_generatedEvidenceReport;
	}

	bool InstructionSupportStatusCatalog::TryGetExplicitStatus(const std::wstring &mnemonic, std::shared_ptr<InstructionSupportStatus> &status)
	{
		if (StringHelper::isEmptyOrWhiteSpace(mnemonic))
		{
			status = nullptr;
			return false;
		}

		const auto iterator = s_byMnemonic.find(StringHelper::toLower(NormalizeMnemonic(mnemonic)));
		if (iterator == s_byMnemonic.end())
		{
			status = nullptr;
			return false;
		}

		status = iterator->second;
		return true;
	}

	std::shared_ptr<InstructionSupportStatus> InstructionSupportStatusCatalog::GetStatus(const std::wstring &mnemonic)
	{
		std::shared_ptr<InstructionSupportStatus> status;
		if (TryGetExplicitStatus(mnemonic, status))
		{
			return status;
		}

		std::wstring normalizedMnemonic = NormalizeMnemonic(mnemonic);
		if (IsaV4Surface::ProhibitedOpcodes->contains(normalizedMnemonic))
		{
			return std::make_shared<InstructionSupportStatus>(normalizedMnemonic, IsaInstructionStatus::Prohibited, RuntimeInstructionEvidence::None);
		}

		if (IsaV4Surface::OptionalExtensions->contains(normalizedMnemonic))
		{
			return std::make_shared<InstructionSupportStatus>(normalizedMnemonic, IsaInstructionStatus::OptionalDisabled, RuntimeInstructionEvidence::DeclaredOnly, L"OptionalScalarExtension");
		}

		if (IsaV4Surface::MandatoryCoreOpcodes->contains(normalizedMnemonic))
		{
			const bool hasOpcodeMetadata = FindOpcodeInfoByMnemonic(normalizedMnemonic).has_value();
			return std::make_shared<InstructionSupportStatus>(normalizedMnemonic, IsaInstructionStatus::Mandatory, hasOpcodeMetadata ? RuntimeInstructionEvidence::DeclaredOnly : RuntimeInstructionEvidence::None, L"", hasOpcodeMetadata, hasOpcodeMetadata, hasOpcodeMetadata, false, false);
		}

		return std::make_shared<InstructionSupportStatus>(normalizedMnemonic, IsaInstructionStatus::Reserved, RuntimeInstructionEvidence::None);
	}

	bool InstructionSupportStatusCatalog::TryGetStatus(Processor::CPU_Core::InstructionsEnum opcode, std::shared_ptr<InstructionSupportStatus> &status)
	{
		std::wstring enumName;
		if (!OpcodeRegistry::TryGetMnemonic(static_cast<unsigned int>(opcode), enumName))
		{
			enumName = std::to_wstring(static_cast<unsigned int>(opcode));
		}
		if (TryGetExplicitStatus(enumName, status))
		{
			return true;
		}

		std::wstring mnemonic;
		if (OpcodeRegistry::TryGetMnemonic(static_cast<unsigned int>(opcode), mnemonic) && TryGetExplicitStatus(mnemonic, status))
		{
			return true;
		}

		status = GetStatus(OpcodeRegistry::TryGetMnemonic(static_cast<unsigned int>(opcode), mnemonic) ? mnemonic : enumName);
		return true;
	}

	std::vector<std::shared_ptr<InstructionSupportStatus>> InstructionSupportStatusCatalog::BuildExplicitStatuses()
	{
		std::vector<std::shared_ptr<InstructionSupportStatus>> rows;

		rows.push_back(std::make_shared<InstructionSupportStatus>(L"SRA", IsaInstructionStatus::Mandatory, RuntimeInstructionEvidence::ConformanceTested, L"Integer64Repair", true, true, true, true, true));

		rows.push_back(std::make_shared<InstructionSupportStatus>(L"ADDIW", IsaInstructionStatus::Mandatory, RuntimeInstructionEvidence::ConformanceTested, L"Integer64Repair", true, true, true, true, true));

		rows.push_back(std::make_shared<InstructionSupportStatus>(L"ADDW", IsaInstructionStatus::Mandatory, RuntimeInstructionEvidence::ConformanceTested, L"Integer64Repair", true, true, true, true, true));

		rows.push_back(std::make_shared<InstructionSupportStatus>(L"SUBW", IsaInstructionStatus::Mandatory, RuntimeInstructionEvidence::ConformanceTested, L"Integer64Repair", true, true, true, true, true));

		rows.push_back(std::make_shared<InstructionSupportStatus>(L"SLLW", IsaInstructionStatus::Mandatory, RuntimeInstructionEvidence::ConformanceTested, L"Integer64Repair", true, true, true, true, true));

		rows.push_back(std::make_shared<InstructionSupportStatus>(L"SRLW", IsaInstructionStatus::Mandatory, RuntimeInstructionEvidence::ConformanceTested, L"Integer64Repair", true, true, true, true, true));

		rows.push_back(std::make_shared<InstructionSupportStatus>(L"SRAW", IsaInstructionStatus::Mandatory, RuntimeInstructionEvidence::ConformanceTested, L"Integer64Repair", true, true, true, true, true));

		rows.push_back(std::make_shared<InstructionSupportStatus>(L"SLLIW", IsaInstructionStatus::Mandatory, RuntimeInstructionEvidence::ConformanceTested, L"Integer64Repair", true, true, true, true, true));

		rows.push_back(std::make_shared<InstructionSupportStatus>(L"SRLIW", IsaInstructionStatus::Mandatory, RuntimeInstructionEvidence::ConformanceTested, L"Integer64Repair", true, true, true, true, true));

		rows.push_back(std::make_shared<InstructionSupportStatus>(L"SRAIW", IsaInstructionStatus::Mandatory, RuntimeInstructionEvidence::ConformanceTested, L"Integer64Repair", true, true, true, true, true));

		rows.push_back(std::make_shared<InstructionSupportStatus>(L"MULW", IsaInstructionStatus::Mandatory, RuntimeInstructionEvidence::ConformanceTested, L"Integer64Repair", true, true, true, true, true));

		rows.push_back(std::make_shared<InstructionSupportStatus>(L"DIVW", IsaInstructionStatus::Mandatory, RuntimeInstructionEvidence::ConformanceTested, L"Integer64Repair", true, true, true, true, true));

		rows.push_back(std::make_shared<InstructionSupportStatus>(L"DIVUW", IsaInstructionStatus::Mandatory, RuntimeInstructionEvidence::ConformanceTested, L"Integer64Repair", true, true, true, true, true));

		rows.push_back(std::make_shared<InstructionSupportStatus>(L"REMW", IsaInstructionStatus::Mandatory, RuntimeInstructionEvidence::ConformanceTested, L"Integer64Repair", true, true, true, true, true));

		rows.push_back(std::make_shared<InstructionSupportStatus>(L"REMUW", IsaInstructionStatus::Mandatory, RuntimeInstructionEvidence::ConformanceTested, L"Integer64Repair", true, true, true, true, true));

		rows.push_back(std::make_shared<InstructionSupportStatus>(L"SEXT.W", IsaInstructionStatus::Mandatory, RuntimeInstructionEvidence::ConformanceTested, L"Integer64Repair", true, true, true, true, true));

		rows.push_back(std::make_shared<InstructionSupportStatus>(L"ZEXT.W", IsaInstructionStatus::Mandatory, RuntimeInstructionEvidence::ConformanceTested, L"Integer64Repair", true, true, true, true, true));

		for (auto mnemonic : s_mandatoryInteger64RepairMnemonics)
		{
			const std::wstring normalizedMnemonic = StringHelper::toLower(mnemonic);
			const bool isMandatoryInteger64Repair = std::any_of(s_mandatoryInteger64RepairMnemonics.begin(), s_mandatoryInteger64RepairMnemonics.end(),
				[&normalizedMnemonic](const std::wstring &candidate) { return StringHelper::toLower(candidate) == normalizedMnemonic; });
			if (isMandatoryInteger64Repair)
			{
				continue;
			}

			rows.push_back(std::make_shared<InstructionSupportStatus>(mnemonic, IsaInstructionStatus::Mandatory, RuntimeInstructionEvidence::None, L"Integer64Repair"));
		}

		rows.push_back(std::make_shared<InstructionSupportStatus>(L"VGATHER", IsaInstructionStatus::OptionalEnabled, RuntimeInstructionEvidence::ConformanceTested, L"VectorIndexedGatherMemory", true, true, true, true, true));
		rows.push_back(std::make_shared<InstructionSupportStatus>(L"VSCATTER", IsaInstructionStatus::OptionalEnabled, RuntimeInstructionEvidence::ConformanceTested, L"VectorIndexedScatterMemory", true, true, true, true, true));

		rows.push_back(std::make_shared<InstructionSupportStatus>(L"CZERO.EQZ", IsaInstructionStatus::OptionalEnabled, RuntimeInstructionEvidence::ConformanceTested, L"ScalarSelectCzero", true, true, true, true, true));
		AddReservedNoAllocation(rows, L"SEQZ", L"ScalarSelectCzero");
		AddReservedNoAllocation(rows, L"SNEZ", L"ScalarSelectCzero");
		AddReservedNoAllocation(rows, L"CSEL", L"ScalarSelectCzero");
		AddOptionalEnabledScalarSelectCzero(rows, L"CZERO.NEZ");

		rows.push_back(std::make_shared<InstructionSupportStatus>(L"CLZ", IsaInstructionStatus::OptionalEnabled, RuntimeInstructionEvidence::ConformanceTested, L"ScalarBitmanipCore", true, true, true, true, true));
		rows.push_back(std::make_shared<InstructionSupportStatus>(L"CTZ", IsaInstructionStatus::OptionalEnabled, RuntimeInstructionEvidence::ConformanceTested, L"ScalarBitmanipCore", true, true, true, true, true));
		AddOptionalEnabledScalarBitmanipCore(rows, L"CPOP");
		AddReservedNoAllocation(rows, L"POPCNT", L"ScalarBitmanipCore");
		AddOptionalEnabledScalarBitmanipCore(rows, L"ROL");
		AddOptionalEnabledScalarBitmanipCore(rows, L"ROR");
		AddOptionalEnabledScalarBitmanipCore(rows, L"ROLI");
		AddOptionalEnabledScalarBitmanipCore(rows, L"RORI");
		AddOptionalEnabledScalarBitmanipCore(rows, L"ANDN");
		AddOptionalEnabledScalarBitmanipCore(rows, L"ORN");
		AddOptionalEnabledScalarBitmanipCore(rows, L"XNOR");
		AddOptionalEnabledScalarBitmanipCore(rows, L"MIN");
		AddOptionalEnabledScalarBitmanipCore(rows, L"MAX");
		AddOptionalEnabledScalarBitmanipCore(rows, L"MINU");
		AddOptionalEnabledScalarBitmanipCore(rows, L"MAXU");
		AddOptionalEnabledScalarBitmanipCore(rows, L"REV8");
		AddOptionalEnabledScalarBitmanipCore(rows, L"BREV8");
		AddOptionalEnabledScalarBitmanipCore(rows, L"SEXT.B");
		AddOptionalEnabledScalarBitmanipCore(rows, L"SEXT.H");
		AddOptionalEnabledScalarBitmanipCore(rows, L"ZEXT.H");
		AddOptionalEnabledScalarBitfield(rows, L"BSET");
		AddOptionalEnabledScalarBitfield(rows, L"BCLR");
		AddOptionalEnabledScalarBitfield(rows, L"BINV");
		AddOptionalEnabledScalarBitfield(rows, L"BEXT");
		AddOptionalEnabledScalarBitfield(rows, L"BSETI");
		AddOptionalEnabledScalarBitfield(rows, L"BCLRI");
		AddOptionalEnabledScalarBitfield(rows, L"BINVI");
		AddOptionalEnabledScalarBitfield(rows, L"BEXTI");

		AddOptionalEnabledScalarAddressGeneration(rows, L"SH1ADD");
		AddOptionalEnabledScalarAddressGeneration(rows, L"SH2ADD");
		AddOptionalEnabledScalarAddressGeneration(rows, L"SH3ADD");
		AddOptionalEnabledScalarAddressGeneration(rows, L"ADD.UW");
		AddOptionalEnabledScalarAddressGeneration(rows, L"SH1ADD.UW");
		AddOptionalEnabledScalarAddressGeneration(rows, L"SH2ADD.UW");
		AddOptionalEnabledScalarAddressGeneration(rows, L"SH3ADD.UW");
		AddOptionalEnabledScalarAddressGeneration(rows, L"SLLI.UW");

		rows.push_back(std::make_shared<InstructionSupportStatus>(L"RDCYCLE", IsaInstructionStatus::OptionalEnabled, RuntimeInstructionEvidence::ConformanceTested, L"ScalarSystemCounter", true, true, true, true, true));
		AddReservedNoAllocation(rows, L"RDTIME", L"ScalarSystemCounter");
		AddReservedNoAllocation(rows, L"RDINSTRET", L"ScalarSystemCounter");
		AddReservedNoAllocation(rows, L"PAUSE", L"ScalarSystemCounter");

		AddOptionalEnabledScalarCarryLessChecksum(rows, L"CLMUL");
		AddOptionalEnabledScalarCarryLessChecksum(rows, L"CLMULH");
		AddOptionalEnabledScalarCarryLessChecksum(rows, L"CLMULR");
		AddReservedNoAllocation(rows, L"CRC32", L"ScalarCrcChecksum");
		AddReservedNoAllocation(rows, L"CRC64", L"ScalarCrcChecksum");
		AddReservedNoAllocation(rows, L"ADC", L"ScalarMultiPrecision");
		AddReservedNoAllocation(rows, L"SBC", L"ScalarMultiPrecision");
		AddReservedNoAllocation(rows, L"ADDC", L"ScalarMultiPrecision");
		AddReservedNoAllocation(rows, L"SUBC", L"ScalarMultiPrecision");

		rows.push_back(std::make_shared<InstructionSupportStatus>(L"DmaStreamCompute", IsaInstructionStatus::OptionalEnabled, RuntimeInstructionEvidence::ConformanceTested, L"Lane6DSC", true, true, true, true, true));
		AddDescriptorOnlyNoAllocation(rows, L"DmaStreamCompute.SUB", L"Lane6DescriptorOp");
		AddDescriptorOnlyNoAllocation(rows, L"DmaStreamCompute.MIN", L"Lane6DescriptorOp");
		AddDescriptorOnlyNoAllocation(rows, L"DmaStreamCompute.MAX", L"Lane6DescriptorOp");
		AddDescriptorOnlyNoAllocation(rows, L"DmaStreamCompute.ABSDIFF", L"Lane6DescriptorOp");
		AddDescriptorOnlyNoAllocation(rows, L"DmaStreamCompute.CLAMP", L"Lane6DescriptorOp");
		AddDescriptorOnlyNoAllocation(rows, L"DmaStreamCompute.CONVERT", L"Lane6DescriptorOp");
		AddDescriptorOnlyNoAllocation(rows, L"DmaStreamCompute.COMPARE", L"Lane6DescriptorOp");
		AddDescriptorOnlyNoAllocation(rows, L"DmaStreamCompute.SELECT", L"Lane6DescriptorOp");
		AddDescriptorOnlyNoAllocation(rows, L"DmaStreamCompute.REDUCE_SUM", L"Lane6DescriptorOp");
		AddDescriptorOnlyNoAllocation(rows, L"DmaStreamCompute.REDUCE_MIN", L"Lane6DescriptorOp");
		AddDescriptorOnlyNoAllocation(rows, L"DmaStreamCompute.REDUCE_MAX", L"Lane6DescriptorOp");
		AddDescriptorOnlyNoAllocation(rows, L"DmaStreamCompute.REDUCE_AND", L"Lane6DescriptorOp");
		AddDescriptorOnlyNoAllocation(rows, L"DmaStreamCompute.REDUCE_OR", L"Lane6DescriptorOp");
		AddDescriptorOnlyNoAllocation(rows, L"DmaStreamCompute.REDUCE_XOR", L"Lane6DescriptorOp");
		AddDescriptorOnlyNoAllocation(rows, L"DSC_SHAPE_STRIDED", L"Lane6DescriptorShape");
		AddDescriptorOnlyNoAllocation(rows, L"DSC_SHAPE_TILED", L"Lane6DescriptorShape");
		AddDescriptorOnlyNoAllocation(rows, L"DSC_SHAPE_SCATTER_GATHER", L"Lane6DescriptorShape");
		AddDescriptorOnlyNoAllocation(rows, L"DSC_SHAPE_2D", L"Lane6DescriptorShape");
		AddDescriptorOnlyNoAllocation(rows, L"DSC_SHAPE_MULTI_RANGE", L"Lane6DescriptorShape");
		rows.push_back(std::make_shared<InstructionSupportStatus>(L"DSC_STATUS", IsaInstructionStatus::OptionalEnabled, RuntimeInstructionEvidence::ConformanceTested, L"Lane6QueueControl", true, true, true, true, true));
		rows.push_back(std::make_shared<InstructionSupportStatus>(L"DSC_QUERY_CAPS", IsaInstructionStatus::OptionalEnabled, RuntimeInstructionEvidence::ConformanceTested, L"Lane6DscQuery", true, true, true, true, true));
		AddReservedNoAllocation(rows, L"DSC_POLL", L"Lane6QueueControl");
		AddReservedNoAllocation(rows, L"DSC_WAIT", L"Lane6QueueControl");
		AddReservedNoAllocation(rows, L"DSC_CANCEL", L"Lane6QueueControl");
		AddReservedNoAllocation(rows, L"DSC_FENCE", L"Lane6QueueControl");
		AddReservedNoAllocation(rows, L"DSC_COMMIT", L"Lane6QueueControl");
		AddReservedNoAllocation(rows, L"DSC_QUERY_BACKEND", L"Lane6DscQuery");
		AddReservedNoAllocation(rows, L"DSC_QUERY_SHAPE", L"Lane6DscQuery");
		rows.push_back(std::make_shared<InstructionSupportStatus>(L"DSC2", IsaInstructionStatus::ParserOnly, RuntimeInstructionEvidence::DeclaredOnly, L"Lane6DSC"));

		AddOptionalEnabledL7SdcProduction(rows, L"ACCEL_QUERY_CAPS");
		AddOptionalEnabledL7SdcProduction(rows, L"ACCEL_SUBMIT");
		AddOptionalEnabledL7SdcProduction(rows, L"ACCEL_POLL");
		AddOptionalEnabledL7SdcProduction(rows, L"ACCEL_WAIT");
		AddOptionalEnabledL7SdcProduction(rows, L"ACCEL_CANCEL");
		AddOptionalEnabledL7SdcProduction(rows, L"ACCEL_FENCE");
		AddOptionalEnabledL7SdcProduction(rows, L"ACCEL_STATUS");
		AddReservedNoAllocation(rows, L"ACCEL_QUERY_ABI", L"Lane7TopologyQueue");
		AddReservedNoAllocation(rows, L"ACCEL_QUERY_TOPOLOGY", L"Lane7TopologyQueue");
		AddReservedNoAllocation(rows, L"ACCEL_OPEN", L"Lane7TopologyQueue");
		AddReservedNoAllocation(rows, L"ACCEL_CLOSE", L"Lane7TopologyQueue");
		AddReservedNoAllocation(rows, L"ACCEL_BIND_QUEUE", L"Lane7TopologyQueue");
		AddReservedNoAllocation(rows, L"ACCEL_UNBIND_QUEUE", L"Lane7TopologyQueue");

		AddOptionalEnabledVectorConfigSystemSingleton(rows, L"VSETVL");
		AddOptionalEnabledVectorConfigSystemSingleton(rows, L"VSETVLI");
		AddOptionalEnabledVectorConfigSystemSingleton(rows, L"VSETIVLI");

		AddOptionalEnabledVectorBinaryCompute(rows, L"VADD");
		AddOptionalEnabledVectorBinaryCompute(rows, L"VSUB");
		AddOptionalEnabledVectorBinaryCompute(rows, L"VMUL");
		AddOptionalEnabledVectorBinaryCompute(rows, L"VDIV");
		AddOptionalEnabledVectorUnaryCompute(rows, L"VSQRT");
		AddOptionalEnabledVectorBinaryCompute(rows, L"VMOD");

		AddOptionalEnabledVectorTransfer(rows, L"VLOAD");
		AddOptionalEnabledVectorTransfer(rows, L"VSTORE");

		AddOptionalEnabledVectorBinaryCompute(rows, L"VXOR");
		AddOptionalEnabledVectorBinaryCompute(rows, L"VOR");
		AddOptionalEnabledVectorBinaryCompute(rows, L"VAND");
		AddOptionalEnabledVectorUnaryCompute(rows, L"VNOT");
		AddOptionalEnabledVectorBinaryCompute(rows, L"VSLL");
		AddOptionalEnabledVectorBinaryCompute(rows, L"VSRL");
		AddOptionalEnabledVectorBinaryCompute(rows, L"VSRA");
		AddOptionalEnabledVectorFma(rows, L"VFMADD");
		AddOptionalEnabledVectorFma(rows, L"VFMSUB");
		AddOptionalEnabledVectorFma(rows, L"VFNMADD");
		AddOptionalEnabledVectorFma(rows, L"VFNMSUB");
		AddOptionalEnabledVectorBinaryCompute(rows, L"VMIN");
		AddOptionalEnabledVectorBinaryCompute(rows, L"VMAX");
		AddOptionalEnabledVectorBinaryCompute(rows, L"VMINU");
		AddOptionalEnabledVectorBinaryCompute(rows, L"VMAXU");

		AddOptionalEnabledPredicateMaskPublication(rows, L"VMAND");
		AddOptionalEnabledPredicateMaskPublication(rows, L"VMOR");
		AddOptionalEnabledPredicateMaskPublication(rows, L"VMXOR");
		AddOptionalEnabledPredicateMaskPublication(rows, L"VMNOT");
		AddOptionalEnabledVectorMaskPrefixPublication(rows, L"VMSBF");
		AddOptionalEnabledVectorZeroExtendPublication(rows, L"VZEXT");
		AddOptionalEnabledVectorScanPrefixPublication(rows, L"VSCAN.SUM");
		AddOptionalEnabledVectorSaturatingAddPolicy(rows, L"VADD.SAT");
		AddReservedNoAllocation(rows, L"VSUB.SAT", L"VectorSaturatingFixedPoint");
		AddReservedNoAllocation(rows, L"VMUL.SAT", L"VectorSaturatingFixedPoint");
		AddReservedNoAllocation(rows, L"VSLL.SAT", L"VectorSaturatingFixedPoint");
		AddReservedNoAllocation(rows, L"VSRL.SAT", L"VectorSaturatingFixedPoint");
		AddReservedNoAllocation(rows, L"VSRA.SAT", L"VectorSaturatingFixedPoint");
		AddReservedNoAllocation(rows, L"VAVG", L"VectorSaturatingFixedPoint");
		AddReservedNoAllocation(rows, L"VAVG.R", L"VectorSaturatingFixedPoint");
		AddReservedNoAllocation(rows, L"VCLIP", L"VectorSaturatingFixedPoint");
		AddReservedNoAllocation(rows, L"VSCAN.MIN", L"VectorScanSegmentMovement");
		AddReservedNoAllocation(rows, L"VSCAN.MAX", L"VectorScanSegmentMovement");
		AddReservedNoAllocation(rows, L"VLDSEG2", L"VectorScanSegmentMovement");
		AddReservedNoAllocation(rows, L"VLDSEG4", L"VectorScanSegmentMovement");
		AddReservedNoAllocation(rows, L"VLDSEG8", L"VectorScanSegmentMovement");
		AddReservedNoAllocation(rows, L"VSTSEG2", L"VectorScanSegmentMovement");
		AddReservedNoAllocation(rows, L"VSTSEG4", L"VectorScanSegmentMovement");
		AddReservedNoAllocation(rows, L"VSTSEG8", L"VectorScanSegmentMovement");
		AddReservedNoAllocation(rows, L"VZIP", L"VectorScanSegmentMovement");
		AddReservedNoAllocation(rows, L"VUNZIP", L"VectorScanSegmentMovement");
		AddReservedNoAllocation(rows, L"VINTERLEAVE", L"VectorScanSegmentMovement");
		AddReservedNoAllocation(rows, L"VDEINTERLEAVE", L"VectorScanSegmentMovement");
		AddReservedNoAllocation(rows, L"VMERGE", L"VectorMaskSelect");
		AddReservedNoAllocation(rows, L"VSELECT", L"VectorMaskSelect");
		AddReservedNoAllocation(rows, L"VFIRST", L"VectorMaskSelect");
		AddReservedNoAllocation(rows, L"VANY", L"VectorMaskSelect");
		AddReservedNoAllocation(rows, L"VALL", L"VectorMaskSelect");
		AddReservedNoAllocation(rows, L"VMSIF", L"VectorMaskSelect");
		AddReservedNoAllocation(rows, L"VMSOF", L"VectorMaskSelect");
		AddReservedNoAllocation(rows, L"VWADD", L"VectorWidenNarrowConvert");
		AddReservedNoAllocation(rows, L"VWADDU", L"VectorWidenNarrowConvert");
		AddReservedNoAllocation(rows, L"VWSUB", L"VectorWidenNarrowConvert");
		AddReservedNoAllocation(rows, L"VWSUBU", L"VectorWidenNarrowConvert");
		AddReservedNoAllocation(rows, L"VWMUL", L"VectorWidenNarrowConvert");
		AddReservedNoAllocation(rows, L"VWMULU", L"VectorWidenNarrowConvert");
		AddReservedNoAllocation(rows, L"VWMACC", L"VectorWidenNarrowConvert");
		AddReservedNoAllocation(rows, L"VNSRL", L"VectorWidenNarrowConvert");
		AddReservedNoAllocation(rows, L"VNSRA", L"VectorWidenNarrowConvert");
		AddReservedNoAllocation(rows, L"VSEXT", L"VectorWidenNarrowConvert");
		AddReservedNoAllocation(rows, L"VCVT.I", L"VectorWidenNarrowConvert");
		AddReservedNoAllocation(rows, L"VCVT.U", L"VectorWidenNarrowConvert");
		AddReservedNoAllocation(rows, L"VCVT.F", L"VectorWidenNarrowConvert");
		AddReservedNoAllocation(rows, L"VDOT.BLOCKSCALE", L"VectorDotMatrixDeferred");
		AddReservedNoAllocation(rows, L"VDOT.ACCUM", L"VectorDotMatrixDeferred");
		AddReservedNoAllocation(rows, L"VDOT.WIDE.I16", L"VectorDotMatrixDeferred");
		AddReservedNoAllocation(rows, L"VDOT.WIDE.I32", L"VectorDotMatrixDeferred");

		AddOptionalEnabledVectorComparisonPredicatePublication(rows, L"VCMPEQ");
		AddOptionalEnabledVectorComparisonPredicatePublication(rows, L"VCMPNE");
		AddOptionalEnabledVectorComparisonPredicatePublication(rows, L"VCMPLT");
		AddOptionalEnabledVectorComparisonPredicatePublication(rows, L"VCMPLE");
		AddOptionalEnabledVectorComparisonPredicatePublication(rows, L"VCMPGT");
		AddOptionalEnabledVectorComparisonPredicatePublication(rows, L"VCMPGE");

		rows.push_back(std::make_shared<InstructionSupportStatus>(L"VPOPC", IsaInstructionStatus::OptionalEnabled, RuntimeInstructionEvidence::ConformanceTested, L"VectorPredicateMaskScalarResult", true, true, true, true, true));

		AddOptionalEnabledVectorPredicativeMovement(rows, L"VCOMPRESS");
		AddOptionalEnabledVectorPredicativeMovement(rows, L"VEXPAND");
		AddOptionalEnabledVectorPermutation(rows, L"VPERMUTE");
		AddOptionalEnabledVectorPermutation(rows, L"VRGATHER");
		AddOptionalEnabledVectorPerm2(rows, L"VPERM2");
		AddOptionalEnabledVectorTranspose(rows, L"VTRANSPOSE");
		AddOptionalEnabledVectorSlide(rows, L"VSLIDEUP");
		AddOptionalEnabledVectorSlide(rows, L"VSLIDEDOWN");
		AddOptionalEnabledVectorSlideOne(rows, L"VSLIDE1UP");
		AddOptionalEnabledVectorSlideOne(rows, L"VSLIDE1DOWN");
		AddOptionalEnabledVectorUnaryCompute(rows, L"VREVERSE");
		AddOptionalEnabledVectorUnaryCompute(rows, L"VPOPCNT");
		AddOptionalEnabledVectorUnaryCompute(rows, L"VCLZ");
		AddOptionalEnabledVectorUnaryCompute(rows, L"VCTZ");
		AddOptionalEnabledVectorUnaryCompute(rows, L"VBREV8");

		AddOptionalEnabledVectorReductionScalarFootprint(rows, L"VREDSUM");
		AddOptionalEnabledVectorReductionScalarFootprint(rows, L"VREDMAX");
		AddOptionalEnabledVectorReductionScalarFootprint(rows, L"VREDMIN");
		AddOptionalEnabledVectorReductionScalarFootprint(rows, L"VREDMAXU");
		AddOptionalEnabledVectorReductionScalarFootprint(rows, L"VREDMINU");
		AddOptionalEnabledVectorReductionScalarFootprint(rows, L"VREDAND");
		AddOptionalEnabledVectorReductionScalarFootprint(rows, L"VREDOR");
		AddOptionalEnabledVectorReductionScalarFootprint(rows, L"VREDXOR");
		AddOptionalEnabledVectorDotProductScalarFootprint(rows, L"VDOT");
		AddOptionalEnabledVectorDotProductScalarFootprint(rows, L"VDOTU");
		AddOptionalEnabledVectorDotProductScalarFootprint(rows, L"VDOTF");
		AddOptionalEnabledVectorDotProductScalarFootprint(rows, L"VDOT_FP8");
		AddOptionalEnabledVectorDotProductWideScalarFootprint(rows, L"VDOT.WIDE");

		AddOptionalEnabledMatrixProduction(rows, L"MTILE_LOAD");
		AddOptionalEnabledMatrixProduction(rows, L"MTILE_STORE");
		AddOptionalEnabledMatrixProduction(rows, L"MTILE_MACC");
		AddOptionalEnabledMatrixProduction(rows, L"MTRANSPOSE");

		AddReservedNoAllocation(rows, L"SFENCE.VMA", L"CacheTlbCoherency");
		AddReservedNoAllocation(rows, L"DCACHE_CLEAN", L"CacheTlbCoherency");
		AddReservedNoAllocation(rows, L"DCACHE_INVAL", L"CacheTlbCoherency");
		AddReservedNoAllocation(rows, L"DCACHE_FLUSH", L"CacheTlbCoherency");
		AddReservedNoAllocation(rows, L"ICACHE_INVAL", L"CacheTlbCoherency");
		AddReservedNoAllocation(rows, L"IOTLB_INV", L"CacheTlbCoherency");
		AddReservedNoAllocation(rows, L"IOMMU_FENCE", L"CacheTlbCoherency");

		return rows;
	}

	void InstructionSupportStatusCatalog::AddOptionalEnabledVectorConfigSystemSingleton(std::vector<std::shared_ptr<InstructionSupportStatus>> &rows, const std::wstring &mnemonic)
	{
		rows.push_back(std::make_shared<InstructionSupportStatus>(mnemonic, IsaInstructionStatus::OptionalEnabled, RuntimeInstructionEvidence::ConformanceTested, L"VectorConfigSystemSingleton", true, true, true, true, true));
	}

	void InstructionSupportStatusCatalog::AddOptionalEnabledVectorTransfer(std::vector<std::shared_ptr<InstructionSupportStatus>> &rows, const std::wstring &mnemonic)
	{
		rows.push_back(std::make_shared<InstructionSupportStatus>(mnemonic, IsaInstructionStatus::OptionalEnabled, RuntimeInstructionEvidence::ConformanceTested, L"VectorTransferCarrier", true, true, true, true, true));
	}

	void InstructionSupportStatusCatalog::AddOptionalEnabledVectorBinaryCompute(std::vector<std::shared_ptr<InstructionSupportStatus>> &rows, const std::wstring &mnemonic)
	{
		rows.push_back(std::make_shared<InstructionSupportStatus>(mnemonic, IsaInstructionStatus::OptionalEnabled, RuntimeInstructionEvidence::ConformanceTested, L"VectorBinaryComputeCarrier", true, true, true, true, true));
	}

	void InstructionSupportStatusCatalog::AddOptionalEnabledVectorSaturatingAddPolicy(std::vector<std::shared_ptr<InstructionSupportStatus>> &rows, const std::wstring &mnemonic)
	{
		rows.push_back(std::make_shared<InstructionSupportStatus>(mnemonic, IsaInstructionStatus::OptionalEnabled, RuntimeInstructionEvidence::ConformanceTested, L"VectorSaturatingAddPolicy", true, true, true, true, true));
	}

	void InstructionSupportStatusCatalog::AddOptionalEnabledVectorUnaryCompute(std::vector<std::shared_ptr<InstructionSupportStatus>> &rows, const std::wstring &mnemonic)
	{
		rows.push_back(std::make_shared<InstructionSupportStatus>(mnemonic, IsaInstructionStatus::OptionalEnabled, RuntimeInstructionEvidence::ConformanceTested, L"VectorUnaryComputeCarrier", true, true, true, true, true));
	}

	void InstructionSupportStatusCatalog::AddOptionalEnabledVectorFma(std::vector<std::shared_ptr<InstructionSupportStatus>> &rows, const std::wstring &mnemonic)
	{
		rows.push_back(std::make_shared<InstructionSupportStatus>(mnemonic, IsaInstructionStatus::OptionalEnabled, RuntimeInstructionEvidence::ConformanceTested, L"VectorFmaDescriptorBacked", true, true, true, true, true));
	}

	void InstructionSupportStatusCatalog::AddOptionalEnabledVectorReductionScalarFootprint(std::vector<std::shared_ptr<InstructionSupportStatus>> &rows, const std::wstring &mnemonic)
	{
		rows.push_back(std::make_shared<InstructionSupportStatus>(mnemonic, IsaInstructionStatus::OptionalEnabled, RuntimeInstructionEvidence::ConformanceTested, L"VectorReductionScalarFootprintCarrier", true, true, true, true, true));
	}

	void InstructionSupportStatusCatalog::AddOptionalEnabledVectorDotProductScalarFootprint(std::vector<std::shared_ptr<InstructionSupportStatus>> &rows, const std::wstring &mnemonic)
	{
		rows.push_back(std::make_shared<InstructionSupportStatus>(mnemonic, IsaInstructionStatus::OptionalEnabled, RuntimeInstructionEvidence::ConformanceTested, L"VectorDotProductScalarFootprint", true, true, true, true, true));
	}

	void InstructionSupportStatusCatalog::AddOptionalEnabledVectorDotProductWideScalarFootprint(std::vector<std::shared_ptr<InstructionSupportStatus>> &rows, const std::wstring &mnemonic)
	{
		rows.push_back(std::make_shared<InstructionSupportStatus>(mnemonic, IsaInstructionStatus::OptionalEnabled, RuntimeInstructionEvidence::ConformanceTested, L"VectorDotProductWideScalarFootprint", true, true, true, true, true));
	}

	void InstructionSupportStatusCatalog::AddOptionalEnabledPredicateMaskPublication(std::vector<std::shared_ptr<InstructionSupportStatus>> &rows, const std::wstring &mnemonic)
	{
		rows.push_back(std::make_shared<InstructionSupportStatus>(mnemonic, IsaInstructionStatus::OptionalEnabled, RuntimeInstructionEvidence::ConformanceTested, L"VectorPredicateMaskPublication", true, true, true, true, true));
	}

	void InstructionSupportStatusCatalog::AddOptionalEnabledVectorMaskPrefixPublication(std::vector<std::shared_ptr<InstructionSupportStatus>> &rows, const std::wstring &mnemonic)
	{
		rows.push_back(std::make_shared<InstructionSupportStatus>(mnemonic, IsaInstructionStatus::OptionalEnabled, RuntimeInstructionEvidence::ConformanceTested, L"VectorMaskPrefixPublication", true, true, true, true, true));
	}

	void InstructionSupportStatusCatalog::AddOptionalEnabledVectorZeroExtendPublication(std::vector<std::shared_ptr<InstructionSupportStatus>> &rows, const std::wstring &mnemonic)
	{
		rows.push_back(std::make_shared<InstructionSupportStatus>(mnemonic, IsaInstructionStatus::OptionalEnabled, RuntimeInstructionEvidence::ConformanceTested, L"VectorZeroExtendPublication", true, true, true, true, true));
	}

	void InstructionSupportStatusCatalog::AddOptionalEnabledVectorScanPrefixPublication(std::vector<std::shared_ptr<InstructionSupportStatus>> &rows, const std::wstring &mnemonic)
	{
		rows.push_back(std::make_shared<InstructionSupportStatus>(mnemonic, IsaInstructionStatus::OptionalEnabled, RuntimeInstructionEvidence::ConformanceTested, L"VectorScanPrefixPublication", true, true, true, true, true));
	}

	void InstructionSupportStatusCatalog::AddOptionalEnabledVectorComparisonPredicatePublication(std::vector<std::shared_ptr<InstructionSupportStatus>> &rows, const std::wstring &mnemonic)
	{
		rows.push_back(std::make_shared<InstructionSupportStatus>(mnemonic, IsaInstructionStatus::OptionalEnabled, RuntimeInstructionEvidence::ConformanceTested, L"VectorComparisonPredicatePublication", true, true, true, true, true));
	}

	void InstructionSupportStatusCatalog::AddOptionalEnabledVectorPredicativeMovement(std::vector<std::shared_ptr<InstructionSupportStatus>> &rows, const std::wstring &mnemonic)
	{
		rows.push_back(std::make_shared<InstructionSupportStatus>(mnemonic, IsaInstructionStatus::OptionalEnabled, RuntimeInstructionEvidence::ConformanceTested, L"VectorPredicativeMovement", true, true, true, true, true));
	}

	void InstructionSupportStatusCatalog::AddOptionalEnabledVectorPermutation(std::vector<std::shared_ptr<InstructionSupportStatus>> &rows, const std::wstring &mnemonic)
	{
		rows.push_back(std::make_shared<InstructionSupportStatus>(mnemonic, IsaInstructionStatus::OptionalEnabled, RuntimeInstructionEvidence::ConformanceTested, L"VectorPermutation", true, true, true, true, true));
	}

	void InstructionSupportStatusCatalog::AddOptionalEnabledVectorPerm2(std::vector<std::shared_ptr<InstructionSupportStatus>> &rows, const std::wstring &mnemonic)
	{
		rows.push_back(std::make_shared<InstructionSupportStatus>(mnemonic, IsaInstructionStatus::OptionalEnabled, RuntimeInstructionEvidence::ConformanceTested, L"VectorPermute2Publication", true, true, true, true, true));
	}

	void InstructionSupportStatusCatalog::AddOptionalEnabledVectorTranspose(std::vector<std::shared_ptr<InstructionSupportStatus>> &rows, const std::wstring &mnemonic)
	{
		rows.push_back(std::make_shared<InstructionSupportStatus>(mnemonic, IsaInstructionStatus::OptionalEnabled, RuntimeInstructionEvidence::ConformanceTested, L"VectorTransposePublication", true, true, true, true, true));
	}

	void InstructionSupportStatusCatalog::AddOptionalEnabledVectorSlide(std::vector<std::shared_ptr<InstructionSupportStatus>> &rows, const std::wstring &mnemonic)
	{
		rows.push_back(std::make_shared<InstructionSupportStatus>(mnemonic, IsaInstructionStatus::OptionalEnabled, RuntimeInstructionEvidence::ConformanceTested, L"VectorSlide", true, true, true, true, true));
	}

	void InstructionSupportStatusCatalog::AddOptionalEnabledVectorSlideOne(std::vector<std::shared_ptr<InstructionSupportStatus>> &rows, const std::wstring &mnemonic)
	{
		rows.push_back(std::make_shared<InstructionSupportStatus>(mnemonic, IsaInstructionStatus::OptionalEnabled, RuntimeInstructionEvidence::ConformanceTested, L"VectorSlideOnePublication", true, true, true, true, true));
	}

	void InstructionSupportStatusCatalog::AddOptionalDisabledMatrix(std::vector<std::shared_ptr<InstructionSupportStatus>> &rows, const std::wstring &mnemonic)
	{
		rows.push_back(std::make_shared<InstructionSupportStatus>(mnemonic, IsaInstructionStatus::OptionalDisabled, RuntimeInstructionEvidence::DeclaredOnly, L"XMatrix", true));
	}

	void InstructionSupportStatusCatalog::AddOptionalEnabledMatrixProduction(std::vector<std::shared_ptr<InstructionSupportStatus>> &rows, const std::wstring &mnemonic)
	{
		rows.push_back(std::make_shared<InstructionSupportStatus>(mnemonic, IsaInstructionStatus::OptionalEnabled, RuntimeInstructionEvidence::ConformanceTested, L"XMatrix", true, true, true, true, true));
	}

	void InstructionSupportStatusCatalog::AddCarrierOnlyL7Sdc(std::vector<std::shared_ptr<InstructionSupportStatus>> &rows, const std::wstring &mnemonic)
	{
		rows.push_back(std::make_shared<InstructionSupportStatus>(mnemonic, IsaInstructionStatus::CarrierOnly, RuntimeInstructionEvidence::Materialized, L"Lane7L7SDC", true, true, true, true, false));
	}

	void InstructionSupportStatusCatalog::AddOptionalEnabledL7SdcProduction(std::vector<std::shared_ptr<InstructionSupportStatus>> &rows, const std::wstring &mnemonic)
	{
		rows.push_back(std::make_shared<InstructionSupportStatus>(mnemonic, IsaInstructionStatus::OptionalEnabled, RuntimeInstructionEvidence::ConformanceTested, L"Lane7L7SDC", true, true, true, true, true));
	}

	void InstructionSupportStatusCatalog::AddReservedNoAllocation(std::vector<std::shared_ptr<InstructionSupportStatus>> &rows, const std::wstring &mnemonic, const std::wstring &extensionName)
	{
		rows.push_back(std::make_shared<InstructionSupportStatus>(mnemonic, IsaInstructionStatus::Reserved, RuntimeInstructionEvidence::None, extensionName));
	}

	void InstructionSupportStatusCatalog::AddDescriptorOnlyNoAllocation(std::vector<std::shared_ptr<InstructionSupportStatus>> &rows, const std::wstring &mnemonic, const std::wstring &extensionName)
	{
		rows.push_back(std::make_shared<InstructionSupportStatus>(mnemonic, IsaInstructionStatus::DescriptorOnly, RuntimeInstructionEvidence::DeclaredOnly, extensionName));
	}

	void InstructionSupportStatusCatalog::AddOptionalEnabledScalarBitmanipCore(std::vector<std::shared_ptr<InstructionSupportStatus>> &rows, const std::wstring &mnemonic)
	{
		rows.push_back(std::make_shared<InstructionSupportStatus>(mnemonic, IsaInstructionStatus::OptionalEnabled, RuntimeInstructionEvidence::ConformanceTested, L"ScalarBitmanipCore", true, true, true, true, true));
	}

	void InstructionSupportStatusCatalog::AddOptionalEnabledScalarBitfield(std::vector<std::shared_ptr<InstructionSupportStatus>> &rows, const std::wstring &mnemonic)
	{
		rows.push_back(std::make_shared<InstructionSupportStatus>(mnemonic, IsaInstructionStatus::OptionalEnabled, RuntimeInstructionEvidence::ConformanceTested, L"ScalarBitfield", true, true, true, true, true));
	}

	void InstructionSupportStatusCatalog::AddOptionalEnabledScalarAddressGeneration(std::vector<std::shared_ptr<InstructionSupportStatus>> &rows, const std::wstring &mnemonic)
	{
		rows.push_back(std::make_shared<InstructionSupportStatus>(mnemonic, IsaInstructionStatus::OptionalEnabled, RuntimeInstructionEvidence::ConformanceTested, L"ScalarAddressGeneration", true, true, true, true, true));
	}

	void InstructionSupportStatusCatalog::AddOptionalEnabledScalarCarryLessChecksum(std::vector<std::shared_ptr<InstructionSupportStatus>> &rows, const std::wstring &mnemonic)
	{
		rows.push_back(std::make_shared<InstructionSupportStatus>(mnemonic, IsaInstructionStatus::OptionalEnabled, RuntimeInstructionEvidence::ConformanceTested, L"ScalarCarryLessChecksum", true, true, true, true, true));
	}

	void InstructionSupportStatusCatalog::AddOptionalEnabledScalarSelectCzero(std::vector<std::shared_ptr<InstructionSupportStatus>> &rows, const std::wstring &mnemonic)
	{
		rows.push_back(std::make_shared<InstructionSupportStatus>(mnemonic, IsaInstructionStatus::OptionalEnabled, RuntimeInstructionEvidence::ConformanceTested, L"ScalarSelectCzero", true, true, true, true, true));
	}

	const std::unordered_map<std::wstring, std::shared_ptr<InstructionSupportStatus>> InstructionSupportStatusCatalog::BuildMnemonicIndex(const std::vector<std::shared_ptr<InstructionSupportStatus>> &statuses)
	{
		std::unordered_map<std::wstring, std::shared_ptr<InstructionSupportStatus>> index;
		for (auto status : statuses)
		{
			index[StringHelper::toLower(NormalizeMnemonic(status->Mnemonic))] = status;
		}

		return index;
	}

	std::optional<OpcodeInfo> InstructionSupportStatusCatalog::FindOpcodeInfoByMnemonic(const std::wstring &mnemonic)
	{
		for (const auto& info : OpcodeRegistry::Opcodes)
		{
			if (StringHelper::toLower(NormalizeMnemonic(info.Mnemonic)) == StringHelper::toLower(NormalizeMnemonic(mnemonic)))
			{
				return info;
			}
		}

		return std::nullopt;
	}

	std::wstring InstructionSupportStatusCatalog::NormalizeMnemonic(const std::wstring &mnemonic)
	{
		return StringHelper::trim(mnemonic);
	}
}
