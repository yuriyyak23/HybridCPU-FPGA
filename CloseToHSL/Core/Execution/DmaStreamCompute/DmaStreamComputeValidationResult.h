#pragma once
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>

namespace YAKSys_Hybrid_CPU::Core::Execution::DmaStreamCompute
{
	enum class DmaStreamComputeValidationFault
	{
		None = 0,
		DescriptorDecodeFault = 1,
		UnsupportedAbiVersion = 2,
		UnsupportedOperation = 3,
		UnsupportedElementType = 4,
		UnsupportedShape = 5,
		DescriptorCarrierDecodeFault = 6,
		DescriptorReferenceLost = 7,
		ReservedFieldFault = 8,
		RangeOverflow = 9,
		AlignmentFault = 10,
		ZeroLengthFault = 11,
		AliasOverlapFault = 12,
		OwnerDomainFault = 13,
		QuotaAdmissionReject = 14,
		ExecutionDisabled = 15,
		BackpressureAdmissionReject = 16,
		TokenCapAdmissionReject = 17,
		UnsupportedCapability = 18,
		MalformedExtension = 19,
		FootprintNormalizationFault = 20,
		AddressSpaceFault = 21,
		UnderApproximatedFootprintFault = 22
	};

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public sealed record DmaStreamComputeValidationResult
//	{
//		private DmaStreamComputeValidationResult(DmaStreamComputeValidationFault fault, System.Nullable<DmaStreamComputeDescriptor> descriptor, string message)
//		{
//			Fault = fault;
//			Descriptor = descriptor;
//			Message = message;
//		}
//
//		public bool IsValid
//		{
//			get
//			{
//				return Fault == DmaStreamComputeValidationFault.None && Descriptor != nullptr;
//			}
//		}
//
//		public bool IsDescriptorAbiAccepted
//		{
//			get
//			{
//				return IsValid;
//			}
//		}
//
//		public DmaStreamComputeValidationFault Fault
//		{
//			get;
//		}
//
//		public System.Nullable<DmaStreamComputeDescriptor> Descriptor
//		{
//			get;
//		}
//
//		public string Message
//		{
//			get;
//		}
//
//		public static DmaStreamComputeValidationResult Valid(DmaStreamComputeDescriptor descriptor)
//		{
//			ArgumentNullException.ThrowIfNull(descriptor);
//			return new DmaStreamComputeValidationResult(DmaStreamComputeValidationFault.None, descriptor, "Descriptor accepted after owner/domain guard; execution remains disabled.");
//		}
//
//		public static DmaStreamComputeValidationResult Fail(DmaStreamComputeValidationFault fault, string message)
//		{
//			if (fault == DmaStreamComputeValidationFault.None)
//			{
//				throw new ArgumentException("Use Valid for successful descriptor validation.", nameof(fault));
//			}
//
//			return new DmaStreamComputeValidationResult(fault, nullptr, message);
//		}
//
//		public DmaStreamComputeDescriptor RequireDescriptorForAdmission()
//		{
//			if (IsDescriptorAbiAccepted && Descriptor != nullptr)
//			{
//				return Descriptor;
//			}
//
//			throw new InvalidOperationException(string.Format("DmaStreamCompute descriptor is not admissible: {0}. {1}", Fault, Message));
//		}
//	}

	class DmaStreamComputeDescriptor;
	class DmaStreamComputeValidationResult final {
	public:
		const DmaStreamComputeValidationFault Fault;
		const std::shared_ptr<DmaStreamComputeDescriptor> Descriptor;
		const std::wstring Message;
		const bool IsValid;
		const bool IsDescriptorAbiAccepted;
	private:
		DmaStreamComputeValidationResult(DmaStreamComputeValidationFault fault, std::shared_ptr<DmaStreamComputeDescriptor> descriptor, std::wstring message)
			: Fault(fault), Descriptor(std::move(descriptor)), Message(std::move(message)), IsValid(Fault == DmaStreamComputeValidationFault::None && Descriptor != nullptr), IsDescriptorAbiAccepted(IsValid) {}
	public:
		static std::shared_ptr<DmaStreamComputeValidationResult> Valid(std::shared_ptr<DmaStreamComputeDescriptor> descriptor) {
			if (!descriptor) throw std::invalid_argument("descriptor");
			return std::shared_ptr<DmaStreamComputeValidationResult>(new DmaStreamComputeValidationResult(DmaStreamComputeValidationFault::None, std::move(descriptor), L"Descriptor accepted after owner/domain guard; execution remains disabled."));
		}
		static std::shared_ptr<DmaStreamComputeValidationResult> Fail(DmaStreamComputeValidationFault fault, const std::wstring& message) {
			if (fault == DmaStreamComputeValidationFault::None) throw std::invalid_argument("Use Valid for successful descriptor validation.");
			return std::shared_ptr<DmaStreamComputeValidationResult>(new DmaStreamComputeValidationResult(fault, nullptr, message));
		}
		std::shared_ptr<DmaStreamComputeDescriptor> RequireDescriptorForAdmission() const {
			if (IsDescriptorAbiAccepted && Descriptor) return Descriptor;
			throw std::logic_error("DmaStreamCompute descriptor is not admissible.");
		}
	};
}
