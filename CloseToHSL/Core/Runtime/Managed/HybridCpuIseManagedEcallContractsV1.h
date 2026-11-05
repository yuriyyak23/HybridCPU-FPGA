#pragma once

#include <string>
#include <utility>

namespace HybridCPU::Platform::Contracts
{
	enum class HybridCpuExternalServiceStatusV1 : unsigned char;
}

namespace HybridCPU::ManagedRuntime
{
	enum class HybridCpuManagedShapeStatusV1 : unsigned char
	{
		Success = 0,
		NullReference = 1,
		InvalidType = 2,
		NegativeLength = 3,
		SizeOverflow = 4,
		BoundsViolation = 5,
		ArrayTypeMismatch = 6,
		UnsupportedShape = 7,
		HeapFailure = 8,
		InitializationFailed = 9
	};

	struct HybridCpuManagedShapeResultV1 final
	{
		HybridCpuManagedShapeStatusV1 Status;
		std::wstring Reason;
		unsigned long long ObjectReference;
		long long ScalarValue;
		bool IsSuccess;

		HybridCpuManagedShapeResultV1(HybridCpuManagedShapeStatusV1 status, std::wstring reason,
			unsigned long long objectReference = 0, long long scalarValue = 0)
			: Status(status), Reason(std::move(reason)), ObjectReference(objectReference), ScalarValue(scalarValue),
			  IsSuccess(status == HybridCpuManagedShapeStatusV1::Success) {}
	};
}

namespace HybridCPU_ISE::CloseToHSL::Core::Runtime::Managed
{
	struct HybridCpuManagedArrayStoreEcallResultV1 final
	{
		bool IsSuccess;
		unsigned long long ExceptionReference;
		std::wstring Reason;
		bool IsManagedThrow;

		HybridCpuManagedArrayStoreEcallResultV1(bool isSuccess, unsigned long long exceptionReference,
			std::wstring reason, bool isManagedThrow = false)
			: IsSuccess(isSuccess), ExceptionReference(exceptionReference), Reason(std::move(reason)), IsManagedThrow(isManagedThrow) {}
	};

	struct HybridCpuManagedEcallObservationV1 final
	{
		unsigned long long Operation, Receiver, Argument1, Argument2;
		HybridCPU::Platform::Contracts::HybridCpuExternalServiceStatusV1 Status;
		int Error;
		unsigned long long Value;

		HybridCpuManagedEcallObservationV1(unsigned long long operation, unsigned long long receiver,
			unsigned long long argument1, unsigned long long argument2,
			HybridCPU::Platform::Contracts::HybridCpuExternalServiceStatusV1 status, int error,
			unsigned long long value)
			: Operation(operation), Receiver(receiver), Argument1(argument1), Argument2(argument2),
			  Status(status), Error(error), Value(value) {}
	};
}
