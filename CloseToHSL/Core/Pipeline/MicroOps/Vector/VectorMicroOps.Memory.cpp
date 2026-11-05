#include "VectorMicroOps.Memory.h"
#include <algorithm>
#include <span>
#include <stdexcept>
#include <type_traits>
#include "../../../../Memory/Subsystem/YAKSys_Hybrid_CPU.Memory.MemorySubsystem.h"
#include "../../../State/ExecutionState.h"
#include "../Types/MicroOp.h"
#include "../../../Execution/StreamEngine/BurstIO/StreamEngine.BurstIO.h"
#include "../Types/MicroOp.Exceptions.h"
#include "../../../../../NonRTL/Arch/InstructionClass.h"
#include "../../../../../NonRTL/Arch/InstructionClassifier.h"
#include "../../../../../NonRTL/Arch/SerializationClass.h"
#include "../../Scheduling/SlotLegality/SlotClassDefinitions.h"
#include "../../../Diagnostics/DecodeProjectionFaultException.h"

using namespace HybridCPU_ISE::CloseToHSL::Memory::Timing;
using namespace YAKSys_Hybrid_CPU::Arch;
using namespace YAKSys_Hybrid_CPU::Core::Registers::Retire;
using namespace YAKSys_Hybrid_CPU::Execution;
using namespace YAKSys_Hybrid_CPU::Memory;
using AddressGen = YAKSys_Hybrid_CPU::Execution::AddressGen;
using PortType = YAKSys_Hybrid_CPU::Execution::PortType;

namespace YAKSys_Hybrid_CPU::Core
{
	namespace
	{
		template <typename T>
		T ReadLittleEndian(std::span<const unsigned char> bytes, std::size_t offset)
		{
			static_assert(std::is_unsigned_v<T>);
			if (offset > bytes.size() || sizeof(T) > bytes.size() - offset)
				throw std::out_of_range("Vector memory descriptor field is outside the available bytes.");

			T value = 0;
			for (std::size_t i = 0; i < sizeof(T); ++i)
				value |= static_cast<T>(bytes[offset + i]) << (i * 8);
			return value;
		}

		std::wstring WidenExceptionText(const std::exception &exception)
		{
			const char *message = exception.what();
			return std::wstring(message, message + std::char_traits<char>::length(message));
		}

		unsigned long long CheckedRangeLength(unsigned long long elementCount, unsigned long long stride, unsigned long long elementSize)
		{
			if (elementCount == 0)
				return 0;
			const unsigned long long precedingElements = elementCount - 1;
			if (stride != 0 && precedingElements > (std::numeric_limits<unsigned long long>::max() - elementSize) / stride)
				throw std::overflow_error("Vector memory range length overflow.");
			return precedingElements * stride + elementSize;
		}

		unsigned long long CheckedAddressEnd(unsigned long long address, unsigned long long length)
		{
			if (length > std::numeric_limits<unsigned long long>::max() - address)
				throw std::overflow_error("Vector memory range end overflow.");
			return address + length;
		}

		unsigned long long CheckedProduct(unsigned long long left, unsigned long long right)
		{
			if (right != 0 && left > std::numeric_limits<unsigned long long>::max() / right)
				throw std::overflow_error("Vector memory address multiplication overflow.");
			return left * right;
		}
	}

	bool LoadSegmentMicroOp::getOwnsPendingMemoryCompletion() const
	{
		return _state == ExecutionState::LoadingOperands && _requestController != nullptr && _controllerRequestId && _controllerRequestId.value() != nullptr && _controllerRequestId.value()->isValid() && _loadBuffer != nullptr && _requestController->OwnsOutstandingVectorSegmentLoad(*_controllerRequestId.value(), 0, getInstruction().getDestSrc1Pointer(), _loadBuffer->size());
	}

	bool LoadSegmentMicroOp::getHasControllerAdmissionBackpressure() const
	{
		return _controllerAdmissionBackpressured && _state == ExecutionState::NotStarted && _requestController == nullptr && !_controllerRequestId;
	}

	LoadSegmentMicroOp::LoadSegmentMicroOp()
	{
		setMemoryOp(true);
	}

	void LoadSegmentMicroOp::InitializeMetadata()
	{
		VectorMicroOp::InitializeMetadata();

		if (getInstruction().getStreamLength() > 0)
		{
			int elemSize = GetElementSize();
			unsigned long long stride = getInstruction().getStride() > 0 ? getInstruction().getStride() : static_cast<unsigned long long>(elemSize);
			unsigned long long totalSize = getInstruction().getStreamLength() * stride;

			// Read from source memory
			setReadMemoryRanges({MemoryRange{getInstruction().getDestSrc1Pointer(), totalSize}});
			setWriteMemoryRanges({});
		}

		RefreshVectorAdmissionMetadata(true, false);
	}

	bool LoadSegmentMicroOp::Execute(std::shared_ptr<Processor::CPU_Core> &core)
	{
		_controllerAdmissionBackpressured = false;
		if (_state == ExecutionState::NotStarted)
		{
			_totalElements = getInstruction().getStreamLength();
			int elemSize = GetElementSize();
			unsigned short stride = getInstruction().getStride();
			if (stride == 0)
			{
				stride = static_cast<unsigned short>(elemSize);
			}

			// Allocate buffer for loaded data
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'checked' in this context:
// ORIGINAL LINE: int totalBytes = checked((int)(_totalElements * (ulong)elemSize));
			int totalBytes = static_cast<int>(_totalElements * static_cast<unsigned long long>(elemSize));
			_loadBuffer = std::make_shared<std::vector<unsigned char>>(totalBytes);

			// A bound subsystem uses the RF-10.6 controller request and
			// completion boundary. The functional fallback remains below.
			std::shared_ptr<MemorySubsystem> memSub = core->GetBoundMemorySubsystem();
			if (memSub != nullptr)
			{
				std::shared_ptr<MemoryCycleController> controller = memSub->getCycleController();
				std::shared_ptr<MemoryAdmissionResult> admission = controller->TryAcceptVectorSegmentLoad(0, getInstruction().getDestSrc1Pointer(), _loadBuffer->size());
				if (admission->Status == MemoryAdmissionStatus::Backpressured)
				{
					_controllerAdmissionBackpressured = true;
					return false;
				}
				if (admission->Status == MemoryAdmissionStatus::Rejected)
				{
					throw InvalidOperationException(admission->Reason.value_or(L"MemoryCycleController rejected a vector-segment load without a reason."));
				}

				_requestController = controller;
				_controllerRequestId = std::make_shared<MemoryRequestId>(admission->RequestId);
				_state = ExecutionState::LoadingOperands;
				return false;
			}
			else
			{
				// Fallback: synchronous load
				YAKSys_Hybrid_CPU::Execution::BurstIO::BurstRead(getInstruction().getDestSrc1Pointer(), std::span<unsigned char>(*_loadBuffer), _totalElements, elemSize, stride);
				_state = ExecutionState::Complete;
				return true;
			}
		}

		if (_state == ExecutionState::LoadingOperands)
		{
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: MemoryCycleController? currentController = core.GetBoundMemorySubsystem() == null ? null : core.GetBoundMemorySubsystem().CycleController;
			std::shared_ptr<MemoryCycleController> currentController = core->GetBoundMemorySubsystem() == nullptr ? nullptr : core->GetBoundMemorySubsystem()->getCycleController();
			if (_requestController == nullptr || !_controllerRequestId || _controllerRequestId.value() == nullptr || !_controllerRequestId.value()->isValid())
			{
				throw InvalidOperationException(L"LoadSegmentMicroOp lost its accepted controller identity while loading operands.");
			}
			if (_requestController != currentController)
			{
				_requestController->TryCancel(*_controllerRequestId.value());
				ClearControllerRequestState(true);
				throw InvalidOperationException(L"LoadSegmentMicroOp controller binding changed while an accepted request was outstanding.");
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
				throw PageFaultException(L"Vector-segment load completed without a completion payload.", getInstruction().getDestSrc1Pointer(), false);
			}
			if (!completion->getSucceeded())
			{
				const std::wstring reason = completion->getFailureReason();
				throw PageFaultException(reason.empty() ? L"Vector-segment source read failed." : reason, getInstruction().getDestSrc1Pointer(), false);
			}
			if (_loadBuffer == nullptr || completion->getData().size() != _loadBuffer->size())
			{
				throw InvalidOperationException(L"LoadSegmentMicroOp controller completion payload does not match the accepted segment byte count.");
			}

			const auto completionData = completion->getData();
			std::copy(completionData.begin(), completionData.end(), _loadBuffer->begin());
			_state = ExecutionState::Complete;
			return true;
		}

		return _state == ExecutionState::Complete;
	}

	std::shared_ptr<std::vector<unsigned char>> LoadSegmentMicroOp::GetLoadedBuffer()
	{
		return _loadBuffer;
	}

	bool LoadSegmentMicroOp::CancelPendingControllerRequest()
	{
		bool canceled = _requestController != nullptr && _controllerRequestId && _controllerRequestId.value() != nullptr && _controllerRequestId.value()->isValid() && _requestController->TryCancel(*_controllerRequestId.value());
		ClearControllerRequestState(true);
		return canceled;
	}

	void LoadSegmentMicroOp::ClearControllerRequestState(bool resetExecutionState)
	{
		_requestController = nullptr;
		_controllerRequestId = std::nullopt;
		_controllerAdmissionBackpressured = false;
		if (resetExecutionState && _state == ExecutionState::LoadingOperands)
		{
			_state = ExecutionState::NotStarted;
			_loadBuffer = nullptr;
		}
	}

	Load2DMicroOp::Load2DMicroOp()
	{
		setMemoryOp(true);
	}

	void Load2DMicroOp::InitializeMetadata()
	{
		VectorMicroOp::InitializeMetadata();

		if (getInstruction().getStreamLength() > 0)
		{
			int elemSize = GetElementSize();
			// 2D addressing: rowStride and colStride from instruction
			// Total memory footprint is complex, use conservative estimate
			unsigned long long estimatedSize = getInstruction().getStreamLength() * static_cast<unsigned long long>(elemSize);
			setReadMemoryRanges({MemoryRange{getInstruction().getDestSrc1Pointer(), estimatedSize}});
			setWriteMemoryRanges({});
		}

		RefreshVectorAdmissionMetadata(true, false);
	}

	bool Load2DMicroOp::Execute(std::shared_ptr<Processor::CPU_Core> &core)
	{
		if (_state == ExecutionState::NotStarted)
		{
			_totalElements = getInstruction().getStreamLength();
			int elemSize = GetElementSize();

			_loadBuffer = std::make_shared<std::vector<unsigned char>>(_totalElements * static_cast<unsigned long long>(elemSize));

			// Use BurstPlanner for 2D addressing
			// For now, delegate to BurstIO.BurstRead2D
			unsigned int rowLength = static_cast<unsigned int>(getInstruction().getImmediate()); // Row length encoded in immediate
			YAKSys_Hybrid_CPU::Execution::BurstIO::BurstRead2D(getInstruction().getDestSrc1Pointer(), std::span<unsigned char>(*_loadBuffer), _totalElements, elemSize, rowLength, getInstruction().getRowStride(), getInstruction().getStride(), 0); // startOffset

			_state = ExecutionState::Complete;
			return true;
		}

		return _state == ExecutionState::Complete;
	}

	std::shared_ptr<std::vector<unsigned char>> Load2DMicroOp::GetLoadedBuffer()
	{
		return _loadBuffer;
	}

	GatherMicroOp::GatherMicroOp()
	{
		setClass(MicroOpClass::Lsu);
		setMemoryOp(false);
		setHasSideEffects(true);
		setStealable(false);
		setInstructionClass(InstructionClass::Memory);
		setSerializationClass(InstructionClassifier::GetSerializationClass(Processor::CPU_Core::InstructionsEnum::VGATHER));
		SetClassFlexiblePlacement(SlotClass::LsuClass);
	}

	void GatherMicroOp::InitializeMetadata()
	{
		VectorMicroOp::InitializeMetadata();
		setClass(MicroOpClass::Lsu);
		setMemoryOp(false);
		setHasSideEffects(true);
		setStealable(false);
		setInstructionClass(InstructionClass::Memory);
		setSerializationClass(InstructionClassifier::GetSerializationClass(Processor::CPU_Core::InstructionsEnum::VGATHER));
		SetClassFlexiblePlacement(SlotClass::LsuClass);
		ThrowIfUnsupportedPublicationContour();
		ThrowIfUnsupportedElementDataTypeForMetadata(L"GatherMicroOp.InitializeMetadata()");

		if (getInstruction().getStreamLength() > 0)
		{
			try
			{
				Indexed2SrcDescriptor descriptor = ResolveDescriptorFromPublishedMemory(L"GatherMicroOp.InitializeMetadata()");
				int elemSize = GetElementSize();
				std::vector<unsigned long long> indices = ReadPublishedIndexVector(descriptor, getInstruction().getStreamLength(), L"GatherMicroOp.InitializeMetadata()");

				setReadMemoryRanges(BuildReadRanges(descriptor, indices, elemSize, getInstruction().getStreamLength()));
				setWriteMemoryRanges({BuildDestinationRange(elemSize, getInstruction().getStreamLength())});
			}
			catch (const DecodeProjectionFaultException &e1)
			{
				throw;
			}
			catch (const std::runtime_error &ex)
			{
				throw DecodeProjectionFaultException(std::format(L"GatherMicroOp.InitializeMetadata() rejected VGATHER indexed publication before MicroOp follow-through: {}", WidenExceptionText(ex)), ex);
			}
		}

		RefreshVectorAdmissionMetadata(true, true);
	}

	bool GatherMicroOp::Execute(std::shared_ptr<Processor::CPU_Core> &core)
	{
		if (_state == ExecutionState::Complete)
		{
			return true;
		}

		if (_state == ExecutionState::NotStarted)
		{
			_totalElements = getInstruction().getStreamLength();
			ThrowIfUnsupportedRuntimeContour();
			ThrowIfZeroLengthVectorComputeContour(L"GatherMicroOp.Execute()");
			ThrowIfUnsupportedElementDataType(L"GatherMicroOp.Execute()");

			int elemSize = GetElementSize();
			_descriptor.emplace(ReadIndexedDescriptor(core, getInstruction().getSrc2Pointer(), L"GatherMicroOp.Execute()"));
			_descriptorResolved = true;

// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'checked' in this context:
// ORIGINAL LINE: _stagedDestinationBuffer = new byte[checked((int)(_totalElements * (ulong)elemSize))];
			_stagedDestinationBuffer = std::make_shared<std::vector<unsigned char>>(static_cast<int>(_totalElements * static_cast<unsigned long long>(elemSize)));
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'checked' in this context:
// ORIGINAL LINE: _indices = new ulong[checked((int)_totalElements)];
			_indices = std::make_shared<std::vector<unsigned long long>>(static_cast<int>(_totalElements));

			for (unsigned long long element = 0; element < _totalElements; element++)
			{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'checked' in this context:
// ORIGINAL LINE: int elementIndex = checked((int)element);
				int elementIndex = static_cast<int>(element);
				unsigned long long index = ReadIndexElement(core, *_descriptor, element, L"GatherMicroOp.Execute()");
				(*_indices)[elementIndex] = index;

// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'checked' in this context:
// ORIGINAL LINE: int destinationOffset = checked((int)(element * (ulong)elemSize));
				int destinationOffset = static_cast<int>(element * static_cast<unsigned long long>(elemSize));
				unsigned long long destinationAddress = GetDestinationAddress(element, elemSize);
				std::vector<unsigned char> elementBuffer(elemSize);

				if (IsLaneActiveForGather(core, element))
				{
					unsigned long long sourceAddress = ResolveSourceAddress(*_descriptor, index, elemSize, L"GatherMicroOp.Execute()");
					core->ReadBoundMainMemoryExact(sourceAddress, elementBuffer, L"GatherMicroOp.Execute()");
				}
				else
				{
					core->ReadBoundMainMemoryExact(destinationAddress, elementBuffer, L"GatherMicroOp.Execute(masked-undisturbed)");
				}

				std::copy_n(elementBuffer.begin(), elemSize, _stagedDestinationBuffer->begin() + destinationOffset);
			}

			_state = ExecutionState::Complete;
			return true;
		}

		return _state == ExecutionState::Complete;
	}

	void GatherMicroOp::EmitWriteBackRetireRecords(std::shared_ptr<Processor::CPU_Core> &core, std::span<RetireRecord> retireRecords, int &retireRecordCount)
	{
		PublishStagedGatherResultAtWriteBack(core);
	}

	void GatherMicroOp::PublishStagedGatherResultAtWriteBack(std::shared_ptr<Processor::CPU_Core> &core)
	{
		if (_state != ExecutionState::Complete || _stagedDestinationBuffer == nullptr)
		{
			throw InvalidOperationException(L"GatherMicroOp WB publication reached retire without a completed staged VGATHER result.");
		}

		int elemSize = GetElementSize();
		for (unsigned long long element = 0; element < _totalElements; element++)
		{
			unsigned long long destinationAddress = GetDestinationAddress(element, elemSize);
			core->ThrowIfBoundMainMemoryRangeUnavailable(destinationAddress, elemSize, L"GatherMicroOp.EmitWriteBackRetireRecords()");
		}

		std::vector<unsigned char> elementBuffer(elemSize);
		for (unsigned long long element = 0; element < _totalElements; element++)
		{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'checked' in this context:
// ORIGINAL LINE: Buffer.BlockCopy(_stagedDestinationBuffer, checked((int)(element * (ulong)elemSize)), elementBuffer, 0, elemSize);
			const auto sourceBegin = _stagedDestinationBuffer->begin() + static_cast<int>(element * static_cast<unsigned long long>(elemSize));
			std::copy_n(sourceBegin, elemSize, elementBuffer.begin());
			core->WriteBoundMainMemoryExact(GetDestinationAddress(element, elemSize), elementBuffer, L"GatherMicroOp.EmitWriteBackRetireRecords()");
		}
	}

	void GatherMicroOp::ThrowIfUnsupportedPublicationContour()
	{
		if (getOpCode() != static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VGATHER) || getInstruction().getOpCode() != static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VGATHER))
		{
			throw DecodeProjectionFaultException(L"GatherMicroOp.InitializeMetadata() can only publish the VGATHER indexed-read contour.");
		}

		if (getInstruction().getIndexed() && !getInstruction().getIs2D())
		{
			return;
		}

		std::wstring addressingContour = getInstruction().getIndexed() && getInstruction().getIs2D() ? L"indexed+2D" : getInstruction().getIs2D() ? L"2D" : L"non-indexed";
		throw DecodeProjectionFaultException(std::format(L"GatherMicroOp.InitializeMetadata() rejected unsupported {} VGATHER publication. ", addressingContour) + L"Phase 01 only opens the 1D indexed-read contour; VSCATTER, 2D, and indexed+2D remain fail-closed.");
	}

	void GatherMicroOp::ThrowIfUnsupportedRuntimeContour()
	{
		if (getOpCode() == static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VGATHER) && getInstruction().getOpCode() == static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VGATHER) && getInstruction().getIndexed() && !getInstruction().getIs2D())
		{
			return;
		}

		std::wstring addressingContour = getInstruction().getIndexed() && getInstruction().getIs2D() ? L"indexed+2D" : getInstruction().getIs2D() ? L"2D" : L"non-indexed";
		throw InvalidOperationException(std::format(L"GatherMicroOp.Execute() rejected unsupported {} VGATHER runtime contour. ", addressingContour) + L"The executable carrier is limited to 1D indexed memory reads.");
	}

	GatherMicroOp::Indexed2SrcDescriptor GatherMicroOp::ResolveDescriptorFromPublishedMemory(const std::wstring &surface)
	{
		if (_descriptorResolved)
		{
			return *_descriptor;
		}

		_descriptor.emplace(ReadIndexedDescriptorFromPublishedMemory(getInstruction().getSrc2Pointer(), surface));
		_descriptorResolved = true;
		return *_descriptor;
	}

	GatherMicroOp::Indexed2SrcDescriptor GatherMicroOp::ReadIndexedDescriptorFromPublishedMemory(unsigned long long descriptorAddress, const std::wstring &surface)
	{
		if (descriptorAddress == 0)
		{
			throw DecodeProjectionFaultException(std::format(L"{} rejected descriptor-less VGATHER publication. Word2 must carry an Indexed2SrcDesc address.", surface));
		}

		std::vector<unsigned char> descriptorBytes(Indexed2SrcDescriptorSize);
		BurstIO::BurstRead(descriptorAddress, descriptorBytes, 1, Indexed2SrcDescriptorSize, Indexed2SrcDescriptorSize, nullptr);
		return ParseIndexedDescriptor(descriptorBytes, descriptorAddress, surface, true);
	}

	GatherMicroOp::Indexed2SrcDescriptor GatherMicroOp::ReadIndexedDescriptor(std::shared_ptr<Processor::CPU_Core> &core, unsigned long long descriptorAddress, const std::wstring &surface)
	{
		if (descriptorAddress == 0)
		{
			throw InvalidOperationException(std::format(L"{} rejected descriptor-less VGATHER runtime execution. Word2 must carry an Indexed2SrcDesc address.", surface));
		}

		std::vector<unsigned char> descriptorBytes(Indexed2SrcDescriptorSize);
		core->ReadBoundMainMemoryExact(descriptorAddress, descriptorBytes, surface);
		return ParseIndexedDescriptor(descriptorBytes, descriptorAddress, surface, false);
	}

	GatherMicroOp::Indexed2SrcDescriptor GatherMicroOp::ParseIndexedDescriptor(std::vector<unsigned char> &descriptorBytes, unsigned long long descriptorAddress, const std::wstring &surface, bool publication)
	{
		if (descriptorBytes.size() != Indexed2SrcDescriptorSize)
		{
				CreateDescriptorException(surface, L"Indexed2SrcDesc must be exactly 32 bytes.", publication);
		}

		unsigned char indexType = descriptorBytes[18];

		if (indexType != Uint32IndexType && indexType != Uint64IndexType)
		{
				CreateDescriptorException(surface, std::format(L"Indexed2SrcDesc IndexType {} is unsupported; only 0=uint32 and 1=uint64 are executable.", static_cast<unsigned int>(indexType)), publication);
		}

		unsigned char indexIsByteOffset = descriptorBytes[19];
		if (indexIsByteOffset > 1)
		{
				CreateDescriptorException(surface, std::format(L"Indexed2SrcDesc IndexIsByteOffset {} is unsupported; only 0/1 are executable.", static_cast<unsigned int>(indexIsByteOffset)), publication);
		}

		for (int i = Indexed2SrcReservedOffset; i < Indexed2SrcDescriptorSize; i++)
		{
			if (descriptorBytes[i] != 0)
			{
				CreateDescriptorException(surface, L"Indexed2SrcDesc reserved bytes 20..31 must be zero for Phase 01 VGATHER.", publication);
			}
		}

		return Indexed2SrcDescriptor(descriptorAddress, ReadLittleEndian<unsigned long long>(descriptorBytes, 0), ReadLittleEndian<unsigned long long>(descriptorBytes, 8), ReadLittleEndian<unsigned short>(descriptorBytes, 16), indexType, indexIsByteOffset != 0);
	}

	void GatherMicroOp::CreateDescriptorException(const std::wstring &surface, const std::wstring &message, bool publication)
	{
		std::wstring fullMessage = std::format(L"{} rejected malformed VGATHER Indexed2SrcDesc: {}", surface, message);
		if (publication)
			throw DecodeProjectionFaultException(fullMessage);
		throw InvalidOperationException(fullMessage);
	}

	std::vector<unsigned long long> GatherMicroOp::ReadPublishedIndexVector(Indexed2SrcDescriptor descriptor, unsigned long long elementCount, const std::wstring &surface)
	{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'checked' in this context:
// ORIGINAL LINE: ulong[] indices = new ulong[checked((int)elementCount)];
		std::vector<unsigned long long> indices(static_cast<int>(elementCount));
		int indexSize = descriptor.getIndexElementSize();
		std::vector<unsigned char> indexBuffer(indexSize);

		for (unsigned long long element = 0; element < elementCount; element++)
		{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'checked' in this context:
// ORIGINAL LINE: int elementIndex = checked((int)element);
			int elementIndex = static_cast<int>(element);
			unsigned long long indexAddress = GetIndexAddress(descriptor, element, surface);
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'checked' in this context:
// ORIGINAL LINE: BurstIO.BurstRead(indexAddress, indexBuffer, elementCount: 1, elementSize: indexSize, stride: checked((ushort)indexSize));
			BurstIO::BurstRead(indexAddress, indexBuffer, 1, indexSize, static_cast<unsigned short>(indexSize), nullptr);
			indices[elementIndex] = indexSize == sizeof(unsigned int) ? ReadLittleEndian<unsigned int>(indexBuffer, 0) : ReadLittleEndian<unsigned long long>(indexBuffer, 0);
		}

		return indices;
	}

	unsigned long long GatherMicroOp::ReadIndexElement(std::shared_ptr<Processor::CPU_Core> &core, Indexed2SrcDescriptor descriptor, unsigned long long element, const std::wstring &surface)
	{
		int indexSize = descriptor.getIndexElementSize();
		std::vector<unsigned char> indexBuffer(indexSize);
		core->ReadBoundMainMemoryExact(GetIndexAddress(descriptor, element, surface), indexBuffer, surface);
		return indexSize == sizeof(unsigned int) ? ReadLittleEndian<unsigned int>(indexBuffer, 0) : ReadLittleEndian<unsigned long long>(indexBuffer, 0);
	}

	std::vector<MicroOp::MemoryRange> GatherMicroOp::BuildReadRanges(Indexed2SrcDescriptor descriptor, const std::vector<unsigned long long> &indices, int elemSize, unsigned long long elementCount)
	{
		std::vector<MemoryRange> ranges;
		ranges.reserve(3);
		ranges.push_back({descriptor.getDescriptorAddress(), Indexed2SrcDescriptorSize});
		ranges.push_back({descriptor.getIndexBase(), CheckedRangeLength(elementCount, descriptor.getEffectiveIndexStride(), static_cast<unsigned long long>(descriptor.getIndexElementSize()))});

		if (indices.empty())
		{
			ranges.push_back({descriptor.getSourceBase(), 0});
			return ranges;
		}

		unsigned long long minimumAddress = std::numeric_limits<unsigned long long>::max();
		unsigned long long maximumEnd = 0;
		for (unsigned long long index : indices)
		{
			const unsigned long long sourceAddress = ResolveSourceAddress(descriptor, index, elemSize, L"GatherMicroOp.InitializeMetadata()");
			minimumAddress = std::min(minimumAddress, sourceAddress);
			maximumEnd = std::max(maximumEnd, CheckedAddressEnd(sourceAddress, static_cast<unsigned long long>(elemSize)));
		}
		ranges.push_back({minimumAddress, maximumEnd - minimumAddress});
		return ranges;
	}

	MicroOp::MemoryRange GatherMicroOp::BuildDestinationRange(int elemSize, unsigned long long elementCount)
	{
		return {getInstruction().getDestSrc1Pointer(), CheckedRangeLength(elementCount, GetDestinationStride(elemSize), static_cast<unsigned long long>(elemSize))};
	}

	unsigned long long GatherMicroOp::GetIndexAddress(Indexed2SrcDescriptor descriptor, unsigned long long element, const std::wstring &surface)
	{
		try
		{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'checked' in this context:
// ORIGINAL LINE: return checked(descriptor.IndexBase + (element * descriptor.EffectiveIndexStride));
			return CheckedAddressEnd(descriptor.getIndexBase(), CheckedProduct(element, descriptor.getEffectiveIndexStride()));
		}
		catch (const std::overflow_error &ex)
		{
			std::throw_with_nested(InvalidOperationException(std::format(L"{} rejected VGATHER index address overflow: {}", surface, WidenExceptionText(ex))));
		}
	}

	unsigned long long GatherMicroOp::GetDestinationAddress(unsigned long long element, int elemSize)
	{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'checked' in this context:
// ORIGINAL LINE: return checked(Instruction.DestSrc1Pointer + (element * GetDestinationStride(elemSize)));
		return CheckedAddressEnd(getInstruction().getDestSrc1Pointer(), CheckedProduct(element, GetDestinationStride(elemSize)));
	}

	unsigned long long GatherMicroOp::GetDestinationStride(int elemSize)
	{
		return getInstruction().getStride() > 0 ? getInstruction().getStride() : static_cast<unsigned long long>(elemSize);
	}

	unsigned long long GatherMicroOp::ResolveSourceAddress(Indexed2SrcDescriptor descriptor, unsigned long long index, int elemSize, const std::wstring &surface)
	{
		try
		{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'checked' in this context:
// ORIGINAL LINE: ulong offset = descriptor.IndexIsByteOffset ? index : checked(index * (ulong)elemSize);
			unsigned long long offset = descriptor.getIndexIsByteOffset() ? index : CheckedProduct(index, static_cast<unsigned long long>(elemSize));
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'checked' in this context:
// ORIGINAL LINE: return checked(descriptor.SourceBase + offset);
			return CheckedAddressEnd(descriptor.getSourceBase(), offset);
		}
		catch (const std::overflow_error &ex)
		{
			std::throw_with_nested(InvalidOperationException(std::format(L"{} rejected VGATHER source address overflow: {}", surface, WidenExceptionText(ex))));
		}
	}

	bool GatherMicroOp::IsLaneActiveForGather(std::shared_ptr<Processor::CPU_Core> &core, unsigned long long element)
	{
		if (element > std::numeric_limits<int>::max())
		{
			return false;
		}

		return core->LaneActive(getInstruction().getPredicateMask(), static_cast<int>(element));
	}

	std::shared_ptr<std::vector<unsigned char>> GatherMicroOp::GetLoadedBuffer()
	{
		return _stagedDestinationBuffer;
	}

	std::shared_ptr<std::vector<unsigned long long>> GatherMicroOp::GetIndices()
	{
		return _indices;
	}

	GatherMicroOp::Indexed2SrcDescriptor::Indexed2SrcDescriptor(unsigned long long descriptorAddress, unsigned long long sourceBase, unsigned long long indexBase, unsigned short indexStride, unsigned char indexType, bool indexIsByteOffset)
		: DescriptorAddress(descriptorAddress), SourceBase(sourceBase), IndexBase(indexBase), IndexStride(indexStride), IndexType(indexType), IndexIsByteOffset(indexIsByteOffset)
	{
	}

	unsigned long long GatherMicroOp::Indexed2SrcDescriptor::getDescriptorAddress() const
	{
		return DescriptorAddress;
	}

	unsigned long long GatherMicroOp::Indexed2SrcDescriptor::getSourceBase() const
	{
		return SourceBase;
	}

	unsigned long long GatherMicroOp::Indexed2SrcDescriptor::getIndexBase() const
	{
		return IndexBase;
	}

	unsigned short GatherMicroOp::Indexed2SrcDescriptor::getIndexStride() const
	{
		return IndexStride;
	}

	unsigned char GatherMicroOp::Indexed2SrcDescriptor::getIndexType() const
	{
		return IndexType;
	}

	bool GatherMicroOp::Indexed2SrcDescriptor::getIndexIsByteOffset() const
	{
		return IndexIsByteOffset;
	}

	int GatherMicroOp::Indexed2SrcDescriptor::getIndexElementSize() const
	{
		return getIndexType() == Uint32IndexType ? sizeof(unsigned int) : sizeof(unsigned long long);
	}

	unsigned long long GatherMicroOp::Indexed2SrcDescriptor::getEffectiveIndexStride() const
	{
		return getIndexStride() != 0 ? getIndexStride() : static_cast<unsigned long long>(getIndexElementSize());
	}

	StoreSegmentMicroOp::StoreSegmentMicroOp()
	{
		setMemoryOp(true);
	}

	void StoreSegmentMicroOp::SetStoreBuffer(std::vector<unsigned char> &buffer)
	{
		_storeBuffer = std::make_shared<std::vector<unsigned char>>(buffer);
	}

	void StoreSegmentMicroOp::InitializeMetadata()
	{
		VectorMicroOp::InitializeMetadata();

		if (getInstruction().getStreamLength() > 0)
		{
			int elemSize = GetElementSize();
			unsigned long long stride = getInstruction().getStride() > 0 ? getInstruction().getStride() : static_cast<unsigned long long>(elemSize);
			unsigned long long totalSize = getInstruction().getStreamLength() * stride;

			setReadMemoryRanges({});
			setWriteMemoryRanges({MemoryRange{getInstruction().getDestSrc1Pointer(), totalSize}});
		}

		RefreshVectorAdmissionMetadata(false, true);
	}

	bool StoreSegmentMicroOp::Execute(std::shared_ptr<Processor::CPU_Core> &core)
	{
		if (_storeBuffer == nullptr)
		{
			throw InvalidOperationException(L"StoreSegmentMicroOp.Execute() rejected missing store buffer on the authoritative vector store contour. A store carrier must fail closed instead of silently completing without architectural writes.");
		}

		if (_state == ExecutionState::NotStarted)
		{
			_totalElements = getInstruction().getStreamLength();
			int elemSize = GetElementSize();
			unsigned short stride = getInstruction().getStride();
			if (stride == 0)
			{
				stride = static_cast<unsigned short>(elemSize);
			}

			// Initiate async write using BurstIO
			std::shared_ptr<MemorySubsystem> memSub = core->GetBoundMemorySubsystem();
			if (memSub != nullptr)
			{
				_requestToken = memSub->EnqueueWrite(0, getInstruction().getDestSrc1Pointer(), static_cast<int>(_storeBuffer->size()), *_storeBuffer);
				_state = ExecutionState::StoringResults;
				return false; // Not complete, needs retry
			}
			else
			{
				// Fallback: synchronous store
				YAKSys_Hybrid_CPU::Execution::BurstIO::BurstWrite(getInstruction().getDestSrc1Pointer(), std::span<const unsigned char>(*_storeBuffer), _totalElements, elemSize, stride);
				_state = ExecutionState::Complete;
				return true;
			}
		}

		if (_state == ExecutionState::StoringResults)
		{
			if (_requestToken != nullptr && _requestToken->isComplete())
			{
				// RF-07.2x: an explicit terminal failure from this exact
				// owned vector-store request is an architectural write
				// fault, not a retry and not an unknown vector exception.
				// The existing single-lane PageFault catch keeps all
				// stage-aware delivery and cleanup authority.
				if (!_requestToken->getSucceeded())
				{
					const std::wstring reason = _requestToken->getFailureReason();
					throw PageFaultException(reason.empty() ? L"Vector-segment store failed." : reason, getInstruction().getDestSrc1Pointer(), true);
				}

				_requestToken->ThrowIfFailed(L"StoreSegmentMicroOp.Execute()");
				_state = ExecutionState::Complete;
				return true;
			}
			return false; // Still waiting
		}

		return _state == ExecutionState::Complete;
	}

	Store2DMicroOp::Store2DMicroOp()
	{
		setMemoryOp(true);
	}

	void Store2DMicroOp::SetStoreBuffer(std::vector<unsigned char> &buffer)
	{
		_storeBuffer = std::make_shared<std::vector<unsigned char>>(buffer);
	}

	void Store2DMicroOp::InitializeMetadata()
	{
		VectorMicroOp::InitializeMetadata();

		if (getInstruction().getStreamLength() > 0)
		{
			int elemSize = GetElementSize();
			unsigned long long estimatedSize = getInstruction().getStreamLength() * static_cast<unsigned long long>(elemSize);
			setReadMemoryRanges({});
			setWriteMemoryRanges({MemoryRange{getInstruction().getDestSrc1Pointer(), estimatedSize}});
		}

		RefreshVectorAdmissionMetadata(false, true);
	}

	bool Store2DMicroOp::Execute(std::shared_ptr<Processor::CPU_Core> &core)
	{
		if (_storeBuffer == nullptr)
		{
			throw InvalidOperationException(L"Store2DMicroOp.Execute() rejected missing store buffer on the authoritative 2D vector store contour. A store carrier must fail closed instead of silently completing without architectural writes.");
		}

		if (_state == ExecutionState::NotStarted)
		{
			_totalElements = getInstruction().getStreamLength();
			int elemSize = GetElementSize();

			// Use BurstIO for 2D store
			unsigned int rowLength = static_cast<unsigned int>(getInstruction().getImmediate()); // Row length encoded in immediate
			YAKSys_Hybrid_CPU::Execution::BurstIO::BurstWrite2D(getInstruction().getDestSrc1Pointer(), std::span<const unsigned char>(*_storeBuffer), _totalElements, elemSize, rowLength, getInstruction().getRowStride(), getInstruction().getStride(), 0); // startOffset

			_state = ExecutionState::Complete;
			return true;
		}

		return _state == ExecutionState::Complete;
	}

	StoreScatterMicroOp::StoreScatterMicroOp()
	{
		setClass(MicroOpClass::Lsu);
		setMemoryOp(false);
		setHasSideEffects(true);
		setStealable(false);
		setInstructionClass(InstructionClass::Memory);
		setSerializationClass(InstructionClassifier::GetSerializationClass(Processor::CPU_Core::InstructionsEnum::VSCATTER));
		SetClassFlexiblePlacement(SlotClass::LsuClass);
	}

	void StoreScatterMicroOp::SetStoreBuffer(std::vector<unsigned char> &buffer, std::vector<unsigned long long> &indices)
	{
		throw InvalidOperationException(L"StoreScatterMicroOp rejects manual buffer/index sideband. Executable VSCATTER is scoped to the canonical Indexed2SrcDesc path.");
	}

	void StoreScatterMicroOp::InitializeMetadata()
	{
		VectorMicroOp::InitializeMetadata();
		setClass(MicroOpClass::Lsu);
		setMemoryOp(false);
		setHasSideEffects(true);
		setStealable(false);
		setInstructionClass(InstructionClass::Memory);
		setSerializationClass(InstructionClassifier::GetSerializationClass(Processor::CPU_Core::InstructionsEnum::VSCATTER));
		SetClassFlexiblePlacement(SlotClass::LsuClass);
		ThrowIfUnsupportedPublicationContour();
		ThrowIfUnsupportedElementDataTypeForMetadata(L"StoreScatterMicroOp.InitializeMetadata()");

		if (getInstruction().getStreamLength() > 0)
		{
			try
			{
				Indexed2SrcDescriptor descriptor = ResolveDescriptorFromPublishedMemory(L"StoreScatterMicroOp.InitializeMetadata()");
				int elemSize = GetElementSize();
				std::vector<unsigned long long> indices = ReadPublishedIndexVector(descriptor, getInstruction().getStreamLength(), L"StoreScatterMicroOp.InitializeMetadata()");

				setReadMemoryRanges(BuildReadRanges(descriptor, elemSize, getInstruction().getStreamLength()));
				setWriteMemoryRanges(BuildWriteRanges(descriptor, indices, elemSize));
			}
			catch (const DecodeProjectionFaultException &e1)
			{
				throw;
			}
			catch (const std::runtime_error &ex)
			{
				throw DecodeProjectionFaultException(std::format(L"StoreScatterMicroOp.InitializeMetadata() rejected VSCATTER indexed publication before MicroOp follow-through: {}", WidenExceptionText(ex)), ex);
			}
		}

		RefreshVectorAdmissionMetadata(true, true);
	}

	bool StoreScatterMicroOp::Execute(std::shared_ptr<Processor::CPU_Core> &core)
	{
		if (_state == ExecutionState::Complete)
		{
			return true;
		}

		if (_state == ExecutionState::NotStarted)
		{
			_totalElements = getInstruction().getStreamLength();
			ThrowIfUnsupportedRuntimeContour();
			ThrowIfZeroLengthVectorComputeContour(L"StoreScatterMicroOp.Execute()");
			ThrowIfUnsupportedElementDataType(L"StoreScatterMicroOp.Execute()");

			int elemSize = GetElementSize();
			_descriptor.emplace(ReadIndexedDescriptor(core, getInstruction().getSrc2Pointer(), L"StoreScatterMicroOp.Execute()"));
			_descriptorResolved = true;

// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'checked' in this context:
// ORIGINAL LINE: _indices = new ulong[checked((int)_totalElements)];
			_indices = std::make_shared<std::vector<unsigned long long>>(static_cast<int>(_totalElements));
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'checked' in this context:
// ORIGINAL LINE: _stagedWrites = new StagedScatterWrite[checked((int)_totalElements)];
			_stagedWrites = std::make_shared<std::vector<StagedScatterWrite>>();
			_stagedWrites->reserve(static_cast<std::size_t>(_totalElements));
			_stagedWriteCount = 0;

			for (unsigned long long element = 0; element < _totalElements; element++)
			{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'checked' in this context:
// ORIGINAL LINE: int elementIndex = checked((int)element);
				int elementIndex = static_cast<int>(element);
				unsigned long long index = ReadIndexElement(core, *_descriptor, element, L"StoreScatterMicroOp.Execute()");
				(*_indices)[elementIndex] = index;

				if (!IsLaneActiveForScatter(core, element))
				{
					continue;
				}

				std::vector<unsigned char> elementBuffer(elemSize);
				core->ReadBoundMainMemoryExact(GetSourceDataAddress(element, elemSize), elementBuffer, L"StoreScatterMicroOp.Execute()");

				unsigned long long targetAddress = ResolveTargetAddress(*_descriptor, index, elemSize, L"StoreScatterMicroOp.Execute()");
				core->ThrowIfBoundMainMemoryRangeUnavailable(targetAddress, elemSize, L"StoreScatterMicroOp.Execute()");

				_stagedWrites->emplace_back(element, targetAddress, elementBuffer);
				++_stagedWriteCount;
			}

			_state = ExecutionState::Complete;
			return true;
		}

		return _state == ExecutionState::Complete;
	}

	void StoreScatterMicroOp::EmitWriteBackRetireRecords(std::shared_ptr<Processor::CPU_Core> &core, std::span<RetireRecord> retireRecords, int &retireRecordCount)
	{
		PublishStagedScatterWritesAtWriteBack(core);
	}

	void StoreScatterMicroOp::PublishStagedScatterWritesAtWriteBack(std::shared_ptr<Processor::CPU_Core> &core)
	{
		if (_state != ExecutionState::Complete || _stagedWrites == nullptr)
		{
			throw InvalidOperationException(L"StoreScatterMicroOp WB publication reached retire without a completed staged VSCATTER result.");
		}

		for (int i = 0; i < _stagedWriteCount; i++)
		{
			StagedScatterWrite write = (*_stagedWrites)[i];
			core->ThrowIfBoundMainMemoryRangeUnavailable(write.getAddress(), write.getData().size(), L"StoreScatterMicroOp.EmitWriteBackRetireRecords()");
		}

		for (int i = 0; i < _stagedWriteCount; i++)
		{
			StagedScatterWrite write = (*_stagedWrites)[i];
			std::vector<unsigned char> data = write.getData();
			core->WriteBoundMainMemoryExact(write.getAddress(), data, L"StoreScatterMicroOp.EmitWriteBackRetireRecords()");
		}
	}

	void StoreScatterMicroOp::ThrowIfUnsupportedPublicationContour()
	{
		if (getOpCode() != static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VSCATTER) || getInstruction().getOpCode() != static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VSCATTER))
		{
			throw DecodeProjectionFaultException(L"StoreScatterMicroOp.InitializeMetadata() can only publish the VSCATTER indexed-write contour.");
		}

		if (getInstruction().getIndexed() && !getInstruction().getIs2D())
		{
			return;
		}

		std::wstring addressingContour = getInstruction().getIndexed() && getInstruction().getIs2D() ? L"indexed+2D" : getInstruction().getIs2D() ? L"2D" : L"non-indexed";
		throw DecodeProjectionFaultException(std::format(L"StoreScatterMicroOp.InitializeMetadata() rejected unsupported {} VSCATTER publication. ", addressingContour) + L"Phase 02 only opens the 1D indexed-write contour; 2D and indexed+2D remain fail-closed.");
	}

	void StoreScatterMicroOp::ThrowIfUnsupportedRuntimeContour()
	{
		if (getOpCode() == static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VSCATTER) && getInstruction().getOpCode() == static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VSCATTER) && getInstruction().getIndexed() && !getInstruction().getIs2D())
		{
			return;
		}

		std::wstring addressingContour = getInstruction().getIndexed() && getInstruction().getIs2D() ? L"indexed+2D" : getInstruction().getIs2D() ? L"2D" : L"non-indexed";
		throw InvalidOperationException(std::format(L"StoreScatterMicroOp.Execute() rejected unsupported {} VSCATTER runtime contour. ", addressingContour) + L"The executable carrier is limited to 1D indexed memory writes.");
	}

	StoreScatterMicroOp::Indexed2SrcDescriptor StoreScatterMicroOp::ResolveDescriptorFromPublishedMemory(const std::wstring &surface)
	{
		if (_descriptorResolved)
		{
			return *_descriptor;
		}

		_descriptor.emplace(ReadIndexedDescriptorFromPublishedMemory(getInstruction().getSrc2Pointer(), surface));
		_descriptorResolved = true;
		return *_descriptor;
	}

	StoreScatterMicroOp::Indexed2SrcDescriptor StoreScatterMicroOp::ReadIndexedDescriptorFromPublishedMemory(unsigned long long descriptorAddress, const std::wstring &surface)
	{
		if (descriptorAddress == 0)
		{
			throw DecodeProjectionFaultException(std::format(L"{} rejected descriptor-less VSCATTER publication. Word2 must carry an Indexed2SrcDesc address.", surface));
		}

		std::vector<unsigned char> descriptorBytes(Indexed2SrcDescriptorSize);
		BurstIO::BurstRead(descriptorAddress, descriptorBytes, 1, Indexed2SrcDescriptorSize, Indexed2SrcDescriptorSize, nullptr);
		return ParseIndexedDescriptor(descriptorBytes, descriptorAddress, surface, true);
	}

	StoreScatterMicroOp::Indexed2SrcDescriptor StoreScatterMicroOp::ReadIndexedDescriptor(std::shared_ptr<Processor::CPU_Core> &core, unsigned long long descriptorAddress, const std::wstring &surface)
	{
		if (descriptorAddress == 0)
		{
			throw InvalidOperationException(std::format(L"{} rejected descriptor-less VSCATTER runtime execution. Word2 must carry an Indexed2SrcDesc address.", surface));
		}

		std::vector<unsigned char> descriptorBytes(Indexed2SrcDescriptorSize);
		core->ReadBoundMainMemoryExact(descriptorAddress, descriptorBytes, surface);
		return ParseIndexedDescriptor(descriptorBytes, descriptorAddress, surface, false);
	}

	StoreScatterMicroOp::Indexed2SrcDescriptor StoreScatterMicroOp::ParseIndexedDescriptor(std::vector<unsigned char> &descriptorBytes, unsigned long long descriptorAddress, const std::wstring &surface, bool publication)
	{
		if (descriptorBytes.size() != Indexed2SrcDescriptorSize)
		{
			CreateDescriptorException(surface, L"Indexed2SrcDesc must be exactly 32 bytes.", publication);
		}

		unsigned char indexType = descriptorBytes[18];

		if (indexType != Uint32IndexType && indexType != Uint64IndexType)
		{
			CreateDescriptorException(surface, std::format(L"Indexed2SrcDesc IndexType {} is unsupported; only 0=uint32 and 1=uint64 are executable.", static_cast<unsigned int>(indexType)), publication);
		}

		unsigned char indexIsByteOffset = descriptorBytes[19];
		if (indexIsByteOffset > 1)
		{
			CreateDescriptorException(surface, std::format(L"Indexed2SrcDesc IndexIsByteOffset {} is unsupported; only 0/1 are executable.", static_cast<unsigned int>(indexIsByteOffset)), publication);
		}

		for (int i = Indexed2SrcReservedOffset; i < Indexed2SrcDescriptorSize; i++)
		{
			if (descriptorBytes[i] != 0)
			{
				CreateDescriptorException(surface, L"Indexed2SrcDesc reserved bytes 20..31 must be zero for Phase 02 VSCATTER.", publication);
			}
		}

		return Indexed2SrcDescriptor(descriptorAddress, ReadLittleEndian<unsigned long long>(descriptorBytes, 0), ReadLittleEndian<unsigned long long>(descriptorBytes, 8), ReadLittleEndian<unsigned short>(descriptorBytes, 16), indexType, indexIsByteOffset != 0);
	}

	void StoreScatterMicroOp::CreateDescriptorException(const std::wstring &surface, const std::wstring &message, bool publication)
	{
		std::wstring fullMessage = std::format(L"{} rejected malformed VSCATTER Indexed2SrcDesc: {}", surface, message);
		if (publication)
			throw DecodeProjectionFaultException(fullMessage);
		throw InvalidOperationException(fullMessage);
	}

	std::vector<unsigned long long> StoreScatterMicroOp::ReadPublishedIndexVector(Indexed2SrcDescriptor descriptor, unsigned long long elementCount, const std::wstring &surface)
	{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'checked' in this context:
// ORIGINAL LINE: ulong[] indices = new ulong[checked((int)elementCount)];
		std::vector<unsigned long long> indices(static_cast<int>(elementCount));
		int indexSize = descriptor.getIndexElementSize();
		std::vector<unsigned char> indexBuffer(indexSize);

		for (unsigned long long element = 0; element < elementCount; element++)
		{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'checked' in this context:
// ORIGINAL LINE: int elementIndex = checked((int)element);
			int elementIndex = static_cast<int>(element);
			unsigned long long indexAddress = GetIndexAddress(descriptor, element, surface);
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'checked' in this context:
// ORIGINAL LINE: BurstIO.BurstRead(indexAddress, indexBuffer, elementCount: 1, elementSize: indexSize, stride: checked((ushort)indexSize));
			BurstIO::BurstRead(indexAddress, indexBuffer, 1, indexSize, static_cast<unsigned short>(indexSize), nullptr);
			indices[elementIndex] = indexSize == sizeof(unsigned int) ? ReadLittleEndian<unsigned int>(indexBuffer, 0) : ReadLittleEndian<unsigned long long>(indexBuffer, 0);
		}

		return indices;
	}

	unsigned long long StoreScatterMicroOp::ReadIndexElement(std::shared_ptr<Processor::CPU_Core> &core, Indexed2SrcDescriptor descriptor, unsigned long long element, const std::wstring &surface)
	{
		int indexSize = descriptor.getIndexElementSize();
		std::vector<unsigned char> indexBuffer(indexSize);
		core->ReadBoundMainMemoryExact(GetIndexAddress(descriptor, element, surface), indexBuffer, surface);
		return indexSize == sizeof(unsigned int) ? ReadLittleEndian<unsigned int>(indexBuffer, 0) : ReadLittleEndian<unsigned long long>(indexBuffer, 0);
	}

	std::vector<MicroOp::MemoryRange> StoreScatterMicroOp::BuildReadRanges(Indexed2SrcDescriptor descriptor, int elemSize, unsigned long long elementCount)
	{
		return {
			MemoryRange{descriptor.getDescriptorAddress(), Indexed2SrcDescriptorSize},
			MemoryRange{descriptor.getIndexBase(), CheckedRangeLength(elementCount, descriptor.getEffectiveIndexStride(), static_cast<unsigned long long>(descriptor.getIndexElementSize()))},
			MemoryRange{getInstruction().getDestSrc1Pointer(), CheckedRangeLength(elementCount, GetSourceDataStride(elemSize), static_cast<unsigned long long>(elemSize))}};
	}

	std::vector<MicroOp::MemoryRange> StoreScatterMicroOp::BuildWriteRanges(Indexed2SrcDescriptor descriptor, const std::vector<unsigned long long> &indices, int elemSize)
	{
		std::vector<MemoryRange> ranges;
		ranges.reserve(indices.size());
		for (unsigned long long index : indices)
			ranges.push_back({ResolveTargetAddress(descriptor, index, elemSize, L"StoreScatterMicroOp.InitializeMetadata()"), static_cast<unsigned long long>(elemSize)});
		return ranges;
	}

	unsigned long long StoreScatterMicroOp::GetIndexAddress(Indexed2SrcDescriptor descriptor, unsigned long long element, const std::wstring &surface)
	{
		try
		{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'checked' in this context:
// ORIGINAL LINE: return checked(descriptor.IndexBase + (element * descriptor.EffectiveIndexStride));
			return CheckedAddressEnd(descriptor.getIndexBase(), CheckedProduct(element, descriptor.getEffectiveIndexStride()));
		}
		catch (const std::overflow_error &ex)
		{
			std::throw_with_nested(InvalidOperationException(std::format(L"{} rejected VSCATTER index address overflow: {}", surface, WidenExceptionText(ex))));
		}
	}

	unsigned long long StoreScatterMicroOp::GetSourceDataAddress(unsigned long long element, int elemSize)
	{
		try
		{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'checked' in this context:
// ORIGINAL LINE: return checked(Instruction.DestSrc1Pointer + (element * GetSourceDataStride(elemSize)));
			return CheckedAddressEnd(getInstruction().getDestSrc1Pointer(), CheckedProduct(element, GetSourceDataStride(elemSize)));
		}
		catch (const std::overflow_error &ex)
		{
			std::throw_with_nested(InvalidOperationException(std::format(L"StoreScatterMicroOp.Execute() rejected VSCATTER source-data address overflow: {}", WidenExceptionText(ex))));
		}
	}

	unsigned long long StoreScatterMicroOp::GetSourceDataStride(int elemSize)
	{
		return getInstruction().getStride() > 0 ? getInstruction().getStride() : static_cast<unsigned long long>(elemSize);
	}

	unsigned long long StoreScatterMicroOp::ResolveTargetAddress(Indexed2SrcDescriptor descriptor, unsigned long long index, int elemSize, const std::wstring &surface)
	{
		try
		{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'checked' in this context:
// ORIGINAL LINE: ulong offset = descriptor.IndexIsByteOffset ? index : checked(index * (ulong)elemSize);
			unsigned long long offset = descriptor.getIndexIsByteOffset() ? index : CheckedProduct(index, static_cast<unsigned long long>(elemSize));
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'checked' in this context:
// ORIGINAL LINE: return checked(descriptor.MemoryBase + offset);
			return CheckedAddressEnd(descriptor.getMemoryBase(), offset);
		}
		catch (const std::overflow_error &ex)
		{
			std::throw_with_nested(InvalidOperationException(std::format(L"{} rejected VSCATTER target address overflow: {}", surface, WidenExceptionText(ex))));
		}
	}

	bool StoreScatterMicroOp::IsLaneActiveForScatter(std::shared_ptr<Processor::CPU_Core> &core, unsigned long long element)
	{
		if (element > std::numeric_limits<int>::max())
		{
			return false;
		}

		return core->LaneActive(getInstruction().getPredicateMask(), static_cast<int>(element));
	}

	std::shared_ptr<std::vector<unsigned long long>> StoreScatterMicroOp::GetIndices()
	{
		return _indices;
	}

	std::vector<StoreScatterMicroOp::StagedScatterWrite> StoreScatterMicroOp::GetStagedWrites()
	{
		if (_stagedWrites == nullptr)
		{
			return {};
		}

		return std::vector<StagedScatterWrite>(_stagedWrites->begin(), _stagedWrites->begin() + _stagedWriteCount);
	}

	StoreScatterMicroOp::StagedScatterWrite::StagedScatterWrite(unsigned long long element, unsigned long long address, std::vector<unsigned char> &data)
		: Element(element), Address(address), Data(data)
	{
	}

	unsigned long long StoreScatterMicroOp::StagedScatterWrite::getElement() const
	{
		return Element;
	}

	unsigned long long StoreScatterMicroOp::StagedScatterWrite::getAddress() const
	{
		return Address;
	}

	std::vector<unsigned char> StoreScatterMicroOp::StagedScatterWrite::getData() const
	{
		return Data;
	}

	StoreScatterMicroOp::Indexed2SrcDescriptor::Indexed2SrcDescriptor(unsigned long long descriptorAddress, unsigned long long memoryBase, unsigned long long indexBase, unsigned short indexStride, unsigned char indexType, bool indexIsByteOffset)
		: DescriptorAddress(descriptorAddress), MemoryBase(memoryBase), IndexBase(indexBase), IndexStride(indexStride), IndexType(indexType), IndexIsByteOffset(indexIsByteOffset)
	{
	}

	unsigned long long StoreScatterMicroOp::Indexed2SrcDescriptor::getDescriptorAddress() const
	{
		return DescriptorAddress;
	}

	unsigned long long StoreScatterMicroOp::Indexed2SrcDescriptor::getMemoryBase() const
	{
		return MemoryBase;
	}

	unsigned long long StoreScatterMicroOp::Indexed2SrcDescriptor::getIndexBase() const
	{
		return IndexBase;
	}

	unsigned short StoreScatterMicroOp::Indexed2SrcDescriptor::getIndexStride() const
	{
		return IndexStride;
	}

	unsigned char StoreScatterMicroOp::Indexed2SrcDescriptor::getIndexType() const
	{
		return IndexType;
	}

	bool StoreScatterMicroOp::Indexed2SrcDescriptor::getIndexIsByteOffset() const
	{
		return IndexIsByteOffset;
	}

	int StoreScatterMicroOp::Indexed2SrcDescriptor::getIndexElementSize() const
	{
		return getIndexType() == Uint32IndexType ? sizeof(unsigned int) : sizeof(unsigned long long);
	}

	unsigned long long StoreScatterMicroOp::Indexed2SrcDescriptor::getEffectiveIndexStride() const
	{
		return getIndexStride() != 0 ? getIndexStride() : static_cast<unsigned long long>(getIndexElementSize());
	}
}
