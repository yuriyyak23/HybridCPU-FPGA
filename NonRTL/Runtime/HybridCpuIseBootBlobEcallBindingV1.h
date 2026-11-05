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

	struct HybridCpuIseBootBlobEcallObservationV1 final
	{
		unsigned long long Pc;
		unsigned long long ReturnAddress;
		unsigned long long BlobId;
		unsigned long long ArgumentCount;
		HybridCpuExternalServiceStatusV1 Status;
		unsigned long long Reference;
		std::wstring Reason;

		HybridCpuIseBootBlobEcallObservationV1(unsigned long long pc, unsigned long long returnAddress,
			unsigned long long blobId, unsigned long long argumentCount,
			HybridCpuExternalServiceStatusV1 status, unsigned long long reference, std::wstring reason)
			: Pc(pc), ReturnAddress(returnAddress), BlobId(blobId), ArgumentCount(argumentCount),
			  Status(status), Reference(reference), Reason(std::move(reason))
		{
		}
	};
// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public sealed record HybridCpuIseBootBlobEcallObservationV1(ulong Pc, ulong ReturnAddress, ulong BlobId, ulong ArgumentCount, HybridCpuExternalServiceStatusV1 Status, ulong Reference, string Reason);

	/// <summary>Retirement bridge wiring for the existing non-parking File/BLOB kernel contract.</summary>
	class HybridCpuIseBootBlobEcallBindingV1 final
	{
	public:
		static bool TryHandle(std::shared_ptr<IHybridCpuRuntimeKernelV1> kernel, std::shared_ptr<EcallEvent> ecall, std::shared_ptr<ICanonicalCpuState> state, PrivilegeLevel privilege, std::optional<std::shared_ptr<HybridCpuIseBootBlobEcallObservationV1>> &observation);
	};

}
