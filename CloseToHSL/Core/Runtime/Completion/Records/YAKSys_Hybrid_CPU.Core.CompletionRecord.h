#pragma once

#include "CompletionRecord.h"
#include "../../../Virtualization/Compatibility/FrozenAbi/VmcsFieldAliases/VmExitReason.h"
#include "tangible_exception_helper.h"
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{
	class VmxExitQualification;
    class TrapCompletionPublicationFenceResult;
    class CompletionRecord final : public std::enable_shared_from_this<CompletionRecord>
	{
	private:
		static inline const std::shared_ptr<CompletionRecord> None = std::make_shared<CompletionRecord>();
		const CompletionRecordClass RecordClass;
		const unsigned int ReasonCode;
		const unsigned long long Qualification;
		const unsigned long long FaultAddress;
		const unsigned long long FaultAux;

	public:
		CompletionRecord();

		CompletionRecord(CompletionRecordClass recordClass, unsigned int reasonCode, unsigned long long qualification, unsigned long long faultAddress, unsigned long long faultAux);

		static std::shared_ptr<CompletionRecord> getNone();

		CompletionRecordClass getRecordClass() const;

		unsigned int getReasonCode() const;

		unsigned long long getQualification() const;

		unsigned long long getFaultAddress() const;

		unsigned long long getFaultAux() const;

		bool isEmpty() const;



		bool isCompatibilityProjectionSource() const;

		static std::shared_ptr<CompletionRecord> FromCompatibilityExit(std::shared_ptr<TrapCompletionPublicationFenceResult> publicationFence, VmExitReason reason, std::shared_ptr<VmxExitQualification> qualification, unsigned long long faultAddress = 0, unsigned long long faultAux = 0);

		static bool TryFromCompatibilityExit(std::shared_ptr<TrapCompletionPublicationFenceResult> publicationFence, VmExitReason reason, std::shared_ptr<VmxExitQualification> qualification, std::shared_ptr<CompletionRecord> &record, unsigned long long faultAddress = 0, unsigned long long faultAux = 0);
	};
}
