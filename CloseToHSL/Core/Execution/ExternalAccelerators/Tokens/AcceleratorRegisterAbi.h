#pragma once

#include "../Capabilities/AcceleratorCapabilityRegistry.h"
#include "AcceleratorTokenResults.h"
#include <cmath>
#include <memory>

using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Capabilities;

namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Tokens
{

	enum class AcceleratorRegisterAbiWriteKind : unsigned char
	{
		NoWriteRejected = 0,
		WriteRegister = 1,
		NoWritePreciseFault = 2
	};

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public sealed record AcceleratorRegisterAbiResult
//	{
//		private AcceleratorRegisterAbiResult(AcceleratorRegisterAbiWriteKind writeKind, ulong registerValue, AcceleratorTokenFaultCode faultCode, string message)
//		{
//			WriteKind = writeKind;
//			RegisterValue = registerValue;
//			FaultCode = faultCode;
//			Message = message;
//		}
//
//		public AcceleratorRegisterAbiWriteKind WriteKind
//		{
//			get;
//		}
//
//		public bool WritesRegister
//		{
//			get
//			{
//				return WriteKind == AcceleratorRegisterAbiWriteKind.WriteRegister;
//			}
//		}
//
//		public bool RequiresPreciseFault
//		{
//			get
//			{
//				return WriteKind == AcceleratorRegisterAbiWriteKind.NoWritePreciseFault;
//			}
//		}
//
//		public bool IsRejectedNoWrite
//		{
//			get
//			{
//				return WriteKind == AcceleratorRegisterAbiWriteKind.NoWriteRejected;
//			}
//		}
//
//		public ulong RegisterValue
//		{
//			get;
//		}
//
//		public AcceleratorTokenFaultCode FaultCode
//		{
//			get;
//		}
//
//		public string Message
//		{
//			get;
//		}
//
//		public static AcceleratorRegisterAbiResult Write(ulong registerValue, string message, AcceleratorTokenFaultCode faultCode = AcceleratorTokenFaultCode.None)
//		{
//			return new(AcceleratorRegisterAbiWriteKind.WriteRegister, registerValue, faultCode, message);
//		}
//
//		public static AcceleratorRegisterAbiResult NoWriteRejected(AcceleratorTokenFaultCode faultCode, string message)
//		{
//			return new(AcceleratorRegisterAbiWriteKind.NoWriteRejected, 0, faultCode, message);
//		}
//
//		public static AcceleratorRegisterAbiResult PreciseFaultNoWrite(AcceleratorTokenFaultCode faultCode, string message)
//		{
//			return new(AcceleratorRegisterAbiWriteKind.NoWritePreciseFault, 0, faultCode, message);
//		}
//	}

	class AcceleratorRegisterAbiResult final {
	public:
		const AcceleratorRegisterAbiWriteKind WriteKind;
		const unsigned long long RegisterValue;
		const AcceleratorTokenFaultCode FaultCode;
		const std::wstring Message;
		const bool WritesRegister, RequiresPreciseFault, IsRejectedNoWrite;
		AcceleratorRegisterAbiResult(AcceleratorRegisterAbiWriteKind kind, unsigned long long value, AcceleratorTokenFaultCode fault, std::wstring message)
			: WriteKind(kind), RegisterValue(value), FaultCode(fault), Message(std::move(message)), WritesRegister(kind == AcceleratorRegisterAbiWriteKind::WriteRegister), RequiresPreciseFault(kind == AcceleratorRegisterAbiWriteKind::NoWritePreciseFault), IsRejectedNoWrite(kind == AcceleratorRegisterAbiWriteKind::NoWriteRejected) {}
		static std::shared_ptr<AcceleratorRegisterAbiResult> Write(unsigned long long value, const std::wstring& message, AcceleratorTokenFaultCode fault = AcceleratorTokenFaultCode::None) { return std::make_shared<AcceleratorRegisterAbiResult>(AcceleratorRegisterAbiWriteKind::WriteRegister, value, fault, message); }
		static std::shared_ptr<AcceleratorRegisterAbiResult> NoWriteRejected(AcceleratorTokenFaultCode fault, const std::wstring& message) { return std::make_shared<AcceleratorRegisterAbiResult>(AcceleratorRegisterAbiWriteKind::NoWriteRejected, 0, fault, message); }
		static std::shared_ptr<AcceleratorRegisterAbiResult> PreciseFaultNoWrite(AcceleratorTokenFaultCode fault, const std::wstring& message) { return std::make_shared<AcceleratorRegisterAbiResult>(AcceleratorRegisterAbiWriteKind::NoWritePreciseFault, 0, fault, message); }
	};
	class AcceleratorRegisterAbi final
	{
		// Runtime-owned result packing for the Phase 08 current L7-SDC contour.
		// SystemDeviceCommandMicroOp publishes these values only through execute/capture
		// and retire writeback; token handles remain evidence only.
	public:
		static std::shared_ptr<AcceleratorRegisterAbiResult> FromSubmitAdmission(std::shared_ptr<AcceleratorTokenAdmissionResult> admissionResult);

		static std::shared_ptr<AcceleratorRegisterAbiResult> FromStatusLookup(std::shared_ptr<AcceleratorTokenLookupResult> lookupResult);

		static std::shared_ptr<AcceleratorRegisterAbiResult> FromCapabilityQuery(std::shared_ptr<AcceleratorCapabilityAcceptanceResult> capabilityAcceptance);
	};

}
