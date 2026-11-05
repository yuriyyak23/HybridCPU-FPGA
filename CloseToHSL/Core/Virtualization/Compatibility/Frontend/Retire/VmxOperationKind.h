#pragma once
namespace YAKSys_Hybrid_CPU::Core {
enum class VmxOperationKind : unsigned char
	{
		None = 0,
		VmxOn = 1,
		VmxOff = 2,
		VmLaunch = 3,
		VmResume = 4,
		VmRead = 5,
		VmWrite = 6,
		VmClear = 7,
		VmPtrLd = 8,
		VmPtrSt = 9,
		VmCall = 10,
		Invept = 11,
		Invvpid = 12,
		VmFunc = 13,
		VmSaveX = 14,
		VmRestX = 15,
		InterceptExit = 16
	};
}
