#include "AcceleratorCommandDescriptor.h"
#include "../Auth/AcceleratorOwnerDomainGuard.h"

using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Auth;

namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Descriptors
{
		bool AcceleratorCommandDescriptor::Equals(const std::shared_ptr<AcceleratorCommandDescriptor>& other) const {
			if (!other) return false;
			auto ownerEqual = [](const auto& a, const auto& b) { return a == b || (a && b && *a == *b); };
			auto rangesEqual = [](const auto& a, const auto& b) {
				if (a.size() != b.size()) return false;
				for (size_t i = 0; i < a.size(); ++i)
					if (a[i] != b[i] && (!a[i] || !b[i] || !(*a[i] == *b[i]))) return false;
				return true;
			};
			            auto guardEqual = [&](const auto& a, const auto& b) {
                if (a == b) return true;
                if (!a || !b) return false;
                auto evidenceEqual = [](const auto& x, const auto& y) {
                    return x == y || (x && y && x->Source == y->Source && x->OwnerVirtualThreadId == y->OwnerVirtualThreadId
                        && x->OwnerContextId == y->OwnerContextId && x->OwnerCoreId == y->OwnerCoreId && x->OwnerPodId == y->OwnerPodId
                        && x->DomainTag == y->DomainTag && x->ActiveDomainCertificate == y->ActiveDomainCertificate
                        && x->MappingEpoch == y->MappingEpoch && x->IommuDomainEpoch == y->IommuDomainEpoch
                        && x->EvidenceIdentity == y->EvidenceIdentity && x->RegistryAcceleratorId == y->RegistryAcceleratorId);
                };
                const auto& x = a->LegalityDecision; const auto& y = b->LegalityDecision;
                return a->Surface == b->Surface && a->Fault == b->Fault && ownerEqual(a->DescriptorOwnerBinding, b->DescriptorOwnerBinding)
                    && evidenceEqual(a->Evidence, b->Evidence) && a->Message == b->Message
                    && x.isAllowed() == y.isAllowed() && x.getRejectKind() == y.getRejectKind()
                    && x.getCertificateDetail() == y.getCertificateDetail() && x.getAuthoritySource() == y.getAuthoritySource()
                    && x.getAttemptedReplayCertificateReuse() == y.getAttemptedReplayCertificateReuse();
            };
            const auto& h = Header; const auto& o = other->Header;
			return DescriptorReference == other->DescriptorReference
				&& h.Magic == o.Magic && h.AbiVersion == o.AbiVersion && h.HeaderSize == o.HeaderSize && h.DescriptorSize == o.DescriptorSize
				&& h.AcceleratorClass == o.AcceleratorClass && h.AcceleratorId == o.AcceleratorId && h.Operation == o.Operation && h.Datatype == o.Datatype && h.Shape == o.Shape
				&& h.ShapeRank == o.ShapeRank && h.SourceRangeCount == o.SourceRangeCount && h.DestinationRangeCount == o.DestinationRangeCount && h.ScratchRangeCount == o.ScratchRangeCount
				&& h.PartialCompletionPolicy == o.PartialCompletionPolicy && h.Alignment == o.Alignment && h.ElementCount == o.ElementCount && h.CapabilityVersion == o.CapabilityVersion
				&& h.Identity == o.Identity && ownerEqual(h.OwnerBinding, o.OwnerBinding) && h.ScratchRequiredBytes == o.ScratchRequiredBytes
				&& AbiVersion == other->AbiVersion && HeaderSize == other->HeaderSize && DescriptorSize == other->DescriptorSize
				&& AcceleratorClass == other->AcceleratorClass && AcceleratorId == other->AcceleratorId && Operation == other->Operation && Datatype == other->Datatype && Shape == other->Shape
				&& ShapeRank == other->ShapeRank && ElementCount == other->ElementCount && CapabilityVersion == other->CapabilityVersion && Alignment == other->Alignment
				&& ScratchRequirement.RequiredBytes == other->ScratchRequirement.RequiredBytes && rangesEqual(ScratchRequirement.Ranges, other->ScratchRequirement.Ranges)
				&& PartialCompletionPolicy == other->PartialCompletionPolicy && ownerEqual(OwnerBinding, other->OwnerBinding) && guardEqual(OwnerGuardDecision, other->OwnerGuardDecision)
				&& Identity == other->Identity && rangesEqual(SourceRanges, other->SourceRanges) && rangesEqual(DestinationRanges, other->DestinationRanges) && rangesEqual(ScratchRanges, other->ScratchRanges)
				&& NormalizedFootprint.Hash == other->NormalizedFootprint.Hash && rangesEqual(NormalizedFootprint.SourceRanges, other->NormalizedFootprint.SourceRanges)
				&& rangesEqual(NormalizedFootprint.DestinationRanges, other->NormalizedFootprint.DestinationRanges) && rangesEqual(NormalizedFootprint.ScratchRanges, other->NormalizedFootprint.ScratchRanges);
		}

}
