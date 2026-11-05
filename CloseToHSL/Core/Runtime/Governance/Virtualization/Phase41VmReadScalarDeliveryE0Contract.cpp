#include "Phase41VmReadScalarDeliveryE0Contract.h"

namespace YAKSys_Hybrid_CPU::Core
{

	std::vector<unsigned short> Phase41VmReadScalarDeliveryE0Contract::getExactFieldIds()
	{
		return ExactFieldIds;
	}

	std::vector<std::shared_ptr<VmReadScalarDeliveryE0FindingV2>> Phase41VmReadScalarDeliveryE0Contract::getFindings()
	{
		return Findings;
	}

	bool Phase41VmReadScalarDeliveryE0Contract::getRuntimeAuthorityGranted()
	{
		return false;
	}

	bool Phase41VmReadScalarDeliveryE0Contract::getSourceValueAvailable()
	{
		return false;
	}

	bool Phase41VmReadScalarDeliveryE0Contract::getResultReceiptIssued()
	{
		return false;
	}

	bool Phase41VmReadScalarDeliveryE0Contract::getRegisterWritebackAuthorized()
	{
		return false;
	}

	bool Phase41VmReadScalarDeliveryE0Contract::getRetireCommitAuthorized()
	{
		return false;
	}

	bool Phase41VmReadScalarDeliveryE0Contract::getBackendExecutionAuthorized()
	{
		return false;
	}

	bool Phase41VmReadScalarDeliveryE0Contract::getUnderlyingVirtualizationMutationAuthorized()
	{
		return false;
	}

	bool Phase41VmReadScalarDeliveryE0Contract::getCompletionPublicationAuthorized()
	{
		return false;
	}
}
