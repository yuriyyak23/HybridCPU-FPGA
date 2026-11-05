#include "VectorMicroOps.Scan.h"
#include "../../../State/ExecutionState.h"
#include "../../../../../Arch/DataType.h"
#include "../../../Execution/Vector/ALU/YAKSys_Hybrid_CPU.Execution.VectorALU.h"
#include "../../../Diagnostics/DecodeProjectionFaultException.h"

using namespace HybridCPU_ISE::Arch;
using namespace YAKSys_Hybrid_CPU::Arch;
using namespace YAKSys_Hybrid_CPU::Core::Registers::Retire;
using namespace YAKSys_Hybrid_CPU::Execution;

namespace YAKSys_Hybrid_CPU::Core
{

	void VectorScanSumMicroOp::InitializeMetadata()
	{
		VectorMicroOp::InitializeMetadata();
		setMemoryOp(false);
		ThrowIfUnsupportedPublicationContour();

		if (getInstruction().getStreamLength() > 0)
		{
			int elementSize = GetElementSize();
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'checked' in this context:
// ORIGINAL LINE: ulong totalSize = checked(Instruction.StreamLength * (ulong)elementSize);
			if (getInstruction().getStreamLength() > std::numeric_limits<unsigned long long>::max() / static_cast<unsigned long long>(elementSize))
			{
				throw std::overflow_error("VectorScanSumMicroOp memory range size overflowed.");
			}
			unsigned long long totalSize = getInstruction().getStreamLength() * static_cast<unsigned long long>(elementSize);

			setReadMemoryRanges({MemoryRange{getInstruction().getSrc2Pointer(), totalSize}});
			setWriteMemoryRanges({MemoryRange{getInstruction().getDestSrc1Pointer(), totalSize}});
		}

		RefreshVectorAdmissionMetadata(true, true);
	}

	bool VectorScanSumMicroOp::Execute(std::shared_ptr<Processor::CPU_Core> &core)
	{
		if (_state == ExecutionState::Complete)
		{
			return true;
		}

		if (_state == ExecutionState::NotStarted)
		{
			_totalElements = getInstruction().getStreamLength();
			ThrowIfUnsupportedRuntimeContour();
			ThrowIfZeroLengthVectorComputeContour(L"VectorScanSumMicroOp.Execute()");

			DataTypeEnum dataType = getInstruction().getDataTypeValue();
			int elementSize = DataTypeUtils::SizeOf(dataType);
			long long signedAccumulator = 0;
			unsigned long long unsignedAccumulator = 0;

// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'checked' in this context:
// ORIGINAL LINE: _stagedWrites = new StagedVectorWrite[checked((int)_totalElements)];
			if (_totalElements > static_cast<unsigned long long>(std::numeric_limits<int>::max()))
			{
				throw std::overflow_error("VectorScanSumMicroOp element count exceeds the supported staged-write capacity.");
			}
			_stagedWrites.clear();
			_stagedWrites.reserve(static_cast<std::size_t>(_totalElements));
			_stagedWriteCount = 0;

			for (unsigned long long element = 0; element < _totalElements; element++)
			{
				if (!IsLaneActiveForScan(core, element))
				{
					continue;
				}

// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'checked' in this context:
// ORIGINAL LINE: ulong sourceAddress = checked(Instruction.Src2Pointer + (element * (ulong)elementSize));
				const unsigned long long elementWidth = static_cast<unsigned long long>(elementSize);
				if (element > std::numeric_limits<unsigned long long>::max() / elementWidth)
				{
					throw std::overflow_error("VectorScanSumMicroOp source offset overflowed.");
				}
				const unsigned long long sourceOffset = element * elementWidth;
				if (getInstruction().getSrc2Pointer() > std::numeric_limits<unsigned long long>::max() - sourceOffset)
				{
					throw std::overflow_error("VectorScanSumMicroOp source address overflowed.");
				}
				unsigned long long sourceAddress = getInstruction().getSrc2Pointer() + sourceOffset;
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'checked' in this context:
// ORIGINAL LINE: ulong destinationAddress = checked(Instruction.DestSrc1Pointer + (element * (ulong)elementSize));
				if (getInstruction().getDestSrc1Pointer() > std::numeric_limits<unsigned long long>::max() - sourceOffset)
				{
					throw std::overflow_error("VectorScanSumMicroOp destination address overflowed.");
				}
				unsigned long long destinationAddress = getInstruction().getDestSrc1Pointer() + sourceOffset;

				std::vector<unsigned char> sourceBuffer(elementSize);
				core->ReadBoundMainMemoryExact(sourceAddress, sourceBuffer, L"VectorScanSumMicroOp.Execute()");

				std::vector<unsigned char> destinationBuffer(elementSize);
				VectorALU::ApplyScanSumElement(getOpCode(), dataType, sourceBuffer, destinationBuffer, signedAccumulator, unsignedAccumulator, core);

				core->ThrowIfBoundMainMemoryRangeUnavailable(destinationAddress, elementSize, L"VectorScanSumMicroOp.Execute()");

				_stagedWrites.emplace_back(element, destinationAddress, destinationBuffer);
				++_stagedWriteCount;
			}

			_state = ExecutionState::Complete;
		}

		return true;
	}

	void VectorScanSumMicroOp::EmitWriteBackRetireRecords(std::shared_ptr<Processor::CPU_Core> &core, std::span<RetireRecord> retireRecords, int &retireRecordCount)
	{
		PublishStagedWritesAtWriteBack(core);
	}

	void VectorScanSumMicroOp::PublishStagedWritesAtWriteBack(std::shared_ptr<Processor::CPU_Core> &core)
	{
		if (_state != ExecutionState::Complete || _stagedWriteCount < 0 ||
			static_cast<std::size_t>(_stagedWriteCount) != _stagedWrites.size())
		{
			throw InvalidOperationException(L"VectorScanSumMicroOp WB publication reached retire without a completed staged VSCAN.SUM result.");
		}

		for (int i = 0; i < _stagedWriteCount; i++)
		{
			StagedVectorWrite write = _stagedWrites[i];
			core->ThrowIfBoundMainMemoryRangeUnavailable(write.getAddress(), write.getData().size(), L"VectorScanSumMicroOp.EmitWriteBackRetireRecords()");
		}

		for (int i = 0; i < _stagedWriteCount; i++)
		{
			StagedVectorWrite write = _stagedWrites[i];
			std::vector<unsigned char> data = write.getData();
			core->WriteBoundMainMemoryExact(write.getAddress(), data, L"VectorScanSumMicroOp.EmitWriteBackRetireRecords()");
		}
	}

	void VectorScanSumMicroOp::ThrowIfUnsupportedPublicationContour()
	{
		if (getOpCode() != static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VSCAN_SUM) || getInstruction().getOpCode() != static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VSCAN_SUM))
		{
			throw DecodeProjectionFaultException(L"VectorScanSumMicroOp.InitializeMetadata() can only publish the Phase 05A VSCAN.SUM contour.");
		}

		if (getInstruction().getIndexed() || getInstruction().getIs2D())
		{
			std::wstring addressingContour = getInstruction().getIndexed() && getInstruction().getIs2D() ? L"indexed+2D" : getInstruction().getIndexed() ? L"indexed" : L"2D";
			throw DecodeProjectionFaultException(std::format(L"VectorScanSumMicroOp.InitializeMetadata() rejected unsupported {} VSCAN.SUM publication. ", addressingContour) + L"Phase 05A only opens the packed 1D integer prefix-sum contour.");
		}

		if (getInstruction().getImmediate() != 0)
		{
			throw DecodeProjectionFaultException(L"VectorScanSumMicroOp.InitializeMetadata() rejected non-zero VSCAN.SUM immediate sideband.");
		}

		if (getInstruction().getStride() != 0)
		{
			throw DecodeProjectionFaultException(L"VectorScanSumMicroOp.InitializeMetadata() rejected non-zero VSCAN.SUM stride. "
				L"The selected Phase 05A contour is packed 1D only.");
		}

		ThrowIfUnsupportedScanDataType(getInstruction().getDataTypeValue(), L"VectorScanSumMicroOp.InitializeMetadata()");
	}

	void VectorScanSumMicroOp::ThrowIfUnsupportedRuntimeContour()
	{
		if (!getInstruction().getIndexed() && !getInstruction().getIs2D() && getInstruction().getImmediate() == 0 && getInstruction().getStride() == 0)
		{
			ThrowIfUnsupportedScanDataType(getInstruction().getDataTypeValue(), L"VectorScanSumMicroOp.Execute()");
			return;
		}

		throw InvalidOperationException(L"VectorScanSumMicroOp.Execute() rejected a non-canonical VSCAN.SUM runtime contour. "
			L"Only packed 1D integer prefix-sum semantics are executable.");
	}

	void VectorScanSumMicroOp::ThrowIfUnsupportedScanDataType(DataTypeEnum dataType, const std::wstring &surface)
	{
		if (DataTypeUtils::IsInteger(dataType))
		{
			return;
		}

		throw DecodeProjectionFaultException(std::format(L"{} rejected unsupported VSCAN.SUM DataType {}. ", surface, static_cast<unsigned int>(dataType)) + L"Phase 05A publishes only integer prefix-sum semantics.");
	}

	bool VectorScanSumMicroOp::IsLaneActiveForScan(std::shared_ptr<Processor::CPU_Core> &core, unsigned long long element)
	{
		if (element > std::numeric_limits<int>::max())
		{
			return false;
		}

		return core->LaneActive(getInstruction().getPredicateMask(), static_cast<int>(element));
	}

	std::vector<VectorScanSumMicroOp::StagedVectorWrite> VectorScanSumMicroOp::GetStagedWrites()
	{
		if (_stagedWrites.empty())
		{
			return {};
		}

		if (_stagedWriteCount < 0 || static_cast<size_t>(_stagedWriteCount) > _stagedWrites.size())
		{
			throw InvalidOperationException(L"VectorScanSumMicroOp staged-write count is outside its owned buffer.");
		}

		return std::vector<StagedVectorWrite>(_stagedWrites.begin(), _stagedWrites.begin() + _stagedWriteCount);
	}

	VectorScanSumMicroOp::StagedVectorWrite::StagedVectorWrite(unsigned long long element, unsigned long long address, std::vector<unsigned char> &data)
		: Element(element), Address(address), Data(data)
	{
	}

	unsigned long long VectorScanSumMicroOp::StagedVectorWrite::getElement() const
	{
		return Element;
	}

	unsigned long long VectorScanSumMicroOp::StagedVectorWrite::getAddress() const
	{
		return Address;
	}

	std::vector<unsigned char> VectorScanSumMicroOp::StagedVectorWrite::getData() const
	{
		return Data;
	}
}
