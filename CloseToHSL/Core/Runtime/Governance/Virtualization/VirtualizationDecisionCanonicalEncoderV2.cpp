#include "VirtualizationDecisionCanonicalEncoderV2.h"
#include "../../../Decoder/Sha256Digest.h"
#include <Windows.h>
#include <algorithm>
#include <limits>
#include <stdexcept>
#include <tuple>

namespace YAKSys_Hybrid_CPU::Core
{
namespace
{
    constexpr unsigned char SpecArtifact = 1;
    constexpr unsigned char AcceptanceArtifact = 2;
    constexpr unsigned char RevocationArtifact = 3;
    constexpr unsigned char SupersessionArtifact = 4;

    std::vector<unsigned char> Utf8(const std::wstring& value)
    {
        if (value.empty()) return {};
        const int required = WideCharToMultiByte(CP_UTF8, 0, value.data(), static_cast<int>(value.size()), nullptr, 0, nullptr, nullptr);
        if (required <= 0) throw std::invalid_argument("String cannot be encoded as UTF-8.");
        std::vector<unsigned char> bytes(static_cast<size_t>(required));
        const int written = WideCharToMultiByte(CP_UTF8, 0, value.data(), static_cast<int>(value.size()), reinterpret_cast<char*>(bytes.data()), required, nullptr, nullptr);
        if (written != required) throw std::invalid_argument("String cannot be encoded as UTF-8.");
        return bytes;
    }

    std::vector<unsigned char> FromHex(const std::wstring& value)
    {
        auto nibble = [](wchar_t c) -> unsigned char {
            if (c >= L'0' && c <= L'9') return static_cast<unsigned char>(c - L'0');
            if (c >= L'a' && c <= L'f') return static_cast<unsigned char>(c - L'a' + 10);
            throw std::invalid_argument("Hexadecimal input is not canonical lowercase ASCII.");
        };
        if ((value.size() & 1U) != 0) throw std::invalid_argument("Hexadecimal input must contain complete bytes.");
        std::vector<unsigned char> bytes;
        bytes.reserve(value.size() / 2);
        for (size_t i = 0; i < value.size(); i += 2)
            bytes.push_back(static_cast<unsigned char>((nibble(value[i]) << 4) | nibble(value[i + 1])));
        return bytes;
    }
}

std::vector<unsigned char> VirtualizationDecisionCanonicalEncoderV2::EncodeSpecPayload(const std::shared_ptr<VirtualizationDecisionSpecV2>& spec)
{
    if (!spec) throw std::invalid_argument("Spec is required.");
    CanonicalWriter writer(SpecArtifact);
    WriteSpecFields(writer, spec);
    return writer.ToVector();
}

std::vector<unsigned char> VirtualizationDecisionCanonicalEncoderV2::EncodeSpec(const std::shared_ptr<VirtualizationDecisionSpecV2>& spec)
{
    if (!spec) throw std::invalid_argument("Spec is required.");
    CanonicalWriter writer(SpecArtifact);
    WriteSpecFields(writer, spec);
    writer.WriteSha256(44, spec->SpecDigest);
    return writer.ToVector();
}

std::wstring VirtualizationDecisionCanonicalEncoderV2::ComputeSpecDigest(const std::shared_ptr<VirtualizationDecisionSpecV2>& spec)
{
    return ComputeSha256Hex(EncodeSpecPayload(spec));
}

std::vector<unsigned char> VirtualizationDecisionCanonicalEncoderV2::EncodeAcceptancePayload(const std::shared_ptr<VirtualizationDecisionAcceptanceRecordV2>& record)
{
    if (!record) throw std::invalid_argument("Acceptance record is required.");
    CanonicalWriter writer(AcceptanceArtifact);
    WriteAcceptanceFields(writer, record);
    return writer.ToVector();
}

std::vector<unsigned char> VirtualizationDecisionCanonicalEncoderV2::EncodeAcceptance(const std::shared_ptr<VirtualizationDecisionAcceptanceRecordV2>& record)
{
    if (!record) throw std::invalid_argument("Acceptance record is required.");
    CanonicalWriter writer(AcceptanceArtifact);
    WriteAcceptanceFields(writer, record);
    writer.WriteSha256(14, record->AcceptanceDigest);
    return writer.ToVector();
}

std::wstring VirtualizationDecisionCanonicalEncoderV2::ComputeAcceptanceDigest(const std::shared_ptr<VirtualizationDecisionAcceptanceRecordV2>& record)
{
    return ComputeSha256Hex(EncodeAcceptancePayload(record));
}

std::vector<unsigned char> VirtualizationDecisionCanonicalEncoderV2::EncodeRevocationPayload(const std::shared_ptr<VirtualizationDecisionRevocationRecordV2>& record)
{
    if (!record) throw std::invalid_argument("Revocation record is required.");
    CanonicalWriter writer(RevocationArtifact);
    WriteRevocationFields(writer, record);
    return writer.ToVector();
}

std::vector<unsigned char> VirtualizationDecisionCanonicalEncoderV2::EncodeRevocation(const std::shared_ptr<VirtualizationDecisionRevocationRecordV2>& record)
{
    if (!record) throw std::invalid_argument("Revocation record is required.");
    CanonicalWriter writer(RevocationArtifact);
    WriteRevocationFields(writer, record);
    writer.WriteSha256(10, record->RevocationDigest);
    return writer.ToVector();
}

std::wstring VirtualizationDecisionCanonicalEncoderV2::ComputeRevocationDigest(const std::shared_ptr<VirtualizationDecisionRevocationRecordV2>& record)
{
    return ComputeSha256Hex(EncodeRevocationPayload(record));
}

std::vector<unsigned char> VirtualizationDecisionCanonicalEncoderV2::EncodeSupersessionPayload(const std::shared_ptr<VirtualizationDecisionSupersessionRecordV2>& record)
{
    if (!record) throw std::invalid_argument("Supersession record is required.");
    CanonicalWriter writer(SupersessionArtifact);
    WriteSupersessionFields(writer, record);
    return writer.ToVector();
}

std::vector<unsigned char> VirtualizationDecisionCanonicalEncoderV2::EncodeSupersession(const std::shared_ptr<VirtualizationDecisionSupersessionRecordV2>& record)
{
    if (!record) throw std::invalid_argument("Supersession record is required.");
    CanonicalWriter writer(SupersessionArtifact);
    WriteSupersessionFields(writer, record);
    writer.WriteSha256(11, record->SupersessionDigest);
    return writer.ToVector();
}

std::wstring VirtualizationDecisionCanonicalEncoderV2::ComputeSupersessionDigest(const std::shared_ptr<VirtualizationDecisionSupersessionRecordV2>& record)
{
    return ComputeSha256Hex(EncodeSupersessionPayload(record));
}

bool VirtualizationDecisionCanonicalEncoderV2::IsCanonicalSha256(const std::wstring& value)
{
    return value.size() == 64 && std::all_of(value.begin(), value.end(), [](wchar_t c) { return (c >= L'0' && c <= L'9') || (c >= L'a' && c <= L'f'); });
}

bool VirtualizationDecisionCanonicalEncoderV2::IsCanonicalCommitSha(const std::wstring& value)
{
    return value.size() == 40 && std::all_of(value.begin(), value.end(), [](wchar_t c) { return (c >= L'0' && c <= L'9') || (c >= L'a' && c <= L'f'); });
}

void VirtualizationDecisionCanonicalEncoderV2::WriteSpecFields(CanonicalWriter& writer, const std::shared_ptr<VirtualizationDecisionSpecV2>& spec)
{
    writer.WriteUInt32(1, spec->SchemaVersion); writer.WriteString(2, spec->DecisionId); writer.WriteString(3, spec->OperationNamespace);
    writer.WriteUInt16(4, spec->LeafWidth); writer.WriteUInt16(5, spec->InvalidLeaf); writer.WriteUInt16(6, spec->NumericLeaf);
    writer.WriteString(7, spec->OperationId); writer.WriteEnum(8, spec->OwnerClass); writer.WriteUInt64(9, spec->OwnerId);
    writer.WriteUInt32(10, spec->OwnerPolicyVersion); writer.WriteUInt32(11, spec->OwnerEpoch); writer.WriteUInt32(12, spec->OperandAbiVersion);
    writer.WriteString(13, spec->Rs1Contract); writer.WriteString(14, spec->Rs2Contract); writer.WriteString(15, spec->RdContract);
    writer.WriteEnum(16, spec->ResultAbi); writer.WriteEnum(17, spec->EffectClass); writer.WriteEnum(18, spec->CapabilityRequirement);
    writer.WriteUInt64(19, spec->CapabilityMask); writer.WriteBoolean(20, spec->RequiresTypedGrant);
    writer.WriteEnum(21, spec->DelegationPolicy); writer.WriteEnum(22, spec->RevocationPolicy); writer.WriteEnum(23, spec->CapabilityMigrationClass);
    writer.WriteEnum(24, spec->EvidenceVisibility); writer.WriteEnum(25, spec->FrontendProjectionPolicy);
    writer.WriteEnum(26, spec->ExecutionEvidenceRequirement); writer.WriteEnum(27, spec->DomainRequirement);
    writer.WriteBoolean(28, spec->RequireNonZeroDomainTag); writer.WriteBoolean(29, spec->RequiresMemoryDomain);
    writer.WriteBoolean(30, spec->RequiresIoDomain); writer.WriteEnum(31, spec->AddressSpaceRequirement);
    writer.WriteEnum(32, spec->SecureDomainPolicy); writer.WriteEnum(33, spec->CancellationPolicy); writer.WriteEnum(34, spec->ReplayPolicy);
    writer.WriteEnum(35, spec->OperationMigrationPolicy); writer.WriteEnum(36, spec->CompletionEvidenceClass);
    writer.WriteEnum(37, spec->CompletionMigrationClass); writer.WriteEnum(38, spec->CompletionProjectionPolicy);
    writer.WriteEnum(39, spec->CompletionPolicy); writer.WriteEnum(40, spec->RetirePolicy); writer.WriteEnum(41, spec->AdjacentLeafPolicy);
    writer.WriteEnum(42, spec->CrossNamespacePolicy); writer.WriteOwnerMap(43, spec->OwnerMap);
    const bool hasProjectionProfile = spec->OperationClass != VirtualizationDecisionOperationClassV2::Unspecified ||
        spec->AuthorityPlane != VirtualizationDecisionAuthorityPlaneV2::Unspecified || spec->ExactFieldIds.has_value() ||
        spec->MutationClass != VirtualizationDecisionMutationClassV2::Unspecified || !spec->DependencyContract.empty() ||
        spec->VmcsMetadataOnly || spec->RequiresConformanceProof;
    if (hasProjectionProfile) {
        writer.WriteEnum(45, spec->OperationClass); writer.WriteEnum(46, spec->AuthorityPlane);
        writer.WriteUInt16Collection(47, spec->ExactFieldIds.value_or(std::vector<unsigned short>{}));
        writer.WriteEnum(48, spec->MutationClass); writer.WriteString(49, spec->DependencyContract);
        writer.WriteBoolean(50, spec->VmcsMetadataOnly); writer.WriteBoolean(51, spec->RequiresConformanceProof);
    }
}

void VirtualizationDecisionCanonicalEncoderV2::WriteAcceptanceFields(CanonicalWriter& writer, const std::shared_ptr<VirtualizationDecisionAcceptanceRecordV2>& record)
{
    writer.WriteUInt32(1, record->SchemaVersion); writer.WriteString(2, record->DecisionId); writer.WriteSha256(3, record->SpecDigest);
    writer.WriteCommitSha(4, record->SpecCommitSha); writer.WriteEnum(5, record->AcceptanceState); writer.WriteString(6, record->AcceptedBy);
    writer.WriteUInt32(7, record->AcceptancePolicyVersion); if (!record->OwnerReviewEvidence) throw std::invalid_argument("Owner review evidence is required.");
    writer.WriteReviewEvidence(8, *record->OwnerReviewEvidence);
    if (!record->ArchitectureReviewEvidence) throw std::invalid_argument("Architecture review evidence is required.");
    writer.WriteReviewEvidence(9, *record->ArchitectureReviewEvidence); writer.WriteCommitSha(10, record->CodeOwnersBlobSha);
    writer.WriteOptionalString(11, record->SupersedesDecisionId); writer.WriteOptionalSha256(12, record->SupersedesAcceptanceDigest);
}

void VirtualizationDecisionCanonicalEncoderV2::WriteRevocationFields(CanonicalWriter& writer, const std::shared_ptr<VirtualizationDecisionRevocationRecordV2>& record)
{
    writer.WriteUInt32(1, record->SchemaVersion); writer.WriteString(2, record->RevocationId); writer.WriteString(3, record->DecisionId);
    writer.WriteSha256(4, record->AcceptanceDigest); writer.WriteEnum(5, record->State); writer.WriteString(6, record->RevokedBy);
    writer.WriteString(7, record->Reason); writer.WriteUInt64(8, record->Sequence);
}

void VirtualizationDecisionCanonicalEncoderV2::WriteSupersessionFields(CanonicalWriter& writer, const std::shared_ptr<VirtualizationDecisionSupersessionRecordV2>& record)
{
    writer.WriteUInt32(1, record->SchemaVersion); writer.WriteString(2, record->SupersessionId); writer.WriteString(3, record->SupersededDecisionId);
    writer.WriteSha256(4, record->SupersededAcceptanceDigest); writer.WriteString(5, record->SupersedingDecisionId);
    writer.WriteSha256(6, record->SupersedingAcceptanceDigest); writer.WriteEnum(7, record->State);
    writer.WriteString(8, record->SupersededBy); writer.WriteUInt64(9, record->Sequence);
}

std::wstring VirtualizationDecisionCanonicalEncoderV2::ComputeSha256Hex(const std::vector<unsigned char>& bytes)
{
    return Decoder::Sha256Hex(std::span<const unsigned char>(bytes.data(), bytes.size()));
}

VirtualizationDecisionCanonicalEncoderV2::CanonicalWriter::CanonicalWriter(unsigned char artifactKind)
{
    static constexpr unsigned char magic[] = {'H','C','P','U','V','D','2',0};
    buffer_.insert(buffer_.end(), magic, magic + sizeof(magic));
    buffer_.push_back(artifactKind);
    buffer_.push_back(static_cast<unsigned char>((EncodingVersion >> 8) & 0xff));
    buffer_.push_back(static_cast<unsigned char>(EncodingVersion & 0xff));
}

void VirtualizationDecisionCanonicalEncoderV2::CanonicalWriter::WriteUInt16(unsigned short tag, unsigned short value)
{
    std::vector<unsigned char> payload; WriteBigEndian(payload, value); WriteField(tag, TypeUInt16, payload);
}
void VirtualizationDecisionCanonicalEncoderV2::CanonicalWriter::WriteUInt32(unsigned short tag, unsigned int value)
{
    std::vector<unsigned char> payload; WriteBigEndian(payload, value); WriteField(tag, TypeUInt32, payload);
}
void VirtualizationDecisionCanonicalEncoderV2::CanonicalWriter::WriteUInt64(unsigned short tag, unsigned long long value)
{
    std::vector<unsigned char> payload; WriteBigEndian(payload, value); WriteField(tag, TypeUInt64, payload);
}
void VirtualizationDecisionCanonicalEncoderV2::CanonicalWriter::WriteUInt16Collection(unsigned short tag, const std::vector<unsigned short>& values)
{
    std::vector<unsigned short> ordered(values); std::sort(ordered.begin(), ordered.end());
    std::vector<unsigned char> payload; WriteUInt32Raw(payload, static_cast<unsigned int>(ordered.size()));
    for (auto value : ordered) WriteBigEndian(payload, value);
    WriteField(tag, TypeCollection, payload);
}
void VirtualizationDecisionCanonicalEncoderV2::CanonicalWriter::WriteBoolean(unsigned short tag, bool value)
{
    const unsigned char payload[1] = {static_cast<unsigned char>(value ? 1 : 0)}; WriteField(tag, TypeBoolean, payload);
}
void VirtualizationDecisionCanonicalEncoderV2::CanonicalWriter::WriteString(unsigned short tag, const std::wstring& value)
{
    const auto bytes = Utf8(value); WriteField(tag, TypeUtf8, bytes);
}
void VirtualizationDecisionCanonicalEncoderV2::CanonicalWriter::WriteCommitSha(unsigned short tag, const std::wstring& value)
{
    const auto bytes = FromHex(RequireCommitSha(value)); WriteField(tag, TypeSha1, bytes);
}
void VirtualizationDecisionCanonicalEncoderV2::CanonicalWriter::WriteSha256(unsigned short tag, const std::wstring& value)
{
    const auto bytes = FromHex(RequireSha256(value)); WriteField(tag, TypeSha256, bytes);
}
void VirtualizationDecisionCanonicalEncoderV2::CanonicalWriter::WriteOptionalString(unsigned short tag, const std::shared_ptr<const std::wstring>& value)
{
    std::vector<unsigned char> payload; payload.push_back(static_cast<unsigned char>(value ? 1 : 0));
    if (value) { auto bytes = Utf8(*value); payload.insert(payload.end(), bytes.begin(), bytes.end()); }
    WriteField(tag, TypeOptional, payload);
}
void VirtualizationDecisionCanonicalEncoderV2::CanonicalWriter::WriteOptionalSha256(unsigned short tag, const std::shared_ptr<const std::wstring>& value)
{
    std::vector<unsigned char> payload; payload.push_back(static_cast<unsigned char>(value ? 1 : 0));
    if (value) { auto bytes = FromHex(RequireSha256(*value)); payload.insert(payload.end(), bytes.begin(), bytes.end()); }
    WriteField(tag, TypeOptional, payload);
}
void VirtualizationDecisionCanonicalEncoderV2::CanonicalWriter::WriteOwnerMap(unsigned short tag, const std::vector<std::shared_ptr<VirtualizationDecisionOwnerMapEntryV2>>& ownerMap)
{
    std::vector<std::shared_ptr<VirtualizationDecisionOwnerMapEntryV2>> ordered(ownerMap);
    if (std::any_of(ordered.begin(), ordered.end(), [](const auto& value) { return !value; })) throw std::invalid_argument("Owner-map entries cannot be null.");
    std::sort(ordered.begin(), ordered.end(), [](const auto& a, const auto& b) { return a->FieldOrOperation < b->FieldOrOperation; });
    std::vector<unsigned char> payload; WriteUInt32Raw(payload, static_cast<unsigned int>(ordered.size()));
    for (const auto& entry : ordered) {
        WriteStringRaw(payload, entry->FieldOrOperation); WriteStringRaw(payload, entry->Owner); WriteStringRaw(payload, entry->ValueSource);
        WriteStringRaw(payload, entry->CapabilityPolicy); WriteStringRaw(payload, entry->EvidenceClass); WriteStringRaw(payload, entry->MigrationClass); WriteStringRaw(payload, entry->DenialReason);
    }
    WriteField(tag, TypeCollection, payload);
}
void VirtualizationDecisionCanonicalEncoderV2::CanonicalWriter::WriteReviewEvidence(unsigned short tag, const VirtualizationDecisionReviewEvidenceV2& evidence)
{
    std::vector<unsigned char> payload; WriteUInt32Raw(payload, static_cast<unsigned int>(evidence.Role));
    WriteUInt32Raw(payload, static_cast<unsigned int>(evidence.AuthorityPlane)); WriteUInt32Raw(payload, static_cast<unsigned int>(evidence.State));
    WriteStringRaw(payload, evidence.Principal); WriteStringRaw(payload, evidence.ReviewedDecisionId);
    auto digest = FromHex(RequireSha256(evidence.ReviewedSpecDigest)); WriteRawWithLength(payload, digest);
    auto sha = FromHex(RequireCommitSha(evidence.ReviewedSpecCommitSha)); WriteRawWithLength(payload, sha);
    WriteStringRaw(payload, evidence.EvidenceId); WriteField(tag, TypeCollection, payload);
}
std::vector<unsigned char> VirtualizationDecisionCanonicalEncoderV2::CanonicalWriter::ToVector() const { return buffer_; }
void VirtualizationDecisionCanonicalEncoderV2::CanonicalWriter::WriteField(unsigned short tag, unsigned char type, std::span<const unsigned char> payload)
{
    if (payload.size() > (std::numeric_limits<unsigned int>::max)()) throw std::length_error("Canonical field is too large.");
    std::vector<unsigned char> header; header.reserve(7); WriteBigEndian(header, tag); header.push_back(type); WriteBigEndian(header, static_cast<unsigned int>(payload.size()));
    WriteRaw(header); WriteRaw(payload);
}
void VirtualizationDecisionCanonicalEncoderV2::CanonicalWriter::WriteRaw(std::span<const unsigned char> value) { buffer_.insert(buffer_.end(), value.begin(), value.end()); }
void VirtualizationDecisionCanonicalEncoderV2::CanonicalWriter::WriteStringRaw(std::vector<unsigned char>& writer, const std::wstring& value)
{
    auto bytes = Utf8(value); WriteRawWithLength(writer, bytes);
}
void VirtualizationDecisionCanonicalEncoderV2::CanonicalWriter::WriteRawWithLength(std::vector<unsigned char>& writer, std::span<const unsigned char> value)
{
    if (value.size() > (std::numeric_limits<unsigned int>::max)()) throw std::length_error("Canonical payload is too large.");
    WriteUInt32Raw(writer, static_cast<unsigned int>(value.size())); writer.insert(writer.end(), value.begin(), value.end());
}
void VirtualizationDecisionCanonicalEncoderV2::CanonicalWriter::WriteUInt32Raw(std::vector<unsigned char>& writer, unsigned int value) { WriteBigEndian(writer, value); }
std::wstring VirtualizationDecisionCanonicalEncoderV2::CanonicalWriter::RequireSha256(const std::wstring& value)
{
    if (!IsCanonicalSha256(value)) throw std::invalid_argument("SHA-256 must be 64 lowercase hexadecimal characters."); return value;
}
std::wstring VirtualizationDecisionCanonicalEncoderV2::CanonicalWriter::RequireCommitSha(const std::wstring& value)
{
    if (!IsCanonicalCommitSha(value)) throw std::invalid_argument("Commit SHA must be 40 lowercase hexadecimal characters."); return value;
}
}
