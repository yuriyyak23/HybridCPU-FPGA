#include "YAKSys_Hybrid_CPU.Core.CompletionRecord.h"
#include "TrapCompletionPublicationFence.h"
#include "../../../Virtualization/Compatibility/FrozenAbi/VmcsFieldAliases/VmxExitQualification.h"

namespace YAKSys_Hybrid_CPU::Core
{

	CompletionRecord::CompletionRecord() : CompletionRecord(CompletionRecordClass::None, 0, 0, 0, 0)
	{
	}

	CompletionRecord::CompletionRecord(CompletionRecordClass recordClass, unsigned int reasonCode, unsigned long long qualification, unsigned long long faultAddress, unsigned long long faultAux) : RecordClass(recordClass), ReasonCode(reasonCode), Qualification(qualification), FaultAddress(faultAddress), FaultAux(faultAux) {}

	std::shared_ptr<CompletionRecord> CompletionRecord::getNone()
	{
		return None;
	}

	CompletionRecordClass CompletionRecord::getRecordClass() const
	{
		return RecordClass;
	}

	unsigned int CompletionRecord::getReasonCode() const
	{
		return ReasonCode;
	}

	unsigned long long CompletionRecord::getQualification() const
	{
		return Qualification;
	}

	unsigned long long CompletionRecord::getFaultAddress() const
	{
		return FaultAddress;
	}

	unsigned long long CompletionRecord::getFaultAux() const
	{
		return FaultAux;
	}

	bool CompletionRecord::isEmpty() const
	{
		return getRecordClass() == CompletionRecordClass::None && getReasonCode() == 0 && getQualification() == 0 && getFaultAddress() == 0 && getFaultAux() == 0;
	}

	bool CompletionRecord::isCompatibilityProjectionSource() const
	{
		return getRecordClass() == CompletionRecordClass::CompatibilityExit;
	}

	std::shared_ptr<CompletionRecord> CompletionRecord::FromCompatibilityExit(std::shared_ptr<TrapCompletionPublicationFenceResult> publicationFence, VmExitReason reason, std::shared_ptr<VmxExitQualification> qualification, unsigned long long faultAddress, unsigned long long faultAux)
	{
		std::shared_ptr<CompletionRecord> record;
		if (!TryFromCompatibilityExit(publicationFence, reason, qualification, record, faultAddress, faultAux))
		{
			throw InvalidOperationException(L"Compatibility exit completion publication was denied by the neutral trap publication fence.");
		}

		return record;
	}

	bool CompletionRecord::TryFromCompatibilityExit(std::shared_ptr<TrapCompletionPublicationFenceResult> publicationFence, VmExitReason reason, std::shared_ptr<VmxExitQualification> qualification, std::shared_ptr<CompletionRecord> &record, unsigned long long faultAddress, unsigned long long faultAux)
	{
		if (!publicationFence->CompletionPublicationAllowed)
		{
			record = getNone();
			return false;
		}

		record = std::make_shared<CompletionRecord>(CompletionRecordClass::CompatibilityExit, static_cast<unsigned int>(reason), qualification->Encode(), faultAddress, faultAux);
		return true;
	}
}
