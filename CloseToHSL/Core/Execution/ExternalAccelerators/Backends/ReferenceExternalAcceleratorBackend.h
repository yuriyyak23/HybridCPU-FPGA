#pragma once

#include "ExternalAcceleratorBackends.h"
#include "../Telemetry/AcceleratorTelemetry.h"
#include "../Queues/AcceleratorCommandQueue.h"
#include "../Memory/AcceleratorMemoryModel.h"
#include "../Tokens/AcceleratorTokenStore.h"
#include "../Tokens/AcceleratorToken.h"
#include <vector>
#include <limits>
#include <optional>
#include <memory>

using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Auth;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Descriptors;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Memory;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Queues;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Telemetry;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Tokens;

namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Backends
{

	/// <summary>
	/// Runtime-owned reference backend for the Phase 08 current L7-SDC contour.
	/// It performs queue admission, guarded source reads, backend-private staging,
	/// and DeviceComplete promotion without granting direct memory publication.
	/// </summary>
	class ReferenceExternalAcceleratorBackend final : public IExternalAcceleratorBackend
	{
	private:
		const std::shared_ptr<IAcceleratorBackendClock> _clock;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private readonly AcceleratorTelemetry? _telemetry;
		const std::shared_ptr<AcceleratorTelemetry> _telemetry;
		const std::shared_ptr<ExternalAcceleratorFeatureSwitch> _featureSwitch;

	public:
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public ReferenceExternalAcceleratorBackend(IAcceleratorBackendClock? clock = null, System.Nullable<AcceleratorTelemetry> telemetry = null, System.Nullable<ExternalAcceleratorFeatureSwitch> featureSwitch = null)
		ReferenceExternalAcceleratorBackend(std::shared_ptr<IAcceleratorBackendClock> clock = nullptr, std::shared_ptr<AcceleratorTelemetry> telemetry = nullptr, std::optional<std::shared_ptr<ExternalAcceleratorFeatureSwitch>> featureSwitch = std::nullopt);

		std::shared_ptr<AcceleratorBackendResult> TrySubmit(std::shared_ptr<AcceleratorQueueAdmissionRequest> request, std::shared_ptr<IAcceleratorCommandQueue> queue, std::optional<std::shared_ptr<AcceleratorGuardEvidence>> &currentGuardEvidence) override;

		std::shared_ptr<AcceleratorBackendResult> Tick(std::shared_ptr<IAcceleratorCommandQueue> queue, std::shared_ptr<IAcceleratorMemoryPortal> memoryPortal, std::shared_ptr<IAcceleratorStagingBuffer> stagingBuffer, std::optional<std::shared_ptr<AcceleratorGuardEvidence>> &currentGuardEvidence) override;

		std::shared_ptr<AcceleratorBackendResult> TryCancel(std::shared_ptr<AcceleratorTokenStore> tokenStore, std::shared_ptr<AcceleratorTokenHandle> handle, std::optional<std::shared_ptr<AcceleratorGuardEvidence>> &currentGuardEvidence) override;

	private:
		std::shared_ptr<AcceleratorBackendResult> RecordResult(std::shared_ptr<AcceleratorBackendResult> result);

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private static byte[] BuildDeterministicStagedBytes(AcceleratorToken token, IReadOnlyList<AcceleratorMemoryRead> reads, int length)
		static std::vector<unsigned char> BuildDeterministicStagedBytes(std::shared_ptr<AcceleratorToken> token, const std::vector<std::shared_ptr<AcceleratorMemoryRead>> &reads, int length);
	};

}

