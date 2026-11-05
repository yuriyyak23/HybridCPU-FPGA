#include "DmaStreamComputeRuntime.h"
#include "../../Pipeline/MicroOps/Lane6DmaStream/DmaStreamComputeMicroOp.h"
#include "../../Architecture/Common/YAKSys_Hybrid_CPU.Processor.h"
#include <bit>
#include <cstdint>

namespace YAKSys_Hybrid_CPU::Core::Execution::DmaStreamCompute
{
	namespace
	{
		unsigned long long ReadLittleEndian(std::span<const unsigned char> bytes)
		{
			unsigned long long value = 0;
			for (size_t i = 0; i < bytes.size(); ++i)
				value |= static_cast<unsigned long long>(bytes[i]) << (i * 8);
			return value;
		}

		void WriteLittleEndian(std::span<unsigned char> bytes, unsigned long long value)
		{
			for (size_t i = 0; i < bytes.size(); ++i)
				bytes[i] = static_cast<unsigned char>((value >> (i * 8)) & 0xffu);
		}
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public static DmaStreamComputeExecutionResult ExecuteToCommitPending(DmaStreamComputeDescriptor descriptor, ulong tokenId = 0, DmaStreamComputeTelemetryCounters? telemetry = null)
	std::shared_ptr<DmaStreamComputeExecutionResult> DmaStreamComputeRuntime::ExecuteToCommitPending(std::shared_ptr<DmaStreamComputeDescriptor> descriptor, unsigned long long tokenId, std::shared_ptr<DmaStreamComputeTelemetryCounters> telemetry)
	{
		return ExecuteToCommitPending(descriptor, std::make_shared<DmaStreamAcceleratorBackend>(Processor::MainMemory, telemetry), tokenId, telemetry);
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public static DmaStreamComputeExecutionResult ExecuteToCommitPending(DmaStreamComputeDescriptor descriptor, DmaStreamAcceleratorBackend backend, ulong tokenId = 0, DmaStreamComputeTelemetryCounters? telemetry = null)
	std::shared_ptr<DmaStreamComputeExecutionResult> DmaStreamComputeRuntime::ExecuteToCommitPending(std::shared_ptr<DmaStreamComputeDescriptor> descriptor, std::shared_ptr<DmaStreamAcceleratorBackend> backend, unsigned long long tokenId, std::shared_ptr<DmaStreamComputeTelemetryCounters> telemetry)
	{
		ArgumentNullException::ThrowIfNull(descriptor);
		ArgumentNullException::ThrowIfNull(backend);
		backend->AttachTelemetry(telemetry);

		std::shared_ptr<DmaStreamComputeToken> token = std::make_shared<DmaStreamComputeToken>(descriptor, tokenId == 0 ? descriptor->DescriptorIdentityHash : tokenId, telemetry);
		std::shared_ptr<DmaStreamComputeCommitResult> completion = ExecuteTokenToCommitPending(descriptor, token, backend);
		return CreateResult(token, completion, backend);
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public static DmaStreamComputeExecutionResult ExecuteMaterializedMicroOpToCommitPending(DmaStreamComputeMicroOp microOp, DmaStreamComputeTokenStore tokenStore, DmaStreamComputeIssueAdmissionMetadata issueMetadata, DmaStreamAcceleratorBackend? backend = null, System.Nullable<DmaStreamComputeTelemetryCounters> telemetry = null)
	std::shared_ptr<DmaStreamComputeExecutionResult> DmaStreamComputeRuntime::ExecuteMaterializedMicroOpToCommitPending(std::shared_ptr<DmaStreamComputeMicroOp> microOp, std::shared_ptr<DmaStreamComputeTokenStore> tokenStore, std::shared_ptr<DmaStreamComputeIssueAdmissionMetadata> issueMetadata, std::shared_ptr<DmaStreamAcceleratorBackend> backend, std::shared_ptr<DmaStreamComputeTelemetryCounters> telemetry)
	{
		ArgumentNullException::ThrowIfNull(microOp);
		ArgumentNullException::ThrowIfNull(tokenStore);
		ArgumentNullException::ThrowIfNull(issueMetadata);

		backend = (backend != nullptr) ? backend : std::make_shared<DmaStreamAcceleratorBackend>(Processor::MainMemory, telemetry);
		backend->AttachTelemetry(telemetry);

		std::shared_ptr<DmaStreamComputeIssueAdmissionResult> admission = tokenStore->TryAllocateAtIssueAdmission(DmaStreamComputeIssueAdmissionRequest::ForLane6(microOp, *issueMetadata, telemetry));

		if (!admission->IsAccepted || admission->Entry == nullptr || admission->Entry->Token == nullptr)
		{
			throw InvalidOperationException(std::wstring(L"DmaStreamCompute materialized lane6 execution rejected before token-owned runtime entry: ") + admission->Message);
		}

		std::shared_ptr<DmaStreamComputeToken> token = admission->Entry->Token;
		std::shared_ptr<DmaStreamComputeCommitResult> completion = ExecuteTokenToCommitPending(microOp->getDescriptor(), token, backend);
		return CreateResult(token, completion, backend, admission);
	}

	std::shared_ptr<DmaStreamComputeCommitResult> DmaStreamComputeRuntime::ExecuteTokenToCommitPending(std::shared_ptr<DmaStreamComputeDescriptor> descriptor, std::shared_ptr<DmaStreamComputeToken> token, std::shared_ptr<DmaStreamAcceleratorBackend> backend)
	{
		token->MarkIssued();

		std::shared_ptr<DmaStreamComputeCommitResult> validationFault;
		if (!TryValidateRuntimeDescriptor(descriptor, token, validationFault))
		{
			return validationFault;
		}

		std::vector<std::vector<unsigned char>> operands;
		std::shared_ptr<DmaStreamComputeCommitResult> readFault;
		if (!TryReadOperands(descriptor, token, backend, operands, readFault))
		{
			return readFault;
		}

		token->MarkReadsComplete();

		std::vector<unsigned char> output;
		std::shared_ptr<DmaStreamComputeCommitResult> computeFault;
		if (!TryCompute(descriptor, token, operands, backend, output, computeFault))
		{
			return computeFault;
		}

		std::shared_ptr<DmaStreamComputeCommitResult> stageResult = StageOutput(descriptor, token, backend, output);
		if (stageResult->getRequiresRetireExceptionPublication())
		{
			return stageResult;
		}

		return token->MarkComputeComplete();
	}

	std::shared_ptr<DmaStreamComputeExecutionResult> DmaStreamComputeRuntime::CreateResult(std::shared_ptr<DmaStreamComputeToken> token, std::shared_ptr<DmaStreamComputeCommitResult> completion, std::shared_ptr<DmaStreamAcceleratorBackend> backend, const std::optional<std::shared_ptr<DmaStreamComputeIssueAdmissionResult>>& issueAdmission)
	{
		auto result = std::make_shared<DmaStreamComputeExecutionResult>();
		result->Token = std::move(token);
		result->Completion = std::move(completion);
		result->Telemetry = backend->SnapshotTelemetry();
		result->IssueAdmission = issueAdmission;
		result->RequiresRetireExceptionPublication = result->Completion && result->Completion->getRequiresRetireExceptionPublication();
		result->IsCommitPending = result->Token->getState() == DmaStreamComputeTokenState::CommitPending && !result->RequiresRetireExceptionPublication;
		if (result->IssueAdmission && *result->IssueAdmission)
		{
			result->TokenHandle = (*result->IssueAdmission)->Handle;
			result->IsStoreTracked = (*result->IssueAdmission)->HasAllocatedToken;
		}
		return result;
	}

	bool DmaStreamComputeRuntime::TryValidateRuntimeDescriptor(std::shared_ptr<DmaStreamComputeDescriptor> descriptor, std::shared_ptr<DmaStreamComputeToken> token, std::shared_ptr<DmaStreamComputeCommitResult> &fault)
	{
	// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
	// ORIGINAL LINE: fault = null!;
		fault = nullptr;

		if (descriptor->RangeEncoding != DmaStreamComputeRangeEncoding::InlineContiguous)
		{
			fault = token->PublishFault(DmaStreamComputeTokenFaultKind::UnsupportedAbiOrOperation, L"DmaStreamCompute runtime only supports inline contiguous range encoding in Phase 06.", descriptor->DescriptorReference.DescriptorAddress, false);
			return false;
		}

		if (descriptor->PartialCompletionPolicy != DmaStreamComputePartialCompletionPolicy::AllOrNone)
		{
			fault = token->PublishFault(DmaStreamComputeTokenFaultKind::UnsupportedAbiOrOperation, L"DmaStreamCompute runtime only supports all-or-none partial completion in Phase 06.", descriptor->DescriptorReference.DescriptorAddress, false);
			return false;
		}

		if (ResolveElementSize(descriptor->ElementType) == 0)
		{
			fault = token->PublishFault(DmaStreamComputeTokenFaultKind::UnsupportedAbiOrOperation, L"DmaStreamCompute runtime reached an unsupported element type.", descriptor->DescriptorReference.DescriptorAddress, false);
			return false;
		}

		if (GetExpectedOperandCount(descriptor->Operation) == 0)
		{
			fault = token->PublishFault(DmaStreamComputeTokenFaultKind::UnsupportedAbiOrOperation, L"DmaStreamCompute runtime reached an unsupported operation.", descriptor->DescriptorReference.DescriptorAddress, false);
			return false;
		}

		if (descriptor->ReadMemoryRanges.size() != static_cast<size_t>(GetExpectedOperandCount(descriptor->Operation)) || descriptor->WriteMemoryRanges.empty())
		{
			fault = token->PublishFault(DmaStreamComputeTokenFaultKind::DescriptorDecodeFault, L"DmaStreamCompute runtime descriptor range counts do not match the Phase 06 execution contract.", descriptor->DescriptorReference.DescriptorAddress, false);
			return false;
		}

		if (!IsSupportedRuntimeShape(descriptor->Operation, descriptor->Shape))
		{
			fault = token->PublishFault(DmaStreamComputeTokenFaultKind::UnsupportedAbiOrOperation, L"DmaStreamCompute runtime reached an unsupported Phase 06 DSC1 shape.", descriptor->DescriptorReference.DescriptorAddress, false);
			return false;
		}

		return true;
	}

	bool DmaStreamComputeRuntime::TryReadOperands(std::shared_ptr<DmaStreamComputeDescriptor> descriptor, std::shared_ptr<DmaStreamComputeToken> token, std::shared_ptr<DmaStreamAcceleratorBackend> backend, std::vector<std::vector<unsigned char>> &operands, std::shared_ptr<DmaStreamComputeCommitResult> &fault)
	{
		operands.clear();
	// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
	// ORIGINAL LINE: fault = null!;
		fault = nullptr;

		int operandCount = static_cast<int>(descriptor->ReadMemoryRanges.size());
		operands = std::vector<std::vector<unsigned char>>(operandCount);
		for (int i = 0; i < operandCount; i++)
		{
			const DmaStreamComputeMemoryRange &range = descriptor->ReadMemoryRanges[static_cast<size_t>(i)];
			if (range.Length > static_cast<unsigned long long>(std::numeric_limits<int>::max()))
			{
				const std::wstring message = L"DmaStreamCompute runtime source range length " + std::to_wstring(range.Length) + L" exceeds the materializable Phase 07 buffer limit.";
				fault = token->PublishFault(DmaStreamComputeTokenFaultKind::MemoryFault, message, range.Address, false);
				return false;
			}

			std::vector<unsigned char> buffer(static_cast<size_t>(range.Length));
			std::wstring message;
			if (!backend->TryReadRange(std::make_shared<DmaStreamComputeMemoryRange>(range), buffer, message))
			{
				fault = token->PublishFault(DmaStreamComputeTokenFaultKind::MemoryFault, message, range.Address, false);
				return false;
			}

			operands[i] = buffer;
		}

		return true;
	}

	bool DmaStreamComputeRuntime::TryCompute(std::shared_ptr<DmaStreamComputeDescriptor> descriptor, std::shared_ptr<DmaStreamComputeToken> token, std::vector<std::vector<unsigned char>> &operands, std::shared_ptr<DmaStreamAcceleratorBackend> backend, std::vector<unsigned char> &output, std::shared_ptr<DmaStreamComputeCommitResult> &fault)
	{
		output.clear();
	// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
	// ORIGINAL LINE: fault = null!;
		fault = nullptr;

		int elementSize = ResolveElementSize(descriptor->ElementType);
		unsigned long long totalWriteBytes;
		if (!TryGetTotalWriteBytes(descriptor, totalWriteBytes) || totalWriteBytes > std::numeric_limits<int>::max() || totalWriteBytes % static_cast<unsigned long long>(elementSize) != 0)
		{
			fault = token->PublishFault(DmaStreamComputeTokenFaultKind::PartialCompletionFault, L"DmaStreamCompute runtime write footprint is not exactly materializable as complete elements.", descriptor->DescriptorReference.DescriptorAddress, true);
			return false;
		}

		switch (descriptor->Operation)
		{
			case DmaStreamComputeOperationKind::Copy:
				return TryComputeCopy(descriptor, token, operands, totalWriteBytes, backend, output, fault);

			case DmaStreamComputeOperationKind::Add:
			case DmaStreamComputeOperationKind::Mul:
				return TryComputeBinary(descriptor, token, operands, totalWriteBytes, backend, output, fault);

			case DmaStreamComputeOperationKind::Fma:
				return TryComputeFma(descriptor, token, operands, totalWriteBytes, backend, output, fault);

			case DmaStreamComputeOperationKind::Reduce:
				return TryComputeReduce(descriptor, token, operands, totalWriteBytes, backend, output, fault);

			default:
				fault = token->PublishFault(DmaStreamComputeTokenFaultKind::UnsupportedAbiOrOperation, L"DmaStreamCompute runtime reached an unsupported operation.", descriptor->DescriptorReference.DescriptorAddress, false);
				return false;
		}
	}

	bool DmaStreamComputeRuntime::TryComputeCopy(std::shared_ptr<DmaStreamComputeDescriptor> descriptor, std::shared_ptr<DmaStreamComputeToken> token, std::vector<std::vector<unsigned char>> &operands, unsigned long long totalWriteBytes, std::shared_ptr<DmaStreamAcceleratorBackend> backend, std::vector<unsigned char> &output, std::shared_ptr<DmaStreamComputeCommitResult> &fault)
	{
		output.clear();
	// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
	// ORIGINAL LINE: fault = null!;
		fault = nullptr;

		if (operands.size() != 1 || static_cast<unsigned long long>(operands[0].size()) != totalWriteBytes)
		{
			fault = token->PublishFault(DmaStreamComputeTokenFaultKind::PartialCompletionFault, L"DmaStreamCompute copy requires source and destination byte footprints to match exactly.", descriptor->DescriptorReference.DescriptorAddress, true);
			return false;
		}

		output = std::vector<unsigned char>(static_cast<int>(totalWriteBytes));
		output = operands[0];
		int elementSize = ResolveElementSize(descriptor->ElementType);
		backend->RecordComputeElements(totalWriteBytes / static_cast<unsigned long long>(elementSize), descriptor->Operation);
		return true;
	}

	bool DmaStreamComputeRuntime::TryComputeBinary(std::shared_ptr<DmaStreamComputeDescriptor> descriptor, std::shared_ptr<DmaStreamComputeToken> token, std::vector<std::vector<unsigned char>> &operands, unsigned long long totalWriteBytes, std::shared_ptr<DmaStreamAcceleratorBackend> backend, std::vector<unsigned char> &output, std::shared_ptr<DmaStreamComputeCommitResult> &fault)
	{
		output.clear();
	// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
	// ORIGINAL LINE: fault = null!;
		fault = nullptr;

		if (!ValidateOperandByteLengths(descriptor, token, operands, totalWriteBytes, fault))
		{
			return false;
		}

		int elementSize = ResolveElementSize(descriptor->ElementType);
		unsigned long long elementCount = totalWriteBytes / static_cast<unsigned long long>(elementSize);
		output = std::vector<unsigned char>(static_cast<int>(totalWriteBytes));
		for (unsigned long long i = 0; i < elementCount; i++)
		{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'checked' in this context:
// ORIGINAL LINE: int offset = checked((int)(i * (ulong)elementSize));
			int offset = static_cast<int>(i * static_cast<unsigned long long>(elementSize));
			if (IsFloatingElement(descriptor->ElementType))
			{
				double left = LoadFloat(operands[0], offset, descriptor->ElementType);
				double right = LoadFloat(operands[1], offset, descriptor->ElementType);
				double value = descriptor->Operation == DmaStreamComputeOperationKind::Add ? left + right : left * right;
				StoreFloat(output, offset, descriptor->ElementType, value);
			}
			else
			{
				unsigned long long left = LoadUnsigned(operands[0], offset, descriptor->ElementType);
				unsigned long long right = LoadUnsigned(operands[1], offset, descriptor->ElementType);
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: ulong value = descriptor.Operation == DmaStreamComputeOperationKind.Add ? unchecked(left + right) : unchecked(left * right);
				unsigned long long value = descriptor->Operation == DmaStreamComputeOperationKind::Add ? left + right : left * right;
				StoreUnsigned(output, offset, descriptor->ElementType, value);
			}
		}

		backend->RecordComputeElements(elementCount, descriptor->Operation);
		return true;
	}

	bool DmaStreamComputeRuntime::TryComputeFma(std::shared_ptr<DmaStreamComputeDescriptor> descriptor, std::shared_ptr<DmaStreamComputeToken> token, std::vector<std::vector<unsigned char>> &operands, unsigned long long totalWriteBytes, std::shared_ptr<DmaStreamAcceleratorBackend> backend, std::vector<unsigned char> &output, std::shared_ptr<DmaStreamComputeCommitResult> &fault)
	{
		output.clear();
	// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
	// ORIGINAL LINE: fault = null!;
		fault = nullptr;

		if (!ValidateOperandByteLengths(descriptor, token, operands, totalWriteBytes, fault))
		{
			return false;
		}

		int elementSize = ResolveElementSize(descriptor->ElementType);
		unsigned long long elementCount = totalWriteBytes / static_cast<unsigned long long>(elementSize);
		output = std::vector<unsigned char>(static_cast<int>(totalWriteBytes));
		for (unsigned long long i = 0; i < elementCount; i++)
		{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'checked' in this context:
// ORIGINAL LINE: int offset = checked((int)(i * (ulong)elementSize));
			int offset = static_cast<int>(i * static_cast<unsigned long long>(elementSize));
			if (IsFloatingElement(descriptor->ElementType))
			{
				double a = LoadFloat(operands[0], offset, descriptor->ElementType);
				double b = LoadFloat(operands[1], offset, descriptor->ElementType);
				double c = LoadFloat(operands[2], offset, descriptor->ElementType);
				StoreFloat(output, offset, descriptor->ElementType, (a * b) + c);
			}
			else
			{
				unsigned long long a = LoadUnsigned(operands[0], offset, descriptor->ElementType);
				unsigned long long b = LoadUnsigned(operands[1], offset, descriptor->ElementType);
				unsigned long long c = LoadUnsigned(operands[2], offset, descriptor->ElementType);
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: StoreUnsigned(output, offset, descriptor.ElementType, unchecked((a * b) + c));
				StoreUnsigned(output, offset, descriptor->ElementType, (a * b) + c);
			}
		}

		backend->RecordComputeElements(elementCount, descriptor->Operation);
		return true;
	}

	bool DmaStreamComputeRuntime::TryComputeReduce(std::shared_ptr<DmaStreamComputeDescriptor> descriptor, std::shared_ptr<DmaStreamComputeToken> token, std::vector<std::vector<unsigned char>> &operands, unsigned long long totalWriteBytes, std::shared_ptr<DmaStreamAcceleratorBackend> backend, std::vector<unsigned char> &output, std::shared_ptr<DmaStreamComputeCommitResult> &fault)
	{
		output.clear();
	// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
	// ORIGINAL LINE: fault = null!;
		fault = nullptr;

		int elementSize = ResolveElementSize(descriptor->ElementType);
		if (operands.size() != 1 || operands[0].empty() || operands[0].size() % elementSize != 0 || totalWriteBytes != static_cast<unsigned long long>(elementSize))
		{
			fault = token->PublishFault(DmaStreamComputeTokenFaultKind::PartialCompletionFault, L"DmaStreamCompute reduce requires one complete source vector and one scalar destination element.", descriptor->DescriptorReference.DescriptorAddress, true);
			return false;
		}

		unsigned long long elementCount = static_cast<unsigned long long>(operands[0].size() / elementSize);
		output = std::vector<unsigned char>(elementSize);
		if (IsFloatingElement(descriptor->ElementType))
		{
			double accumulator = 0;
			for (unsigned long long i = 0; i < elementCount; i++)
			{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'checked' in this context:
// ORIGINAL LINE: accumulator += LoadFloat(operands[0], checked((int)(i * (ulong)elementSize)), descriptor.ElementType);
				accumulator += LoadFloat(operands[0], static_cast<int>(i * static_cast<unsigned long long>(elementSize)), descriptor->ElementType);
			}

			StoreFloat(output, 0, descriptor->ElementType, accumulator);
		}
		else
		{
			unsigned long long accumulator = 0;
			for (unsigned long long i = 0; i < elementCount; i++)
			{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'checked' in this context:
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: accumulator = unchecked(accumulator + LoadUnsigned(operands[0], checked((int)(i * (ulong)elementSize)), descriptor.ElementType));
				accumulator = accumulator + LoadUnsigned(operands[0], static_cast<int>(i * static_cast<unsigned long long>(elementSize)), descriptor->ElementType);
			}

			StoreUnsigned(output, 0, descriptor->ElementType, accumulator);
		}

		backend->RecordComputeElements(elementCount, descriptor->Operation);
		return true;
	}

	bool DmaStreamComputeRuntime::ValidateOperandByteLengths(std::shared_ptr<DmaStreamComputeDescriptor> descriptor, std::shared_ptr<DmaStreamComputeToken> token, std::vector<std::vector<unsigned char>> &operands, unsigned long long totalWriteBytes, std::shared_ptr<DmaStreamComputeCommitResult> &fault)
	{
	// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
	// ORIGINAL LINE: fault = null!;
		fault = nullptr;
		if (operands.size() != GetExpectedOperandCount(descriptor->Operation))
		{
			fault = token->PublishFault(DmaStreamComputeTokenFaultKind::DescriptorDecodeFault, L"DmaStreamCompute runtime operand count changed after descriptor acceptance.", descriptor->DescriptorReference.DescriptorAddress, false);
			return false;
		}

		for (int i = 0; i < operands.size(); i++)
		{
			if (static_cast<unsigned long long>(operands[i].size()) == totalWriteBytes)
			{
				continue;
			}

			fault = token->PublishFault(DmaStreamComputeTokenFaultKind::PartialCompletionFault, L"DmaStreamCompute element-wise operation requires every source footprint to match the destination footprint exactly.", descriptor->ReadMemoryRanges[i].Address, false);
			return false;
		}

		return true;
	}

	std::shared_ptr<DmaStreamComputeCommitResult> DmaStreamComputeRuntime::StageOutput(std::shared_ptr<DmaStreamComputeDescriptor> descriptor, std::shared_ptr<DmaStreamComputeToken> token, std::shared_ptr<DmaStreamAcceleratorBackend> backend, std::vector<unsigned char> &output)
	{
		int offset = 0;
		try
		{
			for (size_t i = 0; i < descriptor->WriteMemoryRanges.size(); i++)
			{
				const DmaStreamComputeMemoryRange &range = descriptor->WriteMemoryRanges[i];
				if (range.Length > static_cast<unsigned long long>(std::numeric_limits<int>::max()) || static_cast<size_t>(offset) > output.size() || static_cast<size_t>(range.Length) > output.size() - static_cast<size_t>(offset))
				{
					return token->PublishFault(DmaStreamComputeTokenFaultKind::PartialCompletionFault, L"DmaStreamCompute runtime output buffer does not exactly cover the descriptor destination footprint.", range.Address, true);
				}

				token->StageDestinationWrite(range.Address, std::span<const unsigned char>(output).subspan(static_cast<size_t>(offset), static_cast<size_t>(range.Length)));
				backend->RecordStagedWrite(range.Length);
				offset += static_cast<int>(range.Length);
			}
		}
		catch (const InvalidOperationException &e1)
		{
	// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
	// ORIGINAL LINE: return DmaStreamComputeCommitResult.Faulted(token.LastFault!);
			const auto lastFault = token->getLastFault();
			return DmaStreamComputeCommitResult::Faulted(lastFault && *lastFault ? *lastFault : nullptr);
		}

		if (offset != output.size())
		{
			return token->PublishFault(DmaStreamComputeTokenFaultKind::PartialCompletionFault, L"DmaStreamCompute runtime produced extra bytes outside the accepted destination footprint.", descriptor->DescriptorReference.DescriptorAddress, true);
		}

		return DmaStreamComputeCommitResult::Pending(token->getState());
	}

	bool DmaStreamComputeRuntime::TryGetTotalWriteBytes(std::shared_ptr<DmaStreamComputeDescriptor> descriptor, unsigned long long &totalWriteBytes)
	{
		totalWriteBytes = 0;
		if (descriptor->WriteMemoryRanges.empty())
		{
			return false;
		}

		for (size_t i = 0; i < descriptor->WriteMemoryRanges.size(); i++)
		{
			unsigned long long length = descriptor->WriteMemoryRanges[i].Length;
			if (length == 0 || totalWriteBytes > std::numeric_limits<unsigned long long>::max() - length)
			{
				return false;
			}

			totalWriteBytes += length;
		}

		return true;
	}

	int DmaStreamComputeRuntime::GetExpectedOperandCount(DmaStreamComputeOperationKind operation)
	{
		int temp_var44;
		switch (operation)
		{
			case DmaStreamComputeOperationKind::Copy:
				temp_var44 = 1;
				break;
			case DmaStreamComputeOperationKind::Add:
				temp_var44 = 2;
				break;
			case DmaStreamComputeOperationKind::Mul:
				temp_var44 = 2;
				break;
			case DmaStreamComputeOperationKind::Fma:
				temp_var44 = 3;
				break;
			case DmaStreamComputeOperationKind::Reduce:
				temp_var44 = 1;
				break;
			default:
				temp_var44 = 0;
				break;
		}
		return temp_var44;
	}

	bool DmaStreamComputeRuntime::IsSupportedRuntimeShape(DmaStreamComputeOperationKind operation, DmaStreamComputeShapeKind shape)
	{
		return shape == DmaStreamComputeShapeKind::Contiguous1D || (operation == DmaStreamComputeOperationKind::Reduce && shape == DmaStreamComputeShapeKind::FixedReduce);
	}

	int DmaStreamComputeRuntime::ResolveElementSize(DmaStreamComputeElementType elementType)
	{
		int temp_var45;
		switch (elementType)
		{
			case DmaStreamComputeElementType::UInt8:
				temp_var45 = 1;
				break;
			case DmaStreamComputeElementType::UInt16:
				temp_var45 = 2;
				break;
			case DmaStreamComputeElementType::UInt32:
				temp_var45 = 4;
				break;
			case DmaStreamComputeElementType::UInt64:
				temp_var45 = 8;
				break;
			case DmaStreamComputeElementType::Float32:
				temp_var45 = 4;
				break;
			case DmaStreamComputeElementType::Float64:
				temp_var45 = 8;
				break;
			default:
				temp_var45 = 0;
				break;
		}
		return temp_var45;
	}

	bool DmaStreamComputeRuntime::IsFloatingElement(DmaStreamComputeElementType elementType)
	{
		return (elementType == DmaStreamComputeElementType::Float32 || elementType == DmaStreamComputeElementType::Float64);
	}

	unsigned long long DmaStreamComputeRuntime::LoadUnsigned(std::span<const unsigned char> buffer, int offset, DmaStreamComputeElementType elementType)
	{
		switch (elementType)
		{
			case DmaStreamComputeElementType::UInt8: return buffer[static_cast<size_t>(offset)];
			case DmaStreamComputeElementType::UInt16: return ReadLittleEndian(buffer.subspan(static_cast<size_t>(offset), 2));
			case DmaStreamComputeElementType::UInt32: return ReadLittleEndian(buffer.subspan(static_cast<size_t>(offset), 4));
			case DmaStreamComputeElementType::UInt64: return ReadLittleEndian(buffer.subspan(static_cast<size_t>(offset), 8));
			default: throw InvalidOperationException(L"Unsigned load reached a non-unsigned DmaStreamCompute element type.");
		}
	}

	void DmaStreamComputeRuntime::StoreUnsigned(std::span<unsigned char> buffer, int offset, DmaStreamComputeElementType elementType, unsigned long long value)
	{
		switch (elementType)
		{
			case DmaStreamComputeElementType::UInt8:
				buffer[offset] = static_cast<unsigned char>(value);
				break;
			case DmaStreamComputeElementType::UInt16:
				WriteLittleEndian(buffer.subspan(static_cast<size_t>(offset), 2), value);
				break;
			case DmaStreamComputeElementType::UInt32:
				WriteLittleEndian(buffer.subspan(static_cast<size_t>(offset), 4), value);
				break;
			case DmaStreamComputeElementType::UInt64:
				WriteLittleEndian(buffer.subspan(static_cast<size_t>(offset), 8), value);
				break;
			default:
				throw InvalidOperationException(L"Unsigned store reached a non-unsigned DmaStreamCompute element type.");
		}
	}

	double DmaStreamComputeRuntime::LoadFloat(std::span<const unsigned char> buffer, int offset, DmaStreamComputeElementType elementType)
	{
		switch (elementType)
		{
			case DmaStreamComputeElementType::Float32:
				return static_cast<double>(std::bit_cast<float>(static_cast<uint32_t>(ReadLittleEndian(buffer.subspan(static_cast<size_t>(offset), 4)))));
			case DmaStreamComputeElementType::Float64:
				return std::bit_cast<double>(ReadLittleEndian(buffer.subspan(static_cast<size_t>(offset), 8)));
			default:
				throw InvalidOperationException(L"Floating-point load reached a non-floating DmaStreamCompute element type.");
		}
	}

	void DmaStreamComputeRuntime::StoreFloat(std::span<unsigned char> buffer, int offset, DmaStreamComputeElementType elementType, double value)
	{
		switch (elementType)
		{
			case DmaStreamComputeElementType::Float32:
				WriteLittleEndian(buffer.subspan(static_cast<size_t>(offset), 4), std::bit_cast<uint32_t>(static_cast<float>(value)));
				break;
			case DmaStreamComputeElementType::Float64:
				WriteLittleEndian(buffer.subspan(static_cast<size_t>(offset), 8), std::bit_cast<uint64_t>(value));
				break;
			default:
				throw InvalidOperationException(L"Floating-point store reached a non-floating DmaStreamCompute element type.");
		}
	}
}


