#include "YAKSys_Hybrid_CPU.Core.SafetyVerifier.h"
#include "../Metadata/MicroOpAdmissionMetadata.h"
#include "../MicroOps/Types/MicroOp.h"
#include "../../Runtime/Completion/Records/SecureComputeNoEffectCompletionOwner.h"
#include "../../Runtime/Completion/Records/SecureComputeMemoryReadCompletionOwner.h"
#include "../MicroOps/Types/MicroOp.IO.h"
#include "../MicroOps/Memory/MicroOp.LoadStore.h"
#include "../../Decoder/CanonicalSecurityOperationIdentity.h"
#include "../../Decoder/Sha256Digest.h"
#include "../../Execution/Rf07ExecutionOutcomeContracts.h"
#include "../Scheduling/SlotPlacementMetadata.h"
#include "../../Architecture/Common/YAKSys_Hybrid_CPU.Processor.h"
#include "../../../../NonRTL/Arch/Compat/HybridCPU_ISE.Arch.VLIW_Instruction.h"
#include "../../Decoder/DeclarativeDecoderStages.h"
#include "../Scheduling/Rf06ScalarSchedulerRouting.h"
#include "../../Runtime/Events/Hypercalls/Governance/VirtualizationOperationOwnerSnapshot.h"
#include "../../Runtime/Events/Hypercalls/Governance/Phase38VirtualizationDecisionAcceptanceV2.h"
#include "../../Runtime/Capabilities/Grants/RuntimeCapabilityGrantLease.h"
#include "../../Runtime/Capabilities/Descriptors/YAKSys_Hybrid_CPU.Core.CapabilityDescriptorSet.h"
#include "../../../../ExternalRuntime/Contracts/ExternalSecureMemoryRead.h"
#include "../../Runtime/Domains/Authority/YAKSys_Hybrid_CPU.Core.RootAuthorityDescriptor.h"
#include "../../Runtime/Events/Hypercalls/Governance/Phase38VirtualizationDecisionSpecV2.h"
#include "../../Runtime/Governance/Virtualization/VirtualizationDecisionValidatorV2.h"
#include "SecureComputeMemoryReadContractProjection.h"
#include "../../Runtime/Capabilities/Governance/YAKSys_Hybrid_CPU.Core.RuntimeCapabilityIds.h"
#include "../../Runtime/Capabilities/Grants/CapabilityGrant.h"
#include "../../Runtime/Domains/Services/YAKSys_Hybrid_CPU.Core.DomainRuntimeOperation.h"
#include "../../Runtime/Domains/Services/DomainRuntimeOperation.h"
#include "../../Runtime/Events/Hypercalls/DomainHypercallRuntimeExecutor.h"
#include "../../Runtime/Events/Hypercalls/VirtualizationOperationDecisionManifest.h"
#include "../Assist/InterCore/YAKSys_Hybrid_CPU.Core.MicroOpScheduler.h"

using namespace YAKSys_Hybrid_CPU::Core::Execution::DmaStreamCompute;
using namespace HybridCPU_ISE::CloseToHSL::Memory::MMU;
using namespace YAKSys_Hybrid_CPU::Core::Pipeline::MicroOps;
using namespace HybridCPU::ExternalRuntime::Contracts;
using namespace HybridCPU_ISE::Arch;
using namespace YAKSys_Hybrid_CPU::Core::Decoder;
using namespace YAKSys_Hybrid_CPU::Core::Registers;
using namespace YAKSys_Hybrid_CPU::Core::Registers::Retire;

namespace
{
	bool SameMutexIdentity(const std::any &left, const std::any &right)
	{
		if (left.type() != right.type()) return false;
		if (const auto *l = std::any_cast<std::shared_ptr<std::mutex>>(&left))
		{
			const auto *r = std::any_cast<std::shared_ptr<std::mutex>>(&right);
			return r != nullptr && *l == *r;
		}
		if (const auto *l = std::any_cast<std::shared_ptr<std::recursive_mutex>>(&left))
		{
			const auto *r = std::any_cast<std::shared_ptr<std::recursive_mutex>>(&right);
			return r != nullptr && *l == *r;
		}
		return false;
	}

	std::shared_ptr<std::mutex> GetMutex(const std::any &value)
	{
		if (const auto *mutex = std::any_cast<std::shared_ptr<std::mutex>>(&value)) return *mutex;
		return nullptr;
	}

	struct ScopeExit final
	{
		std::function<void()> Action;
		~ScopeExit() { if (Action) Action(); }
	};
}

namespace YAKSys_Hybrid_CPU::Core
{

	LegalityDecision SafetyVerifier::EvaluateSmtCertificate(BundleResourceCertificate4Way certificate, std::shared_ptr<MicroOp> candidate, LegalityAuthoritySource authoritySource, bool attemptedReplayReuse)
	{
		CertificateRejectDetail detail;
		if (certificate.CanInjectWithReason(candidate, detail))
		{
			return LegalityDecision::Allow(authoritySource, attemptedReplayReuse);
		}

		return LegalityDecision::Reject(MapCertificateReject(detail), detail, authoritySource, attemptedReplayReuse);
	}

	LegalityDecision SafetyVerifier::EvaluateInterCoreCertificate(std::shared_ptr<BundleResourceCertificate> certificate, std::shared_ptr<MicroOp> candidate, LegalityAuthoritySource authoritySource, bool attemptedReplayReuse)
	{
		CertificateRejectDetail detail;
		if (certificate->CanInjectWithReason(candidate, detail))
		{
			return LegalityDecision::Allow(authoritySource, attemptedReplayReuse);
		}

		return LegalityDecision::Reject(MapCertificateReject(detail), detail, authoritySource, attemptedReplayReuse);
	}

	RejectKind SafetyVerifier::MapCertificateReject(CertificateRejectDetail detail)
	{
	// C# TO C++ CONVERTER TASK: The following 'switch expression' was not converted by C# to C++ Converter:
	//			return detail switch
	//			{
	//				CertificateRejectDetail.SharedResourceConflict => RejectKind.CrossLaneConflict,
	//				CertificateRejectDetail.RegisterGroupConflict => RejectKind.CrossLaneConflict,
	//				_ => RejectKind.CrossLaneConflict
	//			};
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private bool VerifyInjectionFast128(IReadOnlyList<MicroOp?> bundle, MicroOp candidate, SafetyMask128 globalHardwareMask = default)
	bool SafetyVerifier::VerifyInjectionFast128(const std::vector<std::shared_ptr<MicroOp>> &bundle, std::shared_ptr<MicroOp> candidate, SafetyMask128 globalHardwareMask)
	{
		std::shared_ptr<MicroOpAdmissionMetadata> candidateAdmission = candidate->getAdmissionMetadata();
		SafetyMask128 candidateSharedMask = candidateAdmission->SharedStructuralMask;
		unsigned int candidateRegisterMask = candidateAdmission->RegisterHazardMask;

		if (candidateSharedMask.isZero() && candidateRegisterMask == 0)
		{
			return false;
		}

		SafetyMask128 bundleSharedMask = globalHardwareMask;
		unsigned int bundleRegisterMask = 0;
		for (int i = 0; i < bundle.size(); i++)
		{
			std::shared_ptr<MicroOp> op = bundle[i];
			if (op != nullptr)
			{
				std::shared_ptr<MicroOpAdmissionMetadata> admission = op->getAdmissionMetadata();
				bundleSharedMask = bundleSharedMask | admission->SharedStructuralMask;
				bundleRegisterMask |= admission->RegisterHazardMask;
			}
		}

		if (candidateSharedMask.isNonZero() && bundleSharedMask.ConflictsWith(candidateSharedMask))
		{
			return false;
		}

		return !HasRegisterConflict(bundleRegisterMask, candidateRegisterMask);
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private LegalityDecision EvaluateInterCoreCompatibility(IReadOnlyList<MicroOp?> bundle, MicroOp candidate, int bundleOwnerThreadId, SafetyMask128 globalHardwareMask)
	LegalityDecision SafetyVerifier::EvaluateInterCoreCompatibility(const std::vector<std::shared_ptr<MicroOp>> &bundle, std::shared_ptr<MicroOp> candidate, int bundleOwnerThreadId, SafetyMask128 globalHardwareMask)
	{
		if (HasStructuralAdmissionMask(candidate) && BundleHasAllStructuralAdmissionMasks(bundle))
		{
			constexpr LegalityAuthoritySource structuralAuthoritySource = LegalityAuthoritySource::AdmissionMetadataStructuralCheck;

			return VerifyInjectionFast128(bundle, candidate, globalHardwareMask) ? LegalityDecision::Allow(structuralAuthoritySource, false) : LegalityDecision::Reject(RejectKind::CrossLaneConflict, CertificateRejectDetail::SharedResourceConflict, structuralAuthoritySource, false);
		}

		constexpr LegalityAuthoritySource authoritySource = LegalityAuthoritySource::DetailedCompatibilityCheck;

		if (!CheckRegisterDependencies(bundle, candidate, bundleOwnerThreadId, candidate->getOwnerThreadId()))
		{
			return LegalityDecision::Reject(RejectKind::CrossLaneConflict, CertificateRejectDetail::RegisterGroupConflict, authoritySource, false);
		}

		if (!CheckMemoryDependencies(bundle, candidate, bundleOwnerThreadId, candidate->getOwnerThreadId()))
		{
			return LegalityDecision::Reject(RejectKind::CrossLaneConflict, CertificateRejectDetail::SharedResourceConflict, authoritySource, false);
		}

		return LegalityDecision::Allow(authoritySource, false);
	}

	SafetyMask128 SafetyVerifier::GetStructuralAdmissionMask(std::shared_ptr<MicroOp> microOp)
	{
		return microOp == nullptr ? SafetyMask128::Zero : microOp->getAdmissionMetadata()->StructuralSafetyMask;
	}

	std::vector<MicroOpMemoryRange> SafetyVerifier::GetSafetyVisibleReadRanges(std::shared_ptr<MicroOp> microOp)
	{
		if (microOp == nullptr)
		{
			return {};
		}

		const auto admission = microOp->getAdmissionMetadata();
		if (admission != nullptr && !admission->NormalizedReadMemoryRanges.empty())
		{
			std::vector<MicroOpMemoryRange> normalizedRanges;
			normalizedRanges.reserve(admission->NormalizedReadMemoryRanges.size());
			for (const auto &[address, length] : admission->NormalizedReadMemoryRanges)
			{
				normalizedRanges.push_back({address, length});
			}
			return normalizedRanges;
		}

		return microOp->getReadMemoryRanges();
	}

	bool SafetyVerifier::HasStructuralAdmissionMask(std::shared_ptr<MicroOp> microOp)
	{
		return microOp != nullptr && microOp->getAdmissionMetadata()->HasStructuralSafetyMask;
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private static bool BundleHasAllStructuralAdmissionMasks(IReadOnlyList<MicroOp?> bundle)
	bool SafetyVerifier::BundleHasAllStructuralAdmissionMasks(const std::vector<std::shared_ptr<MicroOp>> &bundle)
	{
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: foreach (MicroOp? op in bundle)
		for (auto op : bundle)
		{
			if (op != nullptr && !HasStructuralAdmissionMask(op))
			{
				return false;
			}
		}

		return true;
	}

	bool SafetyVerifier::HasRegisterConflict(unsigned int bundleRegisterMask, unsigned int candidateRegisterMask)
	{
		unsigned short candidateRead = static_cast<unsigned short>(candidateRegisterMask & 0xFFFF);
		unsigned short candidateWrite = static_cast<unsigned short>(candidateRegisterMask >> 16);
		unsigned short bundleRead = static_cast<unsigned short>(bundleRegisterMask & 0xFFFF);
		unsigned short bundleWrite = static_cast<unsigned short>(bundleRegisterMask >> 16);

		return ((candidateRead & bundleWrite) | (candidateWrite & bundleRead) | (candidateWrite & bundleWrite)) != 0;
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private bool CheckRegisterDependencies(IReadOnlyList<MicroOp?> bundle, MicroOp candidate, int bundleOwnerThreadId, int candidateOwnerThreadId)
	bool SafetyVerifier::CheckRegisterDependencies(const std::vector<std::shared_ptr<MicroOp>> &bundle, std::shared_ptr<MicroOp> candidate, int bundleOwnerThreadId, int candidateOwnerThreadId)
	{
		const auto candidateReadRegisters = candidate->getReadRegisters();
		const auto candidateWriteRegisters = candidate->getWriteRegisters();
		std::unordered_set<int> candidateReads(candidateReadRegisters.begin(), candidateReadRegisters.end());
		std::unordered_set<int> candidateWrites(candidateWriteRegisters.begin(), candidateWriteRegisters.end());

		for (auto op : bundle)
		{
			if (op == nullptr)
			{
				continue;
			}

			const auto readRegisters = op->getReadRegisters();
			const auto writeRegisters = op->getWriteRegisters();
			std::unordered_set<int> opReads(readRegisters.begin(), readRegisters.end());
			std::unordered_set<int> opWrites(writeRegisters.begin(), writeRegisters.end());

			if (std::any_of(candidateWrites.begin(), candidateWrites.end(), [&opReads](int reg) { return opReads.contains(reg); }))
			{
				return false;
			}

			if (std::any_of(candidateReads.begin(), candidateReads.end(), [&opWrites](int reg) { return opWrites.contains(reg); }))
			{
				return false;
			}

			if (std::any_of(candidateWrites.begin(), candidateWrites.end(), [&opWrites](int reg) { return opWrites.contains(reg); }))
			{
				return false;
			}
		}

		return true;
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private bool CheckMemoryDependencies(IReadOnlyList<MicroOp?> bundle, MicroOp candidate, int bundleOwnerThreadId, int candidateOwnerThreadId)
	bool SafetyVerifier::CheckMemoryDependencies(const std::vector<std::shared_ptr<MicroOp>> &bundle, std::shared_ptr<MicroOp> candidate, int bundleOwnerThreadId, int candidateOwnerThreadId)
	{
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: IReadOnlyList<(ulong Address, ulong Length)> candidateReadRanges = GetSafetyVisibleReadRanges(candidate);
		const std::vector<MicroOp::MemoryRange> candidateReadRanges = GetSafetyVisibleReadRanges(candidate);
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: IReadOnlyList<(ulong Address, ulong Length)> candidateWriteRanges = candidate.WriteMemoryRanges ?? Array.Empty<(ulong Address, ulong Length)>();
		const std::vector<MicroOp::MemoryRange> candidateWriteRanges = candidate->getWriteMemoryRanges();

		if (candidateReadRanges.empty() && candidateWriteRanges.empty())
		{
			return true;
		}

		for (auto op : bundle)
		{
			if (op == nullptr)
			{
				continue;
			}

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: IReadOnlyList<(ulong Address, ulong Length)> opReadRanges = GetSafetyVisibleReadRanges(op);
			const std::vector<MicroOp::MemoryRange> opReadRanges = GetSafetyVisibleReadRanges(op);
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: IReadOnlyList<(ulong Address, ulong Length)> opWriteRanges = op.WriteMemoryRanges ?? Array.Empty<(ulong Address, ulong Length)>();
			const std::vector<MicroOp::MemoryRange> opWriteRanges = op->getWriteMemoryRanges();

			if (candidateWriteRanges.size() != 0 && opReadRanges.size() != 0)
			{
				if (HasRangeOverlap(candidateWriteRanges, opReadRanges))
				{
					return false;
				}
			}

			if (candidateReadRanges.size() != 0 && opWriteRanges.size() != 0)
			{
				if (HasRangeOverlap(candidateReadRanges, opWriteRanges))
				{
					return false;
				}
			}

			if (candidateWriteRanges.size() != 0 && opWriteRanges.size() != 0)
			{
				if (HasRangeOverlap(candidateWriteRanges, opWriteRanges))
				{
					return false;
				}
			}
		}

		return true;
	}

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private bool HasRangeOverlap(IReadOnlyList<(ulong Address, ulong Length)> ranges1, IReadOnlyList<(ulong Address, ulong Length)> ranges2)
	bool SafetyVerifier::HasRangeOverlap(const std::vector<MicroOp::MemoryRange> &ranges1, const std::vector<MicroOp::MemoryRange> &ranges2)
	{
		return HasAnyMemoryRangeOverlap(ranges1, ranges2);
	}

	VerificationContext SafetyVerifier::getFormalContext() const
	{
		return FormalContext;
	}

	void SafetyVerifier::setFormalContext(VerificationContext value)
	{
		FormalContext = value;
	}

	SafetyVerifier::SafetyVerifier()
	{
		setFormalContext(VerificationContext());
	}

	bool SafetyVerifier::getKernelDomainIsolation() const
	{
		return KernelDomainIsolation;
	}

	void SafetyVerifier::setKernelDomainIsolation(bool value)
	{
		KernelDomainIsolation = value;
	}

	bool SafetyVerifier::isKernelDomainIsolationEnabled() const
	{
		return getKernelDomainIsolation();
	}

	bool SafetyVerifier::VerifyDomainCertificate(std::shared_ptr<MicroOp> injectedOp, unsigned long long podDomainCert)
	{
		return EvaluateDomainIsolationProbe(injectedOp, podDomainCert).isAllowed();
	}

	DomainIsolationProbeResult SafetyVerifier::EvaluateDomainIsolationProbe(std::shared_ptr<MicroOp> injectedOp, unsigned long long podDomainCert)
	{
		ArgumentNullException::ThrowIfNull(injectedOp);
		return EvaluateDomainIsolationProbe(injectedOp->getPlacement().DomainTag, podDomainCert);
	}

	DomainIsolationProbeResult SafetyVerifier::EvaluateDomainIsolationProbe(unsigned long long domainTag, unsigned long long podDomainCert)
	{
		// Phase 04 guard plane: when KernelDomainIsolation is active, kernel ops
		// (DomainTag==0) must not be admitted into user-domain bundles.
		// This closes the timing side-channel where a privileged micro-op could
		// probe user cache/TLB state via replay-aware densification.
		// HLS: single AND gate, zero gate delay addition.
		if (domainTag == 0)
		{
			if (getKernelDomainIsolation() && podDomainCert != 0)
			{
				return DomainIsolationProbeResult(false, false, true);
			}

			return DomainIsolationProbeResult(true, false, false);
		}

		// podDomainCert 0 = no domain enforcement configured
		if (podDomainCert == 0)
		{
			return DomainIsolationProbeResult(true, false, false);
		}

		// Hardware AND gate: domain must match certificate
		bool allowed = (domainTag & podDomainCert) != 0;
		return DomainIsolationProbeResult(allowed, !allowed, false);
	}

	std::shared_ptr<DmaStreamComputeOwnerGuardDecision> SafetyVerifier::EvaluateDmaStreamComputeOwnerGuard(std::shared_ptr<DmaStreamComputeOwnerBinding> descriptorOwnerBinding, std::shared_ptr<DmaStreamComputeOwnerGuardContext> runtimeOwnerContext)
	{
		ArgumentNullException::ThrowIfNull(descriptorOwnerBinding);
		ArgumentNullException::ThrowIfNull(runtimeOwnerContext);

		if (descriptorOwnerBinding->OwnerVirtualThreadId != runtimeOwnerContext->OwnerVirtualThreadId)
		{
			return RejectDmaStreamComputeOwnerGuard(descriptorOwnerBinding, runtimeOwnerContext, RejectKind::OwnerMismatch, L"DmaStreamCompute owner VT mismatch; raw VLIW transport hints are not owner authority.");
		}

		if (descriptorOwnerBinding->OwnerContextId != runtimeOwnerContext->OwnerContextId)
		{
			return RejectDmaStreamComputeOwnerGuard(descriptorOwnerBinding, runtimeOwnerContext, RejectKind::OwnerMismatch, L"DmaStreamCompute owner context mismatch.");
		}

		if (descriptorOwnerBinding->OwnerCoreId != runtimeOwnerContext->OwnerCoreId || descriptorOwnerBinding->OwnerPodId != runtimeOwnerContext->OwnerPodId)
		{
			return RejectDmaStreamComputeOwnerGuard(descriptorOwnerBinding, runtimeOwnerContext, RejectKind::OwnerMismatch, L"DmaStreamCompute owner core/pod mismatch; cross-core reuse is not authorized.");
		}

		if (descriptorOwnerBinding->DeviceId != DmaStreamComputeDescriptor::CanonicalLane6DeviceId || descriptorOwnerBinding->DeviceId != runtimeOwnerContext->DeviceId)
		{
			return RejectDmaStreamComputeOwnerGuard(descriptorOwnerBinding, runtimeOwnerContext, RejectKind::OwnerMismatch, L"DmaStreamCompute DeviceId cannot bypass the canonical lane6 owner/domain guard.");
		}

		if (descriptorOwnerBinding->OwnerDomainTag != runtimeOwnerContext->OwnerDomainTag)
		{
			return RejectDmaStreamComputeOwnerGuard(descriptorOwnerBinding, runtimeOwnerContext, RejectKind::DomainMismatch, L"DmaStreamCompute owner domain tag mismatch.");
		}

		DomainIsolationProbeResult domainProbe = EvaluateDomainIsolationProbe(descriptorOwnerBinding->OwnerDomainTag, runtimeOwnerContext->ActiveDomainCertificate);
		if (!domainProbe.isAllowed())
		{
			return RejectDmaStreamComputeOwnerGuard(descriptorOwnerBinding, runtimeOwnerContext, RejectKind::DomainMismatch, domainProbe.isKernelToUserBlock() ? L"DmaStreamCompute kernel-domain descriptor is not legal in a user-domain guard context." : L"DmaStreamCompute owner domain is not covered by the active domain certificate.");
		}

		return DmaStreamComputeOwnerGuardDecision::Allow(descriptorOwnerBinding, *runtimeOwnerContext, L"DmaStreamCompute owner/domain guard succeeded.");
	}

	std::shared_ptr<DmaStreamComputeOwnerGuardDecision> SafetyVerifier::RejectDmaStreamComputeOwnerGuard(std::shared_ptr<DmaStreamComputeOwnerBinding> descriptorOwnerBinding, std::shared_ptr<DmaStreamComputeOwnerGuardContext> runtimeOwnerContext, RejectKind rejectKind, const std::wstring &message)
	{
		return DmaStreamComputeOwnerGuardDecision::Reject(descriptorOwnerBinding, *runtimeOwnerContext, rejectKind, message);
	}

	InterCoreDomainGuardDecision SafetyVerifier::EvaluateInterCoreDomainGuard(std::shared_ptr<MicroOp> candidate, unsigned long long requestedDomainTag)
	{
		ArgumentNullException::ThrowIfNull(candidate);

		DomainIsolationProbeResult probe = requestedDomainTag == 0 ? DomainIsolationProbeResult(true, false, false) : EvaluateDomainIsolationProbe(candidate, requestedDomainTag);
		LegalityDecision decision = probe.isAllowed() ? LegalityDecision::Allow(LegalityAuthoritySource::GuardPlane, false) : CreateGuardReject(RejectKind::DomainMismatch);
		return InterCoreDomainGuardDecision(decision, probe);
	}

	InterCoreDomainGuardDecision SafetyVerifier::ILegalityChecker_EvaluateInterCoreDomainGuard(std::shared_ptr<MicroOp> candidate, unsigned long long requestedDomainTag)
	{
		return EvaluateInterCoreDomainGuard(candidate, requestedDomainTag);
	}

	LegalityDecision SafetyVerifier::ILegalityChecker_EvaluateSmtBoundaryGuard(PhaseCertificateTemplateKey4Way liveTemplateKey)
	{
		return EvaluateSmtBoundaryGuard(liveTemplateKey);
	}

	LegalityDecision SafetyVerifier::CreateGuardReject(RejectKind rejectKind)
	{
		return LegalityDecision::Reject(rejectKind, CertificateRejectDetail::None, LegalityAuthoritySource::GuardPlane, false);
	}

	std::optional<LegalityDecision> SafetyVerifier::TryResolveInterCoreGuardDecision(int bundleOwnerThreadId, unsigned long long requestedDomainTag, std::shared_ptr<MicroOp> candidate)
	{
		if (requestedDomainTag != 0)
		{
			InterCoreDomainGuardDecision domainGuard = EvaluateInterCoreDomainGuard(candidate, requestedDomainTag);
			if (!domainGuard.isAllowed())
			{
				return domainGuard.getLegalityDecision();
			}
		}

		if (candidate->getScoreboardPending())
		{
			return CreateGuardReject(RejectKind::RareHazard);
		}

		if (bundleOwnerThreadId == candidate->getOwnerThreadId())
		{
			return LegalityDecision::Allow(LegalityAuthoritySource::GuardPlane, false);
		}

		if (!candidate->getAdmissionMetadata()->IsStealable)
		{
			return CreateGuardReject(RejectKind::RareHazard);
		}

		if (candidate->isControlFlow())
		{
			return CreateGuardReject(RejectKind::Ordering);
		}

		return std::nullopt;
	}

	std::optional<LegalityDecision> SafetyVerifier::TryRejectSmtOwnerDomainGuard(SmtBundleMetadata4Way bundleMetadata, std::shared_ptr<MicroOp> candidate)
	{
		if (bundleMetadata.getHasKnownOwnerContext() && candidate->getOwnerContextId() != bundleMetadata.getOwnerContextId())
		{
			return CreateGuardReject(RejectKind::OwnerMismatch);
		}

		if (bundleMetadata.getHasDomainRestriction() && !EvaluateDomainIsolationProbe(candidate, bundleMetadata.getOwnerDomainTag()).isAllowed())
		{
			return CreateGuardReject(RejectKind::DomainMismatch);
		}

		return std::nullopt;
	}

	LegalityDecision SafetyVerifier::EvaluateSmtBoundaryGuard(PhaseCertificateTemplateKey4Way liveTemplateKey)
	{
		if (liveTemplateKey.getBoundaryGuard().getBlocksSmtInjection())
		{
			return LegalityDecision::Reject(RejectKind::Boundary, CertificateRejectDetail::None, LegalityAuthoritySource::GuardPlane, false);
		}

		return LegalityDecision::Allow(LegalityAuthoritySource::GuardPlane, false);
	}

	bool SafetyVerifier::TryClassifyMemoryFootprintInvalidation(std::shared_ptr<MicroOp> writer, std::shared_ptr<MicroOp> replayEvidence, ReplayPhaseInvalidationReason &reason)
	{
		reason = ReplayPhaseInvalidationReason::None;
		if (writer == nullptr || replayEvidence == nullptr)
		{
			return false;
		}

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: IReadOnlyList<(ulong Address, ulong Length)> writerRanges = writer.WriteMemoryRanges ?? Array.Empty<(ulong Address, ulong Length)>();
		const std::vector<MicroOp::MemoryRange> writerRanges = writer->getWriteMemoryRanges();
		if (writerRanges.empty())
		{
			return false;
		}

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: IReadOnlyList<(ulong Address, ulong Length)> evidenceReadRanges = GetSafetyVisibleReadRanges(replayEvidence);
		const std::vector<MicroOp::MemoryRange> evidenceReadRanges = GetSafetyVisibleReadRanges(replayEvidence);
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: IReadOnlyList<(ulong Address, ulong Length)> evidenceWriteRanges = replayEvidence.WriteMemoryRanges ?? Array.Empty<(ulong Address, ulong Length)>();
		const std::vector<MicroOp::MemoryRange> evidenceWriteRanges = replayEvidence->getWriteMemoryRanges();

		if (!HasAnyMemoryRangeOverlap(writerRanges, evidenceReadRanges) && !HasAnyMemoryRangeOverlap(writerRanges, evidenceWriteRanges))
		{
			return false;
		}

		reason = ReplayPhaseInvalidationReason::MemoryFootprintOverlap;
		return true;
	}

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private static bool HasAnyMemoryRangeOverlap(IReadOnlyList<(ulong Address, ulong Length)> leftRanges, IReadOnlyList<(ulong Address, ulong Length)> rightRanges)
	bool SafetyVerifier::HasAnyMemoryRangeOverlap(const std::vector<MicroOp::MemoryRange> &leftRanges, const std::vector<MicroOp::MemoryRange> &rightRanges)
	{
		if (leftRanges.empty() || rightRanges.empty())
		{
			return false;
		}

// C# TO C++ CONVERTER TASK: C++ has no equivalent to C# deconstruction declarations:
		for (const auto& [leftAddress, leftLength] : leftRanges)
		{
			if (leftLength == 0)
			{
				continue;
			}

			unsigned long long leftEnd = SaturatingRangeEnd(leftAddress, leftLength);
// C# TO C++ CONVERTER TASK: C++ has no equivalent to C# deconstruction declarations:
			for (const auto& [rightAddress, rightLength] : rightRanges)
			{
				if (rightLength == 0)
				{
					continue;
				}

				unsigned long long rightEnd = SaturatingRangeEnd(rightAddress, rightLength);
				if (leftAddress < rightEnd && rightAddress < leftEnd)
				{
					return true;
				}
			}
		}

		return false;
	}

	unsigned long long SafetyVerifier::SaturatingRangeEnd(unsigned long long address, unsigned long long length)
	{
		unsigned long long remaining = std::numeric_limits<unsigned long long>::max() - address;
		return length > remaining ? std::numeric_limits<unsigned long long>::max() : address + length;
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private VerificationCondition GenerateRegisterIsolationCondition(IReadOnlyList<MicroOp?> bundle, MicroOp candidate, int bundleOwner, int candidateOwner)
	VerificationCondition SafetyVerifier::GenerateRegisterIsolationCondition(const std::vector<std::shared_ptr<MicroOp>> &bundle, std::shared_ptr<MicroOp> candidate, int bundleOwner, int candidateOwner)
	{
		VerificationCondition vc;
		vc.Type = VerificationConditionType::RegisterIsolation;
		vc.Description = std::format(L"Thread {} register writes don't affect thread {}", candidateOwner, bundleOwner);

		const auto candidateWriteRegisters = candidate->getWriteRegisters();
		std::unordered_set<int> candidateWrites(candidateWriteRegisters.begin(), candidateWriteRegisters.end());
		for (auto op : bundle)
		{
			if (op == nullptr || op->getOwnerThreadId() != bundleOwner)
			{
				continue;
			}

			const auto readRegisters = op->getReadRegisters();
			const auto writeRegisters = op->getWriteRegisters();
			std::unordered_set<int> bundleReads(readRegisters.begin(), readRegisters.end());
			std::unordered_set<int> bundleWrites(writeRegisters.begin(), writeRegisters.end());

			const auto overlaps = [&candidateWrites](const std::unordered_set<int> &other) {
				return std::any_of(candidateWrites.begin(), candidateWrites.end(), [&other](int reg) { return other.contains(reg); });
			};
			if (overlaps(bundleReads) || overlaps(bundleWrites))
			{
				vc.IsValid = false;
				const auto joinRegisters = [](const std::unordered_set<int> &registers) {
					std::wstring joined;
					for (int reg : registers) { if (!joined.empty()) joined += L","; joined += std::to_wstring(reg); }
					return joined;
				};
				vc.CounterExample = std::format(L"Register conflict: candidate writes {} overlaps bundle R{} W{}", joinRegisters(candidateWrites), joinRegisters(bundleReads), joinRegisters(bundleWrites));
				return vc;
			}
		}

		vc.IsValid = true;
		vc.FormalSpec = GenerateSMTRegisterIsolationSpec(candidateWrites, bundle);
		return vc;
	}

	VerificationCondition SafetyVerifier::GenerateMemoryDomainCondition(std::shared_ptr<MicroOp> candidate, int bundleOwner, int candidateOwner)
	{
		VerificationCondition vc;
		vc.Type = VerificationConditionType::MemoryDomainSeparation;
		vc.Description = std::format(L"Thread {} memory accesses respect SIP boundaries", candidateOwner);

		MemoryDomain candidateDomain;
		MemoryDomain bundleDomain;
		const auto &threadDomains = getFormalContext().ThreadMemoryDomains;
		const auto candidateDomainIt = threadDomains.find(candidateOwner);
		const auto bundleDomainIt = threadDomains.find(bundleOwner);
		if (candidateDomainIt == threadDomains.end() || bundleDomainIt == threadDomains.end())
		{
			vc.IsValid = true;
			vc.Description += L" (domains not configured, basic check only)";
			return vc;
		}
		bundleDomain = bundleDomainIt->second;
		candidateDomain = candidateDomainIt->second;

		std::vector<MicroOp::MemoryRange> allRanges;
		const auto readRanges = GetSafetyVisibleReadRanges(candidate);
		allRanges.insert(allRanges.end(), readRanges.begin(), readRanges.end());
		const auto writeRanges = candidate->getWriteMemoryRanges();
		allRanges.insert(allRanges.end(), writeRanges.begin(), writeRanges.end());

		for (const auto &range : allRanges)
		{
			if (!candidateDomain.Contains(range.Address, range.Length))
			{
				vc.IsValid = false;
				vc.CounterExample = std::format(L"Access to 0x{:X}+{} outside thread {} domain", range.Address, range.Length, candidateOwner);
				return vc;
			}

			if (bundleDomain.Overlaps(range.Address, range.Length))
			{
				vc.IsValid = false;
				vc.CounterExample = std::format(L"Memory range 0x{:X}+{} overlaps protected domain", range.Address, range.Length);
				return vc;
			}
		}

		vc.IsValid = true;
		vc.FormalSpec = GenerateSMTMemoryDomainSpec(candidateDomain, bundleDomain, allRanges);
		return vc;
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private VerificationCondition GenerateFSPNonInterferenceCondition(IReadOnlyList<MicroOp?> bundle, int targetSlot, MicroOp candidate)
	VerificationCondition SafetyVerifier::GenerateFSPNonInterferenceCondition(const std::vector<std::shared_ptr<MicroOp>> &bundle, int targetSlot, std::shared_ptr<MicroOp> candidate)
	{
		VerificationCondition vc;
		vc.Type = VerificationConditionType::FSPNonInterference;
		vc.Description = L"FSP injection maintains non-interference";

		if (!candidate->getAdmissionMetadata()->IsStealable)
		{
			vc.IsValid = false;
			vc.CounterExample = L"Candidate is not stealable";
			return vc;
		}

		if (candidate->isControlFlow())
		{
			vc.IsValid = false;
			vc.CounterExample = L"Control flow operations cannot be stolen";
			return vc;
		}

		if (targetSlot < 0 || targetSlot >= 8)
		{
			vc.IsValid = false;
			vc.CounterExample = std::format(L"Invalid target slot: {}", targetSlot);
			return vc;
		}

		vc.IsValid = true;
		return vc;
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private string GenerateSMTRegisterIsolationSpec(HashSet<int> candidateWrites, IReadOnlyList<MicroOp?> bundle)
	std::wstring SafetyVerifier::GenerateSMTRegisterIsolationSpec(std::unordered_set<int> &candidateWrites, const std::vector<std::shared_ptr<MicroOp>> &bundle)
	{
		std::shared_ptr<StringBuilder> sb = std::make_shared<StringBuilder>();
		sb->appendLine(L"; Register isolation constraint");
		sb->appendLine(L"(assert (forall ((r Int))");
		sb->appendLine(L"  (=> (member r candidate_writes)");
		sb->appendLine(L"      (and (not (member r bundle_reads))");
		sb->appendLine(L"           (not (member r bundle_writes))))))");
		return sb->toString();
	}

	std::wstring SafetyVerifier::GenerateSMTMemoryDomainSpec(MemoryDomain candidateDomain, MemoryDomain bundleDomain, std::vector<MicroOp::MemoryRange> &accessRanges)
	{
		std::shared_ptr<StringBuilder> sb = std::make_shared<StringBuilder>();
		sb->appendLine(L"; Memory domain separation constraint");
		sb->appendLine(L"(declare-const candidate_domain_base (_ BitVec 64))");
		sb->appendLine(L"(declare-const candidate_domain_size (_ BitVec 64))");
		sb->appendLine(L"(declare-const bundle_domain_base (_ BitVec 64))");
		sb->appendLine(L"(declare-const bundle_domain_size (_ BitVec 64))");
		sb->appendLine(std::format("(assert (= candidate_domain_base #x{:016X}))", candidateDomain.BaseAddress));
		sb->appendLine(std::format("(assert (= candidate_domain_size #x{:016X}))", candidateDomain.Size));
		sb->appendLine(std::format("(assert (= bundle_domain_base #x{:016X}))", bundleDomain.BaseAddress));
		sb->appendLine(std::format("(assert (= bundle_domain_size #x{:016X}))", bundleDomain.Size));
		sb->appendLine(L"; Domains do not overlap");
		sb->appendLine(L"(assert (or");
		sb->appendLine(L"  (bvuge candidate_domain_base (bvadd bundle_domain_base bundle_domain_size))");
		sb->appendLine(L"  (bvuge bundle_domain_base (bvadd candidate_domain_base candidate_domain_size))))");
		return sb->toString();
	}

	void SafetyVerifier::LogVerificationConditions(std::vector<VerificationCondition> &conditions)
	{
		FormalContext.GeneratedConditions.insert(FormalContext.GeneratedConditions.end(), conditions.begin(), conditions.end());
	}

	void SafetyVerifier::SetCurrentCycle(long long cycle)
	{
		_currentCycle = cycle;
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: public BundleResourceProof GenerateProof(IReadOnlyList<MicroOp?> bundle, SecurityContext context)
	BundleResourceProof SafetyVerifier::GenerateProof(const std::vector<std::shared_ptr<MicroOp>> &bundle, SecurityContext context)
	{
		if (bundle.empty() || bundle.size() != 8)
		{
			BundleResourceProof temp_var = BundleResourceProof();
			temp_var.IsValid = false;
			temp_var.VerificationStatus = L"Invalid bundle (null or wrong size)";
			return temp_var;
		}

		try
		{
			VerifyMemoryIsolation(bundle, context);
		}
		catch (const std::runtime_error &ex)
		{
			BundleResourceProof temp_var2 = BundleResourceProof();
			temp_var2.IsValid = false;
			const std::string errorMessage(ex.what());
			temp_var2.VerificationStatus = L"Memory isolation violation: " + std::wstring(errorMessage.begin(), errorMessage.end());
			return temp_var2;
		}

		if (!VerifyBundle(bundle))
		{
			BundleResourceProof temp_var3 = BundleResourceProof();
			temp_var3.IsValid = false;
			temp_var3.VerificationStatus = L"Register hazard or isolation violation detected";
			return temp_var3;
		}

		unsigned int bundleHash = BundleResourceProof::CalculateBundleHash(bundle);

		unsigned int threadMask = 0;
		for (auto op : bundle)
		{
			if (op != nullptr && op->getOwnerThreadId() >= 0 && op->getOwnerThreadId() < 32)
			{
				threadMask |= static_cast<unsigned int>(1 << op->getOwnerThreadId());
			}
		}

		BundleResourceProof proof;
		proof.Cycle = _currentCycle;
		proof.BundleHash = bundleHash;
		proof.AllowedMemoryRangeStart = context.MinAddr;
		proof.AllowedMemoryRangeEnd = context.MaxAddr;
		proof.ThreadMask = threadMask;
		proof.Timestamp = std::chrono::system_clock::now();
		proof.IsValid = true;
		proof.VerificationStatus = L"All invariants verified";

		proof = SignProof(proof);
		return proof;
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private void VerifyMemoryIsolation(IReadOnlyList<MicroOp?> bundle, SecurityContext context)
	void SafetyVerifier::VerifyMemoryIsolation(const std::vector<std::shared_ptr<MicroOp>> &bundle, SecurityContext context)
	{
		for (auto op : bundle)
		{
			if (op == nullptr)
			{
				continue;
			}

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: IReadOnlyList<(ulong Address, ulong Length)> readRanges = GetSafetyVisibleReadRanges(op);
			const std::vector<MicroOp::MemoryRange> readRanges = GetSafetyVisibleReadRanges(op);
			if (readRanges.size() != 0)
			{
				for (const auto &range : readRanges)
				{
					if (range.Address < context.MinAddr || (range.Address + range.Length) > context.MaxAddr)
					{
						throw InvalidOperationException(std::format("Memory read at 0x{0:0>X} violates bounds ", range.Address) + std::format("[0x{0:0>X}, 0x{1:0>X})", context.MinAddr, context.MaxAddr));
					}
				}
			}

			const auto writeRanges = op->getWriteMemoryRanges();
			if (!writeRanges.empty())
			{
				for (const auto &range : writeRanges)
				{
					if (range.Address < context.MinAddr || (range.Address + range.Length) > context.MaxAddr)
					{
						throw InvalidOperationException(std::format("Memory write at 0x{0:0>X} violates bounds ", range.Address) + std::format("[0x{0:0>X}, 0x{1:0>X})", context.MinAddr, context.MaxAddr));
					}
				}
			}
		}
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: public bool VerifyInjectionWithProof(IReadOnlyList<MicroOp?> bundle, int targetSlot, MicroOp candidate, int bundleOwnerThreadId, int candidateOwnerThreadId, out VerificationCondition[] conditions)
	bool SafetyVerifier::VerifyInjectionWithProof(const std::vector<std::shared_ptr<MicroOp>> &bundle, int targetSlot, std::shared_ptr<MicroOp> candidate, int bundleOwnerThreadId, int candidateOwnerThreadId, std::vector<VerificationCondition> &conditions)
	{
		std::vector<VerificationCondition> conditionList;

		// Generate verification condition for register isolation
		conditionList.push_back(GenerateRegisterIsolationCondition(bundle, candidate, bundleOwnerThreadId, candidateOwnerThreadId));

		// Generate verification condition for memory domain separation
		conditionList.push_back(GenerateMemoryDomainCondition(candidate, bundleOwnerThreadId, candidateOwnerThreadId));

		// Generate verification condition for FSP non-interference
		conditionList.push_back(GenerateFSPNonInterferenceCondition(bundle, targetSlot, candidate));

		conditions = std::move(conditionList);

		// Check all conditions
		const bool allValid = std::all_of(conditions.begin(), conditions.end(), [](const VerificationCondition &condition) { return condition.IsValid; });

		// Log to verification database
		if (getFormalContext().EnableFormalChecks)
		{
			LogVerificationConditions(conditions);
		}

		return allValid;
	}

#if defined(TESTING)
	SafetyVerifier::ResearchVirtualizationOperationAdmissionCertificate::ResearchVirtualizationOperationAdmissionCertificate(std::any verifierSeal, std::any ownerSeal, unsigned long long ownerPolicyGeneration, std::any contextSeal, unsigned long long contextGeneration, unsigned long long verifierGeneration, ResearchVirtualizationProbeOperation operation, std::shared_ptr<ResearchVirtualizationProbeIdentity> identity, std::shared_ptr<VirtualizationAdmissionCertificate> e1Carrier) : _verifierSeal(verifierSeal)
	{
		OwnerSeal = ownerSeal;
		OwnerPolicyGeneration = ownerPolicyGeneration;
		ContextSeal = contextSeal;
		ContextGeneration = contextGeneration;
		VerifierGeneration = verifierGeneration;
		Operation = operation;
		Identity = identity;
		E1Carrier = e1Carrier;
	}

	std::any SafetyVerifier::ResearchVirtualizationOperationAdmissionCertificate::getOwnerSeal() const
	{
		return OwnerSeal;
	}

	unsigned long long SafetyVerifier::ResearchVirtualizationOperationAdmissionCertificate::getOwnerPolicyGeneration() const
	{
		return OwnerPolicyGeneration;
	}

	std::any SafetyVerifier::ResearchVirtualizationOperationAdmissionCertificate::getContextSeal() const
	{
		return ContextSeal;
	}

	unsigned long long SafetyVerifier::ResearchVirtualizationOperationAdmissionCertificate::getContextGeneration() const
	{
		return ContextGeneration;
	}

	unsigned long long SafetyVerifier::ResearchVirtualizationOperationAdmissionCertificate::getVerifierGeneration() const
	{
		return VerifierGeneration;
	}

	ResearchVirtualizationProbeOperation SafetyVerifier::ResearchVirtualizationOperationAdmissionCertificate::getOperation() const
	{
		return Operation;
	}

	std::shared_ptr<ResearchVirtualizationProbeIdentity> SafetyVerifier::ResearchVirtualizationOperationAdmissionCertificate::getIdentity() const
	{
		return Identity;
	}

	std::shared_ptr<VirtualizationAdmissionCertificate> SafetyVerifier::ResearchVirtualizationOperationAdmissionCertificate::getE1Carrier() const
	{
		return E1Carrier;
	}

	bool SafetyVerifier::ResearchVirtualizationOperationAdmissionCertificate::getHasNumericLeaf() const
	{
		return false;
	}

	bool SafetyVerifier::ResearchVirtualizationOperationAdmissionCertificate::getCompletionPublicationAuthorized() const
	{
		return false;
	}

	bool SafetyVerifier::ResearchVirtualizationOperationAdmissionCertificate::getRetirePublicationAuthorized() const
	{
		return false;
	}

	bool SafetyVerifier::ResearchVirtualizationOperationAdmissionCertificate::WasIssuedBy(std::any verifierSeal)
	{
// C# TO C++ CONVERTER TASK: Cannot test for object identity between 2 non-pointer objects in C++:
		return this->ReferenceEquals(_verifierSeal, verifierSeal);
	}

	std::shared_ptr<ResearchVirtualizationProbeAdmissionResult> SafetyVerifier::IssueResearchVirtualizationOperationAdmission(std::shared_ptr<ResearchVirtualizationRuntimeOwner::PolicySnapshot> policy, std::shared_ptr<ResearchVirtualizationOperationContext> context, std::shared_ptr<ResearchVirtualizationOperationContext::IdentitySnapshot> identitySnapshot, ReplayPhaseContext replayPhase, SmtBundleMetadata4Way bundle, std::shared_ptr<VmxMicroOp> carrier, int sourceSlotId, int workingSlotId, std::shared_ptr<VirtualizationAdmissionCertificate> e1)
	{
		ArgumentNullException::ThrowIfNull(policy);
		ArgumentNullException::ThrowIfNull(context);
		ArgumentNullException::ThrowIfNull(identitySnapshot);
		ArgumentNullException::ThrowIfNull(carrier);
		ArgumentNullException::ThrowIfNull(e1);

		if (policy->getOperation() != ResearchVirtualizationProbeOperation::StateMinimalLiveness)
		{
			return DenyResearchVirtualizationAdmission(ResearchVirtualizationProbeAdmissionDecision::DeniedUnsupportedOperation, L"The research policy does not name the state-minimal probe.");
		}

		if (!context->IsLive(identitySnapshot))
		{
			return DenyResearchVirtualizationAdmission(ResearchVirtualizationProbeAdmissionDecision::DeniedRuntimeIdentitySnapshot, L"The operation identity snapshot is foreign or invalidated.");
		}

		std::shared_ptr<ResearchVirtualizationProbeIdentity> identity = identitySnapshot->getIdentity();
		if (!identity->IsComplete)
		{
			return DenyResearchVirtualizationAdmission(ResearchVirtualizationProbeAdmissionDecision::DeniedIncompleteIdentity, L"Every E2 research identity must be materialized.");
		}

		std::shared_ptr<VirtualizationAdmissionValidationResult> e1Validation = ValidateVirtualizationAdmission(replayPhase, bundle, carrier, sourceSlotId, workingSlotId, e1);
		if (!e1Validation->IsValidForFaultOnlyTransport)
		{
			return DenyResearchVirtualizationAdmission(ResearchVirtualizationProbeAdmissionDecision::DeniedE1Carrier, e1Validation->Reason);
		}

		if (identity->VirtualThreadId != e1->getVirtualThreadId() || identity->OwnerContextId != e1->getOwnerContextId() || identity->DomainTag != e1->getDomainTag() || identity->CarrierAttemptId != e1->getAttemptId() || identity->ReplayEpoch != e1->getReplayEpoch())
		{
			return DenyResearchVirtualizationAdmission(ResearchVirtualizationProbeAdmissionDecision::DeniedCarrierIdentityMismatch, L"The research identities do not match the live E1 carrier attempt.");
		}

		return new(ResearchVirtualizationProbeAdmissionDecision::IssuedForResearchExecution, std::make_shared<ResearchVirtualizationOperationAdmissionCertificate>(_virtualizationAdmissionIssuerSeal, policy->getOwnerSeal(), policy->getPolicyGeneration(), identitySnapshot->getContextSeal(), identitySnapshot->getContextGeneration(), e1->getIssuerGeneration(), policy->getOperation(), identity, e1), L"SafetyVerifier issued one TESTING-only operation certificate over a live E1 carrier.");
	}

	bool SafetyVerifier::IsResearchVirtualizationOperationAdmissionLive(std::shared_ptr<ResearchVirtualizationOperationAdmissionCertificate> certificate)
	{
		ArgumentNullException::ThrowIfNull(certificate);

		if (!certificate->WasIssuedBy(_virtualizationAdmissionIssuerSeal) || certificate->getVerifierGeneration() != _virtualizationAdmissionIssuerGeneration)
		{
			return false;
		}

		std::optional<std::shared_ptr<LiveVirtualizationAdmission>> live;
		return _liveVirtualizationAdmissions->TryGetValue(certificate->getE1Carrier(), live) && live.AttemptId == certificate->getIdentity()->CarrierAttemptId && live.IssuerGeneration == _virtualizationAdmissionIssuerGeneration;
	}

	std::shared_ptr<ResearchVirtualizationProbeAdmissionResult> SafetyVerifier::DenyResearchVirtualizationAdmission(ResearchVirtualizationProbeAdmissionDecision decision, const std::wstring &reason)
	{
		return new(decision, nullptr, reason);
	}

#endif

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: public BundleResourceCertificate CreateCertificate(IReadOnlyList<MicroOp?> bundle, int ownerThreadId, ulong cycleNumber)
	std::shared_ptr<BundleResourceCertificate> SafetyVerifier::CreateCertificate(const std::vector<std::shared_ptr<MicroOp>> &bundle, int ownerThreadId, unsigned long long cycleNumber)
	{
		return BundleResourceCertificate::Create(bundle, ownerThreadId, cycleNumber);
	}

	bool SafetyVerifier::VerifyInjectionWithCertificate(std::shared_ptr<BundleResourceCertificate> certificate, std::shared_ptr<MicroOp> candidate)
	{
		if (certificate == nullptr)
		{
			throw std::invalid_argument("certificate");
		}

		if (candidate == nullptr)
		{
			throw std::invalid_argument("candidate");
		}

		// Use certificate's built-in conflict check
		return certificate->CanInject(candidate);
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: internal LegalityDecision EvaluateInterCoreLegality(IReadOnlyList<MicroOp?> bundle, BundleResourceCertificate bundleCertificate, PhaseCertificateTemplateKey liveTemplateKey, PhaseCertificateTemplate phaseTemplate, int bundleOwnerThreadId, ulong requestedDomainTag, MicroOp candidate, SafetyMask128 globalHardwareMask = default)
	LegalityDecision SafetyVerifier::EvaluateInterCoreLegality(const std::vector<std::shared_ptr<MicroOp>> &bundle, std::shared_ptr<BundleResourceCertificate> bundleCertificate, PhaseCertificateTemplateKey liveTemplateKey, PhaseCertificateTemplate phaseTemplate, int bundleOwnerThreadId, unsigned long long requestedDomainTag, std::shared_ptr<MicroOp> candidate, SafetyMask128 globalHardwareMask)
	{
		ArgumentNullException::ThrowIfNull(bundle);
		ArgumentNullException::ThrowIfNull(bundleCertificate);
		ArgumentNullException::ThrowIfNull(candidate);

		if (bundle.size() != 8)
		{
// C# TO C++ CONVERTER TASK: This exception's constructor requires only one argument:
// ORIGINAL LINE: throw new ArgumentException("Bundle must contain exactly 8 slots.", nameof(bundle));
			throw std::invalid_argument("Bundle must contain exactly 8 slots.");
		}

		if (const auto guardDecision = TryResolveInterCoreGuardDecision(bundleOwnerThreadId, requestedDomainTag, candidate))
		{
			return *guardDecision;
		}

		SafetyMask128 candidateSharedMask = candidate->getAdmissionMetadata()->SharedStructuralMask;
		bool attemptedReplayReuse = liveTemplateKey.getPhaseKey().isValid() && candidateSharedMask.isNonZero();
		if (attemptedReplayReuse)
		{
			if (globalHardwareMask.ConflictsWith(candidateSharedMask))
			{
				return LegalityDecision::Reject(RejectKind::CrossLaneConflict, CertificateRejectDetail::SharedResourceConflict, LegalityAuthoritySource::GuardPlane, attemptedReplayReuse);
			}

			if (phaseTemplate.Matches(liveTemplateKey))
			{
	// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
	// ORIGINAL LINE: return EvaluateInterCoreCertificate( phaseTemplate.Certificate!, candidate, LegalityAuthoritySource.ReplayPhaseCertificate, attemptedReplayReuse);
				return EvaluateInterCoreCertificate(phaseTemplate.getCertificate(), candidate, LegalityAuthoritySource::ReplayPhaseCertificate, attemptedReplayReuse);
			}

			return EvaluateInterCoreCertificate(bundleCertificate, candidate, LegalityAuthoritySource::StructuralCertificate, attemptedReplayReuse);
		}

		return EvaluateInterCoreCompatibility(bundle, candidate, bundleOwnerThreadId, globalHardwareMask);
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: LegalityDecision ILegalityChecker.EvaluateInterCoreLegality(IReadOnlyList<MicroOp?> bundle, BundleResourceCertificate bundleCertificate, PhaseCertificateTemplateKey liveTemplateKey, PhaseCertificateTemplate phaseTemplate, int bundleOwnerThreadId, ulong requestedDomainTag, MicroOp candidate, SafetyMask128 globalHardwareMask)
	LegalityDecision SafetyVerifier::ILegalityChecker_EvaluateInterCoreLegality(const std::vector<std::shared_ptr<MicroOp>> &bundle, std::shared_ptr<BundleResourceCertificate> bundleCertificate, PhaseCertificateTemplateKey liveTemplateKey, PhaseCertificateTemplate phaseTemplate, int bundleOwnerThreadId, unsigned long long requestedDomainTag, std::shared_ptr<MicroOp> candidate, SafetyMask128 globalHardwareMask)
	{
		return EvaluateInterCoreLegality(bundle, bundleCertificate, liveTemplateKey, phaseTemplate, bundleOwnerThreadId, requestedDomainTag, candidate, globalHardwareMask);
	}

	Guid SafetyVerifier::getSecureRegisterLocalScope() const
	{
		return SecureRegisterLocalScope;
	}

	std::shared_ptr<SecureComputeNoEffectCompletionOwner> SafetyVerifier::getSecureNoEffectCompletion()
	{
		{
		std::scoped_lock<std::recursive_mutex> lock(secureRegisterLocalGate);
			return secureNoEffectCompletion = (secureNoEffectCompletion != nullptr) ? secureNoEffectCompletion : std::make_shared<SecureComputeNoEffectCompletionOwner>(shared_from_this(), secureRegisterLocalGate);
		}
	}

	bool SafetyVerifier::IsSecureNoEffectOwner(std::shared_ptr<SecureComputeNoEffectCompletionOwner> owner)
	{
		return secureNoEffectCompletion == owner;
	}

	bool SafetyVerifier::HasConsumedSecureMachinePermit(std::shared_ptr<SecureComputePreparedMachineAdmission> prepared)
	{
		{
			std::scoped_lock<std::recursive_mutex> lock(secureRegisterLocalGate);
			return prepared->getCertificate() == prepared->getPermit()->getCertificate() && secureIssuePermits != nullptr && secureIssuePermits->HasConsumed(prepared->getPermit());
		}
	}

	std::optional<std::shared_ptr<ExternalSecureExecutionAttempt>> SafetyVerifier::CreateSecureBackendAttempt(std::shared_ptr<SecureComputePreparedMachineAdmission> prepared)
	{
		{
			std::scoped_lock<std::recursive_mutex> lock(secureRegisterLocalGate);
			if (!IsCurrentSecureComputeCertificate(prepared->getCertificate()) || secureBackendAttempts.contains(prepared->getCertificate()))
			{
				return std::nullopt;
			}
			auto scheduled = prepared->getCertificate()->getPlaced()->getScheduledOperation();
			auto attempt = std::make_shared<ExternalSecureExecutionAttempt>(prepared->getCertificate()->getReservation(), getSecureRegisterLocalScope(), prepared->getCertificate()->getGeneration(), scheduled->getOperationId()->OperationAttempt, scheduled->getOperationId()->WorkingBundleSequence, scheduled->getOperationId()->VirtualThreadId, scheduled->getAdmission()->OwnerContextId, scheduled->getAdmission()->SourceProvenance->SourceSlotIndex, scheduled->getOperationId()->WorkingSlotIndex, scheduled->getPhysicalLane());
			secureBackendAttempts.emplace(prepared->getCertificate(), attempt);
			return attempt;
		}
	}

	bool SafetyVerifier::IsSecureBackendAttempt(std::shared_ptr<SecureComputePreparedMachineAdmission> prepared, std::shared_ptr<ExternalSecureExecutionAttempt> attempt)
	{
		{
			std::scoped_lock<std::recursive_mutex> lock(secureRegisterLocalGate);
			const auto original = secureBackendAttempts.find(prepared->getCertificate());
			return original != secureBackendAttempts.end() && original->second == attempt;
		}
	}

	std::shared_ptr<SecureComputeCanonicalAdmissionResult> SafetyVerifier::RunSecureNoEffect(std::shared_ptr<SecureComputePreparedMachineAdmission> prepared, std::shared_ptr<ExternalSecureExecutionAttempt> attempt, std::shared_ptr<ExternalSecureExecutionConsumptionResult> consumption, std::shared_ptr<SecureComputeMachinePermitConsumption> machineFact, std::shared_ptr<IExternalSecureNoEffectExecutionProvider> backend)
	{
		auto owner = getSecureNoEffectCompletion();
	// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
	// ORIGINAL LINE: var pending = owner.Begin(prepared, attempt, consumption.Receipt!, backend);
		auto pending = owner->Begin(prepared, attempt, consumption->Receipt, backend);
		auto request = attempt->Request;
		if (pending == nullptr)
		{
			return std::make_shared<SecureComputeCanonicalAdmissionResult>(request, SecureComputeCanonicalAdmissionDecision::DeniedNoEffectCompletion, prepared->getResult()->getCanonical(), prepared->getCertificate()->getPlaced(), machineFact, attempt, consumption);
		}
		auto submission = pending->getSubmission();
		struct PendingCancellation final
		{
			std::shared_ptr<SecureComputeNoEffectCompletionOwner> Owner;
			std::shared_ptr<SecureComputeNoEffectCompletionOwner::Pending> Pending;
			bool Active = true;
			void Cancel() { if (Active && Owner && Pending) Owner->Cancel(Pending); Active = false; }
			~PendingCancellation() { Cancel(); }
		} cancellation{owner, pending};
		auto Result = [&](SecureComputeCanonicalAdmissionDecision decision,
			std::optional<std::shared_ptr<ExternalSecureNoEffectExecutionResult>> outcome,
			std::shared_ptr<SecureComputeNoEffectRetirement> retirement) {
			return std::make_shared<SecureComputeCanonicalAdmissionResult>(request, decision,
				prepared->getResult()->getCanonical(), prepared->getCertificate()->getPlaced(), machineFact,
				attempt, consumption, submission, std::move(outcome), std::move(retirement));
		};
		try
		{
			std::optional<std::shared_ptr<ExternalSecureNoEffectExecutionResult>> outcome;
			try
			{
				outcome = backend->SubmitNoEffect(submission);
			}
			catch (...)
			{
				outcome = std::nullopt;
			}
// C# TO C++ CONVERTER TASK: The following 'is' operator pattern is not converted by C# to C++ Converter:
			if (!outcome || !*outcome || (*outcome)->getSubmission() != submission ||
				((*outcome)->getStatus() != ExternalSecureDomainProviderStatus::Receipt &&
				 (*outcome)->getStatus() != ExternalSecureDomainProviderStatus::Stale &&
				 (*outcome)->getStatus() != ExternalSecureDomainProviderStatus::Rejected))
			{
				return Result(SecureComputeCanonicalAdmissionDecision::DeniedBackendCleanupRequired, nullptr, nullptr);
			}
			if ((*outcome)->getStatus() != ExternalSecureDomainProviderStatus::Receipt)
			{
				return Result(SecureComputeCanonicalAdmissionDecision::DeniedNoEffectCompletion, outcome, nullptr);
			}
			auto original = owner->ObserveOriginal(pending);
// C# TO C++ CONVERTER TASK: Cannot test for object identity between 2 non-pointer objects in C++:
			if (!original || *original != *outcome)
			{
				return Result(SecureComputeCanonicalAdmissionDecision::DeniedBackendCleanupRequired, nullptr, nullptr);
			}
	// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
	// ORIGINAL LINE: var accepted = owner.Accept(pending, outcome.Receipt!);
			auto accepted = owner->Accept(pending, (*outcome)->getReceipt());
			auto retired = accepted == nullptr ? nullptr : owner->Retire(pending, accepted);
			return Result(retired == nullptr ? SecureComputeCanonicalAdmissionDecision::DeniedNoEffectCompletion : SecureComputeCanonicalAdmissionDecision::NoEffectRetired, outcome, retired);
		}
		catch (...)
		{
			cancellation.Cancel();
			throw;
		}
	}

	std::shared_ptr<SecureComputeIssuePermitOwner> SafetyVerifier::getSecureIssuePermits()
	{
		return secureIssuePermits = (secureIssuePermits != nullptr) ? secureIssuePermits : std::make_shared<SecureComputeIssuePermitOwner>(shared_from_this(), secureRegisterLocalGate);
	}

	std::shared_ptr<SecureComputeCanonicalAdmissionResult> SafetyVerifier::AdmitSecureRegisterLocal(std::shared_ptr<ExternalSecureRegisterLocalRequest> request, std::shared_ptr<ExternalSecureExecutionReservation> reservation, std::shared_ptr<IExternalSecureExecutionConsumptionProvider> backend)
	{
		return RunSecureRegisterLocal(request, reservation, backend, false);
	}

	std::shared_ptr<SecureComputeCanonicalAdmissionResult> SafetyVerifier::ExecuteSecureRegisterLocal(std::shared_ptr<ExternalSecureRegisterLocalRequest> request, std::shared_ptr<ExternalSecureExecutionReservation> reservation, std::shared_ptr<IExternalSecureExecutionConsumptionProvider> backend)
	{
		return RunSecureRegisterLocal(request, reservation, backend, true);
	}

	std::shared_ptr<SecureComputeCanonicalAdmissionResult> SafetyVerifier::RunSecureRegisterLocal(std::shared_ptr<ExternalSecureRegisterLocalRequest> request, std::shared_ptr<ExternalSecureExecutionReservation> reservation, std::shared_ptr<IExternalSecureExecutionConsumptionProvider> backend, bool execute)
	{
		ArgumentNullException::ThrowIfNull(request);
		ArgumentNullException::ThrowIfNull(reservation);
		ArgumentNullException::ThrowIfNull(backend);
		if (execute && std::dynamic_pointer_cast<IExternalSecureRegisterLocalExecutionProvider>(backend) == nullptr)
		{
			return SecureComputeCanonicalAdmissionResult::DeniedLocalBinding(request);
		}
		auto prepared = PrepareSecureRegisterLocalMachineAdmission(request, reservation);
		if (prepared == nullptr)
		{
			return SecureComputeCanonicalAdmissionResult::DeniedLocalBinding(request);
		}
		return RunPreparedSecureRegisterLocal(request, reservation, backend, execute, prepared);
	}

	std::shared_ptr<SecureComputeCanonicalAdmissionResult> SafetyVerifier::RunPreparedSecureRegisterLocal(std::shared_ptr<ExternalSecureRegisterLocalRequest> request, std::shared_ptr<ExternalSecureExecutionReservation> reservation, std::shared_ptr<IExternalSecureExecutionConsumptionProvider> backend, bool execute, std::shared_ptr<SecureComputePreparedMachineAdmission> prepared)
	{
		auto placed = prepared->getCertificate()->getPlaced();
		auto attemptResult = CreateSecureBackendAttempt(prepared);
		if (!attemptResult || !*attemptResult)
		{
			CompleteSecureRegisterLocalMachineAdmission(prepared);
			return SecureComputeCanonicalAdmissionResult::DeniedLocalBinding(request);
		}
		auto attempt = *attemptResult;
		const auto machineAdmissionCleanup = std::shared_ptr<void>(nullptr, [this, prepared](void *) {
			CompleteSecureRegisterLocalMachineAdmission(prepared);
		});
		std::optional<std::shared_ptr<ExternalSecureExecutionConsumptionResult>> outcome;
			try
			{
				outcome = backend->Consume(attempt);
			}
			catch (...)
			{
				outcome = std::nullopt;
			}
// C# TO C++ CONVERTER TASK: The following 'is' operator pattern is not converted by C# to C++ Converter:
			if (!outcome || !*outcome || (*outcome)->Attempt != attempt ||
				((*outcome)->Status != ExternalSecureDomainProviderStatus::Receipt &&
				 (*outcome)->Status != ExternalSecureDomainProviderStatus::Rejected &&
				 (*outcome)->Status != ExternalSecureDomainProviderStatus::Stale))
			{
				return std::make_shared<SecureComputeCanonicalAdmissionResult>(request, SecureComputeCanonicalAdmissionDecision::DeniedBackendCleanupRequired, prepared->getResult()->getCanonical(), placed, nullptr, attempt);
			}
			if ((*outcome)->Status != ExternalSecureDomainProviderStatus::Receipt)
			{
				return std::make_shared<SecureComputeCanonicalAdmissionResult>(request, SecureComputeCanonicalAdmissionDecision::DeniedLocalBinding, prepared->getResult()->getCanonical(), placed, nullptr, attempt, outcome);
			}
			// Consume's returned envelope is not an original-source observation.
			// Resolve the exact retained provider outcome before spending the
			// one-shot machine permit. Query is outside all verifier/session
			// owner locks and may revoke this certificate; Consume below must
			// still revalidate it after this second provider callback.
			std::optional<std::shared_ptr<ExternalSecureExecutionConsumptionResult>> original;
			try
			{
				original = backend->QueryConsumption(attempt);
			}
			catch (...)
			{
				original = std::nullopt;
			}
// C# TO C++ CONVERTER TASK: Cannot test for object identity between 2 non-pointer objects in C++:
			if (!original || !*original || *original != *outcome)
			{
				return std::make_shared<SecureComputeCanonicalAdmissionResult>(request, SecureComputeCanonicalAdmissionDecision::DeniedBackendCleanupRequired, prepared->getResult()->getCanonical(), placed, nullptr, attempt);
			}
			auto consumed = ConsumeSecureRegisterLocalMachinePermit(prepared, request, placed, reservation);
			std::shared_ptr<IExternalSecureRegisterLocalExecutionProvider> arithmetic = std::dynamic_pointer_cast<IExternalSecureRegisterLocalExecutionProvider>(backend);
			if (execute && consumed != nullptr && arithmetic != nullptr)
			{
				return RunSecureArithmetic(prepared, attempt, *outcome, consumed, arithmetic);
			}
			std::shared_ptr<IExternalSecureNoEffectExecutionProvider> noEffect = std::dynamic_pointer_cast<IExternalSecureNoEffectExecutionProvider>(backend);
			if (consumed != nullptr && noEffect != nullptr)
			{
				return RunSecureNoEffect(prepared, attempt, *outcome, consumed, noEffect);
			}
		return std::make_shared<SecureComputeCanonicalAdmissionResult>(request, consumed == nullptr ? SecureComputeCanonicalAdmissionDecision::DeniedLocalBinding : SecureComputeCanonicalAdmissionDecision::DeniedExecutionTailUnavailable, prepared->getResult()->getCanonical(), placed, consumed, attempt, outcome);
	}

	std::shared_ptr<SecureComputeCanonicalAdmissionResult> SafetyVerifier::AdmitSecureRegisterLocal(std::shared_ptr<ExternalSecureRegisterLocalRequest> request, std::shared_ptr<ExternalSecureExecutionReservation> reservation)
	{
		ArgumentNullException::ThrowIfNull(request);
		ArgumentNullException::ThrowIfNull(reservation);
		if (reservation->Request != request)
		{
			return SecureComputeCanonicalAdmissionResult::DeniedLocalBinding(request);
		}
		{
			std::scoped_lock<std::recursive_mutex> lock(secureRegisterLocalGate);
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: SecureComputePreparedMachineAdmission? prepared = PrepareSecureRegisterLocalMachineAdmission(request, reservation);
			std::shared_ptr<SecureComputePreparedMachineAdmission> prepared = PrepareSecureRegisterLocalMachineAdmission(request, reservation);
			if (prepared == nullptr)
			{
				return std::make_shared<SecureComputeCanonicalAdmissionResult>(request, SecureComputeCanonicalAdmissionDecision::DeniedSecurePermitUnavailable, nullptr);
			}
			const auto cleanup = std::shared_ptr<void>(nullptr, [this, prepared](void *) { CompleteSecureRegisterLocalMachineAdmission(prepared); });
			auto consumed = ConsumeSecureRegisterLocalMachinePermit(prepared, request, prepared->getCertificate()->getPlaced(), reservation);
			return std::make_shared<SecureComputeCanonicalAdmissionResult>(request, consumed == nullptr ? SecureComputeCanonicalAdmissionDecision::DeniedSecurePermitUnavailable : SecureComputeCanonicalAdmissionDecision::DeniedBackendEntitlementUnavailable, prepared->getResult()->getCanonical(), prepared->getCertificate()->getPlaced(), consumed);
		}
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal SecureComputePreparedMachineAdmission? PrepareSecureRegisterLocalMachineAdmission(ExternalSecureRegisterLocalRequest request, ExternalSecureExecutionReservation reservation)
	std::shared_ptr<SecureComputePreparedMachineAdmission> SafetyVerifier::PrepareSecureRegisterLocalMachineAdmission(std::shared_ptr<ExternalSecureRegisterLocalRequest> request, std::shared_ptr<ExternalSecureExecutionReservation> reservation)
	{
		{
			std::scoped_lock<std::recursive_mutex> lock(secureRegisterLocalGate);
			// A bounded live inventory, independent from session operation history.
			const bool domainRevoked = std::any_of(revokedSecureCertificateDomains.begin(), revokedSecureCertificateDomains.end(), [&request](const auto &domain) { return domain && domain->Equals(request->Binding.SecureDomain); });
			if (secureCertificateScopeExhausted || secureCertificates.size() + memoryReadCertificates.size() >= 8 || domainRevoked || reservation->Request != request)
			{
				return nullptr;
			}
			auto result = AdmitSecureRegisterLocal(request);
// C# TO C++ CONVERTER TASK: The following 'is' operator pattern is not converted by C# to C++ Converter:
			auto placed = result->getPlacedIntent();
			if (placed == nullptr || !VerifyFinalSecureRegisterLocalPlacement(placed))
			{
				return nullptr;
			}
			auto certificate = std::make_shared<SecureComputeMachineCertificate>(placed, reservation, secureCertificateGeneration);
			secureCertificates.emplace(certificate, false); // actual verifier issuance registration
			auto permit = getSecureIssuePermits()->Register(certificate);
			if (permit == nullptr)
			{
				secureCertificates.erase(certificate);
				return nullptr;
			}
			return std::make_shared<SecureComputePreparedMachineAdmission>(result, certificate, permit);
		}
	}

	bool SafetyVerifier::VerifyFinalSecureRegisterLocalPlacement(std::shared_ptr<SecureComputePlacedIntent> placed)
	{
		auto scheduled = placed->getScheduledOperation();
		auto admission = scheduled->getAdmission();
		auto contract = admission->Contract;
		auto issued = placed->getIssuedAttempt();
		if (issued == nullptr || !issued->isCurrent() || issued->getScheduledOperation() != scheduled || issued->getExecutionRecord()->getState() != Execution::ExecutionRecordState::Issued || contract == nullptr || contract->getGeneratedBinding()->Opcode != Processor::CPU_Core::IsaOpcodeValues::ADD || CanonicalSecurityOperationIdentity::FromGeneratedBinding(contract->getGeneratedBinding()) != CanonicalSecurityOperationKind::Ordinary || contract->getStaticEffectContract() != L"RegisterWrite" || contract->getMemory()->Kind != MemoryCapabilityKind::NoMemory || contract->getReadRegisters() != std::vector<int>{1, 2} || contract->getWriteRegisters() != std::vector<int>{3} || contract->getPlacement()->RequiredSlotClass != SlotClass::AluClass || contract->getPlacement()->PinningKind != SlotPinningKind::ClassFlexible || (SlotClassLaneMap::GetLaneMask(SlotClass::AluClass) & (1 << scheduled->getPhysicalLane())) == 0 || scheduled->getOperationId()->OperationAttempt == 0 || scheduled->getOperationId()->VirtualThreadId != 1 || admission->VirtualThreadId != 1 || admission->OwnerContextId != 1 || admission->DomainTag != 0 || scheduled->getOperationId()->WorkingSlotIndex != 0 || admission->SourceProvenance->SourceSlotIndex != 0 || scheduled->getOperationId()->WorkingBundleSequence != admission->SourceProvenance->SourceBundleSerial || admission->SourceProvenance->FetchEpoch != scheduled->getOperationId()->WorkingBundleSequence)
		{
			return false;
		}
		return true;
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal SecureComputePreparedMachineAdmission? PrepareLiveSecureRegisterLocal(SecureComputeCanonicalAdmissionResult result, ExternalSecureExecutionReservation reservation, SecureComputeLiveCoreTarget target)
	std::shared_ptr<SecureComputePreparedMachineAdmission> SafetyVerifier::PrepareLiveSecureRegisterLocal(std::shared_ptr<SecureComputeCanonicalAdmissionResult> result, std::shared_ptr<ExternalSecureExecutionReservation> reservation, std::shared_ptr<SecureComputeLiveCoreTarget> target)
	{
		{
			std::scoped_lock<std::recursive_mutex> lock(secureRegisterLocalGate);
// C# TO C++ CONVERTER TASK: The following 'is' operator pattern is not converted by C# to C++ Converter:
			auto placed = result->getPlacedIntent();
			auto issued = placed == nullptr ? nullptr : placed->getIssuedAttempt();
			const bool domainRevoked = std::any_of(revokedSecureCertificateDomains.begin(), revokedSecureCertificateDomains.end(), [&result](const auto &domain) { return domain && domain->Equals(result->getRequest()->Binding.SecureDomain); });
			if (target == nullptr || !target->isCurrent() || secureCertificateScopeExhausted || secureCertificates.size() >= 8 || domainRevoked || reservation->Request != result->getRequest() || placed == nullptr || issued == nullptr || !issued->isCurrent() || issued->getScheduledOperation() != placed->getScheduledOperation())
			{
				return nullptr;
			}
			auto scheduled = placed->getScheduledOperation();
			auto admission = scheduled->getAdmission();
			auto contract = admission->Contract;
			const int targetVirtualThread = target->getVirtualThread();
			if (contract == nullptr || contract->getGeneratedBinding()->Opcode != Processor::CPU_Core::IsaOpcodeValues::ADD || CanonicalSecurityOperationIdentity::FromGeneratedBinding(contract->getGeneratedBinding()) != CanonicalSecurityOperationKind::Ordinary || contract->getPlacement()->RequiredSlotClass != SlotClass::AluClass || contract->getPlacement()->PinningKind != SlotPinningKind::ClassFlexible || admission->DomainTag != 0 || (SlotClassLaneMap::GetLaneMask(SlotClass::AluClass) & (1 << scheduled->getPhysicalLane())) == 0 || contract->getMemory()->Kind != MemoryCapabilityKind::NoMemory || contract->getStaticEffectContract() != L"RegisterWrite" || contract->getReadRegisters() != std::vector<int>{1, 2} || contract->getWriteRegisters() != std::vector<int>{3} || targetVirtualThread < 0 || targetVirtualThread > 1 || scheduled->getOperationId()->VirtualThreadId != targetVirtualThread || admission->VirtualThreadId != targetVirtualThread || admission->SourceProvenance->SourceVirtualThreadId != targetVirtualThread || admission->OwnerContextId != 0 || issued->getExecutionRecord()->getState() != Execution::ExecutionRecordState::Issued)
			{
				return nullptr;
			}
			auto certificate = std::make_shared<SecureComputeMachineCertificate>(placed, reservation, secureCertificateGeneration, target);
			secureCertificates.emplace(certificate, false);
			auto permit = getSecureIssuePermits()->Register(certificate);
			if (permit == nullptr)
			{
				secureCertificates.erase(certificate);
				return nullptr;
			}
			return std::make_shared<SecureComputePreparedMachineAdmission>(result, certificate, permit);
		}
	}

	bool SafetyVerifier::IsCurrentSecureComputeCertificate(std::shared_ptr<SecureComputeMachineCertificate> certificate)
	{
		{
			std::scoped_lock<std::recursive_mutex> lock(secureRegisterLocalGate);
// C# TO C++ CONVERTER TASK: The following 'is' operator pattern is not converted by C# to C++ Converter:
			auto liveTarget = certificate->getLiveCoreTarget();
			auto issued = certificate->getPlaced()->getIssuedAttempt();
			return !secureCertificateScopeExhausted && certificate->getGeneration() == secureCertificateGeneration && secureCertificates.contains(certificate) && (liveTarget == nullptr || liveTarget->isCurrent()) && issued != nullptr && issued->isCurrent() && issued->getExecutionRecord()->getState() == Execution::ExecutionRecordState::Issued;
		}
	}

	bool SafetyVerifier::RegisterSecureComputeCertificatePermit(std::shared_ptr<SecureComputeMachineCertificate> certificate)
	{
		{
			std::scoped_lock<std::recursive_mutex> lock(secureRegisterLocalGate);
			if (!IsCurrentSecureComputeCertificate(certificate) || secureCertificates[certificate])
			{
				return false;
			}
			secureCertificates[certificate] = true;
			return true;
		}
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal SecureComputeMachinePermitConsumption? ConsumeSecureRegisterLocalMachinePermit(SecureComputePreparedMachineAdmission prepared, ExternalSecureRegisterLocalRequest request, SecureComputePlacedIntent placed, ExternalSecureExecutionReservation reservation)
	std::shared_ptr<SecureComputeMachinePermitConsumption> SafetyVerifier::ConsumeSecureRegisterLocalMachinePermit(std::shared_ptr<SecureComputePreparedMachineAdmission> prepared, std::shared_ptr<ExternalSecureRegisterLocalRequest> request, std::shared_ptr<SecureComputePlacedIntent> placed, std::shared_ptr<ExternalSecureExecutionReservation> reservation)
	{
		{
			std::scoped_lock<std::recursive_mutex> lock(secureRegisterLocalGate);
			return getSecureIssuePermits()->Consume(prepared->getPermit(), request, placed, reservation);
		}
	}

	void SafetyVerifier::CompleteSecureRegisterLocalMachineAdmission(std::shared_ptr<SecureComputePreparedMachineAdmission> prepared)
	{
		{
			std::scoped_lock<std::recursive_mutex> lock(secureRegisterLocalGate);
			secureCertificates.erase(prepared->getCertificate());
			secureBackendAttempts.erase(prepared->getCertificate());
			secureSubmissions.erase(prepared->getCertificate());
			if (secureIssuePermits != nullptr)
			{
				secureIssuePermits->PruneInvalidated();
			}
			if (secureNoEffectCompletion != nullptr)
			{
				secureNoEffectCompletion->PruneInvalidated();
			}
			if (secureRegisterLocalCompletion != nullptr)
			{
				secureRegisterLocalCompletion->PruneInvalidated();
			}
		}
	}

	void SafetyVerifier::InvalidateSecureRegisterLocalAdmissions(std::shared_ptr<ExternalSecureDomainLease> domain)
	{
		{
			std::scoped_lock<std::recursive_mutex> lock(secureRegisterLocalGate);
			if (domain == nullptr)
			{
				return;
			}
			const bool alreadyRevoked = std::any_of(revokedSecureCertificateDomains.begin(), revokedSecureCertificateDomains.end(), [&domain](const auto &existing) { return existing && existing->Equals(*domain); });
			if (!alreadyRevoked)
			{
				revokedSecureCertificateDomains.push_back(domain);
			}
			std::vector<std::shared_ptr<SecureComputeMachineCertificate>> invalidCertificates;
			for (const auto &[certificate, registered] : secureCertificates)
			{
				const auto placed = certificate->getPlaced();
				const auto request = placed->getRequest();
				if (request && request->Binding.SecureDomain.Equals(*domain))
				{
					invalidCertificates.push_back(certificate);
				}
			}
			for (const auto &certificate : invalidCertificates)
			{
				secureCertificates.erase(certificate);
				secureBackendAttempts.erase(certificate);
				secureSubmissions.erase(certificate);
			}
			if (secureIssuePermits != nullptr)
			{
				secureIssuePermits->PruneInvalidated();
			}
			if (secureNoEffectCompletion != nullptr)
			{
				secureNoEffectCompletion->PruneInvalidated();
			}
			if (secureRegisterLocalCompletion != nullptr)
			{
				secureRegisterLocalCompletion->PruneInvalidated();
			}
		}
	}

	void SafetyVerifier::InvalidateAllSecureRegisterLocalAdmissions()
	{
		{
			std::scoped_lock<std::recursive_mutex> lock(secureRegisterLocalGate);
			secureCertificates.clear();
			secureBackendAttempts.clear();
			secureSubmissions.clear();
			if (secureCertificateGeneration == std::numeric_limits<unsigned long long>::max())
			{
				secureCertificateScopeExhausted = true;
			}
			else
			{
				secureCertificateGeneration++;
			}
			if (secureIssuePermits != nullptr)
			{
				secureIssuePermits->PruneInvalidated();
			}
			if (secureNoEffectCompletion != nullptr)
			{
				secureNoEffectCompletion->InvalidateAfterRestore();
			}
			if (secureRegisterLocalCompletion != nullptr)
			{
				secureRegisterLocalCompletion->InvalidateAfterRestore();
			}
		}
	}

	std::shared_ptr<SecureComputeCanonicalAdmissionResult> SafetyVerifier::AdmitSecureRegisterLocal(std::shared_ptr<ExternalSecureRegisterLocalRequest> request)
	{
		ArgumentNullException::ThrowIfNull(request);
		std::vector<VLIW_Instruction> raw(8);
		VLIW_Instruction temp_var = VLIW_Instruction();
		temp_var.setOpCode(Processor::CPU_Core::IsaOpcodeValues::ADD);
		temp_var.setWord1(VLIW_Instruction::PackArchRegs(3, 1, 2));
		raw[0] = temp_var;
		std::shared_ptr<DeclarativeDecodedBundle> decoded;
		std::optional<std::shared_ptr<DeclarativeDecodedBundle>> decodedResult;
		std::optional<std::shared_ptr<DecodeFailure>> decodeFailure;
		if (!DeclarativeDecoderPipeline::TryDecodeBundle(std::span<const VLIW_Instruction>(raw), nullptr, 0, 0, decodedResult, decodeFailure) || !decodedResult || !*decodedResult || (*decodedResult)->CanonicalBundleValue->GetSlot(0)->getSecurityOperationKind() != CanonicalSecurityOperationKind::Ordinary)
		{
			return SecureComputeCanonicalAdmissionResult::DeniedLocalBinding(request);
		}
		decoded = *decodedResult;

		{
			std::scoped_lock<std::recursive_mutex> lock(secureRegisterLocalGate);
			if (secureRegisterLocalSequence == std::numeric_limits<unsigned long long>::max())
			{
				return std::make_shared<SecureComputeCanonicalAdmissionResult>(request, SecureComputeCanonicalAdmissionDecision::DeniedMachinePlacement, decoded->CanonicalBundleValue);
			}
			unsigned long long sequence = ++secureRegisterLocalSequence;
			// Standalone bounded composition owns its empty working bundle. This
			// does not pretend to sample a live CPU core or reuse its certificates.
			// The approved RF-06 adapter uses the canonical generated contract;
			// Stage A, legality and Stage B lane choice remain scheduler-owned.
			auto scheduler = std::make_shared<MicroOpScheduler>();
			scheduler->setTypedSlotEnabled(true);
			auto canonical = decoded->CanonicalBundleValue->GetSlot(0);
			auto provenance = std::make_shared<SourceOperationProvenance>(decoded->CanonicalBundleValue->getSemanticKey(), 1, sequence, SlotId::Create(0), sequence);
			auto routed = Rf06ScalarSchedulerRouting::Route(scheduler, canonical, provenance, 1, std::vector<std::shared_ptr<MicroOp>>(8), 0, 0, 2, sequence, secureRegisterLocalAttemptIssuer);
			if (!routed->isScheduled() || routed->getScheduledOperation() == nullptr)
			{
				return std::make_shared<SecureComputeCanonicalAdmissionResult>(request, SecureComputeCanonicalAdmissionDecision::DeniedMachinePlacement, decoded->CanonicalBundleValue);
			}
			// The MicroOp and packed array stay local and are never dispatched.
			// The unreserved entry has no registered certificate/permit.
			// Placement facts alone cannot authorize issue; the reserved overload
			// additionally consumes a machine-local permit before its optional tail.
	// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
	// ORIGINAL LINE: return new(request, SecureComputeCanonicalAdmissionDecision.DeniedSecurePermitUnavailable, decoded.CanonicalBundle, new(request, routed.ScheduledOperation, routed.Carrier!.PostStageBIssuedAttempt));
			auto carrier = routed->getCarrier();
			auto placed = std::make_shared<SecureComputePlacedIntent>(request, routed->getScheduledOperation(), carrier == nullptr ? nullptr : carrier->getPostStageBIssuedAttempt());
			return std::make_shared<SecureComputeCanonicalAdmissionResult>(request, SecureComputeCanonicalAdmissionDecision::DeniedSecurePermitUnavailable, decoded->CanonicalBundleValue, placed);
		}
	}

	std::shared_ptr<SecureComputeRegisterLocalCompletionOwner> SafetyVerifier::getSecureRegisterLocalCompletion()
	{
		{
		std::scoped_lock<std::recursive_mutex> lock(secureRegisterLocalGate);
			return secureRegisterLocalCompletion = (secureRegisterLocalCompletion != nullptr) ? secureRegisterLocalCompletion : std::make_shared<SecureComputeRegisterLocalCompletionOwner>(shared_from_this(), secureRegisterLocalGate);
		}
	}

	bool SafetyVerifier::IsSecureRegisterLocalOwner(std::shared_ptr<SecureComputeRegisterLocalCompletionOwner> owner)
	{
		return secureRegisterLocalCompletion == owner;
	}

	bool SafetyVerifier::TryRegisterSecureSubmission(std::shared_ptr<SecureComputePreparedMachineAdmission> prepared)
	{
		{
			std::scoped_lock<std::recursive_mutex> lock(secureRegisterLocalGate);
			return HasConsumedSecureMachinePermit(prepared) && secureSubmissions.insert(prepared->getCertificate()).second;
		}
	}

	std::shared_ptr<SecureComputeCanonicalAdmissionResult> SafetyVerifier::RunSecureArithmetic(std::shared_ptr<SecureComputePreparedMachineAdmission> prepared, std::shared_ptr<ExternalSecureExecutionAttempt> attempt, std::shared_ptr<ExternalSecureExecutionConsumptionResult> consumption, std::shared_ptr<SecureComputeMachinePermitConsumption> machineFact, std::shared_ptr<IExternalSecureRegisterLocalExecutionProvider> backend)
	{
		auto owner = getSecureRegisterLocalCompletion();
	// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
	// ORIGINAL LINE: var pending = owner.Begin(prepared, attempt, consumption.Receipt!, backend);
		auto pending = owner->Begin(prepared, attempt, consumption->Receipt, backend);
		auto request = attempt->Request;
		if (pending == nullptr)
		{
			return std::make_shared<SecureComputeCanonicalAdmissionResult>(request, SecureComputeCanonicalAdmissionDecision::DeniedRegisterLocalCompletion, prepared->getResult()->getCanonical(), prepared->getCertificate()->getPlaced(), machineFact, attempt, consumption);
		}
		auto submission = pending->getSubmission();
		struct PendingCancellation final
		{
			std::shared_ptr<SecureComputeRegisterLocalCompletionOwner> Owner;
			std::shared_ptr<SecureComputeRegisterLocalCompletionOwner::Pending> Pending;
			bool Active = true;
			void Cancel() { if (Active && Owner && Pending) Owner->Cancel(Pending); Active = false; }
			~PendingCancellation() { Cancel(); }
		} cancellation{owner, pending};
		auto Result = [&](SecureComputeCanonicalAdmissionDecision decision,
			std::optional<std::shared_ptr<ExternalSecureRegisterLocalExecutionResult>> outcome,
			std::shared_ptr<SecureComputeRegisterLocalRetirement> retirement) {
			return std::make_shared<SecureComputeCanonicalAdmissionResult>(request, decision,
				prepared->getResult()->getCanonical(), prepared->getCertificate()->getPlaced(), machineFact,
				attempt, consumption, std::nullopt, std::nullopt, nullptr, submission,
				std::move(outcome), std::move(retirement));
		};
		try
		{
			std::optional<std::shared_ptr<ExternalSecureRegisterLocalExecutionResult>> outcome;
			try
			{
				outcome = backend->SubmitRegisterLocal(submission);
			}
			catch (...)
			{
				outcome = std::nullopt;
			}
// C# TO C++ CONVERTER TASK: The following 'is' operator pattern is not converted by C# to C++ Converter:
			if (!outcome || !*outcome || (*outcome)->getSubmission() != submission ||
				((*outcome)->getStatus() != ExternalSecureDomainProviderStatus::Receipt &&
				 (*outcome)->getStatus() != ExternalSecureDomainProviderStatus::Stale &&
				 (*outcome)->getStatus() != ExternalSecureDomainProviderStatus::Rejected))
			{
				return Result(SecureComputeCanonicalAdmissionDecision::DeniedBackendCleanupRequired, nullptr, nullptr);
			}
			if ((*outcome)->getStatus() != ExternalSecureDomainProviderStatus::Receipt)
			{
				return Result(SecureComputeCanonicalAdmissionDecision::DeniedRegisterLocalCompletion, outcome, nullptr);
			}
			auto original = owner->ObserveOriginal(pending);
// C# TO C++ CONVERTER TASK: Cannot test for object identity between 2 non-pointer objects in C++:
			if (!original || *original != *outcome)
			{
				return Result(SecureComputeCanonicalAdmissionDecision::DeniedBackendCleanupRequired, nullptr, nullptr);
			}
	// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
	// ORIGINAL LINE: var accepted = owner.Accept(pending, outcome.Receipt!);
			auto accepted = owner->Accept(pending, (*outcome)->getReceipt());
			auto retired = accepted == nullptr ? nullptr : owner->Retire(pending, accepted);
			return Result(retired == nullptr ? SecureComputeCanonicalAdmissionDecision::DeniedRegisterLocalCompletion : SecureComputeCanonicalAdmissionDecision::RegisterLocalRetired, outcome, retired);
		}
		catch (...)
		{
			cancellation.Cancel();
			throw;
		}
	}

	LegalityDecision SafetyVerifier::EvaluateSmtLegality(BundleResourceCertificate4Way bundleCertificate, PhaseCertificateTemplateKey4Way liveTemplateKey, PhaseCertificateTemplate4Way phaseTemplate, std::shared_ptr<MicroOp> candidate)
	{
		ArgumentNullException::ThrowIfNull(candidate);

		LegalityDecision boundaryDecision = EvaluateSmtBoundaryGuard(liveTemplateKey);
		if (!boundaryDecision.isAllowed())
		{
			return boundaryDecision;
		}

		if (const auto guardReject = TryRejectSmtOwnerDomainGuard(liveTemplateKey.getBundleMetadata(), candidate))
		{
			return *guardReject;
		}

		bool attemptedReplayReuse = liveTemplateKey.getPhaseKey().isValid();
		if (attemptedReplayReuse && phaseTemplate.Matches(liveTemplateKey))
		{
			return EvaluateSmtCertificate(phaseTemplate.getCertificate(), candidate, LegalityAuthoritySource::ReplayPhaseCertificate, attemptedReplayReuse);
		}

		return EvaluateSmtCertificate(bundleCertificate, candidate, LegalityAuthoritySource::StructuralCertificate, attemptedReplayReuse);
	}

	LegalityDecision SafetyVerifier::ILegalityChecker_EvaluateSmtLegality(BundleResourceCertificate4Way bundleCertificate, PhaseCertificateTemplateKey4Way liveTemplateKey, PhaseCertificateTemplate4Way phaseTemplate, std::shared_ptr<MicroOp> candidate)
	{
		return EvaluateSmtLegality(bundleCertificate, liveTemplateKey, phaseTemplate, candidate);
	}

	TypedSlotRejectClassification SafetyVerifier::ClassifyReject(TypedSlotRejectReason admissionReject, CertificateRejectDetail certDetail, SlotClass candidateClass, SlotPinningKind pinningKind)
	{
		return TypedSlotRejectClassification(admissionReject, certDetail, candidateClass, pinningKind, admissionReject == TypedSlotRejectReason::PinnedLaneConflict, (admissionReject == TypedSlotRejectReason::StaticClassOvercommit || admissionReject == TypedSlotRejectReason::DynamicClassExhaustion), admissionReject == TypedSlotRejectReason::StaticClassOvercommit, admissionReject == TypedSlotRejectReason::DynamicClassExhaustion, (admissionReject == TypedSlotRejectReason::ScoreboardReject || admissionReject == TypedSlotRejectReason::BankPendingReject || admissionReject == TypedSlotRejectReason::HardwareBudgetReject || admissionReject == TypedSlotRejectReason::AssistQuotaReject || admissionReject == TypedSlotRejectReason::AssistBackpressureReject || admissionReject == TypedSlotRejectReason::SpeculationBudgetReject || admissionReject == TypedSlotRejectReason::DynamicClassExhaustion));
	}

	TypedSlotRejectClassification SafetyVerifier::ILegalityChecker_ClassifyReject(TypedSlotRejectReason admissionReject, CertificateRejectDetail certDetail, SlotClass candidateClass, SlotPinningKind pinningKind)
	{
		return ClassifyReject(admissionReject, certDetail, candidateClass, pinningKind);
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public static bool ValidateTypedSlotFacts(TypedSlotBundleFacts facts, MicroOp?[] bundle, int bundleWidth = 8)
	bool SafetyVerifier::ValidateTypedSlotFacts(TypedSlotBundleFacts facts, std::vector<std::shared_ptr<MicroOp>> &bundle, int bundleWidth)
	{
		ArgumentNullException::ThrowIfNull(bundle);

		// Current ValidationOnly mainline: empty/default facts mean the producer
		// did not supply typed-slot facts for this bundle, so runtime legality
		// must continue without treating that absence as an admission failure.
		if (facts.isEmpty())
		{
			return true;
		}

		int limit = bundle.size() < bundleWidth ? bundle.size() : bundleWidth;

		unsigned char actualAlu = 0, actualLsu = 0, actualDma = 0, actualMatrixTileStream = 0, actualBranch = 0, actualSys = 0;
		int actualFlexible = 0, actualPinned = 0;
		int totalNonNull = 0;

		// Track pinned lane IDs for duplicate detection (8-bit occupancy for lanes 0-7)
		unsigned char pinnedLaneOccupancy = 0;

		for (int i = 0; i < limit; i++)
		{
			auto op = bundle[i];
			if (op == nullptr)
			{
				continue;
			}

			totalNonNull++;
			SlotPlacementMetadata placement = op->getPlacement();

			// Check 1: per-slot class match
			if (placement.RequiredSlotClass != facts.GetSlotClass(i))
			{
				return false;
			}

			switch (placement.RequiredSlotClass)
			{
				case SlotClass::AluClass:
					actualAlu++;
					break;
				case SlotClass::LsuClass:
					actualLsu++;
					break;
				case SlotClass::DmaStreamClass:
					actualDma++;
					break;
				case SlotClass::MatrixTileStreamClass:
					actualMatrixTileStream++;
					break;
				case SlotClass::BranchControl:
					actualBranch++;
					break;
				case SlotClass::SystemSingleton:
					actualSys++;
					break;
			}

			if (placement.PinningKind == SlotPinningKind::HardPinned)
			{
				actualPinned++;
				unsigned char laneBit = static_cast<unsigned char>(1 << placement.PinnedLaneId);
				if ((pinnedLaneOccupancy & laneBit) != 0)
				{
					return false;
				}

				pinnedLaneOccupancy |= laneBit;
			}
			else
			{
				actualFlexible++;
			}
		}

		if (facts.getAluCount() != actualAlu || facts.getLsuCount() != actualLsu || facts.getDmaStreamCount() != actualDma || facts.getMatrixTileStreamCount() != actualMatrixTileStream || facts.getBranchControlCount() != actualBranch || facts.getSystemSingletonCount() != actualSys)
		{
			return false;
		}

		if (facts.getAluCount() > SlotClassLaneMap::GetClassCapacity(SlotClass::AluClass) || facts.getLsuCount() > SlotClassLaneMap::GetClassCapacity(SlotClass::LsuClass) || facts.getDmaStreamCount() > SlotClassLaneMap::GetClassCapacity(SlotClass::DmaStreamClass) || facts.getMatrixTileStreamCount() > SlotClassLaneMap::GetClassCapacity(SlotClass::MatrixTileStreamClass) || facts.getBranchControlCount() > SlotClassLaneMap::GetClassCapacity(SlotClass::BranchControl) || facts.getSystemSingletonCount() > SlotClassLaneMap::GetClassCapacity(SlotClass::SystemSingleton))
		{
			return false;
		}

		if (facts.getDmaStreamCount() + facts.getMatrixTileStreamCount() > 1)
		{
			return false;
		}

		if (facts.getPinnedOpCount() + facts.getFlexibleOpCount() != totalNonNull)
		{
			return false;
		}

		if (facts.getPinnedOpCount() != actualPinned || facts.getFlexibleOpCount() != actualFlexible)
		{
			return false;
		}

		return true;
	}

	bool SafetyVerifier::VerifyTypedSlotAgreement(TypedSlotBundleFacts compilerFacts, SlotClassCapacityState runtimeCapacity)
	{
		return compilerFacts.getAluCount() <= runtimeCapacity.AluTotal && compilerFacts.getLsuCount() <= runtimeCapacity.LsuTotal && compilerFacts.getDmaStreamCount() <= runtimeCapacity.DmaStreamTotal && compilerFacts.getMatrixTileStreamCount() <= runtimeCapacity.MatrixTileStreamTotal && compilerFacts.getDmaStreamCount() + compilerFacts.getMatrixTileStreamCount() <= 1 && compilerFacts.getBranchControlCount() <= runtimeCapacity.BranchControlTotal && compilerFacts.getSystemSingletonCount() <= runtimeCapacity.SystemSingletonTotal;
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: public bool VerifyBundle(IReadOnlyList<MicroOp?> bundle)
	bool SafetyVerifier::VerifyBundle(const std::vector<std::shared_ptr<MicroOp>> &bundle)
	{
		if (bundle.empty() || bundle.size() != 8)
		{
			return false;
		}

		// Check each pair of operations for conflicts
		for (int i = 0; i < bundle.size(); i++)
		{
			if (bundle[i] == nullptr)
			{
				continue;
			}

			for (int j = i + 1; j < bundle.size(); j++)
			{
				if (bundle[j] == nullptr)
				{
					continue;
				}

				// If different threads, verify safety
				if (bundle[i]->getOwnerThreadId() != bundle[j]->getOwnerThreadId())
				{
					// Check register dependencies
					if (!CheckRegisterDependencies({bundle[i]}, bundle[j], bundle[i]->getOwnerThreadId(), bundle[j]->getOwnerThreadId()))
					{
						return false;
					}

					// Check memory dependencies
					if (!CheckMemoryDependencies({bundle[i]}, bundle[j], bundle[i]->getOwnerThreadId(), bundle[j]->getOwnerThreadId()))
					{
						return false;
					}
				}
			}
		}

		return true; // Bundle is safe
	}

	std::wstring SafetyVerifier::ExportSMTLib2Proofs()
	{
		std::shared_ptr<StringBuilder> sb = std::make_shared<StringBuilder>();
		sb->appendLine(L"; SMT-LIB2 format proof of FSP safety properties");
		sb->appendLine(L"(set-logic QF_BV)");
		sb->appendLine();

		for (auto condition : getFormalContext().GeneratedConditions)
		{
			sb->appendLine(std::format(L"; {}: {}", static_cast<int>(condition.Type), condition.Description));
			if (!condition.FormalSpec.empty())
			{
				sb->appendLine(condition.FormalSpec);
			}
			sb->appendLine();
		}

		sb->appendLine(L"(check-sat)");
		return sb->toString();
	}

	BundleResourceProof SafetyVerifier::SignProof(BundleResourceProof proof)
	{
		// Simulated signature generation for validation fixtures only.

		// For ISE: generate deterministic signature from proof contents
// C# TO C++ CONVERTER NOTE: The following 'using' block is replaced by its C++ equivalent:
// ORIGINAL LINE: using (SHA256 sha256 = SHA256.Create())
		std::vector<unsigned char> message;
		const auto appendNativeBytes = [&message](const auto &value) {
			const auto *bytes = reinterpret_cast<const unsigned char *>(&value);
			message.insert(message.end(), bytes, bytes + sizeof(value));
		};
		appendNativeBytes(proof.Cycle);
		appendNativeBytes(proof.BundleHash);
		appendNativeBytes(proof.AllowedMemoryRangeStart);
		appendNativeBytes(proof.AllowedMemoryRangeEnd);
		appendNativeBytes(proof.ThreadMask);
		const std::wstring digest = Decoder::Sha256Hex(message);
		proof.Signature.clear();
		proof.Signature.reserve(digest.size() / 2);
		for (std::size_t i = 0; i + 1 < digest.size(); i += 2)
		{
			const auto byte = static_cast<unsigned char>(std::stoul(digest.substr(i, 2), nullptr, 16));
			proof.Signature.push_back(byte);
		}

		return proof;
	}

	SafetyVerifier::VirtualizationAdmissionCertificate::VirtualizationAdmissionCertificate(std::any issuerSeal, unsigned long long issuerGeneration, unsigned long long attemptId, unsigned short opcode, VmxOperationKind operation, int virtualThreadId, int ownerContextId, unsigned long long domainTag, int sourceSlotId, int workingSlotId, unsigned long long bundleIdentity, unsigned long long replayEpoch, unsigned long long carrierIdentityDigest)
		: IssuerGeneration(issuerGeneration), AttemptId(attemptId), Opcode(opcode), Operation(operation),
		  VirtualThreadId(virtualThreadId), OwnerContextId(ownerContextId), DomainTag(domainTag),
		  SourceSlotId(sourceSlotId), WorkingSlotId(workingSlotId), BundleIdentity(bundleIdentity),
		  ReplayEpoch(replayEpoch), CarrierIdentityDigest(carrierIdentityDigest), _issuerSeal(std::move(issuerSeal))
	{
	}

	unsigned long long SafetyVerifier::VirtualizationAdmissionCertificate::getIssuerGeneration() const
	{
		return IssuerGeneration;
	}

	unsigned int SafetyVerifier::VirtualizationAdmissionCertificate::getSchemaVersion() const
	{
		return 1;
	}

	unsigned long long SafetyVerifier::VirtualizationAdmissionCertificate::getAttemptId() const
	{
		return AttemptId;
	}

	unsigned short SafetyVerifier::VirtualizationAdmissionCertificate::getOpcode() const
	{
		return Opcode;
	}

	VmxOperationKind SafetyVerifier::VirtualizationAdmissionCertificate::getOperation() const
	{
		return Operation;
	}

	int SafetyVerifier::VirtualizationAdmissionCertificate::getVirtualThreadId() const
	{
		return VirtualThreadId;
	}

	int SafetyVerifier::VirtualizationAdmissionCertificate::getOwnerContextId() const
	{
		return OwnerContextId;
	}

	unsigned long long SafetyVerifier::VirtualizationAdmissionCertificate::getDomainTag() const
	{
		return DomainTag;
	}

	int SafetyVerifier::VirtualizationAdmissionCertificate::getSourceSlotId() const
	{
		return SourceSlotId;
	}

	int SafetyVerifier::VirtualizationAdmissionCertificate::getWorkingSlotId() const
	{
		return WorkingSlotId;
	}

	unsigned long long SafetyVerifier::VirtualizationAdmissionCertificate::getBundleIdentity() const
	{
		return BundleIdentity;
	}

	unsigned long long SafetyVerifier::VirtualizationAdmissionCertificate::getReplayEpoch() const
	{
		return ReplayEpoch;
	}

	unsigned long long SafetyVerifier::VirtualizationAdmissionCertificate::getAttemptEpoch() const
	{
		return getReplayEpoch();
	}

	unsigned long long SafetyVerifier::VirtualizationAdmissionCertificate::getReplayGeneration() const
	{
		return getIssuerGeneration();
	}

	unsigned long long SafetyVerifier::VirtualizationAdmissionCertificate::getCarrierIdentityDigest() const
	{
		return CarrierIdentityDigest;
	}

	bool SafetyVerifier::VirtualizationAdmissionCertificate::getHasAcceptedNumericLeaf() const
	{
		return false;
	}

	bool SafetyVerifier::VirtualizationAdmissionCertificate::getHasMaterializedAddressSpaceIdentity() const
	{
		return false;
	}

	unsigned long long SafetyVerifier::VirtualizationAdmissionCertificate::getAddressSpaceTag() const
	{
		return 0;
	}

	bool SafetyVerifier::VirtualizationAdmissionCertificate::getHasMaterializedDescriptorIdentity() const
	{
		return false;
	}

	unsigned long long SafetyVerifier::VirtualizationAdmissionCertificate::getDescriptorIdentityDigest() const
	{
		return 0;
	}

	unsigned long long SafetyVerifier::VirtualizationAdmissionCertificate::getDescriptorEpoch() const
	{
		return 0;
	}

	bool SafetyVerifier::VirtualizationAdmissionCertificate::getHasCapabilityGrantIdentity() const
	{
		return false;
	}

	unsigned long long SafetyVerifier::VirtualizationAdmissionCertificate::getCapabilityGrantIdentity() const
	{
		return 0;
	}

	unsigned long long SafetyVerifier::VirtualizationAdmissionCertificate::getCapabilityRevocationEpoch() const
	{
		return 0;
	}

	bool SafetyVerifier::VirtualizationAdmissionCertificate::getHasEvidencePolicyIdentity() const
	{
		return false;
	}

	unsigned long long SafetyVerifier::VirtualizationAdmissionCertificate::getEvidencePolicyDigest() const
	{
		return 0;
	}

	unsigned long long SafetyVerifier::VirtualizationAdmissionCertificate::getEvidenceEpoch() const
	{
		return 0;
	}

	bool SafetyVerifier::VirtualizationAdmissionCertificate::getHasRestoreGeneration() const
	{
		return false;
	}

	unsigned long long SafetyVerifier::VirtualizationAdmissionCertificate::getRestoreGeneration() const
	{
		return 0;
	}

	bool SafetyVerifier::VirtualizationAdmissionCertificate::getBackendExecutionAuthorized() const
	{
		return false;
	}

	bool SafetyVerifier::VirtualizationAdmissionCertificate::getCompletionPublicationAuthorized() const
	{
		return false;
	}

	bool SafetyVerifier::VirtualizationAdmissionCertificate::getRetirePublicationAuthorized() const
	{
		return false;
	}

	bool SafetyVerifier::VirtualizationAdmissionCertificate::WasIssuedBy(std::any issuerSeal)
	{
		if (_issuerSeal.type() != typeid(std::shared_ptr<int>) || issuerSeal.type() != typeid(std::shared_ptr<int>))
		{
			return false;
		}
		return std::any_cast<std::shared_ptr<int>>(_issuerSeal) == std::any_cast<std::shared_ptr<int>>(issuerSeal);
	}

	std::shared_ptr<VirtualizationAdmissionIssueResult> SafetyVerifier::IssueVirtualizationAdmissionAfterStageB(ReplayPhaseContext replayPhase, SmtBundleMetadata4Way bundleMetadata, std::shared_ptr<MicroOp> candidate, int sourceSlotId, int selectedLane)
	{
		std::shared_ptr<VmxMicroOp> vmx = std::dynamic_pointer_cast<VmxMicroOp>(candidate);
		if (vmx == nullptr)
		{
			return DenyIssue(VirtualizationAdmissionIssueDecision::NotVmxMicroOp, L"Only a canonical VmxMicroOp may receive E1 virtualization admission.");
		}

		VmxOperationKind operation;
		if (!vmx->TryResolveFrozenOperation(operation))
		{
			return DenyIssue(VirtualizationAdmissionIssueDecision::UnknownFrozenOpcode, L"The candidate opcode is not in the frozen VMX compatibility vocabulary.");
		}

		VtId validatedVirtualThreadId;
		if (!VtId::TryCreate(vmx->getVirtualThreadId(), validatedVirtualThreadId))
		{
			return DenyIssue(VirtualizationAdmissionIssueDecision::InvalidVirtualThread, L"The VMX candidate has no valid canonical virtual-thread identity.");
		}

		if (vmx->getOwnerThreadId() != vmx->getVirtualThreadId() || bundleMetadata.getOwnerVirtualThreadId() != vmx->getVirtualThreadId())
		{
			return DenyIssue(VirtualizationAdmissionIssueDecision::OwnerVirtualThreadMismatch, L"Owner, candidate and bundle virtual-thread identities must match.");
		}

		if (bundleMetadata.getHasKnownOwnerContext() && bundleMetadata.getOwnerContextId() != vmx->getOwnerContextId())
		{
			return DenyIssue(VirtualizationAdmissionIssueDecision::OwnerContextMismatch, L"The live bundle owner context does not match the VMX candidate.");
		}

		if (bundleMetadata.getHasDomainRestriction() && bundleMetadata.getOwnerDomainTag() != vmx->getPlacement().DomainTag)
		{
			return DenyIssue(VirtualizationAdmissionIssueDecision::DomainMismatch, L"The live bundle domain does not match the VMX candidate.");
		}

		SlotPlacementMetadata placement = vmx->getPlacement();
		if (placement.PinningKind != SlotPinningKind::HardPinned || placement.RequiredSlotClass != SlotClass::SystemSingleton || placement.PinnedLaneId != 7 || sourceSlotId != 7)
		{
			return DenyIssue(VirtualizationAdmissionIssueDecision::InvalidSourceSlot, L"E1 recognizes only the frozen system-singleton VMX source slot.");
		}

		if (selectedLane != placement.PinnedLaneId)
		{
			return DenyIssue(VirtualizationAdmissionIssueDecision::WorkingSlotMismatch, L"The Stage-B working lane does not match the frozen VMX source slot.");
		}

		if (vmx->getVirtualizationAdmission() != nullptr)
		{
			return DenyIssue(VirtualizationAdmissionIssueDecision::DuplicateAttempt, L"A VMX carrier cannot receive a second E1 admission certificate.");
		}

		unsigned long long attemptId = AllocateVirtualizationAdmissionAttemptId();
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: ushort opcode = unchecked((ushort)vmx.OpCode);
		unsigned short opcode = static_cast<unsigned short>(vmx->getOpCode());
		std::shared_ptr<VirtualizationAdmissionCertificate> certificate = std::make_shared<VirtualizationAdmissionCertificate>(_virtualizationAdmissionIssuerSeal, _virtualizationAdmissionIssuerGeneration, attemptId, opcode, operation, vmx->getVirtualThreadId(), vmx->getOwnerContextId(), vmx->getPlacement().DomainTag, sourceSlotId, selectedLane, ComputeBundleIdentity(replayPhase, bundleMetadata), replayPhase.getEpochId(), ComputeCarrierIdentityDigest(vmx));
		_liveVirtualizationAdmissions->Add(certificate, std::make_shared<LiveVirtualizationAdmission>(attemptId, _virtualizationAdmissionIssuerGeneration));

		return std::make_shared<VirtualizationAdmissionIssueResult>(VirtualizationAdmissionIssueDecision::IssuedForFaultOnlyTransport, certificate, L"SafetyVerifier issued an E1 certificate for fault-only canonical transport.");
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal VirtualizationAdmissionValidationResult ValidateVirtualizationAdmission(ReplayPhaseContext replayPhase, SmtBundleMetadata4Way bundleMetadata, MicroOp candidate, int sourceSlotId, int selectedLane, VirtualizationAdmissionCertificate? certificate)
	std::shared_ptr<VirtualizationAdmissionValidationResult> SafetyVerifier::ValidateVirtualizationAdmission(ReplayPhaseContext replayPhase, SmtBundleMetadata4Way bundleMetadata, std::shared_ptr<MicroOp> candidate, int sourceSlotId, int selectedLane, std::shared_ptr<VirtualizationAdmissionCertificate> certificate)
	{
		if (certificate == nullptr)
		{
			return DenyValidation(VirtualizationAdmissionValidationDecision::MissingCertificate, L"Certificate is missing.");
		}

		if (!certificate->WasIssuedBy(_virtualizationAdmissionIssuerSeal))
		{
			return DenyValidation(VirtualizationAdmissionValidationDecision::IssuerMismatch, L"Certificate issuer mismatch.");
		}

		std::shared_ptr<LiveVirtualizationAdmission> live;
		if (!_liveVirtualizationAdmissions->TryGetValue(certificate, live) || live == nullptr || live->AttemptId != certificate->getAttemptId())
		{
			return DenyValidation(VirtualizationAdmissionValidationDecision::IssuanceNotLive, L"Certificate issuance is not live.");
		}

		if (certificate->getIssuerGeneration() != _virtualizationAdmissionIssuerGeneration || live->IssuerGeneration != _virtualizationAdmissionIssuerGeneration)
		{
			return DenyValidation(VirtualizationAdmissionValidationDecision::IssuerGenerationMismatch, L"Certificate was invalidated by an issuer-generation change.");
		}

		std::shared_ptr<VmxMicroOp> vmx = std::dynamic_pointer_cast<VmxMicroOp>(candidate);
		VmxOperationKind operation;
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: if (candidate is not VmxMicroOp vmx || !vmx.TryResolveFrozenOperation(out VmxOperationKind operation) || certificate.Opcode != unchecked((ushort)vmx.OpCode) || certificate.Operation != operation || certificate.CarrierIdentityDigest != ComputeCarrierIdentityDigest(vmx))
		if (vmx == nullptr || !vmx->TryResolveFrozenOperation(operation) || certificate->getOpcode() != static_cast<unsigned short>(vmx->getOpCode()) || certificate->getOperation() != operation || certificate->getCarrierIdentityDigest() != ComputeCarrierIdentityDigest(vmx))
		{
			return DenyValidation(VirtualizationAdmissionValidationDecision::OpcodeOrOperationMismatch, L"Opcode, operation or carrier identity changed after issuance.");
		}

		if (certificate->getVirtualThreadId() != vmx->getVirtualThreadId() || vmx->getOwnerThreadId() != vmx->getVirtualThreadId() || bundleMetadata.getOwnerVirtualThreadId() != vmx->getVirtualThreadId())
		{
			return DenyValidation(VirtualizationAdmissionValidationDecision::VirtualThreadMismatch, L"Virtual-thread identity mismatch.");
		}

		if (certificate->getOwnerContextId() != vmx->getOwnerContextId() || (bundleMetadata.getHasKnownOwnerContext() && bundleMetadata.getOwnerContextId() != vmx->getOwnerContextId()))
		{
			return DenyValidation(VirtualizationAdmissionValidationDecision::OwnerContextMismatch, L"Owner context mismatch.");
		}

		if (certificate->getDomainTag() != vmx->getPlacement().DomainTag || (bundleMetadata.getHasDomainRestriction() && bundleMetadata.getOwnerDomainTag() != vmx->getPlacement().DomainTag))
		{
			return DenyValidation(VirtualizationAdmissionValidationDecision::DomainMismatch, L"Domain identity mismatch.");
		}

		if (certificate->getSourceSlotId() != sourceSlotId || sourceSlotId != vmx->getPlacement().PinnedLaneId || certificate->getWorkingSlotId() != selectedLane || selectedLane != vmx->getPlacement().PinnedLaneId)
		{
			return DenyValidation(VirtualizationAdmissionValidationDecision::SourceOrWorkingSlotMismatch, L"Source or working slot identity mismatch.");
		}

		if (certificate->getBundleIdentity() != ComputeBundleIdentity(replayPhase, bundleMetadata) || certificate->getReplayEpoch() != replayPhase.getEpochId())
		{
			return DenyValidation(VirtualizationAdmissionValidationDecision::BundleOrReplayMismatch, L"Bundle or replay identity mismatch.");
		}

		return std::make_shared<VirtualizationAdmissionValidationResult>(VirtualizationAdmissionValidationDecision::ValidForFaultOnlyTransport, L"Certificate is valid only for fault-only canonical transport.");
	}

	void SafetyVerifier::InvalidateVirtualizationAdmissions(ReplayPhaseInvalidationReason reason)
	{
		(void)reason;
//		unchecked
			_virtualizationAdmissionIssuerGeneration++;
			if (_virtualizationAdmissionIssuerGeneration == 0)
			{
				_virtualizationAdmissionIssuerGeneration = 1;
			}
// C# TO C++ CONVERTER TASK: End of the original C# 'unchecked' block.
	}

	unsigned long long SafetyVerifier::AllocateVirtualizationAdmissionAttemptId()
	{
		unsigned long long attemptId = _nextVirtualizationAdmissionAttemptId++;
		if (attemptId == 0)
		{
			attemptId = _nextVirtualizationAdmissionAttemptId++;
		}

		return attemptId;
	}

	unsigned long long SafetyVerifier::ComputeBundleIdentity(ReplayPhaseContext replayPhase, SmtBundleMetadata4Way bundleMetadata)
	{
		unsigned long long hash = 1469598103934665603ULL;
		Mix(hash, replayPhase.getEpochId());
		Mix(hash, replayPhase.getCachedPc());
		Mix(hash, replayPhase.getCompletedReplays());
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: Mix(ref hash, unchecked((ulong)(uint)bundleMetadata.OwnerVirtualThreadId));
		Mix(hash, static_cast<unsigned long long>(static_cast<unsigned int>(bundleMetadata.getOwnerVirtualThreadId())));
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: Mix(ref hash, unchecked((ulong)(uint)bundleMetadata.OwnerContextId));
		Mix(hash, static_cast<unsigned long long>(static_cast<unsigned int>(bundleMetadata.getOwnerContextId())));
		Mix(hash, bundleMetadata.getOwnerDomainTag());
		Mix(hash, bundleMetadata.getBundleDomainShapeId());
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: Mix(ref hash, unchecked((ulong)(uint)bundleMetadata.OperationCount));
		Mix(hash, static_cast<unsigned long long>(static_cast<unsigned int>(bundleMetadata.getOperationCount())));
		return hash;
	}

	unsigned long long SafetyVerifier::ComputeCarrierIdentityDigest(std::shared_ptr<VmxMicroOp> vmx)
	{
		unsigned long long hash = 1469598103934665603ULL;
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: Mix(ref hash, unchecked((ushort)vmx.OpCode));
		Mix(hash, static_cast<unsigned short>(vmx->getOpCode()));
		Mix(hash, vmx->getRd());
		Mix(hash, vmx->getRs1());
		Mix(hash, vmx->getRs2());
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: Mix(ref hash, unchecked((ulong)(uint)vmx.Placement.PinnedLaneId));
		Mix(hash, static_cast<unsigned long long>(static_cast<unsigned int>(vmx->getPlacement().PinnedLaneId)));
		return hash;
	}

	void SafetyVerifier::Mix(unsigned long long &hash, unsigned long long value)
	{
		hash ^= value;
		hash *= 1099511628211ULL;
	}

	std::shared_ptr<VirtualizationAdmissionIssueResult> SafetyVerifier::DenyIssue(VirtualizationAdmissionIssueDecision decision, const std::wstring &reason)
	{
		return std::make_shared<VirtualizationAdmissionIssueResult>(decision, nullptr, reason);
	}

	std::shared_ptr<VirtualizationAdmissionValidationResult> SafetyVerifier::DenyValidation(VirtualizationAdmissionValidationDecision decision, const std::wstring &reason)
	{
		return std::make_shared<VirtualizationAdmissionValidationResult>(decision, reason);
	}

	SafetyVerifier::VirtualizationOperationAdmissionCertificate::VirtualizationOperationAdmissionCertificate(std::any issuerSeal, unsigned long long issuanceSequence, unsigned long long attemptId, unsigned long long e1IssuerGeneration, int virtualThreadId, int ownerContextId, unsigned long long domainTag, unsigned long long bundleIdentity, unsigned long long replayEpoch, const std::wstring &decisionId, const std::wstring &specDigest, const std::wstring &acceptanceDigest, unsigned long long ownerId, unsigned int ownerPolicyVersion, unsigned int ownerEpoch, const std::wstring &operationNamespace, const std::wstring &operationId, unsigned short numericLeaf, const std::wstring &ownerPolicyDigest, const std::wstring &operandDigest, unsigned long long capabilityGrantIdentity, unsigned long long capabilityGeneration, unsigned long long rootAuthorityEpoch, unsigned long long restoreGeneration, const std::wstring &certificateDigest)
		: IssuanceSequence(issuanceSequence), AttemptId(attemptId), E1IssuerGeneration(e1IssuerGeneration),
		  VirtualThreadId(virtualThreadId), OwnerContextId(ownerContextId), DomainTag(domainTag),
		  BundleIdentity(bundleIdentity), ReplayEpoch(replayEpoch), DecisionId(decisionId), SpecDigest(specDigest),
		  AcceptanceDigest(acceptanceDigest), OwnerId(ownerId), OwnerPolicyVersion(ownerPolicyVersion), OwnerEpoch(ownerEpoch),
		  OperationNamespace(operationNamespace), OperationId(operationId), NumericLeaf(numericLeaf),
		  OwnerPolicyDigest(ownerPolicyDigest), OperandDigest(operandDigest),
		  CapabilityGrantIdentity(capabilityGrantIdentity), CapabilityGeneration(capabilityGeneration),
		  RootAuthorityEpoch(rootAuthorityEpoch), RestoreGeneration(restoreGeneration),
		  CertificateDigest(certificateDigest), _issuerSeal(std::move(issuerSeal))
	{
	}

	unsigned int SafetyVerifier::VirtualizationOperationAdmissionCertificate::getSchemaVersion() const
	{
		return 2;
	}

	unsigned long long SafetyVerifier::VirtualizationOperationAdmissionCertificate::getIssuanceSequence() const
	{
		return IssuanceSequence;
	}

	unsigned long long SafetyVerifier::VirtualizationOperationAdmissionCertificate::getAttemptId() const
	{
		return AttemptId;
	}

	unsigned long long SafetyVerifier::VirtualizationOperationAdmissionCertificate::getE1IssuerGeneration() const
	{
		return E1IssuerGeneration;
	}

	int SafetyVerifier::VirtualizationOperationAdmissionCertificate::getVirtualThreadId() const
	{
		return VirtualThreadId;
	}

	int SafetyVerifier::VirtualizationOperationAdmissionCertificate::getOwnerContextId() const
	{
		return OwnerContextId;
	}

	unsigned long long SafetyVerifier::VirtualizationOperationAdmissionCertificate::getDomainTag() const
	{
		return DomainTag;
	}

	unsigned long long SafetyVerifier::VirtualizationOperationAdmissionCertificate::getBundleIdentity() const
	{
		return BundleIdentity;
	}

	unsigned long long SafetyVerifier::VirtualizationOperationAdmissionCertificate::getReplayEpoch() const
	{
		return ReplayEpoch;
	}

	std::wstring SafetyVerifier::VirtualizationOperationAdmissionCertificate::getDecisionId() const
	{
		return DecisionId;
	}

	std::wstring SafetyVerifier::VirtualizationOperationAdmissionCertificate::getSpecDigest() const
	{
		return SpecDigest;
	}

	std::wstring SafetyVerifier::VirtualizationOperationAdmissionCertificate::getAcceptanceDigest() const
	{
		return AcceptanceDigest;
	}

	unsigned long long SafetyVerifier::VirtualizationOperationAdmissionCertificate::getOwnerId() const
	{
		return OwnerId;
	}

	unsigned int SafetyVerifier::VirtualizationOperationAdmissionCertificate::getOwnerPolicyVersion() const
	{
		return OwnerPolicyVersion;
	}

	unsigned int SafetyVerifier::VirtualizationOperationAdmissionCertificate::getOwnerEpoch() const
	{
		return OwnerEpoch;
	}

	std::wstring SafetyVerifier::VirtualizationOperationAdmissionCertificate::getOperationNamespace() const
	{
		return OperationNamespace;
	}

	std::wstring SafetyVerifier::VirtualizationOperationAdmissionCertificate::getOperationId() const
	{
		return OperationId;
	}

	unsigned short SafetyVerifier::VirtualizationOperationAdmissionCertificate::getNumericLeaf() const
	{
		return NumericLeaf;
	}

	std::wstring SafetyVerifier::VirtualizationOperationAdmissionCertificate::getOwnerPolicyDigest() const
	{
		return OwnerPolicyDigest;
	}

	std::wstring SafetyVerifier::VirtualizationOperationAdmissionCertificate::getOperandDigest() const
	{
		return OperandDigest;
	}

	unsigned long long SafetyVerifier::VirtualizationOperationAdmissionCertificate::getCapabilityGrantIdentity() const
	{
		return CapabilityGrantIdentity;
	}

	unsigned long long SafetyVerifier::VirtualizationOperationAdmissionCertificate::getCapabilityGeneration() const
	{
		return CapabilityGeneration;
	}

	unsigned long long SafetyVerifier::VirtualizationOperationAdmissionCertificate::getRootAuthorityEpoch() const
	{
		return RootAuthorityEpoch;
	}

	unsigned long long SafetyVerifier::VirtualizationOperationAdmissionCertificate::getRestoreGeneration() const
	{
		return RestoreGeneration;
	}

	std::wstring SafetyVerifier::VirtualizationOperationAdmissionCertificate::getCertificateDigest() const
	{
		return CertificateDigest;
	}

	bool SafetyVerifier::VirtualizationOperationAdmissionCertificate::getHasAddressSpaceIdentity() const
	{
		return false;
	}

	bool SafetyVerifier::VirtualizationOperationAdmissionCertificate::getHasEvidenceIdentity() const
	{
		return false;
	}

	bool SafetyVerifier::VirtualizationOperationAdmissionCertificate::getBackendExecutionAuthorized() const
	{
		return false;
	}

	bool SafetyVerifier::VirtualizationOperationAdmissionCertificate::getCompletionPublicationAuthorized() const
	{
		return false;
	}

	bool SafetyVerifier::VirtualizationOperationAdmissionCertificate::getRetirePublicationAuthorized() const
	{
		return false;
	}

	bool SafetyVerifier::VirtualizationOperationAdmissionCertificate::WasIssuedBy(std::any seal)
	{
		const auto *expected = std::any_cast<std::shared_ptr<int>>(&_issuerSeal);
		const auto *actual = std::any_cast<std::shared_ptr<int>>(&seal);
		return expected != nullptr && actual != nullptr && *expected == *actual;
	}

	std::shared_ptr<VirtualizationOperationAdmissionCertificate> SafetyVerifier::VirtualizationOperationAdmissionCertificate::Create(std::any issuerSeal, unsigned long long issuanceSequence, std::shared_ptr<VirtualizationE2IssueRequest> request)
	{
	// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
	// ORIGINAL LINE: VirtualizationAdmissionCertificate e1 = request.E1!;
		std::shared_ptr<VirtualizationAdmissionCertificate> e1 = request->E1;
	// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
	// ORIGINAL LINE: VirtualizationOperationOwnerSnapshot owner = request.OwnerPolicy!;
		std::shared_ptr<VirtualizationOperationOwnerSnapshot> owner = request->OwnerPolicy;
	// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
	// ORIGINAL LINE: VirtualizationOperandSnapshot operand = request.Operand!;
		std::shared_ptr<VirtualizationOperandSnapshot> operand = request->Operand;
	// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
	// ORIGINAL LINE: RuntimeCapabilityGrantLease lease = request.CapabilityLease!;
		std::shared_ptr<RuntimeCapabilityGrantLease> lease = request->CapabilityLease;
	// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
	// ORIGINAL LINE: RootAuthorityDescriptor root = request.RootAuthority!;
		std::shared_ptr<RootAuthorityDescriptor> root = request->RootAuthority;
	// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
	// ORIGINAL LINE: string digest = VirtualizationE2Digest.Compute( issuanceSequence, e1.AttemptId, e1.IssuerGeneration, e1.VirtualThreadId, e1.OwnerContextId, e1.DomainTag, e1.BundleIdentity, e1.ReplayEpoch, owner.DecisionId, owner.SpecDigest, owner.AcceptanceDigest, owner.OwnerId, owner.OwnerPolicyVersion, owner.OwnerEpoch, owner.OperationNamespace, owner.OperationId, owner.NumericLeaf, owner.PolicyDigest, operand.OperandDigest, lease.GrantIdentity, lease.Generation, root.AuthorityEpoch, request.RestoreOwner!.CurrentGeneration);
		const auto restoreGeneration = request->RestoreOwner->getCurrentGeneration();
		std::wstring digest = VirtualizationE2Digest::Compute(issuanceSequence, e1->getAttemptId(), e1->getIssuerGeneration(), e1->getVirtualThreadId(), e1->getOwnerContextId(), e1->getDomainTag(), e1->getBundleIdentity(), e1->getReplayEpoch(), owner->getDecisionId(), owner->getSpecDigest(), owner->getAcceptanceDigest(), owner->getOwnerId(), owner->getOwnerPolicyVersion(), owner->getOwnerEpoch(), owner->getOperationNamespace(), owner->getOperationId(), owner->getNumericLeaf(), owner->getPolicyDigest(), operand->getOperandDigest(), lease->getGrantIdentity(), lease->getGeneration(), root->getAuthorityEpoch(), restoreGeneration);
		return std::shared_ptr<VirtualizationOperationAdmissionCertificate>(new VirtualizationOperationAdmissionCertificate(issuerSeal, issuanceSequence, e1->getAttemptId(), e1->getIssuerGeneration(), e1->getVirtualThreadId(), e1->getOwnerContextId(), e1->getDomainTag(), e1->getBundleIdentity(), e1->getReplayEpoch(), owner->getDecisionId(), owner->getSpecDigest(), owner->getAcceptanceDigest(), owner->getOwnerId(), owner->getOwnerPolicyVersion(), owner->getOwnerEpoch(), owner->getOperationNamespace(), owner->getOperationId(), owner->getNumericLeaf(), owner->getPolicyDigest(), operand->getOperandDigest(), lease->getGrantIdentity(), lease->getGeneration(), root->getAuthorityEpoch(), restoreGeneration, digest));
	}

	SafetyVerifier::LiveVirtualizationE2::LiveVirtualizationE2(std::shared_ptr<VirtualizationE2IssueRequest> request) : Request(std::move(request))
	{
	}

	std::shared_ptr<VirtualizationE2IssueRequest> SafetyVerifier::LiveVirtualizationE2::getRequest() const
	{
		return Request;
	}

	VirtualizationE2State SafetyVerifier::LiveVirtualizationE2::getState() const
	{
		return State;
	}

	void SafetyVerifier::LiveVirtualizationE2::setState(VirtualizationE2State value)
	{
		State = value;
	}

	std::shared_ptr<VirtualizationE2Result> SafetyVerifier::IssueVirtualizationE2(std::shared_ptr<VirtualizationE2IssueRequest> request)
	{
		if (request->Carrier == nullptr || request->E1 == nullptr || request->OwnerPolicy == nullptr || request->Operand == nullptr || request->DomainContext == nullptr || request->RootAuthority == nullptr || request->CapabilityOwner == nullptr || request->CapabilityLease == nullptr || request->RestoreOwner == nullptr || request->LifecycleGate == nullptr)
		{
			return DenyE2(VirtualizationE2Decision::MissingInput, L"E2 requires live E1, O1, operand, domain, root, capability, restore, and lifecycle inputs.");
		}

// C# TO C++ CONVERTER TASK: The following .NET 'String.Equals' reference is not converted:
		const auto owner = request->OwnerPolicy;
		if (owner != Phase38VirtualizationOperationOwnerSnapshotRegistry::getExactSnapshot() || owner->getDecisionId() != VirtualizationDecisionValidatorV2::ExpectedDecisionId || owner->getSpecDigest() != Phase38VirtualizationDecisionSpecV2::getInstance()->SpecDigest || owner->getAcceptanceDigest() != Phase38VirtualizationDecisionAcceptanceV2::getRecord()->AcceptanceDigest || owner->getOwnerId() != VirtualizationDecisionValidatorV2::ExpectedOwnerId || owner->getOwnerPolicyVersion() != 1 || owner->getOwnerEpoch() != 1 || owner->getOperationNamespace() != VirtualizationDecisionValidatorV2::ExpectedOperationNamespace || owner->getOperationId() != VirtualizationDecisionValidatorV2::ExpectedOperationId || owner->getNumericLeaf() != 1)
		{
			return DenyE2(VirtualizationE2Decision::InvalidD2OrOwnerPolicy, L"E2 requires the exact current accepted D2/O1 instance.");
		}

		std::shared_ptr<VirtualizationAdmissionValidationResult> e1Validation = ValidateVirtualizationAdmission(request->ReplayPhase, request->BundleMetadata, request->Carrier, request->SourceSlotId, request->WorkingSlotId, request->E1);
		if (!e1Validation->IsValidForFaultOnlyTransport)
		{
			return DenyE2(VirtualizationE2Decision::InvalidE1, e1Validation->Reason);
		}

		std::shared_ptr<VirtualizationOperandValidationResult> operandValidation = VirtualizationOperandSnapshotMaterializer::ValidateForE2Input(request->Operand, request->Carrier, request->E1, request->OwnerPolicy, request->RestoreOwner->getCurrentGeneration());
		if (!operandValidation->isValidForE2Input())
		{
			return DenyE2(VirtualizationE2Decision::InvalidOperand, operandValidation->Reason);
		}

		if (request->RestoreOwner->getCurrentGeneration() == 0 || request->Operand->getRestoreGeneration() != request->RestoreOwner->getCurrentGeneration())
		{
			return DenyE2(VirtualizationE2Decision::RestoreGenerationMismatch, L"E2 restore generation is absent or stale.");
		}

		if (request->DomainContext->getDomainTag() == 0 || request->DomainContext->getDomainTag() != request->E1->getDomainTag() || request->DomainContext->getAddressSpaceTag() != 0)
		{
			return DenyE2(VirtualizationE2Decision::DomainIdentityMismatch, L"Exact no-state E2 requires matching execution domain and absent address-space identity.");
		}

		if (!request->CapabilityOwner->IsLive(request->CapabilityLease))
		{
			return DenyE2(VirtualizationE2Decision::CapabilityLeaseNotLive, L"Typed capability lease is absent, forged, revoked or stale.");
		}

		std::shared_ptr<CapabilityGrant> grant = request->CapabilityLease->getGrant();
		if (ResolveExactGrant(request->DomainContext) != grant || grant->getCapabilityMask() != RuntimeCapabilityIds::VmCallProbeNoStateV1Mask || grant->getScope() != CapabilityGrantScope::DomainGranted || !grant->isGranted() || grant->getOwnerDomainId() != request->DomainContext->getDomainTag() || grant->getDelegationPolicy() != CapabilityDelegationPolicy::NonDelegable || grant->getRevocationPolicy() != CapabilityRevocationPolicy::RuntimeRevocable || grant->getMigrationClass() != CapabilityMigrationClass::DomainLocal || grant->getEvidenceVisibility() != CapabilityEvidenceVisibility::HostOnly || grant->getFrontendProjectionPolicy() != CapabilityFrontendProjectionPolicy::NeverProject)
		{
			return DenyE2(VirtualizationE2Decision::CapabilityPolicyMismatch, L"Capability lease does not match the exact D2 owner policy.");
		}

		std::shared_ptr<RootAuthorityDescriptor> root = request->RootAuthority;
		if (!root->isRuntimeRoot() || root->getAuthorityEpoch() == 0 || !root->HasCapability(RuntimeCapabilityIds::VmCallProbeNoStateV1Mask) || root->getAllowCompatibilityFrontendActivation() || root->getAllowAuthoritativeStateMutation())
		{
			return DenyE2(VirtualizationE2Decision::RootAuthorityMismatch, L"E2 requires a non-mutating runtime root with the exact capability and non-zero epoch.");
		}

		std::shared_ptr<DomainRuntimeOperation> operation = std::make_shared<DomainRuntimeOperation>(DomainRuntimeOperationKind::InvokeHypercall, DomainRuntimeOperationSource::RuntimeService, true, DomainRuntimeOperationAuthorityClass::NoStateExecution);
		std::shared_ptr<RuntimeBoundaryAdmissionResult> commonAdmission = _virtualizationE2BoundaryAdmission->Validate(std::make_shared<RuntimeBoundaryAdmissionRequest>(request->DomainContext, root, nullptr, operation, DomainBoundaryDescriptor::ExecutionOnly, CapabilityBoundaryRequirement::TypedGrant(RuntimeCapabilityIds::VmCallProbeNoStateV1Mask, CapabilityGrantScope::DomainGranted), EvidenceBoundaryRequirement::None));
		if (!commonAdmission->IsAllowed)
		{
			return DenyE2(VirtualizationE2Decision::CommonRuntimeAdmissionDenied, commonAdmission->Message);
		}

		std::shared_ptr<DomainHypercallLifecycleGate::TransitionLease> transition;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: if (request.LifecycleGate.DomainTag != request.DomainContext.DomainTag || !request.LifecycleGate.TryBeginTransition(request.DomainContext.DomainTag, DomainHypercallTransitionKind.NewE2, out DomainHypercallLifecycleGate.TransitionLease? transition) || transition == null)
		if (request->LifecycleGate->getDomainTag() != request->DomainContext->getDomainTag() || !request->LifecycleGate->TryBeginTransition(request->DomainContext->getDomainTag(), DomainHypercallTransitionKind::NewE2, transition) || transition == nullptr)
		{
			return DenyE2(VirtualizationE2Decision::LifecycleGateDenied, L"The exact per-domain lifecycle gate is disabled or draining.");
		}

// C# TO C++ CONVERTER NOTE: The following 'using' block is replaced by its C++ equivalent:
// ORIGINAL LINE: using (transition)
		{
	#if defined(TESTING)
			request->LifecycleGate.NotifyTransitionGapForTesting(DomainHypercallTransitionKind::NewE2);
	#endif

			{
				std::scoped_lock<std::mutex> lock(_virtualizationE2Sync);
				if (_issuedVirtualizationE2ByOperand->Contains(request->Operand))
				{
					return DenyE2(VirtualizationE2Decision::DuplicateAttempt, L"One operand snapshot may receive only one E2 issuance.");
				}

				unsigned long long sequence = AllocateE2Sequence();
				std::shared_ptr<VirtualizationOperationAdmissionCertificate> certificate = VirtualizationOperationAdmissionCertificate::Create(_virtualizationE2IssuerSeal, sequence, request);
				_liveVirtualizationE2->Add(certificate, std::make_shared<LiveVirtualizationE2>(request));
				_issuedVirtualizationE2.insert(certificate);
				_issuedVirtualizationE2ByOperand->Add(request->Operand);
				return std::make_shared<VirtualizationE2Result>(VirtualizationE2Decision::Issued, certificate, L"SafetyVerifier issued exact fault-only E2 admission.");
			}
		}
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal VirtualizationE2Result ValidateVirtualizationE2(VirtualizationOperationAdmissionCertificate? certificate, System.Nullable<VirtualizationRestoreGenerationOwner> restoreOwner)
	std::shared_ptr<VirtualizationE2Result> SafetyVerifier::ValidateVirtualizationE2(std::shared_ptr<VirtualizationOperationAdmissionCertificate> certificate, std::shared_ptr<VirtualizationRestoreGenerationOwner> restoreOwner)
	{
		if (certificate == nullptr)
		{
			return DenyE2(VirtualizationE2Decision::MissingInput, L"E2 certificate is missing.");
		}
		if (!certificate->WasIssuedBy(_virtualizationE2IssuerSeal))
		{
			return DenyE2(VirtualizationE2Decision::CertificateIssuerMismatch, L"E2 issuer mismatch.");
		}
		std::shared_ptr<LiveVirtualizationE2> live;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: if (!_liveVirtualizationE2.TryGetValue(certificate, out LiveVirtualizationE2? live))
		if (!_liveVirtualizationE2->TryGetValue(certificate, live))
		{
			return DenyE2(VirtualizationE2Decision::CertificateNotLive, L"E2 is not present in the issuer live registry.");
		}
		if (live->getState() == VirtualizationE2State::Revoked)
		{
			return DenyE2(VirtualizationE2Decision::Revoked, L"E2 was revoked.");
		}
		if (live->getState() != VirtualizationE2State::Issued)
		{
			return DenyE2(VirtualizationE2Decision::CertificateNotLive, L"E2 is no longer in the issued state.");
		}

		std::shared_ptr<VirtualizationE2IssueRequest> request = live->getRequest();
		if (request->OwnerPolicy != Phase38VirtualizationOperationOwnerSnapshotRegistry::getExactSnapshot())
		{
			return DenyE2(VirtualizationE2Decision::InvalidD2OrOwnerPolicy, L"E2 no longer resolves to the exact accepted O1.");
		}
		if (restoreOwner == nullptr || restoreOwner != request->RestoreOwner || restoreOwner->getCurrentGeneration() == 0 || certificate->getRestoreGeneration() != restoreOwner->getCurrentGeneration())
		{
			return DenyE2(VirtualizationE2Decision::RestoreGenerationMismatch, L"E2 was invalidated by restore generation change.");
		}
	// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
	// ORIGINAL LINE: if (!request.CapabilityOwner!.IsLive(request.CapabilityLease))
		if (!request->CapabilityOwner->IsLive(request->CapabilityLease))
		{
			return DenyE2(VirtualizationE2Decision::CapabilityLeaseNotLive, L"E2 capability generation is no longer live.");
		}
	// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
	// ORIGINAL LINE: VirtualizationAdmissionValidationResult e1Validation = ValidateVirtualizationAdmission( request.ReplayPhase, request.BundleMetadata, request.Carrier!, request.SourceSlotId, request.WorkingSlotId, request.E1);
		std::shared_ptr<VirtualizationAdmissionValidationResult> e1Validation = ValidateVirtualizationAdmission(request->ReplayPhase, request->BundleMetadata, request->Carrier, request->SourceSlotId, request->WorkingSlotId, request->E1);
		if (!e1Validation->IsValidForFaultOnlyTransport)
		{
			return DenyE2(VirtualizationE2Decision::InvalidE1, e1Validation->Reason);
		}
		std::shared_ptr<VirtualizationOperandValidationResult> operandValidation = VirtualizationOperandSnapshotMaterializer::ValidateForE2Input(request->Operand, request->Carrier, request->E1, request->OwnerPolicy, restoreOwner->getCurrentGeneration());
		if (!operandValidation->isValidForE2Input())
		{
			return DenyE2(VirtualizationE2Decision::InvalidOperand, operandValidation->Reason);
		}
	// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
	// ORIGINAL LINE: if (certificate.AttemptId != request.E1!.AttemptId || certificate.E1IssuerGeneration != request.E1.IssuerGeneration || certificate.VirtualThreadId != request.E1.VirtualThreadId || certificate.OwnerContextId != request.E1.OwnerContextId || certificate.DomainTag != request.E1.DomainTag || certificate.BundleIdentity != request.E1.BundleIdentity || certificate.ReplayEpoch != request.E1.ReplayEpoch || certificate.NumericLeaf != request.Operand!.Rs1Value || certificate.CapabilityGrantIdentity != request.CapabilityLease!.GrantIdentity || certificate.CapabilityGeneration != request.CapabilityLease.Generation || certificate.RootAuthorityEpoch != request.RootAuthority!.AuthorityEpoch)
		if (certificate->getAttemptId() != request->E1->getAttemptId() || certificate->getE1IssuerGeneration() != request->E1->getIssuerGeneration() || certificate->getVirtualThreadId() != request->E1->getVirtualThreadId() || certificate->getOwnerContextId() != request->E1->getOwnerContextId() || certificate->getDomainTag() != request->E1->getDomainTag() || certificate->getBundleIdentity() != request->E1->getBundleIdentity() || certificate->getReplayEpoch() != request->E1->getReplayEpoch() || certificate->getNumericLeaf() != request->Operand->getRs1Value() || certificate->getCapabilityGrantIdentity() != request->CapabilityLease->getGrantIdentity() || certificate->getCapabilityGeneration() != request->CapabilityLease->getGeneration() || certificate->getRootAuthorityEpoch() != request->RootAuthority->getAuthorityEpoch())
		{
			return DenyE2(VirtualizationE2Decision::CertificateBindingMismatch, L"E2 binding no longer matches its live inputs.");
		}

		std::wstring digest = VirtualizationE2Digest::Compute(certificate->getIssuanceSequence(), certificate->getAttemptId(), certificate->getE1IssuerGeneration(), certificate->getVirtualThreadId(), certificate->getOwnerContextId(), certificate->getDomainTag(), certificate->getBundleIdentity(), certificate->getReplayEpoch(), certificate->getDecisionId(), certificate->getSpecDigest(), certificate->getAcceptanceDigest(), certificate->getOwnerId(), certificate->getOwnerPolicyVersion(), certificate->getOwnerEpoch(), certificate->getOperationNamespace(), certificate->getOperationId(), certificate->getNumericLeaf(), certificate->getOwnerPolicyDigest(), certificate->getOperandDigest(), certificate->getCapabilityGrantIdentity(), certificate->getCapabilityGeneration(), certificate->getRootAuthorityEpoch(), certificate->getRestoreGeneration());
// C# TO C++ CONVERTER TASK: The following .NET 'String.Equals' reference is not converted:
		if (certificate->getCertificateDigest() != digest)
		{
			return DenyE2(VirtualizationE2Decision::CertificateDigestMismatch, L"E2 canonical digest mismatch.");
		}

		return std::make_shared<VirtualizationE2Result>(VirtualizationE2Decision::Issued, certificate, L"E2 is live and valid; no backend consumer exists in PR-D.");
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal bool RevokeVirtualizationE2(VirtualizationOperationAdmissionCertificate? certificate)
	bool SafetyVerifier::RevokeVirtualizationE2(std::shared_ptr<VirtualizationOperationAdmissionCertificate> certificate)
	{
		{
			std::scoped_lock<std::mutex> lock(_virtualizationE2Sync);
			std::shared_ptr<LiveVirtualizationE2> live;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: if (certificate == null || !certificate.WasIssuedBy(_virtualizationE2IssuerSeal) || !_liveVirtualizationE2.TryGetValue(certificate, out LiveVirtualizationE2? live) || live.State != VirtualizationE2State.Issued)
			if (certificate == nullptr || !certificate->WasIssuedBy(_virtualizationE2IssuerSeal) || !_liveVirtualizationE2->TryGetValue(certificate, live) || live->getState() != VirtualizationE2State::Issued)
			{
				return false;
			}
			live->setState(VirtualizationE2State::Revoked);
			_issuedVirtualizationE2.erase(certificate);
			return true;
		}
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal VirtualizationE2ConsumptionResult ConsumeVirtualizationE2FromExactExecutor(VirtualizationOperationAdmissionCertificate? certificate, System.Nullable<VirtualizationRestoreGenerationOwner> restoreOwner, DomainHypercallLifecycleGate lifecycleGate, Object consumerSeal)
	std::shared_ptr<VirtualizationE2ConsumptionResult> SafetyVerifier::ConsumeVirtualizationE2FromExactExecutor(std::shared_ptr<VirtualizationOperationAdmissionCertificate> certificate, std::shared_ptr<VirtualizationRestoreGenerationOwner> restoreOwner, std::shared_ptr<DomainHypercallLifecycleGate> lifecycleGate, std::any consumerSeal)
	{
		if (!DomainHypercallRuntimeExecutor::IsExactConsumerSeal(consumerSeal))
		{
			return std::make_shared<VirtualizationE2ConsumptionResult>(VirtualizationE2ConsumptionDecision::InvalidConsumer, L"Only the exact neutral runtime executor may consume E2.");
		}

		{
			std::scoped_lock<std::mutex> lock(_virtualizationE2Sync);
			std::shared_ptr<VirtualizationE2Result> validation = ValidateVirtualizationE2(certificate, restoreOwner);
			std::shared_ptr<LiveVirtualizationE2> live;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: if (!validation.IsLive || certificate == null || !_liveVirtualizationE2.TryGetValue(certificate, out LiveVirtualizationE2? live) || live.State != VirtualizationE2State.Issued || !ReferenceEquals(live.Request.LifecycleGate, lifecycleGate))
			if (!validation->IsLive() || certificate == nullptr || !_liveVirtualizationE2->TryGetValue(certificate, live) || live->getState() != VirtualizationE2State::Issued || live->getRequest()->LifecycleGate != lifecycleGate)
			{
				return std::make_shared<VirtualizationE2ConsumptionResult>(VirtualizationE2ConsumptionDecision::InvalidCertificate, validation->Reason);
			}

			live->setState(VirtualizationE2State::ConsumedByExecutor);
			_issuedVirtualizationE2.erase(certificate);
			return std::make_shared<VirtualizationE2ConsumptionResult>(VirtualizationE2ConsumptionDecision::Consumed, L"Exact neutral runtime executor consumed E2 once.");
		}
	}

	VirtualizationE2State SafetyVerifier::GetVirtualizationE2State(std::shared_ptr<VirtualizationOperationAdmissionCertificate> certificate)
	{
		std::shared_ptr<LiveVirtualizationE2> live;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: return _liveVirtualizationE2.TryGetValue(certificate, out LiveVirtualizationE2? live) ? live.State : VirtualizationE2State.Revoked;
		return _liveVirtualizationE2->TryGetValue(certificate, live) ? live->getState() : VirtualizationE2State::Revoked;
	}

	int SafetyVerifier::CountLiveVirtualizationE2(unsigned long long domainTag)
	{
		{
			std::scoped_lock<std::mutex> lock(_virtualizationE2Sync);
			return static_cast<int>(std::count_if(_issuedVirtualizationE2.begin(), _issuedVirtualizationE2.end(), [domainTag](const auto &certificate)
			{
				return certificate != nullptr && certificate->getDomainTag() == domainTag;
			}));
		}
	}

	int SafetyVerifier::CancelLiveVirtualizationE2ForDrain(unsigned long long domainTag)
	{
		{
			std::scoped_lock<std::mutex> lock(_virtualizationE2Sync);
			std::vector<std::shared_ptr<VirtualizationOperationAdmissionCertificate>> cancelled;
			for (const auto &certificate : _issuedVirtualizationE2)
			{
				if (certificate != nullptr && certificate->getDomainTag() == domainTag)
					cancelled.push_back(certificate);
			}
			for (auto certificate : cancelled)
			{
				std::shared_ptr<LiveVirtualizationE2> live;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: if (_liveVirtualizationE2.TryGetValue(certificate, out LiveVirtualizationE2? live) && live.State == VirtualizationE2State.Issued)
				if (_liveVirtualizationE2->TryGetValue(certificate, live) && live->getState() == VirtualizationE2State::Issued)
				{
					live->setState(VirtualizationE2State::Revoked);
				}
				_issuedVirtualizationE2.erase(certificate);
			}
			return cancelled.size();
		}
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal VirtualizationOperationAdmissionResult EvaluateVirtualizationOperationAdmission(VirtualizationOperationAdmissionRequest request, INeutralVirtualizationOperationOwner? owner = null)
	std::shared_ptr<VirtualizationOperationAdmissionResult> SafetyVerifier::EvaluateVirtualizationOperationAdmission(std::shared_ptr<VirtualizationOperationAdmissionRequest> request, std::shared_ptr<INeutralVirtualizationOperationOwner> owner)
	{
		std::shared_ptr<VirtualizationOperationDecisionValidationResult> d2 = VirtualizationOperationDecisionManifestValidator::Validate(std::make_shared<VirtualizationOperationDecisionManifest>(request->DecisionManifest), std::make_shared<VirtualizationDecisionAttributionEvidence>(request->Attribution));
		if (!d2->IsStructurallyValidGovernanceEvidence())
		{
			return DenyLegacy(VirtualizationOperationAdmissionDecision::DeniedD2DecisionArtifact, d2->Reason);
		}
		(void)owner;
		return DenyLegacy(VirtualizationOperationAdmissionDecision::DeniedOwnerInterfaceDisabled, L"Phase-34 boolean E2 evaluation remains disabled; use the typed SafetyVerifier-only v2 contour.");
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private static CapabilityGrant? ResolveExactGrant(DomainRuntimeContext context)
	std::shared_ptr<CapabilityGrant> SafetyVerifier::ResolveExactGrant(std::shared_ptr<DomainRuntimeContext> context)
	{
		std::shared_ptr<CapabilityGrant> grant;
		return context->getCapabilities()->getTypedGrants()->TryGetGrant(RuntimeCapabilityIds::VmCallProbeNoStateV1Mask, CapabilityGrantScope::DomainGranted, grant) ? grant : nullptr;
	}

	unsigned long long SafetyVerifier::AllocateE2Sequence()
	{
		unsigned long long sequence = _nextVirtualizationE2Sequence++;
		if (sequence == 0)
		{
			sequence = _nextVirtualizationE2Sequence++;
		}
		return sequence;
	}

	std::shared_ptr<VirtualizationE2Result> SafetyVerifier::DenyE2(VirtualizationE2Decision decision, const std::wstring &reason)
	{
		return std::make_shared<VirtualizationE2Result>(decision, nullptr, reason);
	}

	std::shared_ptr<VirtualizationOperationAdmissionResult> SafetyVerifier::DenyLegacy(VirtualizationOperationAdmissionDecision decision, const std::wstring &reason)
	{
		return std::make_shared<VirtualizationOperationAdmissionResult>(decision, nullptr, reason);
	}

	bool SafetyVerifier::IsCurrentMemoryReadCertificate(std::shared_ptr<SecureComputeMemoryReadCertificate> certificate)
	{
		if (certificate == nullptr || secureCertificateScopeExhausted || certificate->getGeneration() != secureCertificateGeneration || !memoryReadCertificates.contains(certificate))
			return false;
		const auto request = certificate->getRequest();
		const auto reservation = certificate->getReservation();
		const auto origin = certificate->getOrigin();
		const auto issued = certificate->getIssued();
		if (!request || !reservation || !origin || !issued || reservation->Request != request || origin->Region != request->Region)
			return false;
		const auto revoked = std::any_of(revokedSecureCertificateDomains.begin(), revokedSecureCertificateDomains.end(), [&request](const auto &domain)
		{
			return domain != nullptr && domain->Equals(request->Binding.SecureDomain);
		});
		if (revoked) return false;
		const auto ownerCurrent = certificate->getOwnerCurrent();
		return (!ownerCurrent || ownerCurrent()) &&
			SameMutexIdentity(origin->SynchronizationRoot, certificate->getSourceGate()) &&
			SameMutexIdentity(origin->ProviderSynchronizationRoot, certificate->getProviderGate()) &&
			issued->isCurrent() && issued->getExecutionRecord() &&
			issued->getExecutionRecord()->getState() == Execution::ExecutionRecordState::Issued &&
			origin->IsCurrent(reservation);
	}

	bool SafetyVerifier::RegisterMemoryReadPermit(std::shared_ptr<SecureComputeMemoryReadCertificate> certificate)
	{
		if (!IsCurrentMemoryReadCertificate(certificate) || memoryReadCertificates[certificate])
		{
			return false;
		}
		memoryReadCertificates[certificate] = true;
		return true;
	}

	bool SafetyVerifier::HasConsumedMemoryReadPermit(std::shared_ptr<SecureComputeMemoryReadIssuePermit> permit)
	{
		return getSecureIssuePermits()->HasConsumed(permit);
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public SecureComputeMemoryReadMachineResult ExecuteSecureMemoryRead(ExternalSecureMemoryReadRequest request, ExternalSecureMemoryReadReservation reservation, IExternalSecureMemoryReadProvider backend, IExternalSecureMemoryReadOrigin originalSource, System.Func<bool>? ownerCurrent = null)
	std::shared_ptr<SecureComputeMemoryReadMachineResult> SafetyVerifier::ExecuteSecureMemoryRead(std::shared_ptr<ExternalSecureMemoryReadRequest> request, std::shared_ptr<ExternalSecureMemoryReadReservation> reservation, std::shared_ptr<IExternalSecureMemoryReadProvider> backend, std::shared_ptr<IExternalSecureMemoryReadOrigin> originalSource, std::function<bool()> ownerCurrent)
	{
		ArgumentNullException::ThrowIfNull(request);
		ArgumentNullException::ThrowIfNull(reservation);
		ArgumentNullException::ThrowIfNull(backend);
		ArgumentNullException::ThrowIfNull(originalSource);
		std::shared_ptr<IExternalSecureMemoryReadOriginProvider> origins = std::dynamic_pointer_cast<IExternalSecureMemoryReadOriginProvider>(backend);
		if (origins == nullptr || reservation->Request != request)
		{
			return std::make_shared<SecureComputeMemoryReadMachineResult>(request, SecureComputeMemoryReadDecision::Denied);
		}
		// Backend lookup is outside machine/source locks and must match the host's independently retained owner.
		try
		{
			if (origins->ResolveMemoryReadOrigin(reservation) != originalSource)
			{
				return std::make_shared<SecureComputeMemoryReadMachineResult>(request, SecureComputeMemoryReadDecision::Denied);
			}
		}
		catch (...)
		{
			return std::make_shared<SecureComputeMemoryReadMachineResult>(request, SecureComputeMemoryReadDecision::CleanupRequired);
		}
		std::any sourceGate, providerGate, memoryGate;
		try
		{
			sourceGate = originalSource->SynchronizationRoot;
			providerGate = originalSource->ProviderSynchronizationRoot;
			memoryGate = originalSource->MemorySynchronizationRoot;
		}
		catch (...)
		{
			return std::make_shared<SecureComputeMemoryReadMachineResult>(request, SecureComputeMemoryReadDecision::Denied);
		}
		if (!sourceGate.has_value() || !providerGate.has_value() || !memoryGate.has_value())
		{
			return std::make_shared<SecureComputeMemoryReadMachineResult>(request, SecureComputeMemoryReadDecision::Denied);
		}
		auto sourceMutex = GetMutex(sourceGate);
		auto providerMutex = GetMutex(providerGate);
		auto memoryMutex = GetMutex(memoryGate);
		if (!sourceMutex || !providerMutex || !memoryMutex)
			return std::make_shared<SecureComputeMemoryReadMachineResult>(request, SecureComputeMemoryReadDecision::Denied);
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: SecureComputeMemoryReadCertificate? certificate;
		std::shared_ptr<SecureComputeMemoryReadCertificate> certificate;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: SecureComputeMemoryReadIssuePermit? permit;
		std::shared_ptr<SecureComputeMemoryReadIssuePermit> permit;
		{
			std::scoped_lock<std::recursive_mutex> lock(secureRegisterLocalGate);
			{
				std::scoped_lock<std::mutex> lock(*sourceMutex);
				{
					std::scoped_lock<std::mutex> lock(*providerMutex);
					std::scoped_lock<std::mutex> memoryLock(*memoryMutex);
            if (memoryReadOperations.find(request->Operation) != memoryReadOperations.end())
                return std::make_shared<SecureComputeMemoryReadMachineResult>(request, SecureComputeMemoryReadDecision::Denied);
			if (memoryReadOperations.size() >= 1024)
			{
				return std::make_shared<SecureComputeMemoryReadMachineResult>(request, SecureComputeMemoryReadDecision::Denied);
			}
			memoryReadOperations.emplace(request->Operation, request);
			if (ownerCurrent && !ownerCurrent())
			{
				return std::make_shared<SecureComputeMemoryReadMachineResult>(request, SecureComputeMemoryReadDecision::Denied);
			}
			certificate = PrepareMemoryRead(request, reservation, originalSource, sourceGate, providerGate, ownerCurrent);
			if (certificate == nullptr)
			{
				return std::make_shared<SecureComputeMemoryReadMachineResult>(request, SecureComputeMemoryReadDecision::Denied);
			}
			permit = getSecureIssuePermits()->Register(certificate);
			if (permit == nullptr)
			{
				memoryReadCertificates.erase(certificate);
				return std::make_shared<SecureComputeMemoryReadMachineResult>(request, SecureComputeMemoryReadDecision::Denied);
			}
			auto scheduled = certificate->getIssued()->getScheduledOperation();
			auto operationId = scheduled->getOperationId();
			auto admission = scheduled->getAdmission();
			certificate->Attempt = std::make_shared<ExternalSecureMemoryReadAttempt>(reservation, getSecureRegisterLocalScope(), certificate->getGeneration(), operationId->OperationAttempt, operationId->WorkingBundleSequence, operationId->VirtualThreadId, admission->OwnerContextId, admission->SourceProvenance->SourceSlotIndex, operationId->WorkingSlotIndex, scheduled->getPhysicalLane());
				}
			}
		}
	// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
	// ORIGINAL LINE: var attempt = certificate.Attempt!;
		auto attempt = certificate->Attempt.value();
		std::optional<std::shared_ptr<ExternalSecureMemoryReadConsumptionResult>> consumption;
		std::optional<std::shared_ptr<ExternalSecureMemoryReadSubmission>> submission;
		std::optional<std::shared_ptr<ExternalSecureMemoryReadResult>> outcome;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: SecureComputeMemoryReadCompletionOwner.Pending? pending = null;
		std::shared_ptr<SecureComputeMemoryReadCompletionOwner::Pending> pending = nullptr;
		ScopeExit cleanup{[&]()
		{
			std::scoped_lock<std::recursive_mutex> lock(secureRegisterLocalGate);
			memoryReadCertificates.erase(certificate);
			if (pending && memoryReadCompletion) memoryReadCompletion->Cancel(pending);
			certificate->getIssued()->InvalidateAfterPipelineDiscard();
			if (secureIssuePermits) getSecureIssuePermits()->PruneMemoryRead();
		}};
		auto result = [&](SecureComputeMemoryReadDecision decision, std::shared_ptr<SecureComputeMemoryReadRetirement> retirement = nullptr)
		{
			return std::make_shared<SecureComputeMemoryReadMachineResult>(request, decision, certificate->getCanonical(), certificate->getIssued()->getScheduledOperation(), certificate->Attempt, consumption, submission, outcome, retirement);
		};
		try
			{
				consumption = backend->ConsumeMemoryRead(attempt);
			}
			catch (...)
			{
				return result(SecureComputeMemoryReadDecision::CleanupRequired);
			}
			std::optional<std::shared_ptr<ExternalSecureMemoryReadConsumptionResult>> original;
			try
			{
				original = backend->QueryMemoryReadConsumption(attempt);
			}
			catch (...)
			{
				return result(SecureComputeMemoryReadDecision::CleanupRequired);
			}
// C# TO C++ CONVERTER TASK: Cannot test for object identity between 2 non-pointer objects in C++:
			if (!consumption || !original || original.value() != consumption.value() || consumption.value()->Attempt != attempt)
			{
				return result(SecureComputeMemoryReadDecision::CleanupRequired);
			}
			if (consumption.value()->Receipt == nullptr || consumption.value()->Status != ExternalSecureDomainProviderStatus::Receipt)
			{
				return result(SecureComputeMemoryReadDecision::Denied);
			}
			submission = std::make_shared<ExternalSecureMemoryReadSubmission>(consumption.value()->Receipt);
			{
				std::scoped_lock<std::recursive_mutex> lock(secureRegisterLocalGate);
				{
					std::scoped_lock<std::mutex> lock(*sourceMutex);
					{
						std::scoped_lock<std::mutex> lock(*providerMutex);
				if (!getSecureIssuePermits()->Consume(permit))
				{
					return result(SecureComputeMemoryReadDecision::Denied);
				}
				memoryReadCompletion = (memoryReadCompletion != nullptr) ? memoryReadCompletion : std::make_shared<SecureComputeMemoryReadCompletionOwner>(shared_from_this(), secureRegisterLocalGate);
				pending = memoryReadCompletion->Begin(certificate, permit, submission.value());
				if (pending == nullptr)
				{
					return result(SecureComputeMemoryReadDecision::Denied);
				}
					}
				}
			}
			try
			{
				outcome = backend->SubmitMemoryRead(submission.value());
			}
			catch (...)
			{
				return result(SecureComputeMemoryReadDecision::CleanupRequired);
			}
			std::optional<std::shared_ptr<ExternalSecureMemoryReadResult>> observed;
			try
			{
				observed = backend->QueryMemoryRead(submission.value());
			}
			catch (...)
			{
				return result(SecureComputeMemoryReadDecision::CleanupRequired);
			}
// C# TO C++ CONVERTER TASK: Cannot test for object identity between 2 non-pointer objects in C++:
			if (!outcome || !observed || observed.value() != outcome.value() || outcome.value()->Submission != submission.value())
			{
				return result(SecureComputeMemoryReadDecision::CleanupRequired);
			}
			if (outcome.value()->Receipt == nullptr || outcome.value()->Status != ExternalSecureDomainProviderStatus::Receipt)
			{
				return result(SecureComputeMemoryReadDecision::Denied);
			}
			{
				std::scoped_lock<std::recursive_mutex> lock(secureRegisterLocalGate);
				{
					std::scoped_lock<std::mutex> lock(*sourceMutex);
					{
						std::scoped_lock<std::mutex> lock(*providerMutex);
						{
							std::scoped_lock<std::mutex> lock(*memoryMutex);
				if (!getSecureIssuePermits()->HasConsumed(permit) || !originalSource->IsOriginalRead(outcome.value()->Receipt))
				{
					return result(SecureComputeMemoryReadDecision::Denied);
				}
	// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
	// ORIGINAL LINE: var retired = memoryReadCompletion!.AcceptAndRetire(pending, outcome.Receipt);
					auto retired = memoryReadCompletion->AcceptAndRetire(pending, outcome.value()->Receipt);
					return result(retired == nullptr ? SecureComputeMemoryReadDecision::Denied : SecureComputeMemoryReadDecision::LoadRetired, retired);
						}
					}
				}
			}
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private SecureComputeMemoryReadCertificate? PrepareMemoryRead(ExternalSecureMemoryReadRequest request, ExternalSecureMemoryReadReservation reservation, IExternalSecureMemoryReadOrigin origin, Object sourceGate, Object providerGate, System.Nullable<System.Func<bool>> ownerCurrent)
	std::shared_ptr<SecureComputeMemoryReadCertificate> SafetyVerifier::PrepareMemoryRead(std::shared_ptr<ExternalSecureMemoryReadRequest> request, std::shared_ptr<ExternalSecureMemoryReadReservation> reservation, std::shared_ptr<IExternalSecureMemoryReadOrigin> origin, std::any sourceGate, std::any providerGate, std::function<bool()> ownerCurrent)
	{
		const bool revoked = std::any_of(revokedSecureCertificateDomains.begin(), revokedSecureCertificateDomains.end(), [&request](const auto &domain)
		{
			return domain != nullptr && domain->Equals(request->Binding.SecureDomain);
		});
		if (secureCertificateScopeExhausted || revoked || memoryReadCertificates.size() + secureCertificates.size() >= 8 || origin->Region != request->Region || request->Region->Length < 8 || request->Region->Length > 4096 || (request->Address & 7) != 0 || request->Address >= 0xFFFF000000000000ULL || request->Address < request->Region->Address || request->Address - request->Region->Address > request->Region->Length - 8 || !origin->IsCurrent(reservation) || secureRegisterLocalSequence == std::numeric_limits<unsigned long long>::max())
		{
			return nullptr;
		}
		std::vector<VLIW_Instruction> raw(8);
		raw[0].setOpCode(Processor::CPU_Core::IsaOpcodeValues::LD);
		raw[0].setWord1(VLIW_Instruction::PackArchRegs(3, 1, 0));
		std::optional<std::shared_ptr<DeclarativeDecodedBundle>> decoded;
		std::optional<std::shared_ptr<DecodeFailure>> failure;
		if (!DeclarativeDecoderPipeline::TryDecodeBundle(std::span<const VLIW_Instruction>(raw), nullptr, 0, 0, decoded, failure) || !decoded || !decoded.value())
		{
			return nullptr;
		}
		auto decodedBundle = decoded.value();
		auto canonicalBundle = decodedBundle->CanonicalBundleValue;
		auto canonical = canonicalBundle ? canonicalBundle->GetSlot(0) : nullptr;
		auto loadPlan = canonical ? canonical->ScalarLoadAddressPlan : nullptr;
		if (!canonical || !loadPlan || loadPlan->getAccessSize() != 8 || loadPlan->getBaseRegisterId() != 1 || loadPlan->getDestinationRegisterId() != 3 || loadPlan->getSignedDisplacement() != 0 || canonical->getSecurityOperationKind() != CanonicalSecurityOperationKind::Ordinary)
		{
			return nullptr;
		}
		auto memory = MemoryCapability::Create(MemoryCapabilityKind::Load, std::vector<FrozenMemoryRange>{FrozenMemoryRange(request->Address, 8)}, MemoryBankId(0));
		auto contract = SecureComputeMemoryReadContractProjection::Resolve(canonical, memory, 1);
		auto carrier = std::make_shared<LoadMicroOp>();
		carrier->setOpCode(canonical->Opcode);
		carrier->setAddress(request->Address);
		carrier->setSize(8);
		carrier->setBaseRegID(1);
		carrier->setDestRegID(3);
		carrier->setWritesRegister(true);
		carrier->setOwnerThreadId(1);
		carrier->setVirtualThreadId(1);
		carrier->setOwnerContextId(1);
		carrier->BindStandaloneSecureReadBank(request->Address - request->Region->Address, request->Region->Length);
		carrier->InitializeMetadata();
		carrier->BindSoftwareSecureSource();
		unsigned long long sequence = ++secureRegisterLocalSequence;
		auto provenance = std::make_shared<SourceOperationProvenance>(canonicalBundle->getSemanticKey(), 1, sequence, SlotId::Create(0), sequence);
		auto admission = AdmissionRecord::Create(provenance, contract, 1, 1, 0);
		auto scheduler = std::make_shared<MicroOpScheduler>();
		scheduler->setTypedSlotEnabled(true);
		std::shared_ptr<MicroOp> carrierBase = carrier;
		auto route = Rf06ScalarSchedulerRouting::Route(scheduler, admission, carrierBase, std::vector<std::shared_ptr<MicroOp>>(8), 0, 0, 2, sequence, 0, secureRegisterLocalAttemptIssuer);
		auto issued = carrier->getPostStageBIssuedAttempt();
		auto scheduled = issued ? issued->getScheduledOperation() : nullptr;
		if (!route->isScheduled() || !issued || !issued->isCurrent() || !scheduled || scheduled->getAdmission() != admission || (SlotClassLaneMap::GetLaneMask(SlotClass::LsuClass) & (1 << scheduled->getPhysicalLane())) == 0 || !origin->IsCurrent(reservation))
		{
			return nullptr;
		}
		auto certificate = std::make_shared<SecureComputeMemoryReadCertificate>(request, reservation, canonicalBundle, issued, origin, sourceGate, providerGate, secureCertificateGeneration, ownerCurrent);
		memoryReadCertificates.emplace(certificate, false);
		return certificate;
	}
}


