#pragma once

#include "VectorStreamExecutionExtensionDescriptor.h"
#include "VectorStreamState.h"
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{
	class VectorStreamExecutionExtensionDescriptor final : public std::enable_shared_from_this<VectorStreamExecutionExtensionDescriptor>
	{
	private:
		const VectorStreamStateAuthority Authority;
		const bool Enabled;
		const VectorStreamSaveMask AllowedSaveMask;
		const unsigned long long MaxVectorLength;
		const unsigned long long StreamDescriptorTableBase;
		const unsigned long long StreamDescriptorTableLimit;
		const unsigned long long ReplayEpoch;
		const bool AllowsCompatibilityProjection;

	public:
		VectorStreamExecutionExtensionDescriptor();

		VectorStreamExecutionExtensionDescriptor(VectorStreamStateAuthority authority, bool enabled, VectorStreamSaveMask allowedSaveMask, unsigned long long maxVectorLength, unsigned long long streamDescriptorTableBase, unsigned long long streamDescriptorTableLimit, unsigned long long replayEpoch, bool allowsCompatibilityProjection);

		VectorStreamStateAuthority getAuthority() const;

		bool getEnabled() const;

		VectorStreamSaveMask getAllowedSaveMask() const;

		unsigned long long getMaxVectorLength() const;

		unsigned long long getStreamDescriptorTableBase() const;

		unsigned long long getStreamDescriptorTableLimit() const;

		unsigned long long getReplayEpoch() const;

		bool getAllowsCompatibilityProjection() const;

		bool isRuntimeAuthoritative() const;

		bool getHasBoundStreamDescriptorTable() const;

		bool AllowsSaveRestore(VectorStreamSaveMask requestedMask);

		bool AllowsVectorLength(unsigned long long vectorLength);

		std::shared_ptr<VectorStreamExecutionExtensionDescriptor> WithReplayEpoch(unsigned long long replayEpoch);
	};
}
