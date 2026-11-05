#pragma once

#include "VirtualizationDecisionContractsV2.h"
#include <concepts>
#include <memory>
#include <span>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <vector>

namespace YAKSys_Hybrid_CPU::Core
{
    class VirtualizationDecisionCanonicalEncoderV2 final
    {
    public:
        static constexpr unsigned short EncodingVersion = 1;
        static std::vector<unsigned char> EncodeSpecPayload(const std::shared_ptr<VirtualizationDecisionSpecV2>& spec);
        static std::vector<unsigned char> EncodeSpec(const std::shared_ptr<VirtualizationDecisionSpecV2>& spec);
        static std::wstring ComputeSpecDigest(const std::shared_ptr<VirtualizationDecisionSpecV2>& spec);
        static std::vector<unsigned char> EncodeAcceptancePayload(const std::shared_ptr<VirtualizationDecisionAcceptanceRecordV2>& record);
        static std::vector<unsigned char> EncodeAcceptance(const std::shared_ptr<VirtualizationDecisionAcceptanceRecordV2>& record);
        static std::wstring ComputeAcceptanceDigest(const std::shared_ptr<VirtualizationDecisionAcceptanceRecordV2>& record);
        static std::vector<unsigned char> EncodeRevocationPayload(const std::shared_ptr<VirtualizationDecisionRevocationRecordV2>& record);
        static std::vector<unsigned char> EncodeRevocation(const std::shared_ptr<VirtualizationDecisionRevocationRecordV2>& record);
        static std::wstring ComputeRevocationDigest(const std::shared_ptr<VirtualizationDecisionRevocationRecordV2>& record);
        static std::vector<unsigned char> EncodeSupersessionPayload(const std::shared_ptr<VirtualizationDecisionSupersessionRecordV2>& record);
        static std::vector<unsigned char> EncodeSupersession(const std::shared_ptr<VirtualizationDecisionSupersessionRecordV2>& record);
        static std::wstring ComputeSupersessionDigest(const std::shared_ptr<VirtualizationDecisionSupersessionRecordV2>& record);
        static bool IsCanonicalSha256(const std::wstring& value);
        static bool IsCanonicalCommitSha(const std::wstring& value);

    private:
        class CanonicalWriter;
        static void WriteSpecFields(CanonicalWriter& writer, const std::shared_ptr<VirtualizationDecisionSpecV2>& spec);
        static void WriteAcceptanceFields(CanonicalWriter& writer, const std::shared_ptr<VirtualizationDecisionAcceptanceRecordV2>& record);
        static void WriteRevocationFields(CanonicalWriter& writer, const std::shared_ptr<VirtualizationDecisionRevocationRecordV2>& record);
        static void WriteSupersessionFields(CanonicalWriter& writer, const std::shared_ptr<VirtualizationDecisionSupersessionRecordV2>& record);
        static std::wstring ComputeSha256Hex(const std::vector<unsigned char>& bytes);

        class CanonicalWriter final
        {
            static constexpr unsigned char TypeUInt16 = 1;
            static constexpr unsigned char TypeUInt32 = 2;
            static constexpr unsigned char TypeUInt64 = 3;
            static constexpr unsigned char TypeBoolean = 4;
            static constexpr unsigned char TypeEnum = 5;
            static constexpr unsigned char TypeUtf8 = 6;
            static constexpr unsigned char TypeSha1 = 7;
            static constexpr unsigned char TypeSha256 = 8;
            static constexpr unsigned char TypeCollection = 9;
            static constexpr unsigned char TypeOptional = 10;
            std::vector<unsigned char> buffer_;
        public:
            explicit CanonicalWriter(unsigned char artifactKind);
            void WriteUInt16(unsigned short tag, unsigned short value);
            void WriteUInt32(unsigned short tag, unsigned int value);
            void WriteUInt64(unsigned short tag, unsigned long long value);
            void WriteUInt16Collection(unsigned short tag, const std::vector<unsigned short>& values);
            void WriteBoolean(unsigned short tag, bool value);
            template<typename TEnum> requires std::is_enum_v<TEnum>
            void WriteEnum(unsigned short tag, TEnum value)
            {
                std::vector<unsigned char> payload;
                WriteBigEndian(payload, static_cast<unsigned int>(value));
                WriteField(tag, TypeEnum, payload);
            }
            void WriteString(unsigned short tag, const std::wstring& value);
            void WriteCommitSha(unsigned short tag, const std::wstring& value);
            void WriteSha256(unsigned short tag, const std::wstring& value);
            void WriteOptionalString(unsigned short tag, const std::shared_ptr<const std::wstring>& value);
            void WriteOptionalSha256(unsigned short tag, const std::shared_ptr<const std::wstring>& value);
            void WriteOwnerMap(unsigned short tag, const std::vector<std::shared_ptr<VirtualizationDecisionOwnerMapEntryV2>>& ownerMap);
            void WriteReviewEvidence(unsigned short tag, const VirtualizationDecisionReviewEvidenceV2& evidence);
            std::vector<unsigned char> ToVector() const;
        private:
            void WriteField(unsigned short tag, unsigned char type, std::span<const unsigned char> payload);
            void WriteRaw(std::span<const unsigned char> value);
            void WriteStringRaw(std::vector<unsigned char>& writer, const std::wstring& value);
            void WriteRawWithLength(std::vector<unsigned char>& writer, std::span<const unsigned char> value);
            static void WriteUInt32Raw(std::vector<unsigned char>& writer, unsigned int value);
            template<typename TUnsigned> requires std::is_unsigned_v<TUnsigned>
            static void WriteBigEndian(std::vector<unsigned char>& destination, TUnsigned value)
            {
                for (int shift = static_cast<int>(sizeof(TUnsigned) * 8) - 8; shift >= 0; shift -= 8)
                    destination.push_back(static_cast<unsigned char>((value >> shift) & static_cast<TUnsigned>(0xff)));
            }
            static std::wstring RequireSha256(const std::wstring& value);
            static std::wstring RequireCommitSha(const std::wstring& value);
        };
    };
}
