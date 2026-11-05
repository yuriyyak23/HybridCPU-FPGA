#include "VmcsCompatMapper.h"

using namespace YAKSys_Hybrid_CPU::Core::Vmcs::V2;

namespace YAKSys_Hybrid_CPU::Core::Vmx::Compatibility
{

// C# TO C++ CONVERTER TASK: Throw expressions are not converted by C# to C++ Converter:
// ORIGINAL LINE: public VmcsCompatMapper(VmcsV2BlockDirectory directory)
	VmcsCompatMapper::VmcsCompatMapper(std::shared_ptr<VmcsV2BlockDirectory> directory) : _directory((directory != nullptr) ? directory : throw std::invalid_argument("directory"))
	{
	}

	bool VmcsCompatMapper::isPrimaryLayoutOwner() const
	{
		return false;
	}

	bool VmcsCompatMapper::TryMap(VmcsField legacyField, std::shared_ptr<VmcsV2FieldDescriptor> &descriptor)
	{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'typeof' operator:
		return _directory->TryGetLegacyAlias(legacyField, descriptor);
	}

	std::shared_ptr<VmcsV2FieldDescriptor> VmcsCompatMapper::MapOrThrow(VmcsField legacyField)
	{
		std::shared_ptr<VmcsV2FieldDescriptor> descriptor;
		if (TryMap(legacyField, descriptor))
		{
			return descriptor;
		}

		throw std::invalid_argument(std::format("VMCS field 0x{:04X} is not a frozen VMX8 compatibility alias.", static_cast<unsigned short>(legacyField)));
	}
}
