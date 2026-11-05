#pragma once

namespace YAKSys_Hybrid_CPU::Core
{

// C# TO C++ CONVERTER NOTE: The following .NET attribute has no direct equivalent in C++:
// ORIGINAL LINE: [Flags] public enum VectorStreamSaveMask : ulong
	enum class VectorStreamSaveMask : int
	{
		None = 0,
		VectorConfig = 1ULL << 0,
		ExceptionPolicy = 1ULL << 1,
		ExceptionCounters = 1ULL << 2,
		PredicateRegisters = 1ULL << 3,
		DirtyState = 1ULL << 4,
		StreamPolicy = 1ULL << 5,
		Architectural = VectorConfig | ExceptionPolicy | ExceptionCounters | PredicateRegisters | DirtyState,
		All = Architectural | StreamPolicy
	};

	enum class VectorExceptionAction : unsigned char
	{
		Accumulate = 0,
		Inject = 1,
		ReflectAsCompatibilityExit = 2
	};

	enum class StreamDescriptorFaultKind : unsigned char
	{
		None = 0,
		VirtualizationDisabled = 1,
		DescriptorTableUnbound = 2,
		AddressOutOfRange = 3,
		InvalidLength = 4,
		UnsupportedOpcode = 5,
		StreamLengthTooLarge = 6,
		ValidationEvidenceMissing = 7,
		ValidationEvidenceStale = 8,
		DescriptorDecodeFault = 9,
		DescriptorMemoryReadFailed = 10,
		OwnerMismatch = 11
	};

	enum class StreamReplayReason : unsigned char
	{
		None = 0,
		StreamReplayEpochChanged = 1,
		DescriptorValidationStale = 2,
		DirtyStateRequiresSave = 3
	};

	class IVectorExceptionCounterState
	{
	public:
		virtual void SetVectorExceptionCounters(unsigned long long overflowCount, unsigned long long underflowCount, unsigned long long divByZeroCount, unsigned long long invalidOpCount, unsigned long long inexactCount) = 0;
	};


}
