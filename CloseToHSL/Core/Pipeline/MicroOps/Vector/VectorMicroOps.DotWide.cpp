#include "VectorMicroOps.DotWide.h"
#include "../../../State/ExecutionState.h"
#include "../../../Execution/Vector/ALU/YAKSys_Hybrid_CPU.Execution.VectorALU.h"
#include "../../../Diagnostics/DecodeProjectionFaultException.h"
#include <limits>

using namespace HybridCPU_ISE::Arch;
using namespace YAKSys_Hybrid_CPU::Arch;
using namespace YAKSys_Hybrid_CPU::Core::Registers::Retire;
using namespace YAKSys_Hybrid_CPU::Execution;

namespace YAKSys_Hybrid_CPU::Core
{

	void VectorDotWideMicroOp::InitializeMetadata()
	{
		VectorMicroOp::InitializeMetadata();
		setMemoryOp(false);
		ThrowIfUnsupportedPublicationContour();

		if (getInstruction().getStreamLength() > 0)
		{
			int sourceElementSize = GetSupportedSourceElementSize(getInstruction().getDataTypeValue(), L"VectorDotWideMicroOp.InitializeMetadata()");
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'checked' in this context:
// ORIGINAL LINE: ulong totalSourceBytes = checked(Instruction.StreamLength * (ulong)sourceElementSize);
			if (getInstruction().getStreamLength() > std::numeric_limits<unsigned long long>::max() / static_cast<unsigned long long>(sourceElementSize))
			{
				throw std::overflow_error("VectorDotWideMicroOp source byte length overflowed.");
			}
			unsigned long long totalSourceBytes = getInstruction().getStreamLength() * static_cast<unsigned long long>(sourceElementSize);

			setReadMemoryRanges({
				MemoryRange{getInstruction().getDestSrc1Pointer(), totalSourceBytes},
				MemoryRange{getInstruction().getSrc2Pointer(), totalSourceBytes}});
			setWriteMemoryRanges({MemoryRange{getInstruction().getDestSrc1Pointer(), static_cast<unsigned long long>(sizeof(float))}});
		}

		RefreshVectorAdmissionMetadata(true, true);
	}

	bool VectorDotWideMicroOp::Execute(std::shared_ptr<Processor::CPU_Core> &core)
	{
		if (_state == ExecutionState::Complete)
		{
			return true;
		}

		if (_state == ExecutionState::NotStarted)
		{
			_totalElements = getInstruction().getStreamLength();
			ThrowIfUnsupportedRuntimeContour();
			ThrowIfZeroLengthVectorComputeContour(L"VectorDotWideMicroOp.Execute()");

			int sourceElementSize = GetSupportedSourceElementSize(getInstruction().getDataTypeValue(), L"VectorDotWideMicroOp.Execute()");
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'checked' in this context:
// ORIGINAL LINE: int totalSourceBytes = checked((int)(_totalElements * (ulong)sourceElementSize));
			if (_totalElements > static_cast<unsigned long long>(std::numeric_limits<int>::max()) / static_cast<unsigned long long>(sourceElementSize))
			{
				throw std::overflow_error("VectorDotWideMicroOp source byte length exceeds supported buffer size.");
			}
			int totalSourceBytes = static_cast<int>(_totalElements * static_cast<unsigned long long>(sourceElementSize));
			std::vector<unsigned char> sourceA(totalSourceBytes);
			std::vector<unsigned char> sourceB(totalSourceBytes);
			std::vector<unsigned char> result(sizeof(float));

			core->ReadBoundMainMemoryExact(getInstruction().getDestSrc1Pointer(), sourceA, L"VectorDotWideMicroOp.Execute()");
			core->ReadBoundMainMemoryExact(getInstruction().getSrc2Pointer(), sourceB, L"VectorDotWideMicroOp.Execute()");
			core->ThrowIfBoundMainMemoryRangeUnavailable(getInstruction().getDestSrc1Pointer(), sizeof(float), L"VectorDotWideMicroOp.Execute()");

			VectorALU::ApplyDotProduct(getOpCode(), getInstruction().getDataTypeValue(), sourceA, sourceB, result, sourceElementSize, _totalElements, getInstruction().getPredicateMask(), core);

			_stagedResult.emplace(getInstruction().getDestSrc1Pointer(), result);
			_state = ExecutionState::Complete;
		}

		return true;
	}

	void VectorDotWideMicroOp::EmitWriteBackRetireRecords(std::shared_ptr<Processor::CPU_Core> &core, std::span<RetireRecord> retireRecords, int &retireRecordCount)
	{
		(void)retireRecords;
		(void)retireRecordCount;
		PublishStagedResultAtWriteBack(core);
	}

	void VectorDotWideMicroOp::PublishStagedResultAtWriteBack(std::shared_ptr<Processor::CPU_Core> &core)
	{
		if (_state != ExecutionState::Complete || !_stagedResult)
		{
			throw InvalidOperationException(L"VectorDotWideMicroOp WB publication reached retire without a completed staged VDOT.WIDE result.");
		}

		StagedScalarWrite write = _stagedResult.value();
		core->ThrowIfBoundMainMemoryRangeUnavailable(write.getAddress(), write.getData().size(), L"VectorDotWideMicroOp.EmitWriteBackRetireRecords()");
		std::vector<unsigned char> data = write.getData();
		core->WriteBoundMainMemoryExact(write.getAddress(), data, L"VectorDotWideMicroOp.EmitWriteBackRetireRecords()");
	}

	void VectorDotWideMicroOp::ThrowIfUnsupportedPublicationContour()
	{
		if (getOpCode() != static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VDOT_WIDE) || getInstruction().getOpCode() != static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VDOT_WIDE))
		{
			throw DecodeProjectionFaultException(L"VectorDotWideMicroOp.InitializeMetadata() can only publish the Phase 09 VDOT.WIDE contour.");
		}

		if (getInstruction().getIndexed() || getInstruction().getIs2D())
		{
			std::wstring addressingContour = getInstruction().getIndexed() && getInstruction().getIs2D() ? L"indexed+2D" : getInstruction().getIndexed() ? L"indexed" : L"2D";
			throw DecodeProjectionFaultException(std::format(L"VectorDotWideMicroOp.InitializeMetadata() rejected unsupported {} VDOT.WIDE publication. ", addressingContour) + L"Phase 09 only opens the packed 1D FP16/BF16/FP8-to-FP32 plus INT8/UINT8-to-32-bit scalar-footprint contours.");
		}

		if (getInstruction().getImmediate() != 0)
		{
			throw DecodeProjectionFaultException(L"VectorDotWideMicroOp.InitializeMetadata() rejected non-zero VDOT.WIDE immediate sideband.");
		}

		if (getInstruction().getStride() != 0)
		{
			throw DecodeProjectionFaultException(L"VectorDotWideMicroOp.InitializeMetadata() rejected non-zero VDOT.WIDE stride. "
				L"The selected Phase 09 contour is packed 1D only.");
		}

		ThrowIfUnsupportedWideDotDataType(getInstruction().getDataTypeValue(), L"VectorDotWideMicroOp.InitializeMetadata()");
		ThrowIfUnsupportedAlignment(L"VectorDotWideMicroOp.InitializeMetadata()");
	}

	void VectorDotWideMicroOp::ThrowIfUnsupportedRuntimeContour()
	{
		if (!getInstruction().getIndexed() && !getInstruction().getIs2D() && getInstruction().getImmediate() == 0 && getInstruction().getStride() == 0)
		{
			ThrowIfUnsupportedWideDotDataType(getInstruction().getDataTypeValue(), L"VectorDotWideMicroOp.Execute()");
			ThrowIfUnsupportedAlignment(L"VectorDotWideMicroOp.Execute()");
			return;
		}

		throw InvalidOperationException(L"VectorDotWideMicroOp.Execute() rejected a non-canonical VDOT.WIDE runtime contour. "
			L"Only packed 1D FP16/BF16/FP8-to-FP32 plus INT8/UINT8-to-32-bit scalar-footprint semantics are executable.");
	}

	void VectorDotWideMicroOp::ThrowIfUnsupportedAlignment(const std::wstring &surface)
	{
		if ((getInstruction().getDestSrc1Pointer() & 0x3ULL) != 0)
		{
			throw DecodeProjectionFaultException(std::format(L"{} rejected unaligned VDOT.WIDE destination/source1 pointer. ", surface) + L"The FP32 scalar result requires 4-byte alignment.");
		}

		if (GetSupportedSourceElementSize(getInstruction().getDataTypeValue(), surface) == 2 && (getInstruction().getSrc2Pointer() & 0x1ULL) != 0)
		{
			throw DecodeProjectionFaultException(std::format(L"{} rejected unaligned VDOT.WIDE source2 pointer. ", surface) + L"FP16/BF16 source elements require 2-byte alignment.");
		}
	}

	void VectorDotWideMicroOp::ThrowIfUnsupportedWideDotDataType(DataTypeEnum dataType, const std::wstring &surface)
	{
		(void)GetSupportedSourceElementSize(dataType, surface);
	}

	int VectorDotWideMicroOp::GetSupportedSourceElementSize(DataTypeEnum dataType, const std::wstring &surface)
	{
		if (dataType == DataTypeEnum::FLOAT16 || dataType == DataTypeEnum::BFLOAT16)
		{
			return 2;
		}

		if (dataType == DataTypeEnum::FLOAT8_E4M3 || dataType == DataTypeEnum::FLOAT8_E5M2 || dataType == DataTypeEnum::INT8 || dataType == DataTypeEnum::UINT8)
		{
			return 1;
		}

		throw DecodeProjectionFaultException(std::format(L"{} rejected unsupported VDOT.WIDE DataType {}. ", surface, static_cast<unsigned int>(dataType)) + L"Phase 09 publishes only FP16/BF16/FP8 and INT8/UINT8 inputs with 32-bit scalar result publication.");
	}

	std::optional<VectorDotWideMicroOp::StagedScalarWrite> VectorDotWideMicroOp::GetStagedResult()
	{
		return _stagedResult;
	}

	VectorDotWideMicroOp::StagedScalarWrite::StagedScalarWrite(
		unsigned long long address,
		const std::vector<unsigned char> &data)
		: Address(address), Data(data)
	{
	}

	unsigned long long VectorDotWideMicroOp::StagedScalarWrite::getAddress() const
	{
		return Address;
	}

	std::vector<unsigned char> VectorDotWideMicroOp::StagedScalarWrite::getData() const
	{
		return Data;
	}
}
