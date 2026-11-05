#pragma once

#include "../Architecture/Common/YAKSys_Hybrid_CPU.Processor.h"
#include "../ISA/Instructions/NonVmx/Lanes00_03Vector/MatrixTile/MatrixTileIrProjectionAndMaterializer.h"
#include "../ISA/Instructions/NonVmx/Lanes00_03Vector/MatrixTile/MatrixTileResourceContour.h"
#include "Scheduling/SlotLegality/SlotClassDefinitions.h"
#include "../Contracts/CompilerTransport/VliwBundleAnnotations.h"
#include "../../../NonRTL/Arch/Compat/VLIW_Bundle.h"
#include "../Decoder/DecodedInstructionBundle.h"
#include "MicroOps/Types/MicroOp.h"
#include "../../../NonRTL/Arch/Compat/HybridCPU_ISE.Arch.VLIW_Instruction.h"
#include "MicroOps/MatrixTile/MatrixTileMicroOps.h"
#include <string>
#include <vector>
#include <stdexcept>
#include <optional>
#include <format>
#include "tangible_exception_helper.h"
#include <memory>

// C# TO C++ CONVERTER NOTE: Forward class declarations:
namespace YAKSys_Hybrid_CPU::Core { class MatrixTileFullPipelineStepEvidence; }

using namespace HybridCPU_ISE::Arch;
using namespace YAKSys_Hybrid_CPU::Arch;
using namespace YAKSys_Hybrid_CPU::CloseToHSL::Core::ISA::Instructions::NonVmx::Lanes00_03Vector::MatrixTile;
using namespace YAKSys_Hybrid_CPU::Core::Decoder;
using namespace YAKSys_Hybrid_CPU::Core::Pipeline::MicroOps;
// C# TO C++ CONVERTER TASK: The C# 'using static' statement  cannot be converted to C++:
//using static YAKSys_Hybrid_CPU.Processor.CPU_Core;

namespace YAKSys_Hybrid_CPU::Core
{
	using MatrixTileFullPipelineBeforeRetireCallback = std::function<void (std::shared_ptr<Processor::CPU_Core> &core, std::shared_ptr<MatrixTileFullPipelineStepEvidence> step, std::shared_ptr<MatrixTileExecutionCaptureRecord> capture)>;

	using MatrixTileFullPipelineBeforeExecuteCallback = std::function<void (std::shared_ptr<Processor::CPU_Core> &core, std::shared_ptr<MatrixTileFullPipelineStepEvidence> step)>;

	using MatrixTileFullPipelineAfterRetireCallback = std::function<void (std::shared_ptr<Processor::CPU_Core> &core, std::shared_ptr<MatrixTileFullPipelineStepEvidence> step, std::shared_ptr<MatrixTileExecutionCaptureRecord> capture, std::shared_ptr<MatrixTileRetireOutcome> retireOutcome)>;

	using MatrixTileFullPipelineCaptureMutator = std::function<std::shared_ptr<MatrixTileExecutionCaptureRecord> (std::shared_ptr<MatrixTileFullPipelineStepEvidence> step, std::shared_ptr<MatrixTileExecutionCaptureRecord> capture)>;

	class MatrixTileFullPipelineHarnessOptions final : public std::enable_shared_from_this<MatrixTileFullPipelineHarnessOptions>
	{
	private:
		unsigned long long BaseBundleAddress = 0xA'0000ULL;
		unsigned long long BaseBundleSerial = 500ULL;
		int OwnerVirtualThreadId = 0;
		bool ReplayAfterSuccessfulRetire = true;
		std::optional<MatrixTileFullPipelineBeforeExecuteCallback> BeforeExecute;
		std::optional<MatrixTileFullPipelineBeforeRetireCallback> BeforeRetire;
		std::optional<MatrixTileFullPipelineAfterRetireCallback> AfterRetire;
		std::optional<MatrixTileFullPipelineCaptureMutator> MutateCaptureBeforeRetire;

	public:
		unsigned long long getBaseBundleAddress() const;
		void setBaseBundleAddress(unsigned long long value);

		unsigned long long getBaseBundleSerial() const;
		void setBaseBundleSerial(unsigned long long value);

		int getOwnerVirtualThreadId() const;
		void setOwnerVirtualThreadId(int value);

		bool getReplayAfterSuccessfulRetire() const;
		void setReplayAfterSuccessfulRetire(bool value);

		std::optional<MatrixTileFullPipelineBeforeExecuteCallback> getBeforeExecute() const;
		void setBeforeExecute(const std::optional<MatrixTileFullPipelineBeforeExecuteCallback> &value);

		std::optional<MatrixTileFullPipelineBeforeRetireCallback> getBeforeRetire() const;
		void setBeforeRetire(const std::optional<MatrixTileFullPipelineBeforeRetireCallback> &value);

		std::optional<MatrixTileFullPipelineAfterRetireCallback> getAfterRetire() const;
		void setAfterRetire(const std::optional<MatrixTileFullPipelineAfterRetireCallback> &value);

		std::optional<MatrixTileFullPipelineCaptureMutator> getMutateCaptureBeforeRetire() const;
		void setMutateCaptureBeforeRetire(const std::optional<MatrixTileFullPipelineCaptureMutator> &value);
	};

	class MatrixTileFullPipelineReport final : public std::enable_shared_from_this<MatrixTileFullPipelineReport>
	{
	private:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly IReadOnlyList<MatrixTileFullPipelineStepEvidence> Steps;
		const std::vector<std::shared_ptr<MatrixTileFullPipelineStepEvidence>> Steps;

	public:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: public MatrixTileFullPipelineReport(IReadOnlyList<MatrixTileFullPipelineStepEvidence> steps)
		MatrixTileFullPipelineReport(const std::vector<std::shared_ptr<MatrixTileFullPipelineStepEvidence>> &steps);

		const std::vector<std::shared_ptr<MatrixTileFullPipelineStepEvidence>> getSteps() const;

		int getRuntimeInstructionCount() const;

		int getRetirePublicationCount() const;

		int getReplayRoundTripCount() const;

		int getFailClosedRejectionCount() const;
	};

	class MatrixTileFullPipelineStepEvidence final : public std::enable_shared_from_this<MatrixTileFullPipelineStepEvidence>
	{
	private:
		int BundleIndex = 0;
		int SlotIndex = 0;
		int ScheduledLaneIndex = -1;
		unsigned long long BundleAddress = 0;
		unsigned long long BundleSerial = 0;
		InstructionsEnum Opcode = static_cast<InstructionsEnum>(0);
		bool FetchObserved = false;
		bool DecodeObserved = false;
		bool ScheduleObserved = false;
		bool ExecuteObserved = false;
		bool RetireObserved = false;
		bool ReplayRollbackObserved = false;
		std::shared_ptr<InstructionSlotMetadata> SourceSlotMetadata;
		std::shared_ptr<InstructionSlotMetadata> DecodedSlotMetadata;
		MatrixTileIrProjectionFaultKind ProjectionFaultKind = static_cast<MatrixTileIrProjectionFaultKind>(0);
		MatrixTileProjectedOperationKind OperationKind = static_cast<MatrixTileProjectedOperationKind>(0);
		MatrixTileRuntimeResourceClass RuntimeResourceClass = static_cast<MatrixTileRuntimeResourceClass>(0);
		SlotClass RequiredSlotClass = static_cast<SlotClass>(0);
		unsigned char SchedulerLaneMask = 0;
		std::shared_ptr<MatrixTileMicroOpDependencyMetadata> DependencyMetadata;
		std::optional<std::shared_ptr<MatrixTileExecutionCaptureRecord>> Capture;
		std::optional<std::shared_ptr<MatrixTileRetireOutcome>> RetireOutcome;
		std::optional<std::shared_ptr<MatrixTileRollbackOutcome>> RollbackOutcome;
		std::optional<std::shared_ptr<MatrixTileRetireOutcome>> ReplayOutcome;
		std::wstring FailureMessage;

	public:
		int getBundleIndex() const;
		void setBundleIndex(int value);

		int getSlotIndex() const;
		void setSlotIndex(int value);

		int getScheduledLaneIndex() const;
		void setScheduledLaneIndex(int value);

		unsigned long long getBundleAddress() const;
		void setBundleAddress(unsigned long long value);

		unsigned long long getBundleSerial() const;
		void setBundleSerial(unsigned long long value);

		InstructionsEnum getOpcode() const;
		void setOpcode(InstructionsEnum value);

		bool getFetchObserved() const;
		void setFetchObserved(bool value);

		bool getDecodeObserved() const;
		void setDecodeObserved(bool value);

		bool getScheduleObserved() const;
		void setScheduleObserved(bool value);

		bool getExecuteObserved() const;
		void setExecuteObserved(bool value);

		bool getRetireObserved() const;
		void setRetireObserved(bool value);

		bool getReplayRollbackObserved() const;
		void setReplayRollbackObserved(bool value);

		std::shared_ptr<InstructionSlotMetadata> getSourceSlotMetadata() const;
		void setSourceSlotMetadata(std::shared_ptr<InstructionSlotMetadata> value);

		std::shared_ptr<InstructionSlotMetadata> getDecodedSlotMetadata() const;
		void setDecodedSlotMetadata(std::shared_ptr<InstructionSlotMetadata> value);

		MatrixTileIrProjectionFaultKind getProjectionFaultKind() const;
		void setProjectionFaultKind(MatrixTileIrProjectionFaultKind value);

		MatrixTileProjectedOperationKind getOperationKind() const;
		void setOperationKind(MatrixTileProjectedOperationKind value);

		MatrixTileRuntimeResourceClass getRuntimeResourceClass() const;
		void setRuntimeResourceClass(MatrixTileRuntimeResourceClass value);

		SlotClass getRequiredSlotClass() const;
		void setRequiredSlotClass(SlotClass value);

		unsigned char getSchedulerLaneMask() const;
		void setSchedulerLaneMask(unsigned char value);

		std::shared_ptr<MatrixTileMicroOpDependencyMetadata> getDependencyMetadata() const;
		void setDependencyMetadata(std::shared_ptr<MatrixTileMicroOpDependencyMetadata> value);

		bool getSidebandPreserved() const;

		std::optional<std::shared_ptr<MatrixTileExecutionCaptureRecord>> getCapture() const;
		void setCapture(const std::optional<std::shared_ptr<MatrixTileExecutionCaptureRecord>> &value);

		std::optional<std::shared_ptr<MatrixTileRetireOutcome>> getRetireOutcome() const;
		void setRetireOutcome(const std::optional<std::shared_ptr<MatrixTileRetireOutcome>> &value);

		std::optional<std::shared_ptr<MatrixTileRollbackOutcome>> getRollbackOutcome() const;
		void setRollbackOutcome(const std::optional<std::shared_ptr<MatrixTileRollbackOutcome>> &value);

		std::optional<std::shared_ptr<MatrixTileRetireOutcome>> getReplayOutcome() const;
		void setReplayOutcome(const std::optional<std::shared_ptr<MatrixTileRetireOutcome>> &value);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public string? FailureMessage {get;set;}
		std::wstring getFailureMessage() const;
		void setFailureMessage(const std::wstring &value);

		bool getFailClosedRejected() const;
	};

	class MatrixTileFullPipelineHarness final
	{
	public:
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: public static MatrixTileFullPipelineReport RunCompilerLoweredBundlesForTesting(ref Processor.CPU_Core core, IReadOnlyList<VLIW_Bundle> loweredBundles, IReadOnlyList<VliwBundleAnnotations> loweredBundleAnnotations, MatrixTileFullPipelineHarnessOptions? options = null)
		static std::shared_ptr<MatrixTileFullPipelineReport> RunCompilerLoweredBundlesForTesting(std::shared_ptr<Processor::CPU_Core> &core, const std::vector<VLIW_Bundle> &loweredBundles, const std::vector<std::shared_ptr<VliwBundleAnnotations>> &loweredBundleAnnotations, std::shared_ptr<MatrixTileFullPipelineHarnessOptions> options = nullptr);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal static MicroOp?[] BuildCanonicalMatrixTileCarrierBundle(DecodedInstructionBundle decodedBundle)
		static std::vector<std::shared_ptr<MicroOp>> BuildCanonicalMatrixTileCarrierBundle(std::shared_ptr<DecodedInstructionBundle> decodedBundle);

	private:
		static std::vector<VLIW_Instruction> FetchBundleSlots(VLIW_Bundle bundle);

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private static int FindScheduledLane(IReadOnlyList<MicroOp> scheduledBundle, MatrixTileMicroOp matrixTileMicroOp)
		static int FindScheduledLane(const std::vector<std::shared_ptr<MicroOp>> &scheduledBundle, std::shared_ptr<MatrixTileMicroOp> matrixTileMicroOp);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private static bool ValidateCompilerMatrixTileSidebandShape(InstructionsEnum opcode, InstructionSlotMetadata metadata, out string? failureMessage)
		static bool ValidateCompilerMatrixTileSidebandShape(InstructionsEnum opcode, std::shared_ptr<InstructionSlotMetadata> metadata, std::wstring &failureMessage);
	};
}
