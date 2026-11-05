#pragma once

#include "../Types/MicroOp.h"
#include "../../Assist/Runtime/AssistRuntime.h"
#include "../../Metadata/LocalityHint.h"
#include "../../Metadata/ISlotMetadataBuilder.h"
#include "../../Metadata/ReadRangeMetadataHelper.h"
#include "../../Scheduling/SlotLegality/SlotClassDefinitions.h"
#include "../../../Architecture/Common/YAKSys_Hybrid_CPU.Processor.h"
#include <string>
#include <vector>
#include <cmath>
#include <stdexcept>
#include <any>
#include <format>
#include <memory>

using namespace YAKSys_Hybrid_CPU::Arch;
using namespace YAKSys_Hybrid_CPU::Core::Memory;
using namespace YAKSys_Hybrid_CPU::Core::Pipeline::Metadata;

namespace YAKSys_Hybrid_CPU::Core
{
	/// <summary>
	/// Architecturally invisible assist carrier for donor-prefetch / LDSA / VDSA.
	/// The assist plane is legality-bound via <see cref="AdmissionMetadata"/>,
	/// placement-bound via <see cref="Placement"/>, non-retiring, and replay-discardable.
	/// Assist does not publish retire-visible architectural state, but its carrier-memory
	/// effects plus replay and telemetry evidence remain intentionally observable.
	/// </summary>
	class AssistMicroOp final : public MicroOp
	{
	private:
		const AssistKind Kind;
		const AssistExecutionMode ExecutionMode;
		const AssistCarrierKind CarrierKind;
		const unsigned long long BaseAddress;
		const unsigned long long PrefetchLength;
		const unsigned char ElementSize;
		const unsigned int ElementCount;
		const int CarrierVirtualThreadId;
		const int DonorVirtualThreadId;
		const int TargetVirtualThreadId;
		const int CarrierCoreId;
		const int TargetCoreId;
		const unsigned short PodId;
		const unsigned long long ReplayEpochId;
		const unsigned long long AssistEpochId;
		const YAKSys_Hybrid_CPU::Core::LocalityHint LocalityHint;
		const AssistDonorSourceDescriptor DonorSource;
		const int MemoryBankId;
		unsigned int ReservedPrefetchLines = 0;

		static constexpr unsigned long long CacheLineSize = 32;

	public:
		CanonicalDecodePublicationMode getCanonicalDecodePublication() const override;

		AssistMicroOp(AssistKind kind, AssistExecutionMode executionMode, AssistCarrierKind carrierKind, unsigned long long baseAddress, unsigned long long prefetchLength, unsigned char elementSize, unsigned int elementCount, AssistOwnerBinding ownerBinding);

		AssistKind getKind() const;

		AssistExecutionMode getExecutionMode() const;

		AssistCarrierKind getCarrierKind() const;

		unsigned long long getBaseAddress() const;

		unsigned long long getPrefetchLength() const;

		unsigned char getElementSize() const;

		unsigned int getElementCount() const;

		int getCarrierVirtualThreadId() const;

		int getDonorVirtualThreadId() const;

		int getTargetVirtualThreadId() const;

		int getCarrierCoreId() const;

		int getTargetCoreId() const;

		unsigned short getPodId() const;

		unsigned long long getReplayEpochId() const;

		unsigned long long getAssistEpochId() const;

		YAKSys_Hybrid_CPU::Core::LocalityHint getLocalityHint() const;

		AssistDonorSourceDescriptor getDonorSource() const;

		int getMemoryBankId() const;

		bool getHasResolvedMemoryBankId() const;

		SlotClass getCarrierSlotClass() const;

		bool getHasExplicitCoreOwnership() const;

		unsigned int getReservedPrefetchLines() const;
	private:
		void setReservedPrefetchLines(unsigned int value);

	public:
		bool isAssist() const override;

		bool isRetireVisible() const override;

		bool isReplayDiscardable() const override;

		bool getSuppressesArchitecturalFaults() const override;

		bool Execute(std::shared_ptr<Processor::CPU_Core> &core) override;

		std::wstring GetDescription() override;

		unsigned int EstimatePrefetchLineDemand(AssistMemoryQuota quota);

		unsigned int ResolvePrefetchLineBudget(AssistMemoryQuota quota);

		void BindReservedPrefetchLines(unsigned int reservedPrefetchLines);

		static bool TryCreateFromSeed(std::shared_ptr<MicroOp> seed, int carrierVirtualThreadId, unsigned long long replayEpochId, unsigned long long assistEpochId, std::shared_ptr<AssistMicroOp> &assistMicroOp);

		static bool TryCreateFromSeed(std::shared_ptr<MicroOp> seed, int carrierVirtualThreadId, int targetVirtualThreadId, unsigned long long replayEpochId, unsigned long long assistEpochId, std::shared_ptr<AssistMicroOp> &assistMicroOp);

		static bool TryCreateInterCoreTransportFromSeed(std::shared_ptr<MicroOp> seed, int donorCoreId, unsigned short donorPodId, unsigned long long donorAssistEpochId, AssistInterCoreTransport &transport);

		static bool TryCreateInterCoreTransportFromSeed(std::shared_ptr<MicroOp> seed, int donorCoreId, unsigned short donorPodId, AssistInterCoreTransport &transport);

		static bool TryCreateFromInterCoreTransport(AssistInterCoreTransport transport, unsigned short podId, int carrierCoreId, int carrierVirtualThreadId, int targetCoreId, int targetVirtualThreadId, int targetOwnerContextId, unsigned long long targetDomainTag, unsigned long long replayEpochId, unsigned long long assistEpochId, std::shared_ptr<AssistMicroOp> &assistMicroOp);

	private:
		static bool IsInterCoreTransportEligible(AssistKind kind, AssistExecutionMode executionMode, AssistCarrierKind carrierKind);

	public:
		static bool IsSupportedInterCoreSeed(std::shared_ptr<MicroOp> seed);

	private:
		static bool TryClassifyInterCoreSeed(std::shared_ptr<MicroOp> seed, AssistKind &kind, AssistExecutionMode &executionMode, AssistCarrierKind &carrierKind, unsigned long long &baseAddress, unsigned long long &prefetchLength, unsigned char &elementSize, unsigned int &elementCount, YAKSys_Hybrid_CPU::Core::LocalityHint &localityHint, bool &isWriteBackVectorSeed, bool &isHotLoadScalarSeed, bool &isHotStoreScalarSeed, bool &isColdStoreScalarSeed, bool &isDefaultStoreScalarSeed);

		static bool TryClassifySeed(std::shared_ptr<MicroOp> seed, bool widenInterCoreScalarLoadAssistToLane6, bool allowInterCoreScalarStoreAssist, bool allowInterCoreVectorWriteBackAssist, AssistKind &kind, AssistExecutionMode &executionMode, AssistCarrierKind &carrierKind, unsigned long long &baseAddress, unsigned long long &prefetchLength, unsigned char &elementSize, unsigned int &elementCount, YAKSys_Hybrid_CPU::Core::LocalityHint &localityHint, bool &isWriteBackVectorSeed, bool &isHotLoadScalarSeed, bool &isHotStoreScalarSeed, bool &isColdStoreScalarSeed, bool &isDefaultStoreScalarSeed);

		static bool IsSupportedAssistSeed(std::shared_ptr<MicroOp> seed, bool allowVectorWriteBackSeed);

		static unsigned int ResolveLane6ScalarElementCount(unsigned long long prefetchLength, unsigned char elementSize);

		static bool TryResolvePrimaryAssistWindow(std::shared_ptr<MicroOp> seed, bool allowInterCoreScalarStoreAssist, bool allowInterCoreVectorWriteBackAssist, unsigned long long &baseAddress, unsigned long long &prefetchLength, bool &isWriteBackVectorSeed);

		static void ValidateSupportedTuple(AssistKind kind, AssistExecutionMode executionMode, AssistCarrierKind carrierKind, AssistOwnerBinding ownerBinding);

		static int NormalizeAssistVirtualThreadId(int virtualThreadId, int ownerThreadId);

		static int ResolveMemoryBankId(unsigned long long address);

		static void ValidateVirtualThreadId(int virtualThreadId, const std::wstring &paramName);

	protected:
		std::shared_ptr<AssistMicroOp> shared_from_this()
		{
			return std::static_pointer_cast<AssistMicroOp>(MicroOp::shared_from_this());
		}
	};
}



