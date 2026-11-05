#pragma once

#include "ExecutionExtensionDescriptor.h"
#include "../../../Lanes/VectorStream/State/YAKSys_Hybrid_CPU.Core.VectorStreamExecutionExtensionDescriptor.h"
#include "../../../Lanes/VectorStream/State/VectorStreamState.h"
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{
	class ExecutionExtensionDescriptor final : public std::enable_shared_from_this<ExecutionExtensionDescriptor>
	{
	private:
		const ExecutionExtensionAuthority Authority;
		const std::shared_ptr<VectorStreamExecutionExtensionDescriptor> VectorStream;
		const unsigned long long ExtensionEpoch;
		const bool CompatibilityProjectionEnabled;

	public:
		ExecutionExtensionDescriptor();

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public ExecutionExtensionDescriptor(ExecutionExtensionAuthority authority, VectorStreamExecutionExtensionDescriptor? vectorStream, ulong extensionEpoch, bool compatibilityProjectionEnabled)
		ExecutionExtensionDescriptor(ExecutionExtensionAuthority authority, std::shared_ptr<VectorStreamExecutionExtensionDescriptor> vectorStream, unsigned long long extensionEpoch, bool compatibilityProjectionEnabled);

		ExecutionExtensionAuthority getAuthority() const;

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public VectorStreamExecutionExtensionDescriptor? VectorStream {get;}
		std::shared_ptr<VectorStreamExecutionExtensionDescriptor> getVectorStream() const;

		unsigned long long getExtensionEpoch() const;

		bool getCompatibilityProjectionEnabled() const;

		bool isRuntimeAuthoritative() const;

		bool getHasVectorStream() const;

		bool getAllowsVectorStreamProjection() const;

		bool CanSaveRestoreVectorStream(VectorStreamSaveMask mask);

		std::shared_ptr<ExecutionExtensionDescriptor> WithVectorStream(std::shared_ptr<VectorStreamExecutionExtensionDescriptor> vectorStream);
	};
}
