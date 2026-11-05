#include "YAKSys_Hybrid_CPU.Core.VectorStreamExecutionExtensionDescriptor.h"

namespace YAKSys_Hybrid_CPU::Core
{

	VectorStreamExecutionExtensionDescriptor::VectorStreamExecutionExtensionDescriptor() : VectorStreamExecutionExtensionDescriptor(VectorStreamStateAuthority::RuntimeOwned, false, VectorStreamSaveMask::None, 0, 0, 0, 0, false)
	{
	}

	VectorStreamExecutionExtensionDescriptor::VectorStreamExecutionExtensionDescriptor(VectorStreamStateAuthority authority, bool enabled, VectorStreamSaveMask allowedSaveMask, unsigned long long maxVectorLength, unsigned long long streamDescriptorTableBase, unsigned long long streamDescriptorTableLimit, unsigned long long replayEpoch, bool allowsCompatibilityProjection)
		: Authority(authority), Enabled(enabled), AllowedSaveMask(allowedSaveMask),
		  MaxVectorLength(maxVectorLength), StreamDescriptorTableBase(streamDescriptorTableBase),
		  StreamDescriptorTableLimit(streamDescriptorTableLimit), ReplayEpoch(replayEpoch),
		  AllowsCompatibilityProjection(allowsCompatibilityProjection)
	{
	}

	VectorStreamStateAuthority VectorStreamExecutionExtensionDescriptor::getAuthority() const
	{
		return Authority;
	}

	bool VectorStreamExecutionExtensionDescriptor::getEnabled() const
	{
		return Enabled;
	}

	VectorStreamSaveMask VectorStreamExecutionExtensionDescriptor::getAllowedSaveMask() const
	{
		return AllowedSaveMask;
	}

	unsigned long long VectorStreamExecutionExtensionDescriptor::getMaxVectorLength() const
	{
		return MaxVectorLength;
	}

	unsigned long long VectorStreamExecutionExtensionDescriptor::getStreamDescriptorTableBase() const
	{
		return StreamDescriptorTableBase;
	}

	unsigned long long VectorStreamExecutionExtensionDescriptor::getStreamDescriptorTableLimit() const
	{
		return StreamDescriptorTableLimit;
	}

	unsigned long long VectorStreamExecutionExtensionDescriptor::getReplayEpoch() const
	{
		return ReplayEpoch;
	}

	bool VectorStreamExecutionExtensionDescriptor::getAllowsCompatibilityProjection() const
	{
		return AllowsCompatibilityProjection;
	}

	bool VectorStreamExecutionExtensionDescriptor::isRuntimeAuthoritative() const
	{
		return getAuthority() == VectorStreamStateAuthority::RuntimeOwned;
	}

	bool VectorStreamExecutionExtensionDescriptor::getHasBoundStreamDescriptorTable() const
	{
		return getStreamDescriptorTableLimit() > getStreamDescriptorTableBase();
	}

	bool VectorStreamExecutionExtensionDescriptor::AllowsSaveRestore(VectorStreamSaveMask requestedMask)
	{
		return isRuntimeAuthoritative() && getEnabled() && requestedMask != VectorStreamSaveMask::None &&
			(static_cast<int>(getAllowedSaveMask()) & static_cast<int>(requestedMask)) == static_cast<int>(requestedMask);
	}

	bool VectorStreamExecutionExtensionDescriptor::AllowsVectorLength(unsigned long long vectorLength)
	{
		return getEnabled() && getMaxVectorLength() != 0 && vectorLength <= getMaxVectorLength();
	}

	std::shared_ptr<VectorStreamExecutionExtensionDescriptor> VectorStreamExecutionExtensionDescriptor::WithReplayEpoch(unsigned long long replayEpoch)
	{
		return std::make_shared<VectorStreamExecutionExtensionDescriptor>(getAuthority(), getEnabled(), getAllowedSaveMask(), getMaxVectorLength(), getStreamDescriptorTableBase(), getStreamDescriptorTableLimit(), replayEpoch, getAllowsCompatibilityProjection());
	}
}
