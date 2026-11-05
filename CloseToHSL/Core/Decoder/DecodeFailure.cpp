#include "DecodeFailure.h"
#include "Sha256Digest.h"
#include <algorithm>
#include <cwctype>
#include <stdexcept>
#include <utility>

namespace YAKSys_Hybrid_CPU::Core::Decoder {
DecodeFailure::DecodeFailure(DecodeFailureCode code, int slotIndex, std::wstring field, std::wstring rawHash, std::wstring message)
    : Code(code), SlotIndex(slotIndex), Field(std::move(field)), RawHash(std::move(rawHash)), Message(std::move(message)) {}
std::shared_ptr<DecodeFailure> DecodeFailure::Create(DecodeFailureCode code, int slotIndex, const std::wstring &field,
    std::span<const unsigned char> rawBytes, const std::wstring &message) {
    auto blank=[](const std::wstring &s){ return s.empty() || std::all_of(s.begin(),s.end(),[](wchar_t c){return std::iswspace(c)!=0;}); };
    if (blank(field) || blank(message)) throw std::invalid_argument("field and message must not be blank");
    return std::make_shared<DecodeFailure>(code, slotIndex, field, Sha256Hex(rawBytes), message);
}
}
