#include "Phase47CurrentCompletionVmReadScalarDeliveryE0Contract.h"

namespace YAKSys_Hybrid_CPU::Core
{

	std::vector<unsigned short> Phase47CurrentCompletionVmReadScalarDeliveryE0Contract::getExactFieldIds()
	{
		return ExactFieldIds;
	}

	std::vector<std::shared_ptr<CurrentCompletionProducerE0Entry>> Phase47CurrentCompletionVmReadScalarDeliveryE0Contract::getProducerMatrix()
	{
		return ProducerMatrix;
	}

	std::vector<std::shared_ptr<CurrentCompletionFieldValidityE0Entry>> Phase47CurrentCompletionVmReadScalarDeliveryE0Contract::getFieldValidityMatrix()
	{
		return FieldValidityMatrix;
	}

	std::vector<std::shared_ptr<VmReadScalarDeliveryE0FindingV2>> Phase47CurrentCompletionVmReadScalarDeliveryE0Contract::getFindings()
	{
		return Findings;
	}

	bool Phase47CurrentCompletionVmReadScalarDeliveryE0Contract::getCanonicalCommitPointProven()
	{
		return false;
	}

	bool Phase47CurrentCompletionVmReadScalarDeliveryE0Contract::getAnyProducerEligible()
	{
		return false;
	}

	bool Phase47CurrentCompletionVmReadScalarDeliveryE0Contract::getFieldValidityProven()
	{
		return false;
	}

	bool Phase47CurrentCompletionVmReadScalarDeliveryE0Contract::getDomainCurrentCompletionOwnerCreated()
	{
		return false;
	}

	bool Phase47CurrentCompletionVmReadScalarDeliveryE0Contract::getSpecV2Materialized()
	{
		return false;
	}

	bool Phase47CurrentCompletionVmReadScalarDeliveryE0Contract::getAcceptanceRecordV2Materialized()
	{
		return false;
	}

	bool Phase47CurrentCompletionVmReadScalarDeliveryE0Contract::getRuntimeAuthorityGranted()
	{
		return false;
	}

	bool Phase47CurrentCompletionVmReadScalarDeliveryE0Contract::getSourceValueAvailable()
	{
		return false;
	}

	bool Phase47CurrentCompletionVmReadScalarDeliveryE0Contract::getResultReceiptIssued()
	{
		return false;
	}

	bool Phase47CurrentCompletionVmReadScalarDeliveryE0Contract::getRegisterWritebackAuthorized()
	{
		return false;
	}

	bool Phase47CurrentCompletionVmReadScalarDeliveryE0Contract::getRetireCommitAuthorized()
	{
		return false;
	}

	bool Phase47CurrentCompletionVmReadScalarDeliveryE0Contract::getBackendExecutionAuthorized()
	{
		return false;
	}

	bool Phase47CurrentCompletionVmReadScalarDeliveryE0Contract::getCompletionPublicationAuthorized()
	{
		return false;
	}

	bool Phase47CurrentCompletionVmReadScalarDeliveryE0Contract::getProductionCompositionAuthorized()
	{
		return false;
	}

	std::shared_ptr<CurrentCompletionFieldValidityE0Entry> Phase47CurrentCompletionVmReadScalarDeliveryE0Contract::DeniedField(VmcsField field)
	{
		return std::make_shared<CurrentCompletionFieldValidityE0Entry>(field, false, L"No eligible committed producer and CompletionRecord carries no explicit per-field presence bit; zero fallback is forbidden.");
	}
}

