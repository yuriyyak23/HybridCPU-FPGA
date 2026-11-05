#pragma once

#include "../Types/MicroOp.h"
#include "../../../Architecture/Common/YAKSys_Hybrid_CPU.Processor.h"
#include "../../../Architecture/Registers/Retire/RetireRecord.h"
#include "../../../Execution/DmaStreamCompute/DmaStreamComputeCapabilityQuery.h"
#include "../../Safety/ResourceBitset.h"
#include <string>
#include <vector>
#include <optional>
#include <format>
#include "tangible_exception_helper.h"
#include <memory>

using namespace YAKSys_Hybrid_CPU::Core::Execution::DmaStreamCompute;
using namespace YAKSys_Hybrid_CPU::Core::Registers::Retire;

namespace YAKSys_Hybrid_CPU::Core
{
	/// <summary>
	/// Lane6 capability query for the Phase 07A DSC_QUERY_CAPS contour.
	/// It captures a bounded runtime-owned capability word at execute and
	/// publishes the scalar result only through retire/writeback.
	/// </summary>
	class DmaStreamComputeQueryCapsMicroOp final : public MicroOp
	{
	private:
		const unsigned short DestinationRegister;
		std::shared_ptr<DmaStreamComputeCapabilityQueryReplayEvidence> LastQueryReplayEvidence;

		static constexpr unsigned int Lane6QueryOwnerPodId = 0;

		std::optional<std::shared_ptr<DmaStreamComputeCapabilityQueryResult>> _lastQueryResult;
		std::optional<std::shared_ptr<DmaStreamComputeOwnerBinding>> _lastOwnerBinding;
		unsigned long long _capturedCapabilityWord = 0;

	public:
		DmaStreamComputeQueryCapsMicroOp(unsigned short destinationRegister);

		unsigned short getDestinationRegister() const;

		std::optional<std::shared_ptr<DmaStreamComputeCapabilityQueryResult>> getLastQueryResult() const;

		std::optional<std::shared_ptr<DmaStreamComputeOwnerBinding>> getLastOwnerBinding() const;

		std::shared_ptr<DmaStreamComputeCapabilityQueryReplayEvidence> getLastQueryReplayEvidence() const;
	private:
		static unsigned short NormalizeDestinationRegister(unsigned short destinationRegister);
		void setLastQueryReplayEvidence(std::shared_ptr<DmaStreamComputeCapabilityQueryReplayEvidence> value);

	public:
		bool getUsedStreamEngineFallback() const;

		bool getUsedDmaControllerFallback() const;

		CanonicalDecodePublicationMode getCanonicalDecodePublication() const override;

		bool Execute(std::shared_ptr<Processor::CPU_Core> &core) override;

		void EmitWriteBackRetireRecords(std::shared_ptr<Processor::CPU_Core> &core, std::span<RetireRecord> retireRecords, int &retireRecordCount) override;

		bool TryGetPrimaryWriteBackResult(unsigned long long &value) override;

		void CapturePrimaryWriteBackResult(unsigned long long value) override;

		void RefreshWriteMetadata() override;

		void RefreshOwnerDependentMetadata();

		std::wstring GetDescription() override;

	private:
		std::shared_ptr<DmaStreamComputeOwnerBinding> BuildOwnerBinding(std::shared_ptr<Processor::CPU_Core> &core, int vtId);

		static ResourceBitset BuildResourceMask(unsigned long long ownerDomainTag);

	protected:
		std::shared_ptr<DmaStreamComputeQueryCapsMicroOp> shared_from_this()
		{
			return std::static_pointer_cast<DmaStreamComputeQueryCapsMicroOp>(std::enable_shared_from_this<MicroOp>::shared_from_this());
		}
	};
}
