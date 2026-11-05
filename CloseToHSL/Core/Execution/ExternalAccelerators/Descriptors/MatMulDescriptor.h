#pragma once

#include "AcceleratorCommandDescriptor.h"
#include <string>
#include <vector>
#include <cmath>
#include <limits>
#include <optional>
#include <memory>
#include <stdexcept>
#include <utility>

namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Descriptors
{

// C# TO C++ CONVERTER NOTE: The following .NET attribute has no direct equivalent in C++:
// ORIGINAL LINE: [Flags] public enum MatMulLayoutFlags : ushort
	enum class MatMulLayoutFlags : unsigned short
	{
		None = 0,
		RowMajorA = 1 << 0,
		RowMajorB = 1 << 1,
		RowMajorC = 1 << 2,
		TransposeA = 1 << 3,
		TransposeB = 1 << 4
	};
	struct MatMulDatatypeTriple final { AcceleratorDatatype InputDatatype{}; AcceleratorDatatype AccumulatorDatatype{}; AcceleratorDatatype OutputDatatype{}; };


// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct MatMulDatatypeTriple(AcceleratorDatatype InputDatatype, AcceleratorDatatype AccumulatorDatatype, AcceleratorDatatype OutputDatatype);

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public sealed record MatMulDescriptor
//	{
//		public required ulong ABase
//		{
//			get;
//			set;
//		}
//
//		public required ulong BBase
//		{
//			get;
//			set;
//		}
//
//		public required ulong CBase
//		{
//			get;
//			set;
//		}
//
//		public required uint M
//		{
//			get;
//			set;
//		}
//
//		public required uint N
//		{
//			get;
//			set;
//		}
//
//		public required uint K
//		{
//			get;
//			set;
//		}
//
//		public required uint Lda
//		{
//			get;
//			set;
//		}
//
//		public required uint Ldb
//		{
//			get;
//			set;
//		}
//
//		public required uint Ldc
//		{
//			get;
//			set;
//		}
//
//		public required uint TileM
//		{
//			get;
//			set;
//		}
//
//		public required uint TileN
//		{
//			get;
//			set;
//		}
//
//		public required uint TileK
//		{
//			get;
//			set;
//		}
//
//		public required MatMulDatatypeTriple Datatypes
//		{
//			get;
//			set;
//		}
//
//		public required MatMulLayoutFlags LayoutFlags
//		{
//			get;
//			set;
//		}
//
//		public AcceleratorPartialCompletionPolicy PartialPolicy
//		{
//			get;
//			set;
//		} = AcceleratorPartialCompletionPolicy.AllOrNone;
//	}

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public sealed record MatMulFootprint
//	{
//		public required AcceleratorMemoryRange ARange
//		{
//			get;
//			set;
//		}
//
//		public required AcceleratorMemoryRange BRange
//		{
//			get;
//			set;
//		}
//
//		public required AcceleratorMemoryRange CRange
//		{
//			get;
//			set;
//		}
//
//		public IReadOnlyList<AcceleratorMemoryRange> SourceRanges
//		{
//			get
//			{
//				return new[] { ARange, BRange };
//			}
//		}
//
//		public IReadOnlyList<AcceleratorMemoryRange> DestinationRanges
//		{
//			get
//			{
//				return new[] { CRange };
//			}
//		}
//	}

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public sealed record MatMulDescriptorValidationResult
//	{
//		private MatMulDescriptorValidationResult(bool isValid, AcceleratorDescriptorFault fault, System.Nullable<MatMulDescriptor> descriptor, System.Nullable<MatMulFootprint> footprint, string message)
//		{
//			IsValid = isValid;
//			Fault = fault;
//			Descriptor = descriptor;
//			Footprint = footprint;
//			Message = message;
//		}
//
//		public bool IsValid
//		{
//			get;
//		}
//
//		public bool IsRejected
//		{
//			get
//			{
//				return !IsValid;
//			}
//		}
//
//		public AcceleratorDescriptorFault Fault
//		{
//			get;
//		}
//
//		public System.Nullable<MatMulDescriptor> Descriptor
//		{
//			get;
//		}
//
//		public System.Nullable<MatMulFootprint> Footprint
//		{
//			get;
//		}
//
//		public string Message
//		{
//			get;
//		}
//
//		public bool GrantsCommandSubmissionAuthority
//		{
//			get
//			{
//				return false;
//			}
//		}
//
//		public bool GrantsExecutionAuthority
//		{
//			get
//			{
//				return false;
//			}
//		}
//
//		public bool GrantsCommitAuthority
//		{
//			get
//			{
//				return false;
//			}
//		}
//
//		public static MatMulDescriptorValidationResult Valid(MatMulDescriptor descriptor, MatMulFootprint footprint, string message)
//		{
//			return new(isValid: true, AcceleratorDescriptorFault.None, descriptor, footprint, message);
//		}
//
//		public static MatMulDescriptorValidationResult Reject(AcceleratorDescriptorFault fault, string message)
//		{
//			if (fault == AcceleratorDescriptorFault.None)
//			{
//				throw new ArgumentException("Use Valid for accepted MatMul descriptors.", nameof(fault));
//			}
//
//			return new MatMulDescriptorValidationResult(isValid: false, fault, descriptor: nullptr, footprint: nullptr, message);
//		}
//	}

		struct MatMulDescriptor final {
		unsigned long long ABase=0,BBase=0,CBase=0; unsigned int M=0,N=0,K=0,Lda=0,Ldb=0,Ldc=0,TileM=0,TileN=0,TileK=0;
		MatMulDatatypeTriple Datatypes{}; MatMulLayoutFlags LayoutFlags=MatMulLayoutFlags::None; AcceleratorPartialCompletionPolicy PartialPolicy=AcceleratorPartialCompletionPolicy::AllOrNone;
	};
	struct MatMulFootprint final {
		std::shared_ptr<AcceleratorMemoryRange> ARange,BRange,CRange;
		std::vector<std::shared_ptr<AcceleratorMemoryRange>> SourceRanges() const { return {ARange,BRange}; }
		std::vector<std::shared_ptr<AcceleratorMemoryRange>> DestinationRanges() const { return {CRange}; }
	};
	class MatMulDescriptorValidationResult final {
		MatMulDescriptorValidationResult(bool valid,AcceleratorDescriptorFault fault,std::shared_ptr<MatMulDescriptor> descriptor,std::shared_ptr<MatMulFootprint> footprint,std::wstring message):IsValid(valid),IsRejected(!valid),Fault(fault),Descriptor(std::move(descriptor)),Footprint(std::move(footprint)),Message(std::move(message)){}
	public:
		const bool IsValid; const bool IsRejected; const AcceleratorDescriptorFault Fault;
		const std::shared_ptr<MatMulDescriptor> Descriptor; const std::shared_ptr<MatMulFootprint> Footprint; const std::wstring Message;
		static constexpr bool GrantsCommandSubmissionAuthority=false,GrantsExecutionAuthority=false,GrantsCommitAuthority=false;
		static std::shared_ptr<MatMulDescriptorValidationResult> Valid(std::shared_ptr<MatMulDescriptor>d,std::shared_ptr<MatMulFootprint>f,std::wstring m){return std::shared_ptr<MatMulDescriptorValidationResult>(new MatMulDescriptorValidationResult(true,AcceleratorDescriptorFault::None,std::move(d),std::move(f),std::move(m)));}
		static std::shared_ptr<MatMulDescriptorValidationResult> Reject(AcceleratorDescriptorFault f,std::wstring m){if(f==AcceleratorDescriptorFault::None)throw std::invalid_argument("Use Valid for accepted MatMul descriptors.");return std::shared_ptr<MatMulDescriptorValidationResult>(new MatMulDescriptorValidationResult(false,f,nullptr,nullptr,std::move(m)));}
	};
	constexpr MatMulLayoutFlags operator|(MatMulLayoutFlags a, MatMulLayoutFlags b) noexcept {
		return static_cast<MatMulLayoutFlags>(static_cast<unsigned short>(a) | static_cast<unsigned short>(b));
	}
	constexpr MatMulLayoutFlags operator&(MatMulLayoutFlags a, MatMulLayoutFlags b) noexcept {
		return static_cast<MatMulLayoutFlags>(static_cast<unsigned short>(a) & static_cast<unsigned short>(b));
	}
	constexpr MatMulLayoutFlags operator~(MatMulLayoutFlags value) noexcept {
		return static_cast<MatMulLayoutFlags>(static_cast<unsigned short>(~static_cast<unsigned short>(value)));
	}
	class MatMulDescriptorValidator final : public std::enable_shared_from_this<MatMulDescriptorValidator>
	{
	public:
		static constexpr unsigned long long MaxOutputElements = 4096;

	private:
		static constexpr MatMulLayoutFlags SupportedLayoutMask = MatMulLayoutFlags::RowMajorA | MatMulLayoutFlags::RowMajorB | MatMulLayoutFlags::RowMajorC | MatMulLayoutFlags::TransposeA | MatMulLayoutFlags::TransposeB;

	public:
		std::shared_ptr<MatMulDescriptorValidationResult> Validate(std::shared_ptr<MatMulDescriptor> descriptor, std::optional<std::shared_ptr<AcceleratorCommandDescriptor>> commandDescriptor = std::nullopt);

		std::shared_ptr<MatMulDescriptorValidationResult> ValidateShape(std::shared_ptr<MatMulDescriptor> descriptor);

		std::shared_ptr<MatMulDescriptorValidationResult> ValidateStrides(std::shared_ptr<MatMulDescriptor> descriptor);

		std::shared_ptr<MatMulDescriptorValidationResult> ValidateDatatypes(std::shared_ptr<MatMulDescriptor> descriptor);

		std::shared_ptr<MatMulDescriptorValidationResult> NormalizeFootprints(std::shared_ptr<MatMulDescriptor> descriptor);

		static unsigned short DatatypeSizeBytes(AcceleratorDatatype datatype);

	private:
		static std::shared_ptr<MatMulDescriptorValidationResult> ValidateCommandDescriptorBinding(std::shared_ptr<MatMulDescriptor> descriptor, std::shared_ptr<MatMulFootprint> footprint, std::shared_ptr<AcceleratorCommandDescriptor> commandDescriptor);

		static bool TryMatrixSpanBytes(unsigned int rows, unsigned int cols, unsigned int stride, unsigned short elementSize, unsigned long long &bytes);

		static bool TryMul(unsigned long long left, unsigned long long right, unsigned long long &result);

		static bool IsRangeValid(unsigned long long address, unsigned long long length);

		static bool IsAligned(unsigned long long address, unsigned short alignment);

		static bool HasAmbiguousAliases(std::shared_ptr<MatMulFootprint> footprint, std::wstring &message);

		static bool RangesOverlap(std::shared_ptr<AcceleratorMemoryRange> left, std::shared_ptr<AcceleratorMemoryRange> right);

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private static bool RangesEqual(IReadOnlyList<AcceleratorMemoryRange> left, IReadOnlyList<AcceleratorMemoryRange> right)
		static bool RangesEqual(const std::vector<std::shared_ptr<AcceleratorMemoryRange>> &left, const std::vector<std::shared_ptr<AcceleratorMemoryRange>> &right);
	};

}
