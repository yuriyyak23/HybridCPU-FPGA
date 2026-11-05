#pragma once

#include "DmaStreamComputeTelemetry.h"
#include "DmaStreamComputeReplayEvidenceComparison.h"
#include "DmaStreamComputeDescriptor.h"
#include "DmaStreamComputeToken.h"
#include "DmaStreamComputeOwnerBinding.h"
#include "DmaStreamComputeOwnerBinding.h"
#include <string>
#include <memory>
#include "../../Pipeline/Certificates/StructuralIdentity/HardwareHash.h"
#include "../../Pipeline/Scheduling/SlotLegality/SlotClassDefinitions.h"

using namespace YAKSys_Hybrid_CPU::Core;

namespace YAKSys_Hybrid_CPU::Core::Execution::DmaStreamCompute
{
	enum class DmaStreamComputeCarrierKind : unsigned char
	{
		None = 0,
		CanonicalDecodedSideband = 1,
		CustomRegistryCarrier = 2,
		MissingDescriptorPayload = 3
	};

	enum class DmaStreamComputeTokenEvidenceKind : unsigned char
	{
		NotCreated = 0,
		LifecycleSnapshot = 1
	};
		struct DmaStreamComputeCarrierEvidence { DmaStreamComputeCarrierKind CarrierKind{}; unsigned short CanonicalOpcode=0; std::shared_ptr<DmaStreamComputeDescriptorReference> DescriptorReference; unsigned long long CarrierIdentityHash=0; bool HasDescriptorPayload() const; bool IsReplayReusable() const; bool Equals(const DmaStreamComputeCarrierEvidence& other) const; static DmaStreamComputeCarrierEvidence CanonicalDecodedSideband(std::shared_ptr<DmaStreamComputeDescriptorReference> reference,unsigned short opcode=0); static DmaStreamComputeCarrierEvidence MissingDescriptorPayload(std::shared_ptr<DmaStreamComputeDescriptorReference> reference,unsigned short opcode=0); static DmaStreamComputeCarrierEvidence CustomRegistryCarrier(std::shared_ptr<DmaStreamComputeDescriptorReference> reference,unsigned short opcode); };
	struct DmaStreamComputeFootprintEvidence { unsigned long long NormalizedReadFootprintHash=0,NormalizedWriteFootprintHash=0,NormalizedFootprintHash=0; DmaStreamComputeAliasPolicy AliasPolicy{}; unsigned short NormalizedReadRangeCount=0,NormalizedWriteRangeCount=0; bool IsComplete() const; bool Equals(const DmaStreamComputeFootprintEvidence& other) const; static DmaStreamComputeFootprintEvidence FromDescriptor(std::shared_ptr<DmaStreamComputeDescriptor> descriptor); };
	struct DmaStreamComputeOwnerDomainEvidence { unsigned short OwnerVirtualThreadId=0; unsigned int OwnerContextId=0,OwnerCoreId=0,OwnerPodId=0; unsigned long long OwnerDomainTag=0; unsigned int DeviceId=0; unsigned short RuntimeOwnerVirtualThreadId=0; unsigned int RuntimeOwnerContextId=0,RuntimeOwnerCoreId=0,RuntimeOwnerPodId=0; unsigned long long RuntimeOwnerDomainTag=0,ActiveDomainCertificate=0; unsigned int RuntimeDeviceId=0; bool GuardAllowed=false; RejectKind RejectKind{}; LegalityAuthoritySource AuthoritySource{}; bool AttemptedReplayCertificateReuse=false; unsigned long long EvidenceHash=0; bool IsComplete() const; bool Equals(const DmaStreamComputeOwnerDomainEvidence& other) const; static DmaStreamComputeOwnerDomainEvidence FromDecision(std::shared_ptr<DmaStreamComputeOwnerBinding> owner,std::shared_ptr<DmaStreamComputeOwnerGuardDecision> decision); };
	struct DmaStreamComputeTokenLifecycleEvidence { DmaStreamComputeTokenEvidenceKind EvidenceKind{}; unsigned long long DescriptorIdentityHash=0,TokenId=0; DmaStreamComputeTokenState State{}; DmaStreamComputeTokenCancelReason CancelReason{}; DmaStreamComputeTokenFaultKind FaultKind{}; int StagedWriteCount=0; bool HasCommitted=false; unsigned long long EvidenceHash=0; bool IsComplete() const; bool Equals(const DmaStreamComputeTokenLifecycleEvidence& other) const; static DmaStreamComputeTokenLifecycleEvidence NotCreated(unsigned long long descriptorIdentityHash); static DmaStreamComputeTokenLifecycleEvidence FromToken(std::shared_ptr<DmaStreamComputeToken> token); };struct DmaStreamComputeLanePlacementEvidence
	{
		SlotClass RequiredSlotClass{}; SlotPinningKind PinningKind{}; int SelectedLane=0; unsigned char FreeLaneMask=0, StableDonorMask=0; bool ReplayActive=false; unsigned long long EvidenceHash=0;
		bool IsComplete() const { return RequiredSlotClass==SlotClass::DmaStreamClass && PinningKind==SlotPinningKind::HardPinned && SelectedLane==6 && EvidenceHash!=0; }
		static DmaStreamComputeLanePlacementEvidence DescriptorSurface();
		static DmaStreamComputeLanePlacementEvidence MaterializedLane6(int selectedLane, unsigned char freeLaneMask, unsigned char stableDonorMask, bool replayActive);
	};
	struct DmaStreamComputeReplayEvidence
	{
		std::shared_ptr<DmaStreamComputeDescriptorReference> DescriptorReference;
		unsigned short AbiVersion=0, HeaderSize=0; unsigned int TotalSize=0; unsigned long long DescriptorIdentityHash=0, CertificateInputHash=0;
		DmaStreamComputeOperationKind Operation{}; DmaStreamComputeElementType ElementType{}; DmaStreamComputeShapeKind Shape{}; DmaStreamComputeRangeEncoding RangeEncoding{}; DmaStreamComputePartialCompletionPolicy PartialCompletionPolicy{}; DmaStreamComputeAliasPolicy AliasPolicy{};
		DmaStreamComputeCarrierEvidence CarrierEvidence{}; DmaStreamComputeFootprintEvidence FootprintEvidence{}; DmaStreamComputeOwnerDomainEvidence OwnerDomainEvidence{}; DmaStreamComputeTokenLifecycleEvidence TokenLifecycleEvidence{}; DmaStreamComputeLanePlacementEvidence LanePlacementEvidence{}; unsigned long long EnvelopeHash=0;
		bool IsComplete() const;
		static std::shared_ptr<DmaStreamComputeReplayEvidence> CreateForDescriptor(std::shared_ptr<DmaStreamComputeDescriptor> descriptor, DmaStreamComputeCarrierEvidence carrierEvidence={}, DmaStreamComputeTokenLifecycleEvidence tokenLifecycleEvidence={}, DmaStreamComputeLanePlacementEvidence lanePlacementEvidence={});
	};
	class DmaStreamComputeReplayEvidenceComparer final
	{
	public:
		static bool CanReuse(std::shared_ptr<DmaStreamComputeReplayEvidence> expected, std::shared_ptr<DmaStreamComputeReplayEvidence> live, std::shared_ptr<DmaStreamComputeReplayEvidenceComparison>& comparison, std::shared_ptr<DmaStreamComputeTelemetryCounters> telemetry=nullptr);
		static std::shared_ptr<DmaStreamComputeReplayEvidenceComparison> Compare(std::shared_ptr<DmaStreamComputeReplayEvidence> expected, std::shared_ptr<DmaStreamComputeReplayEvidence> live, std::shared_ptr<DmaStreamComputeTelemetryCounters> telemetry=nullptr);
	private:
		static bool HasDescriptorPayloadLoss(std::shared_ptr<DmaStreamComputeReplayEvidence> evidence);
		static bool HasUnsupportedCarrier(std::shared_ptr<DmaStreamComputeReplayEvidence> evidence);
		static bool DescriptorShapeMatches(std::shared_ptr<DmaStreamComputeReplayEvidence> expected, std::shared_ptr<DmaStreamComputeReplayEvidence> live, std::wstring& mismatchField);
	};
}




