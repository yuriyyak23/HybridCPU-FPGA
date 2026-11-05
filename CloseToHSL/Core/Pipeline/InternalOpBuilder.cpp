#include "InternalOpBuilder.h"
#include "../../../NonRTL/Arch/YAKSys_Hybrid_CPU.Arch.OpcodeRegistry.h"
#include "../../../NonRTL/Arch/OpcodeInfo.Types.h"
#include "../../../NonRTL/Arch/InstructionClass.h"
#include "../../../NonRTL/Arch/SerializationClass.h"
#include "../Diagnostics/YAKSys_Hybrid_CPU.Core.InstructionRegistry.h"
#include "MicroOps/System/MicroOp.System.h"
#include "MicroOps/Control/MicroOp.Control.h"
#include "../Virtualization/Compatibility/Frontend/Retire/VmxRetireModel.h"
#include "../../../Arch/CsrAddresses.h"

using namespace YAKSys_Hybrid_CPU::Arch;
using namespace YAKSys_Hybrid_CPU::Core::Pipeline::MicroOps;
using namespace YAKSys_Hybrid_CPU::Core::Registers;
using OpcodeValues = YAKSys_Hybrid_CPU::Processor::CPU_Core::IsaOpcodeValues;

namespace
{
	std::shared_ptr<InstructionIR> MakeInstructionIR(unsigned short opcode, InstructionClass instructionClass, SerializationClass serializationClass, unsigned char rd = 0)
	{
		auto instruction = std::make_shared<InstructionIR>();
		instruction->CanonicalOpcode = std::make_shared<YAKSys_Hybrid_CPU::Processor::CPU_Core::IsaOpcode>(opcode);
		instruction->Class = instructionClass;
		instruction->SerializationClass = serializationClass;
		instruction->Rd = rd;
		return instruction;
	}
	bool HasInstructionFlag(InstructionFlags flags, InstructionFlags flag)
	{
		return (static_cast<int>(flags) & static_cast<int>(flag)) != 0;
	}
}

namespace YAKSys_Hybrid_CPU::Core::Pipeline
{

	std::shared_ptr<InternalOp> InternalOpBuilder::Build(std::shared_ptr<InstructionIR> instruction)
	{
		unsigned short opcode = instruction->CanonicalOpcode->Value;
		InternalOpKind kind = MapToKind(opcode);
		InternalOpFlags flags = DeriveFlags(opcode);
		flags |= DeriveAtomicOrderingFlags(kind, instruction);

		std::shared_ptr<InternalOp> temp_var = std::make_shared<InternalOp>();
		temp_var->Kind = kind;
		temp_var->Rs1 = instruction->Rs1;
		temp_var->Rs2 = instruction->Rs2;
		temp_var->Rd = instruction->Rd;
		temp_var->Immediate = instruction->Imm;
		temp_var->DataType = ResolveDataType(opcode, instruction);
		temp_var->CsrTarget = ResolveCsrTarget(opcode, instruction);
		temp_var->Flags = flags;
		return temp_var;
	}

	InternalOpKind InternalOpBuilder::MapToKind(unsigned short opcode)
	{
		std::optional<OpcodeInfo> opcodeInfo = OpcodeRegistry::GetInfo(opcode);
		if (opcodeInfo)
		{
			switch (opcodeInfo.value().InstructionClass)
			{
				case InstructionClass::ScalarAlu:
// C# TO C++ CONVERTER NOTE: The following 'switch' operated on a string and was converted to C++ 'if-else' logic:
//					switch (opcodeInfo.Value.Mnemonic)
// ORIGINAL LINE: case "ADD":
					if (opcodeInfo.value().Mnemonic == L"ADD")
					{
							return InternalOpKind::Add;
					}
// ORIGINAL LINE: case "ADDW":
					else if (opcodeInfo.value().Mnemonic == L"ADDW")
					{
							return InternalOpKind::AddW;
					}
// ORIGINAL LINE: case "SUBW":
					else if (opcodeInfo.value().Mnemonic == L"SUBW")
					{
							return InternalOpKind::SubW;
					}
// ORIGINAL LINE: case "SLLW":
					else if (opcodeInfo.value().Mnemonic == L"SLLW")
					{
							return InternalOpKind::SllW;
					}
// ORIGINAL LINE: case "SRLW":
					else if (opcodeInfo.value().Mnemonic == L"SRLW")
					{
							return InternalOpKind::SrlW;
					}
// ORIGINAL LINE: case "SRAW":
					else if (opcodeInfo.value().Mnemonic == L"SRAW")
					{
							return InternalOpKind::SraW;
					}
// ORIGINAL LINE: case "SUB":
					else if (opcodeInfo.value().Mnemonic == L"SUB")
					{
							return InternalOpKind::Sub;
					}
// ORIGINAL LINE: case "MUL":
					else if (opcodeInfo.value().Mnemonic == L"MUL")
					{
							return InternalOpKind::Mul;
					}
// ORIGINAL LINE: case "MULW":
					else if (opcodeInfo.value().Mnemonic == L"MULW")
					{
							return InternalOpKind::MulW;
					}
// ORIGINAL LINE: case "DIVW":
					else if (opcodeInfo.value().Mnemonic == L"DIVW")
					{
							return InternalOpKind::DivW;
					}
// ORIGINAL LINE: case "DIVUW":
					else if (opcodeInfo.value().Mnemonic == L"DIVUW")
					{
							return InternalOpKind::DivuW;
					}
// ORIGINAL LINE: case "REMW":
					else if (opcodeInfo.value().Mnemonic == L"REMW")
					{
							return InternalOpKind::RemW;
					}
// ORIGINAL LINE: case "REMUW":
					else if (opcodeInfo.value().Mnemonic == L"REMUW")
					{
							return InternalOpKind::RemuW;
					}
// ORIGINAL LINE: case "SEXT.W":
					else if (opcodeInfo.value().Mnemonic == L"SEXT.W")
					{
							return InternalOpKind::SextW;
					}
// ORIGINAL LINE: case "ZEXT.W":
					else if (opcodeInfo.value().Mnemonic == L"ZEXT.W")
					{
							return InternalOpKind::ZextW;
					}
// ORIGINAL LINE: case "CZERO.EQZ":
					else if (opcodeInfo.value().Mnemonic == L"CZERO.EQZ")
					{
							return InternalOpKind::CzeroEqz;
					}
// ORIGINAL LINE: case "CZERO.NEZ":
					else if (opcodeInfo.value().Mnemonic == L"CZERO.NEZ")
					{
							return InternalOpKind::CzeroNez;
					}
// ORIGINAL LINE: case "CLZ":
					else if (opcodeInfo.value().Mnemonic == L"CLZ")
					{
							return InternalOpKind::Clz;
					}
// ORIGINAL LINE: case "CTZ":
					else if (opcodeInfo.value().Mnemonic == L"CTZ")
					{
							return InternalOpKind::Ctz;
					}
// ORIGINAL LINE: case "CPOP":
					else if (opcodeInfo.value().Mnemonic == L"CPOP")
					{
							return InternalOpKind::Cpop;
					}
// ORIGINAL LINE: case "SEXT.B":
					else if (opcodeInfo.value().Mnemonic == L"SEXT.B")
					{
							return InternalOpKind::SextB;
					}
// ORIGINAL LINE: case "SEXT.H":
					else if (opcodeInfo.value().Mnemonic == L"SEXT.H")
					{
							return InternalOpKind::SextH;
					}
// ORIGINAL LINE: case "ZEXT.H":
					else if (opcodeInfo.value().Mnemonic == L"ZEXT.H")
					{
							return InternalOpKind::ZextH;
					}
// ORIGINAL LINE: case "ROL":
					else if (opcodeInfo.value().Mnemonic == L"ROL")
					{
							return InternalOpKind::Rol;
					}
// ORIGINAL LINE: case "ROR":
					else if (opcodeInfo.value().Mnemonic == L"ROR")
					{
							return InternalOpKind::Ror;
					}
// ORIGINAL LINE: case "ROLI":
					else if (opcodeInfo.value().Mnemonic == L"ROLI")
					{
							return InternalOpKind::RolI;
					}
// ORIGINAL LINE: case "RORI":
					else if (opcodeInfo.value().Mnemonic == L"RORI")
					{
							return InternalOpKind::RorI;
					}
// ORIGINAL LINE: case "BSET":
					else if (opcodeInfo.value().Mnemonic == L"BSET")
					{
							return InternalOpKind::Bset;
					}
// ORIGINAL LINE: case "BCLR":
					else if (opcodeInfo.value().Mnemonic == L"BCLR")
					{
							return InternalOpKind::Bclr;
					}
// ORIGINAL LINE: case "BINV":
					else if (opcodeInfo.value().Mnemonic == L"BINV")
					{
							return InternalOpKind::Binv;
					}
// ORIGINAL LINE: case "BEXT":
					else if (opcodeInfo.value().Mnemonic == L"BEXT")
					{
							return InternalOpKind::Bext;
					}
// ORIGINAL LINE: case "BSETI":
					else if (opcodeInfo.value().Mnemonic == L"BSETI")
					{
							return InternalOpKind::BsetI;
					}
// ORIGINAL LINE: case "BCLRI":
					else if (opcodeInfo.value().Mnemonic == L"BCLRI")
					{
							return InternalOpKind::BclrI;
					}
// ORIGINAL LINE: case "BINVI":
					else if (opcodeInfo.value().Mnemonic == L"BINVI")
					{
							return InternalOpKind::BinvI;
					}
// ORIGINAL LINE: case "BEXTI":
					else if (opcodeInfo.value().Mnemonic == L"BEXTI")
					{
							return InternalOpKind::BextI;
					}
// ORIGINAL LINE: case "ANDN":
					else if (opcodeInfo.value().Mnemonic == L"ANDN")
					{
							return InternalOpKind::AndN;
					}
// ORIGINAL LINE: case "ORN":
					else if (opcodeInfo.value().Mnemonic == L"ORN")
					{
							return InternalOpKind::OrN;
					}
// ORIGINAL LINE: case "XNOR":
					else if (opcodeInfo.value().Mnemonic == L"XNOR")
					{
							return InternalOpKind::Xnor;
					}
// ORIGINAL LINE: case "MIN":
					else if (opcodeInfo.value().Mnemonic == L"MIN")
					{
							return InternalOpKind::Min;
					}
// ORIGINAL LINE: case "MAX":
					else if (opcodeInfo.value().Mnemonic == L"MAX")
					{
							return InternalOpKind::Max;
					}
// ORIGINAL LINE: case "MINU":
					else if (opcodeInfo.value().Mnemonic == L"MINU")
					{
							return InternalOpKind::MinU;
					}
// ORIGINAL LINE: case "MAXU":
					else if (opcodeInfo.value().Mnemonic == L"MAXU")
					{
							return InternalOpKind::MaxU;
					}
// ORIGINAL LINE: case "REV8":
					else if (opcodeInfo.value().Mnemonic == L"REV8")
					{
							return InternalOpKind::Rev8;
					}
// ORIGINAL LINE: case "BREV8":
					else if (opcodeInfo.value().Mnemonic == L"BREV8")
					{
							return InternalOpKind::Brev8;
					}
// ORIGINAL LINE: case "SH1ADD":
					else if (opcodeInfo.value().Mnemonic == L"SH1ADD")
					{
							return InternalOpKind::Sh1Add;
					}
// ORIGINAL LINE: case "SH2ADD":
					else if (opcodeInfo.value().Mnemonic == L"SH2ADD")
					{
							return InternalOpKind::Sh2Add;
					}
// ORIGINAL LINE: case "SH3ADD":
					else if (opcodeInfo.value().Mnemonic == L"SH3ADD")
					{
							return InternalOpKind::Sh3Add;
					}
// ORIGINAL LINE: case "ADD.UW":
					else if (opcodeInfo.value().Mnemonic == L"ADD.UW")
					{
							return InternalOpKind::AddUw;
					}
// ORIGINAL LINE: case "SH1ADD.UW":
					else if (opcodeInfo.value().Mnemonic == L"SH1ADD.UW")
					{
							return InternalOpKind::Sh1AddUw;
					}
// ORIGINAL LINE: case "SH2ADD.UW":
					else if (opcodeInfo.value().Mnemonic == L"SH2ADD.UW")
					{
							return InternalOpKind::Sh2AddUw;
					}
// ORIGINAL LINE: case "SH3ADD.UW":
					else if (opcodeInfo.value().Mnemonic == L"SH3ADD.UW")
					{
							return InternalOpKind::Sh3AddUw;
					}
// ORIGINAL LINE: case "SLLI.UW":
					else if (opcodeInfo.value().Mnemonic == L"SLLI.UW")
					{
							return InternalOpKind::SlliUw;
					}
// ORIGINAL LINE: case "CLMUL":
					else if (opcodeInfo.value().Mnemonic == L"CLMUL")
					{
							return InternalOpKind::ClMul;
					}
// ORIGINAL LINE: case "CLMULH":
					else if (opcodeInfo.value().Mnemonic == L"CLMULH")
					{
							return InternalOpKind::ClMulH;
					}
// ORIGINAL LINE: case "CLMULR":
					else if (opcodeInfo.value().Mnemonic == L"CLMULR")
					{
							return InternalOpKind::ClMulR;
					}
// ORIGINAL LINE: case "DIV":
					else if (opcodeInfo.value().Mnemonic == L"DIV")
					{
							return InternalOpKind::Div;
					}
// ORIGINAL LINE: case "SLT":
					else if (opcodeInfo.value().Mnemonic == L"SLT")
					{
							return InternalOpKind::Slt;
					}
// ORIGINAL LINE: case "SLTU":
					else if (opcodeInfo.value().Mnemonic == L"SLTU")
					{
							return InternalOpKind::Sltu;
					}
// ORIGINAL LINE: case "MULH":
					else if (opcodeInfo.value().Mnemonic == L"MULH")
					{
							return InternalOpKind::MulH;
					}
// ORIGINAL LINE: case "MULHU":
					else if (opcodeInfo.value().Mnemonic == L"MULHU")
					{
							return InternalOpKind::MulHu;
					}
// ORIGINAL LINE: case "MULHSU":
					else if (opcodeInfo.value().Mnemonic == L"MULHSU")
					{
							return InternalOpKind::MulHsu;
					}
// ORIGINAL LINE: case "DIVU":
					else if (opcodeInfo.value().Mnemonic == L"DIVU")
					{
							return InternalOpKind::Divu;
					}
// ORIGINAL LINE: case "REM":
					else if (opcodeInfo.value().Mnemonic == L"REM")
					{
							return InternalOpKind::Rem;
					}
// ORIGINAL LINE: case "REMU":
					else if (opcodeInfo.value().Mnemonic == L"REMU")
					{
							return InternalOpKind::Remu;
					}
// ORIGINAL LINE: case "ADDI":
					else if (opcodeInfo.value().Mnemonic == L"ADDI")
					{
							return InternalOpKind::AddI;
					}
// ORIGINAL LINE: case "ADDIW":
					else if (opcodeInfo.value().Mnemonic == L"ADDIW")
					{
							return InternalOpKind::AddIW;
					}
// ORIGINAL LINE: case "ANDI":
					else if (opcodeInfo.value().Mnemonic == L"ANDI")
					{
							return InternalOpKind::AndI;
					}
// ORIGINAL LINE: case "ORI":
					else if (opcodeInfo.value().Mnemonic == L"ORI")
					{
							return InternalOpKind::OrI;
					}
// ORIGINAL LINE: case "XORI":
					else if (opcodeInfo.value().Mnemonic == L"XORI")
					{
							return InternalOpKind::XorI;
					}
// ORIGINAL LINE: case "SLTI":
					else if (opcodeInfo.value().Mnemonic == L"SLTI")
					{
							return InternalOpKind::SltI;
					}
// ORIGINAL LINE: case "SLTIU":
					else if (opcodeInfo.value().Mnemonic == L"SLTIU")
					{
							return InternalOpKind::SltiU;
					}
// ORIGINAL LINE: case "SLLI":
					else if (opcodeInfo.value().Mnemonic == L"SLLI")
					{
							return InternalOpKind::SllI;
					}
// ORIGINAL LINE: case "SLLIW":
					else if (opcodeInfo.value().Mnemonic == L"SLLIW")
					{
							return InternalOpKind::SllIW;
					}
// ORIGINAL LINE: case "SRLIW":
					else if (opcodeInfo.value().Mnemonic == L"SRLIW")
					{
							return InternalOpKind::SrlIW;
					}
// ORIGINAL LINE: case "SRAIW":
					else if (opcodeInfo.value().Mnemonic == L"SRAIW")
					{
							return InternalOpKind::SraIW;
					}
// ORIGINAL LINE: case "SRLI":
					else if (opcodeInfo.value().Mnemonic == L"SRLI")
					{
							return InternalOpKind::SrlI;
					}
// ORIGINAL LINE: case "SRA":
					else if (opcodeInfo.value().Mnemonic == L"SRA")
					{
							return InternalOpKind::Sra;
					}
// ORIGINAL LINE: case "SRAI":
					else if (opcodeInfo.value().Mnemonic == L"SRAI")
					{
							return InternalOpKind::SraI;
					}
// ORIGINAL LINE: case "LUI":
					else if (opcodeInfo.value().Mnemonic == L"LUI")
					{
							return InternalOpKind::Lui;
					}
// ORIGINAL LINE: case "AUIPC":
					else if (opcodeInfo.value().Mnemonic == L"AUIPC")
					{
							return InternalOpKind::Auipc;
					}
					break;

				case InstructionClass::ControlFlow:
	// C# TO C++ CONVERTER TASK: The following 'switch expression' was not converted by C# to C++ Converter:
	//						return opcode switch
	//						{
	//							OpcodeValues.JAL => InternalOpKind.Jal,
	//							OpcodeValues.JALR => InternalOpKind.Jalr,
	//							_ => InternalOpKind.Branch,
	//						};

				case InstructionClass::Atomic:
				{
					InstructionFlags atomicFlags = opcodeInfo.value().Flags;
					auto atomicInstruction = MakeInstructionIR(opcode, InstructionClass::Atomic, opcodeInfo.value().SerializationClass);
					unsigned char accessSize;
					if (!InstructionRegistry::TryResolvePublishedAtomicAccessSize(atomicInstruction, accessSize))
					{
						throw InvalidOperationException(L"Published atomic opcode did not resolve an authoritative access size.");
					}

					bool isDwordAtomic = accessSize > 4;
					if (opcodeInfo.value().OperandCount == 1 && HasInstructionFlag(atomicFlags, InstructionFlags::MemoryRead) && !HasInstructionFlag(atomicFlags, InstructionFlags::MemoryWrite))
					{
						return isDwordAtomic ? InternalOpKind::LrD : InternalOpKind::LrW;
					}

					if (opcodeInfo.value().OperandCount == 2 && !HasInstructionFlag(atomicFlags, InstructionFlags::MemoryRead) && HasInstructionFlag(atomicFlags, InstructionFlags::MemoryWrite))
					{
						return isDwordAtomic ? InternalOpKind::ScD : InternalOpKind::ScW;
					}

					if (HasInstructionFlag(atomicFlags, InstructionFlags::MemoryRead) && HasInstructionFlag(atomicFlags, InstructionFlags::MemoryWrite))
					{
						return isDwordAtomic ? InternalOpKind::AmoDword : InternalOpKind::AmoWord;
					}
					break;

				}
// C# TO C++ CONVERTER TASK: The 'when' keyword in 'case' statements is not converted by C# to C++ Converter:
// ORIGINAL LINE: case InstructionClass.Memory when !opcodeInfo.Value.IsVector:
				case InstructionClass::Memory:
					if (!OpcodeRegistry::IsVectorOp(opcode) && HasInstructionFlag(opcodeInfo.value().Flags, InstructionFlags::MemoryRead) && !HasInstructionFlag(opcodeInfo.value().Flags, InstructionFlags::MemoryWrite))
					{
						return InternalOpKind::Load;
					}

					if (!OpcodeRegistry::IsVectorOp(opcode) && HasInstructionFlag(opcodeInfo.value().Flags, InstructionFlags::MemoryWrite))
					{
						return InternalOpKind::Store;
					}
					break;

				case InstructionClass::System:
				{
					if (!OpcodeRegistry::IsVectorOp(opcode) && opcodeInfo.value().OperandCount > 0)
					{
						return InternalOpKind::Store;
					}

					auto systemInstruction = MakeInstructionIR(opcode, InstructionClass::System, opcodeInfo.value().SerializationClass);

					SystemEventKind systemEventKind;
					if (InstructionRegistry::TryResolvePublishedSystemEventKind(systemInstruction, systemEventKind))
					{
	// C# TO C++ CONVERTER TASK: The following 'switch expression' was not converted by C# to C++ Converter:
	//							return systemEventKind switch
	//							{
	//								SystemEventKind.Fence => InternalOpKind.Fence,
	//								SystemEventKind.FenceI => InternalOpKind.FenceI,
	//								SystemEventKind.Ecall => InternalOpKind.Ecall,
	//								SystemEventKind.Ebreak => InternalOpKind.Ebreak,
	//								SystemEventKind.Mret => InternalOpKind.Mret,
	//								SystemEventKind.Sret => InternalOpKind.Sret,
	//								SystemEventKind.Wfi => InternalOpKind.Wfi,
	//								_ => throw new InvalidOperationException(
	//									$"Published system opcode {opcode} did not resolve to an authoritative internal-op kind."), };
					}
					break;

				}
				case InstructionClass::Csr:
				{
					auto csrInstruction = MakeInstructionIR(opcode, InstructionClass::Csr, opcodeInfo.value().SerializationClass, opcodeInfo.value().OperandCount == 0 ? 0 : 1);

					std::shared_ptr<CSRMicroOp> csrMicroOp;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: if (InstructionRegistry.TryCreatePublishedCsrMicroOp(in csrInstruction, out CSRMicroOp? csrMicroOp) && csrMicroOp != null)
					if (InstructionRegistry::TryCreatePublishedCsrMicroOp(csrInstruction, csrMicroOp) && csrMicroOp != nullptr)
					{
						if (std::dynamic_pointer_cast<CsrReadWriteMicroOp>(csrMicroOp) != nullptr || std::dynamic_pointer_cast<CsrReadWriteImmediateMicroOp>(csrMicroOp) != nullptr)
						{
							return InternalOpKind::CsrReadWrite;
						}

						if (std::dynamic_pointer_cast<CsrReadSetMicroOp>(csrMicroOp) != nullptr || std::dynamic_pointer_cast<CsrReadSetImmediateMicroOp>(csrMicroOp) != nullptr)
						{
							return InternalOpKind::CsrReadSet;
						}

						if (std::dynamic_pointer_cast<CsrReadClearMicroOp>(csrMicroOp) != nullptr || std::dynamic_pointer_cast<CsrReadClearImmediateMicroOp>(csrMicroOp) != nullptr)
						{
							return InternalOpKind::CsrReadClear;
						}

						if (std::dynamic_pointer_cast<CsrClearMicroOp>(csrMicroOp) != nullptr)
						{
							return InternalOpKind::CsrClear;
						}

						if (std::dynamic_pointer_cast<CsrReadCounterMicroOp>(csrMicroOp) != nullptr)
						{
							return InternalOpKind::CsrReadCounter;
						}
					}
					break;

				}
				case InstructionClass::SmtVt:
				{
					auto smtVtInstruction = MakeInstructionIR(opcode, InstructionClass::SmtVt, opcodeInfo.value().SerializationClass);

					SystemEventKind smtVtEventKind;
					if (InstructionRegistry::TryResolvePublishedSystemEventKind(smtVtInstruction, smtVtEventKind))
					{
	// C# TO C++ CONVERTER TASK: The following 'switch expression' was not converted by C# to C++ Converter:
	//							return smtVtEventKind switch
	//							{
	//								SystemEventKind.Wfe => InternalOpKind.Wfe,
	//								SystemEventKind.Sev => InternalOpKind.Sev,
	//								SystemEventKind.PodBarrier => InternalOpKind.PodBarrier,
	//								SystemEventKind.VtBarrier => InternalOpKind.VtBarrier,
	//								_ => InternalOpKind.Yield,
	//							};
					}

					if (!HasInstructionFlag(opcodeInfo.value().Flags, InstructionFlags::Privileged))
					{
						return InternalOpKind::Yield;
					}
					break;

				}
				case InstructionClass::Vmx:
				{
					auto vmxInstruction = MakeInstructionIR(opcode, InstructionClass::Vmx, opcodeInfo.value().SerializationClass);

					VmxOperationKind operationKind;
					if (InstructionRegistry::TryResolvePublishedVmxOperationKind(vmxInstruction, operationKind))
					{
	// C# TO C++ CONVERTER TASK: The following 'switch expression' was not converted by C# to C++ Converter:
	//							return operationKind switch
	//							{
	//								VmxOperationKind.VmxOn => InternalOpKind.VmxOn,
	//								VmxOperationKind.VmxOff => InternalOpKind.VmxOff,
	//								VmxOperationKind.VmLaunch => InternalOpKind.VmLaunch,
	//								VmxOperationKind.VmResume => InternalOpKind.VmResume,
	//								VmxOperationKind.VmRead => InternalOpKind.VmRead,
	//								VmxOperationKind.VmWrite => InternalOpKind.VmWrite,
	//								VmxOperationKind.VmClear => InternalOpKind.VmClear,
	//								VmxOperationKind.VmPtrLd => InternalOpKind.VmPtrLd,
	//								VmxOperationKind.VmPtrSt => InternalOpKind.VmPtrSt,
	//								VmxOperationKind.VmCall => InternalOpKind.VmCall,
	//								VmxOperationKind.Invept => InternalOpKind.Invept,
	//								VmxOperationKind.Invvpid => InternalOpKind.Invvpid,
	//								VmxOperationKind.VmFunc => InternalOpKind.VmFunc,
	//								VmxOperationKind.VmSaveX => InternalOpKind.VmSaveX,
	//								VmxOperationKind.VmRestX => InternalOpKind.VmRestX,
	//								_ => throw new InvalidOperationException(
	//									$"Published VMX opcode {opcode} did not resolve to an authoritative internal-op kind."), };
					}
					break;
				}
			}
		}

	// C# TO C++ CONVERTER TASK: The following 'switch expression' was not converted by C# to C++ Converter:
	//			return opcode switch
	//			{
	//				// ── Scalar ALU reg-reg ────────────────────────────────────────
	//				OpcodeValues.ADDW => InternalOpKind.AddW,
	//				OpcodeValues.SUBW => InternalOpKind.SubW,
	//				OpcodeValues.SLLW => InternalOpKind.SllW,
	//				OpcodeValues.SRLW => InternalOpKind.SrlW,
	//				OpcodeValues.SRAW => InternalOpKind.SraW,
	//				OpcodeValues.MULW => InternalOpKind.MulW,
	//				OpcodeValues.DIVW => InternalOpKind.DivW,
	//				OpcodeValues.DIVUW => InternalOpKind.DivuW,
	//				OpcodeValues.REMW => InternalOpKind.RemW,
	//				OpcodeValues.REMUW => InternalOpKind.RemuW,
	//				OpcodeValues.SEXT_W => InternalOpKind.SextW,
	//				OpcodeValues.ZEXT_W => InternalOpKind.ZextW,
	//				OpcodeValues.CZERO_EQZ => InternalOpKind.CzeroEqz,
	//				OpcodeValues.CZERO_NEZ => InternalOpKind.CzeroNez,
	//				OpcodeValues.CLZ => InternalOpKind.Clz,
	//				OpcodeValues.CTZ => InternalOpKind.Ctz,
	//				OpcodeValues.CPOP => InternalOpKind.Cpop,
	//				OpcodeValues.SEXT_B => InternalOpKind.SextB,
	//				OpcodeValues.SEXT_H => InternalOpKind.SextH,
	//				OpcodeValues.ZEXT_H => InternalOpKind.ZextH,
	//				OpcodeValues.ROL => InternalOpKind.Rol,
	//				OpcodeValues.ROR => InternalOpKind.Ror,
	//				OpcodeValues.ROLI => InternalOpKind.RolI,
	//				OpcodeValues.RORI => InternalOpKind.RorI,
	//				OpcodeValues.BSET => InternalOpKind.Bset,
	//				OpcodeValues.BCLR => InternalOpKind.Bclr,
	//				OpcodeValues.BINV => InternalOpKind.Binv,
	//				OpcodeValues.BEXT => InternalOpKind.Bext,
	//				OpcodeValues.BSETI => InternalOpKind.BsetI,
	//				OpcodeValues.BCLRI => InternalOpKind.BclrI,
	//				OpcodeValues.BINVI => InternalOpKind.BinvI,
	//				OpcodeValues.BEXTI => InternalOpKind.BextI,
	//				OpcodeValues.ANDN => InternalOpKind.AndN,
	//				OpcodeValues.ORN => InternalOpKind.OrN,
	//				OpcodeValues.XNOR => InternalOpKind.Xnor,
	//				OpcodeValues.MIN => InternalOpKind.Min,
	//				OpcodeValues.MAX => InternalOpKind.Max,
	//				OpcodeValues.MINU => InternalOpKind.MinU,
	//				OpcodeValues.MAXU => InternalOpKind.MaxU,
	//				OpcodeValues.REV8 => InternalOpKind.Rev8,
	//				OpcodeValues.BREV8 => InternalOpKind.Brev8,
	//				OpcodeValues.SH1ADD => InternalOpKind.Sh1Add,
	//				OpcodeValues.SH2ADD => InternalOpKind.Sh2Add,
	//				OpcodeValues.SH3ADD => InternalOpKind.Sh3Add,
	//				OpcodeValues.ADD_UW => InternalOpKind.AddUw,
	//				OpcodeValues.SH1ADD_UW => InternalOpKind.Sh1AddUw,
	//				OpcodeValues.SH2ADD_UW => InternalOpKind.Sh2AddUw,
	//				OpcodeValues.SH3ADD_UW => InternalOpKind.Sh3AddUw,
	//				OpcodeValues.SLLI_UW => InternalOpKind.SlliUw,
	//				OpcodeValues.CLMUL => InternalOpKind.ClMul,
	//				OpcodeValues.CLMULH => InternalOpKind.ClMulH,
	//				OpcodeValues.CLMULR => InternalOpKind.ClMulR,
	//				OpcodeValues.SLL => InternalOpKind.Sll,
	//				OpcodeValues.SRL => InternalOpKind.Srl,
	//				OpcodeValues.SRA => InternalOpKind.Sra,
	//				OpcodeValues.XOR => InternalOpKind.Xor,
	//				OpcodeValues.OR => InternalOpKind.Or,
	//				OpcodeValues.AND => InternalOpKind.And,
	//
	//				// ── Scalar M-extension ────────────────────────────────────────
	//
	//				// ── Scalar ALU immediate ──────────────────────────────────────
	//
	//				// ── CSR ───────────────────────────────────────────────────────
	//
	//				// ── SMT/VT ────────────────────────────────────────────────────
	//
	//				// ── VMX ───────────────────────────────────────────────────────
	//
	//				// ── Nop ───────────────────────────────────────────────────────
	//				OpcodeValues.Nope => InternalOpKind.AddI, // NOP ≡ ADDI x0, x0, 0
	//
	//				_ => throw new ArgumentOutOfRangeException(nameof(opcode),
	//						 $"No InternalOpKind mapping for opcode {OpcodeRegistry.GetMnemonicOrHex(opcode)} ({opcode}). " +
	//						 "Vector opcodes are not mapped through InternalOpBuilder — they use the vector execution path."), };
	}

	InternalOpDataType InternalOpBuilder::ResolveDataType(unsigned short opcode, std::shared_ptr<InstructionIR> instruction)
	{
	// C# TO C++ CONVERTER TASK: The following 'switch expression' was not converted by C# to C++ Converter:
	//			return opcode switch
	//			{
	//				OpcodeValues.SEXT_B => InternalOpDataType.Byte,
	//				OpcodeValues.SEXT_H or OpcodeValues.ZEXT_H => InternalOpDataType.Half,
	//				OpcodeValues.ADDIW or OpcodeValues.SLLIW or OpcodeValues.SRLIW or OpcodeValues.SRAIW or OpcodeValues.ADDW or OpcodeValues.SUBW or OpcodeValues.SLLW or OpcodeValues.SRLW or OpcodeValues.SRAW or OpcodeValues.MULW or OpcodeValues.DIVW or OpcodeValues.DIVUW or OpcodeValues.REMW or OpcodeValues.REMUW or OpcodeValues.SEXT_W or OpcodeValues.ZEXT_W => InternalOpDataType.Word,
	//				_ => instruction.DataType,
	//			};
	}

	std::optional<unsigned short> InternalOpBuilder::ResolveCsrTarget(unsigned short opcode, std::shared_ptr<InstructionIR> instruction)
	{
		if (opcode == OpcodeValues::RDCYCLE)
		{
			return CsrAddresses::Cycle;
		}

		if (instruction->Class != InstructionClass::Csr)
			return std::nullopt;
		return instruction->CsrAddress ? instruction->CsrAddress : std::optional<unsigned short>(static_cast<unsigned short>(instruction->Imm & 0xFFF));
	}

	InternalOpFlags InternalOpBuilder::DeriveFlags(unsigned short opcode)
	{
		std::optional<OpcodeInfo> opcodeInfo = OpcodeRegistry::GetInfo(opcode);
		InternalOpKind kind = MapToKind(opcode);

		if ((kind == InternalOpKind::Div || kind == InternalOpKind::DivW || kind == InternalOpKind::RemW || kind == InternalOpKind::SextW || kind == InternalOpKind::SextB || kind == InternalOpKind::SextH || kind == InternalOpKind::Rem || kind == InternalOpKind::Slt || kind == InternalOpKind::SltI))
		{
			return InternalOpFlags::Signed;
		}

		if ((kind == InternalOpKind::Sra || kind == InternalOpKind::SraW || kind == InternalOpKind::SraI || kind == InternalOpKind::SraIW))
		{
			return InternalOpFlags::ArithmeticShift;
		}

		if (opcodeInfo)
		{
			const std::wstring &mnemonic = opcodeInfo.value().Mnemonic;
			if (opcodeInfo.value().InstructionClass == InstructionClass::ControlFlow && (mnemonic == L"BLT" || mnemonic == L"BGE"))
			{
				return InternalOpFlags::Signed;
			}
		}
		return InternalOpFlags::None;

	// C# TO C++ CONVERTER TASK: The following 'switch expression' was not converted by C# to C++ Converter:
	//			return opcode switch
	//			{
	//				_ => InternalOpFlags.None,
	//			};
	}

	InternalOpFlags InternalOpBuilder::DeriveAtomicOrderingFlags(InternalOpKind kind, std::shared_ptr<InstructionIR> instruction)
	{
// C# TO C++ CONVERTER TASK: The following 'is' operator pattern is not converted by C# to C++ Converter:
		if ((kind != InternalOpKind::LrW && kind != InternalOpKind::ScW && kind != InternalOpKind::LrD && kind != InternalOpKind::ScD && kind != InternalOpKind::AmoWord && kind != InternalOpKind::AmoDword))
		{
			return InternalOpFlags::None;
		}

		InternalOpFlags flags = InternalOpFlags::None;
		if (instruction->AcquireOrdering)
		{
			flags |= InternalOpFlags::AcquireOrdering;
		}

		if (instruction->ReleaseOrdering)
		{
			flags |= InternalOpFlags::ReleaseOrdering;
		}

		return flags;
	}
}
