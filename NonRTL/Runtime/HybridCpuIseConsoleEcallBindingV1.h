#pragma once

#include "HybridCpuIseBootBlobContractsV1.h"
#include "../../CloseToHSL/Core/Architecture/Registers/CSR/CsrAccessPolicy.h"
#include "../../CloseToHSL/Core/Pipeline/PipelineEvent.h"
#include "../../CloseToHSL/Core/State/ICpuState.h"
#include <string>
#include <limits>
#include <optional>
#include <memory>
#include <utility>

using namespace HybridCPU::Platform::Contracts;
using namespace HybridCPU::RuntimeKernel;
using namespace YAKSys_Hybrid_CPU::Core;
using namespace YAKSys_Hybrid_CPU::Core::Pipeline;
using namespace YAKSys_Hybrid_CPU::Core::Registers;

namespace HybridCPU_ISE::NonRTL::Runtime
{
	struct HybridCpuIseConsoleEcallObservationV1 final
	{
		unsigned long long Pc;
		unsigned long long ReturnAddress;
		unsigned long long Operation;
		unsigned long long BufferAddress;
		unsigned long long BufferLength;
		HybridCpuExternalServiceStatusV1 Status;
		std::wstring Reason;

		HybridCpuIseConsoleEcallObservationV1(unsigned long long pc, unsigned long long returnAddress,
			unsigned long long operation, unsigned long long bufferAddress, unsigned long long bufferLength,
			HybridCpuExternalServiceStatusV1 status, std::wstring reason)
			: Pc(pc), ReturnAddress(returnAddress), Operation(operation), BufferAddress(bufferAddress),
			  BufferLength(bufferLength), Status(status), Reason(std::move(reason))
		{
		}
	};

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public sealed record HybridCpuIseConsoleEcallObservationV1(ulong Pc, ulong ReturnAddress, ulong Operation, ulong BufferAddress, ulong BufferLength, HybridCpuExternalServiceStatusV1 Status, string Reason);

	/// <summary>Existing synchronous console contract transported at retirement; kernel owns mapping validation.</summary>
	class HybridCpuIseConsoleEcallBindingV1 final
	{
	public:
		static bool TryHandle(std::shared_ptr<IHybridCpuRuntimeKernelV1> kernel, std::shared_ptr<EcallEvent> ecall, std::shared_ptr<ICanonicalCpuState> state, PrivilegeLevel privilege, std::optional<std::shared_ptr<HybridCpuIseConsoleEcallObservationV1>> &observation);
	};

}
