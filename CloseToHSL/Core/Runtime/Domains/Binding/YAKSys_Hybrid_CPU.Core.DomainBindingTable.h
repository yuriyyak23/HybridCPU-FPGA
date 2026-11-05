#pragma once

#include "DomainBindingTable.h"
#include "DomainBindingTable.h"
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{
	class DomainBindingTable final : public std::enable_shared_from_this<DomainBindingTable>
	{
	public:
		std::shared_ptr<DomainBindingResult> Validate(std::shared_ptr<DomainBindingRequest> request);

		bool CanBind(std::shared_ptr<DomainBindingRequest> request);
	};
}
