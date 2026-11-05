#pragma once

#include "../../../Virtualization/Compatibility/FrozenAbi/VmcsFieldAliases/VmcsField.h"
#include "VmReadScalarDeliveryE0FindingV2.h"
#include <memory>
#include <vector>

namespace YAKSys_Hybrid_CPU::Core
{
	class Phase55CompletionReasonQualificationVmReadE0Contract final
	{
	private:
		static const std::vector<unsigned short> &ExactFieldIds();
		static const std::vector<unsigned short> &ExplicitlyDeniedFieldIds();
		static const std::vector<std::shared_ptr<VmReadScalarDeliveryE0FindingV2>> &Findings();

	public:
		static std::vector<unsigned short> getExactFieldIds();
		static std::vector<unsigned short> getExplicitlyDeniedFieldIds();
		static std::vector<std::shared_ptr<VmReadScalarDeliveryE0FindingV2>> getFindings();

		static bool getRuntimeAuthorityGranted();
		static bool getCompletionCreationAuthorized();
		static bool getVmxAuthorityGranted();
		static bool getGuestPhysicalAddressAuthorized();
		static bool getEptViolationQualificationAuthorized();
		static bool getProductionCompositionAuthorized();
	};
}
