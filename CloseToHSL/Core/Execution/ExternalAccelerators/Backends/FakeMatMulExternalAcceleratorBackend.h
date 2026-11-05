#pragma once

#include "ExternalAcceleratorBackends.h"
#include "../Descriptors/MatMulDescriptor.h"
#include "../Telemetry/AcceleratorTelemetry.h"
#include "../Queues/AcceleratorCommandQueue.h"
#include "../Memory/AcceleratorMemoryModel.h"
#include "../Tokens/AcceleratorTokenStore.h"
#include "../Tokens/AcceleratorToken.h"
#include "../Tokens/AcceleratorTokenStatusWord.h"
#include <string>
#include <unordered_map>
#include <vector>
#include <limits>
#include <stdexcept>
#include <optional>
#include <memory>
#include <span>

using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Capabilities;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Auth;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Descriptors;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Memory;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Queues;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Telemetry;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Tokens;

namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Backends
{

	class FakeMatMulExternalAcceleratorBackend final : public IExternalAcceleratorBackend
	{
	private:
		const std::shared_ptr<IAcceleratorBackendClock> _clock;
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly Dictionary<ulong, MatMulDescriptor> _descriptorsByIdentity = new();
		std::unordered_map<unsigned long long, std::shared_ptr<MatMulDescriptor>> _descriptorsByIdentity = std::unordered_map<unsigned long long, std::shared_ptr<MatMulDescriptor>>();
		const std::shared_ptr<MatMulDescriptorValidator> _validator = std::make_shared<MatMulDescriptorValidator>();
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private readonly AcceleratorTelemetry? _telemetry;
		const std::shared_ptr<AcceleratorTelemetry> _telemetry;
		const std::shared_ptr<ExternalAcceleratorFeatureSwitch> _featureSwitch;

	public:
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public FakeMatMulExternalAcceleratorBackend(IAcceleratorBackendClock? clock = null, System.Nullable<AcceleratorTelemetry> telemetry = null, System.Nullable<ExternalAcceleratorFeatureSwitch> featureSwitch = null)
		FakeMatMulExternalAcceleratorBackend(std::shared_ptr<IAcceleratorBackendClock> clock = nullptr, std::shared_ptr<AcceleratorTelemetry> telemetry = nullptr, std::optional<std::shared_ptr<ExternalAcceleratorFeatureSwitch>> featureSwitch = std::nullopt);

		bool isTestOnly() const;

		void RegisterDescriptor(std::shared_ptr<AcceleratorCommandDescriptor> commandDescriptor, std::shared_ptr<MatMulDescriptor> matMulDescriptor);

		std::shared_ptr<AcceleratorBackendResult> TrySubmit(std::shared_ptr<AcceleratorQueueAdmissionRequest> request, std::shared_ptr<IAcceleratorCommandQueue> queue, std::optional<std::shared_ptr<AcceleratorGuardEvidence>> &currentGuardEvidence) override;

		std::shared_ptr<AcceleratorBackendResult> Tick(std::shared_ptr<IAcceleratorCommandQueue> queue, std::shared_ptr<IAcceleratorMemoryPortal> memoryPortal, std::shared_ptr<IAcceleratorStagingBuffer> stagingBuffer, std::optional<std::shared_ptr<AcceleratorGuardEvidence>> &currentGuardEvidence) override;

		std::shared_ptr<AcceleratorBackendResult> TryCancel(std::shared_ptr<AcceleratorTokenStore> tokenStore, std::shared_ptr<AcceleratorTokenHandle> handle, std::optional<std::shared_ptr<AcceleratorGuardEvidence>> &currentGuardEvidence) override;

	private:
		std::shared_ptr<AcceleratorBackendResult> RecordResult(std::shared_ptr<AcceleratorBackendResult> result);

	public:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: public static bool TryBuildStagedMatMulResultForTest(MatMulDescriptor descriptor, MatMulFootprint footprint, IReadOnlyList<AcceleratorMemoryRead> reads, out byte[] stagedBytes, out string message)
		static bool TryBuildStagedMatMulResultForTest(std::shared_ptr<MatMulDescriptor> descriptor, std::shared_ptr<MatMulFootprint> footprint, const std::vector<std::shared_ptr<AcceleratorMemoryRead>> &reads, std::vector<unsigned char> &stagedBytes, std::wstring &message);

	private:
		static std::shared_ptr<AcceleratorBackendResult> FaultToken(std::shared_ptr<AcceleratorToken> token, AcceleratorTokenFaultCode faultCode, const std::wstring &message, std::shared_ptr<AcceleratorGuardEvidence> currentGuardEvidence, unsigned long long tick);

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private static bool TryFindRead(IReadOnlyList<AcceleratorMemoryRead> reads, AcceleratorMemoryRange range, out ReadOnlyMemory<byte> data)
		static bool TryFindRead(const std::vector<std::shared_ptr<AcceleratorMemoryRead>> &reads, std::shared_ptr<AcceleratorMemoryRange> range, std::span<const unsigned char> &data);

		static void ComputeFloat32(std::shared_ptr<MatMulDescriptor> descriptor, std::span<const unsigned char> a, std::span<const unsigned char> b, std::span<unsigned char> c);

		static void ComputeFloat64(std::shared_ptr<MatMulDescriptor> descriptor, std::span<const unsigned char> a, std::span<const unsigned char> b, std::span<unsigned char> c);

		static void ComputeInt32(std::shared_ptr<MatMulDescriptor> descriptor, std::span<const unsigned char> a, std::span<const unsigned char> b, std::span<unsigned char> c);

		static float ReadSingle(std::span<const unsigned char> data, unsigned long long byteOffset);

		static double ReadDouble(std::span<const unsigned char> data, unsigned long long byteOffset);

		static int ReadInt32(std::span<const unsigned char> data, unsigned long long byteOffset);
	};

}

