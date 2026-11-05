#include "VirtualizationOperandSnapshot.h"
#include "../../../Pipeline/MicroOps/Types/MicroOp.IO.h"
#include "../../../Decoder/Sha256Digest.h"
#include <stdexcept>
#include "../../../Virtualization/Compatibility/Frontend/Retire/VmxRetireModel.h"
#include "../../../Architecture/Common/YAKSys_Hybrid_CPU.Processor.h"
#include "../../../Architecture/Registers/Physical/PhysicalRegisterFile.h"
#include "../../Governance/Virtualization/VirtualizationDecisionValidatorV2.h"

namespace YAKSys_Hybrid_CPU::Core
{

	VirtualizationOperandSnapshot::VirtualizationOperandSnapshot(unsigned long long attemptId, unsigned long long e1IssuerGeneration, int virtualThreadId, int ownerContextId, unsigned long long domainTag, unsigned char rs1Selector, unsigned long long rs1Value, unsigned char rs2Selector, unsigned long long rs2Value, unsigned char rdSelector, int sourceSlotId, int workingSlotId, unsigned long long bundleIdentity, unsigned long long replayEpoch, unsigned long long carrierIdentityDigest, unsigned long long restoreGeneration, unsigned long long captureSequence, const std::wstring &ownerPolicyDigest, const std::wstring &operandDigest)
		: AttemptId(attemptId),
		  E1IssuerGeneration(e1IssuerGeneration),
		  VirtualThreadId(virtualThreadId),
		  OwnerContextId(ownerContextId),
		  DomainTag(domainTag),
		  Rs1Selector(rs1Selector),
		  Rs1Value(rs1Value),
		  Rs2Selector(rs2Selector),
		  Rs2Value(rs2Value),
		  RdSelector(rdSelector),
		  SourceSlotId(sourceSlotId),
		  WorkingSlotId(workingSlotId),
		  BundleIdentity(bundleIdentity),
		  ReplayEpoch(replayEpoch),
		  CarrierIdentityDigest(carrierIdentityDigest),
		  RestoreGeneration(restoreGeneration),
		  CaptureSequence(captureSequence),
		  OwnerPolicyDigest(ownerPolicyDigest),
		  OperandDigest(operandDigest)
	{
	}

	unsigned int VirtualizationOperandSnapshot::getSchemaVersion() const
	{
		return 1;
	}

	unsigned long long VirtualizationOperandSnapshot::getAttemptId() const
	{
		return AttemptId;
	}

	unsigned long long VirtualizationOperandSnapshot::getE1IssuerGeneration() const
	{
		return E1IssuerGeneration;
	}

	int VirtualizationOperandSnapshot::getVirtualThreadId() const
	{
		return VirtualThreadId;
	}

	int VirtualizationOperandSnapshot::getOwnerContextId() const
	{
		return OwnerContextId;
	}

	unsigned long long VirtualizationOperandSnapshot::getDomainTag() const
	{
		return DomainTag;
	}

	unsigned char VirtualizationOperandSnapshot::getRs1Selector() const
	{
		return Rs1Selector;
	}

	unsigned long long VirtualizationOperandSnapshot::getRs1Value() const
	{
		return Rs1Value;
	}

	unsigned char VirtualizationOperandSnapshot::getRs2Selector() const
	{
		return Rs2Selector;
	}

	unsigned long long VirtualizationOperandSnapshot::getRs2Value() const
	{
		return Rs2Value;
	}

	unsigned char VirtualizationOperandSnapshot::getRdSelector() const
	{
		return RdSelector;
	}

	int VirtualizationOperandSnapshot::getSourceSlotId() const
	{
		return SourceSlotId;
	}

	int VirtualizationOperandSnapshot::getWorkingSlotId() const
	{
		return WorkingSlotId;
	}

	unsigned long long VirtualizationOperandSnapshot::getBundleIdentity() const
	{
		return BundleIdentity;
	}

	unsigned long long VirtualizationOperandSnapshot::getReplayEpoch() const
	{
		return ReplayEpoch;
	}

	unsigned long long VirtualizationOperandSnapshot::getCarrierIdentityDigest() const
	{
		return CarrierIdentityDigest;
	}

	unsigned long long VirtualizationOperandSnapshot::getRestoreGeneration() const
	{
		return RestoreGeneration;
	}

	unsigned long long VirtualizationOperandSnapshot::getCaptureSequence() const
	{
		return CaptureSequence;
	}

	std::wstring VirtualizationOperandSnapshot::getOwnerPolicyDigest() const
	{
		return OwnerPolicyDigest;
	}

	std::wstring VirtualizationOperandSnapshot::getOperandDigest() const
	{
		return OperandDigest;
	}

	bool VirtualizationOperandSnapshot::getBackendExecutionAuthorized() const
	{
		return false;
	}

	bool VirtualizationOperandSnapshot::getCompletionPublicationAuthorized() const
	{
		return false;
	}

	bool VirtualizationOperandSnapshot::getRetirePublicationAuthorized() const
	{
		return false;
	}

	std::shared_ptr<VirtualizationOperandSnapshot> VirtualizationOperandSnapshot::CreateValidated(std::shared_ptr<SafetyVerifier::VirtualizationAdmissionCertificate> e1, unsigned char rs1Selector, unsigned long long rs1Value, unsigned long long restoreGeneration, unsigned long long captureSequence, std::shared_ptr<VirtualizationOperationOwnerSnapshot> ownerPolicy)
	{
		std::wstring digest = VirtualizationOperandSnapshotDigest::Compute(e1->getAttemptId(), e1->getIssuerGeneration(), e1->getVirtualThreadId(), e1->getOwnerContextId(), e1->getDomainTag(), rs1Selector, rs1Value, 0, 0, 0, e1->getSourceSlotId(), e1->getWorkingSlotId(), e1->getBundleIdentity(), e1->getReplayEpoch(), e1->getCarrierIdentityDigest(), restoreGeneration, captureSequence, ownerPolicy->getPolicyDigest());
		return std::shared_ptr<VirtualizationOperandSnapshot>(new VirtualizationOperandSnapshot(e1->getAttemptId(), e1->getIssuerGeneration(), e1->getVirtualThreadId(), e1->getOwnerContextId(), e1->getDomainTag(), rs1Selector, rs1Value, 0, 0, 0, e1->getSourceSlotId(), e1->getWorkingSlotId(), e1->getBundleIdentity(), e1->getReplayEpoch(), e1->getCarrierIdentityDigest(), restoreGeneration, captureSequence, ownerPolicy->getPolicyDigest(), digest));
	}

	std::wstring VirtualizationOperandSnapshotDigest::Compute(unsigned long long attemptId, unsigned long long issuerGeneration, int virtualThreadId, int ownerContextId, unsigned long long domainTag, unsigned char rs1Selector, unsigned long long rs1Value, unsigned char rs2Selector, unsigned long long rs2Value, unsigned char rdSelector, int sourceSlotId, int workingSlotId, unsigned long long bundleIdentity, unsigned long long replayEpoch, unsigned long long carrierIdentityDigest, unsigned long long restoreGeneration, unsigned long long captureSequence, const std::wstring &ownerPolicyDigest)
	{
        std::vector<unsigned char> bytes = Envelope;
        WriteUInt64(bytes, attemptId); WriteUInt64(bytes, issuerGeneration);
        WriteUInt32(bytes, static_cast<unsigned int>(virtualThreadId));
        WriteUInt32(bytes, static_cast<unsigned int>(ownerContextId));
        WriteUInt64(bytes, domainTag); bytes.push_back(rs1Selector); WriteUInt64(bytes, rs1Value);
        bytes.push_back(rs2Selector); WriteUInt64(bytes, rs2Value); bytes.push_back(rdSelector);
        WriteUInt32(bytes, static_cast<unsigned int>(sourceSlotId));
        WriteUInt32(bytes, static_cast<unsigned int>(workingSlotId));
        WriteUInt64(bytes, bundleIdentity); WriteUInt64(bytes, replayEpoch);
        WriteUInt64(bytes, carrierIdentityDigest); WriteUInt64(bytes, restoreGeneration);
        WriteUInt64(bytes, captureSequence);
        if (ownerPolicyDigest.size() % 2 != 0) throw std::invalid_argument("Invalid hexadecimal digest");
        auto digit = [](wchar_t c) -> unsigned char {
            if (c >= L'0' && c <= L'9') return static_cast<unsigned char>(c - L'0');
            if (c >= L'a' && c <= L'f') return static_cast<unsigned char>(c - L'a' + 10);
            if (c >= L'A' && c <= L'F') return static_cast<unsigned char>(c - L'A' + 10);
            throw std::invalid_argument("Invalid hexadecimal digest");
        };
        for (size_t i = 0; i < ownerPolicyDigest.size(); i += 2)
            bytes.push_back(static_cast<unsigned char>((digit(ownerPolicyDigest[i]) << 4) | digit(ownerPolicyDigest[i + 1])));
        return YAKSys_Hybrid_CPU::Core::Decoder::Sha256Hex(bytes);
    }
    void VirtualizationOperandSnapshotDigest::WriteUInt32(std::vector<unsigned char> &stream, unsigned int value)
    {
        for (int shift = 24; shift >= 0; shift -= 8) stream.push_back(static_cast<unsigned char>(value >> shift));
    }
    void VirtualizationOperandSnapshotDigest::WriteUInt64(std::vector<unsigned char> &stream, unsigned long long value)
    {
        for (int shift = 56; shift >= 0; shift -= 8) stream.push_back(static_cast<unsigned char>(value >> shift));
    }

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal VirtualizationOperandCaptureResult CaptureAfterValidatedE1(VmxMicroOp? carrier, System.Nullable<SafetyVerifier.VirtualizationAdmissionCertificate> e1, ulong rs1Value, ulong restoreGeneration, System.Nullable<VirtualizationOperationOwnerSnapshot> ownerPolicy)
	std::shared_ptr<VirtualizationOperandCaptureResult> VirtualizationOperandSnapshotMaterializer::CaptureAfterValidatedE1(std::shared_ptr<VmxMicroOp> carrier, std::shared_ptr<SafetyVerifier::VirtualizationAdmissionCertificate> e1, unsigned long long rs1Value, unsigned long long restoreGeneration, std::shared_ptr<VirtualizationOperationOwnerSnapshot> ownerPolicy)
	{
		if (carrier == nullptr || e1 == nullptr || ownerPolicy == nullptr)
		{
			return Deny(VirtualizationOperandCaptureDecision::MissingInput, L"Operand capture requires carrier, live E1 and O1.");
		}

		if (carrier->getVirtualizationAdmission() != e1)
		{
			return Deny(VirtualizationOperandCaptureDecision::InvalidE1, L"Operand capture requires the E1 attached to this exact carrier.");
		}

		VmxOperationKind operation;
		if (e1->getOpcode() != Processor::CPU_Core::IsaOpcodeValues::VMCALL || e1->getOperation() != VmxOperationKind::VmCall || !carrier->TryResolveFrozenOperation(operation) || operation != VmxOperationKind::VmCall)
		{
			return Deny(VirtualizationOperandCaptureDecision::NotExactVmCall, L"Operand capture accepts only the canonical VMCALL carrier.");
		}

		if (carrier->getRs1() == 0 || carrier->getRs1() >= Registers::RenameMap::ArchRegs || carrier->getRs2() != 0 || carrier->getRd() != 0)
		{
			return Deny(VirtualizationOperandCaptureDecision::InvalidRegisterShape, L"VMCALL requires Rs1=architectural register, Rs2=x0 and Rd=x0.");
		}

		if ((rs1Value & ~0xFFFFULL) != 0)
		{
			return Deny(VirtualizationOperandCaptureDecision::LeafHighBitsSet, L"VMCALL numeric leaf has non-zero bits above the accepted width.");
		}

		if (rs1Value == 0 || rs1Value != ownerPolicy->getNumericLeaf())
		{
			return Deny(VirtualizationOperandCaptureDecision::InvalidLeafValue, L"VMCALL numeric leaf is zero, adjacent or otherwise not the O1 exact leaf.");
		}

		if (e1->getDomainTag() == 0)
		{
			return Deny(VirtualizationOperandCaptureDecision::MissingDomainIdentity, L"Operand capture requires a non-zero E1 domain tag.");
		}

		if (e1->getSourceSlotId() != 7 || e1->getWorkingSlotId() != 7 || carrier->getPlacement().PinnedLaneId != 7)
		{
			return Deny(VirtualizationOperandCaptureDecision::InvalidSlotIdentity, L"Operand capture requires the canonical system-singleton source and working slot.");
		}

		if (restoreGeneration == 0)
		{
			return Deny(VirtualizationOperandCaptureDecision::MissingRestoreGeneration, L"Operand capture requires a non-zero restore generation.");
		}

// C# TO C++ CONVERTER TASK: The following .NET 'String.Equals' reference is not converted:
		if (ownerPolicy->getOperationNamespace() != VirtualizationDecisionValidatorV2::ExpectedOperationNamespace || ownerPolicy->getOperationId() != VirtualizationDecisionValidatorV2::ExpectedOperationId || ownerPolicy->getNumericLeaf() != 1 || ownerPolicy->getPolicyDigest() != Phase38VirtualizationOperationOwnerSnapshotRegistry::getExactSnapshot()->getPolicyDigest())
		{
			return Deny(VirtualizationOperandCaptureDecision::OwnerPolicyMismatch, L"Operand capture requires the exact loaded O1 policy.");
		}

		std::lock_guard<std::mutex> lock(_captureGate);
        for (auto it = _captured.begin(); it != _captured.end();) { if (it->expired()) it = _captured.erase(it); else ++it; }
        if (_captured.find(e1) != _captured.end())
		{
			return Deny(VirtualizationOperandCaptureDecision::DuplicateAttempt, L"An E1 attempt can materialize operands only once.");
		}

		unsigned long long captureSequence = AllocateCaptureSequence();
		std::shared_ptr<VirtualizationOperandSnapshot> snapshot = VirtualizationOperandSnapshot::CreateValidated(e1, carrier->getRs1(), rs1Value, restoreGeneration, captureSequence, ownerPolicy);
		_captured.insert(e1);
		return std::make_shared<VirtualizationOperandCaptureResult>(VirtualizationOperandCaptureDecision::Captured, snapshot, L"Canonical Rs1 value captured once after E1 validation.");
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal static VirtualizationOperandValidationResult ValidateForE2Input(VirtualizationOperandSnapshot? snapshot, System.Nullable<VmxMicroOp> carrier, System.Nullable<SafetyVerifier.VirtualizationAdmissionCertificate> e1, System.Nullable<VirtualizationOperationOwnerSnapshot> ownerPolicy, ulong currentRestoreGeneration)
	std::shared_ptr<VirtualizationOperandValidationResult> VirtualizationOperandSnapshotMaterializer::ValidateForE2Input(std::shared_ptr<VirtualizationOperandSnapshot> snapshot, std::shared_ptr<VmxMicroOp> carrier, std::shared_ptr<SafetyVerifier::VirtualizationAdmissionCertificate> e1, std::shared_ptr<VirtualizationOperationOwnerSnapshot> ownerPolicy, unsigned long long currentRestoreGeneration)
	{
		if (snapshot == nullptr || carrier == nullptr || e1 == nullptr || ownerPolicy == nullptr)
		{
			return Invalid(VirtualizationOperandValidationDecision::MissingInput, L"Operand validation requires snapshot, carrier, E1 and O1.");
		}

		if (carrier->getVirtualizationAdmission() != e1 || carrier->getVirtualizationOperandSnapshot() != snapshot)
		{
			return Invalid(VirtualizationOperandValidationDecision::CarrierOrE1Mismatch, L"Operand snapshot is not attached to this exact E1 carrier.");
		}

// C# TO C++ CONVERTER TASK: The following .NET 'String.Equals' reference is not converted:
		if (snapshot->getOwnerPolicyDigest() != ownerPolicy->getPolicyDigest() || ownerPolicy->getPolicyDigest() != Phase38VirtualizationOperationOwnerSnapshotRegistry::getExactSnapshot()->getPolicyDigest())
		{
			return Invalid(VirtualizationOperandValidationDecision::OwnerPolicyMismatch, L"Operand snapshot O1 identity is stale or foreign.");
		}

		if (currentRestoreGeneration == 0 || snapshot->getRestoreGeneration() != currentRestoreGeneration)
		{
			return Invalid(VirtualizationOperandValidationDecision::RestoreGenerationMismatch, L"Operand snapshot was invalidated by restore generation change.");
		}

		if (snapshot->getAttemptId() != e1->getAttemptId() || snapshot->getE1IssuerGeneration() != e1->getIssuerGeneration() || snapshot->getVirtualThreadId() != e1->getVirtualThreadId() || snapshot->getOwnerContextId() != e1->getOwnerContextId() || snapshot->getDomainTag() == 0 || snapshot->getDomainTag() != e1->getDomainTag() || snapshot->getRs1Selector() != carrier->getRs1() || snapshot->getRs1Value() != ownerPolicy->getNumericLeaf() || snapshot->getRs2Selector() != 0 || snapshot->getRs2Value() != 0 || snapshot->getRdSelector() != 0 || carrier->getRs2() != 0 || carrier->getRd() != 0 || snapshot->getSourceSlotId() != e1->getSourceSlotId() || snapshot->getWorkingSlotId() != e1->getWorkingSlotId() || snapshot->getBundleIdentity() != e1->getBundleIdentity() || snapshot->getReplayEpoch() != e1->getReplayEpoch() || snapshot->getCarrierIdentityDigest() != e1->getCarrierIdentityDigest() || snapshot->getCaptureSequence() == 0)
		{
			return Invalid(VirtualizationOperandValidationDecision::IdentityMismatch, L"Operand snapshot identity no longer matches E1, O1 or the canonical carrier.");
		}

		std::wstring digest = VirtualizationOperandSnapshotDigest::Compute(snapshot->getAttemptId(), snapshot->getE1IssuerGeneration(), snapshot->getVirtualThreadId(), snapshot->getOwnerContextId(), snapshot->getDomainTag(), snapshot->getRs1Selector(), snapshot->getRs1Value(), snapshot->getRs2Selector(), snapshot->getRs2Value(), snapshot->getRdSelector(), snapshot->getSourceSlotId(), snapshot->getWorkingSlotId(), snapshot->getBundleIdentity(), snapshot->getReplayEpoch(), snapshot->getCarrierIdentityDigest(), snapshot->getRestoreGeneration(), snapshot->getCaptureSequence(), snapshot->getOwnerPolicyDigest());
// C# TO C++ CONVERTER TASK: The following .NET 'String.Equals' reference is not converted:
		if (snapshot->getOperandDigest() != digest)
		{
			return Invalid(VirtualizationOperandValidationDecision::DigestMismatch, L"Operand snapshot canonical digest mismatch.");
		}

		return std::make_shared<VirtualizationOperandValidationResult>(VirtualizationOperandValidationDecision::ValidForE2Input, L"Operand snapshot is immutable, current and eligible only as future E2 input.");
	}

	unsigned long long VirtualizationOperandSnapshotMaterializer::AllocateCaptureSequence()
	{
		unsigned long long sequence = _nextCaptureSequence++;
		if (sequence == 0)
		{
			sequence = _nextCaptureSequence++;
		}
		return sequence;
	}

	std::shared_ptr<VirtualizationOperandCaptureResult> VirtualizationOperandSnapshotMaterializer::Deny(VirtualizationOperandCaptureDecision decision, const std::wstring &reason)
	{
		return std::make_shared<VirtualizationOperandCaptureResult>(decision, nullptr, reason);
	}

	std::shared_ptr<VirtualizationOperandValidationResult> VirtualizationOperandSnapshotMaterializer::Invalid(VirtualizationOperandValidationDecision decision, const std::wstring &reason)
	{
		return std::make_shared<VirtualizationOperandValidationResult>(decision, reason);
	}

	unsigned long long VirtualizationRestoreGenerationOwner::getCurrentGeneration() const
	{
		{
		std::scoped_lock<std::mutex> lock(_gate);
			return _currentGeneration;
		}
	}

	void VirtualizationRestoreGenerationOwner::AdvanceAfterRestore()
	{
		{
			std::scoped_lock<std::mutex> lock(_gate);
			// Exhaustion must refuse the restore before architectural publication,
			// rather than reissuing an identity held by an earlier incarnation.
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'checked' in this context:
// ORIGINAL LINE: _currentGeneration = checked(_currentGeneration + 1);
			_currentGeneration = _currentGeneration + 1;
		}
	}
}
