#pragma once

#include <string>
#include <memory>

namespace YAKSys_Hybrid_CPU::CloseToHSL::Core::ISA::Instructions::NonVmx::Lanes00_03Vector::MatrixTile
{

	enum class MatrixTileStateOwnerKind : unsigned char
	{
		Unspecified = 0,
		GuestArchitecturalTileRegisterFile = 1
	};

	enum class MatrixTileLifetimePolicyKind : unsigned char
	{
		Unspecified = 0,
		GuestArchitecturalTileRegisterFile = 1
	};

	enum class MatrixTileDescriptorLayoutKind : unsigned char
	{
		Unspecified = 0,
		RowMajor = 1
	};

	struct MatrixTileCanonicalDescriptorAbi final {
		unsigned short Rows = 0, Columns = 0, ElementSizeBytes = 0;
		unsigned int StrideBytes = 0;
		MatrixTileDescriptorLayoutKind Layout = MatrixTileDescriptorLayoutKind::Unspecified;
		static MatrixTileCanonicalDescriptorAbi Create(unsigned short rows, unsigned short columns, unsigned short elementSizeBytes, unsigned int strideBytes, MatrixTileDescriptorLayoutKind layout = MatrixTileDescriptorLayoutKind::RowMajor) { return {rows, columns, elementSizeBytes, strideBytes, layout}; }
		bool isZeroEncoding() const noexcept { return Rows == 0 && Columns == 0 && ElementSizeBytes == 0 && StrideBytes == 0 && Layout == MatrixTileDescriptorLayoutKind::Unspecified; }
		bool hasValidShape() const noexcept { return Rows != 0 && Columns != 0 && StrideBytes != 0; }
		bool hasValidElementSize() const noexcept { return ElementSizeBytes != 0; }
		bool hasCanonicalLayout() const noexcept { return Layout == MatrixTileDescriptorLayoutKind::RowMajor; }
		bool isCanonical() const noexcept { return hasValidShape() && hasValidElementSize() && hasCanonicalLayout(); }
		bool isReservedEncoding() const noexcept { return !isZeroEncoding() && !isCanonical(); }
		bool operator==(const MatrixTileCanonicalDescriptorAbi&) const = default;
	};
// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct MatrixTileCanonicalDescriptorAbi(ushort Rows, ushort Columns, ushort ElementSizeBytes, uint StrideBytes, MatrixTileDescriptorLayoutKind Layout)
//	{
//		public static MatrixTileCanonicalDescriptorAbi Zero
//		{
//			get;
//		} = default;
//
//		public static MatrixTileCanonicalDescriptorAbi Create(ushort rows, ushort columns, ushort elementSizeBytes, uint strideBytes, MatrixTileDescriptorLayoutKind layout = MatrixTileDescriptorLayoutKind.RowMajor)
//		{
//			return new MatrixTileCanonicalDescriptorAbi(rows, columns, elementSizeBytes, strideBytes, layout);
//		}
//
//		public bool IsZeroEncoding
//		{
//			get
//			{
//				return Rows == 0 && Columns == 0 && ElementSizeBytes == 0 && StrideBytes == 0 && Layout == MatrixTileDescriptorLayoutKind.Unspecified;
//			}
//		}
//
//		public bool HasValidShape
//		{
//			get
//			{
//				return Rows != 0 && Columns != 0 && StrideBytes != 0;
//			}
//		}
//
//		public bool HasValidElementSize
//		{
//			get
//			{
//				return ElementSizeBytes != 0;
//			}
//		}
//
//		public bool HasCanonicalLayout
//		{
//			get
//			{
//				return Layout == MatrixTileDescriptorLayoutKind.RowMajor;
//			}
//		}
//
//		public bool IsCanonical
//		{
//			get
//			{
//				return HasValidShape && HasValidElementSize && HasCanonicalLayout;
//			}
//		}
//
//		public bool IsReservedEncoding
//		{
//			get
//			{
//				return !IsZeroEncoding && !IsCanonical;
//			}
//		}
//	}

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct MatrixTileArchitecturalTileStateContract(MatrixTileStateOwnerKind OwnerKind, MatrixTileLifetimePolicyKind LifetimePolicy, MatrixTileCanonicalDescriptorAbi Descriptor, bool GuestVisible, bool HostOwnedEvidenceIsNonArchitectural)
//	{
//		public bool HasSelectedOwner
//		{
//			get
//			{
//				return OwnerKind == MatrixTileStateOwnerKind.GuestArchitecturalTileRegisterFile;
//			}
//		}
//
//		public bool HasSelectedLifetimePolicy
//		{
//			get
//			{
//				return LifetimePolicy == MatrixTileLifetimePolicyKind.GuestArchitecturalTileRegisterFile;
//			}
//		}
//
//		public bool HasCanonicalDescriptor
//		{
//			get
//			{
//				return Descriptor.IsCanonical;
//			}
//		}
//	}

	struct MatrixTileArchitecturalTileStateContract final {
		MatrixTileStateOwnerKind OwnerKind;
		MatrixTileLifetimePolicyKind LifetimePolicy;
		MatrixTileCanonicalDescriptorAbi Descriptor;
		bool GuestVisible, HostOwnedEvidenceIsNonArchitectural;
		bool hasSelectedOwner() const noexcept { return OwnerKind == MatrixTileStateOwnerKind::GuestArchitecturalTileRegisterFile; }
		bool hasSelectedLifetimePolicy() const noexcept { return LifetimePolicy == MatrixTileLifetimePolicyKind::GuestArchitecturalTileRegisterFile; }
		bool hasCanonicalDescriptor() const noexcept { return Descriptor.isCanonical(); }
	};
	class MatrixTileArchitecturalTileStateAndDescriptorAbi final
	{
	private:
		static inline const std::shared_ptr<MatrixTileArchitecturalTileStateContract> GuestArchitecturalTileStateContract = std::make_shared<MatrixTileArchitecturalTileStateContract>(MatrixTileStateOwnerKind::GuestArchitecturalTileRegisterFile, MatrixTileLifetimePolicyKind::GuestArchitecturalTileRegisterFile, MatrixTileCanonicalDescriptorAbi::Create(1, 1, 1, 1), true, true);

	public:
		static inline const std::wstring TileStateOwnerDecision = L"GuestArchitecturalTileRegisterFileOwnerSelected";
		static inline const std::wstring TileStateLifetimeDecision = L"GuestArchitecturalTileRegisterFileLifetimeSelected";
		static inline const std::wstring DescriptorCarrierDecision = L"CanonicalMatrixTileDescriptorCarrier";
		static inline const std::wstring DescriptorValidationDecision = L"RowsColumnsElementSizeStrideAndLayoutValidated";
		static inline const std::wstring ReservedDescriptorDecision = L"ZeroAndReservedDescriptorsFailClosed";

		static constexpr bool HasArchitecturalTileStateOwner = true;
		static constexpr bool HasCanonicalTileDescriptorAbi = true;
		static constexpr bool HasRuntimeOwnedTileStateContract = true;
		static constexpr bool HasTileDescriptorValidationHelpers = true;
		static constexpr bool HasReservedDescriptorFailFastTests = true;
		static constexpr bool KeepsHostOwnedEvidenceNonArchitectural = true;
		static constexpr bool KeepsCompilerHandoffBlocked = true;

		static std::shared_ptr<MatrixTileArchitecturalTileStateContract> getGuestArchitecturalTileStateContract();

		static bool IsGuestArchitecturalOwner(MatrixTileStateOwnerKind ownerKind);

		static bool HasCanonicalLifetimePolicy(MatrixTileLifetimePolicyKind lifetimePolicy);

		static bool IsCanonicalDescriptor(std::shared_ptr<MatrixTileCanonicalDescriptorAbi> descriptor);

		static bool IsZeroDescriptor(std::shared_ptr<MatrixTileCanonicalDescriptorAbi> descriptor);

		static bool IsReservedDescriptor(std::shared_ptr<MatrixTileCanonicalDescriptorAbi> descriptor);

		static std::wstring ValidateDescriptor(std::shared_ptr<MatrixTileCanonicalDescriptorAbi> descriptor);
	};

}
