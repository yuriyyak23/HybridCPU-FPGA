#pragma once

#include "../../../../../ExternalRuntime/Contracts/ExternalOperationContracts.h"
#include "ExternalAcceleratorBackends.h"
#include "../Queues/AcceleratorCommandQueue.h"
#include "../Memory/AcceleratorMemoryModel.h"
#include "../Tokens/AcceleratorTokenStore.h"
#include "../Tokens/AcceleratorToken.h"
#include <string>
#include <unordered_map>
#include <stdexcept>
#include <optional>
#include <memory>
#include <utility>

// C# TO C++ CONVERTER NOTE: Forward class declarations:
namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Backends { class OperationState; }

using namespace HybridCPU::ExternalRuntime::Contracts;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Auth;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Descriptors;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Memory;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Queues;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Tokens;

namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Backends
{

	/// <summary>
	/// Opt-in L7 adapter backend. It only translates an already validated descriptor
	/// into semantic contract data; the external provider remains the authority for
	/// admission and lifecycle receipts. It does not expose or interpret transport
	/// identity and it never enables direct coherent architectural output.
	/// </summary>
	class ProviderNeutralExternalAcceleratorBackend final : public IExternalAcceleratorBackend, public IExternalAcceleratorBackendPollsExternalOperation, public IExternalAcceleratorPublicationEvidenceProvider
	{
	private:
		class OperationState;
		const std::shared_ptr<IExternalOperationProvider> _provider;
		const std::shared_ptr<IAcceleratorBackendClock> _clock;
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly Dictionary<ulong, OperationState> _operations = new();
		std::unordered_map<unsigned long long, std::shared_ptr<OperationState>> _operations = std::unordered_map<unsigned long long, std::shared_ptr<OperationState>>();

	public:
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER TASK: Throw expressions are not converted by C# to C++ Converter:
// ORIGINAL LINE: public ProviderNeutralExternalAcceleratorBackend(IExternalOperationProvider provider, IAcceleratorBackendClock? clock = null)
		ProviderNeutralExternalAcceleratorBackend(std::shared_ptr<IExternalOperationProvider> provider, std::shared_ptr<IAcceleratorBackendClock> clock = nullptr);

		std::shared_ptr<AcceleratorBackendResult> TrySubmit(std::shared_ptr<AcceleratorQueueAdmissionRequest> request, std::shared_ptr<IAcceleratorCommandQueue> queue, std::optional<std::shared_ptr<AcceleratorGuardEvidence>> &currentGuardEvidence) override;

		std::shared_ptr<AcceleratorBackendResult> Tick(std::shared_ptr<IAcceleratorCommandQueue> queue, std::shared_ptr<IAcceleratorMemoryPortal> memoryPortal, std::shared_ptr<IAcceleratorStagingBuffer> stagingBuffer, std::optional<std::shared_ptr<AcceleratorGuardEvidence>> &currentGuardEvidence) override;

		std::shared_ptr<AcceleratorBackendResult> TryCancel(std::shared_ptr<AcceleratorTokenStore> tokenStore, std::shared_ptr<AcceleratorTokenHandle> handle, std::optional<std::shared_ptr<AcceleratorGuardEvidence>> &currentGuardEvidence) override;

		std::optional<std::shared_ptr<ExternalOperationPublicationEvidence>> GetPublicationEvidence(std::shared_ptr<AcceleratorToken> token) override;

	private:
		static std::shared_ptr<ExternalOperationSemanticRequest> CreateSemanticRequest(std::shared_ptr<AcceleratorCommandDescriptor> descriptor);

		static Guid CorrelationFrom(std::shared_ptr<AcceleratorDescriptorIdentity> identity);

		static bool MatchesSemantic(std::shared_ptr<ExternalOperationRequest> request, std::shared_ptr<ExternalOperationSemanticRequest> semantic);

		static ExternalOperationStage Accept(ExternalOperationStage current, std::shared_ptr<ExternalOperationRequest> request, std::shared_ptr<ExternalOperationProviderPollResult> result);

		static std::shared_ptr<AcceleratorBackendResult> RejectAndFault(std::shared_ptr<AcceleratorToken> token, std::optional<std::shared_ptr<AcceleratorGuardEvidence>> &evidence, unsigned long long tick, const std::wstring &message);

	private:
// C# TO C++ CONVERTER TASK: C++ has no equivalent to C# primary constructors:
// ORIGINAL LINE: private sealed class OperationState(AcceleratorToken token, AcceleratorCommandDescriptor descriptor, ExternalOperationRequest request, ExternalOperationStage stage)
		class OperationState final : public std::enable_shared_from_this<OperationState>
		{
		private:
			const std::shared_ptr<AcceleratorToken> Token;
			const std::shared_ptr<AcceleratorCommandDescriptor> Descriptor;
			const std::shared_ptr<ExternalOperationRequest> Request;
			ExternalOperationStage Stage;
			bool Visible = false;
			std::optional<std::shared_ptr<ExternalOperationCompletionReceipt>> Completion;
			std::optional<std::shared_ptr<ExternalOperationProgressReceipt>> Visibility;

		public:
			OperationState(std::shared_ptr<AcceleratorToken> token,
				std::shared_ptr<AcceleratorCommandDescriptor> descriptor,
				std::shared_ptr<ExternalOperationRequest> request,
				ExternalOperationStage stage);

			std::shared_ptr<AcceleratorToken> getToken() const;
			std::shared_ptr<AcceleratorCommandDescriptor> getDescriptor() const;
			std::shared_ptr<ExternalOperationRequest> getRequest() const;
			ExternalOperationStage getStage() const;
			void setStage(ExternalOperationStage value);
			bool getVisible() const;
			void setVisible(bool value);
			std::optional<std::shared_ptr<ExternalOperationCompletionReceipt>> getCompletion() const;
			void setCompletion(const std::optional<std::shared_ptr<ExternalOperationCompletionReceipt>> &value);
			std::optional<std::shared_ptr<ExternalOperationProgressReceipt>> getVisibility() const;
			void setVisibility(const std::optional<std::shared_ptr<ExternalOperationProgressReceipt>> &value);
		};
	};

}
