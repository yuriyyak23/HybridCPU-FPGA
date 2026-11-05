#pragma once
#include "MatrixTileArchitecturalTileStateAndDescriptorAbi.h"

#include <string>
#include <limits>
#include <format>
#include <memory>

namespace YAKSys_Hybrid_CPU::CloseToHSL::Core::ISA::Instructions::NonVmx::Lanes00_03Vector::MatrixTile
{

	enum class MatrixTileMemoryOperationKind : unsigned char
	{
		Unspecified = 0,
		Load = 1,
		Store = 2
	};

	enum class MatrixTileEffectiveAddressSourceKind : unsigned char
	{
		Unspecified = 0,
		ExplicitRuntimeTileMemoryOperand = 1
	};

	enum class MatrixTileMemoryOrderingPolicyKind : unsigned char
	{
		Unspecified = 0,
		RetireOrderedAllOrNone = 1
	};

	enum class MatrixTileMemoryPublicationPolicyKind : unsigned char
	{
		Unspecified = 0,
		RetireStagedLoadPublication = 1,
		RetireStagedStoreCommit = 2
	};

	enum class MatrixTileMemoryFaultKind : unsigned char
	{
		None = 0,
		UnsupportedOperation = 1,
		UnsupportedEffectiveAddressSource = 2,
		ZeroDescriptor = 3,
		ReservedDescriptor = 4,
		UnsupportedElementSize = 5,
		RowByteOverflow = 6,
		StrideTooSmall = 7,
		AlignmentFault = 8,
		AddressOverflow = 9,
		InvalidPageSize = 10,
		PartialMemoryFault = 11
	};

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct MatrixTileMemoryFaultPoint(ushort Row, ushort Column, uint ByteOffsetInRow, ulong Address, bool IsStore);

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct MatrixTileMemoryShapeContract(MatrixTileMemoryOperationKind Operation, MatrixTileEffectiveAddressSourceKind EffectiveAddressSource, MatrixTileCanonicalDescriptorAbi Descriptor, ulong BaseAddress, ushort PageSizeBytes);

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct MatrixTileMemoryShapeValidationResult(bool IsValid, MatrixTileMemoryFaultKind FaultKind, MatrixTileMemoryFaultPoint FaultPoint, bool HasFaultPoint, ulong FirstByteAddress, ulong LastByteAddress, ulong TotalByteFootprint, uint RowByteCount, bool CrossesPageBoundary, MatrixTileMemoryPublicationPolicyKind PublicationPolicy, MatrixTileMemoryOrderingPolicyKind OrderingPolicy)
//	{
//		public bool IsMemoryShapeAbiAccepted
//		{
//			get
//			{
//				return IsValid;
//			}
//		}
//
//		public static MatrixTileMemoryShapeValidationResult Valid(ulong firstByteAddress, ulong lastByteAddress, ulong totalByteFootprint, uint rowByteCount, bool crossesPageBoundary, MatrixTileMemoryPublicationPolicyKind publicationPolicy)
//		{
//			return new MatrixTileMemoryShapeValidationResult(IsValid: true, FaultKind: MatrixTileMemoryFaultKind.None, FaultPoint: default, HasFaultPoint: false, firstByteAddress, lastByteAddress, totalByteFootprint, rowByteCount, crossesPageBoundary, publicationPolicy, MatrixTileMemoryOrderingPolicyKind.RetireOrderedAllOrNone);
//		}
//
//		public static MatrixTileMemoryShapeValidationResult Fault(MatrixTileMemoryFaultKind faultKind, MatrixTileMemoryFaultPoint faultPoint = default, bool hasFaultPoint = false)
//		{
//			return new MatrixTileMemoryShapeValidationResult(IsValid: false, faultKind, faultPoint, hasFaultPoint, FirstByteAddress: 0, LastByteAddress: 0, TotalByteFootprint: 0, RowByteCount: 0, CrossesPageBoundary: false, MatrixTileMemoryPublicationPolicyKind.Unspecified, MatrixTileMemoryOrderingPolicyKind.Unspecified);
//		}
//	}

	struct MatrixTileMemoryFaultPoint final { unsigned short Row = 0, Column = 0; unsigned int ByteOffsetInRow = 0; unsigned long long Address = 0; bool IsStore = false; };
	struct MatrixTileMemoryShapeContract final {
		MatrixTileMemoryOperationKind Operation;
		MatrixTileEffectiveAddressSourceKind EffectiveAddressSource;
		std::shared_ptr<MatrixTileCanonicalDescriptorAbi> Descriptor;
		unsigned long long BaseAddress;
		unsigned short PageSizeBytes;
	};
	struct MatrixTileMemoryShapeValidationResult final {
		bool IsValid = false;
		MatrixTileMemoryFaultKind FaultKind = MatrixTileMemoryFaultKind::None;
		MatrixTileMemoryFaultPoint FaultPoint{};
		bool HasFaultPoint = false;
		unsigned long long FirstByteAddress = 0, LastByteAddress = 0, TotalByteFootprint = 0;
		unsigned int RowByteCount = 0;
		bool CrossesPageBoundary = false;
		MatrixTileMemoryPublicationPolicyKind PublicationPolicy = MatrixTileMemoryPublicationPolicyKind::Unspecified;
		MatrixTileMemoryOrderingPolicyKind OrderingPolicy = MatrixTileMemoryOrderingPolicyKind::Unspecified;
		bool isMemoryShapeAbiAccepted() const noexcept { return IsValid; }
		static std::shared_ptr<MatrixTileMemoryShapeValidationResult> Valid(unsigned long long first, unsigned long long last, unsigned long long total, unsigned int rowBytes, bool crosses, MatrixTileMemoryPublicationPolicyKind policy) { return std::make_shared<MatrixTileMemoryShapeValidationResult>(MatrixTileMemoryShapeValidationResult{true, MatrixTileMemoryFaultKind::None, {}, false, first, last, total, rowBytes, crosses, policy, MatrixTileMemoryOrderingPolicyKind::RetireOrderedAllOrNone}); }
		static std::shared_ptr<MatrixTileMemoryShapeValidationResult> Fault(MatrixTileMemoryFaultKind kind, MatrixTileMemoryFaultPoint point = {}, bool hasPoint = false) { return std::make_shared<MatrixTileMemoryShapeValidationResult>(MatrixTileMemoryShapeValidationResult{false, kind, point, hasPoint}); }
	};
	class MatrixTileMemoryShapeAndFaultAbi final
	{
	public:
		static constexpr unsigned short DefaultPageSizeBytes = 4096;

		static inline const std::wstring MemoryShapeFaultDecision = L"ClosedTileMemoryShapeAndFaultAbi";
		static inline const std::wstring EffectiveAddressDecision = L"ExplicitRuntimeTileMemoryOperandEaSelected";
		static inline const std::wstring ShapeValidationDecision = L"DescriptorRowsColumnsElementStrideAndAddressOverflowValidated";
		static inline const std::wstring AlignmentDecision = L"ElementSizeAlignedBaseAndStrideRequired";
		static inline const std::wstring PageCrossingDecision = L"PageCrossingAllowedWithPreciseFaultPoint";
		static inline const std::wstring FaultReplayDecision = L"PreciseRowColumnFaultPointAndReplayIdentitySelected";
		static inline const std::wstring SideEffectOwnerDecision = L"RetireOwnedLoadPublicationAndStoreCommitSelected";
		static inline const std::wstring MemoryOrderingDecision = L"AllOrNoneRetireOrderedMemorySideEffects";
		static inline const std::wstring EaFallbackDecision = L"MemoryEaFallbackIsNotTileMemoryExecutionAuthority";

		static constexpr bool HasTileMemoryShapeFaultAbi = true;
		static constexpr bool HasEffectiveAddressSource = true;
		static constexpr bool HasShapeValidation = true;
		static constexpr bool HasAddressOverflowValidation = true;
		static constexpr bool HasAlignmentPolicy = true;
		static constexpr bool HasPageCrossingPolicy = true;
		static constexpr bool HasPartialFaultPolicy = true;
		static constexpr bool HasMemoryOrderingPolicy = true;
		static constexpr bool HasLoadStoreSideEffectOwner = true;
		static constexpr bool HasRetireRollbackPolicy = true;
		static constexpr bool KeepsEaFallbackNonAuthority = true;
		static constexpr bool KeepsMemorySideEffectsUnopened = true;
		static constexpr bool KeepsCompilerHandoffBlocked = true;

		static std::shared_ptr<MatrixTileMemoryShapeContract> CreateLoadContract(std::shared_ptr<MatrixTileCanonicalDescriptorAbi> descriptor, unsigned long long baseAddress, unsigned short pageSizeBytes = DefaultPageSizeBytes);

		static std::shared_ptr<MatrixTileMemoryShapeContract> CreateStoreContract(std::shared_ptr<MatrixTileCanonicalDescriptorAbi> descriptor, unsigned long long baseAddress, unsigned short pageSizeBytes = DefaultPageSizeBytes);

		static std::shared_ptr<MatrixTileMemoryShapeValidationResult> Validate(std::shared_ptr<MatrixTileMemoryShapeContract> contract);

		static std::shared_ptr<MatrixTileMemoryShapeValidationResult> ProjectPartialMemoryFault(std::shared_ptr<MatrixTileMemoryShapeContract> contract, unsigned short row, unsigned short column, unsigned short byteOffsetInElement = 0);

		static std::shared_ptr<MatrixTileMemoryFaultPoint> CreatePreciseFaultPoint(std::shared_ptr<MatrixTileMemoryShapeContract> contract, unsigned short row, unsigned short column, unsigned short byteOffsetInElement = 0);

		static bool IsSupportedElementSize(unsigned short elementSizeBytes);

		static bool IsElementAligned(unsigned long long value, unsigned short elementSizeBytes);

		static MatrixTileMemoryPublicationPolicyKind GetPublicationPolicy(MatrixTileMemoryOperationKind operation);

	private:
		static bool TryGetTotalByteFootprint(std::shared_ptr<MatrixTileCanonicalDescriptorAbi> descriptor, unsigned long long rowBytes, unsigned long long &totalBytes);

		static bool CrossesPageBoundary(unsigned long long firstByteAddress, unsigned long long lastByteAddress, unsigned short pageSizeBytes);
	};

}
