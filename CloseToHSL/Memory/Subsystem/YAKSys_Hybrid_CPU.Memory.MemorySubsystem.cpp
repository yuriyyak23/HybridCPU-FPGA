#include "YAKSys_Hybrid_CPU.Memory.MemorySubsystem.h"
#include "../../Core/Runtime/Memory/Translation/CpuInstructionTranslationContour.h"
#include "../DMA/DMAController.h"
#include "../../Core/Pipeline/MicroOps/Types/MicroOp.h"
#include "MemoryBankGeometryUpdateResult.h"
#include "PhysicalMemoryBankGeometry.h"
#include "PhysicalMemoryBankBinding.h"
#include "PhysicalMemoryBankIndex.h"
#include "MemoryBankGeometryGeneration.h"
#include "../../Core/Virtualization/Compatibility/Adapters/IO/YAKSys_Hybrid_CPU.Memory.IOMMU.h"
#include "../../Core/Execution/Memory/LoadStore/MemoryBankRouting.h"
#include "../../Core/Execution/StreamEngine/BurstIO/BurstPlanner.h"
#include "../../Core/Execution/BurstIO/AcceleratorRuntimeFailClosed.h"

using namespace HybridCPU_ISE::CloseToHSL::Memory::Cache;
using namespace HybridCPU_ISE::CloseToHSL::Memory::DMA;
using namespace HybridCPU_ISE::CloseToHSL::Memory::Subsystem;
using namespace HybridCPU_ISE::CloseToHSL::Memory::Timing;
using namespace YAKSys_Hybrid_CPU::Core::Memory;
using namespace YAKSys_Hybrid_CPU::Execution;

namespace YAKSys_Hybrid_CPU::Memory
{

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public static MemorySubsystem CreateForMainMemory(ref Processor proc, Processor.MainMemoryArea mainMemory, DMAController? dmaController = null)
	std::shared_ptr<MemorySubsystem> MemorySubsystem::CreateForMainMemory(Processor &proc, std::shared_ptr<Processor::MainMemoryArea> mainMemory, std::shared_ptr<DMAController> dmaController)
	{
		ArgumentNullException::ThrowIfNull(mainMemory);
		return std::shared_ptr<MemorySubsystem>(new MemorySubsystem(proc, dmaController, mainMemory));
	}

	bool MemorySubsystem::getHasExplicitMainMemoryBinding() const
	{
		return boundMainMemory != nullptr;
	}

	bool MemorySubsystem::IsBoundToMainMemory(std::shared_ptr<Processor::MainMemoryArea> memory)
	{
		return boundMainMemory == memory;
	}

	int MemorySubsystem::getNumBanks() const
	{
		{
		std::lock_guard<std::mutex> lock(geometryLifecycleGate);
			return _numBanks;
		}
	}

	void MemorySubsystem::setNumBanks(int value)
	{
		{
		std::lock_guard<std::mutex> lock(geometryLifecycleGate);
			int sanitized = std::max(1, value);
			if (_numBanks == sanitized)
			{
				return;
			}

			_numBanks = sanitized;
			ReconfigureBankTopology();
		}
	}

	int MemorySubsystem::getBankWidthBytes() const
	{
		{
		std::lock_guard<std::mutex> lock(geometryLifecycleGate);
			return _bankWidthBytes;
		}
	}

	void MemorySubsystem::setBankWidthBytes(int value)
	{
		{
		std::lock_guard<std::mutex> lock(geometryLifecycleGate);
			_bankWidthBytes = value;
		}
	}

	std::shared_ptr<PhysicalMemoryBankGeometry> MemorySubsystem::getPublishedPhysicalBankGeometry() const
	{
		{
		std::lock_guard<std::mutex> lock(geometryLifecycleGate);
			return _publishedPhysicalBankGeometry;
		}
	}

	std::shared_ptr<MemoryBankGeometryUpdateResult> MemorySubsystem::TryReplacePhysicalMemoryBankGeometry(int bankCount, int bankWidthBytes)
	{
		return getCycleController()->TryReplacePhysicalMemoryBankGeometry(bankCount, bankWidthBytes);
	}

	std::shared_ptr<PhysicalMemoryBankBinding> MemorySubsystem::CapturePublishedPhysicalMemoryBankBindingUnderControllerGate(unsigned long long address)
	{
		{
			std::lock_guard<std::mutex> lock(geometryLifecycleGate);
			std::shared_ptr<PhysicalMemoryBankGeometry> geometry = _publishedPhysicalBankGeometry;
			unsigned long long rawBankIndex = (address / static_cast<unsigned long long>(geometry->BankWidthBytes)) % static_cast<unsigned long long>(geometry->BankCount);
			std::shared_ptr<PhysicalMemoryBankIndex> bankIndex = std::make_shared<PhysicalMemoryBankIndex>(PhysicalMemoryBankIndex::Create(static_cast<int>(rawBankIndex)));
			return PhysicalMemoryBankBinding::Create(bankIndex, geometry->Generation);
		}
	}

	CanonicalVectorPhysicalBankEnvelope MemorySubsystem::CapturePublishedCanonicalVectorPhysicalBankEnvelopeUnderControllerGate(unsigned long long sourceAddress, unsigned long long elementCount, unsigned short stride)
	{
		{
			std::lock_guard<std::mutex> lock(geometryLifecycleGate);
			std::shared_ptr<PhysicalMemoryBankGeometry> geometry = _publishedPhysicalBankGeometry;
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'checked' in this context:
// ORIGINAL LINE: int capturedElementCount = checked((int)elementCount);
			int capturedElementCount = static_cast<int>(elementCount);
			std::vector<PhysicalMemoryBankIndex> sourceBankIndexes(capturedElementCount);

			for (int elementIndex = 0; elementIndex < capturedElementCount; elementIndex++)
			{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'checked' in this context:
// ORIGINAL LINE: ulong elementAddress = checked(sourceAddress + checked((ulong)elementIndex * stride));
				unsigned long long elementAddress = sourceAddress + static_cast<unsigned long long>(elementIndex) * stride;
				unsigned long long rawBankIndex = (elementAddress / static_cast<unsigned long long>(geometry->BankWidthBytes)) % static_cast<unsigned long long>(geometry->BankCount);
				sourceBankIndexes[elementIndex] = PhysicalMemoryBankIndex::Create(static_cast<int>(rawBankIndex));
			}

			return CanonicalVectorPhysicalBankEnvelope::Create(*geometry->Generation, sourceBankIndexes);
		}
	}

	int MemorySubsystem::getDmaThresholdBytes() const
	{
		return DmaThresholdBytes;
	}

	void MemorySubsystem::setDmaThresholdBytes(int value)
	{
		DmaThresholdBytes = value;
	}

	int MemorySubsystem::getAxiBoundary() const
	{
		return AxiBoundary;
	}

	void MemorySubsystem::setAxiBoundary(int value)
	{
		AxiBoundary = value;
	}

	int MemorySubsystem::getMaxBurstLength() const
	{
		return MaxBurstLength;
	}

	void MemorySubsystem::setMaxBurstLength(int value)
	{
		MaxBurstLength = value;
	}

	int MemorySubsystem::getNumMemoryPorts() const
	{
		{
		std::lock_guard<std::mutex> lock(geometryLifecycleGate);
			return _numMemoryPorts;
		}
	}

	void MemorySubsystem::setNumMemoryPorts(int value)
	{
		{
		std::lock_guard<std::mutex> lock(geometryLifecycleGate);
			int sanitized = std::max(1, value);
			if (_numMemoryPorts == sanitized)
			{
				return;
			}

			_numMemoryPorts = sanitized;
			ReconfigurePortStates();
		}
	}

	int MemorySubsystem::getReadWriteTurnaroundPenalty() const
	{
		return ReadWriteTurnaroundPenalty;
	}

	void MemorySubsystem::setReadWriteTurnaroundPenalty(int value)
	{
		ReadWriteTurnaroundPenalty = value;
	}

	double MemorySubsystem::getBankBandwidthGBps() const
	{
		return BankBandwidthGBps;
	}

	void MemorySubsystem::setBankBandwidthGBps(double value)
	{
		BankBandwidthGBps = value;
	}

	BankArbitrationPolicy MemorySubsystem::getArbitrationPolicy() const
	{
		return ArbitrationPolicy;
	}

	void MemorySubsystem::setArbitrationPolicy(BankArbitrationPolicy value)
	{
		ArbitrationPolicy = value;
	}

	int MemorySubsystem::getPortSwitchingPenalty() const
	{
		return PortSwitchingPenalty;
	}

	void MemorySubsystem::setPortSwitchingPenalty(int value)
	{
		PortSwitchingPenalty = value;
	}

	int MemorySubsystem::getRowBufferHitLatency() const
	{
		return RowBufferHitLatency;
	}

	void MemorySubsystem::setRowBufferHitLatency(int value)
	{
		RowBufferHitLatency = value;
	}

	int MemorySubsystem::getRowBufferMissLatency() const
	{
		return RowBufferMissLatency;
	}

	void MemorySubsystem::setRowBufferMissLatency(int value)
	{
		RowBufferMissLatency = value;
	}

	int MemorySubsystem::getRowBufferSize() const
	{
		return RowBufferSize;
	}

	void MemorySubsystem::setRowBufferSize(int value)
	{
		RowBufferSize = value;
	}

	std::shared_ptr<StreamRegisterFile> MemorySubsystem::getStreamRegisters() const
	{
		return StreamRegisters;
	}

	void MemorySubsystem::setStreamRegisters(std::shared_ptr<StreamRegisterFile> value)
	{
		StreamRegisters = value;
	}

	std::shared_ptr<GlobalDistributedCache> MemorySubsystem::getL3Cache() const
	{
		return L3Cache;
	}

	void MemorySubsystem::setL3Cache(std::shared_ptr<GlobalDistributedCache> value)
	{
		L3Cache = value;
	}

	long long MemorySubsystem::getTotalBursts() const
	{
		return TotalBursts;
	}

	void MemorySubsystem::setTotalBursts(long long value)
	{
		TotalBursts = value;
	}

	long long MemorySubsystem::getTotalBytesTransferred() const
	{
		return TotalBytesTransferred;
	}

	void MemorySubsystem::setTotalBytesTransferred(long long value)
	{
		TotalBytesTransferred = value;
	}

	long long MemorySubsystem::getBankConflicts() const
	{
		return BankConflicts;
	}

	void MemorySubsystem::setBankConflicts(long long value)
	{
		BankConflicts = value;
	}

	long long MemorySubsystem::getStallCycles() const
	{
		return StallCycles;
	}

	void MemorySubsystem::setStallCycles(long long value)
	{
		StallCycles = value;
	}

	long long MemorySubsystem::getDmaTransfers() const
	{
		return DmaTransfers;
	}

	void MemorySubsystem::setDmaTransfers(long long value)
	{
		DmaTransfers = value;
	}

	double MemorySubsystem::getAverageBurstLength() const
	{
		if (getTotalBursts() == 0)
		{
			return 0.0;
		}
		return static_cast<double>(getTotalBytesTransferred()) / getTotalBursts();
	}

	long long MemorySubsystem::getTotalWaitCycles() const
	{
		return TotalWaitCycles;
	}

	void MemorySubsystem::setTotalWaitCycles(long long value)
	{
		TotalWaitCycles = value;
	}

	double MemorySubsystem::getAverageWaitCycles() const
	{
		if (getTotalBursts() == 0)
		{
			return 0.0;
		}
		return static_cast<double>(getTotalWaitCycles()) / getTotalBursts();
	}

	int MemorySubsystem::getMaxQueueDepth() const
	{
		return MaxQueueDepth;
	}

	void MemorySubsystem::setMaxQueueDepth(int value)
	{
		MaxQueueDepth = value;
	}

	int MemorySubsystem::getCurrentQueuedRequests() const
	{
		{
		std::lock_guard<std::mutex> lock(geometryLifecycleGate);
			int total = 0;
			if (!bankQueues.empty())
			{
				for (auto queue : bankQueues)
				{
					total += queue.size();
				}
			}
			return total;
		}
	}

	std::vector<int> MemorySubsystem::GetBankQueueDepthsSnapshot()
	{
		{
			std::lock_guard<std::mutex> lock(geometryLifecycleGate);
			std::vector<int> depths(getNumBanks());
			for (int i = 0; i < getNumBanks(); i++)
			{
				depths[i] = bankQueues[i].size();
			}

			return depths;
		}
	}

	long long MemorySubsystem::getCurrentCycle() const
	{
		{
		std::lock_guard<std::mutex> lock(geometryLifecycleGate);
			return currentCycle;
		}
	}

	std::shared_ptr<MemoryCycleController> MemorySubsystem::getCycleController() const
	{
		std::lock_guard<std::mutex> lock(cycleControllerInitializationGate);
		if (!CycleController)
		{
			CycleController = std::make_shared<MemoryCycleController>(const_cast<MemorySubsystem *>(this)->shared_from_this());
		}
		return CycleController;
	}
	bool MemorySubsystem::isChannelOverloaded() const
	{
		{
		std::lock_guard<std::mutex> lock(geometryLifecycleGate);
			return getCurrentQueuedRequests() > (getNumBanks() * HardwareQueueSaturationThreshold);
		}
	}

	HardwareOccupancySnapshot128 MemorySubsystem::GetHardwareOccupancySnapshot128()
	{
		{
			std::lock_guard<std::mutex> lock(geometryLifecycleGate);
			return _cachedHardwareOccupancySnapshot;
		}
	}

	Core::SafetyMask128 MemorySubsystem::GetHardwareOccupancyMask128()
	{
		return GetHardwareOccupancySnapshot128().getOverloadedResources();
	}

	void MemorySubsystem::AdvanceSamplingEpoch()
	{
		{
			std::lock_guard<std::mutex> lock(geometryLifecycleGate);
			_hardwareSamplingEpoch++;
			_cachedHardwareOccupancySnapshot = BuildHardwareOccupancySnapshot();
		}
	}

	HardwareOccupancySnapshot128 MemorySubsystem::BuildHardwareOccupancySnapshot()
	{
		Core::SafetyMask128 mask = Core::SafetyMask128::Zero;
		unsigned short memoryBankBudgetAtLeastOneMask = 0;
		unsigned short memoryBankBudgetAtLeastTwoMask = 0;
		unsigned short readBankBudgetAtLeastOneMask = 0;
		unsigned short readBankBudgetAtLeastTwoMask = 0;
		unsigned short writeBankBudgetAtLeastOneMask = 0;
		unsigned short writeBankBudgetAtLeastTwoMask = 0;
		int queuedReadRequests = 0;
		int queuedWriteRequests = 0;
		int readyReadPorts = CountDirectionallyReadyPorts(true);
		int readyWritePorts = CountDirectionallyReadyPorts(false);

		int trackedBanks = std::min(getNumBanks(), 16);
		for (int i = 0; i < getNumBanks(); i++)
		{
			int bankQueuedRequests = bankQueues[i].size();
			int bankQueuedReadRequests = 0;
			for (auto request : bankQueues[i])
			{
				if (request.IsRead)
				{
					bankQueuedReadRequests++;
				}
			}

			int bankQueuedWriteRequests = bankQueuedRequests - bankQueuedReadRequests;
			queuedReadRequests += bankQueuedReadRequests;
			queuedWriteRequests += bankQueuedWriteRequests;

			if (i >= trackedBanks)
			{
				continue;
			}

			int remainingBankBudget = HardwareQueueSaturationThreshold - bankQueuedRequests;
			if (remainingBankBudget <= 0)
			{
				mask = mask | Core::ResourceMaskBuilder::ForMemoryBank128(i);
				continue;
			}

			memoryBankBudgetAtLeastOneMask |= static_cast<unsigned short>(1 << i);
			if (remainingBankBudget >= 2)
			{
				memoryBankBudgetAtLeastTwoMask |= static_cast<unsigned short>(1 << i);
			}

			int remainingReadBankBudget = ProjectDirectionalBudget(remainingBankBudget, bankQueuedReadRequests, HardwareQueueSaturationThreshold, readyReadPorts);
			if (remainingReadBankBudget >= 1)
			{
				readBankBudgetAtLeastOneMask |= static_cast<unsigned short>(1 << i);
			}
			if (remainingReadBankBudget >= 2)
			{
				readBankBudgetAtLeastTwoMask |= static_cast<unsigned short>(1 << i);
			}

			int remainingWriteBankBudget = ProjectDirectionalBudget(remainingBankBudget, bankQueuedWriteRequests, HardwareQueueSaturationThreshold, readyWritePorts);
			if (remainingWriteBankBudget >= 1)
			{
				writeBankBudgetAtLeastOneMask |= static_cast<unsigned short>(1 << i);
			}
			if (remainingWriteBankBudget >= 2)
			{
				writeBankBudgetAtLeastTwoMask |= static_cast<unsigned short>(1 << i);
			}
		}

		int totalQueueCapacity = getNumBanks() * HardwareQueueSaturationThreshold;
		int remainingGlobalBudget = totalQueueCapacity - (queuedReadRequests + queuedWriteRequests);
		unsigned char memoryIssueBudget = SaturateMemoryIssueBudget(remainingGlobalBudget);
		int remainingReadBudget = ProjectDirectionalBudget(remainingGlobalBudget, queuedReadRequests, totalQueueCapacity, readyReadPorts);
		int remainingWriteBudget = ProjectDirectionalBudget(remainingGlobalBudget, queuedWriteRequests, totalQueueCapacity, readyWritePorts);
		if (remainingGlobalBudget <= 0)
		{
			mask = mask | Core::ResourceMaskBuilder::ForLoad128();
			mask = mask | Core::ResourceMaskBuilder::ForStore128();
		}

		return HardwareOccupancySnapshot128(mask, currentCycle, _hardwareSamplingEpoch, memoryIssueBudget, memoryBankBudgetAtLeastOneMask, memoryBankBudgetAtLeastTwoMask, SaturateMemoryIssueBudget(remainingReadBudget), readBankBudgetAtLeastOneMask, readBankBudgetAtLeastTwoMask, SaturateMemoryIssueBudget(remainingWriteBudget), writeBankBudgetAtLeastOneMask, writeBankBudgetAtLeastTwoMask);
	}

	inline unsigned char MemorySubsystem::SaturateMemoryIssueBudget(int remainingGlobalBudget)
	{
		if (remainingGlobalBudget <= 0)
		{
			return 0;
		}

		if (remainingGlobalBudget == 1)
		{
			return 1;
		}

		return LiveWidenedMemoryIssueWidth;
	}

	inline int MemorySubsystem::ProjectDirectionalBudget(int remainingSharedBudget, int queuedDirectionalRequests, int directionalCapacity, int directionallyReadyPorts)
	{
		if (remainingSharedBudget <= 0)
		{
			return 0;
		}

		if (directionallyReadyPorts <= 0)
		{
			return 0;
		}

		int remainingDirectionalHeadroom = directionalCapacity - queuedDirectionalRequests;
		if (remainingDirectionalHeadroom <= 0)
		{
			return 0;
		}

		return std::min(std::min(remainingSharedBudget, remainingDirectionalHeadroom), directionallyReadyPorts);
	}

	long long MemorySubsystem::getUnalignedAccessCount() const
	{
		return UnalignedAccessCount;
	}

	void MemorySubsystem::setUnalignedAccessCount(long long value)
	{
		UnalignedAccessCount = value;
	}

	long long MemorySubsystem::getTotalAlignmentPenalty() const
	{
		return TotalAlignmentPenalty;
	}

	void MemorySubsystem::setTotalAlignmentPenalty(long long value)
	{
		TotalAlignmentPenalty = value;
	}

	double MemorySubsystem::getBurstEfficiency() const
	{
		return BurstEfficiency;
	}

	void MemorySubsystem::setBurstEfficiency(double value)
	{
		BurstEfficiency = value;
	}

	long long MemorySubsystem::getTotalBurstTimingCycles() const
	{
		return TotalBurstTimingCycles;
	}

	void MemorySubsystem::setTotalBurstTimingCycles(long long value)
	{
		TotalBurstTimingCycles = value;
	}

	unsigned long long MemorySubsystem::BurstEventArgs::getAddress() const
	{
		return Address;
	}

	void MemorySubsystem::BurstEventArgs::setAddress(unsigned long long value)
	{
		Address = value;
	}

	int MemorySubsystem::BurstEventArgs::getLength() const
	{
		return Length;
	}

	void MemorySubsystem::BurstEventArgs::setLength(int value)
	{
		Length = value;
	}

	bool MemorySubsystem::BurstEventArgs::isRead() const
	{
		return IsRead;
	}

	void MemorySubsystem::BurstEventArgs::setRead(bool value)
	{
		IsRead = value;
	}

	int MemorySubsystem::BurstEventArgs::getBankId() const
	{
		return BankId;
	}

	void MemorySubsystem::BurstEventArgs::setBankId(int value)
	{
		BankId = value;
	}

	long long MemorySubsystem::BurstEventArgs::getTimestamp() const
	{
		return Timestamp;
	}

	void MemorySubsystem::BurstEventArgs::setTimestamp(long long value)
	{
		Timestamp = value;
	}

	unsigned long long MemorySubsystem::MemoryRequestToken::getRequestID() const
	{
		return RequestID;
	}

	void MemorySubsystem::MemoryRequestToken::setRequestID(unsigned long long value)
	{
		RequestID = value;
	}

	bool MemorySubsystem::MemoryRequestToken::isComplete() const
	{
		return IsComplete;
	}

	void MemorySubsystem::MemoryRequestToken::setComplete(bool value)
	{
		IsComplete = value;
	}

	bool MemorySubsystem::MemoryRequestToken::getSucceeded() const
	{
		return Succeeded;
	}

	void MemorySubsystem::MemoryRequestToken::setSucceeded(bool value)
	{
		Succeeded = value;
	}

	unsigned long long MemorySubsystem::MemoryRequestToken::getDeviceID() const
	{
		return DeviceID;
	}

	void MemorySubsystem::MemoryRequestToken::setDeviceID(unsigned long long value)
	{
		DeviceID = value;
	}

	unsigned long long MemorySubsystem::MemoryRequestToken::getAddress() const
	{
		return Address;
	}

	void MemorySubsystem::MemoryRequestToken::setAddress(unsigned long long value)
	{
		Address = value;
	}

	int MemorySubsystem::MemoryRequestToken::getSize() const
	{
		return Size;
	}

	void MemorySubsystem::MemoryRequestToken::setSize(int value)
	{
		Size = value;
	}

	bool MemorySubsystem::MemoryRequestToken::isRead() const
	{
		return IsRead;
	}

	void MemorySubsystem::MemoryRequestToken::setRead(bool value)
	{
		IsRead = value;
	}

	bool MemorySubsystem::MemoryRequestToken::getDefersPhysicalWriteUntilRetire() const
	{
		return DefersPhysicalWriteUntilRetire;
	}

	void MemorySubsystem::MemoryRequestToken::setDefersPhysicalWriteUntilRetire(bool value)
	{
		DefersPhysicalWriteUntilRetire = value;
	}

	long long MemorySubsystem::MemoryRequestToken::getEnqueueCycle() const
	{
		return EnqueueCycle;
	}

	void MemorySubsystem::MemoryRequestToken::setEnqueueCycle(long long value)
	{
		EnqueueCycle = value;
	}

	long long MemorySubsystem::MemoryRequestToken::getCompleteCycle() const
	{
		return CompleteCycle;
	}

	void MemorySubsystem::MemoryRequestToken::setCompleteCycle(long long value)
	{
		CompleteCycle = value;
	}

	std::wstring MemorySubsystem::MemoryRequestToken::getFailureReason() const
	{
		return FailureReason;
	}

	void MemorySubsystem::MemoryRequestToken::setFailureReason(const std::wstring &value)
	{
		FailureReason = value;
	}

	MemorySubsystem::MemoryRequestToken::MemoryRequestToken(unsigned long long requestID, unsigned long long deviceID, unsigned long long address, int size, std::vector<unsigned char> &dataBuffer, bool isRead, long long enqueueCycle, bool defersPhysicalWriteUntilRetire, std::shared_ptr<PhysicalMemoryBankBinding> physicalBankBinding) : physicalBankBinding(physicalBankBinding)
	{
		setRequestID(requestID);
		setDeviceID(deviceID);
		setAddress(address);
		setSize(size);
		buffer = dataBuffer;
		setRead(isRead);
		setDefersPhysicalWriteUntilRetire(defersPhysicalWriteUntilRetire);
		setComplete(false);
		setSucceeded(false);
		setEnqueueCycle(enqueueCycle);
		setCompleteCycle(0);
		setFailureReason(L"");
	}

	std::vector<unsigned char> MemorySubsystem::MemoryRequestToken::GetBuffer()
	{
		return buffer;
	}

	std::shared_ptr<PhysicalMemoryBankBinding> MemorySubsystem::MemoryRequestToken::GetPhysicalBankBindingForOwner()
	{
		return physicalBankBinding;
	}

	void MemorySubsystem::MemoryRequestToken::ThrowIfFailed(const std::wstring &executionSurface)
	{
		if (!isComplete() || getSucceeded())
		{
			return;
		}

		std::wstring requestKind = isRead() ? L"read" : L"write";
		std::wstring failureReason = StringHelper::isEmptyOrWhiteSpace(getFailureReason()) ? std::format(L"The underlying async {0:s} request did not materialize successfully.", requestKind) : getFailureReason();

		throw InvalidOperationException(std::format(L"{0:s} observed completed async MemorySubsystem request {1:d} for {2:s} at 0x{3:0>X} ", executionSurface, getRequestID(), requestKind, getAddress()) + std::format(L"covering {0:d} byte(s), but the request did not materialize successfully. {1:s} ", getSize(), failureReason) + L"The authoritative runtime surface must fail closed instead of decoding buffered zeros, reusing stale data, or retiring a memory-visible success after unsuccessful async completion.");
	}

		MemorySubsystem::PhysicalBankTopologyCandidate::PhysicalBankTopologyCandidate(const std::vector<bool> &bankOccupied, const std::vector<long long> &bankLastAccessCycle, const std::vector<std::deque<MemorySubsystem::BankRequest>> &bankQueues, const std::vector<MemorySubsystem::PortState> &portStates)
		: BankOccupied(bankOccupied), BankLastAccessCycle(bankLastAccessCycle), BankQueues(bankQueues), PortStates(portStates) {}

	std::vector<bool> MemorySubsystem::PhysicalBankTopologyCandidate::getBankOccupied() const
	{
		return BankOccupied;
	}

	std::vector<long long> MemorySubsystem::PhysicalBankTopologyCandidate::getBankLastAccessCycle() const
	{
		return BankLastAccessCycle;
	}

	std::vector<std::deque<MemorySubsystem::BankRequest>> MemorySubsystem::PhysicalBankTopologyCandidate::getBankQueues() const
	{
		return BankQueues;
	}

	std::vector<MemorySubsystem::PortState> MemorySubsystem::PhysicalBankTopologyCandidate::getPortStates() const
	{
		return PortStates;
	}

	int MemorySubsystem::getMaxInflightBurstsPerDomain() const
	{
		return MaxInflightBurstsPerDomain;
	}

	void MemorySubsystem::setMaxInflightBurstsPerDomain(int value)
	{
		MaxInflightBurstsPerDomain = value;
	}

	long long MemorySubsystem::getDomainBurstStalls() const
	{
		return DomainBurstStalls;
	}

	void MemorySubsystem::setDomainBurstStalls(long long value)
	{
		DomainBurstStalls = value;
	}

	void MemorySubsystem::ReconfigureBankTopology()
	{
		std::vector<bool> resizedBankOccupied(_numBanks);
		std::vector<long long> resizedBankLastAccessCycle(_numBanks);
		std::vector<std::deque<BankRequest>> resizedBankQueues(_numBanks);
		const auto &existingBankQueues = bankQueues;

		int copiedBanks = static_cast<int>(std::min(existingBankQueues.size(), static_cast<std::size_t>(_numBanks)));
		for (int i = 0; i < _numBanks; i++)
		{
			if (i < copiedBanks)
			{
				resizedBankQueues[i] = existingBankQueues[i];
			}
			else
			{
				resizedBankQueues[i] = std::deque<BankRequest>();
			}
		}

		if (!bankOccupied.empty())
		{
			std::copy_n(bankOccupied.begin(), std::min(bankOccupied.size(), static_cast<std::size_t>(_numBanks)), resizedBankOccupied.begin());
		}

		if (!bankLastAccessCycle.empty())
		{
			std::copy_n(bankLastAccessCycle.begin(), std::min(bankLastAccessCycle.size(), static_cast<std::size_t>(_numBanks)), resizedBankLastAccessCycle.begin());
		}

		bankOccupied = resizedBankOccupied;
		bankLastAccessCycle = resizedBankLastAccessCycle;
		bankQueues = resizedBankQueues;
		roundRobinIndex = _numBanks == 0 ? 0 : roundRobinIndex % _numBanks;

		RefreshHardwareSamplingAfterTopologyChange();
	}

	void MemorySubsystem::ReconfigurePortStates()
	{
		std::vector<PortState> resizedPortStates(_numMemoryPorts);
		int copiedPorts = _portStates.empty() ? 0 : std::min(_portStates.size(), static_cast<std::size_t>(_numMemoryPorts));

		if (!_portStates.empty() && copiedPorts > 0)
		{
			std::copy_n(_portStates.begin(), copiedPorts, resizedPortStates.begin());
		}

		for (int i = 0; i < resizedPortStates.size(); i++)
		{
			if (i >= copiedPorts)
			{
				PortState temp_var = PortState();
				temp_var.PortId = i;
				temp_var.CurrentBankId = -1;
				temp_var.LastSwitchCycle = 0;
				temp_var.Busy = false;
				temp_var.HasLastDirection = false;
				temp_var.LastWasRead = true;
				temp_var.DirectionReadyCycle = 0;
				resizedPortStates[i] = temp_var;
			}
			else
			{
				resizedPortStates[i].PortId = i;
			}
		}

		_portStates = resizedPortStates;

		RefreshHardwareSamplingAfterTopologyChange();
	}

	void MemorySubsystem::RefreshHardwareSamplingAfterTopologyChange()
	{
		if (bankQueues.empty() || _portStates.empty())
		{
			return;
		}

		AdvanceSamplingEpoch();
	}

	std::shared_ptr<MemoryBankGeometryUpdateResult> MemorySubsystem::TryReplacePhysicalMemoryBankGeometryUnderControllerGate(int bankCount, int bankWidthBytes, bool controllerIsQuiescent)
	{
		{
			std::lock_guard<std::mutex> lock(geometryLifecycleGate);
			if (!PhysicalMemoryBankGeometry::IsBankCountRepresentable(bankCount))
			{
				return MemoryBankGeometryUpdateResult::Rejected(MemoryBankGeometryUpdateRejectReason::InvalidBankCount);
			}

			if (!PhysicalMemoryBankGeometry::IsBankWidthRepresentable(bankWidthBytes))
			{
				return MemoryBankGeometryUpdateResult::Rejected(MemoryBankGeometryUpdateRejectReason::InvalidBankWidth);
			}

			if (!controllerIsQuiescent || !IsPhysicalBankGeometryOwnerQuiescent())
			{
				return MemoryBankGeometryUpdateResult::Rejected(MemoryBankGeometryUpdateRejectReason::Busy);
			}

			if (_lastIssuedPhysicalBankGeometryGeneration == MemoryBankGeometryGeneration::MaxValue)
			{
				return MemoryBankGeometryUpdateResult::Rejected(MemoryBankGeometryUpdateRejectReason::GenerationExhausted);
			}

			std::shared_ptr<PhysicalBankTopologyCandidate> candidate;
			if (!TryPreparePhysicalBankTopologyCandidate(bankCount, candidate))
			{
				return MemoryBankGeometryUpdateResult::Rejected(MemoryBankGeometryUpdateRejectReason::PlatformRejected);
			}

			unsigned long long nextGenerationRaw = _lastIssuedPhysicalBankGeometryGeneration + 1ULL;
			std::shared_ptr<MemoryBankGeometryGeneration> nextGeneration = std::make_shared<MemoryBankGeometryGeneration>(MemoryBankGeometryGeneration::Create(nextGenerationRaw));
			std::shared_ptr<PhysicalMemoryBankGeometry> nextGeometry = PhysicalMemoryBankGeometry::Create(bankCount, bankWidthBytes, nextGeneration);

			_numBanks = bankCount;
			_bankWidthBytes = bankWidthBytes;
			bankOccupied = candidate->getBankOccupied();
			bankLastAccessCycle = candidate->getBankLastAccessCycle();
			bankQueues = candidate->getBankQueues();
			_portStates = candidate->getPortStates();
			roundRobinIndex = 0;
			_publishedPhysicalBankGeometry = nextGeometry;
			_lastIssuedPhysicalBankGeometryGeneration = nextGenerationRaw;
			AdvanceSamplingEpoch();

			return MemoryBankGeometryUpdateResult::Applied;
		}
	}

	bool MemorySubsystem::IsPhysicalBankGeometryOwnerQuiescent()
	{
		{
			std::lock_guard<std::mutex> lock(pendingRequestsGate);
			for (const auto &entry : pendingRequests)
			{
				if (!entry.second || !entry.second->isComplete())
				{
					return false;
				}
			}
		}

		for (auto queue : bankQueues)
		{
			if (queue.size() != 0)
			{
				return false;
			}
		}

		if (TRB.getOutstandingCount() != 0)
		{
			return false;
		}

		for (auto active : bankOccupied)
		{
			if (active)
			{
				return false;
			}
		}

		for (auto port : _portStates)
		{
			if (port.Busy)
			{
				return false;
			}
		}

		return true;
	}

	bool MemorySubsystem::TryPreparePhysicalBankTopologyCandidate(int bankCount, std::shared_ptr<PhysicalBankTopologyCandidate> &candidate)
	{
	// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
	// ORIGINAL LINE: candidate = null!;
		candidate = nullptr;
		if (bankCount < 0 || static_cast<std::size_t>(bankCount) > std::vector<bool>().max_size())
		{
			return false;
		}

		try
		{
			std::vector<bool> candidateBankOccupied(bankCount);
			std::vector<long long> candidateBankLastAccessCycle(bankCount);
			std::vector<std::deque<BankRequest>> candidateBankQueues(bankCount);
			for (int bankIndex = 0; bankIndex < bankCount; bankIndex++)
			{
				candidateBankQueues[bankIndex] = std::deque<BankRequest>();
			}

			std::vector<PortState> candidatePortStates(_numMemoryPorts);
			for (int portIndex = 0; portIndex < candidatePortStates.size(); portIndex++)
			{
				PortState temp_var = PortState();
				temp_var.PortId = portIndex;
				temp_var.CurrentBankId = -1;
				temp_var.LastSwitchCycle = 0;
				temp_var.Busy = false;
				temp_var.HasLastDirection = false;
				temp_var.LastWasRead = true;
				temp_var.DirectionReadyCycle = 0;
				candidatePortStates[portIndex] = temp_var;
			}

			candidate = std::make_shared<PhysicalBankTopologyCandidate>(candidateBankOccupied, candidateBankLastAccessCycle, candidateBankQueues, candidatePortStates);
			return true;
		}
		catch (const OutOfMemoryException &e1)
		{
			return false;
		}
		catch (const std::overflow_error &e2)
		{
			return false;
		}
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public MemorySubsystem(ref Processor proc, DMAController? dmaController = null) : this(ref proc, dmaController, null)
	MemorySubsystem::MemorySubsystem(Processor &proc, std::shared_ptr<DMAController> dmaController) : MemorySubsystem(proc, dmaController, nullptr)
	{
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private MemorySubsystem(ref Processor proc, DMAController? dmaController, System.Nullable<Processor.MainMemoryArea> mainMemory)
	MemorySubsystem::MemorySubsystem(Processor &proc, std::shared_ptr<DMAController> dmaController, std::shared_ptr<Processor::MainMemoryArea> mainMemory) : boundMainMemory(mainMemory), dma(dmaController), processor(proc), pendingRequests(std::unordered_map<unsigned long long, std::shared_ptr<MemoryRequestToken>>())
	{

		setStreamRegisters(std::make_shared<StreamRegisterFile>());
		setL3Cache(std::make_shared<GlobalDistributedCache>());

		bankOccupied = std::vector<bool>(getNumBanks());
		bankLastAccessCycle = std::vector<long long>(getNumBanks());
		bankQueues = std::vector<std::deque<BankRequest>>(getNumBanks());
		for (int i = 0; i < getNumBanks(); i++)
		{
			bankQueues[i] = std::deque<BankRequest>();
		}
		roundRobinIndex = 0;
		currentCycle = 0;
		nextRequestID = 1;
// C# TO C++ CONVERTER TASK: You cannot use 'shared_from_this' in a constructor:
		// Plan 08: Initialize Transaction Reorder Buffer
		TRB = TransactionReorderBuffer(0);

		// Phase 2: Initialize port states
		_portStates = std::vector<PortState>(getNumMemoryPorts());
		for (int i = 0; i < getNumMemoryPorts(); i++)
		{
			PortState temp_var = PortState();
			temp_var.PortId = i;
			temp_var.CurrentBankId = -1;
			temp_var.LastSwitchCycle = 0;
			temp_var.Busy = false;
			temp_var.HasLastDirection = false;
			temp_var.LastWasRead = true;
			temp_var.DirectionReadyCycle = 0;
			_portStates[i] = temp_var;
		}

		std::shared_ptr<MemoryBankGeometryGeneration> initialGeneration = std::make_shared<MemoryBankGeometryGeneration>(MemoryBankGeometryGeneration::Create(MemoryBankGeometryGeneration::MinValue));
		_publishedPhysicalBankGeometry = PhysicalMemoryBankGeometry::Create(_numBanks, _bankWidthBytes, initialGeneration);
		_lastIssuedPhysicalBankGeometryGeneration = initialGeneration->getValue();

		AdvanceSamplingEpoch();

		// IOMMU is already initialized in Processor constructor
		// We just need to reference the static class
	}

	bool MemorySubsystem::Read(unsigned long long deviceID, unsigned long long address, std::span<unsigned char> buffer)
	{
		{
			std::lock_guard<std::mutex> lock(geometryLifecycleGate);
			if (buffer.size() == 0)
			{
				return true;
			}

			int length = buffer.size();
			std::shared_ptr<PhysicalMemoryBankIndex> bankIndex = ComputeBankId(address);

			// Fire BurstStarted event
			OnBurstStarted(address, length, true, bankIndex->getValue());

			// Check for bank conflict with adaptive stall calculation
			if (bankOccupied[bankIndex->getValue()])
			{
				setBankConflicts(getBankConflicts() + 1);

				// Calculate adaptive stall based on queue depth and transfer size
				int queueDepth = bankQueues[bankIndex->getValue()].size();
				int stallCycles = CalculateAdaptiveStall(queueDepth, length);
				setStallCycles(getStallCycles() + stallCycles);
				currentCycle += stallCycles;

				// Update max queue depth metric
				if (queueDepth > getMaxQueueDepth())
				{
					setMaxQueueDepth(queueDepth);
				}
			}

			// Mark bank as occupied
			bankOccupied[bankIndex->getValue()] = true;
			bankLastAccessCycle[bankIndex->getValue()] = currentCycle;

			// Use IOMMU for direct access
			bool success = IOMMU::ReadBurst(deviceID, address, buffer, boundMainMemory);

			// Release bank
			bankOccupied[bankIndex->getValue()] = false;

			// Update statistics
			if (success)
			{
				setTotalBursts(getTotalBursts() + 1);
				setTotalBytesTransferred(getTotalBytesTransferred() + length);
			}

			// Simulate memory access latency (4 cycles base + length dependent)
			int accessCycles = 4 + (length / getBankWidthBytes());
			currentCycle += accessCycles;

			// Fire BurstCompleted event
			OnBurstCompleted(address, length, true, bankIndex->getValue());

			return success;
		}
	}

	bool MemorySubsystem::Write(unsigned long long deviceID, unsigned long long address, std::span<const unsigned char> buffer)
	{
		{
			std::lock_guard<std::mutex> lock(geometryLifecycleGate);
			if (buffer.size() == 0)
			{
				return true;
			}

			int length = buffer.size();
			std::shared_ptr<PhysicalMemoryBankIndex> bankIndex = ComputeBankId(address);

			// Fire BurstStarted event
			OnBurstStarted(address, length, false, bankIndex->getValue());

			// Check for bank conflict with adaptive stall calculation
			if (bankOccupied[bankIndex->getValue()])
			{
				setBankConflicts(getBankConflicts() + 1);

				// Calculate adaptive stall based on queue depth and transfer size
				int queueDepth = bankQueues[bankIndex->getValue()].size();
				int stallCycles = CalculateAdaptiveStall(queueDepth, length);
				setStallCycles(getStallCycles() + stallCycles);
				currentCycle += stallCycles;

				// Update max queue depth metric
				if (queueDepth > getMaxQueueDepth())
				{
					setMaxQueueDepth(queueDepth);
				}
			}

			// Mark bank as occupied
			bankOccupied[bankIndex->getValue()] = true;
			bankLastAccessCycle[bankIndex->getValue()] = currentCycle;

			// Use IOMMU for direct access
			bool success = IOMMU::WriteBurst(deviceID, address, buffer, boundMainMemory);

			// Release bank
			bankOccupied[bankIndex->getValue()] = false;

			// Update statistics
			if (success)
			{
				setTotalBursts(getTotalBursts() + 1);
				setTotalBytesTransferred(getTotalBytesTransferred() + length);
			}

			// Simulate memory access latency (4 cycles base + length dependent)
			int accessCycles = 4 + (length / getBankWidthBytes());
			currentCycle += accessCycles;

			// Fire BurstCompleted event
			OnBurstCompleted(address, length, false, bankIndex->getValue());

			return success;
		}
	}

	std::shared_ptr<PhysicalMemoryBankIndex> MemorySubsystem::ComputeBankId(unsigned long long address)
	{
		return std::make_shared<PhysicalMemoryBankIndex>(PhysicalMemoryBankIndex::FromRawValue(Core::Memory::MemoryBankRouting::ResolveBankId(address, getBankWidthBytes(), getNumBanks())));
	}

	bool MemorySubsystem::RemoveQueuedBankRequest(unsigned long long requestID, int bankId)
	{
		if (static_cast<unsigned int>(bankId) >= static_cast<unsigned int>(getNumBanks()) || bankQueues[bankId].empty())
		{
			return false;
		}

		std::deque<BankRequest> retainedRequests(bankQueues[bankId].size());
		bool removed = false;
		while (bankQueues[bankId].size() > 0)
		{
			BankRequest request = bankQueues[bankId].front();
			bankQueues[bankId].pop_front();
			if (!removed && request.RequestID == requestID)
			{
				removed = true;
				continue;
			}

			retainedRequests.push_back(request);
		}

		while (retainedRequests.size() > 0)
		{
			bankQueues[bankId].push_back(retainedRequests.front());
			retainedRequests.pop_front();
		}

		return removed;
	}

	int MemorySubsystem::CalculateAdaptiveStall(int queueDepth, int transferSize)
	{
		// Base stall increases with queue depth
		int baseStall = queueDepth * 2;

		// Add transfer-dependent latency based on bandwidth
		// Bandwidth in bytes/cycle = (BankBandwidthGBps * 1e9) / (frequency in Hz)
		// Assuming 1 GHz frequency for simplicity: 1 cycle = 1 ns
		double bytesPerCycle = getBankBandwidthGBps(); // GB/s ≈ bytes/cycle at 1 GHz
		int transferLatency = static_cast<int>(std::ceil(transferSize / bytesPerCycle));

		return baseStall + transferLatency;
	}

	int MemorySubsystem::SelectNextBank()
	{
		switch (getArbitrationPolicy())
		{
			case BankArbitrationPolicy::RoundRobin:
				return SelectBankRoundRobin();

			case BankArbitrationPolicy::WeightedFair:
				return SelectBankWeightedFair();

			case BankArbitrationPolicy::Priority:
				return SelectBankPriority();

			case BankArbitrationPolicy::FixedStatic:
				// ProcessBankQueues handles FixedStatic directly to enable multiple ports
				return -1;

			default:
				return SelectBankRoundRobin();
		}
	}

	int MemorySubsystem::SelectBankRoundRobin()
	{
		int startIndex = roundRobinIndex;
		do
		{
			if (!bankOccupied[roundRobinIndex] && bankQueues[roundRobinIndex].size() > 0)
			{
				int selected = roundRobinIndex;
				roundRobinIndex = (roundRobinIndex + 1) % getNumBanks();
				return selected;
			}
			roundRobinIndex = (roundRobinIndex + 1) % getNumBanks();
		} while (roundRobinIndex != startIndex);

		return -1; // No available banks
	}

	int MemorySubsystem::SelectBankWeightedFair()
	{
		int selectedBank = -1;
		int maxWeight = -1;

		for (int i = 0; i < getNumBanks(); i++)
		{
			if (!bankOccupied[i] && bankQueues[i].size() > 0)
			{
				// Weight = queue depth + average priority
				int queueDepth = bankQueues[i].size();
				int avgPriority = 0;
				for (auto req : bankQueues[i])
				{
					avgPriority += req.Priority;
				}
				avgPriority = queueDepth > 0 ? avgPriority / queueDepth : 0;

				int weight = queueDepth * 10 + avgPriority;
				if (weight > maxWeight)
				{
					maxWeight = weight;
					selectedBank = i;
				}
			}
		}

		return selectedBank;
	}

	int MemorySubsystem::SelectBankPriority()
	{
		int selectedBank = -1;
		int highestPriority = -1;

		for (int i = 0; i < getNumBanks(); i++)
		{
			if (!bankOccupied[i] && bankQueues[i].size() > 0)
			{
				BankRequest request = bankQueues[i].front();
				if (request.Priority > highestPriority)
				{
					highestPriority = request.Priority;
					selectedBank = i;
				}
			}
		}

		return selectedBank;
	}

	void MemorySubsystem::ProcessBankQueues()
	{
		if (getArbitrationPolicy() == BankArbitrationPolicy::FixedStatic)
		{
			// Strict TDM scheduling: bank selection depends purely on the cycle counter (Cycle-Determinism)
			for (int p = 0; p < getNumMemoryPorts(); p++)
			{
				int targetBank = static_cast<int>((currentCycle + p) % getNumBanks());
				if (!bankOccupied[targetBank] && bankQueues[targetBank].size() > 0)
				{
					BankRequest request = bankQueues[targetBank].front();
					bankQueues[targetBank].pop_front();
					ExecuteBankRequest(targetBank, request);
				}
			}
			return;
		}

		// Process up to NumMemoryPorts banks concurrently using dynamic arbitration
		int portsUsed = 0;
		int maxAttempts = getNumBanks();
		int attempts = 0;

		while (portsUsed < getNumMemoryPorts() && attempts < maxAttempts)
		{
			int selectedBank = SelectNextBank();
			if (selectedBank == -1)
			{
				break; // No available banks according to policy
			}

			BankRequest request = bankQueues[selectedBank].front();
		bankQueues[selectedBank].pop_front();
			ExecuteBankRequest(selectedBank, request);
			portsUsed++;
			attempts++;
		}
	}

	void MemorySubsystem::ExecuteBankRequest(int bankId, BankRequest request)
	{
		long long waitCycles = currentCycle - request.EnqueueCycle;
		setTotalWaitCycles(getTotalWaitCycles() + waitCycles);
		int portId = AllocatePortForBank(bankId, request.IsRead);

		// Phase 3: Calculate accurate burst timing using BurstPlanner
		BurstTiming timing = BurstPlanner::CalculateBurstTiming(request.Address, request.Length, request.IsRead);

		// Track unaligned accesses
		if (timing.AlignmentPenalty > 0)
		{
			setUnalignedAccessCount(getUnalignedAccessCount() + 1);
			setTotalAlignmentPenalty(getTotalAlignmentPenalty() + timing.AlignmentPenalty);
		}

		// Track burst efficiency
		if (timing.TotalCycles > 0)
		{
			int theoreticalCycles = timing.DataCycles;
			int actualCycles = timing.TotalCycles;
			double burstEff = theoreticalCycles / static_cast<double>(actualCycles);

			// Running average of burst efficiency
			if (getTotalBursts() > 0)
			{
				setBurstEfficiency((getBurstEfficiency() * 0.95) + (burstEff * 0.05));
			}
			else
			{
				setBurstEfficiency(burstEff);
			}
		}

		// Accumulate burst timing cycles
		setTotalBurstTimingCycles(getTotalBurstTimingCycles() + timing.TotalCycles);

		// Mark bank as occupied
		bankOccupied[bankId] = true;
		bankLastAccessCycle[bankId] = currentCycle;

		// Execute the request via IOMMU or DMA
		bool success = false;
		if (request.IsRead)
		{
			// Perform read operation
			success = IOMMU::ReadBurst(request.DeviceID, request.Address, std::span<unsigned char>(request.Buffer.data(), static_cast<std::size_t>(request.Length)), boundMainMemory);
		}
		else
		{
			// Retire-deferred stores still consume queue/bank timing here, but
			// the backing-memory side effect remains blocked until WB retire.
			if (request.DefersPhysicalWriteUntilRetire)
			{
				success = true;
			}
			else
			{
				success = IOMMU::WriteBurst(request.DeviceID, request.Address, std::span<unsigned char>(request.Buffer.data(), static_cast<std::size_t>(request.Length)), boundMainMemory);
			}
		}

		{
		// Find and complete the associated memory request token
			std::lock_guard<std::mutex> lock(pendingRequestsGate);
			std::shared_ptr<MemoryRequestToken> matchingToken;
			auto pendingRequests_iterator = pendingRequests.find(request.RequestID);
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: if (pendingRequests.TryGetValue(request.RequestID, out MemoryRequestToken? matchingToken) && !matchingToken.IsComplete)
			if (pendingRequests_iterator != pendingRequests.end() && pendingRequests_iterator->second && !pendingRequests_iterator->second->isComplete())
			{
				matchingToken = pendingRequests_iterator->second;
				matchingToken->setComplete(true);
				matchingToken->setSucceeded(success);
				matchingToken->setCompleteCycle(currentCycle);
				matchingToken->setFailureReason(success ? L"" : std::format(L"MemorySubsystem {0:s} request failed after bank admission at 0x{1:0>X} for {2:d} byte(s).", (request.IsRead ? L"read" : L"write"), request.Address, request.Length));
			}

		}

		// Update statistics
		if (success)
		{
			setTotalBursts(getTotalBursts() + 1);
			setTotalBytesTransferred(getTotalBytesTransferred() + request.Length);
		}

		// Release bank after operation
		bankOccupied[bankId] = false;
		ReleasePortAfterOperation(portId, bankId, request.IsRead);
	}

	void MemorySubsystem::AdvanceBoundDmaAgentOneCycle()
	{
		if (dma != nullptr)
		{
			dma->ExecuteCycle();
		}
	}

	void MemorySubsystem::ResetStatistics()
	{
		// Telemetry reset is observation-only. It deliberately does not
		// reset MemoryCycle or any request/completion state.
		getCycleController()->ResetTelemetry();
		{
			std::lock_guard<std::mutex> lock(geometryLifecycleGate);
			setTotalBursts(0);
			setTotalBytesTransferred(0);
			setBankConflicts(0);
			setStallCycles(0);
			setDmaTransfers(0);
			setTotalWaitCycles(0);
			setMaxQueueDepth(0);

			// Phase 3: Reset burst timing metrics
			setUnalignedAccessCount(0);
			setTotalAlignmentPenalty(0);
			setBurstEfficiency(0.0);
			setTotalBurstTimingCycles(0);

		}
	}

	void MemorySubsystem::AdvanceCycles(long long cycles)
	{
		getCycleController()->AdvanceCompatibilityCycles(cycles);
	}

	void MemorySubsystem::AdvanceLegacyAgentOneCycle()
	{
		{
			std::lock_guard<std::mutex> lock(geometryLifecycleGate);
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'checked' in this context:
// ORIGINAL LINE: currentCycle = checked(currentCycle + 1);
			currentCycle = currentCycle + 1;

			// Advance L3 cache cycle counter
			getL3Cache()->AdvanceCycle();

			// === TRB Issue Phase: find next transaction whose bank is free ===
			const auto& bankBusySpan = bankOccupied;
			int issuableIdx = TRB.FindNextIssuable(bankBusySpan);
			if (issuableIdx >= 0)
			{
				TransactionReorderBuffer::TrbEntry entry = TRB.GetEntry(issuableIdx);
				if (entry.TargetBank >= 0 && entry.TargetBank < getNumBanks())
				{
					bankOccupied[entry.TargetBank] = true;
				}

				// Mark complete immediately (single-cycle model)
				TRB.Complete(issuableIdx, static_cast<unsigned long long>(currentCycle));
			}

			// === TRB Retire Phase: in-order commit ===
			TransactionReorderBuffer::TrbEntry retired;
			while (TRB.TryRetire(retired))
			{
				if (retired.TargetBank >= 0 && retired.TargetBank < getNumBanks())
				{
					bankOccupied[retired.TargetBank] = false;
				}
			}

			// Process any queued requests
			ProcessBankQueues();

			// Release banks that have been idle for sufficient time
			for (int i = 0; i < getNumBanks(); i++)
			{
				if (bankOccupied[i] && (currentCycle - bankLastAccessCycle[i]) > 100)
				{
					bankOccupied[i] = false;
				}
			}

			AdvanceSamplingEpoch();
		}
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal bool ExecuteControllerReadStep(ulong deviceId, ulong address, PhysicalMemoryBankBinding physicalBankBinding, byte[] destination, YAKSys_Hybrid_CPU.Core.Memory.CpuInstructionTranslationOwner.PhysicalReadBinding? cpuPhysicalRead = null)
	bool MemorySubsystem::ExecuteControllerReadStep(unsigned long long deviceId, unsigned long long address, std::shared_ptr<PhysicalMemoryBankBinding> physicalBankBinding, std::vector<unsigned char> &destination, std::shared_ptr<YAKSys_Hybrid_CPU::Core::Memory::PhysicalReadBinding> cpuPhysicalRead)
	{
		ArgumentNullException::ThrowIfNull(destination);
		{
			std::lock_guard<std::mutex> lock(geometryLifecycleGate);
			std::shared_ptr<PhysicalMemoryBankGeometry> geometry = _publishedPhysicalBankGeometry;
			if (!physicalBankBinding->IsWellFormed || physicalBankBinding->Generation->getValue() != geometry->Generation->getValue() || physicalBankBinding->BankIndex->getValue() >= geometry->BankCount)
			{
				return false;
			}

			return cpuPhysicalRead != nullptr ? cpuPhysicalRead->Read(boundMainMemory, address, destination) : IOMMU::ReadBurst(deviceId, address, std::span<unsigned char>(destination.data(), destination.size()), boundMainMemory);
		}
	}

	bool MemorySubsystem::ExecuteControllerCanonicalVectorElementReadStepUnderOwnerGate(unsigned long long deviceId, unsigned long long address, std::vector<unsigned char> &destination)
	{
		ArgumentNullException::ThrowIfNull(destination);
		return IOMMU::ReadBurst(deviceId, address, std::span<unsigned char>(destination.data(), destination.size()), boundMainMemory);
	}

	bool MemorySubsystem::ExecuteControllerVectorTransferReadStep(unsigned long long deviceId, unsigned long long sourceAddress, unsigned long long elementCount, int elementSize, unsigned short stride, CanonicalVectorPhysicalBankEnvelope physicalBankEnvelope, std::vector<unsigned char> &packedDestination)
	{
		{
			std::lock_guard<std::mutex> lock(geometryLifecycleGate);
			ArgumentNullException::ThrowIfNull(packedDestination);
			if (elementCount == 0 || elementSize <= 0 || stride == 0 || elementCount > std::numeric_limits<int>::max())
			{
				return false;
			}

			std::shared_ptr<PhysicalMemoryBankGeometry> geometry = _publishedPhysicalBankGeometry;
			if (!physicalBankEnvelope.isWellFormed() || physicalBankEnvelope.getGeneration() != *geometry->Generation || physicalBankEnvelope.getElementCount() != elementCount)
			{
				return false;
			}

			for (int elementIndex = 0; elementIndex < physicalBankEnvelope.getCount(); elementIndex++)
			{
				if (physicalBankEnvelope.GetSourceBankIndex(elementIndex).getValue() >= geometry->BankCount)
				{
					return false;
				}
			}

			try
			{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'checked' in this context:
// ORIGINAL LINE: if ((ulong)packedDestination.size() != checked(elementCount * (ulong)elementSize))
				if (static_cast<unsigned long long>(packedDestination.size()) != elementCount * static_cast<unsigned long long>(elementSize))
				{
					return false;
				}
			}
			catch (const std::overflow_error &e1)
			{
				return false;
			}

			if (stride == elementSize)
			{
				return ExecuteControllerCanonicalVectorElementReadStepUnderOwnerGate(deviceId, sourceAddress, packedDestination);
			}

			for (unsigned long long element = 0; element < elementCount; element++)
			{
				unsigned long long address;
				int offset;
				try
				{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'checked' in this context:
// ORIGINAL LINE: address = checked(sourceAddress + checked(element * stride));
					address = sourceAddress + element * stride;
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'checked' in this context:
// ORIGINAL LINE: offset = checked((int)(element * (ulong)elementSize));
					offset = static_cast<int>(element * static_cast<unsigned long long>(elementSize));
				}
				catch (const std::overflow_error &e2)
				{
					return false;
				}

				std::vector<unsigned char> elementBuffer(elementSize);
				if (!ExecuteControllerCanonicalVectorElementReadStepUnderOwnerGate(deviceId, address, elementBuffer))
				{
					return false;
				}

				std::copy(elementBuffer.begin(), elementBuffer.end(), packedDestination.begin() + offset);
			}

			return true;
		}
	}

	void MemorySubsystem::OnBurstStarted(unsigned long long address, int length, bool isRead, int bankId)
	{
		if (BurstStarted != nullptr)
		{
			std::shared_ptr<BurstEventArgs> temp_var = std::make_shared<BurstEventArgs>();
			temp_var->setAddress(address);
			temp_var->setLength(length);
			temp_var->setRead(isRead);
			temp_var->setBankId(bankId);
			temp_var->setTimestamp(currentCycle);
			for (auto listener : BurstStarted->listeners())
			{
				listener(this, temp_var);
			}
		}
	}

	void MemorySubsystem::OnBurstCompleted(unsigned long long address, int length, bool isRead, int bankId)
	{
		if (BurstCompleted != nullptr)
		{
			std::shared_ptr<BurstEventArgs> temp_var = std::make_shared<BurstEventArgs>();
			temp_var->setAddress(address);
			temp_var->setLength(length);
			temp_var->setRead(isRead);
			temp_var->setBankId(bankId);
			temp_var->setTimestamp(currentCycle);
			for (auto listener : BurstCompleted->listeners())
			{
				listener(this, temp_var);
			}
		}
	}

	int MemorySubsystem::CalculatePortSwitchPenalty(int portId, int newBankId)
	{
		if (portId < 0 || portId >= _portStates.size())
		{
			return 0;
		}

// C# TO C++ CONVERTER TASK: 'ref locals' are not converted by C# to C++ Converter:
// ORIGINAL LINE: ref PortState port = ref _portStates[portId];
		PortState port = _portStates[portId];

		// If port is switching to a different bank, apply penalty
		if (port.CurrentBankId != -1 && port.CurrentBankId != newBankId)
		{
			return getPortSwitchingPenalty();
		}

		return 0; // No penalty if same bank or port was idle
	}

	int MemorySubsystem::CalculateMemoryLatency(unsigned long long address, int size)
	{
		// Calculate row address
		unsigned long long rowAddress = address / static_cast<unsigned long long>(getRowBufferSize());

		std::shared_ptr<PhysicalMemoryBankIndex> bankIndex = ComputeBankId(address);
		unsigned long long lastRowAddress = bankLastAccessCycle[bankIndex->getValue()] > 0 ? static_cast<unsigned long long>(bankLastAccessCycle[bankIndex->getValue()]) / static_cast<unsigned long long>(getRowBufferSize()) : std::numeric_limits<unsigned long long>::max();

		// Row buffer hit if accessing same row
		if (rowAddress == lastRowAddress)
		{
			return getRowBufferHitLatency();
		}

		// Row buffer miss - need to load new row
		return getRowBufferMissLatency();
	}

	int MemorySubsystem::AllocatePortForBank(int bankId)
	{
		return AllocatePortForBank(bankId, true);
	}

	int MemorySubsystem::AllocatePortForBank(int bankId, bool isRead)
	{
		// Prefer idle ports that are already ready for the requested direction.
		for (int i = 0; i < _portStates.size(); i++)
		{
			if (!_portStates[i].Busy && IsPortDirectionReady(i, isRead))
			{
				_portStates[i].Busy = true;
				_portStates[i].CurrentBankId = bankId;
				_portStates[i].LastSwitchCycle = currentCycle;
				return i;
			}
		}

		// Then fall back to any idle port if the caller forces the issue.
		for (int i = 0; i < _portStates.size(); i++)
		{
			if (!_portStates[i].Busy)
			{
				_portStates[i].Busy = true;
				_portStates[i].CurrentBankId = bankId;
				_portStates[i].LastSwitchCycle = currentCycle;
				return i;
			}
		}

		int portId = bankId % _portStates.size();
		_portStates[portId].CurrentBankId = bankId;
		_portStates[portId].LastSwitchCycle = currentCycle;
		return portId;
	}

	void MemorySubsystem::ReleasePortAfterOperation(int portId)
	{
		ReleasePortAfterOperation(portId, -1, true);
	}

	void MemorySubsystem::ReleasePortAfterOperation(int portId, int bankId, bool isRead)
	{
		if (portId >= 0 && portId < _portStates.size())
		{
			_portStates[portId].Busy = false;
			if (bankId >= 0)
			{
				_portStates[portId].CurrentBankId = bankId;
			}

			_portStates[portId].LastSwitchCycle = currentCycle;
			_portStates[portId].HasLastDirection = true;
			_portStates[portId].LastWasRead = isRead;
			_portStates[portId].DirectionReadyCycle = currentCycle + getReadWriteTurnaroundPenalty();
		}
	}

	inline bool MemorySubsystem::IsPortDirectionReady(int portId, bool isRead)
	{
		if (static_cast<unsigned int>(portId) >= static_cast<unsigned int>(_portStates.size()))
		{
			return false;
		}

// C# TO C++ CONVERTER TASK: 'ref locals' are not converted by C# to C++ Converter:
// ORIGINAL LINE: ref readonly PortState port = ref _portStates[portId];
		PortState port = _portStates[portId];
		if (!port.HasLastDirection || port.LastWasRead == isRead)
		{
			return true;
		}

		return currentCycle >= port.DirectionReadyCycle;
	}

	inline int MemorySubsystem::CountDirectionallyReadyPorts(bool isRead)
	{
		int readyPorts = 0;
		for (int i = 0; i < _portStates.size(); i++)
		{
			if (IsPortDirectionReady(i, isRead))
			{
				readyPorts++;
			}
		}

		return readyPorts;
	}

	void MemorySubsystem::RegisterAcceleratorDevice(unsigned long long deviceId, YAKSys_Hybrid_CPU::Execution::AcceleratorDMACapabilities capabilities)
	{
		AcceleratorRuntimeFailClosed::ThrowRegistrationNotSupported();
	}

	std::shared_ptr<YAKSys_Hybrid_CPU::Execution::DMATransferToken> MemorySubsystem::InitiateAcceleratorDMA(unsigned long long deviceId, unsigned long long srcAddr, unsigned long long dstAddr, int size)
	{
		return AcceleratorRuntimeFailClosed::ThrowTransferNotSupported();
	}

	std::shared_ptr<MemoryRequestToken> MemorySubsystem::EnqueueRead(unsigned long long deviceID, unsigned long long address, int size, std::vector<unsigned char> &buffer)
	{
		if (buffer.empty() || buffer.size() < size)
		{
			throw std::invalid_argument("Buffer is null or too small");
		}

		{
			std::lock_guard<std::mutex> lock(geometryLifecycleGate);
			unsigned long long requestID = nextRequestID++;
			std::shared_ptr<PhysicalMemoryBankIndex> bankIndex = ComputeBankId(address);
			std::shared_ptr<PhysicalMemoryBankBinding> physicalBankBinding = PhysicalMemoryBankBinding::Create(bankIndex, _publishedPhysicalBankGeometry->Generation);
			std::shared_ptr<MemoryRequestToken> token = std::make_shared<MemoryRequestToken>(requestID, deviceID, address, size, buffer, true, currentCycle, false, physicalBankBinding);

			{
			// Store in pending requests
				std::lock_guard<std::mutex> lock(pendingRequestsGate);
				pendingRequests[requestID] = token;
			}

			// Enqueue to appropriate bank
			BankRequest bankRequest = {.RequestID = requestID, .DeviceID = deviceID, .Address = address, .Length = size, .IsRead = true, .Priority = 5, .EnqueueCycle = currentCycle, .Buffer = buffer, .PhysicalBankBinding = physicalBankBinding};

			bankQueues[physicalBankBinding->BankIndex->getValue()].push_back(bankRequest);

			return token;
		}
	}

	std::shared_ptr<MemoryRequestToken> MemorySubsystem::EnqueueWrite(unsigned long long deviceID, unsigned long long address, int size, std::vector<unsigned char> &buffer, bool deferPhysicalWriteUntilRetire)
	{
		if (buffer.empty() || buffer.size() < size)
		{
			throw std::invalid_argument("Buffer is null or too small");
		}

		{
			std::lock_guard<std::mutex> lock(geometryLifecycleGate);
			unsigned long long requestID = nextRequestID++;
			std::shared_ptr<PhysicalMemoryBankIndex> bankIndex = ComputeBankId(address);
			std::shared_ptr<PhysicalMemoryBankBinding> physicalBankBinding = PhysicalMemoryBankBinding::Create(bankIndex, _publishedPhysicalBankGeometry->Generation);
			std::shared_ptr<MemoryRequestToken> token = std::make_shared<MemoryRequestToken>(requestID, deviceID, address, size, buffer, false, currentCycle, deferPhysicalWriteUntilRetire, physicalBankBinding);

			{
			// Store in pending requests
				std::lock_guard<std::mutex> lock(pendingRequestsGate);
				pendingRequests[requestID] = token;
			}

			// Enqueue to appropriate bank
			BankRequest bankRequest = {.RequestID = requestID, .DeviceID = deviceID, .Address = address, .Length = size, .IsRead = false, .DefersPhysicalWriteUntilRetire = deferPhysicalWriteUntilRetire, .Priority = 5, .EnqueueCycle = currentCycle, .Buffer = buffer, .PhysicalBankBinding = physicalBankBinding};

			bankQueues[physicalBankBinding->BankIndex->getValue()].push_back(bankRequest);

			return token;
		}
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public bool CancelPendingRequest(MemoryRequestToken? token)
	bool MemorySubsystem::CancelPendingRequest(std::shared_ptr<MemoryRequestToken> token)
	{
		return token != nullptr && CancelPendingRequest(token->getRequestID());
	}

	bool MemorySubsystem::CancelPendingRequest(unsigned long long requestID)
	{
		if (requestID == 0)
		{
			return false;
		}

		{
			std::lock_guard<std::mutex> lock(geometryLifecycleGate);
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: MemoryRequestToken? token;
			std::shared_ptr<MemoryRequestToken> token;
			std::shared_ptr<PhysicalMemoryBankGeometry> geometry = _publishedPhysicalBankGeometry;
			std::shared_ptr<PhysicalMemoryBankBinding> physicalBankBinding;
			{
				std::lock_guard<std::mutex> lock(pendingRequestsGate);
				auto pendingRequests_iterator = pendingRequests.find(requestID);
				if (pendingRequests_iterator == pendingRequests.end() || !pendingRequests_iterator->second || pendingRequests_iterator->second->isComplete())
				{
					return false;
				}
				token = pendingRequests_iterator->second;

				physicalBankBinding = token->GetPhysicalBankBindingForOwner();
				if (!physicalBankBinding->IsWellFormed || physicalBankBinding->Generation->getValue() != geometry->Generation->getValue() || physicalBankBinding->BankIndex->getValue() >= geometry->BankCount)
				{
					return false;
				}

				pendingRequests.erase(requestID);
			}

			return RemoveQueuedBankRequest(requestID, physicalBankBinding->BankIndex->getValue());
		}
	}
}
