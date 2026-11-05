#pragma once

#include "MatrixTileMemoryShapeAndFaultAbi.h"
#include "MatrixTileAccumulatorAndTransposePolicyAbi.h"
#include "MatrixTileRuntimeOwnedVlmRows.h"
#include "MatrixTileIrProjectionAndMaterializer.h"
#include "MatrixTileExecuteCaptureAbi.h"
#include "MatrixTileRetirePublicationAbi.h"
#include "MatrixTileReplayRollbackAbi.h"
#include "MatrixTilePositiveGoldenArtifactManifest.h"
#include "MatrixTileCompilerEmissionHandoffPackage.h"
#include "MatrixTileArchitecturalTileStateAndDescriptorAbi.h"
#include "MatrixTileStreamTransferAbi.h"
#include "MatrixTileNumericPolicyAbi.h"
#include "MatrixTileMaccArithmeticAbi.h"
#include "MatrixTileLayoutPolicyAbi.h"
#include "MatrixTilePolicyBoundIdentityAbi.h"
#include <string>
#include <vector>
#include <format>
#include <memory>
#include <utility>

namespace YAKSys_Hybrid_CPU::CloseToHSL::Core::ISA::Instructions::NonVmx::Lanes00_03Vector::MatrixTile
{

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct MatrixTileRuntimeIsaPackageRow(string Mnemonic, bool IsTileMemory, bool IsLoad, bool IsStore, bool IsMacc, bool IsTranspose, bool RequiresTileMemoryShapeFaultModel, bool RequiresAccumulatorTileAbi, bool RequiresTransposeTilePolicyAbi, bool RequiresRetireStagedPublication, bool RequiresRetireStagedCommit, string PrimaryBlockedGate);

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct MatrixTileRuntimeIsaClosureGateRow(string GateName, string EvidenceCode, bool IsExecutableClosure, bool BlocksCompilerUpdate, string Blocker);

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct MatrixTileRemainingRuntimeIsaTaskRow(string TaskName, string Status, bool BlocksExecutableClosure, bool BlocksCompilerUpdate, string RequiredDecision);

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct MatrixTileOpcodeAuthorityRow(string Mnemonic, ushort NumericOpcode, bool HasPackageOpcodeIdentityAuthority, string StatusCatalogAuthority, bool HasExecutableStatusCatalogPromotion, bool HasExecutableOpcodeAuthority, bool HasDescriptorAuthority, bool RequiresFutureAdr);

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct MatrixTilePhase01AuthorityDecisionRow(string Mnemonic, ushort NumericOpcode, string RetainedNumericOpcodeRole, string DescriptorAuthorityRole, string PackageFeatureRole, string MemoryOrTransposePolicyDecision, string StatusCatalogPromotionRule, bool HasProductionAuthoritySource, bool BlocksPhase02);

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct MatrixTilePhase02StateDescriptorDecisionRow(string OpenPoolItem, string RequiredAbiSurface, string Decision, string PrimaryBlocker, bool HasRuntimeOwnedAbi, bool HasCanonicalCarrier, bool BlocksPhase03);

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct MatrixTilePhase02InstructionDescriptorRequirementRow(string Mnemonic, string TileStateRequirement, string DescriptorRequirement, string DescriptorValidationDecision, bool HasDescriptorRoundTripAbi, bool HasReservedDescriptorFailFastTests, bool KeepsExternalDescriptorsNonAuthority);

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct MatrixTilePhase03MemoryFaultDecisionRow(string Mnemonic, bool IsLoad, bool IsStore, string RequiredAbiSurface, string Decision, string PrimaryBlocker, bool HasDeterministicShapeValidation, bool HasReplayableFaultModel, bool HasRetireOwnedSideEffectCommit, bool KeepsEaFallbackNonAuthority);

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct MatrixTilePhase04SemanticAbiDecisionRow(string Mnemonic, string RequiredAbiSurface, string Decision, string PrimaryBlocker, bool HasShapeCompatibilityPolicy, bool HasElementTypePolicy, bool HasRetireReplayPolicy, bool KeepsFallbackNonAuthority);

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct MatrixTilePhase05VlmDecisionRow(string Mnemonic, string RequiredVlmSurface, string Decision, string PrimaryBlocker, bool HasRuntimeOwnedVlmRow, bool HasFeatureGate, bool HasElementWidthLegality, bool HasTileShapeLegality, bool HasMemoryLayoutLegality, bool HasReservedDisabledRow, bool KeepsMetadataNonAuthority, bool KeepsDecoderAdmissionFailClosed);

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct MatrixTilePhase06DecoderEncoderDecisionRow(string Mnemonic, string RequiredAbiSurface, string Decision, string PrimaryBlocker, bool HasCanonicalDecoderAcceptance, bool HasEncoderRoundTripAbi, bool HasDescriptorDecodeValidation, bool HasReservedFieldFaultBehavior, bool KeepsIllegalRowsBeforeIrMaterializer, bool KeepsCompilerAcceptanceNonEvidence);

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct MatrixTilePhase07IrMaterializerDecisionRow(string Mnemonic, string RequiredRuntimeSurface, string Decision, string PrimaryBlocker, bool HasInstructionIrTileDescriptorProjection, bool HasMemoryOperandProjection, bool HasAccumulatorOperandProjection, bool HasTransposePolicyProjection, bool HasRegistryEntry, bool HasMaterializerFactory, bool HasMaterializedTypedRuntimeObject, bool PreservesDescriptorValidationResults, bool KeepsCompilerIrNonAuthority);

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct MatrixTilePhase08MicroOpSchedulerDecisionRow(string Mnemonic, string RequiredRuntimeSurface, string Decision, string PrimaryBlocker, bool RequiresTileMemoryDependencyMetadata, bool RequiresTileRegisterDependencyMetadata, bool RequiresAccumulatorDependencyMetadata, bool HasTypedTileMicroOp, bool HasTileMemoryDependencyMetadata, bool HasTileRegisterDependencyMetadata, bool HasAccumulatorDependencyMetadata, bool HasSchedulerLaneBinding, bool HasIssueConstraints, bool HasCaptureBarriers, bool BlocksSchedulerMaterializerVlmBypass, bool KeepsVmxBackendFallbackNonAuthority);

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct MatrixTilePhase09ExecuteCaptureDecisionRow(string Mnemonic, string RequiredRuntimeSurface, string Decision, string PrimaryBlocker, bool RequiresTileLoadCaptureBuffer, bool RequiresTileStorePendingWriteBuffer, bool RequiresMaccCaptureResult, bool RequiresTransposeCaptureResult, bool RequiresDeterministicExceptionCapture, bool RequiresMemoryFaultCapture, bool RequiresTileStateReadSnapshot, bool RequiresAccumulatorReadSnapshot, bool HasExecutionCaptureSemantics, bool HasTileLoadCaptureBuffer, bool HasTileStorePendingWriteBuffer, bool HasMaccCaptureResult, bool HasTransposeCaptureResult, bool HasDeterministicExceptionCapture, bool HasMemoryFaultCapture, bool HasTileStateReadSnapshot, bool HasAccumulatorReadSnapshot, bool HasRetirePublication, bool HasReplayRollbackConformance, bool BlocksArchitecturalSideEffectsBeforeRetire, bool KeepsVmxBackendFallbackNonAuthority);

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct MatrixTilePhase10RetirePublicationDecisionRow(string Mnemonic, string RequiredRuntimeSurface, string Decision, string PrimaryBlocker, bool RequiresTileLoadRetirePublication, bool RequiresTileStoreRetireCommit, bool RequiresAccumulatorRetirePublication, bool RequiresTransposeRetirePublication, bool RequiresFaultRetirementPolicy, bool RequiresWritebackOwnership, bool RequiresSideEffectPublicationOrdering, bool RequiresArchitecturalStateVisibilityRules, bool HasRetirePublicationAndCommit, bool HasTileLoadRetirePublication, bool HasTileStoreRetireCommit, bool HasAccumulatorRetirePublication, bool HasTransposeRetirePublication, bool HasFaultRetirementPolicy, bool HasWritebackOwnership, bool HasSideEffectPublicationOrdering, bool HasArchitecturalStateVisibilityRules, bool BlocksExecuteCaptureToRetireBypass, bool KeepsHostOwnedEvidenceNonArchitectural, bool KeepsVmxBackendFallbackNonAuthority);

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct MatrixTilePhase11ReplayRollbackDecisionRow(string Mnemonic, string RequiredRuntimeSurface, string Decision, string PrimaryBlocker, bool RequiresDecodedInstructionReplayIdentity, bool RequiresTileDescriptorReplayIdentity, bool RequiresPendingTileWriteRollback, bool RequiresPendingMemoryStoreRollback, bool RequiresAccumulatorRollback, bool RequiresDeterministicReplayAfterMemoryFault, bool RequiresDeterministicReplayAfterDescriptorFault, bool RequiresLegalIllegalConformanceVectors, bool HasReplayRollbackConformance, bool HasDecodedInstructionReplayIdentity, bool HasTileDescriptorReplayIdentity, bool HasPendingTileWriteRollback, bool HasPendingMemoryStoreRollback, bool HasAccumulatorRollback, bool HasDeterministicReplayAfterMemoryFault, bool HasDeterministicReplayAfterDescriptorFault, bool HasLegalIllegalConformanceVectors, bool BlocksReplayWithoutRetirePublication, bool BlocksCaptureRecordIdentityBypass, bool KeepsVmxBackendFallbackNonAuthority);

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct MatrixTilePhase12GoldenArtifactDecisionRow(string Mnemonic, string RequiredArtifactSurface, string Decision, string NoFallbackDecision, string PrimaryBlocker, bool RequiresLegalDecodeEncodeRoundTripVectors, bool RequiresLegalIrMaterializerProjectionVectors, bool RequiresLegalExecuteRetireVectors, bool RequiresMemoryFaultVectors, bool RequiresDescriptorFaultVectors, bool RequiresAccumulatorVectors, bool RequiresTransposeVectors, bool RequiresReplayRollbackVectors, bool RequiresNegativeReservedVectors, bool RequiresRuntimeNoFallbackNoHiddenLoweringRegressionEvidence, bool HasPositiveExecutableGoldenArtifacts, bool HasRuntimeNoFallbackNoHiddenLoweringRegressionEvidence, bool HasLegalDecodeEncodeRoundTripVectors, bool HasLegalIrMaterializerProjectionVectors, bool HasLegalExecuteRetireVectors, bool HasMemoryFaultVectors, bool HasDescriptorFaultVectors, bool HasAccumulatorVectors, bool HasTransposeVectors, bool HasReplayRollbackVectors, bool HasNegativeReservedVectors, bool BlocksPositiveGoldenPublication, bool BlocksNoFallbackRegressionEvidenceBypass, bool KeepsVmxBackendFallbackNonAuthority, bool KeepsCompilerScopeClosed, bool KeepsCompilerHandoffBlocked);

	struct MatrixTileRuntimeIsaPackageRow final
	{
		std::wstring Mnemonic;
		bool IsTileMemory;
		bool IsLoad;
		bool IsStore;
		bool IsMacc;
		bool IsTranspose;
		bool RequiresTileMemoryShapeFaultModel;
		bool RequiresAccumulatorTileAbi;
		bool RequiresTransposeTilePolicyAbi;
		bool RequiresRetireStagedPublication;
		bool RequiresRetireStagedCommit;
		std::wstring PrimaryBlockedGate;
		MatrixTileRuntimeIsaPackageRow(std::wstring mnemonic, bool isTileMemory, bool isLoad, bool isStore, bool isMacc, bool isTranspose, bool requiresTileMemoryShapeFaultModel, bool requiresAccumulatorTileAbi, bool requiresTransposeTilePolicyAbi, bool requiresRetireStagedPublication, bool requiresRetireStagedCommit, std::wstring primaryBlockedGate) : Mnemonic(std::move(mnemonic)), IsTileMemory(isTileMemory), IsLoad(isLoad), IsStore(isStore), IsMacc(isMacc), IsTranspose(isTranspose), RequiresTileMemoryShapeFaultModel(requiresTileMemoryShapeFaultModel), RequiresAccumulatorTileAbi(requiresAccumulatorTileAbi), RequiresTransposeTilePolicyAbi(requiresTransposeTilePolicyAbi), RequiresRetireStagedPublication(requiresRetireStagedPublication), RequiresRetireStagedCommit(requiresRetireStagedCommit), PrimaryBlockedGate(std::move(primaryBlockedGate)) {}
	};

	struct MatrixTileRuntimeIsaClosureGateRow final
	{
		std::wstring GateName;
		std::wstring EvidenceCode;
		bool IsExecutableClosure;
		bool BlocksCompilerUpdate;
		std::wstring Blocker;
		MatrixTileRuntimeIsaClosureGateRow(std::wstring gateName, std::wstring evidenceCode, bool isExecutableClosure, bool blocksCompilerUpdate, std::wstring blocker) : GateName(std::move(gateName)), EvidenceCode(std::move(evidenceCode)), IsExecutableClosure(isExecutableClosure), BlocksCompilerUpdate(blocksCompilerUpdate), Blocker(std::move(blocker)) {}
	};

	struct MatrixTileRemainingRuntimeIsaTaskRow final
	{
		std::wstring TaskName;
		std::wstring Status;
		bool BlocksExecutableClosure;
		bool BlocksCompilerUpdate;
		std::wstring RequiredDecision;
		MatrixTileRemainingRuntimeIsaTaskRow(std::wstring taskName, std::wstring status, bool blocksExecutableClosure, bool blocksCompilerUpdate, std::wstring requiredDecision) : TaskName(std::move(taskName)), Status(std::move(status)), BlocksExecutableClosure(blocksExecutableClosure), BlocksCompilerUpdate(blocksCompilerUpdate), RequiredDecision(std::move(requiredDecision)) {}
	};

	struct MatrixTileOpcodeAuthorityRow final
	{
		std::wstring Mnemonic;
		unsigned short NumericOpcode;
		bool HasPackageOpcodeIdentityAuthority;
		std::wstring StatusCatalogAuthority;
		bool HasExecutableStatusCatalogPromotion;
		bool HasExecutableOpcodeAuthority;
		bool HasDescriptorAuthority;
		bool RequiresFutureAdr;
		MatrixTileOpcodeAuthorityRow(std::wstring mnemonic, unsigned short numericOpcode, bool hasPackageOpcodeIdentityAuthority, std::wstring statusCatalogAuthority, bool hasExecutableStatusCatalogPromotion, bool hasExecutableOpcodeAuthority, bool hasDescriptorAuthority, bool requiresFutureAdr) : Mnemonic(std::move(mnemonic)), NumericOpcode(numericOpcode), HasPackageOpcodeIdentityAuthority(hasPackageOpcodeIdentityAuthority), StatusCatalogAuthority(std::move(statusCatalogAuthority)), HasExecutableStatusCatalogPromotion(hasExecutableStatusCatalogPromotion), HasExecutableOpcodeAuthority(hasExecutableOpcodeAuthority), HasDescriptorAuthority(hasDescriptorAuthority), RequiresFutureAdr(requiresFutureAdr) {}
	};

	struct MatrixTilePhase01AuthorityDecisionRow final
	{
		std::wstring Mnemonic;
		unsigned short NumericOpcode;
		std::wstring RetainedNumericOpcodeRole;
		std::wstring DescriptorAuthorityRole;
		std::wstring PackageFeatureRole;
		std::wstring MemoryOrTransposePolicyDecision;
		std::wstring StatusCatalogPromotionRule;
		bool HasProductionAuthoritySource;
		bool BlocksPhase02;
		MatrixTilePhase01AuthorityDecisionRow(std::wstring mnemonic, unsigned short numericOpcode, std::wstring retainedNumericOpcodeRole, std::wstring descriptorAuthorityRole, std::wstring packageFeatureRole, std::wstring memoryOrTransposePolicyDecision, std::wstring statusCatalogPromotionRule, bool hasProductionAuthoritySource, bool blocksPhase02) : Mnemonic(std::move(mnemonic)), NumericOpcode(numericOpcode), RetainedNumericOpcodeRole(std::move(retainedNumericOpcodeRole)), DescriptorAuthorityRole(std::move(descriptorAuthorityRole)), PackageFeatureRole(std::move(packageFeatureRole)), MemoryOrTransposePolicyDecision(std::move(memoryOrTransposePolicyDecision)), StatusCatalogPromotionRule(std::move(statusCatalogPromotionRule)), HasProductionAuthoritySource(hasProductionAuthoritySource), BlocksPhase02(blocksPhase02) {}
	};

	struct MatrixTilePhase02StateDescriptorDecisionRow final
	{
		std::wstring OpenPoolItem;
		std::wstring RequiredAbiSurface;
		std::wstring Decision;
		std::wstring PrimaryBlocker;
		bool HasRuntimeOwnedAbi;
		bool HasCanonicalCarrier;
		bool BlocksPhase03;
		MatrixTilePhase02StateDescriptorDecisionRow(std::wstring openPoolItem, std::wstring requiredAbiSurface, std::wstring decision, std::wstring primaryBlocker, bool hasRuntimeOwnedAbi, bool hasCanonicalCarrier, bool blocksPhase03) : OpenPoolItem(std::move(openPoolItem)), RequiredAbiSurface(std::move(requiredAbiSurface)), Decision(std::move(decision)), PrimaryBlocker(std::move(primaryBlocker)), HasRuntimeOwnedAbi(hasRuntimeOwnedAbi), HasCanonicalCarrier(hasCanonicalCarrier), BlocksPhase03(blocksPhase03) {}
	};

	struct MatrixTilePhase02InstructionDescriptorRequirementRow final
	{
		std::wstring Mnemonic;
		std::wstring TileStateRequirement;
		std::wstring DescriptorRequirement;
		std::wstring DescriptorValidationDecision;
		bool HasDescriptorRoundTripAbi;
		bool HasReservedDescriptorFailFastTests;
		bool KeepsExternalDescriptorsNonAuthority;
		MatrixTilePhase02InstructionDescriptorRequirementRow(std::wstring mnemonic, std::wstring tileStateRequirement, std::wstring descriptorRequirement, std::wstring descriptorValidationDecision, bool hasDescriptorRoundTripAbi, bool hasReservedDescriptorFailFastTests, bool keepsExternalDescriptorsNonAuthority) : Mnemonic(std::move(mnemonic)), TileStateRequirement(std::move(tileStateRequirement)), DescriptorRequirement(std::move(descriptorRequirement)), DescriptorValidationDecision(std::move(descriptorValidationDecision)), HasDescriptorRoundTripAbi(hasDescriptorRoundTripAbi), HasReservedDescriptorFailFastTests(hasReservedDescriptorFailFastTests), KeepsExternalDescriptorsNonAuthority(keepsExternalDescriptorsNonAuthority) {}
	};

	struct MatrixTilePhase03MemoryFaultDecisionRow final
	{
		std::wstring Mnemonic;
		bool IsLoad;
		bool IsStore;
		std::wstring RequiredAbiSurface;
		std::wstring Decision;
		std::wstring PrimaryBlocker;
		bool HasDeterministicShapeValidation;
		bool HasReplayableFaultModel;
		bool HasRetireOwnedSideEffectCommit;
		bool KeepsEaFallbackNonAuthority;
		MatrixTilePhase03MemoryFaultDecisionRow(std::wstring mnemonic, bool isLoad, bool isStore, std::wstring requiredAbiSurface, std::wstring decision, std::wstring primaryBlocker, bool hasDeterministicShapeValidation, bool hasReplayableFaultModel, bool hasRetireOwnedSideEffectCommit, bool keepsEaFallbackNonAuthority) : Mnemonic(std::move(mnemonic)), IsLoad(isLoad), IsStore(isStore), RequiredAbiSurface(std::move(requiredAbiSurface)), Decision(std::move(decision)), PrimaryBlocker(std::move(primaryBlocker)), HasDeterministicShapeValidation(hasDeterministicShapeValidation), HasReplayableFaultModel(hasReplayableFaultModel), HasRetireOwnedSideEffectCommit(hasRetireOwnedSideEffectCommit), KeepsEaFallbackNonAuthority(keepsEaFallbackNonAuthority) {}
	};

	struct MatrixTilePhase04SemanticAbiDecisionRow final
	{
		std::wstring Mnemonic;
		std::wstring RequiredAbiSurface;
		std::wstring Decision;
		std::wstring PrimaryBlocker;
		bool HasShapeCompatibilityPolicy;
		bool HasElementTypePolicy;
		bool HasRetireReplayPolicy;
		bool KeepsFallbackNonAuthority;
		MatrixTilePhase04SemanticAbiDecisionRow(std::wstring mnemonic, std::wstring requiredAbiSurface, std::wstring decision, std::wstring primaryBlocker, bool hasShapeCompatibilityPolicy, bool hasElementTypePolicy, bool hasRetireReplayPolicy, bool keepsFallbackNonAuthority) : Mnemonic(std::move(mnemonic)), RequiredAbiSurface(std::move(requiredAbiSurface)), Decision(std::move(decision)), PrimaryBlocker(std::move(primaryBlocker)), HasShapeCompatibilityPolicy(hasShapeCompatibilityPolicy), HasElementTypePolicy(hasElementTypePolicy), HasRetireReplayPolicy(hasRetireReplayPolicy), KeepsFallbackNonAuthority(keepsFallbackNonAuthority) {}
	};

	struct MatrixTilePhase05VlmDecisionRow final
	{
		std::wstring Mnemonic;
		std::wstring RequiredVlmSurface;
		std::wstring Decision;
		std::wstring PrimaryBlocker;
		bool HasRuntimeOwnedVlmRow;
		bool HasFeatureGate;
		bool HasElementWidthLegality;
		bool HasTileShapeLegality;
		bool HasMemoryLayoutLegality;
		bool HasReservedDisabledRow;
		bool KeepsMetadataNonAuthority;
		bool KeepsDecoderAdmissionFailClosed;
		MatrixTilePhase05VlmDecisionRow(std::wstring mnemonic, std::wstring requiredVlmSurface, std::wstring decision, std::wstring primaryBlocker, bool hasRuntimeOwnedVlmRow, bool hasFeatureGate, bool hasElementWidthLegality, bool hasTileShapeLegality, bool hasMemoryLayoutLegality, bool hasReservedDisabledRow, bool keepsMetadataNonAuthority, bool keepsDecoderAdmissionFailClosed) : Mnemonic(std::move(mnemonic)), RequiredVlmSurface(std::move(requiredVlmSurface)), Decision(std::move(decision)), PrimaryBlocker(std::move(primaryBlocker)), HasRuntimeOwnedVlmRow(hasRuntimeOwnedVlmRow), HasFeatureGate(hasFeatureGate), HasElementWidthLegality(hasElementWidthLegality), HasTileShapeLegality(hasTileShapeLegality), HasMemoryLayoutLegality(hasMemoryLayoutLegality), HasReservedDisabledRow(hasReservedDisabledRow), KeepsMetadataNonAuthority(keepsMetadataNonAuthority), KeepsDecoderAdmissionFailClosed(keepsDecoderAdmissionFailClosed) {}
	};

	struct MatrixTilePhase06DecoderEncoderDecisionRow final
	{
		std::wstring Mnemonic;
		std::wstring RequiredAbiSurface;
		std::wstring Decision;
		std::wstring PrimaryBlocker;
		bool HasCanonicalDecoderAcceptance;
		bool HasEncoderRoundTripAbi;
		bool HasDescriptorDecodeValidation;
		bool HasReservedFieldFaultBehavior;
		bool KeepsIllegalRowsBeforeIrMaterializer;
		bool KeepsCompilerAcceptanceNonEvidence;
		MatrixTilePhase06DecoderEncoderDecisionRow(std::wstring mnemonic, std::wstring requiredAbiSurface, std::wstring decision, std::wstring primaryBlocker, bool hasCanonicalDecoderAcceptance, bool hasEncoderRoundTripAbi, bool hasDescriptorDecodeValidation, bool hasReservedFieldFaultBehavior, bool keepsIllegalRowsBeforeIrMaterializer, bool keepsCompilerAcceptanceNonEvidence) : Mnemonic(std::move(mnemonic)), RequiredAbiSurface(std::move(requiredAbiSurface)), Decision(std::move(decision)), PrimaryBlocker(std::move(primaryBlocker)), HasCanonicalDecoderAcceptance(hasCanonicalDecoderAcceptance), HasEncoderRoundTripAbi(hasEncoderRoundTripAbi), HasDescriptorDecodeValidation(hasDescriptorDecodeValidation), HasReservedFieldFaultBehavior(hasReservedFieldFaultBehavior), KeepsIllegalRowsBeforeIrMaterializer(keepsIllegalRowsBeforeIrMaterializer), KeepsCompilerAcceptanceNonEvidence(keepsCompilerAcceptanceNonEvidence) {}
	};

	struct MatrixTilePhase07IrMaterializerDecisionRow final
	{
		std::wstring Mnemonic;
		std::wstring RequiredRuntimeSurface;
		std::wstring Decision;
		std::wstring PrimaryBlocker;
		bool HasInstructionIrTileDescriptorProjection;
		bool HasMemoryOperandProjection;
		bool HasAccumulatorOperandProjection;
		bool HasTransposePolicyProjection;
		bool HasRegistryEntry;
		bool HasMaterializerFactory;
		bool HasMaterializedTypedRuntimeObject;
		bool PreservesDescriptorValidationResults;
		bool KeepsCompilerIrNonAuthority;
		MatrixTilePhase07IrMaterializerDecisionRow(std::wstring mnemonic, std::wstring requiredRuntimeSurface, std::wstring decision, std::wstring primaryBlocker, bool hasInstructionIrTileDescriptorProjection, bool hasMemoryOperandProjection, bool hasAccumulatorOperandProjection, bool hasTransposePolicyProjection, bool hasRegistryEntry, bool hasMaterializerFactory, bool hasMaterializedTypedRuntimeObject, bool preservesDescriptorValidationResults, bool keepsCompilerIrNonAuthority) : Mnemonic(std::move(mnemonic)), RequiredRuntimeSurface(std::move(requiredRuntimeSurface)), Decision(std::move(decision)), PrimaryBlocker(std::move(primaryBlocker)), HasInstructionIrTileDescriptorProjection(hasInstructionIrTileDescriptorProjection), HasMemoryOperandProjection(hasMemoryOperandProjection), HasAccumulatorOperandProjection(hasAccumulatorOperandProjection), HasTransposePolicyProjection(hasTransposePolicyProjection), HasRegistryEntry(hasRegistryEntry), HasMaterializerFactory(hasMaterializerFactory), HasMaterializedTypedRuntimeObject(hasMaterializedTypedRuntimeObject), PreservesDescriptorValidationResults(preservesDescriptorValidationResults), KeepsCompilerIrNonAuthority(keepsCompilerIrNonAuthority) {}
	};

	struct MatrixTilePhase08MicroOpSchedulerDecisionRow final
	{
		std::wstring Mnemonic;
		std::wstring RequiredRuntimeSurface;
		std::wstring Decision;
		std::wstring PrimaryBlocker;
		bool RequiresTileMemoryDependencyMetadata;
		bool RequiresTileRegisterDependencyMetadata;
		bool RequiresAccumulatorDependencyMetadata;
		bool HasTypedTileMicroOp;
		bool HasTileMemoryDependencyMetadata;
		bool HasTileRegisterDependencyMetadata;
		bool HasAccumulatorDependencyMetadata;
		bool HasSchedulerLaneBinding;
		bool HasIssueConstraints;
		bool HasCaptureBarriers;
		bool BlocksSchedulerMaterializerVlmBypass;
		bool KeepsVmxBackendFallbackNonAuthority;
		MatrixTilePhase08MicroOpSchedulerDecisionRow(std::wstring mnemonic, std::wstring requiredRuntimeSurface, std::wstring decision, std::wstring primaryBlocker, bool requiresTileMemoryDependencyMetadata, bool requiresTileRegisterDependencyMetadata, bool requiresAccumulatorDependencyMetadata, bool hasTypedTileMicroOp, bool hasTileMemoryDependencyMetadata, bool hasTileRegisterDependencyMetadata, bool hasAccumulatorDependencyMetadata, bool hasSchedulerLaneBinding, bool hasIssueConstraints, bool hasCaptureBarriers, bool blocksSchedulerMaterializerVlmBypass, bool keepsVmxBackendFallbackNonAuthority) : Mnemonic(std::move(mnemonic)), RequiredRuntimeSurface(std::move(requiredRuntimeSurface)), Decision(std::move(decision)), PrimaryBlocker(std::move(primaryBlocker)), RequiresTileMemoryDependencyMetadata(requiresTileMemoryDependencyMetadata), RequiresTileRegisterDependencyMetadata(requiresTileRegisterDependencyMetadata), RequiresAccumulatorDependencyMetadata(requiresAccumulatorDependencyMetadata), HasTypedTileMicroOp(hasTypedTileMicroOp), HasTileMemoryDependencyMetadata(hasTileMemoryDependencyMetadata), HasTileRegisterDependencyMetadata(hasTileRegisterDependencyMetadata), HasAccumulatorDependencyMetadata(hasAccumulatorDependencyMetadata), HasSchedulerLaneBinding(hasSchedulerLaneBinding), HasIssueConstraints(hasIssueConstraints), HasCaptureBarriers(hasCaptureBarriers), BlocksSchedulerMaterializerVlmBypass(blocksSchedulerMaterializerVlmBypass), KeepsVmxBackendFallbackNonAuthority(keepsVmxBackendFallbackNonAuthority) {}
	};

	struct MatrixTilePhase09ExecuteCaptureDecisionRow final
	{
		std::wstring Mnemonic;
		std::wstring RequiredRuntimeSurface;
		std::wstring Decision;
		std::wstring PrimaryBlocker;
		bool RequiresTileLoadCaptureBuffer;
		bool RequiresTileStorePendingWriteBuffer;
		bool RequiresMaccCaptureResult;
		bool RequiresTransposeCaptureResult;
		bool RequiresDeterministicExceptionCapture;
		bool RequiresMemoryFaultCapture;
		bool RequiresTileStateReadSnapshot;
		bool RequiresAccumulatorReadSnapshot;
		bool HasExecutionCaptureSemantics;
		bool HasTileLoadCaptureBuffer;
		bool HasTileStorePendingWriteBuffer;
		bool HasMaccCaptureResult;
		bool HasTransposeCaptureResult;
		bool HasDeterministicExceptionCapture;
		bool HasMemoryFaultCapture;
		bool HasTileStateReadSnapshot;
		bool HasAccumulatorReadSnapshot;
		bool HasRetirePublication;
		bool HasReplayRollbackConformance;
		bool BlocksArchitecturalSideEffectsBeforeRetire;
		bool KeepsVmxBackendFallbackNonAuthority;
		MatrixTilePhase09ExecuteCaptureDecisionRow(std::wstring mnemonic, std::wstring requiredRuntimeSurface, std::wstring decision, std::wstring primaryBlocker, bool requiresTileLoadCaptureBuffer, bool requiresTileStorePendingWriteBuffer, bool requiresMaccCaptureResult, bool requiresTransposeCaptureResult, bool requiresDeterministicExceptionCapture, bool requiresMemoryFaultCapture, bool requiresTileStateReadSnapshot, bool requiresAccumulatorReadSnapshot, bool hasExecutionCaptureSemantics, bool hasTileLoadCaptureBuffer, bool hasTileStorePendingWriteBuffer, bool hasMaccCaptureResult, bool hasTransposeCaptureResult, bool hasDeterministicExceptionCapture, bool hasMemoryFaultCapture, bool hasTileStateReadSnapshot, bool hasAccumulatorReadSnapshot, bool hasRetirePublication, bool hasReplayRollbackConformance, bool blocksArchitecturalSideEffectsBeforeRetire, bool keepsVmxBackendFallbackNonAuthority) : Mnemonic(std::move(mnemonic)), RequiredRuntimeSurface(std::move(requiredRuntimeSurface)), Decision(std::move(decision)), PrimaryBlocker(std::move(primaryBlocker)), RequiresTileLoadCaptureBuffer(requiresTileLoadCaptureBuffer), RequiresTileStorePendingWriteBuffer(requiresTileStorePendingWriteBuffer), RequiresMaccCaptureResult(requiresMaccCaptureResult), RequiresTransposeCaptureResult(requiresTransposeCaptureResult), RequiresDeterministicExceptionCapture(requiresDeterministicExceptionCapture), RequiresMemoryFaultCapture(requiresMemoryFaultCapture), RequiresTileStateReadSnapshot(requiresTileStateReadSnapshot), RequiresAccumulatorReadSnapshot(requiresAccumulatorReadSnapshot), HasExecutionCaptureSemantics(hasExecutionCaptureSemantics), HasTileLoadCaptureBuffer(hasTileLoadCaptureBuffer), HasTileStorePendingWriteBuffer(hasTileStorePendingWriteBuffer), HasMaccCaptureResult(hasMaccCaptureResult), HasTransposeCaptureResult(hasTransposeCaptureResult), HasDeterministicExceptionCapture(hasDeterministicExceptionCapture), HasMemoryFaultCapture(hasMemoryFaultCapture), HasTileStateReadSnapshot(hasTileStateReadSnapshot), HasAccumulatorReadSnapshot(hasAccumulatorReadSnapshot), HasRetirePublication(hasRetirePublication), HasReplayRollbackConformance(hasReplayRollbackConformance), BlocksArchitecturalSideEffectsBeforeRetire(blocksArchitecturalSideEffectsBeforeRetire), KeepsVmxBackendFallbackNonAuthority(keepsVmxBackendFallbackNonAuthority) {}
	};

	struct MatrixTilePhase10RetirePublicationDecisionRow final
	{
		std::wstring Mnemonic;
		std::wstring RequiredRuntimeSurface;
		std::wstring Decision;
		std::wstring PrimaryBlocker;
		bool RequiresTileLoadRetirePublication;
		bool RequiresTileStoreRetireCommit;
		bool RequiresAccumulatorRetirePublication;
		bool RequiresTransposeRetirePublication;
		bool RequiresFaultRetirementPolicy;
		bool RequiresWritebackOwnership;
		bool RequiresSideEffectPublicationOrdering;
		bool RequiresArchitecturalStateVisibilityRules;
		bool HasRetirePublicationAndCommit;
		bool HasTileLoadRetirePublication;
		bool HasTileStoreRetireCommit;
		bool HasAccumulatorRetirePublication;
		bool HasTransposeRetirePublication;
		bool HasFaultRetirementPolicy;
		bool HasWritebackOwnership;
		bool HasSideEffectPublicationOrdering;
		bool HasArchitecturalStateVisibilityRules;
		bool BlocksExecuteCaptureToRetireBypass;
		bool KeepsHostOwnedEvidenceNonArchitectural;
		bool KeepsVmxBackendFallbackNonAuthority;
		MatrixTilePhase10RetirePublicationDecisionRow(std::wstring mnemonic, std::wstring requiredRuntimeSurface, std::wstring decision, std::wstring primaryBlocker, bool requiresTileLoadRetirePublication, bool requiresTileStoreRetireCommit, bool requiresAccumulatorRetirePublication, bool requiresTransposeRetirePublication, bool requiresFaultRetirementPolicy, bool requiresWritebackOwnership, bool requiresSideEffectPublicationOrdering, bool requiresArchitecturalStateVisibilityRules, bool hasRetirePublicationAndCommit, bool hasTileLoadRetirePublication, bool hasTileStoreRetireCommit, bool hasAccumulatorRetirePublication, bool hasTransposeRetirePublication, bool hasFaultRetirementPolicy, bool hasWritebackOwnership, bool hasSideEffectPublicationOrdering, bool hasArchitecturalStateVisibilityRules, bool blocksExecuteCaptureToRetireBypass, bool keepsHostOwnedEvidenceNonArchitectural, bool keepsVmxBackendFallbackNonAuthority) : Mnemonic(std::move(mnemonic)), RequiredRuntimeSurface(std::move(requiredRuntimeSurface)), Decision(std::move(decision)), PrimaryBlocker(std::move(primaryBlocker)), RequiresTileLoadRetirePublication(requiresTileLoadRetirePublication), RequiresTileStoreRetireCommit(requiresTileStoreRetireCommit), RequiresAccumulatorRetirePublication(requiresAccumulatorRetirePublication), RequiresTransposeRetirePublication(requiresTransposeRetirePublication), RequiresFaultRetirementPolicy(requiresFaultRetirementPolicy), RequiresWritebackOwnership(requiresWritebackOwnership), RequiresSideEffectPublicationOrdering(requiresSideEffectPublicationOrdering), RequiresArchitecturalStateVisibilityRules(requiresArchitecturalStateVisibilityRules), HasRetirePublicationAndCommit(hasRetirePublicationAndCommit), HasTileLoadRetirePublication(hasTileLoadRetirePublication), HasTileStoreRetireCommit(hasTileStoreRetireCommit), HasAccumulatorRetirePublication(hasAccumulatorRetirePublication), HasTransposeRetirePublication(hasTransposeRetirePublication), HasFaultRetirementPolicy(hasFaultRetirementPolicy), HasWritebackOwnership(hasWritebackOwnership), HasSideEffectPublicationOrdering(hasSideEffectPublicationOrdering), HasArchitecturalStateVisibilityRules(hasArchitecturalStateVisibilityRules), BlocksExecuteCaptureToRetireBypass(blocksExecuteCaptureToRetireBypass), KeepsHostOwnedEvidenceNonArchitectural(keepsHostOwnedEvidenceNonArchitectural), KeepsVmxBackendFallbackNonAuthority(keepsVmxBackendFallbackNonAuthority) {}
	};

	struct MatrixTilePhase11ReplayRollbackDecisionRow final
	{
		std::wstring Mnemonic;
		std::wstring RequiredRuntimeSurface;
		std::wstring Decision;
		std::wstring PrimaryBlocker;
		bool RequiresDecodedInstructionReplayIdentity;
		bool RequiresTileDescriptorReplayIdentity;
		bool RequiresPendingTileWriteRollback;
		bool RequiresPendingMemoryStoreRollback;
		bool RequiresAccumulatorRollback;
		bool RequiresDeterministicReplayAfterMemoryFault;
		bool RequiresDeterministicReplayAfterDescriptorFault;
		bool RequiresLegalIllegalConformanceVectors;
		bool HasReplayRollbackConformance;
		bool HasDecodedInstructionReplayIdentity;
		bool HasTileDescriptorReplayIdentity;
		bool HasPendingTileWriteRollback;
		bool HasPendingMemoryStoreRollback;
		bool HasAccumulatorRollback;
		bool HasDeterministicReplayAfterMemoryFault;
		bool HasDeterministicReplayAfterDescriptorFault;
		bool HasLegalIllegalConformanceVectors;
		bool BlocksReplayWithoutRetirePublication;
		bool BlocksCaptureRecordIdentityBypass;
		bool KeepsVmxBackendFallbackNonAuthority;
		MatrixTilePhase11ReplayRollbackDecisionRow(std::wstring mnemonic, std::wstring requiredRuntimeSurface, std::wstring decision, std::wstring primaryBlocker, bool requiresDecodedInstructionReplayIdentity, bool requiresTileDescriptorReplayIdentity, bool requiresPendingTileWriteRollback, bool requiresPendingMemoryStoreRollback, bool requiresAccumulatorRollback, bool requiresDeterministicReplayAfterMemoryFault, bool requiresDeterministicReplayAfterDescriptorFault, bool requiresLegalIllegalConformanceVectors, bool hasReplayRollbackConformance, bool hasDecodedInstructionReplayIdentity, bool hasTileDescriptorReplayIdentity, bool hasPendingTileWriteRollback, bool hasPendingMemoryStoreRollback, bool hasAccumulatorRollback, bool hasDeterministicReplayAfterMemoryFault, bool hasDeterministicReplayAfterDescriptorFault, bool hasLegalIllegalConformanceVectors, bool blocksReplayWithoutRetirePublication, bool blocksCaptureRecordIdentityBypass, bool keepsVmxBackendFallbackNonAuthority) : Mnemonic(std::move(mnemonic)), RequiredRuntimeSurface(std::move(requiredRuntimeSurface)), Decision(std::move(decision)), PrimaryBlocker(std::move(primaryBlocker)), RequiresDecodedInstructionReplayIdentity(requiresDecodedInstructionReplayIdentity), RequiresTileDescriptorReplayIdentity(requiresTileDescriptorReplayIdentity), RequiresPendingTileWriteRollback(requiresPendingTileWriteRollback), RequiresPendingMemoryStoreRollback(requiresPendingMemoryStoreRollback), RequiresAccumulatorRollback(requiresAccumulatorRollback), RequiresDeterministicReplayAfterMemoryFault(requiresDeterministicReplayAfterMemoryFault), RequiresDeterministicReplayAfterDescriptorFault(requiresDeterministicReplayAfterDescriptorFault), RequiresLegalIllegalConformanceVectors(requiresLegalIllegalConformanceVectors), HasReplayRollbackConformance(hasReplayRollbackConformance), HasDecodedInstructionReplayIdentity(hasDecodedInstructionReplayIdentity), HasTileDescriptorReplayIdentity(hasTileDescriptorReplayIdentity), HasPendingTileWriteRollback(hasPendingTileWriteRollback), HasPendingMemoryStoreRollback(hasPendingMemoryStoreRollback), HasAccumulatorRollback(hasAccumulatorRollback), HasDeterministicReplayAfterMemoryFault(hasDeterministicReplayAfterMemoryFault), HasDeterministicReplayAfterDescriptorFault(hasDeterministicReplayAfterDescriptorFault), HasLegalIllegalConformanceVectors(hasLegalIllegalConformanceVectors), BlocksReplayWithoutRetirePublication(blocksReplayWithoutRetirePublication), BlocksCaptureRecordIdentityBypass(blocksCaptureRecordIdentityBypass), KeepsVmxBackendFallbackNonAuthority(keepsVmxBackendFallbackNonAuthority) {}
	};

	struct MatrixTilePhase12GoldenArtifactDecisionRow final
	{
		std::wstring Mnemonic;
		std::wstring RequiredArtifactSurface;
		std::wstring Decision;
		std::wstring NoFallbackDecision;
		std::wstring PrimaryBlocker;
		bool RequiresLegalDecodeEncodeRoundTripVectors;
		bool RequiresLegalIrMaterializerProjectionVectors;
		bool RequiresLegalExecuteRetireVectors;
		bool RequiresMemoryFaultVectors;
		bool RequiresDescriptorFaultVectors;
		bool RequiresAccumulatorVectors;
		bool RequiresTransposeVectors;
		bool RequiresReplayRollbackVectors;
		bool RequiresNegativeReservedVectors;
		bool RequiresRuntimeNoFallbackNoHiddenLoweringRegressionEvidence;
		bool HasPositiveExecutableGoldenArtifacts;
		bool HasRuntimeNoFallbackNoHiddenLoweringRegressionEvidence;
		bool HasLegalDecodeEncodeRoundTripVectors;
		bool HasLegalIrMaterializerProjectionVectors;
		bool HasLegalExecuteRetireVectors;
		bool HasMemoryFaultVectors;
		bool HasDescriptorFaultVectors;
		bool HasAccumulatorVectors;
		bool HasTransposeVectors;
		bool HasReplayRollbackVectors;
		bool HasNegativeReservedVectors;
		bool BlocksPositiveGoldenPublication;
		bool BlocksNoFallbackRegressionEvidenceBypass;
		bool KeepsVmxBackendFallbackNonAuthority;
		bool KeepsCompilerScopeClosed;
		bool KeepsCompilerHandoffBlocked;
		MatrixTilePhase12GoldenArtifactDecisionRow(std::wstring mnemonic, std::wstring requiredArtifactSurface, std::wstring decision, std::wstring noFallbackDecision, std::wstring primaryBlocker, bool requiresLegalDecodeEncodeRoundTripVectors, bool requiresLegalIrMaterializerProjectionVectors, bool requiresLegalExecuteRetireVectors, bool requiresMemoryFaultVectors, bool requiresDescriptorFaultVectors, bool requiresAccumulatorVectors, bool requiresTransposeVectors, bool requiresReplayRollbackVectors, bool requiresNegativeReservedVectors, bool requiresRuntimeNoFallbackNoHiddenLoweringRegressionEvidence, bool hasPositiveExecutableGoldenArtifacts, bool hasRuntimeNoFallbackNoHiddenLoweringRegressionEvidence, bool hasLegalDecodeEncodeRoundTripVectors, bool hasLegalIrMaterializerProjectionVectors, bool hasLegalExecuteRetireVectors, bool hasMemoryFaultVectors, bool hasDescriptorFaultVectors, bool hasAccumulatorVectors, bool hasTransposeVectors, bool hasReplayRollbackVectors, bool hasNegativeReservedVectors, bool blocksPositiveGoldenPublication, bool blocksNoFallbackRegressionEvidenceBypass, bool keepsVmxBackendFallbackNonAuthority, bool keepsCompilerScopeClosed, bool keepsCompilerHandoffBlocked) : Mnemonic(std::move(mnemonic)), RequiredArtifactSurface(std::move(requiredArtifactSurface)), Decision(std::move(decision)), NoFallbackDecision(std::move(noFallbackDecision)), PrimaryBlocker(std::move(primaryBlocker)), RequiresLegalDecodeEncodeRoundTripVectors(requiresLegalDecodeEncodeRoundTripVectors), RequiresLegalIrMaterializerProjectionVectors(requiresLegalIrMaterializerProjectionVectors), RequiresLegalExecuteRetireVectors(requiresLegalExecuteRetireVectors), RequiresMemoryFaultVectors(requiresMemoryFaultVectors), RequiresDescriptorFaultVectors(requiresDescriptorFaultVectors), RequiresAccumulatorVectors(requiresAccumulatorVectors), RequiresTransposeVectors(requiresTransposeVectors), RequiresReplayRollbackVectors(requiresReplayRollbackVectors), RequiresNegativeReservedVectors(requiresNegativeReservedVectors), RequiresRuntimeNoFallbackNoHiddenLoweringRegressionEvidence(requiresRuntimeNoFallbackNoHiddenLoweringRegressionEvidence), HasPositiveExecutableGoldenArtifacts(hasPositiveExecutableGoldenArtifacts), HasRuntimeNoFallbackNoHiddenLoweringRegressionEvidence(hasRuntimeNoFallbackNoHiddenLoweringRegressionEvidence), HasLegalDecodeEncodeRoundTripVectors(hasLegalDecodeEncodeRoundTripVectors), HasLegalIrMaterializerProjectionVectors(hasLegalIrMaterializerProjectionVectors), HasLegalExecuteRetireVectors(hasLegalExecuteRetireVectors), HasMemoryFaultVectors(hasMemoryFaultVectors), HasDescriptorFaultVectors(hasDescriptorFaultVectors), HasAccumulatorVectors(hasAccumulatorVectors), HasTransposeVectors(hasTransposeVectors), HasReplayRollbackVectors(hasReplayRollbackVectors), HasNegativeReservedVectors(hasNegativeReservedVectors), BlocksPositiveGoldenPublication(blocksPositiveGoldenPublication), BlocksNoFallbackRegressionEvidenceBypass(blocksNoFallbackRegressionEvidenceBypass), KeepsVmxBackendFallbackNonAuthority(keepsVmxBackendFallbackNonAuthority), KeepsCompilerScopeClosed(keepsCompilerScopeClosed), KeepsCompilerHandoffBlocked(keepsCompilerHandoffBlocked) {}
	};

	class MatrixTileRuntimeIsaPackageContract final
	{
	public:
		static inline const std::wstring Phase = L"Phase14";
		static inline const std::wstring PackageName = L"MatrixTileRuntimeIsaPackage";
		static inline const std::wstring Decision = L"Phase14TileStreamResourceContourAndPositiveCompilerHandoffClosed";
		static inline const std::wstring ClosureAttemptDate = L"2026-06-07";
		static inline const std::wstring ClosureAttemptDecision = L"ExecutableClosureBlockedByMissingAbiRuntimeEvidence";
		static inline const std::wstring EvidenceBoundary = L"MatrixTileRuntimeExecutableAuthority";
		static inline const std::wstring ExtensionName = L"XMatrix";
		static inline const std::wstring VmxBoundary = L"GenericRuntimeOnly";
		static inline const std::wstring TileExecutionModelGateDecision = L"ClosedExecuteCaptureTileStateSnapshotModel";
		static inline const std::wstring TileDescriptorAbiGateDecision = L"ClosedCanonicalTileDescriptorAbi";
		static inline const std::wstring MemoryFaultModelGateDecision = MatrixTileMemoryShapeAndFaultAbi::MemoryShapeFaultDecision;
		static inline const std::wstring AccumulatorTileAbiGateDecision = MatrixTileAccumulatorAndTransposePolicyAbi::MaccAccumulatorDecision;
		static inline const std::wstring TransposePolicyAbiGateDecision = MatrixTileAccumulatorAndTransposePolicyAbi::TransposeCarrierDecision;
		static inline const std::wstring VectorLegalityMatrixGateDecision = MatrixTileRuntimeOwnedVlmRows::RuntimeOwnedVlmRowsDecision;
		static inline const std::wstring RuntimePipelineGateDecision = L"ClosedMatrixTileRuntimePipelineThroughPositiveGoldens";
		static inline const std::wstring DecoderEncoderGateDecision = L"ClosedMatrixTileDecoderEncoderAbi";
		static inline const std::wstring InstructionIrProjectionGateDecision = MatrixTileIrProjectionAndMaterializer::TileDescriptorProjectionDecision;
		static inline const std::wstring RegistryMaterializerGateDecision = MatrixTileIrProjectionAndMaterializer::MaterializerFactoryDecision;
		static inline const std::wstring TypedTileMicroOpGateDecision = L"ClosedOperationSpecificMatrixTileMicroOpCarrier";
		static inline const std::wstring SchedulerLaneBindingGateDecision = L"ClosedMatrixTileMemoryLane6AndComputeLanePlacement";
		static inline const std::wstring ExecuteCaptureGateDecision = MatrixTileExecuteCaptureAbi::ExecuteCaptureDecision;
		static inline const std::wstring RetirePublicationGateDecision = MatrixTileRetirePublicationAbi::RetirePublicationDecision;
		static inline const std::wstring ReplayRollbackGateDecision = MatrixTileReplayRollbackAbi::ReplayRollbackDecision;
		static inline const std::wstring GoldenArtifactsGateDecision = MatrixTilePositiveGoldenArtifactManifest::ManifestDecision;
		static inline const std::wstring CompilerUpdateReadinessDecision = MatrixTileCompilerEmissionHandoffPackage::CompilerBoundaryDecision;
		static inline const std::wstring CompilerNoEmissionBoundaryDecision = L"SupersededByExistingPositiveCompilerImplementationCompilerCodeUnchangedByPhase14";
		static inline const std::wstring ClosureLedgerDecision = L"RuntimeIsaPhases01To14ClosedExecutableStatusAndCompilerHandoffReady";
		static inline const std::wstring NegativeGoldenManifestPath = L"NonRTL/Core/ISA/Instructions/NonVmx/Docs/ImplPlan/PHASE_09B_MATRIX_TILE_NEGATIVE_GOLDEN_MANIFEST_2026-06-07.md";
		static inline const std::wstring PositiveGoldenManifestPath = L"CloseToHSL/Core/ISA/Instructions/NonVmx/Lanes00_03Vector/MatrixTile/MatrixTilePositiveGoldenArtifactManifest.cs";
		static inline const std::wstring GoldenArtifactPublicationDecision = MatrixTilePositiveGoldenArtifactManifest::ManifestDecision;
		static inline const std::wstring RemainingRuntimeIsaTaskPoolDecision = L"RuntimeIsaPhases01To14ClosedNoRemainingRuntimeTasks";
		static inline const std::wstring StatusCatalogAuthorityDecision = MatrixTileCompilerEmissionHandoffPackage::StatusCatalogDecision;
		static inline const std::wstring OpcodeDescriptorAuthorityDecision = L"PackageOpcodesAreExecutableIdentityAuthorityWithinClosedRuntimeEvidenceChain";
		static inline const std::wstring Phase01AuthorityAdrPath = L"Documentation/InstructionsList/MTILE_RefPlan/01_authority_status_catalog_and_opcode_adr.md";
		static inline const std::wstring Phase01AuthorityDecision = L"ClosedRetainedNumericOpcodesArePackageOpcodeIdentityAuthority";
		static inline const std::wstring Phase01AuthorityReviewDate = L"2026-06-08";
		static inline const std::wstring Phase01RetainedOpcodeRole = L"PackageOpcodeIdentityAuthorityOnlyNoExecution";
		static inline const std::wstring Phase01DescriptorAuthorityRole = L"NoDescriptorOpTypeAuthorityOpened";
		static inline const std::wstring Phase01PackageFeatureRole = L"SingleXMatrixDeclaredPackageFeatureDeferred";
		static inline const std::wstring Phase01StatusCatalogPromotionRule = L"PositiveStatusCatalogPromotionOnlyAfterFullRuntimeIsaEvidenceChainAndPhase13";
		static inline const std::wstring Phase01MtileMemoryPolicyDecision = MatrixTileMemoryShapeAndFaultAbi::MemoryShapeFaultDecision;
		static inline const std::wstring Phase01MtransposePolicyDecision = MatrixTileAccumulatorAndTransposePolicyAbi::TransposeCarrierDecision;
		static inline const std::wstring Phase01BlockingReason = L"Phase12ClosedStatusCatalogAndCompilerHandoffRemain";
		static inline const std::wstring Phase02StateDescriptorAbiPath = L"Documentation/InstructionsList/MTILE_RefPlan/02_tile_state_and_descriptor_abi.md";
		static inline const std::wstring Phase02StateDescriptorDecision = L"ClosedGuestArchitecturalTileStateOwnerAndCanonicalDescriptorAbi";
		static inline const std::wstring Phase02ReviewDate = L"2026-06-08";
		static inline const std::wstring Phase02TileStateOwnerDecision = MatrixTileArchitecturalTileStateAndDescriptorAbi::TileStateOwnerDecision;
		static inline const std::wstring Phase02DescriptorCarrierDecision = MatrixTileArchitecturalTileStateAndDescriptorAbi::DescriptorCarrierDecision;
		static inline const std::wstring Phase02DescriptorValidationDecision = MatrixTileArchitecturalTileStateAndDescriptorAbi::DescriptorValidationDecision;
		static inline const std::wstring Phase02ReservedDescriptorDecision = MatrixTileArchitecturalTileStateAndDescriptorAbi::ReservedDescriptorDecision;
		static inline const std::wstring Phase02GuestHostEvidenceBoundary = L"HostOwnedEvidenceCannotBecomeGuestArchitecturalTileState";
		static inline const std::wstring Phase02BlockingReason = L"Phase12ClosedStatusCatalogAndCompilerHandoffRemain";
		static inline const std::wstring Phase03MemoryShapeFaultAbiPath = L"Documentation/InstructionsList/MTILE_RefPlan/03_memory_shape_and_fault_model.md";
		static inline const std::wstring Phase03MemoryShapeFaultDecision = MatrixTileMemoryShapeAndFaultAbi::MemoryShapeFaultDecision;
		static inline const std::wstring Phase03ReviewDate = L"2026-06-08";
		static inline const std::wstring Phase03EffectiveAddressDecision = MatrixTileMemoryShapeAndFaultAbi::EffectiveAddressDecision;
		static inline const std::wstring Phase03ShapeValidationDecision = MatrixTileMemoryShapeAndFaultAbi::ShapeValidationDecision;
		static inline const std::wstring Phase03FaultReplayDecision = MatrixTileMemoryShapeAndFaultAbi::FaultReplayDecision;
		static inline const std::wstring Phase03SideEffectOwnerDecision = MatrixTileMemoryShapeAndFaultAbi::SideEffectOwnerDecision;
		static inline const std::wstring Phase03EaFallbackDecision = MatrixTileMemoryShapeAndFaultAbi::EaFallbackDecision;
		static inline const std::wstring Phase03BlockingReason = L"Phase12ClosedStatusCatalogAndCompilerHandoffRemain";
		static inline const std::wstring Phase04AccumulatorTransposeAbiPath = L"Documentation/InstructionsList/MTILE_RefPlan/04_accumulator_and_transpose_policy_abi.md";
		static inline const std::wstring Phase04AccumulatorTransposeDecision = MatrixTileAccumulatorAndTransposePolicyAbi::AccumulatorTransposeDecision;
		static inline const std::wstring Phase04ReviewDate = L"2026-06-08";
		static inline const std::wstring Phase04MaccAccumulatorDecision = MatrixTileAccumulatorAndTransposePolicyAbi::MaccAccumulatorDecision;
		static inline const std::wstring Phase04MaccShapeDecision = MatrixTileAccumulatorAndTransposePolicyAbi::MaccShapeDecision;
		static inline const std::wstring Phase04MaccRetireReplayDecision = MatrixTileAccumulatorAndTransposePolicyAbi::MaccRetireReplayDecision;
		static inline const std::wstring Phase04TransposeCarrierDecision = MatrixTileAccumulatorAndTransposePolicyAbi::TransposeCarrierDecision;
		static inline const std::wstring Phase04TransposeLayoutDecision = MatrixTileAccumulatorAndTransposePolicyAbi::TransposeLayoutDecision;
		static inline const std::wstring Phase04TransposeRetireReplayDecision = MatrixTileAccumulatorAndTransposePolicyAbi::TransposeRetireReplayDecision;
		static inline const std::wstring Phase04FallbackDecision = MatrixTileAccumulatorAndTransposePolicyAbi::FallbackDecision;
		static inline const std::wstring Phase04BlockingReason = L"Phase12ClosedStatusCatalogAndCompilerHandoffRemain";
		static inline const std::wstring Phase05RuntimeOwnedVlmRowsPath = L"Documentation/InstructionsList/MTILE_RefPlan/05_runtime_owned_vlm_rows.md";
		static inline const std::wstring Phase05RuntimeOwnedVlmRowsDecision = MatrixTileRuntimeOwnedVlmRows::RuntimeOwnedVlmRowsDecision;
		static inline const std::wstring Phase05ReviewDate = L"2026-06-08";
		static inline const std::wstring Phase05FeatureGateDecision = MatrixTileRuntimeOwnedVlmRows::FeatureGateDecision;
		static inline const std::wstring Phase05ElementWidthDecision = MatrixTileRuntimeOwnedVlmRows::ElementWidthDecision;
		static inline const std::wstring Phase05TileShapeDecision = MatrixTileRuntimeOwnedVlmRows::TileShapeDecision;
		static inline const std::wstring Phase05MemoryLayoutDecision = MatrixTileRuntimeOwnedVlmRows::MemoryLayoutDecision;
		static inline const std::wstring Phase05ReservedDisabledDecision = MatrixTileRuntimeOwnedVlmRows::ReservedDisabledDecision;
		static inline const std::wstring Phase05MetadataNonAuthorityDecision = MatrixTileRuntimeOwnedVlmRows::MetadataNonAuthorityDecision;
		static inline const std::wstring Phase05DecoderAdmissionDecision = MatrixTileRuntimeOwnedVlmRows::DecoderAdmissionDecision;
		static inline const std::wstring Phase05BlockingReason = L"Phase12ClosedStatusCatalogAndCompilerHandoffRemain";
		static inline const std::wstring Phase06DecoderEncoderAbiPath = L"Documentation/InstructionsList/MTILE_RefPlan/06_decoder_encoder_abi.md";
		static inline const std::wstring Phase06DecoderEncoderDecision = L"ClosedMatrixTileDecoderEncoderAbi";
		static inline const std::wstring Phase06ReviewDate = L"2026-06-08";
		static inline const std::wstring Phase06DispatchSourceDecision = L"OpcodeOrDescriptorDispatchSourceSelected";
		static inline const std::wstring Phase06BinaryLayoutDecision = L"CanonicalVectorCarrierBinaryFieldLayoutSelected";
		static inline const std::wstring Phase06OperandMappingDecision = L"CanonicalVectorCarrierOperandMappingSelected";
		static inline const std::wstring Phase06DescriptorDecodeDecision = L"DescriptorEncodingAndValidationSelected";
		static inline const std::wstring Phase06ReservedFieldDecision = L"ReservedMalformedDisabledDecodeFaultAbiSelected";
		static inline const std::wstring Phase06EncoderRoundTripDecision = L"EncoderRoundTripAbiSelected";
		static inline const std::wstring Phase06FeatureGateDecision = L"RuntimeOwnedVlmRowsSelectCanonicalMatrixTileDecode";
		static inline const std::wstring Phase06BlockingReason = L"Phase12ClosedStatusCatalogAndCompilerHandoffRemain";
		static inline const std::wstring Phase07IrProjectionMaterializerPath = L"Documentation/InstructionsList/MTILE_RefPlan/07_ir_projection_and_materializer.md";
		static inline const std::wstring Phase07IrProjectionMaterializerDecision = MatrixTileIrProjectionAndMaterializer::IrProjectionMaterializerDecision;
		static inline const std::wstring Phase07ReviewDate = L"2026-06-08";
		static inline const std::wstring Phase07TileDescriptorProjectionDecision = MatrixTileIrProjectionAndMaterializer::TileDescriptorProjectionDecision;
		static inline const std::wstring Phase07MemoryOperandProjectionDecision = MatrixTileIrProjectionAndMaterializer::MemoryOperandProjectionDecision;
		static inline const std::wstring Phase07AccumulatorProjectionDecision = MatrixTileIrProjectionAndMaterializer::AccumulatorProjectionDecision;
		static inline const std::wstring Phase07TransposeProjectionDecision = MatrixTileIrProjectionAndMaterializer::TransposeProjectionDecision;
		static inline const std::wstring Phase07RegistryEntryDecision = MatrixTileIrProjectionAndMaterializer::RegistryEntryDecision;
		static inline const std::wstring Phase07MaterializerFactoryDecision = MatrixTileIrProjectionAndMaterializer::MaterializerFactoryDecision;
		static inline const std::wstring Phase07TypedObjectDecision = MatrixTileIrProjectionAndMaterializer::TypedObjectDecision;
		static inline const std::wstring Phase07InvalidIrDecision = MatrixTileIrProjectionAndMaterializer::InvalidIrDecision;
		static inline const std::wstring Phase07CompilerIrBoundaryDecision = MatrixTileIrProjectionAndMaterializer::CompilerIrBoundaryDecision;
		static inline const std::wstring Phase07BlockingReason = L"Phase12ClosedStatusCatalogAndCompilerHandoffRemain";
		static inline const std::wstring Phase08TypedTileMicroOpSchedulerLanePath = L"Documentation/InstructionsList/MTILE_RefPlan/08_typed_tile_microop_and_scheduler_lane.md";
		static inline const std::wstring Phase08TypedTileMicroOpSchedulerLaneDecision = L"SupersededByPhase14OperationSpecificResourceContour";
		static inline const std::wstring Phase08ReviewDate = L"2026-06-08";
		static inline const std::wstring Phase08TypedMicroOpDecision = L"ClosedTypedTileMicroOpAbi";
		static inline const std::wstring Phase08TileMemoryDependencyDecision = L"ClosedTileMemoryDependencyMetadata";
		static inline const std::wstring Phase08TileRegisterDependencyDecision = L"ClosedTileRegisterDependencyMetadata";
		static inline const std::wstring Phase08AccumulatorDependencyDecision = L"ClosedAccumulatorDependencyMetadata";
		static inline const std::wstring Phase08SchedulerLaneBindingDecision = L"ClosedMatrixTileMemoryLane6AndComputeLanes00To03Binding";
		static inline const std::wstring Phase08IssueConstraintsDecision = L"ClosedTileIssueConstraints";
		static inline const std::wstring Phase08CaptureBarrierDecision = L"ClosedTileMemoryAndTileStateCaptureBarrierMetadata";
		static inline const std::wstring Phase08BypassDecision = L"NoMicroOpSchedulerMaterializerBypassWithoutPriorPhases";
		static inline const std::wstring Phase08FallbackDecision = L"NoVmxBackendOrExternalFallbackAuthority";
		static inline const std::wstring Phase08BlockingReason = L"Phase12ClosedStatusCatalogAndCompilerHandoffRemain";
		static inline const std::wstring Phase09ExecuteCaptureSemanticsPath = L"Documentation/InstructionsList/MTILE_RefPlan/09_execute_capture_semantics.md";
		static inline const std::wstring Phase09ExecuteCaptureSemanticsDecision = MatrixTileExecuteCaptureAbi::ExecuteCaptureDecision;
		static inline const std::wstring Phase09ReviewDate = L"2026-06-09";
		static inline const std::wstring Phase09LoadCaptureDecision = MatrixTileExecuteCaptureAbi::TileLoadCaptureDecision;
		static inline const std::wstring Phase09StoreCaptureDecision = MatrixTileExecuteCaptureAbi::TileStoreCaptureDecision;
		static inline const std::wstring Phase09MaccCaptureDecision = MatrixTileExecuteCaptureAbi::MaccCaptureDecision;
		static inline const std::wstring Phase09TransposeCaptureDecision = MatrixTileExecuteCaptureAbi::TransposeCaptureDecision;
		static inline const std::wstring Phase09DeterministicCaptureDecision = MatrixTileExecuteCaptureAbi::DeterministicExceptionCaptureDecision;
		static inline const std::wstring Phase09MemoryFaultCaptureDecision = MatrixTileExecuteCaptureAbi::MemoryFaultCaptureDecision;
		static inline const std::wstring Phase09TileStateSnapshotDecision = MatrixTileExecuteCaptureAbi::TileStateSnapshotDecision;
		static inline const std::wstring Phase09AccumulatorSnapshotDecision = MatrixTileExecuteCaptureAbi::AccumulatorSnapshotDecision;
		static inline const std::wstring Phase09RetirePublicationDecision = MatrixTileExecuteCaptureAbi::RetirePublicationDecision;
		static inline const std::wstring Phase09ReplayRollbackDecision = MatrixTileExecuteCaptureAbi::ReplayRollbackDecision;
		static inline const std::wstring Phase09BypassDecision = MatrixTileExecuteCaptureAbi::BypassDecision;
		static inline const std::wstring Phase09FallbackDecision = MatrixTileExecuteCaptureAbi::FallbackDecision;
		static inline const std::wstring Phase09BlockingReason = L"NonePhase14ClosedHistoricalExecuteCaptureBoundaryPreserved";
		static inline const std::wstring Phase10RetirePublicationCommitPath = L"Documentation/InstructionsList/MTILE_RefPlan/10_retire_publication_and_commit.md";
		static inline const std::wstring Phase10RetirePublicationCommitDecision = MatrixTileRetirePublicationAbi::RetirePublicationDecision;
		static inline const std::wstring Phase10ReviewDate = L"2026-06-10";
		static inline const std::wstring Phase10LoadRetireDecision = MatrixTileRetirePublicationAbi::LoadPublicationDecision;
		static inline const std::wstring Phase10StoreRetireDecision = MatrixTileRetirePublicationAbi::StoreCommitDecision;
		static inline const std::wstring Phase10MaccRetireDecision = MatrixTileRetirePublicationAbi::MaccPublicationDecision;
		static inline const std::wstring Phase10TransposeRetireDecision = MatrixTileRetirePublicationAbi::TransposePublicationDecision;
		static inline const std::wstring Phase10FaultRetirementDecision = MatrixTileRetirePublicationAbi::FaultRetirementDecision;
		static inline const std::wstring Phase10WritebackOwnershipDecision = MatrixTileRetirePublicationAbi::WritebackOwnershipDecision;
		static inline const std::wstring Phase10SideEffectOrderingDecision = MatrixTileRetirePublicationAbi::SideEffectOrderingDecision;
		static inline const std::wstring Phase10ArchitecturalVisibilityDecision = MatrixTileRetirePublicationAbi::ArchitecturalVisibilityDecision;
		static inline const std::wstring Phase10BypassDecision = MatrixTileRetirePublicationAbi::DuplicateRetireDecision;
		static inline const std::wstring Phase10CaptureCorrelationDecision = MatrixTileRetirePublicationAbi::CorrelationDecision;
		static inline const std::wstring Phase10HostEvidenceDecision = L"NoHostOwnedEvidencePublishedAsGuestArchitecturalState";
		static inline const std::wstring Phase10FallbackDecision = MatrixTileRetirePublicationAbi::FallbackDecision;
		static inline const std::wstring Phase10BlockingReason = L"NonePhase10ClosedPhase11ReplayRollbackClosed";
		static inline const std::wstring Phase11ReplayRollbackConformancePath = L"Documentation/InstructionsList/MTILE_RefPlan/11_replay_rollback_conformance.md";
		static inline const std::wstring Phase11ReplayRollbackConformanceDecision = MatrixTileReplayRollbackAbi::ReplayRollbackDecision;
		static inline const std::wstring Phase11ReviewDate = L"2026-06-10";
		static inline const std::wstring Phase11DecodedInstructionReplayIdentityDecision = MatrixTileReplayRollbackAbi::DecodedInstructionIdentityDecision;
		static inline const std::wstring Phase11TileDescriptorReplayIdentityDecision = MatrixTileReplayRollbackAbi::TileDescriptorIdentityDecision;
		static inline const std::wstring Phase11PendingTileWriteRollbackDecision = MatrixTileReplayRollbackAbi::TileRollbackDecision;
		static inline const std::wstring Phase11PendingMemoryStoreRollbackDecision = MatrixTileReplayRollbackAbi::MemoryRollbackDecision;
		static inline const std::wstring Phase11AccumulatorRollbackDecision = MatrixTileReplayRollbackAbi::AccumulatorRollbackDecision;
		static inline const std::wstring Phase11MemoryFaultReplayDecision = MatrixTileReplayRollbackAbi::MemoryFaultReplayDecision;
		static inline const std::wstring Phase11DescriptorFaultReplayDecision = MatrixTileReplayRollbackAbi::DescriptorFaultReplayDecision;
		static inline const std::wstring Phase11ConformanceVectorDecision = L"ClosedReplayRollbackLegalIllegalConformanceVectors";
		static inline const std::wstring Phase11BypassDecision = MatrixTileReplayRollbackAbi::BypassDecision;
		static inline const std::wstring Phase11FallbackDecision = MatrixTileReplayRollbackAbi::FallbackDecision;
		static inline const std::wstring Phase11BlockingReason = L"NonePhase11ClosedPhase12Closed";
		static inline const std::wstring Phase12PositiveExecutableGoldenArtifactsPath = L"Documentation/InstructionsList/MTILE_RefPlan/12_positive_executable_golden_artifacts.md";
		static inline const std::wstring Phase12PositiveExecutableGoldenArtifactsDecision = MatrixTilePositiveGoldenArtifactManifest::ManifestDecision;
		static inline const std::wstring Phase12RuntimeNoFallbackNoHiddenLoweringRegressionDecision = MatrixTilePositiveGoldenArtifactManifest::NoFallbackDecision;
		static inline const std::wstring Phase12ReviewDate = L"2026-06-10";
		static inline const std::wstring Phase12DecodeEncodeVectorsDecision = L"ClosedLegalDecodeEncodeRoundTripGoldenVectors";
		static inline const std::wstring Phase12IrMaterializerVectorsDecision = L"ClosedLegalIrMaterializerProjectionGoldenVectors";
		static inline const std::wstring Phase12ExecuteRetireVectorsDecision = L"ClosedLegalExecuteRetireGoldenVectors";
		static inline const std::wstring Phase12MemoryFaultVectorsDecision = L"ClosedLoadStoreAllOrNoneMemoryFaultGoldenVectors";
		static inline const std::wstring Phase12DescriptorFaultVectorsDecision = L"ClosedDescriptorFaultGoldenVectors";
		static inline const std::wstring Phase12AccumulatorVectorsDecision = L"ClosedAccumulatorGoldenVectors";
		static inline const std::wstring Phase12TransposeVectorsDecision = L"ClosedTransposeGoldenVectors";
		static inline const std::wstring Phase12ReplayRollbackVectorsDecision = L"ClosedReplayRollbackGoldenVectors";
		static inline const std::wstring Phase12ReservedRowsDecision = MatrixTilePositiveGoldenArtifactManifest::ReservedCarrierDecision;
		static inline const std::wstring Phase12NoFallbackRegressionDecision = MatrixTileNoFallbackEvidenceContract::EvidenceDecision;
		static inline const std::wstring Phase12BlockingReason = L"NonePhase14RegeneratedPlacementSensitiveEvidenceClosed";
		static inline const std::wstring Phase13RuntimeIsaReadinessForCompilerHandoffPath = L"Documentation/InstructionsList/MTILE_RefPlan/13_runtime_isa_readiness_for_compiler_handoff.md";
		static inline const std::wstring Phase13ReviewDate = L"2026-06-10";
		static inline const std::wstring Phase13CompilerVisibleNoEmissionBoundaryDecision = L"SupersededByExistingPositiveCompilerImplementation";
		static inline const std::wstring Phase13PositiveCompilerEmissionHandoffDecision = MatrixTileCompilerEmissionHandoffPackage::HandoffDecision;
		static inline const std::wstring Phase13BlockingReason = L"SupersededPhase13ReadinessWasSuspendedUntilPhase14Evidence";
		static inline const std::wstring Phase14TileStreamResourceContourPath = L"Documentation/InstructionsList/MTILE_RefPlan/14_tile_stream_resource_contour_correction.md";
		static inline const std::wstring Phase14ReviewDate = L"2026-06-11";
		static inline const std::wstring Phase14ResourceClassificationDecision = L"ClosedMatrixTileMemoryAndMatrixTileComputeRuntimeClassification";
		static inline const std::wstring Phase14SchedulerPlacementDecision = L"ClosedMatrixTileMemoryLane6AndMatrixTileComputePlacement";
		static inline const std::wstring Phase14StreamTransferDecision = MatrixTileStreamTransferAbi::TransferDecision;
		static inline const std::wstring Phase14ReadinessDecision = L"ClosedAfterRegeneratedPlacementSensitiveRuntimeEvidence";
		static inline const std::wstring Phase15NumericPolicyAbiPath = L"Documentation/InstructionsList/MTILE_RefPlan/15_numeric_policy_abi_and_supported_profiles.md";
		static inline const std::wstring Phase15NumericPolicyDecision = MatrixTileNumericPolicyAbi::PolicyDecision;
		static inline const std::wstring Phase15NumericReadinessDecision = L"NumericSensitiveRuntimeAndCompilerSidebandEvidenceClosed";
		static inline const std::wstring Phase16FormalArithmeticLayoutPath = L"Documentation/InstructionsList/MTILE_RefPlan/16_formal_macc_arithmetic_and_layout_policy.md";
		static inline const std::wstring Phase16FormalArithmeticDecision = MatrixTileMaccArithmeticAbi::FormalFunctionDecision;
		static inline const std::wstring Phase16LayoutPolicyDecision = MatrixTileLayoutPolicyAbi::PolicyDecision;
		static inline const std::wstring Phase17PolicyBoundIdentityPath = L"Documentation/InstructionsList/MTILE_RefPlan/17_numeric_capture_retire_replay_identity.md";
		static inline const std::wstring Phase17PolicyBoundCaptureIdentityDecision = MatrixTilePolicyBoundIdentityAbi::CaptureIdentityDecision;
		static inline const std::wstring Phase17PolicyBoundReplayIdentityDecision = MatrixTilePolicyBoundIdentityAbi::ReplayIdentityDecision;
		static inline const std::wstring Phase18NumericLayoutGoldenCorpusPath = L"Documentation/Stream WhiteBook/03_MatrixTile/Golden/matrix_tile_numeric_layout_golden_v1.json";
		static inline const std::wstring Phase18NumericLayoutGoldenCorpusDecision = L"ClosedMachineReadableNumericLayoutGoldenCorpusV1";
		static inline const std::wstring Phase19PackageReclosurePath = L"Documentation/InstructionsList/MTILE_RefPlan/19_package_reclosure_and_compiler_conformance.md";
		static inline const std::wstring Phase19CompilerConformanceDecision = L"ClosedCompilerMatrixTileLoweredAnnotationsCarryNumericLayoutPolicySidebands";
		static inline const std::wstring Phase19CompilerConformanceBlocker = L"NonePhase19CompilerSidebandConformanceClosed";
		static inline const std::wstring Phase19RequiredCompilerBridge = L"CompilerOwnedBundleAnnotationsMustCarryRuntimeOwnedMatrixTileNumericPolicyAndLayoutPolicy";
		static inline const std::wstring SupersededSchedulerClaim = L"AllMatrixTileMicroOpsUseCommonAluClass";
		static inline const std::wstring CompilerHandoffGateDecision = Phase13PositiveCompilerEmissionHandoffDecision;
	private:
	private:
		static inline const std::vector<std::shared_ptr<MatrixTilePhase01AuthorityDecisionRow>> Phase01AuthorityDecisionRows = {std::make_shared<MatrixTilePhase01AuthorityDecisionRow>(L"MTILE_LOAD", 216, Phase01RetainedOpcodeRole, Phase01DescriptorAuthorityRole, Phase01PackageFeatureRole, Phase01MtileMemoryPolicyDecision, Phase01StatusCatalogPromotionRule, true, false), std::make_shared<MatrixTilePhase01AuthorityDecisionRow>(L"MTILE_STORE", 217, Phase01RetainedOpcodeRole, Phase01DescriptorAuthorityRole, Phase01PackageFeatureRole, Phase01MtileMemoryPolicyDecision, Phase01StatusCatalogPromotionRule, true, false), std::make_shared<MatrixTilePhase01AuthorityDecisionRow>(L"MTILE_MACC", 218, Phase01RetainedOpcodeRole, Phase01DescriptorAuthorityRole, Phase01PackageFeatureRole, AccumulatorTileAbiGateDecision, Phase01StatusCatalogPromotionRule, true, false), std::make_shared<MatrixTilePhase01AuthorityDecisionRow>(L"MTRANSPOSE", 219, Phase01RetainedOpcodeRole, Phase01DescriptorAuthorityRole, Phase01PackageFeatureRole, Phase01MtransposePolicyDecision, Phase01StatusCatalogPromotionRule, true, false)};
		static inline const std::vector<std::shared_ptr<MatrixTilePhase02StateDescriptorDecisionRow>> Phase02StateDescriptorDecisionRows = {std::make_shared<MatrixTilePhase02StateDescriptorDecisionRow>(L"architectural tile state owner", L"tile register namespace, lifetime, guest-visible contents, save/restore, replay/rollback owner", Phase02TileStateOwnerDecision, L"Phase12ClosedStatusCatalogAndCompilerHandoffRemain", true, true, false), std::make_shared<MatrixTilePhase02StateDescriptorDecisionRow>(L"canonical tile descriptor ABI", L"tile identity, rows, columns, element size, stride, layout, zero/invalid/reserved encodings", Phase02DescriptorCarrierDecision, L"Phase12ClosedStatusCatalogAndCompilerHandoffRemain", true, true, false)};
		static inline const std::vector<std::shared_ptr<MatrixTilePhase02InstructionDescriptorRequirementRow>> Phase02InstructionDescriptorRequirementRows = {std::make_shared<MatrixTilePhase02InstructionDescriptorRequirementRow>(L"MTILE_LOAD", Phase02TileStateOwnerDecision, Phase02DescriptorCarrierDecision, Phase02DescriptorValidationDecision, true, true, true), std::make_shared<MatrixTilePhase02InstructionDescriptorRequirementRow>(L"MTILE_STORE", Phase02TileStateOwnerDecision, Phase02DescriptorCarrierDecision, Phase02DescriptorValidationDecision, true, true, true), std::make_shared<MatrixTilePhase02InstructionDescriptorRequirementRow>(L"MTILE_MACC", Phase02TileStateOwnerDecision, Phase02DescriptorCarrierDecision, Phase02DescriptorValidationDecision, true, true, true), std::make_shared<MatrixTilePhase02InstructionDescriptorRequirementRow>(L"MTRANSPOSE", Phase02TileStateOwnerDecision, Phase02DescriptorCarrierDecision, Phase02DescriptorValidationDecision, true, true, true)};
		static inline const std::vector<std::shared_ptr<MatrixTilePhase03MemoryFaultDecisionRow>> Phase03MemoryFaultDecisionRows = {std::make_shared<MatrixTilePhase03MemoryFaultDecisionRow>(L"MTILE_LOAD", true, false, L"effective address, tile descriptor shape, alignment, page crossing, partial-fault capture, retire publication, replay/rollback", Phase03MemoryShapeFaultDecision, L"Phase12ClosedStatusCatalogAndCompilerHandoffRemain", true, true, true, true), std::make_shared<MatrixTilePhase03MemoryFaultDecisionRow>(L"MTILE_STORE", false, true, L"effective address, tile descriptor shape, alignment, page crossing, partial-fault capture, retire store commit, replay/rollback", Phase03MemoryShapeFaultDecision, L"Phase12ClosedStatusCatalogAndCompilerHandoffRemain", true, true, true, true)};
		static inline const std::vector<std::shared_ptr<MatrixTilePhase04SemanticAbiDecisionRow>> Phase04SemanticAbiDecisionRows = {std::make_shared<MatrixTilePhase04SemanticAbiDecisionRow>(L"MTILE_MACC", L"source tile operands, accumulator operand, output ownership, shape compatibility, dtype widening, signedness, saturation/wrap/exception behavior, invalid shape behavior, retire/replay", Phase04MaccAccumulatorDecision, L"Phase12ClosedStatusCatalogAndCompilerHandoffRemain", true, true, true, true), std::make_shared<MatrixTilePhase04SemanticAbiDecisionRow>(L"MTRANSPOSE", L"source and destination tile operands, in-place/out-of-place carrier, aliasing, layout permutation, shape transform, element preservation, reserved descriptor behavior, retire/replay", Phase04TransposeCarrierDecision, L"Phase12ClosedStatusCatalogAndCompilerHandoffRemain", true, true, true, true)};
		static inline const std::vector<std::shared_ptr<MatrixTilePhase05VlmDecisionRow>> Phase05VlmDecisionRows = {std::make_shared<MatrixTilePhase05VlmDecisionRow>(L"MTILE_LOAD", L"instruction legality, XMatrix feature gate, element widths, tile shape contour, memory layout modes, disabled/reserved row behavior, decoder rejection", Phase05RuntimeOwnedVlmRowsDecision, L"Phase12ClosedStatusCatalogAndCompilerHandoffRemain", true, true, true, true, true, true, true, true), std::make_shared<MatrixTilePhase05VlmDecisionRow>(L"MTILE_STORE", L"instruction legality, XMatrix feature gate, element widths, tile shape contour, memory layout modes, disabled/reserved row behavior, decoder rejection", Phase05RuntimeOwnedVlmRowsDecision, L"Phase12ClosedStatusCatalogAndCompilerHandoffRemain", true, true, true, true, true, true, true, true), std::make_shared<MatrixTilePhase05VlmDecisionRow>(L"MTILE_MACC", L"instruction legality, XMatrix feature gate, element widths, tile shape contour, accumulator shape/dtype legality, disabled/reserved row behavior, decoder rejection", Phase05RuntimeOwnedVlmRowsDecision, L"Phase12ClosedStatusCatalogAndCompilerHandoffRemain", true, true, true, true, true, true, true, true), std::make_shared<MatrixTilePhase05VlmDecisionRow>(L"MTRANSPOSE", L"instruction legality, XMatrix feature gate, element widths, tile shape contour, transpose alias/layout legality, disabled/reserved row behavior, decoder rejection", Phase05RuntimeOwnedVlmRowsDecision, L"Phase12ClosedStatusCatalogAndCompilerHandoffRemain", true, true, true, true, true, true, true, true)};
		static inline const std::vector<std::shared_ptr<MatrixTilePhase06DecoderEncoderDecisionRow>> Phase06DecoderEncoderDecisionRows = {std::make_shared<MatrixTilePhase06DecoderEncoderDecisionRow>(L"MTILE_LOAD", L"opcode or descriptor dispatch source, binary field layout, operand mapping, descriptor encoding and validation, reserved/malformed/disabled decode faults, encoder round-trip, package feature gate", Phase06DecoderEncoderDecision, L"Phase12ClosedStatusCatalogAndCompilerHandoffRemain", true, true, true, true, true, true), std::make_shared<MatrixTilePhase06DecoderEncoderDecisionRow>(L"MTILE_STORE", L"opcode or descriptor dispatch source, binary field layout, operand mapping, descriptor encoding and validation, reserved/malformed/disabled decode faults, encoder round-trip, package feature gate", Phase06DecoderEncoderDecision, L"Phase12ClosedStatusCatalogAndCompilerHandoffRemain", true, true, true, true, true, true), std::make_shared<MatrixTilePhase06DecoderEncoderDecisionRow>(L"MTILE_MACC", L"opcode or descriptor dispatch source, binary field layout, operand mapping, descriptor encoding and validation, reserved/malformed/disabled decode faults, encoder round-trip, package feature gate", Phase06DecoderEncoderDecision, L"Phase12ClosedStatusCatalogAndCompilerHandoffRemain", true, true, true, true, true, true), std::make_shared<MatrixTilePhase06DecoderEncoderDecisionRow>(L"MTRANSPOSE", L"opcode or descriptor dispatch source, binary field layout, operand mapping, descriptor encoding and validation, reserved/malformed/disabled decode faults, encoder round-trip, package feature gate", Phase06DecoderEncoderDecision, L"Phase12ClosedStatusCatalogAndCompilerHandoffRemain", true, true, true, true, true, true)};
		static inline const std::vector<std::shared_ptr<MatrixTilePhase07IrMaterializerDecisionRow>> Phase07IrMaterializerDecisionRows = {std::make_shared<MatrixTilePhase07IrMaterializerDecisionRow>(L"MTILE_LOAD", L"InstructionIR tile descriptor projection, load memory operand projection, registry entry, materializer factory, typed runtime object, descriptor validation result preservation", Phase07IrProjectionMaterializerDecision, L"Phase12ClosedStatusCatalogAndCompilerHandoffRemain", true, true, false, false, true, true, true, true, true), std::make_shared<MatrixTilePhase07IrMaterializerDecisionRow>(L"MTILE_STORE", L"InstructionIR tile descriptor projection, store memory operand projection, registry entry, materializer factory, typed runtime object, descriptor validation result preservation", Phase07IrProjectionMaterializerDecision, L"Phase12ClosedStatusCatalogAndCompilerHandoffRemain", true, true, false, false, true, true, true, true, true), std::make_shared<MatrixTilePhase07IrMaterializerDecisionRow>(L"MTILE_MACC", L"InstructionIR tile descriptor projection, accumulator operand projection, registry entry, materializer factory, typed runtime object, descriptor validation result preservation", Phase07IrProjectionMaterializerDecision, L"Phase12ClosedStatusCatalogAndCompilerHandoffRemain", true, false, true, false, true, true, true, true, true), std::make_shared<MatrixTilePhase07IrMaterializerDecisionRow>(L"MTRANSPOSE", L"InstructionIR tile descriptor projection, transpose policy projection, registry entry, materializer factory, typed runtime object, descriptor validation result preservation", Phase07IrProjectionMaterializerDecision, L"Phase12ClosedStatusCatalogAndCompilerHandoffRemain", true, false, false, true, true, true, true, true, true)};
		static inline const std::vector<std::shared_ptr<MatrixTilePhase08MicroOpSchedulerDecisionRow>> Phase08MicroOpSchedulerDecisionRows = {std::make_shared<MatrixTilePhase08MicroOpSchedulerDecisionRow>(L"MTILE_LOAD", L"typed MTILE_LOAD MicroOp, canonical tile descriptor, tile memory dependency metadata, tile register write dependency metadata, scheduler lane binding, issue constraints, memory and tile-state capture barriers", Phase08TypedTileMicroOpSchedulerLaneDecision, L"Phase12ClosedStatusCatalogAndCompilerHandoffRemain", true, true, false, true, true, true, false, true, true, true, true, true), std::make_shared<MatrixTilePhase08MicroOpSchedulerDecisionRow>(L"MTILE_STORE", L"typed MTILE_STORE MicroOp, canonical tile descriptor, tile memory dependency metadata, tile register read dependency metadata, scheduler lane binding, issue constraints, memory and tile-state capture barriers", Phase08TypedTileMicroOpSchedulerLaneDecision, L"Phase12ClosedStatusCatalogAndCompilerHandoffRemain", true, true, false, true, true, true, false, true, true, true, true, true), std::make_shared<MatrixTilePhase08MicroOpSchedulerDecisionRow>(L"MTILE_MACC", L"typed MTILE_MACC MicroOp, canonical tile descriptor, tile register read/write dependency metadata, accumulator dependency metadata, scheduler lane binding, issue constraints, tile-state capture barriers", Phase08TypedTileMicroOpSchedulerLaneDecision, L"Phase12ClosedStatusCatalogAndCompilerHandoffRemain", false, true, true, true, false, true, true, true, true, true, true, true), std::make_shared<MatrixTilePhase08MicroOpSchedulerDecisionRow>(L"MTRANSPOSE", L"typed MTRANSPOSE MicroOp, canonical tile descriptor, tile register read/write dependency metadata, transpose policy dependency metadata, scheduler lane binding, issue constraints, tile-state capture barriers", Phase08TypedTileMicroOpSchedulerLaneDecision, L"Phase12ClosedStatusCatalogAndCompilerHandoffRemain", false, true, false, true, false, true, false, true, true, true, true, true)};
		static inline const std::vector<std::wstring> RequiredExecutableEvidenceChain = {L"CAT", L"OP", L"DEC", L"IR", L"MAT", L"OBJ", L"UOP", L"SCH", L"RSC", L"EXE", L"RET", L"RPL", L"TST", L"GLD", L"NOE", L"HND"};
		static inline const std::vector<std::wstring> RequestedClosureGates = {L"tile execution model", L"tile descriptor ABI", L"memory-shape/fault model", L"accumulator tile ABI", L"transpose policy ABI", L"VLM closure", L"decoder/encoder ABI", L"IR projection", L"materializer", L"typed tile MicroOp", L"scheduler lane binding", L"tile-stream resource contour", L"execute/capture", L"retire", L"replay/rollback", L"runtime/ISA conformance tests", L"golden artifacts", L"runtime no-fallback/no-hidden-lowering regression evidence", L"positive compiler emission handoff package"};
		static inline const std::vector<std::wstring> BlockedProductionGates = {L"NoHostOwnedEvidencePublication", L"NoLane6DscFallback", L"NoGenericStreamEngineExecutionAuthority", L"NoLane7Fallback", L"NoExternalBackendFallback", L"NoVmxSpecificPath"};
		static inline const std::vector<std::wstring> TileExecutionModelBlockers = {};
		static inline const std::vector<std::wstring> TileDescriptorAbiBlockers = {};
		static inline const std::vector<std::wstring> MemoryFaultModelBlockers = {};
		static inline const std::vector<std::wstring> AccumulatorTileAbiBlockers = {};
		static inline const std::vector<std::wstring> TransposePolicyAbiBlockers = {};
		static inline const std::vector<std::wstring> VectorLegalityMatrixBlockers = {};
		static inline const std::vector<std::wstring> MicroOpSchedulerBlockers = {};
		static inline const std::vector<std::wstring> PositiveGoldenPublicationPrerequisites = {L"ArchitecturalTileStateOwner", L"CanonicalTileDescriptorAbi", L"MemoryShapeAndPartialFaultModel", L"AccumulatorTileAbi", L"TransposePolicyAbi", L"RuntimeOwnedVlmRows", L"DecoderEncoderAbi", L"InstructionIrTileProjection", L"RegistryMaterializerAuthority", L"TypedTileMicroOp", L"SchedulerLaneBinding", L"ExecutionCaptureSemantics", L"RetireOwnedPublicationOrCommit", L"ReplayRollbackConformance", L"RuntimeNoFallbackNoHiddenLoweringRegressionEvidence", L"ExecutableGoldenVectors"};
		static inline const std::vector<std::shared_ptr<MatrixTileRemainingRuntimeIsaTaskRow>> RemainingRuntimeIsaOpenTasks = {};
		static inline const std::vector<std::wstring> ClosedAsBlockedRuntimeIsaTaskPools = {L"TileStateAndDescriptorAbiAudit", L"Phase02TileStateAndDescriptorAbi", L"Phase03TileMemoryShapeFaultAbi", L"Phase04AccumulatorTransposeSemanticAbi", L"AccumulatorAndTransposeAbiAudit", L"Phase05RuntimeOwnedMatrixTileVlmRows", L"Phase06DecoderEncoderAbi", L"Phase07IrProjectionAndMaterializer", L"VectorLegalityMatrixDescriptorOnlyAudit", L"DecoderIrMaterializerUopFailClosedAudit", L"TypedTileMicroOpSchedulerLaneAudit", L"Phase09ExecuteCaptureSemantics", L"Phase10RetirePublicationAndCommit", L"Phase11ReplayRollbackConformance", L"Phase12PositiveExecutableGoldenArtifacts", L"Phase12RuntimeNoFallbackNoHiddenLoweringEvidence", L"ExecuteRetireReplayFailClosedAudit", L"NegativeGoldenManifest", L"PositiveGoldenArtifactManifest", L"Phase01OpcodeIdentityAuthorityAdr", L"CompilerVisibleNoEmissionBoundaryHandoff", L"Phase13PositiveStatusCatalogPromotion", L"Phase13PositiveCompilerEmissionHandoffPackage", L"Phase14TileStreamResourceContourCorrection"};
		static inline const std::vector<std::wstring> ExternalEvidenceNonAuthoritySources = {L"Lane6Dsc2Tile2DParser", L"Lane7MatMulDescriptor", L"Lane7AcceleratorTopology", L"VectorTranspose", L"VectorSegmentMemory", L"ScopedVdotWide"};
		static inline const std::vector<std::wstring> RuntimePipelineBlockers = {};

	public:

		static constexpr bool ClosesTileExecutionModel = true;
		static constexpr bool ClosesTileDescriptorAbi = true;
		static constexpr bool ClosesMemoryShapeFaultModel = true;
		static constexpr bool ClosesAccumulatorTileAbi = true;
		static constexpr bool ClosesTransposePolicyAbi = true;
		static constexpr bool ClosesVectorLegalityMatrix = true;
		static constexpr bool ClosesDecoderEncoderAbi = true;
		static constexpr bool ClosesInstructionIrProjection = true;
		static constexpr bool ClosesRegistryMaterializer = true;
		static constexpr bool ClosesTypedTileMicroOp = true;
		static constexpr bool ClosesSchedulerLaneBinding = true;
		static constexpr bool ClosesExecuteCapture = true;
		static constexpr bool ClosesRetirePublication = true;
		static constexpr bool ClosesReplayRollback = true;
		static constexpr bool ClosesGoldenArtifacts = true;
		static constexpr bool ClosesTileStreamResourceContour = true;

		static constexpr bool OpensDecoderEncoderAbi = true;
		static constexpr bool OpensInstructionIrProjection = true;
		static constexpr bool OpensRegistryMaterializer = true;
		static constexpr bool PublishesTypedTileMicroOp = true;
		static constexpr bool OpensSchedulerLaneBinding = true;
		static constexpr bool OpensExecutionCapture = true;
		static constexpr bool OpensRetireWritebackOrSideEffects = true;
		static constexpr bool OpensReplayRollback = true;
		static constexpr bool OpensCompilerHelper = false;
		static constexpr bool OpensVmxSpecificPath = false;
		static constexpr bool PublishesHostOwnedEvidence = false;
		static constexpr bool HasArchitecturalTileRegisterFile = true;
		static constexpr bool HasMemoryBackedTileStateModel = false;
		static constexpr bool HasCanonicalTileDescriptorCarrier = true;
		static constexpr bool HasTileLifetimeOwnershipPolicy = true;
		static constexpr bool HasTileElementTypePolicy = true;
		static constexpr bool HasTileShapePolicy = true;
		static constexpr bool HasTileMemoryAlignmentPolicy = true;
		static constexpr bool HasPartialFaultPolicy = true;
		static constexpr bool HasRetireOwnedTilePublicationPolicy = true;
		static constexpr bool HasAccumulatorTileAbi = MatrixTileAccumulatorAndTransposePolicyAbi::HasAccumulatorTileAbi;
		static constexpr bool HasAccumulatorTileStateOwner = MatrixTileAccumulatorAndTransposePolicyAbi::HasAccumulatorTileStateOwner;
		static constexpr bool HasAccumulatorTileFootprintPolicy = MatrixTileAccumulatorAndTransposePolicyAbi::HasAccumulatorTileFootprintPolicy;
		static constexpr bool HasAccumulatorDtypePolicy = MatrixTileAccumulatorAndTransposePolicyAbi::HasAccumulatorDtypePolicy;
		static constexpr bool HasMaccShapeCompatibilityPolicy = MatrixTileAccumulatorAndTransposePolicyAbi::HasMaccShapeCompatibilityPolicy;
		static constexpr bool HasMaccExceptionOrSaturationPolicy = MatrixTileAccumulatorAndTransposePolicyAbi::HasMaccExceptionOrSaturationPolicy;
		static constexpr bool HasTransposePolicyAbi = MatrixTileAccumulatorAndTransposePolicyAbi::HasTransposePolicyAbi;
		static constexpr bool HasTransposePolicyCarrier = MatrixTileAccumulatorAndTransposePolicyAbi::HasTransposePolicyCarrier;
		static constexpr bool HasTransposeSourceDestinationAliasPolicy = MatrixTileAccumulatorAndTransposePolicyAbi::HasTransposeSourceDestinationAliasPolicy;
		static constexpr bool HasInPlaceTransposePolicy = MatrixTileAccumulatorAndTransposePolicyAbi::HasInPlaceTransposePolicy;
		static constexpr bool HasTransposeLayoutPermutationPolicy = MatrixTileAccumulatorAndTransposePolicyAbi::HasTransposeLayoutPermutationPolicy;
		static bool getHasVectorLegalityMatrixRows();
		static bool getHasMatrixTileVlmContour();
		static constexpr bool IsReadyForCompilerUpdate = true;
		static constexpr bool HasExecutableRuntimeIsaClosure = true;
		static constexpr bool BlocksCompilerUpdate = false;
		static constexpr bool IsReadyForCompilerVisibleNoEmissionBoundary = false;
		static constexpr bool HasCompilerVisibleNoEmissionBoundaryPackage = true;
		static constexpr bool AllowsCompilerVisibleNoEmissionBoundaryWork = false;
		static constexpr bool BlocksCompilerVisibleNoEmissionBoundary = true;
		static constexpr bool IsReadyForPositiveCompilerEmissionHandoff = true;
		static constexpr bool HasPositiveCompilerEmissionHandoffPackage = true;
		static constexpr bool BlocksPositiveCompilerEmission = false;
		static constexpr bool BlocksCompilerHelperIrSelectionEmission = false;
		static constexpr bool Phase13ClosesCompilerVisibleNoEmissionBoundary = false;
		static constexpr bool Phase13KeepsPositiveCompilerEmissionBlocked = false;
		static constexpr bool Phase13KeepsCompilerCodeUnmodified = true;
		static constexpr bool Phase14WasFailClosedDuringCorrection = true;
		static constexpr bool Phase14SupersedesAllAluSchedulerClaim = true;
		static constexpr bool Phase14RegeneratedPlacementSensitiveGoldenEvidence = true;
		static constexpr bool Phase14CompilerHandoffWasSuspendedUntilClosure = true;
		static constexpr bool HasExplicitNumericPolicyAbi = true;
		static constexpr bool HasPhase15SupportedNumericProfileMatrix = true;
		static constexpr bool HasPhase16FormalMaccArithmetic = true;
		static constexpr bool HasPhase16LayoutPolicyAbi = true;
		static constexpr bool HasPhase17PolicyBoundCaptureIdentity = true;
		static constexpr bool HasPhase17PolicyBoundReplayIdentity = true;
		static constexpr bool HasPhase18MachineReadableNumericLayoutGoldenCorpus = true;
		static constexpr bool Phase19RuntimeNumericEvidenceReady = true;
		static constexpr bool Phase19CompilerCarrierConformanceReady = true;
		static constexpr bool Phase19CompilerNoFallbackConformanceReady = true;
		static constexpr bool Phase19CompilerRuntimeRejectionConformanceReady = true;
		static constexpr bool Phase19CompilerSidebandConformanceReady = true;
		static constexpr bool Phase19PackageReclosureReady = true;
		static constexpr bool Phase19RequiresCompilerOwnedSidebandBridge = false;
		static constexpr bool NumericSensitivePackageReadiness = true;
		static constexpr bool NumericSensitiveClosesGoldenArtifacts = true;
		static constexpr bool PositiveNumericHandoffReady = true;
		static constexpr bool StatusCatalogPromotionIsProvisionalDuringNumericReclosure = false;
		static constexpr bool HasMatrixTileMemoryResourceClass = true;
		static constexpr bool HasMatrixTileComputeResourceClass = true;
		static constexpr bool HasDedicatedMatrixTileStreamLane6 = true;
		static constexpr bool UsesDmaStreamComputeAuthority = false;
		static constexpr bool UsesGenericStreamEngineExecutionAuthority = false;
		static constexpr bool StreamTransportOwnsArchitecturalPublication = false;
		static constexpr bool HasNegativeGoldenManifest = true;
		static constexpr bool HasPositiveExecutableGoldenVectors = MatrixTilePositiveGoldenArtifactManifest::HasPositiveExecutableGoldenArtifacts;
		static constexpr bool BlocksPositiveGoldenPublication = false;
		static constexpr bool HasClosableRuntimeIsaTaskWithoutAdr = false;
		static constexpr bool HasRemainingRuntimeIsaOpenTasks = false;
		static constexpr bool HasExecutableStatusCatalogPromotion = true;
		static constexpr bool HasOpcodeOrDescriptorAuthority = true;
		static constexpr bool TreatsRetainedNumericOpcodeAsExecutableAuthority = false;
		static constexpr bool RequiresAdrForOpcodeOrDescriptorAuthority = false;
		static constexpr bool HasPhase01ProductionAuthoritySource = true;
		static constexpr bool Phase01AuthorityAdrBlocksPhase02 = false;
		static constexpr bool Phase01KeepsStatusCatalogOptionalDisabled = true;
		static constexpr bool Phase01KeepsDecoderEncoderFailClosed = true;
		static constexpr bool Phase01KeepsCompilerHandoffBlocked = true;
		static constexpr bool HasPhase02ArchitecturalTileStateOwner = MatrixTileArchitecturalTileStateAndDescriptorAbi::HasArchitecturalTileStateOwner;
		static constexpr bool HasPhase02CanonicalTileDescriptorAbi = MatrixTileArchitecturalTileStateAndDescriptorAbi::HasCanonicalTileDescriptorAbi;
		static constexpr bool HasPhase02RuntimeOwnedTileStateContract = MatrixTileArchitecturalTileStateAndDescriptorAbi::HasRuntimeOwnedTileStateContract;
		static constexpr bool HasPhase02TileDescriptorValidationHelpers = MatrixTileArchitecturalTileStateAndDescriptorAbi::HasTileDescriptorValidationHelpers;
		static constexpr bool HasPhase02ReservedDescriptorFailFastTests = MatrixTileArchitecturalTileStateAndDescriptorAbi::HasReservedDescriptorFailFastTests;
		static constexpr bool Phase02KeepsExternalDescriptorsNonAuthority = true;
		static constexpr bool Phase02KeepsHostOwnedEvidenceNonArchitectural = true;
		static constexpr bool Phase02KeepsCompilerHandoffBlocked = true;
		static constexpr bool Phase02BlocksPhase03 = false;
		static constexpr bool HasPhase03TileMemoryShapeFaultAbi = MatrixTileMemoryShapeAndFaultAbi::HasTileMemoryShapeFaultAbi;
		static constexpr bool HasPhase03EffectiveAddressSource = MatrixTileMemoryShapeAndFaultAbi::HasEffectiveAddressSource;
		static constexpr bool HasPhase03ShapeValidation = MatrixTileMemoryShapeAndFaultAbi::HasShapeValidation;
		static constexpr bool HasPhase03AddressOverflowValidation = MatrixTileMemoryShapeAndFaultAbi::HasAddressOverflowValidation;
		static constexpr bool HasPhase03AlignmentPolicy = MatrixTileMemoryShapeAndFaultAbi::HasAlignmentPolicy;
		static constexpr bool HasPhase03PageCrossingPolicy = MatrixTileMemoryShapeAndFaultAbi::HasPageCrossingPolicy;
		static constexpr bool HasPhase03PartialFaultPolicy = MatrixTileMemoryShapeAndFaultAbi::HasPartialFaultPolicy;
		static constexpr bool HasPhase03MemoryOrderingPolicy = MatrixTileMemoryShapeAndFaultAbi::HasMemoryOrderingPolicy;
		static constexpr bool HasPhase03LoadStoreSideEffectOwner = MatrixTileMemoryShapeAndFaultAbi::HasLoadStoreSideEffectOwner;
		static constexpr bool HasPhase03RetireRollbackPolicy = MatrixTileMemoryShapeAndFaultAbi::HasRetireRollbackPolicy;
		static constexpr bool Phase03KeepsEaFallbackNonAuthority = MatrixTileMemoryShapeAndFaultAbi::KeepsEaFallbackNonAuthority;
		static constexpr bool Phase03KeepsMemorySideEffectsUnopened = MatrixTileMemoryShapeAndFaultAbi::KeepsMemorySideEffectsUnopened;
		static constexpr bool Phase03KeepsCompilerHandoffBlocked = MatrixTileMemoryShapeAndFaultAbi::KeepsCompilerHandoffBlocked;
		static constexpr bool HasPhase04AccumulatorTileAbi = MatrixTileAccumulatorAndTransposePolicyAbi::HasAccumulatorTileAbi;
		static constexpr bool HasPhase04TransposePolicyAbi = MatrixTileAccumulatorAndTransposePolicyAbi::HasTransposePolicyAbi;
		static constexpr bool HasPhase04AccumulatorFootprint = MatrixTileAccumulatorAndTransposePolicyAbi::HasAccumulatorTileFootprintPolicy;
		static constexpr bool HasPhase04AccumulatorDtypePromotion = MatrixTileAccumulatorAndTransposePolicyAbi::HasAccumulatorDtypePolicy;
		static constexpr bool HasPhase04AccumulatorExceptionSaturationPolicy = MatrixTileAccumulatorAndTransposePolicyAbi::HasMaccExceptionOrSaturationPolicy;
		static constexpr bool HasPhase04AccumulatorShapeCompatibility = MatrixTileAccumulatorAndTransposePolicyAbi::HasMaccShapeCompatibilityPolicy;
		static constexpr bool HasPhase04TransposeSourceDestinationPolicy = MatrixTileAccumulatorAndTransposePolicyAbi::HasTransposeSourceDestinationAliasPolicy;
		static constexpr bool HasPhase04TransposeInPlaceAliasPolicy = MatrixTileAccumulatorAndTransposePolicyAbi::HasInPlaceTransposePolicy;
		static constexpr bool HasPhase04TransposeLayoutPermutationPolicy = MatrixTileAccumulatorAndTransposePolicyAbi::HasTransposeLayoutPermutationPolicy;
		static constexpr bool HasPhase04InvalidShapeTypeAliasDeterminism = MatrixTileAccumulatorAndTransposePolicyAbi::HasInvalidShapeTypeAliasDeterminism;
		static constexpr bool Phase04KeepsVectorTransposeNonAuthority = MatrixTileAccumulatorAndTransposePolicyAbi::KeepsVectorTransposeNonAuthority;
		static constexpr bool Phase04KeepsExternalBackendNonAuthority = MatrixTileAccumulatorAndTransposePolicyAbi::KeepsExternalBackendNonAuthority;
		static constexpr bool Phase04KeepsCompilerMatrixIrIndependent = MatrixTileAccumulatorAndTransposePolicyAbi::KeepsCompilerMatrixIrIndependent;
		static constexpr bool Phase04KeepsCompilerHandoffBlocked = MatrixTileAccumulatorAndTransposePolicyAbi::KeepsCompilerHandoffBlocked;
		static bool getHasPhase05RuntimeOwnedVlmRows();
		static bool getHasPhase05MatrixTileVlmFamily();
		static bool getHasPhase05DescriptorBackedOnlyContour();
		static bool getHasPhase05FeatureGate();
		static bool getHasPhase05ElementWidthLegality();
		static bool getHasPhase05TileShapeLegality();
		static bool getHasPhase05MemoryLayoutLegality();
		static bool getHasPhase05ReservedDisabledRows();
		static bool getPhase05KeepsNonDescriptorContoursFailClosed();
		static bool getPhase05KeepsExecutableContoursClosed();
		static constexpr bool Phase05KeepsClassifierMetadataNonAuthority = MatrixTileRuntimeOwnedVlmRows::KeepsClassifierMetadataNonAuthority;
		static constexpr bool Phase05KeepsOptionalDisabledStatusNonAuthority = MatrixTileRuntimeOwnedVlmRows::KeepsOptionalDisabledStatusNonAuthority;
		static constexpr bool Phase05KeepsDecoderAdmissionBlocked = MatrixTileRuntimeOwnedVlmRows::KeepsDecoderAdmissionBlocked;
		static constexpr bool Phase05KeepsCompilerEmissionIndependent = MatrixTileRuntimeOwnedVlmRows::KeepsCompilerEmissionIndependent;
		static constexpr bool Phase05KeepsCompilerHandoffBlocked = MatrixTileRuntimeOwnedVlmRows::KeepsCompilerHandoffBlocked;
		static constexpr bool HasPhase06CanonicalDecoderAcceptance = true;
		static constexpr bool HasPhase06EncoderAbi = true;
		static constexpr bool HasPhase06BinaryFieldLayout = true;
		static constexpr bool HasPhase06OperandFieldMapping = true;
		static constexpr bool HasPhase06DescriptorDecodeValidation = true;
		static constexpr bool HasPhase06ReservedMalformedFaultBehavior = true;
		static constexpr bool HasPhase06EncoderRoundTripTests = true;
		static constexpr bool HasPhase06PackageFeatureDecodeGate = true;
		static constexpr bool Phase06KeepsIllegalRowsBeforeIrMaterializer = true;
		static constexpr bool Phase06KeepsCompilerAcceptanceNonEvidence = true;
		static constexpr bool Phase06KeepsCompilerEmissionOutOfScope = true;
		static constexpr bool Phase06KeepsCompilerHandoffBlocked = true;
		static constexpr bool HasPhase07InstructionIrTileProjection = MatrixTileIrProjectionAndMaterializer::HasInstructionIrTileProjection;
		static constexpr bool HasPhase07TileDescriptorIrCarrier = MatrixTileIrProjectionAndMaterializer::HasTileDescriptorIrCarrier;
		static constexpr bool HasPhase07MemoryOperandProjection = MatrixTileIrProjectionAndMaterializer::HasMemoryOperandProjection;
		static constexpr bool HasPhase07AccumulatorOperandProjection = MatrixTileIrProjectionAndMaterializer::HasAccumulatorOperandProjection;
		static constexpr bool HasPhase07TransposePolicyProjection = MatrixTileIrProjectionAndMaterializer::HasTransposePolicyProjection;
		static constexpr bool HasPhase07RegistryEntries = MatrixTileIrProjectionAndMaterializer::HasRegistryEntries;
		static constexpr bool HasPhase07MaterializerFactories = MatrixTileIrProjectionAndMaterializer::HasMaterializerFactories;
		static constexpr bool HasPhase07MaterializedTypedRuntimeObjects = MatrixTileIrProjectionAndMaterializer::HasMaterializedTypedRuntimeObjects;
		static constexpr bool HasPhase07DescriptorValidationResultPreservation = MatrixTileIrProjectionAndMaterializer::HasDescriptorValidationResultPreservation;
		static constexpr bool HasPhase07InvalidIrProjectionFaults = MatrixTileIrProjectionAndMaterializer::HasInvalidIrProjectionFaults;
		static constexpr bool Phase07KeepsCompilerIrNonAuthority = MatrixTileIrProjectionAndMaterializer::KeepsCompilerIrNonAuthority;
		static constexpr bool Phase07KeepsCompilerScopeClosed = MatrixTileIrProjectionAndMaterializer::KeepsCompilerScopeClosed;
		static constexpr bool Phase07KeepsCompilerHandoffBlocked = MatrixTileIrProjectionAndMaterializer::KeepsCompilerHandoffBlocked;
		static constexpr bool HasPhase08TypedTileMicroOp = true;
		static constexpr bool HasPhase08TileMemoryDependencyMetadata = true;
		static constexpr bool HasPhase08TileRegisterDependencyMetadata = true;
		static constexpr bool HasPhase08AccumulatorDependencyMetadata = true;
		static constexpr bool HasPhase08TileDependencyModel = true;
		static constexpr bool HasPhase08SchedulerLaneBinding = true;
		static constexpr bool HasPhase08IssueConstraints = true;
		static constexpr bool HasPhase08CaptureBarriers = true;
		static constexpr bool HasPhase08VmxOrBackendFallback = false;
		static constexpr bool Phase08BlocksMicroOpCreationBeforeMaterializer = true;
		static constexpr bool Phase08BlocksSchedulerMaterializerVlmBypass = true;
		static constexpr bool Phase08KeepsVmxBackendFallbackNonAuthority = true;
		static constexpr bool Phase08KeepsCompilerScopeClosed = true;
		static constexpr bool Phase08KeepsCompilerHandoffBlocked = true;
		static constexpr bool HasPhase09ExecutionCaptureSemantics = MatrixTileExecuteCaptureAbi::HasExecutionCaptureSemantics;
		static constexpr bool HasPhase09TileLoadCaptureBuffer = MatrixTileExecuteCaptureAbi::HasTileLoadCaptureBuffer;
		static constexpr bool HasPhase09TileStorePendingWriteBuffer = MatrixTileExecuteCaptureAbi::HasTileStorePendingWriteBuffer;
		static constexpr bool HasPhase09MaccCaptureResult = MatrixTileExecuteCaptureAbi::HasMaccCaptureResult;
		static constexpr bool HasPhase09TransposeCaptureResult = MatrixTileExecuteCaptureAbi::HasTransposeCaptureResult;
		static constexpr bool HasPhase09DeterministicExceptionCapture = MatrixTileExecuteCaptureAbi::HasDeterministicExceptionCapture;
		static constexpr bool HasPhase09MemoryFaultCapture = MatrixTileExecuteCaptureAbi::HasMemoryFaultCapture;
		static constexpr bool HasPhase09TileStateReadSnapshot = MatrixTileExecuteCaptureAbi::HasTileStateReadSnapshot;
		static constexpr bool HasPhase09AccumulatorReadSnapshot = MatrixTileExecuteCaptureAbi::HasAccumulatorReadSnapshot;
		static constexpr bool HasPhase09RetirePublication = MatrixTileExecuteCaptureAbi::HasRetirePublication;
		static constexpr bool HasPhase09ReplayRollbackConformance = MatrixTileExecuteCaptureAbi::HasReplayRollbackConformance;
		static constexpr bool Phase09BlocksCaptureToRetireBypass = MatrixTileExecuteCaptureAbi::BlocksCaptureToRetireBypass;
		static constexpr bool Phase09KeepsRetirePublicationNonAuthority = MatrixTileExecuteCaptureAbi::KeepsRetirePublicationNonAuthority;
		static constexpr bool Phase09KeepsReplayRollbackNonAuthority = MatrixTileExecuteCaptureAbi::KeepsReplayRollbackNonAuthority;
		static constexpr bool Phase09KeepsVmxBackendFallbackNonAuthority = true;
		static constexpr bool Phase09KeepsCompilerScopeClosed = MatrixTileExecuteCaptureAbi::KeepsCompilerScopeClosed;
		static constexpr bool Phase09KeepsCompilerHandoffBlocked = MatrixTileExecuteCaptureAbi::KeepsCompilerHandoffBlocked;
		static constexpr bool HasPhase10RetirePublicationCommit = MatrixTileRetirePublicationAbi::HasRetirePublicationAndCommit;
		static constexpr bool HasPhase10TileLoadRetirePublication = MatrixTileRetirePublicationAbi::HasTileLoadRetirePublication;
		static constexpr bool HasPhase10TileStoreRetireCommit = MatrixTileRetirePublicationAbi::HasTileStoreRetireCommit;
		static constexpr bool HasPhase10AccumulatorRetirePublication = MatrixTileRetirePublicationAbi::HasAccumulatorRetirePublication;
		static constexpr bool HasPhase10TransposeRetirePublication = MatrixTileRetirePublicationAbi::HasTransposeRetirePublication;
		static constexpr bool HasPhase10FaultRetirementPolicy = MatrixTileRetirePublicationAbi::HasFaultRetirementPolicy;
		static constexpr bool HasPhase10WritebackOwnership = MatrixTileRetirePublicationAbi::HasWritebackOwnership;
		static constexpr bool HasPhase10SideEffectPublicationOrdering = MatrixTileRetirePublicationAbi::HasSideEffectPublicationOrdering;
		static constexpr bool HasPhase10ArchitecturalStateVisibilityRules = MatrixTileRetirePublicationAbi::HasArchitecturalStateVisibilityRules;
		static constexpr bool HasPhase10CaptureToRetireCorrelation = MatrixTileRetirePublicationAbi::HasCaptureToRetireCorrelation;
		static constexpr bool Phase10RejectsDuplicateRetire = MatrixTileRetirePublicationAbi::RejectsDuplicateRetire;
		static constexpr bool Phase10RejectsCancelledRetire = MatrixTileRetirePublicationAbi::RejectsCancelledRetire;
		static constexpr bool Phase10BlocksExecuteCaptureToRetireBypass = MatrixTileRetirePublicationAbi::BlocksExecuteCaptureToRetireBypass;
		static constexpr bool Phase10KeepsHostOwnedEvidenceNonArchitectural = MatrixTileRetirePublicationAbi::KeepsHostOwnedEvidenceNonArchitectural;
		static constexpr bool Phase10KeepsVmxBackendFallbackNonAuthority = true;
		static constexpr bool Phase10KeepsCompilerScopeClosed = MatrixTileRetirePublicationAbi::KeepsCompilerScopeClosed;
		static constexpr bool Phase10KeepsCompilerHandoffBlocked = true;
		static constexpr bool HasPhase11ReplayRollbackConformance = MatrixTileReplayRollbackAbi::HasReplayRollbackConformance;
		static constexpr bool HasPhase11DecodedInstructionReplayIdentity = MatrixTileReplayRollbackAbi::HasDecodedInstructionReplayIdentity;
		static constexpr bool HasPhase11TileDescriptorReplayIdentity = MatrixTileReplayRollbackAbi::HasTileDescriptorReplayIdentity;
		static constexpr bool HasPhase11PendingTileWriteRollback = MatrixTileReplayRollbackAbi::HasPendingTileWriteRollback;
		static constexpr bool HasPhase11PendingMemoryStoreRollback = MatrixTileReplayRollbackAbi::HasPendingMemoryStoreRollback;
		static constexpr bool HasPhase11AccumulatorRollback = MatrixTileReplayRollbackAbi::HasAccumulatorRollback;
		static constexpr bool HasPhase11DeterministicReplayAfterMemoryFault = MatrixTileReplayRollbackAbi::HasDeterministicReplayAfterMemoryFault;
		static constexpr bool HasPhase11DeterministicReplayAfterDescriptorFault = MatrixTileReplayRollbackAbi::HasDeterministicReplayAfterDescriptorFault;
		static constexpr bool HasPhase11LegalIllegalConformanceVectors = MatrixTileReplayRollbackAbi::HasLegalIllegalConformanceVectors;
		static constexpr bool Phase11BlocksReplayWithoutRetirePublication = MatrixTileReplayRollbackAbi::BlocksReplayWithoutRetirePublication;
		static constexpr bool Phase11BlocksCaptureRecordIdentityBypass = MatrixTileReplayRollbackAbi::BlocksCaptureRecordIdentityBypass;
		static constexpr bool Phase11KeepsVmxBackendFallbackNonAuthority = true;
		static constexpr bool Phase11KeepsCompilerScopeClosed = true;
		static constexpr bool Phase11KeepsCompilerHandoffBlocked = true;
		static constexpr bool HasPhase12PositiveExecutableGoldenArtifacts = MatrixTilePositiveGoldenArtifactManifest::HasPositiveExecutableGoldenArtifacts;
		static constexpr bool HasPhase12RuntimeNoFallbackNoHiddenLoweringRegressionEvidence = MatrixTilePositiveGoldenArtifactManifest::HasRuntimeNoFallbackNoHiddenLoweringRegressionEvidence;
		static constexpr bool HasPhase12DecodeEncodeGoldenVectors = MatrixTilePositiveGoldenArtifactManifest::HasLegalDecodeEncodeRoundTripVectors;
		static constexpr bool HasPhase12IrMaterializerGoldenVectors = MatrixTilePositiveGoldenArtifactManifest::HasLegalIrMaterializerProjectionVectors;
		static constexpr bool HasPhase12ExecuteRetireGoldenVectors = MatrixTilePositiveGoldenArtifactManifest::HasLegalExecuteRetireVectors;
		static constexpr bool HasPhase12MemoryFaultGoldenVectors = MatrixTilePositiveGoldenArtifactManifest::HasMemoryFaultVectors;
		static constexpr bool HasPhase12DescriptorFaultGoldenVectors = MatrixTilePositiveGoldenArtifactManifest::HasDescriptorFaultVectors;
		static constexpr bool HasPhase12AccumulatorGoldenVectors = MatrixTilePositiveGoldenArtifactManifest::HasAccumulatorVectors;
		static constexpr bool HasPhase12TransposeGoldenVectors = MatrixTilePositiveGoldenArtifactManifest::HasTransposeVectors;
		static constexpr bool HasPhase12ReplayRollbackGoldenVectors = MatrixTilePositiveGoldenArtifactManifest::HasReplayRollbackVectors;
		static constexpr bool HasPhase12ReservedRowGoldenVectors = MatrixTilePositiveGoldenArtifactManifest::HasNegativeReservedCarrierVectors;
		static constexpr bool Phase12BlocksPositiveGoldenPublication = false;
		static constexpr bool Phase12BlocksNoFallbackRegressionEvidenceBypass = true;
		static constexpr bool Phase12KeepsVmxBackendFallbackNonAuthority = true;
		static constexpr bool Phase12KeepsCompilerScopeClosed = true;
		static constexpr bool Phase12KeepsCompilerHandoffBlocked = true;
		static constexpr bool HasCanonicalDecoderAcceptance = true;
		static constexpr bool HasEncoderAbi = true;
		static constexpr bool HasInstructionIrTileProjection = true;
		static constexpr bool HasRegistryFactory = true;
		static constexpr bool HasMaterializerFactory = true;
		static constexpr bool HasTypedTileMicroOp = true;
		static constexpr bool HasSchedulerLaneBinding = true;
		static constexpr bool HasExecutionCaptureSemantics = true;
		static constexpr bool HasRetireWritebackOrSideEffectPublication = true;
		static constexpr bool HasReplayRollbackConformance = true;
		static constexpr bool HasGoldenArtifacts = MatrixTilePositiveGoldenArtifactManifest::HasPositiveExecutableGoldenArtifacts;
		static constexpr bool TreatsLane6Dsc2Tile2DAsAuthority = false;
		static constexpr bool TreatsLane7MatMulDescriptorAsAuthority = false;
		static constexpr bool TreatsMemoryEaFallbackAsTileExecutionAuthority = false;
		static constexpr bool TreatsInstructionClassifierAsExecutionAuthority = false;
		static constexpr bool HasCompilerHandoffPackage = true;
		static constexpr bool HasCurrentCompilerImplementation = MatrixTileCompilerEmissionHandoffPackage::CurrentCompilerImplementationExists;

	private:
		static inline const std::vector<std::shared_ptr<MatrixTileRuntimeIsaPackageRow>> RowTable = {std::make_shared<MatrixTileRuntimeIsaPackageRow>(L"MTILE_LOAD", true, true, false, false, false, true, false, false, true, false, L"NonePhase14Closed"), std::make_shared<MatrixTileRuntimeIsaPackageRow>(L"MTILE_STORE", true, false, true, false, false, true, false, false, false, true, L"NonePhase14Closed"), std::make_shared<MatrixTileRuntimeIsaPackageRow>(L"MTILE_MACC", false, false, false, true, false, false, true, false, true, false, L"NonePhase14Closed"), std::make_shared<MatrixTileRuntimeIsaPackageRow>(L"MTRANSPOSE", false, false, false, false, true, false, false, true, true, false, L"NonePhase14Closed")};

	public:
		static std::vector<std::shared_ptr<MatrixTileRuntimeIsaPackageRow>> getRows();

	private:
		static inline const std::vector<std::shared_ptr<MatrixTileOpcodeAuthorityRow>> OpcodeAuthorityRowTable = {std::make_shared<MatrixTileOpcodeAuthorityRow>(L"MTILE_LOAD", 216, true, StatusCatalogAuthorityDecision, true, true, false, false), std::make_shared<MatrixTileOpcodeAuthorityRow>(L"MTILE_STORE", 217, true, StatusCatalogAuthorityDecision, true, true, false, false), std::make_shared<MatrixTileOpcodeAuthorityRow>(L"MTILE_MACC", 218, true, StatusCatalogAuthorityDecision, true, true, false, false), std::make_shared<MatrixTileOpcodeAuthorityRow>(L"MTRANSPOSE", 219, true, StatusCatalogAuthorityDecision, true, true, false, false)};

	public:
		static std::vector<std::shared_ptr<MatrixTileOpcodeAuthorityRow>> getOpcodeAuthorityRows();

		static std::vector<std::shared_ptr<MatrixTilePhase01AuthorityDecisionRow>> getPhase01AuthorityDecisionRows();

		static std::vector<std::shared_ptr<MatrixTilePhase02StateDescriptorDecisionRow>> getPhase02StateDescriptorDecisionRows();

		static std::vector<std::shared_ptr<MatrixTilePhase02InstructionDescriptorRequirementRow>> getPhase02InstructionDescriptorRequirementRows();

		static std::vector<std::shared_ptr<MatrixTilePhase03MemoryFaultDecisionRow>> getPhase03MemoryFaultDecisionRows();

		static std::vector<std::shared_ptr<MatrixTilePhase04SemanticAbiDecisionRow>> getPhase04SemanticAbiDecisionRows();

		static std::vector<std::shared_ptr<MatrixTilePhase05VlmDecisionRow>> getPhase05VlmDecisionRows();

		static std::vector<std::shared_ptr<MatrixTilePhase06DecoderEncoderDecisionRow>> getPhase06DecoderEncoderDecisionRows();

		static std::vector<std::shared_ptr<MatrixTilePhase07IrMaterializerDecisionRow>> getPhase07IrMaterializerDecisionRows();

		static std::vector<std::shared_ptr<MatrixTilePhase08MicroOpSchedulerDecisionRow>> getPhase08MicroOpSchedulerDecisionRows();

	private:
		static inline const std::vector<std::shared_ptr<MatrixTilePhase09ExecuteCaptureDecisionRow>> Phase09ExecuteCaptureDecisionRowTable = {std::make_shared<MatrixTilePhase09ExecuteCaptureDecisionRow>(L"MTILE_LOAD", L"typed MTILE_LOAD MicroOp, tile load capture buffer, deterministic exception capture, memory fault capture, tile state read snapshot, retire-owned publication, replayable fault capture", Phase09ExecuteCaptureSemanticsDecision, L"HistoricalPhase09BoundaryRetireReplayGoldenAndPromotionOpen", true, false, false, false, true, true, true, false, true, true, false, false, false, true, true, true, false, false, false, true, true), std::make_shared<MatrixTilePhase09ExecuteCaptureDecisionRow>(L"MTILE_STORE", L"typed MTILE_STORE MicroOp, tile store pending write buffer, deterministic exception capture, memory fault capture, tile state read snapshot, retire-owned publication, replayable fault capture", Phase09ExecuteCaptureSemanticsDecision, L"HistoricalPhase09BoundaryRetireReplayGoldenAndPromotionOpen", false, true, false, false, true, true, true, false, true, false, true, false, false, true, true, true, false, false, false, true, true), std::make_shared<MatrixTilePhase09ExecuteCaptureDecisionRow>(L"MTILE_MACC", L"typed MTILE_MACC MicroOp, matrix MACC capture result, accumulator read snapshot, deterministic exception capture, memory fault capture, retire-owned publication, replayable fault capture", Phase09ExecuteCaptureSemanticsDecision, L"HistoricalPhase09BoundaryRetireReplayGoldenAndPromotionOpen", false, false, true, false, true, true, true, true, true, false, false, true, false, true, true, true, true, false, false, true, true), std::make_shared<MatrixTilePhase09ExecuteCaptureDecisionRow>(L"MTRANSPOSE", L"typed MTRANSPOSE MicroOp, matrix transpose capture result, tile state read snapshot, deterministic exception capture, memory fault capture, retire-owned publication, replayable fault capture", Phase09ExecuteCaptureSemanticsDecision, L"HistoricalPhase09BoundaryRetireReplayGoldenAndPromotionOpen", false, false, false, true, true, true, true, false, true, false, false, false, true, true, true, true, false, false, false, true, true)};

	public:
		static std::vector<std::shared_ptr<MatrixTilePhase09ExecuteCaptureDecisionRow>> getPhase09ExecuteCaptureDecisionRows();

	private:
		static inline const std::vector<std::shared_ptr<MatrixTilePhase10RetirePublicationDecisionRow>> Phase10RetirePublicationDecisionRowTable = {std::make_shared<MatrixTilePhase10RetirePublicationDecisionRow>(L"MTILE_LOAD", L"retire-owned tile load publication path, fault retirement policy, writeback ownership, side-effect publication ordering, architectural tile-state visibility rules", Phase10RetirePublicationCommitDecision, L"NonePhase10Closed", true, false, false, false, true, true, true, true, true, true, false, false, false, true, true, true, true, true, true, true), std::make_shared<MatrixTilePhase10RetirePublicationDecisionRow>(L"MTILE_STORE", L"retire-owned tile store commit path, fault retirement policy, writeback ownership, side-effect publication ordering, architectural memory visibility rules", Phase10RetirePublicationCommitDecision, L"NonePhase10Closed", false, true, false, false, true, true, true, true, true, false, true, false, false, true, true, true, true, true, true, true), std::make_shared<MatrixTilePhase10RetirePublicationDecisionRow>(L"MTILE_MACC", L"retire-owned accumulator update publication path, fault retirement policy, writeback ownership, side-effect publication ordering, architectural accumulator visibility rules", Phase10RetirePublicationCommitDecision, L"NonePhase10Closed", false, false, true, false, true, true, true, true, true, false, false, true, false, true, true, true, true, true, true, true), std::make_shared<MatrixTilePhase10RetirePublicationDecisionRow>(L"MTRANSPOSE", L"retire-owned transpose result publication path, fault retirement policy, writeback ownership, side-effect publication ordering, architectural tile-state visibility rules", Phase10RetirePublicationCommitDecision, L"NonePhase10Closed", false, false, false, true, true, true, true, true, true, false, false, false, true, true, true, true, true, true, true, true)};

	public:
		static std::vector<std::shared_ptr<MatrixTilePhase10RetirePublicationDecisionRow>> getPhase10RetirePublicationDecisionRows();

	private:
		static inline const std::vector<std::shared_ptr<MatrixTilePhase11ReplayRollbackDecisionRow>> Phase11ReplayRollbackDecisionRowTable = {std::make_shared<MatrixTilePhase11ReplayRollbackDecisionRow>(L"MTILE_LOAD", L"decoded MTILE_LOAD replay identity, tile descriptor replay identity, pending tile write rollback, deterministic memory-fault replay, descriptor-fault replay, legal/illegal conformance vectors", Phase11ReplayRollbackConformanceDecision, L"NonePhase11Closed", true, true, true, false, false, true, true, true, true, true, true, true, false, false, true, true, true, true, true, true), std::make_shared<MatrixTilePhase11ReplayRollbackDecisionRow>(L"MTILE_STORE", L"decoded MTILE_STORE replay identity, tile descriptor replay identity, pending memory store rollback, deterministic memory-fault replay, descriptor-fault replay, legal/illegal conformance vectors", Phase11ReplayRollbackConformanceDecision, L"NonePhase11Closed", true, true, false, true, false, true, true, true, true, true, true, false, true, false, true, true, true, true, true, true), std::make_shared<MatrixTilePhase11ReplayRollbackDecisionRow>(L"MTILE_MACC", L"decoded MTILE_MACC replay identity, tile descriptor replay identity, accumulator rollback, deterministic descriptor-fault replay, legal/illegal conformance vectors", Phase11ReplayRollbackConformanceDecision, L"NonePhase11Closed", true, true, false, false, true, false, true, true, true, true, true, false, false, true, false, true, true, true, true, true), std::make_shared<MatrixTilePhase11ReplayRollbackDecisionRow>(L"MTRANSPOSE", L"decoded MTRANSPOSE replay identity, tile descriptor replay identity, pending tile write rollback, deterministic descriptor-fault replay, legal/illegal conformance vectors", Phase11ReplayRollbackConformanceDecision, L"NonePhase11Closed", true, true, true, false, false, false, true, true, true, true, true, true, false, false, false, true, true, true, true, true)};

	public:
		static std::vector<std::shared_ptr<MatrixTilePhase11ReplayRollbackDecisionRow>> getPhase11ReplayRollbackDecisionRows();

	private:
		static inline const std::vector<std::shared_ptr<MatrixTilePhase12GoldenArtifactDecisionRow>> Phase12GoldenArtifactDecisionRowTable = {std::make_shared<MatrixTilePhase12GoldenArtifactDecisionRow>(L"MTILE_LOAD", L"positive decode/encode round-trip vectors, IR/materializer projection vectors, execute/retire vectors, memory-fault vectors, descriptor-fault vectors, replay/rollback vectors, reserved-row vectors, runtime no-fallback/no-hidden-lowering evidence", Phase12PositiveExecutableGoldenArtifactsDecision, Phase12RuntimeNoFallbackNoHiddenLoweringRegressionDecision, L"NonePhase12Closed", true, true, true, true, true, false, false, true, true, true, true, true, true, true, true, true, true, false, false, true, true, false, true, true, true, true), std::make_shared<MatrixTilePhase12GoldenArtifactDecisionRow>(L"MTILE_STORE", L"positive decode/encode round-trip vectors, IR/materializer projection vectors, execute/retire vectors, memory-fault vectors, descriptor-fault vectors, replay/rollback vectors, reserved-row vectors, runtime no-fallback/no-hidden-lowering evidence", Phase12PositiveExecutableGoldenArtifactsDecision, Phase12RuntimeNoFallbackNoHiddenLoweringRegressionDecision, L"NonePhase12Closed", true, true, true, true, true, false, false, true, true, true, true, true, true, true, true, true, true, false, false, true, true, false, true, true, true, true), std::make_shared<MatrixTilePhase12GoldenArtifactDecisionRow>(L"MTILE_MACC", L"positive decode/encode round-trip vectors, IR/materializer projection vectors, execute/retire vectors, accumulator vectors, descriptor-fault vectors, replay/rollback vectors, reserved-row vectors, runtime no-fallback/no-hidden-lowering evidence", Phase12PositiveExecutableGoldenArtifactsDecision, Phase12RuntimeNoFallbackNoHiddenLoweringRegressionDecision, L"NonePhase12Closed", true, true, true, false, true, true, false, true, true, true, true, true, true, true, true, false, true, true, false, true, true, false, true, true, true, true), std::make_shared<MatrixTilePhase12GoldenArtifactDecisionRow>(L"MTRANSPOSE", L"positive decode/encode round-trip vectors, IR/materializer projection vectors, execute/retire vectors, transpose vectors, descriptor-fault vectors, replay/rollback vectors, reserved-row vectors, runtime no-fallback/no-hidden-lowering evidence", Phase12PositiveExecutableGoldenArtifactsDecision, Phase12RuntimeNoFallbackNoHiddenLoweringRegressionDecision, L"NonePhase12Closed", true, true, true, false, true, false, true, true, true, true, true, true, true, true, true, false, true, false, true, true, true, false, true, true, true, true)};

	public:
		static std::vector<std::shared_ptr<MatrixTilePhase12GoldenArtifactDecisionRow>> getPhase12GoldenArtifactDecisionRows();

		static std::vector<std::wstring> getRequiredExecutableEvidenceChain();

		static std::vector<std::wstring> getRequestedClosureGates();

		static std::vector<std::wstring> getBlockedProductionGates();

		static std::vector<std::wstring> getTileExecutionModelBlockers();

		static std::vector<std::wstring> getTileDescriptorAbiBlockers();

		static std::vector<std::wstring> getMemoryFaultModelBlockers();

		static std::vector<std::wstring> getAccumulatorTileAbiBlockers();

		static std::vector<std::wstring> getTransposePolicyAbiBlockers();

		static std::vector<std::wstring> getVectorLegalityMatrixBlockers();

		static std::vector<std::wstring> getMicroOpSchedulerBlockers();

	private:
		static inline const std::vector<std::shared_ptr<MatrixTileRuntimeIsaClosureGateRow>> ClosureGateRowTable = {std::make_shared<MatrixTileRuntimeIsaClosureGateRow>(L"status/catalog promotion", L"CAT", true, false, MatrixTileCompilerEmissionHandoffPackage::StatusCatalogDecision), std::make_shared<MatrixTileRuntimeIsaClosureGateRow>(L"opcode/descriptor authority", L"OP", true, false, L"ClosedExecutablePackageOpcodeAuthority"), std::make_shared<MatrixTileRuntimeIsaClosureGateRow>(L"tile state/descriptor ABI", L"ABI", true, false, L"ClosedRuntimeOwnedTileStateAndDescriptorAbi"), std::make_shared<MatrixTileRuntimeIsaClosureGateRow>(L"memory-shape/fault model", L"ABI", true, false, L"ClosedTileMemoryShapeAndFaultAbi"), std::make_shared<MatrixTileRuntimeIsaClosureGateRow>(L"accumulator/transpose ABI", L"ABI", true, false, L"ClosedAccumulatorAndTransposeSemanticAbi"), std::make_shared<MatrixTileRuntimeIsaClosureGateRow>(L"VLM rows", L"VLM", true, false, MatrixTileRuntimeOwnedVlmRows::RuntimeOwnedVlmRowsDecision), std::make_shared<MatrixTileRuntimeIsaClosureGateRow>(L"decoder/encoder", L"DEC", true, false, Phase06DecoderEncoderDecision), std::make_shared<MatrixTileRuntimeIsaClosureGateRow>(L"IR projection", L"IR", true, false, MatrixTileIrProjectionAndMaterializer::TileDescriptorProjectionDecision), std::make_shared<MatrixTileRuntimeIsaClosureGateRow>(L"materializer", L"MAT", true, false, MatrixTileIrProjectionAndMaterializer::MaterializerFactoryDecision), std::make_shared<MatrixTileRuntimeIsaClosureGateRow>(L"typed tile MicroOp", L"UOP", true, false, TypedTileMicroOpGateDecision), std::make_shared<MatrixTileRuntimeIsaClosureGateRow>(L"scheduler lane", L"SCH", true, false, SchedulerLaneBindingGateDecision), std::make_shared<MatrixTileRuntimeIsaClosureGateRow>(L"tile-stream resource contour", L"RSC", true, false, Phase14ReadinessDecision), std::make_shared<MatrixTileRuntimeIsaClosureGateRow>(L"execute/capture", L"EXE", true, false, MatrixTileExecuteCaptureAbi::ExecuteCaptureDecision), std::make_shared<MatrixTileRuntimeIsaClosureGateRow>(L"retire", L"RET", true, false, MatrixTileRetirePublicationAbi::RetirePublicationDecision), std::make_shared<MatrixTileRuntimeIsaClosureGateRow>(L"replay/rollback", L"RPL", true, false, MatrixTileReplayRollbackAbi::ReplayRollbackDecision), std::make_shared<MatrixTileRuntimeIsaClosureGateRow>(L"runtime/ISA conformance tests", L"TST", true, false, L"ClosedMatrixTilePhase11ConformanceVectors"), std::make_shared<MatrixTileRuntimeIsaClosureGateRow>(L"golden artifacts", L"GLD", true, false, MatrixTilePositiveGoldenArtifactManifest::ManifestDecision), std::make_shared<MatrixTileRuntimeIsaClosureGateRow>(L"runtime no-fallback/no-hidden-lowering regression evidence", L"NOE", true, false, MatrixTileNoFallbackEvidenceContract::EvidenceDecision), std::make_shared<MatrixTileRuntimeIsaClosureGateRow>(L"positive compiler emission handoff package", L"HND", true, false, MatrixTileCompilerEmissionHandoffPackage::HandoffDecision)};

	public:
		static std::vector<std::shared_ptr<MatrixTileRuntimeIsaClosureGateRow>> getClosureGateRows();

		static std::vector<std::wstring> getPositiveGoldenPublicationPrerequisites();

		static std::vector<std::shared_ptr<MatrixTileRemainingRuntimeIsaTaskRow>> getRemainingRuntimeIsaOpenTasks();

		static std::vector<std::wstring> getClosedAsBlockedRuntimeIsaTaskPools();

		static std::vector<std::wstring> getExternalEvidenceNonAuthoritySources();

		static std::vector<std::wstring> getRuntimePipelineBlockers();

		static std::shared_ptr<MatrixTileRuntimeIsaPackageRow> GetRow(const std::wstring &mnemonic);

		static bool ContainsRow(const std::wstring &mnemonic);

		static void RequireExecutableAuthority(const std::wstring &mnemonic);

		static void RequireCompilerUpdateReadiness();

		static void RequirePositiveCompilerEmissionReadiness();

		static void RequireCompilerVisibleNoEmissionBoundaryReadiness();
	};

}
