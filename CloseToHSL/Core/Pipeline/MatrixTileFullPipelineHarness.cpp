#include "MatrixTileFullPipelineHarness.h"
#include "../Frontend/Decode/VliwDecoderV4Bridge/VliwDecoderV4.h"
#include "Assist/InterCore/YAKSys_Hybrid_CPU.Core.MicroOpScheduler.h"
#include "../Decoder/CanonicalDecodedContracts.h"
#include "../Decoder/DecodedInstruction.h"
#include "../Contracts/CompilerTransport/InstructionSlotMetadata.h"
#include "Metadata/BundleMetadata.h"
#include "MicroOps/IR/InstructionIR.h"
#include "../ISA/Instructions/NonVmx/Lanes00_03Vector/MatrixTile/MatrixTileRuntimeOwnedVlmRows.h"
#include "../ISA/Instructions/NonVmx/Lanes00_03Vector/MatrixTile/MatrixTileReplayRollbackAbi.h"
#include "../ISA/Instructions/NonVmx/Lanes00_03Vector/MatrixTile/MatrixTileRetirePublicationAbi.h"
#include "../Diagnostics/DecodeProjectionFaultException.h"
#include <typeinfo>
#include <codecvt>
#include <locale>

using namespace HybridCPU_ISE::Arch;
using namespace YAKSys_Hybrid_CPU::Arch;
using namespace YAKSys_Hybrid_CPU::CloseToHSL::Core::ISA::Instructions::NonVmx::Lanes00_03Vector::MatrixTile;
using namespace YAKSys_Hybrid_CPU::Core::Decoder;
using namespace YAKSys_Hybrid_CPU::Core::Pipeline::MicroOps;

namespace YAKSys_Hybrid_CPU::Core
{
	namespace
	{
		std::wstring MatrixTileOpcodeName(InstructionsEnum opcode)
		{
		switch (opcode)
		{
		case InstructionsEnum::MTILE_LOAD: return L"MTILE_LOAD";
		case InstructionsEnum::MTILE_STORE: return L"MTILE_STORE";
		case InstructionsEnum::MTILE_MACC: return L"MTILE_MACC";
		case InstructionsEnum::MTRANSPOSE: return L"MTRANSPOSE";
		default: return std::to_wstring(static_cast<unsigned int>(opcode));
		}
		}

		std::wstring ExceptionMessage(const std::exception &exception)
		{
			return std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t>().from_bytes(exception.what());
		}
	}

	unsigned long long MatrixTileFullPipelineHarnessOptions::getBaseBundleAddress() const
	{
		return BaseBundleAddress;
	}

	void MatrixTileFullPipelineHarnessOptions::setBaseBundleAddress(unsigned long long value)
	{
		BaseBundleAddress = value;
	}

	unsigned long long MatrixTileFullPipelineHarnessOptions::getBaseBundleSerial() const
	{
		return BaseBundleSerial;
	}

	void MatrixTileFullPipelineHarnessOptions::setBaseBundleSerial(unsigned long long value)
	{
		BaseBundleSerial = value;
	}

	int MatrixTileFullPipelineHarnessOptions::getOwnerVirtualThreadId() const
	{
		return OwnerVirtualThreadId;
	}

	void MatrixTileFullPipelineHarnessOptions::setOwnerVirtualThreadId(int value)
	{
		OwnerVirtualThreadId = value;
	}

	bool MatrixTileFullPipelineHarnessOptions::getReplayAfterSuccessfulRetire() const
	{
		return ReplayAfterSuccessfulRetire;
	}

	void MatrixTileFullPipelineHarnessOptions::setReplayAfterSuccessfulRetire(bool value)
	{
		ReplayAfterSuccessfulRetire = value;
	}

	std::optional<MatrixTileFullPipelineBeforeExecuteCallback> MatrixTileFullPipelineHarnessOptions::getBeforeExecute() const
	{
		return BeforeExecute;
	}

	void MatrixTileFullPipelineHarnessOptions::setBeforeExecute(const std::optional<MatrixTileFullPipelineBeforeExecuteCallback> &value)
	{
		BeforeExecute = value;
	}

	std::optional<MatrixTileFullPipelineBeforeRetireCallback> MatrixTileFullPipelineHarnessOptions::getBeforeRetire() const
	{
		return BeforeRetire;
	}

	void MatrixTileFullPipelineHarnessOptions::setBeforeRetire(const std::optional<MatrixTileFullPipelineBeforeRetireCallback> &value)
	{
		BeforeRetire = value;
	}

	std::optional<MatrixTileFullPipelineAfterRetireCallback> MatrixTileFullPipelineHarnessOptions::getAfterRetire() const
	{
		return AfterRetire;
	}

	void MatrixTileFullPipelineHarnessOptions::setAfterRetire(const std::optional<MatrixTileFullPipelineAfterRetireCallback> &value)
	{
		AfterRetire = value;
	}

	std::optional<MatrixTileFullPipelineCaptureMutator> MatrixTileFullPipelineHarnessOptions::getMutateCaptureBeforeRetire() const
	{
		return MutateCaptureBeforeRetire;
	}

	void MatrixTileFullPipelineHarnessOptions::setMutateCaptureBeforeRetire(const std::optional<MatrixTileFullPipelineCaptureMutator> &value)
	{
		MutateCaptureBeforeRetire = value;
	}

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: public MatrixTileFullPipelineReport(IReadOnlyList<MatrixTileFullPipelineStepEvidence> steps)
	MatrixTileFullPipelineReport::MatrixTileFullPipelineReport(const std::vector<std::shared_ptr<MatrixTileFullPipelineStepEvidence>> &steps) : Steps(steps)
	{
	}

	const std::vector<std::shared_ptr<MatrixTileFullPipelineStepEvidence>> MatrixTileFullPipelineReport::getSteps() const
	{
		return Steps;
	}

	int MatrixTileFullPipelineReport::getRuntimeInstructionCount() const
	{
		int count = 0;
		for (auto step : getSteps())
		{
			if (step->getExecuteObserved())
			{
				count++;
			}
		}

		return count;
	}

	int MatrixTileFullPipelineReport::getRetirePublicationCount() const
	{
		int count = 0;
		for (auto step : getSteps())
		{
			const auto retireOutcome = step->getRetireOutcome();
			if (retireOutcome && retireOutcome.value() && retireOutcome.value()->IsSuccess)
			{
				count++;
			}
		}

		return count;
	}

	int MatrixTileFullPipelineReport::getReplayRoundTripCount() const
	{
		int count = 0;
		for (auto step : getSteps())
		{
			if (step->getRollbackOutcome() && step->getReplayOutcome())
			{
				count++;
			}
		}

		return count;
	}

	int MatrixTileFullPipelineReport::getFailClosedRejectionCount() const
	{
		int count = 0;
		for (auto step : getSteps())
		{
			if (step->getFailClosedRejected())
			{
				count++;
			}
		}

		return count;
	}

	int MatrixTileFullPipelineStepEvidence::getBundleIndex() const
	{
		return BundleIndex;
	}

	void MatrixTileFullPipelineStepEvidence::setBundleIndex(int value)
	{
		BundleIndex = value;
	}

	int MatrixTileFullPipelineStepEvidence::getSlotIndex() const
	{
		return SlotIndex;
	}

	void MatrixTileFullPipelineStepEvidence::setSlotIndex(int value)
	{
		SlotIndex = value;
	}

	int MatrixTileFullPipelineStepEvidence::getScheduledLaneIndex() const
	{
		return ScheduledLaneIndex;
	}

	void MatrixTileFullPipelineStepEvidence::setScheduledLaneIndex(int value)
	{
		ScheduledLaneIndex = value;
	}

	unsigned long long MatrixTileFullPipelineStepEvidence::getBundleAddress() const
	{
		return BundleAddress;
	}

	void MatrixTileFullPipelineStepEvidence::setBundleAddress(unsigned long long value)
	{
		BundleAddress = value;
	}

	unsigned long long MatrixTileFullPipelineStepEvidence::getBundleSerial() const
	{
		return BundleSerial;
	}

	void MatrixTileFullPipelineStepEvidence::setBundleSerial(unsigned long long value)
	{
		BundleSerial = value;
	}

	InstructionsEnum MatrixTileFullPipelineStepEvidence::getOpcode() const
	{
		return Opcode;
	}

	void MatrixTileFullPipelineStepEvidence::setOpcode(InstructionsEnum value)
	{
		Opcode = value;
	}

	bool MatrixTileFullPipelineStepEvidence::getFetchObserved() const
	{
		return FetchObserved;
	}

	void MatrixTileFullPipelineStepEvidence::setFetchObserved(bool value)
	{
		FetchObserved = value;
	}

	bool MatrixTileFullPipelineStepEvidence::getDecodeObserved() const
	{
		return DecodeObserved;
	}

	void MatrixTileFullPipelineStepEvidence::setDecodeObserved(bool value)
	{
		DecodeObserved = value;
	}

	bool MatrixTileFullPipelineStepEvidence::getScheduleObserved() const
	{
		return ScheduleObserved;
	}

	void MatrixTileFullPipelineStepEvidence::setScheduleObserved(bool value)
	{
		ScheduleObserved = value;
	}

	bool MatrixTileFullPipelineStepEvidence::getExecuteObserved() const
	{
		return ExecuteObserved;
	}

	void MatrixTileFullPipelineStepEvidence::setExecuteObserved(bool value)
	{
		ExecuteObserved = value;
	}

	bool MatrixTileFullPipelineStepEvidence::getRetireObserved() const
	{
		return RetireObserved;
	}

	void MatrixTileFullPipelineStepEvidence::setRetireObserved(bool value)
	{
		RetireObserved = value;
	}

	bool MatrixTileFullPipelineStepEvidence::getReplayRollbackObserved() const
	{
		return ReplayRollbackObserved;
	}

	void MatrixTileFullPipelineStepEvidence::setReplayRollbackObserved(bool value)
	{
		ReplayRollbackObserved = value;
	}

	std::shared_ptr<InstructionSlotMetadata> MatrixTileFullPipelineStepEvidence::getSourceSlotMetadata() const
	{
		return SourceSlotMetadata;
	}

	void MatrixTileFullPipelineStepEvidence::setSourceSlotMetadata(std::shared_ptr<InstructionSlotMetadata> value)
	{
		SourceSlotMetadata = value;
	}

	std::shared_ptr<InstructionSlotMetadata> MatrixTileFullPipelineStepEvidence::getDecodedSlotMetadata() const
	{
		return DecodedSlotMetadata;
	}

	void MatrixTileFullPipelineStepEvidence::setDecodedSlotMetadata(std::shared_ptr<InstructionSlotMetadata> value)
	{
		DecodedSlotMetadata = value;
	}

	MatrixTileIrProjectionFaultKind MatrixTileFullPipelineStepEvidence::getProjectionFaultKind() const
	{
		return ProjectionFaultKind;
	}

	void MatrixTileFullPipelineStepEvidence::setProjectionFaultKind(MatrixTileIrProjectionFaultKind value)
	{
		ProjectionFaultKind = value;
	}

	MatrixTileProjectedOperationKind MatrixTileFullPipelineStepEvidence::getOperationKind() const
	{
		return OperationKind;
	}

	void MatrixTileFullPipelineStepEvidence::setOperationKind(MatrixTileProjectedOperationKind value)
	{
		OperationKind = value;
	}

	MatrixTileRuntimeResourceClass MatrixTileFullPipelineStepEvidence::getRuntimeResourceClass() const
	{
		return RuntimeResourceClass;
	}

	void MatrixTileFullPipelineStepEvidence::setRuntimeResourceClass(MatrixTileRuntimeResourceClass value)
	{
		RuntimeResourceClass = value;
	}

	SlotClass MatrixTileFullPipelineStepEvidence::getRequiredSlotClass() const
	{
		return RequiredSlotClass;
	}

	void MatrixTileFullPipelineStepEvidence::setRequiredSlotClass(SlotClass value)
	{
		RequiredSlotClass = value;
	}

	unsigned char MatrixTileFullPipelineStepEvidence::getSchedulerLaneMask() const
	{
		return SchedulerLaneMask;
	}

	void MatrixTileFullPipelineStepEvidence::setSchedulerLaneMask(unsigned char value)
	{
		SchedulerLaneMask = value;
	}

	std::shared_ptr<MatrixTileMicroOpDependencyMetadata> MatrixTileFullPipelineStepEvidence::getDependencyMetadata() const
	{
		return DependencyMetadata;
	}

	void MatrixTileFullPipelineStepEvidence::setDependencyMetadata(std::shared_ptr<MatrixTileMicroOpDependencyMetadata> value)
	{
		DependencyMetadata = value;
	}

	bool MatrixTileFullPipelineStepEvidence::getSidebandPreserved() const
	{
		return getSourceSlotMetadata()->MatrixTileNumericPolicy == getDecodedSlotMetadata()->MatrixTileNumericPolicy && getSourceSlotMetadata()->MatrixTileLayoutPolicy == getDecodedSlotMetadata()->MatrixTileLayoutPolicy;
	}

	std::optional<std::shared_ptr<MatrixTileExecutionCaptureRecord>> MatrixTileFullPipelineStepEvidence::getCapture() const
	{
		return Capture;
	}

	void MatrixTileFullPipelineStepEvidence::setCapture(const std::optional<std::shared_ptr<MatrixTileExecutionCaptureRecord>> &value)
	{
		Capture = value;
	}

	std::optional<std::shared_ptr<MatrixTileRetireOutcome>> MatrixTileFullPipelineStepEvidence::getRetireOutcome() const
	{
		return RetireOutcome;
	}

	void MatrixTileFullPipelineStepEvidence::setRetireOutcome(const std::optional<std::shared_ptr<MatrixTileRetireOutcome>> &value)
	{
		RetireOutcome = value;
	}

	std::optional<std::shared_ptr<MatrixTileRollbackOutcome>> MatrixTileFullPipelineStepEvidence::getRollbackOutcome() const
	{
		return RollbackOutcome;
	}

	void MatrixTileFullPipelineStepEvidence::setRollbackOutcome(const std::optional<std::shared_ptr<MatrixTileRollbackOutcome>> &value)
	{
		RollbackOutcome = value;
	}

	std::optional<std::shared_ptr<MatrixTileRetireOutcome>> MatrixTileFullPipelineStepEvidence::getReplayOutcome() const
	{
		return ReplayOutcome;
	}

	void MatrixTileFullPipelineStepEvidence::setReplayOutcome(const std::optional<std::shared_ptr<MatrixTileRetireOutcome>> &value)
	{
		ReplayOutcome = value;
	}

	std::wstring MatrixTileFullPipelineStepEvidence::getFailureMessage() const
	{
		return FailureMessage;
	}

	void MatrixTileFullPipelineStepEvidence::setFailureMessage(const std::wstring &value)
	{
		FailureMessage = value;
	}

	bool MatrixTileFullPipelineStepEvidence::getFailClosedRejected() const
	{
		return getFailureMessage() != L"";
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: public static MatrixTileFullPipelineReport RunCompilerLoweredBundlesForTesting(ref Processor.CPU_Core core, IReadOnlyList<VLIW_Bundle> loweredBundles, IReadOnlyList<VliwBundleAnnotations> loweredBundleAnnotations, MatrixTileFullPipelineHarnessOptions? options = null)
	std::shared_ptr<MatrixTileFullPipelineReport> MatrixTileFullPipelineHarness::RunCompilerLoweredBundlesForTesting(std::shared_ptr<Processor::CPU_Core> &core, const std::vector<VLIW_Bundle> &loweredBundles, const std::vector<std::shared_ptr<VliwBundleAnnotations>> &loweredBundleAnnotations, std::shared_ptr<MatrixTileFullPipelineHarnessOptions> options)
	{
		// These collections are non-null C++ references; individual annotation entries remain nullable.
		if (loweredBundles.size() != loweredBundleAnnotations.size())
		{
// C# TO C++ CONVERTER TASK: This exception's constructor requires only one argument:
// ORIGINAL LINE: throw new ArgumentException("Lowered bundle annotation count must match lowered bundle count.", nameof(loweredBundleAnnotations));
			throw std::invalid_argument("Lowered bundle annotation count must match lowered bundle count.");
		}

		options = (options != nullptr) ? options : std::make_shared<MatrixTileFullPipelineHarnessOptions>();
		std::shared_ptr<VliwDecoderV4> decoder = std::make_shared<VliwDecoderV4>();
		std::shared_ptr<MicroOpScheduler> scheduler = std::make_shared<MicroOpScheduler>();
		scheduler->setTypedSlotEnabled(true);
		std::vector<std::shared_ptr<MatrixTileFullPipelineStepEvidence>> steps;

		for (int bundleIndex = 0; bundleIndex < loweredBundles.size(); bundleIndex++)
		{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'checked' in this context:
// ORIGINAL LINE: ulong bundleAddress = checked(options.BaseBundleAddress + ((ulong)bundleIndex * 0x20UL));
			unsigned long long bundleAddress = options->getBaseBundleAddress() + (static_cast<unsigned long long>(bundleIndex) * 0x20ULL);
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'checked' in this context:
// ORIGINAL LINE: ulong bundleSerial = checked(options.BaseBundleSerial + (ulong)bundleIndex);
			unsigned long long bundleSerial = options->getBaseBundleSerial() + static_cast<unsigned long long>(bundleIndex);
			VLIW_Bundle bundle = loweredBundles[bundleIndex];
			std::shared_ptr<VliwBundleAnnotations> annotations = (loweredBundleAnnotations[bundleIndex] != nullptr) ? loweredBundleAnnotations[bundleIndex] : VliwBundleAnnotations::getEmpty();
			std::vector<VLIW_Instruction> rawSlots = FetchBundleSlots(bundle);

			std::shared_ptr<DecodedInstructionBundle> decodedBundle = decoder->DecodeInstructionBundle(rawSlots, annotations, bundleAddress, bundleSerial);
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: MicroOp?[] carrierBundle = BuildCanonicalMatrixTileCarrierBundle(decodedBundle);
			std::vector<std::shared_ptr<MicroOp>> carrierBundle = BuildCanonicalMatrixTileCarrierBundle(decodedBundle);
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'checked' in this context:
// ORIGINAL LINE: MicroOp[] scheduledBundle = scheduler.PackBundleIntraCoreSmt(carrierBundle, options.OwnerVirtualThreadId, checked((int)core.CoreID), eligibleVirtualThreadMask: 0);
			std::vector<std::shared_ptr<MicroOp>> scheduledBundle = scheduler->PackBundleIntraCoreSmt(carrierBundle, options->getOwnerVirtualThreadId(), static_cast<int>(core->getCoreID()), 0, nullptr);

			for (int slotIndex = 0; slotIndex < BundleMetadata::BundleSlotCount; slotIndex++)
			{
				InstructionsEnum opcode = static_cast<InstructionsEnum>(rawSlots[slotIndex].getOpCode());
				if (!MatrixTileRuntimeOwnedVlmRows::IsMatrixTileOpcode(opcode))
				{
					continue;
				}

				std::shared_ptr<DecodedInstruction> decodedSlot = decodedBundle->GetDecodedSlot(slotIndex);
				std::shared_ptr<InstructionSlotMetadata> sourceMetadata;
				annotations->TryGetInstructionSlotMetadata(slotIndex, sourceMetadata);
				std::shared_ptr<MatrixTileFullPipelineStepEvidence> step = std::make_shared<MatrixTileFullPipelineStepEvidence>();
				step->setBundleIndex(bundleIndex);
				step->setSlotIndex(slotIndex);
				step->setBundleAddress(bundleAddress);
				step->setBundleSerial(bundleSerial);
				step->setOpcode(opcode);
				step->setFetchObserved(true);
				step->setDecodeObserved(decodedSlot->IsOccupied);
				step->setSourceSlotMetadata(sourceMetadata != nullptr ? sourceMetadata : InstructionSlotMetadata::getDefault());
				step->setDecodedSlotMetadata(decodedSlot->SlotMetadata);
				steps.push_back(step);

				if (!decodedSlot->IsOccupied)
				{
					step->setFailureMessage(L"MatrixTile opcode decoded as an empty slot.");
					continue;
				}

				std::wstring sidebandFailure;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: if (!ValidateCompilerMatrixTileSidebandShape(opcode, decodedSlot.SlotMetadata, out string? sidebandFailure))
				if (!ValidateCompilerMatrixTileSidebandShape(opcode, decodedSlot->SlotMetadata, sidebandFailure))
				{
					step->setFailureMessage(sidebandFailure);
					continue;
				}

				std::shared_ptr<InstructionIR> instruction = decodedSlot->RequireInstruction();
				const auto &projectionValue = instruction->MatrixTileProjection;
				if (!projectionValue || !projectionValue.value())
				{
					step->setFailureMessage(L"MatrixTile opcode did not publish a decoded MatrixTile projection.");
					continue;
				}
				auto projection = projectionValue.value();

				step->setProjectionFaultKind(projection->FaultKind);
				step->setOperationKind(projection->OperationKind);
				if (projection->FaultKind != MatrixTileIrProjectionFaultKind::None || !projection->IsRuntimeLegal)
				{
					const wchar_t *faultName = L"Unknown";
					switch (projection->FaultKind)
					{
					case MatrixTileIrProjectionFaultKind::None: faultName = L"None"; break;
					case MatrixTileIrProjectionFaultKind::NonMatrixTileOpcode: faultName = L"NonMatrixTileOpcode"; break;
					case MatrixTileIrProjectionFaultKind::MissingVectorPayload: faultName = L"MissingVectorPayload"; break;
					case MatrixTileIrProjectionFaultKind::UnsupportedDataType: faultName = L"UnsupportedDataType"; break;
					case MatrixTileIrProjectionFaultKind::InvalidShapeEncoding: faultName = L"InvalidShapeEncoding"; break;
					case MatrixTileIrProjectionFaultKind::MemoryShapeFault: faultName = L"MemoryShapeFault"; break;
					case MatrixTileIrProjectionFaultKind::MaccSemanticFault: faultName = L"MaccSemanticFault"; break;
					case MatrixTileIrProjectionFaultKind::TransposeSemanticFault: faultName = L"TransposeSemanticFault"; break;
					case MatrixTileIrProjectionFaultKind::RuntimeOwnedVlmFault: faultName = L"RuntimeOwnedVlmFault"; break;
					case MatrixTileIrProjectionFaultKind::NumericPolicyFault: faultName = L"NumericPolicyFault"; break;
					case MatrixTileIrProjectionFaultKind::LayoutPolicyFault: faultName = L"LayoutPolicyFault"; break;
				}
					step->setFailureMessage(std::wstring(L"Decoded MatrixTile projection failed closed before execution: ") + faultName + L".");
					continue;
				}

				std::shared_ptr<MatrixTileMicroOp> matrixTileMicroOp = std::dynamic_pointer_cast<MatrixTileMicroOp>(carrierBundle[slotIndex]);
				if (matrixTileMicroOp == nullptr)
				{
					std::wstring actualTypeName = L"null";
					if (carrierBundle[slotIndex])
					{
						std::string nativeTypeName = typeid(*carrierBundle[slotIndex]).name();
						const auto namespaceSeparator = nativeTypeName.find_last_of(':');
						if (namespaceSeparator != std::string::npos)
							nativeTypeName.erase(0, namespaceSeparator + 1);
						constexpr char classPrefix[] = "class ";
						if (nativeTypeName.compare(0, sizeof(classPrefix) - 1, classPrefix) == 0)
							nativeTypeName.erase(0, sizeof(classPrefix) - 1);
						actualTypeName.assign(nativeTypeName.begin(), nativeTypeName.end());
					}
					step->setFailureMessage(std::wstring(L"Decoded MatrixTile slot materialized as ") + actualTypeName + L" instead of MatrixTileMicroOp.");
					continue;
				}

				int scheduledLane = FindScheduledLane(scheduledBundle, matrixTileMicroOp);
				step->setScheduledLaneIndex(scheduledLane);
				step->setScheduleObserved(scheduledLane >= 0);
				step->setRuntimeResourceClass(matrixTileMicroOp->getRuntimeResourceClass());
				step->setRequiredSlotClass(matrixTileMicroOp->getPlacement().RequiredSlotClass);
				step->setSchedulerLaneMask(matrixTileMicroOp->getSchedulerLaneMask());
				step->setDependencyMetadata(matrixTileMicroOp->getDependencyMetadata());
				if (scheduledLane < 0)
				{
					step->setFailureMessage(L"Scheduler did not preserve the decoded MatrixTile carrier.");
					continue;
				}

				try
				{
					const auto beforeExecute = options->getBeforeExecute();
					if (beforeExecute && *beforeExecute)
					{
						(*beforeExecute)(core, step);
					}
					if (!matrixTileMicroOp->Execute(core))
					{
						step->setFailureMessage(L"MatrixTile execute returned false.");
						continue;
					}

					step->setExecuteObserved(true);
				auto captureResult = matrixTileMicroOp->getLastExecutionCapture();
				if (!captureResult)
					throw InvalidOperationException(MatrixTileOpcodeName(opcode) + L" did not publish a Phase09 execution capture.");
				std::shared_ptr<MatrixTileExecutionCaptureRecord> capture = *captureResult;
					step->setCapture(capture);

					const auto beforeRetire = options->getBeforeRetire();
					if (beforeRetire && *beforeRetire)
					{
						(*beforeRetire)(core, step, capture);
					}
					std::shared_ptr<MatrixTileExecutionCaptureRecord> retireCapture = capture;
					const auto mutateCapture = options->getMutateCaptureBeforeRetire();
					if (mutateCapture && *mutateCapture)
					{
						auto mutatedCapture = (*mutateCapture)(step, capture);
						if (mutatedCapture)
							retireCapture = std::move(mutatedCapture);
					}
					std::shared_ptr<MatrixTileRetireOutcome> retireOutcome = matrixTileMicroOp->RetireCapturedResult(core, retireCapture);
					if (!retireOutcome)
						throw InvalidOperationException(L"MatrixTile retire did not publish an outcome.");
					step->setRetireObserved(true);
					step->setRetireOutcome(retireOutcome);
					const auto afterRetire = options->getAfterRetire();
					if (afterRetire && *afterRetire)
					{
						(*afterRetire)(core, step, retireCapture, retireOutcome);
					}

					if (options->getReplayAfterSuccessfulRetire() && retireOutcome->IsSuccess)
					{
					std::shared_ptr<MatrixTileReplayRollbackJournal> journal = matrixTileMicroOp->getLastReplayRollbackJournal();
					if (!journal)
						throw InvalidOperationException(MatrixTileOpcodeName(opcode) + L" did not publish a retire-owned replay/rollback journal.");
						step->setRollbackOutcome(matrixTileMicroOp->RollbackRetiredResult(core, journal->getReplayIdentity()));
						step->setReplayOutcome(matrixTileMicroOp->ReplayRolledBackResult(core, journal->getReplayIdentity()));
						step->setReplayRollbackObserved(true);
					}
				}
				catch (const MatrixTileRetireValidationException &ex)
				{
					step->setFailureMessage(ExceptionMessage(ex));
				}
				catch (const MatrixTileRetireFaultException &ex)
				{
					step->setFailureMessage(ExceptionMessage(ex));
				}
				catch (const DecodeProjectionFaultException &ex)
				{
					step->setFailureMessage(ExceptionMessage(ex));
				}
			}
		}

		return std::make_shared<MatrixTileFullPipelineReport>(steps);
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal static MicroOp?[] BuildCanonicalMatrixTileCarrierBundle(DecodedInstructionBundle decodedBundle)
	std::vector<std::shared_ptr<MicroOp>> MatrixTileFullPipelineHarness::BuildCanonicalMatrixTileCarrierBundle(std::shared_ptr<DecodedInstructionBundle> decodedBundle)
	{
		if (!decodedBundle)
			throw ArgumentNullException("decodedBundle");
		std::shared_ptr<CanonicalBundle> canonicalBundle = decodedBundle->getCanonicalBundle();
		if (!canonicalBundle)
		throw InvalidOperationException(L"MatrixTile harness requires the public canonical decoder handoff.");
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: MicroOp?[] carriers = new MicroOp?[BundleMetadata.BundleSlotCount];
		std::vector<std::shared_ptr<MicroOp>> carriers(BundleMetadata::BundleSlotCount);
		for (int slotIndex = 0; slotIndex < carriers.size(); slotIndex++)
		{
			std::shared_ptr<DecodedInstruction> decodedSlot = decodedBundle->GetDecodedSlot(slotIndex);
			std::shared_ptr<CanonicalDecodedInstruction> canonicalSlot = canonicalBundle->GetSlot(slotIndex);
			if (!decodedSlot->IsOccupied)
			{
				if (canonicalSlot->IsOccupied || canonicalSlot->StaticBinding != nullptr)
				{
					throw InvalidOperationException(std::wstring(L"MatrixTile canonical handoff disagrees about empty slot ") + std::to_wstring(slotIndex) + L".");
				}

				continue;
			}

			if (!MatrixTileRuntimeOwnedVlmRows::IsMatrixTileOpcode(decodedSlot->Instruction->CanonicalOpcode->ToInstructionsEnum()))
			{
				throw InvalidOperationException(std::wstring(L"MatrixTile-only harness received non-MatrixTile occupied slot ") + std::to_wstring(slotIndex) + L".");
			}

			try
			{
				carriers[slotIndex] = MatrixTileIrProjectionAndMaterializer::MaterializeCanonicalCarrier(canonicalSlot, decodedSlot);
			}
			catch (const DecodeProjectionFaultException &e1)
			{
				// The harness records decoder-published projection faults in
				// its per-slot evidence below. No carrier crosses admission.
				carriers[slotIndex] = nullptr;
			}
		}

		return carriers;
	}

	std::vector<VLIW_Instruction> MatrixTileFullPipelineHarness::FetchBundleSlots(VLIW_Bundle bundle)
	{
		std::vector<VLIW_Instruction> rawSlots(BundleMetadata::BundleSlotCount);
		for (int slotIndex = 0; slotIndex < rawSlots.size(); slotIndex++)
		{
			rawSlots[slotIndex] = bundle.GetInstruction(slotIndex);
		}

		return rawSlots;
	}

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private static int FindScheduledLane(IReadOnlyList<MicroOp> scheduledBundle, MatrixTileMicroOp matrixTileMicroOp)
	int MatrixTileFullPipelineHarness::FindScheduledLane(const std::vector<std::shared_ptr<MicroOp>> &scheduledBundle, std::shared_ptr<MatrixTileMicroOp> matrixTileMicroOp)
	{
		for (int laneIndex = 0; laneIndex < scheduledBundle.size(); laneIndex++)
		{
			if (scheduledBundle[laneIndex] == matrixTileMicroOp)
			{
				return laneIndex;
			}
		}

		return -1;
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private static bool ValidateCompilerMatrixTileSidebandShape(InstructionsEnum opcode, InstructionSlotMetadata metadata, out string? failureMessage)
	bool MatrixTileFullPipelineHarness::ValidateCompilerMatrixTileSidebandShape(InstructionsEnum opcode, std::shared_ptr<InstructionSlotMetadata> metadata, std::wstring &failureMessage)
	{
		failureMessage = L"";
		bool hasNumeric = metadata->MatrixTileNumericPolicy != nullptr;
		bool hasLayout = metadata->MatrixTileLayoutPolicy != nullptr;
		switch (opcode)
		{
			case InstructionsEnum::MTILE_LOAD:
			case InstructionsEnum::MTILE_STORE:
				if (hasNumeric || hasLayout)
				{
					failureMessage = MatrixTileOpcodeName(opcode) + L" compiler-lowered transport carried compute numeric/layout sideband authority.";
					return false;
				}

				return true;

			case InstructionsEnum::MTILE_MACC:
				if (!hasNumeric || !hasLayout)
				{
					failureMessage = L"MTILE_MACC compiler-lowered transport did not carry explicit runtime-owned numeric and layout sidebands.";
					return false;
				}

				return true;

			case InstructionsEnum::MTRANSPOSE:
				if (hasNumeric || !hasLayout)
				{
					failureMessage = L"MTRANSPOSE compiler-lowered transport must carry layout-only sideband authority.";
					return false;
				}

				return true;

			default:
				failureMessage = MatrixTileOpcodeName(opcode) + L" is not a MatrixTile compiler-positive opcode.";
				return false;
		}
	}
}
