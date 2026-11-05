#include "Phase43GuestPcSpFlagsVmReadScalarDeliveryE0Contract.h"

namespace YAKSys_Hybrid_CPU::Core
{

	std::vector<unsigned short> Phase43GuestPcSpFlagsVmReadScalarDeliveryE0Contract::getExactFieldIds()
	{
		return ExactFieldIds;
	}

	std::vector<std::shared_ptr<VmReadScalarDeliveryE0FindingV2>> Phase43GuestPcSpFlagsVmReadScalarDeliveryE0Contract::getFindings()
	{
		return Findings;
	}

	bool Phase43GuestPcSpFlagsVmReadScalarDeliveryE0Contract::getRuntimeAuthorityGranted()
	{
		return false;
	}

	bool Phase43GuestPcSpFlagsVmReadScalarDeliveryE0Contract::getSourceValueAvailable()
	{
		return false;
	}

	bool Phase43GuestPcSpFlagsVmReadScalarDeliveryE0Contract::getResultReceiptIssued()
	{
		return false;
	}

	bool Phase43GuestPcSpFlagsVmReadScalarDeliveryE0Contract::getRegisterWritebackAuthorized()
	{
		return false;
	}

	bool Phase43GuestPcSpFlagsVmReadScalarDeliveryE0Contract::getRetireCommitAuthorized()
	{
		return false;
	}

	bool Phase43GuestPcSpFlagsVmReadScalarDeliveryE0Contract::getBackendExecutionAuthorized()
	{
		return false;
	}

	bool Phase43GuestPcSpFlagsVmReadScalarDeliveryE0Contract::getUnderlyingVirtualizationMutationAuthorized()
	{
		return false;
	}

	bool Phase43GuestPcSpFlagsVmReadScalarDeliveryE0Contract::getCompletionPublicationAuthorized()
	{
		return false;
	}

	bool Phase43GuestPcSpFlagsVmReadScalarDeliveryE0Contract::getProductionCompositionAuthorized()
	{
		return false;
	}
}

