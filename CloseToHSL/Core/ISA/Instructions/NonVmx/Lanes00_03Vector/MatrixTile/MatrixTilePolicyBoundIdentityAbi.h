#pragma once

#include "MatrixTileIrProjectionAndMaterializer.h"
#include "MatrixTileRetirePublicationAbi.h"
#include "MatrixTileResourceContour.h"
#include "../../../../../Pipeline/Scheduling/SlotLegality/SlotClassDefinitions.h"
#include <string>
#include <limits>
#include <optional>
#include <memory>

namespace YAKSys_Hybrid_CPU::Core { struct MatrixTileMicroOpDependencyMetadata; }

using namespace YAKSys_Hybrid_CPU::Core;
using namespace YAKSys_Hybrid_CPU::Core::Pipeline::MicroOps;

namespace YAKSys_Hybrid_CPU::CloseToHSL::Core::ISA::Instructions::NonVmx::Lanes00_03Vector::MatrixTile
{
	struct MatrixTilePolicyBoundCaptureIdentity final
	{
		const unsigned short AbiVersion;
		const int OwnerThreadId;
		const int MemoryDomainId;
		const unsigned int Opcode;
		const MatrixTileProjectedOperationKind OperationKind;
		const unsigned short NumericPolicyAbiVersion;
		const unsigned long long NumericPolicyFingerprint;
		const unsigned short LayoutPolicyAbiVersion;
		const unsigned long long LayoutPolicyFingerprint;
		const unsigned long long DescriptorFingerprint;
		const MatrixTileRuntimeResourceClass ResourceClass;
		const YAKSys_Hybrid_CPU::Core::SlotClass SlotClass;
		const unsigned char EligibleLaneMask;
		const unsigned char PhysicalLaneId;
		const unsigned long long SourceSnapshotFingerprint;
		const unsigned long long SecondarySnapshotFingerprint;
		const unsigned long long AccumulatorSnapshotFingerprint;
		const unsigned long long FootprintBytes;
		const unsigned long long ReplayEpoch;
		const unsigned long long DependencyFingerprint;
		const MatrixTileRetirePublicationKind PublicationSurface;
		const unsigned long long IdentityFingerprint;
		const bool IsValid;

		MatrixTilePolicyBoundCaptureIdentity(
			unsigned short abiVersion, int ownerThreadId, int memoryDomainId,
			unsigned int opcode, MatrixTileProjectedOperationKind operationKind,
			unsigned short numericPolicyAbiVersion, unsigned long long numericPolicyFingerprint,
			unsigned short layoutPolicyAbiVersion, unsigned long long layoutPolicyFingerprint,
			unsigned long long descriptorFingerprint, MatrixTileRuntimeResourceClass resourceClass,
			YAKSys_Hybrid_CPU::Core::SlotClass slotClass, unsigned char eligibleLaneMask, unsigned char physicalLaneId,
			unsigned long long sourceSnapshotFingerprint, unsigned long long secondarySnapshotFingerprint,
			unsigned long long accumulatorSnapshotFingerprint, unsigned long long footprintBytes,
			unsigned long long replayEpoch, unsigned long long dependencyFingerprint,
			MatrixTileRetirePublicationKind publicationSurface, unsigned long long identityFingerprint);

		bool operator==(const MatrixTilePolicyBoundCaptureIdentity &) const = default;
	};

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct MatrixTilePolicyBoundCaptureIdentity(ushort AbiVersion, int OwnerThreadId, int MemoryDomainId, uint Opcode, MatrixTileProjectedOperationKind OperationKind, ushort NumericPolicyAbiVersion, ulong NumericPolicyFingerprint, ushort LayoutPolicyAbiVersion, ulong LayoutPolicyFingerprint, ulong DescriptorFingerprint, MatrixTileRuntimeResourceClass ResourceClass, SlotClass SlotClass, byte EligibleLaneMask, byte PhysicalLaneId, ulong SourceSnapshotFingerprint, ulong SecondarySnapshotFingerprint, ulong AccumulatorSnapshotFingerprint, ulong FootprintBytes, ulong ReplayEpoch, ulong DependencyFingerprint, MatrixTileRetirePublicationKind PublicationSurface, ulong IdentityFingerprint)
//	{
//		public bool IsValid
//		{
//			get
//			{
//				return AbiVersion == MatrixTilePolicyBoundIdentityAbi.CurrentAbiVersion && OwnerThreadId is >= 0 @and < Processor.CPU_Core.SmtWays && MemoryDomainId == OwnerThreadId && Opcode != 0 && OperationKind != MatrixTileProjectedOperationKind.Unspecified && DescriptorFingerprint != 0 && ResourceClass != MatrixTileRuntimeResourceClass.None && SlotClass != SlotClass.Unclassified && EligibleLaneMask != 0 && FootprintBytes != 0 && ReplayEpoch != 0 && DependencyFingerprint != 0 && PublicationSurface != MatrixTileRetirePublicationKind.None && IdentityFingerprint != 0 && (OperationKind != MatrixTileProjectedOperationKind.Macc || (NumericPolicyAbiVersion == MatrixTileNumericPolicyAbi.CurrentAbiVersion && NumericPolicyFingerprint != 0 && LayoutPolicyAbiVersion == MatrixTileLayoutPolicyAbi.CurrentAbiVersion && LayoutPolicyFingerprint != 0)) && (OperationKind != MatrixTileProjectedOperationKind.Transpose || (LayoutPolicyAbiVersion == MatrixTileLayoutPolicyAbi.CurrentAbiVersion && LayoutPolicyFingerprint != 0));
//			}
//		}
//	}

	class MatrixTilePolicyBoundIdentityAbi final
	{
	public:
		static constexpr unsigned short CurrentAbiVersion = 1;
		static inline const std::wstring CaptureIdentityDecision = L"PolicyDescriptorResourceDependencyEpochAndPublicationBoundCaptureIdentity";
		static inline const std::wstring ReplayIdentityDecision = L"NumericAndLayoutPolicyFingerprintsAreInseparableFromReplayIdentity";
		static inline const std::wstring EpochDecision = L"CoreOwnedMatrixTileReplayInvalidationEpoch";
		static constexpr bool UsesExistingRetirePublication = true;
		static constexpr bool UsesExistingReplayRollbackJournal = true;
		static constexpr bool UsesSrfOrHostEvidenceAsAuthority = false;

		static std::shared_ptr<MatrixTileExecutionCaptureRecord> Bind(std::shared_ptr<MatrixTileExecutionCaptureRecord> capture, std::shared_ptr<MatrixTileMaterializedInstruction> materializedInstruction, std::shared_ptr<MatrixTileMicroOpDependencyMetadata> dependency, int ownerThreadId, int memoryDomainId, unsigned long long replayEpoch);

		static bool ValidateCapture(std::shared_ptr<MatrixTileExecutionCaptureRecord> capture, unsigned long long currentReplayEpoch);

		static bool MatchesMaterializedInstruction(std::shared_ptr<MatrixTilePolicyBoundCaptureIdentity> identity, std::shared_ptr<MatrixTileMaterializedInstruction> materializedInstruction);

		static unsigned long long ComputeDependencyFingerprint(std::shared_ptr<MatrixTileMicroOpDependencyMetadata> dependency);

		static unsigned long long ComputeDependencyFingerprint(std::shared_ptr<MatrixTileMaterializedInstruction> materializedInstruction);

		static unsigned long long ComputeDescriptorFingerprint(std::shared_ptr<MatrixTileMaterializedInstruction> materializedInstruction);

		static unsigned long long ComputeDescriptorFingerprint(std::shared_ptr<MatrixTileExecutionCaptureRecord> capture);

		static unsigned long long ComputeImageFingerprint(std::shared_ptr<MatrixTileTileImage> image);

		static unsigned long long ComputeIdentityFingerprint(std::shared_ptr<MatrixTilePolicyBoundCaptureIdentity> identity);

	private:
		static MatrixTileRetirePublicationKind GetPublicationSurface(MatrixTileProjectedOperationKind operationKind);

		static std::shared_ptr<MatrixTileMicroOpDependencyMetadata> CreateDependencyMetadata(std::shared_ptr<MatrixTileInstructionIrProjection> projection);

		static unsigned short GetTileId(unsigned long long pointer);

		static unsigned short GetHighTileIdOrDefault(unsigned long long pointer, unsigned short fallback);

		static unsigned long long BeginHash();

		static unsigned long long FinishHash(unsigned long long hash);

		static void AddBool(unsigned long long &hash, bool value);

		static void AddByte(unsigned long long &hash, unsigned char value);

		static void AddUInt16(unsigned long long &hash, unsigned short value);

		static void AddUInt32(unsigned long long &hash, unsigned int value);

		static void AddUInt64(unsigned long long &hash, unsigned long long value);

		static void AddDescriptor(unsigned long long &hash, std::shared_ptr<MatrixTileCanonicalDescriptorAbi> descriptor);

		static void AddDescriptor(unsigned long long &hash, const MatrixTileCanonicalDescriptorAbi &descriptor);
	};

}
