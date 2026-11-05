#pragma once

#include "../../../Generated/VmcsProjection/YAKSys_Hybrid_CPU.Core.VmcsFieldAliasProjection.h"
#include "../Completion/YAKSys_Hybrid_CPU.Core.CompletionProjectionService.h"
#include "../../../../SecureCompute/Compatibility/Projection/PrivilegedExecutionStateProjectionService.h"
#include "../../../FrozenAbi/VmcsFieldAliases/VmcsField.h"
#include "../../../Generated/VmcsProjection/VmcsFieldProjectionSchema.h"
#include "../../../../../Runtime/Services/RuntimeBoundaryAdmissionService.h"
#include "../../../../../System/Vmcs/V2/VmcsV2FieldModel.h"
#include <string>
#include <utility>
#include <stdexcept>
#include <format>
#include <memory>

using namespace YAKSys_Hybrid_CPU::Core::Vmcs::V2;

namespace YAKSys_Hybrid_CPU::Core
{

	enum class VmcsReadOnlyValueProjectionDecision : unsigned char
	{
		NotEvaluated = 0,
		ReadOnlyValueProjected = 1,
		RuntimeAdmissionDenied = 2,
		UnknownFieldDenied = 3,
		SchemaReadDenied = 4,
		AliasAccessDenied = 5,
		NeutralOwnerValueSourceMissing = 6,
		CompletionSourceMissing = 7,
		CompletionSourceDenied = 8,
		MemorySourceMissing = 9,
		MemorySourceDenied = 10,
		CompatibilityControlValueProjectionDenied = 11,
		HostAddressSpaceOwnerMissing = 12,
		ExecutionSourceMissing = 13,
		ExecutionSourceDenied = 14,
		PrivilegedExecutionStateProjectionDenied = 15,
		HostExecutionStateOwnerMissing = 16
	};

class ExecutionDomainDescriptor; class MemoryDomainDescriptor; class ExecutionDomainReadOnlyStateView; struct MemoryDomainReadOnlyTranslationView;
struct VmcsReadOnlyValueProjectionRequest {
    const unsigned short FieldId; const std::shared_ptr<RuntimeBoundaryAdmissionResult> RuntimeAdmission; const std::shared_ptr<EvidencePolicyDescriptor> EvidencePolicy; const bool DescriptorValidated;
    const std::shared_ptr<ExecutionDomainDescriptor> Execution; const std::shared_ptr<MemoryDomainDescriptor> Memory; const std::shared_ptr<CompletionRecord> Completion;
    const std::shared_ptr<PrivilegedExecutionStateDescriptor> PrivilegedExecutionState; const unsigned long long RuntimeDomainTag, RuntimeAddressSpaceTag; const PrivilegedExecutionStateEpoch CurrentPrivilegedExecutionStateEpoch; const bool PrivilegedExecutionStateConformanceProven;
    VmcsReadOnlyValueProjectionRequest(unsigned short field, std::shared_ptr<RuntimeBoundaryAdmissionResult> admission, std::shared_ptr<EvidencePolicyDescriptor> evidence, bool descriptorValidated, std::shared_ptr<ExecutionDomainDescriptor> execution, std::shared_ptr<MemoryDomainDescriptor> memory, std::shared_ptr<CompletionRecord> completion, std::shared_ptr<PrivilegedExecutionStateDescriptor> privileged = nullptr, unsigned long long domain = 0, unsigned long long addressSpace = 0, PrivilegedExecutionStateEpoch epoch = {}, bool conformance = false)
      : FieldId(field), RuntimeAdmission(std::move(admission)), EvidencePolicy(std::move(evidence)), DescriptorValidated(descriptorValidated), Execution(std::move(execution)), Memory(std::move(memory)), Completion(std::move(completion)), PrivilegedExecutionState(std::move(privileged)), RuntimeDomainTag(domain), RuntimeAddressSpaceTag(addressSpace), CurrentPrivilegedExecutionStateEpoch(epoch), PrivilegedExecutionStateConformanceProven(conformance) {}
};
struct VmcsReadOnlyValueProjectionResult {
    const VmcsReadOnlyValueProjectionDecision Decision; const VmcsField Field; const std::shared_ptr<VmcsFieldProjectionSchemaEntry> SchemaEntry; const std::shared_ptr<VmcsFieldAliasResult> AliasAccess; const std::shared_ptr<VmcsV2ValidationResult> Validation; const long long Value; const std::wstring Reason;
    std::shared_ptr<PrivilegedExecutionStateProjectionResult> PrivilegedProjection;
    VmcsReadOnlyValueProjectionResult(VmcsReadOnlyValueProjectionDecision decision, VmcsField field, std::shared_ptr<VmcsFieldProjectionSchemaEntry> schema, std::shared_ptr<VmcsFieldAliasResult> alias, std::shared_ptr<VmcsV2ValidationResult> validation, long long value, std::wstring reason) : Decision(decision), Field(field), SchemaEntry(std::move(schema)), AliasAccess(std::move(alias)), Validation(std::move(validation)), Value(value), Reason(std::move(reason)) {}
    bool isProjected() const { return Decision == VmcsReadOnlyValueProjectionDecision::ReadOnlyValueProjected && Validation && Validation->Succeeded; }
};
	class VmcsReadOnlyValueProjectionService final : public std::enable_shared_from_this<VmcsReadOnlyValueProjectionService>
	{
	private:
		const std::shared_ptr<VmcsFieldAliasProjection> _aliasProjection;
		const std::shared_ptr<CompletionProjectionService> _completionProjection;
		const std::shared_ptr<PrivilegedExecutionStateProjectionService> _privilegedProjection;

	public:
		VmcsReadOnlyValueProjectionService();

		VmcsReadOnlyValueProjectionService(std::shared_ptr<VmcsFieldAliasProjection> aliasProjection, std::shared_ptr<CompletionProjectionService> completionProjection);

// C# TO C++ CONVERTER TASK: Throw expressions are not converted by C# to C++ Converter:
// ORIGINAL LINE: public VmcsReadOnlyValueProjectionService(VmcsFieldAliasProjection aliasProjection, CompletionProjectionService completionProjection, PrivilegedExecutionStateProjectionService privilegedProjection)
		VmcsReadOnlyValueProjectionService(std::shared_ptr<VmcsFieldAliasProjection> aliasProjection, std::shared_ptr<CompletionProjectionService> completionProjection, std::shared_ptr<PrivilegedExecutionStateProjectionService> privilegedProjection);

		std::shared_ptr<VmcsReadOnlyValueProjectionResult> Project(std::shared_ptr<VmcsReadOnlyValueProjectionRequest> request);

	private:
		std::shared_ptr<VmcsReadOnlyValueProjectionResult> ProjectCompletionOwnedValue(std::shared_ptr<VmcsReadOnlyValueProjectionRequest> request, VmcsField field, std::shared_ptr<VmcsFieldProjectionSchemaEntry> entry, std::shared_ptr<VmcsFieldAliasResult> alias);

		static std::shared_ptr<VmcsReadOnlyValueProjectionResult> ProjectMemoryOwnedValue(std::shared_ptr<VmcsReadOnlyValueProjectionRequest> request, VmcsField field, std::shared_ptr<VmcsFieldProjectionSchemaEntry> entry, std::shared_ptr<VmcsFieldAliasResult> alias);

		std::shared_ptr<VmcsReadOnlyValueProjectionResult> ProjectExecutionOwnedValue(std::shared_ptr<VmcsReadOnlyValueProjectionRequest> request, VmcsField field, std::shared_ptr<VmcsFieldProjectionSchemaEntry> entry, std::shared_ptr<VmcsFieldAliasResult> alias);

		static bool TryProjectExecutionField(VmcsField field, std::shared_ptr<ExecutionDomainReadOnlyStateView> view, long long &value, std::wstring &missingReason);

		static bool TryProjectMemoryField(VmcsField field, std::shared_ptr<MemoryDomainReadOnlyTranslationView> view, long long &value);

		static std::shared_ptr<VmcsReadOnlyValueProjectionResult> Denied(VmcsReadOnlyValueProjectionDecision decision, VmcsField field, std::shared_ptr<VmcsFieldProjectionSchemaEntry> entry, std::shared_ptr<VmcsFieldAliasResult> alias, unsigned short fieldId, const std::wstring &reason);
	};

}
