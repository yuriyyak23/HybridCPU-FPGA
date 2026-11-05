#pragma once
#include <cwctype>
#include <memory>
#include <string>

namespace YAKSys_Hybrid_CPU::Core
{

	enum class NeutralHypercallBackendOwnerSource : unsigned char
	{
		None = 0,
		NeutralRuntimeOwner = 1,
		CompatibilityProjection = 2,
		CompatibilityStateVocabulary = 3,
		CapabilityProjection = 4,
		MigrationCheckpoint = 5,
		LaneStreamBoundary = 6,
		SecureComputeBoundary = 7,
		CompilerEmission = 8
	};

	enum class NeutralHypercallBackendOwnerRfcAdrState : unsigned char
	{
		Missing = 0,
		DraftOnly = 1
	};

	enum class NeutralHypercallBackendOperationClass : unsigned char
	{
		Unknown = 0,
		NoStateNoPayloadDomainLocal = 1
	};

	enum class NeutralHypercallBackendLeafSelection : unsigned char
	{
		Missing = 0,
		CandidateOnlyNoNumericLeaf = 1
	};

struct NeutralHypercallBackendOwnerDescriptor {
    const unsigned long long OwnerId; const NeutralHypercallBackendOwnerSource Source; const std::wstring RfcAdrId; const NeutralHypercallBackendOwnerRfcAdrState RfcAdrState;
    const NeutralHypercallBackendOperationClass OperationClass; const NeutralHypercallBackendLeafSelection LeafSelection; const bool NoPayloadOnly, DomainLocalOnly, NegativeTestsPresent;
    NeutralHypercallBackendOwnerDescriptor(unsigned long long id, NeutralHypercallBackendOwnerSource source, std::wstring rfc, NeutralHypercallBackendOwnerRfcAdrState state, NeutralHypercallBackendOperationClass op, NeutralHypercallBackendLeafSelection leaf, bool noPayload, bool local, bool tests) : OwnerId(id), Source(source), RfcAdrId(std::move(rfc)), RfcAdrState(state), OperationClass(op), LeafSelection(leaf), NoPayloadOnly(noPayload), DomainLocalOnly(local), NegativeTestsPresent(tests) {}
    static constexpr const wchar_t* CandidateRfcAdrId = L"RFC-HV-VMCALL-NO-STATE-OWNER-0001";
    static std::shared_ptr<NeutralHypercallBackendOwnerDescriptor> DraftNoStateCandidate(unsigned long long id, bool tests=true) { return std::make_shared<NeutralHypercallBackendOwnerDescriptor>(id,NeutralHypercallBackendOwnerSource::NeutralRuntimeOwner,CandidateRfcAdrId,NeutralHypercallBackendOwnerRfcAdrState::DraftOnly,NeutralHypercallBackendOperationClass::NoStateNoPayloadDomainLocal,NeutralHypercallBackendLeafSelection::CandidateOnlyNoNumericLeaf,true,true,tests); }
    bool isMaterialized() const { if (!OwnerId || Source == NeutralHypercallBackendOwnerSource::None) return false; for (auto c : RfcAdrId) if (!std::iswspace(c)) return true; return false; }
    bool isNeutralRuntimeOwner() const { return Source == NeutralHypercallBackendOwnerSource::NeutralRuntimeOwner; }
    bool isCandidateDraft() const { return RfcAdrId == CandidateRfcAdrId && RfcAdrState == NeutralHypercallBackendOwnerRfcAdrState::DraftOnly; }
    bool hasNoStateCandidateShape() const { return OperationClass == NeutralHypercallBackendOperationClass::NoStateNoPayloadDomainLocal && LeafSelection == NeutralHypercallBackendLeafSelection::CandidateOnlyNoNumericLeaf && NoPayloadOnly && DomainLocalOnly; }
};
}