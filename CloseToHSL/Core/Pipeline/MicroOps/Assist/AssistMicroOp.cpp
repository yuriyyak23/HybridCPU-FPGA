#include "AssistMicroOp.h"
#include "../Vector/VectorMicroOps.h"
#include "../../../../../Arch/DataType.h"
#include "../../../../../NonRTL/Arch/InstructionClass.h"
#include "../../../../../NonRTL/Arch/SerializationClass.h"
#include "../../../Execution/Memory/LoadStore/MemoryBankRouting.h"
#include "../Memory/MicroOp.LoadStore.h"
#include "../../Metadata/MicroOpAdmissionMetadata.h"
#include "../../../Architecture/Common/StreamEngineId.h"
#include <algorithm>

using namespace YAKSys_Hybrid_CPU::Arch;
using namespace YAKSys_Hybrid_CPU::Core::Memory;
using namespace YAKSys_Hybrid_CPU::Core::Pipeline::Metadata;

namespace YAKSys_Hybrid_CPU::Core
{
	static const wchar_t *AssistKindName(AssistKind kind)
	{
		switch (kind)
		{
		case AssistKind::DonorPrefetch: return L"DonorPrefetch";
		case AssistKind::Ldsa: return L"Ldsa";
		case AssistKind::Vdsa: return L"Vdsa";
		default: return L"Unknown";
		}
	}

	static const wchar_t *SlotClassName(SlotClass slotClass)
	{
		switch (slotClass)
		{
		case SlotClass::AluClass: return L"AluClass";
		case SlotClass::LsuClass: return L"LsuClass";
		case SlotClass::DmaStreamClass: return L"DmaStreamClass";
		case SlotClass::BranchControl: return L"BranchControl";
		case SlotClass::SystemSingleton: return L"SystemSingleton";
		case SlotClass::MatrixTileStreamClass: return L"MatrixTileStreamClass";
		case SlotClass::Unclassified: return L"Unclassified";
		default: return L"Unknown";
		}
	}

	static const wchar_t *AssistDonorSourceKindName(AssistDonorSourceKind kind)
	{
		switch (kind)
		{
		case AssistDonorSourceKind::SameThreadSeed: return L"SameThreadSeed";
		case AssistDonorSourceKind::IntraCoreVtDonorVector: return L"IntraCoreVtDonorVector";
		case AssistDonorSourceKind::InterCoreVtDonorVector: return L"InterCoreVtDonorVector";
		case AssistDonorSourceKind::InterCoreSameVtSeed: return L"InterCoreSameVtSeed";
		case AssistDonorSourceKind::InterCoreVtDonorSeed: return L"InterCoreVtDonorSeed";
		case AssistDonorSourceKind::InterCoreSameVtVector: return L"InterCoreSameVtVector";
		case AssistDonorSourceKind::InterCoreSameVtVectorWriteback: return L"InterCoreSameVtVectorWriteback";
		case AssistDonorSourceKind::InterCoreVtDonorVectorWriteback: return L"InterCoreVtDonorVectorWriteback";
		case AssistDonorSourceKind::InterCoreSameVtHotLoadSeed: return L"InterCoreSameVtHotLoadSeed";
		case AssistDonorSourceKind::InterCoreVtDonorHotLoadSeed: return L"InterCoreVtDonorHotLoadSeed";
		case AssistDonorSourceKind::InterCoreSameVtHotStoreSeed: return L"InterCoreSameVtHotStoreSeed";
		case AssistDonorSourceKind::InterCoreVtDonorHotStoreSeed: return L"InterCoreVtDonorHotStoreSeed";
		case AssistDonorSourceKind::InterCoreSameVtColdStoreSeed: return L"InterCoreSameVtColdStoreSeed";
		case AssistDonorSourceKind::InterCoreVtDonorColdStoreSeed: return L"InterCoreVtDonorColdStoreSeed";
		case AssistDonorSourceKind::InterCoreSameVtDefaultStoreSeed: return L"InterCoreSameVtDefaultStoreSeed";
		case AssistDonorSourceKind::InterCoreVtDonorDefaultStoreSeed: return L"InterCoreVtDonorDefaultStoreSeed";
		default: return L"Unknown";
		}
	}

	CanonicalDecodePublicationMode AssistMicroOp::getCanonicalDecodePublication() const
	{
		return CanonicalDecodePublicationMode::SelfPublishes;
	}

	AssistMicroOp::AssistMicroOp(AssistKind kind, AssistExecutionMode executionMode, AssistCarrierKind carrierKind, unsigned long long baseAddress, unsigned long long prefetchLength, unsigned char elementSize, unsigned int elementCount, AssistOwnerBinding ownerBinding)
		: Kind(kind), ExecutionMode(executionMode), CarrierKind(carrierKind), BaseAddress(baseAddress),
		  PrefetchLength(prefetchLength == 0 ? CacheLineSize : prefetchLength),
		  ElementSize(elementSize == 0 ? static_cast<unsigned char>(1) : elementSize),
		  ElementCount(elementCount == 0 ? 1u : elementCount),
		  CarrierVirtualThreadId(ownerBinding.getCarrierVirtualThreadId()), DonorVirtualThreadId(ownerBinding.getDonorVirtualThreadId()),
		  TargetVirtualThreadId(ownerBinding.getTargetVirtualThreadId()), CarrierCoreId(ownerBinding.getCarrierCoreId()),
		  TargetCoreId(ownerBinding.getTargetCoreId()), PodId(ownerBinding.getPodId()),
		  ReplayEpochId(ownerBinding.getReplayEpochId()), AssistEpochId(ownerBinding.getAssistEpochId()),
		  LocalityHint(ownerBinding.getLocalityHint()), DonorSource(ownerBinding.getDonorSource()),
		  MemoryBankId(ResolveMemoryBankId(baseAddress))
	{
		ValidateSupportedTuple(kind, executionMode, carrierKind, ownerBinding);
		setMemoryLocalityHint(ownerBinding.getLocalityHint());

		setOpCode(Processor::CPU_Core::IsaOpcodeValues::Nope);
		setLatency(1);
		setMemoryOp(true);
		setWritesRegister(false);
		setHasSideEffects(false);
		SlotClass carrierSlotClass = YAKSys_Hybrid_CPU::Core::AssistCarrierKindExtensions::ResolveSlotClass(carrierKind);
		setClass(carrierSlotClass == SlotClass::DmaStreamClass ? MicroOpClass::Dma : MicroOpClass::Lsu);
		setInstructionClass(InstructionClass::Memory);
		setSerializationClass(SerializationClass::Free);
		SetClassFlexiblePlacement(carrierSlotClass);
// C# TO C++ CONVERTER TASK: C# 'with expressions' are not converted by C# to C++ Converter:
//		Placement = Placement with { DomainTag = ownerBinding.DomainTag };
		setOwnerThreadId(ownerBinding.getTargetVirtualThreadId());
		setOwnerContextId(ownerBinding.getOwnerContextId());
		setVirtualThreadId(ownerBinding.getTargetVirtualThreadId());

		setReadRegisters({});
		setWriteRegisters({});
		setReadMemoryRanges({{getBaseAddress(), getPrefetchLength()}});
		setWriteMemoryRanges({});

		if (carrierSlotClass == SlotClass::DmaStreamClass)
		{
			std::shared_ptr<StreamEngineId> streamEngine = std::make_shared<StreamEngineId>(StreamEngineId::Zero);
			setResourceMask(ResourceMaskBuilder::ForDMAChannel(0));
			setSafetyMask(ResourceMaskBuilder::ForDMAChannel128(0));
			setResourceMask(getResourceMask() | ResourceMaskBuilder::ForStreamEngine(streamEngine));
			setSafetyMask(getSafetyMask() | ResourceMaskBuilder::ForStreamEngine128(streamEngine));
		}
		else
		{
			setResourceMask(ResourceMaskBuilder::ForLoad());
			setSafetyMask(ResourceMaskBuilder::ForLoad128());
		}

		if (MemoryBankRouting::IsResolvedSchedulerVisibleBankId(getMemoryBankId()))
		{
			setResourceMask(getResourceMask() | ResourceMaskBuilder::ForMemoryBank(getMemoryBankId()));
			setSafetyMask(getSafetyMask() | ResourceMaskBuilder::ForMemoryBank128(getMemoryBankId()));
		}

		setOriginalResourceMask(getResourceMask());
		RefreshAdmissionMetadata();
	}

	AssistKind AssistMicroOp::getKind() const
	{
		return Kind;
	}

	AssistExecutionMode AssistMicroOp::getExecutionMode() const
	{
		return ExecutionMode;
	}

	AssistCarrierKind AssistMicroOp::getCarrierKind() const
	{
		return CarrierKind;
	}

	unsigned long long AssistMicroOp::getBaseAddress() const
	{
		return BaseAddress;
	}

	unsigned long long AssistMicroOp::getPrefetchLength() const
	{
		return PrefetchLength;
	}

	unsigned char AssistMicroOp::getElementSize() const
	{
		return ElementSize;
	}

	unsigned int AssistMicroOp::getElementCount() const
	{
		return ElementCount;
	}

	int AssistMicroOp::getCarrierVirtualThreadId() const
	{
		return CarrierVirtualThreadId;
	}

	int AssistMicroOp::getDonorVirtualThreadId() const
	{
		return DonorVirtualThreadId;
	}

	int AssistMicroOp::getTargetVirtualThreadId() const
	{
		return TargetVirtualThreadId;
	}

	int AssistMicroOp::getCarrierCoreId() const
	{
		return CarrierCoreId;
	}

	int AssistMicroOp::getTargetCoreId() const
	{
		return TargetCoreId;
	}

	unsigned short AssistMicroOp::getPodId() const
	{
		return PodId;
	}

	unsigned long long AssistMicroOp::getReplayEpochId() const
	{
		return ReplayEpochId;
	}

	unsigned long long AssistMicroOp::getAssistEpochId() const
	{
		return AssistEpochId;
	}

	YAKSys_Hybrid_CPU::Core::LocalityHint AssistMicroOp::getLocalityHint() const
	{
		return LocalityHint;
	}

	AssistDonorSourceDescriptor AssistMicroOp::getDonorSource() const
	{
		return DonorSource;
	}

	int AssistMicroOp::getMemoryBankId() const
	{
		return MemoryBankId;
	}

	bool AssistMicroOp::getHasResolvedMemoryBankId() const
	{
		return MemoryBankRouting::IsResolvedSchedulerVisibleBankId(getMemoryBankId());
	}

	SlotClass AssistMicroOp::getCarrierSlotClass() const
	{
		return getPlacement().RequiredSlotClass;
	}

	bool AssistMicroOp::getHasExplicitCoreOwnership() const
	{
		return getCarrierCoreId() >= 0 || getTargetCoreId() >= 0;
	}

	unsigned int AssistMicroOp::getReservedPrefetchLines() const
	{
		return ReservedPrefetchLines;
	}

	void AssistMicroOp::setReservedPrefetchLines(unsigned int value)
	{
		ReservedPrefetchLines = value;
	}

	bool AssistMicroOp::isAssist() const
	{
		return true;
	}

	bool AssistMicroOp::isRetireVisible() const
	{
		return false;
	}

	bool AssistMicroOp::isReplayDiscardable() const
	{
		return true;
	}

	bool AssistMicroOp::getSuppressesArchitecturalFaults() const
	{
		return true;
	}

	bool AssistMicroOp::Execute(std::shared_ptr<Processor::CPU_Core> &core)
	{
		return core->ExecuteAssistMicroOp(shared_from_this());
	}

	std::wstring AssistMicroOp::GetDescription()
	{
		std::wstring coreScope = getHasExplicitCoreOwnership()
			? std::format(L", CarrierCore={}, TargetCore={}, Pod={}", getCarrierCoreId(), getTargetCoreId(), getPodId())
			: L"";
		return std::format(L"Assist[{}/{}/{}] Base=0x{:X}, Len={}, CarrierVT={}, DonorVT={}, TargetVT={}{}",
			AssistKindName(getKind()), SlotClassName(getCarrierSlotClass()), AssistDonorSourceKindName(getDonorSource().getKind()),
			getBaseAddress(), getPrefetchLength(), getCarrierVirtualThreadId(), getDonorVirtualThreadId(), getTargetVirtualThreadId(), coreScope);
	}

	unsigned int AssistMicroOp::EstimatePrefetchLineDemand(AssistMemoryQuota quota)
	{
		unsigned long long requestedLines = std::max(1ULL, (getPrefetchLength() + CacheLineSize - 1ULL) / CacheLineSize);
		unsigned long long cappedLines = std::min<unsigned long long>(requestedLines, quota.ResolveLineCap(getLocalityHint()));
		return static_cast<unsigned int>(cappedLines);
	}

	unsigned int AssistMicroOp::ResolvePrefetchLineBudget(AssistMemoryQuota quota)
	{
		return getReservedPrefetchLines() == 0 ? EstimatePrefetchLineDemand(quota) : getReservedPrefetchLines();
	}

	void AssistMicroOp::BindReservedPrefetchLines(unsigned int reservedPrefetchLines)
	{
		setReservedPrefetchLines(std::max(1u, reservedPrefetchLines));
	}

	bool AssistMicroOp::TryCreateFromSeed(std::shared_ptr<MicroOp> seed, int carrierVirtualThreadId, unsigned long long replayEpochId, unsigned long long assistEpochId, std::shared_ptr<AssistMicroOp> &assistMicroOp)
	{
	// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
	// ORIGINAL LINE: assistMicroOp = null!;
		assistMicroOp = nullptr;
		if (seed == nullptr)
		{
			return false;
		}

		int donorVirtualThreadId = NormalizeAssistVirtualThreadId(seed->getVirtualThreadId(), seed->getOwnerThreadId());
		return TryCreateFromSeed(seed, carrierVirtualThreadId, donorVirtualThreadId, replayEpochId, assistEpochId, assistMicroOp);
	}

	bool AssistMicroOp::TryCreateFromSeed(std::shared_ptr<MicroOp> seed, int carrierVirtualThreadId, int targetVirtualThreadId, unsigned long long replayEpochId, unsigned long long assistEpochId, std::shared_ptr<AssistMicroOp> &assistMicroOp)
	{
	// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
	// ORIGINAL LINE: assistMicroOp = null!;
		assistMicroOp = nullptr;
		if (!IsSupportedAssistSeed(seed, false))
		{
			return false;
		}

		AssistKind kind;
		AssistExecutionMode executionMode;
		AssistCarrierKind carrierKind;
		unsigned long long baseAddress;
		unsigned long long prefetchLength;
		unsigned char elementSize;
		unsigned int elementCount;
		YAKSys_Hybrid_CPU::Core::LocalityHint localityHint;
		bool isWriteBackVectorSeed = false;
		bool isHotLoadScalarSeed = false;
		bool isHotStoreScalarSeed = false;
		bool isColdStoreScalarSeed = false;
		bool isDefaultStoreScalarSeed = false;
		if (!TryClassifySeed(seed, false, false, false, kind, executionMode, carrierKind, baseAddress, prefetchLength, elementSize, elementCount, localityHint, isWriteBackVectorSeed, isHotLoadScalarSeed, isHotStoreScalarSeed, isColdStoreScalarSeed, isDefaultStoreScalarSeed))
		{
			return false;
		}

		int donorVirtualThreadId = NormalizeAssistVirtualThreadId(seed->getVirtualThreadId(), seed->getOwnerThreadId());
		targetVirtualThreadId = NormalizeAssistVirtualThreadId(targetVirtualThreadId, donorVirtualThreadId);
		AssistDonorSourceDescriptor donorSource;
		if (!AssistDonorSourceDescriptor::TryCreate(kind, executionMode, carrierKind, donorVirtualThreadId, targetVirtualThreadId, seed->getOwnerContextId(), seed->getPlacement().DomainTag, donorSource))
		{
			return false;
		}

		AssistOwnerBinding binding = AssistOwnerBinding(carrierVirtualThreadId, donorVirtualThreadId, targetVirtualThreadId, seed->getOwnerContextId(), seed->getPlacement().DomainTag, replayEpochId, assistEpochId, localityHint, donorSource);

		assistMicroOp = std::make_shared<AssistMicroOp>(kind, executionMode, carrierKind, baseAddress, prefetchLength, elementSize, elementCount, binding);
		return true;
	}

	bool AssistMicroOp::TryCreateInterCoreTransportFromSeed(std::shared_ptr<MicroOp> seed, int donorCoreId, unsigned short donorPodId, unsigned long long donorAssistEpochId, AssistInterCoreTransport &transport)
	{
		transport = YAKSys_Hybrid_CPU::Core::AssistInterCoreTransport();
		if (!IsSupportedInterCoreSeed(seed))
		{
			return false;
		}

		transport = AssistInterCoreTransport(seed, donorCoreId, donorPodId, donorAssistEpochId);
		return transport.isValid();
	}

	bool AssistMicroOp::TryCreateInterCoreTransportFromSeed(std::shared_ptr<MicroOp> seed, int donorCoreId, unsigned short donorPodId, AssistInterCoreTransport &transport)
	{
		return TryCreateInterCoreTransportFromSeed(seed, donorCoreId, donorPodId, 0, transport);
	}

	bool AssistMicroOp::TryCreateFromInterCoreTransport(AssistInterCoreTransport transport, unsigned short podId, int carrierCoreId, int carrierVirtualThreadId, int targetCoreId, int targetVirtualThreadId, int targetOwnerContextId, unsigned long long targetDomainTag, unsigned long long replayEpochId, unsigned long long assistEpochId, std::shared_ptr<AssistMicroOp> &assistMicroOp)
	{
	// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
	// ORIGINAL LINE: assistMicroOp = null!;
		assistMicroOp = nullptr;
		if (!transport.isValid())
		{
			return false;
		}

		std::shared_ptr<MicroOp> seed = transport.getSeed();
		AssistKind kind;
		AssistExecutionMode executionMode;
		AssistCarrierKind carrierKind;
		unsigned long long baseAddress;
		unsigned long long prefetchLength;
		unsigned char elementSize;
		unsigned int elementCount;
		YAKSys_Hybrid_CPU::Core::LocalityHint localityHint;
		bool isWriteBackVectorSeed;
		bool isHotLoadScalarSeed;
		bool isHotStoreScalarSeed;
		bool isColdStoreScalarSeed;
		bool isDefaultStoreScalarSeed;
		if (!TryClassifyInterCoreSeed(seed, kind, executionMode, carrierKind, baseAddress, prefetchLength, elementSize, elementCount, localityHint, isWriteBackVectorSeed, isHotLoadScalarSeed, isHotStoreScalarSeed, isColdStoreScalarSeed, isDefaultStoreScalarSeed))
		{
			return false;
		}

		if (!IsInterCoreTransportEligible(kind, executionMode, carrierKind))
		{
			return false;
		}

		int donorVirtualThreadId = NormalizeAssistVirtualThreadId(transport.getDonorVirtualThreadId(), transport.getDonorVirtualThreadId());
		targetVirtualThreadId = NormalizeAssistVirtualThreadId(targetVirtualThreadId, donorVirtualThreadId);

		AssistDonorSourceDescriptor donorSource;
		if (!AssistDonorSourceDescriptor::TryCreate(kind, executionMode, carrierKind, donorVirtualThreadId, targetVirtualThreadId, transport.getDonorOwnerContextId(), transport.getDonorDomainTag(), transport.getDonorCoreId(), transport.getDonorPodId(), transport.getDonorAssistEpochId(), isWriteBackVectorSeed, isHotLoadScalarSeed, isHotStoreScalarSeed, isColdStoreScalarSeed, isDefaultStoreScalarSeed, donorSource))
		{
			return false;
		}

		AssistOwnerBinding binding = AssistOwnerBinding(carrierVirtualThreadId, donorVirtualThreadId, targetVirtualThreadId, targetOwnerContextId, targetDomainTag, replayEpochId, assistEpochId, localityHint, donorSource, carrierCoreId, targetCoreId, podId);

		assistMicroOp = std::make_shared<AssistMicroOp>(kind, executionMode, carrierKind, baseAddress, prefetchLength, elementSize, elementCount, binding);
		return true;
	}

	bool AssistMicroOp::IsInterCoreTransportEligible(AssistKind kind, AssistExecutionMode executionMode, AssistCarrierKind carrierKind)
	{
		if (!AssistTupleSupport::IsSupportedCarrierTuple(kind, executionMode, carrierKind))
		{
			return false;
		}

		const bool cachePrefetchOnLsu = executionMode == AssistExecutionMode::CachePrefetch && carrierKind == AssistCarrierKind::LsuHosted;
		const bool streamPrefetchOnLane6 = executionMode == AssistExecutionMode::StreamRegisterPrefetch && carrierKind == AssistCarrierKind::Lane6Dma;
		switch (kind)
		{
		case AssistKind::Vdsa:
			return streamPrefetchOnLane6;
		case AssistKind::Ldsa:
		case AssistKind::DonorPrefetch:
			return cachePrefetchOnLsu || streamPrefetchOnLane6;
		default:
			return false;
		}
	}

	bool AssistMicroOp::IsSupportedInterCoreSeed(std::shared_ptr<MicroOp> seed)
	{
		if (!IsSupportedAssistSeed(seed, true))
		{
			return false;
		}

		AssistKind kind;
		AssistExecutionMode executionMode;
		AssistCarrierKind carrierKind;
		unsigned long long baseAddress = 0;
		unsigned long long prefetchLength = 0;
		unsigned char elementSize = 1;
		unsigned int elementCount = 1;
		YAKSys_Hybrid_CPU::Core::LocalityHint localityHint = YAKSys_Hybrid_CPU::Core::LocalityHint::None;
		bool isWriteBackVectorSeed = false;
		bool isHotLoadScalarSeed = false;
		bool isHotStoreScalarSeed = false;
		bool isColdStoreScalarSeed = false;
		bool isDefaultStoreScalarSeed = false;
		if (!TryClassifyInterCoreSeed(seed, kind, executionMode, carrierKind, baseAddress, prefetchLength, elementSize, elementCount, localityHint, isWriteBackVectorSeed, isHotLoadScalarSeed, isHotStoreScalarSeed, isColdStoreScalarSeed, isDefaultStoreScalarSeed))
		{
			return false;
		}

		return IsInterCoreTransportEligible(kind, executionMode, carrierKind);
	}

	bool AssistMicroOp::TryClassifyInterCoreSeed(std::shared_ptr<MicroOp> seed, AssistKind &kind, AssistExecutionMode &executionMode, AssistCarrierKind &carrierKind, unsigned long long &baseAddress, unsigned long long &prefetchLength, unsigned char &elementSize, unsigned int &elementCount, YAKSys_Hybrid_CPU::Core::LocalityHint &localityHint, bool &isWriteBackVectorSeed, bool &isHotLoadScalarSeed, bool &isHotStoreScalarSeed, bool &isColdStoreScalarSeed, bool &isDefaultStoreScalarSeed)
	{
		return TryClassifySeed(seed, true, true, true, kind, executionMode, carrierKind, baseAddress, prefetchLength, elementSize, elementCount, localityHint, isWriteBackVectorSeed, isHotLoadScalarSeed, isHotStoreScalarSeed, isColdStoreScalarSeed, isDefaultStoreScalarSeed);
	}

	bool AssistMicroOp::TryClassifySeed(std::shared_ptr<MicroOp> seed, bool widenInterCoreScalarLoadAssistToLane6, bool allowInterCoreScalarStoreAssist, bool allowInterCoreVectorWriteBackAssist, AssistKind &kind, AssistExecutionMode &executionMode, AssistCarrierKind &carrierKind, unsigned long long &baseAddress, unsigned long long &prefetchLength, unsigned char &elementSize, unsigned int &elementCount, YAKSys_Hybrid_CPU::Core::LocalityHint &localityHint, bool &isWriteBackVectorSeed, bool &isHotLoadScalarSeed, bool &isHotStoreScalarSeed, bool &isColdStoreScalarSeed, bool &isDefaultStoreScalarSeed)
	{
		kind = AssistKind::DonorPrefetch;
		executionMode = AssistExecutionMode::CachePrefetch;
		carrierKind = AssistCarrierKind::LsuHosted;
		baseAddress = 0;
		prefetchLength = 0;
		elementSize = 1;
		elementCount = 1;
		localityHint = seed->getMemoryLocalityHint();
		isWriteBackVectorSeed = false;
		isHotLoadScalarSeed = false;
		isHotStoreScalarSeed = false;
		isColdStoreScalarSeed = false;
		isDefaultStoreScalarSeed = false;

		if (!TryResolvePrimaryAssistWindow(seed, allowInterCoreScalarStoreAssist, allowInterCoreVectorWriteBackAssist, baseAddress, prefetchLength, isWriteBackVectorSeed))
		{
		return false;
		}

		std::shared_ptr<VectorMicroOp> vectorMicroOp = std::dynamic_pointer_cast<VectorMicroOp>(seed);
		if (vectorMicroOp != nullptr)
		{
			kind = AssistKind::Vdsa;
			executionMode = AssistExecutionMode::StreamRegisterPrefetch;
			carrierKind = AssistCarrierKind::Lane6Dma;

			int vectorElementSize = DataTypeUtils::SizeOf(vectorMicroOp->getInstruction().getDataTypeValue());
			elementSize = static_cast<unsigned char>(std::clamp(vectorElementSize, 1, 32));
			unsigned long long streamLength = vectorMicroOp->getInstruction().getStreamLength() == 0 ? 1 : vectorMicroOp->getInstruction().getStreamLength();
			elementCount = static_cast<unsigned int>(std::min(streamLength, 256ULL));
			prefetchLength = std::max(prefetchLength, static_cast<unsigned long long>(elementSize) * elementCount);
			return true;
		}

		std::shared_ptr<StoreMicroOp> storeMicroOp = std::dynamic_pointer_cast<StoreMicroOp>(seed);
		if (storeMicroOp != nullptr)
		{
			elementSize = storeMicroOp->getSize() == 0 ? static_cast<unsigned char>(1) : storeMicroOp->getSize();
			kind = allowInterCoreScalarStoreAssist && localityHint == LocalityHint::Cold ? AssistKind::Ldsa : AssistKind::DonorPrefetch;
			if (allowInterCoreScalarStoreAssist && localityHint == LocalityHint::Cold)
			{
				executionMode = AssistExecutionMode::StreamRegisterPrefetch;
				carrierKind = AssistCarrierKind::Lane6Dma;
				isColdStoreScalarSeed = true;
				prefetchLength = std::max(prefetchLength, CacheLineSize);
				elementCount = ResolveLane6ScalarElementCount(prefetchLength, elementSize);
				return true;
			}

			if (allowInterCoreScalarStoreAssist && localityHint == LocalityHint::Hot)
			{
				executionMode = AssistExecutionMode::StreamRegisterPrefetch;
				carrierKind = AssistCarrierKind::Lane6Dma;
				isHotStoreScalarSeed = true;
				prefetchLength = std::max(prefetchLength, CacheLineSize);
				elementCount = ResolveLane6ScalarElementCount(prefetchLength, elementSize);
				return true;
			}

			if (allowInterCoreScalarStoreAssist)
			{
				executionMode = AssistExecutionMode::StreamRegisterPrefetch;
				carrierKind = AssistCarrierKind::Lane6Dma;
				isDefaultStoreScalarSeed = true;
				prefetchLength = std::max(prefetchLength, CacheLineSize);
				elementCount = ResolveLane6ScalarElementCount(prefetchLength, elementSize);
				return true;
			}

			elementCount = 1;
			prefetchLength = std::max(prefetchLength, static_cast<unsigned long long>(elementSize));
			return true;
		}

		std::shared_ptr<LoadMicroOp> loadMicroOp = std::dynamic_pointer_cast<LoadMicroOp>(seed);
		if (loadMicroOp != nullptr)
		{
			elementSize = loadMicroOp->getSize() == 0 ? static_cast<unsigned char>(1) : loadMicroOp->getSize();
			kind = localityHint == LocalityHint::Cold ? AssistKind::Ldsa : AssistKind::DonorPrefetch;
			if (widenInterCoreScalarLoadAssistToLane6 && localityHint == LocalityHint::Hot)
			{
				executionMode = AssistExecutionMode::StreamRegisterPrefetch;
				carrierKind = AssistCarrierKind::Lane6Dma;
				isHotLoadScalarSeed = true;
				prefetchLength = std::max(prefetchLength, CacheLineSize);
				elementCount = ResolveLane6ScalarElementCount(prefetchLength, elementSize);
				return true;
			}

			if (widenInterCoreScalarLoadAssistToLane6)
			{
				executionMode = AssistExecutionMode::StreamRegisterPrefetch;
				carrierKind = AssistCarrierKind::Lane6Dma;
				prefetchLength = std::max(prefetchLength, CacheLineSize);
				elementCount = ResolveLane6ScalarElementCount(prefetchLength, elementSize);
				return true;
			}

			elementCount = 1;
			prefetchLength = std::max(prefetchLength, static_cast<unsigned long long>(elementSize));
			return true;
		}

		if (std::dynamic_pointer_cast<LoadStoreMicroOp>(seed) != nullptr)
		{
			kind = localityHint == LocalityHint::Cold ? AssistKind::Ldsa : AssistKind::DonorPrefetch;
			elementSize = static_cast<unsigned char>(std::clamp(static_cast<int>(std::min<unsigned long long>(prefetchLength, CacheLineSize)), 1, 32));
			elementCount = 1;
			prefetchLength = std::max(prefetchLength, static_cast<unsigned long long>(elementSize));
			return true;
		}

		return false;
	}

	bool AssistMicroOp::IsSupportedAssistSeed(std::shared_ptr<MicroOp> seed, bool allowVectorWriteBackSeed)
	{
		if (seed == nullptr || seed->isAssist())
		{
			return false;
		}

		if (seed->isMemoryOp())
		{
			return true;
		}

		if (std::dynamic_pointer_cast<VectorMicroOp>(seed) == nullptr)
		{
			return false;
		}

		return !seed->getReadMemoryRanges().empty() ||
			(allowVectorWriteBackSeed && !seed->getWriteMemoryRanges().empty());
	}

	unsigned int AssistMicroOp::ResolveLane6ScalarElementCount(unsigned long long prefetchLength, unsigned char elementSize)
	{
		unsigned long long normalizedElementSize = elementSize == 0 ? 1ULL : elementSize;
		unsigned long long normalizedPrefetchLength = std::max(prefetchLength, CacheLineSize);
		unsigned long long elementCount = std::max(1ULL, normalizedPrefetchLength / normalizedElementSize);
		return static_cast<unsigned int>(std::min(elementCount, 256ULL));
	}

	bool AssistMicroOp::TryResolvePrimaryAssistWindow(std::shared_ptr<MicroOp> seed, bool allowInterCoreScalarStoreAssist, bool allowInterCoreVectorWriteBackAssist, unsigned long long &baseAddress, unsigned long long &prefetchLength, bool &isWriteBackVectorSeed)
	{
		baseAddress = 0;
		prefetchLength = 0;
		isWriteBackVectorSeed = false;

		const auto &readRanges = seed->getReadMemoryRanges();
		if (!readRanges.empty())
		{
			const std::shared_ptr<MicroOpAdmissionMetadata> admissionMetadata = seed->getAdmissionMetadata();
			const auto &coalescing = admissionMetadata->AssistCoalescingDescriptorValue;
			if (coalescing.getIsValid())
			{
				baseAddress = coalescing.BaseAddress;
				prefetchLength = coalescing.PrefetchLength;
			}
			else
			{
				const auto &normalizedReadRanges = admissionMetadata->NormalizedReadMemoryRanges;
				if (!normalizedReadRanges.empty())
				{
					baseAddress = normalizedReadRanges.front().first;
					prefetchLength = normalizedReadRanges.front().second;
				}
				else
				{
					baseAddress = readRanges.front().Address;
					prefetchLength = readRanges.front().Length;
				}
			}

			prefetchLength = prefetchLength == 0 ? CacheLineSize : prefetchLength;
			return true;
		}

		const auto &writeRanges = seed->getWriteMemoryRanges();
		if (allowInterCoreScalarStoreAssist && std::dynamic_pointer_cast<StoreMicroOp>(seed) != nullptr && !writeRanges.empty())
		{
			baseAddress = writeRanges.front().Address;
			prefetchLength = writeRanges.front().Length;
			prefetchLength = prefetchLength == 0 ? CacheLineSize : prefetchLength;
			return true;
		}

		if (allowInterCoreVectorWriteBackAssist && std::dynamic_pointer_cast<VectorMicroOp>(seed) != nullptr && writeRanges.size() > 0)
		{
			baseAddress = writeRanges.front().Address;
			prefetchLength = writeRanges.front().Length;
			prefetchLength = prefetchLength == 0 ? CacheLineSize : prefetchLength;
			isWriteBackVectorSeed = true;
			return true;
		}

		return false;
	}

	void AssistMicroOp::ValidateSupportedTuple(AssistKind kind, AssistExecutionMode executionMode, AssistCarrierKind carrierKind, AssistOwnerBinding ownerBinding)
	{
		if (!AssistTupleSupport::IsSupportedCarrierTuple(kind, executionMode, carrierKind))
		{
// C# TO C++ CONVERTER TASK: This exception's constructor requires only one argument:
// ORIGINAL LINE: throw new ArgumentException("Assist kind/execution/carrier tuple is not supported by the landed runtime.", nameof(carrierKind));
			throw std::invalid_argument("Assist kind/execution/carrier tuple is not supported by the landed runtime.");
		}

		ValidateVirtualThreadId(ownerBinding.getCarrierVirtualThreadId(), L"ownerBinding");
		ValidateVirtualThreadId(ownerBinding.getDonorVirtualThreadId(), L"ownerBinding");
		ValidateVirtualThreadId(ownerBinding.getTargetVirtualThreadId(), L"ownerBinding");

		if (ownerBinding.getDonorSource().getDonorVirtualThreadId() != ownerBinding.getDonorVirtualThreadId() || ownerBinding.getDonorSource().getTargetVirtualThreadId() != ownerBinding.getTargetVirtualThreadId())
		{
// C# TO C++ CONVERTER TASK: This exception's constructor requires only one argument:
// ORIGINAL LINE: throw new ArgumentException("Assist owner binding must match donor-source virtual-thread identity.", nameof(ownerBinding));
			throw std::invalid_argument("Assist owner binding must match donor-source virtual-thread identity.");
		}

		if (!ownerBinding.getDonorSource().IsLegalFor(kind, executionMode, carrierKind))
		{
// C# TO C++ CONVERTER TASK: This exception's constructor requires only one argument:
// ORIGINAL LINE: throw new ArgumentException("Assist donor-source tuple is not supported by the landed runtime.", nameof(ownerBinding));
			throw std::invalid_argument("Assist donor-source tuple is not supported by the landed runtime.");
		}
	}

	int AssistMicroOp::NormalizeAssistVirtualThreadId(int virtualThreadId, int ownerThreadId)
	{
		int resolved = virtualThreadId >= 0 && virtualThreadId < Processor::CPU_Core::SmtWays ? virtualThreadId : ownerThreadId;
		return std::clamp(resolved, 0, Processor::CPU_Core::SmtWays - 1);
	}

	int AssistMicroOp::ResolveMemoryBankId(unsigned long long address)
	{
		return MemoryBankRouting::ResolveSchedulerVisibleBankId(address);
	}

	void AssistMicroOp::ValidateVirtualThreadId(int virtualThreadId, const std::wstring &paramName)
	{
		if (virtualThreadId < 0 || virtualThreadId >= Processor::CPU_Core::SmtWays)
		{
			throw ArgumentOutOfRangeException(paramName, virtualThreadId, L"Assist virtual-thread ownership must stay within the landed SMT contour.");
		}
	}
}
