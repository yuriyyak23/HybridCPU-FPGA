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
	struct HybridCpuIseClockReadEcallObservationV1 final
	{
		unsigned long long Pc;
		unsigned long long ReturnAddress;
		unsigned long long Operation;
		HybridCpuExternalServiceStatusV1 Status;
		unsigned long long Value;
		std::wstring Reason;

		HybridCpuIseClockReadEcallObservationV1(unsigned long long pc, unsigned long long returnAddress,
			unsigned long long operation, HybridCpuExternalServiceStatusV1 status,
			unsigned long long value, std::wstring reason)
			: Pc(pc), ReturnAddress(returnAddress), Operation(operation), Status(status),
			  Value(value), Reason(std::move(reason))
		{
		}
	};

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public sealed record HybridCpuIseClockReadEcallObservationV1(ulong Pc, ulong ReturnAddress, ulong Operation, HybridCpuExternalServiceStatusV1 Status, ulong Value, string Reason);

	/// <summary>Register transport for existing synchronous kernel clock reads. No clock or scheduler authority.</summary>
	class HybridCpuIseClockReadEcallBindingV1 final
	{
	public:
		static bool TryHandle(std::shared_ptr<IHybridCpuRuntimeKernelV1> kernel, std::shared_ptr<EcallEvent> ecall, std::shared_ptr<ICanonicalCpuState> state, PrivilegeLevel privilege, std::optional<std::shared_ptr<HybridCpuIseClockReadEcallObservationV1>> &observation);
	};

}
