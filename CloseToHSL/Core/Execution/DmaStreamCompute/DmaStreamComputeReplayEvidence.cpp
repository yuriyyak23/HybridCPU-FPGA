#include "DmaStreamComputeReplayEvidence.h"
#include "../../Pipeline/Certificates/ReplayPhaseSubstrate.h"

using namespace YAKSys_Hybrid_CPU::Core;

namespace YAKSys_Hybrid_CPU::Core::Execution::DmaStreamCompute
{
	// Implementations for the value evidence records live in this translation unit.
	bool DmaStreamComputeCarrierEvidence::HasDescriptorPayload() const { return DescriptorReference && DescriptorReference->DescriptorSize != 0 && DescriptorReference->DescriptorIdentityHash != 0; }
	bool DmaStreamComputeCarrierEvidence::IsReplayReusable() const { return CarrierKind == DmaStreamComputeCarrierKind::CanonicalDecodedSideband && HasDescriptorPayload() && CarrierIdentityHash != 0; }
	bool DmaStreamComputeCarrierEvidence::Equals(const DmaStreamComputeCarrierEvidence& o) const { return CarrierKind==o.CarrierKind && CanonicalOpcode==o.CanonicalOpcode && CarrierIdentityHash==o.CarrierIdentityHash && ((!DescriptorReference&&!o.DescriptorReference)||(DescriptorReference&&o.DescriptorReference&&*DescriptorReference==*o.DescriptorReference)); }
	static unsigned long long DmaHashCarrier(DmaStreamComputeCarrierKind kind, unsigned short opcode, const DmaStreamComputeDescriptorReference& r) { HardwareHash h; h.Initialize(); h.Compress(static_cast<unsigned char>(kind)); h.Compress(opcode); h.Compress(r.DescriptorAddress); h.Compress(r.DescriptorSize); h.Compress(r.DescriptorIdentityHash); auto v=h.Finalize(); return v?v:0xD5C0CA221ULL; }
	DmaStreamComputeCarrierEvidence DmaStreamComputeCarrierEvidence::CanonicalDecodedSideband(std::shared_ptr<DmaStreamComputeDescriptorReference> r,unsigned short op) { if(!r) throw std::invalid_argument("descriptorReference"); return {DmaStreamComputeCarrierKind::CanonicalDecodedSideband,op,r,DmaHashCarrier(DmaStreamComputeCarrierKind::CanonicalDecodedSideband,op,*r)}; }
	DmaStreamComputeCarrierEvidence DmaStreamComputeCarrierEvidence::MissingDescriptorPayload(std::shared_ptr<DmaStreamComputeDescriptorReference> r,unsigned short op) { if(!r) throw std::invalid_argument("descriptorReference"); return {DmaStreamComputeCarrierKind::MissingDescriptorPayload,op,r,DmaHashCarrier(DmaStreamComputeCarrierKind::MissingDescriptorPayload,op,*r)}; }
	DmaStreamComputeCarrierEvidence DmaStreamComputeCarrierEvidence::CustomRegistryCarrier(std::shared_ptr<DmaStreamComputeDescriptorReference> r,unsigned short op) { if(!r) throw std::invalid_argument("descriptorReference"); return {DmaStreamComputeCarrierKind::CustomRegistryCarrier,op,r,DmaHashCarrier(DmaStreamComputeCarrierKind::CustomRegistryCarrier,op,*r)}; }
	bool DmaStreamComputeFootprintEvidence::IsComplete() const { return NormalizedReadFootprintHash && NormalizedWriteFootprintHash && NormalizedFootprintHash && NormalizedReadRangeCount && NormalizedWriteRangeCount; }
	bool DmaStreamComputeFootprintEvidence::Equals(const DmaStreamComputeFootprintEvidence& o) const { return NormalizedReadFootprintHash==o.NormalizedReadFootprintHash && NormalizedWriteFootprintHash==o.NormalizedWriteFootprintHash && NormalizedFootprintHash==o.NormalizedFootprintHash && AliasPolicy==o.AliasPolicy && NormalizedReadRangeCount==o.NormalizedReadRangeCount && NormalizedWriteRangeCount==o.NormalizedWriteRangeCount; }
	DmaStreamComputeFootprintEvidence DmaStreamComputeFootprintEvidence::FromDescriptor(std::shared_ptr<DmaStreamComputeDescriptor> d) { if(!d) throw std::invalid_argument("descriptor"); auto makeHash=[](const auto& rs){ HardwareHash h; h.Initialize(); h.Compress(rs.size()); for(const auto& r:rs){h.Compress(r.Address);h.Compress(r.Length);} auto v=h.Finalize(); return v?v:0xD5C0F007ULL; }; if(d->NormalizedReadMemoryRanges.size()>65535 || d->NormalizedWriteMemoryRanges.size()>65535) throw std::overflow_error("DMA normalized range count exceeds evidence field"); return {makeHash(d->NormalizedReadMemoryRanges),makeHash(d->NormalizedWriteMemoryRanges),d->NormalizedFootprintHash,d->AliasPolicy,static_cast<unsigned short>(d->NormalizedReadMemoryRanges.size()),static_cast<unsigned short>(d->NormalizedWriteMemoryRanges.size())}; }
	bool DmaStreamComputeOwnerDomainEvidence::IsComplete() const { return GuardAllowed && AuthoritySource==LegalityAuthoritySource::GuardPlane && !AttemptedReplayCertificateReuse && EvidenceHash!=0; }
	bool DmaStreamComputeOwnerDomainEvidence::Equals(const DmaStreamComputeOwnerDomainEvidence& o) const { return OwnerVirtualThreadId==o.OwnerVirtualThreadId&&OwnerContextId==o.OwnerContextId&&OwnerCoreId==o.OwnerCoreId&&OwnerPodId==o.OwnerPodId&&OwnerDomainTag==o.OwnerDomainTag&&DeviceId==o.DeviceId&&RuntimeOwnerVirtualThreadId==o.RuntimeOwnerVirtualThreadId&&RuntimeOwnerContextId==o.RuntimeOwnerContextId&&RuntimeOwnerCoreId==o.RuntimeOwnerCoreId&&RuntimeOwnerPodId==o.RuntimeOwnerPodId&&RuntimeOwnerDomainTag==o.RuntimeOwnerDomainTag&&ActiveDomainCertificate==o.ActiveDomainCertificate&&RuntimeDeviceId==o.RuntimeDeviceId&&GuardAllowed==o.GuardAllowed&&RejectKind==o.RejectKind&&AuthoritySource==o.AuthoritySource&&AttemptedReplayCertificateReuse==o.AttemptedReplayCertificateReuse&&EvidenceHash==o.EvidenceHash; }
	bool DmaStreamComputeTokenLifecycleEvidence::IsComplete() const { return DescriptorIdentityHash && EvidenceHash; }
	bool DmaStreamComputeTokenLifecycleEvidence::Equals(const DmaStreamComputeTokenLifecycleEvidence& o) const { return EvidenceKind==o.EvidenceKind&&DescriptorIdentityHash==o.DescriptorIdentityHash&&TokenId==o.TokenId&&State==o.State&&CancelReason==o.CancelReason&&FaultKind==o.FaultKind&&StagedWriteCount==o.StagedWriteCount&&HasCommitted==o.HasCommitted&&EvidenceHash==o.EvidenceHash; }
	DmaStreamComputeOwnerDomainEvidence DmaStreamComputeOwnerDomainEvidence::FromDecision(std::shared_ptr<DmaStreamComputeOwnerBinding> owner, std::shared_ptr<DmaStreamComputeOwnerGuardDecision> decision)
	{
		if (!owner || !decision) throw std::invalid_argument("owner/guardDecision");
		const auto& c=decision->RuntimeOwnerContext; const auto& l=decision->LegalityDecision; HardwareHash h; h.Initialize();
		h.Compress(owner->OwnerVirtualThreadId); h.Compress(owner->OwnerContextId); h.Compress(owner->OwnerCoreId); h.Compress(owner->OwnerPodId); h.Compress(owner->OwnerDomainTag); h.Compress(owner->DeviceId);
		h.Compress(c.OwnerVirtualThreadId); h.Compress(c.OwnerContextId); h.Compress(c.OwnerCoreId); h.Compress(c.OwnerPodId); h.Compress(c.OwnerDomainTag); h.Compress(c.ActiveDomainCertificate); h.Compress(c.DeviceId);
		h.Compress(decision->IsAllowed?1ULL:0ULL); h.Compress(static_cast<unsigned char>(l.getRejectKind())); h.Compress(static_cast<unsigned char>(l.getAuthoritySource())); h.Compress(l.getAttemptedReplayCertificateReuse()?1ULL:0ULL);
		auto digest=h.Finalize(); if(!digest) digest=0xD5C00A11ULL;
		return {owner->OwnerVirtualThreadId,owner->OwnerContextId,owner->OwnerCoreId,owner->OwnerPodId,owner->OwnerDomainTag,owner->DeviceId,c.OwnerVirtualThreadId,c.OwnerContextId,c.OwnerCoreId,c.OwnerPodId,c.OwnerDomainTag,c.ActiveDomainCertificate,c.DeviceId,decision->IsAllowed,l.getRejectKind(),l.getAuthoritySource(),l.getAttemptedReplayCertificateReuse(),digest};
	}
	DmaStreamComputeTokenLifecycleEvidence DmaStreamComputeTokenLifecycleEvidence::NotCreated(unsigned long long id)
	{
		HardwareHash h; h.Initialize(); h.Compress(static_cast<unsigned char>(DmaStreamComputeTokenEvidenceKind::NotCreated)); h.Compress(id); h.Compress(0); h.Compress(static_cast<unsigned char>(DmaStreamComputeTokenState::Admitted)); h.Compress(static_cast<unsigned char>(DmaStreamComputeTokenCancelReason::None)); h.Compress(static_cast<unsigned char>(DmaStreamComputeTokenFaultKind::None)); h.Compress(0); h.Compress(0);
		auto digest=h.Finalize(); if(!digest) digest=0xD5C070C0ULL;
		return {DmaStreamComputeTokenEvidenceKind::NotCreated,id,0,DmaStreamComputeTokenState::Admitted,DmaStreamComputeTokenCancelReason::None,DmaStreamComputeTokenFaultKind::None,0,false,digest};
	}
	DmaStreamComputeTokenLifecycleEvidence DmaStreamComputeTokenLifecycleEvidence::FromToken(std::shared_ptr<DmaStreamComputeToken> token)
	{
		if (!token) throw std::invalid_argument("token");
		const auto f=token->getLastFault(); const auto fk=f && f.value() ? f.value()->FaultKind : DmaStreamComputeTokenFaultKind::None;
		const auto state=token->getState(); const auto cancel=token->getCancelReason(); const auto count=token->getStagedWriteCount(); const auto committed=token->getHasCommitted(); const auto id=token->getDescriptor()->DescriptorIdentityHash; const auto tid=token->getTokenId();
		HardwareHash h; h.Initialize(); h.Compress(static_cast<unsigned char>(DmaStreamComputeTokenEvidenceKind::LifecycleSnapshot)); h.Compress(id); h.Compress(tid); h.Compress(static_cast<unsigned char>(state)); h.Compress(static_cast<unsigned char>(cancel)); h.Compress(static_cast<unsigned char>(fk)); h.Compress(static_cast<unsigned int>(count)); h.Compress(committed?1ULL:0ULL);
		auto digest=h.Finalize(); if(!digest) digest=0xD5C070C0ULL;
		return {DmaStreamComputeTokenEvidenceKind::LifecycleSnapshot,id,tid,state,cancel,fk,count,committed,digest};
	}
	DmaStreamComputeLanePlacementEvidence DmaStreamComputeLanePlacementEvidence::DescriptorSurface() { return MaterializedLane6(6,SlotClassLaneMap::GetLaneMask(SlotClass::DmaStreamClass),0,false); }
	DmaStreamComputeLanePlacementEvidence DmaStreamComputeLanePlacementEvidence::MaterializedLane6(int lane,unsigned char freeMask,unsigned char donorMask,bool replay)
	{
		HardwareHash h; h.Initialize(); h.Compress(static_cast<unsigned char>(SlotClass::DmaStreamClass)); h.Compress(static_cast<unsigned char>(SlotPinningKind::HardPinned)); h.Compress(static_cast<unsigned int>(lane)); h.Compress(freeMask); h.Compress(donorMask); h.Compress(replay?1ULL:0ULL);
		auto digest=h.Finalize(); if(!digest) digest=0xD5C01A6EULL;
		return {SlotClass::DmaStreamClass,SlotPinningKind::HardPinned,lane,freeMask,donorMask,replay,digest};
	}
	std::shared_ptr<DmaStreamComputeReplayEvidence> DmaStreamComputeReplayEvidence::CreateForDescriptor(std::shared_ptr<DmaStreamComputeDescriptor> d,DmaStreamComputeCarrierEvidence carrier,DmaStreamComputeTokenLifecycleEvidence token,DmaStreamComputeLanePlacementEvidence lane)
	{
		if(!d) throw std::invalid_argument("descriptor");
		if(carrier.CarrierKind==DmaStreamComputeCarrierKind::None) carrier=DmaStreamComputeCarrierEvidence::CanonicalDecodedSideband(std::make_shared<DmaStreamComputeDescriptorReference>(d->DescriptorReference));
		if(!token.EvidenceHash) token=DmaStreamComputeTokenLifecycleEvidence::NotCreated(d->DescriptorIdentityHash);
		if(!lane.EvidenceHash) lane=DmaStreamComputeLanePlacementEvidence::DescriptorSurface();
		auto footprint=DmaStreamComputeFootprintEvidence::FromDescriptor(d); auto owner=DmaStreamComputeOwnerDomainEvidence::FromDecision(d->OwnerBinding,d->OwnerGuardDecision);
		HardwareHash h; h.Initialize(); const auto& r=d->DescriptorReference; h.Compress(r.DescriptorAddress); h.Compress(r.DescriptorSize); h.Compress(r.DescriptorIdentityHash); h.Compress(d->AbiVersion); h.Compress(d->HeaderSize); h.Compress(d->TotalSize); h.Compress(d->DescriptorIdentityHash); h.Compress(d->CertificateInputHash); h.Compress(static_cast<unsigned short>(d->Operation)); h.Compress(static_cast<unsigned short>(d->ElementType)); h.Compress(static_cast<unsigned short>(d->Shape)); h.Compress(static_cast<unsigned short>(d->RangeEncoding)); h.Compress(static_cast<unsigned short>(d->PartialCompletionPolicy)); h.Compress(static_cast<unsigned short>(d->AliasPolicy)); h.Compress(carrier.CarrierIdentityHash); h.Compress(footprint.NormalizedReadFootprintHash); h.Compress(footprint.NormalizedWriteFootprintHash); h.Compress(footprint.NormalizedFootprintHash); h.Compress(owner.EvidenceHash); h.Compress(token.EvidenceHash); h.Compress(lane.EvidenceHash);
		auto envelope=h.Finalize(); if(!envelope) envelope=0xD5C0E8E0ULL;
		auto result=std::make_shared<DmaStreamComputeReplayEvidence>(); result->DescriptorReference=std::make_shared<DmaStreamComputeDescriptorReference>(r); result->AbiVersion=d->AbiVersion; result->HeaderSize=d->HeaderSize; result->TotalSize=d->TotalSize; result->DescriptorIdentityHash=d->DescriptorIdentityHash; result->CertificateInputHash=d->CertificateInputHash; result->Operation=d->Operation; result->ElementType=d->ElementType; result->Shape=d->Shape; result->RangeEncoding=d->RangeEncoding; result->PartialCompletionPolicy=d->PartialCompletionPolicy; result->AliasPolicy=d->AliasPolicy; result->CarrierEvidence=carrier; result->FootprintEvidence=footprint; result->OwnerDomainEvidence=owner; result->TokenLifecycleEvidence=token; result->LanePlacementEvidence=lane; result->EnvelopeHash=envelope; return result;
	}
	bool DmaStreamComputeReplayEvidence::IsComplete() const
	{
		return DescriptorIdentityHash != 0 &&
			DescriptorReference != nullptr &&
			DescriptorReference->DescriptorSize != 0 &&
			DescriptorReference->DescriptorIdentityHash == DescriptorIdentityHash &&
			CarrierEvidence.IsReplayReusable() &&
			FootprintEvidence.IsComplete() &&
			OwnerDomainEvidence.IsComplete() &&
			TokenLifecycleEvidence.IsComplete() &&
			LanePlacementEvidence.IsComplete() &&
			EnvelopeHash != 0;
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public static bool CanReuse(DmaStreamComputeReplayEvidence expected, DmaStreamComputeReplayEvidence live, out DmaStreamComputeReplayEvidenceComparison comparison, DmaStreamComputeTelemetryCounters? telemetry = null)
	bool DmaStreamComputeReplayEvidenceComparer::CanReuse(std::shared_ptr<DmaStreamComputeReplayEvidence> expected, std::shared_ptr<DmaStreamComputeReplayEvidence> live, std::shared_ptr<DmaStreamComputeReplayEvidenceComparison> &comparison, std::shared_ptr<DmaStreamComputeTelemetryCounters> telemetry)
	{
		comparison = Compare(expected, live);
		if (telemetry != nullptr)
		{
			telemetry->RecordReplayEvidenceComparison(comparison);
		}
		return comparison->CanReuse;
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public static DmaStreamComputeReplayEvidenceComparison Compare(DmaStreamComputeReplayEvidence expected, DmaStreamComputeReplayEvidence live, DmaStreamComputeTelemetryCounters? telemetry = null)
	std::shared_ptr<DmaStreamComputeReplayEvidenceComparison> DmaStreamComputeReplayEvidenceComparer::Compare(std::shared_ptr<DmaStreamComputeReplayEvidence> expected, std::shared_ptr<DmaStreamComputeReplayEvidence> live, std::shared_ptr<DmaStreamComputeTelemetryCounters> telemetry)
	{
		std::shared_ptr<DmaStreamComputeReplayEvidenceComparison> comparison;

		if (HasDescriptorPayloadLoss(expected) || HasDescriptorPayloadLoss(live))
		{
			comparison = DmaStreamComputeReplayEvidenceComparison::Miss(ReplayPhaseInvalidationReason::DmaStreamComputeDescriptorPayloadLost, L"CarrierPayload");
			if (telemetry != nullptr)
			{
				telemetry->RecordReplayEvidenceComparison(comparison);
			}
			return comparison;
		}

		if (HasUnsupportedCarrier(expected) || HasUnsupportedCarrier(live))
		{
			comparison = DmaStreamComputeReplayEvidenceComparison::Miss(ReplayPhaseInvalidationReason::DmaStreamComputeCarrierMismatch, L"Carrier");
			if (telemetry != nullptr)
			{
				telemetry->RecordReplayEvidenceComparison(comparison);
			}
			return comparison;
		}

		if (!expected->IsComplete() || !live->IsComplete())
		{
			comparison = DmaStreamComputeReplayEvidenceComparison::Miss(ReplayPhaseInvalidationReason::DmaStreamComputeIncompleteEvidence, L"IncompleteEvidence");
			if (telemetry != nullptr)
			{
				telemetry->RecordReplayEvidenceComparison(comparison);
			}
			return comparison;
		}

		std::wstring descriptorField;
		if (!DescriptorShapeMatches(expected, live, descriptorField))
		{
			comparison = DmaStreamComputeReplayEvidenceComparison::Miss(ReplayPhaseInvalidationReason::DmaStreamComputeDescriptorMismatch, descriptorField);
			if (telemetry != nullptr)
			{
				telemetry->RecordReplayEvidenceComparison(comparison);
			}
			return comparison;
		}

		if (!expected->CarrierEvidence.Equals(live->CarrierEvidence))
		{
			comparison = DmaStreamComputeReplayEvidenceComparison::Miss(ReplayPhaseInvalidationReason::DmaStreamComputeCarrierMismatch, L"Carrier");
			if (telemetry != nullptr)
			{
				telemetry->RecordReplayEvidenceComparison(comparison);
			}
			return comparison;
		}

		if (expected->CertificateInputHash != live->CertificateInputHash)
		{
			comparison = DmaStreamComputeReplayEvidenceComparison::Miss(ReplayPhaseInvalidationReason::DmaStreamComputeCertificateInputMismatch, L"CertificateInputHash");
			if (telemetry != nullptr)
			{
				telemetry->RecordReplayEvidenceComparison(comparison);
			}
			return comparison;
		}

		if (!expected->FootprintEvidence.Equals(live->FootprintEvidence))
		{
			comparison = DmaStreamComputeReplayEvidenceComparison::Miss(ReplayPhaseInvalidationReason::DmaStreamComputeFootprintMismatch, L"Footprint");
			if (telemetry != nullptr)
			{
				telemetry->RecordReplayEvidenceComparison(comparison);
			}
			return comparison;
		}

		if (!expected->OwnerDomainEvidence.Equals(live->OwnerDomainEvidence))
		{
			comparison = DmaStreamComputeReplayEvidenceComparison::Miss(ReplayPhaseInvalidationReason::DmaStreamComputeOwnerDomainMismatch, L"OwnerDomain");
			if (telemetry != nullptr)
			{
				telemetry->RecordReplayEvidenceComparison(comparison);
			}
			return comparison;
		}

		if (!expected->TokenLifecycleEvidence.Equals(live->TokenLifecycleEvidence))
		{
			comparison = DmaStreamComputeReplayEvidenceComparison::Miss(ReplayPhaseInvalidationReason::DmaStreamComputeTokenEvidenceMismatch, L"TokenLifecycle");
			if (telemetry != nullptr)
			{
				telemetry->RecordReplayEvidenceComparison(comparison);
			}
			return comparison;
		}

		const auto& expectedLane = expected->LanePlacementEvidence;
		const auto& liveLane = live->LanePlacementEvidence;
		if (expectedLane.RequiredSlotClass != liveLane.RequiredSlotClass ||
			expectedLane.PinningKind != liveLane.PinningKind ||
			expectedLane.SelectedLane != liveLane.SelectedLane ||
			expectedLane.FreeLaneMask != liveLane.FreeLaneMask ||
			expectedLane.StableDonorMask != liveLane.StableDonorMask ||
			expectedLane.ReplayActive != liveLane.ReplayActive ||
			expectedLane.EvidenceHash != liveLane.EvidenceHash)
		{
			comparison = DmaStreamComputeReplayEvidenceComparison::Miss(ReplayPhaseInvalidationReason::DmaStreamComputeLanePlacementMismatch, L"LanePlacement");
			if (telemetry != nullptr)
			{
				telemetry->RecordReplayEvidenceComparison(comparison);
			}
			return comparison;
		}

		if (expected->EnvelopeHash != live->EnvelopeHash)
		{
			comparison = DmaStreamComputeReplayEvidenceComparison::Miss(ReplayPhaseInvalidationReason::DmaStreamComputeDescriptorMismatch, L"EnvelopeHash");
			if (telemetry != nullptr)
			{
				telemetry->RecordReplayEvidenceComparison(comparison);
			}
			return comparison;
		}

		comparison = DmaStreamComputeReplayEvidenceComparison::ReuseHit;
		if (telemetry != nullptr)
		{
			telemetry->RecordReplayEvidenceComparison(comparison);
		}
		return comparison;
	}

	bool DmaStreamComputeReplayEvidenceComparer::HasDescriptorPayloadLoss(std::shared_ptr<DmaStreamComputeReplayEvidence> evidence)
	{
		return evidence->CarrierEvidence.CarrierKind == DmaStreamComputeCarrierKind::MissingDescriptorPayload ||
			(evidence->CarrierEvidence.CarrierKind == DmaStreamComputeCarrierKind::CanonicalDecodedSideband &&
				(evidence->CarrierEvidence.DescriptorReference == nullptr ||
					evidence->CarrierEvidence.DescriptorReference->DescriptorSize == 0 ||
					evidence->CarrierEvidence.DescriptorReference->DescriptorIdentityHash == 0));
	}

	bool DmaStreamComputeReplayEvidenceComparer::HasUnsupportedCarrier(std::shared_ptr<DmaStreamComputeReplayEvidence> evidence)
	{
		return evidence->CarrierEvidence.CarrierKind == DmaStreamComputeCarrierKind::CustomRegistryCarrier;
	}

	bool DmaStreamComputeReplayEvidenceComparer::DescriptorShapeMatches(std::shared_ptr<DmaStreamComputeReplayEvidence> expected, std::shared_ptr<DmaStreamComputeReplayEvidence> live, std::wstring &mismatchField)
	{
		mismatchField = L"";

		if (expected->DescriptorReference == nullptr || live->DescriptorReference == nullptr ||
			*expected->DescriptorReference != *live->DescriptorReference)
		{
			mismatchField = L"DescriptorReference";
			return false;
		}

		if (expected->DescriptorIdentityHash != live->DescriptorIdentityHash)
		{
			mismatchField = L"DescriptorIdentity";
			return false;
		}

		if (expected->AbiVersion != live->AbiVersion)
		{
			mismatchField = L"AbiVersion";
			return false;
		}

		if (expected->HeaderSize != live->HeaderSize || expected->TotalSize != live->TotalSize)
		{
			mismatchField = L"DescriptorSize";
			return false;
		}

		if (expected->Operation != live->Operation)
		{
			mismatchField = L"Operation";
			return false;
		}

		if (expected->ElementType != live->ElementType)
		{
			mismatchField = L"ElementType";
			return false;
		}

		if (expected->Shape != live->Shape)
		{
			mismatchField = L"Shape";
			return false;
		}

		if (expected->RangeEncoding != live->RangeEncoding)
		{
			mismatchField = L"RangeEncoding";
			return false;
		}

		if (expected->PartialCompletionPolicy != live->PartialCompletionPolicy)
		{
			mismatchField = L"PartialCompletionPolicy";
			return false;
		}

		if (expected->AliasPolicy != live->AliasPolicy)
		{
			mismatchField = L"AliasPolicy";
			return false;
		}

		return true;
	}
}
