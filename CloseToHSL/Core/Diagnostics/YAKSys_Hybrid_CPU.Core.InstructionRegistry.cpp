#include "YAKSys_Hybrid_CPU.Core.InstructionRegistry.h"
#include <cwctype>
#include <stdexcept>
#include <typeinfo>
#include "DecodeProjectionFaultException.h"
#include "../../../NonRTL/Arch/YAKSys_Hybrid_CPU.Arch.OpcodeRegistry.h"
#include "../../../NonRTL/Arch/InstructionClassifier.h"
#include "../../../NonRTL/Arch/InstructionClass.h"
#include "../../../Arch/CsrAddresses.h"
#include "../ISA/Instructions/NonVmx/Lanes00_03Vector/MatrixTile/MatrixTileRuntimeOwnedVlmRows.h"
#include "../../../NonRTL/Arch/VectorLegalityMatrix.h"
#include "../Pipeline/MicroOps/Vector/MicroOp.Compute.h"
#include "../Pipeline/MicroOps/Vector/VectorMicroOps.Compute.h"
#include "../Pipeline/MicroOps/Vector/VectorMicroOps.Saturating.h"
#include "../Pipeline/MicroOps/Vector/VectorMicroOps.Data.h"
#include "../Pipeline/MicroOps/Vector/VectorMicroOps.Memory.h"
#include "../Pipeline/MicroOps/Vector/VectorMicroOps.Scan.h"
#include "../Pipeline/MicroOps/Vector/VectorMicroOps.Permute2.h"
#include "../Pipeline/MicroOps/Vector/VectorMicroOps.Transpose.h"
#include "../Pipeline/MicroOps/Vector/VectorMicroOps.SlideOne.h"
#include "../Pipeline/MicroOps/Vector/VectorMicroOps.DotWide.h"
#include "../Pipeline/MicroOps/Lane6DmaStream/DmaStreamComputeStatusMicroOp.h"
#include "../Pipeline/MicroOps/Lane6DmaStream/DmaStreamComputeQueryCapsMicroOp.h"
#include "../Pipeline/Scheduling/SlotLegality/SlotClassDefinitions.h"
#include "../Pipeline/MicroOps/Types/MicroOp.Misc.h"
#include "../Pipeline/MicroOps/Types/MicroOp.IO.h"
#include "../Pipeline/MicroOps/MatrixTile/MatrixTileMicroOps.h"
#include "../Pipeline/Metadata/ReadRangeMetadataHelper.h"
#include "../Pipeline/MicroOps/Memory/MicroOp.LoadStore.h"
#include "../Legality/BundleLegalityAnalyzer.h"

using namespace YAKSys_Hybrid_CPU::Arch;
using namespace HybridCPU_ISE::Arch;
using namespace YAKSys_Hybrid_CPU::Core::Pipeline::MicroOps;
using namespace YAKSys_Hybrid_CPU::Core::Registers;
using OpcodeValues = YAKSys_Hybrid_CPU::Processor::CPU_Core::IsaOpcodeValues;
using namespace YAKSys_Hybrid_CPU::Core::Legality;
using namespace YAKSys_Hybrid_CPU::CloseToHSL::Core::ISA::Instructions::NonVmx::Lanes00_03Vector::MatrixTile;
using namespace YAKSys_Hybrid_CPU::Core::Pipeline::Metadata;

namespace YAKSys_Hybrid_CPU::Core
{

	void InstructionRegistry::Clear()
	{
		_factories.clear();
		_descriptors.clear();
		_customAccelerators.clear();
		_customAcceleratorOpcodes.clear();
		_initialized = false;
	}

	int InstructionRegistry::GetRegisteredCount()
	{
		if (!_initialized)
		{
			Initialize();
		}

		return _factories.size();
	}

	const std::vector<unsigned int> InstructionRegistry::GetRegisteredOpcodes()
	{
		if (!_initialized)
		{
			Initialize();
		}

		std::vector<unsigned int> opcodes;
		opcodes.reserve(_factories.size());
		for (const auto &entry : _factories)
		{
			opcodes.push_back(entry.first);
		}
		std::sort(opcodes.begin(), opcodes.end());
		return opcodes;
	}

	void InstructionRegistry::RegisterAccelerator(std::shared_ptr<ICustomAccelerator> accelerator)
	{
		ArgumentNullException::ThrowIfNull(accelerator);

		_customAccelerators[accelerator->getName()] = accelerator;

		for (auto opcode : accelerator->getSupportedOpcodes())
		{
			_customAcceleratorOpcodes.insert(opcode);
		}
	}

	bool InstructionRegistry::IsCustomAcceleratorOpcode(unsigned int opCode)
	{
		return std::find(_customAcceleratorOpcodes.begin(), _customAcceleratorOpcodes.end(), opCode) != _customAcceleratorOpcodes.end();
	}

	std::shared_ptr<InvalidOpcodeException> InstructionRegistry::CreateUnsupportedCustomAcceleratorException(unsigned int opcode, const std::wstring &opcodeIdentifier, int slotIndex)
	{
		const auto message = std::format(L"Opcode '{}' (slot {}) targets a registered custom accelerator contour, but the current runtime has no truthful canonical publication, operand ABI, placement, DMA, or replay/retire follow-through for custom accelerators. Decode must fail closed instead of projecting scalar/system success.", opcodeIdentifier, slotIndex);
		return std::make_shared<InvalidOpcodeException>(message, opcodeIdentifier, slotIndex, false);
	}

	std::shared_ptr<InvalidOpcodeException> InstructionRegistry::CreateUnsupportedCustomAcceleratorException(unsigned int opcode)
	{
		return std::make_shared<InvalidOpcodeException>(std::format(L"Opcode 0x{0:0>X} reached the registered custom accelerator runtime surface, but the current runtime has no truthful canonical publication, operand ABI, placement, DMA, or replay/retire follow-through for custom accelerators. Direct/manual publication must fail closed.", opcode));
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public static ICustomAccelerator? GetAccelerator(string name)
	std::shared_ptr<ICustomAccelerator> InstructionRegistry::GetAccelerator(const std::wstring &name)
	{
		std::shared_ptr<ICustomAccelerator> accelerator;
		std::unordered_map<std::wstring, std::shared_ptr<ICustomAccelerator>>::const_iterator _customAccelerators_iterator = _customAccelerators.find(name);
		if (_customAccelerators_iterator != _customAccelerators.end())
		{
			accelerator = _customAccelerators_iterator->second;
		}
		return accelerator;
	}

	const std::unordered_map<std::wstring, std::shared_ptr<ICustomAccelerator>> InstructionRegistry::GetAllAccelerators()
	{
		return _customAccelerators;
	}

	unsigned long long InstructionRegistry::SignExtendScalarImmediate(unsigned short rawImmediate)
	{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: return unchecked((ulong)(long)(short)rawImmediate);
		return static_cast<unsigned long long>(static_cast<long long>(static_cast<short>(rawImmediate)));
	}

	void InstructionRegistry::RejectScalarImmediateRegisterFormAlias(DecoderContext const ctx)
	{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// C# TO C++ CONVERTER TASK: The following 'is' operator pattern is not converted by C# to C++ Converter:
// ORIGINAL LINE: if (unchecked((ushort)ctx.OpCode) is not(YAKSys_Hybrid_CPU.Processor.CPU_Core.IsaOpcodeValues.SLLIW or OpcodeValues.SRLIW or OpcodeValues.SRAIW) || ctx.Reg3ID == 0)
		const auto opcode = static_cast<unsigned short>(ctx.OpCode);
		if ((opcode != OpcodeValues::SLLIW && opcode != OpcodeValues::SRLIW && opcode != OpcodeValues::SRAIW) || ctx.Reg3ID == 0)
		{
			return;
		}

		throw DecodeProjectionFaultException(std::format(L"Scalar-immediate opcode {0:s} requires canonical Word1=(rd, rs1, x0). Register-form aliasing through rs2 is not accepted.", OpcodeRegistry::GetMnemonicOrHex(ctx.OpCode)));
	}

	void InstructionRegistry::RejectScalarUnaryRegisterFormAlias(DecoderContext const ctx)
	{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// C# TO C++ CONVERTER TASK: The following 'is' operator pattern is not converted by C# to C++ Converter:
// ORIGINAL LINE: if (unchecked((ushort)ctx.OpCode) is not(YAKSys_Hybrid_CPU.Processor.CPU_Core.IsaOpcodeValues.SEXT_W or OpcodeValues.ZEXT_W or OpcodeValues.CLZ or OpcodeValues.CTZ or OpcodeValues.CPOP or OpcodeValues.SEXT_B or OpcodeValues.SEXT_H or OpcodeValues.ZEXT_H or OpcodeValues.REV8 or OpcodeValues.BREV8))
		const auto opcode = static_cast<unsigned short>(ctx.OpCode);
		if (opcode != OpcodeValues::SEXT_W && opcode != OpcodeValues::ZEXT_W && opcode != OpcodeValues::CLZ && opcode != OpcodeValues::CTZ && opcode != OpcodeValues::CPOP && opcode != OpcodeValues::SEXT_B && opcode != OpcodeValues::SEXT_H && opcode != OpcodeValues::ZEXT_H && opcode != OpcodeValues::REV8 && opcode != OpcodeValues::BREV8)
		{
			return;
		}

		if (ctx.Reg3ID != 0)
		{
			throw DecodeProjectionFaultException(std::format(L"Scalar-unary opcode {0:s} requires canonical Word1=(rd, rs1, x0). Register-form aliasing through rs2 is not accepted.", OpcodeRegistry::GetMnemonicOrHex(ctx.OpCode)));
		}

		if (ctx.HasImmediate && ctx.Immediate != 0)
		{
			throw DecodeProjectionFaultException(std::format(L"Scalar-unary opcode {0:s} requires canonical Immediate=0. Immediate-form aliasing is not accepted.", OpcodeRegistry::GetMnemonicOrHex(ctx.OpCode)));
		}
	}

	void InstructionRegistry::RejectScalarAddressGenerationRegisterFormAlias(DecoderContext const ctx)
	{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// C# TO C++ CONVERTER TASK: The following 'is' operator pattern is not converted by C# to C++ Converter:
// ORIGINAL LINE: if (unchecked((ushort)ctx.OpCode) is not(YAKSys_Hybrid_CPU.Processor.CPU_Core.IsaOpcodeValues.SH1ADD or OpcodeValues.SH2ADD or OpcodeValues.SH3ADD or OpcodeValues.ADD_UW or OpcodeValues.SH1ADD_UW or OpcodeValues.SH2ADD_UW or OpcodeValues.SH3ADD_UW))
		const auto opcode = static_cast<unsigned short>(ctx.OpCode);
		if (opcode != OpcodeValues::SH1ADD && opcode != OpcodeValues::SH2ADD && opcode != OpcodeValues::SH3ADD && opcode != OpcodeValues::ADD_UW && opcode != OpcodeValues::SH1ADD_UW && opcode != OpcodeValues::SH2ADD_UW && opcode != OpcodeValues::SH3ADD_UW)
		{
			return;
		}

		if (ctx.HasImmediate && ctx.Immediate != 0)
		{
			throw DecodeProjectionFaultException(std::format(L"Scalar address-generation opcode {0:s} requires canonical Immediate=0. Immediate-form aliasing is not accepted.", OpcodeRegistry::GetMnemonicOrHex(ctx.OpCode)));
		}
	}

	unsigned short InstructionRegistry::GetScalarAddressGenerationImmediate(DecoderContext const ctx)
	{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: if (unchecked((ushort)ctx.OpCode) is not OpcodeValues.SLLI_UW)
		if (static_cast<unsigned short>(ctx.OpCode) != OpcodeValues::SLLI_UW)
		{
			return 0;
		}

		std::wstring opcodeName = OpcodeRegistry::GetMnemonicOrHex(ctx.OpCode);
		if (ctx.Reg3ID != 0)
		{
			throw DecodeProjectionFaultException(std::format(L"Scalar address-generation immediate opcode {0:s} requires canonical Word1=(rd, rs1, x0). ", opcodeName) + L"Register-form aliasing through rs2 is not accepted.");
		}

		unsigned short immediate = GetRequiredDecoderImmediate(ctx, std::format(L"Scalar address-generation immediate opcode {0:s}", opcodeName));

		if (immediate > 0x3F)
		{
			throw DecodeProjectionFaultException(std::format(L"Scalar address-generation immediate opcode {0:s} requires imm6 Immediate in [0, 63]. ", opcodeName) + L"Out-of-range immediate payloads are not accepted.");
		}

		return immediate;
	}

	void InstructionRegistry::RejectScalarCarryLessRegisterFormAlias(DecoderContext const ctx)
	{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// C# TO C++ CONVERTER TASK: The following 'is' operator pattern is not converted by C# to C++ Converter:
// ORIGINAL LINE: if (unchecked((ushort)ctx.OpCode) is not(YAKSys_Hybrid_CPU.Processor.CPU_Core.IsaOpcodeValues.CLMUL or OpcodeValues.CLMULH or OpcodeValues.CLMULR))
		const auto opcode = static_cast<unsigned short>(ctx.OpCode);
		if (opcode != OpcodeValues::CLMUL && opcode != OpcodeValues::CLMULH && opcode != OpcodeValues::CLMULR)
		{
			return;
		}

		if (ctx.HasImmediate && ctx.Immediate != 0)
		{
			throw DecodeProjectionFaultException(std::format(L"Scalar carry-less opcode {0:s} requires canonical Immediate=0. ", OpcodeRegistry::GetMnemonicOrHex(ctx.OpCode)) + L"Immediate-form aliasing is not accepted.");
		}
	}

	void InstructionRegistry::RejectScalarRotateRegisterFormAlias(DecoderContext const ctx)
	{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// C# TO C++ CONVERTER TASK: The following 'is' operator pattern is not converted by C# to C++ Converter:
// ORIGINAL LINE: if (unchecked((ushort)ctx.OpCode) is not(YAKSys_Hybrid_CPU.Processor.CPU_Core.IsaOpcodeValues.ROL or OpcodeValues.ROR))
		const auto opcode = static_cast<unsigned short>(ctx.OpCode);
		if (opcode != OpcodeValues::ROL && opcode != OpcodeValues::ROR)
		{
			return;
		}

		if (ctx.HasImmediate && ctx.Immediate != 0)
		{
			throw DecodeProjectionFaultException(std::format(L"Scalar rotate opcode {0:s} requires canonical Immediate=0. ", OpcodeRegistry::GetMnemonicOrHex(ctx.OpCode)) + L"Immediate-form aliasing is not accepted.");
		}
	}

	unsigned short InstructionRegistry::GetScalarRotateImmediate(DecoderContext const ctx)
	{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// C# TO C++ CONVERTER TASK: The following 'is' operator pattern is not converted by C# to C++ Converter:
// ORIGINAL LINE: if (unchecked((ushort)ctx.OpCode) is not(YAKSys_Hybrid_CPU.Processor.CPU_Core.IsaOpcodeValues.ROLI or OpcodeValues.RORI))
		const auto opcode = static_cast<unsigned short>(ctx.OpCode);
		if (opcode != OpcodeValues::ROLI && opcode != OpcodeValues::RORI)
		{
			return 0;
		}

		std::wstring opcodeName = OpcodeRegistry::GetMnemonicOrHex(ctx.OpCode);
		if (ctx.Reg3ID != 0)
		{
			throw DecodeProjectionFaultException(std::format(L"Scalar rotate-immediate opcode {0:s} requires canonical Word1=(rd, rs1, x0). ", opcodeName) + L"Register-form aliasing through rs2 is not accepted.");
		}

		unsigned short immediate = GetRequiredDecoderImmediate(ctx, std::format(L"Scalar rotate-immediate opcode {0:s}", opcodeName));

		if (immediate > 0x3F)
		{
			throw DecodeProjectionFaultException(std::format(L"Scalar rotate-immediate opcode {0:s} requires imm6 Immediate in [0, 63]. ", opcodeName) + L"Out-of-range immediate payloads are not accepted.");
		}

		return immediate;
	}

	void InstructionRegistry::RejectScalarBooleanInvertRegisterFormAlias(DecoderContext const ctx)
	{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// C# TO C++ CONVERTER TASK: The following 'is' operator pattern is not converted by C# to C++ Converter:
// ORIGINAL LINE: if (unchecked((ushort)ctx.OpCode) is not(YAKSys_Hybrid_CPU.Processor.CPU_Core.IsaOpcodeValues.ANDN or OpcodeValues.ORN or OpcodeValues.XNOR))
		const auto opcode = static_cast<unsigned short>(ctx.OpCode);
		if (opcode != OpcodeValues::ANDN && opcode != OpcodeValues::ORN && opcode != OpcodeValues::XNOR)
		{
			return;
		}

		if (ctx.HasImmediate && ctx.Immediate != 0)
		{
			throw DecodeProjectionFaultException(std::format(L"Scalar boolean-invert opcode {0:s} requires canonical Immediate=0. ", OpcodeRegistry::GetMnemonicOrHex(ctx.OpCode)) + L"Immediate-form aliasing is not accepted.");
		}
	}

	void InstructionRegistry::RejectScalarBitfieldRegisterFormAlias(DecoderContext const ctx)
	{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// C# TO C++ CONVERTER TASK: The following 'is' operator pattern is not converted by C# to C++ Converter:
// ORIGINAL LINE: if (unchecked((ushort)ctx.OpCode) is not(YAKSys_Hybrid_CPU.Processor.CPU_Core.IsaOpcodeValues.BSET or OpcodeValues.BCLR or OpcodeValues.BINV or OpcodeValues.BEXT))
		const auto opcode = static_cast<unsigned short>(ctx.OpCode);
		if (opcode != OpcodeValues::BSET && opcode != OpcodeValues::BCLR && opcode != OpcodeValues::BINV && opcode != OpcodeValues::BEXT)
		{
			return;
		}

		if (ctx.HasImmediate && ctx.Immediate != 0)
		{
			throw DecodeProjectionFaultException(std::format(L"Scalar bitfield opcode {0:s} requires canonical Immediate=0. ", OpcodeRegistry::GetMnemonicOrHex(ctx.OpCode)) + L"Immediate-form aliasing is not accepted.");
		}
	}

	unsigned short InstructionRegistry::GetScalarBitfieldImmediate(DecoderContext const ctx)
	{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// C# TO C++ CONVERTER TASK: The following 'is' operator pattern is not converted by C# to C++ Converter:
// ORIGINAL LINE: if (unchecked((ushort)ctx.OpCode) is not(YAKSys_Hybrid_CPU.Processor.CPU_Core.IsaOpcodeValues.BSETI or OpcodeValues.BCLRI or OpcodeValues.BINVI or OpcodeValues.BEXTI))
		const auto opcode = static_cast<unsigned short>(ctx.OpCode);
		if (opcode != OpcodeValues::BSETI && opcode != OpcodeValues::BCLRI && opcode != OpcodeValues::BINVI && opcode != OpcodeValues::BEXTI)
		{
			return 0;
		}

		std::wstring opcodeName = OpcodeRegistry::GetMnemonicOrHex(ctx.OpCode);
		if (ctx.Reg3ID != 0)
		{
			throw DecodeProjectionFaultException(std::format(L"Scalar bitfield-immediate opcode {0:s} requires canonical Word1=(rd, rs1, x0). ", opcodeName) + L"Register-form aliasing through rs2 is not accepted.");
		}

		unsigned short immediate = GetRequiredDecoderImmediate(ctx, std::format(L"Scalar bitfield-immediate opcode {0:s}", opcodeName));

		if (immediate > 0x3F)
		{
			throw DecodeProjectionFaultException(std::format(L"Scalar bitfield-immediate opcode {0:s} requires imm6 Immediate in [0, 63]. ", opcodeName) + L"Out-of-range immediate payloads are not accepted.");
		}

		return immediate;
	}

	void InstructionRegistry::RejectScalarMinMaxRegisterFormAlias(DecoderContext const ctx)
	{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// C# TO C++ CONVERTER TASK: The following 'is' operator pattern is not converted by C# to C++ Converter:
// ORIGINAL LINE: if (unchecked((ushort)ctx.OpCode) is not(YAKSys_Hybrid_CPU.Processor.CPU_Core.IsaOpcodeValues.MIN or OpcodeValues.MAX or OpcodeValues.MINU or OpcodeValues.MAXU))
		const auto opcode = static_cast<unsigned short>(ctx.OpCode);
		if (opcode != OpcodeValues::MIN && opcode != OpcodeValues::MAX && opcode != OpcodeValues::MINU && opcode != OpcodeValues::MAXU)
		{
			return;
		}

		if (ctx.HasImmediate && ctx.Immediate != 0)
		{
			throw DecodeProjectionFaultException(std::format(L"Scalar min/max opcode {0:s} requires canonical Immediate=0. ", OpcodeRegistry::GetMnemonicOrHex(ctx.OpCode)) + L"Immediate-form aliasing is not accepted.");
		}
	}

	void InstructionRegistry::RejectScalarZeroingSelectRegisterFormAlias(DecoderContext const ctx)
	{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: if (unchecked((ushort)ctx.OpCode) is not OpcodeValues.CZERO_NEZ)
		if (static_cast<unsigned short>(ctx.OpCode) != OpcodeValues::CZERO_NEZ)
		{
			return;
		}

		if (ctx.HasImmediate && ctx.Immediate != 0)
		{
			throw DecodeProjectionFaultException(std::format(L"Scalar zeroing-select opcode {0:s} requires canonical Immediate=0. ", OpcodeRegistry::GetMnemonicOrHex(ctx.OpCode)) + L"Immediate-form aliasing is not accepted.");
		}
	}

	unsigned short InstructionRegistry::GetRequiredDecoderImmediate(DecoderContext const ctx, const std::wstring &contourName)
	{
		if (!ctx.HasImmediate)
		{
			throw DecodeProjectionFaultException(std::format(L"{0:s} requires projected DecoderContext immediate handoff. ", contourName) + L"Raw VLIW_Instruction.Immediate fallback is retired from the decoder-to-runtime ABI.");
		}

		return ctx.Immediate;
	}

	unsigned char InstructionRegistry::GetRequiredDecoderDataType(DecoderContext const ctx, const std::wstring &contourName)
	{
		if (!ctx.HasDataType)
		{
			throw DecodeProjectionFaultException(std::format(L"{0:s} requires projected DecoderContext data-type handoff. ", contourName) + L"Raw VLIW_Instruction.DataType fallback is retired from the decoder-to-runtime ABI.");
		}

		return ctx.DataType;
	}

	unsigned long long InstructionRegistry::GetRequiredDecoderMemoryAddress(DecoderContext const ctx, const std::wstring &contourName)
	{
		if (ctx.HasMemoryAddress)
		{
			return ctx.MemoryAddress;
		}

		if (ctx.AuxData != 0)
		{
			return ctx.AuxData;
		}

		if (ctx.HasImmediate)
		{
			return SignExtendScalarImmediate(ctx.Immediate);
		}

		throw DecodeProjectionFaultException(std::format(L"{0:s} requires projected DecoderContext memory-address handoff. ", contourName) + L"Raw VLIW_Instruction.Immediate fallback is retired from the decoder-to-runtime ABI.");
	}

	bool InstructionRegistry::TryDecodeCanonicalArchRegister(unsigned short rawRegId, unsigned char &registerId)
	{
		if (rawRegId == ArchRegisterTripletEncoding::NoReg)
		{
			registerId = ArchRegisterTripletEncoding::NoArchReg;
			return true;
		}

		ArchRegId archRegId;
		if (ArchRegId::TryCreate(rawRegId, archRegId))
		{
			registerId = archRegId.getValue();
			return true;
		}

		registerId = ArchRegisterTripletEncoding::NoArchReg;
		return false;
	}

	bool InstructionRegistry::TryDecodeCanonicalOrUnpackedNoArchRegister(unsigned short rawRegId, unsigned char &registerId)
	{
		if (rawRegId == ArchRegisterTripletEncoding::NoArchReg)
		{
			registerId = ArchRegisterTripletEncoding::NoArchReg;
			return true;
		}

		return TryDecodeCanonicalArchRegister(rawRegId, registerId);
	}

	bool InstructionRegistry::TryResolveCanonicalBranchPackedRegisters(DecoderContext const ctx, unsigned char &rd, unsigned char &rs1, unsigned char &rs2)
	{
		rd = ArchRegisterTripletEncoding::NoArchReg;
		rs1 = ArchRegisterTripletEncoding::NoArchReg;
		rs2 = ArchRegisterTripletEncoding::NoArchReg;

		if (ctx.HasPackedRegisterTriplet && ArchRegisterTripletEncoding::TryUnpack(ctx.PackedRegisterTriplet, rd, rs1, rs2))
		{
			return true;
		}

		return TryDecodeCanonicalArchRegister(ctx.Reg1ID, rd) && TryDecodeCanonicalArchRegister(ctx.Reg2ID, rs1) && TryDecodeCanonicalArchRegister(ctx.Reg3ID, rs2);
	}

	void InstructionRegistry::ApplyCanonicalDirectBranchProjection(std::shared_ptr<BranchMicroOp> branchMicroOp, DecoderContext const ctx)
	{
		unsigned char rd;
		unsigned char rs1;
		unsigned char rs2;
		if (!TryResolveCanonicalBranchPackedRegisters(ctx, rd, rs1, rs2))
		{
			return;
		}

// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: switch (unchecked((ushort)ctx.OpCode))
		switch (static_cast<unsigned short>(ctx.OpCode))
		{
			case OpcodeValues::JAL:
				branchMicroOp->ApplyCanonicalRuntimeOperandProjection(rd, ArchRegisterTripletEncoding::NoArchReg, ArchRegisterTripletEncoding::NoArchReg);
				break;
			case OpcodeValues::JALR:
				branchMicroOp->ApplyCanonicalRuntimeOperandProjection(rd, rs1, ArchRegisterTripletEncoding::NoArchReg);
				break;
			case OpcodeValues::BEQ:
			case OpcodeValues::BNE:
			case OpcodeValues::BLT:
			case OpcodeValues::BGE:
			case OpcodeValues::BLTU:
			case OpcodeValues::BGEU:
				branchMicroOp->ApplyCanonicalRuntimeOperandProjection(ArchRegisterTripletEncoding::NoArchReg, rs1, rs2);
				break;
		}
	}

	unsigned short InstructionRegistry::ResolveOpcode(std::shared_ptr<InstructionIR> instruction)
	{
		ArgumentNullException::ThrowIfNull(instruction);
		ArgumentNullException::ThrowIfNull(instruction->CanonicalOpcode);
		return instruction->CanonicalOpcode->Value;
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal static bool TryCreatePublishedControlFlowMicroOp(in InstructionIR instruction, out BranchMicroOp? branchMicroOp)
	bool InstructionRegistry::TryCreatePublishedControlFlowMicroOp(std::shared_ptr<InstructionIR> instruction, std::shared_ptr<BranchMicroOp> &branchMicroOp)
	{
		branchMicroOp = nullptr;

		std::optional<OpcodeInfo> opcodeInfo = OpcodeRegistry::GetInfo(static_cast<unsigned int>(ResolveOpcode(instruction)));
		if (!opcodeInfo || opcodeInfo.value().InstructionClass != InstructionClass::ControlFlow)
		{
			return false;
		}

		unsigned int opCode = static_cast<unsigned int>(ResolveOpcode(instruction));
		if (!IsRegistered(opCode))
		{
			return false;
		}

		unsigned short opcode = ResolveOpcode(instruction);
		branchMicroOp = std::make_shared<BranchMicroOp>();
		branchMicroOp->setOpCode(opCode);
		branchMicroOp->setConditional(opcode == OpcodeValues::BEQ || opcode == OpcodeValues::BNE || opcode == OpcodeValues::BLT || opcode == OpcodeValues::BGE || opcode == OpcodeValues::BLTU || opcode == OpcodeValues::BGEU);
		if (instruction->HasAbsoluteAddressing)
		{
			branchMicroOp->ApplyCanonicalRuntimeTargetProjection(static_cast<unsigned long long>(instruction->Imm));
		}

		ApplyCanonicalPublishedBranchProjection(branchMicroOp, opcode, instruction->Rd, instruction->Rs1, instruction->Rs2);
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: branchMicroOp.ApplyCanonicalRuntimeRelativeTargetProjection(unchecked((short)instruction.Imm));
		branchMicroOp->ApplyCanonicalRuntimeRelativeTargetProjection(static_cast<short>(instruction->Imm));
		branchMicroOp->InitializeMetadata();
		return true;
	}

	bool InstructionRegistry::TryResolvePublishedSystemEventKind(std::shared_ptr<InstructionIR> instruction, SystemEventKind &systemEventKind)
	{
		systemEventKind = static_cast<YAKSys_Hybrid_CPU::Core::SystemEventKind>(0);

		std::optional<OpcodeInfo> opcodeInfo = OpcodeRegistry::GetInfo(static_cast<unsigned int>(ResolveOpcode(instruction)));
// C# TO C++ CONVERTER TASK: The following 'is' operator pattern is not converted by C# to C++ Converter:
		if (!opcodeInfo || (opcodeInfo.value().InstructionClass != InstructionClass::System && opcodeInfo.value().InstructionClass != InstructionClass::SmtVt))
		{
			return false;
		}

		return TryResolvePublishedSystemEventKindFromOpcode(ResolveOpcode(instruction), systemEventKind);
	}

	bool InstructionRegistry::TryResolvePublishedSystemReadRegisters(std::shared_ptr<InstructionIR> instruction, std::vector<int> &readRegisters)
	{
		readRegisters.clear();

		std::optional<OpcodeInfo> opcodeInfo = OpcodeRegistry::GetInfo(static_cast<unsigned int>(ResolveOpcode(instruction)));
		if (!opcodeInfo || opcodeInfo.value().InstructionClass != InstructionClass::System)
		{
			return false;
		}

		return TryResolvePublishedSystemReadRegistersFromOpcode(ResolveOpcode(instruction), instruction->Rs1, instruction->Rs2, readRegisters);
	}

	bool InstructionRegistry::TryResolvePublishedStreamControlRetireContour(std::shared_ptr<InstructionIR> instruction, bool &requiresSerializingBoundaryFollowThrough)
	{
		requiresSerializingBoundaryFollowThrough = false;

		std::optional<OpcodeInfo> opcodeInfo = OpcodeRegistry::GetInfo(static_cast<unsigned int>(ResolveOpcode(instruction)));
// C# TO C++ CONVERTER TASK: The following 'is' operator pattern is not converted by C# to C++ Converter:
		if (!opcodeInfo || (opcodeInfo.value().InstructionClass != InstructionClass::System && opcodeInfo.value().InstructionClass != InstructionClass::SmtVt))
		{
			return false;
		}

		return TryResolvePublishedStreamControlRetireContourFromOpcode(ResolveOpcode(instruction), requiresSerializingBoundaryFollowThrough);
	}

	bool InstructionRegistry::TryResolvePublishedAtomicAccessSize(std::shared_ptr<InstructionIR> instruction, unsigned char &accessSize)
	{
		accessSize = 0;

		std::optional<OpcodeInfo> opcodeInfo = OpcodeRegistry::GetInfo(static_cast<unsigned int>(ResolveOpcode(instruction)));
		if (!opcodeInfo || opcodeInfo.value().InstructionClass != InstructionClass::Atomic)
		{
			return false;
		}

		return TryResolvePublishedAtomicAccessSizeFromOpcode(ResolveOpcode(instruction), accessSize);
	}

	bool InstructionRegistry::TryResolvePublishedVmxOperationKind(std::shared_ptr<InstructionIR> instruction, VmxOperationKind &operationKind)
	{
		operationKind = VmxOperationKind::None;

		std::optional<OpcodeInfo> opcodeInfo = OpcodeRegistry::GetInfo(static_cast<unsigned int>(ResolveOpcode(instruction)));
		if (!opcodeInfo || opcodeInfo.value().InstructionClass != InstructionClass::Vmx)
		{
			return false;
		}

		return TryResolvePublishedVmxOperationKindFromOpcode(ResolveOpcode(instruction), operationKind);
	}

	bool InstructionRegistry::TryResolvePublishedSystemEventKindFromOpcode(unsigned short opcode, SystemEventKind &systemEventKind)
	{
		switch (opcode)
		{
			case OpcodeValues::FENCE:
				systemEventKind = SystemEventKind::Fence;
				return true;
			case OpcodeValues::FENCE_I:
				systemEventKind = SystemEventKind::FenceI;
				return true;
			case OpcodeValues::ECALL:
				systemEventKind = SystemEventKind::Ecall;
				return true;
			case OpcodeValues::EBREAK:
				systemEventKind = SystemEventKind::Ebreak;
				return true;
			case OpcodeValues::MRET:
				systemEventKind = SystemEventKind::Mret;
				return true;
			case OpcodeValues::SRET:
				systemEventKind = SystemEventKind::Sret;
				return true;
			case OpcodeValues::WFI:
				systemEventKind = SystemEventKind::Wfi;
				return true;
			case OpcodeValues::WFE:
				systemEventKind = SystemEventKind::Wfe;
				return true;
			case OpcodeValues::SEV:
				systemEventKind = SystemEventKind::Sev;
				return true;
			case OpcodeValues::YIELD:
				systemEventKind = SystemEventKind::Yield;
				return true;
			case OpcodeValues::POD_BARRIER:
				systemEventKind = SystemEventKind::PodBarrier;
				return true;
			case OpcodeValues::VT_BARRIER:
				systemEventKind = SystemEventKind::VtBarrier;
				return true;
			default:
				systemEventKind = static_cast<YAKSys_Hybrid_CPU::Core::SystemEventKind>(0);
				return false;
		}
	}

	bool InstructionRegistry::TryResolvePublishedSystemReadRegistersFromOpcode(unsigned short opcode, unsigned char rs1, unsigned char rs2, std::vector<int> &readRegisters)
	{
		switch (opcode)
		{
			case OpcodeValues::ECALL:
				readRegisters = {17};
				return true;
			case OpcodeValues::FENCE:
			case OpcodeValues::FENCE_I:
			case OpcodeValues::EBREAK:
			case OpcodeValues::MRET:
			case OpcodeValues::SRET:
			case OpcodeValues::WFI:
				readRegisters.clear();
				return true;
			case OpcodeValues::VSETVL:
				readRegisters = BuildArchitecturalReadRegisterList(rs1, rs2);
				return true;
			case OpcodeValues::VSETVLI:
				readRegisters = BuildArchitecturalReadRegisterList(rs1);
				return true;
			case OpcodeValues::VSETIVLI:
			case OpcodeValues::ACCEL_QUERY_CAPS:
			case OpcodeValues::ACCEL_SUBMIT:
				readRegisters.clear();
				return true;
			case OpcodeValues::ACCEL_POLL:
			case OpcodeValues::ACCEL_STATUS:
			case OpcodeValues::ACCEL_WAIT:
			case OpcodeValues::ACCEL_CANCEL:
			case OpcodeValues::ACCEL_FENCE:
				readRegisters = BuildArchitecturalReadRegisterList(rs1);
				return true;
			default:
				readRegisters.clear();
				return false;
		}
	}

	bool InstructionRegistry::TryResolvePublishedStreamControlRetireContourFromOpcode(unsigned short opcode, bool &requiresSerializingBoundaryFollowThrough)
	{
		requiresSerializingBoundaryFollowThrough = opcode == OpcodeValues::STREAM_WAIT;

		return (opcode == OpcodeValues::STREAM_SETUP || opcode == OpcodeValues::STREAM_START || opcode == OpcodeValues::STREAM_WAIT);
	}

	bool InstructionRegistry::TryResolvePublishedAtomicAccessSizeFromOpcode(unsigned short opcode, unsigned char &accessSize)
	{
		int temp_var20;
		switch (opcode)
		{
			case OpcodeValues::LR_W:
			case OpcodeValues::SC_W:
			case OpcodeValues::AMOADD_W:
			case OpcodeValues::AMOSWAP_W:
			case OpcodeValues::AMOOR_W:
			case OpcodeValues::AMOAND_W:
			case OpcodeValues::AMOXOR_W:
			case OpcodeValues::AMOMIN_W:
			case OpcodeValues::AMOMAX_W:
			case OpcodeValues::AMOMINU_W:
			case OpcodeValues::AMOMAXU_W:
				temp_var20 = 4;
				break;
			case OpcodeValues::LR_D:
			case OpcodeValues::SC_D:
			case OpcodeValues::AMOADD_D:
			case OpcodeValues::AMOSWAP_D:
			case OpcodeValues::AMOOR_D:
			case OpcodeValues::AMOAND_D:
			case OpcodeValues::AMOXOR_D:
			case OpcodeValues::AMOMIN_D:
			case OpcodeValues::AMOMAX_D:
			case OpcodeValues::AMOMINU_D:
			case OpcodeValues::AMOMAXU_D:
				temp_var20 = 8;
				break;
			default:
				temp_var20 = static_cast<unsigned char>(0);
				break;
		}
		accessSize = temp_var20;

		return accessSize != 0;
	}

	bool InstructionRegistry::TryResolvePublishedVmxOperationKindFromOpcode(unsigned short opcode, VmxOperationKind &operationKind)
	{
		operationKind = VmxOperationKind::None;
		switch (opcode)
		{
			case OpcodeValues::VMXON: operationKind = VmxOperationKind::VmxOn; break;
			case OpcodeValues::VMXOFF: operationKind = VmxOperationKind::VmxOff; break;
			case OpcodeValues::VMLAUNCH: operationKind = VmxOperationKind::VmLaunch; break;
			case OpcodeValues::VMRESUME: operationKind = VmxOperationKind::VmResume; break;
			case OpcodeValues::VMREAD: operationKind = VmxOperationKind::VmRead; break;
			case OpcodeValues::VMWRITE: operationKind = VmxOperationKind::VmWrite; break;
			case OpcodeValues::VMCLEAR: operationKind = VmxOperationKind::VmClear; break;
			case OpcodeValues::VMPTRLD: operationKind = VmxOperationKind::VmPtrLd; break;
			case OpcodeValues::VMPTRST: operationKind = VmxOperationKind::VmPtrSt; break;
			case OpcodeValues::VMCALL: operationKind = VmxOperationKind::VmCall; break;
			case OpcodeValues::INVEPT: operationKind = VmxOperationKind::Invept; break;
			case OpcodeValues::INVVPID: operationKind = VmxOperationKind::Invvpid; break;
			case OpcodeValues::VMFUNC: operationKind = VmxOperationKind::VmFunc; break;
			case OpcodeValues::VMSAVEX: operationKind = VmxOperationKind::VmSaveX; break;
			case OpcodeValues::VMRESTX: operationKind = VmxOperationKind::VmRestX; break;
			default: break;
		}
		return operationKind != VmxOperationKind::None;
	}
	const std::vector<int> InstructionRegistry::BuildArchitecturalReadRegisterList(unsigned char first, unsigned char second)
	{
		bool hasFirst = HasArchitecturalRegister(first);
		bool hasSecond = HasArchitecturalRegister(second);

		if (hasFirst && hasSecond)
		{
			return std::vector<int> {static_cast<int>(first), static_cast<int>(second)};
		}

		if (hasFirst)
		{
			return std::vector<int> {static_cast<int>(first)};
		}

		if (hasSecond)
		{
			return std::vector<int> {static_cast<int>(second)};
		}

		return {};
	}

	bool InstructionRegistry::HasArchitecturalRegister(unsigned char registerId)
	{
		return registerId != 0 && registerId != ArchRegisterTripletEncoding::NoArchReg;
	}

	unsigned short InstructionRegistry::ToLegacyDecoderField(unsigned char registerId)
	{
		return registerId == ArchRegisterTripletEncoding::NoArchReg ? ArchRegisterTripletEncoding::NoReg : registerId;
	}

	void InstructionRegistry::ApplyCanonicalPublishedBranchProjection(std::shared_ptr<BranchMicroOp> branchMicroOp, unsigned short opcode, unsigned char rd, unsigned char rs1, unsigned char rs2)
	{
		switch (opcode)
		{
			case OpcodeValues::JAL:
				branchMicroOp->ApplyCanonicalRuntimeOperandProjection(rd, ArchRegisterTripletEncoding::NoArchReg, ArchRegisterTripletEncoding::NoArchReg);
				break;
			case OpcodeValues::JALR:
				branchMicroOp->ApplyCanonicalRuntimeOperandProjection(rd, rs1, ArchRegisterTripletEncoding::NoArchReg);
				break;
			case OpcodeValues::BEQ:
			case OpcodeValues::BNE:
			case OpcodeValues::BLT:
			case OpcodeValues::BGE:
			case OpcodeValues::BLTU:
			case OpcodeValues::BGEU:
				branchMicroOp->ApplyCanonicalRuntimeOperandProjection(ArchRegisterTripletEncoding::NoArchReg, rs1, rs2);
				break;
		}
	}

	OpcodeInfo InstructionRegistry::RequirePublishedOpcodeInfo(unsigned int opCode)
	{
		std::optional<OpcodeInfo> info = OpcodeRegistry::GetInfo(opCode);
		if (!info)
		{
			throw InvalidOperationException(std::format(L"Opcode {0:s} does not have a published canonical descriptor in OpcodeRegistry.", OpcodeRegistry::GetMnemonicOrHex(opCode)));
		}

		return info.value();
	}

	std::shared_ptr<MicroOpDescriptor> InstructionRegistry::CreatePublishedSystemLikeDescriptor(unsigned int opCode, std::optional<bool> &writesRegister, std::optional<bool> &isMemoryOp, int memFootprintClass)
	{
		OpcodeInfo info = RequirePublishedOpcodeInfo(opCode);
// C# TO C++ CONVERTER TASK: The following 'is' operator pattern is not converted by C# to C++ Converter:
		if (info.InstructionClass != InstructionClass::System && info.InstructionClass != InstructionClass::Csr && info.InstructionClass != InstructionClass::SmtVt && info.InstructionClass != InstructionClass::Vmx)
		{
			throw InvalidOperationException(std::format(L"Opcode {0:s} is not a published system/CSR/VMX contour and cannot use the canonical system-like descriptor path.", OpcodeRegistry::GetMnemonicOrHex(opCode)));
		}

		std::shared_ptr<MicroOpDescriptor> temp_var = std::make_shared<MicroOpDescriptor>(info.ExecutionLatency, memFootprintClass);
		temp_var->setWritesRegister(writesRegister);
		temp_var->setMemoryOp(isMemoryOp);
		return temp_var;
	}

	std::shared_ptr<MicroOpDescriptor> InstructionRegistry::CreatePublishedScalarDescriptor(unsigned int opCode)
	{
		OpcodeInfo info = RequirePublishedOpcodeInfo(opCode);
		if (info.InstructionClass != InstructionClass::ScalarAlu)
		{
			throw InvalidOperationException(std::format(L"Opcode {0:s} is not a published scalar contour and cannot use the canonical scalar descriptor path.", OpcodeRegistry::GetMnemonicOrHex(opCode)));
		}

		std::shared_ptr<MicroOpDescriptor> temp_var = std::make_shared<MicroOpDescriptor>();
		temp_var->setLatency(info.ExecutionLatency);
		temp_var->setMemFootprintClass(0);
		temp_var->setWritesRegister(true);
		temp_var->setMemoryOp(false);
		return temp_var;
	}

	void InstructionRegistry::RegisterPublishedDescriptorOnly(unsigned int opCode, int memFootprintClass, std::optional<bool> &writesRegister, std::optional<bool> &isMemoryOp)
	{
		OpcodeInfo info = RequirePublishedOpcodeInfo(opCode);
		std::shared_ptr<MicroOpDescriptor> temp_var = std::make_shared<MicroOpDescriptor>();
		temp_var->setLatency(info.ExecutionLatency);
		temp_var->setMemFootprintClass(memFootprintClass);
		temp_var->setWritesRegister(writesRegister);
		temp_var->setMemoryOp(isMemoryOp);
		RegisterOpAttributes(opCode, temp_var);
	}

	void InstructionRegistry::RegisterScalarRegisterOp(unsigned int opCode)
	{
		RegisterSemanticFactory(opCode, [=] (DecoderContext ctx)
		{
			std::shared_ptr<ScalarALUMicroOp> op = std::make_shared<ScalarALUMicroOp>();
			op->setOpCode(ctx.OpCode);
			op->setDestRegID(ctx.Reg1ID);
			op->setSrc1RegID(ctx.Reg2ID);
			op->setSrc2RegID(ctx.Reg3ID);
			op->setUsesImmediate(false);
    
			op->InitializeMetadata();
			return op;
		});
		RegisterOpAttributes(opCode, CreatePublishedScalarDescriptor(opCode));
	}

	void InstructionRegistry::RegisterScalarAddressGenerationRegisterOp(unsigned int opCode)
	{
		RegisterSemanticFactory(opCode, [=] (DecoderContext ctx)
		{
			RejectScalarAddressGenerationRegisterFormAlias(ctx);
    
			std::shared_ptr<ScalarALUMicroOp> op = std::make_shared<ScalarALUMicroOp>();
			op->setOpCode(ctx.OpCode);
			op->setDestRegID(ctx.Reg1ID);
			op->setSrc1RegID(ctx.Reg2ID);
			op->setSrc2RegID(ctx.Reg3ID);
			op->setUsesImmediate(false);
    
			op->InitializeMetadata();
			return op;
		});
		RegisterOpAttributes(opCode, CreatePublishedScalarDescriptor(opCode));
	}

	void InstructionRegistry::RegisterScalarAddressGenerationImmediateOp(unsigned int opCode)
	{
		RegisterSemanticFactory(opCode, [=] (DecoderContext ctx)
		{
			unsigned short immediate = GetScalarAddressGenerationImmediate(ctx);
    
			std::shared_ptr<ScalarALUMicroOp> op = std::make_shared<ScalarALUMicroOp>();
			op->setOpCode(ctx.OpCode);
			op->setDestRegID(ctx.Reg1ID);
			op->setSrc1RegID(ctx.Reg2ID);
			op->setSrc2RegID(VLIW_Instruction::NoReg);
			op->setImmediate(immediate);
			op->setUsesImmediate(true);
    
			op->InitializeMetadata();
			return op;
		});
		RegisterOpAttributes(opCode, CreatePublishedScalarDescriptor(opCode));
	}

	void InstructionRegistry::RegisterScalarCarryLessRegisterOp(unsigned int opCode)
	{
		RegisterSemanticFactory(opCode, [=] (DecoderContext ctx)
		{
			RejectScalarCarryLessRegisterFormAlias(ctx);
    
			std::shared_ptr<ScalarALUMicroOp> op = std::make_shared<ScalarALUMicroOp>();
			op->setOpCode(ctx.OpCode);
			op->setDestRegID(ctx.Reg1ID);
			op->setSrc1RegID(ctx.Reg2ID);
			op->setSrc2RegID(ctx.Reg3ID);
			op->setUsesImmediate(false);
    
			op->InitializeMetadata();
			return op;
		});
		RegisterOpAttributes(opCode, CreatePublishedScalarDescriptor(opCode));
	}

	void InstructionRegistry::RegisterScalarRotateRegisterOp(unsigned int opCode)
	{
		RegisterSemanticFactory(opCode, [=] (DecoderContext ctx)
		{
			RejectScalarRotateRegisterFormAlias(ctx);
    
			std::shared_ptr<ScalarALUMicroOp> op = std::make_shared<ScalarALUMicroOp>();
			op->setOpCode(ctx.OpCode);
			op->setDestRegID(ctx.Reg1ID);
			op->setSrc1RegID(ctx.Reg2ID);
			op->setSrc2RegID(ctx.Reg3ID);
			op->setUsesImmediate(false);
    
			op->InitializeMetadata();
			return op;
		});
		RegisterOpAttributes(opCode, CreatePublishedScalarDescriptor(opCode));
	}

	void InstructionRegistry::RegisterScalarRotateImmediateOp(unsigned int opCode)
	{
		RegisterSemanticFactory(opCode, [=] (DecoderContext ctx)
		{
			unsigned short immediate = GetScalarRotateImmediate(ctx);
    
			std::shared_ptr<ScalarALUMicroOp> op = std::make_shared<ScalarALUMicroOp>();
			op->setOpCode(ctx.OpCode);
			op->setDestRegID(ctx.Reg1ID);
			op->setSrc1RegID(ctx.Reg2ID);
			op->setSrc2RegID(VLIW_Instruction::NoReg);
			op->setImmediate(immediate);
			op->setUsesImmediate(true);
    
			op->InitializeMetadata();
			return op;
		});
		RegisterOpAttributes(opCode, CreatePublishedScalarDescriptor(opCode));
	}

	void InstructionRegistry::RegisterScalarBooleanInvertRegisterOp(unsigned int opCode)
	{
		RegisterSemanticFactory(opCode, [=] (DecoderContext ctx)
		{
			RejectScalarBooleanInvertRegisterFormAlias(ctx);
    
			std::shared_ptr<ScalarALUMicroOp> op = std::make_shared<ScalarALUMicroOp>();
			op->setOpCode(ctx.OpCode);
			op->setDestRegID(ctx.Reg1ID);
			op->setSrc1RegID(ctx.Reg2ID);
			op->setSrc2RegID(ctx.Reg3ID);
			op->setUsesImmediate(false);
    
			op->InitializeMetadata();
			return op;
		});
		RegisterOpAttributes(opCode, CreatePublishedScalarDescriptor(opCode));
	}

	void InstructionRegistry::RegisterScalarBitfieldRegisterOp(unsigned int opCode)
	{
		RegisterSemanticFactory(opCode, [=] (DecoderContext ctx)
		{
			RejectScalarBitfieldRegisterFormAlias(ctx);
    
			std::shared_ptr<ScalarALUMicroOp> op = std::make_shared<ScalarALUMicroOp>();
			op->setOpCode(ctx.OpCode);
			op->setDestRegID(ctx.Reg1ID);
			op->setSrc1RegID(ctx.Reg2ID);
			op->setSrc2RegID(ctx.Reg3ID);
			op->setUsesImmediate(false);
    
			op->InitializeMetadata();
			return op;
		});
		RegisterOpAttributes(opCode, CreatePublishedScalarDescriptor(opCode));
	}

	void InstructionRegistry::RegisterScalarBitfieldImmediateOp(unsigned int opCode)
	{
		RegisterSemanticFactory(opCode, [=] (DecoderContext ctx)
		{
			unsigned short immediate = GetScalarBitfieldImmediate(ctx);
    
			std::shared_ptr<ScalarALUMicroOp> op = std::make_shared<ScalarALUMicroOp>();
			op->setOpCode(ctx.OpCode);
			op->setDestRegID(ctx.Reg1ID);
			op->setSrc1RegID(ctx.Reg2ID);
			op->setSrc2RegID(VLIW_Instruction::NoReg);
			op->setImmediate(immediate);
			op->setUsesImmediate(true);
    
			op->InitializeMetadata();
			return op;
		});
		RegisterOpAttributes(opCode, CreatePublishedScalarDescriptor(opCode));
	}

	void InstructionRegistry::RegisterScalarMinMaxRegisterOp(unsigned int opCode)
	{
		RegisterSemanticFactory(opCode, [=] (DecoderContext ctx)
		{
			RejectScalarMinMaxRegisterFormAlias(ctx);
    
			std::shared_ptr<ScalarALUMicroOp> op = std::make_shared<ScalarALUMicroOp>();
			op->setOpCode(ctx.OpCode);
			op->setDestRegID(ctx.Reg1ID);
			op->setSrc1RegID(ctx.Reg2ID);
			op->setSrc2RegID(ctx.Reg3ID);
			op->setUsesImmediate(false);
    
			op->InitializeMetadata();
			return op;
		});
		RegisterOpAttributes(opCode, CreatePublishedScalarDescriptor(opCode));
	}

	void InstructionRegistry::RegisterScalarZeroingSelectRegisterOp(unsigned int opCode)
	{
		RegisterSemanticFactory(opCode, [=] (DecoderContext ctx)
		{
			RejectScalarZeroingSelectRegisterFormAlias(ctx);
    
			std::shared_ptr<ScalarALUMicroOp> op = std::make_shared<ScalarALUMicroOp>();
			op->setOpCode(ctx.OpCode);
			op->setDestRegID(ctx.Reg1ID);
			op->setSrc1RegID(ctx.Reg2ID);
			op->setSrc2RegID(ctx.Reg3ID);
			op->setUsesImmediate(false);
    
			op->InitializeMetadata();
			return op;
		});
		RegisterOpAttributes(opCode, CreatePublishedScalarDescriptor(opCode));
	}

	void InstructionRegistry::RegisterScalarUnaryRegisterOp(unsigned int opCode)
	{
		RegisterSemanticFactory(opCode, [=] (DecoderContext ctx)
		{
			RejectScalarUnaryRegisterFormAlias(ctx);
    
			std::shared_ptr<ScalarALUMicroOp> op = std::make_shared<ScalarALUMicroOp>();
			op->setOpCode(ctx.OpCode);
			op->setDestRegID(ctx.Reg1ID);
			op->setSrc1RegID(ctx.Reg2ID);
			op->setSrc2RegID(VLIW_Instruction::NoReg);
			op->setUsesImmediate(false);
    
			op->InitializeMetadata();
			return op;
		});
		RegisterOpAttributes(opCode, CreatePublishedScalarDescriptor(opCode));
	}

	void InstructionRegistry::RegisterScalarImmediateOp(unsigned int opCode)
	{
		RegisterSemanticFactory(opCode, [=] (DecoderContext ctx)
		{
			RejectScalarImmediateRegisterFormAlias(ctx);
    
			std::shared_ptr<ScalarALUMicroOp> op = std::make_shared<ScalarALUMicroOp>();
			op->setOpCode(ctx.OpCode);
			op->setDestRegID(ctx.Reg1ID);
			op->setSrc1RegID(ctx.Reg2ID);
			op->setImmediate(SignExtendScalarImmediate(GetRequiredDecoderImmediate(ctx, std::format(L"Scalar-immediate opcode {0:s}", OpcodeRegistry::GetMnemonicOrHex(ctx.OpCode)))));
			op->setUsesImmediate(true);
    
			op->InitializeMetadata();
			return op;
		});
		RegisterOpAttributes(opCode, CreatePublishedScalarDescriptor(opCode));
	}

	void InstructionRegistry::RegisterTypedLoadOp(unsigned int opCode)
	{
		RegisterSemanticFactory(opCode, [=] (DecoderContext ctx)
		{
			unsigned char accessSize = opCode == static_cast<unsigned int>(OpcodeValues::LB) ||
				opCode == static_cast<unsigned int>(OpcodeValues::LBU) ? 1 :
				opCode == static_cast<unsigned int>(OpcodeValues::LH) ||
				opCode == static_cast<unsigned int>(OpcodeValues::LHU) ? 2 :
				opCode == static_cast<unsigned int>(OpcodeValues::LW) ||
				opCode == static_cast<unsigned int>(OpcodeValues::LWU) ? 4 : 8;
			std::shared_ptr<LoadMicroOp> op = std::make_shared<LoadMicroOp>();
			op->setOpCode(ctx.OpCode);
			op->setDestRegID(ctx.Reg1ID);
			op->setBaseRegID(ctx.Reg2ID);
			op->setUsesBaseRegisterAddressing(ctx.Reg2ID != VLIW_Instruction::NoReg && !ctx.HasMemoryAddress);
			op->setAddressDisplacement(static_cast<short>(ctx.Immediate));
			op->setAddress(GetRequiredDecoderMemoryAddress(ctx, std::format(L"Typed load opcode {0:s}", OpcodeRegistry::GetMnemonicOrHex(ctx.OpCode))));
			op->setSize(accessSize);
    
			op->InitializeMetadata();
			return op;
		});
		OpcodeInfo info = RequirePublishedOpcodeInfo(opCode);
		if (info.InstructionClass != InstructionClass::Memory)
		{
			throw InvalidOperationException(std::format(L"Opcode {0:s} is not a published memory-load contour and cannot use the canonical typed-load descriptor path.", OpcodeRegistry::GetMnemonicOrHex(opCode)));
		}

		std::shared_ptr<MicroOpDescriptor> temp_var = std::make_shared<MicroOpDescriptor>();
		temp_var->setLatency(info.ExecutionLatency);
		temp_var->setMemFootprintClass(1);
		temp_var->setMemoryOp(true);
		temp_var->setWritesRegister(true);
		RegisterOpAttributes(opCode, temp_var);
	}

	void InstructionRegistry::RegisterTypedStoreOp(unsigned int opCode)
	{
		RegisterSemanticFactory(opCode, [=] (DecoderContext ctx)
		{
			unsigned char accessSize = opCode == static_cast<unsigned int>(OpcodeValues::SB) ? 1 :
				opCode == static_cast<unsigned int>(OpcodeValues::SH) ? 2 :
				opCode == static_cast<unsigned int>(OpcodeValues::SW) ? 4 : 8;
			std::shared_ptr<StoreMicroOp> op = std::make_shared<StoreMicroOp>();
			op->setOpCode(ctx.OpCode);
			op->setSrcRegID(ctx.Reg3ID);
			op->setBaseRegID(ctx.Reg2ID);
			op->setUsesBaseRegisterAddressing(ctx.Reg2ID != VLIW_Instruction::NoReg && !ctx.HasMemoryAddress);
			op->setAddressDisplacement(static_cast<short>(ctx.Immediate));
			op->setAddress(GetRequiredDecoderMemoryAddress(ctx, std::format(L"Typed store opcode {0:s}", OpcodeRegistry::GetMnemonicOrHex(ctx.OpCode))));
			op->setSize(accessSize);
    
			op->InitializeMetadata();
			return op;
		});
		OpcodeInfo info = RequirePublishedOpcodeInfo(opCode);
		if (info.InstructionClass != InstructionClass::Memory)
		{
			throw InvalidOperationException(std::format(L"Opcode {0:s} is not a published memory-store contour and cannot use the canonical typed-store descriptor path.", OpcodeRegistry::GetMnemonicOrHex(opCode)));
		}

		std::shared_ptr<MicroOpDescriptor> temp_var = std::make_shared<MicroOpDescriptor>();
		temp_var->setLatency(info.ExecutionLatency);
		temp_var->setMemFootprintClass(1);
		temp_var->setMemoryOp(true);
		temp_var->setWritesRegister(false);
		RegisterOpAttributes(opCode, temp_var);
	}

	void InstructionRegistry::RegisterBranchOp(unsigned int opCode, bool isConditional)
	{
		RegisterSemanticFactory(opCode, [=] (DecoderContext ctx)
		{
			std::shared_ptr<BranchMicroOp> op = std::make_shared<BranchMicroOp>();
			op->setOpCode(ctx.OpCode);
			op->setConditional(isConditional);
			op->setTargetAddress(ctx.AuxData);
			op->setReg1ID(ctx.Reg1ID);
			op->setReg2ID(ctx.Reg2ID);
    
			ApplyCanonicalDirectBranchProjection(op, ctx);
			op->ApplyCanonicalRuntimeRelativeTargetProjection(static_cast<short>(GetRequiredDecoderImmediate(ctx, std::format(L"Control-flow opcode {0:s}", OpcodeRegistry::GetMnemonicOrHex(ctx.OpCode)))));
    
			op->InitializeMetadata();
			return op;
		});
		OpcodeInfo info = RequirePublishedOpcodeInfo(opCode);
		if (info.InstructionClass != InstructionClass::ControlFlow)
		{
			throw InvalidOperationException(std::format(L"Opcode {0:s} is not a published control-flow contour and cannot use the canonical branch descriptor path.", OpcodeRegistry::GetMnemonicOrHex(opCode)));
		}

		std::shared_ptr<MicroOpDescriptor> temp_var = std::make_shared<MicroOpDescriptor>();
		temp_var->setLatency(info.ExecutionLatency);
		temp_var->setMemFootprintClass(0);
		RegisterOpAttributes(opCode, temp_var);
	}

	void InstructionRegistry::RegisterAtomicOp(unsigned int opCode)
	{
		RegisterSemanticFactory(opCode, [=] (DecoderContext ctx)
		{
			bool isWordAtomic = opCode == static_cast<unsigned int>(OpcodeValues::LR_W) ||
				opCode == static_cast<unsigned int>(OpcodeValues::SC_W) ||
				opCode == static_cast<unsigned int>(OpcodeValues::AMOSWAP_W) ||
				opCode == static_cast<unsigned int>(OpcodeValues::AMOADD_W) ||
				opCode == static_cast<unsigned int>(OpcodeValues::AMOXOR_W) ||
				opCode == static_cast<unsigned int>(OpcodeValues::AMOAND_W) ||
				opCode == static_cast<unsigned int>(OpcodeValues::AMOOR_W) ||
				opCode == static_cast<unsigned int>(OpcodeValues::AMOMIN_W) ||
				opCode == static_cast<unsigned int>(OpcodeValues::AMOMAX_W) ||
				opCode == static_cast<unsigned int>(OpcodeValues::AMOMINU_W) ||
				opCode == static_cast<unsigned int>(OpcodeValues::AMOMAXU_W);
			std::shared_ptr<AtomicMicroOp> op = std::make_shared<AtomicMicroOp>();
			op->setOpCode(ctx.OpCode);
			op->setDestRegID(ctx.Reg1ID);
			op->setBaseRegID(ctx.Reg2ID);
			op->setSrcRegID(ctx.Reg3ID);
			op->setAddress(ctx.AuxData);
			op->setAcquireOrdering(ctx.AcquireOrdering);
			op->setReleaseOrdering(ctx.ReleaseOrdering);
			op->setSize(isWordAtomic ? 4 : 8);
    
			op->InitializeMetadata();
			return op;
		});
		OpcodeInfo info = RequirePublishedOpcodeInfo(opCode);
		if (info.InstructionClass != InstructionClass::Atomic)
		{
			throw InvalidOperationException(std::format(L"Opcode {0:s} is not a published atomic contour and cannot use the canonical atomic descriptor path.", OpcodeRegistry::GetMnemonicOrHex(opCode)));
		}

		std::shared_ptr<MicroOpDescriptor> temp_var = std::make_shared<MicroOpDescriptor>();
		temp_var->setLatency(info.ExecutionLatency);
		temp_var->setMemFootprintClass(1);
		temp_var->setMemoryOp(true);
		temp_var->setWritesRegister(true);
		RegisterOpAttributes(opCode, temp_var);
	}

	void InstructionRegistry::RegisterVmxOp(unsigned int opCode)
	{
		RegisterSemanticFactory(opCode, [=] (DecoderContext ctx)
		{
			unsigned short opcode = static_cast<unsigned short>(ctx.OpCode);
			unsigned char rd;
			unsigned char rs1;
			unsigned char rs2;
			if (!TryDecodeCanonicalOrUnpackedNoArchRegister(ctx.Reg1ID, rd) || !TryDecodeCanonicalOrUnpackedNoArchRegister(ctx.Reg2ID, rs1) || !TryDecodeCanonicalOrUnpackedNoArchRegister(ctx.Reg3ID, rs2))
			{
				throw DecodeProjectionFaultException(std::format(L"Opcode {0:s} reached InstructionRegistry.CreateMicroOp(...) with a non-canonical VMX register encoding. ", OpcodeRegistry::GetMnemonicOrHex(opcode)) + L"VMX manual publication requires flat architectural register ids or an explicit NoReg/NoArchReg sentinel.");
			}
    
			std::shared_ptr<VmxMicroOp> op = std::make_shared<VmxMicroOp>();
			op->setOpCode(ctx.OpCode);
			op->setRd(rd);
			op->setDestRegID(rd == ArchRegisterTripletEncoding::NoArchReg ? ArchRegisterTripletEncoding::NoReg : rd);
			op->setRs1(rs1);
			op->setRs2(rs2);
			op->setWritesRegister(IsVmxRegisterWritebackOpcode(opcode) && rd != 0 && rd != ArchRegisterTripletEncoding::NoArchReg);
    
			op->RefreshWriteMetadata();
			return op;
		});
		RegisterOpAttributes(opCode, CreatePublishedSystemLikeDescriptor(opCode));
	}

	bool InstructionRegistry::IsVmxRegisterWritebackOpcode(unsigned short opcode)
	{
		return (opcode == OpcodeValues::VMREAD || opcode == OpcodeValues::VMPTRST || opcode == OpcodeValues::VMFUNC);
	}

	unsigned short InstructionRegistry::NormalizeCanonicalOrSuppressedCsrDestinationRegister(unsigned int opCode, unsigned short rawRegId)
	{
		unsigned char registerId;
		if (!TryDecodeCanonicalOrUnpackedNoArchRegister(rawRegId, registerId))
		{
			throw DecodeProjectionFaultException(std::format(L"Opcode {0:s} reached InstructionRegistry.CreateMicroOp(...) with a non-canonical destination register encoding. ", OpcodeRegistry::GetMnemonicOrHex(opCode)) + L"CSR manual publication requires a flat architectural rd or an explicit NoReg/NoArchReg sentinel so optional writeback stays synchronized with the authoritative lane-7 carrier.");
		}

		return registerId == ArchRegisterTripletEncoding::NoArchReg ? ArchRegisterTripletEncoding::NoReg : registerId;
	}

	unsigned short InstructionRegistry::NormalizeCanonicalOrSuppressedCsrSourceRegister(unsigned int opCode, unsigned short rawRegId, const std::wstring &operandName)
	{
		unsigned char registerId;
		if (!TryDecodeCanonicalArchRegister(rawRegId, registerId))
		{
			throw DecodeProjectionFaultException(std::format(L"Opcode {0:s} reached InstructionRegistry.CreateMicroOp(...) with a non-canonical {1:s} register encoding. ", OpcodeRegistry::GetMnemonicOrHex(opCode), operandName) + L"CSR manual publication requires a flat architectural source register or an explicit NoReg/x0 suppression sentinel so runtime follow-through does not invent hidden register truth.");
		}

		return registerId == ArchRegisterTripletEncoding::NoArchReg ? ArchRegisterTripletEncoding::NoReg : registerId;
	}

	unsigned long long InstructionRegistry::NormalizeCanonicalCsrImmediateField(unsigned int opCode, unsigned short rawImmediateField, const std::wstring &operandName)
	{
		if (rawImmediateField > 0x1F)
		{
			throw DecodeProjectionFaultException(std::format(L"Opcode {0:s} reached InstructionRegistry.CreateMicroOp(...) with a non-canonical {1:s} immediate encoding. ", OpcodeRegistry::GetMnemonicOrHex(opCode), operandName) + L"CSR immediate manual publication must provide the canonical 5-bit zimm payload instead of relying on compatibility masking.");
		}

		return rawImmediateField;
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal static bool TryCreatePublishedCsrMicroOp(in InstructionIR instruction, out CSRMicroOp? csrMicroOp)
	bool InstructionRegistry::TryCreatePublishedCsrMicroOp(std::shared_ptr<InstructionIR> instruction, std::shared_ptr<CSRMicroOp> &csrMicroOp)
	{
		csrMicroOp = nullptr;

		unsigned short opcode = ResolveOpcode(instruction);
		if (InstructionClassifier::GetClass(opcode) != InstructionClass::Csr)
		{
			return false;
		}

		switch (opcode)
		{
			case Processor::CPU_Core::IsaOpcodeValues::RDCYCLE:
				csrMicroOp = CreatePublishedCounterReadCsrMicroOp(opcode, instruction, CsrAddresses::Cycle);
				return true;
			case Processor::CPU_Core::IsaOpcodeValues::CSRRW:
				csrMicroOp = CreatePublishedRegisterSourceCsrMicroOp<CsrReadWriteMicroOp>(opcode, instruction);
				return true;
			case Processor::CPU_Core::IsaOpcodeValues::CSRRS:
				csrMicroOp = CreatePublishedRegisterSourceCsrMicroOp<CsrReadSetMicroOp>(opcode, instruction);
				return true;
			case Processor::CPU_Core::IsaOpcodeValues::CSRRC:
				csrMicroOp = CreatePublishedRegisterSourceCsrMicroOp<CsrReadClearMicroOp>(opcode, instruction);
				return true;
			case Processor::CPU_Core::IsaOpcodeValues::CSRRWI:
				if (auto result = CreatePublishedImmediateSourceCsrMicroOp<CsrReadWriteImmediateMicroOp>(opcode, instruction))
				{
					csrMicroOp = result.value();
					return true;
				}
				return false;
			case Processor::CPU_Core::IsaOpcodeValues::CSRRSI:
				if (auto result = CreatePublishedImmediateSourceCsrMicroOp<CsrReadSetImmediateMicroOp>(opcode, instruction))
				{
					csrMicroOp = result.value();
					return true;
				}
				return false;
			case Processor::CPU_Core::IsaOpcodeValues::CSRRCI:
				if (auto result = CreatePublishedImmediateSourceCsrMicroOp<CsrReadClearImmediateMicroOp>(opcode, instruction))
				{
					csrMicroOp = result.value();
					return true;
				}
				return false;
			case Processor::CPU_Core::IsaOpcodeValues::CSR_CLEAR:
				csrMicroOp = std::make_shared<CsrClearMicroOp>();
				csrMicroOp->setOpCode(opcode);
				csrMicroOp->InitializeMetadata();
				return true;
			case Processor::CPU_Core::IsaOpcodeValues::VSETVEXCPMASK:
				csrMicroOp = CreatePublishedVectorExceptionControlCsrMicroOp(opcode, instruction, CsrAddresses::VexcpMask);
				return true;
			case Processor::CPU_Core::IsaOpcodeValues::VSETVEXCPPRI:
				csrMicroOp = CreatePublishedVectorExceptionControlCsrMicroOp(opcode, instruction, CsrAddresses::VexcpPri);
				return true;
			default:
				return false;
		}
	}

	std::shared_ptr<CsrReadCounterMicroOp> InstructionRegistry::CreatePublishedCounterReadCsrMicroOp(unsigned short opcode, std::shared_ptr<InstructionIR> instruction, unsigned short csrAddress)
	{
		if (instruction->Rs1 != 0 || instruction->Rs2 != 0 || instruction->Imm != 0)
		{
			throw DecodeProjectionFaultException(std::format(L"Opcode {0:s} requires canonical rd-only counter-read payload: rs1=x0, rs2=x0, Immediate=0.", OpcodeRegistry::GetMnemonicOrHex(opcode)));
		}

		unsigned short destinationRegister = NormalizeCanonicalOrSuppressedCsrDestinationRegister(opcode, ToLegacyDecoderField(instruction->Rd));

		std::shared_ptr<CsrReadCounterMicroOp> csrMicroOp = std::make_shared<CsrReadCounterMicroOp>();
		csrMicroOp->setOpCode(opcode);
		csrMicroOp->setCSRAddress(csrAddress);
		csrMicroOp->setDestRegID(destinationRegister);
		csrMicroOp->setWritesRegister(destinationRegister != 0 && destinationRegister != ArchRegisterTripletEncoding::NoReg);

		csrMicroOp->InitializeMetadata();
		return csrMicroOp;
	}

	std::shared_ptr<CsrReadWriteMicroOp> InstructionRegistry::CreatePublishedVectorExceptionControlCsrMicroOp(unsigned short opcode, std::shared_ptr<InstructionIR> instruction, unsigned short csrAddress)
	{
		std::shared_ptr<CsrReadWriteMicroOp> csrMicroOp = std::make_shared<CsrReadWriteMicroOp>();
		csrMicroOp->setOpCode(opcode);
		csrMicroOp->setCSRAddress(csrAddress);
		csrMicroOp->setDestRegID(ArchRegisterTripletEncoding::NoReg);
		csrMicroOp->setSrcRegID(NormalizeCanonicalOrSuppressedCsrSourceRegister(opcode, ToLegacyDecoderField(instruction->Rs1), L"rs1"));

		csrMicroOp->InitializeMetadata();
		return csrMicroOp;
	}

	void InstructionRegistry::RegisterCounterReadCsrOp(unsigned int opCode, unsigned short csrAddress)
	{
		RegisterSemanticFactory(opCode, [=] (DecoderContext ctx)
		{
			ValidateCounterReadCsrDecoderContext(ctx, csrAddress);
    
			unsigned short destinationRegister = NormalizeCanonicalOrSuppressedCsrDestinationRegister(ctx.OpCode, ctx.Reg1ID);
    
			std::shared_ptr<CsrReadCounterMicroOp> op = std::make_shared<CsrReadCounterMicroOp>();
			op->setOpCode(ctx.OpCode);
			op->setCSRAddress(csrAddress);
			op->setDestRegID(destinationRegister);
			op->setWritesRegister(destinationRegister != 0 && destinationRegister != ArchRegisterTripletEncoding::NoReg);
    
			op->InitializeMetadata();
			return op;
		});

		RegisterOpAttributes(opCode, CreatePublishedSystemLikeDescriptor(opCode, true, false, 0));
	}

	void InstructionRegistry::ValidateCounterReadCsrDecoderContext(DecoderContext const ctx, unsigned short csrAddress)
	{
		if (ctx.HasImmediate && ctx.Immediate != 0)
		{
			throw DecodeProjectionFaultException(std::format(L"Counter-read opcode {0:s} is fixed to CSR 0x{1:03X} and requires canonical Immediate=0.", OpcodeRegistry::GetMnemonicOrHex(ctx.OpCode), csrAddress));
		}

		unsigned char rs1;
		unsigned char rs2;
		if (!TryDecodeCanonicalOrUnpackedNoArchRegister(ctx.Reg2ID, rs1) || !TryDecodeCanonicalOrUnpackedNoArchRegister(ctx.Reg3ID, rs2))
		{
			throw DecodeProjectionFaultException(std::format(L"Counter-read opcode {0:s} reached InstructionRegistry.CreateMicroOp(...) with non-canonical source register encoding. ", OpcodeRegistry::GetMnemonicOrHex(ctx.OpCode)) + L"Manual publication must use rd-only canonical payload: rs1=x0, rs2=x0.");
		}

		if (rs1 != 0 && rs1 != ArchRegisterTripletEncoding::NoArchReg || rs2 != 0 && rs2 != ArchRegisterTripletEncoding::NoArchReg)
		{
			throw DecodeProjectionFaultException(std::format(L"Counter-read opcode {0:s} requires canonical rd-only payload: rs1=x0, rs2=x0.", OpcodeRegistry::GetMnemonicOrHex(ctx.OpCode)));
		}
	}

	void InstructionRegistry::RegisterCsrClearOp(unsigned int opCode)
	{
		RegisterSemanticFactory(opCode, [=] (DecoderContext ctx)
		{
			std::shared_ptr<CsrClearMicroOp> op = std::make_shared<CsrClearMicroOp>();
			op->setOpCode(ctx.OpCode);
			op->InitializeMetadata();
			return op;
		});

		RegisterOpAttributes(opCode, CreatePublishedSystemLikeDescriptor(opCode, false, false, 0));
	}

	void InstructionRegistry::RegisterVectorExceptionControlCsrOp(unsigned int opCode, unsigned short csrAddress)
	{
		RegisterSemanticFactory(opCode, [=] (DecoderContext ctx)
		{
			unsigned char sourceRegister;
			if (!TryDecodeCanonicalArchRegister(ctx.Reg2ID, sourceRegister))
			{
				throw DecodeProjectionFaultException(std::format(L"Opcode {0:s} reached InstructionRegistry.CreateMicroOp(...) with a non-canonical source register encoding. ", OpcodeRegistry::GetMnemonicOrHex(ctx.OpCode)) + L"Vector exception control publication requires a flat architectural rs1 so the mainline CSR carrier preserves authoritative metadata and retire follow-through.");
			}
    
			unsigned short materializedSourceRegister = sourceRegister == ArchRegisterTripletEncoding::NoArchReg ? ArchRegisterTripletEncoding::NoReg : sourceRegister;
    
			std::shared_ptr<CsrReadWriteMicroOp> op = std::make_shared<CsrReadWriteMicroOp>();
			op->setOpCode(ctx.OpCode);
			op->setCSRAddress(csrAddress);
			op->setDestRegID(ArchRegisterTripletEncoding::NoReg);
			op->setSrcRegID(materializedSourceRegister);
    
			op->InitializeMetadata();
    
			unsigned short opcode = static_cast<unsigned short>(ctx.OpCode);
			InstructionClass instructionClass = InstructionClassifier::GetClass(opcode);
			op->ApplyCanonicalDecodeProjection(instructionClass, InstructionClassifier::GetSerializationClass(opcode), BundleLegalityAnalyzer::BuildCanonicalPlacement(instructionClass, op->getPlacement().DomainTag), false, false, false, sourceRegister != 0 && sourceRegister != ArchRegisterTripletEncoding::NoArchReg ? std::vector<int>{static_cast<int>(sourceRegister)} : std::vector<int>{}, std::vector<int>{});
			op->RefreshAdmissionMetadata();
			return op;
		});

		std::shared_ptr<MicroOpDescriptor> temp_var = std::make_shared<MicroOpDescriptor>(1, 0);
		temp_var->setWritesRegister(false);
		temp_var->setMemoryOp(false);
		RegisterOpAttributes(opCode, temp_var);
	}

	void InstructionRegistry::RegisterMatrixTileInstructions()
	{
		RegisterMatrixTileMicroOp(static_cast<unsigned int>(InstructionsEnum::MTILE_LOAD));
		RegisterMatrixTileMicroOp(static_cast<unsigned int>(InstructionsEnum::MTILE_STORE));
		RegisterMatrixTileMicroOp(static_cast<unsigned int>(InstructionsEnum::MTILE_MACC));
		RegisterMatrixTileMicroOp(static_cast<unsigned int>(InstructionsEnum::MTRANSPOSE));
	}

	bool InstructionRegistry::IsMatrixTileMaterializerRegistered(unsigned int opCode)
	{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'typeof' operator:
		if (opCode > std::numeric_limits<unsigned short>::max() || !OpcodeRegistry::GetInfo(opCode).has_value())
		{
			return false;
		}

		return MatrixTileRuntimeOwnedVlmRows::IsMatrixTileOpcode(static_cast<InstructionsEnum>(opCode));
	}

	bool InstructionRegistry::TryCreateMatrixTileRuntimeObject(std::shared_ptr<InstructionIR> instruction, std::shared_ptr<MatrixTileMaterializedInstruction> &materializedInstruction, MatrixTileIrProjectionFaultKind &faultKind)
	{
		InstructionsEnum opcode = static_cast<InstructionsEnum>(ResolveOpcode(instruction));
		if (!IsMatrixTileMaterializerRegistered(static_cast<unsigned int>(opcode)))
		{
			materializedInstruction = nullptr;
			faultKind = MatrixTileIrProjectionFaultKind::NonMatrixTileOpcode;
			return false;
		}

		return MatrixTileIrProjectionAndMaterializer::TryMaterialize(instruction, materializedInstruction, faultKind);
	}

	bool InstructionRegistry::IsMatrixTileMicroOpRegistered(unsigned int opCode)
	{
		return IsRegistered(opCode) && IsMatrixTileMaterializerRegistered(opCode);
	}

	void InstructionRegistry::RegisterMatrixTileMicroOp(unsigned int opCode)
	{
		RegisterSemanticFactory(opCode, [=] (DecoderContext ctx)
		{
			std::shared_ptr<MatrixTileMaterializedInstruction> materializedInstruction = CreateRequiredMatrixTileRuntimeObject(ctx);
			std::shared_ptr<MatrixTileMicroOp> microOp = MatrixTileIrProjectionAndMaterializer::CreateTypedMicroOp(materializedInstruction);
			microOp->InitializeMetadata();
			return microOp;
		});

		OpcodeInfo opcodeInfo = RequirePublishedOpcodeInfo(opCode);
		std::shared_ptr<MicroOpDescriptor> temp_var = std::make_shared<MicroOpDescriptor>();
		temp_var->setLatency(opcodeInfo.ExecutionLatency);
		bool isMemoryOp = opCode == static_cast<unsigned int>(InstructionsEnum::MTILE_LOAD) ||
			opCode == static_cast<unsigned int>(InstructionsEnum::MTILE_STORE);
		temp_var->setMemFootprintClass(isMemoryOp ? 3 : 0);
		temp_var->setWritesRegister(false);
		temp_var->setMemoryOp(isMemoryOp);
		RegisterOpAttributes(opCode, temp_var);
	}

	std::shared_ptr<MatrixTileMaterializedInstruction> InstructionRegistry::CreateRequiredMatrixTileRuntimeObject(DecoderContext const ctx)
	{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'typeof' operator:
		if (ctx.OpCode > std::numeric_limits<unsigned short>::max() || !OpcodeRegistry::GetInfo(ctx.OpCode).has_value())
		{
			throw DecodeProjectionFaultException(std::format(L"Opcode 0x{0:0>X} reached the MTILE Phase08 MicroOp factory without canonical opcode identity.", ctx.OpCode));
		}

		InstructionsEnum opcode = static_cast<InstructionsEnum>(ctx.OpCode);
		if (!MatrixTileRuntimeOwnedVlmRows::IsMatrixTileOpcode(opcode))
		{
			throw DecodeProjectionFaultException(std::format(L"Opcode {0:s} reached the MTILE Phase08 MicroOp factory without a runtime-owned VLM matrix row.", OpcodeRegistry::GetMnemonicOrHex(static_cast<unsigned int>(opcode))));
		}

		std::shared_ptr<VectorInstructionPayload> payload = GetRequiredProjectedMatrixTilePayload(ctx, opcode);
		std::shared_ptr<MatrixTileInstructionIrProjection> projection = MatrixTileIrProjectionAndMaterializer::ProjectDecodedVectorPayload(opcode, payload, ctx.HasImmediate ? ctx.Immediate: 0, true);

		std::shared_ptr<MatrixTileMaterializedInstruction> materializedInstruction;
		MatrixTileIrProjectionFaultKind faultKind;
		if (!MatrixTileIrProjectionAndMaterializer::TryMaterialize(projection, materializedInstruction, faultKind))
		{
			const wchar_t *faultKindName = L"None";
			switch (faultKind)
			{
				case MatrixTileIrProjectionFaultKind::NonMatrixTileOpcode: faultKindName = L"NonMatrixTileOpcode"; break;
				case MatrixTileIrProjectionFaultKind::MissingVectorPayload: faultKindName = L"MissingVectorPayload"; break;
				case MatrixTileIrProjectionFaultKind::UnsupportedDataType: faultKindName = L"UnsupportedDataType"; break;
				case MatrixTileIrProjectionFaultKind::InvalidShapeEncoding: faultKindName = L"InvalidShapeEncoding"; break;
				case MatrixTileIrProjectionFaultKind::MemoryShapeFault: faultKindName = L"MemoryShapeFault"; break;
				case MatrixTileIrProjectionFaultKind::MaccSemanticFault: faultKindName = L"MaccSemanticFault"; break;
				case MatrixTileIrProjectionFaultKind::TransposeSemanticFault: faultKindName = L"TransposeSemanticFault"; break;
				case MatrixTileIrProjectionFaultKind::RuntimeOwnedVlmFault: faultKindName = L"RuntimeOwnedVlmFault"; break;
				case MatrixTileIrProjectionFaultKind::NumericPolicyFault: faultKindName = L"NumericPolicyFault"; break;
				case MatrixTileIrProjectionFaultKind::LayoutPolicyFault: faultKindName = L"LayoutPolicyFault"; break;
				default: break;
			}
			throw DecodeProjectionFaultException(std::format(L"{0:s} Phase08 MicroOp factory rejected illegal MTILE IR projection: {1:s}.", OpcodeRegistry::GetMnemonicOrHex(static_cast<unsigned int>(opcode)), faultKindName));
		}

		return materializedInstruction;
	}

	std::shared_ptr<VectorInstructionPayload> InstructionRegistry::GetRequiredProjectedMatrixTilePayload(DecoderContext const ctx, InstructionsEnum opcode)
	{
		if (!ctx.HasVectorPayload)
		{
			throw DecodeProjectionFaultException(std::format(L"{0:s} requires projected matrix/tile vector-payload handoff before typed MicroOp materialization.", OpcodeRegistry::GetMnemonicOrHex(static_cast<unsigned int>(opcode))));
		}

// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'checked' in this context:
// ORIGINAL LINE: return new VectorInstructionPayload(ctx.VectorPrimaryPointer, ctx.VectorSecondaryPointer, ctx.VectorStreamLength, ctx.VectorStride, ctx.VectorRowStride, ctx.HasVectorAddressingContour && ctx.IndexedAddressing, ctx.HasVectorAddressingContour && ctx.Is2DAddressing, ctx.TailAgnostic, ctx.MaskAgnostic, ctx.Saturating, checked((byte)ctx.PredicateMask), GetRequiredDecoderDataType(in ctx, string.Format("Matrix/tile opcode {0}", opcode))) { MatrixTileNumericPolicy = ctx.MatrixTileNumericPolicy, MatrixTileLayoutPolicy = ctx.MatrixTileLayoutPolicy };
		std::shared_ptr<VectorInstructionPayload> temp_var = std::make_shared<VectorInstructionPayload>(ctx.VectorPrimaryPointer, ctx.VectorSecondaryPointer, ctx.VectorStreamLength, ctx.VectorStride, ctx.VectorRowStride, ctx.HasVectorAddressingContour && ctx.IndexedAddressing, ctx.HasVectorAddressingContour && ctx.Is2DAddressing, ctx.TailAgnostic, ctx.MaskAgnostic, ctx.Saturating, static_cast<unsigned char>(ctx.PredicateMask), GetRequiredDecoderDataType(ctx, std::format(L"Matrix/tile opcode {0:s}", OpcodeRegistry::GetMnemonicOrHex(static_cast<unsigned int>(opcode)))));
		temp_var->MatrixTileNumericPolicy = ctx.MatrixTileNumericPolicy ? ctx.MatrixTileNumericPolicy.value() : nullptr;
		temp_var->MatrixTileLayoutPolicy = ctx.MatrixTileLayoutPolicy ? ctx.MatrixTileLayoutPolicy.value() : nullptr;
		return temp_var;
	}

	unsigned short InstructionRegistry::NormalizeRequiredLegacyMemoryRegister(unsigned int opCode, unsigned short rawRegId, const std::wstring &operandName, const std::wstring &contourName)
	{
		unsigned char registerId;
		if (!TryDecodeCanonicalArchRegister(rawRegId, registerId) || registerId == std::numeric_limits<unsigned char>::max())
		{
			throw DecodeProjectionFaultException(std::format(L"Retained legacy {0:s} direct/manual publication reached InstructionRegistry.CreateMicroOp(...) with a non-canonical or missing {1:s} register encoding. ", contourName, operandName) + L"This contour must fail closed instead of publishing phantom register truth or synthetic zero follow-through.");
		}
		return registerId;
	}

	std::wstring InstructionRegistry::DescribeNonRepresentableVectorAddressingContour(bool indexed, bool is2D)
	{
		return indexed && is2D ? L"indexed+2D" : indexed ? L"indexed" : L"2D";
	}

	std::wstring InstructionRegistry::DescribeNonRepresentableVectorAddressingContour(VLIW_Instruction const instruction)
	{
		return DescribeNonRepresentableVectorAddressingContour(instruction.getIndexed(), instruction.getIs2D());
	}

	bool InstructionRegistry::TryResolveUnsupportedVectorAddressingContour(DecoderContext const ctx, std::wstring &addressingContour)
	{
		if (!ctx.HasVectorAddressingContour)
		{
			throw DecodeProjectionFaultException(std::format(L"Opcode {0:s} reached InstructionRegistry.CreateMicroOp(...) without projected DecoderContext vector-addressing contour handoff. ", OpcodeRegistry::GetMnemonicOrHex(ctx.OpCode)) + L"Raw VLIW_Instruction Indexed/Is2D fallback is retired from the decoder-to-runtime ABI, so this path must fail closed instead of reopening a non-representable compat surface.");
		}

		bool indexed = ctx.IndexedAddressing;
		bool is2D = ctx.Is2DAddressing;

		if (!indexed && !is2D)
		{
			addressingContour = L"";
			return false;
		}

		Processor::CPU_Core::InstructionsEnum opcode = static_cast<Processor::CPU_Core::InstructionsEnum>(ctx.OpCode);
		VectorContourLegalityStatus legalityStatus;
		if (!VectorLegalityMatrix::TryGetAddressingStatus(opcode, indexed, is2D, legalityStatus))
		{
			throw DecodeProjectionFaultException(std::format(L"Opcode {0:s} reached InstructionRegistry.CreateMicroOp(...) with no runtime-owned vector legality matrix row. ", OpcodeRegistry::GetMnemonicOrHex(ctx.OpCode)) + L"Vector indexed/2D addressing must fail closed until the runtime matrix explicitly publishes an executable contour.");
		}

		if (legalityStatus == VectorContourLegalityStatus::Executable)
		{
			addressingContour = L"";
			return false;
		}

		addressingContour = DescribeNonRepresentableVectorAddressingContour(indexed, is2D);
		return true;
	}

	VLIW_Instruction InstructionRegistry::GetRequiredProjectedVectorInstruction(DecoderContext const ctx)
	{
		Processor::CPU_Core::InstructionsEnum opcode = static_cast<Processor::CPU_Core::InstructionsEnum>(ctx.OpCode);
		if (!ctx.HasVectorPayload)
		{
			throw DecodeProjectionFaultException(std::format(L"Vector opcode {0:s} requires projected DecoderContext vector payload handoff. ", OpcodeRegistry::GetMnemonicOrHex(static_cast<unsigned int>(opcode))) + L"Raw VLIW_Instruction vector payload fallback is retired from the decoder-to-runtime ABI.");
		}

		VLIW_Instruction temp_var = VLIW_Instruction();
		temp_var.setOpCode(ctx.OpCode);
		temp_var.setDataType(GetRequiredDecoderDataType(ctx, std::format(L"Vector opcode {0:s}", OpcodeRegistry::GetMnemonicOrHex(static_cast<unsigned int>(opcode)))));
		temp_var.setPredicateMask(static_cast<unsigned char>(ctx.PredicateMask));
		temp_var.setImmediate(ctx.HasImmediate ? ctx.Immediate : static_cast<unsigned short>(0));
		temp_var.setDestSrc1Pointer(ctx.VectorPrimaryPointer);
		temp_var.setSrc2Pointer(ctx.VectorSecondaryPointer);
		temp_var.setRowStride(ctx.VectorRowStride);
		temp_var.setStreamLength(ctx.VectorStreamLength);
		temp_var.setStride(ctx.VectorStride);
		temp_var.setIndexed(ctx.HasVectorAddressingContour && ctx.IndexedAddressing);
		temp_var.setIs2D(ctx.HasVectorAddressingContour && ctx.Is2DAddressing);
		temp_var.setTailAgnostic(ctx.TailAgnostic);
		temp_var.setMaskAgnostic(ctx.MaskAgnostic);
		temp_var.setSaturating(ctx.Saturating);
		return temp_var;
	}

	void InstructionRegistry::RegisterPublishedVectorBinaryOp(unsigned int opCode)
	{
		RegisterSemanticFactory(opCode, [=] (DecoderContext ctx) -> std::shared_ptr<MicroOp>
		{
			std::wstring value;
			if (TryResolveUnsupportedVectorAddressingContour(ctx, value))
			{
				throw DecodeProjectionFaultException(std::format(L"Opcode {0:s} reached InstructionRegistry.CreateMicroOp(...) with unsupported {1:s} vector-binary addressing. ", OpcodeRegistry::GetMnemonicOrHex(ctx.OpCode), value) + L"The authoritative binary carrier only publishes 1D in-place memory truth and must fail closed instead of reopening a non-representable compat surface.");
			}
			if (ctx.Saturating)
			{
				if (!OpcodeRegistry::SupportsSaturatingAddPolicy(opCode))
				{
					throw DecodeProjectionFaultException(std::format(L"Opcode {0:s} reached InstructionRegistry.CreateMicroOp(...) with the saturating policy bit set, but Phase 05B opens only VADD.SAT.", OpcodeRegistry::GetMnemonicOrHex(ctx.OpCode)));
				}
    
				std::shared_ptr<VectorSaturatingAddMicroOp> saturatingAddMicroOp = std::make_shared<VectorSaturatingAddMicroOp>();
				saturatingAddMicroOp->setOpCode(ctx.OpCode);
				saturatingAddMicroOp->setInstruction(GetRequiredProjectedVectorInstruction(ctx));
				saturatingAddMicroOp->setPredicateMask(ctx.PredicateMask);
				saturatingAddMicroOp->InitializeMetadata();
				return saturatingAddMicroOp;
			}
    
			std::shared_ptr<VectorBinaryOpMicroOp> vectorBinaryOpMicroOp = std::make_shared<VectorBinaryOpMicroOp>();
			vectorBinaryOpMicroOp->setOpCode(ctx.OpCode);
			vectorBinaryOpMicroOp->setInstruction(GetRequiredProjectedVectorInstruction(ctx));
			vectorBinaryOpMicroOp->setPredicateMask(ctx.PredicateMask);
			vectorBinaryOpMicroOp->InitializeMetadata();
			return vectorBinaryOpMicroOp;
		});
		OpcodeInfo opcodeInfo = RequirePublishedOpcodeInfo(opCode);
		if (!opcodeInfo.isVector())
		{
			throw InvalidOperationException(std::format(L"Opcode {0:s} is not a published vector contour and cannot use the canonical vector-binary descriptor path.", OpcodeRegistry::GetMnemonicOrHex(opCode)));
		}
		std::shared_ptr<MicroOpDescriptor> temp_var = std::make_shared<MicroOpDescriptor>();
		temp_var->setLatency(opcodeInfo.ExecutionLatency);
		temp_var->setMemFootprintClass(2);
		temp_var->setMemoryOp(false);
		temp_var->setWritesRegister(false);
		RegisterOpAttributes(opCode, temp_var);
	}

	void InstructionRegistry::RegisterVectorTransferOp(unsigned int opCode, unsigned char latency)
	{
		RegisterSemanticFactory(opCode, [=] (DecoderContext ctx)
		{
			std::wstring value;
			if (TryResolveUnsupportedVectorAddressingContour(ctx, value))
			{
				throw DecodeProjectionFaultException(std::format(L"Opcode {0:s} reached InstructionRegistry.CreateMicroOp(...) with unsupported {1:s} vector-transfer addressing. ", OpcodeRegistry::GetMnemonicOrHex(ctx.OpCode), value) + L"The authoritative VLOAD/VSTORE carrier only publishes 1D transfer-shape truth and must fail closed instead of reopening a non-representable compat surface.");
			}
			std::shared_ptr<VectorTransferMicroOp> vectorTransferMicroOp = std::make_shared<VectorTransferMicroOp>();
			vectorTransferMicroOp->setOpCode(ctx.OpCode);
			vectorTransferMicroOp->setInstruction(GetRequiredProjectedVectorInstruction(ctx));
			vectorTransferMicroOp->setPredicateMask(ctx.PredicateMask);
			vectorTransferMicroOp->InitializeMetadata();
			return vectorTransferMicroOp;
		});
		std::shared_ptr<MicroOpDescriptor> temp_var = std::make_shared<MicroOpDescriptor>();
		temp_var->setLatency(latency);
		temp_var->setMemFootprintClass(3);
		temp_var->setMemoryOp(false);
		temp_var->setWritesRegister(false);
		RegisterOpAttributes(opCode, temp_var);
	}

	void InstructionRegistry::RegisterPublishedVectorGatherOp(unsigned int opCode)
	{
		if (opCode != static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VGATHER))
		{
			throw InvalidOperationException(L"The indexed vector-gather registration helper is intentionally scoped to VGATHER only.");
		}

		RegisterSemanticFactory(opCode, [=] (DecoderContext ctx)
		{
			if (!ctx.HasVectorAddressingContour || !ctx.HasVectorPayload)
			{
				throw DecodeProjectionFaultException(L"VGATHER reached InstructionRegistry.CreateMicroOp(...) without canonical vector payload/addressing projection.");
			}
    
			if (!ctx.IndexedAddressing || ctx.Is2DAddressing)
			{
				std::wstring addressingContour = !ctx.IndexedAddressing && !ctx.Is2DAddressing ? L"non-indexed" : DescribeNonRepresentableVectorAddressingContour(ctx.IndexedAddressing, ctx.Is2DAddressing);
				if (StringHelper::isEmptyOrWhiteSpace(addressingContour))
				{
					addressingContour = L"non-indexed";
				}
    
				throw DecodeProjectionFaultException(std::format(L"Opcode {0:s} reached InstructionRegistry.CreateMicroOp(...) with unsupported {1:s} VGATHER addressing. ", OpcodeRegistry::GetMnemonicOrHex(ctx.OpCode), addressingContour) + L"Phase 01 only publishes the 1D indexed-read contour; VSCATTER, 2D, and indexed+2D remain fail closed.");
			}
    
			VectorContourLegalityStatus legalityStatus = VectorLegalityMatrix::GetAddressingStatus(Processor::CPU_Core::InstructionsEnum::VGATHER, true, false);
			if (legalityStatus != VectorContourLegalityStatus::Executable)
			{
				throw DecodeProjectionFaultException(L"VGATHER indexed publication reached the registry before the runtime legality matrix opened the executable contour.");
			}
    
			std::shared_ptr<GatherMicroOp> gatherMicroOp = std::make_shared<GatherMicroOp>();
			gatherMicroOp->setOpCode(ctx.OpCode);
			gatherMicroOp->setInstruction(GetRequiredProjectedVectorInstruction(ctx));
			gatherMicroOp->setPredicateMask(ctx.PredicateMask);
			gatherMicroOp->InitializeMetadata();
			return gatherMicroOp;
		});

		OpcodeInfo opcodeInfo = RequirePublishedOpcodeInfo(opCode);
		std::shared_ptr<MicroOpDescriptor> temp_var = std::make_shared<MicroOpDescriptor>();
		temp_var->setLatency(opcodeInfo.ExecutionLatency);
		temp_var->setMemFootprintClass(3);
		temp_var->setMemoryOp(false);
		temp_var->setWritesRegister(false);
		RegisterOpAttributes(opCode, temp_var);
	}

	void InstructionRegistry::RegisterPublishedVectorScatterOp(unsigned int opCode)
	{
		if (opCode != static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VSCATTER))
		{
			throw InvalidOperationException(L"The indexed vector-scatter registration helper is intentionally scoped to VSCATTER only.");
		}

		RegisterSemanticFactory(opCode, [=] (DecoderContext ctx)
		{
			if (!ctx.HasVectorAddressingContour || !ctx.HasVectorPayload)
			{
				throw DecodeProjectionFaultException(L"VSCATTER reached InstructionRegistry.CreateMicroOp(...) without canonical vector payload/addressing projection.");
			}
    
			if (!ctx.IndexedAddressing || ctx.Is2DAddressing)
			{
				std::wstring addressingContour = !ctx.IndexedAddressing && !ctx.Is2DAddressing ? L"non-indexed" : DescribeNonRepresentableVectorAddressingContour(ctx.IndexedAddressing, ctx.Is2DAddressing);
				if (StringHelper::isEmptyOrWhiteSpace(addressingContour))
				{
					addressingContour = L"non-indexed";
				}
    
				throw DecodeProjectionFaultException(std::format(L"Opcode {0:s} reached InstructionRegistry.CreateMicroOp(...) with unsupported {1:s} VSCATTER addressing. ", OpcodeRegistry::GetMnemonicOrHex(ctx.OpCode), addressingContour) + L"Phase 02 only publishes the 1D indexed-write contour; 2D and indexed+2D remain fail closed.");
			}
    
			VectorContourLegalityStatus legalityStatus = VectorLegalityMatrix::GetAddressingStatus(Processor::CPU_Core::InstructionsEnum::VSCATTER, true, false);
			if (legalityStatus != VectorContourLegalityStatus::Executable)
			{
				throw DecodeProjectionFaultException(L"VSCATTER indexed publication reached the registry before the runtime legality matrix opened the executable contour.");
			}
    
			std::shared_ptr<StoreScatterMicroOp> scatterMicroOp = std::make_shared<StoreScatterMicroOp>();
			scatterMicroOp->setOpCode(ctx.OpCode);
			scatterMicroOp->setInstruction(GetRequiredProjectedVectorInstruction(ctx));
			scatterMicroOp->setPredicateMask(ctx.PredicateMask);
			scatterMicroOp->InitializeMetadata();
			return scatterMicroOp;
		});

		OpcodeInfo opcodeInfo = RequirePublishedOpcodeInfo(opCode);
		std::shared_ptr<MicroOpDescriptor> temp_var = std::make_shared<MicroOpDescriptor>();
		temp_var->setLatency(opcodeInfo.ExecutionLatency);
		temp_var->setMemFootprintClass(3);
		temp_var->setMemoryOp(false);
		temp_var->setWritesRegister(false);
		RegisterOpAttributes(opCode, temp_var);
	}

	void InstructionRegistry::RegisterPublishedVectorUnaryOp(unsigned int opCode)
	{
		RegisterSemanticFactory(opCode, [=] (DecoderContext ctx)
		{
			std::wstring value;
			if (TryResolveUnsupportedVectorAddressingContour(ctx, value))
			{
				throw DecodeProjectionFaultException(std::format(L"Opcode {0:s} reached InstructionRegistry.CreateMicroOp(...) with unsupported {1:s} vector-unary addressing. ", OpcodeRegistry::GetMnemonicOrHex(ctx.OpCode), value) + L"The authoritative unary carrier only publishes 1D in-place memory truth and must fail closed instead of reopening a non-representable compat surface.");
			}
			std::shared_ptr<VectorUnaryOpMicroOp> vectorUnaryOpMicroOp = std::make_shared<VectorUnaryOpMicroOp>();
			vectorUnaryOpMicroOp->setOpCode(ctx.OpCode);
			vectorUnaryOpMicroOp->setInstruction(GetRequiredProjectedVectorInstruction(ctx));
			vectorUnaryOpMicroOp->setPredicateMask(ctx.PredicateMask);
			vectorUnaryOpMicroOp->InitializeMetadata();
			return vectorUnaryOpMicroOp;
		});
		OpcodeInfo opcodeInfo = RequirePublishedOpcodeInfo(opCode);
		if (!opcodeInfo.isVector())
		{
			throw InvalidOperationException(std::format(L"Opcode {0:s} is not a published vector contour and cannot use the canonical vector-unary descriptor path.", OpcodeRegistry::GetMnemonicOrHex(opCode)));
		}
		std::shared_ptr<MicroOpDescriptor> temp_var = std::make_shared<MicroOpDescriptor>();
		temp_var->setLatency(opcodeInfo.ExecutionLatency);
		temp_var->setMemFootprintClass(2);
		temp_var->setMemoryOp(false);
		temp_var->setWritesRegister(false);
		RegisterOpAttributes(opCode, temp_var);
	}

	void InstructionRegistry::RegisterPublishedVectorFmaOp(unsigned int opCode)
	{
		RegisterSemanticFactory(opCode, [=] (DecoderContext ctx)
		{
			std::wstring value;
			if (TryResolveUnsupportedVectorAddressingContour(ctx, value))
			{
				throw DecodeProjectionFaultException(std::format(L"Opcode {0:s} reached InstructionRegistry.CreateMicroOp(...) with unsupported {1:s} vector-FMA addressing. ", OpcodeRegistry::GetMnemonicOrHex(ctx.OpCode), value) + L"The authoritative FMA carrier does not publish indexed/2D tri-operand follow-through truth and must fail closed instead of reopening a non-representable compat surface.");
			}
			std::shared_ptr<VectorFmaMicroOp> vectorFmaMicroOp = std::make_shared<VectorFmaMicroOp>();
			vectorFmaMicroOp->setOpCode(ctx.OpCode);
			vectorFmaMicroOp->setInstruction(GetRequiredProjectedVectorInstruction(ctx));
			vectorFmaMicroOp->setPredicateMask(ctx.PredicateMask);
			vectorFmaMicroOp->InitializeMetadata();
			return vectorFmaMicroOp;
		});
		OpcodeInfo opcodeInfo = RequirePublishedOpcodeInfo(opCode);
		if (!opcodeInfo.isVector())
		{
			throw InvalidOperationException(std::format(L"Opcode {0:s} is not a published vector contour and cannot use the canonical vector-FMA descriptor path.", OpcodeRegistry::GetMnemonicOrHex(opCode)));
		}
		std::shared_ptr<MicroOpDescriptor> temp_var = std::make_shared<MicroOpDescriptor>();
		temp_var->setLatency(opcodeInfo.ExecutionLatency);
		temp_var->setMemFootprintClass(2);
		temp_var->setMemoryOp(false);
		temp_var->setWritesRegister(false);
		RegisterOpAttributes(opCode, temp_var);
	}

	void InstructionRegistry::RegisterPublishedVectorReductionOp(unsigned int opCode)
	{
		RegisterSemanticFactory(opCode, [=] (DecoderContext ctx)
		{
			std::wstring value;
			if (TryResolveUnsupportedVectorAddressingContour(ctx, value))
			{
				throw DecodeProjectionFaultException(std::format(L"Opcode {0:s} reached InstructionRegistry.CreateMicroOp(...) with unsupported {1:s} vector-reduction addressing. ", OpcodeRegistry::GetMnemonicOrHex(ctx.OpCode), value) + L"The authoritative reduction carrier only publishes 1D scalar-footprint memory truth and must fail closed instead of reopening a non-representable compat surface.");
			}
			std::shared_ptr<VectorReductionMicroOp> vectorReductionMicroOp = std::make_shared<VectorReductionMicroOp>();
			vectorReductionMicroOp->setOpCode(ctx.OpCode);
			vectorReductionMicroOp->setInstruction(GetRequiredProjectedVectorInstruction(ctx));
			vectorReductionMicroOp->setPredicateMask(ctx.PredicateMask);
			vectorReductionMicroOp->InitializeMetadata();
			return vectorReductionMicroOp;
		});
		OpcodeInfo opcodeInfo = RequirePublishedOpcodeInfo(opCode);
		if (!opcodeInfo.isVector())
		{
			throw InvalidOperationException(std::format(L"Opcode {0:s} is not a published vector contour and cannot use the canonical vector-reduction descriptor path.", OpcodeRegistry::GetMnemonicOrHex(opCode)));
		}
		std::shared_ptr<MicroOpDescriptor> temp_var = std::make_shared<MicroOpDescriptor>();
		temp_var->setLatency(opcodeInfo.ExecutionLatency);
		temp_var->setMemFootprintClass(2);
		temp_var->setMemoryOp(false);
		temp_var->setWritesRegister(false);
		RegisterOpAttributes(opCode, temp_var);
	}

	void InstructionRegistry::RegisterPublishedVectorComparisonOp(unsigned int opCode)
	{
		RegisterSemanticFactory(opCode, [=] (DecoderContext ctx)
		{
			std::wstring value;
			if (TryResolveUnsupportedVectorAddressingContour(ctx, value))
			{
				throw DecodeProjectionFaultException(std::format(L"Opcode {0:s} reached InstructionRegistry.CreateMicroOp(...) with unsupported {1:s} vector-comparison addressing. ", OpcodeRegistry::GetMnemonicOrHex(ctx.OpCode), value) + L"The authoritative comparison carrier only publishes 1D predicate-state truth and must fail closed instead of reopening a non-representable compat surface.");
			}
			std::shared_ptr<VectorComparisonMicroOp> vectorComparisonMicroOp = std::make_shared<VectorComparisonMicroOp>();
			vectorComparisonMicroOp->setOpCode(ctx.OpCode);
			vectorComparisonMicroOp->setInstruction(GetRequiredProjectedVectorInstruction(ctx));
			vectorComparisonMicroOp->setPredicateMask(ctx.PredicateMask);
			vectorComparisonMicroOp->InitializeMetadata();
			return vectorComparisonMicroOp;
		});
		OpcodeInfo opcodeInfo = RequirePublishedOpcodeInfo(opCode);
		if (!opcodeInfo.isVector())
		{
			throw InvalidOperationException(std::format(L"Opcode {0:s} is not a published vector contour and cannot use the canonical vector-comparison descriptor path.", OpcodeRegistry::GetMnemonicOrHex(opCode)));
		}
		std::shared_ptr<MicroOpDescriptor> temp_var = std::make_shared<MicroOpDescriptor>();
		temp_var->setLatency(opcodeInfo.ExecutionLatency);
		temp_var->setMemFootprintClass(2);
		temp_var->setMemoryOp(false);
		temp_var->setWritesRegister(false);
		RegisterOpAttributes(opCode, temp_var);
	}

	void InstructionRegistry::RegisterPublishedVectorMaskOp(unsigned int opCode)
	{
		RegisterSemanticFactory(opCode, [=] (DecoderContext ctx)
		{
			std::wstring value;
			if (TryResolveUnsupportedVectorAddressingContour(ctx, value))
			{
				throw DecodeProjectionFaultException(std::format(L"Opcode {0:s} reached InstructionRegistry.CreateMicroOp(...) with unsupported {1:s} vector-mask addressing. ", OpcodeRegistry::GetMnemonicOrHex(ctx.OpCode), value) + L"The authoritative predicate-mask carrier does not materialize any addressed vector surface and must fail closed instead of reopening an addressing-tag compat surface.");
			}
			if (ctx.OpCode == static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VMSBF) && ((ctx.Immediate >> 4) & 0x0F) != 0)
			{
				throw DecodeProjectionFaultException(std::wstring(L"VMSBF reached InstructionRegistry.CreateMicroOp(...) with a non-zero reserved src2 predicate nibble. ") + L"Phase 04 publishes only the canonical unary mask-prefix ABI: src predicate in imm[3:0], dest predicate in imm[11:8].");
			}
			std::shared_ptr<VectorMaskOpMicroOp> vectorMaskOpMicroOp = std::make_shared<VectorMaskOpMicroOp>();
			vectorMaskOpMicroOp->setOpCode(ctx.OpCode);
			vectorMaskOpMicroOp->setInstruction(GetRequiredProjectedVectorInstruction(ctx));
			vectorMaskOpMicroOp->setPredicateMask(ctx.PredicateMask);
			vectorMaskOpMicroOp->InitializeMetadata();
			return vectorMaskOpMicroOp;
		});
		OpcodeInfo opcodeInfo = RequirePublishedOpcodeInfo(opCode);
		if (!opcodeInfo.isVector())
		{
			throw InvalidOperationException(std::format(L"Opcode {0:s} is not a published vector contour and cannot use the canonical vector-mask descriptor path.", OpcodeRegistry::GetMnemonicOrHex(opCode)));
		}
		std::shared_ptr<MicroOpDescriptor> temp_var = std::make_shared<MicroOpDescriptor>();
		temp_var->setLatency(opcodeInfo.ExecutionLatency);
		temp_var->setMemFootprintClass(0);
		temp_var->setMemoryOp(false);
		temp_var->setWritesRegister(false);
		RegisterOpAttributes(opCode, temp_var);
	}

	void InstructionRegistry::RegisterPublishedVectorMaskPopCountOp(unsigned int opCode)
	{
		RegisterSemanticFactory(opCode, [=] (DecoderContext ctx)
		{
			std::wstring value;
			if (TryResolveUnsupportedVectorAddressingContour(ctx, value))
			{
				throw DecodeProjectionFaultException(std::format(L"Opcode {0:s} reached InstructionRegistry.CreateMicroOp(...) with unsupported {1:s} vector-mask-popcount addressing. ", OpcodeRegistry::GetMnemonicOrHex(ctx.OpCode), value) + L"The authoritative scalar-result VPOPC carrier only publishes predicate-to-scalar writeback truth and must fail closed instead of reopening an addressing-tag compat surface.");
			}
			std::shared_ptr<VectorMaskPopCountMicroOp> vectorMaskPopCountMicroOp = std::make_shared<VectorMaskPopCountMicroOp>();
			vectorMaskPopCountMicroOp->setOpCode(ctx.OpCode);
			vectorMaskPopCountMicroOp->setInstruction(GetRequiredProjectedVectorInstruction(ctx));
			vectorMaskPopCountMicroOp->setPredicateMask(ctx.PredicateMask);
			vectorMaskPopCountMicroOp->InitializeMetadata();
			return vectorMaskPopCountMicroOp;
		});
		OpcodeInfo opcodeInfo = RequirePublishedOpcodeInfo(opCode);
		if (!opcodeInfo.isVector())
		{
			throw InvalidOperationException(std::format(L"Opcode {0:s} is not a published vector contour and cannot use the canonical vector-mask-popcount descriptor path.", OpcodeRegistry::GetMnemonicOrHex(opCode)));
		}
		std::shared_ptr<MicroOpDescriptor> temp_var = std::make_shared<MicroOpDescriptor>();
		temp_var->setLatency(opcodeInfo.ExecutionLatency);
		temp_var->setMemFootprintClass(0);
		temp_var->setMemoryOp(false);
		temp_var->setWritesRegister(true);
		RegisterOpAttributes(opCode, temp_var);
	}

	void InstructionRegistry::RegisterPublishedVectorZeroExtendOp(unsigned int opCode)
	{
		if (opCode != static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VZEXT))
		{
			throw InvalidOperationException(L"The Phase 05 vector zero-extend registration helper is intentionally scoped to VZEXT only.");
		}

		RegisterSemanticFactory(opCode, [=] (DecoderContext ctx)
		{
			std::wstring value;
			if (TryResolveUnsupportedVectorAddressingContour(ctx, value))
			{
				throw DecodeProjectionFaultException(std::format(L"Opcode {0:s} reached InstructionRegistry.CreateMicroOp(...) with unsupported {1:s} VZEXT addressing. ", OpcodeRegistry::GetMnemonicOrHex(ctx.OpCode), value) + L"Phase 05 only publishes the packed 1D unsigned zero-extension memory-to-memory contour.");
			}
			if (ctx.HasImmediate && ctx.Immediate != 0)
			{
				throw DecodeProjectionFaultException(std::wstring(L"VZEXT reached InstructionRegistry.CreateMicroOp(...) with a non-zero reserved immediate sideband. ") + L"Phase 05 publishes only the canonical packed 1D ABI: dest base in primary pointer, source base in secondary pointer, unsigned source type in DataType.");
			}
    
			std::shared_ptr<VectorZeroExtendMicroOp> microOp = std::make_shared<VectorZeroExtendMicroOp>();
			microOp->setOpCode(ctx.OpCode);
			microOp->setInstruction(GetRequiredProjectedVectorInstruction(ctx));
			microOp->setPredicateMask(ctx.PredicateMask);
			microOp->InitializeMetadata();
			return microOp;
		});

		OpcodeInfo opcodeInfo = RequirePublishedOpcodeInfo(opCode);
		if (!OpcodeRegistry::IsWidenNarrowConvertOp(opCode))
		{
			throw InvalidOperationException(std::format(L"Opcode {0:s} is not a published widen/narrow/convert contour.", OpcodeRegistry::GetMnemonicOrHex(opCode)));
		}

		std::shared_ptr<MicroOpDescriptor> temp_var = std::make_shared<MicroOpDescriptor>();
		temp_var->setLatency(opcodeInfo.ExecutionLatency);
		temp_var->setMemFootprintClass(2);
		temp_var->setMemoryOp(false);
		temp_var->setWritesRegister(false);
		RegisterOpAttributes(opCode, temp_var);
	}

	void InstructionRegistry::RegisterPublishedVectorScanSumOp(unsigned int opCode)
	{
		if (opCode != static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VSCAN_SUM))
		{
			throw InvalidOperationException(L"The Phase 05A vector scan registration helper is intentionally scoped to VSCAN.SUM only.");
		}

		RegisterSemanticFactory(opCode, [=] (DecoderContext ctx)
		{
			std::wstring value;
			if (TryResolveUnsupportedVectorAddressingContour(ctx, value))
			{
				throw DecodeProjectionFaultException(std::format(L"Opcode {0:s} reached InstructionRegistry.CreateMicroOp(...) with unsupported {1:s} VSCAN.SUM addressing. ", OpcodeRegistry::GetMnemonicOrHex(ctx.OpCode), value) + L"Phase 05A only publishes the packed 1D integer inclusive-prefix sum memory-to-memory contour.");
			}
			if (ctx.HasImmediate && ctx.Immediate != 0)
			{
				throw DecodeProjectionFaultException(std::wstring(L"VSCAN.SUM reached InstructionRegistry.CreateMicroOp(...) with a non-zero reserved immediate sideband. ") + L"Phase 05A publishes only the canonical packed 1D ABI: destination base in primary pointer, source base in secondary pointer, integer element type in DataType.");
			}
    
			std::shared_ptr<VectorScanSumMicroOp> microOp = std::make_shared<VectorScanSumMicroOp>();
			microOp->setOpCode(ctx.OpCode);
			microOp->setInstruction(GetRequiredProjectedVectorInstruction(ctx));
			microOp->setPredicateMask(ctx.PredicateMask);
			microOp->InitializeMetadata();
			return microOp;
		});

		OpcodeInfo opcodeInfo = RequirePublishedOpcodeInfo(opCode);
		if (!OpcodeRegistry::IsScanOp(opCode))
		{
			throw InvalidOperationException(std::format(L"Opcode {0:s} is not a published vector scan contour.", OpcodeRegistry::GetMnemonicOrHex(opCode)));
		}

		std::shared_ptr<MicroOpDescriptor> temp_var = std::make_shared<MicroOpDescriptor>();
		temp_var->setLatency(opcodeInfo.ExecutionLatency);
		temp_var->setMemFootprintClass(2);
		temp_var->setMemoryOp(false);
		temp_var->setWritesRegister(false);
		RegisterOpAttributes(opCode, temp_var);
	}

	void InstructionRegistry::RegisterPublishedVectorPermutationOp(unsigned int opCode)
	{
		RegisterSemanticFactory(opCode, [=] (DecoderContext ctx)
		{
			std::wstring value;
			if (TryResolveUnsupportedVectorAddressingContour(ctx, value))
			{
				throw DecodeProjectionFaultException(std::format(L"Opcode {0:s} reached InstructionRegistry.CreateMicroOp(...) with unsupported {1:s} vector-permutation addressing. ", OpcodeRegistry::GetMnemonicOrHex(ctx.OpCode), value) + L"The authoritative permutation carrier only publishes 1D two-surface memory truth and must fail closed instead of reopening a non-representable compat surface.");
			}
			std::shared_ptr<VectorPermutationMicroOp> vectorPermutationMicroOp = std::make_shared<VectorPermutationMicroOp>();
			vectorPermutationMicroOp->setOpCode(ctx.OpCode);
			vectorPermutationMicroOp->setInstruction(GetRequiredProjectedVectorInstruction(ctx));
			vectorPermutationMicroOp->setPredicateMask(ctx.PredicateMask);
			vectorPermutationMicroOp->InitializeMetadata();
			return vectorPermutationMicroOp;
		});
		OpcodeInfo opcodeInfo = RequirePublishedOpcodeInfo(opCode);
		if (!opcodeInfo.isVector())
		{
			throw InvalidOperationException(std::format(L"Opcode {0:s} is not a published vector contour and cannot use the canonical vector-permutation descriptor path.", OpcodeRegistry::GetMnemonicOrHex(opCode)));
		}
		std::shared_ptr<MicroOpDescriptor> temp_var = std::make_shared<MicroOpDescriptor>();
		temp_var->setLatency(opcodeInfo.ExecutionLatency);
		temp_var->setMemFootprintClass(2);
		temp_var->setMemoryOp(false);
		temp_var->setWritesRegister(false);
		RegisterOpAttributes(opCode, temp_var);
	}

	void InstructionRegistry::RegisterPublishedVectorPredicativeMovementOp(unsigned int opCode)
	{
		RegisterSemanticFactory(opCode, [=] (DecoderContext ctx)
		{
			std::wstring value;
			if (TryResolveUnsupportedVectorAddressingContour(ctx, value))
			{
				throw DecodeProjectionFaultException(std::format(L"Opcode {0:s} reached InstructionRegistry.CreateMicroOp(...) with unsupported {1:s} vector-predicative-movement addressing. ", OpcodeRegistry::GetMnemonicOrHex(ctx.OpCode), value) + L"The authoritative predicative-movement carrier only publishes 1D single-surface truth and must fail closed instead of reopening a non-representable compat surface.");
			}
			std::shared_ptr<VectorPredicativeMovementMicroOp> vectorPredicativeMovementMicroOp = std::make_shared<VectorPredicativeMovementMicroOp>();
			vectorPredicativeMovementMicroOp->setOpCode(ctx.OpCode);
			vectorPredicativeMovementMicroOp->setInstruction(GetRequiredProjectedVectorInstruction(ctx));
			vectorPredicativeMovementMicroOp->setPredicateMask(ctx.PredicateMask);
			vectorPredicativeMovementMicroOp->InitializeMetadata();
			return vectorPredicativeMovementMicroOp;
		});
		OpcodeInfo opcodeInfo = RequirePublishedOpcodeInfo(opCode);
		if (!opcodeInfo.isVector())
		{
			throw InvalidOperationException(std::format(L"Opcode {0:s} is not a published vector contour and cannot use the canonical vector-predicative-movement descriptor path.", OpcodeRegistry::GetMnemonicOrHex(opCode)));
		}
		std::shared_ptr<MicroOpDescriptor> temp_var = std::make_shared<MicroOpDescriptor>();
		temp_var->setLatency(opcodeInfo.ExecutionLatency);
		temp_var->setMemFootprintClass(2);
		temp_var->setMemoryOp(false);
		temp_var->setWritesRegister(false);
		RegisterOpAttributes(opCode, temp_var);
	}

	void InstructionRegistry::RegisterPublishedVectorSlideOp(unsigned int opCode)
	{
		RegisterSemanticFactory(opCode, [=] (DecoderContext ctx)
		{
			std::wstring value;
			if (TryResolveUnsupportedVectorAddressingContour(ctx, value))
			{
				throw DecodeProjectionFaultException(std::format(L"Opcode {0:s} reached InstructionRegistry.CreateMicroOp(...) with unsupported {1:s} vector-slide addressing. ", OpcodeRegistry::GetMnemonicOrHex(ctx.OpCode), value) + L"The authoritative slide carrier only publishes 1D single-surface memory truth and must fail closed instead of reopening a non-representable compat surface.");
			}
			std::shared_ptr<VectorSlideMicroOp> vectorSlideMicroOp = std::make_shared<VectorSlideMicroOp>();
			vectorSlideMicroOp->setOpCode(ctx.OpCode);
			vectorSlideMicroOp->setInstruction(GetRequiredProjectedVectorInstruction(ctx));
			vectorSlideMicroOp->setPredicateMask(ctx.PredicateMask);
			vectorSlideMicroOp->InitializeMetadata();
			return vectorSlideMicroOp;
		});
		OpcodeInfo opcodeInfo = RequirePublishedOpcodeInfo(opCode);
		if (!opcodeInfo.isVector())
		{
			throw InvalidOperationException(std::format(L"Opcode {0:s} is not a published vector contour and cannot use the canonical vector-slide descriptor path.", OpcodeRegistry::GetMnemonicOrHex(opCode)));
		}
		std::shared_ptr<MicroOpDescriptor> temp_var = std::make_shared<MicroOpDescriptor>();
		temp_var->setLatency(opcodeInfo.ExecutionLatency);
		temp_var->setMemFootprintClass(2);
		temp_var->setMemoryOp(false);
		temp_var->setWritesRegister(false);
		RegisterOpAttributes(opCode, temp_var);
	}

	void InstructionRegistry::RegisterPublishedVectorPerm2Op(unsigned int opCode)
	{
		if (opCode != static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VPERM2))
		{
			throw InvalidOperationException(L"The Phase 09 permute2 registration helper is intentionally scoped to VPERM2 only.");
		}

		RegisterSemanticFactory(opCode, [=] (DecoderContext ctx)
		{
			std::wstring value;
			if (TryResolveUnsupportedVectorAddressingContour(ctx, value))
			{
				throw DecodeProjectionFaultException(std::format(L"Opcode {0:s} reached InstructionRegistry.CreateMicroOp(...) with unsupported {1:s} VPERM2 addressing. ", OpcodeRegistry::GetMnemonicOrHex(ctx.OpCode), value) + L"Phase 09 only publishes the packed 1D two-source two-lane immediate-controlled contour.");
			}
    
			std::shared_ptr<VectorPermute2MicroOp> microOp = std::make_shared<VectorPermute2MicroOp>();
			microOp->setOpCode(ctx.OpCode);
			microOp->setInstruction(GetRequiredProjectedVectorInstruction(ctx));
			microOp->setPredicateMask(ctx.PredicateMask);
			microOp->InitializeMetadata();
			return microOp;
		});

		OpcodeInfo opcodeInfo = RequirePublishedOpcodeInfo(opCode);
		if (!opcodeInfo.isVector())
		{
			throw InvalidOperationException(std::format(L"Opcode {0:s} is not a published vector contour and cannot use the Phase 09 permute2 descriptor path.", OpcodeRegistry::GetMnemonicOrHex(opCode)));
		}

		std::shared_ptr<MicroOpDescriptor> temp_var = std::make_shared<MicroOpDescriptor>();
		temp_var->setLatency(opcodeInfo.ExecutionLatency);
		temp_var->setMemFootprintClass(2);
		temp_var->setMemoryOp(false);
		temp_var->setWritesRegister(false);
		RegisterOpAttributes(opCode, temp_var);
	}

	void InstructionRegistry::RegisterPublishedVectorTransposeOp(unsigned int opCode)
	{
		if (opCode != static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VTRANSPOSE))
		{
			throw InvalidOperationException(L"The Phase 09 transpose registration helper is intentionally scoped to VTRANSPOSE only.");
		}

		RegisterSemanticFactory(opCode, [=] (DecoderContext ctx)
		{
			std::wstring value;
			if (TryResolveUnsupportedVectorAddressingContour(ctx, value))
			{
				throw DecodeProjectionFaultException(std::format(L"Opcode {0:s} reached InstructionRegistry.CreateMicroOp(...) with unsupported {1:s} VTRANSPOSE addressing. ", OpcodeRegistry::GetMnemonicOrHex(ctx.OpCode), value) + L"Phase 09 only publishes the packed 1D single-surface fixed 2x2 transpose contour.");
			}
    
			std::shared_ptr<VectorTransposeMicroOp> microOp = std::make_shared<VectorTransposeMicroOp>();
			microOp->setOpCode(ctx.OpCode);
			microOp->setInstruction(GetRequiredProjectedVectorInstruction(ctx));
			microOp->setPredicateMask(ctx.PredicateMask);
			microOp->InitializeMetadata();
			return microOp;
		});

		OpcodeInfo opcodeInfo = RequirePublishedOpcodeInfo(opCode);
		if (!opcodeInfo.isVector())
		{
			throw InvalidOperationException(std::format(L"Opcode {0:s} is not a published vector contour and cannot use the Phase 09 transpose descriptor path.", OpcodeRegistry::GetMnemonicOrHex(opCode)));
		}

		std::shared_ptr<MicroOpDescriptor> temp_var = std::make_shared<MicroOpDescriptor>();
		temp_var->setLatency(opcodeInfo.ExecutionLatency);
		temp_var->setMemFootprintClass(2);
		temp_var->setMemoryOp(false);
		temp_var->setWritesRegister(false);
		RegisterOpAttributes(opCode, temp_var);
	}

	void InstructionRegistry::RegisterPublishedVectorSlideOneUpOp(unsigned int opCode)
	{
		if (opCode != static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VSLIDE1UP))
		{
			throw InvalidOperationException(L"The Phase 09 slide-one registration helper is intentionally scoped to VSLIDE1UP only.");
		}

		RegisterSemanticFactory(opCode, [=] (DecoderContext ctx)
		{
			std::wstring value;
			if (TryResolveUnsupportedVectorAddressingContour(ctx, value))
			{
				throw DecodeProjectionFaultException(std::format(L"Opcode {0:s} reached InstructionRegistry.CreateMicroOp(...) with unsupported {1:s} VSLIDE1UP addressing. ", OpcodeRegistry::GetMnemonicOrHex(ctx.OpCode), value) + L"Phase 09 only publishes the packed 1D fixed-one-lane single-surface contour.");
			}
    
			std::shared_ptr<VectorSlideOneUpMicroOp> microOp = std::make_shared<VectorSlideOneUpMicroOp>();
			microOp->setOpCode(ctx.OpCode);
			microOp->setInstruction(GetRequiredProjectedVectorInstruction(ctx));
			microOp->setPredicateMask(ctx.PredicateMask);
			microOp->InitializeMetadata();
			return microOp;
		});

		OpcodeInfo opcodeInfo = RequirePublishedOpcodeInfo(opCode);
		if (!opcodeInfo.isVector())
		{
			throw InvalidOperationException(std::format(L"Opcode {0:s} is not a published vector contour and cannot use the Phase 09 slide-one descriptor path.", OpcodeRegistry::GetMnemonicOrHex(opCode)));
		}

		std::shared_ptr<MicroOpDescriptor> temp_var = std::make_shared<MicroOpDescriptor>();
		temp_var->setLatency(opcodeInfo.ExecutionLatency);
		temp_var->setMemFootprintClass(2);
		temp_var->setMemoryOp(false);
		temp_var->setWritesRegister(false);
		RegisterOpAttributes(opCode, temp_var);
	}

	void InstructionRegistry::RegisterPublishedVectorSlideOneDownOp(unsigned int opCode)
	{
		if (opCode != static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VSLIDE1DOWN))
		{
			throw InvalidOperationException(L"The Phase 09 slide-one-down registration helper is intentionally scoped to VSLIDE1DOWN only.");
		}

		RegisterSemanticFactory(opCode, [=] (DecoderContext ctx)
		{
			std::wstring value;
			if (TryResolveUnsupportedVectorAddressingContour(ctx, value))
			{
				throw DecodeProjectionFaultException(std::format(L"Opcode {0:s} reached InstructionRegistry.CreateMicroOp(...) with unsupported {1:s} VSLIDE1DOWN addressing. ", OpcodeRegistry::GetMnemonicOrHex(ctx.OpCode), value) + L"Phase 09 only publishes the packed 1D fixed-one-lane single-surface contour.");
			}
    
			std::shared_ptr<VectorSlideOneDownMicroOp> microOp = std::make_shared<VectorSlideOneDownMicroOp>();
			microOp->setOpCode(ctx.OpCode);
			microOp->setInstruction(GetRequiredProjectedVectorInstruction(ctx));
			microOp->setPredicateMask(ctx.PredicateMask);
			microOp->InitializeMetadata();
			return microOp;
		});

		OpcodeInfo opcodeInfo = RequirePublishedOpcodeInfo(opCode);
		if (!opcodeInfo.isVector())
		{
			throw InvalidOperationException(std::format(L"Opcode {0:s} is not a published vector contour and cannot use the Phase 09 slide-one-down descriptor path.", OpcodeRegistry::GetMnemonicOrHex(opCode)));
		}

		std::shared_ptr<MicroOpDescriptor> temp_var = std::make_shared<MicroOpDescriptor>();
		temp_var->setLatency(opcodeInfo.ExecutionLatency);
		temp_var->setMemFootprintClass(2);
		temp_var->setMemoryOp(false);
		temp_var->setWritesRegister(false);
		RegisterOpAttributes(opCode, temp_var);
	}

	void InstructionRegistry::RegisterPublishedVectorDotProductOp(unsigned int opCode)
	{
		RegisterSemanticFactory(opCode, [=] (DecoderContext ctx)
		{
			std::wstring value;
			if (TryResolveUnsupportedVectorAddressingContour(ctx, value))
			{
				throw DecodeProjectionFaultException(std::format(L"Opcode {0:s} reached InstructionRegistry.CreateMicroOp(...) with unsupported {1:s} vector-dot-product addressing. ", OpcodeRegistry::GetMnemonicOrHex(ctx.OpCode), value) + L"The authoritative dot-product carrier only publishes 1D scalar-footprint truth and must fail closed instead of reopening a non-representable compat surface.");
			}
			std::shared_ptr<VectorDotProductMicroOp> vectorDotProductMicroOp = std::make_shared<VectorDotProductMicroOp>();
			vectorDotProductMicroOp->setOpCode(ctx.OpCode);
			vectorDotProductMicroOp->setInstruction(GetRequiredProjectedVectorInstruction(ctx));
			vectorDotProductMicroOp->setPredicateMask(ctx.PredicateMask);
			vectorDotProductMicroOp->InitializeMetadata();
			return vectorDotProductMicroOp;
		});
		OpcodeInfo opcodeInfo = RequirePublishedOpcodeInfo(opCode);
		if (!opcodeInfo.isVector())
		{
			throw InvalidOperationException(std::format(L"Opcode {0:s} is not a published vector contour and cannot use the canonical vector-dot-product descriptor path.", OpcodeRegistry::GetMnemonicOrHex(opCode)));
		}
		std::shared_ptr<MicroOpDescriptor> temp_var = std::make_shared<MicroOpDescriptor>();
		temp_var->setLatency(opcodeInfo.ExecutionLatency);
		temp_var->setMemFootprintClass(2);
		temp_var->setMemoryOp(false);
		temp_var->setWritesRegister(false);
		RegisterOpAttributes(opCode, temp_var);
	}

	void InstructionRegistry::RegisterPublishedVectorDotProductWideOp(unsigned int opCode)
	{
		if (opCode != static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VDOT_WIDE))
		{
			throw InvalidOperationException(L"The Phase 09 wide-dot registration helper is intentionally scoped to VDOT.WIDE only.");
		}

		RegisterSemanticFactory(opCode, [=] (DecoderContext ctx)
		{
			std::wstring value;
			if (TryResolveUnsupportedVectorAddressingContour(ctx, value))
			{
				throw DecodeProjectionFaultException(std::format(L"Opcode {0:s} reached InstructionRegistry.CreateMicroOp(...) with unsupported {1:s} VDOT.WIDE addressing. ", OpcodeRegistry::GetMnemonicOrHex(ctx.OpCode), value) + L"The Phase 09 wide-dot carrier only publishes packed 1D scalar-footprint truth and must fail closed instead of reopening a non-representable compat surface.");
			}
    
			std::shared_ptr<VectorDotWideMicroOp> microOp = std::make_shared<VectorDotWideMicroOp>();
			microOp->setOpCode(ctx.OpCode);
			microOp->setInstruction(GetRequiredProjectedVectorInstruction(ctx));
			microOp->setPredicateMask(ctx.PredicateMask);
			microOp->InitializeMetadata();
			return microOp;
		});

		OpcodeInfo opcodeInfo = RequirePublishedOpcodeInfo(opCode);
		if (!opcodeInfo.isVector())
		{
			throw InvalidOperationException(std::format(L"Opcode {0:s} is not a published vector contour and cannot use the Phase 09 wide-dot descriptor path.", OpcodeRegistry::GetMnemonicOrHex(opCode)));
		}

		std::shared_ptr<MicroOpDescriptor> temp_var = std::make_shared<MicroOpDescriptor>();
		temp_var->setLatency(opcodeInfo.ExecutionLatency);
		temp_var->setMemFootprintClass(2);
		temp_var->setMemoryOp(false);
		temp_var->setWritesRegister(false);
		RegisterOpAttributes(opCode, temp_var);
	}

	void InstructionRegistry::RegisterVectorConfigOp(unsigned int opCode)
	{
		RegisterSemanticFactory(opCode, [=] (DecoderContext ctx)
		{
			Processor::CPU_Core::InstructionsEnum instructionsEnum = static_cast<Processor::CPU_Core::InstructionsEnum>(ctx.OpCode);
			unsigned char registerId;
			if (!TryDecodeCanonicalArchRegister(ctx.Reg1ID, registerId))
			{
				throw DecodeProjectionFaultException(std::format(L"Opcode {0:s} reached InstructionRegistry.CreateMicroOp(...) with a non-canonical rd encoding. ", OpcodeRegistry::GetMnemonicOrHex(static_cast<unsigned int>(instructionsEnum))) + L"Vector-config manual publication requires flat architectural register ids.");
			}
			unsigned short destRegID = ((registerId == std::numeric_limits<unsigned char>::max()) ? std::numeric_limits<unsigned short>::max() : registerId);
			std::shared_ptr<VConfigMicroOp> vConfigMicroOp = std::make_shared<VConfigMicroOp>();
			vConfigMicroOp->setOpCode(ctx.OpCode);
			vConfigMicroOp->setDestRegID(destRegID);
			switch (instructionsEnum)
			{
			case Processor::CPU_Core::InstructionsEnum::VSETVL:
			{
				unsigned char registerId3;
				unsigned char registerId4;
				if (!TryDecodeCanonicalArchRegister(ctx.Reg2ID, registerId3) || !TryDecodeCanonicalArchRegister(ctx.Reg3ID, registerId4) || registerId3 == std::numeric_limits<unsigned char>::max() || registerId4 == std::numeric_limits<unsigned char>::max())
				{
					throw DecodeProjectionFaultException(L"VSETVL manual publication requires canonical rs1/rs2 architectural registers.");
				}
				vConfigMicroOp->ConfigureForRegisterVType(registerId3, registerId4);
				break;
			}
			case Processor::CPU_Core::InstructionsEnum::VSETVLI:
			{
				unsigned char registerId2;
				if (!TryDecodeCanonicalArchRegister(ctx.Reg2ID, registerId2) || registerId2 == std::numeric_limits<unsigned char>::max())
				{
					throw DecodeProjectionFaultException(L"VSETVLI manual publication requires a canonical rs1 architectural register.");
				}
				vConfigMicroOp->ConfigureForImmediateVType(VectorConfigOperationKind::Vsetvli, registerId2, GetRequiredDecoderDataType(ctx, L"Vector-config opcode VSETVLI"));
				break;
			}
			case Processor::CPU_Core::InstructionsEnum::VSETIVLI:
				vConfigMicroOp->ConfigureForImmediateAvlAndVType(GetRequiredDecoderImmediate(ctx, L"Vector-config opcode VSETIVLI"), GetRequiredDecoderDataType(ctx, L"Vector-config opcode VSETIVLI"));
				break;
			default:
				throw InvalidOperationException(std::format(L"Unsupported vector-config opcode {0:s}.", OpcodeRegistry::GetMnemonicOrHex(static_cast<unsigned int>(instructionsEnum))));
			}
			vConfigMicroOp->InitializeMetadata();
			return vConfigMicroOp;
		});
		std::optional<bool> isMemoryOp = false;
		RegisterOpAttributes(opCode, CreatePublishedSystemLikeDescriptor(opCode, std::nullopt, isMemoryOp));
	}

	void InstructionRegistry::RegisterRetainedMoveOp(unsigned int opCode)
	{
		RegisterSemanticFactory(opCode, [=] (DecoderContext ctx)
		{
			unsigned char dataType = GetRequiredDecoderDataType(ctx, L"Retained legacy Move");
			if (dataType == 2 || dataType == 3)
			{
				throw DecodeProjectionFaultException(std::format(L"Retained legacy Move DT={0} memory contour reached InstructionRegistry.CreateMicroOp(...) directly. ", dataType) + L"This contour must be canonicalized to Load/Store before runtime materialization/manual publication.");
			}
			if (dataType == 4)
			{
				throw DecodeProjectionFaultException(L"Retained legacy Move DT=4 dual-write contour is unsupported and must fail closed.");
			}
			if (dataType == 5)
			{
				throw DecodeProjectionFaultException(L"Retained legacy Move DT=5 triple-destination contour is unsupported and must fail closed.");
			}
	// C# TO C++ CONVERTER TASK: The following 'switch expression' was not converted by C# to C++ Converter:
	//					ushort num = dataType switch
	//					{
	//						0 => ctx.Reg2ID,
	//						1 => ctx.Reg1ID,
	//						_ => throw new DecodeProjectionFaultException($"Retained legacy Move DT={dataType} is unsupported on the direct/manual runtime publication path."),
	//					};
			unsigned short destRegID;
			switch (dataType)
			{
				case 0: destRegID = ctx.Reg2ID; break;
				case 1: destRegID = ctx.Reg1ID; break;
				default:
					throw DecodeProjectionFaultException(std::format(L"Retained legacy Move DT={0} is unsupported on the direct/manual runtime publication path.", dataType));
			}
			std::shared_ptr<MoveMicroOp> moveMicroOp = std::make_shared<MoveMicroOp>();
			moveMicroOp->setOpCode(ctx.OpCode);
			moveMicroOp->setDestRegID(destRegID);
			moveMicroOp->setMemoryOp(false);
			moveMicroOp->setWritesRegister(dataType == 0 || dataType == 1);
			moveMicroOp->ApplyCanonicalRuntimeMoveShapeProjection(dataType, ctx.Reg1ID, ctx.Reg2ID, ctx.AuxData);
			moveMicroOp->RefreshWriteMetadata();
			return moveMicroOp;
		});
		std::shared_ptr<MicroOpDescriptor> descriptor = std::make_shared<MicroOpDescriptor>();
		descriptor->setLatency(1);
		descriptor->setMemFootprintClass(1);
		RegisterOpAttributes(opCode, descriptor);
	}

	void InstructionRegistry::RegisterRetainedMoveNumOp(unsigned int opCode)
	{
		RegisterSemanticFactory(opCode, [=] (DecoderContext ctx)
		{
			std::shared_ptr<MoveMicroOp> moveMicroOp = std::make_shared<MoveMicroOp>();
			moveMicroOp->setOpCode(ctx.OpCode);
			moveMicroOp->setDestRegID(ctx.Reg1ID);
			moveMicroOp->setMemoryOp(false);
			moveMicroOp->setWritesRegister(true);
			moveMicroOp->ApplyCanonicalRuntimeMoveShapeProjection(1, ctx.Reg1ID, ctx.Reg2ID, ctx.AuxData);
			moveMicroOp->RefreshWriteMetadata();
			return moveMicroOp;
		});
		std::shared_ptr<MicroOpDescriptor> temp_var = std::make_shared<MicroOpDescriptor>();
		temp_var->setLatency(1);
		temp_var->setMemFootprintClass(1);
		RegisterOpAttributes(opCode, temp_var);
	}

	void InstructionRegistry::RegisterRetainedAbsoluteLoadOp(unsigned int opCode)
	{
		RegisterSemanticFactory(opCode, [=] (DecoderContext ctx)
		{
			std::shared_ptr<LoadMicroOp> loadMicroOp = std::make_shared<LoadMicroOp>();
			loadMicroOp->setOpCode(ctx.OpCode);
			loadMicroOp->setDestRegID(NormalizeRequiredLegacyMemoryRegister(ctx.OpCode, ctx.Reg1ID, L"destination", L"Load"));
			loadMicroOp->setBaseRegID(std::numeric_limits<unsigned short>::max());
			loadMicroOp->setAddress(GetRequiredDecoderMemoryAddress(ctx, L"Retained legacy Load"));
			loadMicroOp->setSize(8);
			loadMicroOp->setWritesRegister(true);
			loadMicroOp->InitializeMetadata();
			return loadMicroOp;
		});
		std::shared_ptr<MicroOpDescriptor> temp_var = std::make_shared<MicroOpDescriptor>();
		temp_var->setLatency(1);
		temp_var->setMemFootprintClass(1);
		temp_var->setMemoryOp(true);
		temp_var->setWritesRegister(true);
		RegisterOpAttributes(opCode, temp_var);
	}

	void InstructionRegistry::RegisterRetainedAbsoluteStoreOp(unsigned int opCode)
	{
		RegisterSemanticFactory(opCode, [=] (DecoderContext ctx)
		{
			unsigned short rawRegId = ((ctx.Reg1ID != std::numeric_limits<unsigned short>::max()) ? ctx.Reg1ID : ctx.Reg3ID);
			std::shared_ptr<StoreMicroOp> storeMicroOp = std::make_shared<StoreMicroOp>();
			storeMicroOp->setOpCode(ctx.OpCode);
			storeMicroOp->setSrcRegID(NormalizeRequiredLegacyMemoryRegister(ctx.OpCode, rawRegId, L"source", L"Store"));
			storeMicroOp->setBaseRegID(std::numeric_limits<unsigned short>::max());
			storeMicroOp->setAddress(GetRequiredDecoderMemoryAddress(ctx, L"Retained legacy Store"));
			storeMicroOp->setSize(8);
			storeMicroOp->InitializeMetadata();
			return storeMicroOp;
		});
		std::shared_ptr<MicroOpDescriptor> temp_var = std::make_shared<MicroOpDescriptor>();
		temp_var->setLatency(1);
		temp_var->setMemFootprintClass(1);
		temp_var->setMemoryOp(true);
		temp_var->setWritesRegister(false);
		RegisterOpAttributes(opCode, temp_var);
	}

	void InstructionRegistry::RegisterBaseInstructionSet()
	{
		RegisterSemanticFactory(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::Nope), [=] (DecoderContext ctx)
		{
			std::shared_ptr<NopMicroOp> temp_var = std::make_shared<NopMicroOp>();
			temp_var->setOpCode(ctx.OpCode);
			temp_var->setLatency(0);
			return temp_var;
		});
		std::shared_ptr<MicroOpDescriptor> temp_var2 = std::make_shared<MicroOpDescriptor>();
		temp_var2->setLatency(0);
		temp_var2->setMemFootprintClass(0);
		temp_var2->setMemoryOp(false);
		RegisterOpAttributes(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::Nope), temp_var2);



		RegisterScalarImmediateOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::ADDI));
		RegisterScalarImmediateOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::ADDIW));
		RegisterScalarImmediateOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::SLLIW));
		RegisterScalarImmediateOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::SRLIW));
		RegisterScalarImmediateOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::SRAIW));
		RegisterScalarImmediateOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::ANDI));
		RegisterScalarImmediateOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::ORI));
		RegisterScalarImmediateOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::XORI));
		RegisterScalarImmediateOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::SLTI));
		RegisterScalarImmediateOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::SLTIU));
		RegisterScalarImmediateOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::SLLI));
		RegisterScalarImmediateOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::SRLI));
		RegisterScalarImmediateOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::SRAI));
		RegisterScalarRegisterOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::SLT));
		RegisterScalarRegisterOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::SLTU));
		RegisterScalarImmediateOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::LUI));
		RegisterScalarImmediateOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::AUIPC));
		RegisterScalarRegisterOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::MULH));
		RegisterScalarRegisterOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::MULHU));
		RegisterScalarRegisterOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::MULHSU));
		RegisterScalarRegisterOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::DIVU));
		RegisterScalarRegisterOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::REM));
		RegisterScalarRegisterOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::REMU));

		RegisterTypedLoadOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::LB));
		RegisterTypedLoadOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::LBU));
		RegisterTypedLoadOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::LH));
		RegisterTypedLoadOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::LHU));
		RegisterTypedLoadOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::LW));
		RegisterTypedLoadOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::LWU));
		RegisterTypedLoadOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::LD));

		RegisterTypedStoreOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::SB));
		RegisterTypedStoreOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::SH));
		RegisterTypedStoreOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::SW));
		RegisterTypedStoreOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::SD));
		RegisterSemanticFactory(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::DmaStreamCompute), [=] (DecoderContext ctx) -> std::shared_ptr<MicroOp>
		{
			throw DecodeProjectionFaultException(std::wstring(L"DmaStreamCompute requires a guard-accepted descriptor sideband from native VLIW decode. ") + L"InstructionRegistry raw factory publication is not the canonical lane6 descriptor path.");
		});
		std::shared_ptr<MicroOpDescriptor> temp_var3 = std::make_shared<MicroOpDescriptor>();
		temp_var3->setLatency(8);
		temp_var3->setMemFootprintClass(3);
		temp_var3->setMemoryOp(true);
		temp_var3->setWritesRegister(false);
		RegisterOpAttributes(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::DmaStreamCompute), temp_var3);
		RegisterSemanticFactory(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::DSC_STATUS), [=] (DecoderContext ctx)
		{
			if (ctx.Reg3ID != 0 && ctx.Reg3ID != HybridCPU_ISE::Arch::VLIW_Instruction::NoReg)
			{
				throw DecodeProjectionFaultException(L"DSC_STATUS uses the canonical packed register ABI rd, rs1(token-id), x0.");
			}
    
			return std::make_shared<DmaStreamComputeStatusMicroOp>(ctx.Reg1ID, ctx.Reg2ID);
		});
		std::shared_ptr<MicroOpDescriptor> temp_var4 = std::make_shared<MicroOpDescriptor>();
		temp_var4->setLatency(2);
		temp_var4->setMemFootprintClass(1);
		temp_var4->setMemoryOp(false);
		RegisterOpAttributes(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::DSC_STATUS), temp_var4);
		RegisterSemanticFactory(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::DSC_QUERY_CAPS), [=] (DecoderContext ctx)
		{
			if (!IsZeroOrNoRegister(ctx.Reg2ID) || !IsZeroOrNoRegister(ctx.Reg3ID))
			{
				throw DecodeProjectionFaultException(L"DSC_QUERY_CAPS uses the canonical packed register ABI rd, x0, x0.");
			}
    
			return std::make_shared<DmaStreamComputeQueryCapsMicroOp>(ctx.Reg1ID);
		});
		std::shared_ptr<MicroOpDescriptor> temp_var5 = std::make_shared<MicroOpDescriptor>();
		temp_var5->setLatency(2);
		temp_var5->setMemFootprintClass(1);
		temp_var5->setMemoryOp(false);
		RegisterOpAttributes(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::DSC_QUERY_CAPS), temp_var5);

		RegisterSystemDeviceCommandOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::ACCEL_QUERY_CAPS), [] (DecoderContext ctx)
		{
				if (!IsZeroOrNoRegister(ctx.Reg2ID) || !IsZeroOrNoRegister(ctx.Reg3ID))
				{
					throw DecodeProjectionFaultException(L"ACCEL_QUERY_CAPS uses the canonical packed register ABI rd, x0, x0.");
				}
    
				return std::make_shared<AcceleratorQueryCapsMicroOp>(ctx.Reg1ID);
		});
		RegisterSystemDeviceCommandOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::ACCEL_SUBMIT), [] (DecoderContext ctx)
		{
				if (!IsZeroOrNoRegister(ctx.Reg2ID) || !IsZeroOrNoRegister(ctx.Reg3ID))
				{
					throw DecodeProjectionFaultException(L"ACCEL_SUBMIT uses the canonical packed register ABI rd, x0, x0 plus typed descriptor sideband.");
				}
    
				return std::make_shared<AcceleratorSubmitMicroOp>(ctx.Reg1ID);
		});
		RegisterSystemDeviceCommandOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::ACCEL_POLL), [] (DecoderContext ctx)
		{
			return std::make_shared<AcceleratorPollMicroOp>(ctx.Reg1ID, ctx.Reg2ID);
		});
		RegisterSystemDeviceCommandOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::ACCEL_WAIT), [] (DecoderContext ctx)
		{
			return std::make_shared<AcceleratorWaitMicroOp>(ctx.Reg1ID, ctx.Reg2ID);
		});
		RegisterSystemDeviceCommandOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::ACCEL_CANCEL), [] (DecoderContext ctx)
		{
			return std::make_shared<AcceleratorCancelMicroOp>(ctx.Reg1ID, ctx.Reg2ID);
		});
		RegisterSystemDeviceCommandOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::ACCEL_FENCE), [] (DecoderContext ctx)
		{
			return std::make_shared<AcceleratorFenceMicroOp>(ctx.Reg1ID, ctx.Reg2ID);
		});
		RegisterSystemDeviceCommandOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::ACCEL_STATUS), [] (DecoderContext ctx)
		{
				if (!IsZeroOrNoRegister(ctx.Reg3ID))
				{
					throw DecodeProjectionFaultException(L"ACCEL_STATUS uses the canonical packed register ABI rd, rs1(token-id), x0.");
				}
    
				return std::make_shared<AcceleratorStatusMicroOp>(ctx.Reg1ID, ctx.Reg2ID);
		});

		RegisterBranchOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::JAL), false);
		RegisterBranchOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::JALR), false);
		RegisterBranchOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::BEQ), true);
		RegisterBranchOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::BNE), true);
		RegisterBranchOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::BLT), true);
		RegisterBranchOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::BGE), true);
		RegisterBranchOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::BLTU), true);
		RegisterBranchOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::BGEU), true);

		RegisterAtomicOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::LR_W));
		RegisterAtomicOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::SC_W));
		RegisterAtomicOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::LR_D));
		RegisterAtomicOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::SC_D));
		RegisterAtomicOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::AMOADD_W));
		RegisterAtomicOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::AMOSWAP_W));
		RegisterAtomicOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::AMOOR_W));
		RegisterAtomicOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::AMOAND_W));
		RegisterAtomicOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::AMOXOR_W));
		RegisterAtomicOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::AMOMIN_W));
		RegisterAtomicOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::AMOMAX_W));
		RegisterAtomicOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::AMOMINU_W));
		RegisterAtomicOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::AMOMAXU_W));
		RegisterAtomicOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::AMOADD_D));
		RegisterAtomicOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::AMOSWAP_D));
		RegisterAtomicOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::AMOOR_D));
		RegisterAtomicOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::AMOAND_D));
		RegisterAtomicOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::AMOXOR_D));
		RegisterAtomicOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::AMOMIN_D));
		RegisterAtomicOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::AMOMAX_D));
		RegisterAtomicOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::AMOMINU_D));
		RegisterAtomicOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::AMOMAXU_D));

		RegisterSemanticFactory(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::FENCE), [=] (DecoderContext ctx)
		{
			return CreateFenceSystemEventMicroOp(ctx, [] () {return SysEventMicroOp::ForFence();});
		});
		RegisterOpAttributes(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::FENCE), CreatePublishedSystemLikeDescriptor(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::FENCE)));

		RegisterSemanticFactory(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::FENCE_I), [=] (DecoderContext ctx)
		{
			return CreateFenceSystemEventMicroOp(ctx, [] () {return SysEventMicroOp::ForFenceI();});
		});
		RegisterOpAttributes(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::FENCE_I), CreatePublishedSystemLikeDescriptor(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::FENCE_I)));

		RegisterCsrClearOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::CSR_CLEAR));
		RegisterCounterReadCsrOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::RDCYCLE), CsrAddresses::Cycle);

		RegisterSemanticFactory(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::ECALL), [=] (DecoderContext ctx)
		{
			return CreateSystemEventMicroOp(ctx.OpCode, [] () {return SysEventMicroOp::ForEcall();});
		});
		RegisterOpAttributes(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::ECALL), CreatePublishedSystemLikeDescriptor(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::ECALL)));
		RegisterSemanticFactory(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::EBREAK), [=] (DecoderContext ctx)
		{
			return CreateSystemEventMicroOp(ctx.OpCode, [] () {return SysEventMicroOp::ForEbreak();});
		});
		RegisterOpAttributes(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::EBREAK), CreatePublishedSystemLikeDescriptor(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::EBREAK)));
		RegisterSemanticFactory(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::MRET), [=] (DecoderContext ctx)
		{
			return CreateSystemEventMicroOp(ctx.OpCode, [] () {return SysEventMicroOp::ForMret();});
		});
		RegisterOpAttributes(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::MRET), CreatePublishedSystemLikeDescriptor(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::MRET)));
		RegisterSemanticFactory(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::SRET), [=] (DecoderContext ctx)
		{
			return CreateSystemEventMicroOp(ctx.OpCode, [] () {return SysEventMicroOp::ForSret();});
		});
		RegisterOpAttributes(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::SRET), CreatePublishedSystemLikeDescriptor(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::SRET)));
		RegisterSemanticFactory(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::WFI), [=] (DecoderContext ctx)
		{
			return CreateSystemEventMicroOp(ctx.OpCode, [] () {return SysEventMicroOp::ForWfi();});
		});
		RegisterOpAttributes(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::WFI), CreatePublishedSystemLikeDescriptor(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::WFI)));
		RegisterSemanticFactory(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::WFE), [=] (DecoderContext ctx)
		{
			return CreateSystemEventMicroOp(ctx.OpCode, [] () {return SysEventMicroOp::ForWfe();});
		});
		RegisterOpAttributes(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::WFE), CreatePublishedSystemLikeDescriptor(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::WFE)));
		RegisterSemanticFactory(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::SEV), [=] (DecoderContext ctx)
		{
			return CreateSystemEventMicroOp(ctx.OpCode, [] () {return SysEventMicroOp::ForSev();});
		});
		RegisterOpAttributes(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::SEV), CreatePublishedSystemLikeDescriptor(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::SEV)));
		RegisterSemanticFactory(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::YIELD), [=] (DecoderContext ctx)
		{
			return CreateSystemEventMicroOp(ctx.OpCode, [] () {return SysEventMicroOp::ForYield();});
		});
		RegisterOpAttributes(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::YIELD), CreatePublishedSystemLikeDescriptor(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::YIELD)));
		RegisterSemanticFactory(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::POD_BARRIER), [=] (DecoderContext ctx)
		{
			return CreateSystemEventMicroOp(ctx.OpCode, [] () {return SysEventMicroOp::ForPodBarrier();});
		});
		RegisterOpAttributes(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::POD_BARRIER), CreatePublishedSystemLikeDescriptor(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::POD_BARRIER)));
		RegisterSemanticFactory(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VT_BARRIER), [=] (DecoderContext ctx)
		{
			return CreateSystemEventMicroOp(ctx.OpCode, [] () {return SysEventMicroOp::ForVtBarrier();});
		});
		RegisterOpAttributes(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VT_BARRIER), CreatePublishedSystemLikeDescriptor(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VT_BARRIER)));

		RegisterSemanticFactory(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::CSRRW), [=] (DecoderContext ctx)
		{
			std::shared_ptr<CsrReadWriteMicroOp> temp_var6 = std::make_shared<CsrReadWriteMicroOp>();
			temp_var6->setOpCode(ctx.OpCode);
			temp_var6->setCSRAddress(GetRequiredDecoderImmediate(ctx, std::format(L"CSR opcode {0:s}", OpcodeRegistry::GetMnemonicOrHex(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::CSRRW)))));
			temp_var6->setDestRegID(NormalizeCanonicalOrSuppressedCsrDestinationRegister(ctx.OpCode, ctx.Reg1ID));
			temp_var6->setSrcRegID(NormalizeCanonicalOrSuppressedCsrSourceRegister(ctx.OpCode, ctx.Reg2ID, L"rs1"));
			return temp_var6;
		});
		RegisterOpAttributes(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::CSRRW), CreatePublishedSystemLikeDescriptor(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::CSRRW), true, false, 0));

		RegisterSemanticFactory(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::CSRRS), [=] (DecoderContext ctx)
		{
			std::shared_ptr<CsrReadSetMicroOp> temp_var7 = std::make_shared<CsrReadSetMicroOp>();
			temp_var7->setOpCode(ctx.OpCode);
			temp_var7->setCSRAddress(GetRequiredDecoderImmediate(ctx, std::format(L"CSR opcode {0:s}", OpcodeRegistry::GetMnemonicOrHex(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::CSRRS)))));
			temp_var7->setDestRegID(NormalizeCanonicalOrSuppressedCsrDestinationRegister(ctx.OpCode, ctx.Reg1ID));
			temp_var7->setSrcRegID(NormalizeCanonicalOrSuppressedCsrSourceRegister(ctx.OpCode, ctx.Reg2ID, L"rs1"));
			return temp_var7;
		});
		RegisterOpAttributes(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::CSRRS), CreatePublishedSystemLikeDescriptor(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::CSRRS), true, false, 0));

		RegisterSemanticFactory(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::CSRRC), [=] (DecoderContext ctx)
		{
			std::shared_ptr<CsrReadClearMicroOp> temp_var8 = std::make_shared<CsrReadClearMicroOp>();
			temp_var8->setOpCode(ctx.OpCode);
			temp_var8->setCSRAddress(GetRequiredDecoderImmediate(ctx, std::format(L"CSR opcode {0:s}", OpcodeRegistry::GetMnemonicOrHex(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::CSRRC)))));
			temp_var8->setDestRegID(NormalizeCanonicalOrSuppressedCsrDestinationRegister(ctx.OpCode, ctx.Reg1ID));
			temp_var8->setSrcRegID(NormalizeCanonicalOrSuppressedCsrSourceRegister(ctx.OpCode, ctx.Reg2ID, L"rs1"));
			return temp_var8;
		});
		RegisterOpAttributes(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::CSRRC), CreatePublishedSystemLikeDescriptor(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::CSRRC), true, false, 0));

		RegisterSemanticFactory(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::CSRRWI), [=] (DecoderContext ctx)
		{
			std::shared_ptr<CsrReadWriteImmediateMicroOp> temp_var9 = std::make_shared<CsrReadWriteImmediateMicroOp>();
			temp_var9->setOpCode(ctx.OpCode);
			temp_var9->setCSRAddress(GetRequiredDecoderImmediate(ctx, std::format(L"CSR opcode {0:s}", OpcodeRegistry::GetMnemonicOrHex(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::CSRRWI)))));
			temp_var9->setDestRegID(NormalizeCanonicalOrSuppressedCsrDestinationRegister(ctx.OpCode, ctx.Reg1ID));
			temp_var9->setWriteValue(NormalizeCanonicalCsrImmediateField(ctx.OpCode, ctx.Reg2ID, L"zimm"));
			return temp_var9;
		});
		RegisterOpAttributes(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::CSRRWI), CreatePublishedSystemLikeDescriptor(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::CSRRWI), true, false, 0));

		RegisterSemanticFactory(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::CSRRSI), [=] (DecoderContext ctx)
		{
			std::shared_ptr<CsrReadSetImmediateMicroOp> temp_var10 = std::make_shared<CsrReadSetImmediateMicroOp>();
			temp_var10->setOpCode(ctx.OpCode);
			temp_var10->setCSRAddress(GetRequiredDecoderImmediate(ctx, std::format(L"CSR opcode {0:s}", OpcodeRegistry::GetMnemonicOrHex(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::CSRRSI)))));
			temp_var10->setDestRegID(NormalizeCanonicalOrSuppressedCsrDestinationRegister(ctx.OpCode, ctx.Reg1ID));
			temp_var10->setWriteValue(NormalizeCanonicalCsrImmediateField(ctx.OpCode, ctx.Reg2ID, L"zimm"));
			return temp_var10;
		});
		RegisterOpAttributes(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::CSRRSI), CreatePublishedSystemLikeDescriptor(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::CSRRSI), true, false, 0));

		RegisterSemanticFactory(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::CSRRCI), [=] (DecoderContext ctx)
		{
			std::shared_ptr<CsrReadClearImmediateMicroOp> temp_var11 = std::make_shared<CsrReadClearImmediateMicroOp>();
			temp_var11->setOpCode(ctx.OpCode);
			temp_var11->setCSRAddress(GetRequiredDecoderImmediate(ctx, std::format(L"CSR opcode {0:s}", OpcodeRegistry::GetMnemonicOrHex(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::CSRRCI)))));
			temp_var11->setDestRegID(NormalizeCanonicalOrSuppressedCsrDestinationRegister(ctx.OpCode, ctx.Reg1ID));
			temp_var11->setWriteValue(NormalizeCanonicalCsrImmediateField(ctx.OpCode, ctx.Reg2ID, L"zimm"));
			return temp_var11;
		});
		RegisterOpAttributes(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::CSRRCI), CreatePublishedSystemLikeDescriptor(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::CSRRCI), true, false, 0));

		RegisterVectorExceptionControlCsrOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VSETVEXCPMASK), CsrAddresses::VexcpMask);
		RegisterVectorExceptionControlCsrOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VSETVEXCPPRI), CsrAddresses::VexcpPri);

		RegisterSemanticFactory(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::STREAM_SETUP), [=] (DecoderContext ctx)
		{
			std::shared_ptr<StreamControlMicroOp> op = std::make_shared<StreamControlMicroOp>();
			op->setOpCode(ctx.OpCode);
			op->InitializeMetadata();
			return op;
		});
		RegisterOpAttributes(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::STREAM_SETUP), CreatePublishedSystemLikeDescriptor(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::STREAM_SETUP), std::nullopt, std::nullopt, 3));
		RegisterSemanticFactory(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::STREAM_START), [=] (DecoderContext ctx)
		{
			std::shared_ptr<StreamControlMicroOp> op = std::make_shared<StreamControlMicroOp>();
			op->setOpCode(ctx.OpCode);
			op->InitializeMetadata();
			return op;
		});
		RegisterOpAttributes(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::STREAM_START), CreatePublishedSystemLikeDescriptor(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::STREAM_START), std::nullopt, std::nullopt, 3));
		RegisterSemanticFactory(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::STREAM_WAIT), [=] (DecoderContext ctx)
		{
			std::shared_ptr<StreamControlMicroOp> op = std::make_shared<StreamControlMicroOp>();
			op->setOpCode(ctx.OpCode);
			op->InitializeMetadata();
			return op;
		});
		RegisterOpAttributes(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::STREAM_WAIT), CreatePublishedSystemLikeDescriptor(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::STREAM_WAIT), std::nullopt, std::nullopt, 3));

		RegisterVmxOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VMXON));
		RegisterVmxOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VMXOFF));
		RegisterVmxOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VMLAUNCH));
		RegisterVmxOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VMRESUME));
		RegisterVmxOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VMREAD));
		RegisterVmxOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VMWRITE));
		RegisterVmxOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VMCLEAR));
		RegisterVmxOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VMPTRLD));
		RegisterVmxOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VMPTRST));
		RegisterVmxOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VMCALL));
		RegisterVmxOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::INVEPT));
		RegisterVmxOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::INVVPID));
		RegisterVmxOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VMFUNC));
		RegisterVmxOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VMSAVEX));
		RegisterVmxOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VMRESTX));
	}

	void InstructionRegistry::RegisterSystemDeviceCommandOp(unsigned int opCode, std::function<std::shared_ptr<SystemDeviceCommandMicroOp>(DecoderContext)> factory)
	{
		RegisterSemanticFactory(opCode, [=] (DecoderContext ctx)
		{
			std::shared_ptr<SystemDeviceCommandMicroOp> microOp = factory(ctx);
			if (microOp->getOpCode() != ctx.OpCode)
			{
				throw DecodeProjectionFaultException(std::format(L"L7-SDC factory opcode mismatch: expected 0x{0:0>X}, created 0x{1:0>X}.", ctx.OpCode, microOp->getOpCode()));
			}
    
			return microOp;
		});

		RegisterOpAttributes(opCode, CreatePublishedSystemLikeDescriptor(opCode, std::nullopt, false, 0));
	}

	bool InstructionRegistry::IsZeroOrNoRegister(unsigned short registerId)
	{
		return registerId == 0 || registerId == HybridCPU_ISE::Arch::VLIW_Instruction::NoReg;
	}

	void InstructionRegistry::Initialize()
	{
		if (_initialized)
		{
			return;
		}

		_initialized = true;

		RegisterScalarArithmeticInstructions();
		RegisterVectorInstructions();
		RegisterMatrixTileInstructions();
		RegisterBaseInstructionSet();
	}

	void InstructionRegistry::RegisterScalarArithmeticInstructions()
	{
		// ========== Scalar Arithmetic ==========
		// Example: Register with explicit MicroOpDescriptor for better FSP awareness
		RegisterSemanticFactory(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::ADD), [=] (DecoderContext ctx)
		{
			std::shared_ptr<ScalarALUMicroOp> op = std::make_shared<ScalarALUMicroOp>();
			op->setOpCode(ctx.OpCode);
			op->setDestRegID(ctx.Reg1ID);
			op->setSrc1RegID(ctx.Reg2ID);
			op->setSrc2RegID(ctx.Reg3ID);
			op->setUsesImmediate(false);
			op->InitializeMetadata();
			return op;
		});
		RegisterOpAttributes(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::ADD), CreatePublishedScalarDescriptor(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::ADD)));

		RegisterSemanticFactory(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::SUB), [=] (DecoderContext ctx)
		{
			std::shared_ptr<ScalarALUMicroOp> op = std::make_shared<ScalarALUMicroOp>();
			op->setOpCode(ctx.OpCode);
			op->setDestRegID(ctx.Reg1ID);
			op->setSrc1RegID(ctx.Reg2ID);
			op->setSrc2RegID(ctx.Reg3ID);
			op->setUsesImmediate(false);
			op->InitializeMetadata();
			return op;
		});
		RegisterOpAttributes(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::SUB), CreatePublishedScalarDescriptor(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::SUB)));

		RegisterScalarRegisterOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::ADDW));
		RegisterScalarRegisterOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::SUBW));
		RegisterScalarRegisterOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::SLLW));
		RegisterScalarRegisterOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::SRLW));
		RegisterScalarRegisterOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::SRAW));
		RegisterScalarRegisterOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::MULW));
		RegisterScalarRegisterOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::DIVW));
		RegisterScalarRegisterOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::DIVUW));
		RegisterScalarRegisterOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::REMW));
		RegisterScalarRegisterOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::REMUW));
		RegisterScalarUnaryRegisterOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::SEXT_W));
		RegisterScalarUnaryRegisterOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::ZEXT_W));

		RegisterSemanticFactory(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::MUL), [=] (DecoderContext ctx)
		{
			std::shared_ptr<ScalarALUMicroOp> op = std::make_shared<ScalarALUMicroOp>();
			op->setOpCode(ctx.OpCode);
			op->setDestRegID(ctx.Reg1ID);
			op->setSrc1RegID(ctx.Reg2ID);
			op->setSrc2RegID(ctx.Reg3ID);
			op->setUsesImmediate(false);
			op->InitializeMetadata();
			return op;
		});
		RegisterOpAttributes(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::MUL), CreatePublishedScalarDescriptor(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::MUL)));

		RegisterSemanticFactory(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::DIV), [=] (DecoderContext ctx)
		{
			std::shared_ptr<ScalarALUMicroOp> op = std::make_shared<ScalarALUMicroOp>();
			op->setOpCode(ctx.OpCode);
			op->setDestRegID(ctx.Reg1ID);
			op->setSrc1RegID(ctx.Reg2ID);
			op->setSrc2RegID(ctx.Reg3ID);
			op->setUsesImmediate(false);
			op->InitializeMetadata();
			return op;
		});
		RegisterOpAttributes(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::DIV), CreatePublishedScalarDescriptor(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::DIV)));

		RegisterSemanticFactory(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::SLL), [=] (DecoderContext ctx)
		{
			std::shared_ptr<ScalarALUMicroOp> op = std::make_shared<ScalarALUMicroOp>();
			op->setOpCode(ctx.OpCode);
			op->setDestRegID(ctx.Reg1ID);
			op->setSrc1RegID(ctx.Reg2ID);
			op->setSrc2RegID(ctx.Reg3ID);
			op->setUsesImmediate(false);
			op->InitializeMetadata();
			return op;
		});
		std::shared_ptr<MicroOpDescriptor> temp_var = std::make_shared<MicroOpDescriptor>();
		temp_var->setLatency(1);
		temp_var->setMemFootprintClass(0);
		temp_var->setWritesRegister(true);
		temp_var->setMemoryOp(false);
		RegisterOpAttributes(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::SLL), temp_var);

		RegisterSemanticFactory(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::SRL), [=] (DecoderContext ctx)
		{
			std::shared_ptr<ScalarALUMicroOp> op = std::make_shared<ScalarALUMicroOp>();
			op->setOpCode(ctx.OpCode);
			op->setDestRegID(ctx.Reg1ID);
			op->setSrc1RegID(ctx.Reg2ID);
			op->setSrc2RegID(ctx.Reg3ID);
			op->setUsesImmediate(false);
			op->InitializeMetadata();
			return op;
		});
		std::shared_ptr<MicroOpDescriptor> temp_var2 = std::make_shared<MicroOpDescriptor>();
		temp_var2->setLatency(1);
		temp_var2->setMemFootprintClass(0);
		temp_var2->setWritesRegister(true);
		temp_var2->setMemoryOp(false);
		RegisterOpAttributes(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::SRL), temp_var2);

		RegisterScalarRegisterOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::SRA));

		RegisterSemanticFactory(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::XOR), [=] (DecoderContext ctx)
		{
			std::shared_ptr<ScalarALUMicroOp> op = std::make_shared<ScalarALUMicroOp>();
			op->setOpCode(ctx.OpCode);
			op->setDestRegID(ctx.Reg1ID);
			op->setSrc1RegID(ctx.Reg2ID);
			op->setSrc2RegID(ctx.Reg3ID);
			op->setUsesImmediate(false);
			op->InitializeMetadata();
			return op;
		});
		std::shared_ptr<MicroOpDescriptor> temp_var3 = std::make_shared<MicroOpDescriptor>();
		temp_var3->setLatency(1);
		temp_var3->setMemFootprintClass(0);
		temp_var3->setWritesRegister(true);
		temp_var3->setMemoryOp(false);
		RegisterOpAttributes(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::XOR), temp_var3);

		RegisterSemanticFactory(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::OR), [=] (DecoderContext ctx)
		{
			std::shared_ptr<ScalarALUMicroOp> op = std::make_shared<ScalarALUMicroOp>();
			op->setOpCode(ctx.OpCode);
			op->setDestRegID(ctx.Reg1ID);
			op->setSrc1RegID(ctx.Reg2ID);
			op->setSrc2RegID(ctx.Reg3ID);
			op->setUsesImmediate(false);
			op->InitializeMetadata();
			return op;
		});
		std::shared_ptr<MicroOpDescriptor> temp_var4 = std::make_shared<MicroOpDescriptor>();
		temp_var4->setLatency(1);
		temp_var4->setMemFootprintClass(0);
		temp_var4->setWritesRegister(true);
		temp_var4->setMemoryOp(false);
		RegisterOpAttributes(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::OR), temp_var4);

		RegisterSemanticFactory(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::AND), [=] (DecoderContext ctx)
		{
			std::shared_ptr<ScalarALUMicroOp> op = std::make_shared<ScalarALUMicroOp>();
			op->setOpCode(ctx.OpCode);
			op->setDestRegID(ctx.Reg1ID);
			op->setSrc1RegID(ctx.Reg2ID);
			op->setSrc2RegID(ctx.Reg3ID);
			op->setUsesImmediate(false);
			op->InitializeMetadata();
			return op;
		});
		std::shared_ptr<MicroOpDescriptor> temp_var5 = std::make_shared<MicroOpDescriptor>();
		temp_var5->setLatency(1);
		temp_var5->setMemFootprintClass(0);
		temp_var5->setWritesRegister(true);
		temp_var5->setMemoryOp(false);
		RegisterOpAttributes(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::AND), temp_var5);

		RegisterScalarRegisterOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::CZERO_EQZ));
		RegisterScalarUnaryRegisterOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::CLZ));
		RegisterScalarUnaryRegisterOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::CTZ));
		RegisterScalarUnaryRegisterOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::CPOP));
		RegisterScalarUnaryRegisterOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::SEXT_B));
		RegisterScalarUnaryRegisterOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::SEXT_H));
		RegisterScalarUnaryRegisterOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::ZEXT_H));
		RegisterScalarRotateRegisterOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::ROL));
		RegisterScalarRotateRegisterOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::ROR));
		RegisterScalarRotateImmediateOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::ROLI));
		RegisterScalarRotateImmediateOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::RORI));
		RegisterScalarBitfieldRegisterOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::BSET));
		RegisterScalarBitfieldRegisterOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::BCLR));
		RegisterScalarBitfieldRegisterOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::BINV));
		RegisterScalarBitfieldRegisterOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::BEXT));
		RegisterScalarBitfieldImmediateOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::BSETI));
		RegisterScalarBitfieldImmediateOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::BCLRI));
		RegisterScalarBitfieldImmediateOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::BINVI));
		RegisterScalarBitfieldImmediateOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::BEXTI));
		RegisterScalarBooleanInvertRegisterOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::ANDN));
		RegisterScalarBooleanInvertRegisterOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::ORN));
		RegisterScalarBooleanInvertRegisterOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::XNOR));
		RegisterScalarMinMaxRegisterOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::MIN));
		RegisterScalarMinMaxRegisterOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::MAX));
		RegisterScalarMinMaxRegisterOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::MINU));
		RegisterScalarMinMaxRegisterOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::MAXU));
		RegisterScalarUnaryRegisterOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::REV8));
		RegisterScalarUnaryRegisterOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::BREV8));
		RegisterScalarZeroingSelectRegisterOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::CZERO_NEZ));
		RegisterScalarAddressGenerationRegisterOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::SH1ADD));
		RegisterScalarAddressGenerationRegisterOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::SH2ADD));
		RegisterScalarAddressGenerationRegisterOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::SH3ADD));
		RegisterScalarAddressGenerationRegisterOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::ADD_UW));
		RegisterScalarAddressGenerationRegisterOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::SH1ADD_UW));
		RegisterScalarAddressGenerationRegisterOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::SH2ADD_UW));
		RegisterScalarAddressGenerationRegisterOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::SH3ADD_UW));
		RegisterScalarAddressGenerationImmediateOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::SLLI_UW));
		RegisterScalarCarryLessRegisterOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::CLMUL));
		RegisterScalarCarryLessRegisterOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::CLMULH));
		RegisterScalarCarryLessRegisterOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::CLMULR));

	}

	void InstructionRegistry::RegisterVectorInstructions()
	{
		// ========== Vector Operations ==========
		RegisterVectorConfigOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VSETVL));
		RegisterVectorConfigOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VSETVLI));
		RegisterVectorConfigOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VSETIVLI));

		RegisterPublishedVectorBinaryOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VADD));
		RegisterPublishedVectorBinaryOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VSUB));
		RegisterPublishedVectorBinaryOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VMUL));
		RegisterPublishedVectorBinaryOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VDIV));
		RegisterPublishedVectorBinaryOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VMOD));

		// Legacy vector transfer opcodes still execute through StreamEngine but must now
		// materialize a concrete MicroOp so the live WB retire path keeps explicit authority.
		RegisterVectorTransferOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VLOAD), 4);
		RegisterVectorTransferOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VSTORE), 4);

		RegisterPublishedVectorBinaryOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VXOR));
		RegisterPublishedVectorBinaryOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VOR));
		RegisterPublishedVectorBinaryOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VAND));
		RegisterPublishedVectorBinaryOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VSLL));
		RegisterPublishedVectorBinaryOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VSRL));
		RegisterPublishedVectorBinaryOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VSRA));
		RegisterPublishedVectorBinaryOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VMIN));
		RegisterPublishedVectorBinaryOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VMAX));
		RegisterPublishedVectorBinaryOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VMINU));
		RegisterPublishedVectorBinaryOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VMAXU));

		RegisterPublishedVectorUnaryOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VSQRT));
		RegisterPublishedVectorUnaryOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VNOT));
		RegisterPublishedVectorUnaryOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VPOPCNT));
		RegisterPublishedVectorUnaryOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VCLZ));
		RegisterPublishedVectorUnaryOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VCTZ));
		RegisterPublishedVectorUnaryOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VBREV8));
		RegisterPublishedVectorUnaryOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VREVERSE));

		RegisterPublishedVectorComparisonOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VCMPEQ));
		RegisterPublishedVectorComparisonOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VCMPNE));
		RegisterPublishedVectorComparisonOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VCMPLT));
		RegisterPublishedVectorComparisonOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VCMPLE));
		RegisterPublishedVectorComparisonOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VCMPGT));
		RegisterPublishedVectorComparisonOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VCMPGE));

		RegisterPublishedVectorFmaOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VFMADD));
		RegisterPublishedVectorFmaOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VFMSUB));
		RegisterPublishedVectorFmaOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VFNMADD));
		RegisterPublishedVectorFmaOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VFNMSUB));

		RegisterPublishedVectorReductionOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VREDSUM));
		RegisterPublishedVectorReductionOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VREDMAX));
		RegisterPublishedVectorReductionOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VREDMIN));
		RegisterPublishedVectorReductionOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VREDMAXU));
		RegisterPublishedVectorReductionOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VREDMINU));
		RegisterPublishedVectorReductionOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VREDAND));
		RegisterPublishedVectorReductionOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VREDOR));
		RegisterPublishedVectorReductionOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VREDXOR));

		RegisterPublishedVectorDotProductOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VDOT));
		RegisterPublishedVectorDotProductOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VDOTU));
		RegisterPublishedVectorDotProductOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VDOTF));
		RegisterPublishedVectorDotProductOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VDOT_FP8));
		RegisterPublishedVectorDotProductWideOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VDOT_WIDE));

		RegisterPublishedVectorPermutationOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VPERMUTE));
		RegisterPublishedVectorPerm2Op(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VPERM2));
		RegisterPublishedVectorTransposeOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VTRANSPOSE));
		RegisterPublishedVectorPermutationOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VRGATHER));
		RegisterPublishedVectorGatherOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VGATHER));
		RegisterPublishedVectorScatterOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VSCATTER));
		RegisterPublishedVectorSlideOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VSLIDEUP));
		RegisterPublishedVectorSlideOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VSLIDEDOWN));
		RegisterPublishedVectorSlideOneUpOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VSLIDE1UP));
		RegisterPublishedVectorSlideOneDownOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VSLIDE1DOWN));
		RegisterPublishedVectorMaskOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VMAND));
		RegisterPublishedVectorMaskOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VMOR));
		RegisterPublishedVectorMaskOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VMXOR));
		RegisterPublishedVectorMaskOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VMNOT));
		RegisterPublishedVectorMaskOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VMSBF));
		RegisterPublishedVectorMaskPopCountOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VPOPC));
		RegisterPublishedVectorZeroExtendOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VZEXT));
		RegisterPublishedVectorScanSumOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VSCAN_SUM));

		RegisterPublishedVectorPredicativeMovementOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VCOMPRESS));
		RegisterPublishedVectorPredicativeMovementOp(static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VEXPAND));
	}

	void InstructionRegistry::RegisterSemanticFactory(unsigned int opCode, MicroOpFactory factory)
	{
		_factories[opCode] = factory;
		if (!_descriptors.contains(opCode))
		{
			_descriptors[opCode] = std::make_shared<MicroOpDescriptor>();
		}
	}

	void InstructionRegistry::RegisterOpAttributes(unsigned int opCode, std::shared_ptr<MicroOpDescriptor> descriptor)
	{
		_descriptors[opCode] = descriptor;
	}

	std::shared_ptr<MicroOpDescriptor> InstructionRegistry::GetDescriptor(unsigned int opCode)
	{
		if (!_initialized)
		{
			Initialize();
		}

		auto descriptorsIterator = _descriptors.find(opCode);
		return descriptorsIterator != _descriptors.end() ? descriptorsIterator->second : nullptr;
	}

	std::shared_ptr<MicroOp> InstructionRegistry::CreateMicroOp(unsigned int opCode, DecoderContext context)
	{
		if (!_initialized)
		{
			Initialize();
		}

		if (IsCustomAcceleratorOpcode(opCode))
		{
			throw CreateUnsupportedCustomAcceleratorException(opCode);
		}

		MicroOpFactory factory;
		std::unordered_map<unsigned int, MicroOpFactory>::const_iterator _factories_iterator = _factories.find(opCode);
		if (_factories_iterator != _factories.end())
		{
			factory = _factories_iterator->second;
			std::shared_ptr<MicroOp> microOp = factory(context);

			// Apply descriptor flag overrides (Blueprint §2/§4: factory creates the MicroOp;
			// the descriptor provides semantic flags so factories need not set them directly).
			auto descriptorsIterator = _descriptors.find(opCode);
			if (microOp != nullptr && descriptorsIterator != _descriptors.end())
			{
				std::shared_ptr<MicroOpDescriptor> desc = descriptorsIterator->second;
				bool prevWrites = microOp->getWritesRegister();
				if (desc->getWritesRegister())
				{
					bool writesRegister = desc->getWritesRegister().value();
					if (writesRegister && std::dynamic_pointer_cast<ScalarALUMicroOp>(microOp) != nullptr && microOp->getDestRegID() == VLIW_Instruction::NoReg)
					{
						// Scalar descriptors publish the opcode-level capability to write.
						// The decoded instruction instance remains non-writing when its exact
						// canonical destination is absent. Promoting this contour would invent
						// a register effect that is absent from the canonical IR and leave the
						// authoritative WriteRegisters set empty.
						writesRegister = false;
					}

					microOp->setWritesRegister(writesRegister);
				}

				if (desc->isMemoryOp())
				{
					microOp->setMemoryOp(desc->isMemoryOp().value());
				}

				// Blueprint Phase 4 fix: if the descriptor promoted WritesRegister from
				// false to true, the factory may have already called InitializeMetadata()
				// while WritesRegister was still false — leaving WriteRegisters empty and
				// ResourceMask without the write-resource bits. Allow the MicroOp to
				// re-synchronise its write metadata now that the flag is correct.
				if (!prevWrites && microOp->getWritesRegister())
				{
					microOp->RefreshWriteMetadata();
				}
			}
			if (microOp != nullptr)
			{
				microOp->ValidatePublishedWriteRegisterContract(L"InstructionRegistry.CreateMicroOp");
			}

			return microOp;
		}
		// Unknown opcode: always throw IllegalInstruction — no NOP fallback (v6 checklist A3).
		throw InvalidOperationException(std::format(L"Unsupported instruction opcode: 0x{0:0>X} ({1:s})", opCode, OpcodeRegistry::GetMnemonicOrHex(opCode)));
	}

	bool InstructionRegistry::IsRegistered(unsigned int opCode)
	{
		if (!_initialized)
		{
			Initialize();
		}

		return _factories.contains(opCode);
	}

	unsigned long long InstructionRegistry::getResourceMaskStructuralFallbackCount()
	{
		return static_cast<unsigned long long>(_resourceMaskStructuralFallbackCount.load(std::memory_order_relaxed));
	}

	unsigned long long InstructionRegistry::getPlacementStructuralFallbackCount()
	{
		return static_cast<unsigned long long>(_placementStructuralFallbackCount.load(std::memory_order_relaxed));
	}

	void InstructionRegistry::ResetSafetyFallbackTelemetryForTesting()
	{
		_resourceMaskStructuralFallbackCount.store(0, std::memory_order_relaxed);
		_placementStructuralFallbackCount.store(0, std::memory_order_relaxed);
	}

	SafetyMask128 InstructionRegistry::BuildExplicitStructuralSafetyMask(std::shared_ptr<MicroOp> microOp)
	{
		ArgumentNullException::ThrowIfNull(microOp);

		SafetyMask128 explicitMask = SafetyMask128(microOp->getResourceMask().Low, microOp->getResourceMask().High);

		if (microOp->getPlacement().RequiredSlotClass == SlotClass::BranchControl ||
			microOp->getPlacement().RequiredSlotClass == SlotClass::SystemSingleton)
		{
			explicitMask = explicitMask | SafetyMask128(0ULL, StructuralFallbackLane7SingletonBitHigh);
		}
		else if ((microOp->getPlacement().RequiredSlotClass == SlotClass::DmaStreamClass ||
			microOp->getPlacement().RequiredSlotClass == SlotClass::MatrixTileStreamClass) && explicitMask.isZero())
		{
			explicitMask = explicitMask | SafetyMask128(0ULL, StructuralFallbackDmaStreamLaneBitHigh);
		}

		if (explicitMask.isZero() && (microOp->getSerializationClass() != SerializationClass::Free || microOp->isControlFlow() || microOp->getHasSideEffects()))
		{
			explicitMask = explicitMask | SafetyMask128(0ULL, StructuralFallbackSerializedOpBitHigh);
		}

		return explicitMask;
	}

	bool InstructionRegistry::RequiresExplicitStructuralSafetyMask(std::shared_ptr<MicroOp> microOp)
	{
		ArgumentNullException::ThrowIfNull(microOp);

		if (microOp->getCanonicalDecodePublication() == CanonicalDecodePublicationMode::Unspecified || std::dynamic_pointer_cast<NopMicroOp>(microOp) != nullptr)
		{
			return false;
		}

		bool hasMemoryVisibility = !microOp->getReadMemoryRanges().empty() || !microOp->getWriteMemoryRanges().empty();

		const auto slotClass = microOp->getPlacement().RequiredSlotClass;
		return microOp->isMemoryOp() || hasMemoryVisibility || microOp->isControlFlow() || microOp->getHasSideEffects() ||
			microOp->getSerializationClass() != SerializationClass::Free ||
			slotClass == SlotClass::BranchControl || slotClass == SlotClass::SystemSingleton ||
			slotClass == SlotClass::DmaStreamClass || slotClass == SlotClass::MatrixTileStreamClass;
	}

	std::shared_ptr<InvalidOperationException> InstructionRegistry::CreateMissingExplicitStructuralSafetyMaskException(std::shared_ptr<MicroOp> microOp, const std::wstring &surface)
	{
		ArgumentNullException::ThrowIfNull(microOp);
		if (surface.empty() || std::all_of(surface.begin(), surface.end(), [](wchar_t ch) { return std::iswspace(ch) != 0; }))
		{
			throw std::invalid_argument("surface must not be null, empty, or whitespace");
		}

		const auto typeName = typeid(*microOp).name();
		const std::wstring wideTypeName(typeName, typeName + std::char_traits<char>::length(typeName));
		return std::make_shared<InvalidOperationException>(std::format(L"{0:s} observed {1:s} (opcode 0x{2:0>X}) without an explicit structural safety mask on a canonical non-NOP contour. ", surface, wideTypeName, microOp->getOpCode()) + L"Micro-ops with memory visibility, control flow, side effects, or non-free serialization must publish explicit structural safety truth instead of relying on hidden resource/placement fallback synthesis.");
	}

	std::shared_ptr<SysEventMicroOp> InstructionRegistry::CreateSystemEventMicroOp(unsigned int opcode, std::function<std::shared_ptr<SysEventMicroOp>()> factory)
	{
		ArgumentNullException::ThrowIfNull(factory);

		std::shared_ptr<SysEventMicroOp> microOp = factory();
		microOp->setOpCode(opcode);
		microOp->InitializeMetadata();
		return microOp;
	}

	std::shared_ptr<SysEventMicroOp> InstructionRegistry::CreateFenceSystemEventMicroOp(DecoderContext context, std::function<std::shared_ptr<SysEventMicroOp>()> factory)
	{
		ValidateCanonicalFenceDecoderContext(context);
		return CreateSystemEventMicroOp(context.OpCode, factory);
	}

	void InstructionRegistry::ValidateCanonicalFenceDecoderContext(DecoderContext context)
	{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: ushort opcode = unchecked((ushort)context.OpCode);
		unsigned short opcode = static_cast<unsigned short>(context.OpCode);
		bool hasUnsupportedPayload = context.Immediate != 0 || context.PredicateMask != 0 || context.AcquireOrdering || context.ReleaseOrdering || context.IndexedAddressing || context.Is2DAddressing || context.VectorPrimaryPointer != 0 || context.VectorSecondaryPointer != 0 || context.VectorStreamLength != 0 || context.VectorStride != 0 || context.VectorRowStride != 0 || context.TailAgnostic || context.MaskAgnostic || context.Saturating || context.HasVectorPayload;

		if (!hasUnsupportedPayload)
		{
			return;
		}

		throw DecodeProjectionFaultException(std::format(L"Opcode {0:s} reached InstructionRegistry.CreateMicroOp(...) with unsupported Phase 10 fence payload. ", OpcodeRegistry::GetMnemonicOrHex(opcode)) + L"FENCE/FENCE.I materialization accepts only the canonical zero-payload form; " + L"decoder context fields and aq/rl bits are evidence only, not ordering authority.");
	}

	SafetyMask128 InstructionRegistry::ComputeSafetyMask(unsigned int opCode, std::shared_ptr<MicroOp> microOp, int memoryDomainId)
	{
		if (!_initialized)
		{
			Initialize();
		}

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: IReadOnlyList<(ulong Address, ulong Length)> readMemoryRanges = microOp.ReadMemoryRanges ?? Array.Empty<(ulong Address, ulong Length)>();
		std::vector<std::pair<unsigned long long, unsigned long long>> readMemoryRanges;
		readMemoryRanges.reserve(microOp->getReadMemoryRanges().size());
		for (const auto &range : microOp->getReadMemoryRanges())
		{
			readMemoryRanges.emplace_back(range.Address, range.Length);
		}
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: IReadOnlyList<(ulong Address, ulong Length)> normalizedReadMemoryRanges = readMemoryRanges;
		std::vector<std::pair<unsigned long long, unsigned long long>> normalizedReadMemoryRanges = readMemoryRanges;
		std::vector<std::pair<unsigned long long, unsigned long long>> normalizedRanges;
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: if (ReadRangeMetadataHelper.TryNormalizeContiguousReadRanges(readMemoryRanges, out IReadOnlyList<(ulong Address, ulong Length)> normalizedRanges))
		if (ReadRangeMetadataHelper::TryNormalizeContiguousReadRanges(readMemoryRanges, normalizedRanges))
		{
			normalizedReadMemoryRanges = normalizedRanges;
		}

		std::shared_ptr<AssistCoalescingDescriptor> assistCoalescingDescriptor = ReadRangeMetadataHelper::BuildAssistCoalescingDescriptor(readMemoryRanges, normalizedReadMemoryRanges);
		ReadRangeMetadataHelper::ValidateCoalescedRangeMetadata(readMemoryRanges, normalizedReadMemoryRanges, assistCoalescingDescriptor);

		std::vector<std::pair<unsigned long long, unsigned long long>> writeMemoryRanges;
		writeMemoryRanges.reserve(microOp->getWriteMemoryRanges().size());
		for (const auto &range : microOp->getWriteMemoryRanges())
		{
			writeMemoryRanges.emplace_back(range.Address, range.Length);
		}

		SafetyMaskContext context = {.ReadRegisters = microOp->getReadRegisters(), .WriteRegisters = microOp->getWriteRegisters(), .ReadMemoryRanges = readMemoryRanges, .NormalizedReadMemoryRanges = normalizedReadMemoryRanges, .WriteMemoryRanges = writeMemoryRanges, .AssistCoalescingDescriptor = assistCoalescingDescriptor, .MemoryDomainId = memoryDomainId, .IsMemoryOp = microOp->isMemoryOp(), .IsLoad = std::dynamic_pointer_cast<LoadMicroOp>(microOp) != nullptr, .IsStore = std::dynamic_pointer_cast<StoreMicroOp>(microOp) != nullptr, .IsAtomic = std::dynamic_pointer_cast<AtomicMicroOp>(microOp) != nullptr || microOp->getInstructionClass() == InstructionClass::Atomic, .VirtualThreadId = microOp->getVirtualThreadId()};

		SafetyMask128 computedMask = DefaultSafetyMask128Generator(context);

		return ApplyStructuralFallbackSafetyMask(computedMask, microOp);
	}

	SafetyMask128 InstructionRegistry::DefaultSafetyMask128Generator(SafetyMaskContext context)
	{
		unsigned long long low = 0;

		if (!context.ReadRegisters.empty())
		{
			for (auto reg : context.ReadRegisters)
			{
				int group = reg / 4;
				if (group < 16)
				{
					low |= 1ULL << group;
				}
			}
		}

		if (!context.WriteRegisters.empty())
		{
			for (auto reg : context.WriteRegisters)
			{
				int group = reg / 4;
				if (group < 16)
				{
					low |= 1ULL << (16 + group);
				}
			}
		}

		if (context.IsMemoryOp && context.MemoryDomainId >= 0 && context.MemoryDomainId < 16)
		{
			low |= 1ULL << (32 + context.MemoryDomainId);
		}

		if (context.IsLoad)
		{
			low |= 1ULL << 48;
		}

		if (context.IsStore)
		{
			low |= 1ULL << 49;
		}

		if (context.IsAtomic)
		{
			low |= 1ULL << 50;
		}

		return SafetyMask128(low, 0ULL);
	}

	SafetyMask128 InstructionRegistry::ApplyStructuralFallbackSafetyMask(SafetyMask128 computedMask, std::shared_ptr<MicroOp> microOp)
	{
		if (computedMask.isNonZero())
		{
			return computedMask;
		}

		if (!microOp->allowsStructuralSafetyFallback())
		{
			if (RequiresExplicitStructuralSafetyMask(microOp))
			{
				throw CreateMissingExplicitStructuralSafetyMaskException(microOp, L"InstructionRegistry.ComputeSafetyMask()");
			}

			return SafetyMask128::Zero;
		}

		SafetyMask128 structuralMask = SafetyMask128(microOp->getResourceMask().Low, microOp->getResourceMask().High);
		if (structuralMask.isNonZero())
		{
			_resourceMaskStructuralFallbackCount.fetch_add(1, std::memory_order_relaxed);
			return structuralMask;
		}

		SafetyMask128 placementFallbackMask = BuildPlacementBackedStructuralSafetyMask(microOp->getPlacement(), microOp->getSerializationClass(), microOp->isControlFlow(), microOp->getHasSideEffects());

		if (placementFallbackMask.isNonZero())
		{
			_placementStructuralFallbackCount.fetch_add(1, std::memory_order_relaxed);
		}

		return placementFallbackMask;
	}

	SafetyMask128 InstructionRegistry::BuildPlacementBackedStructuralSafetyMask(SlotPlacementMetadata placement, SerializationClass serializationClass, bool isControlFlow, bool hasSideEffects)
	{
		switch (placement.RequiredSlotClass)
		{
			case SlotClass::BranchControl:
			case SlotClass::SystemSingleton:
				return SafetyMask128(0ULL, StructuralFallbackLane7SingletonBitHigh);
			case SlotClass::DmaStreamClass:
			case SlotClass::MatrixTileStreamClass:
				return SafetyMask128(0ULL, StructuralFallbackDmaStreamLaneBitHigh);
			default:
				if (serializationClass != SerializationClass::Free || isControlFlow || hasSideEffects)
				{
					return SafetyMask128(0ULL, StructuralFallbackSerializedOpBitHigh);
				}
				return SafetyMask128::Zero;
		}
	}
}
