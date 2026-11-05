#pragma once


#include "../Decode/YAKSys_Hybrid_CPU.Core.VmxCompatDecodeBoundary.h"
#include "../Projection/YAKSys_Hybrid_CPU.Core.VmxCompatProjectionService.h"
#include "../../../../Runtime/Services/YAKSys_Hybrid_CPU.Core.RuntimeBoundaryAdmissionService.h"
#include "../Projection/VmcsRead/VmcsReadOnlyValueProjectionService.h"
#include "VmxCompatibilityAdmissionService.h"
#include "../../../../Runtime/Events/Traps/YAKSys_Hybrid_CPU.Core.TrapPolicyBitmap.h"
#include "../../../../Runtime/Domains/Descriptors/TrapPolicy/YAKSys_Hybrid_CPU.Core.TrapPolicyDescriptor.h"
#include "VmxCompatibilityAdmissionService.Traps.h"
#include <string>
#include <format>
#include <memory>

using namespace YAKSys_Hybrid_CPU::Core::Vmcs::V2;
// C# TO C++ CONVERTER TASK: The C# 'using static' statement  cannot be converted to C++:
//using static YAKSys_Hybrid_CPU.Processor.CPU_Core;


namespace YAKSys_Hybrid_CPU::Core
{
	class VmxCompatibilityAdmissionService final : public std::enable_shared_from_this<VmxCompatibilityAdmissionService>
	{
	private:
		const std::shared_ptr<VmxCompatDecodeBoundary> _decodeBoundary;
		const std::shared_ptr<VmxCompatProjectionService> _projectionService;
		const std::shared_ptr<RuntimeBoundaryAdmissionService> _runtimeAdmission;
		const std::shared_ptr<VmcsReadOnlyValueProjectionService> _readOnlyValueProjection;

	public:
		VmxCompatibilityAdmissionService();

		VmxCompatibilityAdmissionService(std::shared_ptr<VmxCompatDecodeBoundary> decodeBoundary, std::shared_ptr<VmxCompatProjectionService> projectionService, std::shared_ptr<RuntimeBoundaryAdmissionService> runtimeAdmission, std::shared_ptr<VmcsReadOnlyValueProjectionService> readOnlyValueProjection);

		std::shared_ptr<VmxCompatibilityVmReadAdmissionResult> AdmitVmReadProjection(std::shared_ptr<VmxCompatibilityVmReadAdmissionRequest> request);

	private:
		static std::shared_ptr<VmxCompatibilityVmReadAdmissionResult> CreateResult(VmxCompatibilityVmReadAdmissionDecision decision, std::shared_ptr<VmxCompatDecodeResult> decode, std::shared_ptr<VmxCompatProjectionResult> projection, std::shared_ptr<RuntimeBoundaryAdmissionResult> admission, std::shared_ptr<VmcsV2ValidationResult> validation, std::shared_ptr<VmcsReadOnlyValueProjectionResult> valueProjection, long long value, const std::wstring &reason);


	public:
		std::shared_ptr<VmxCompatibilityTrapAdmissionResult> AdmitVmCallTrapProjection(std::shared_ptr<VmxCompatibilityVmCallTrapAdmissionRequest> request);

	private:
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private static bool TryEvaluateNeutralTrapPolicy(TrapPolicyDescriptor? descriptor, System.Nullable<TrapPolicyBitmap> bitmap, TrapRequest request, bool domainValidated, out NeutralTrapResult result, out string reason)
		static bool TryEvaluateNeutralTrapPolicy(std::shared_ptr<TrapPolicyDescriptor> descriptor, std::shared_ptr<TrapPolicyBitmap> bitmap, std::shared_ptr<TrapRequest> request, bool domainValidated, std::shared_ptr<NeutralTrapResult> &result, std::wstring &reason);

		static std::shared_ptr<VmxCompatibilityTrapAdmissionResult> CreateTrapResult(VmxCompatibilityTrapAdmissionDecision decision, std::shared_ptr<VmxCompatDecodeResult> decode, std::shared_ptr<VmxCompatProjectionResult> projection, std::shared_ptr<RuntimeBoundaryAdmissionResult> admission, std::shared_ptr<NeutralTrapResult> neutralResult, const std::wstring &reason);

		static std::shared_ptr<VmxCompatibilityTrapAdmissionResult> CreateTrapResult(VmxCompatibilityTrapAdmissionDecision decision, std::shared_ptr<VmxCompatDecodeResult> decode, std::shared_ptr<VmxCompatProjectionResult> projection, std::shared_ptr<RuntimeBoundaryAdmissionResult> admission, std::shared_ptr<NeutralTrapResult> neutralResult, std::shared_ptr<HypercallBackendAdmissionResult> backendAdmission);

		static std::shared_ptr<VmxCompatibilityTrapAdmissionResult> CreateTrapResult(VmxCompatibilityTrapAdmissionDecision decision, std::shared_ptr<VmxCompatDecodeResult> decode, std::shared_ptr<VmxCompatProjectionResult> projection, std::shared_ptr<RuntimeBoundaryAdmissionResult> admission, std::shared_ptr<NeutralTrapResult> neutralResult, std::shared_ptr<HypercallBackendAdmissionResult> backendAdmission, const std::wstring &reason);

		static std::shared_ptr<HypercallBackendAdmissionResult> CreateHypercallBackendAdmission(std::shared_ptr<VmxCompatibilityVmCallTrapAdmissionRequest> request, std::shared_ptr<RuntimeBoundaryAdmissionResult> admission, std::shared_ptr<NeutralTrapResult> neutralResult);
	};
}
