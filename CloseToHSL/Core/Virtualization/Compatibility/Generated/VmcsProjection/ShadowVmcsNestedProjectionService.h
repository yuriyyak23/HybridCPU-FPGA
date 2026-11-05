#pragma once

#include "../../../../Runtime/Nested/Projection/INestedProjectionService.h"
#include "../../../../System/Vmcs/V2/VmcsV2Descriptor.h"
#include "../../../../Runtime/Nested/Descriptors/YAKSys_Hybrid_CPU.Core.NestedDomainDescriptor.h"
#include <string>
#include <memory>

using namespace YAKSys_Hybrid_CPU::Core::Nested;
using namespace YAKSys_Hybrid_CPU::Core::Vmcs::V2;

namespace YAKSys_Hybrid_CPU::Core
{

	class ShadowVmcsNestedProjectionService final : public INestedProjectionService
	{
	public:
		static inline const std::wstring CompatibilityBridgePath = L"Core/VMX/Compatibility/Generated/VmcsProjection/ShadowVmcsNestedProjectionService.cs";

		static bool isRetirementFenced();

		ShadowVmcsNestedProjectionService(std::shared_ptr<VmcsV2Descriptor> descriptor);

		bool TryEnable(std::shared_ptr<NestedDomainDescriptor> domain, std::shared_ptr<NestedEnablementRequest> request, std::shared_ptr<NestedValidationResult> &validation) override;

		void Disable(std::shared_ptr<NestedDomainDescriptor> domain) override;
	};

}
