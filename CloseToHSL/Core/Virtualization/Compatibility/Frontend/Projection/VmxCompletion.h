#pragma once

namespace YAKSys_Hybrid_CPU::Core::Vmcs::V2
{

	enum class VmxCompletionKind : unsigned char
	{
		Success = 0,
		VmFailValid = 1,
		VmFailInvalid = 2,
		VmAbort = 3,
		VmExit = 4
	};

	enum class VmFailCode : int
	{
		None = 0,
		UnknownVmcsField = 1,
		VmcsFieldAccessDenied = 2,
		GuestGprPersistenceIncomplete = 3,
		HostEvidenceAccessDenied = 4
	};

	enum class VmAbortCode : int
	{
		None = 0,
		CorruptVmcsDescriptor = 1
	};

}
