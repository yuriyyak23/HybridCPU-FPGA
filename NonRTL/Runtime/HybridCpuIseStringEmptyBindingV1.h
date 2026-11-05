#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include "HybridCpuIseBootBlobContractsV1.h"
#include "../../CloseToHSL/Core/Runtime/Managed/HybridCpuIseManagedEcallContractsV1.h"

namespace HybridCPU::ManagedRuntime
{
	class HybridCpuManagedStringRuntimeV1 final
	{
		std::shared_ptr<HybridCpuManagedTypeSystemV1> _types;
		std::shared_ptr<HybridCpuManagedHeapAllocatorV1> _heap;
		std::unordered_map<std::wstring, unsigned long long> _literals;

	public:
		HybridCpuManagedStringRuntimeV1(std::shared_ptr<HybridCpuManagedTypeSystemV1> types,
			std::shared_ptr<HybridCpuManagedHeapAllocatorV1> heap);
		std::shared_ptr<HybridCpuManagedShapeResultV1> MaterializeLiteral(unsigned long long typeHandle,
			const std::wstring& value);
	};
}

namespace HybridCPU_ISE::NonRTL::Runtime
{

	/// <summary>Pre-CPU materialization of the compiler's exact runtime-preinitialized String.Empty layout.</summary>
	class HybridCpuIseStringEmptyBindingV1 final
	{
	public:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: public static bool TryInitialize(HybridCpuManagedTypeSystemV1 types, HybridCpuManagedStringRuntimeV1 strings, IReadOnlyList<HybridCpuModuleInitializerRegistrationV1> initializers, out string reason)
		static bool TryInitialize(
			std::shared_ptr<HybridCPU::ManagedRuntime::HybridCpuManagedTypeSystemV1> types,
			std::shared_ptr<HybridCPU::ManagedRuntime::HybridCpuManagedStringRuntimeV1> strings,
			const std::vector<std::shared_ptr<HybridCPU::Platform::Contracts::HybridCpuModuleInitializerRegistrationV1>>& initializers,
			std::wstring& reason);
	};

}
