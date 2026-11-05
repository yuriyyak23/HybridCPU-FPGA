#include "VectorMicroOps.Compute.h"
#include <algorithm>
#include <array>
#include <span>
#include <stdexcept>
#include <type_traits>
#include "../../../State/ExecutionState.h"
#include "../../../Execution/StreamEngine/BurstIO/StreamEngine.BurstIO.h"
#include "../../../Execution/Vector/ALU/YAKSys_Hybrid_CPU.Execution.VectorALU.h"
#include "../../../Diagnostics/DecodeProjectionFaultException.h"
#include "../../../Execution/ExecutionFaultContract.h"
#include "../../../Architecture/Registers/Physical/PhysicalRegisterFile.h"
#include "../../../../../NonRTL/Arch/Compat/HybridCPU_ISE.Arch.VLIW_Instruction.h"
#include "../../../../../Arch/DataType.h"
#include "../../../Execution/Vector/Codec/ElementCodec.h"

using namespace YAKSys_Hybrid_CPU::Arch;
using namespace YAKSys_Hybrid_CPU::Execution;
using namespace YAKSys_Hybrid_CPU::Core::Registers::Retire;
using AddressGen = YAKSys_Hybrid_CPU::Execution::AddressGen;
using PortType = YAKSys_Hybrid_CPU::Execution::PortType;
using namespace HybridCPU_ISE::Arch;

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

		template <typename T>
		T ReadLittleEndian(std::span<const unsigned char> bytes)
		{
			static_assert(std::is_unsigned_v<T>);
			if (bytes.size() < sizeof(T))
				throw std::out_of_range("Vector descriptor field is shorter than its integer width.");

			T value = 0;
			for (std::size_t i = 0; i < sizeof(T); ++i)
				value |= static_cast<T>(bytes[i]) << (i * 8);
			return value;
		}

		void CopyPrefix(std::span<const unsigned char> source, std::span<unsigned char> destination)
		{
			if (source.size() > destination.size())
				throw std::out_of_range("Vector scratch copy exceeds destination span.");
			std::copy(source.begin(), source.end(), destination.begin());
		}
	}

	void VectorBinaryOpMicroOp::InitializeMetadata()
	{
		VectorMicroOp::InitializeMetadata();

		// Binary operations can be stolen
		setMemoryOp(false);
		ThrowIfUnsupportedElementDataTypeForMetadata(L"VectorBinaryOpMicroOp.InitializeMetadata()");

		// Calculate memory ranges based on instruction parameters
		if (getInstruction().getStreamLength() > 0)
		{
			int elemSize = GetElementSize();
			unsigned long long stride = getInstruction().getStride() > 0 ? getInstruction().getStride() : static_cast<unsigned long long>(elemSize);
			unsigned long long totalSize = getInstruction().getStreamLength() * stride;

			// Read from two sources
			setReadMemoryRanges({
				MemoryRange{getInstruction().getDestSrc1Pointer(), totalSize},
				MemoryRange{getInstruction().getSrc2Pointer(), totalSize}});

			// Write to destination (same as first source for in-place operations)
			setWriteMemoryRanges({MemoryRange{getInstruction().getDestSrc1Pointer(), totalSize}});
		}

		RefreshVectorAdmissionMetadata(true, true);
	}

	bool VectorBinaryOpMicroOp::Execute(std::shared_ptr<Processor::CPU_Core> &core)
	{
		if (_state == ExecutionState::NotStarted)
		{
			_totalElements = getInstruction().getStreamLength();
			ThrowIfUnsupportedAddressingContour();
			ThrowIfZeroLengthVectorComputeContour(L"VectorBinaryOpMicroOp.Execute()");

			ThrowIfUnsupportedElementDataType(L"VectorBinaryOpMicroOp.Execute()");

			_state = ExecutionState::LoadingOperands;
		}

		// Process strip-mining chunks
		while (_elementsProcessed < _totalElements)
		{
			unsigned long long remaining = _totalElements - _elementsProcessed;
			unsigned long long chunkSize = GetChunkSize(remaining);

			if (_state == ExecutionState::LoadingOperands)
			{
				LoadChunk(core, chunkSize);
				_state = ExecutionState::Computing;
			}

			if (_state == ExecutionState::Computing)
			{
				ComputeChunk(core, chunkSize);
				_state = ExecutionState::StoringResults;
			}

			if (_state == ExecutionState::StoringResults)
			{
				StoreChunk(core, chunkSize);
				_elementsProcessed += chunkSize;
				_state = ExecutionState::LoadingOperands; // Next chunk
			}
		}

		_state = ExecutionState::Complete;
		return true;
	}

	void VectorBinaryOpMicroOp::ThrowIfUnsupportedAddressingContour()
	{
		if (!getInstruction().getIndexed() && !getInstruction().getIs2D())
		{
			return;
		}

		std::wstring addressingContour = getInstruction().getIndexed() && getInstruction().getIs2D() ? L"indexed+2D" : getInstruction().getIndexed() ? L"indexed" : L"2D";
		throw InvalidOperationException(std::format(L"VectorBinaryOpMicroOp.Execute() rejected {} addressing on the authoritative mainline binary contour. ", addressingContour) + L"This carrier only follows through 1D in-place memory semantics and must fail closed instead of hidden success on a non-representable compat surface.");
	}

	void VectorBinaryOpMicroOp::LoadChunk(std::shared_ptr<Processor::CPU_Core> &core, unsigned long long chunkSize)
	{
		int elemSize = GetElementSize();
		unsigned long long ptrA = getInstruction().getDestSrc1Pointer() + (_elementsProcessed * static_cast<unsigned long long>(elemSize));
		unsigned long long ptrB = getInstruction().getSrc2Pointer() + (_elementsProcessed * static_cast<unsigned long long>(elemSize));
		unsigned short stride = getInstruction().getStride();

		if (stride == 0)
		{
			stride = static_cast<unsigned short>(elemSize);
		}

		// Get scratch buffers
		std::span<unsigned char> bufA = core->GetScratchA();
		std::span<unsigned char> bufB = core->GetScratchB();
		std::span<unsigned char> bufDst = core->GetScratchDst();

		// Use BurstIO for memory transfers
		YAKSys_Hybrid_CPU::Execution::BurstIO::BurstRead(ptrA, bufA, chunkSize, elemSize, stride);
		YAKSys_Hybrid_CPU::Execution::BurstIO::BurstRead(ptrB, bufB, chunkSize, elemSize, stride);

		// Binary vector semantics preserve masked-off destination lanes, and the current
		// in-place carrier aliases destination with source A on the published contour.
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'checked' in this context:
// ORIGINAL LINE: int chunkByteCount = checked((int)(chunkSize * (ulong)elemSize));
		int chunkByteCount = static_cast<int>(chunkSize * static_cast<unsigned long long>(elemSize));
		CopyPrefix(CheckedSubspan(bufA, 0, chunkByteCount), bufDst);
	}

	void VectorBinaryOpMicroOp::ComputeChunk(std::shared_ptr<Processor::CPU_Core> &core, unsigned long long chunkSize)
	{
		int elemSize = GetElementSize();
		std::span<unsigned char> bufA = core->GetScratchA();
		std::span<unsigned char> bufB = core->GetScratchB();
		std::span<unsigned char> bufDst = core->GetScratchDst();

		// Delegate to VectorALU for type-agnostic computation
		VectorALU::ApplyBinary(getOpCode(), getInstruction().getDataTypeValue(), bufA, bufB, bufDst, elemSize, chunkSize, getInstruction().getPredicateMask(), getInstruction().getTailAgnostic(), getInstruction().getMaskAgnostic(), core);
	}

	void VectorBinaryOpMicroOp::StoreChunk(std::shared_ptr<Processor::CPU_Core> &core, unsigned long long chunkSize)
	{
		int elemSize = GetElementSize();
		unsigned long long ptrDst = getInstruction().getDestSrc1Pointer() + (_elementsProcessed * static_cast<unsigned long long>(elemSize));
		unsigned short stride = getInstruction().getStride();

		if (stride == 0)
		{
			stride = static_cast<unsigned short>(elemSize);
		}

		std::span<unsigned char> bufDst = core->GetScratchDst();

		// Use BurstIO for memory transfers
		YAKSys_Hybrid_CPU::Execution::BurstIO::BurstWrite(ptrDst, bufDst, chunkSize, elemSize, stride);
	}

	void VectorUnaryOpMicroOp::InitializeMetadata()
	{
		VectorMicroOp::InitializeMetadata();

		// Unary operations can be stolen
		setMemoryOp(false);
		ThrowIfUnsupportedElementDataTypeForMetadata(L"VectorUnaryOpMicroOp.InitializeMetadata()");

		// Calculate memory ranges based on instruction parameters
		if (getInstruction().getStreamLength() > 0)
		{
			int elemSize = GetElementSize();
			unsigned long long stride = getInstruction().getStride() > 0 ? getInstruction().getStride() : static_cast<unsigned long long>(elemSize);
			unsigned long long totalSize = getInstruction().getStreamLength() * stride;

			// Read from one source
			setReadMemoryRanges({MemoryRange{getInstruction().getDestSrc1Pointer(), totalSize}});

			// Write to destination (same as source for in-place operations)
			setWriteMemoryRanges({MemoryRange{getInstruction().getDestSrc1Pointer(), totalSize}});
		}

		RefreshVectorAdmissionMetadata(true, true);
	}

	bool VectorUnaryOpMicroOp::Execute(std::shared_ptr<Processor::CPU_Core> &core)
	{
		if (_state == ExecutionState::NotStarted)
		{
			_totalElements = getInstruction().getStreamLength();
			ThrowIfUnsupportedAddressingContour();
			ThrowIfZeroLengthVectorComputeContour(L"VectorUnaryOpMicroOp.Execute()");

			ThrowIfUnsupportedElementDataType(L"VectorUnaryOpMicroOp.Execute()");

			_state = ExecutionState::LoadingOperands;
		}

		// Process strip-mining chunks
		while (_elementsProcessed < _totalElements)
		{
			unsigned long long remaining = _totalElements - _elementsProcessed;
			unsigned long long chunkSize = GetChunkSize(remaining);

			if (_state == ExecutionState::LoadingOperands)
			{
				LoadChunk(core, chunkSize);
				_state = ExecutionState::Computing;
			}

			if (_state == ExecutionState::Computing)
			{
				ComputeChunk(core, chunkSize);
				_state = ExecutionState::StoringResults;
			}

			if (_state == ExecutionState::StoringResults)
			{
				StoreChunk(core, chunkSize);
				_elementsProcessed += chunkSize;
				_state = ExecutionState::LoadingOperands;
			}
		}

		_state = ExecutionState::Complete;
		return true;
	}

	void VectorUnaryOpMicroOp::ThrowIfUnsupportedAddressingContour()
	{
		if (!getInstruction().getIndexed() && !getInstruction().getIs2D())
		{
			return;
		}

		std::wstring addressingContour = getInstruction().getIndexed() && getInstruction().getIs2D() ? L"indexed+2D" : getInstruction().getIndexed() ? L"indexed" : L"2D";
		throw InvalidOperationException(std::format(L"VectorUnaryOpMicroOp.Execute() rejected {} addressing on the authoritative mainline unary contour. ", addressingContour) + L"This carrier only follows through 1D in-place memory semantics and must fail closed instead of hidden success on a non-representable compat surface.");
	}

	void VectorUnaryOpMicroOp::LoadChunk(std::shared_ptr<Processor::CPU_Core> &core, unsigned long long chunkSize)
	{
		int elemSize = GetElementSize();
		unsigned long long ptr = getInstruction().getDestSrc1Pointer() + (_elementsProcessed * static_cast<unsigned long long>(elemSize));
		unsigned short stride = getInstruction().getStride();
		if (stride == 0)
		{
			stride = static_cast<unsigned short>(elemSize);
		}

		std::span<unsigned char> bufSrc = core->GetScratchA();
		std::span<unsigned char> bufDst = core->GetScratchDst();
		YAKSys_Hybrid_CPU::Execution::BurstIO::BurstRead(ptr, bufSrc, chunkSize, elemSize, stride);

		// Unary in-place semantics preserve masked-off destination lanes, so seed the
		// destination scratch surface from the current vector contents before ApplyUnary mutates it.
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'checked' in this context:
// ORIGINAL LINE: int chunkByteCount = checked((int)(chunkSize * (ulong)elemSize));
		int chunkByteCount = static_cast<int>(chunkSize * static_cast<unsigned long long>(elemSize));
		CopyPrefix(CheckedSubspan(bufSrc, 0, chunkByteCount), bufDst);
	}

	void VectorUnaryOpMicroOp::ComputeChunk(std::shared_ptr<Processor::CPU_Core> &core, unsigned long long chunkSize)
	{
		int elemSize = GetElementSize();
		std::span<unsigned char> bufSrc = core->GetScratchA();
		std::span<unsigned char> bufDst = core->GetScratchDst();

		VectorALU::ApplyUnary(getOpCode(), getInstruction().getDataTypeValue(), bufSrc, bufDst, elemSize, chunkSize, getInstruction().getPredicateMask(), getInstruction().getTailAgnostic(), getInstruction().getMaskAgnostic(), core);
	}

	void VectorUnaryOpMicroOp::StoreChunk(std::shared_ptr<Processor::CPU_Core> &core, unsigned long long chunkSize)
	{
		int elemSize = GetElementSize();
		unsigned long long ptr = getInstruction().getDestSrc1Pointer() + (_elementsProcessed * static_cast<unsigned long long>(elemSize));
		unsigned short stride = getInstruction().getStride();
		if (stride == 0)
		{
			stride = static_cast<unsigned short>(elemSize);
		}

		std::span<unsigned char> bufDst = core->GetScratchDst();
		YAKSys_Hybrid_CPU::Execution::BurstIO::BurstWrite(ptr, bufDst, chunkSize, elemSize, stride);
	}

	void VectorFmaMicroOp::InitializeMetadata()
	{
		VectorMicroOp::InitializeMetadata();

		// FMA operations can be stolen
		setMemoryOp(false);
		ThrowIfUnsupportedElementDataTypeForMetadata(L"VectorFmaMicroOp.InitializeMetadata()");
		ThrowIfUnsupportedAddressingContour(L"VectorFmaMicroOp.InitializeMetadata()", true);

		// Calculate memory ranges based on instruction parameters
		if (getInstruction().getStreamLength() > 0)
		{
			ResolveTriOperandDescriptor(L"VectorFmaMicroOp.InitializeMetadata()", true);

			int elemSize = GetElementSize();
			unsigned long long destStride = getInstruction().getStride() > 0 ? getInstruction().getStride() : static_cast<unsigned long long>(elemSize);
			unsigned long long totalDestSize = getInstruction().getStreamLength() * destStride;
			unsigned long long totalSrcASize = getInstruction().getStreamLength() * static_cast<unsigned long long>(_srcAStride);
			unsigned long long totalSrcBSize = getInstruction().getStreamLength() * static_cast<unsigned long long>(_srcBStride);

			// Read from accumulator/destination and both descriptor-backed sources.
			setReadMemoryRanges({
				MemoryRange{getInstruction().getDestSrc1Pointer(), totalDestSize},
				MemoryRange{_srcAPointer, totalSrcASize},
				MemoryRange{_srcBPointer, totalSrcBSize}});

			// Write to destination (same as vd)
			setWriteMemoryRanges({MemoryRange{getInstruction().getDestSrc1Pointer(), totalDestSize}});
		}

		RefreshVectorAdmissionMetadata(true, true);
	}

	bool VectorFmaMicroOp::Execute(std::shared_ptr<Processor::CPU_Core> &core)
	{
		if (_state == ExecutionState::NotStarted)
		{
			_totalElements = getInstruction().getStreamLength();
			ThrowIfUnsupportedAddressingContour(L"VectorFmaMicroOp.Execute()", false);
			ThrowIfZeroLengthVectorComputeContour(L"VectorFmaMicroOp.Execute()");
			ThrowIfUnsupportedElementDataType(L"VectorFmaMicroOp.Execute()");
			ResolveTriOperandDescriptor(L"VectorFmaMicroOp.Execute()", false);

			_state = ExecutionState::LoadingOperands;
		}

		while (_elementsProcessed < _totalElements)
		{
			unsigned long long remaining = _totalElements - _elementsProcessed;
			unsigned long long chunkSize = GetChunkSize(remaining);

			if (_state == ExecutionState::LoadingOperands)
			{
				LoadChunk(core, chunkSize);
				_state = ExecutionState::Computing;
			}

			if (_state == ExecutionState::Computing)
			{
				ComputeChunk(core, chunkSize);
				_state = ExecutionState::StoringResults;
			}

			if (_state == ExecutionState::StoringResults)
			{
				StoreChunk(core, chunkSize);
				_elementsProcessed += chunkSize;
				_state = _elementsProcessed >= _totalElements ? ExecutionState::Complete : ExecutionState::LoadingOperands;
			}
		}

		return _state == ExecutionState::Complete;
	}

	void VectorFmaMicroOp::LoadChunk(std::shared_ptr<Processor::CPU_Core> &core, unsigned long long chunkSize)
	{
		int elemSize = GetElementSize();
		unsigned long long destStride = getInstruction().getStride() > 0 ? getInstruction().getStride() : static_cast<unsigned long long>(elemSize);
		unsigned long long ptrDst = getInstruction().getDestSrc1Pointer() + (_elementsProcessed * destStride);
		unsigned long long ptrSrcA = _srcAPointer + (_elementsProcessed * static_cast<unsigned long long>(_srcAStride));
		unsigned long long ptrSrcB = _srcBPointer + (_elementsProcessed * static_cast<unsigned long long>(_srcBStride));

		std::span<unsigned char> bufDst = core->GetScratchDst();
		std::span<unsigned char> bufSrcA = core->GetScratchA();
		std::span<unsigned char> bufSrcB = core->GetScratchB();

		YAKSys_Hybrid_CPU::Execution::BurstIO::BurstRead(ptrDst, bufDst, chunkSize, elemSize, static_cast<unsigned short>(destStride));
		YAKSys_Hybrid_CPU::Execution::BurstIO::BurstRead(ptrSrcA, bufSrcA, chunkSize, elemSize, _srcAStride);
		YAKSys_Hybrid_CPU::Execution::BurstIO::BurstRead(ptrSrcB, bufSrcB, chunkSize, elemSize, _srcBStride);
	}

	void VectorFmaMicroOp::ComputeChunk(std::shared_ptr<Processor::CPU_Core> &core, unsigned long long chunkSize)
	{
		int elemSize = GetElementSize();
		std::span<unsigned char> bufDst = core->GetScratchDst();
		std::span<unsigned char> bufSrcA = core->GetScratchA();
		std::span<unsigned char> bufSrcB = core->GetScratchB();

		VectorALU::ApplyFMA(getOpCode(), getInstruction().getDataTypeValue(), bufDst, bufSrcA, bufSrcB, elemSize, chunkSize, getInstruction().getPredicateMask(), getInstruction().getTailAgnostic(), getInstruction().getMaskAgnostic(), core);
	}

	void VectorFmaMicroOp::StoreChunk(std::shared_ptr<Processor::CPU_Core> &core, unsigned long long chunkSize)
	{
		int elemSize = GetElementSize();
		unsigned long long destStride = getInstruction().getStride() > 0 ? getInstruction().getStride() : static_cast<unsigned long long>(elemSize);
		unsigned long long ptrDst = getInstruction().getDestSrc1Pointer() + (_elementsProcessed * destStride);

		std::span<unsigned char> bufDst = core->GetScratchDst();
		YAKSys_Hybrid_CPU::Execution::BurstIO::BurstWrite(ptrDst, bufDst, chunkSize, elemSize, static_cast<unsigned short>(destStride));
	}

	void VectorFmaMicroOp::ResolveTriOperandDescriptor(const std::wstring &surface, bool isPublicationContour)
	{
		if (_descriptorResolved)
		{
			return;
		}

		if (getInstruction().getSrc2Pointer() == 0)
		{
			std::wstring contour = isPublicationContour ? L"publication" : L"runtime";
			std::wstring message = std::format(L"{} rejected descriptor-less tri-operand FMA {} on the authoritative descriptor-backed contour. ", surface, contour) + L"Current VFM* encoding requires Src2Pointer to reference a TriOpDesc, so this carrier must fail closed instead of reviving the legacy descriptor-less side path.";
			if (isPublicationContour)
			{
				throw DecodeProjectionFaultException(message);
			}

			throw InvalidOperationException(message);
		}

		int elemSize = GetElementSize();
		TriOpDesc desc = ReadTriOpDesc(getInstruction().getSrc2Pointer());
		_srcAPointer = desc.SrcA;
		_srcBPointer = desc.SrcB;
		_srcAStride = desc.StrideA != 0 ? desc.StrideA : static_cast<unsigned short>(elemSize);
		_srcBStride = desc.StrideB != 0 ? desc.StrideB : static_cast<unsigned short>(elemSize);
		_descriptorResolved = true;
	}

	TriOpDesc VectorFmaMicroOp::ReadTriOpDesc(unsigned long long descriptorAddr)
	{
		std::array<unsigned char, 20> descStorage{};
		std::span<unsigned char> descBuf(descStorage);
		YAKSys_Hybrid_CPU::Execution::BurstIO::BurstRead(descriptorAddr, descBuf, 1, 20, 20);

		TriOpDesc desc;
		desc.SrcA = ReadLittleEndian<unsigned long long>(CheckedSubspan(descBuf, 0, 8));
		desc.SrcB = ReadLittleEndian<unsigned long long>(CheckedSubspan(descBuf, 8, 8));
		desc.StrideA = ReadLittleEndian<unsigned short>(CheckedSubspan(descBuf, 16, 2));
		desc.StrideB = ReadLittleEndian<unsigned short>(CheckedSubspan(descBuf, 18, 2));
		return desc;
	}

	void VectorFmaMicroOp::ThrowIfUnsupportedAddressingContour(const std::wstring &surface, bool isPublicationContour)
	{
		if (!getInstruction().getIndexed() && !getInstruction().getIs2D())
		{
			return;
		}

		std::wstring addressingContour = getInstruction().getIndexed() && getInstruction().getIs2D() ? L"indexed+2D" : getInstruction().getIndexed() ? L"indexed" : L"2D";
		std::wstring contour = isPublicationContour ? L"publication" : L"runtime";
		throw InvalidOperationException(std::format(L"{} rejected unsupported {} vector-FMA {} on the authoritative descriptor-backed tri-operand contour. ", surface, addressingContour, contour) + L"This carrier does not publish indexed/2D follow-through truth and must fail closed instead of hidden compat success on a non-representable surface.");
	}

	void VectorReductionMicroOp::InitializeMetadata()
	{
		VectorMicroOp::InitializeMetadata();

		// Reduction operations can be stolen
		setMemoryOp(false);
		ThrowIfUnsupportedElementDataTypeForMetadata(L"VectorReductionMicroOp.InitializeMetadata()");

		// Calculate memory ranges based on instruction parameters
		if (getInstruction().getStreamLength() > 0)
		{
			int elemSize = GetElementSize();
			unsigned long long stride = getInstruction().getStride() > 0 ? getInstruction().getStride() : static_cast<unsigned long long>(elemSize);
			unsigned long long totalSize = getInstruction().getStreamLength() * stride;

			// Read from entire vector
			setReadMemoryRanges({MemoryRange{getInstruction().getDestSrc1Pointer(), totalSize}});

			// Write only one scalar element (element 0)
			setWriteMemoryRanges({MemoryRange{getInstruction().getDestSrc1Pointer(), static_cast<unsigned long long>(elemSize)}});
		}

		RefreshVectorAdmissionMetadata(true, true);
	}

	bool VectorReductionMicroOp::Execute(std::shared_ptr<Processor::CPU_Core> &core)
	{
		if (_state == ExecutionState::NotStarted)
		{
			_totalElements = getInstruction().getStreamLength();
			ThrowIfUnsupportedAddressingContour();
			ThrowIfZeroLengthVectorComputeContour(L"VectorReductionMicroOp.Execute()");

			ThrowIfUnsupportedElementDataType(L"VectorReductionMicroOp.Execute()");

			_state = ExecutionState::LoadingOperands;
		}

		// For reductions, we process the entire vector as a single chunk
		// since we need to accumulate across all elements
		if (_state == ExecutionState::LoadingOperands)
		{
			LoadAllElements(core);
			_state = ExecutionState::Computing;
		}

		if (_state == ExecutionState::Computing)
		{
			ComputeReduction(core);
			_state = ExecutionState::StoringResults;
		}

		if (_state == ExecutionState::StoringResults)
		{
			StoreScalarResult(core);
			_state = ExecutionState::Complete;
		}

		return true;
	}

	void VectorReductionMicroOp::ThrowIfUnsupportedAddressingContour()
	{
		if (!getInstruction().getIndexed() && !getInstruction().getIs2D())
		{
			return;
		}

		std::wstring addressingContour = getInstruction().getIndexed() && getInstruction().getIs2D() ? L"indexed+2D" : getInstruction().getIndexed() ? L"indexed" : L"2D";
		throw InvalidOperationException(std::format(L"VectorReductionMicroOp.Execute() rejected {} addressing on the authoritative mainline reduction contour. ", addressingContour) + L"This carrier only follows through 1D scalar-footprint memory semantics and must fail closed instead of hidden success on a non-representable compat surface.");
	}

	void VectorReductionMicroOp::LoadAllElements(std::shared_ptr<Processor::CPU_Core> &core)
	{
		int elemSize = GetElementSize();
		unsigned long long ptr = getInstruction().getDestSrc1Pointer();
		unsigned short stride = getInstruction().getStride();
		if (stride == 0)
		{
			stride = static_cast<unsigned short>(elemSize);
		}

		std::span<unsigned char> bufSrc = core->GetScratchA();
		YAKSys_Hybrid_CPU::Execution::BurstIO::BurstRead(ptr, bufSrc, _totalElements, elemSize, stride);
	}

	void VectorReductionMicroOp::ComputeReduction(std::shared_ptr<Processor::CPU_Core> &core)
	{
		int elemSize = GetElementSize();
		std::span<unsigned char> bufSrc = core->GetScratchA();
		std::span<unsigned char> bufDst = core->GetScratchDst();

		// ApplyReduction writes result to element 0 of destination
		VectorALU::ApplyReduction(getOpCode(), getInstruction().getDataTypeValue(), bufSrc, bufDst, elemSize, _totalElements, getInstruction().getPredicateMask(), core);
	}

	void VectorReductionMicroOp::StoreScalarResult(std::shared_ptr<Processor::CPU_Core> &core)
	{
		int elemSize = GetElementSize();
		unsigned long long ptrDst = getInstruction().getDestSrc1Pointer();

		std::span<unsigned char> bufDst = core->GetScratchDst();

		// Write only the first element (scalar result)
		YAKSys_Hybrid_CPU::Execution::BurstIO::BurstWrite(ptrDst, bufDst, 1, elemSize, static_cast<unsigned short>(elemSize));
	}

	void VectorComparisonMicroOp::InitializeMetadata()
	{
		VectorMicroOp::InitializeMetadata();

		setMemoryOp(false);
		setHasSideEffects(true);
		ThrowIfUnsupportedElementDataTypeForMetadata(L"VectorComparisonMicroOp.InitializeMetadata()");

		// Calculate memory ranges based on instruction parameters
		if (getInstruction().getStreamLength() > 0)
		{
			int elemSize = GetElementSize();
			unsigned long long stride = getInstruction().getStride() > 0 ? getInstruction().getStride() : static_cast<unsigned long long>(elemSize);
			unsigned long long totalSize = getInstruction().getStreamLength() * stride;

			// Comparison reads vector operands from memory but publishes only
			// predicate-state follow-through on the authoritative mainline contour.
			setReadMemoryRanges({
				MemoryRange{getInstruction().getDestSrc1Pointer(), totalSize},
				MemoryRange{getInstruction().getSrc2Pointer(), totalSize}});

			setWriteMemoryRanges({});
		}

		RefreshVectorAdmissionMetadata(true, false);
	}

	bool VectorComparisonMicroOp::Execute(std::shared_ptr<Processor::CPU_Core> &core)
	{
		if (_state == ExecutionState::Complete)
		{
			return true;
		}

		if (_state == ExecutionState::NotStarted)
		{
			_totalElements = getInstruction().getStreamLength();
			ThrowIfUnsupportedAddressingContour();
			if (_totalElements == 0)
			{
				throw InvalidOperationException(L"VectorComparisonMicroOp.Execute() rejected StreamLength == 0 on the authoritative mainline predicate-publication contour instead of hidden success/no-op.");
			}

			int elemSize = GetElementSize();
			if (elemSize == 0)
			{
				throw ExecutionFaultContract::CreateUnsupportedVectorElementTypeException(std::format(L"VectorComparisonMicroOp.Execute() rejected unsupported element DataType 0x{:02X} on the authoritative mainline predicate-publication contour instead of hidden success/no-op.", getInstruction().getDataType()));
			}

			_elementsProcessed = 0;
			_pendingChunkResultMask = 0;
			_accumulatedResultMask = 0;
			_state = ExecutionState::LoadingOperands;
		}

		// Process strip-mining chunks
		while (_elementsProcessed < _totalElements)
		{
			unsigned long long remaining = _totalElements - _elementsProcessed;
			unsigned long long chunkSize = GetChunkSize(remaining);

			if (_state == ExecutionState::LoadingOperands)
			{
				LoadChunk(core, chunkSize);
				_state = ExecutionState::Computing;
			}

			if (_state == ExecutionState::Computing)
			{
				ComputeChunk(core, chunkSize);
				_state = ExecutionState::StoringResults;
			}

			if (_state == ExecutionState::StoringResults)
			{
				StoreChunk(core, chunkSize);
				_elementsProcessed += chunkSize;
				_state = ExecutionState::LoadingOperands;
			}
		}

		PublishPredicateMask(core);
		_state = ExecutionState::Complete;
		return true;
	}

	void VectorComparisonMicroOp::ThrowIfUnsupportedAddressingContour()
	{
		if (!getInstruction().getIndexed() && !getInstruction().getIs2D())
		{
			return;
		}

		std::wstring addressingContour = getInstruction().getIndexed() && getInstruction().getIs2D() ? L"indexed+2D" : getInstruction().getIndexed() ? L"indexed" : L"2D";
		throw InvalidOperationException(std::format(L"VectorComparisonMicroOp.Execute() rejected {} addressing on the authoritative mainline predicate-publication contour. ", addressingContour) + L"This carrier only follows through 1D predicate-state truth and must fail closed instead of hidden compat success on a non-representable surface.");
	}

	void VectorComparisonMicroOp::LoadChunk(std::shared_ptr<Processor::CPU_Core> &core, unsigned long long chunkSize)
	{
		int elemSize = GetElementSize();
		unsigned long long ptrA = getInstruction().getDestSrc1Pointer() + (_elementsProcessed * static_cast<unsigned long long>(elemSize));
		unsigned long long ptrB = getInstruction().getSrc2Pointer() + (_elementsProcessed * static_cast<unsigned long long>(elemSize));
		unsigned short stride = getInstruction().getStride();
		if (stride == 0)
		{
			stride = static_cast<unsigned short>(elemSize);
		}

		std::span<unsigned char> bufA = core->GetScratchA();
		std::span<unsigned char> bufB = core->GetScratchB();

		YAKSys_Hybrid_CPU::Execution::BurstIO::BurstRead(ptrA, bufA, chunkSize, elemSize, stride);
		YAKSys_Hybrid_CPU::Execution::BurstIO::BurstRead(ptrB, bufB, chunkSize, elemSize, stride);
	}

	void VectorComparisonMicroOp::ComputeChunk(std::shared_ptr<Processor::CPU_Core> &core, unsigned long long chunkSize)
	{
		int elemSize = GetElementSize();
		std::span<unsigned char> bufA = core->GetScratchA();
		std::span<unsigned char> bufB = core->GetScratchB();

		_pendingChunkResultMask = VectorALU::ApplyComparison(getOpCode(), getInstruction().getDataTypeValue(), bufA, bufB, elemSize, chunkSize, static_cast<unsigned char>(getInstruction().getPredicateMask()), core);
	}

	void VectorComparisonMicroOp::StoreChunk(std::shared_ptr<Processor::CPU_Core> &core, unsigned long long chunkSize)
	{
		int shift = static_cast<int>(std::min(_elementsProcessed, 64ULL));
		if (shift < 64)
		{
			unsigned long long shiftedMask = shift == 0 ? _pendingChunkResultMask : _pendingChunkResultMask << shift;
			_accumulatedResultMask |= shiftedMask;
		}
	}

	void VectorComparisonMicroOp::PublishPredicateMask(std::shared_ptr<Processor::CPU_Core> &core)
	{
		unsigned char destPredReg = static_cast<unsigned char>(getInstruction().getImmediate() & 0x0F);
		core->SetPredicateRegister(destPredReg, _accumulatedResultMask);
	}

	void VectorMaskOpMicroOp::InitializeMetadata()
	{
		VectorMicroOp::InitializeMetadata();

		setMemoryOp(false);
		setHasSideEffects(true);
		setReadMemoryRanges({});
		setWriteMemoryRanges({});

		RefreshVectorAdmissionMetadata(false, false);
	}

	bool VectorMaskOpMicroOp::Execute(std::shared_ptr<Processor::CPU_Core> &core)
	{
		if (_state == ExecutionState::Complete)
		{
			return true;
		}

		if (_state == ExecutionState::NotStarted)
		{
			_totalElements = getInstruction().getStreamLength();
			ThrowIfUnsupportedAddressingContour();
			if (_totalElements == 0)
			{
				throw InvalidOperationException(L"VectorMaskOpMicroOp.Execute() rejected StreamLength == 0 on the authoritative mainline predicate-publication contour instead of hidden success/no-op.");
			}

			_state = ExecutionState::Computing;
		}

		if (getOpCode() == Processor::CPU_Core::IsaOpcodeValues::VPOPC)
		{
			throw InvalidOperationException(L"VectorMaskOpMicroOp.Execute() rejected scalar-writing VPOPC on the authoritative mainline predicate-publication contour; the opcode still requires dedicated scalar retire/apply follow-through instead of hidden predicate-only success.");
		}

		ExecutePredicateMaskManipulation(core);
		_state = ExecutionState::Complete;
		return true;
	}

	void VectorMaskOpMicroOp::ThrowIfUnsupportedAddressingContour()
	{
		if (!getInstruction().getIndexed() && !getInstruction().getIs2D())
		{
			return;
		}

		std::wstring addressingContour = getInstruction().getIndexed() && getInstruction().getIs2D() ? L"indexed+2D" : getInstruction().getIndexed() ? L"indexed" : L"2D";
		throw InvalidOperationException(std::format(L"VectorMaskOpMicroOp.Execute() rejected {} addressing on the authoritative mainline predicate-publication contour. ", addressingContour) + L"This carrier does not materialize any addressed vector surface and must fail closed instead of hidden compat success on an addressing-tag surface.");
	}

	void VectorMaskOpMicroOp::ExecutePredicateMaskManipulation(std::shared_ptr<Processor::CPU_Core> &core)
	{
		unsigned char srcPred1 = static_cast<unsigned char>((getInstruction().getImmediate() >> 0) & 0x0F);
		unsigned char srcPred2 = static_cast<unsigned char>((getInstruction().getImmediate() >> 4) & 0x0F);
		unsigned char destPred = static_cast<unsigned char>((getInstruction().getImmediate() >> 8) & 0x0F);

		unsigned long long mask1 = core->GetPredicateRegister(srcPred1);

	// C# TO C++ CONVERTER TASK: The following 'switch expression' was not converted by C# to C++ Converter:
	//			ulong resultMask = OpCode switch
	//			{
	//				Processor.CPU_Core.IsaOpcodeValues.VMAND => VectorALU.ApplyMaskBinary(
	//						OpCode, mask1, core.GetPredicateRegister(srcPred2)), Processor.CPU_Core.IsaOpcodeValues.VMOR => VectorALU.ApplyMaskBinary(
	//						OpCode,
	//						mask1,
	//						core.GetPredicateRegister(srcPred2)),
	//				Processor.CPU_Core.IsaOpcodeValues.VMXOR =>
	//					VectorALU.ApplyMaskBinary(
	//						OpCode,
	//						mask1,
	//						core.GetPredicateRegister(srcPred2)),
	//				Processor.CPU_Core.IsaOpcodeValues.VMNOT =>
	//					VectorALU.ApplyMaskUnary(OpCode, mask1),
	//				Processor.CPU_Core.IsaOpcodeValues.VMSBF =>
	//					ResolveMaskSetBeforeFirstResult(mask1),
	//				_ => throw new InvalidOperationException(
	//					$"VectorMaskOpMicroOp.Execute() rejected unsupported opcode 0x{OpCode:X} on the authoritative mainline predicate-publication contour.")
	//			};

		unsigned long long resultMask = 0;
		switch (getOpCode())
		{
			case Processor::CPU_Core::IsaOpcodeValues::VMAND:
			case Processor::CPU_Core::IsaOpcodeValues::VMOR:
			case Processor::CPU_Core::IsaOpcodeValues::VMXOR:
				resultMask = VectorALU::ApplyMaskBinary(getOpCode(), mask1, core->GetPredicateRegister(srcPred2));
				break;
			case Processor::CPU_Core::IsaOpcodeValues::VMNOT:
				resultMask = VectorALU::ApplyMaskUnary(getOpCode(), mask1);
				break;
			case Processor::CPU_Core::IsaOpcodeValues::VMSBF:
				resultMask = ResolveMaskSetBeforeFirstResult(mask1);
				break;
			default:
				throw InvalidOperationException(std::format(L"VectorMaskOpMicroOp.Execute() rejected unsupported opcode 0x{:X} on the authoritative mainline predicate-publication contour.", getOpCode()));
		}

		core->SetPredicateRegister(destPred, resultMask);
	}

	unsigned long long VectorMaskOpMicroOp::ResolveMaskSetBeforeFirstResult(unsigned long long mask)
	{
		unsigned char reservedSrcPred2 = static_cast<unsigned char>((getInstruction().getImmediate() >> 4) & 0x0F);
		if (reservedSrcPred2 != 0)
		{
			throw InvalidOperationException(L"VectorMaskOpMicroOp.Execute() rejected VMSBF with a non-zero reserved src2 predicate nibble. "
				L"The Phase 04 contour is the canonical unary mask-prefix ABI only.");
		}

		unsigned long long vl = std::min(_totalElements, static_cast<unsigned long long>(Processor::CPU_Core::RVV_Config::VLMAX));
		return VectorALU::ApplyMaskSetBeforeFirst(mask, vl);
	}

	void VectorMaskPopCountMicroOp::InitializeMetadata()
	{
		VectorMicroOp::InitializeMetadata();

		setMemoryOp(false);
		setReadMemoryRanges({});
		setWriteMemoryRanges({});

		unsigned short destRegId = static_cast<unsigned short>((getInstruction().getImmediate() >> 8) & 0x0F);
		if (destRegId < YAKSys_Hybrid_CPU::Core::Registers::RenameMap::ArchRegs)
		{
			setDestRegID(destRegId);
			setWritesRegister(true);
			setWriteRegisters(std::vector<int> {static_cast<int>(destRegId)});
		}
		else
		{
			setDestRegID(VLIW_Instruction::NoReg);
			setWritesRegister(false);
			setWriteRegisters({});
		}

		RefreshVectorAdmissionMetadata(false, false);
	}

	bool VectorMaskPopCountMicroOp::Execute(std::shared_ptr<Processor::CPU_Core> &core)
	{
		if (_state == ExecutionState::Complete)
		{
			return true;
		}

		if (_state == ExecutionState::NotStarted)
		{
			_totalElements = getInstruction().getStreamLength();
			ThrowIfUnsupportedAddressingContour();
			if (_totalElements == 0)
			{
				throw InvalidOperationException(L"VectorMaskPopCountMicroOp.Execute() rejected StreamLength == 0 on the authoritative mainline scalar-result contour instead of hidden success/no-op.");
			}

			unsigned char srcPred = static_cast<unsigned char>(getInstruction().getImmediate() & 0x0F);
			unsigned long long vl = std::min(static_cast<unsigned long long>(getInstruction().getStreamLength()), static_cast<unsigned long long>(Processor::CPU_Core::RVV_Config::VLMAX));
			_result = VectorALU::MaskPopCount(core->GetPredicateRegister(srcPred), vl);
			_state = ExecutionState::Complete;
		}

		return true;
	}

	void VectorMaskPopCountMicroOp::ThrowIfUnsupportedAddressingContour()
	{
		if (!getInstruction().getIndexed() && !getInstruction().getIs2D())
		{
			return;
		}

		std::wstring addressingContour = getInstruction().getIndexed() && getInstruction().getIs2D() ? L"indexed+2D" : getInstruction().getIndexed() ? L"indexed" : L"2D";
		throw InvalidOperationException(std::format(L"VectorMaskPopCountMicroOp.Execute() rejected {} addressing on the authoritative mainline scalar-result contour. ", addressingContour) + L"This carrier only follows through predicate-to-scalar writeback truth and must fail closed instead of hidden compat success on an addressing-tag surface.");
	}

	void VectorMaskPopCountMicroOp::EmitWriteBackRetireRecords(std::shared_ptr<Processor::CPU_Core> &core, std::span<RetireRecord> retireRecords, int &retireRecordCount)
	{
		if (!getWritesRegister())
		{
			return;
		}

		int vtId = NormalizeExecutionVtId(getOwnerThreadId());
		AppendWriteBackRetireRecord(retireRecords, retireRecordCount, RetireRecord::RegisterWrite(vtId, getDestRegID(), _result));
	}

	bool VectorMaskPopCountMicroOp::TryGetPrimaryWriteBackResult(unsigned long long &value)
	{
		value = _result;
		return getWritesRegister();
	}

	void VectorMaskPopCountMicroOp::CapturePrimaryWriteBackResult(unsigned long long value)
	{
		_result = value;
	}

	void VectorZeroExtendMicroOp::InitializeMetadata()
	{
		VectorMicroOp::InitializeMetadata();
		setMemoryOp(false);
		ThrowIfUnsupportedPublicationContour();

		if (getInstruction().getStreamLength() > 0)
		{
			int sourceElementSize = GetElementSize();
			DataTypeEnum destinationType = ResolveDestinationDataType(getInstruction().getDataTypeValue());
			int destinationElementSize = DataTypeUtils::SizeOf(destinationType);

// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'checked' in this context:
// ORIGINAL LINE: ReadMemoryRanges = [(Instruction.Src2Pointer, checked(Instruction.StreamLength * (ulong)sourceElementSize))];
			setReadMemoryRanges({MemoryRange{getInstruction().getSrc2Pointer(), getInstruction().getStreamLength() * static_cast<unsigned long long>(sourceElementSize)}});
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'checked' in this context:
// ORIGINAL LINE: WriteMemoryRanges = [(Instruction.DestSrc1Pointer, checked(Instruction.StreamLength * (ulong)destinationElementSize))];
			setWriteMemoryRanges({MemoryRange{getInstruction().getDestSrc1Pointer(), getInstruction().getStreamLength() * static_cast<unsigned long long>(destinationElementSize)}});
		}

		RefreshVectorAdmissionMetadata(true, true);
	}

	bool VectorZeroExtendMicroOp::Execute(std::shared_ptr<Processor::CPU_Core> &core)
	{
		if (_state == ExecutionState::Complete)
		{
			return true;
		}

		if (_state == ExecutionState::NotStarted)
		{
			_totalElements = getInstruction().getStreamLength();
			ThrowIfUnsupportedRuntimeContour();
			ThrowIfZeroLengthVectorComputeContour(L"VectorZeroExtendMicroOp.Execute()");

			DataTypeEnum sourceType = getInstruction().getDataTypeValue();
			DataTypeEnum destinationType = ResolveDestinationDataType(sourceType);
			int sourceElementSize = DataTypeUtils::SizeOf(sourceType);
			int destinationElementSize = DataTypeUtils::SizeOf(destinationType);

// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'checked' in this context:
// ORIGINAL LINE: _stagedWrites = new StagedVectorWrite[checked((int)_totalElements)];
			if (_totalElements > static_cast<unsigned long long>(std::numeric_limits<int>::max()))
			{
				throw std::overflow_error("VectorZeroExtendMicroOp element count exceeds the supported staged-write capacity.");
			}
			_stagedWrites.clear();
			_stagedWrites.reserve(static_cast<std::size_t>(_totalElements));
			_stagedWriteCount = 0;

			for (unsigned long long element = 0; element < _totalElements; element++)
			{
				if (!IsLaneActiveForZeroExtend(core, element))
				{
					continue;
				}

// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'checked' in this context:
// ORIGINAL LINE: ulong sourceAddress = checked(Instruction.Src2Pointer + (element * (ulong)sourceElementSize));
				unsigned long long sourceAddress = getInstruction().getSrc2Pointer() + (element * static_cast<unsigned long long>(sourceElementSize));
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'checked' in this context:
// ORIGINAL LINE: ulong destinationAddress = checked(Instruction.DestSrc1Pointer + (element * (ulong)destinationElementSize));
				unsigned long long destinationAddress = getInstruction().getDestSrc1Pointer() + (element * static_cast<unsigned long long>(destinationElementSize));

				std::vector<unsigned char> sourceBuffer(sourceElementSize);
				core->ReadBoundMainMemoryExact(sourceAddress, sourceBuffer, L"VectorZeroExtendMicroOp.Execute()");

				std::vector<unsigned char> destinationBuffer(destinationElementSize);
				unsigned long long value = ElementCodec::LoadU(sourceBuffer, 0, sourceType);
				ElementCodec::StoreU(destinationBuffer, 0, destinationType, value);

				core->ThrowIfBoundMainMemoryRangeUnavailable(destinationAddress, destinationElementSize, L"VectorZeroExtendMicroOp.Execute()");

				_stagedWrites.emplace_back(element, destinationAddress, destinationBuffer);
				++_stagedWriteCount;
			}

			_state = ExecutionState::Complete;
		}

		return true;
	}

	void VectorZeroExtendMicroOp::EmitWriteBackRetireRecords(std::shared_ptr<Processor::CPU_Core> &core, std::span<RetireRecord> retireRecords, int &retireRecordCount)
	{
		PublishStagedWritesAtWriteBack(core);
	}

	void VectorZeroExtendMicroOp::PublishStagedWritesAtWriteBack(std::shared_ptr<Processor::CPU_Core> &core)
	{
		if (_state != ExecutionState::Complete)
		{
			throw InvalidOperationException(L"VectorZeroExtendMicroOp WB publication reached retire without a completed staged VZEXT result.");
		}

		for (int i = 0; i < _stagedWriteCount; i++)
		{
			StagedVectorWrite write = _stagedWrites[i];
			core->ThrowIfBoundMainMemoryRangeUnavailable(write.getAddress(), write.getData().size(), L"VectorZeroExtendMicroOp.EmitWriteBackRetireRecords()");
		}

		for (int i = 0; i < _stagedWriteCount; i++)
		{
			StagedVectorWrite write = _stagedWrites[i];
			std::vector<unsigned char> data = write.getData();
			core->WriteBoundMainMemoryExact(write.getAddress(), data, L"VectorZeroExtendMicroOp.EmitWriteBackRetireRecords()");
		}
	}

	void VectorZeroExtendMicroOp::ThrowIfUnsupportedPublicationContour()
	{
		if (getOpCode() != static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VZEXT) || getInstruction().getOpCode() != static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VZEXT))
		{
			throw DecodeProjectionFaultException(L"VectorZeroExtendMicroOp.InitializeMetadata() can only publish the Phase 05 VZEXT contour.");
		}

		if (getInstruction().getIndexed() || getInstruction().getIs2D())
		{
			std::wstring addressingContour = getInstruction().getIndexed() && getInstruction().getIs2D() ? L"indexed+2D" : getInstruction().getIndexed() ? L"indexed" : L"2D";
			throw DecodeProjectionFaultException(std::format(L"VectorZeroExtendMicroOp.InitializeMetadata() rejected unsupported {} VZEXT publication. ", addressingContour) + L"Phase 05 only opens the packed 1D unsigned zero-extension contour.");
		}

		if (getInstruction().getImmediate() != 0)
		{
			throw DecodeProjectionFaultException(L"VectorZeroExtendMicroOp.InitializeMetadata() rejected non-zero VZEXT immediate sideband.");
		}

		if (getInstruction().getStride() != 0)
		{
			throw DecodeProjectionFaultException(L"VectorZeroExtendMicroOp.InitializeMetadata() rejected non-zero VZEXT stride. "
				L"The selected Phase 05 contour is packed 1D only because one carrier stride cannot describe both source and widened destination element widths.");
		}

		(void)ResolveDestinationDataType(getInstruction().getDataTypeValue());
	}

	void VectorZeroExtendMicroOp::ThrowIfUnsupportedRuntimeContour()
	{
		if (!getInstruction().getIndexed() && !getInstruction().getIs2D() && getInstruction().getImmediate() == 0 && getInstruction().getStride() == 0)
		{
			(void)ResolveDestinationDataType(getInstruction().getDataTypeValue());
			return;
		}

		throw InvalidOperationException(L"VectorZeroExtendMicroOp.Execute() rejected a non-canonical VZEXT runtime contour. "
			L"Only packed 1D unsigned source types UINT8/UINT16/UINT32 are executable.");
	}

	DataTypeEnum VectorZeroExtendMicroOp::ResolveDestinationDataType(DataTypeEnum sourceType)
	{
		switch (sourceType)
		{
			case DataTypeEnum::UINT8: return DataTypeEnum::UINT16;
			case DataTypeEnum::UINT16: return DataTypeEnum::UINT32;
			case DataTypeEnum::UINT32: return DataTypeEnum::UINT64;
			default:
				throw DecodeProjectionFaultException(std::format(L"VZEXT supports only unsigned source DataType UINT8, UINT16, or UINT32; got {}.", static_cast<int>(sourceType)));
		}
	}

	bool VectorZeroExtendMicroOp::IsLaneActiveForZeroExtend(std::shared_ptr<Processor::CPU_Core> &core, unsigned long long element)
	{
		if (element > std::numeric_limits<int>::max())
		{
			return false;
		}

		return core->LaneActive(getInstruction().getPredicateMask(), static_cast<int>(element));
	}

	std::vector<VectorZeroExtendMicroOp::StagedVectorWrite> VectorZeroExtendMicroOp::GetStagedWrites()
	{
		if (_stagedWrites.empty())
		{
			return {};
		}

		if (_stagedWriteCount < 0 || static_cast<size_t>(_stagedWriteCount) > _stagedWrites.size())
		{
			throw InvalidOperationException(L"VectorZeroExtendMicroOp staged-write count is outside its owned buffer.");
		}

		return std::vector<StagedVectorWrite>(_stagedWrites.begin(), _stagedWrites.begin() + _stagedWriteCount);
	}

	VectorZeroExtendMicroOp::StagedVectorWrite::StagedVectorWrite(unsigned long long element, unsigned long long address, std::vector<unsigned char> &data)
		: Element(element), Address(address), Data(data)
	{
	}

	unsigned long long VectorZeroExtendMicroOp::StagedVectorWrite::getElement() const
	{
		return Element;
	}

	unsigned long long VectorZeroExtendMicroOp::StagedVectorWrite::getAddress() const
	{
		return Address;
	}

	std::vector<unsigned char> VectorZeroExtendMicroOp::StagedVectorWrite::getData() const
	{
		return Data;
	}
}
