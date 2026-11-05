#include "GeneratedIsaShadowCatalog.g.h"
#include <stdexcept>
#include <charconv>

using namespace YAKSys_Hybrid_CPU::Arch;

namespace YAKSys_Hybrid_CPU::Arch::Generated
{

	std::shared_ptr<const std::unordered_set<std::wstring>> GeneratedIsaCatalog::GetStaticPolicy(const std::wstring& policyId)
{
    const auto it = StaticPolicies.find(policyId);
    if (it == StaticPolicies.end()) throw std::out_of_range("Unknown generated ISA static policy.");
    return std::make_shared<const std::unordered_set<std::wstring>>(it->second);
}
const std::unordered_map<std::wstring, std::unordered_set<std::wstring>> GeneratedIsaCatalog::CreateStaticPolicies()
	{
		return std::unordered_map<std::wstring, std::unordered_set<std::wstring>> {{L"CarrierOnlyOpcodes", {}}, {L"DescriptorOnlyOpcodes", { L"DSC_SHAPE_2D", L"DSC_SHAPE_MULTI_RANGE", L"DSC_SHAPE_SCATTER_GATHER", L"DSC_SHAPE_STRIDED", L"DSC_SHAPE_TILED", L"DmaStreamCompute.ABSDIFF", L"DmaStreamCompute.CLAMP", L"DmaStreamCompute.COMPARE", L"DmaStreamCompute.CONVERT", L"DmaStreamCompute.MAX", L"DmaStreamCompute.MIN", L"DmaStreamCompute.REDUCE_AND", L"DmaStreamCompute.REDUCE_MAX", L"DmaStreamCompute.REDUCE_MIN", L"DmaStreamCompute.REDUCE_OR", L"DmaStreamCompute.REDUCE_SUM", L"DmaStreamCompute.REDUCE_XOR", L"DmaStreamCompute.SELECT", L"DmaStreamCompute.SUB" }}, {L"MandatoryCoreClasses", { L"Atomic", L"ControlFlow", L"Csr", L"Memory", L"ScalarAlu", L"SmtVt", L"System", L"Vmx" }}, {L"MandatoryCoreOpcodes", { L"ADD", L"ADDI", L"ADDIW", L"ADDW", L"AMOADD_D", L"AMOADD_W", L"AMOAND_D", L"AMOAND_W", L"AMOMAXU_D", L"AMOMAXU_W", L"AMOMAX_D", L"AMOMAX_W", L"AMOMINU_D", L"AMOMINU_W", L"AMOMIN_D", L"AMOMIN_W", L"AMOOR_D", L"AMOOR_W", L"AMOSWAP_D", L"AMOSWAP_W", L"AMOXOR_D", L"AMOXOR_W", L"AND", L"ANDI", L"AUIPC", L"BEQ", L"BGE", L"BGEU", L"BLT", L"BLTU", L"BNE", L"CSRRC", L"CSRRCI", L"CSRRS", L"CSRRSI", L"CSRRW", L"CSRRWI", L"DIV", L"DIVU", L"DIVUW", L"DIVW", L"DmaStreamCompute", L"EBREAK", L"ECALL", L"FENCE", L"FENCE_I", L"JAL", L"JALR", L"LB", L"LBU", L"LD", L"LH", L"LHU", L"LR_D", L"LR_W", L"LUI", L"LW", L"LWU", L"MRET", L"MUL", L"MULH", L"MULHSU", L"MULHU", L"MULW", L"OR", L"ORI", L"POD_BARRIER", L"REM", L"REMU", L"REMUW", L"REMW", L"SB", L"SC_D", L"SC_W", L"SD", L"SEV", L"SH", L"SLL", L"SLLI", L"SLLIW", L"SLLW", L"SLT", L"SLTI", L"SLTIU", L"SLTU", L"SRA", L"SRAI", L"SRAIW", L"SRAW", L"SRET", L"SRL", L"SRLI", L"SRLIW", L"SRLW", L"SUB", L"SUBW", L"SW", L"VMCLEAR", L"VMLAUNCH", L"VMPTRLD", L"VMREAD", L"VMRESUME", L"VMWRITE", L"VMXOFF", L"VMXON", L"VT_BARRIER", L"WFE", L"WFI", L"XOR", L"XORI", L"YIELD" }}, {L"MandatoryInteger64RepairOpcodes", { L"ADDIW", L"ADDW", L"DIVUW", L"DIVW", L"MULW", L"REMUW", L"REMW", L"SEXT.W", L"SLLIW", L"SLLW", L"SRA", L"SRAIW", L"SRAW", L"SRLIW", L"SRLW", L"SUBW", L"ZEXT.W" }}, {L"OptionalDisabledOpcodes", {}}, {L"OptionalEnabledOpcodes", { L"ACCEL_CANCEL", L"ACCEL_FENCE", L"ACCEL_POLL", L"ACCEL_QUERY_CAPS", L"ACCEL_STATUS", L"ACCEL_SUBMIT", L"ACCEL_WAIT", L"ADD.UW", L"ANDN", L"BCLR", L"BCLRI", L"BEXT", L"BEXTI", L"BINV", L"BINVI", L"BREV8", L"BSET", L"BSETI", L"CLMUL", L"CLMULH", L"CLMULR", L"CLZ", L"CPOP", L"CTZ", L"CZERO.EQZ", L"CZERO.NEZ", L"DSC_QUERY_CAPS", L"DSC_STATUS", L"DmaStreamCompute", L"MAX", L"MAXU", L"MIN", L"MINU", L"MTILE_LOAD", L"MTILE_MACC", L"MTILE_STORE", L"MTRANSPOSE", L"ORN", L"RDCYCLE", L"REV8", L"ROL", L"ROLI", L"ROR", L"RORI", L"SEXT.B", L"SEXT.H", L"SH1ADD", L"SH1ADD.UW", L"SH2ADD", L"SH2ADD.UW", L"SH3ADD", L"SH3ADD.UW", L"SLLI.UW", L"VADD", L"VAND", L"VBREV8", L"VCLZ", L"VCMPEQ", L"VCMPGE", L"VCMPGT", L"VCMPLE", L"VCMPLT", L"VCMPNE", L"VCOMPRESS", L"VCTZ", L"VDIV", L"VDOT", L"VDOT.WIDE", L"VDOTF", L"VDOTU", L"VDOT_FP8", L"VEXPAND", L"VFMADD", L"VFMSUB", L"VFNMADD", L"VFNMSUB", L"VGATHER", L"VLOAD", L"VMAND", L"VMAX", L"VMAXU", L"VMIN", L"VMINU", L"VMNOT", L"VMOD", L"VMOR", L"VMSBF", L"VMUL", L"VMXOR", L"VNOT", L"VOR", L"VPERM2", L"VPERMUTE", L"VPOPC", L"VPOPCNT", L"VREDAND", L"VREDMAX", L"VREDMAXU", L"VREDMIN", L"VREDMINU", L"VREDOR", L"VREDSUM", L"VREDXOR", L"VREVERSE", L"VRGATHER", L"VSCAN.SUM", L"VSCATTER", L"VSETIVLI", L"VSETVL", L"VSETVLI", L"VSLIDE1DOWN", L"VSLIDE1UP", L"VSLIDEDOWN", L"VSLIDEUP", L"VSLL", L"VSQRT", L"VSRA", L"VSRL", L"VSTORE", L"VSUB", L"VTRANSPOSE", L"VXOR", L"VZEXT", L"XNOR", L"ZEXT.H" }}, {L"OptionalExtensions", { L"NOT", L"POPCNT", L"XFMAC", L"XSQRT" }}, {L"ParserOnlyOpcodes", { L"DSC2" }}, {L"ProhibitedOpcodes", { L"14", L"147", L"148", L"15", L"18", L"CALL", L"CSR_READ", L"CSR_WRITE", L"FSP_FENCE", L"HINT_COLD", L"HINT_HOT", L"HINT_LIKELY", L"HINT_NOSTEAL", L"HINT_REUSE", L"HINT_STEALABLE", L"HINT_STREAM", L"HINT_UNLIKELY", L"JMP", L"LI", L"MV", L"NOP", L"RDVTID", L"RDVTMASK", L"RET" }}, {L"ReservedOpcodes", { L"DCACHE_CLEAN", L"DCACHE_FLUSH", L"DCACHE_INVAL", L"ICACHE_INVAL", L"IOMMU_FENCE", L"IOTLB_INV", L"SFENCE.VMA" }}, {L"SystemDeviceCommandOpcodes", { L"ACCEL_CANCEL", L"ACCEL_FENCE", L"ACCEL_POLL", L"ACCEL_QUERY_CAPS", L"ACCEL_STATUS", L"ACCEL_SUBMIT", L"ACCEL_WAIT" }}};
	}

	const std::unordered_map<std::wstring, std::wstring> GeneratedIsaCatalog::CreatePipelineClassMap()
	{
		return std::unordered_map<std::wstring, std::wstring>
		{
			{L"ACCEL_CANCEL", L"SYS_SERIAL"},
			{L"ACCEL_FENCE", L"SYS_SERIAL"},
			{L"ACCEL_POLL", L"SYS_SERIAL"},
			{L"ACCEL_QUERY_CAPS", L"SYS_SERIAL"},
			{L"ACCEL_STATUS", L"SYS_SERIAL"},
			{L"ACCEL_SUBMIT", L"SYS_SERIAL"},
			{L"ACCEL_WAIT", L"SYS_SERIAL"},
			{L"ADD", L"ALU"},
			{L"ADD.UW", L"ALU"},
			{L"ADDI", L"ALU"},
			{L"ADDIW", L"ALU"},
			{L"ADDW", L"ALU"},
			{L"AMOADD_D", L"ATOM"},
			{L"AMOADD_W", L"ATOM"},
			{L"AMOAND_D", L"ATOM"},
			{L"AMOAND_W", L"ATOM"},
			{L"AMOMAXU_D", L"ATOM"},
			{L"AMOMAXU_W", L"ATOM"},
			{L"AMOMAX_D", L"ATOM"},
			{L"AMOMAX_W", L"ATOM"},
			{L"AMOMINU_D", L"ATOM"},
			{L"AMOMINU_W", L"ATOM"},
			{L"AMOMIN_D", L"ATOM"},
			{L"AMOMIN_W", L"ATOM"},
			{L"AMOOR_D", L"ATOM"},
			{L"AMOOR_W", L"ATOM"},
			{L"AMOSWAP_D", L"ATOM"},
			{L"AMOSWAP_W", L"ATOM"},
			{L"AMOXOR_D", L"ATOM"},
			{L"AMOXOR_W", L"ATOM"},
			{L"AND", L"ALU"},
			{L"ANDI", L"ALU"},
			{L"ANDN", L"ALU"},
			{L"AUIPC", L"ALU"},
			{L"BCLR", L"ALU"},
			{L"BCLRI", L"ALU"},
			{L"BEQ", L"BR"},
			{L"BEXT", L"ALU"},
			{L"BEXTI", L"ALU"},
			{L"BGE", L"BR"},
			{L"BGEU", L"BR"},
			{L"BINV", L"ALU"},
			{L"BINVI", L"ALU"},
			{L"BLT", L"BR"},
			{L"BLTU", L"BR"},
			{L"BNE", L"BR"},
			{L"BREV8", L"ALU"},
			{L"BSET", L"ALU"},
			{L"BSETI", L"ALU"},
			{L"CLMUL", L"ALU"},
			{L"CLMULH", L"ALU"},
			{L"CLMULR", L"ALU"},
			{L"CLZ", L"ALU"},
			{L"CPOP", L"ALU"},
			{L"CSRRC", L"CSR_SERIAL"},
			{L"CSRRCI", L"CSR_SERIAL"},
			{L"CSRRS", L"CSR_SERIAL"},
			{L"CSRRSI", L"CSR_SERIAL"},
			{L"CSRRW", L"CSR_SERIAL"},
			{L"CSRRWI", L"CSR_SERIAL"},
			{L"CTZ", L"ALU"},
			{L"CZERO.EQZ", L"ALU"},
			{L"CZERO.NEZ", L"ALU"},
			{L"DIV", L"ALU"},
			{L"DIVU", L"ALU"},
			{L"DIVUW", L"ALU"},
			{L"DIVW", L"ALU"},
			{L"DSC_QUERY_CAPS", L"DMA_STREAM"},
			{L"DSC_STATUS", L"DMA_STREAM"},
			{L"DmaStreamCompute", L"DMA_STREAM"},
			{L"EBREAK", L"SYS_SERIAL"},
			{L"ECALL", L"SYS_SERIAL"},
			{L"FENCE", L"SYS_SERIAL"},
			{L"FENCE_I", L"SYS_SERIAL"},
			{L"JAL", L"BR"},
			{L"JALR", L"BR"},
			{L"LB", L"LSU"},
			{L"LBU", L"LSU"},
			{L"LD", L"LSU"},
			{L"LH", L"LSU"},
			{L"LHU", L"LSU"},
			{L"LR_D", L"ATOM"},
			{L"LR_W", L"ATOM"},
			{L"LUI", L"ALU"},
			{L"LW", L"LSU"},
			{L"LWU", L"LSU"},
			{L"MAX", L"ALU"},
			{L"MAXU", L"ALU"},
			{L"MIN", L"ALU"},
			{L"MINU", L"ALU"},
			{L"MRET", L"SYS_SERIAL"},
			{L"MUL", L"ALU"},
			{L"MULH", L"ALU"},
			{L"MULHSU", L"ALU"},
			{L"MULHU", L"ALU"},
			{L"MULW", L"ALU"},
			{L"OR", L"ALU"},
			{L"ORI", L"ALU"},
			{L"ORN", L"ALU"},
			{L"POD_BARRIER", L"SYS_SERIAL"},
			{L"RDCYCLE", L"CSR_SERIAL"},
			{L"REM", L"ALU"},
			{L"REMU", L"ALU"},
			{L"REMUW", L"ALU"},
			{L"REMW", L"ALU"},
			{L"REV8", L"ALU"},
			{L"ROL", L"ALU"},
			{L"ROLI", L"ALU"},
			{L"ROR", L"ALU"},
			{L"RORI", L"ALU"},
			{L"SB", L"LSU"},
			{L"SC_D", L"ATOM"},
			{L"SC_W", L"ATOM"},
			{L"SD", L"LSU"},
			{L"SEV", L"SYS_SERIAL"},
			{L"SEXT.B", L"ALU"},
			{L"SEXT.H", L"ALU"},
			{L"SEXT.W", L"ALU"},
			{L"SH", L"LSU"},
			{L"SH1ADD", L"ALU"},
			{L"SH1ADD.UW", L"ALU"},
			{L"SH2ADD", L"ALU"},
			{L"SH2ADD.UW", L"ALU"},
			{L"SH3ADD", L"ALU"},
			{L"SH3ADD.UW", L"ALU"},
			{L"SLL", L"ALU"},
			{L"SLLI", L"ALU"},
			{L"SLLI.UW", L"ALU"},
			{L"SLLIW", L"ALU"},
			{L"SLLW", L"ALU"},
			{L"SLT", L"ALU"},
			{L"SLTI", L"ALU"},
			{L"SLTIU", L"ALU"},
			{L"SLTU", L"ALU"},
			{L"SRA", L"ALU"},
			{L"SRAI", L"ALU"},
			{L"SRAIW", L"ALU"},
			{L"SRAW", L"ALU"},
			{L"SRET", L"SYS_SERIAL"},
			{L"SRL", L"ALU"},
			{L"SRLI", L"ALU"},
			{L"SRLIW", L"ALU"},
			{L"SRLW", L"ALU"},
			{L"SUB", L"ALU"},
			{L"SUBW", L"ALU"},
			{L"SW", L"LSU"},
			{L"VDOT.WIDE", L"ALU"},
			{L"VMCLEAR", L"VMX_SERIAL"},
			{L"VMLAUNCH", L"VMX_SERIAL"},
			{L"VMPTRLD", L"VMX_SERIAL"},
			{L"VMREAD", L"VMX_SERIAL"},
			{L"VMRESUME", L"VMX_SERIAL"},
			{L"VMSBF", L"ALU"},
			{L"VMWRITE", L"VMX_SERIAL"},
			{L"VMXOFF", L"VMX_SERIAL"},
			{L"VMXON", L"VMX_SERIAL"},
			{L"VPERM2", L"ALU"},
			{L"VSCAN.SUM", L"ALU"},
			{L"VSLIDE1DOWN", L"ALU"},
			{L"VSLIDE1UP", L"ALU"},
			{L"VTRANSPOSE", L"ALU"},
			{L"VT_BARRIER", L"SYS_SERIAL"},
			{L"VZEXT", L"ALU"},
			{L"WFE", L"SYS_SERIAL"},
			{L"WFI", L"SYS_SERIAL"},
			{L"XNOR", L"ALU"},
			{L"XOR", L"ALU"},
			{L"XORI", L"ALU"},
			{L"YIELD", L"SYS_SERIAL"},
			{L"ZEXT.H", L"ALU"},
			{L"ZEXT.W", L"ALU"}
		};
	}

	bool GeneratedIsaCatalog::TryGetDescriptor(unsigned int opcode, std::shared_ptr<GeneratedIsaDescriptor> &descriptor)
	{
		for (auto candidate : Descriptors)
		{
			if (candidate->Opcode == opcode)
			{
			descriptor = candidate;
			return true;
			}
		}
		descriptor = nullptr;
		return false;
	}

	std::vector<OpcodeInfo> GeneratedIsaCatalog::CreateOpcodeInfos()
	{
		std::vector<OpcodeInfo> opcodes;
		opcodes.reserve(Descriptors.size());
		for (const auto& descriptor : Descriptors)
		{
			unsigned int operandCount = 0;
			if (descriptor->OperandSchema != L"operands-0")
			{
				constexpr std::wstring_view prefix = L"operands-";
				if (!descriptor->OperandSchema.starts_with(prefix))
				{
					throw std::invalid_argument("Invalid generated ISA operand schema.");
				}
				const auto digits = std::wstring_view(descriptor->OperandSchema).substr(prefix.size());
				for (const wchar_t digit : digits)
				{
					if (digit < L'0' || digit > L'9')
					{
						throw std::invalid_argument("Invalid generated ISA operand count.");
					}
					operandCount = operandCount * 10 + static_cast<unsigned int>(digit - L'0');
				}
			}
			if (operandCount > 3)
			{
				throw std::out_of_range("Generated ISA operand count exceeds OpcodeInfo range.");
			}
			opcodes.emplace_back(descriptor->Opcode, descriptor->Mnemonic, descriptor->OpcodeCategory, static_cast<unsigned char>(operandCount), descriptor->InstructionFlags, descriptor->ExecutionLatency, descriptor->MemoryBandwidth, descriptor->StaticClass, descriptor->Serialization);
		}
		return opcodes;
	}
}


