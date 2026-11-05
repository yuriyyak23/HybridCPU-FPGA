#include "VectorMicroOps.SlideOne.h"
#include "../../../State/ExecutionState.h"
#include "../../../Diagnostics/DecodeProjectionFaultException.h"
#include "../../../../../Arch/DataType.h"
#include <limits>
#include <stdexcept>

namespace
{
	unsigned long long CheckedMultiply(unsigned long long left, unsigned long long right)
	{
		if (right != 0 && left > std::numeric_limits<unsigned long long>::max() / right)
			throw std::overflow_error("VSLIDE1 address multiplication overflow.");
		return left * right;
	}

	unsigned long long CheckedAdd(unsigned long long left, unsigned long long right)
	{
		if (right > std::numeric_limits<unsigned long long>::max() - left)
			throw std::overflow_error("VSLIDE1 address addition overflow.");
		return left + right;
	}
}

using namespace HybridCPU_ISE::Arch;
using namespace YAKSys_Hybrid_CPU::Arch;
using namespace YAKSys_Hybrid_CPU::Core::Registers::Retire;

namespace YAKSys_Hybrid_CPU::Core
{

	void VectorSlideOneUpMicroOp::InitializeMetadata()
	{
		VectorMicroOp::InitializeMetadata();
		setMemoryOp(false);
		ThrowIfUnsupportedPublicationContour();

		if (getInstruction().getStreamLength() > 0)
		{
			int elementSize = GetElementSize();
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'checked' in this context:
// ORIGINAL LINE: ulong totalSize = checked(Instruction.StreamLength * (ulong)elementSize);
			unsigned long long totalSize = CheckedMultiply(getInstruction().getStreamLength(), static_cast<unsigned long long>(elementSize));

			setReadMemoryRanges({MemoryRange{getInstruction().getDestSrc1Pointer(), totalSize}});
			setWriteMemoryRanges({MemoryRange{getInstruction().getDestSrc1Pointer(), totalSize}});
		}

		RefreshVectorAdmissionMetadata(true, true);
	}

	bool VectorSlideOneUpMicroOp::Execute(std::shared_ptr<Processor::CPU_Core> &core)
	{
		if (_state == ExecutionState::Complete)
		{
			return true;
		}

		if (_state == ExecutionState::NotStarted)
		{
			_totalElements = getInstruction().getStreamLength();
			ThrowIfUnsupportedRuntimeContour();
			ThrowIfZeroLengthVectorComputeContour(L"VectorSlideOneUpMicroOp.Execute()");

			int elementSize = GetElementSize();
			if (_totalElements > static_cast<unsigned long long>(std::numeric_limits<int>::max()))
				throw std::overflow_error("VSLIDE1UP staged-write count exceeds the supported vector size.");
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'checked' in this context:
// ORIGINAL LINE: _stagedWrites = new StagedVectorWrite[checked((int)_totalElements)];
			_stagedWrites.clear();
			_stagedWrites.reserve(static_cast<std::size_t>(_totalElements));
			_stagedWriteCount = 0;

			for (unsigned long long element = 1; element < _totalElements; element++)
			{
				if (!LaneMayOverwrite(core, element))
				{
					continue;
				}

// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'checked' in this context:
// ORIGINAL LINE: ulong sourceAddress = checked(Instruction.DestSrc1Pointer + ((element - 1) * (ulong)elementSize));
				unsigned long long sourceAddress = CheckedAdd(getInstruction().getDestSrc1Pointer(), CheckedMultiply(element - 1, static_cast<unsigned long long>(elementSize)));
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'checked' in this context:
// ORIGINAL LINE: ulong destinationAddress = checked(Instruction.DestSrc1Pointer + (element * (ulong)elementSize));
				unsigned long long destinationAddress = CheckedAdd(getInstruction().getDestSrc1Pointer(), CheckedMultiply(element, static_cast<unsigned long long>(elementSize)));

				std::vector<unsigned char> sourceBuffer(elementSize);
				core->ReadBoundMainMemoryExact(sourceAddress, sourceBuffer, L"VectorSlideOneUpMicroOp.Execute()");

				core->ThrowIfBoundMainMemoryRangeUnavailable(destinationAddress, elementSize, L"VectorSlideOneUpMicroOp.Execute()");

				_stagedWrites.emplace_back(element, destinationAddress, sourceBuffer);
				++_stagedWriteCount;
			}

			_state = ExecutionState::Complete;
		}

		return true;
	}

	void VectorSlideOneUpMicroOp::EmitWriteBackRetireRecords(std::shared_ptr<Processor::CPU_Core> &core, std::span<RetireRecord> retireRecords, int &retireRecordCount)
	{
		PublishStagedWritesAtWriteBack(core);
	}

	void VectorSlideOneUpMicroOp::PublishStagedWritesAtWriteBack(std::shared_ptr<Processor::CPU_Core> &core)
	{
		if (_state != ExecutionState::Complete || _stagedWriteCount < 0 || static_cast<size_t>(_stagedWriteCount) != _stagedWrites.size())
		{
			throw InvalidOperationException(L"VectorSlideOneUpMicroOp WB publication reached retire without a completed staged VSLIDE1UP result.");
		}

		for (int i = 0; i < _stagedWriteCount; i++)
		{
			StagedVectorWrite write = _stagedWrites[i];
			core->ThrowIfBoundMainMemoryRangeUnavailable(write.getAddress(), write.getData().size(), L"VectorSlideOneUpMicroOp.EmitWriteBackRetireRecords()");
		}

		for (int i = 0; i < _stagedWriteCount; i++)
		{
			StagedVectorWrite write = _stagedWrites[i];
			std::vector<unsigned char> data = write.getData();
			core->WriteBoundMainMemoryExact(write.getAddress(), data, L"VectorSlideOneUpMicroOp.EmitWriteBackRetireRecords()");
		}
	}

	void VectorSlideOneUpMicroOp::ThrowIfUnsupportedPublicationContour()
	{
		if (getOpCode() != static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VSLIDE1UP) || getInstruction().getOpCode() != static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VSLIDE1UP))
		{
			throw DecodeProjectionFaultException(L"VectorSlideOneUpMicroOp.InitializeMetadata() can only publish the Phase 09 VSLIDE1UP contour.");
		}

		if (getInstruction().getIndexed() || getInstruction().getIs2D())
		{
			std::wstring addressingContour = getInstruction().getIndexed() && getInstruction().getIs2D() ? L"indexed+2D" : getInstruction().getIndexed() ? L"indexed" : L"2D";
			throw DecodeProjectionFaultException(std::format(L"VectorSlideOneUpMicroOp.InitializeMetadata() rejected unsupported {} VSLIDE1UP publication. Phase 09 only opens the packed 1D fixed-one-lane single-surface contour.", addressingContour));
		}

		if (getInstruction().getImmediate() != 0)
		{
			throw DecodeProjectionFaultException(L"VectorSlideOneUpMicroOp.InitializeMetadata() rejected non-zero VSLIDE1UP immediate sideband.");
		}

		if (getInstruction().getStride() != 0)
		{
			throw DecodeProjectionFaultException(L"VectorSlideOneUpMicroOp.InitializeMetadata() rejected non-zero VSLIDE1UP stride. The selected Phase 09 contour is packed 1D only.");
		}

		if (getInstruction().getSrc2Pointer() != 0)
		{
			throw DecodeProjectionFaultException(L"VectorSlideOneUpMicroOp.InitializeMetadata() rejected non-zero VSLIDE1UP secondary pointer. The selected Phase 09 contour is single-surface only.");
		}

		ThrowIfUnsupportedSlideOneDataType(getInstruction().getDataTypeValue(), L"VectorSlideOneUpMicroOp.InitializeMetadata()");
	}

	void VectorSlideOneUpMicroOp::ThrowIfUnsupportedRuntimeContour()
	{
		if (!getInstruction().getIndexed() && !getInstruction().getIs2D() && getInstruction().getImmediate() == 0 && getInstruction().getStride() == 0 && getInstruction().getSrc2Pointer() == 0)
		{
			ThrowIfUnsupportedSlideOneDataType(getInstruction().getDataTypeValue(), L"VectorSlideOneUpMicroOp.Execute()");
			return;
		}

		throw InvalidOperationException(L"VectorSlideOneUpMicroOp.Execute() rejected a non-canonical VSLIDE1UP runtime contour. Only packed 1D fixed-one-lane single-surface semantics are executable.");
	}

	void VectorSlideOneUpMicroOp::ThrowIfUnsupportedSlideOneDataType(DataTypeEnum dataType, const std::wstring &surface)
	{
		if (DataTypeUtils::IsValid(dataType) && DataTypeUtils::SizeOf(dataType) != 0)
		{
			return;
		}

		throw DecodeProjectionFaultException(std::format(L"{} rejected unsupported VSLIDE1UP DataType {}. Phase 09 publishes only element-sized packed slide-one semantics.", surface, static_cast<unsigned int>(dataType)));
	}

	bool VectorSlideOneUpMicroOp::LaneMayOverwrite(std::shared_ptr<Processor::CPU_Core> &core, unsigned long long element)
	{
		if (element > std::numeric_limits<int>::max())
		{
			return false;
		}

		bool laneActive = core->LaneActive(getInstruction().getPredicateMask(), static_cast<int>(element));
		bool effectiveMaskAgnostic = getInstruction().getMaskAgnostic() || core->getVectorConfig()->getMaskAgnostic() != 0;
		return laneActive || effectiveMaskAgnostic;
	}

	std::vector<VectorSlideOneUpMicroOp::StagedVectorWrite> VectorSlideOneUpMicroOp::GetStagedWrites()
	{
		if (_stagedWrites.empty())
		{
			return {};
		}

		if (_stagedWriteCount < 0 || static_cast<size_t>(_stagedWriteCount) > _stagedWrites.size())
		{
			throw InvalidOperationException(L"VectorSlideOneUpMicroOp staged-write count is outside its owned buffer.");
		}

		return std::vector<StagedVectorWrite>(_stagedWrites.begin(), _stagedWrites.begin() + _stagedWriteCount);
	}

	VectorSlideOneUpMicroOp::StagedVectorWrite::StagedVectorWrite(unsigned long long element, unsigned long long address, std::vector<unsigned char> &data)
		: Element(element), Address(address), Data(data)
	{
	}

	unsigned long long VectorSlideOneUpMicroOp::StagedVectorWrite::getElement() const
	{
		return Element;
	}

	unsigned long long VectorSlideOneUpMicroOp::StagedVectorWrite::getAddress() const
	{
		return Address;
	}

	std::vector<unsigned char> VectorSlideOneUpMicroOp::StagedVectorWrite::getData() const
	{
		return Data;
	}

	void VectorSlideOneDownMicroOp::InitializeMetadata()
	{
		VectorMicroOp::InitializeMetadata();
		setMemoryOp(false);
		ThrowIfUnsupportedPublicationContour();

		if (getInstruction().getStreamLength() > 0)
		{
			int elementSize = GetElementSize();
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'checked' in this context:
// ORIGINAL LINE: ulong totalSize = checked(Instruction.StreamLength * (ulong)elementSize);
			unsigned long long totalSize = CheckedMultiply(getInstruction().getStreamLength(), static_cast<unsigned long long>(elementSize));

			setReadMemoryRanges({MemoryRange{getInstruction().getDestSrc1Pointer(), totalSize}});
			setWriteMemoryRanges({MemoryRange{getInstruction().getDestSrc1Pointer(), totalSize}});
		}

		RefreshVectorAdmissionMetadata(true, true);
	}

	bool VectorSlideOneDownMicroOp::Execute(std::shared_ptr<Processor::CPU_Core> &core)
	{
		if (_state == ExecutionState::Complete)
		{
			return true;
		}

		if (_state == ExecutionState::NotStarted)
		{
			_totalElements = getInstruction().getStreamLength();
			ThrowIfUnsupportedRuntimeContour();
			ThrowIfZeroLengthVectorComputeContour(L"VectorSlideOneDownMicroOp.Execute()");

			int elementSize = GetElementSize();
			if (_totalElements > static_cast<unsigned long long>(std::numeric_limits<int>::max()))
				throw std::overflow_error("VSLIDE1DOWN staged-write count exceeds the supported vector size.");
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'checked' in this context:
// ORIGINAL LINE: _stagedWrites = new StagedVectorWrite[checked((int)_totalElements)];
			_stagedWrites.clear();
			_stagedWrites.reserve(static_cast<std::size_t>(_totalElements));
			_stagedWriteCount = 0;

			for (unsigned long long element = 0; element < _totalElements - 1; element++)
			{
				if (!LaneMayOverwrite(core, element))
				{
					continue;
				}

// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'checked' in this context:
// ORIGINAL LINE: ulong sourceAddress = checked(Instruction.DestSrc1Pointer + ((element + 1) * (ulong)elementSize));
				unsigned long long sourceAddress = CheckedAdd(getInstruction().getDestSrc1Pointer(), CheckedMultiply(element + 1, static_cast<unsigned long long>(elementSize)));
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'checked' in this context:
// ORIGINAL LINE: ulong destinationAddress = checked(Instruction.DestSrc1Pointer + (element * (ulong)elementSize));
				unsigned long long destinationAddress = CheckedAdd(getInstruction().getDestSrc1Pointer(), CheckedMultiply(element, static_cast<unsigned long long>(elementSize)));

				std::vector<unsigned char> sourceBuffer(elementSize);
				core->ReadBoundMainMemoryExact(sourceAddress, sourceBuffer, L"VectorSlideOneDownMicroOp.Execute()");

				core->ThrowIfBoundMainMemoryRangeUnavailable(destinationAddress, elementSize, L"VectorSlideOneDownMicroOp.Execute()");

				_stagedWrites.emplace_back(element, destinationAddress, sourceBuffer);
				++_stagedWriteCount;
			}

			_state = ExecutionState::Complete;
		}

		return true;
	}

	void VectorSlideOneDownMicroOp::EmitWriteBackRetireRecords(std::shared_ptr<Processor::CPU_Core> &core, std::span<RetireRecord> retireRecords, int &retireRecordCount)
	{
		PublishStagedWritesAtWriteBack(core);
	}

	void VectorSlideOneDownMicroOp::PublishStagedWritesAtWriteBack(std::shared_ptr<Processor::CPU_Core> &core)
	{
		if (_state != ExecutionState::Complete || _stagedWriteCount < 0 || static_cast<size_t>(_stagedWriteCount) != _stagedWrites.size())
		{
			throw InvalidOperationException(L"VectorSlideOneDownMicroOp WB publication reached retire without a completed staged VSLIDE1DOWN result.");
		}

		for (int i = 0; i < _stagedWriteCount; i++)
		{
			StagedVectorWrite write = _stagedWrites[i];
			core->ThrowIfBoundMainMemoryRangeUnavailable(write.getAddress(), write.getData().size(), L"VectorSlideOneDownMicroOp.EmitWriteBackRetireRecords()");
		}

		for (int i = 0; i < _stagedWriteCount; i++)
		{
			StagedVectorWrite write = _stagedWrites[i];
			std::vector<unsigned char> data = write.getData();
			core->WriteBoundMainMemoryExact(write.getAddress(), data, L"VectorSlideOneDownMicroOp.EmitWriteBackRetireRecords()");
		}
	}

	void VectorSlideOneDownMicroOp::ThrowIfUnsupportedPublicationContour()
	{
		if (getOpCode() != static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VSLIDE1DOWN) || getInstruction().getOpCode() != static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VSLIDE1DOWN))
		{
			throw DecodeProjectionFaultException(L"VectorSlideOneDownMicroOp.InitializeMetadata() can only publish the Phase 09 VSLIDE1DOWN contour.");
		}

		if (getInstruction().getIndexed() || getInstruction().getIs2D())
		{
			std::wstring addressingContour = getInstruction().getIndexed() && getInstruction().getIs2D() ? L"indexed+2D" : getInstruction().getIndexed() ? L"indexed" : L"2D";
			throw DecodeProjectionFaultException(std::format(L"VectorSlideOneDownMicroOp.InitializeMetadata() rejected unsupported {} VSLIDE1DOWN publication. Phase 09 only opens the packed 1D fixed-one-lane single-surface contour.", addressingContour));
		}

		if (getInstruction().getImmediate() != 0)
		{
			throw DecodeProjectionFaultException(L"VectorSlideOneDownMicroOp.InitializeMetadata() rejected non-zero VSLIDE1DOWN immediate sideband.");
		}

		if (getInstruction().getStride() != 0)
		{
			throw DecodeProjectionFaultException(L"VectorSlideOneDownMicroOp.InitializeMetadata() rejected non-zero VSLIDE1DOWN stride. The selected Phase 09 contour is packed 1D only.");
		}

		if (getInstruction().getSrc2Pointer() != 0)
		{
			throw DecodeProjectionFaultException(L"VectorSlideOneDownMicroOp.InitializeMetadata() rejected non-zero VSLIDE1DOWN secondary pointer. The selected Phase 09 contour is single-surface only.");
		}

		ThrowIfUnsupportedSlideOneDataType(getInstruction().getDataTypeValue(), L"VectorSlideOneDownMicroOp.InitializeMetadata()");
	}

	void VectorSlideOneDownMicroOp::ThrowIfUnsupportedRuntimeContour()
	{
		if (!getInstruction().getIndexed() && !getInstruction().getIs2D() && getInstruction().getImmediate() == 0 && getInstruction().getStride() == 0 && getInstruction().getSrc2Pointer() == 0)
		{
			ThrowIfUnsupportedSlideOneDataType(getInstruction().getDataTypeValue(), L"VectorSlideOneDownMicroOp.Execute()");
			return;
		}

		throw InvalidOperationException(L"VectorSlideOneDownMicroOp.Execute() rejected a non-canonical VSLIDE1DOWN runtime contour. Only packed 1D fixed-one-lane single-surface semantics are executable.");
	}

	void VectorSlideOneDownMicroOp::ThrowIfUnsupportedSlideOneDataType(DataTypeEnum dataType, const std::wstring &surface)
	{
		if (DataTypeUtils::IsValid(dataType) && DataTypeUtils::SizeOf(dataType) != 0)
		{
			return;
		}

		throw DecodeProjectionFaultException(std::format(L"{} rejected unsupported VSLIDE1DOWN DataType {}. Phase 09 publishes only element-sized packed slide-one semantics.", surface, static_cast<unsigned int>(dataType)));
	}

	bool VectorSlideOneDownMicroOp::LaneMayOverwrite(std::shared_ptr<Processor::CPU_Core> &core, unsigned long long element)
	{
		if (element > std::numeric_limits<int>::max())
		{
			return false;
		}

		bool laneActive = core->LaneActive(getInstruction().getPredicateMask(), static_cast<int>(element));
		bool effectiveMaskAgnostic = getInstruction().getMaskAgnostic() || core->getVectorConfig()->getMaskAgnostic() != 0;
		return laneActive || effectiveMaskAgnostic;
	}

	std::vector<VectorSlideOneDownMicroOp::StagedVectorWrite> VectorSlideOneDownMicroOp::GetStagedWrites()
	{
		if (_stagedWrites.empty())
		{
			return {};
		}

		if (_stagedWriteCount < 0 || static_cast<size_t>(_stagedWriteCount) > _stagedWrites.size())
		{
			throw InvalidOperationException(L"VectorSlideOneDownMicroOp staged-write count is outside its owned buffer.");
		}

		return std::vector<StagedVectorWrite>(_stagedWrites.begin(), _stagedWrites.begin() + _stagedWriteCount);
	}

	VectorSlideOneDownMicroOp::StagedVectorWrite::StagedVectorWrite(unsigned long long element, unsigned long long address, std::vector<unsigned char> &data)
		: Element(element), Address(address), Data(data)
	{
	}

	unsigned long long VectorSlideOneDownMicroOp::StagedVectorWrite::getElement() const
	{
		return Element;
	}

	unsigned long long VectorSlideOneDownMicroOp::StagedVectorWrite::getAddress() const
	{
		return Address;
	}

	std::vector<unsigned char> VectorSlideOneDownMicroOp::StagedVectorWrite::getData() const
	{
		return Data;
	}
}
