#pragma once
#include <memory>
#include <span>
#include <string>

namespace YAKSys_Hybrid_CPU::Core::Decoder {
enum class DecodeFailureCode : unsigned short {
    Unknown=0, UnknownOpcode=1, ProhibitedOpcode=2, ReservedEncoding=3,
    OperandEncoding=4, ExtensionPayload=5, Sideband=6, BundleShape=7, UnsupportedOpcode=8
};
struct DecodeFailure final {
    const DecodeFailureCode Code;
    const int SlotIndex;
    const std::wstring Field, RawHash, Message;
    DecodeFailure(DecodeFailureCode code, int slotIndex, std::wstring field, std::wstring rawHash, std::wstring message);
    static std::shared_ptr<DecodeFailure> Create(DecodeFailureCode code, int slotIndex, const std::wstring &field,
        std::span<const unsigned char> rawBytes, const std::wstring &message);
};
}
