#include "MatrixTileRetirePublicationAbi.h"
#include "MatrixTilePolicyBoundIdentityAbi.h"
#include "MatrixTileStreamTransferAbi.h"
#include "MatrixTileNumericPolicyAbi.h"
#include "MatrixTileLayoutPolicyAbi.h"
#include "YAKSys_Hybrid_CPU.CloseToHSL.Core.ISA.Instructions.NonVmx.Lanes00_03Vector.MatrixTile.MtileLoadInstruction.h"
#include "YAKSys_Hybrid_CPU.CloseToHSL.Core.ISA.Instructions.NonVmx.Lanes00_03Vector.MatrixTile.MtileStoreInstruction.h"
#include "YAKSys_Hybrid_CPU.CloseToHSL.Core.ISA.Instructions.NonVmx.Lanes00_03Vector.MatrixTile.MtileMaccInstruction.h"
#include "YAKSys_Hybrid_CPU.CloseToHSL.Core.ISA.Instructions.NonVmx.Lanes00_03Vector.MatrixTile.MtransposeInstruction.h"
#include <limits>

namespace YAKSys_Hybrid_CPU::CloseToHSL::Core::ISA::Instructions::NonVmx::Lanes00_03Vector::MatrixTile
{
	namespace
	{
		std::wstring OperationKindName(MatrixTileProjectedOperationKind operationKind)
		{
			switch (operationKind)
			{
			case MatrixTileProjectedOperationKind::Unspecified: return L"Unspecified";
			case MatrixTileProjectedOperationKind::Load: return L"Load";
			case MatrixTileProjectedOperationKind::Store: return L"Store";
			case MatrixTileProjectedOperationKind::Macc: return L"Macc";
			case MatrixTileProjectedOperationKind::Transpose: return L"Transpose";
			default: return std::to_wstring(static_cast<unsigned int>(operationKind));
			}
		}
	}

	MatrixTileRetireValidationException::MatrixTileRetireValidationException(const std::wstring &message) : InvalidOperationException(message)
	{
	}

MatrixTileRetireFaultException::MatrixTileRetireFaultException(std::shared_ptr<MatrixTileRetireOutcome> outcome) : InvalidOperationException(outcome->Message), Outcome(outcome)
	{
	}

	std::shared_ptr<MatrixTileRetireOutcome> MatrixTileRetireFaultException::getOutcome() const
	{
		return Outcome;
	}

	std::shared_ptr<MatrixTileCaptureIdentity> MatrixTileRetirePublicationAbi::CreateCaptureIdentity(unsigned int coreId, int ownerThreadId, unsigned int opcode, MatrixTileProjectedOperationKind operationKind, unsigned long long captureOrdinal, std::shared_ptr<MatrixTileExecutionCaptureRecord> capture)
	{
		if (!capture)
			throw *Validation(L"MTILE capture identity creation requires a capture envelope.");
		std::shared_ptr<MatrixTilePolicyBoundCaptureIdentity> policyIdentity = capture->PolicyIdentity;
		if (!policyIdentity || !policyIdentity->IsValid || policyIdentity->IdentityFingerprint != MatrixTilePolicyBoundIdentityAbi::ComputeIdentityFingerprint(policyIdentity))
		{
			throw *Validation(L"MTILE capture identity creation requires a policy-bound Phase17 capture envelope.");
		}

		return std::make_shared<MatrixTileCaptureIdentity>(coreId, ownerThreadId, opcode, operationKind, captureOrdinal, ComputeCaptureFingerprint(capture));
	}

	std::shared_ptr<MatrixTileRetireOutcome> MatrixTileRetirePublicationAbi::Retire(std::shared_ptr<Processor::CPU_Core> &core, std::shared_ptr<MatrixTileExecutionCaptureRecord> capture, std::shared_ptr<MatrixTileCaptureIdentity> expectedIdentity)
	{
		ValidateForRetire(core, capture, expectedIdentity);

		if (capture->HasFault)
		{
			return MatrixTileRetireOutcome::Fault(capture, MatrixTileRetireFaultKind::CapturedExecutionFault, CreateFaultRetirementMessage(capture));
		}

		switch (capture->OperationKind)
		{
		case MatrixTileProjectedOperationKind::Load:
			return RetireTilePublication(core, capture, MatrixTileExecutionCaptureKind::TileLoad, MatrixTileRetirePublicationKind::TileState);
		case MatrixTileProjectedOperationKind::Store:
			return RetireStore(core, capture);
		case MatrixTileProjectedOperationKind::Macc:
			return RetireTilePublication(core, capture, MatrixTileExecutionCaptureKind::Macc, MatrixTileRetirePublicationKind::Accumulator);
		case MatrixTileProjectedOperationKind::Transpose:
			return RetireTilePublication(core, capture, MatrixTileExecutionCaptureKind::Transpose, MatrixTileRetirePublicationKind::TileState);
		default:
			throw *Validation(std::format(L"MTILE retire rejected unsupported operation kind {}.", OperationKindName(capture->OperationKind)));
		}
	}

	void MatrixTileRetirePublicationAbi::ValidateForRetire(std::shared_ptr<Processor::CPU_Core> core, std::shared_ptr<MatrixTileExecutionCaptureRecord> capture, std::shared_ptr<MatrixTileCaptureIdentity> expectedIdentity)
	{
		if (!core || !capture || !expectedIdentity)
			throw *Validation(L"MTILE retire requires a core, capture and expected identity.");
		ValidateCorrelation(core, capture, expectedIdentity);
		if (!MatrixTilePolicyBoundIdentityAbi::ValidateCapture(capture, core->GetMatrixTileReplayInvalidationEpoch()))
		{
			throw *Validation(L"MTILE retire rejected a capture whose policy-bound identity envelope no longer matches runtime state.");
		}

		ValidateEnvelope(capture);
		MatrixTileStreamTransferAbi::ValidateCapture(capture);

		if (capture->HasFault)
		{
			ValidateFaultCapture(capture);
			return;
		}

		switch (capture->OperationKind)
		{
			case MatrixTileProjectedOperationKind::Load:
				ValidateTilePublicationCapture(capture, MatrixTileExecutionCaptureKind::TileLoad);
				if (!capture->MemoryValidation.has_value() || !capture->MemoryValidation.value() || !capture->MemoryValidation.value()->isMemoryShapeAbiAccepted())
				{
					throw *Validation(L"MTILE_LOAD retire requires preserved valid memory-shape validation.");
				}

				break;

			case MatrixTileProjectedOperationKind::Store:
				ValidateStoreCapture(capture);
				ValidateStoreWrites(capture);
				break;

			case MatrixTileProjectedOperationKind::Macc:
				ValidateTilePublicationCapture(capture, MatrixTileExecutionCaptureKind::Macc);
				if (!capture->SemanticValidation.has_value() || !capture->SemanticValidation.value() || !capture->SemanticValidation.value()->isSemanticAbiAccepted() || !capture->AccumulatorSnapshot || !capture->AccumulatorSnapshot->IsCanonicalPacked())
				{
					throw *Validation(L"MTILE_MACC retire requires a valid semantic result and accumulator snapshot.");
				}

				break;

			case MatrixTileProjectedOperationKind::Transpose:
				ValidateTilePublicationCapture(capture, MatrixTileExecutionCaptureKind::Transpose);
				if (!capture->SemanticValidation.has_value() || !capture->SemanticValidation.value() || !capture->SemanticValidation.value()->isSemanticAbiAccepted() || !capture->SourceSnapshot || !capture->SourceSnapshot->IsCanonicalPacked())
				{
					throw *Validation(L"MTRANSPOSE retire requires a valid semantic result and source snapshot.");
				}

				break;

			default:
			throw *Validation(std::format(L"MTILE retire rejected unsupported operation kind {}.", OperationKindName(capture->OperationKind)));
		}
	}

	std::shared_ptr<MatrixTileRetireOutcome> MatrixTileRetirePublicationAbi::RetireTilePublication(std::shared_ptr<Processor::CPU_Core> &core, std::shared_ptr<MatrixTileExecutionCaptureRecord> capture, MatrixTileExecutionCaptureKind expectedCaptureKind, MatrixTileRetirePublicationKind publicationKind)
	{
		ValidateTilePublicationCapture(capture, expectedCaptureKind);

		core->PublishRetiredMatrixTile(capture->CaptureIdentity.OwnerThreadId, capture->ResultImage);
		return MatrixTileRetireOutcome::Published(capture->CaptureIdentity, publicationKind);
	}

	std::shared_ptr<MatrixTileRetireOutcome> MatrixTileRetirePublicationAbi::RetireStore(std::shared_ptr<Processor::CPU_Core> &core, std::shared_ptr<MatrixTileExecutionCaptureRecord> capture)
	{
		ValidateStoreCapture(capture);
		std::wstring failureMessage;
		if (!core->TryCommitRetiredMatrixTileStoreAllOrNone(capture->PendingStoreWrites, failureMessage))
		{
			return MatrixTileRetireOutcome::Fault(capture, MatrixTileRetireFaultKind::MemoryCommitFault, failureMessage);
		}

		return MatrixTileRetireOutcome::Published(capture->CaptureIdentity, MatrixTileRetirePublicationKind::MemoryStore);
	}

	void MatrixTileRetirePublicationAbi::ValidateTilePublicationCapture(std::shared_ptr<MatrixTileExecutionCaptureRecord> capture, MatrixTileExecutionCaptureKind expectedCaptureKind)
	{
		if (!capture || capture->CaptureKind != expectedCaptureKind || !capture->ResultImage ||
			!capture->ResultImage->IsCanonicalPacked() || capture->ResultImage->TileId != capture->DestinationTileId ||
			!capture->ResultTileDescriptor || capture->ResultImage->Descriptor != *capture->ResultTileDescriptor ||
			!capture->PendingStoreWrites.empty())
		{
			const std::wstring mnemonic = capture ? capture->Mnemonic : L"MTILE";
			throw *Validation(std::format(L"{} retire rejected a malformed staged tile publication.", mnemonic));
		}
	}

	void MatrixTileRetirePublicationAbi::ValidateStoreCapture(std::shared_ptr<MatrixTileExecutionCaptureRecord> capture)
	{
		if (!capture || capture->CaptureKind != MatrixTileExecutionCaptureKind::TileStore || !capture->SourceSnapshot ||
			!capture->SourceSnapshot->IsCanonicalPacked() || capture->SourceSnapshot->TileId != capture->SourceTileId ||
			!capture->TileDescriptor || capture->SourceSnapshot->Descriptor != *capture->TileDescriptor ||
			!capture->MemoryValidation.has_value() || !capture->MemoryValidation.value() ||
			!capture->MemoryValidation.value()->isMemoryShapeAbiAccepted())
		{
			throw *Validation(L"MTILE_STORE retire rejected a malformed staged store capture.");
		}
	}

	void MatrixTileRetirePublicationAbi::ValidateCorrelation(std::shared_ptr<Processor::CPU_Core> core, std::shared_ptr<MatrixTileExecutionCaptureRecord> capture, std::shared_ptr<MatrixTileCaptureIdentity> expectedIdentity)
	{
		if (!core || !capture || !expectedIdentity || !expectedIdentity->IsValid || !capture->HasRetireCorrelation() || !(capture->CaptureIdentity == *expectedIdentity))
		{
			throw *Validation(L"MTILE retire rejected a missing or mismatched capture correlation identity.");
		}

		if (capture->CaptureIdentity.CaptureFingerprint != ComputeCaptureFingerprint(capture))
		{
			throw *Validation(L"MTILE retire rejected a capture whose staged payload no longer matches its execute correlation.");
		}

		if (capture->CaptureIdentity.CoreId != core->getCoreID())
		{
			throw *Validation(std::format(L"MTILE retire capture belongs to core {} but retire owner is core {}.", capture->CaptureIdentity.CoreId, core->getCoreID()));
		}

		std::shared_ptr<MatrixTilePolicyBoundCaptureIdentity> policyIdentity = capture->PolicyIdentity;
		if (!policyIdentity || policyIdentity->OwnerThreadId != capture->CaptureIdentity.OwnerThreadId || policyIdentity->Opcode != capture->CaptureIdentity.Opcode || policyIdentity->OperationKind != capture->CaptureIdentity.OperationKind)
		{
			throw *Validation(L"MTILE retire rejected a capture whose policy-bound owner/opcode identity diverged from capture correlation.");
		}

		unsigned int expectedOpcode = GetOpcode(capture->OperationKind);
		std::wstring expectedMnemonic = GetMnemonic(capture->OperationKind);
// C# TO C++ CONVERTER TASK: The following .NET 'String.Equals' reference is not converted:
		if (capture->CaptureIdentity.Opcode != expectedOpcode || capture->CaptureIdentity.OperationKind != capture->OperationKind || capture->Mnemonic != expectedMnemonic)
		{
			throw *Validation(L"MTILE retire rejected a wrong-instruction capture correlation.");
		}
	}

	void MatrixTileRetirePublicationAbi::ValidateEnvelope(std::shared_ptr<MatrixTileExecutionCaptureRecord> capture)
	{
		if (!capture->RequiresRetirePublication || !capture->BlocksArchitecturalSideEffectsBeforeRetire || capture->UsesFallbackPath)
		{
			throw *Validation(std::format(L"{} retire requires a validated Phase09 capture with retire-owned publication and no fallback path.", capture->Mnemonic));
		}

		if (capture->HasFault != (capture->CaptureKind == MatrixTileExecutionCaptureKind::Fault))
		{
			throw *Validation(std::format(L"{} retire rejected an inconsistent success/fault capture envelope.", capture->Mnemonic));
		}
	}

	void MatrixTileRetirePublicationAbi::ValidateFaultCapture(std::shared_ptr<MatrixTileExecutionCaptureRecord> capture)
	{
		if (capture->FaultKind == MatrixTileExecutionFaultKind::None ||
			(capture->ResultImage && capture->ResultImage->IsCanonicalPacked()) || !capture->PendingStoreWrites.empty())
		{
			throw *Validation(std::format(L"{} retire rejected a fault capture carrying publishable side effects.", capture->Mnemonic));
		}
	}

	void MatrixTileRetirePublicationAbi::ValidateStoreWrites(std::shared_ptr<MatrixTileExecutionCaptureRecord> capture)
	{
		if (!capture || !capture->TileDescriptor || !capture->MemoryValidation.has_value() || !capture->MemoryValidation.value())
			throw *Validation(L"MTILE_STORE retire is missing its validated descriptor or memory range.");

		const auto &writes = capture->PendingStoreWrites;
		if (writes.size() != capture->TileDescriptor->Rows)
		{
			throw *Validation(L"MTILE_STORE retire requires exactly one staged write per tile row.");
		}

		const auto columns = static_cast<unsigned long long>(capture->TileDescriptor->Columns);
		const auto elementSize = static_cast<unsigned long long>(capture->TileDescriptor->ElementSizeBytes);
		if (elementSize != 0 && columns > static_cast<unsigned long long>(std::numeric_limits<int>::max()) / elementSize)
			throw *Validation(L"MTILE_STORE row byte length exceeds the supported integer range.");
		const int rowByteLength = static_cast<int>(columns * elementSize);
		const unsigned long long firstByteAddress = capture->MemoryValidation.value()->FirstByteAddress;

		for (std::size_t index = 0; index < writes.size(); ++index)
		{
			const auto &write = writes[index];
			if (!write)
				throw *Validation(std::format(L"MTILE_STORE retire rejected malformed staged row {}.", index));
			const unsigned long long stride = capture->TileDescriptor->StrideBytes;
			if (stride != 0 && static_cast<unsigned long long>(index) >
				(std::numeric_limits<unsigned long long>::max() - firstByteAddress) / stride)
				throw *Validation(std::format(L"MTILE_STORE retire rejected overflowing staged row {}.", index));
			const unsigned long long expectedAddress = firstByteAddress + (static_cast<unsigned long long>(index) * stride);
			if (write->Row != static_cast<unsigned short>(index) || write->Address != expectedAddress || write->Data.size() != static_cast<std::size_t>(rowByteLength))
			{
				throw *Validation(std::format(L"MTILE_STORE retire rejected malformed staged row {}.", index));
			}
		}
	}

	std::wstring MatrixTileRetirePublicationAbi::CreateFaultRetirementMessage(std::shared_ptr<MatrixTileExecutionCaptureRecord> capture)
	{
		const std::wstring message = !capture || StringHelper::isEmptyOrWhiteSpace(capture->FaultMessage)
			? L"captured runtime fault" : capture->FaultMessage;
		const std::wstring mnemonic = capture ? capture->Mnemonic : L"MTILE";
		return std::format(L"{} retired fault without architectural publication: {}", mnemonic, message);
	}

	unsigned long long MatrixTileRetirePublicationAbi::ComputeCaptureFingerprint(std::shared_ptr<MatrixTileExecutionCaptureRecord> capture)
	{
		constexpr unsigned long long offsetBasis = 14695981039346656037ULL;
		constexpr unsigned long long prime = 1099511628211ULL;
		unsigned long long hash = offsetBasis;
		auto AddByte = [](unsigned long long &current, unsigned char value) { current ^= value; current *= prime; };
		auto AddUInt16 = [&](unsigned long long &current, unsigned short value) { AddByte(current, static_cast<unsigned char>(value)); AddByte(current, static_cast<unsigned char>(value >> 8)); };
		auto AddUInt32 = [&](unsigned long long &current, unsigned int value) { for (unsigned int shift = 0; shift < 32; shift += 8) AddByte(current, static_cast<unsigned char>(value >> shift)); };
		auto AddUInt64 = [&](unsigned long long &current, unsigned long long value) { for (unsigned int shift = 0; shift < 64; shift += 8) AddByte(current, static_cast<unsigned char>(value >> shift)); };
		auto AddString = [&](unsigned long long &current, const std::wstring &value) { for (wchar_t character : value) AddUInt16(current, static_cast<unsigned short>(character)); AddByte(current, 0); };
		auto AddDescriptor = [&](unsigned long long &current, const MatrixTileCanonicalDescriptorAbi *descriptor)
		{
			AddUInt16(current, descriptor ? descriptor->Rows : 0);
			AddUInt16(current, descriptor ? descriptor->Columns : 0);
			AddUInt16(current, descriptor ? descriptor->ElementSizeBytes : 0);
			AddUInt32(current, descriptor ? descriptor->StrideBytes : 0);
			AddByte(current, descriptor ? static_cast<unsigned char>(descriptor->Layout) : 0);
		};
		auto AddImage = [&](unsigned long long &current, const std::shared_ptr<MatrixTileTileImage> &image)
		{
			AddUInt16(current, image ? image->TileId : 0);
			AddUInt16(current, image ? image->Descriptor.Rows : 0);
			AddUInt16(current, image ? image->Descriptor.Columns : 0);
			AddUInt16(current, image ? image->Descriptor.ElementSizeBytes : 0);
			AddUInt32(current, image ? image->Descriptor.StrideBytes : 0);
			AddByte(current, image ? static_cast<unsigned char>(image->Descriptor.Layout) : 0);
			const std::size_t length = image ? image->Data.size() : 0;
			if (length > std::numeric_limits<unsigned int>::max()) throw *Validation(L"MTILE image data exceeds the supported fingerprint length.");
			AddUInt32(current, static_cast<unsigned int>(length));
			if (image) for (unsigned char byte : image->Data) AddByte(current, byte);
		};
		auto AddNumericPolicy = [&](unsigned long long &current, const std::optional<std::shared_ptr<MatrixTileNumericPolicy>> &policy)
		{
			AddByte(current, policy.has_value() ? 1 : 0);
			const MatrixTileNumericPolicy *value = (policy && policy.value()) ? policy.value().get() : nullptr;
			AddUInt16(current, value ? value->AbiVersion : 0);
			AddByte(current, value ? static_cast<unsigned char>(value->ProfileId) : 0);
			AddUInt32(current, value ? static_cast<unsigned int>(value->ElementType) : 0);
			AddUInt32(current, value ? static_cast<unsigned int>(value->AccumulatorType) : 0);
			AddUInt32(current, value ? static_cast<unsigned int>(value->PublishFormat) : 0);
			AddByte(current, value ? static_cast<unsigned char>(value->Signedness) : 0);
			AddByte(current, value ? static_cast<unsigned char>(value->WideningRule) : 0);
			AddByte(current, value ? static_cast<unsigned char>(value->MultiplyRule) : 0);
			AddByte(current, value ? static_cast<unsigned char>(value->AddRule) : 0);
			AddByte(current, value ? static_cast<unsigned char>(value->RoundingMode) : 0);
			AddByte(current, value ? static_cast<unsigned char>(value->SaturationMode) : 0);
			AddByte(current, value ? static_cast<unsigned char>(value->OverflowMode) : 0);
			AddByte(current, value ? static_cast<unsigned char>(value->NaNPolicy) : 0);
			AddByte(current, value ? static_cast<unsigned char>(value->InfinityPolicy) : 0);
			AddByte(current, value ? static_cast<unsigned char>(value->DenormalPolicy) : 0);
			AddByte(current, value ? static_cast<unsigned char>(value->ReproducibilityMode) : 0);
			AddByte(current, value ? static_cast<unsigned char>(value->ExceptionPolicy) : 0);
			AddUInt64(current, value ? value->Fingerprint : 0);
		};
		auto AddLayoutPolicy = [&](unsigned long long &current, const std::optional<std::shared_ptr<MatrixTileLayoutPolicy>> &policy)
		{
			AddByte(current, policy.has_value() ? 1 : 0);
			const MatrixTileLayoutPolicy *value = (policy && policy.value()) ? policy.value().get() : nullptr;
			AddUInt16(current, value ? value->AbiVersion : 0);
			AddByte(current, value ? static_cast<unsigned char>(value->ProfileId) : 0);
			AddByte(current, value ? static_cast<unsigned char>(value->OperationKind) : 0);
			AddByte(current, value ? static_cast<unsigned char>(value->SourceAddressing) : 0);
			AddByte(current, value ? static_cast<unsigned char>(value->SecondaryAddressing) : 0);
			AddByte(current, value ? static_cast<unsigned char>(value->DestinationAddressing) : 0);
			AddByte(current, value ? static_cast<unsigned char>(value->KIterationOrder) : 0);
			AddByte(current, value ? static_cast<unsigned char>(value->TransposePermutation) : 0);
			AddByte(current, value ? static_cast<unsigned char>(value->TransposeAliasPolicy) : 0);
			AddUInt64(current, value ? value->Fingerprint : 0);
		};
		auto AddPolicyIdentity = [&](unsigned long long &current, const std::shared_ptr<MatrixTilePolicyBoundCaptureIdentity> &identity)
		{
			AddUInt16(current, identity ? identity->AbiVersion : 0);
			AddUInt32(current, identity ? static_cast<unsigned int>(identity->OwnerThreadId) : 0);
			AddUInt32(current, identity ? static_cast<unsigned int>(identity->MemoryDomainId) : 0);
			AddUInt32(current, identity ? identity->Opcode : 0);
			AddByte(current, identity ? static_cast<unsigned char>(identity->OperationKind) : 0);
			AddUInt16(current, identity ? identity->NumericPolicyAbiVersion : 0);
			AddUInt64(current, identity ? identity->NumericPolicyFingerprint : 0);
			AddUInt16(current, identity ? identity->LayoutPolicyAbiVersion : 0);
			AddUInt64(current, identity ? identity->LayoutPolicyFingerprint : 0);
			AddUInt64(current, identity ? identity->DescriptorFingerprint : 0);
			AddByte(current, identity ? static_cast<unsigned char>(identity->ResourceClass) : 0);
			AddByte(current, identity ? static_cast<unsigned char>(identity->SlotClass) : 0);
			AddByte(current, identity ? identity->EligibleLaneMask : 0);
			AddByte(current, identity ? identity->PhysicalLaneId : 0);
			AddUInt64(current, identity ? identity->SourceSnapshotFingerprint : 0);
			AddUInt64(current, identity ? identity->SecondarySnapshotFingerprint : 0);
			AddUInt64(current, identity ? identity->AccumulatorSnapshotFingerprint : 0);
			AddUInt64(current, identity ? identity->FootprintBytes : 0);
			AddUInt64(current, identity ? identity->ReplayEpoch : 0);
			AddUInt64(current, identity ? identity->DependencyFingerprint : 0);
			AddByte(current, identity ? static_cast<unsigned char>(identity->PublicationSurface) : 0);
			AddUInt64(current, identity ? identity->IdentityFingerprint : 0);
		};
		auto CheckedCount = [](std::size_t count)
		{
			if (count > std::numeric_limits<unsigned int>::max()) throw *Validation(L"MTILE capture collection exceeds the supported fingerprint length.");
			return static_cast<unsigned int>(count);
		};

// C# TO C++ CONVERTER TASK: Local functions are not converted by C# to C++ Converter:
//		static void AddByte(ref ulong current, byte value)
	//			{
	//				current ^= value;
	//				current *= prime;
	//			}

// C# TO C++ CONVERTER TASK: Local functions are not converted by C# to C++ Converter:
//		static void AddUInt16(ref ulong current, ushort value)
	//			{
	//				AddByte(ref current, (byte)value);
	//				AddByte(ref current, (byte)(value >> 8));
	//			}

// C# TO C++ CONVERTER TASK: Local functions are not converted by C# to C++ Converter:
//		static void AddUInt32(ref ulong current, uint value)
	//			{
	//				for (int shift = 0; shift < 32; shift += 8)
	//				{
	//					AddByte(ref current, (byte)(value >> shift));
	//				}
	//			}

// C# TO C++ CONVERTER TASK: Local functions are not converted by C# to C++ Converter:
//		static void AddUInt64(ref ulong current, ulong value)
	//			{
	//				for (int shift = 0; shift < 64; shift += 8)
	//				{
	//					AddByte(ref current, (byte)(value >> shift));
	//				}
	//			}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER TASK: Local functions are not converted by C# to C++ Converter:
// ORIGINAL LINE: static void AddString(ref ulong current, string? value)
//		static void AddString(ref ulong current, string value)
	//			{
	//				if (value == nullptr)
	//				{
	//					AddByte(ref current, 0xFF);
	//					return;
	//				}
	//
	//				foreach (char character in value)
	//				{
	//					AddUInt16(ref current, character);
	//				}
	//
	//				AddByte(ref current, 0);
	//			}

// C# TO C++ CONVERTER TASK: Local functions are not converted by C# to C++ Converter:
//		static void AddDescriptor(ref ulong current, MatrixTileCanonicalDescriptorAbi descriptor)
	//			{
	//				AddUInt16(ref current, descriptor.Rows);
	//				AddUInt16(ref current, descriptor.Columns);
	//				AddUInt16(ref current, descriptor.ElementSizeBytes);
	//				AddUInt32(ref current, descriptor.StrideBytes);
	//				AddByte(ref current, (byte)descriptor.Layout);
	//			}

// C# TO C++ CONVERTER TASK: Local functions are not converted by C# to C++ Converter:
//		static void AddImage(ref ulong current, MatrixTileTileImage image)
	//			{
	//				AddUInt16(ref current, image.TileId);
	//				AddDescriptor(ref current, image.Descriptor);
	//				byte[]? data = image.Data;
	//				AddUInt32(ref current, checked((uint)((data == nullptr ? nullptr : data.Length) ?? 0)));
	//				if (data == nullptr)
	//				{
	//					return;
	//				}
	//
	//				for (int index = 0; index < data.Length; index++)
	//				{
	//					AddByte(ref current, data[index]);
	//				}
	//			}

// C# TO C++ CONVERTER TASK: Local functions are not converted by C# to C++ Converter:
//		static void AddNumericPolicy(ref ulong current, System.Nullable<MatrixTileNumericPolicy> policy)
	//			{
	//				AddByte(ref current, policy.HasValue ? (byte)1 : (byte)0);
	//				if (!policy.HasValue)
	//				{
	//					return;
	//				}
	//
	//				MatrixTileNumericPolicy value = policy.Value;
	//				AddUInt16(ref current, value.AbiVersion);
	//				AddByte(ref current, (byte)value.ProfileId);
	//				AddUInt32(ref current, unchecked((uint)value.ElementType));
	//				AddUInt32(ref current, unchecked((uint)value.AccumulatorType));
	//				AddUInt32(ref current, unchecked((uint)value.PublishFormat));
	//				AddByte(ref current, (byte)value.Signedness);
	//				AddByte(ref current, (byte)value.WideningRule);
	//				AddByte(ref current, (byte)value.MultiplyRule);
	//				AddByte(ref current, (byte)value.AddRule);
	//				AddByte(ref current, (byte)value.RoundingMode);
	//				AddByte(ref current, (byte)value.SaturationMode);
	//				AddByte(ref current, (byte)value.OverflowMode);
	//				AddByte(ref current, (byte)value.NaNPolicy);
	//				AddByte(ref current, (byte)value.InfinityPolicy);
	//				AddByte(ref current, (byte)value.DenormalPolicy);
	//				AddByte(ref current, (byte)value.ReproducibilityMode);
	//				AddByte(ref current, (byte)value.ExceptionPolicy);
	//				AddUInt64(ref current, value.Fingerprint);
	//			}

// C# TO C++ CONVERTER TASK: Local functions are not converted by C# to C++ Converter:
//		static void AddLayoutPolicy(ref ulong current, System.Nullable<MatrixTileLayoutPolicy> policy)
	//			{
	//				AddByte(ref current, policy.HasValue ? (byte)1 : (byte)0);
	//				if (!policy.HasValue)
	//				{
	//					return;
	//				}
	//
	//				MatrixTileLayoutPolicy value = policy.Value;
	//				AddUInt16(ref current, value.AbiVersion);
	//				AddByte(ref current, (byte)value.ProfileId);
	//				AddByte(ref current, (byte)value.OperationKind);
	//				AddByte(ref current, (byte)value.SourceAddressing);
	//				AddByte(ref current, (byte)value.SecondaryAddressing);
	//				AddByte(ref current, (byte)value.DestinationAddressing);
	//				AddByte(ref current, (byte)value.KIterationOrder);
	//				AddByte(ref current, (byte)value.TransposePermutation);
	//				AddByte(ref current, (byte)value.TransposeAliasPolicy);
	//				AddUInt64(ref current, value.Fingerprint);
	//			}

// C# TO C++ CONVERTER TASK: Local functions are not converted by C# to C++ Converter:
//		static void AddPolicyIdentity(ref ulong current, MatrixTilePolicyBoundCaptureIdentity identity)
	//			{
	//				AddUInt16(ref current, identity.AbiVersion);
	//				AddUInt32(ref current, unchecked((uint)identity.OwnerThreadId));
	//				AddUInt32(ref current, unchecked((uint)identity.MemoryDomainId));
	//				AddUInt32(ref current, identity.Opcode);
	//				AddByte(ref current, (byte)identity.OperationKind);
	//				AddUInt16(ref current, identity.NumericPolicyAbiVersion);
	//				AddUInt64(ref current, identity.NumericPolicyFingerprint);
	//				AddUInt16(ref current, identity.LayoutPolicyAbiVersion);
	//				AddUInt64(ref current, identity.LayoutPolicyFingerprint);
	//				AddUInt64(ref current, identity.DescriptorFingerprint);
	//				AddByte(ref current, (byte)identity.ResourceClass);
	//				AddByte(ref current, (byte)identity.SlotClass);
	//				AddByte(ref current, identity.EligibleLaneMask);
	//				AddByte(ref current, identity.PhysicalLaneId);
	//				AddUInt64(ref current, identity.SourceSnapshotFingerprint);
	//				AddUInt64(ref current, identity.SecondarySnapshotFingerprint);
	//				AddUInt64(ref current, identity.AccumulatorSnapshotFingerprint);
	//				AddUInt64(ref current, identity.FootprintBytes);
	//				AddUInt64(ref current, identity.ReplayEpoch);
	//				AddUInt64(ref current, identity.DependencyFingerprint);
	//				AddByte(ref current, (byte)identity.PublicationSurface);
	//				AddUInt64(ref current, identity.IdentityFingerprint);
	//			}

		AddByte(hash, static_cast<unsigned char>(capture->CaptureKind));
		AddByte(hash, static_cast<unsigned char>(capture->OperationKind));
		AddString(hash, capture->Mnemonic);
		AddUInt16(hash, capture->SourceTileId);
		AddUInt16(hash, capture->SecondaryTileId);
		AddUInt16(hash, capture->DestinationTileId);
		AddDescriptor(hash, capture->TileDescriptor.get());
		AddDescriptor(hash, capture->SecondaryTileDescriptor.get());
		AddDescriptor(hash, capture->ResultTileDescriptor.get());
		AddImage(hash, capture->SourceSnapshot);
		AddImage(hash, capture->SecondarySnapshot);
		AddImage(hash, capture->AccumulatorSnapshot);
		AddImage(hash, capture->ResultImage);
		const auto &transfer = capture->StreamTransfer;
		AddUInt32(hash, transfer.CoreId);
		AddUInt32(hash, static_cast<unsigned int>(transfer.OwnerThreadId));
		AddUInt32(hash, transfer.Opcode);
		AddByte(hash, static_cast<unsigned char>(transfer.OperationKind));
		AddByte(hash, static_cast<unsigned char>(transfer.ResourceClass));
		AddByte(hash, static_cast<unsigned char>(transfer.SlotClass));
		AddByte(hash, transfer.PhysicalLaneId);
		AddByte(hash, transfer.StreamEngineChannel);
		AddByte(hash, static_cast<unsigned char>(transfer.Direction));
		AddUInt16(hash, transfer.ExpectedRows);
		AddUInt64(hash, transfer.TotalBytes);
		AddUInt64(hash, transfer.TransferFingerprint);
		AddByte(hash, transfer.Completed ? static_cast<unsigned char>(1) : static_cast<unsigned char>(0));
		AddUInt32(hash, CheckedCount(transfer.Windows.size()));
		for (const auto &window : transfer.Windows)
		{
			AddUInt16(hash, window ? window->Row : 0);
			AddUInt16(hash, window ? window->Window : 0);
			AddUInt64(hash, window ? window->Address : 0);
			AddUInt32(hash, window ? window->ByteCount : 0);
			AddUInt64(hash, window ? window->DataFingerprint : 0);
		}

		AddByte(hash, capture->MemoryValidation.has_value() ? static_cast<unsigned char>(1) : static_cast<unsigned char>(0));
		if (capture->MemoryValidation.has_value())
		{
			const auto &validation = capture->MemoryValidation.value();
			AddByte(hash, validation && validation->isMemoryShapeAbiAccepted() ? static_cast<unsigned char>(1) : static_cast<unsigned char>(0));
			AddByte(hash, validation ? static_cast<unsigned char>(validation->FaultKind) : 0);
			AddUInt16(hash, validation ? validation->FaultPoint.Row : 0);
			AddUInt16(hash, validation ? validation->FaultPoint.Column : 0);
			AddUInt32(hash, validation ? validation->FaultPoint.ByteOffsetInRow : 0);
			AddUInt64(hash, validation ? validation->FaultPoint.Address : 0);
			AddByte(hash, validation && validation->FaultPoint.IsStore ? static_cast<unsigned char>(1) : 0);
			AddByte(hash, validation && validation->HasFaultPoint ? static_cast<unsigned char>(1) : 0);
			AddUInt64(hash, validation ? validation->FirstByteAddress : 0);
			AddUInt64(hash, validation ? validation->LastByteAddress : 0);
			AddUInt64(hash, validation ? validation->TotalByteFootprint : 0);
			AddUInt32(hash, validation ? validation->RowByteCount : 0);
			AddByte(hash, validation && validation->CrossesPageBoundary ? static_cast<unsigned char>(1) : 0);
			AddByte(hash, validation ? static_cast<unsigned char>(validation->PublicationPolicy) : 0);
			AddByte(hash, validation ? static_cast<unsigned char>(validation->OrderingPolicy) : 0);
		}

		AddByte(hash, capture->SemanticValidation.has_value() ? static_cast<unsigned char>(1) : static_cast<unsigned char>(0));
		if (capture->SemanticValidation.has_value())
		{
			const auto &validation = capture->SemanticValidation.value();
			AddByte(hash, validation && validation->isSemanticAbiAccepted() ? static_cast<unsigned char>(1) : static_cast<unsigned char>(0));
			AddByte(hash, validation ? static_cast<unsigned char>(validation->FaultKind) : 0);
			AddDescriptor(hash, validation && validation->ResultDescriptor ? validation->ResultDescriptor.get() : nullptr);
			AddUInt16(hash, validation ? validation->ResultElementSizeBytes : 0);
			AddByte(hash, validation && validation->RequiresRetirePublication ? static_cast<unsigned char>(1) : static_cast<unsigned char>(0));
			AddByte(hash, validation && validation->RequiresReplayIdentity ? static_cast<unsigned char>(1) : static_cast<unsigned char>(0));
			AddByte(hash, validation && validation->UsesFallbackPath ? static_cast<unsigned char>(1) : static_cast<unsigned char>(0));
		}

		AddUInt32(hash, CheckedCount(capture->PendingStoreWrites.size()));
		for (const auto &write : capture->PendingStoreWrites)
		{
			AddUInt16(hash, write ? write->Row : 0);
			AddUInt64(hash, write ? write->Address : 0);
			const std::size_t dataLength = write ? write->Data.size() : 0;
			if (dataLength > std::numeric_limits<unsigned int>::max())
				throw *Validation(L"MTILE store row exceeds the supported fingerprint length.");
			AddUInt32(hash, static_cast<unsigned int>(dataLength));
			if (write) for (unsigned char byte : write->Data) AddByte(hash, byte);
		}

		AddByte(hash, static_cast<unsigned char>(capture->FaultKind));
		AddByte(hash, static_cast<unsigned char>(capture->MemoryFaultKind));
		AddByte(hash, static_cast<unsigned char>(capture->SemanticFaultKind));
		AddUInt16(hash, capture->FaultPoint.Row);
		AddUInt16(hash, capture->FaultPoint.Column);
		AddUInt32(hash, capture->FaultPoint.ByteOffsetInRow);
		AddUInt64(hash, capture->FaultPoint.Address);
		AddByte(hash, capture->FaultPoint.IsStore ? static_cast<unsigned char>(1) : static_cast<unsigned char>(0));
		AddByte(hash, capture->HasFaultPoint ? static_cast<unsigned char>(1) : static_cast<unsigned char>(0));
		AddString(hash, capture->FaultMessage);
		AddByte(hash, capture->HasFault ? static_cast<unsigned char>(1) : static_cast<unsigned char>(0));
		AddByte(hash, capture->RequiresRetirePublication ? static_cast<unsigned char>(1) : static_cast<unsigned char>(0));
		AddByte(hash, capture->RequiresReplayIdentity ? static_cast<unsigned char>(1) : static_cast<unsigned char>(0));
		AddByte(hash, capture->BlocksArchitecturalSideEffectsBeforeRetire ? static_cast<unsigned char>(1) : static_cast<unsigned char>(0));
		AddByte(hash, capture->UsesFallbackPath ? static_cast<unsigned char>(1) : static_cast<unsigned char>(0));
		AddNumericPolicy(hash, capture->NumericPolicy);
		AddLayoutPolicy(hash, capture->LayoutPolicy);
		AddPolicyIdentity(hash, capture->PolicyIdentity);

		return hash == 0 ? 1ULL : hash;
	}

	unsigned int MatrixTileRetirePublicationAbi::GetOpcode(MatrixTileProjectedOperationKind operationKind)
	{
		int temp_var77;
		switch (operationKind)
		{
			case MatrixTileProjectedOperationKind::Load:
				temp_var77 = Processor::CPU_Core::IsaOpcodeValues::MTILE_LOAD;
				break;
			case MatrixTileProjectedOperationKind::Store:
				temp_var77 = Processor::CPU_Core::IsaOpcodeValues::MTILE_STORE;
				break;
			case MatrixTileProjectedOperationKind::Macc:
				temp_var77 = Processor::CPU_Core::IsaOpcodeValues::MTILE_MACC;
				break;
			case MatrixTileProjectedOperationKind::Transpose:
				temp_var77 = Processor::CPU_Core::IsaOpcodeValues::MTRANSPOSE;
				break;
			default:
				temp_var77 = 0;
				break;
		}
		return temp_var77;
	}

	std::wstring MatrixTileRetirePublicationAbi::GetMnemonic(MatrixTileProjectedOperationKind operationKind)
	{
		switch (operationKind)
		{
		case MatrixTileProjectedOperationKind::Load: return MtileLoadInstruction::Mnemonic;
		case MatrixTileProjectedOperationKind::Store: return MtileStoreInstruction::Mnemonic;
		case MatrixTileProjectedOperationKind::Macc: return MtileMaccInstruction::Mnemonic;
		case MatrixTileProjectedOperationKind::Transpose: return MtransposeInstruction::Mnemonic;
		default: return L"";
		}
	}

	std::shared_ptr<MatrixTileRetireValidationException> MatrixTileRetirePublicationAbi::Validation(const std::wstring &message)
	{
		return std::make_shared<MatrixTileRetireValidationException>(message);
	}
}
