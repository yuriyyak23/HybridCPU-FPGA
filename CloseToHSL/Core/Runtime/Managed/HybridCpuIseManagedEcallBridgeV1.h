#pragma once
#include "../../../../NonRTL/Runtime/HybridCpuIseBootBlobEcallBindingV1.h"
#include "../../../../NonRTL/Runtime/HybridCpuIseClockReadEcallBindingV1.h"
#include "../../../../NonRTL/Runtime/HybridCpuIseConsoleEcallBindingV1.h"

#include "IHybridCpuIseEcallBridgeV1.h"
#include "HybridCpuIseManagedEcallContractsV1.h"
#include "../../Architecture/Registers/CSR/CsrAccessPolicy.h"
#include "../../State/ICpuState.h"
#include <string>
#include <unordered_set>
#include <vector>
#include <deque>
#include <limits>
#include <stdexcept>
#include <functional>
#include <optional>
#include <format>
#include <memory>

using namespace HybridCPU::ManagedRuntime;
using namespace HybridCPU::Platform::Contracts;
using namespace HybridCPU::RuntimeKernel;
using namespace HybridCPU_ISE::NonRTL::Runtime;
using namespace YAKSys_Hybrid_CPU::Core;
using namespace YAKSys_Hybrid_CPU::Core::Pipeline;
using namespace YAKSys_Hybrid_CPU::Core::Registers;

namespace HybridCPU_ISE::CloseToHSL::Core::Runtime::Managed
{

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public sealed record HybridCpuManagedArrayStoreEcallResultV1(bool IsSuccess, ulong ExceptionReference, string Reason, bool IsManagedThrow = false);
// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public sealed record HybridCpuManagedEcallObservationV1(ulong Operation, ulong Receiver, ulong Argument1, ulong Argument2, HybridCpuExternalServiceStatusV1 Status, int Error, ulong Value);

	/// <summary>
	/// Production retirement-time bridge for the bounded managed-runtime ECALL surface.
	/// It owns no ambient/global CPU state and handles only an exact user-mode envelope.
	/// </summary>
	class HybridCpuIseManagedEcallBridgeV1 final : public ::YAKSys_Hybrid_CPU::Core::IHybridCpuIseEcallBridgeV1
	{
	private:
		std::optional<int> LastProcessExitCode;
		std::shared_ptr<HybridCpuIseBootBlobEcallObservationV1> LastBootBlobObservation;
		std::shared_ptr<HybridCPU_ISE::NonRTL::Runtime::HybridCpuIseClockReadEcallObservationV1> LastClockReadObservation;
		std::shared_ptr<HybridCPU_ISE::NonRTL::Runtime::HybridCpuIseConsoleEcallObservationV1> LastConsoleObservation;
		unsigned long long ProcessExitRetireCount = 0;
		std::wstring LastRejectedReason;
		unsigned long long LastManagedOperation = 0;
		std::optional<HybridCpuManagedShapeStatusV1> LastManagedStatus;
		std::wstring LastManagedReason;
		std::optional<HybridCpuExternalServiceStatusV1> LastManagedExternalStatus;
		std::optional<int> LastManagedExternalError;
		unsigned long long LastManagedExternalValue = 0;
		unsigned long long LastManagedReceiver = 0;
		unsigned long long LastManagedArgument1 = 0;
		unsigned long long LastManagedArgument2 = 0;

		static constexpr unsigned long long VliwBundleBytes = 256;
		const std::shared_ptr<IHybridCpuRuntimeKernelV1> _kernel;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private readonly System.Func<ulong, HybridCpuManagedShapeResultV1>? _argumentMessage;
		const std::function<std::shared_ptr<HybridCpuManagedShapeResultV1>(unsigned long long)> _argumentMessage;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private readonly System.Func<ulong, HybridCpuManagedShapeResultV1>? _ensureTypeInitialized;
		const std::function<std::shared_ptr<HybridCpuManagedShapeResultV1>(unsigned long long)> _ensureTypeInitialized;
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly HashSet<ulong> _activeCpuInitializerHandles = [];
		std::unordered_set<unsigned long long> _activeCpuInitializerHandles = {};
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private readonly System.Func<ulong, int, int, HybridCpuManagedShapeResultV1>? _staticStoreInt32;
		const std::function<std::shared_ptr<HybridCpuManagedShapeResultV1>(unsigned long long, int, int)> _staticStoreInt32;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private readonly System.Func<ulong, int, ulong, HybridCpuManagedShapeResultV1>? _staticStoreReference;
		const std::function<std::shared_ptr<HybridCpuManagedShapeResultV1>(unsigned long long, int, unsigned long long)> _staticStoreReference;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private readonly System.Func<ulong, int, HybridCpuManagedShapeResultV1>? _staticLoadInt32;
		const std::function<std::shared_ptr<HybridCpuManagedShapeResultV1>(unsigned long long, int)> _staticLoadInt32;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private readonly System.Func<ulong, int, HybridCpuManagedShapeResultV1>? _staticLoadReference;
		const std::function<std::shared_ptr<HybridCpuManagedShapeResultV1>(unsigned long long, int)> _staticLoadReference;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private readonly System.Func<HybridCpuManagedShapeResultV1>? _allocateNullReferenceException;
		const std::function<std::shared_ptr<HybridCpuManagedShapeResultV1>()> _allocateNullReferenceException;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private readonly System.Func<ulong, int, int, HybridCpuManagedArrayStoreEcallResultV1>? _arrayStoreInt32;
		const std::function<std::shared_ptr<HybridCpuManagedArrayStoreEcallResultV1>(unsigned long long, int, int)> _arrayStoreInt32;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private readonly System.Func<ulong, int, int, HybridCpuManagedArrayStoreEcallResultV1>? _arrayStoreInt8;
		const std::function<std::shared_ptr<HybridCpuManagedArrayStoreEcallResultV1>(unsigned long long, int, int)> _arrayStoreInt8;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private readonly System.Func<ulong, int, int, HybridCpuManagedArrayStoreEcallResultV1>? _arrayStoreInt16;
		const std::function<std::shared_ptr<HybridCpuManagedArrayStoreEcallResultV1>(unsigned long long, int, int)> _arrayStoreInt16;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private readonly System.Func<ulong, int, ulong, HybridCpuManagedArrayStoreEcallResultV1>? _arrayStoreReference;
		const std::function<std::shared_ptr<HybridCpuManagedArrayStoreEcallResultV1>(unsigned long long, int, unsigned long long)> _arrayStoreReference;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private readonly System.Func<ulong, int, HybridCpuManagedArrayStoreEcallResultV1>? _arrayLoadReference;
		const std::function<std::shared_ptr<HybridCpuManagedArrayStoreEcallResultV1>(unsigned long long, int)> _arrayLoadReference;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private readonly System.Func<ulong, int, HybridCpuManagedArrayStoreEcallResultV1>? _arrayLoadInt32;
		const std::function<std::shared_ptr<HybridCpuManagedArrayStoreEcallResultV1>(unsigned long long, int)> _arrayLoadInt32;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private readonly System.Func<ulong, int, HybridCpuManagedArrayStoreEcallResultV1>? _arrayLoadInt64;
		const std::function<std::shared_ptr<HybridCpuManagedArrayStoreEcallResultV1>(unsigned long long, int)> _arrayLoadInt64;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private readonly System.Func<long, bool, HybridCpuManagedArrayStoreEcallResultV1>? _nullableValue;
		const std::function<std::shared_ptr<HybridCpuManagedArrayStoreEcallResultV1>(long long, bool)> _nullableValue;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private readonly System.Func<ulong, int, HybridCpuManagedArrayStoreEcallResultV1>? _arrayLoadUInt8;
		const std::function<std::shared_ptr<HybridCpuManagedArrayStoreEcallResultV1>(unsigned long long, int)> _arrayLoadUInt8;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private readonly System.Func<ulong, int, HybridCpuManagedArrayStoreEcallResultV1>? _arrayLoadInt16;
		const std::function<std::shared_ptr<HybridCpuManagedArrayStoreEcallResultV1>(unsigned long long, int)> _arrayLoadInt16;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private readonly System.Func<ulong, int, HybridCpuManagedArrayStoreEcallResultV1>? _arrayLoadUInt16;
		const std::function<std::shared_ptr<HybridCpuManagedArrayStoreEcallResultV1>(unsigned long long, int)> _arrayLoadUInt16;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private readonly System.Func<ulong, ulong, int, HybridCpuManagedArrayStoreEcallResultV1>? _arrayCopyAll;
		const std::function<std::shared_ptr<HybridCpuManagedArrayStoreEcallResultV1>(unsigned long long, unsigned long long, int)> _arrayCopyAll;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private readonly System.Func<ulong, int, ulong, int, int, HybridCpuManagedArrayStoreEcallResultV1>? _arrayCopy;
		const std::function<std::shared_ptr<HybridCpuManagedArrayStoreEcallResultV1>(unsigned long long, int, unsigned long long, int, int)> _arrayCopy;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private readonly System.Func<ulong, int, byte[]?>? _readGuestMemory;
		const std::function<std::optional<std::vector<unsigned char>>(unsigned long long, int)> _readGuestMemory;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private readonly System.Func<ulong, ulong, HybridCpuManagedArrayStoreEcallResultV1>? _isInstance;
		const std::function<std::shared_ptr<HybridCpuManagedArrayStoreEcallResultV1>(unsigned long long, unsigned long long)> _isInstance;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private readonly System.Func<ulong, int, HybridCpuManagedArrayStoreEcallResultV1>? _stringCharacter;
		const std::function<std::shared_ptr<HybridCpuManagedArrayStoreEcallResultV1>(unsigned long long, int)> _stringCharacter;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private readonly System.Func<ulong, ulong, HybridCpuManagedArrayStoreEcallResultV1>? _stringConcat2;
		const std::function<std::shared_ptr<HybridCpuManagedArrayStoreEcallResultV1>(unsigned long long, unsigned long long)> _stringConcat2;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private readonly System.Func<ulong, ulong, ulong, HybridCpuManagedArrayStoreEcallResultV1>? _stringConcat3;
		const std::function<std::shared_ptr<HybridCpuManagedArrayStoreEcallResultV1>(unsigned long long, unsigned long long, unsigned long long)> _stringConcat3;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private readonly System.Func<ulong, HybridCpuManagedArrayStoreEcallResultV1>? _stringLength;
		const std::function<std::shared_ptr<HybridCpuManagedArrayStoreEcallResultV1>(unsigned long long)> _stringLength;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private readonly System.Func<ulong, ulong, HybridCpuManagedArrayStoreEcallResultV1>? _initializeArray;
		const std::function<std::shared_ptr<HybridCpuManagedArrayStoreEcallResultV1>(unsigned long long, unsigned long long)> _initializeArray;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private readonly System.Func<ulong, ulong, HybridCpuManagedArrayStoreEcallResultV1>? _stringFromUtf16Array;
		const std::function<std::shared_ptr<HybridCpuManagedArrayStoreEcallResultV1>(unsigned long long, unsigned long long)> _stringFromUtf16Array;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private readonly System.Func<ulong, int, HybridCpuManagedArrayStoreEcallResultV1>? _newArray;
		const std::function<std::shared_ptr<HybridCpuManagedArrayStoreEcallResultV1>(unsigned long long, int)> _newArray;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private readonly System.Func<ulong, HybridCpuManagedArrayStoreEcallResultV1>? _allocateObject;
		const std::function<std::shared_ptr<HybridCpuManagedArrayStoreEcallResultV1>(unsigned long long)> _allocateObject;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private readonly System.Func<ulong, HybridCpuManagedArrayStoreEcallResultV1>? _arrayEmpty;
		const std::function<std::shared_ptr<HybridCpuManagedArrayStoreEcallResultV1>(unsigned long long)> _arrayEmpty;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private readonly System.Func<ulong, HybridCpuManagedArrayStoreEcallResultV1>? _arrayLength;
		const std::function<std::shared_ptr<HybridCpuManagedArrayStoreEcallResultV1>(unsigned long long)> _arrayLength;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private readonly System.Func<uint, uint, HybridCpuManagedArrayStoreEcallResultV1>? _divideUInt32;
		const std::function<std::shared_ptr<HybridCpuManagedArrayStoreEcallResultV1>(unsigned int, unsigned int)> _divideUInt32;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private readonly System.Func<int, int, HybridCpuManagedArrayStoreEcallResultV1>? _divideInt32;
		const std::function<std::shared_ptr<HybridCpuManagedArrayStoreEcallResultV1>(int, int)> _divideInt32;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private readonly System.Func<int, int, HybridCpuManagedArrayStoreEcallResultV1>? _remainderInt32;
		const std::function<std::shared_ptr<HybridCpuManagedArrayStoreEcallResultV1>(int, int)> _remainderInt32;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private readonly System.Func<long, long, HybridCpuManagedArrayStoreEcallResultV1>? _divideInt64;
		const std::function<std::shared_ptr<HybridCpuManagedArrayStoreEcallResultV1>(long long, long long)> _divideInt64;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private readonly System.Func<long, HybridCpuManagedArrayStoreEcallResultV1>? _mathAbsInt64;
		const std::function<std::shared_ptr<HybridCpuManagedArrayStoreEcallResultV1>(long long)> _mathAbsInt64;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private readonly System.Func<ulong, ulong, HybridCpuManagedArrayStoreEcallResultV1>? _argumentNullCtorParamName;
		const std::function<std::shared_ptr<HybridCpuManagedArrayStoreEcallResultV1>(unsigned long long, unsigned long long)> _argumentNullCtorParamName;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private readonly System.Func<ulong, ulong, HybridCpuManagedArrayStoreEcallResultV1>? _argumentOutOfRangeCtorParamName;
		const std::function<std::shared_ptr<HybridCpuManagedArrayStoreEcallResultV1>(unsigned long long, unsigned long long)> _argumentOutOfRangeCtorParamName;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private readonly System.Func<ulong, ulong, HybridCpuManagedArrayStoreEcallResultV1>? _stringNotEquals;
		const std::function<std::shared_ptr<HybridCpuManagedArrayStoreEcallResultV1>(unsigned long long, unsigned long long)> _stringNotEquals;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private readonly System.Func<ulong, HybridCpuManagedArrayStoreEcallResultV1>? _arrayClear;
		const std::function<std::shared_ptr<HybridCpuManagedArrayStoreEcallResultV1>(unsigned long long)> _arrayClear;

	public:
		std::optional<int> getLastProcessExitCode() const;
	private:
		void setLastProcessExitCode(const std::optional<int> &value);
	public:
		std::shared_ptr<HybridCpuIseBootBlobEcallObservationV1> getLastBootBlobObservation() const;
	private:
		void setLastBootBlobObservation(std::shared_ptr<HybridCpuIseBootBlobEcallObservationV1> value);
	public:
		std::shared_ptr<HybridCPU_ISE::NonRTL::Runtime::HybridCpuIseClockReadEcallObservationV1> getLastClockReadObservation() const;
	private:
		void setLastClockReadObservation(std::shared_ptr<HybridCPU_ISE::NonRTL::Runtime::HybridCpuIseClockReadEcallObservationV1> value);
	public:
		std::shared_ptr<HybridCPU_ISE::NonRTL::Runtime::HybridCpuIseConsoleEcallObservationV1> getLastConsoleObservation() const;
	private:
		void setLastConsoleObservation(std::shared_ptr<HybridCPU_ISE::NonRTL::Runtime::HybridCpuIseConsoleEcallObservationV1> value);
	public:
		unsigned long long getProcessExitRetireCount() const;
	private:
		void setProcessExitRetireCount(unsigned long long value);
	public:
		std::wstring getLastRejectedReason() const;
	private:
		void setLastRejectedReason(const std::wstring &value);
	public:
		unsigned long long getLastManagedOperation() const;
	private:
		void setLastManagedOperation(unsigned long long value);
	public:
		std::optional<HybridCpuManagedShapeStatusV1> getLastManagedStatus() const;
	private:
		void setLastManagedStatus(const std::optional<HybridCpuManagedShapeStatusV1> &value);
	public:
		std::wstring getLastManagedReason() const;
	private:
		void setLastManagedReason(const std::wstring &value);
	public:
		std::optional<HybridCpuExternalServiceStatusV1> getLastManagedExternalStatus() const;
	private:
		void setLastManagedExternalStatus(const std::optional<HybridCpuExternalServiceStatusV1> &value);
	public:
		std::optional<int> getLastManagedExternalError() const;
	private:
		void setLastManagedExternalError(const std::optional<int> &value);
	public:
		unsigned long long getLastManagedExternalValue() const;
	private:
		void setLastManagedExternalValue(unsigned long long value);
	public:
		unsigned long long getLastManagedReceiver() const;
	private:
		void setLastManagedReceiver(unsigned long long value);
	public:
		unsigned long long getLastManagedArgument1() const;
	private:
		void setLastManagedArgument1(unsigned long long value);
	public:
		unsigned long long getLastManagedArgument2() const;
	private:
		void setLastManagedArgument2(unsigned long long value);
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly Queue<HybridCpuManagedEcallObservationV1> _managedObservations = new();
		std::deque<std::shared_ptr<HybridCpuManagedEcallObservationV1>> _managedObservations = std::deque<std::shared_ptr<HybridCpuManagedEcallObservationV1>>();
	public:
		const std::vector<std::shared_ptr<HybridCpuManagedEcallObservationV1>> getManagedObservations() const;

		bool TryBeginCpuInitializer(unsigned long long typeHandle);

		bool TryCompleteCpuInitializer(unsigned long long typeHandle);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER TASK: Throw expressions are not converted by C# to C++ Converter:
// ORIGINAL LINE: public HybridCpuIseManagedEcallBridgeV1(IHybridCpuRuntimeKernelV1 kernel, System.Func<ulong, HybridCpuManagedShapeResultV1>? argumentMessage = null, System.Func<ulong, HybridCpuManagedShapeResultV1>? ensureTypeInitialized = null, System.Nullable<System.Func<ulong, int, int, HybridCpuManagedShapeResultV1>> staticStoreInt32 = null, System.Nullable<System.Func<ulong, int, ulong, HybridCpuManagedShapeResultV1>> staticStoreReference = null, System.Nullable<System.Func<ulong, int, HybridCpuManagedShapeResultV1>> staticLoadInt32 = null, System.Nullable<System.Func<ulong, int, HybridCpuManagedShapeResultV1>> staticLoadReference = null, System.Nullable<System.Func<HybridCpuManagedShapeResultV1>> allocateNullReferenceException = null, System.Nullable<System.Func<ulong, int, int, HybridCpuManagedArrayStoreEcallResultV1>> arrayStoreInt32 = null, System.Nullable<System.Func<ulong, int, int, HybridCpuManagedArrayStoreEcallResultV1>> arrayStoreInt8 = null, System.Nullable<System.Func<ulong, int, int, HybridCpuManagedArrayStoreEcallResultV1>> arrayStoreInt16 = null, System.Nullable<System.Func<ulong, int, ulong, HybridCpuManagedArrayStoreEcallResultV1>> arrayStoreReference = null, System.Nullable<System.Func<ulong, int, HybridCpuManagedArrayStoreEcallResultV1>> arrayLoadReference = null, System.Nullable<System.Func<ulong, int, HybridCpuManagedArrayStoreEcallResultV1>> arrayLoadInt32 = null, System.Nullable<System.Func<ulong, int, HybridCpuManagedArrayStoreEcallResultV1>> arrayLoadUInt8 = null, System.Nullable<System.Func<ulong, int, HybridCpuManagedArrayStoreEcallResultV1>> arrayLoadInt16 = null, System.Nullable<System.Func<ulong, int, HybridCpuManagedArrayStoreEcallResultV1>> arrayLoadUInt16 = null, System.Nullable<System.Func<ulong, ulong, int, HybridCpuManagedArrayStoreEcallResultV1>> arrayCopyAll = null, System.Nullable<System.Func<ulong, ulong, HybridCpuManagedArrayStoreEcallResultV1>> isInstance = null, System.Nullable<System.Func<ulong, int, HybridCpuManagedArrayStoreEcallResultV1>> stringCharacter = null, System.Nullable<System.Func<ulong, ulong, HybridCpuManagedArrayStoreEcallResultV1>> stringConcat2 = null, System.Nullable<System.Func<ulong, ulong, ulong, HybridCpuManagedArrayStoreEcallResultV1>> stringConcat3 = null, System.Nullable<System.Func<ulong, HybridCpuManagedArrayStoreEcallResultV1>> stringLength = null, System.Nullable<System.Func<ulong, ulong, HybridCpuManagedArrayStoreEcallResultV1>> initializeArray = null, System.Nullable<System.Func<ulong, ulong, HybridCpuManagedArrayStoreEcallResultV1>> stringFromUtf16Array = null, System.Nullable<System.Func<ulong, int, HybridCpuManagedArrayStoreEcallResultV1>> newArray = null, System.Nullable<System.Func<ulong, HybridCpuManagedArrayStoreEcallResultV1>> allocateObject = null, System.Nullable<System.Func<ulong, HybridCpuManagedArrayStoreEcallResultV1>> arrayEmpty = null, System.Nullable<System.Func<ulong, HybridCpuManagedArrayStoreEcallResultV1>> arrayLength = null, System.Nullable<System.Func<uint, uint, HybridCpuManagedArrayStoreEcallResultV1>> divideUInt32 = null, System.Nullable<System.Func<int, int, HybridCpuManagedArrayStoreEcallResultV1>> divideInt32 = null, System.Nullable<System.Func<int, int, HybridCpuManagedArrayStoreEcallResultV1>> remainderInt32 = null, System.Nullable<System.Func<long, long, HybridCpuManagedArrayStoreEcallResultV1>> divideInt64 = null, System.Nullable<System.Func<long, HybridCpuManagedArrayStoreEcallResultV1>> mathAbsInt64 = null, System.Nullable<System.Func<ulong, int, ulong, int, int, HybridCpuManagedArrayStoreEcallResultV1>> arrayCopy = null, System.Nullable<System.Func<ulong, int, byte[]?>> readGuestMemory = null, System.Nullable<System.Func<ulong, ulong, HybridCpuManagedArrayStoreEcallResultV1>> argumentNullCtorParamName = null, System.Nullable<System.Func<ulong, ulong, HybridCpuManagedArrayStoreEcallResultV1>> argumentOutOfRangeCtorParamName = null, System.Nullable<System.Func<ulong, ulong, HybridCpuManagedArrayStoreEcallResultV1>> stringNotEquals = null, System.Nullable<System.Func<ulong, HybridCpuManagedArrayStoreEcallResultV1>> arrayClear = null, System.Nullable<System.Func<ulong, int, HybridCpuManagedArrayStoreEcallResultV1>> arrayLoadInt64 = null, System.Nullable<System.Func<long, bool, HybridCpuManagedArrayStoreEcallResultV1>> nullableValue = null)
		HybridCpuIseManagedEcallBridgeV1(std::shared_ptr<IHybridCpuRuntimeKernelV1> kernel, std::function<std::shared_ptr<HybridCpuManagedShapeResultV1>(unsigned long long)> argumentMessage = nullptr, std::function<std::shared_ptr<HybridCpuManagedShapeResultV1>(unsigned long long)> ensureTypeInitialized = nullptr, std::function<std::shared_ptr<HybridCpuManagedShapeResultV1>(unsigned long long, int, int)> staticStoreInt32 = nullptr, std::function<std::shared_ptr<HybridCpuManagedShapeResultV1>(unsigned long long, int, unsigned long long)> staticStoreReference = nullptr, std::function<std::shared_ptr<HybridCpuManagedShapeResultV1>(unsigned long long, int)> staticLoadInt32 = nullptr, std::function<std::shared_ptr<HybridCpuManagedShapeResultV1>(unsigned long long, int)> staticLoadReference = nullptr, std::function<std::shared_ptr<HybridCpuManagedShapeResultV1>()> allocateNullReferenceException = nullptr, std::function<std::shared_ptr<HybridCpuManagedArrayStoreEcallResultV1>(unsigned long long, int, int)> arrayStoreInt32 = nullptr, std::function<std::shared_ptr<HybridCpuManagedArrayStoreEcallResultV1>(unsigned long long, int, int)> arrayStoreInt8 = nullptr, std::function<std::shared_ptr<HybridCpuManagedArrayStoreEcallResultV1>(unsigned long long, int, int)> arrayStoreInt16 = nullptr, std::function<std::shared_ptr<HybridCpuManagedArrayStoreEcallResultV1>(unsigned long long, int, unsigned long long)> arrayStoreReference = nullptr, std::function<std::shared_ptr<HybridCpuManagedArrayStoreEcallResultV1>(unsigned long long, int)> arrayLoadReference = nullptr, std::function<std::shared_ptr<HybridCpuManagedArrayStoreEcallResultV1>(unsigned long long, int)> arrayLoadInt32 = nullptr, std::function<std::shared_ptr<HybridCpuManagedArrayStoreEcallResultV1>(unsigned long long, int)> arrayLoadUInt8 = nullptr, std::function<std::shared_ptr<HybridCpuManagedArrayStoreEcallResultV1>(unsigned long long, int)> arrayLoadInt16 = nullptr, std::function<std::shared_ptr<HybridCpuManagedArrayStoreEcallResultV1>(unsigned long long, int)> arrayLoadUInt16 = nullptr, std::function<std::shared_ptr<HybridCpuManagedArrayStoreEcallResultV1>(unsigned long long, unsigned long long, int)> arrayCopyAll = nullptr, std::function<std::shared_ptr<HybridCpuManagedArrayStoreEcallResultV1>(unsigned long long, unsigned long long)> isInstance = nullptr, std::function<std::shared_ptr<HybridCpuManagedArrayStoreEcallResultV1>(unsigned long long, int)> stringCharacter = nullptr, std::function<std::shared_ptr<HybridCpuManagedArrayStoreEcallResultV1>(unsigned long long, unsigned long long)> stringConcat2 = nullptr, std::function<std::shared_ptr<HybridCpuManagedArrayStoreEcallResultV1>(unsigned long long, unsigned long long, unsigned long long)> stringConcat3 = nullptr, std::function<std::shared_ptr<HybridCpuManagedArrayStoreEcallResultV1>(unsigned long long)> stringLength = nullptr, std::function<std::shared_ptr<HybridCpuManagedArrayStoreEcallResultV1>(unsigned long long, unsigned long long)> initializeArray = nullptr, std::function<std::shared_ptr<HybridCpuManagedArrayStoreEcallResultV1>(unsigned long long, unsigned long long)> stringFromUtf16Array = nullptr, std::function<std::shared_ptr<HybridCpuManagedArrayStoreEcallResultV1>(unsigned long long, int)> newArray = nullptr, std::function<std::shared_ptr<HybridCpuManagedArrayStoreEcallResultV1>(unsigned long long)> allocateObject = nullptr, std::function<std::shared_ptr<HybridCpuManagedArrayStoreEcallResultV1>(unsigned long long)> arrayEmpty = nullptr, std::function<std::shared_ptr<HybridCpuManagedArrayStoreEcallResultV1>(unsigned long long)> arrayLength = nullptr, std::function<std::shared_ptr<HybridCpuManagedArrayStoreEcallResultV1>(unsigned int, unsigned int)> divideUInt32 = nullptr, std::function<std::shared_ptr<HybridCpuManagedArrayStoreEcallResultV1>(int, int)> divideInt32 = nullptr, std::function<std::shared_ptr<HybridCpuManagedArrayStoreEcallResultV1>(int, int)> remainderInt32 = nullptr, std::function<std::shared_ptr<HybridCpuManagedArrayStoreEcallResultV1>(long long, long long)> divideInt64 = nullptr, std::function<std::shared_ptr<HybridCpuManagedArrayStoreEcallResultV1>(long long)> mathAbsInt64 = nullptr, std::function<std::shared_ptr<HybridCpuManagedArrayStoreEcallResultV1>(unsigned long long, int, unsigned long long, int, int)> arrayCopy = nullptr, std::function<std::optional<std::vector<unsigned char>>(unsigned long long, int)> readGuestMemory = nullptr, std::function<std::shared_ptr<HybridCpuManagedArrayStoreEcallResultV1>(unsigned long long, unsigned long long)> argumentNullCtorParamName = nullptr, std::function<std::shared_ptr<HybridCpuManagedArrayStoreEcallResultV1>(unsigned long long, unsigned long long)> argumentOutOfRangeCtorParamName = nullptr, std::function<std::shared_ptr<HybridCpuManagedArrayStoreEcallResultV1>(unsigned long long, unsigned long long)> stringNotEquals = nullptr, std::function<std::shared_ptr<HybridCpuManagedArrayStoreEcallResultV1>(unsigned long long)> arrayClear = nullptr, std::function<std::shared_ptr<HybridCpuManagedArrayStoreEcallResultV1>(unsigned long long, int)> arrayLoadInt64 = nullptr, std::function<std::shared_ptr<HybridCpuManagedArrayStoreEcallResultV1>(long long, bool)> nullableValue = nullptr);

		bool TryHandle(std::shared_ptr<EcallEvent> ecall, std::shared_ptr<ICanonicalCpuState> state, PrivilegeLevel privilege) override;

	private:
		static unsigned long long Read(std::shared_ptr<ICanonicalCpuState> state, unsigned char vt, int register_Keyword);

		static bool CanonicalUInt32(unsigned long long value);

		static bool CanonicalInt32(unsigned long long value);
	};

}
