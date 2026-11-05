#include "HybridCpuIseClockReadEcallBindingV1.h"

#include <limits>
#include <utility>
#include <vector>

using namespace HybridCPU::Platform::Contracts;
using namespace HybridCPU::RuntimeKernel;
using namespace YAKSys_Hybrid_CPU::Core;
using namespace YAKSys_Hybrid_CPU::Core::Pipeline;
using namespace YAKSys_Hybrid_CPU::Core::Registers;

namespace HybridCPU_ISE::NonRTL::Runtime
{
	bool HybridCpuIseClockReadEcallBindingV1::TryHandle(
		std::shared_ptr<IHybridCpuRuntimeKernelV1> kernel,
		std::shared_ptr<EcallEvent> ecall,
		std::shared_ptr<ICanonicalCpuState> state,
		PrivilegeLevel privilege,
		std::optional<std::shared_ptr<HybridCpuIseClockReadEcallObservationV1>> &observation)
	{
		observation = std::nullopt;
		if (!kernel || !ecall || !state)
		{
			return false;
		}

		const auto read = [&state, &ecall](int registerIndex) -> unsigned long long
		{
			return static_cast<unsigned long long>(state->ReadRegister(ecall->VtId, registerIndex));
		};
		const unsigned long long pc = state->ReadPc(ecall->VtId);
		const unsigned long long operation = read(11);
		const std::shared_ptr<HybridCpuExecutionContextDescriptorV1> context = kernel->CurrentContext();
		if (ecall->EcallCode != static_cast<long long>(HybridCpuExternalServiceEcallContractV1::EcallNumber) ||
			read(10) != static_cast<unsigned long long>(HybridCpuHostServiceV1::Clock) ||
			(operation != HybridCpuVirtualClockServiceContractV1::ReadTicksOperation &&
				operation != HybridCpuVirtualClockServiceContractV1::ReadDoomTicsOperation) ||
			privilege != PrivilegeLevel::User || !context || context->VirtualThreadCarrier != ecall->VtId ||
			pc > std::numeric_limits<unsigned long long>::max() - 256)
		{
			return false;
		}

		HybridCpuExternalServiceStatusV1 status = HybridCpuExternalServiceStatusV1::InvalidRequest;
		unsigned long long value = 0;
		int error = 0;
		std::wstring reason = L"Clock read requires no arguments and a valid buffer-access envelope.";
		if (read(15) == 0 && read(14) <= std::numeric_limits<unsigned char>::max())
		{
			const auto draft = std::make_shared<HybridCpuExternalServiceRequestV1>(
				context->ContextId,
				HybridCpuHostServiceV1::Clock,
				operation,
				HybridCpuPrivilegeModeV1::User,
				read(12),
				read(13),
				static_cast<HybridCpuHostBufferAccessV1>(read(14)),
				std::vector<unsigned long long>{},
				L"");
			const auto request = std::make_shared<HybridCpuExternalServiceRequestV1>(*draft);
			request->TransitionDigest = HybridCpuManagedInteropContractV1::ComputeTransitionDigest(draft);
			const std::shared_ptr<HybridCpuExternalServiceResultV1> result = kernel->ExternalServiceTransition(*request);
			if (result)
			{
				status = result->Status;
				value = result->ReturnValue;
				error = result->ErrorCode;
				reason = result->Reason;
			}
			else
			{
				status = HybridCpuExternalServiceStatusV1::ProviderFailure;
				reason = L"Clock external service transition returned no result.";
			}
		}

		observation = std::make_shared<HybridCpuIseClockReadEcallObservationV1>(
			pc, read(1), operation, status, value, reason);
		state->WriteRegister(ecall->VtId, HybridCpuExternalServiceEcallContractV1::ResultValueRegister, value);
		state->WriteRegister(ecall->VtId, HybridCpuExternalServiceEcallContractV1::ResultStatusRegister,
			static_cast<unsigned long long>(status));
		state->WriteRegister(ecall->VtId, HybridCpuExternalServiceEcallContractV1::ResultErrorRegister,
			static_cast<unsigned long long>(static_cast<long long>(error)));
		state->WritePc(ecall->VtId, pc + 256);
		return true;
	}
}
