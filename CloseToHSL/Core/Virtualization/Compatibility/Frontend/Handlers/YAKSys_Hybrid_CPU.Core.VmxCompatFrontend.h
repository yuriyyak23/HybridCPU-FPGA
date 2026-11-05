#pragma once
#include "VmxCompatFrontend.h"

#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{
	class VmxCompatFrontend final : public std::enable_shared_from_this<VmxCompatFrontend>
	{
	public:
		std::shared_ptr<VmxCompatFrontendAdmissionResult> ValidateAdmission(std::shared_ptr<VmxCompatFrontendAdmissionRequest> request);

		bool CanAdmit(std::shared_ptr<VmxCompatFrontendAdmissionRequest> request);
	};
}
