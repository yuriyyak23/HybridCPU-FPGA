#pragma once

#include "../../Vmcs/V2/VmcsV2BlockDirectory.h"
#include "../../../Virtualization/Compatibility/FrozenAbi/VmcsFieldAliases/VmcsField.h"
#include <stdexcept>
#include <format>
#include <memory>

using namespace YAKSys_Hybrid_CPU::Core::Vmcs::V2;

namespace YAKSys_Hybrid_CPU::Core::Vmx::Compatibility
{

	class VmcsCompatMapper final : public std::enable_shared_from_this<VmcsCompatMapper>
	{
	private:
		const std::shared_ptr<VmcsV2BlockDirectory> _directory;

	public:
// C# TO C++ CONVERTER TASK: Throw expressions are not converted by C# to C++ Converter:
// ORIGINAL LINE: public VmcsCompatMapper(VmcsV2BlockDirectory directory)
		VmcsCompatMapper(std::shared_ptr<VmcsV2BlockDirectory> directory);

		bool isPrimaryLayoutOwner() const;

		bool TryMap(VmcsField legacyField, std::shared_ptr<VmcsV2FieldDescriptor> &descriptor);

		std::shared_ptr<VmcsV2FieldDescriptor> MapOrThrow(VmcsField legacyField);
	};

}
