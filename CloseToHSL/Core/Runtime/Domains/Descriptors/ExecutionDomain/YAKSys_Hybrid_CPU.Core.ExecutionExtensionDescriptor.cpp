#include "YAKSys_Hybrid_CPU.Core.ExecutionExtensionDescriptor.h"

namespace YAKSys_Hybrid_CPU::Core
{

	ExecutionExtensionDescriptor::ExecutionExtensionDescriptor() : ExecutionExtensionDescriptor(ExecutionExtensionAuthority::RuntimeOwned, nullptr, 0, false)
	{
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public ExecutionExtensionDescriptor(ExecutionExtensionAuthority authority, VectorStreamExecutionExtensionDescriptor? vectorStream, ulong extensionEpoch, bool compatibilityProjectionEnabled)
	ExecutionExtensionDescriptor::ExecutionExtensionDescriptor(ExecutionExtensionAuthority authority, std::shared_ptr<VectorStreamExecutionExtensionDescriptor> vectorStream, unsigned long long extensionEpoch, bool compatibilityProjectionEnabled)
		: Authority(authority),
		  VectorStream(vectorStream),
		  ExtensionEpoch(extensionEpoch),
		  CompatibilityProjectionEnabled(compatibilityProjectionEnabled)
	{
	}

	ExecutionExtensionAuthority ExecutionExtensionDescriptor::getAuthority() const
	{
		return Authority;
	}

	std::shared_ptr<VectorStreamExecutionExtensionDescriptor> ExecutionExtensionDescriptor::getVectorStream() const
	{
		return VectorStream;
	}

	unsigned long long ExecutionExtensionDescriptor::getExtensionEpoch() const
	{
		return ExtensionEpoch;
	}

	bool ExecutionExtensionDescriptor::getCompatibilityProjectionEnabled() const
	{
		return CompatibilityProjectionEnabled;
	}

	bool ExecutionExtensionDescriptor::isRuntimeAuthoritative() const
	{
		return getAuthority() == ExecutionExtensionAuthority::RuntimeOwned;
	}

	bool ExecutionExtensionDescriptor::getHasVectorStream() const
	{
		return getVectorStream() != nullptr;
	}

	bool ExecutionExtensionDescriptor::getAllowsVectorStreamProjection() const
	{
		const auto vectorStream = getVectorStream();
		return isRuntimeAuthoritative() &&
			getCompatibilityProjectionEnabled() &&
			vectorStream != nullptr &&
			vectorStream->getAllowsCompatibilityProjection();
	}

	bool ExecutionExtensionDescriptor::CanSaveRestoreVectorStream(VectorStreamSaveMask mask)
	{
		return isRuntimeAuthoritative() && getVectorStream() != nullptr && getVectorStream()->AllowsSaveRestore(mask);
	}

	std::shared_ptr<ExecutionExtensionDescriptor> ExecutionExtensionDescriptor::WithVectorStream(std::shared_ptr<VectorStreamExecutionExtensionDescriptor> vectorStream)
	{
		return std::make_shared<ExecutionExtensionDescriptor>(
			getAuthority(), vectorStream, getExtensionEpoch(), getCompatibilityProjectionEnabled());
	}
}
