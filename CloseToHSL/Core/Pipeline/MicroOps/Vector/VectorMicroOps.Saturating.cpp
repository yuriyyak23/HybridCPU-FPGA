#include "VectorMicroOps.Saturating.h"
#include "../../../State/ExecutionState.h"
#include "../../../../../Arch/DataTypeEnum.h"
#include "../../../Execution/Vector/ALU/YAKSys_Hybrid_CPU.Execution.VectorALU.h"
#include "../../../Diagnostics/DecodeProjectionFaultException.h"
#include "../../../../../Arch/DataType.h"
#include "../../../Execution/ExecutionFaultContract.h"
#include <limits>

namespace
{
	unsigned long long CheckedMultiply(unsigned long long left, unsigned long long right)
	{
		if (right != 0 && left > std::numeric_limits<unsigned long long>::max() / right)
			throw std::overflow_error("VADD.SAT address multiplication overflow.");
		return left * right;
	}

	unsigned long long CheckedAdd(unsigned long long left, unsigned long long right)
	{
		if (right > std::numeric_limits<unsigned long long>::max() - left)
			throw std::overflow_error("VADD.SAT address addition overflow.");
		return left + right;
	}
}

using namespace HybridCPU_ISE::Arch;
using namespace YAKSys_Hybrid_CPU::Arch;
using namespace YAKSys_Hybrid_CPU::Core::Pipeline::MicroOps;
using namespace YAKSys_Hybrid_CPU::Core::Registers::Retire;
using namespace YAKSys_Hybrid_CPU::Execution;

namespace YAKSys_Hybrid_CPU::Core
{

	void VectorSaturatingAddMicroOp::InitializeMetadata()
	{
		VectorMicroOp::InitializeMetadata();
		setMemoryOp(false);
		ThrowIfUnsupportedPublicationContour();

		if (getInstruction().getStreamLength() > 0)
		{
			int elemSize = GetElementSize();
			unsigned long long stride = ResolveEffectiveStride(elemSize);
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'checked' in this context:
// ORIGINAL LINE: ulong totalSize = checked(Instruction.StreamLength * stride);
			unsigned long long totalSize = CheckedMultiply(getInstruction().getStreamLength(), stride);

			setReadMemoryRanges({MemoryRange{getInstruction().getDestSrc1Pointer(), totalSize}, MemoryRange{getInstruction().getSrc2Pointer(), totalSize}});
			setWriteMemoryRanges({MemoryRange{getInstruction().getDestSrc1Pointer(), totalSize}});
		}

		RefreshVectorAdmissionMetadata(true, true);
	}

	bool VectorSaturatingAddMicroOp::Execute(std::shared_ptr<Processor::CPU_Core> &core)
	{
		if (_state == ExecutionState::Complete)
		{
			return true;
		}

		if (_state == ExecutionState::NotStarted)
		{
			_totalElements = getInstruction().getStreamLength();
			ThrowIfUnsupportedRuntimeContour();
			ThrowIfZeroLengthVectorComputeContour(L"VectorSaturatingAddMicroOp.Execute()");

			DataTypeEnum dataType = getInstruction().getDataTypeValue();
			int elemSize = GetElementSize();
			unsigned long long stride = ResolveEffectiveStride(elemSize);
			if (_totalElements > static_cast<unsigned long long>(std::numeric_limits<int>::max()))
				throw std::overflow_error("VADD.SAT staged-write count exceeds the supported vector size.");

// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'checked' in this context:
// ORIGINAL LINE: _stagedWrites = new StagedVectorWrite[checked((int)_totalElements)];
			_stagedWrites.clear();
			_stagedWrites.reserve(static_cast<std::size_t>(_totalElements));
			_stagedWriteCount = 0;

			for (unsigned long long element = 0; element < _totalElements; element++)
			{
				if (!IsLaneActiveForSaturatingAdd(core, element))
				{
					continue;
				}

// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'checked' in this context:
// ORIGINAL LINE: ulong offset = checked(element * stride);
				unsigned long long offset = CheckedMultiply(element, stride);
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'checked' in this context:
// ORIGINAL LINE: ulong sourceAAddress = checked(Instruction.DestSrc1Pointer + offset);
				unsigned long long sourceAAddress = CheckedAdd(getInstruction().getDestSrc1Pointer(), offset);
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'checked' in this context:
// ORIGINAL LINE: ulong sourceBAddress = checked(Instruction.Src2Pointer + offset);
				unsigned long long sourceBAddress = CheckedAdd(getInstruction().getSrc2Pointer(), offset);
				unsigned long long destinationAddress = sourceAAddress;

				std::vector<unsigned char> sourceA(elemSize);
				std::vector<unsigned char> sourceB(elemSize);
				std::vector<unsigned char> destination(elemSize);

				core->ReadBoundMainMemoryExact(sourceAAddress, sourceA, L"VectorSaturatingAddMicroOp.Execute()");
				core->ReadBoundMainMemoryExact(sourceBAddress, sourceB, L"VectorSaturatingAddMicroOp.Execute()");

				VectorALU::ApplySaturatingAddElement(getOpCode(), dataType, sourceA, sourceB, destination, core);

				core->ThrowIfBoundMainMemoryRangeUnavailable(destinationAddress, elemSize, L"VectorSaturatingAddMicroOp.Execute()");

				_stagedWrites.emplace_back(element, destinationAddress, destination);
				++_stagedWriteCount;
			}

			_state = ExecutionState::Complete;
		}

		return true;
	}

	void VectorSaturatingAddMicroOp::EmitWriteBackRetireRecords(std::shared_ptr<Processor::CPU_Core> &core, std::span<RetireRecord> retireRecords, int &retireRecordCount)
	{
		(void)retireRecords;
		(void)retireRecordCount;
		PublishStagedWritesAtWriteBack(core);
	}

	void VectorSaturatingAddMicroOp::PublishStagedWritesAtWriteBack(std::shared_ptr<Processor::CPU_Core> &core)
	{
		if (_state != ExecutionState::Complete || _stagedWriteCount < 0 || static_cast<std::size_t>(_stagedWriteCount) > _stagedWrites.size())
		{
			throw InvalidOperationException(L"VectorSaturatingAddMicroOp WB publication reached retire without a completed staged VADD.SAT result.");
		}

		for (int i = 0; i < _stagedWriteCount; i++)
		{
			StagedVectorWrite write = _stagedWrites[i];
			core->ThrowIfBoundMainMemoryRangeUnavailable(write.getAddress(), write.getData().size(), L"VectorSaturatingAddMicroOp.EmitWriteBackRetireRecords()");
		}

		for (int i = 0; i < _stagedWriteCount; i++)
		{
			StagedVectorWrite write = _stagedWrites[i];
			std::vector<unsigned char> data = write.getData();
			core->WriteBoundMainMemoryExact(write.getAddress(), data, L"VectorSaturatingAddMicroOp.EmitWriteBackRetireRecords()");
		}
	}

	void VectorSaturatingAddMicroOp::ThrowIfUnsupportedPublicationContour()
	{
		if (getOpCode() != static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VADD) || getInstruction().getOpCode() != static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VADD) || !getInstruction().getSaturating())
		{
			throw DecodeProjectionFaultException(L"VectorSaturatingAddMicroOp.InitializeMetadata() can only publish the Phase 05B VADD.SAT policy contour.");
		}

		if (getInstruction().getIndexed() || getInstruction().getIs2D())
		{
			std::wstring addressingContour = getInstruction().getIndexed() && getInstruction().getIs2D() ? L"indexed+2D" : getInstruction().getIndexed() ? L"indexed" : L"2D";
			throw DecodeProjectionFaultException(std::format(L"VectorSaturatingAddMicroOp.InitializeMetadata() rejected unsupported {} VADD.SAT publication. Phase 05B opens only the 1D memory-to-memory saturating add policy contour.", addressingContour));
		}

		if (getInstruction().getImmediate() != 0)
		{
			throw DecodeProjectionFaultException(L"VectorSaturatingAddMicroOp.InitializeMetadata() rejected non-zero VADD.SAT immediate sideband.");
		}

		ThrowIfUnsupportedIntegerDataType(L"VectorSaturatingAddMicroOp.InitializeMetadata()");
		ResolveEffectiveStride(GetElementSize());
	}

	void VectorSaturatingAddMicroOp::ThrowIfUnsupportedRuntimeContour()
	{
		if (!getInstruction().getIndexed() && !getInstruction().getIs2D() && getInstruction().getImmediate() == 0 && getInstruction().getSaturating())
		{
			ThrowIfUnsupportedIntegerDataType(L"VectorSaturatingAddMicroOp.Execute()");
			ResolveEffectiveStride(GetElementSize());
			return;
		}

		throw InvalidOperationException(L"VectorSaturatingAddMicroOp.Execute() rejected a non-canonical VADD.SAT runtime contour. Only 1D integer signed/unsigned saturating add is executable.");
	}

	void VectorSaturatingAddMicroOp::ThrowIfUnsupportedIntegerDataType(const std::wstring &surface)
	{
		DataTypeEnum dataType = getInstruction().getDataTypeValue();
		if (DataTypeUtils::IsSignedInteger(dataType) || DataTypeUtils::IsUnsignedInteger(dataType))
		{
			return;
		}

		throw DecodeProjectionFaultException(std::format(L"{} rejected unsupported VADD.SAT DataType {}. Phase 05B opens saturating add only for integer signed/unsigned element types.", surface, static_cast<unsigned int>(dataType)));
	}

	unsigned long long VectorSaturatingAddMicroOp::ResolveEffectiveStride(int elemSize)
	{
		if (elemSize <= 0)
		{
			throw ExecutionFaultContract::CreateUnsupportedVectorElementTypeException(std::format(L"VADD.SAT rejected unsupported element DataType 0x{:02X}.", static_cast<unsigned int>(getInstruction().getDataType())));
		}

		if (getInstruction().getStride() == 0)
		{
			return static_cast<unsigned long long>(elemSize);
		}

		if (getInstruction().getStride() < elemSize)
		{
		throw DecodeProjectionFaultException(L"VectorSaturatingAddMicroOp rejected VADD.SAT stride smaller than element size. Overlapping element publication is outside the selected Phase 05B contour.");
		}

		return getInstruction().getStride();
	}

	bool VectorSaturatingAddMicroOp::IsLaneActiveForSaturatingAdd(std::shared_ptr<Processor::CPU_Core> &core, unsigned long long element)
	{
		if (element > std::numeric_limits<int>::max())
		{
			return false;
		}

		return core->LaneActive(getInstruction().getPredicateMask(), static_cast<int>(element));
	}

	std::vector<VectorSaturatingAddMicroOp::StagedVectorWrite> VectorSaturatingAddMicroOp::GetStagedWrites()
	{
		if (_stagedWrites.empty())
		{
			return {};
		}

		if (_stagedWriteCount < 0 || static_cast<size_t>(_stagedWriteCount) > _stagedWrites.size())
		{
			throw InvalidOperationException(L"VectorSaturatingAddMicroOp staged-write count is outside its owned buffer.");
		}

		return std::vector<StagedVectorWrite>(_stagedWrites.begin(), _stagedWrites.begin() + _stagedWriteCount);
	}

	VectorSaturatingAddMicroOp::StagedVectorWrite::StagedVectorWrite(unsigned long long element, unsigned long long address, std::vector<unsigned char> &data)
		: Element(element), Address(address), Data(data)
	{
	}

	unsigned long long VectorSaturatingAddMicroOp::StagedVectorWrite::getElement() const
	{
		return Element;
	}

	unsigned long long VectorSaturatingAddMicroOp::StagedVectorWrite::getAddress() const
	{
		return Address;
	}

	std::vector<unsigned char> VectorSaturatingAddMicroOp::StagedVectorWrite::getData() const
	{
		return Data;
	}
}
