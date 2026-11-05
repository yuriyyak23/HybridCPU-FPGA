#include "VectorMicroOps.Data.h"
#include <algorithm>
#include <span>
#include <stdexcept>
#include "../../../State/ExecutionState.h"
#include "../../../Execution/StreamEngine/BurstIO/StreamEngine.BurstIO.h"
#include "../../../Execution/Vector/ALU/YAKSys_Hybrid_CPU.Execution.VectorALU.h"
#include "../../../../../NonRTL/Arch/YAKSys_Hybrid_CPU.Arch.OpcodeRegistry.h"
#include "../../../../../Arch/DataTypeEnum.h"
#include "../../../Execution/ExecutionFaultContract.h"
#include "../../../Diagnostics/DecodeProjectionFaultException.h"
#include "../../Safety/ResourceBitset.h"
#include "../../Scheduling/SlotLegality/SlotClassDefinitions.h"
#include "../../../../../NonRTL/Arch/InstructionClassifier.h"
#include "../Types/MicroOp.Exceptions.h"
#include "../../../../Memory/Subsystem/YAKSys_Hybrid_CPU.Memory.MemorySubsystem.h"
#include <new>

#ifdef FormatMessage
#undef FormatMessage
#endif

using namespace HybridCPU_ISE::Arch;
using namespace HybridCPU_ISE::CloseToHSL::Memory::Timing;
using namespace YAKSys_Hybrid_CPU::Execution;
using namespace YAKSys_Hybrid_CPU::Core::Registers;
using AddressGen = YAKSys_Hybrid_CPU::Execution::AddressGen;
using PortType = YAKSys_Hybrid_CPU::Execution::PortType;

namespace YAKSys_Hybrid_CPU::Core
{
	namespace
	{
		template <typename T>
		std::span<T> CheckedSubspan(std::span<T> source, std::size_t offset, std::size_t count)
		{
			if (offset > source.size() || count > source.size() - offset)
				throw std::out_of_range("Vector scratch slice is outside the available span.");
			return source.subspan(offset, count);
		}

		void CopySpan(std::span<const unsigned char> source, std::span<unsigned char> destination)
		{
			if (source.size() > destination.size())
				throw std::out_of_range("Vector scratch copy exceeds destination span.");
			std::copy(source.begin(), source.end(), destination.begin());
		}
	}

	void VectorPermutationMicroOp::InitializeMetadata()
	{
		VectorMicroOp::InitializeMetadata();

		// Permutation operations can be stolen
		setMemoryOp(false);
		ThrowIfUnsupportedElementDataTypeForMetadata(L"VectorPermutationMicroOp.InitializeMetadata()");

		// Calculate memory ranges based on instruction parameters
		if (getInstruction().getStreamLength() > 0)
		{
			int elemSize = GetElementSize();
			unsigned long long stride = getInstruction().getStride() > 0 ? getInstruction().getStride() : static_cast<unsigned long long>(elemSize);
			unsigned long long totalSize = getInstruction().getStreamLength() * stride;

			// Read from source vector and index vector
			setReadMemoryRanges({MemoryRange{getInstruction().getDestSrc1Pointer(), totalSize}, MemoryRange{getInstruction().getSrc2Pointer(), totalSize}});

			// Write permuted result
			setWriteMemoryRanges({MemoryRange{getInstruction().getDestSrc1Pointer(), totalSize}});
		}

		RefreshVectorAdmissionMetadata(true, true);
	}

	bool VectorPermutationMicroOp::Execute(std::shared_ptr<Processor::CPU_Core> &core)
	{
		if (_state == ExecutionState::NotStarted)
		{
			_totalElements = getInstruction().getStreamLength();
			ThrowIfUnsupportedAddressingContour();
			ThrowIfZeroLengthVectorComputeContour(L"VectorPermutationMicroOp.Execute()");

			ThrowIfUnsupportedElementDataType(L"VectorPermutationMicroOp.Execute()");

			_state = ExecutionState::LoadingOperands;
		}

		// Permutation requires loading entire vector to avoid read-after-write hazards
		if (_state == ExecutionState::LoadingOperands)
		{
			LoadAllElements(core);
			_state = ExecutionState::Computing;
		}

		if (_state == ExecutionState::Computing)
		{
			ComputePermutation(core);
			_state = ExecutionState::StoringResults;
		}

		if (_state == ExecutionState::StoringResults)
		{
			StoreAllElements(core);
			_state = ExecutionState::Complete;
		}

		return true;
	}

	void VectorPermutationMicroOp::ThrowIfUnsupportedAddressingContour()
	{
		if (!getInstruction().getIndexed() && !getInstruction().getIs2D())
		{
			return;
		}

		std::wstring addressingContour = getInstruction().getIndexed() && getInstruction().getIs2D() ? L"indexed+2D" : getInstruction().getIndexed() ? L"indexed" : L"2D";
		throw InvalidOperationException(std::format(L"VectorPermutationMicroOp.Execute() rejected {} addressing on the authoritative mainline permutation contour. ", addressingContour) + L"This carrier only follows through 1D two-surface memory semantics and must fail closed instead of hidden success on a non-representable compat surface.");
	}

	void VectorPermutationMicroOp::LoadAllElements(std::shared_ptr<Processor::CPU_Core> &core)
	{
		int elemSize = GetElementSize();
		unsigned long long ptrSrc = getInstruction().getDestSrc1Pointer();
		unsigned long long ptrIndex = getInstruction().getSrc2Pointer();
		unsigned short stride = getInstruction().getStride();
		if (stride == 0)
		{
			stride = static_cast<unsigned short>(elemSize);
		}

		std::span<unsigned char> bufSrc = core->GetScratchA();
		std::span<unsigned char> bufIndex = core->GetScratchB();
		std::span<unsigned char> bufDst = core->GetScratchDst();

		YAKSys_Hybrid_CPU::Execution::BurstIO::BurstRead(ptrSrc, bufSrc, _totalElements, elemSize, stride);
		YAKSys_Hybrid_CPU::Execution::BurstIO::BurstRead(ptrIndex, bufIndex, _totalElements, elemSize, stride);
		// Permute semantics preserve masked-off destination lanes, so seed the destination
		// scratch buffer from the current vector surface before ApplyPermute mutates it.
		YAKSys_Hybrid_CPU::Execution::BurstIO::BurstRead(ptrSrc, bufDst, _totalElements, elemSize, stride);
	}

	void VectorPermutationMicroOp::ComputePermutation(std::shared_ptr<Processor::CPU_Core> &core)
	{
		int elemSize = GetElementSize();
		std::span<unsigned char> bufSrc = core->GetScratchA();
		std::span<unsigned char> bufIndex = core->GetScratchB();
		std::span<unsigned char> bufDst = core->GetScratchDst();

		// Use VectorALU.ApplyPermute for permutation logic
		VectorALU::ApplyPermute(getInstruction().getDataTypeValue(), bufSrc, bufIndex, bufDst, elemSize, _totalElements, getInstruction().getPredicateMask(), getInstruction().getTailAgnostic(), getInstruction().getMaskAgnostic(), core);
	}

	void VectorPermutationMicroOp::StoreAllElements(std::shared_ptr<Processor::CPU_Core> &core)
	{
		int elemSize = GetElementSize();
		unsigned long long ptrDst = getInstruction().getDestSrc1Pointer();
		unsigned short stride = getInstruction().getStride();
		if (stride == 0)
		{
			stride = static_cast<unsigned short>(elemSize);
		}

		std::span<unsigned char> bufDst = core->GetScratchDst();

		YAKSys_Hybrid_CPU::Execution::BurstIO::BurstWrite(ptrDst, bufDst, _totalElements, elemSize, stride);
	}

	void VectorSlideMicroOp::InitializeMetadata()
	{
		VectorMicroOp::InitializeMetadata();

		// Slide operations can be stolen
		setMemoryOp(false);
		ThrowIfUnsupportedElementDataTypeForMetadata(L"VectorSlideMicroOp.InitializeMetadata()");

		// Calculate memory ranges based on instruction parameters
		if (getInstruction().getStreamLength() > 0)
		{
			int elemSize = GetElementSize();
			unsigned long long stride = getInstruction().getStride() > 0 ? getInstruction().getStride() : static_cast<unsigned long long>(elemSize);
			unsigned long long totalSize = getInstruction().getStreamLength() * stride;

			// Read from source vector
			setReadMemoryRanges({MemoryRange{getInstruction().getDestSrc1Pointer(), totalSize}});

			// Write shifted result
			setWriteMemoryRanges({MemoryRange{getInstruction().getDestSrc1Pointer(), totalSize}});
		}

		RefreshVectorAdmissionMetadata(true, true);
	}

	bool VectorSlideMicroOp::Execute(std::shared_ptr<Processor::CPU_Core> &core)
	{
		if (_state == ExecutionState::NotStarted)
		{
			_totalElements = getInstruction().getStreamLength();
			ThrowIfUnsupportedAddressingContour();
			ThrowIfZeroLengthVectorComputeContour(L"VectorSlideMicroOp.Execute()");

			ThrowIfUnsupportedElementDataType(L"VectorSlideMicroOp.Execute()");

			_state = ExecutionState::LoadingOperands;
		}

		// Slide requires loading entire vector
		if (_state == ExecutionState::LoadingOperands)
		{
			LoadAllElements(core);
			_state = ExecutionState::Computing;
		}

		if (_state == ExecutionState::Computing)
		{
			ComputeSlide(core);
			_state = ExecutionState::StoringResults;
		}

		if (_state == ExecutionState::StoringResults)
		{
			StoreAllElements(core);
			_state = ExecutionState::Complete;
		}

		return true;
	}

	void VectorSlideMicroOp::ThrowIfUnsupportedAddressingContour()
	{
		if (!getInstruction().getIndexed() && !getInstruction().getIs2D())
		{
			return;
		}

		std::wstring addressingContour = getInstruction().getIndexed() && getInstruction().getIs2D() ? L"indexed+2D" : getInstruction().getIndexed() ? L"indexed" : L"2D";
		throw InvalidOperationException(std::format(L"VectorSlideMicroOp.Execute() rejected {} addressing on the authoritative mainline slide contour. ", addressingContour) + L"This carrier only follows through 1D single-surface memory semantics and must fail closed instead of hidden success on a non-representable compat surface.");
	}

	void VectorSlideMicroOp::LoadAllElements(std::shared_ptr<Processor::CPU_Core> &core)
	{
		int elemSize = GetElementSize();
		unsigned long long ptrSrc = getInstruction().getDestSrc1Pointer();
		unsigned short stride = getInstruction().getStride();
		if (stride == 0)
		{
			stride = static_cast<unsigned short>(elemSize);
		}

		std::span<unsigned char> bufSrc = core->GetScratchA();
		std::span<unsigned char> bufDst = core->GetScratchDst();

		YAKSys_Hybrid_CPU::Execution::BurstIO::BurstRead(ptrSrc, bufSrc, _totalElements, elemSize, stride);
		// Slide semantics preserve undisturbed destination lanes, so seed the destination
		// scratch buffer from the current vector surface before ApplySlideUp/Down mutates it.
		YAKSys_Hybrid_CPU::Execution::BurstIO::BurstRead(ptrSrc, bufDst, _totalElements, elemSize, stride);
	}

	void VectorSlideMicroOp::ComputeSlide(std::shared_ptr<Processor::CPU_Core> &core)
	{
		int elemSize = GetElementSize();
		std::span<unsigned char> bufSrc = core->GetScratchA();
		std::span<unsigned char> bufDst = core->GetScratchDst();
		unsigned short slideAmount = static_cast<unsigned short>(getInstruction().getImmediate());

		// Determine if this is slide up or slide down based on opcode
		bool isSlideUp = getOpCode() == Processor::CPU_Core::IsaOpcodeValues::VSLIDEUP;

		if (isSlideUp)
		{
			VectorALU::ApplySlideUp(getInstruction().getDataTypeValue(), bufSrc, bufDst, elemSize, _totalElements, slideAmount, getInstruction().getPredicateMask(), getInstruction().getTailAgnostic(), getInstruction().getMaskAgnostic(), core);
		}
		else
		{
			VectorALU::ApplySlideDown(getInstruction().getDataTypeValue(), bufSrc, bufDst, elemSize, _totalElements, slideAmount, getInstruction().getPredicateMask(), getInstruction().getTailAgnostic(), getInstruction().getMaskAgnostic(), core);
		}
	}

	void VectorSlideMicroOp::StoreAllElements(std::shared_ptr<Processor::CPU_Core> &core)
	{
		int elemSize = GetElementSize();
		unsigned long long ptrDst = getInstruction().getDestSrc1Pointer();
		unsigned short stride = getInstruction().getStride();
		if (stride == 0)
		{
			stride = static_cast<unsigned short>(elemSize);
		}

		std::span<unsigned char> bufDst = core->GetScratchDst();

		YAKSys_Hybrid_CPU::Execution::BurstIO::BurstWrite(ptrDst, bufDst, _totalElements, elemSize, stride);
	}

	void VectorPredicativeMovementMicroOp::InitializeMetadata()
	{
		VectorMicroOp::InitializeMetadata();

		setMemoryOp(false);
		ThrowIfUnsupportedElementDataTypeForMetadata(L"VectorPredicativeMovementMicroOp.InitializeMetadata()");

		if (getInstruction().getStreamLength() > 0)
		{
			int elemSize = GetElementSize();
			unsigned long long stride = getInstruction().getStride() > 0 ? getInstruction().getStride() : static_cast<unsigned long long>(elemSize);
			unsigned long long totalSize = getInstruction().getStreamLength() * stride;

			// Predicative movement operates in-place over one packed vector surface.
			setReadMemoryRanges({MemoryRange{getInstruction().getDestSrc1Pointer(), totalSize}});
			setWriteMemoryRanges({MemoryRange{getInstruction().getDestSrc1Pointer(), totalSize}});
		}

		RefreshVectorAdmissionMetadata(true, true);
	}

	bool VectorPredicativeMovementMicroOp::Execute(std::shared_ptr<Processor::CPU_Core> &core)
	{
		if (_state == ExecutionState::Complete)
		{
			return true;
		}

		if (_state == ExecutionState::NotStarted)
		{
			_totalElements = getInstruction().getStreamLength();
			ThrowIfUnsupportedAddressingContour();
			ThrowIfZeroLengthVectorComputeContour(L"VectorPredicativeMovementMicroOp.Execute()");
			ThrowIfUnsupportedElementDataType(L"VectorPredicativeMovementMicroOp.Execute()");
			ThrowIfUnsupportedBufferFootprintContour(core);
			_state = ExecutionState::LoadingOperands;
		}

		if (_state == ExecutionState::LoadingOperands)
		{
			LoadAllElements(core);
			_state = ExecutionState::Computing;
		}

		if (_state == ExecutionState::Computing)
		{
			ComputePredicativeMovement(core);
			_state = ExecutionState::StoringResults;
		}

		if (_state == ExecutionState::StoringResults)
		{
			StoreAllElements(core);
			_state = ExecutionState::Complete;
		}

		return true;
	}

	void VectorPredicativeMovementMicroOp::ThrowIfUnsupportedAddressingContour()
	{
		if (!getInstruction().getIndexed() && !getInstruction().getIs2D())
		{
			return;
		}

		std::wstring addressingContour = getInstruction().getIndexed() && getInstruction().getIs2D() ? L"indexed+2D" : getInstruction().getIndexed() ? L"indexed" : L"2D";
		throw InvalidOperationException(std::format(L"VectorPredicativeMovementMicroOp.Execute() rejected {} addressing on the authoritative mainline predicative-movement contour. ", addressingContour) + L"This carrier only follows through 1D single-surface truth and must fail closed instead of hidden compat success on a non-representable surface.");
	}

	void VectorPredicativeMovementMicroOp::ThrowIfUnsupportedBufferFootprintContour(std::shared_ptr<Processor::CPU_Core> &core)
	{
		int elemSize = GetElementSize();
		unsigned long long maxRepresentableElements = static_cast<unsigned long long>(std::min(core->GetScratchA().size() / elemSize, core->GetScratchDst().size() / elemSize));

		if (_totalElements <= maxRepresentableElements)
		{
			return;
		}

		throw InvalidOperationException(std::format(L"VectorPredicativeMovementMicroOp.Execute() rejected StreamLength {} for {} because the authoritative mainline 1D single-surface contour currently materializes the full vector through scratch-backed in-place follow-through. This runtime surface only represents up to {} element(s) at the current element width and must fail closed instead of publishing partial compaction/expansion truth.", _totalElements, Arch::OpcodeRegistry::GetMnemonicOrHex(getOpCode()), maxRepresentableElements));
	}

	void VectorPredicativeMovementMicroOp::LoadAllElements(std::shared_ptr<Processor::CPU_Core> &core)
	{
		int elemSize = GetElementSize();
		unsigned long long ptr = getInstruction().getDestSrc1Pointer();
		unsigned short stride = getInstruction().getStride();
		if (stride == 0)
		{
			stride = static_cast<unsigned short>(elemSize);
		}

// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'checked' in this context:
// ORIGINAL LINE: int byteCount = checked((int)(_totalElements * (ulong)elemSize));
		int byteCount = static_cast<int>(_totalElements * static_cast<unsigned long long>(elemSize));
		std::span<unsigned char> bufSrc = CheckedSubspan(core->GetScratchA(), 0, static_cast<std::size_t>(byteCount));
		std::span<unsigned char> bufDst = CheckedSubspan(core->GetScratchDst(), 0, static_cast<std::size_t>(byteCount));

		YAKSys_Hybrid_CPU::Execution::BurstIO::BurstRead(ptr, bufSrc, _totalElements, elemSize, stride);

		// Both VCOMPRESS and VEXPAND operate in-place on a single published vector surface.
		// Seed the destination buffer before the ALU mutates it so untouched lanes stay truthful.
		CopySpan(bufSrc, bufDst);
	}

	void VectorPredicativeMovementMicroOp::ComputePredicativeMovement(std::shared_ptr<Processor::CPU_Core> &core)
	{
		int elemSize = GetElementSize();
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'checked' in this context:
// ORIGINAL LINE: int byteCount = checked((int)(_totalElements * (ulong)elemSize));
		int byteCount = static_cast<int>(_totalElements * static_cast<unsigned long long>(elemSize));
		std::span<unsigned char> bufSrc = CheckedSubspan(core->GetScratchA(), 0, static_cast<std::size_t>(byteCount));
		std::span<unsigned char> bufDst = CheckedSubspan(core->GetScratchDst(), 0, static_cast<std::size_t>(byteCount));

		switch (getOpCode())
		{
			case Processor::CPU_Core::IsaOpcodeValues::VCOMPRESS:
				VectorALU::ApplyCompress(getInstruction().getDataTypeValue(), bufSrc, bufDst, elemSize, _totalElements, getInstruction().getPredicateMask(), core);
				break;

			case Processor::CPU_Core::IsaOpcodeValues::VEXPAND:
				VectorALU::ApplyExpand(getInstruction().getDataTypeValue(), bufSrc, bufDst, elemSize, _totalElements, getInstruction().getPredicateMask(), getInstruction().getTailAgnostic(), getInstruction().getMaskAgnostic(), core);
				break;

			default:
				throw InvalidOperationException(std::format(L"VectorPredicativeMovementMicroOp.Execute() rejected unsupported opcode 0x{:X} on the authoritative mainline predicative-movement contour.", getOpCode()));
		}
	}

	void VectorPredicativeMovementMicroOp::StoreAllElements(std::shared_ptr<Processor::CPU_Core> &core)
	{
		int elemSize = GetElementSize();
		unsigned long long ptr = getInstruction().getDestSrc1Pointer();
		unsigned short stride = getInstruction().getStride();
		if (stride == 0)
		{
			stride = static_cast<unsigned short>(elemSize);
		}

// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'checked' in this context:
// ORIGINAL LINE: int byteCount = checked((int)(_totalElements * (ulong)elemSize));
		int byteCount = static_cast<int>(_totalElements * static_cast<unsigned long long>(elemSize));
		std::span<unsigned char> bufDst = CheckedSubspan(core->GetScratchDst(), 0, static_cast<std::size_t>(byteCount));
		YAKSys_Hybrid_CPU::Execution::BurstIO::BurstWrite(ptr, bufDst, _totalElements, elemSize, stride);
	}

	void VectorDotProductMicroOp::InitializeMetadata()
	{
		VectorMicroOp::InitializeMetadata();

		// Dot product operations can be stolen
		setMemoryOp(false);
		ThrowIfUnsupportedDotProductElementDataTypeForMetadata();

		// Calculate memory ranges based on instruction parameters
		if (getInstruction().getStreamLength() > 0)
		{
			int elemSize = GetDotProductElementSize();
			unsigned long long stride = getInstruction().getStride() > 0 ? getInstruction().getStride() : static_cast<unsigned long long>(elemSize);

			// Check if this is a widening operation
			bool isWidening = AddressGen::IsWideningOpcode(getOpCode());

			// For widening operations: Alignment check (Early-fail)
			// Destination must be 4-byte aligned for FP32 result
			if (isWidening)
			{
				if ((getInstruction().getDestSrc1Pointer() & 0x3) != 0)
				{
					throw InvalidOperationException(std::format(L"Alignment Fault: Destination address 0x{:X} must be 4-byte aligned for widening operation (opcode={}).", getInstruction().getDestSrc1Pointer(), getOpCode()));
				}
			}

			// Calculate effective element sizes for memory ranges
			unsigned int srcA_ElemSize = AddressGen::GetEffectiveElementSize(getOpCode(), PortType::SourceA, static_cast<unsigned char>(elemSize));
			unsigned int srcB_ElemSize = AddressGen::GetEffectiveElementSize(getOpCode(), PortType::SourceB, static_cast<unsigned char>(elemSize));
			unsigned int dst_ElemSize = AddressGen::GetEffectiveElementSize(getOpCode(), PortType::Destination, static_cast<unsigned char>(elemSize));

			// Calculate total memory footprint (source: 1-byte per element, dest: 4-byte for widening)
			unsigned long long totalSrcBytes = getInstruction().getStreamLength() * srcA_ElemSize;
			unsigned long long totalDstBytes = dst_ElemSize; // Only one scalar result

			// Read from two input vectors (asymmetric read: 1-byte elements for FP8)
			setReadMemoryRanges({MemoryRange{getInstruction().getDestSrc1Pointer(), totalSrcBytes}, MemoryRange{getInstruction().getSrc2Pointer(), totalSrcBytes}});

			// Write only one scalar element (asymmetric write: 4-byte for FP32 result in widening)
			setWriteMemoryRanges({MemoryRange{getInstruction().getDestSrc1Pointer(), totalDstBytes}});
		}

		RefreshVectorAdmissionMetadata(true, true);
	}

	bool VectorDotProductMicroOp::Execute(std::shared_ptr<Processor::CPU_Core> &core)
	{
		if (_state == ExecutionState::NotStarted)
		{
			_totalElements = getInstruction().getStreamLength();
			ThrowIfUnsupportedAddressingContour();
			ThrowIfZeroLengthVectorComputeContour(L"VectorDotProductMicroOp.Execute()");

			ThrowIfUnsupportedDotProductElementDataType();

			_state = ExecutionState::LoadingOperands;
		}

		// Dot product requires loading entire vectors
		if (_state == ExecutionState::LoadingOperands)
		{
			LoadAllElements(core);
			_state = ExecutionState::Computing;
		}

		if (_state == ExecutionState::Computing)
		{
			ComputeDotProduct(core);
			_state = ExecutionState::StoringResults;
		}

		if (_state == ExecutionState::StoringResults)
		{
			StoreScalarResult(core);
			_state = ExecutionState::Complete;
		}

		return true;
	}

	void VectorDotProductMicroOp::ThrowIfUnsupportedAddressingContour()
	{
		if (!getInstruction().getIndexed() && !getInstruction().getIs2D())
		{
			return;
		}

		std::wstring addressingContour = getInstruction().getIndexed() && getInstruction().getIs2D() ? L"indexed+2D" : getInstruction().getIndexed() ? L"indexed" : L"2D";
		throw InvalidOperationException(std::format(L"VectorDotProductMicroOp.Execute() rejected {} addressing on the authoritative mainline dot-product contour. ", addressingContour) + L"This carrier only follows through 1D scalar-footprint truth and must fail closed instead of hidden compat success on a non-representable surface.");
	}

	void VectorDotProductMicroOp::LoadAllElements(std::shared_ptr<Processor::CPU_Core> &core)
	{
		int elemSize = GetDotProductElementSize();
		unsigned long long ptrA = getInstruction().getDestSrc1Pointer();
		unsigned long long ptrB = getInstruction().getSrc2Pointer();
		unsigned short stride = getInstruction().getStride();
		if (stride == 0)
		{
			stride = static_cast<unsigned short>(elemSize);
		}

		std::span<unsigned char> bufA = core->GetScratchA();
		std::span<unsigned char> bufB = core->GetScratchB();

		YAKSys_Hybrid_CPU::Execution::BurstIO::BurstRead(ptrA, bufA, _totalElements, elemSize, stride);
		YAKSys_Hybrid_CPU::Execution::BurstIO::BurstRead(ptrB, bufB, _totalElements, elemSize, stride);
	}

	void VectorDotProductMicroOp::ComputeDotProduct(std::shared_ptr<Processor::CPU_Core> &core)
	{
		int elemSize = GetDotProductElementSize();
		std::span<unsigned char> bufA = core->GetScratchA();
		std::span<unsigned char> bufB = core->GetScratchB();
		std::span<unsigned char> bufDst = core->GetScratchDst();

		// ApplyDotProduct computes scalar result and writes to element 0
		VectorALU::ApplyDotProduct(getOpCode(), getInstruction().getDataTypeValue(), bufA, bufB, bufDst, elemSize, _totalElements, getInstruction().getPredicateMask(), core);
	}

	void VectorDotProductMicroOp::StoreScalarResult(std::shared_ptr<Processor::CPU_Core> &core)
	{
		int elemSize = GetDotProductElementSize();
		unsigned long long ptrDst = getInstruction().getDestSrc1Pointer();
		unsigned int effectiveDestElemSize = AddressGen::GetEffectiveElementSize(getOpCode(), PortType::Destination, static_cast<unsigned char>(elemSize));

		std::span<unsigned char> bufDst = core->GetScratchDst();

		// Widening FP8 dot-product writes a single FP32 scalar result even though the
		// source element size remains 1 byte. Reuse the shared address-gen contract so
		// runtime follow-through matches the already-published metadata footprint.
		YAKSys_Hybrid_CPU::Execution::BurstIO::BurstWrite(ptrDst, bufDst, 1, static_cast<int>(effectiveDestElemSize), static_cast<unsigned short>(effectiveDestElemSize));
	}

	int VectorDotProductMicroOp::GetDotProductElementSize()
	{
		int elemSize = GetElementSize();
		if (elemSize != 0)
		{
			return elemSize;
		}

		return getOpCode() == Processor::CPU_Core::IsaOpcodeValues::VDOT_FP8 && (getInstruction().getDataTypeValue() == DataTypeEnum::FLOAT8_E4M3 || getInstruction().getDataTypeValue() == DataTypeEnum::FLOAT8_E5M2) ? 1 : 0;
	}

	void VectorDotProductMicroOp::ThrowIfUnsupportedDotProductElementDataType()
	{
		if (GetDotProductElementSize() != 0)
		{
			return;
		}

		throw ExecutionFaultContract::CreateUnsupportedVectorElementTypeException(std::format(L"VectorDotProductMicroOp.Execute() rejected unsupported element DataType 0x{:02X} on the authoritative mainline vector compute contour instead of hidden success/no-op.", static_cast<unsigned int>(getInstruction().getDataType())));
	}

	void VectorDotProductMicroOp::ThrowIfUnsupportedDotProductElementDataTypeForMetadata()
	{
		if (getInstruction().getStreamLength() == 0 || GetDotProductElementSize() != 0)
		{
			return;
		}

		std::shared_ptr<DecodeProjectionFaultException> exception = std::make_shared<DecodeProjectionFaultException>(ExecutionFaultContract::FormatMessage(ExecutionFaultCategory::UnsupportedVectorElementType, std::format(L"VectorDotProductMicroOp.InitializeMetadata() rejected unsupported element DataType 0x{:02X} on the authoritative mainline vector publication contour. ", static_cast<unsigned int>(getInstruction().getDataType())) + L"Materialized dot-product carriers must fail closed during metadata publication instead of emitting scalar-footprint facts for an invalid element type."));
		ExecutionFaultContract::Stamp(exception, ExecutionFaultCategory::UnsupportedVectorElementType);
		throw *exception;
	}

	bool VectorTransferMicroOp::getOwnsPendingMemoryCompletion() const
	{
		return _state == ExecutionState::LoadingOperands && _requestController != nullptr && _controllerRequestId && _controllerRequestId.value() != nullptr && _controllerRequestId.value()->isValid() && _requestController->OwnsOutstandingCanonicalVectorTransfer(*_controllerRequestId.value(), _acceptedOpcode, 0, _acceptedSourceAddress, _acceptedDestinationAddress, _acceptedElementCount, _acceptedElementSize, _acceptedStride);
	}

	bool VectorTransferMicroOp::getHasControllerAdmissionBackpressure() const
	{
		return _controllerAdmissionBackpressured && _state == ExecutionState::NotStarted && _requestController == nullptr && !_controllerRequestId;
	}

	bool VectorTransferMicroOp::getHasControllerRetireEffect() const
	{
		return _completedThroughController && _state == ExecutionState::Complete && _retireEffect != nullptr;
	}

	VectorTransferMicroOp::VectorTransferMicroOp()
	{
		setClass(MicroOpClass::Lsu);
		setMemoryOp(false);
		setStealable(false);
		setHasSideEffects(true);
		SetClassFlexiblePlacement(SlotClass::LsuClass);
	}

	void VectorTransferMicroOp::InitializeMetadata()
	{
		VectorMicroOp::InitializeMetadata();

		PublishTransferClassification();
		setMemoryOp(false);
		ThrowIfUnsupportedElementDataTypeForMetadata(L"VectorTransferMicroOp.InitializeMetadata()");

		if (getInstruction().getStreamLength() > 0)
		{
			int elemSize = GetElementSize();
			unsigned long long totalSize = ResolveTransferTotalSize(elemSize);
			unsigned long long readPointer;
			unsigned long long writePointer;
			ResolveTransferEndpoints(readPointer, writePointer);

			setReadMemoryRanges({MemoryRange{readPointer, totalSize}});
			setWriteMemoryRanges({MemoryRange{writePointer, totalSize}});
		}

		RefreshVectorAdmissionMetadata(true, true);
	}

	void VectorTransferMicroOp::PublishTransferClassification()
	{
		switch (getOpCode())
		{
			case Processor::CPU_Core::IsaOpcodeValues::VLOAD:
			case Processor::CPU_Core::IsaOpcodeValues::VSTORE:
				setInstructionClass(Arch::InstructionClass::Memory);
				setSerializationClass(Arch::InstructionClassifier::GetSerializationClass(static_cast<unsigned short>(getOpCode())));
				return;

			default:
				throw InvalidOperationException(std::format(L"VectorTransferMicroOp.InitializeMetadata() rejected unsupported transfer opcode 0x{:X} on the authoritative VLOAD/VSTORE carrier contour.", getOpCode()));
		}
	}

	bool VectorTransferMicroOp::Execute(std::shared_ptr<Processor::CPU_Core> &core)
	{
		_controllerAdmissionBackpressured = false;
		ThrowIfUnsupportedAddressingContour();

		if (getInstruction().getStreamLength() == 0)
		{
			throw InvalidOperationException(L"VectorTransferMicroOp.Execute() rejected StreamLength == 0 on the authoritative mainline VLOAD/VSTORE carrier contour instead of hidden success/no-op.");
		}

		int elemSize = GetElementSize();
		ThrowIfUnsupportedElementDataType(L"VectorTransferMicroOp.Execute()");

		unsigned long long stride = ResolveTransferStride(elemSize);
		unsigned long long readPointer;
		unsigned long long writePointer;
		ResolveTransferEndpoints(readPointer, writePointer);

// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'checked' in this context:
// ORIGINAL LINE: int transferByteCount = checked((int)ResolveTransferByteCount(elemSize));
		int transferByteCount = static_cast<int>(ResolveTransferByteCount(elemSize));
		if (_state == ExecutionState::LoadingOperands && (getOpCode() != _acceptedOpcode || readPointer != _acceptedSourceAddress || writePointer != _acceptedDestinationAddress || getInstruction().getStreamLength() != _acceptedElementCount || elemSize != _acceptedElementSize || stride != _acceptedStride))
		{
			if (_requestController != nullptr && _controllerRequestId && _controllerRequestId.value() != nullptr && _controllerRequestId.value()->isValid())
			{
				_requestController->TryCancel(*_controllerRequestId.value());
			}
			ClearControllerRequestState(true);
			throw InvalidOperationException(L"VectorTransferMicroOp transfer shape changed after controller admission.");
		}
		if (_transferBuffer == nullptr)
		{
			_transferBuffer = std::make_shared<std::vector<unsigned char>>(static_cast<std::size_t>(transferByteCount));
		}
		if (_transferBuffer->size() != static_cast<std::size_t>(transferByteCount))
		{
			if (_state == ExecutionState::LoadingOperands)
			{
				throw InvalidOperationException(L"VectorTransferMicroOp packed byte count changed after controller admission.");
			}
			_transferBuffer = std::make_shared<std::vector<unsigned char>>(static_cast<std::size_t>(transferByteCount));
		}

		if (_state == ExecutionState::NotStarted)
		{
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: MemoryCycleController? controller = core.GetBoundMemorySubsystem() == null ? null : core.GetBoundMemorySubsystem().CycleController;
			std::shared_ptr<MemoryCycleController> controller = core->GetBoundMemorySubsystem() == nullptr ? nullptr : core->GetBoundMemorySubsystem()->getCycleController();
			if (controller != nullptr)
			{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'checked' in this context:
// ORIGINAL LINE: MemoryAdmissionResult admission = controller.TryAcceptCanonicalVectorTransfer(OpCode, 0, readPointer, writePointer, Instruction.StreamLength, elemSize, checked((ushort)stride));
				std::shared_ptr<MemoryAdmissionResult> admission = controller->TryAcceptCanonicalVectorTransfer(getOpCode(), 0, readPointer, writePointer, getInstruction().getStreamLength(), elemSize, static_cast<unsigned short>(stride));
				if (admission->Status == MemoryAdmissionStatus::Backpressured)
				{
					_controllerAdmissionBackpressured = true;
					return false;
				}
				if (admission->Status == MemoryAdmissionStatus::Rejected)
				{
					throw InvalidOperationException(admission->Reason.value_or(L"MemoryCycleController rejected a canonical vector transfer without a reason."));
				}

				_requestController = controller;
				_controllerRequestId = std::make_shared<MemoryRequestId>(admission->RequestId);
				_acceptedOpcode = getOpCode();
				_acceptedSourceAddress = readPointer;
				_acceptedDestinationAddress = writePointer;
				_acceptedElementCount = getInstruction().getStreamLength();
				_acceptedElementSize = elemSize;
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'checked' in this context:
// ORIGINAL LINE: _acceptedStride = checked((ushort)stride);
				_acceptedStride = static_cast<unsigned short>(stride);
				_state = ExecutionState::LoadingOperands;
				return false;
			}

			// No-bound-subsystem compatibility remains the eager functional
			// contour explicitly excluded by RF-10.10.
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'checked' in this context:
// ORIGINAL LINE: YAKSys_Hybrid_CPU.Execution.BurstIO.BurstRead(readPointer, _transferBuffer, Instruction.StreamLength, elemSize, checked((ushort)stride));
			YAKSys_Hybrid_CPU::Execution::BurstIO::BurstRead(readPointer, std::span<unsigned char>(*_transferBuffer), getInstruction().getStreamLength(), elemSize, static_cast<unsigned short>(stride));
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'checked' in this context:
// ORIGINAL LINE: YAKSys_Hybrid_CPU.Execution.BurstIO.BurstWrite(writePointer, _transferBuffer, Instruction.StreamLength, elemSize, checked((ushort)stride));
			YAKSys_Hybrid_CPU::Execution::BurstIO::BurstWrite(writePointer, std::span<const unsigned char>(*_transferBuffer), getInstruction().getStreamLength(), elemSize, static_cast<unsigned short>(stride));

			_state = ExecutionState::Complete;
			return true;
		}

		if (_state == ExecutionState::LoadingOperands)
		{
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: MemoryCycleController? currentController = core.GetBoundMemorySubsystem() == null ? null : core.GetBoundMemorySubsystem().CycleController;
			std::shared_ptr<MemoryCycleController> currentController = core->GetBoundMemorySubsystem() == nullptr ? nullptr : core->GetBoundMemorySubsystem()->getCycleController();
			if (_requestController == nullptr || !_controllerRequestId || _controllerRequestId.value() == nullptr || !_controllerRequestId.value()->isValid())
			{
				throw InvalidOperationException(L"VectorTransferMicroOp lost its accepted controller identity.");
			}
			if (_requestController != currentController)
			{
				_requestController->TryCancel(*_controllerRequestId.value());
				ClearControllerRequestState(true);
				throw InvalidOperationException(L"VectorTransferMicroOp controller binding changed while a request was outstanding.");
			}
			std::shared_ptr<MemoryCompletion> completion;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: if (!_requestController.TryTakeCompletion(_controllerRequestId.Value, out MemoryCompletion? completion))
			if (!_requestController->TryTakeCompletion(*_controllerRequestId.value(), completion))
			{
				return false;
			}

			ClearControllerRequestState(false);
			if (completion == nullptr)
			{
				throw PageFaultException(L"Canonical vector transfer completed without a completion payload.", readPointer, false);
			}
			if (!completion->getSucceeded())
			{
				throw PageFaultException(completion->getFailureReason().empty() ? L"Canonical vector transfer source read failed." : completion->getFailureReason(), readPointer, false);
			}
			if (_transferBuffer == nullptr || completion->getData().size() != _transferBuffer->size())
			{
				throw InvalidOperationException(L"VectorTransferMicroOp controller completion payload does not match the accepted packed byte count.");
			}

			const auto completionData = completion->getData();
			std::copy(completionData.begin(), completionData.end(), _transferBuffer->begin());
			_retireEffect = std::make_shared<VectorTransferRetireEffect>(_acceptedOpcode, _acceptedSourceAddress, _acceptedDestinationAddress, _acceptedElementCount, _acceptedElementSize, _acceptedStride, std::span<const unsigned char>(*_transferBuffer));
			_completedThroughController = true;
			_state = ExecutionState::Complete;
			return true;
		}

		return _state == ExecutionState::Complete;
	}

	std::shared_ptr<VectorTransferRetireEffect> VectorTransferMicroOp::CreateRetireEffect()
	{
		if (!getHasControllerRetireEffect())
		{
			throw InvalidOperationException(L"VectorTransferMicroOp has no completed controller effect for selected retirement.");
		}

	// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
	// ORIGINAL LINE: return _retireEffect!;
		return _retireEffect;
	}

	bool VectorTransferMicroOp::CancelPendingControllerRequest()
	{
		bool canceled = _requestController != nullptr && _controllerRequestId && _controllerRequestId.value() != nullptr && _controllerRequestId.value()->isValid() && _requestController->TryCancel(*_controllerRequestId.value());
		ClearControllerRequestState(true);
		return canceled;
	}

	void VectorTransferMicroOp::ClearControllerRequestState(bool resetExecutionState)
	{
		_requestController = nullptr;
		_controllerRequestId = std::nullopt;
		_controllerAdmissionBackpressured = false;
		if (resetExecutionState && _state == ExecutionState::LoadingOperands)
		{
			_state = ExecutionState::NotStarted;
			_transferBuffer = nullptr;
			_retireEffect = nullptr;
			_completedThroughController = false;
		}
	}

	void VectorTransferMicroOp::ThrowIfUnsupportedAddressingContour()
	{
		if (!getInstruction().getIndexed() && !getInstruction().getIs2D())
		{
			return;
		}

		std::wstring addressingContour = getInstruction().getIndexed() && getInstruction().getIs2D() ? L"indexed+2D" : getInstruction().getIndexed() ? L"indexed" : L"2D";
		throw InvalidOperationException(std::format(L"VectorTransferMicroOp.Execute() rejected {} addressing on the authoritative mainline VLOAD/VSTORE carrier contour. ", addressingContour) + L"This carrier only publishes 1D transfer-shape truth and must fail closed instead of hidden compat success on a non-representable surface.");
	}

	unsigned long long VectorTransferMicroOp::ResolveTransferStride(int elemSize)
	{
		return getInstruction().getStride() > 0 ? getInstruction().getStride() : static_cast<unsigned long long>(elemSize);
	}

	unsigned long long VectorTransferMicroOp::ResolveTransferByteCount(int elemSize)
	{
		return getInstruction().getStreamLength() * static_cast<unsigned long long>(elemSize);
	}

	unsigned long long VectorTransferMicroOp::ResolveTransferTotalSize(int elemSize)
	{
		return getInstruction().getStreamLength() * ResolveTransferStride(elemSize);
	}

	void VectorTransferMicroOp::ResolveTransferEndpoints(unsigned long long &readPointer, unsigned long long &writePointer)
	{
		switch (getOpCode())
		{
			case Processor::CPU_Core::IsaOpcodeValues::VLOAD:
				readPointer = getInstruction().getSrc2Pointer();
				writePointer = getInstruction().getDestSrc1Pointer();
				return;

			case Processor::CPU_Core::IsaOpcodeValues::VSTORE:
				readPointer = getInstruction().getDestSrc1Pointer();
				writePointer = getInstruction().getSrc2Pointer();
				return;

			default:
				throw InvalidOperationException(std::format(L"VectorTransferMicroOp received unsupported transfer opcode 0x{:X} on the authoritative VLOAD/VSTORE carrier contour.", getOpCode()));
		}
	}

	VectorConfigOperationKind VConfigMicroOp::getOperationKind() const
	{
		return OperationKind;
	}

	void VConfigMicroOp::setOperationKind(VectorConfigOperationKind value)
	{
		OperationKind = value;
	}

	unsigned short VConfigMicroOp::getSrcReg1ID() const
	{
		return SrcReg1ID;
	}

	void VConfigMicroOp::setSrcReg1ID(unsigned short value)
	{
		SrcReg1ID = value;
	}

	unsigned short VConfigMicroOp::getSrcReg2ID() const
	{
		return SrcReg2ID;
	}

	void VConfigMicroOp::setSrcReg2ID(unsigned short value)
	{
		SrcReg2ID = value;
	}

	unsigned long long VConfigMicroOp::getEncodedVTypeImmediate() const
	{
		return EncodedVTypeImmediate;
	}

	void VConfigMicroOp::setEncodedVTypeImmediate(unsigned long long value)
	{
		EncodedVTypeImmediate = value;
	}

	unsigned long long VConfigMicroOp::getEncodedAvlImmediate() const
	{
		return EncodedAvlImmediate;
	}

	void VConfigMicroOp::setEncodedAvlImmediate(unsigned long long value)
	{
		EncodedAvlImmediate = value;
	}

	VConfigMicroOp::VConfigMicroOp()
		: _resolvedRetireEffect(VectorConfigRetireEffect::Create(VectorConfigOperationKind::None, 0, 0, 0, 0, false, 0))
	{
		// VConfig cannot be stolen (modifies architectural state)
		setStealable(false);
		setHasSideEffects(true);
		setLatency(1);
		setClass(MicroOpClass::Other);
		setResourceMask(ResourceBitset::Zero);

		// ISA v4 Phase 02: vector config is System class, FullSerial serialization
		setInstructionClass(Arch::InstructionClass::System);
		setSerializationClass(Arch::SerializationClass::FullSerial);
		SetHardPinnedPlacement(SlotClass::SystemSingleton, 7);
	}

	CanonicalDecodePublicationMode VConfigMicroOp::getCanonicalDecodePublication() const
	{
		return CanonicalDecodePublicationMode::SelfPublishes;
	}

	bool VConfigMicroOp::Execute(std::shared_ptr<Processor::CPU_Core> &core)
	{
		int vtId = NormalizeExecutionVtId(getOwnerThreadId());
		unsigned long long avl = ResolveApplicationVectorLength(core, vtId);
		unsigned long long vtype = ResolveVType(core, vtId);

		_actualVectorLength = std::min(avl, Processor::CPU_Core::RVV_Config::VLMAX);
		unsigned char tailAgnostic = static_cast<unsigned char>((vtype >> 6) & 0x1);
		unsigned char maskAgnostic = static_cast<unsigned char>((vtype >> 7) & 0x1);

		bool hasRegisterWriteback = HasArchitecturalDestinationRegister();
		std::destroy_at(&_resolvedRetireEffect);
		std::construct_at(&_resolvedRetireEffect, VectorConfigRetireEffect::Create(getOperationKind(), _actualVectorLength, vtype, tailAgnostic, maskAgnostic, hasRegisterWriteback, getDestRegID()));
		return true;
	}

	void VConfigMicroOp::ConfigureForRegisterVType(unsigned char rs1, unsigned char rs2)
	{
		setOperationKind(VectorConfigOperationKind::Vsetvl);
		setSrcReg1ID(rs1);
		setSrcReg2ID(rs2);
		setEncodedAvlImmediate(0);
		setEncodedVTypeImmediate(0);
	}

	void VConfigMicroOp::ConfigureForImmediateVType(VectorConfigOperationKind operationKind, unsigned char rs1, unsigned long long encodedVTypeImmediate)
	{
		setOperationKind(operationKind);
		setSrcReg1ID(rs1);
		setSrcReg2ID(VLIW_Instruction::NoReg);
		setEncodedAvlImmediate(0);
		setEncodedVTypeImmediate(encodedVTypeImmediate);
	}

	void VConfigMicroOp::ConfigureForImmediateAvlAndVType(unsigned long long encodedAvlImmediate, unsigned long long encodedVTypeImmediate)
	{
		setOperationKind(VectorConfigOperationKind::Vsetivli);
		setSrcReg1ID(VLIW_Instruction::NoReg);
		setSrcReg2ID(VLIW_Instruction::NoReg);
		setEncodedAvlImmediate(encodedAvlImmediate);
		setEncodedVTypeImmediate(encodedVTypeImmediate);
	}

	void VConfigMicroOp::InitializeMetadata()
	{
		bool writesRegister = HasArchitecturalDestinationRegister();
		setWritesRegister(writesRegister);

		std::vector<unsigned short> rawReadRegisters;
		switch (getOperationKind())
		{
			case VectorConfigOperationKind::Vsetvl:
				rawReadRegisters = {getSrcReg1ID(), getSrcReg2ID()};
				break;
			case VectorConfigOperationKind::Vsetvli:
				rawReadRegisters = {getSrcReg1ID()};
				break;
			default:
				break;
		}
		setReadRegisters(BuildReadRegisterList(rawReadRegisters));

		setWriteRegisters(writesRegister ? std::vector<int>{static_cast<int>(getDestRegID())} : std::vector<int>{});
		setReadMemoryRanges({});
		setWriteMemoryRanges({});

		setResourceMask(ResourceBitset::Zero);
		for (auto registerId : getReadRegisters())
		{
			ArchRegId register_Keyword;
			setResourceMask(getResourceMask() | (ArchRegId::TryCreate(registerId, register_Keyword) ? ResourceMaskBuilder::ForArchitecturalRegisterRead(std::make_shared<ArchRegId>(register_Keyword)) : ResourceMaskBuilder::ForRegisterRead(registerId)));
		}

		if (writesRegister)
		{
			ArchRegId destinationRegister;
			setResourceMask(getResourceMask() | (ArchRegId::TryCreate(getDestRegID(), destinationRegister) ? ResourceMaskBuilder::ForArchitecturalRegisterWrite(std::make_shared<ArchRegId>(destinationRegister)) : ResourceMaskBuilder::ForRegisterWrite(getDestRegID())));
		}

		PublishExplicitStructuralSafetyMask();
		RefreshAdmissionMetadata(shared_from_this());
	}

	void VConfigMicroOp::RefreshWriteMetadata()
	{
		InitializeMetadata();
	}

	VectorConfigRetireEffect VConfigMicroOp::CreateRetireEffect()
	{
		return _resolvedRetireEffect;
	}

	bool VConfigMicroOp::TryGetPrimaryWriteBackResult(unsigned long long &value)
	{
		value = _actualVectorLength;
		return HasArchitecturalDestinationRegister();
	}

	void VConfigMicroOp::CapturePrimaryWriteBackResult(unsigned long long value)
	{
		_actualVectorLength = value;
	}

	std::wstring VConfigMicroOp::GetDescription()
	{
		const wchar_t *operationName = L"Unknown";
		switch (getOperationKind())
		{
			case VectorConfigOperationKind::Vsetvl: operationName = L"Vsetvl"; break;
			case VectorConfigOperationKind::Vsetvli: operationName = L"Vsetvli"; break;
			case VectorConfigOperationKind::Vsetivli: operationName = L"Vsetivli"; break;
		}
		return std::format(L"{}: VL={}, VTYPE=0x{:X}", operationName, _actualVectorLength, ResolveDescriptionVType());
	}

	std::vector<int> VConfigMicroOp::BuildReadRegisterList(std::vector<unsigned short> &rawRegisters)
	{
		std::vector<int> registers;
		registers.reserve(rawRegisters.size());
		for (unsigned short rawRegister : rawRegisters)
		{
			if (rawRegister == 0 || rawRegister == VLIW_Instruction::NoReg)
			{
				continue;
			}

			registers.push_back(rawRegister);
		}

		return registers;
	}

	bool VConfigMicroOp::HasArchitecturalDestinationRegister()
	{
		return getDestRegID() != 0 && getDestRegID() != VLIW_Instruction::NoReg;
	}

	unsigned long long VConfigMicroOp::ResolveApplicationVectorLength(std::shared_ptr<Processor::CPU_Core> &core, int vtId)
	{
		return getOperationKind() == VectorConfigOperationKind::Vsetivli ? getEncodedAvlImmediate() : ReadUnifiedScalarSourceOperand(core, vtId, getSrcReg1ID());
	}

	unsigned long long VConfigMicroOp::ResolveVType(std::shared_ptr<Processor::CPU_Core> &core, int vtId)
	{
		return getOperationKind() == VectorConfigOperationKind::Vsetvl ? ReadUnifiedScalarSourceOperand(core, vtId, getSrcReg2ID()) : getEncodedVTypeImmediate();
	}

	unsigned long long VConfigMicroOp::ResolveDescriptionVType()
	{
		return getOperationKind() == VectorConfigOperationKind::Vsetvl ? 0 : getEncodedVTypeImmediate();
	}
}
