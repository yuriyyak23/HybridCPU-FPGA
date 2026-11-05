#include "Phase45MemoryOwnedVmReadScalarDeliveryE0Contract.h"

namespace YAKSys_Hybrid_CPU::Core
{

	std::vector<unsigned short> Phase45MemoryOwnedVmReadScalarDeliveryE0Contract::getExactFieldIds()
	{
		return ExactFieldIds;
	}

	std::vector<std::shared_ptr<VmReadScalarDeliveryE0FindingV2>> Phase45MemoryOwnedVmReadScalarDeliveryE0Contract::getFindings()
	{
		return Findings;
	}

	bool Phase45MemoryOwnedVmReadScalarDeliveryE0Contract::getRuntimeAuthorityGranted()
	{
		return false;
	}

	bool Phase45MemoryOwnedVmReadScalarDeliveryE0Contract::getSourceValueAvailable()
	{
		return false;
	}

	bool Phase45MemoryOwnedVmReadScalarDeliveryE0Contract::getResultReceiptIssued()
	{
		return false;
	}

	bool Phase45MemoryOwnedVmReadScalarDeliveryE0Contract::getRegisterWritebackAuthorized()
	{
		return false;
	}

	bool Phase45MemoryOwnedVmReadScalarDeliveryE0Contract::getRetireCommitAuthorized()
	{
		return false;
	}

	bool Phase45MemoryOwnedVmReadScalarDeliveryE0Contract::getBackendExecutionAuthorized()
	{
		return false;
	}

	bool Phase45MemoryOwnedVmReadScalarDeliveryE0Contract::getCompletionPublicationAuthorized()
	{
		return false;
	}

	bool Phase45MemoryOwnedVmReadScalarDeliveryE0Contract::getProductionCompositionAuthorized()
	{
		return false;
	}
}
