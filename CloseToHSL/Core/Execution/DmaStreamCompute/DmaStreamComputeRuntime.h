#pragma once

#include "DmaStreamComputeTelemetry.h"
#include "DmaStreamAcceleratorBackend.h"
#include "DmaStreamComputeTokenStore.h"
#include "DmaStreamComputeReplayEvidence.h"
namespace YAKSys_Hybrid_CPU::Core { class DmaStreamComputeMicroOp; }
#include "DmaStreamComputeToken.h"
#include "DmaStreamComputeDescriptor.h"
#include <vector>
#include <limits>
#include <optional>
#include <format>
#include "tangible_exception_helper.h"
#include <memory>
#include <span>

namespace YAKSys_Hybrid_CPU::Core::Execution::DmaStreamCompute
{
	struct DmaStreamComputeExecutionResult
	{
		std::shared_ptr<DmaStreamComputeToken> Token;
		std::shared_ptr<DmaStreamComputeCommitResult> Completion;
		std::shared_ptr<DmaStreamComputeBackendTelemetry> Telemetry;
		std::optional<std::shared_ptr<DmaStreamComputeIssueAdmissionResult>> IssueAdmission;
		DmaStreamComputeTokenHandle TokenHandle{};
		bool IsStoreTracked=false;
		bool IsCommitPending=false;
		bool RequiresRetireExceptionPublication=false;
	};

	class DmaStreamComputeRuntime final
	{
		// Explicit helper surface retained for direct runtime tests. Production
		// MicroOp execution enters through ExecuteMaterializedMicroOpToCommitPending
		// so issue/admission owns token allocation.
	public:
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public static DmaStreamComputeExecutionResult ExecuteToCommitPending(DmaStreamComputeDescriptor descriptor, ulong tokenId = 0, DmaStreamComputeTelemetryCounters? telemetry = null)
		static std::shared_ptr<DmaStreamComputeExecutionResult> ExecuteToCommitPending(std::shared_ptr<DmaStreamComputeDescriptor> descriptor, unsigned long long tokenId = 0, std::shared_ptr<DmaStreamComputeTelemetryCounters> telemetry = nullptr);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public static DmaStreamComputeExecutionResult ExecuteToCommitPending(DmaStreamComputeDescriptor descriptor, DmaStreamAcceleratorBackend backend, ulong tokenId = 0, DmaStreamComputeTelemetryCounters? telemetry = null)
		static std::shared_ptr<DmaStreamComputeExecutionResult> ExecuteToCommitPending(std::shared_ptr<DmaStreamComputeDescriptor> descriptor, std::shared_ptr<DmaStreamAcceleratorBackend> backend, unsigned long long tokenId = 0, std::shared_ptr<DmaStreamComputeTelemetryCounters> telemetry = nullptr);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public static DmaStreamComputeExecutionResult ExecuteMaterializedMicroOpToCommitPending(DmaStreamComputeMicroOp microOp, DmaStreamComputeTokenStore tokenStore, DmaStreamComputeIssueAdmissionMetadata issueMetadata, DmaStreamAcceleratorBackend? backend = null, System.Nullable<DmaStreamComputeTelemetryCounters> telemetry = null)
		static std::shared_ptr<DmaStreamComputeExecutionResult> ExecuteMaterializedMicroOpToCommitPending(std::shared_ptr<DmaStreamComputeMicroOp> microOp, std::shared_ptr<DmaStreamComputeTokenStore> tokenStore, std::shared_ptr<DmaStreamComputeIssueAdmissionMetadata> issueMetadata, std::shared_ptr<DmaStreamAcceleratorBackend> backend = nullptr, std::shared_ptr<DmaStreamComputeTelemetryCounters> telemetry = nullptr);

	private:
		static std::shared_ptr<DmaStreamComputeCommitResult> ExecuteTokenToCommitPending(std::shared_ptr<DmaStreamComputeDescriptor> descriptor, std::shared_ptr<DmaStreamComputeToken> token, std::shared_ptr<DmaStreamAcceleratorBackend> backend);

		static std::shared_ptr<DmaStreamComputeExecutionResult> CreateResult(std::shared_ptr<DmaStreamComputeToken> token, std::shared_ptr<DmaStreamComputeCommitResult> completion, std::shared_ptr<DmaStreamAcceleratorBackend> backend, const std::optional<std::shared_ptr<DmaStreamComputeIssueAdmissionResult>>& issueAdmission = {});

		static bool TryValidateRuntimeDescriptor(std::shared_ptr<DmaStreamComputeDescriptor> descriptor, std::shared_ptr<DmaStreamComputeToken> token, std::shared_ptr<DmaStreamComputeCommitResult> &fault);

		static bool TryReadOperands(std::shared_ptr<DmaStreamComputeDescriptor> descriptor, std::shared_ptr<DmaStreamComputeToken> token, std::shared_ptr<DmaStreamAcceleratorBackend> backend, std::vector<std::vector<unsigned char>> &operands, std::shared_ptr<DmaStreamComputeCommitResult> &fault);

		static bool TryCompute(std::shared_ptr<DmaStreamComputeDescriptor> descriptor, std::shared_ptr<DmaStreamComputeToken> token, std::vector<std::vector<unsigned char>> &operands, std::shared_ptr<DmaStreamAcceleratorBackend> backend, std::vector<unsigned char> &output, std::shared_ptr<DmaStreamComputeCommitResult> &fault);

		static bool TryComputeCopy(std::shared_ptr<DmaStreamComputeDescriptor> descriptor, std::shared_ptr<DmaStreamComputeToken> token, std::vector<std::vector<unsigned char>> &operands, unsigned long long totalWriteBytes, std::shared_ptr<DmaStreamAcceleratorBackend> backend, std::vector<unsigned char> &output, std::shared_ptr<DmaStreamComputeCommitResult> &fault);

		static bool TryComputeBinary(std::shared_ptr<DmaStreamComputeDescriptor> descriptor, std::shared_ptr<DmaStreamComputeToken> token, std::vector<std::vector<unsigned char>> &operands, unsigned long long totalWriteBytes, std::shared_ptr<DmaStreamAcceleratorBackend> backend, std::vector<unsigned char> &output, std::shared_ptr<DmaStreamComputeCommitResult> &fault);

		static bool TryComputeFma(std::shared_ptr<DmaStreamComputeDescriptor> descriptor, std::shared_ptr<DmaStreamComputeToken> token, std::vector<std::vector<unsigned char>> &operands, unsigned long long totalWriteBytes, std::shared_ptr<DmaStreamAcceleratorBackend> backend, std::vector<unsigned char> &output, std::shared_ptr<DmaStreamComputeCommitResult> &fault);

		static bool TryComputeReduce(std::shared_ptr<DmaStreamComputeDescriptor> descriptor, std::shared_ptr<DmaStreamComputeToken> token, std::vector<std::vector<unsigned char>> &operands, unsigned long long totalWriteBytes, std::shared_ptr<DmaStreamAcceleratorBackend> backend, std::vector<unsigned char> &output, std::shared_ptr<DmaStreamComputeCommitResult> &fault);

		static bool ValidateOperandByteLengths(std::shared_ptr<DmaStreamComputeDescriptor> descriptor, std::shared_ptr<DmaStreamComputeToken> token, std::vector<std::vector<unsigned char>> &operands, unsigned long long totalWriteBytes, std::shared_ptr<DmaStreamComputeCommitResult> &fault);

		static std::shared_ptr<DmaStreamComputeCommitResult> StageOutput(std::shared_ptr<DmaStreamComputeDescriptor> descriptor, std::shared_ptr<DmaStreamComputeToken> token, std::shared_ptr<DmaStreamAcceleratorBackend> backend, std::vector<unsigned char> &output);

		static bool TryGetTotalWriteBytes(std::shared_ptr<DmaStreamComputeDescriptor> descriptor, unsigned long long &totalWriteBytes);

		static int GetExpectedOperandCount(DmaStreamComputeOperationKind operation);

		static bool IsSupportedRuntimeShape(DmaStreamComputeOperationKind operation, DmaStreamComputeShapeKind shape);

		static int ResolveElementSize(DmaStreamComputeElementType elementType);

		static bool IsFloatingElement(DmaStreamComputeElementType elementType);

		static unsigned long long LoadUnsigned(std::span<const unsigned char> buffer, int offset, DmaStreamComputeElementType elementType);

		static void StoreUnsigned(std::span<unsigned char> buffer, int offset, DmaStreamComputeElementType elementType, unsigned long long value);

		static double LoadFloat(std::span<const unsigned char> buffer, int offset, DmaStreamComputeElementType elementType);

		static void StoreFloat(std::span<unsigned char> buffer, int offset, DmaStreamComputeElementType elementType, double value);
	};
}


