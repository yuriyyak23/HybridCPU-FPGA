#include "VectorMicroOps.h"
#include "../../../Architecture/Common/StreamEngineId.h"
#include "../../../../../NonRTL/Arch/InstructionClass.h"
#include "../../../../../NonRTL/Arch/SerializationClass.h"
#include "../../Scheduling/SlotLegality/SlotClassDefinitions.h"
#include "../../Safety/ResourceBitset.h"
#include "../../Certificates/SafetyMask128.h"
#include "../../../../../Arch/DataType.h"
#include "../../../Execution/ExecutionFaultContract.h"
#include "../../../Diagnostics/DecodeProjectionFaultException.h"

// Windows headers define FormatMessage as an object-like alias to FormatMessageW,
// which would rename the ExecutionFaultContract helper used below.
#ifdef FormatMessage
#undef FormatMessage
#endif

using namespace HybridCPU_ISE::Arch;
using namespace YAKSys_Hybrid_CPU::Arch;
using namespace YAKSys_Hybrid_CPU::Core::Registers;
using namespace YAKSys_Hybrid_CPU::Execution;
using AddressGen = YAKSys_Hybrid_CPU::Execution::AddressGen;
using PortType = YAKSys_Hybrid_CPU::Execution::PortType;

namespace YAKSys_Hybrid_CPU::Core
{

	VLIW_Instruction VectorMicroOp::getInstruction() const
	{
		return Instruction;
	}

	void VectorMicroOp::setInstruction(VLIW_Instruction value)
	{
		Instruction = value;
	}

	CanonicalDecodePublicationMode VectorMicroOp::getCanonicalDecodePublication() const
	{
		return CanonicalDecodePublicationMode::SelfPublishes;
	}

	VectorMicroOp::VectorMicroOp()
	{
		// FSP Metadata: Vector operations can be stolen
		setMemoryOp(true);

		// ISA v4 Phase 02: vector ops are ScalarAlu class (extension slot, not mandatory core)
		setInstructionClass(Arch::InstructionClass::ScalarAlu);
		setSerializationClass(Arch::SerializationClass::Free);
		// Phase 01: Typed-slot taxonomy - vector ops routed through ALU lanes
		SetClassFlexiblePlacement(SlotClass::AluClass);
	}

	void VectorMicroOp::InitializeMetadata()
	{
		// Vector operations don't use scalar registers directly
		setReadRegisters({});
		setWriteRegisters({});

		// Memory ranges will be determined during execution based on:
		// - Addressing mode (1D, 2D, indexed)
		// - Stream length
		// - Element size
		// - Stride
		// For now, leave empty - will be populated by derived classes or during Execute
		setReadMemoryRanges({});
		setWriteMemoryRanges({});
		setResourceMask(ResourceBitset::Zero);
		setSafetyMask(SafetyMask128::Zero);
	}

	void VectorMicroOp::RefreshVectorAdmissionMetadata(bool readsMemory, bool writesMemory)
	{
		setResourceMask(ResourceBitset::Zero);

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: IReadOnlyList<int> readRegisters = ReadRegisters ?? Array.Empty<int>();
		const std::vector<int> readRegisters = getReadRegisters();
		for (int i = 0; i < readRegisters.size(); i++)
		{
			int rawRegister = readRegisters[i];
			ArchRegId register_Keyword;
			setResourceMask(getResourceMask() | (ArchRegId::TryCreate(rawRegister, register_Keyword) ? ResourceMaskBuilder::ForArchitecturalRegisterRead(std::make_shared<ArchRegId>(register_Keyword)) : ResourceMaskBuilder::ForRegisterRead(rawRegister)));
		}

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: IReadOnlyList<int> writeRegisters = WriteRegisters ?? Array.Empty<int>();
		const std::vector<int> writeRegisters = getWriteRegisters();
		for (int i = 0; i < writeRegisters.size(); i++)
		{
			int rawRegister = writeRegisters[i];
			ArchRegId register_Keyword;
			setResourceMask(getResourceMask() | (ArchRegId::TryCreate(rawRegister, register_Keyword) ? ResourceMaskBuilder::ForArchitecturalRegisterWrite(std::make_shared<ArchRegId>(register_Keyword)) : ResourceMaskBuilder::ForRegisterWrite(rawRegister)));
		}

		if (readsMemory || writesMemory)
		{
			std::shared_ptr<StreamEngineId> streamEngine = std::make_shared<StreamEngineId>(StreamEngineId::Zero);
			setResourceMask(getResourceMask() | ResourceMaskBuilder::ForStreamEngine(streamEngine));
		}

		if (readsMemory)
		{
			setResourceMask(getResourceMask() | ResourceMaskBuilder::ForLoad());
		}

		if (writesMemory)
		{
			setResourceMask(getResourceMask() | ResourceMaskBuilder::ForStore());
		}

		PublishExplicitStructuralSafetyMask();
		RefreshAdmissionMetadata(shared_from_this());
	}

	int VectorMicroOp::GetElementSize()
	{
		if (!DataTypeUtils::IsValid(getInstruction().getDataTypeValue()))
		{
			return 0;
		}
		return DataTypeUtils::SizeOf(getInstruction().getDataTypeValue());
	}

	void VectorMicroOp::ThrowIfUnsupportedElementDataType(const std::wstring &executionSurface)
	{
		if (GetElementSize() != 0)
		{
			return;
		}

		throw ExecutionFaultContract::CreateUnsupportedVectorElementTypeException(std::format(L"{} rejected unsupported element DataType 0x{:02X} on the authoritative mainline vector compute contour instead of hidden success/no-op.", executionSurface, static_cast<unsigned int>(getInstruction().getDataType())));
	}

	void VectorMicroOp::ThrowIfUnsupportedElementDataTypeForMetadata(const std::wstring &metadataSurface)
	{
		if (getInstruction().getStreamLength() == 0 || GetElementSize() != 0)
		{
			return;
		}

		std::shared_ptr<DecodeProjectionFaultException> exception = std::make_shared<DecodeProjectionFaultException>((ExecutionFaultContract::FormatMessage)(ExecutionFaultCategory::UnsupportedVectorElementType, std::format(L"{} rejected unsupported element DataType 0x{:02X} on the authoritative mainline vector publication contour. ", metadataSurface, static_cast<unsigned int>(getInstruction().getDataType())) + L"Materialized vector carriers must fail closed during metadata publication instead of emitting zero-footprint or empty follow-through facts for an invalid element type."));
		ExecutionFaultContract::Stamp(exception, ExecutionFaultCategory::UnsupportedVectorElementType);
		throw *exception;
	}

	void VectorMicroOp::ThrowIfZeroLengthVectorComputeContour(const std::wstring &executionSurface)
	{
		if (getInstruction().getStreamLength() != 0)
		{
			return;
		}

		throw InvalidOperationException(std::format(L"{} rejected StreamLength == 0 on the authoritative mainline vector compute contour instead of hidden success/no-op.", executionSurface));
	}

	unsigned long long VectorMicroOp::GetVL(std::shared_ptr<Processor::CPU_Core> &core)
	{
		return std::min(core->getVectorConfig()->getVL(), Processor::CPU_Core::RVV_Config::VLMAX);
	}

	unsigned long long VectorMicroOp::GetChunkSize(unsigned long long remaining)
	{
		unsigned long long vlmax = Processor::CPU_Core::RVV_Config::VLMAX;
		return std::min(remaining, vlmax);
	}

	std::wstring VectorMicroOp::GetDescription()
	{
		return std::format(L"Vector: OpCode={}, Elements={}, Progress={}", getOpCode(), _totalElements, _elementsProcessed);
	}
}
