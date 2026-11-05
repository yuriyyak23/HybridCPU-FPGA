#include "HybridCpuIseManagedEcallBridgeV1.h"
#include "../../../../NonRTL/Runtime/HybridCpuIseBootBlobEcallBindingV1.h"
#include "../../../../NonRTL/Runtime/HybridCpuIseClockReadEcallBindingV1.h"
#include "../../../../NonRTL/Runtime/HybridCpuIseConsoleEcallBindingV1.h"

using namespace HybridCPU::ManagedRuntime;
using namespace HybridCPU::Platform::Contracts;
using namespace HybridCPU::RuntimeKernel;
using namespace HybridCPU_ISE::NonRTL::Runtime;
using namespace YAKSys_Hybrid_CPU::Core;
using namespace YAKSys_Hybrid_CPU::Core::Pipeline;
using namespace YAKSys_Hybrid_CPU::Core::Registers;

namespace HybridCPU_ISE::CloseToHSL::Core::Runtime::Managed
{

	std::optional<int> HybridCpuIseManagedEcallBridgeV1::getLastProcessExitCode() const
	{
		return LastProcessExitCode;
	}

	void HybridCpuIseManagedEcallBridgeV1::setLastProcessExitCode(const std::optional<int> &value)
	{
		LastProcessExitCode = value;
	}

	std::shared_ptr<HybridCpuIseBootBlobEcallObservationV1> HybridCpuIseManagedEcallBridgeV1::getLastBootBlobObservation() const
	{
		return LastBootBlobObservation;
	}

	void HybridCpuIseManagedEcallBridgeV1::setLastBootBlobObservation(std::shared_ptr<HybridCpuIseBootBlobEcallObservationV1> value)
	{
		LastBootBlobObservation = value;
	}

	std::shared_ptr<HybridCpuIseClockReadEcallObservationV1> HybridCpuIseManagedEcallBridgeV1::getLastClockReadObservation() const
	{
		return LastClockReadObservation;
	}

	void HybridCpuIseManagedEcallBridgeV1::setLastClockReadObservation(std::shared_ptr<HybridCpuIseClockReadEcallObservationV1> value)
	{
		LastClockReadObservation = value;
	}

	std::shared_ptr<HybridCpuIseConsoleEcallObservationV1> HybridCpuIseManagedEcallBridgeV1::getLastConsoleObservation() const
	{
		return LastConsoleObservation;
	}

	void HybridCpuIseManagedEcallBridgeV1::setLastConsoleObservation(std::shared_ptr<HybridCpuIseConsoleEcallObservationV1> value)
	{
		LastConsoleObservation = value;
	}

	unsigned long long HybridCpuIseManagedEcallBridgeV1::getProcessExitRetireCount() const
	{
		return ProcessExitRetireCount;
	}

	void HybridCpuIseManagedEcallBridgeV1::setProcessExitRetireCount(unsigned long long value)
	{
		ProcessExitRetireCount = value;
	}

	std::wstring HybridCpuIseManagedEcallBridgeV1::getLastRejectedReason() const
	{
		return LastRejectedReason;
	}

	void HybridCpuIseManagedEcallBridgeV1::setLastRejectedReason(const std::wstring &value)
	{
		LastRejectedReason = value;
	}

	unsigned long long HybridCpuIseManagedEcallBridgeV1::getLastManagedOperation() const
	{
		return LastManagedOperation;
	}

	void HybridCpuIseManagedEcallBridgeV1::setLastManagedOperation(unsigned long long value)
	{
		LastManagedOperation = value;
	}

	std::optional<HybridCpuManagedShapeStatusV1> HybridCpuIseManagedEcallBridgeV1::getLastManagedStatus() const
	{
		return LastManagedStatus;
	}

	void HybridCpuIseManagedEcallBridgeV1::setLastManagedStatus(const std::optional<HybridCpuManagedShapeStatusV1> &value)
	{
		LastManagedStatus = value;
	}

	std::wstring HybridCpuIseManagedEcallBridgeV1::getLastManagedReason() const
	{
		return LastManagedReason;
	}

	void HybridCpuIseManagedEcallBridgeV1::setLastManagedReason(const std::wstring &value)
	{
		LastManagedReason = value;
	}

	std::optional<HybridCpuExternalServiceStatusV1> HybridCpuIseManagedEcallBridgeV1::getLastManagedExternalStatus() const
	{
		return LastManagedExternalStatus;
	}

	void HybridCpuIseManagedEcallBridgeV1::setLastManagedExternalStatus(const std::optional<HybridCpuExternalServiceStatusV1> &value)
	{
		LastManagedExternalStatus = value;
	}

	std::optional<int> HybridCpuIseManagedEcallBridgeV1::getLastManagedExternalError() const
	{
		return LastManagedExternalError;
	}

	void HybridCpuIseManagedEcallBridgeV1::setLastManagedExternalError(const std::optional<int> &value)
	{
		LastManagedExternalError = value;
	}

	unsigned long long HybridCpuIseManagedEcallBridgeV1::getLastManagedExternalValue() const
	{
		return LastManagedExternalValue;
	}

	void HybridCpuIseManagedEcallBridgeV1::setLastManagedExternalValue(unsigned long long value)
	{
		LastManagedExternalValue = value;
	}

	unsigned long long HybridCpuIseManagedEcallBridgeV1::getLastManagedReceiver() const
	{
		return LastManagedReceiver;
	}

	void HybridCpuIseManagedEcallBridgeV1::setLastManagedReceiver(unsigned long long value)
	{
		LastManagedReceiver = value;
	}

	unsigned long long HybridCpuIseManagedEcallBridgeV1::getLastManagedArgument1() const
	{
		return LastManagedArgument1;
	}

	void HybridCpuIseManagedEcallBridgeV1::setLastManagedArgument1(unsigned long long value)
	{
		LastManagedArgument1 = value;
	}

	unsigned long long HybridCpuIseManagedEcallBridgeV1::getLastManagedArgument2() const
	{
		return LastManagedArgument2;
	}

	void HybridCpuIseManagedEcallBridgeV1::setLastManagedArgument2(unsigned long long value)
	{
		LastManagedArgument2 = value;
	}

	const std::vector<std::shared_ptr<HybridCpuManagedEcallObservationV1>> HybridCpuIseManagedEcallBridgeV1::getManagedObservations() const
	{
		return std::vector<std::shared_ptr<HybridCpuManagedEcallObservationV1>>(
			_managedObservations.begin(), _managedObservations.end());
	}

	bool HybridCpuIseManagedEcallBridgeV1::TryBeginCpuInitializer(unsigned long long typeHandle)
	{
		return typeHandle != 0 && _activeCpuInitializerHandles.insert(typeHandle).second;
	}

	bool HybridCpuIseManagedEcallBridgeV1::TryCompleteCpuInitializer(unsigned long long typeHandle)
	{
		return typeHandle != 0 && _activeCpuInitializerHandles.erase(typeHandle) != 0;
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER TASK: Throw expressions are not converted by C# to C++ Converter:
// ORIGINAL LINE: public HybridCpuIseManagedEcallBridgeV1(IHybridCpuRuntimeKernelV1 kernel, System.Func<ulong, HybridCpuManagedShapeResultV1>? argumentMessage = null, System.Func<ulong, HybridCpuManagedShapeResultV1>? ensureTypeInitialized = null, System.Nullable<System.Func<ulong, int, int, HybridCpuManagedShapeResultV1>> staticStoreInt32 = null, System.Nullable<System.Func<ulong, int, ulong, HybridCpuManagedShapeResultV1>> staticStoreReference = null, System.Nullable<System.Func<ulong, int, HybridCpuManagedShapeResultV1>> staticLoadInt32 = null, System.Nullable<System.Func<ulong, int, HybridCpuManagedShapeResultV1>> staticLoadReference = null, System.Nullable<System.Func<HybridCpuManagedShapeResultV1>> allocateNullReferenceException = null, System.Nullable<System.Func<ulong, int, int, HybridCpuManagedArrayStoreEcallResultV1>> arrayStoreInt32 = null, System.Nullable<System.Func<ulong, int, int, HybridCpuManagedArrayStoreEcallResultV1>> arrayStoreInt8 = null, System.Nullable<System.Func<ulong, int, int, HybridCpuManagedArrayStoreEcallResultV1>> arrayStoreInt16 = null, System.Nullable<System.Func<ulong, int, ulong, HybridCpuManagedArrayStoreEcallResultV1>> arrayStoreReference = null, System.Nullable<System.Func<ulong, int, HybridCpuManagedArrayStoreEcallResultV1>> arrayLoadReference = null, System.Nullable<System.Func<ulong, int, HybridCpuManagedArrayStoreEcallResultV1>> arrayLoadInt32 = null, System.Nullable<System.Func<ulong, int, HybridCpuManagedArrayStoreEcallResultV1>> arrayLoadUInt8 = null, System.Nullable<System.Func<ulong, int, HybridCpuManagedArrayStoreEcallResultV1>> arrayLoadInt16 = null, System.Nullable<System.Func<ulong, int, HybridCpuManagedArrayStoreEcallResultV1>> arrayLoadUInt16 = null, System.Nullable<System.Func<ulong, ulong, int, HybridCpuManagedArrayStoreEcallResultV1>> arrayCopyAll = null, System.Nullable<System.Func<ulong, ulong, HybridCpuManagedArrayStoreEcallResultV1>> isInstance = null, System.Nullable<System.Func<ulong, int, HybridCpuManagedArrayStoreEcallResultV1>> stringCharacter = null, System.Nullable<System.Func<ulong, ulong, HybridCpuManagedArrayStoreEcallResultV1>> stringConcat2 = null, System.Nullable<System.Func<ulong, ulong, ulong, HybridCpuManagedArrayStoreEcallResultV1>> stringConcat3 = null, System.Nullable<System.Func<ulong, HybridCpuManagedArrayStoreEcallResultV1>> stringLength = null, System.Nullable<System.Func<ulong, ulong, HybridCpuManagedArrayStoreEcallResultV1>> initializeArray = null, System.Nullable<System.Func<ulong, ulong, HybridCpuManagedArrayStoreEcallResultV1>> stringFromUtf16Array = null, System.Nullable<System.Func<ulong, int, HybridCpuManagedArrayStoreEcallResultV1>> newArray = null, System.Nullable<System.Func<ulong, HybridCpuManagedArrayStoreEcallResultV1>> allocateObject = null, System.Nullable<System.Func<ulong, HybridCpuManagedArrayStoreEcallResultV1>> arrayEmpty = null, System.Nullable<System.Func<ulong, HybridCpuManagedArrayStoreEcallResultV1>> arrayLength = null, System.Nullable<System.Func<uint, uint, HybridCpuManagedArrayStoreEcallResultV1>> divideUInt32 = null, System.Nullable<System.Func<int, int, HybridCpuManagedArrayStoreEcallResultV1>> divideInt32 = null, System.Nullable<System.Func<int, int, HybridCpuManagedArrayStoreEcallResultV1>> remainderInt32 = null, System.Nullable<System.Func<long, long, HybridCpuManagedArrayStoreEcallResultV1>> divideInt64 = null, System.Nullable<System.Func<long, HybridCpuManagedArrayStoreEcallResultV1>> mathAbsInt64 = null, System.Nullable<System.Func<ulong, int, ulong, int, int, HybridCpuManagedArrayStoreEcallResultV1>> arrayCopy = null, System.Nullable<System.Func<ulong, int, byte[]?>> readGuestMemory = null, System.Nullable<System.Func<ulong, ulong, HybridCpuManagedArrayStoreEcallResultV1>> argumentNullCtorParamName = null, System.Nullable<System.Func<ulong, ulong, HybridCpuManagedArrayStoreEcallResultV1>> argumentOutOfRangeCtorParamName = null, System.Nullable<System.Func<ulong, ulong, HybridCpuManagedArrayStoreEcallResultV1>> stringNotEquals = null, System.Nullable<System.Func<ulong, HybridCpuManagedArrayStoreEcallResultV1>> arrayClear = null, System.Nullable<System.Func<ulong, int, HybridCpuManagedArrayStoreEcallResultV1>> arrayLoadInt64 = null, System.Nullable<System.Func<long, bool, HybridCpuManagedArrayStoreEcallResultV1>> nullableValue = null)
	HybridCpuIseManagedEcallBridgeV1::HybridCpuIseManagedEcallBridgeV1(std::shared_ptr<IHybridCpuRuntimeKernelV1> kernel, std::function<std::shared_ptr<HybridCpuManagedShapeResultV1>(unsigned long long)> argumentMessage, std::function<std::shared_ptr<HybridCpuManagedShapeResultV1>(unsigned long long)> ensureTypeInitialized, std::function<std::shared_ptr<HybridCpuManagedShapeResultV1>(unsigned long long, int, int)> staticStoreInt32, std::function<std::shared_ptr<HybridCpuManagedShapeResultV1>(unsigned long long, int, unsigned long long)> staticStoreReference, std::function<std::shared_ptr<HybridCpuManagedShapeResultV1>(unsigned long long, int)> staticLoadInt32, std::function<std::shared_ptr<HybridCpuManagedShapeResultV1>(unsigned long long, int)> staticLoadReference, std::function<std::shared_ptr<HybridCpuManagedShapeResultV1>()> allocateNullReferenceException, std::function<std::shared_ptr<HybridCpuManagedArrayStoreEcallResultV1>(unsigned long long, int, int)> arrayStoreInt32, std::function<std::shared_ptr<HybridCpuManagedArrayStoreEcallResultV1>(unsigned long long, int, int)> arrayStoreInt8, std::function<std::shared_ptr<HybridCpuManagedArrayStoreEcallResultV1>(unsigned long long, int, int)> arrayStoreInt16, std::function<std::shared_ptr<HybridCpuManagedArrayStoreEcallResultV1>(unsigned long long, int, unsigned long long)> arrayStoreReference, std::function<std::shared_ptr<HybridCpuManagedArrayStoreEcallResultV1>(unsigned long long, int)> arrayLoadReference, std::function<std::shared_ptr<HybridCpuManagedArrayStoreEcallResultV1>(unsigned long long, int)> arrayLoadInt32, std::function<std::shared_ptr<HybridCpuManagedArrayStoreEcallResultV1>(unsigned long long, int)> arrayLoadUInt8, std::function<std::shared_ptr<HybridCpuManagedArrayStoreEcallResultV1>(unsigned long long, int)> arrayLoadInt16, std::function<std::shared_ptr<HybridCpuManagedArrayStoreEcallResultV1>(unsigned long long, int)> arrayLoadUInt16, std::function<std::shared_ptr<HybridCpuManagedArrayStoreEcallResultV1>(unsigned long long, unsigned long long, int)> arrayCopyAll, std::function<std::shared_ptr<HybridCpuManagedArrayStoreEcallResultV1>(unsigned long long, unsigned long long)> isInstance, std::function<std::shared_ptr<HybridCpuManagedArrayStoreEcallResultV1>(unsigned long long, int)> stringCharacter, std::function<std::shared_ptr<HybridCpuManagedArrayStoreEcallResultV1>(unsigned long long, unsigned long long)> stringConcat2, std::function<std::shared_ptr<HybridCpuManagedArrayStoreEcallResultV1>(unsigned long long, unsigned long long, unsigned long long)> stringConcat3, std::function<std::shared_ptr<HybridCpuManagedArrayStoreEcallResultV1>(unsigned long long)> stringLength, std::function<std::shared_ptr<HybridCpuManagedArrayStoreEcallResultV1>(unsigned long long, unsigned long long)> initializeArray, std::function<std::shared_ptr<HybridCpuManagedArrayStoreEcallResultV1>(unsigned long long, unsigned long long)> stringFromUtf16Array, std::function<std::shared_ptr<HybridCpuManagedArrayStoreEcallResultV1>(unsigned long long, int)> newArray, std::function<std::shared_ptr<HybridCpuManagedArrayStoreEcallResultV1>(unsigned long long)> allocateObject, std::function<std::shared_ptr<HybridCpuManagedArrayStoreEcallResultV1>(unsigned long long)> arrayEmpty, std::function<std::shared_ptr<HybridCpuManagedArrayStoreEcallResultV1>(unsigned long long)> arrayLength, std::function<std::shared_ptr<HybridCpuManagedArrayStoreEcallResultV1>(unsigned int, unsigned int)> divideUInt32, std::function<std::shared_ptr<HybridCpuManagedArrayStoreEcallResultV1>(int, int)> divideInt32, std::function<std::shared_ptr<HybridCpuManagedArrayStoreEcallResultV1>(int, int)> remainderInt32, std::function<std::shared_ptr<HybridCpuManagedArrayStoreEcallResultV1>(long long, long long)> divideInt64, std::function<std::shared_ptr<HybridCpuManagedArrayStoreEcallResultV1>(long long)> mathAbsInt64, std::function<std::shared_ptr<HybridCpuManagedArrayStoreEcallResultV1>(unsigned long long, int, unsigned long long, int, int)> arrayCopy, std::function<std::optional<std::vector<unsigned char>>(unsigned long long, int)> readGuestMemory, std::function<std::shared_ptr<HybridCpuManagedArrayStoreEcallResultV1>(unsigned long long, unsigned long long)> argumentNullCtorParamName, std::function<std::shared_ptr<HybridCpuManagedArrayStoreEcallResultV1>(unsigned long long, unsigned long long)> argumentOutOfRangeCtorParamName, std::function<std::shared_ptr<HybridCpuManagedArrayStoreEcallResultV1>(unsigned long long, unsigned long long)> stringNotEquals, std::function<std::shared_ptr<HybridCpuManagedArrayStoreEcallResultV1>(unsigned long long)> arrayClear, std::function<std::shared_ptr<HybridCpuManagedArrayStoreEcallResultV1>(unsigned long long, int)> arrayLoadInt64, std::function<std::shared_ptr<HybridCpuManagedArrayStoreEcallResultV1>(long long, bool)> nullableValue) : _kernel((kernel != nullptr) ? kernel : throw std::invalid_argument("kernel")), _argumentMessage(argumentMessage), _ensureTypeInitialized(ensureTypeInitialized), _staticStoreInt32(staticStoreInt32), _staticStoreReference(staticStoreReference), _staticLoadInt32(staticLoadInt32), _staticLoadReference(staticLoadReference), _allocateNullReferenceException(allocateNullReferenceException), _arrayStoreInt32(arrayStoreInt32), _arrayStoreInt8(arrayStoreInt8), _arrayStoreInt16(arrayStoreInt16), _arrayStoreReference(arrayStoreReference), _arrayLoadReference(arrayLoadReference), _arrayLoadInt32(arrayLoadInt32), _arrayLoadInt64(arrayLoadInt64), _nullableValue(nullableValue), _arrayLoadUInt8(arrayLoadUInt8), _arrayLoadInt16(arrayLoadInt16), _arrayLoadUInt16(arrayLoadUInt16), _arrayCopyAll(arrayCopyAll), _arrayCopy(arrayCopy), _readGuestMemory(readGuestMemory), _isInstance(isInstance), _stringCharacter(stringCharacter), _stringConcat2(stringConcat2), _stringConcat3(stringConcat3), _stringLength(stringLength), _initializeArray(initializeArray), _stringFromUtf16Array(stringFromUtf16Array), _newArray(newArray), _allocateObject(allocateObject), _arrayEmpty(arrayEmpty), _arrayLength(arrayLength), _divideUInt32(divideUInt32), _divideInt32(divideInt32), _remainderInt32(remainderInt32), _divideInt64(divideInt64), _mathAbsInt64(mathAbsInt64), _argumentNullCtorParamName(argumentNullCtorParamName), _argumentOutOfRangeCtorParamName(argumentOutOfRangeCtorParamName), _stringNotEquals(stringNotEquals), _arrayClear(arrayClear)
	{
		// ProcessExit is an intrinsic exact bridge operation. A restricted image that uses
		// only the terminal process boundary still requires a bridge even when it imports no
		// optional managed-runtime delegate operation.
	}

	bool HybridCpuIseManagedEcallBridgeV1::TryHandle(std::shared_ptr<EcallEvent> ecall, std::shared_ptr<ICanonicalCpuState> state, PrivilegeLevel privilege)
	{
		if (!ecall || !state)
			throw std::invalid_argument("ecall and state are required");
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: ulong code = unchecked((ulong)ecall.EcallCode);
		unsigned long long code = static_cast<unsigned long long>(ecall->EcallCode);
		unsigned long long service = Read(state, ecall->VtId, HybridCpuExternalServiceEcallContractV1::ServiceRegister);
		if (code != HybridCpuExternalServiceEcallContractV1::EcallNumber)
		{
			setLastRejectedReason(std::format(L"ECALL number 0x{:016x} does not match HCSV 0x{:016x}.", code, HybridCpuExternalServiceEcallContractV1::EcallNumber));
			return false;
		}

		// A handled ECALL must retire to the next fixed-width bundle.  Refuse the
		// bridge when that architectural PC cannot be represented, leaving the
		// ordinary trap path in sole control instead of throwing on the host.
		unsigned long long pc = state->ReadPc(ecall->VtId);
		if (pc > std::numeric_limits<unsigned long long>::max() - VliwBundleBytes)
		{
			return false;
		}

		std::shared_ptr<HybridCpuExecutionContextDescriptorV1> context = _kernel->CurrentContext();
		unsigned long long operation = Read(state, ecall->VtId, HybridCpuExternalServiceEcallContractV1::OperationRegister);
		unsigned long long buffer = Read(state, ecall->VtId, HybridCpuExternalServiceEcallContractV1::BufferAddressRegister);
		unsigned long long length = Read(state, ecall->VtId, HybridCpuExternalServiceEcallContractV1::BufferLengthRegister);
		unsigned long long access = Read(state, ecall->VtId, HybridCpuExternalServiceEcallContractV1::BufferAccessRegister);
		unsigned long long count = Read(state, ecall->VtId, HybridCpuExternalServiceEcallContractV1::ArgumentCountRegister);
		unsigned long long receiver = Read(state, ecall->VtId, HybridCpuExternalServiceEcallContractV1::FirstArgumentRegister);
		unsigned long long argument1 = Read(state, ecall->VtId, HybridCpuExternalServiceEcallContractV1::SecondArgumentRegister);
		unsigned long long argument2 = Read(state, ecall->VtId, HybridCpuExternalServiceEcallContractV1::ThirdArgumentRegister);
		if (service == static_cast<unsigned long long>(HybridCpuHostServiceV1::ProcessExit))
		{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: if (privilege != PrivilegeLevel.User || context == null || context.VirtualThreadCarrier != ecall.VtId || operation != 0 || buffer != 0 || length != 0 || access != (ulong)HybridCpuHostBufferAccessV1.None || count != 1 || argument1 != 0 || argument2 != 0 || receiver != unchecked((ulong)(long)(int)(uint)receiver))
			if (privilege != PrivilegeLevel::User || !context || context->VirtualThreadCarrier != ecall->VtId || operation != 0 || buffer != 0 || length != 0 || access != static_cast<unsigned long long>(HybridCpuHostBufferAccessV1::None) || count != 1 || argument1 != 0 || argument2 != 0 || receiver != static_cast<unsigned long long>(static_cast<long long>(static_cast<int>(static_cast<unsigned int>(receiver)))))
			{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'ToString':
				setLastRejectedReason(std::format(L"ProcessExit envelope rejected: privilege={},context={},context-vt={},event-vt={},operation={},buffer=0x{:x},length={},access={},count={},receiver=0x{:x},argument1=0x{:x},argument2=0x{:x}.",
					static_cast<int>(privilege), context ? L"present" : L"null", context ? context->VirtualThreadCarrier : -1,
					ecall->VtId, operation, buffer, length, access, count, receiver, argument1, argument2));
				return false;
			}
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: int exitCode = unchecked((int)(uint)receiver);
			int exitCode = static_cast<int>(static_cast<unsigned int>(receiver));
			std::shared_ptr<HybridCpuKernelResultV1> exited = _kernel->ProcessExit(exitCode);
			if (!exited->IsSuccess)
			{
				setLastRejectedReason(L"RuntimeKernel rejected ProcessExit: " + exited->Reason);
				return false;
			}
			setLastProcessExitCode(exitCode);
			setProcessExitRetireCount(getProcessExitRetireCount() + 1);
			state->WritePc(ecall->VtId, pc + VliwBundleBytes);
			return true;
		}
		if (service != static_cast<unsigned long long>(HybridCpuHostServiceV1::ManagedRuntime))
		{
			std::optional<std::shared_ptr<HybridCpuIseBootBlobEcallObservationV1>> observation;
			bool handled = HybridCpuIseBootBlobEcallBindingV1::TryHandle(_kernel, ecall, state, privilege, observation);
			if (observation && *observation)
			{
				setLastBootBlobObservation(*observation);
			}
			if (!handled)
			{
				std::optional<std::shared_ptr<HybridCpuIseClockReadEcallObservationV1>> clock;
				handled = HybridCpuIseClockReadEcallBindingV1::TryHandle(_kernel, ecall, state, privilege, clock);
				if (clock && *clock)
				{
					setLastClockReadObservation(*clock);
				}
			}
			if (!handled)
			{
				std::optional<std::shared_ptr<HybridCpuIseConsoleEcallObservationV1>> console;
				handled = HybridCpuIseConsoleEcallBindingV1::TryHandle(_kernel, ecall, state, privilege, console);
				if (console && *console)
				{
					setLastConsoleObservation(*console);
				}
			}
			if (!handled)
			{
				setLastRejectedReason(std::format(L"HCSV service has no admitted ISE binding: service={},operation=0x{:x},pc=0x{:x},x1=0x{:x},buffer=0x{:x},length={},access={},count={},arg0=0x{:x},privilege={}.",
					service, operation, pc, Read(state, ecall->VtId, 1), buffer, length, access, count, receiver,
					static_cast<int>(privilege)));
			}
			return handled;
		}
		setLastManagedReceiver(receiver);
		setLastManagedArgument1(argument1);
		setLastManagedArgument2(argument2);
		setLastManagedStatus(std::nullopt);
		setLastManagedReason(L"");
		int temp_var163;
		switch (operation)
		{
			case HybridCpuManagedRuntimeEcallContractV1::StaticStoreInt32Operation:
				temp_var163 = 3;
				break;
			case HybridCpuManagedRuntimeEcallContractV1::StaticStoreReferenceOperation:
				temp_var163 = 3;
				break;
			case HybridCpuManagedRuntimeEcallContractV1::StaticLoadInt32Operation:
				temp_var163 = 2;
				break;
			case HybridCpuManagedRuntimeEcallContractV1::StaticLoadReferenceOperation:
				temp_var163 = 2;
				break;
			case HybridCpuManagedRuntimeEcallContractV1::AllocateNullReferenceExceptionOperation:
				temp_var163 = 0;
				break;
			case HybridCpuManagedRuntimeEcallContractV1::ArrayStoreInt32Operation:
				temp_var163 = 3;
				break;
			case HybridCpuManagedRuntimeEcallContractV1::ArrayStoreInt8Operation:
				temp_var163 = 3;
				break;
			case HybridCpuManagedRuntimeEcallContractV1::ArrayStoreInt16Operation:
				temp_var163 = 3;
				break;
			case HybridCpuManagedRuntimeEcallContractV1::ArrayStoreReferenceOperation:
				temp_var163 = 3;
				break;
			case HybridCpuManagedRuntimeEcallContractV1::ArrayLoadReferenceOperation:
				temp_var163 = 2;
				break;
			case HybridCpuManagedRuntimeEcallContractV1::ArrayLoadInt32Operation:
				temp_var163 = 2;
				break;
			case HybridCpuManagedRuntimeEcallContractV1::ArrayLoadInt64Operation:
				temp_var163 = 2;
				break;
			case HybridCpuManagedRuntimeEcallContractV1::ArrayLoadUInt8Operation:
				temp_var163 = 2;
				break;
			case HybridCpuManagedRuntimeEcallContractV1::ArrayLoadInt16Operation:
				temp_var163 = 2;
				break;
			case HybridCpuManagedRuntimeEcallContractV1::ArrayLoadUInt16Operation:
				temp_var163 = 2;
				break;
			case HybridCpuManagedRuntimeEcallContractV1::ArrayCopyAllOperation:
				temp_var163 = 3;
				break;
			case HybridCpuManagedRuntimeEcallContractV1::ArrayCopyOperation:
				temp_var163 = 0;
				break;
			case HybridCpuManagedRuntimeEcallContractV1::IsInstanceOperation:
				temp_var163 = 2;
				break;
			case HybridCpuManagedRuntimeEcallContractV1::StringCharacterOperation:
				temp_var163 = 2;
				break;
			case HybridCpuManagedRuntimeEcallContractV1::StringConcat2Operation:
				temp_var163 = 2;
				break;
			case HybridCpuManagedRuntimeEcallContractV1::StringConcat3Operation:
				temp_var163 = 3;
				break;
			case HybridCpuManagedRuntimeEcallContractV1::StringLengthOperation:
				temp_var163 = 1;
				break;
			case HybridCpuManagedRuntimeEcallContractV1::InitializeArrayOperation:
				temp_var163 = 2;
				break;
			case HybridCpuManagedRuntimeEcallContractV1::StringFromUtf16ArrayOperation:
				temp_var163 = 2;
				break;
			case HybridCpuManagedRuntimeEcallContractV1::ArgumentNullCtorParamNameOperation:
				temp_var163 = 2;
				break;
			case HybridCpuManagedRuntimeEcallContractV1::ArgumentOutOfRangeCtorParamNameOperation:
				temp_var163 = 2;
				break;
			case HybridCpuManagedRuntimeEcallContractV1::StringNotEqualsOperation:
				temp_var163 = 2;
				break;
			case HybridCpuManagedRuntimeEcallContractV1::ArrayClearOperation:
				temp_var163 = 1;
				break;
			case HybridCpuManagedRuntimeEcallContractV1::NewArrayOperation:
				temp_var163 = 2;
				break;
			case HybridCpuManagedRuntimeEcallContractV1::AllocateObjectOperation:
				temp_var163 = 1;
				break;
			case HybridCpuManagedRuntimeEcallContractV1::ArrayEmptyOperation:
				temp_var163 = 1;
				break;
			case HybridCpuManagedRuntimeEcallContractV1::ArrayLengthOperation:
				temp_var163 = 1;
				break;
			case HybridCpuManagedRuntimeEcallContractV1::DivideUInt32CheckedOperation:
				temp_var163 = 2;
				break;
			case HybridCpuManagedRuntimeEcallContractV1::DivideInt32CheckedOperation:
				temp_var163 = 2;
				break;
			case HybridCpuManagedRuntimeEcallContractV1::RemainderInt32CheckedOperation:
				temp_var163 = 2;
				break;
			case HybridCpuManagedRuntimeEcallContractV1::DivideInt64CheckedOperation:
				temp_var163 = 2;
				break;
			case HybridCpuManagedRuntimeEcallContractV1::MathAbsInt64CheckedOperation:
				temp_var163 = 1;
				break;
			case HybridCpuManagedRuntimeEcallContractV1::NullableValueInt64CheckedOperation:
				temp_var163 = 2;
				break;
			default:
				temp_var163 = 1;
				break;
		}
		unsigned long long expectedCount = temp_var163;
		HybridCpuExternalServiceStatusV1 status = HybridCpuExternalServiceStatusV1::InvalidRequest;
		unsigned long long value = 0;
		int error = 0;
		bool isArrayCopyBlock = operation == HybridCpuManagedRuntimeEcallContractV1::ArrayCopyOperation;
		bool validBuffer = isArrayCopyBlock ? buffer != 0 && length == HybridCpuManagedRuntimeEcallContractV1::ArrayCopyArgumentBlockBytes && access == static_cast<unsigned long long>(HybridCpuHostBufferAccessV1::Read) : buffer == 0 && length == 0 && access == static_cast<unsigned long long>(HybridCpuHostBufferAccessV1::None);
		if (privilege != PrivilegeLevel::User || !context || context->VirtualThreadCarrier != ecall->VtId || !validBuffer || count != expectedCount)
		{
			status = HybridCpuExternalServiceStatusV1::InvalidRequest;
			error = 22;
		}
		else
		{
			if (operation == HybridCpuManagedRuntimeEcallContractV1::ArgumentNullCtorParamNameOperation && _argumentNullCtorParamName != nullptr)
			{
				std::shared_ptr<HybridCpuManagedArrayStoreEcallResultV1> outcome;
				try
				{
					outcome = _argumentNullCtorParamName(receiver, argument1);
				}
				catch (const std::exception &)
				{
					outcome = std::make_shared<HybridCpuManagedArrayStoreEcallResultV1>(false, 0, L"Managed ArgumentNullException constructor exception was contained.");
				}
				if (outcome->IsSuccess)
				{
					status = HybridCpuExternalServiceStatusV1::Success;
					value = outcome->ExceptionReference;
					error = outcome->IsManagedThrow ? 1 : 0;
				}
				else
				{
					status = HybridCpuExternalServiceStatusV1::ProviderFailure;
					error = 5;
				}
				goto Publish;
			}
			if (operation == HybridCpuManagedRuntimeEcallContractV1::ArgumentOutOfRangeCtorParamNameOperation && _argumentOutOfRangeCtorParamName != nullptr)
			{
				std::shared_ptr<HybridCpuManagedArrayStoreEcallResultV1> outcome;
				try
				{
					outcome = _argumentOutOfRangeCtorParamName(receiver, argument1);
				}
				catch (const std::exception &)
				{
					outcome = std::make_shared<HybridCpuManagedArrayStoreEcallResultV1>(false, 0, L"Managed ArgumentOutOfRangeException constructor exception was contained.");
				}
				if (outcome->IsSuccess)
				{
					status = HybridCpuExternalServiceStatusV1::Success;
					value = outcome->ExceptionReference;
					error = outcome->IsManagedThrow ? 1 : 0;
				}
				else
				{
					status = HybridCpuExternalServiceStatusV1::ProviderFailure;
					error = 5;
				}
				goto Publish;
			}
			if (operation == HybridCpuManagedRuntimeEcallContractV1::StringNotEqualsOperation && _stringNotEquals != nullptr)
			{
				std::shared_ptr<HybridCpuManagedArrayStoreEcallResultV1> outcome;
				try
				{
					outcome = _stringNotEquals(receiver, argument1);
				}
				catch (const std::exception &)
				{
					outcome = std::make_shared<HybridCpuManagedArrayStoreEcallResultV1>(false, 0, L"Managed string inequality exception was contained.");
				}
				if (outcome->IsSuccess)
				{
					status = HybridCpuExternalServiceStatusV1::Success;
					value = outcome->ExceptionReference;
					error = 0;
				}
				else
				{
					status = HybridCpuExternalServiceStatusV1::ProviderFailure;
					error = 5;
				}
				goto Publish;
			}
			if (operation == HybridCpuManagedRuntimeEcallContractV1::ArrayClearOperation && _arrayClear != nullptr)
			{
				std::shared_ptr<HybridCpuManagedArrayStoreEcallResultV1> outcome;
				try
				{
					outcome = _arrayClear(receiver);
				}
				catch (const std::exception &)
				{
					outcome = std::make_shared<HybridCpuManagedArrayStoreEcallResultV1>(false, 0, L"Managed Array.Clear exception was contained.");
				}
				if (outcome->IsSuccess)
				{
					status = HybridCpuExternalServiceStatusV1::Success;
					value = outcome->ExceptionReference;
					error = outcome->IsManagedThrow ? 1 : 0;
				}
				else
				{
					status = HybridCpuExternalServiceStatusV1::ProviderFailure;
					error = 5;
				}
				goto Publish;
			}
			if (operation == HybridCpuManagedRuntimeEcallContractV1::ArrayCopyOperation && _arrayCopy != nullptr && _readGuestMemory != nullptr)
			{
				std::optional<std::vector<unsigned char>> block;
				try
				{
					block = _readGuestMemory(buffer, HybridCpuManagedRuntimeEcallContractV1::ArrayCopyArgumentBlockBytes);
				}
				catch (const std::exception &)
				{
					block.reset();
				}
				if (!block || block->size() != static_cast<std::size_t>(HybridCpuManagedRuntimeEcallContractV1::ArrayCopyArgumentBlockBytes))
				{
					status = HybridCpuExternalServiceStatusV1::InvalidRequest;
					error = 14;
					goto Publish;
				}
				const auto readUInt64LittleEndian = [&block](std::size_t offset) {
					unsigned long long value = 0;
					for (std::size_t byte = 0; byte < 8; ++byte)
						value |= static_cast<unsigned long long>((*block)[offset + byte]) << (byte * 8);
					return value;
				};
				const unsigned long long source = readUInt64LittleEndian(0);
				const long long sourceIndex64 = static_cast<long long>(readUInt64LittleEndian(8));
				const unsigned long long destination = readUInt64LittleEndian(16);
				const long long destinationIndex64 = static_cast<long long>(readUInt64LittleEndian(24));
				const long long copyLength64 = static_cast<long long>(readUInt64LittleEndian(32));
				if (sourceIndex64 < (std::numeric_limits<int>::min)() || sourceIndex64 > (std::numeric_limits<int>::max)() ||
					destinationIndex64 < (std::numeric_limits<int>::min)() || destinationIndex64 > (std::numeric_limits<int>::max)() ||
					copyLength64 < (std::numeric_limits<int>::min)() || copyLength64 > (std::numeric_limits<int>::max)())
				{
					status = HybridCpuExternalServiceStatusV1::InvalidRequest;
					error = 22;
					goto Publish;
				}
				std::shared_ptr<HybridCpuManagedArrayStoreEcallResultV1> outcome;
				try
				{
					outcome = _arrayCopy(source, static_cast<int>(sourceIndex64), destination, static_cast<int>(destinationIndex64), static_cast<int>(copyLength64));
				}
				catch (const std::exception &)
				{
					outcome = std::make_shared<HybridCpuManagedArrayStoreEcallResultV1>(false, 0, L"Managed Array.Copy runtime exception was contained.");
				}
				if (outcome->IsSuccess)
				{
					status = HybridCpuExternalServiceStatusV1::Success;
					value = outcome->ExceptionReference;
					error = outcome->IsManagedThrow ? 1 : 0;
				}
				else
				{
					status = HybridCpuExternalServiceStatusV1::ProviderFailure;
					error = 5;
				}
				goto Publish;
			}
			if (operation == HybridCpuManagedRuntimeEcallContractV1::ArrayEmptyOperation && _arrayEmpty != nullptr)
			{
				std::shared_ptr<HybridCpuManagedArrayStoreEcallResultV1> outcome;
				try
				{
					outcome = _arrayEmpty(receiver);
				}
				catch (const std::exception &)
				{
					outcome = std::make_shared<HybridCpuManagedArrayStoreEcallResultV1>(false, 0, L"Managed Array.Empty runtime exception was contained.");
				}
				if (outcome->IsSuccess)
				{
					status = HybridCpuExternalServiceStatusV1::Success;
					value = outcome->ExceptionReference;
					error = outcome->IsManagedThrow ? 1 : 0;
				}
				else
				{
					status = HybridCpuExternalServiceStatusV1::ProviderFailure;
					error = 5;
				}
				goto Publish;
			}
			if (operation == HybridCpuManagedRuntimeEcallContractV1::ArrayLengthOperation && _arrayLength != nullptr)
			{
				std::shared_ptr<HybridCpuManagedArrayStoreEcallResultV1> outcome;
				try
				{
					outcome = _arrayLength(receiver);
				}
				catch (const std::exception &)
				{
					outcome = std::make_shared<HybridCpuManagedArrayStoreEcallResultV1>(false, 0, L"Managed array-length runtime exception was contained.");
				}
				if (outcome->IsSuccess)
				{
					status = HybridCpuExternalServiceStatusV1::Success;
					value = outcome->ExceptionReference;
					error = outcome->IsManagedThrow ? 1 : 0;
				}
				else
				{
					status = HybridCpuExternalServiceStatusV1::ProviderFailure;
					error = 5;
				}
				goto Publish;
			}
			if (operation == HybridCpuManagedRuntimeEcallContractV1::DivideUInt32CheckedOperation && _divideUInt32 != nullptr && CanonicalUInt32(receiver) && CanonicalUInt32(argument1))
			{
				std::shared_ptr<HybridCpuManagedArrayStoreEcallResultV1> outcome;
				try
				{
					outcome = _divideUInt32(static_cast<unsigned int>(receiver), static_cast<unsigned int>(argument1));
				}
				catch (const std::exception &)
				{
					outcome = std::make_shared<HybridCpuManagedArrayStoreEcallResultV1>(false, 0, L"Managed UInt32 division exception was contained.");
				}
				if (outcome->IsSuccess)
				{
					status = HybridCpuExternalServiceStatusV1::Success;
					value = outcome->ExceptionReference;
					error = outcome->IsManagedThrow ? 1 : 0;
				}
				else
				{
					status = HybridCpuExternalServiceStatusV1::ProviderFailure;
					error = 5;
				}
				goto Publish;
			}
			if (operation == HybridCpuManagedRuntimeEcallContractV1::DivideInt32CheckedOperation && _divideInt32 != nullptr && CanonicalInt32(receiver) && CanonicalInt32(argument1))
			{
				std::shared_ptr<HybridCpuManagedArrayStoreEcallResultV1> outcome;
				try
				{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: outcome = _divideInt32(unchecked((int)(uint)receiver), unchecked((int)(uint)argument1));
					outcome = _divideInt32(static_cast<int>(static_cast<unsigned int>(receiver)), static_cast<int>(static_cast<unsigned int>(argument1)));
				}
				catch (const std::exception &)
				{
					outcome = std::make_shared<HybridCpuManagedArrayStoreEcallResultV1>(false, 0, L"Managed Int32 division exception was contained.");
				}
				if (outcome->IsSuccess)
				{
					status = HybridCpuExternalServiceStatusV1::Success;
					value = outcome->ExceptionReference;
					error = outcome->IsManagedThrow ? 1 : 0;
				}
				else
				{
					status = HybridCpuExternalServiceStatusV1::ProviderFailure;
					error = 5;
				}
				goto Publish;
			}
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: if (operation == HybridCpuManagedRuntimeEcallContractV1.RemainderInt32CheckedOperation && _remainderInt32 != null && receiver == unchecked((ulong)(long)(int)(uint)receiver) && argument1 == unchecked((ulong)(long)(int)(uint)argument1))
			if (operation == HybridCpuManagedRuntimeEcallContractV1::RemainderInt32CheckedOperation && _remainderInt32 != nullptr && receiver == static_cast<unsigned long long>(static_cast<long long>(static_cast<int>(static_cast<unsigned int>(receiver)))) && argument1 == static_cast<unsigned long long>(static_cast<long long>(static_cast<int>(static_cast<unsigned int>(argument1)))))
			{
				std::shared_ptr<HybridCpuManagedArrayStoreEcallResultV1> outcome;
				try
				{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: outcome = _remainderInt32(unchecked((int)(uint)receiver), unchecked((int)(uint)argument1));
					outcome = _remainderInt32(static_cast<int>(static_cast<unsigned int>(receiver)), static_cast<int>(static_cast<unsigned int>(argument1)));
				}
				catch (const std::exception &)
				{
					outcome = std::make_shared<HybridCpuManagedArrayStoreEcallResultV1>(false, 0, L"Managed Int32 remainder exception was contained.");
				}
				if (outcome->IsSuccess)
				{
					status = HybridCpuExternalServiceStatusV1::Success;
					value = outcome->ExceptionReference;
					error = outcome->IsManagedThrow ? 1 : 0;
				}
				else
				{
					status = HybridCpuExternalServiceStatusV1::ProviderFailure;
					error = 5;
				}
				goto Publish;
			}
			if (operation == HybridCpuManagedRuntimeEcallContractV1::DivideInt64CheckedOperation && _divideInt64 != nullptr)
			{
				std::shared_ptr<HybridCpuManagedArrayStoreEcallResultV1> outcome;
				try
				{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: outcome = _divideInt64(unchecked((long)receiver), unchecked((long)argument1));
					outcome = _divideInt64(static_cast<long long>(receiver), static_cast<long long>(argument1));
				}
				catch (const std::exception &)
				{
					outcome = std::make_shared<HybridCpuManagedArrayStoreEcallResultV1>(false, 0, L"Managed Int64 division exception was contained.");
				}
				if (outcome->IsSuccess)
				{
					status = HybridCpuExternalServiceStatusV1::Success;
					value = outcome->ExceptionReference;
					error = outcome->IsManagedThrow ? 1 : 0;
				}
				else
				{
					status = HybridCpuExternalServiceStatusV1::ProviderFailure;
					error = 5;
				}
				goto Publish;
			}
			if (operation == HybridCpuManagedRuntimeEcallContractV1::NullableValueInt64CheckedOperation && _nullableValue != nullptr && argument1 <= 1)
			{
				std::shared_ptr<HybridCpuManagedArrayStoreEcallResultV1> outcome;
				try
				{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: outcome = _nullableValue(unchecked((long)receiver), argument1 == 1);
					outcome = _nullableValue(static_cast<long long>(receiver), argument1 == 1);
				}
				catch (const std::exception &)
				{
					outcome = std::make_shared<HybridCpuManagedArrayStoreEcallResultV1>(false, 0, L"Nullable value runtime exception was contained.");
				}
				if (outcome->IsSuccess)
				{
					status = HybridCpuExternalServiceStatusV1::Success;
					value = outcome->ExceptionReference;
					error = outcome->IsManagedThrow ? 1 : 0;
				}
				else
				{
					status = HybridCpuExternalServiceStatusV1::ProviderFailure;
					error = 5;
				}
				goto Publish;
			}
			if (operation == HybridCpuManagedRuntimeEcallContractV1::MathAbsInt64CheckedOperation && _mathAbsInt64 != nullptr)
			{
				std::shared_ptr<HybridCpuManagedArrayStoreEcallResultV1> outcome;
				try
				{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: outcome = _mathAbsInt64(unchecked((long)receiver));
					outcome = _mathAbsInt64(static_cast<long long>(receiver));
				}
				catch (const std::exception &)
				{
					outcome = std::make_shared<HybridCpuManagedArrayStoreEcallResultV1>(false, 0, L"Managed Int64 Abs exception was contained.");
				}
				if (outcome->IsSuccess)
				{
					status = HybridCpuExternalServiceStatusV1::Success;
					value = outcome->ExceptionReference;
					error = outcome->IsManagedThrow ? 1 : 0;
				}
				else
				{
					status = HybridCpuExternalServiceStatusV1::ProviderFailure;
					error = 5;
				}
				goto Publish;
			}
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: if (operation == HybridCpuManagedRuntimeEcallContractV1.ArrayLoadReferenceOperation && _arrayLoadReference != null && argument1 == unchecked((ulong)(long)(int)(uint)argument1))
			if (operation == HybridCpuManagedRuntimeEcallContractV1::ArrayLoadReferenceOperation && _arrayLoadReference != nullptr && argument1 == static_cast<unsigned long long>(static_cast<long long>(static_cast<int>(static_cast<unsigned int>(argument1)))))
			{
				std::shared_ptr<HybridCpuManagedArrayStoreEcallResultV1> outcome;
				try
				{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: outcome = _arrayLoadReference(receiver, unchecked((int)(uint)argument1));
					outcome = _arrayLoadReference(receiver, static_cast<int>(static_cast<unsigned int>(argument1)));
				}
				catch (const std::exception &)
				{
					outcome = std::make_shared<HybridCpuManagedArrayStoreEcallResultV1>(false, 0, L"Managed reference array-load runtime exception was contained.");
				}
				if (outcome->IsSuccess)
				{
					status = HybridCpuExternalServiceStatusV1::Success;
					value = outcome->ExceptionReference;
					error = outcome->IsManagedThrow ? 1 : 0;
				}
				else
				{
					status = HybridCpuExternalServiceStatusV1::ProviderFailure;
					error = 5;
				}
				goto Publish;
			}
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: if (operation == HybridCpuManagedRuntimeEcallContractV1.StringCharacterOperation && _stringCharacter != null && argument1 == unchecked((ulong)(long)(int)(uint)argument1))
			if (operation == HybridCpuManagedRuntimeEcallContractV1::StringCharacterOperation && _stringCharacter != nullptr && argument1 == static_cast<unsigned long long>(static_cast<long long>(static_cast<int>(static_cast<unsigned int>(argument1)))))
			{
				std::shared_ptr<HybridCpuManagedArrayStoreEcallResultV1> outcome;
				try
				{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: outcome = _stringCharacter(receiver, unchecked((int)(uint)argument1));
					outcome = _stringCharacter(receiver, static_cast<int>(static_cast<unsigned int>(argument1)));
				}
				catch (const std::exception &)
				{
					outcome = std::make_shared<HybridCpuManagedArrayStoreEcallResultV1>(false, 0, L"Managed string-character runtime exception was contained.");
				}
				if (outcome->IsSuccess)
				{
					status = HybridCpuExternalServiceStatusV1::Success;
					value = outcome->ExceptionReference;
					error = outcome->IsManagedThrow ? 1 : 0;
				}
				else
				{
					status = HybridCpuExternalServiceStatusV1::ProviderFailure;
					error = 5;
				}
				goto Publish;
			}
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: if (operation == HybridCpuManagedRuntimeEcallContractV1.ArrayLoadUInt8Operation && _arrayLoadUInt8 != null && argument1 == unchecked((ulong)(long)(int)(uint)argument1))
			if (operation == HybridCpuManagedRuntimeEcallContractV1::ArrayLoadUInt8Operation && _arrayLoadUInt8 != nullptr && argument1 == static_cast<unsigned long long>(static_cast<long long>(static_cast<int>(static_cast<unsigned int>(argument1)))))
			{
				std::shared_ptr<HybridCpuManagedArrayStoreEcallResultV1> outcome;
				try
				{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: outcome = _arrayLoadUInt8(receiver, unchecked((int)(uint)argument1));
					outcome = _arrayLoadUInt8(receiver, static_cast<int>(static_cast<unsigned int>(argument1)));
				}
				catch (const std::exception &)
				{
					outcome = std::make_shared<HybridCpuManagedArrayStoreEcallResultV1>(false, 0, L"Managed array-load-u1 runtime exception was contained.");
				}
				if (outcome->IsSuccess)
				{
					status = HybridCpuExternalServiceStatusV1::Success;
					value = outcome->ExceptionReference;
					error = outcome->IsManagedThrow ? 1 : 0;
				}
				else
				{
					status = HybridCpuExternalServiceStatusV1::ProviderFailure;
					error = 5;
				}
				goto Publish;
			}
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: if (operation == HybridCpuManagedRuntimeEcallContractV1.ArrayLoadInt16Operation && _arrayLoadInt16 != null && argument1 == unchecked((ulong)(long)(int)(uint)argument1))
			if (operation == HybridCpuManagedRuntimeEcallContractV1::ArrayLoadInt16Operation && _arrayLoadInt16 != nullptr && argument1 == static_cast<unsigned long long>(static_cast<long long>(static_cast<int>(static_cast<unsigned int>(argument1)))))
			{
				std::shared_ptr<HybridCpuManagedArrayStoreEcallResultV1> outcome;
				try
				{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: outcome = _arrayLoadInt16(receiver, unchecked((int)(uint)argument1));
					outcome = _arrayLoadInt16(receiver, static_cast<int>(static_cast<unsigned int>(argument1)));
				}
				catch (const std::exception &)
				{
					outcome = std::make_shared<HybridCpuManagedArrayStoreEcallResultV1>(false, 0, L"Managed array-load-i2 runtime exception was contained.");
				}
				if (outcome->IsSuccess)
				{
					status = HybridCpuExternalServiceStatusV1::Success;
					value = outcome->ExceptionReference;
					error = outcome->IsManagedThrow ? 1 : 0;
				}
				else
				{
					status = HybridCpuExternalServiceStatusV1::ProviderFailure;
					error = 5;
				}
				goto Publish;
			}
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: if (operation == HybridCpuManagedRuntimeEcallContractV1.ArrayLoadUInt16Operation && _arrayLoadUInt16 != null && argument1 == unchecked((ulong)(long)(int)(uint)argument1))
			if (operation == HybridCpuManagedRuntimeEcallContractV1::ArrayLoadUInt16Operation && _arrayLoadUInt16 != nullptr && argument1 == static_cast<unsigned long long>(static_cast<long long>(static_cast<int>(static_cast<unsigned int>(argument1)))))
			{
				std::shared_ptr<HybridCpuManagedArrayStoreEcallResultV1> outcome;
				try
				{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: outcome = _arrayLoadUInt16(receiver, unchecked((int)(uint)argument1));
					outcome = _arrayLoadUInt16(receiver, static_cast<int>(static_cast<unsigned int>(argument1)));
				}
				catch (const std::exception &)
				{
					outcome = std::make_shared<HybridCpuManagedArrayStoreEcallResultV1>(false, 0, L"Managed array-load-u2 runtime exception was contained.");
				}
				if (outcome->IsSuccess)
				{
					status = HybridCpuExternalServiceStatusV1::Success;
					value = outcome->ExceptionReference;
					error = outcome->IsManagedThrow ? 1 : 0;
				}
				else
				{
					status = HybridCpuExternalServiceStatusV1::ProviderFailure;
					error = 5;
				}
				goto Publish;
			}
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: if (operation == HybridCpuManagedRuntimeEcallContractV1.ArrayCopyAllOperation && _arrayCopyAll != null && argument2 == unchecked((ulong)(long)(int)(uint)argument2))
			if (operation == HybridCpuManagedRuntimeEcallContractV1::ArrayCopyAllOperation && _arrayCopyAll != nullptr && argument2 == static_cast<unsigned long long>(static_cast<long long>(static_cast<int>(static_cast<unsigned int>(argument2)))))
			{
				std::shared_ptr<HybridCpuManagedArrayStoreEcallResultV1> outcome;
				try
				{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: outcome = _arrayCopyAll(receiver, argument1, unchecked((int)(uint)argument2));
					outcome = _arrayCopyAll(receiver, argument1, static_cast<int>(static_cast<unsigned int>(argument2)));
				}
				catch (const std::exception &)
				{
					outcome = std::make_shared<HybridCpuManagedArrayStoreEcallResultV1>(false, 0, L"Managed Array.Copy exception was contained.");
				}
				if (outcome->IsSuccess)
				{
					status = HybridCpuExternalServiceStatusV1::Success;
					value = outcome->ExceptionReference;
					error = outcome->IsManagedThrow ? 1 : 0;
				}
				else
				{
					status = HybridCpuExternalServiceStatusV1::ProviderFailure;
					error = 5;
				}
				goto Publish;
			}
			if (operation == HybridCpuManagedRuntimeEcallContractV1::IsInstanceOperation && _isInstance != nullptr)
			{
				std::shared_ptr<HybridCpuManagedArrayStoreEcallResultV1> outcome;
				try
				{
					outcome = _isInstance(receiver, argument1);
				}
				catch (const std::exception &)
				{
					outcome = std::make_shared<HybridCpuManagedArrayStoreEcallResultV1>(false, 0, L"Managed isinst runtime exception was contained.");
				}
				if (outcome->IsSuccess)
				{
					status = HybridCpuExternalServiceStatusV1::Success;
					value = outcome->ExceptionReference;
				}
				else
				{
					status = HybridCpuExternalServiceStatusV1::ProviderFailure;
					error = 5;
				}
				goto Publish;
			}
			if (operation == HybridCpuManagedRuntimeEcallContractV1::StringLengthOperation && _stringLength != nullptr)
			{
				std::shared_ptr<HybridCpuManagedArrayStoreEcallResultV1> outcome;
				try
				{
					outcome = _stringLength(receiver);
				}
				catch (const std::exception &)
				{
					outcome = std::make_shared<HybridCpuManagedArrayStoreEcallResultV1>(false, 0, L"Managed string-length runtime exception was contained.");
				}
				if (outcome->IsSuccess)
				{
					status = HybridCpuExternalServiceStatusV1::Success;
					value = outcome->ExceptionReference;
					error = outcome->IsManagedThrow ? 1 : 0;
				}
				else
				{
					status = HybridCpuExternalServiceStatusV1::ProviderFailure;
					error = 5;
				}
				goto Publish;
			}
			if (operation == HybridCpuManagedRuntimeEcallContractV1::StringConcat2Operation && _stringConcat2 != nullptr)
			{
				std::shared_ptr<HybridCpuManagedArrayStoreEcallResultV1> outcome;
				try
				{
					outcome = _stringConcat2(receiver, argument1);
				}
				catch (const std::exception &)
				{
					outcome = std::make_shared<HybridCpuManagedArrayStoreEcallResultV1>(false, 0, L"Managed string-concat2 runtime exception was contained.");
				}
				if (outcome->IsSuccess)
				{
					status = HybridCpuExternalServiceStatusV1::Success;
					value = outcome->ExceptionReference;
					error = outcome->IsManagedThrow ? 1 : 0;
				}
				else
				{
					status = HybridCpuExternalServiceStatusV1::ProviderFailure;
					error = 5;
				}
				goto Publish;
			}
			if (operation == HybridCpuManagedRuntimeEcallContractV1::StringConcat3Operation && _stringConcat3 != nullptr)
			{
				std::shared_ptr<HybridCpuManagedArrayStoreEcallResultV1> outcome;
				try
				{
					outcome = _stringConcat3(receiver, argument1, argument2);
				}
				catch (const std::exception &)
				{
					outcome = std::make_shared<HybridCpuManagedArrayStoreEcallResultV1>(false, 0, L"Managed string-concat3 exception was contained.");
				}
				if (outcome->IsSuccess)
				{
					status = HybridCpuExternalServiceStatusV1::Success;
					value = outcome->ExceptionReference;
					error = outcome->IsManagedThrow ? 1 : 0;
				}
				else
				{
					status = HybridCpuExternalServiceStatusV1::ProviderFailure;
					error = 5;
				}
				goto Publish;
			}
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: if (operation == HybridCpuManagedRuntimeEcallContractV1.ArrayLoadInt32Operation && _arrayLoadInt32 != null && argument1 == unchecked((ulong)(long)(int)(uint)argument1))
			if (operation == HybridCpuManagedRuntimeEcallContractV1::ArrayLoadInt32Operation && _arrayLoadInt32 != nullptr && argument1 == static_cast<unsigned long long>(static_cast<long long>(static_cast<int>(static_cast<unsigned int>(argument1)))))
			{
				std::shared_ptr<HybridCpuManagedArrayStoreEcallResultV1> outcome;
				try
				{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: outcome = _arrayLoadInt32(receiver, unchecked((int)(uint)argument1));
					outcome = _arrayLoadInt32(receiver, static_cast<int>(static_cast<unsigned int>(argument1)));
				}
				catch (const std::exception &)
				{
					outcome = std::make_shared<HybridCpuManagedArrayStoreEcallResultV1>(false, 0, L"Managed i4 array-load runtime exception was contained.");
				}
				if (outcome->IsSuccess)
				{
					status = HybridCpuExternalServiceStatusV1::Success;
					value = outcome->ExceptionReference;
					error = outcome->IsManagedThrow ? 1 : 0;
				}
				else
				{
					status = HybridCpuExternalServiceStatusV1::ProviderFailure;
					error = 5;
				}
				goto Publish;
			}
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: if (operation == HybridCpuManagedRuntimeEcallContractV1.ArrayLoadInt64Operation && _arrayLoadInt64 != null && argument1 == unchecked((ulong)(long)(int)(uint)argument1))
			if (operation == HybridCpuManagedRuntimeEcallContractV1::ArrayLoadInt64Operation && _arrayLoadInt64 != nullptr && argument1 == static_cast<unsigned long long>(static_cast<long long>(static_cast<int>(static_cast<unsigned int>(argument1)))))
			{
				std::shared_ptr<HybridCpuManagedArrayStoreEcallResultV1> outcome;
				try
				{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: outcome = _arrayLoadInt64(receiver, unchecked((int)(uint)argument1));
					outcome = _arrayLoadInt64(receiver, static_cast<int>(static_cast<unsigned int>(argument1)));
				}
				catch (const std::exception &)
				{
					outcome = std::make_shared<HybridCpuManagedArrayStoreEcallResultV1>(false, 0, L"Managed i8 array-load runtime exception was contained.");
				}
				if (outcome->IsSuccess)
				{
					status = HybridCpuExternalServiceStatusV1::Success;
					value = outcome->ExceptionReference;
					error = outcome->IsManagedThrow ? 1 : 0;
				}
				else
				{
					status = HybridCpuExternalServiceStatusV1::ProviderFailure;
					error = 5;
				}
				goto Publish;
			}
			if (operation == HybridCpuManagedRuntimeEcallContractV1::ArrayStoreInt32Operation && _arrayStoreInt32 != nullptr && argument1 <= std::numeric_limits<int>::max())
			{
				std::shared_ptr<HybridCpuManagedArrayStoreEcallResultV1> outcome;
				try
				{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: outcome = _arrayStoreInt32(receiver, (int)argument1, unchecked((int)(uint)argument2));
					outcome = _arrayStoreInt32(receiver, static_cast<int>(argument1), static_cast<int>(static_cast<unsigned int>(argument2)));
				}
				catch (const std::exception &)
				{
					outcome = std::make_shared<HybridCpuManagedArrayStoreEcallResultV1>(false, 0, L"Managed array-store runtime exception was contained.");
				}
				if (outcome->IsSuccess)
				{
					status = HybridCpuExternalServiceStatusV1::Success;
					value = outcome->ExceptionReference;
					error = outcome->ExceptionReference == 0 ? 0 : 1;
				}
				else
				{
					status = HybridCpuExternalServiceStatusV1::ProviderFailure;
					error = 5;
				}
				goto Publish;
			}
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: if (operation == HybridCpuManagedRuntimeEcallContractV1.ArrayStoreInt8Operation && _arrayStoreInt8 != null && argument1 == unchecked((ulong)(long)(int)(uint)argument1))
			if (operation == HybridCpuManagedRuntimeEcallContractV1::ArrayStoreInt8Operation && _arrayStoreInt8 != nullptr && argument1 == static_cast<unsigned long long>(static_cast<long long>(static_cast<int>(static_cast<unsigned int>(argument1)))))
			{
				std::shared_ptr<HybridCpuManagedArrayStoreEcallResultV1> outcome;
				try
				{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: outcome = _arrayStoreInt8(receiver, unchecked((int)(uint)argument1), unchecked((int)(uint)argument2));
					outcome = _arrayStoreInt8(receiver, static_cast<int>(static_cast<unsigned int>(argument1)), static_cast<int>(static_cast<unsigned int>(argument2)));
				}
				catch (const std::exception &)
				{
					outcome = std::make_shared<HybridCpuManagedArrayStoreEcallResultV1>(false, 0, L"Managed i1 array-store runtime exception was contained.");
				}
				if (outcome->IsSuccess)
				{
					status = HybridCpuExternalServiceStatusV1::Success;
					value = outcome->ExceptionReference;
					error = outcome->ExceptionReference == 0 ? 0 : 1;
				}
				else
				{
					status = HybridCpuExternalServiceStatusV1::ProviderFailure;
					error = 5;
				}
				goto Publish;
			}
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: if (operation == HybridCpuManagedRuntimeEcallContractV1.ArrayStoreInt16Operation && _arrayStoreInt16 != null && argument1 == unchecked((ulong)(long)(int)(uint)argument1))
			if (operation == HybridCpuManagedRuntimeEcallContractV1::ArrayStoreInt16Operation && _arrayStoreInt16 != nullptr && argument1 == static_cast<unsigned long long>(static_cast<long long>(static_cast<int>(static_cast<unsigned int>(argument1)))))
			{
				std::shared_ptr<HybridCpuManagedArrayStoreEcallResultV1> outcome;
				try
				{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: outcome = _arrayStoreInt16(receiver, unchecked((int)(uint)argument1), unchecked((int)(uint)argument2));
					outcome = _arrayStoreInt16(receiver, static_cast<int>(static_cast<unsigned int>(argument1)), static_cast<int>(static_cast<unsigned int>(argument2)));
				}
				catch (const std::exception &)
				{
					outcome = std::make_shared<HybridCpuManagedArrayStoreEcallResultV1>(false, 0, L"Managed i2 array-store runtime exception was contained.");
				}
				if (outcome->IsSuccess)
				{
					status = HybridCpuExternalServiceStatusV1::Success;
					value = outcome->ExceptionReference;
					error = outcome->IsManagedThrow ? 1 : 0;
				}
				else
				{
					status = HybridCpuExternalServiceStatusV1::ProviderFailure;
					error = 5;
				}
				goto Publish;
			}
			if (operation == HybridCpuManagedRuntimeEcallContractV1::ArrayStoreReferenceOperation && _arrayStoreReference != nullptr && argument1 <= std::numeric_limits<int>::max())
			{
				std::shared_ptr<HybridCpuManagedArrayStoreEcallResultV1> outcome;
				try
				{
					outcome = _arrayStoreReference(receiver, static_cast<int>(argument1), argument2);
				}
				catch (const std::exception &)
				{
					outcome = std::make_shared<HybridCpuManagedArrayStoreEcallResultV1>(false, 0, L"Managed reference array-store runtime exception was contained.");
				}
				if (outcome->IsSuccess)
				{
					status = HybridCpuExternalServiceStatusV1::Success;
					value = outcome->ExceptionReference;
					error = outcome->ExceptionReference == 0 ? 0 : 1;
				}
				else
				{
					status = HybridCpuExternalServiceStatusV1::ProviderFailure;
					error = 5;
				}
				goto Publish;
			}
			if (operation == HybridCpuManagedRuntimeEcallContractV1::InitializeArrayOperation && _initializeArray != nullptr)
			{
				std::shared_ptr<HybridCpuManagedArrayStoreEcallResultV1> outcome;
				try
				{
					outcome = _initializeArray(receiver, argument1);
				}
				catch (const std::exception &)
				{
					outcome = std::make_shared<HybridCpuManagedArrayStoreEcallResultV1>(false, 0, L"Managed InitializeArray runtime exception was contained.");
				}
				if (outcome->IsSuccess)
				{
					status = HybridCpuExternalServiceStatusV1::Success;
					value = outcome->ExceptionReference;
					error = outcome->ExceptionReference == 0 ? 0 : 1;
				}
				else
				{
					status = HybridCpuExternalServiceStatusV1::ProviderFailure;
					error = 5;
				}
				goto Publish;
			}
			if (operation == HybridCpuManagedRuntimeEcallContractV1::StringFromUtf16ArrayOperation && _stringFromUtf16Array != nullptr)
			{
				std::shared_ptr<HybridCpuManagedArrayStoreEcallResultV1> outcome;
				try
				{
					outcome = _stringFromUtf16Array(receiver, argument1);
				}
				catch (const std::exception &)
				{
					outcome = std::make_shared<HybridCpuManagedArrayStoreEcallResultV1>(false, 0, L"Managed String(char[]) exception was contained.");
				}
				if (outcome->IsSuccess)
				{
					status = HybridCpuExternalServiceStatusV1::Success;
					value = outcome->ExceptionReference;
					error = outcome->IsManagedThrow ? 1 : 0;
				}
				else
				{
					status = HybridCpuExternalServiceStatusV1::ProviderFailure;
					error = 5;
				}
				goto Publish;
			}
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: if (operation == HybridCpuManagedRuntimeEcallContractV1.NewArrayOperation && _newArray != null && argument1 == unchecked((ulong)(long)(int)(uint)argument1))
			if (operation == HybridCpuManagedRuntimeEcallContractV1::NewArrayOperation && _newArray != nullptr && argument1 == static_cast<unsigned long long>(static_cast<long long>(static_cast<int>(static_cast<unsigned int>(argument1)))))
			{
				std::shared_ptr<HybridCpuManagedArrayStoreEcallResultV1> outcome;
				try
				{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: outcome = _newArray(receiver, unchecked((int)(uint)argument1));
					outcome = _newArray(receiver, static_cast<int>(static_cast<unsigned int>(argument1)));
				}
				catch (const std::exception &)
				{
					outcome = std::make_shared<HybridCpuManagedArrayStoreEcallResultV1>(false, 0, L"Managed newarr runtime exception was contained.");
				}
				setLastManagedReason(outcome->Reason);
				if (outcome->IsSuccess)
				{
					status = HybridCpuExternalServiceStatusV1::Success;
					value = outcome->ExceptionReference;
					error = outcome->IsManagedThrow ? 1 : 0;
				}
				else
				{
					status = HybridCpuExternalServiceStatusV1::ProviderFailure;
					error = 5;
				}
				goto Publish;
			}
			if (operation == HybridCpuManagedRuntimeEcallContractV1::AllocateObjectOperation && _allocateObject != nullptr)
			{
				std::shared_ptr<HybridCpuManagedArrayStoreEcallResultV1> outcome;
				try
				{
					outcome = _allocateObject(receiver);
				}
				catch (const std::exception &)
				{
					outcome = std::make_shared<HybridCpuManagedArrayStoreEcallResultV1>(false, 0, L"Managed object allocation exception was contained.");
				}
				setLastManagedReason(outcome->Reason);
				if (outcome->IsSuccess)
				{
					status = HybridCpuExternalServiceStatusV1::Success;
					value = outcome->ExceptionReference;
					error = outcome->IsManagedThrow ? 1 : 0;
				}
				else
				{
					status = HybridCpuExternalServiceStatusV1::ProviderFailure;
					error = 5;
				}
				goto Publish;
			}
			std::shared_ptr<HybridCpuManagedShapeResultV1> result;
			try
			{
				if (operation == HybridCpuManagedRuntimeEcallContractV1::ArgumentExceptionGetMessageOperation && _argumentMessage)
					result = _argumentMessage(receiver);
				else if (operation == HybridCpuManagedRuntimeEcallContractV1::EnsureTypeInitializedOperation &&
					_activeCpuInitializerHandles.contains(receiver))
					result = std::make_shared<HybridCpuManagedShapeResultV1>(HybridCpuManagedShapeStatusV1::Success,
						L"Recursive same-CPU-context request observes image initialization in progress.");
				else if (operation == HybridCpuManagedRuntimeEcallContractV1::EnsureTypeInitializedOperation && _ensureTypeInitialized)
					result = _ensureTypeInitialized(receiver);
				else if (operation == HybridCpuManagedRuntimeEcallContractV1::StaticStoreInt32Operation && _staticStoreInt32 && argument1 <= static_cast<unsigned long long>((std::numeric_limits<int>::max)()))
					result = _staticStoreInt32(receiver, static_cast<int>(argument1), static_cast<int>(static_cast<unsigned int>(argument2)));
				else if (operation == HybridCpuManagedRuntimeEcallContractV1::StaticStoreReferenceOperation && _staticStoreReference && argument1 <= static_cast<unsigned long long>((std::numeric_limits<int>::max)()))
					result = _staticStoreReference(receiver, static_cast<int>(argument1), argument2);
				else if (operation == HybridCpuManagedRuntimeEcallContractV1::StaticLoadInt32Operation && _staticLoadInt32 && argument1 <= static_cast<unsigned long long>((std::numeric_limits<int>::max)()))
					result = _staticLoadInt32(receiver, static_cast<int>(argument1));
				else if (operation == HybridCpuManagedRuntimeEcallContractV1::StaticLoadReferenceOperation && _staticLoadReference && argument1 <= static_cast<unsigned long long>((std::numeric_limits<int>::max)()))
					result = _staticLoadReference(receiver, static_cast<int>(argument1));
				else if (operation == HybridCpuManagedRuntimeEcallContractV1::AllocateNullReferenceExceptionOperation && _allocateNullReferenceException)
					result = _allocateNullReferenceException();
				else
					result = std::make_shared<HybridCpuManagedShapeResultV1>(HybridCpuManagedShapeStatusV1::InvalidType,
						L"Managed runtime ECALL operation has no exact production binding.");
			}
			catch (const std::exception &)
			{
				result = std::make_shared<HybridCpuManagedShapeResultV1>(HybridCpuManagedShapeStatusV1::HeapFailure, L"Managed runtime helper exception was contained.");
			}
			setLastManagedOperation(operation);
			setLastManagedStatus(result->Status);
			setLastManagedReason(result->Reason);
// C# TO C++ CONVERTER TASK: The following 'is' operator pattern is not converted by C# to C++ Converter:
			if (result && result->IsSuccess && ((operation != HybridCpuManagedRuntimeEcallContractV1::ArgumentExceptionGetMessageOperation && operation != HybridCpuManagedRuntimeEcallContractV1::AllocateNullReferenceExceptionOperation) || result->ObjectReference != 0))
			{
				status = HybridCpuExternalServiceStatusV1::Success;
				value = (operation == HybridCpuManagedRuntimeEcallContractV1::StaticLoadInt32Operation ||
					operation == HybridCpuManagedRuntimeEcallContractV1::StaticLoadReferenceOperation)
					? static_cast<unsigned long long>(result->ScalarValue) : result->ObjectReference;
			}
			else
			{
				status = HybridCpuExternalServiceStatusV1::ProviderFailure;
				error = 5;
			}
		}

	Publish:
		if (service == static_cast<unsigned long long>(HybridCpuHostServiceV1::ManagedRuntime))
		{
			setLastManagedOperation(operation);
			setLastManagedExternalStatus(status);
			setLastManagedExternalError(error);
			setLastManagedExternalValue(value);
			if (_managedObservations.size() == 32)
			{
				_managedObservations.pop_front();
			}
			_managedObservations.push_back(std::make_shared<HybridCpuManagedEcallObservationV1>(operation, receiver, argument1, argument2, status, error, value));
		}
		state->WriteRegister(ecall->VtId, HybridCpuExternalServiceEcallContractV1::ResultValueRegister, value);
		state->WriteRegister(ecall->VtId, HybridCpuExternalServiceEcallContractV1::ResultStatusRegister, static_cast<unsigned long long>(status));
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: state.WriteRegister(ecall.VtId, HybridCpuExternalServiceEcallContractV1.ResultErrorRegister, unchecked((ulong)(long)error));
		state->WriteRegister(ecall->VtId, HybridCpuExternalServiceEcallContractV1::ResultErrorRegister, static_cast<unsigned long long>(static_cast<long long>(error)));
		state->WritePc(ecall->VtId, pc + VliwBundleBytes);
		return true;
	}

	unsigned long long HybridCpuIseManagedEcallBridgeV1::Read(std::shared_ptr<ICanonicalCpuState> state, unsigned char vt, int register_Keyword)
	{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: return unchecked((ulong)state.ReadRegister(vt, register));
		return static_cast<unsigned long long>(state->ReadRegister(vt, register_Keyword));
	}

	bool HybridCpuIseManagedEcallBridgeV1::CanonicalUInt32(unsigned long long value)
	{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: return value == (ulong)(uint)value || value == unchecked((ulong)(long)(int)(uint)value);
		return value == static_cast<unsigned long long>(static_cast<unsigned int>(value)) || value == static_cast<unsigned long long>(static_cast<long long>(static_cast<int>(static_cast<unsigned int>(value))));
	}

	bool HybridCpuIseManagedEcallBridgeV1::CanonicalInt32(unsigned long long value)
	{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: return value == unchecked((ulong)(long)(int)(uint)value);
		return value == static_cast<unsigned long long>(static_cast<long long>(static_cast<int>(static_cast<unsigned int>(value))));
	}
}
