#pragma once

#include "MatrixTileIrProjectionAndMaterializer.h"
#include "MatrixTileArchitecturalTileStateAndDescriptorAbi.h"
#include "MatrixTileMemoryShapeAndFaultAbi.h"
#include "MatrixTileStreamTransferAbi.h"
#include "../../../../../Architecture/Common/YAKSys_Hybrid_CPU.Processor.h"
#include <string>
#include <map>
#include <span>
#include <utility>
#include <vector>
#include <functional>
#include <optional>
#include <format>
#include "tangible_exception_helper.h"
#include <memory>

namespace YAKSys_Hybrid_CPU::CloseToHSL::Core::ISA::Instructions::NonVmx::Lanes00_03Vector::MatrixTile
{
	class MatrixTileExecuteCaptureAbi;
	struct MatrixTileStreamTransferRecord;
	struct MatrixTilePolicyBoundCaptureIdentity;
	struct MatrixTileMemoryFaultPoint;
	struct MatrixTileTileImage final
	{
		unsigned short TileId = 0;
		MatrixTileCanonicalDescriptorAbi Descriptor;
		std::vector<unsigned char> Data;
		int getPackedByteLength() const noexcept { return static_cast<int>(Data.size()); }
		bool IsCanonicalPacked() const
		{
			const unsigned long long expectedLength = static_cast<unsigned long long>(Descriptor.Rows) * Descriptor.Columns * Descriptor.ElementSizeBytes;
			return Descriptor.isCanonical() && expectedLength != 0 && Data.size() == expectedLength;
		}
		static std::shared_ptr<MatrixTileTileImage> Create(unsigned short tileId, const MatrixTileCanonicalDescriptorAbi &descriptor, std::span<const unsigned char> data);
		static std::shared_ptr<MatrixTileTileImage> Create(unsigned short tileId, const std::shared_ptr<MatrixTileCanonicalDescriptorAbi> &descriptor, std::span<const unsigned char> data);
		MatrixTileTileImage DeepClone() const { return *this; }
	};
	struct MatrixTileCapturedMemoryWrite final { unsigned short Row = 0; unsigned long long Address = 0; std::vector<unsigned char> Data; };
	struct MatrixTileExecutionCaptureRecord;

	enum class MatrixTileExecutionCaptureKind : unsigned char
	{
		None = 0,
		TileLoad = 1,
		TileStore = 2,
		Macc = 3,
		Transpose = 4,
		Fault = 5
	};

	enum class MatrixTileExecutionFaultKind : unsigned char
	{
		None = 0,
		MissingMemoryContract = 1,
		MissingMemoryValidation = 2,
		InvalidMemoryShape = 3,
		MemoryRangeUnavailable = 4,
		MissingTileStateSnapshot = 5,
		MissingAccumulatorSnapshot = 6,
		MissingSemanticValidation = 7,
		InvalidSemanticContract = 8,
		ArithmeticOverflow = 9,
		UnsupportedOperation = 10,
		ArithmeticPolicyViolation = 11
	};

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct MatrixTileCaptureIdentity(uint CoreId, int OwnerThreadId, uint Opcode, MatrixTileProjectedOperationKind OperationKind, ulong CaptureOrdinal, ulong CaptureFingerprint)
//	{
//		public bool IsValid
//		{
//			get
//			{
//				return OwnerThreadId is >= 0 @and < Processor.CPU_Core.SmtWays && Opcode != 0 && OperationKind != MatrixTileProjectedOperationKind.Unspecified && CaptureOrdinal != 0 && CaptureFingerprint != 0;
//			}
//		}
//	}

	struct MatrixTileCaptureIdentity final
	{
		unsigned int CoreId = 0;
		int OwnerThreadId = 0;
		unsigned int Opcode = 0;
		MatrixTileProjectedOperationKind OperationKind = MatrixTileProjectedOperationKind::Unspecified;
		unsigned long long CaptureOrdinal = 0;
		unsigned long long CaptureFingerprint = 0;
		bool IsValid = false;

		MatrixTileCaptureIdentity() = default;
		MatrixTileCaptureIdentity(unsigned int coreId, int ownerThreadId, unsigned int opcode,
			MatrixTileProjectedOperationKind operationKind, unsigned long long captureOrdinal,
			unsigned long long captureFingerprint)
			: CoreId(coreId), OwnerThreadId(ownerThreadId), Opcode(opcode), OperationKind(operationKind),
			  CaptureOrdinal(captureOrdinal), CaptureFingerprint(captureFingerprint),
			  IsValid(ownerThreadId >= 0 && ownerThreadId < Processor::CPU_Core::SmtWays && opcode != 0 &&
				  operationKind != MatrixTileProjectedOperationKind::Unspecified && captureOrdinal != 0 && captureFingerprint != 0)
		{
		}

		bool operator==(const MatrixTileCaptureIdentity &other) const noexcept
		{
			return CoreId == other.CoreId && OwnerThreadId == other.OwnerThreadId && Opcode == other.Opcode &&
				OperationKind == other.OperationKind && CaptureOrdinal == other.CaptureOrdinal &&
				CaptureFingerprint == other.CaptureFingerprint;
		}

		MatrixTileCaptureIdentity &operator=(const MatrixTileCaptureIdentity &other) noexcept
		{
			if (this != &other)
			{
				CoreId = other.CoreId;
				OwnerThreadId = other.OwnerThreadId;
				Opcode = other.Opcode;
				OperationKind = other.OperationKind;
				CaptureOrdinal = other.CaptureOrdinal;
				CaptureFingerprint = other.CaptureFingerprint;
				IsValid = OwnerThreadId >= 0 && OwnerThreadId < Processor::CPU_Core::SmtWays && Opcode != 0 &&
					OperationKind != MatrixTileProjectedOperationKind::Unspecified && CaptureOrdinal != 0 && CaptureFingerprint != 0;
			}
			return *this;
		}
	};


	struct MatrixTileExecutionCaptureRecord final
	{
		MatrixTileExecutionCaptureKind CaptureKind;
		MatrixTileProjectedOperationKind OperationKind;
		std::wstring Mnemonic;
		unsigned short SourceTileId;
		unsigned short SecondaryTileId;
		unsigned short DestinationTileId;
		std::shared_ptr<MatrixTileCanonicalDescriptorAbi> TileDescriptor;
		std::shared_ptr<MatrixTileCanonicalDescriptorAbi> SecondaryTileDescriptor;
		std::shared_ptr<MatrixTileCanonicalDescriptorAbi> ResultTileDescriptor;
		std::shared_ptr<MatrixTileTileImage> SourceSnapshot;
		std::shared_ptr<MatrixTileTileImage> SecondarySnapshot;
		std::shared_ptr<MatrixTileTileImage> AccumulatorSnapshot;
		std::shared_ptr<MatrixTileTileImage> ResultImage;
		std::optional<std::shared_ptr<MatrixTileMemoryShapeValidationResult>> MemoryValidation;
		std::optional<std::shared_ptr<MatrixTileSemanticValidationResult>> SemanticValidation;
		std::vector<std::shared_ptr<MatrixTileCapturedMemoryWrite>> PendingStoreWrites;
		MatrixTileExecutionFaultKind FaultKind;
		MatrixTileMemoryFaultKind MemoryFaultKind;
		MatrixTileSemanticFaultKind SemanticFaultKind;
		MatrixTileMemoryFaultPoint FaultPoint;
		bool HasFaultPoint;
		std::wstring FaultMessage;
		bool HasFault;
		bool RequiresRetirePublication;
		bool RequiresReplayIdentity;
		bool BlocksArchitecturalSideEffectsBeforeRetire;
		bool UsesFallbackPath;
		MatrixTileCaptureIdentity CaptureIdentity;
		MatrixTileStreamTransferRecord StreamTransfer;
		std::optional<std::shared_ptr<MatrixTileNumericPolicy>> NumericPolicy;
		std::optional<std::shared_ptr<MatrixTileLayoutPolicy>> LayoutPolicy;
		std::shared_ptr<MatrixTilePolicyBoundCaptureIdentity> PolicyIdentity;

		MatrixTileExecutionCaptureRecord(MatrixTileExecutionCaptureKind captureKind,
			MatrixTileProjectedOperationKind operationKind, std::wstring mnemonic,
			unsigned short sourceTileId, unsigned short secondaryTileId, unsigned short destinationTileId,
			std::shared_ptr<MatrixTileCanonicalDescriptorAbi> tileDescriptor,
			std::shared_ptr<MatrixTileCanonicalDescriptorAbi> secondaryTileDescriptor,
			std::shared_ptr<MatrixTileCanonicalDescriptorAbi> resultTileDescriptor,
			std::shared_ptr<MatrixTileTileImage> sourceSnapshot,
			std::shared_ptr<MatrixTileTileImage> secondarySnapshot,
			std::shared_ptr<MatrixTileTileImage> accumulatorSnapshot,
			std::shared_ptr<MatrixTileTileImage> resultImage,
			std::optional<std::shared_ptr<MatrixTileMemoryShapeValidationResult>> memoryValidation,
			std::optional<std::shared_ptr<MatrixTileSemanticValidationResult>> semanticValidation,
			std::vector<std::shared_ptr<MatrixTileCapturedMemoryWrite>> pendingStoreWrites,
			MatrixTileExecutionFaultKind faultKind, MatrixTileMemoryFaultKind memoryFaultKind,
			MatrixTileSemanticFaultKind semanticFaultKind, MatrixTileMemoryFaultPoint faultPoint,
			bool hasFaultPoint, std::wstring faultMessage, bool hasFault, bool requiresRetirePublication,
			bool requiresReplayIdentity, bool blocksArchitecturalSideEffectsBeforeRetire, bool usesFallbackPath)
			: CaptureKind(captureKind), OperationKind(operationKind), Mnemonic(std::move(mnemonic)),
			  SourceTileId(sourceTileId), SecondaryTileId(secondaryTileId), DestinationTileId(destinationTileId),
			  TileDescriptor(std::move(tileDescriptor)), SecondaryTileDescriptor(std::move(secondaryTileDescriptor)),
			  ResultTileDescriptor(std::move(resultTileDescriptor)), SourceSnapshot(std::move(sourceSnapshot)),
			  SecondarySnapshot(std::move(secondarySnapshot)), AccumulatorSnapshot(std::move(accumulatorSnapshot)),
			  ResultImage(std::move(resultImage)), MemoryValidation(std::move(memoryValidation)),
			  SemanticValidation(std::move(semanticValidation)), PendingStoreWrites(std::move(pendingStoreWrites)),
			  FaultKind(faultKind), MemoryFaultKind(memoryFaultKind), SemanticFaultKind(semanticFaultKind),
			  FaultPoint(faultPoint), HasFaultPoint(hasFaultPoint), FaultMessage(std::move(faultMessage)),
			  HasFault(hasFault), RequiresRetirePublication(requiresRetirePublication),
			  RequiresReplayIdentity(requiresReplayIdentity),
			  BlocksArchitecturalSideEffectsBeforeRetire(blocksArchitecturalSideEffectsBeforeRetire),
			  UsesFallbackPath(usesFallbackPath)
		{
		}

		bool HasRetireCorrelation() const { return CaptureIdentity.IsValid; }
		MatrixTileExecutionCaptureRecord DeepClone() const
		{
			MatrixTileExecutionCaptureRecord clone = *this;
			if (SourceSnapshot) clone.SourceSnapshot = std::make_shared<MatrixTileTileImage>(SourceSnapshot->DeepClone());
			if (SecondarySnapshot) clone.SecondarySnapshot = std::make_shared<MatrixTileTileImage>(SecondarySnapshot->DeepClone());
			if (AccumulatorSnapshot) clone.AccumulatorSnapshot = std::make_shared<MatrixTileTileImage>(AccumulatorSnapshot->DeepClone());
			if (ResultImage) clone.ResultImage = std::make_shared<MatrixTileTileImage>(ResultImage->DeepClone());
			clone.StreamTransfer = StreamTransfer.DeepClone();
			clone.PendingStoreWrites.clear();
			for (const auto &write : PendingStoreWrites)
				if (write) clone.PendingStoreWrites.push_back(std::make_shared<MatrixTileCapturedMemoryWrite>(*write));
			return clone;
		}
	};

	// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct MatrixTileCapturedMemoryWrite(ushort Row, ulong Address, byte[] Data)
//	{
//		public MatrixTileCapturedMemoryWrite DeepClone()
//		{
//			return this with { Data = (byte[])Data.Clone() };
//		}
//	}

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct MatrixTileTileImage(ushort TileId, MatrixTileCanonicalDescriptorAbi Descriptor, byte[] Data)
//	{
//		public int PackedByteLength
//		{
//			get
//			{
//				return (Data == nullptr ? nullptr : Data.Length) ?? 0;
//			}
//		}
//
//		public bool IsCanonicalPacked
//		{
//			get
//			{
//				return Descriptor.IsCanonical && Data != nullptr && Data.Length == MatrixTileExecuteCaptureAbi.GetPackedByteLength(Descriptor);
//			}
//		}
//
//		public MatrixTileTileImage DeepClone()
//		{
//			return this with { Data = Data == nullptr ? Array.Empty<byte>() : (byte[])Data.Clone() };
//		}
//
//		public static MatrixTileTileImage Create(ushort tileId, MatrixTileCanonicalDescriptorAbi descriptor, ReadOnlySpan<byte> data)
//		{
//			MatrixTileExecuteCaptureAbi.ThrowIfInvalidPackedTileImage(descriptor, data.Length);
//			return new MatrixTileTileImage(tileId, descriptor, data.ToArray());
//		}
//	}

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct MatrixTileExecutionCaptureRecord(MatrixTileExecutionCaptureKind CaptureKind, MatrixTileProjectedOperationKind OperationKind, string Mnemonic, ushort SourceTileId, ushort SecondaryTileId, ushort DestinationTileId, MatrixTileCanonicalDescriptorAbi TileDescriptor, MatrixTileCanonicalDescriptorAbi SecondaryTileDescriptor, MatrixTileCanonicalDescriptorAbi ResultTileDescriptor, MatrixTileTileImage SourceSnapshot, MatrixTileTileImage SecondarySnapshot, MatrixTileTileImage AccumulatorSnapshot, MatrixTileTileImage ResultImage, System.Nullable<MatrixTileMemoryShapeValidationResult> MemoryValidation, System.Nullable<MatrixTileSemanticValidationResult> SemanticValidation, MatrixTileCapturedMemoryWrite[] PendingStoreWrites, MatrixTileExecutionFaultKind FaultKind, MatrixTileMemoryFaultKind MemoryFaultKind, MatrixTileSemanticFaultKind SemanticFaultKind, MatrixTileMemoryFaultPoint FaultPoint, bool HasFaultPoint, string FaultMessage, bool HasFault, bool RequiresRetirePublication, bool RequiresReplayIdentity, bool BlocksArchitecturalSideEffectsBeforeRetire, bool UsesFallbackPath)
//	{
//		public MatrixTileCaptureIdentity CaptureIdentity
//		{
//			get;
//			set;
//		}
//
//		public MatrixTileStreamTransferRecord StreamTransfer
//		{
//			get;
//			set;
//		}
//
//		public System.Nullable<MatrixTileNumericPolicy> NumericPolicy
//		{
//			get;
//			set;
//		}
//
//		public System.Nullable<MatrixTileLayoutPolicy> LayoutPolicy
//		{
//			get;
//			set;
//		}
//
//		public MatrixTilePolicyBoundCaptureIdentity PolicyIdentity
//		{
//			get;
//			set;
//		}
//
//		public bool HasRetireCorrelation
//		{
//			get
//			{
//				return CaptureIdentity.IsValid;
//			}
//		}
//
//		public MatrixTileExecutionCaptureRecord DeepClone()
//		{
//			MatrixTileCapturedMemoryWrite[] writes = PendingStoreWrites == nullptr ? Array.Empty<MatrixTileCapturedMemoryWrite>() : CloneWrites(PendingStoreWrites);
//
//			return this with { SourceSnapshot = SourceSnapshot.DeepClone(), SecondarySnapshot = SecondarySnapshot.DeepClone(), AccumulatorSnapshot = AccumulatorSnapshot.DeepClone(), ResultImage = ResultImage.DeepClone(), PendingStoreWrites = writes, StreamTransfer = StreamTransfer.DeepClone() };
//		}
//
//		private static MatrixTileCapturedMemoryWrite[] CloneWrites(MatrixTileCapturedMemoryWrite[] writes)
//		{
//			MatrixTileCapturedMemoryWrite[] clone = new MatrixTileCapturedMemoryWrite[writes.Length];
//			for (int i = 0; i < writes.Length; i++)
//			{
//				clone[i] = writes[i].DeepClone();
//			}
//
//			return clone;
//		}
//	}

	class MatrixTileArchitecturalTileRegisterFile final : public std::enable_shared_from_this<MatrixTileArchitecturalTileRegisterFile>
	{
	private:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly Dictionary<(int OwnerThreadId, ushort TileId), MatrixTileTileImage> _tiles = new();
		std::map<std::pair<int, unsigned short>, std::shared_ptr<MatrixTileTileImage>> _tiles;

	public:
		int getTileCount() const;

		void WriteTileForRuntimeSeed(int ownerThreadId, unsigned short tileId, std::shared_ptr<MatrixTileCanonicalDescriptorAbi> descriptor, std::span<const unsigned char> packedData);

		bool TryCaptureSnapshot(int ownerThreadId, unsigned short tileId, std::shared_ptr<MatrixTileCanonicalDescriptorAbi> expectedDescriptor, std::shared_ptr<MatrixTileTileImage> &snapshot);

		bool TryCaptureAnySnapshot(int ownerThreadId, unsigned short tileId, std::shared_ptr<MatrixTileTileImage> &snapshot);

		bool RemoveTile(int ownerThreadId, unsigned short tileId);

		bool ContainsTile(int ownerThreadId, unsigned short tileId);

	private:
		static int ValidateOwnerThreadId(int ownerThreadId);
	};

	class MatrixTileExecuteCaptureAbi final
	{
	public:
		static inline const std::wstring ExecuteCaptureDecision = L"ClosedMatrixTileExecuteCaptureSemantics";
		static inline const std::wstring TileLoadCaptureDecision = L"ClosedTileLoadCaptureBuffer";
		static inline const std::wstring TileStoreCaptureDecision = L"ClosedTileStorePendingWriteBuffer";
		static inline const std::wstring MaccCaptureDecision = L"ClosedMatrixTileMaccCaptureResult";
		static inline const std::wstring TransposeCaptureDecision = L"ClosedMatrixTileTransposeCaptureResult";
		static inline const std::wstring DeterministicExceptionCaptureDecision = L"ClosedDeterministicExceptionCapture";
		static inline const std::wstring MemoryFaultCaptureDecision = L"ClosedMemoryFaultCapture";
		static inline const std::wstring TileStateSnapshotDecision = L"ClosedTileStateReadSnapshot";
		static inline const std::wstring AccumulatorSnapshotDecision = L"ClosedAccumulatorReadSnapshot";
		static inline const std::wstring RetirePublicationDecision = L"NoRetireOwnedTilePublicationOpened";
		static inline const std::wstring ReplayRollbackDecision = L"NoReplayRollbackPublicationOpened";
		static inline const std::wstring BypassDecision = L"NoCaptureToRetireBypassWithoutRetireOwnership";
		static inline const std::wstring FallbackDecision = L"NoScalarVectorDotDscLane7VmxOrBackendFallbackAuthority";

		static constexpr bool HasExecutionCaptureSemantics = true;
		static constexpr bool HasTileLoadCaptureBuffer = true;
		static constexpr bool HasTileStorePendingWriteBuffer = true;
		static constexpr bool HasMaccCaptureResult = true;
		static constexpr bool HasTransposeCaptureResult = true;
		static constexpr bool HasDeterministicExceptionCapture = true;
		static constexpr bool HasMemoryFaultCapture = true;
		static constexpr bool HasTileStateReadSnapshot = true;
		static constexpr bool HasAccumulatorReadSnapshot = true;
		static constexpr bool HasRetirePublication = false;
		static constexpr bool HasReplayRollbackConformance = false;
		static constexpr bool BlocksCaptureToRetireBypass = true;
		static constexpr bool KeepsRetirePublicationNonAuthority = true;
		static constexpr bool KeepsReplayRollbackNonAuthority = true;
		static constexpr bool KeepsCompilerScopeClosed = true;
		static constexpr bool KeepsCompilerHandoffBlocked = true;
		static constexpr bool UsesFallbackPath = false;

		static int GetPackedByteLength(std::shared_ptr<MatrixTileCanonicalDescriptorAbi> descriptor);

		static void ThrowIfInvalidPackedTileImage(std::shared_ptr<MatrixTileCanonicalDescriptorAbi> descriptor, int byteLength);

		static std::shared_ptr<MatrixTileExecutionCaptureRecord> CaptureLoad(const std::wstring &mnemonic, MatrixTileProjectedOperationKind operationKind, unsigned short sourceTileId, unsigned short secondaryTileId, unsigned short destinationTileId, std::shared_ptr<MatrixTileCanonicalDescriptorAbi> tileDescriptor, std::shared_ptr<MatrixTileCanonicalDescriptorAbi> secondaryTileDescriptor, std::shared_ptr<MatrixTileCanonicalDescriptorAbi> resultTileDescriptor, std::optional<std::shared_ptr<MatrixTileMemoryShapeContract>> &memoryContract, std::optional<std::shared_ptr<MatrixTileMemoryShapeValidationResult>> &memoryValidation, std::function<std::optional<std::vector<unsigned char>>(unsigned long long, int)> readMemoryExact);

		static std::shared_ptr<MatrixTileExecutionCaptureRecord> CaptureStore(const std::wstring &mnemonic, MatrixTileProjectedOperationKind operationKind, unsigned short sourceTileId, unsigned short secondaryTileId, unsigned short destinationTileId, std::shared_ptr<MatrixTileCanonicalDescriptorAbi> tileDescriptor, std::shared_ptr<MatrixTileCanonicalDescriptorAbi> secondaryTileDescriptor, std::shared_ptr<MatrixTileCanonicalDescriptorAbi> resultTileDescriptor, std::optional<std::shared_ptr<MatrixTileMemoryShapeContract>> &memoryContract, std::optional<std::shared_ptr<MatrixTileMemoryShapeValidationResult>> &memoryValidation, std::shared_ptr<MatrixTileTileImage> sourceSnapshot);

		static std::shared_ptr<MatrixTileExecutionCaptureRecord> CaptureMacc(const std::wstring &mnemonic, MatrixTileProjectedOperationKind operationKind, unsigned short sourceTileId, unsigned short secondaryTileId, unsigned short destinationTileId, std::shared_ptr<MatrixTileCanonicalDescriptorAbi> tileDescriptor, std::shared_ptr<MatrixTileCanonicalDescriptorAbi> secondaryTileDescriptor, std::shared_ptr<MatrixTileCanonicalDescriptorAbi> resultTileDescriptor, std::optional<std::shared_ptr<MatrixTileSemanticValidationResult>> &semanticValidation, std::optional<std::shared_ptr<MatrixTileMaccSemanticContract>> &maccContract, std::shared_ptr<MatrixTileTileImage> leftSnapshot, std::shared_ptr<MatrixTileTileImage> rightSnapshot, std::shared_ptr<MatrixTileTileImage> accumulatorSnapshot);

		static std::shared_ptr<MatrixTileExecutionCaptureRecord> CaptureTranspose(const std::wstring &mnemonic, MatrixTileProjectedOperationKind operationKind, unsigned short sourceTileId, unsigned short secondaryTileId, unsigned short destinationTileId, std::shared_ptr<MatrixTileCanonicalDescriptorAbi> tileDescriptor, std::shared_ptr<MatrixTileCanonicalDescriptorAbi> secondaryTileDescriptor, std::shared_ptr<MatrixTileCanonicalDescriptorAbi> resultTileDescriptor, std::optional<std::shared_ptr<MatrixTileSemanticValidationResult>> &semanticValidation, std::optional<std::shared_ptr<MatrixTileTransposeSemanticContract>> &transposeContract, std::shared_ptr<MatrixTileTileImage> sourceSnapshot);

	private:
		static std::shared_ptr<MatrixTileExecutionCaptureRecord> Success(MatrixTileExecutionCaptureKind captureKind, MatrixTileProjectedOperationKind operationKind, const std::wstring &mnemonic, unsigned short sourceTileId, unsigned short secondaryTileId, unsigned short destinationTileId, std::shared_ptr<MatrixTileCanonicalDescriptorAbi> tileDescriptor, std::shared_ptr<MatrixTileCanonicalDescriptorAbi> secondaryTileDescriptor, std::shared_ptr<MatrixTileCanonicalDescriptorAbi> resultTileDescriptor, std::shared_ptr<MatrixTileTileImage> sourceSnapshot, std::shared_ptr<MatrixTileTileImage> secondarySnapshot, std::shared_ptr<MatrixTileTileImage> accumulatorSnapshot, std::shared_ptr<MatrixTileTileImage> resultImage, const std::optional<std::shared_ptr<MatrixTileMemoryShapeValidationResult>> &memoryValidation, const std::optional<std::shared_ptr<MatrixTileSemanticValidationResult>> &semanticValidation, const std::vector<std::shared_ptr<MatrixTileCapturedMemoryWrite>> &pendingStoreWrites);

		static std::shared_ptr<MatrixTileExecutionCaptureRecord> Fault(const std::wstring &mnemonic, MatrixTileProjectedOperationKind operationKind, unsigned short sourceTileId, unsigned short secondaryTileId, unsigned short destinationTileId, std::shared_ptr<MatrixTileCanonicalDescriptorAbi> tileDescriptor, std::shared_ptr<MatrixTileCanonicalDescriptorAbi> secondaryTileDescriptor, std::shared_ptr<MatrixTileCanonicalDescriptorAbi> resultTileDescriptor, MatrixTileExecutionFaultKind faultKind, const std::wstring &message);

		static std::shared_ptr<MatrixTileExecutionCaptureRecord> MemoryFault(const std::wstring &mnemonic, MatrixTileProjectedOperationKind operationKind, unsigned short sourceTileId, unsigned short secondaryTileId, unsigned short destinationTileId, std::shared_ptr<MatrixTileCanonicalDescriptorAbi> tileDescriptor, std::shared_ptr<MatrixTileCanonicalDescriptorAbi> secondaryTileDescriptor, std::shared_ptr<MatrixTileCanonicalDescriptorAbi> resultTileDescriptor, std::shared_ptr<MatrixTileMemoryShapeValidationResult> memoryValidation, const std::wstring &message);

		static std::shared_ptr<MatrixTileExecutionCaptureRecord> SemanticFault(const std::wstring &mnemonic, MatrixTileProjectedOperationKind operationKind, unsigned short sourceTileId, unsigned short secondaryTileId, unsigned short destinationTileId, std::shared_ptr<MatrixTileCanonicalDescriptorAbi> tileDescriptor, std::shared_ptr<MatrixTileCanonicalDescriptorAbi> secondaryTileDescriptor, std::shared_ptr<MatrixTileCanonicalDescriptorAbi> resultTileDescriptor, std::shared_ptr<MatrixTileSemanticValidationResult> semanticValidation, const std::wstring &message);

		static bool TryCopyMemoryRowsToPackedTile(std::shared_ptr<MatrixTileMemoryShapeContract> contract, std::shared_ptr<MatrixTileMemoryShapeValidationResult> validation, std::function<std::optional<std::vector<unsigned char>>(unsigned long long, int)> readMemoryExact, std::vector<unsigned char> &packed, std::shared_ptr<MatrixTileMemoryFaultPoint> &faultPoint);

		static std::vector<std::shared_ptr<MatrixTileCapturedMemoryWrite>> CreatePendingStoreWrites(std::shared_ptr<MatrixTileMemoryShapeContract> contract, std::shared_ptr<MatrixTileTileImage> sourceSnapshot);

		static std::shared_ptr<MatrixTileTileImage> Transpose(std::shared_ptr<MatrixTileTileImage> source, unsigned short destinationTileId, std::shared_ptr<MatrixTileCanonicalDescriptorAbi> destinationDescriptor, std::shared_ptr<MatrixTileLayoutPolicy> layoutPolicy);

	};

}
