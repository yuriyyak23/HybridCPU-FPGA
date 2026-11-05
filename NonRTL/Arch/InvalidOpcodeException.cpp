#include "InvalidOpcodeException.h"
#include <codecvt>
#include <locale>

namespace YAKSys_Hybrid_CPU::Arch
{

	std::wstring InvalidOpcodeException::getOpcodeIdentifier() const
	{
		return OpcodeIdentifier;
	}

	int InvalidOpcodeException::getSlotIndex() const
	{
		return SlotIndex;
	}

	bool InvalidOpcodeException::isProhibited() const
	{
		return IsProhibited;
	}

    namespace {
        std::string ToUtf8(const std::wstring &text) {
            return std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t>().to_bytes(text);
        }
    }
    std::exception_ptr InvalidOpcodeException::getInnerException() const noexcept { return InnerException; }
    InvalidOpcodeException::InvalidOpcodeException(const std::wstring &message, const std::wstring &opcodeIdentifier, int slotIndex, bool isProhibited)
        : std::runtime_error(ToUtf8(message)), OpcodeIdentifier(opcodeIdentifier), SlotIndex(slotIndex), IsProhibited(isProhibited), InnerException(nullptr) {}
    InvalidOpcodeException::InvalidOpcodeException(const std::wstring &message)
        : std::runtime_error(ToUtf8(message)), OpcodeIdentifier(), SlotIndex(-1), IsProhibited(false), InnerException(nullptr) {}
    InvalidOpcodeException::InvalidOpcodeException(const std::wstring &message, std::runtime_error inner)
        : std::runtime_error(ToUtf8(message)), OpcodeIdentifier(), SlotIndex(-1), IsProhibited(false), InnerException(std::make_exception_ptr(inner)) {}
}
