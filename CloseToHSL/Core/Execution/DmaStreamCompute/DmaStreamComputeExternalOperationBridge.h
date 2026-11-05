#pragma once

#include "DmaStreamComputeToken.h"
#include "../../../../ExternalRuntime/Contracts/ExternalOperationContracts.h"
#include <string>
#include <unordered_map>
#include <stdexcept>
#include <optional>
#include "tangible_exception_helper.h"
#include <memory>
#include <utility>

using namespace HybridCPU::ExternalRuntime::Contracts;

namespace YAKSys_Hybrid_CPU::Core::Execution::DmaStreamCompute
{
	enum class DmaStreamComputeExternalOperationStatus : unsigned char;
	struct DmaStreamComputeExternalOperationResult;

	/// <summary>
	/// Opt-in provider-neutral bridge for lane-6 DSC. It is deliberately separate
	/// from the L7 accelerator backend and carries no descriptor payload, DMA/IOMMU
	/// translation state, address, or transport identity across the contract boundary.
	/// </summary>
	class DmaStreamComputeExternalOperationBridge final : public std::enable_shared_from_this<DmaStreamComputeExternalOperationBridge>
	{
	private:
		class ExternalOperationState;
		const std::shared_ptr<IExternalOperationProvider> _provider;
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly Dictionary<ulong, ExternalOperationState> _operations = new();
		std::unordered_map<unsigned long long, std::shared_ptr<ExternalOperationState>> _operations = std::unordered_map<unsigned long long, std::shared_ptr<ExternalOperationState>>();

	public:
// C# TO C++ CONVERTER TASK: Throw expressions are not converted by C# to C++ Converter:
// ORIGINAL LINE: public DmaStreamComputeExternalOperationBridge(IExternalOperationProvider provider)
		DmaStreamComputeExternalOperationBridge(std::shared_ptr<IExternalOperationProvider> provider);

		/// <summary>
		/// Performs exact provider admission and one submission for a DSC token. This
		/// does not perform a local DMA read/write or authorize retire publication.
		/// </summary>
		std::shared_ptr<DmaStreamComputeExternalOperationResult> Begin(std::shared_ptr<DmaStreamComputeToken> token);

		/// <summary>
		/// Consumes one provider lifecycle observation. DeviceComplete intentionally
		/// does not make a DSC token commit-pending; only Visible makes the bridge
		/// eligible for a later staged-output/retire integration.
		/// </summary>
		std::shared_ptr<DmaStreamComputeExternalOperationResult> Poll(unsigned long long tokenId);

	private:
		static std::shared_ptr<ExternalOperationSemanticRequest> CreateSemantic(std::shared_ptr<DmaStreamComputeDescriptor> descriptor);

		static Guid CorrelationFrom(unsigned long long descriptorHash, unsigned long long footprintHash);

		static bool Matches(std::shared_ptr<ExternalOperationSemanticRequest> semantic, std::shared_ptr<ExternalOperationRequest> request);

		static ExternalOperationStage Accept(ExternalOperationStage current, std::shared_ptr<ExternalOperationRequest> request, std::shared_ptr<ExternalOperationProviderPollResult> observation);

		static std::shared_ptr<DmaStreamComputeExternalOperationResult> Reject(std::shared_ptr<DmaStreamComputeToken> token, DmaStreamComputeExternalOperationStatus status, const std::wstring &message);

		static std::shared_ptr<DmaStreamComputeExternalOperationResult> Fault(std::shared_ptr<DmaStreamComputeToken> token, DmaStreamComputeExternalOperationStatus status, const std::wstring &message);

		static std::shared_ptr<DmaStreamComputeExternalOperationResult> Result(std::shared_ptr<DmaStreamComputeToken> token, DmaStreamComputeExternalOperationStatus status, std::optional<std::shared_ptr<ExternalOperationRequest>> request, ExternalOperationStage stage, const std::wstring &message);

	private:
// C# TO C++ CONVERTER TASK: C++ has no equivalent to C# primary constructors:
// ORIGINAL LINE: private sealed class ExternalOperationState(DmaStreamComputeToken token, ExternalOperationRequest request, ExternalOperationStage stage)
		class ExternalOperationState final
		{
		private:
			const std::shared_ptr<DmaStreamComputeToken> Token;
			const std::shared_ptr<ExternalOperationRequest> Request;
			ExternalOperationStage Stage;

		public:
			ExternalOperationState(std::shared_ptr<DmaStreamComputeToken> token, std::shared_ptr<ExternalOperationRequest> request, ExternalOperationStage stage);
			std::shared_ptr<DmaStreamComputeToken> getToken() const;
			std::shared_ptr<ExternalOperationRequest> getRequest() const;
			ExternalOperationStage getStage() const;
			void setStage(ExternalOperationStage value);
		};
	};

	enum class DmaStreamComputeExternalOperationStatus : unsigned char
	{
		Rejected = 1,
		Submitted = 2,
		Pending = 3,
		DeviceComplete = 4,
		Visible = 5,
		Stale = 6,
		Faulted = 7
	};

	struct DmaStreamComputeExternalOperationResult final
	{
		const std::shared_ptr<DmaStreamComputeToken> Token;
		const DmaStreamComputeExternalOperationStatus Status;
		const std::optional<std::shared_ptr<ExternalOperationRequest>> Request;
		const ExternalOperationStage Stage;
		const std::wstring Message;

		DmaStreamComputeExternalOperationResult(std::shared_ptr<DmaStreamComputeToken> token,
			DmaStreamComputeExternalOperationStatus status,
			std::optional<std::shared_ptr<ExternalOperationRequest>> request,
			ExternalOperationStage stage, std::wstring message)
			: Token(std::move(token)), Status(status), Request(std::move(request)), Stage(stage), Message(std::move(message)) {}

		const bool MayPublishArchitecturalMemory = false;
		bool operator==(const DmaStreamComputeExternalOperationResult&) const = default;
	};

	/// <summary>Observation only; it never represents DSC retire publication or release.</summary>
// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public sealed record DmaStreamComputeExternalOperationResult(DmaStreamComputeToken Token, DmaStreamComputeExternalOperationStatus Status, System.Nullable<ExternalOperationRequest> Request, ExternalOperationStage Stage, string Message)
//	{
//		public bool MayPublishArchitecturalMemory
//		{
//			get
//			{
//				return false;
//			}
//		}
//	}

}
