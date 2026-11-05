#include "Phase40VmReadProjectionE0Contract.h"

namespace YAKSys_Hybrid_CPU::Core
{
	const std::vector<unsigned short> &Phase40VmReadProjectionE0Contract::ExactFieldIds()
	{
		static const std::vector<unsigned short> fieldIds = {
			static_cast<unsigned short>(VmcsField::GuestCr0),
			static_cast<unsigned short>(VmcsField::GuestCr4)};
		return fieldIds;
	}

	const std::vector<std::shared_ptr<VmReadProjectionE0FindingV2>> &Phase40VmReadProjectionE0Contract::Findings()
	{
		static const std::vector<std::shared_ptr<VmReadProjectionE0FindingV2>> findings = {
			std::make_shared<VmReadProjectionE0FindingV2>(VmReadProjectionE0FindingV2{1, L"CanonicalIngress", L"VMREAD->VmxCompatibilityAdmissionService.AdmitVmReadProjection->ReadCompatibilityProjection"}),
			std::make_shared<VmReadProjectionE0FindingV2>(VmReadProjectionE0FindingV2{2, L"OwnerAndValueSource", L"PrivilegedExecutionStateOwnerPolicy;PrivilegedExecutionStateDescriptor.GuestCr0|GuestCr4"}),
			std::make_shared<VmReadProjectionE0FindingV2>(VmReadProjectionE0FindingV2{3, L"Bindings", L"MaterializedNonZeroDomainTag+ExactAddressSpaceTag+CurrentPolicyEpoch"}),
			std::make_shared<VmReadProjectionE0FindingV2>(VmReadProjectionE0FindingV2{4, L"BitLegalityAndDependencies", L"FieldSpecificAllowedAndRequiredMasks+JointGuestCr0GuestCr4DescriptorLegality"}),
			std::make_shared<VmReadProjectionE0FindingV2>(VmReadProjectionE0FindingV2{5, L"Capability", L"None;NoTypedGrant;NoCapabilityMask"}),
			std::make_shared<VmReadProjectionE0FindingV2>(VmReadProjectionE0FindingV2{6, L"Evidence", L"GuestVisibleReadOnlyProjection+CompatibilityProjectionEvidence+FieldConformanceProof"}),
			std::make_shared<VmReadProjectionE0FindingV2>(VmReadProjectionE0FindingV2{7, L"RestoreMigration", L"RevalidatedAfterRestore;PreRestoreEpochNotReusable"}),
			std::make_shared<VmReadProjectionE0FindingV2>(VmReadProjectionE0FindingV2{8, L"ArchitecturalVisibility", L"ExactSelectedFieldScalar64ReadOnlyProjection"}),
			std::make_shared<VmReadProjectionE0FindingV2>(VmReadProjectionE0FindingV2{9, L"GateDenials", L"DenyOnAnyMissingOwnerSourceDomainAddressSpaceEpochKindBitsEvidenceVisibilityMigrationConformanceGate"}),
			std::make_shared<VmReadProjectionE0FindingV2>(VmReadProjectionE0FindingV2{10, L"NoScalarFallback", L"NoVmcsScalarStore+NoBackingStore+NoCr3FlagsPagingOrCompatibilityControlInference"}),
			std::make_shared<VmReadProjectionE0FindingV2>(VmReadProjectionE0FindingV2{11, L"NoSideAuthority", L"NoWrite+NoBackend+NoMutation+NoCompletion+NoRetire"}),
			std::make_shared<VmReadProjectionE0FindingV2>(VmReadProjectionE0FindingV2{12, L"AdjacentDenials", L"DenyEveryVmReadFieldOutsideGuestCr0GuestCr4ForThisDecision+AlwaysDenyVmWrite"})};
		return findings;
	}

	std::vector<unsigned short> Phase40VmReadProjectionE0Contract::getExactFieldIds()
	{
		return ExactFieldIds();
	}

	std::vector<std::shared_ptr<VmReadProjectionE0FindingV2>> Phase40VmReadProjectionE0Contract::getFindings()
	{
		return Findings();
	}

	bool Phase40VmReadProjectionE0Contract::getRuntimeAuthorityGranted()
	{
		return false;
	}

	bool Phase40VmReadProjectionE0Contract::getProjectionValueAvailable()
	{
		return false;
	}

	bool Phase40VmReadProjectionE0Contract::getCapabilityGranted()
	{
		return false;
	}

	bool Phase40VmReadProjectionE0Contract::getBackendExecutionAuthorized()
	{
		return false;
	}

	bool Phase40VmReadProjectionE0Contract::getMutationAuthorized()
	{
		return false;
	}

	bool Phase40VmReadProjectionE0Contract::getCompletionPublicationAuthorized()
	{
		return false;
	}

	bool Phase40VmReadProjectionE0Contract::getRetirePublicationAuthorized()
	{
		return false;
	}
}
