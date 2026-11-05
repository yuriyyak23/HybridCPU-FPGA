#pragma once

#include <string>
#include <utility>

namespace YAKSys_Hybrid_CPU::Core
{
	struct VmReadScalarDeliveryE0FindingV2 final
	{
		unsigned char Number;
		std::wstring Name;
		std::wstring ExactContract;

		VmReadScalarDeliveryE0FindingV2(unsigned char number, std::wstring name, std::wstring exactContract)
			: Number(number), Name(std::move(name)), ExactContract(std::move(exactContract))
		{
		}
	};
}
