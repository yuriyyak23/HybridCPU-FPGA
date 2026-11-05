#include "MatrixTilePolicyBoundIdentityAbi.h"
#include "MatrixTileResourceContour.h"
#include "../../../../../Pipeline/Scheduling/SlotLegality/SlotClassDefinitions.h"
#include "MatrixTileExecuteCaptureAbi.h"
#include "MatrixTileLayoutPolicyAbi.h"
#include "MatrixTileNumericPolicyAbi.h"
#include "../../../../../Pipeline/MicroOps/MatrixTile/MatrixTileMicroOps.h"
#include "../../../../../Pipeline/MicroOps/IR/InstructionIR.h"
#include <stdexcept>

using namespace YAKSys_Hybrid_CPU::Core;
using namespace YAKSys_Hybrid_CPU::Core::Pipeline::MicroOps;

namespace YAKSys_Hybrid_CPU::CloseToHSL::Core::ISA::Instructions::NonVmx::Lanes00_03Vector::MatrixTile
{
	MatrixTilePolicyBoundCaptureIdentity::MatrixTilePolicyBoundCaptureIdentity(
		unsigned short abiVersion, int ownerThreadId, int memoryDomainId,
		unsigned int opcode, MatrixTileProjectedOperationKind operationKind,
		unsigned short numericPolicyAbiVersion, unsigned long long numericPolicyFingerprint,
		unsigned short layoutPolicyAbiVersion, unsigned long long layoutPolicyFingerprint,
		unsigned long long descriptorFingerprint, MatrixTileRuntimeResourceClass resourceClass,
		YAKSys_Hybrid_CPU::Core::SlotClass slotClass, unsigned char eligibleLaneMask,
		unsigned char physicalLaneId, unsigned long long sourceSnapshotFingerprint,
		unsigned long long secondarySnapshotFingerprint, unsigned long long accumulatorSnapshotFingerprint,
		unsigned long long footprintBytes, unsigned long long replayEpoch,
		unsigned long long dependencyFingerprint, MatrixTileRetirePublicationKind publicationSurface,
		unsigned long long identityFingerprint)
		: AbiVersion(abiVersion), OwnerThreadId(ownerThreadId), MemoryDomainId(memoryDomainId),
		  Opcode(opcode), OperationKind(operationKind), NumericPolicyAbiVersion(numericPolicyAbiVersion),
		  NumericPolicyFingerprint(numericPolicyFingerprint), LayoutPolicyAbiVersion(layoutPolicyAbiVersion),
		  LayoutPolicyFingerprint(layoutPolicyFingerprint), DescriptorFingerprint(descriptorFingerprint),
		  ResourceClass(resourceClass), SlotClass(slotClass), EligibleLaneMask(eligibleLaneMask),
		  PhysicalLaneId(physicalLaneId), SourceSnapshotFingerprint(sourceSnapshotFingerprint),
		  SecondarySnapshotFingerprint(secondarySnapshotFingerprint),
		  AccumulatorSnapshotFingerprint(accumulatorSnapshotFingerprint), FootprintBytes(footprintBytes),
		  ReplayEpoch(replayEpoch), DependencyFingerprint(dependencyFingerprint),
		  PublicationSurface(publicationSurface), IdentityFingerprint(identityFingerprint),
		  IsValid(abiVersion == MatrixTilePolicyBoundIdentityAbi::CurrentAbiVersion &&
			  ownerThreadId >= 0 && ownerThreadId < YAKSys_Hybrid_CPU::Processor::CPU_Core::SmtWays &&
			  memoryDomainId == ownerThreadId && opcode != 0 &&
			  operationKind != MatrixTileProjectedOperationKind::Unspecified && descriptorFingerprint != 0 &&
			  resourceClass != MatrixTileRuntimeResourceClass::None && slotClass != YAKSys_Hybrid_CPU::Core::SlotClass::Unclassified &&
			  eligibleLaneMask != 0 && footprintBytes != 0 && replayEpoch != 0 && dependencyFingerprint != 0 &&
			  publicationSurface != MatrixTileRetirePublicationKind::None && identityFingerprint != 0 &&
			  (operationKind != MatrixTileProjectedOperationKind::Macc ||
				  (numericPolicyAbiVersion == MatrixTileNumericPolicyAbi::CurrentAbiVersion && numericPolicyFingerprint != 0 &&
				   layoutPolicyAbiVersion == MatrixTileLayoutPolicyAbi::CurrentAbiVersion && layoutPolicyFingerprint != 0)) &&
			  (operationKind != MatrixTileProjectedOperationKind::Transpose ||
				  (layoutPolicyAbiVersion == MatrixTileLayoutPolicyAbi::CurrentAbiVersion && layoutPolicyFingerprint != 0)))
	{
	}

	std::shared_ptr<MatrixTileExecutionCaptureRecord> MatrixTilePolicyBoundIdentityAbi::Bind(std::shared_ptr<MatrixTileExecutionCaptureRecord> capture, std::shared_ptr<MatrixTileMaterializedInstruction> materializedInstruction, std::shared_ptr<MatrixTileMicroOpDependencyMetadata> dependency, int ownerThreadId, int memoryDomainId, unsigned long long replayEpoch)
	{
		if (!capture || !materializedInstruction || !materializedInstruction->Projection || !dependency)
			throw std::invalid_argument("MatrixTile policy identity requires capture, materialized instruction, projection, and dependency metadata.");
		const auto &projection = materializedInstruction->Projection;
		std::shared_ptr<MatrixTileMaccSemanticContract> maccContract;
		if (projection->MaccContract && *projection->MaccContract)
			maccContract = *projection->MaccContract;
		std::shared_ptr<MatrixTileTransposeSemanticContract> transposeContract;
		if (projection->TransposeContract && *projection->TransposeContract)
			transposeContract = *projection->TransposeContract;
		const auto numericPolicy = maccContract ? maccContract->NumericPolicy : nullptr;
		auto layoutPolicy = maccContract ? maccContract->LayoutPolicy : nullptr;
		if (!layoutPolicy && transposeContract)
			layoutPolicy = transposeContract->LayoutPolicy;
		MatrixTileRuntimeResourceClass resourceClass = MatrixTileResourceContour::Classify(static_cast<unsigned int>(materializedInstruction->Opcode));
		SlotClass slotClass = MatrixTileResourceContour::ResolveSlotClass(resourceClass);
		unsigned char laneMask = SlotClassLaneMap::GetLaneMask(slotClass);
		unsigned char physicalLaneId = capture->StreamTransfer.TransferFingerprint != 0 ? capture->StreamTransfer.PhysicalLaneId : std::numeric_limits<unsigned char>::max();
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'checked' in this context:
// ORIGINAL LINE: ulong descriptorFootprintBytes = checked((ulong) MatrixTileExecuteCaptureAbi.GetPackedByteLength(materializedInstruction.ResultTileDescriptor));
		unsigned long long descriptorFootprintBytes = static_cast<unsigned long long>(MatrixTileExecuteCaptureAbi::GetPackedByteLength(materializedInstruction->ResultTileDescriptor));
		unsigned long long memoryFootprintBytes = capture->MemoryValidation && *capture->MemoryValidation ? (*capture->MemoryValidation)->TotalByteFootprint : 0;
		unsigned long long footprintBytes = memoryFootprintBytes != 0 ? memoryFootprintBytes : descriptorFootprintBytes;

        const auto makeIdentity = [&](unsigned long long fingerprint) {
            return std::make_shared<MatrixTilePolicyBoundCaptureIdentity>(CurrentAbiVersion, ownerThreadId, memoryDomainId, static_cast<unsigned int>(materializedInstruction->Opcode), materializedInstruction->OperationKind, numericPolicy ? numericPolicy->AbiVersion : 0, numericPolicy ? numericPolicy->Fingerprint : 0, layoutPolicy ? layoutPolicy->AbiVersion : 0, layoutPolicy ? layoutPolicy->Fingerprint : 0, ComputeDescriptorFingerprint(materializedInstruction), resourceClass, slotClass, laneMask, physicalLaneId, ComputeImageFingerprint(capture->SourceSnapshot), ComputeImageFingerprint(capture->SecondarySnapshot), ComputeImageFingerprint(capture->AccumulatorSnapshot), footprintBytes, replayEpoch, ComputeDependencyFingerprint(dependency), GetPublicationSurface(materializedInstruction->OperationKind), fingerprint);
        };
        auto identity = makeIdentity(0);
        identity = makeIdentity(ComputeIdentityFingerprint(identity));

        MatrixTileExecutionCaptureRecord updatedCapture = *capture;
        updatedCapture.NumericPolicy = numericPolicy ? std::optional<std::shared_ptr<MatrixTileNumericPolicy>>(numericPolicy) : std::nullopt;
        updatedCapture.LayoutPolicy = layoutPolicy ? std::optional<std::shared_ptr<MatrixTileLayoutPolicy>>(layoutPolicy) : std::nullopt;
        updatedCapture.PolicyIdentity = std::move(identity);
        return std::make_shared<MatrixTileExecutionCaptureRecord>(std::move(updatedCapture));
	}

	bool MatrixTilePolicyBoundIdentityAbi::ValidateCapture(std::shared_ptr<MatrixTileExecutionCaptureRecord> capture, unsigned long long currentReplayEpoch)
	{
		if (!capture || !capture->PolicyIdentity)
			return false;
		std::shared_ptr<MatrixTilePolicyBoundCaptureIdentity> identity = capture->PolicyIdentity;
		if (!identity->IsValid || identity->ReplayEpoch != currentReplayEpoch || identity->IdentityFingerprint != ComputeIdentityFingerprint(identity) || identity->OperationKind != capture->OperationKind || identity->DescriptorFingerprint != ComputeDescriptorFingerprint(capture) || identity->SourceSnapshotFingerprint != ComputeImageFingerprint(capture->SourceSnapshot) || identity->SecondarySnapshotFingerprint != ComputeImageFingerprint(capture->SecondarySnapshot) || identity->AccumulatorSnapshotFingerprint != ComputeImageFingerprint(capture->AccumulatorSnapshot) || identity->PublicationSurface != GetPublicationSurface(capture->OperationKind))
		{
			return false;
		}

		if (capture->OperationKind == MatrixTileProjectedOperationKind::Macc)
		{
			if (!capture->NumericPolicy.has_value() || !capture->LayoutPolicy.has_value() || !*capture->NumericPolicy || !*capture->LayoutPolicy || !MatrixTileNumericPolicyAbi::Validate(capture->NumericPolicy)->isRuntimeOwnedNumericPolicyAccepted() || !MatrixTileLayoutPolicyAbi::Validate(capture->LayoutPolicy, MatrixTileProjectedOperationKind::Macc)->isRuntimeOwnedLayoutPolicyAccepted() || (*capture->NumericPolicy)->AbiVersion != identity->NumericPolicyAbiVersion || (*capture->NumericPolicy)->Fingerprint != identity->NumericPolicyFingerprint || (*capture->LayoutPolicy)->AbiVersion != identity->LayoutPolicyAbiVersion || (*capture->LayoutPolicy)->Fingerprint != identity->LayoutPolicyFingerprint)
			{
				return false;
			}
		}
		else if (capture->OperationKind == MatrixTileProjectedOperationKind::Transpose)
		{
			if (capture->NumericPolicy.has_value() || !capture->LayoutPolicy.has_value() || !*capture->LayoutPolicy || !MatrixTileLayoutPolicyAbi::Validate(capture->LayoutPolicy, MatrixTileProjectedOperationKind::Transpose)->isRuntimeOwnedLayoutPolicyAccepted() || (*capture->LayoutPolicy)->AbiVersion != identity->LayoutPolicyAbiVersion || (*capture->LayoutPolicy)->Fingerprint != identity->LayoutPolicyFingerprint)
			{
				return false;
			}
		}
		else if (capture->NumericPolicy.has_value() || capture->LayoutPolicy.has_value())
		{
			return false;
		}

		return true;
	}

	bool MatrixTilePolicyBoundIdentityAbi::MatchesMaterializedInstruction(std::shared_ptr<MatrixTilePolicyBoundCaptureIdentity> identity, std::shared_ptr<MatrixTileMaterializedInstruction> materializedInstruction)
	{
		if (!identity || !materializedInstruction || !materializedInstruction->Projection)
			return false;
		const auto &projection = materializedInstruction->Projection;
		std::shared_ptr<MatrixTileMaccSemanticContract> maccContract = projection->MaccContract && *projection->MaccContract ? *projection->MaccContract : nullptr;
		std::shared_ptr<MatrixTileTransposeSemanticContract> transposeContract = projection->TransposeContract && *projection->TransposeContract ? *projection->TransposeContract : nullptr;
		std::shared_ptr<MatrixTileNumericPolicy> numericPolicy = maccContract ? maccContract->NumericPolicy : nullptr;
		std::shared_ptr<MatrixTileLayoutPolicy> layoutPolicy = maccContract ? maccContract->LayoutPolicy : nullptr;
		if (!layoutPolicy && transposeContract)
			layoutPolicy = transposeContract->LayoutPolicy;
		MatrixTileRuntimeResourceClass resourceClass = MatrixTileResourceContour::Classify(static_cast<unsigned int>(materializedInstruction->Opcode));
		SlotClass slotClass = MatrixTileResourceContour::ResolveSlotClass(resourceClass);

        return identity->Opcode == static_cast<unsigned int>(materializedInstruction->Opcode) &&
            identity->OperationKind == materializedInstruction->OperationKind &&
			identity->NumericPolicyAbiVersion == (numericPolicy ? numericPolicy->AbiVersion : 0) &&
			identity->NumericPolicyFingerprint == (numericPolicy ? numericPolicy->Fingerprint : 0) &&
			identity->LayoutPolicyAbiVersion == (layoutPolicy ? layoutPolicy->AbiVersion : 0) &&
			identity->LayoutPolicyFingerprint == (layoutPolicy ? layoutPolicy->Fingerprint : 0) &&
            identity->DescriptorFingerprint == ComputeDescriptorFingerprint(materializedInstruction) &&
            identity->ResourceClass == resourceClass && identity->SlotClass == slotClass &&
            identity->EligibleLaneMask == SlotClassLaneMap::GetLaneMask(slotClass) &&
            identity->PublicationSurface == GetPublicationSurface(materializedInstruction->OperationKind);
	}

	unsigned long long MatrixTilePolicyBoundIdentityAbi::ComputeDependencyFingerprint(std::shared_ptr<MatrixTileMicroOpDependencyMetadata> dependency)
	{
		unsigned long long hash = BeginHash();
		AddByte(hash, static_cast<unsigned char>(dependency->OperationKind));
		AddDescriptor(hash, dependency->TileDescriptor);
		AddDescriptor(hash, dependency->SecondaryTileDescriptor);
		AddDescriptor(hash, dependency->ResultTileDescriptor);
		AddBool(hash, dependency->HasTileMemoryDependencyMetadata);
		AddBool(hash, dependency->HasTileRegisterDependencyMetadata);
		AddBool(hash, dependency->HasAccumulatorDependencyMetadata);
		AddBool(hash, dependency->HasTransposePolicyDependencyMetadata);
		AddBool(hash, dependency->ReadsTileState);
		AddBool(hash, dependency->WritesTileState);
		AddBool(hash, dependency->ReadsAccumulator);
		AddBool(hash, dependency->WritesAccumulator);
		AddUInt16(hash, dependency->SourceTileId);
		AddUInt16(hash, dependency->SecondaryTileId);
		AddUInt16(hash, dependency->DestinationTileId);
		return FinishHash(hash);
	}

	unsigned long long MatrixTilePolicyBoundIdentityAbi::ComputeDependencyFingerprint(std::shared_ptr<MatrixTileMaterializedInstruction> materializedInstruction)
	{
		return ComputeDependencyFingerprint(CreateDependencyMetadata(materializedInstruction->Projection));
	}

	unsigned long long MatrixTilePolicyBoundIdentityAbi::ComputeDescriptorFingerprint(std::shared_ptr<MatrixTileMaterializedInstruction> materializedInstruction)
	{
		unsigned long long hash = BeginHash();
		AddDescriptor(hash, materializedInstruction->TileDescriptor);
		AddDescriptor(hash, materializedInstruction->SecondaryTileDescriptor);
		AddDescriptor(hash, materializedInstruction->ResultTileDescriptor);
		return FinishHash(hash);
	}

	unsigned long long MatrixTilePolicyBoundIdentityAbi::ComputeDescriptorFingerprint(std::shared_ptr<MatrixTileExecutionCaptureRecord> capture)
	{
		unsigned long long hash = BeginHash();
		AddDescriptor(hash, capture->TileDescriptor);
		AddDescriptor(hash, capture->SecondaryTileDescriptor);
		AddDescriptor(hash, capture->ResultTileDescriptor);
		return FinishHash(hash);
	}

	unsigned long long MatrixTilePolicyBoundIdentityAbi::ComputeImageFingerprint(std::shared_ptr<MatrixTileTileImage> image)
	{
		if (!image || !image->IsCanonicalPacked())
		{
			return 0;
		}

		unsigned long long hash = BeginHash();
		AddUInt16(hash, image->TileId);
		AddDescriptor(hash, image->Descriptor);
		for (unsigned char byte : image->Data)
		{
			AddByte(hash, byte);
		}

		return FinishHash(hash);
	}

	unsigned long long MatrixTilePolicyBoundIdentityAbi::ComputeIdentityFingerprint(std::shared_ptr<MatrixTilePolicyBoundCaptureIdentity> identity)
	{
		if (!identity)
			return 0;
		unsigned long long hash = BeginHash();
		AddUInt16(hash, identity->AbiVersion);
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: AddUInt32(ref hash, unchecked((uint)identity.OwnerThreadId));
		AddUInt32(hash, static_cast<unsigned int>(identity->OwnerThreadId));
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: AddUInt32(ref hash, unchecked((uint)identity.MemoryDomainId));
		AddUInt32(hash, static_cast<unsigned int>(identity->MemoryDomainId));
		AddUInt32(hash, identity->Opcode);
		AddByte(hash, static_cast<unsigned char>(identity->OperationKind));
		AddUInt16(hash, identity->NumericPolicyAbiVersion);
		AddUInt64(hash, identity->NumericPolicyFingerprint);
		AddUInt16(hash, identity->LayoutPolicyAbiVersion);
		AddUInt64(hash, identity->LayoutPolicyFingerprint);
		AddUInt64(hash, identity->DescriptorFingerprint);
		AddByte(hash, static_cast<unsigned char>(identity->ResourceClass));
		AddByte(hash, static_cast<unsigned char>(identity->SlotClass));
		AddByte(hash, identity->EligibleLaneMask);
		AddByte(hash, identity->PhysicalLaneId);
		AddUInt64(hash, identity->SourceSnapshotFingerprint);
		AddUInt64(hash, identity->SecondarySnapshotFingerprint);
		AddUInt64(hash, identity->AccumulatorSnapshotFingerprint);
		AddUInt64(hash, identity->FootprintBytes);
		AddUInt64(hash, identity->ReplayEpoch);
		AddUInt64(hash, identity->DependencyFingerprint);
		AddByte(hash, static_cast<unsigned char>(identity->PublicationSurface));
		return FinishHash(hash);
	}

	MatrixTileRetirePublicationKind MatrixTilePolicyBoundIdentityAbi::GetPublicationSurface(MatrixTileProjectedOperationKind operationKind)
	{
	// C# TO C++ CONVERTER TASK: The following 'switch expression' was not converted by C# to C++ Converter:
	//			return operationKind switch
	//			{
	//				MatrixTileProjectedOperationKind.Load => MatrixTileRetirePublicationKind.TileState,
	//				MatrixTileProjectedOperationKind.Store => MatrixTileRetirePublicationKind.MemoryStore,
	//				MatrixTileProjectedOperationKind.Macc => MatrixTileRetirePublicationKind.Accumulator,
	//				MatrixTileProjectedOperationKind.Transpose => MatrixTileRetirePublicationKind.TileState,
	//				_ => MatrixTileRetirePublicationKind.None,
	//			};
		switch (operationKind)
		{
		case MatrixTileProjectedOperationKind::Load:
		case MatrixTileProjectedOperationKind::Transpose:
			return MatrixTileRetirePublicationKind::TileState;
		case MatrixTileProjectedOperationKind::Store:
			return MatrixTileRetirePublicationKind::MemoryStore;
		case MatrixTileProjectedOperationKind::Macc:
			return MatrixTileRetirePublicationKind::Accumulator;
		default:
			return MatrixTileRetirePublicationKind::None;
		}
	}

	std::shared_ptr<MatrixTileMicroOpDependencyMetadata> MatrixTilePolicyBoundIdentityAbi::CreateDependencyMetadata(std::shared_ptr<MatrixTileInstructionIrProjection> projection)
	{
		if (!projection || !projection->SourcePayload)
			return nullptr;
		std::shared_ptr<VectorInstructionPayload> payload = projection->SourcePayload;
		unsigned short sourceTileId = GetTileId(payload->PrimaryPointer);
		unsigned short secondaryTileId = GetTileId(payload->SecondaryPointer);
		std::shared_ptr<MatrixTileTransposeSemanticContract> transposeContract = projection->TransposeContract && *projection->TransposeContract ? *projection->TransposeContract : nullptr;
		unsigned short destinationTileId = projection->OperationKind == MatrixTileProjectedOperationKind::Macc
			? GetHighTileIdOrDefault(payload->SecondaryPointer, secondaryTileId)
			: (transposeContract ? transposeContract->DestinationTileId : secondaryTileId);
		const auto tile = projection->TileDescriptor;
		const auto secondary = projection->SecondaryTileDescriptor;
		const auto result = projection->ResultTileDescriptor;
		using Kind = MatrixTileProjectedOperationKind;
		switch (projection->OperationKind)
		{
		case Kind::Load:
			return std::make_shared<MatrixTileMicroOpDependencyMetadata>(projection->OperationKind, tile, secondary, result, true, true, false, false, false, true, false, false, 0, secondaryTileId, destinationTileId);
		case Kind::Store:
			return std::make_shared<MatrixTileMicroOpDependencyMetadata>(projection->OperationKind, tile, secondary, result, true, true, false, false, true, false, false, false, secondaryTileId, secondaryTileId, destinationTileId);
		case Kind::Macc:
			return std::make_shared<MatrixTileMicroOpDependencyMetadata>(projection->OperationKind, tile, secondary, result, false, true, true, false, true, true, true, true, sourceTileId, secondaryTileId, destinationTileId);
		case Kind::Transpose:
			return std::make_shared<MatrixTileMicroOpDependencyMetadata>(projection->OperationKind, tile, secondary, result, false, true, false, true, true, true, false, false, transposeContract ? transposeContract->SourceTileId : sourceTileId, secondaryTileId, destinationTileId);
		default:
			return std::make_shared<MatrixTileMicroOpDependencyMetadata>();
		}

	// C# TO C++ CONVERTER TASK: The following 'switch expression' was not converted by C# to C++ Converter:
	//			return projection.OperationKind switch
	//			{
	//				MatrixTileProjectedOperationKind.Load => new MatrixTileMicroOpDependencyMetadata(
	//					projection.OperationKind, projection.TileDescriptor, projection.SecondaryTileDescriptor, projection.ResultTileDescriptor, HasTileMemoryDependencyMetadata: true, HasTileRegisterDependencyMetadata: true, HasAccumulatorDependencyMetadata: false, HasTransposePolicyDependencyMetadata: false, ReadsTileState: false, WritesTileState: true, ReadsAccumulator: false, WritesAccumulator: false, SourceTileId: 0, SecondaryTileId: secondaryTileId, DestinationTileId: destinationTileId), MatrixTileProjectedOperationKind.Store => new MatrixTileMicroOpDependencyMetadata(
	//
	//					projection.OperationKind,
	//					projection.TileDescriptor,
	//					projection.SecondaryTileDescriptor,
	//					projection.ResultTileDescriptor,
	//					HasTileMemoryDependencyMetadata: true,
	//					HasTileRegisterDependencyMetadata: true,
	//					HasAccumulatorDependencyMetadata: false,
	//					HasTransposePolicyDependencyMetadata: false,
	//					ReadsTileState: true,
	//					WritesTileState: false,
	//					ReadsAccumulator: false,
	//					WritesAccumulator: false,
	//					SourceTileId: secondaryTileId,
	//					SecondaryTileId: secondaryTileId,
	//					DestinationTileId: destinationTileId),
	//
	//				MatrixTileProjectedOperationKind.Macc => new MatrixTileMicroOpDependencyMetadata(
	//					projection.OperationKind,
	//					projection.TileDescriptor,
	//					projection.SecondaryTileDescriptor,
	//					projection.ResultTileDescriptor,
	//					HasTileMemoryDependencyMetadata: false,
	//					HasTileRegisterDependencyMetadata: true,
	//					HasAccumulatorDependencyMetadata: true,
	//					HasTransposePolicyDependencyMetadata: false,
	//					ReadsTileState: true,
	//					WritesTileState: true,
	//					ReadsAccumulator: true,
	//					WritesAccumulator: true,
	//					SourceTileId: sourceTileId,
	//					SecondaryTileId: secondaryTileId,
	//					DestinationTileId: destinationTileId),
	//
	//				MatrixTileProjectedOperationKind.Transpose => new MatrixTileMicroOpDependencyMetadata(
	//					projection.OperationKind,
	//					projection.TileDescriptor,
	//					projection.SecondaryTileDescriptor,
	//					projection.ResultTileDescriptor,
	//					HasTileMemoryDependencyMetadata: false,
	//					HasTileRegisterDependencyMetadata: true,
	//					HasAccumulatorDependencyMetadata: false,
	//					HasTransposePolicyDependencyMetadata: true,
	//					ReadsTileState: true,
	//					WritesTileState: true,
	//					ReadsAccumulator: false,
	//					WritesAccumulator: false,
	//					SourceTileId: projection.TransposeContract?.SourceTileId ?? sourceTileId,
	//					SecondaryTileId: secondaryTileId,
	//					DestinationTileId: destinationTileId),
	//
	//				_ => default
	//			};
	}

	unsigned short MatrixTilePolicyBoundIdentityAbi::GetTileId(unsigned long long pointer)
	{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'checked' in this context:
// ORIGINAL LINE: return checked((ushort)(pointer & 0xFFFF));
		return static_cast<unsigned short>(pointer & 0xFFFF);
	}

	unsigned short MatrixTilePolicyBoundIdentityAbi::GetHighTileIdOrDefault(unsigned long long pointer, unsigned short fallback)
	{
		unsigned long long high = (pointer >> 16) & 0xFFFF;
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'checked' in this context:
// ORIGINAL LINE: return high == 0 ? fallback : checked((ushort)high);
		return high == 0 ? fallback : static_cast<unsigned short>(high);
	}

	unsigned long long MatrixTilePolicyBoundIdentityAbi::BeginHash()
	{
		return 14695981039346656037ULL;
	}

	unsigned long long MatrixTilePolicyBoundIdentityAbi::FinishHash(unsigned long long hash)
	{
		return hash == 0 ? 1ULL : hash;
	}

	void MatrixTilePolicyBoundIdentityAbi::AddBool(unsigned long long &hash, bool value)
	{
		AddByte(hash, value ? static_cast<unsigned char>(1) : static_cast<unsigned char>(0));
	}

	void MatrixTilePolicyBoundIdentityAbi::AddByte(unsigned long long &hash, unsigned char value)
	{
		hash ^= value;
		hash *= 1099511628211ULL;
	}

	void MatrixTilePolicyBoundIdentityAbi::AddUInt16(unsigned long long &hash, unsigned short value)
	{
		AddByte(hash, static_cast<unsigned char>(value));
		AddByte(hash, static_cast<unsigned char>(value >> 8));
	}

	void MatrixTilePolicyBoundIdentityAbi::AddUInt32(unsigned long long &hash, unsigned int value)
	{
		for (int shift = 0; shift < 32; shift += 8)
		{
			AddByte(hash, static_cast<unsigned char>(value >> shift));
		}
	}

	void MatrixTilePolicyBoundIdentityAbi::AddUInt64(unsigned long long &hash, unsigned long long value)
	{
		for (int shift = 0; shift < 64; shift += 8)
		{
			AddByte(hash, static_cast<unsigned char>(value >> shift));
		}
	}

	void MatrixTilePolicyBoundIdentityAbi::AddDescriptor(unsigned long long &hash, std::shared_ptr<MatrixTileCanonicalDescriptorAbi> descriptor)
	{
		AddUInt16(hash, descriptor->Rows);
		AddUInt16(hash, descriptor->Columns);
		AddUInt16(hash, descriptor->ElementSizeBytes);
		AddUInt32(hash, descriptor->StrideBytes);
		AddByte(hash, static_cast<unsigned char>(descriptor->Layout));
	}

	void MatrixTilePolicyBoundIdentityAbi::AddDescriptor(unsigned long long &hash, const MatrixTileCanonicalDescriptorAbi &descriptor)
	{
		AddUInt16(hash, descriptor.Rows);
		AddUInt16(hash, descriptor.Columns);
		AddUInt16(hash, descriptor.ElementSizeBytes);
		AddUInt32(hash, descriptor.StrideBytes);
		AddByte(hash, static_cast<unsigned char>(descriptor.Layout));
	}
}
