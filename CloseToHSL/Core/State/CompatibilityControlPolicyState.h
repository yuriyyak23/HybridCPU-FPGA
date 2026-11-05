#pragma once

#include "../Runtime/Capabilities/CompatibilityControls/CompatibilityControlPolicyOwner.h"
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{

	class CompatibilityControlPolicyState final : public std::enable_shared_from_this<CompatibilityControlPolicyState>
	{
	public:
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal CompatibilityControlPolicyOwner? Owner;
		std::shared_ptr<CompatibilityControlPolicyOwner> Owner;
		std::shared_ptr<NeutralInterruptRoutingPolicyConsumer> InterruptRoutingConsumer = std::make_shared<NeutralInterruptRoutingPolicyConsumer>();
	};

}
