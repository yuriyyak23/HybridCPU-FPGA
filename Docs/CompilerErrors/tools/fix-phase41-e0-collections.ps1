param([string]$WorkspaceRoot = (Split-Path -Parent (Split-Path -Parent $PSScriptRoot)))
$ErrorActionPreference = 'Stop'
$root = [IO.Path]::GetFullPath($WorkspaceRoot)
$expected = [IO.Path]::GetFullPath((Split-Path -Parent (Split-Path -Parent $PSScriptRoot)))
if ($root -ne $expected) { throw "Unexpected workspace root: $root" }
$log = Join-Path $PSScriptRoot ("fix-phase41-e0-collections-{0}.log" -f (Get-Date -Format 'yyyyMMdd-HHmmss-fff'))
Start-Transcript -LiteralPath $log | Out-Null
try {
    $dir = Join-Path $root 'CloseToHSL\\Core\Runtime\Governance\Virtualization'
    $headerPath = Join-Path $dir 'Phase41VmReadScalarDeliveryE0Contract.h'
    $cppPath = Join-Path $dir 'Phase41VmReadScalarDeliveryE0Contract.cpp'
    $header = @'
#pragma once

#include "../../../Virtualization/Compatibility/FrozenAbi/VmcsFieldAliases/VmcsField.h"
#include "VmReadScalarDeliveryE0FindingV2.h"
#include <memory>
#include <vector>

namespace YAKSys_Hybrid_CPU::Core
{
	/// <summary>
	/// Audited input for the separate GuestCr0/GuestCr4 scalar-delivery D2. This
	/// artifact records constraints only; it issues no receipt or runtime authority.
	/// </summary>
	class Phase41VmReadScalarDeliveryE0Contract final
	{
	private:
		static inline const std::vector<unsigned short> ExactFieldIds = {
			static_cast<unsigned short>(VmcsField::GuestCr0),
			static_cast<unsigned short>(VmcsField::GuestCr4)
		};
		static inline const std::vector<std::shared_ptr<VmReadScalarDeliveryE0FindingV2>> Findings = {
			std::make_shared<VmReadScalarDeliveryE0FindingV2>(1, L"SeparateDecision", L"NeverModifyOrBroadenD2-HV-VMREAD-PROJECTION-V1-GUEST-CR0-CR4-0001"),
			std::make_shared<VmReadScalarDeliveryE0FindingV2>(2, L"SourceOwner", L"PrivilegedExecutionStateOwnerPolicy+PrivilegedExecutionStateDescriptor.GuestCr0|GuestCr4Only"),
			std::make_shared<VmReadScalarDeliveryE0FindingV2>(3, L"SourcePrerequisite", L"AcceptedExactPhase40ReadOnlyProjectionD2+FreshOwnerAdmission+FieldConformanceProof"),
			std::make_shared<VmReadScalarDeliveryE0FindingV2>(4, L"ResultAbi", L"ScalarU64ToDestinationRegister+ArchitecturalRegisterResultOnly"),
			std::make_shared<VmReadScalarDeliveryE0FindingV2>(5, L"AttemptIdentity", L"OpaqueVmReadScalarResultReceiptBoundToLiveAttemptReplayEpochBundleDomainAddressSpaceDescriptorEpochFieldAndRd"),
			std::make_shared<VmReadScalarDeliveryE0FindingV2>(6, L"SpeculativeTransport", L"CanonicalPublishedDestinationDependency+PRF+Rename+EXMEMWBScalarCarrierOnly"),
			std::make_shared<VmReadScalarDeliveryE0FindingV2>(7, L"ArchitecturalCommit", L"WBLocalRetireRecord.RegisterWrite+CanonicalRetireCoordinatorOnly"),
			std::make_shared<VmReadScalarDeliveryE0FindingV2>(8, L"Squash", L"AnySquashBeforePreciseRetireConsumesOrInvalidatesReceiptAndHasZeroArchitecturalEffect"),
			std::make_shared<VmReadScalarDeliveryE0FindingV2>(9, L"Replay", L"ReceiptIsSingleUseAndCannotCrossAttemptReplayEpochSquashRestoreOrIssuerGeneration"),
			std::make_shared<VmReadScalarDeliveryE0FindingV2>(10, L"Migration", L"DrainOnly+NoReceiptCheckpointOrRestore"),
			std::make_shared<VmReadScalarDeliveryE0FindingV2>(11, L"NoSideAuthority", L"NoDirectArchitecturalWrite+NoVMCSWriteback+NoUnderlyingVirtualizationMutation+NoTrapCompletion"),
			std::make_shared<VmReadScalarDeliveryE0FindingV2>(12, L"NoReuse", L"NoVMCALLE5E6+NoProbeD2O1E1ToE7+NoVmxRetireEffect.VmcsReadAuthority"),
			std::make_shared<VmReadScalarDeliveryE0FindingV2>(13, L"AdjacentDenial", L"DenyX0MissingDestinationVMWRITEGuestCr3HostAliasesCompatibilityControlsAndAllOtherFields"),
			std::make_shared<VmReadScalarDeliveryE0FindingV2>(14, L"ReceiptRole", L"ReceiptAttestsOneScalarResultForOneCanonicalCarrierAndIsNotCapabilityAdmissionSourceOwnerOrRetireGrant")
		};

	public:
		static std::vector<unsigned short> getExactFieldIds();
		static std::vector<std::shared_ptr<VmReadScalarDeliveryE0FindingV2>> getFindings();
		static bool getRuntimeAuthorityGranted();
		static bool getSourceValueAvailable();
		static bool getResultReceiptIssued();
		static bool getRegisterWritebackAuthorized();
		static bool getRetireCommitAuthorized();
		static bool getBackendExecutionAuthorized();
		static bool getUnderlyingVirtualizationMutationAuthorized();
		static bool getCompletionPublicationAuthorized();
	};
}
'@
    [IO.File]::WriteAllText($headerPath, $header, [Text.UTF8Encoding]::new($false))
    $cpp = [IO.File]::ReadAllText($cppPath)
    $cpp = $cpp.Replace('ImmutableArray<unsigned short>', 'std::vector<unsigned short>')
    $cpp = $cpp.Replace('ImmutableArray<std::shared_ptr<VmReadScalarDeliveryE0FindingV2>>', 'std::vector<std::shared_ptr<VmReadScalarDeliveryE0FindingV2>>')
    if ($cpp -match 'ImmutableArray') { throw 'ImmutableArray remains in Phase41 implementation.' }
    [IO.File]::WriteAllText($cppPath, $cpp, [Text.UTF8Encoding]::new($false))
    Write-Output 'Updated Phase41 E0 contract header and implementation.'
    Write-Output "Log: $log"
} finally { Stop-Transcript | Out-Null }
