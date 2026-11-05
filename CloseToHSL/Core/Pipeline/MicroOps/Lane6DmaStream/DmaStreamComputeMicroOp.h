#pragma once

#include "../Types/MicroOp.h"
#include "../../../Execution/DmaStreamCompute/DmaStreamComputeDescriptor.h"
#include "../../../Execution/DmaStreamCompute/DmaStreamComputeTokenStore.h"
#include "../../../Execution/DmaStreamCompute/DmaStreamComputeToken.h"
#include "../../../Execution/DmaStreamCompute/DmaStreamComputeReplayEvidence.h"

#include "../../../Architecture/Common/YAKSys_Hybrid_CPU.Processor.h"
#include "../../../Architecture/Registers/Retire/RetireRecord.h"
#include "../../Safety/ResourceBitset.h"
#include <string>
#include <limits>
#include <optional>
#include <format>
#include "tangible_exception_helper.h"
#include <memory>

using namespace YAKSys_Hybrid_CPU::Arch;
namespace YAKSys_Hybrid_CPU::Core::Execution::DmaStreamCompute { struct DmaStreamComputeExecutionResult; }

using namespace YAKSys_Hybrid_CPU::Core::Execution::DmaStreamCompute;
using namespace YAKSys_Hybrid_CPU::Core::Registers;
using namespace YAKSys_Hybrid_CPU::Core::Registers::Retire;
using namespace YAKSys_Hybrid_CPU::Memory;

namespace YAKSys_Hybrid_CPU::Core
{
	/// <summary>
	/// Canonical lane6 typed-slot carrier for descriptor-backed memory-memory compute.
	/// Phase 06 opens the current DSC1 production contour only: lane6 token-owned
	/// runtime execution, staged writes, and retire-owned commit.
	/// </summary>
	class DmaStreamComputeMicroOp final : public MicroOp
	{
	private:
		const std::shared_ptr<DmaStreamComputeDescriptor> Descriptor;
		const std::shared_ptr<DmaStreamComputeDescriptorReference> DescriptorReference;
		const unsigned long long DescriptorIdentityHash;
		const unsigned long long CertificateInputHash;
		const unsigned long long NormalizedFootprintHash;
		const DmaStreamComputeOperationKind Operation;
		const DmaStreamComputeElementType ElementType;
		const DmaStreamComputeShapeKind Shape;
		const DmaStreamComputeRangeEncoding RangeEncoding;
		const DmaStreamComputePartialCompletionPolicy PartialCompletionPolicy;
		const std::shared_ptr<DmaStreamComputeOwnerBinding> OwnerBinding;
		const std::shared_ptr<DmaStreamComputeReplayEvidence> ReplayEvidence;
		std::shared_ptr<DmaStreamComputeReplayEvidence> LastExecutionReplayEvidence;

		std::shared_ptr<DmaStreamComputeTokenStore> _tokenStore = std::make_shared<DmaStreamComputeTokenStore>();
		std::optional<std::shared_ptr<DmaStreamComputeExecutionResult>> _lastExecutionResult;
		std::optional<std::shared_ptr<DmaStreamComputeCommitResult>> _lastRetireCommitResult;

	public:
		DmaStreamComputeMicroOp(std::shared_ptr<DmaStreamComputeDescriptor> descriptor);

		std::shared_ptr<DmaStreamComputeDescriptor> getDescriptor() const;

		std::shared_ptr<DmaStreamComputeDescriptorReference> getDescriptorReference() const;

		unsigned long long getDescriptorIdentityHash() const;

		unsigned long long getCertificateInputHash() const;

		unsigned long long getNormalizedFootprintHash() const;

		DmaStreamComputeOperationKind getOperation() const;

		DmaStreamComputeElementType getElementType() const;

		DmaStreamComputeShapeKind getShape() const;

		DmaStreamComputeRangeEncoding getRangeEncoding() const;

		DmaStreamComputePartialCompletionPolicy getPartialCompletionPolicy() const;

		std::shared_ptr<DmaStreamComputeOwnerBinding> getOwnerBinding() const;

		std::shared_ptr<DmaStreamComputeReplayEvidence> getReplayEvidence() const;

		std::optional<std::shared_ptr<DmaStreamComputeExecutionResult>> getLastExecutionResult() const;

		std::optional<std::shared_ptr<DmaStreamComputeCommitResult>> getLastRetireCommitResult() const;

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public DmaStreamComputeToken? LastExecutionToken
		std::shared_ptr<DmaStreamComputeToken> getLastExecutionToken() const;

		DmaStreamComputeTokenHandle getLastExecutionTokenHandle() const;

		int getActiveTokenCount() const;

		std::shared_ptr<DmaStreamComputeReplayEvidence> getLastExecutionReplayEvidence() const;
	private:
		static std::shared_ptr<DmaStreamComputeDescriptor> ValidateDescriptorForConstruction(std::shared_ptr<DmaStreamComputeDescriptor> descriptor);
		void setLastExecutionReplayEvidence(std::shared_ptr<DmaStreamComputeReplayEvidence> value);

	public:
		CanonicalDecodePublicationMode getCanonicalDecodePublication() const override;

		std::shared_ptr<DmaStreamComputeReplayEvidence> ExportReplayEvidence(DmaStreamComputeTokenLifecycleEvidence tokenLifecycleEvidence = {}, DmaStreamComputeLanePlacementEvidence lanePlacementEvidence = {});

		bool Execute(std::shared_ptr<Processor::CPU_Core> &core) override;

		void Commit(std::shared_ptr<Processor::CPU_Core> &core) override;

		void EmitWriteBackRetireRecords(std::shared_ptr<Processor::CPU_Core> &core, std::span<RetireRecord> retireRecords, int &retireRecordCount) override;

		std::wstring GetDescription() override;

	private:
		static void EnsureMandatoryFootprints(std::shared_ptr<DmaStreamComputeDescriptor> descriptor);

		static void EnsureOwnerDomainGuardAccepted(std::shared_ptr<DmaStreamComputeDescriptor> descriptor);

		static std::vector<MemoryRange> ConvertRanges(const std::vector<DmaStreamComputeMemoryRange>& ranges);

		static bool IsMemoryWritingOperation(DmaStreamComputeOperationKind operation);

		static void EnsurePhase06ProductionContour(std::shared_ptr<DmaStreamComputeDescriptor> descriptor);

		void EnsureNoUnretiredExecution();

		void RejectGuestCompatibilityExecution(std::shared_ptr<Processor::CPU_Core> &core);

		static bool IsPhase06Shape(DmaStreamComputeOperationKind operation, DmaStreamComputeShapeKind shape);

		static int ResolveElementSize(DmaStreamComputeElementType elementType);

// C# TO C++ CONVERTER TASK: Methods returning tuples are not converted by C# to C++ Converter:
//		private static IReadOnlyList<(ulong Address, ulong Length)> ConvertRanges(IReadOnlyList<DmaStreamComputeMemoryRange> ranges)
//		{
//			if (ranges == nullptr || ranges.Count == 0)
//			{
//				return Array.Empty<(ulong Address, ulong Length)>();
//			}
//
//			(ulong Address, ulong Length)[] converted = new(ulong Address, ulong Length)[ranges.Count];
//			for (int i = 0; i < ranges.Count; i++)
//			{
//				converted[i] = (ranges[i].Address, ranges[i].Length);
//			}
//
//			return converted;
//		}

		static ResourceBitset BuildResourceMask(unsigned long long ownerDomainTag);

		static int ConvertOwnerContextId(unsigned int ownerContextId);

	protected:
		std::shared_ptr<DmaStreamComputeMicroOp> shared_from_this()
		{
			return std::static_pointer_cast<DmaStreamComputeMicroOp>(MicroOp::shared_from_this());
		}
	};
}


