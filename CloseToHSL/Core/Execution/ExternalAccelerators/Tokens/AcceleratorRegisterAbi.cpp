#include "AcceleratorRegisterAbi.h"
#include "AcceleratorTokenStatusWord.h"
#include "../Capabilities/AcceleratorCapabilityDescriptor.h"

using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Capabilities;

namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Tokens
{

	std::shared_ptr<AcceleratorRegisterAbiResult> AcceleratorRegisterAbi::FromSubmitAdmission(std::shared_ptr<AcceleratorTokenAdmissionResult> admissionResult)
	{
		if (!admissionResult) throw std::invalid_argument("admissionResult");

		if (admissionResult->IsAccepted)
		{
			return AcceleratorRegisterAbiResult::Write(admissionResult->getHandle()->Value, L"ACCEL_SUBMIT rd receives a nonzero opaque token handle after guarded admission.");
		}

		if (admissionResult->RequiresPreciseFault)
		{
			return AcceleratorRegisterAbiResult::PreciseFaultNoWrite(admissionResult->FaultCode, L"ACCEL_SUBMIT precise fault performs no architectural rd write.");
		}

		return AcceleratorRegisterAbiResult::Write(0, L"ACCEL_SUBMIT non-trapping rejection writes zero to rd.", admissionResult->FaultCode);
	}

	std::shared_ptr<AcceleratorRegisterAbiResult> AcceleratorRegisterAbi::FromStatusLookup(std::shared_ptr<AcceleratorTokenLookupResult> lookupResult)
	{
		if (!lookupResult) throw std::invalid_argument("lookupResult");

		if (!lookupResult->IsAllowed)
		{
			return AcceleratorRegisterAbiResult::NoWriteRejected(lookupResult->FaultCode, L"ACCEL token status rd is written only after guarded token lookup succeeds.");
		}

		return AcceleratorRegisterAbiResult::Write(lookupResult->PackedStatusWord, L"ACCEL_STATUS/POLL/WAIT/CANCEL/FENCE rd receives packed status after guarded lookup.");
	}

	std::shared_ptr<AcceleratorRegisterAbiResult> AcceleratorRegisterAbi::FromCapabilityQuery(std::shared_ptr<AcceleratorCapabilityAcceptanceResult> capabilityAcceptance)
	{
		if (!capabilityAcceptance) throw std::invalid_argument("capabilityAcceptance");

		if (!capabilityAcceptance->isAccepted() || capabilityAcceptance->getDescriptor() == nullptr)
		{
			return AcceleratorRegisterAbiResult::NoWriteRejected(AcceleratorTokenFaultCode::CapabilityNotAccepted, L"ACCEL_QUERY_CAPS rd is written only after guard-backed capability acceptance.");
		}

		std::shared_ptr<AcceleratorCapabilityDescriptor> descriptor = capabilityAcceptance->getDescriptor();
		unsigned long long packedSummary = 0;
		packedSummary |= 1ULL;
		packedSummary |= (static_cast<unsigned long long>(std::min(descriptor->getCapabilityVersion(), 0xFFFFu))) << 16;
		packedSummary |= (static_cast<unsigned long long>(std::min(descriptor->getOperations().size(), static_cast<size_t>(0xFF)))) << 32;
		packedSummary |= (static_cast<unsigned long long>(std::min(descriptor->getResourceModel()->getMaxQueueOccupancy(), 0xFFu))) << 40;
		return AcceleratorRegisterAbiResult::Write(packedSummary, L"ACCEL_QUERY_CAPS rd receives a bounded metadata summary after guard-backed capability acceptance.");
	}
}
