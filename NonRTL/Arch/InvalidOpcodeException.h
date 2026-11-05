#pragma once

#include <string>
#include <stdexcept>
#include <exception>
#include <memory>

namespace YAKSys_Hybrid_CPU::Arch
{
	/// <summary>
	/// Thrown when the ISA v4 decoder encounters an opcode that is either:
	/// <list type="bullet">
	///   <item>Listed in <see cref="IsaV4Surface.ProhibitedOpcodes"/> — assembler pseudo-ops,
	///     hint opcodes, or compiler/emulator wrappers that must not appear in the hardware
	///     instruction stream.</item>
	///   <item>Outside the canonical ISA v4 opcode space and therefore has no known
	///     <see cref="InstructionClass"/>/<see cref="SerializationClass"/> classification.</item>
	/// </list>
	/// </summary>
	class InvalidOpcodeException final : public std::runtime_error, public std::enable_shared_from_this<InvalidOpcodeException>
	{
	private:
		const std::wstring OpcodeIdentifier;
		const int SlotIndex;
		const bool IsProhibited;
        const std::exception_ptr InnerException;

		/// <summary>Opcode name or numeric value that caused the fault.</summary>
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public string? OpcodeIdentifier {get;}
	public:
		std::wstring getOpcodeIdentifier() const;

		/// <summary>Physical bundle slot index (0–7) in which the fault occurred.</summary>
		int getSlotIndex() const;

		/// <summary>
		/// Whether the opcode is specifically in <see cref="IsaV4Surface.ProhibitedOpcodes"/>
		/// (as opposed to simply being unknown / unrecognised).
		/// </summary>
		bool isProhibited() const;
        std::exception_ptr getInnerException() const noexcept;

		/// <summary>
		/// Initializes a new <see cref="InvalidOpcodeException"/> for a prohibited opcode.
		/// </summary>
		InvalidOpcodeException(const std::wstring &message, const std::wstring &opcodeIdentifier, int slotIndex, bool isProhibited = true);

		/// <summary>
		/// Initializes a new <see cref="InvalidOpcodeException"/> without slot context.
		/// Used by static classifier paths.
		/// </summary>
		InvalidOpcodeException(const std::wstring &message);

		/// <summary>
		/// Initializes a new <see cref="InvalidOpcodeException"/> with an inner exception.
		/// </summary>
		InvalidOpcodeException(const std::wstring &message, std::runtime_error inner);
	};
}
