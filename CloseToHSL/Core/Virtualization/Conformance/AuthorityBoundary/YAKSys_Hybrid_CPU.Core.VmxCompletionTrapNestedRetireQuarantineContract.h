#pragma once

#include <string>
#include <vector>
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{
	class VmxCompletionTrapNestedRetireQuarantineContract final : public std::enable_shared_from_this<VmxCompletionTrapNestedRetireQuarantineContract>
	{
	private:
		static inline const std::vector<std::wstring> RemovedCompatibilitySubstratePaths = {L"Core/VMX/Substrate/Completion/Projection/CompletionProjectionService.cs", L"Core/VMX/Substrate/Completion/Records/CompletionRecord.cs", L"Core/VMX/Substrate/Events/Intercepts/TrapDecision.cs", L"Core/VMX/Substrate/Events/Intercepts/TrapPolicyBitmap.cs", L"Core/VMX/Substrate/Events/Intercepts/TrapPolicyService.cs", L"Core/VMX/Substrate/Events/Traps/DomainTrapRecord.cs", L"Core/VMX/Substrate/Events/Traps/SchedulingBudgetTimer.cs", L"Core/VMX/Substrate/Nested/CompletionMapping/NestedCompletionMapper.cs", L"Core/VMX/Substrate/Nested/CompletionMapping/NestedExitMapper.cs", L"Core/VMX/Substrate/Nested/MemoryComposition/NestedExitMapper.MemoryComposition.partial.cs", L"Core/VMX/Substrate/Nested/TrapTranslation/NestedInterceptTranslator.cs", L"Core/VMX/Substrate/Nested/TrapTranslation/NestedInterceptTranslator.Translate.partial.cs", L"Core/VMX/Substrate/Runtime/RetireEvidence/DomainEpochTracker.cs", L"Core/VMX/Substrate/Runtime/RetireEvidence/RetireEvidenceBoundary.cs"};
		static inline const std::vector<std::wstring> NeutralAuthorityPaths = {L"Core/Runtime/Completion/Records/CompletionRecord.cs", L"Core/Runtime/Completion/Routing/CompletionRoutingService.cs", L"Core/Runtime/Domains/Descriptors/CompletionRoute/CompletionRouteDescriptor.cs", L"Core/Runtime/Domains/Descriptors/TrapPolicy/TrapPolicyDescriptor.cs", L"Core/Runtime/Events/Traps/DomainTrapRecord.cs", L"Core/Runtime/Events/Traps/NeutralTrapResult.cs", L"Core/Runtime/Events/Traps/SchedulingBudgetTimer.cs", L"Core/Runtime/Events/Traps/TrapPolicyBitmap.cs", L"Core/Runtime/Events/Traps/TrapRequest.cs", L"Core/Runtime/Nested/Descriptors/NestedDomainDescriptor.cs", L"Core/Runtime/Domains/Descriptors/EvidencePolicy/EvidencePolicyDescriptor.cs", L"Core/Runtime/Domains/Descriptors/Observability/ObservabilityDescriptor.cs", L"Core/Runtime/Evidence/HostOwned/HostOwnedEvidenceBoundary.cs"};
		static inline const std::vector<std::wstring> CompatibilityQuarantinePaths = {L"Core/VMX/Compatibility/Frontend/Projection/Completion/CompletionProjectionService.cs", L"Core/VMX/Compatibility/Frontend/Projection/Completion/CompletionRecordCompatibilityProjection.cs", L"Core/VMX/Compatibility/Frontend/Projection/Events/TrapDecision.cs", L"Core/VMX/Compatibility/Frontend/Projection/Events/TrapPolicyService.cs", L"Core/VMX/Compatibility/Frontend/Projection/Events/VmxTrapProjectionMapper.cs", L"Core/VMX/Compatibility/Frontend/Projection/Nested/NestedCompletionMapper.cs", L"Core/VMX/Compatibility/Frontend/Projection/Nested/NestedExitMapper.cs", L"Core/VMX/Compatibility/Frontend/Projection/Nested/NestedExitMapper.MemoryComposition.partial.cs", L"Core/VMX/Compatibility/Frontend/Projection/Nested/NestedInterceptTranslator.cs", L"Core/VMX/Compatibility/Frontend/Projection/Nested/NestedInterceptTranslator.Translate.partial.cs", L"Core/VMX/Compatibility/Frontend/Retire/DomainEpochTracker.cs", L"Core/VMX/Compatibility/Frontend/Retire/RetireEvidenceBoundary.cs", L"Core/VMX/Compatibility/Generated/VmcsProjection/VmcsV2Blocks.cs", L"NonRTL/Core/System/Vmcs/V2/VmcsV2Descriptor.cs"};
		static inline const std::vector<std::wstring> ForbiddenNeutralAuthorityMarkers = {L"Vmcs", L"VMCS", L"Vmx", L"VMX", L"VmExitReason", L"VmxExitQualification", L"TrapDecision", L"FromCompatibilityExit", L"IsCompatibilityProjectionSource", L"RecordNestedTranslationExit", L"RecordInterceptExit", L"RecordQualifiedExit"};
		static inline const std::vector<std::wstring> RequiredNeutralAuthorityMarkers = {L"CompletionRouteDescriptor", L"TrapPolicyDescriptor", L"DomainTrapRecordAuthority.Runtime", L"NeutralTrapResult", L"TrapRequest", L"NestedDomainDescriptor", L"EvidencePolicyDescriptor", L"ObservabilityDescriptor", L"HostOwnedEvidenceBoundary", L"IsRuntimeAuthoritative"};
		static inline const std::vector<std::wstring> ForbiddenVmcsProjectionHelperMarkers = {L"public void RecordNestedTranslationExit(", L"public void RecordNestedTranslationFault(", L"public void RecordInterceptExit(", L"public void RecordQualifiedExit(", L"TryRouteCompletion(", L"ConfigureRoute(", L"DisableRoute(", L"LaneCompletionRouter Router", L"TrapPolicyBitmap Bitmap", L"SchedulingBudgetTimer Timer"};
		static inline const std::vector<std::wstring> RequiredVmcsProjectionReadOnlyMarkers = {L"IsReadOnlyCompatibilityProjection", L"VMCSv2 scalar projection cache was removed"};

	public:
		static std::vector<std::wstring> getRemovedCompatibilitySubstratePaths();

		static std::vector<std::wstring> getNeutralAuthorityPaths();

		static std::vector<std::wstring> getCompatibilityQuarantinePaths();

		static std::vector<std::wstring> getForbiddenNeutralAuthorityMarkers();

		static std::vector<std::wstring> getRequiredNeutralAuthorityMarkers();

		static std::vector<std::wstring> getForbiddenVmcsProjectionHelperMarkers();

		static std::vector<std::wstring> getRequiredVmcsProjectionReadOnlyMarkers();

		bool getRemovesSubstrateCompatibilityAuthority() const;

		bool getKeepsVmxNamesAsCompatibilityProjectionOnly() const;

		bool getRequiresNeutralRuntimeAuthority() const;

		bool getDoesNotCreateRenamedRuntimeOwner() const;

		bool IsSatisfied();
	};
}
