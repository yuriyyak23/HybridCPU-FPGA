#pragma once

#include "VmReadScalarDeliveryE0FindingV2.h"
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace YAKSys_Hybrid_CPU::Core
{
	enum class CurrentCompletionCoverageContourDisposition : unsigned char
	{
		ExistingCanonicalProducerInsufficient = 0,
		UncalledNeutralHelperNotArchitecturalCompletion = 1,
		CompatibilityMapperNotAuthority = 2,
		ForbiddenExpansionRequired = 3
	};

	struct CurrentCompletionCoverageContourEntry final
	{
		std::wstring Contour;
		CurrentCompletionCoverageContourDisposition Disposition;
		std::wstring Finding;

		CurrentCompletionCoverageContourEntry(std::wstring contour, CurrentCompletionCoverageContourDisposition disposition, std::wstring finding)
			: Contour(std::move(contour)), Disposition(disposition), Finding(std::move(finding))
		{
		}
	};

	class Phase50CurrentCompletionProducerCoveragePrerequisiteContract final
	{
	private:
		static inline const std::vector<std::shared_ptr<CurrentCompletionCoverageContourEntry>> Contours = {
			std::make_shared<CurrentCompletionCoverageContourEntry>(L"CanonicalPipelineTrapEntryProducer", CurrentCompletionCoverageContourDisposition::ExistingCanonicalProducerInsufficient, L"Only neutral mcause reason is committed; qualification is absent, address semantic is virtual, and auxiliary is absent."),
			std::make_shared<CurrentCompletionCoverageContourEntry>(L"IOMMU.TranslateGuestAccess/NestedPageWalker", CurrentCompletionCoverageContourDisposition::UncalledNeutralHelperNotArchitecturalCompletion, L"Typed GPA and translation qualification exist, but no CPU pipeline/retire caller commits them as architectural completion."),
			std::make_shared<CurrentCompletionCoverageContourEntry>(L"NestedExitMapper", CurrentCompletionCoverageContourDisposition::CompatibilityMapperNotAuthority, L"The only qualification consumer maps to VMX vocabulary inside the compatibility frontend."),
			std::make_shared<CurrentCompletionCoverageContourEntry>(L"ProspectiveCpuTranslationFaultCompletionProducer", CurrentCompletionCoverageContourDisposition::ForbiddenExpansionRequired, L"Creating and integrating this producer would expand memory/IOMMU/nested completion scope and requires separate authorization.")
		};
		static inline const std::vector<std::shared_ptr<VmReadScalarDeliveryE0FindingV2>> Findings = {
			std::make_shared<VmReadScalarDeliveryE0FindingV2>(1, L"ProductionRegistration", L"ExactlyOneCanonicalPipelineTrapEntryProducerIsRegistered"),
			std::make_shared<VmReadScalarDeliveryE0FindingV2>(2, L"CpuRetireReachability", L"NoCpuPipelineOrRetireCallerCarriesNestedTranslationResultToArchitecturalCompletionCommit"),
			std::make_shared<VmReadScalarDeliveryE0FindingV2>(3, L"TranslationHelper", L"TypedGpaAndQualificationExistOnlyAsUncommittedMemoryTranslationResults"),
			std::make_shared<VmReadScalarDeliveryE0FindingV2>(4, L"CompatibilityConsumer", L"NestedExitMapperIsTheOnlyQualificationConsumerAndCannotBecomeAuthority"),
			std::make_shared<VmReadScalarDeliveryE0FindingV2>(5, L"ReasonMapping", L"RiscVMcauseToVmExitReasonOwnerApprovedMappingDoesNotExist"),
			std::make_shared<VmReadScalarDeliveryE0FindingV2>(6, L"RequiredOwner", L"ASeparateNeutralCpuTranslationFaultCompletionProducerAndPolicyDecisionWouldBeRequired"),
			std::make_shared<VmReadScalarDeliveryE0FindingV2>(7, L"AuthorizationBoundary", L"MemoryIommuNestedAndVmReadProductionExpansionAreNotAuthorized"),
			std::make_shared<VmReadScalarDeliveryE0FindingV2>(8, L"Disposition", L"BlockedPrerequisiteWithoutSurrogateProducerOrCompatibilityAuthority")
		};

	public:
		static inline const std::wstring CandidateDecisionId = L"D2-HV-VMREAD-SCALAR-DELIVERY-V1-CURRENT-COMPLETION-0004";
		static inline const std::wstring BlockerCode = L"NoCanonicalCpuTranslationFaultCompletionProducerAndExpansionNotAuthorized";

		static std::vector<std::shared_ptr<CurrentCompletionCoverageContourEntry>> getContours();
		static std::vector<std::shared_ptr<VmReadScalarDeliveryE0FindingV2>> getFindings();
		static bool getExistingProducerCoverageComplete();
		static bool getCpuTranslationFaultProducerExists();
		static bool getCpuRetireReachabilityExists();
		static bool getOwnerApprovedReasonMappingExists();
		static bool getExpansionAuthorized();
		static bool getD2MayOpen();
		static bool getRuntimeAuthorityGranted();
	};
}
