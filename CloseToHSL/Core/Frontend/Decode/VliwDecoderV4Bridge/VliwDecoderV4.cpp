#include "VliwDecoderV4.h"
#include "../../../Decoder/DeclarativeDecoderStages.h"
#include "../../../Decoder/DecodeFailureCompatibilityAdapter.h"
#include "../../../Decoder/DecoderRuntimeSidebandValidator.h"
#include "../../../Decoder/CanonicalInstructionIrCompatibilityProjector.h"
#include "../../../Pipeline/Metadata/BundleMetadata.h"
#include "../../../../../NonRTL/Arch/IsaV4Surface.h"
#include <iomanip>
#include <locale>
#include <sstream>
#include <stdexcept>

using namespace HybridCPU_ISE::Arch;
using namespace YAKSys_Hybrid_CPU::Arch;
using namespace YAKSys_Hybrid_CPU::Core::Pipeline::MicroOps;

namespace YAKSys_Hybrid_CPU::Core::Decoder
{
	namespace
	{
		// ProhibitedOpcodes contains invariant ASCII mnemonics and numeric
		// identifiers. ASCII folding therefore has the same membership result
		// as ToUpperInvariant for this set, independent of the process locale.
		std::wstring ToInvariantOpcodeKey(const std::wstring &value)
		{
			std::wstring result = value;
			for (wchar_t &ch : result)
			{
				if (ch >= L'a' && ch <= L'z') ch = static_cast<wchar_t>(ch - (L'a' - L'A'));
			}
			return result;
		}

		bool IsProhibitedOpcodeKey(const std::wstring &value)
		{
			const auto &keys = *IsaV4Surface::ProhibitedOpcodes;
			return keys.find(value) != keys.end() || keys.find(ToInvariantOpcodeKey(value)) != keys.end();
		}
	}

	std::shared_ptr<InstructionIR> VliwDecoderV4::Decode(VLIW_Instruction const instruction, int slotIndex)
	{
		// The historical per-slot API permits an out-of-window diagnostic
		// index in unit tooling. Keep that compatibility while bundle decode
		// remains strictly eight physical slots.
		int rawReaderIndex = static_cast<unsigned int>(slotIndex) < BundleMetadata::BundleSlotCount ? slotIndex : 0;
		const auto rawReaderSlot = RawSlotReader::Read(instruction, rawReaderIndex);
		if (!rawReaderSlot)
		{
			throw std::logic_error("RawSlotReader returned a null slot.");
		}
		// Recreate the immutable record with the caller's diagnostic SlotIndex,
		// matching the original C# record `with { SlotIndex = slotIndex }`.
		auto rawSlot = std::make_shared<RawSlot>(slotIndex, rawReaderSlot->Instruction,
			rawReaderSlot->Opcode, rawReaderSlot->Word0, rawReaderSlot->Word1,
			rawReaderSlot->Word2, rawReaderSlot->Word3);
		std::optional<std::shared_ptr<DeclarativeDecodedSlot>> declarativeSlot;
		std::optional<std::shared_ptr<DecodeFailure>> failure;
		if (!DeclarativeDecoderPipeline::TryDecodeOccupiedSlot(rawSlot, InstructionSlotMetadata::getDefault(), declarativeSlot, failure))
		{
	// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
	// ORIGINAL LINE: throw DecodeFailureCompatibilityAdapter.ToInvalidOpcodeException(failure!, in instruction);
			throw DecodeFailureCompatibilityAdapter::ToInvalidOpcodeException(failure.value_or(nullptr), instruction);
		}
		if (!declarativeSlot || !*declarativeSlot)
		{
			throw std::logic_error("Decoder succeeded without producing a decoded slot.");
		}

		std::shared_ptr<RuntimeSidebandProjection> runtimeSideband = DecoderRuntimeSidebandValidator::ValidateAndResolve(instruction, slotIndex, InstructionSlotMetadata::getDefault());
	// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
	// ORIGINAL LINE: return CanonicalInstructionIrCompatibilityProjector.ProjectInstruction( declarativeSlot!.CanonicalInstruction, declarativeSlot, runtimeSideband);
		const auto decodedSlot = *declarativeSlot;
		return CanonicalInstructionIrCompatibilityProjector::ProjectInstruction(decodedSlot->CanonicalInstruction, decodedSlot, runtimeSideband);
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public DecodedInstructionBundle DecodeInstructionBundle(System.ReadOnlySpan<VLIW_Instruction> bundle, VliwBundleAnnotations? bundleAnnotations, ulong bundleAddress, ulong bundleSerial = 0, System.Nullable<CanonicalDecodeContext> decodeContext = null)
	std::shared_ptr<DecodedInstructionBundle> VliwDecoderV4::DecodeInstructionBundle(std::span<const VLIW_Instruction> bundle, std::shared_ptr<VliwBundleAnnotations> bundleAnnotations, unsigned long long bundleAddress, unsigned long long bundleSerial, std::optional<std::shared_ptr<CanonicalDecodeContext>> decodeContext)
	{
		std::optional<std::shared_ptr<DeclarativeDecodedBundle>> declarativeBundle;
		std::optional<std::shared_ptr<DecodeFailure>> failure;
		if (!DeclarativeDecoderPipeline::TryDecodeBundle(bundle, bundleAnnotations, bundleAddress, bundleSerial, decodeContext, declarativeBundle, failure))
		{
	// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
	// ORIGINAL LINE: throw DecodeFailureCompatibilityAdapter.ToInvalidOpcodeException(failure!, bundle);
			throw DecodeFailureCompatibilityAdapter::ToInvalidOpcodeException(failure.value_or(nullptr), bundle);
		}
		if (!declarativeBundle || !*declarativeBundle)
		{
			throw std::logic_error("Decoder succeeded without producing a decoded bundle.");
		}

	// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
	// ORIGINAL LINE: return CanonicalInstructionIrCompatibilityProjector.ProjectBundle( bundle, bundleAnnotations, declarativeBundle!, bundleAddress, bundleSerial);
		return CanonicalInstructionIrCompatibilityProjector::ProjectBundle(bundle, bundleAnnotations, *declarativeBundle, bundleAddress, bundleSerial);
	}

	std::shared_ptr<DecodedInstructionBundle> VliwDecoderV4::DecodeInstructionBundle(std::span<const VLIW_Instruction> bundle, unsigned long long bundleAddress, unsigned long long bundleSerial)
	{
		return DecodeInstructionBundle(bundle, nullptr, bundleAddress, bundleSerial);
	}

	bool VliwDecoderV4::IsProhibited(unsigned short opcode)
	{
		return IsProhibited(opcode, std::to_wstring(opcode));
	}

	bool VliwDecoderV4::IsProhibited(const std::wstring &opcodeName)
	{
		unsigned short opcode;
		if (TryParseOpcodeIdentifier(opcodeName, opcode))
		{
			return IsProhibited(opcode, opcodeName);
		}

		return IsProhibitedOpcodeKey(opcodeName);
	}

	bool VliwDecoderV4::IsProhibited(unsigned short opcode, const std::wstring &opcodeName)
	{
		if (IsProhibitedOpcodeKey(opcodeName))
		{
			return true;
		}

		const std::wstring decimalIdentifier = std::to_wstring(opcode);
		const auto &prohibitedOpcodes = *IsaV4Surface::ProhibitedOpcodes;
		if (prohibitedOpcodes.find(decimalIdentifier) != prohibitedOpcodes.end())
		{
			return true;
		}

		std::wostringstream hexText;
		hexText.imbue(std::locale::classic());
		hexText << L"0x" << std::uppercase << std::hex << opcode;
		return IsProhibitedOpcodeKey(hexText.str());
	}

	bool VliwDecoderV4::TryParseOpcodeIdentifier(const std::wstring &opcodeIdentifier, unsigned short &opcode)
	{
		// Match ushort.TryParse(..., NumberStyles::Integer, InvariantCulture):
		// trim Unicode whitespace, allow one leading sign, and require decimal
		// digits with a value in the UInt16 range.
		auto isInvariantWhitespace = [](wchar_t ch)
		{
			return ch == L' ' || (ch >= L'\t' && ch <= L'\r') || ch == 0x0085 ||
				ch == 0x00A0 || ch == 0x1680 || (ch >= 0x2000 && ch <= 0x200A) ||
				ch == 0x2028 || ch == 0x2029 || ch == 0x202F || ch == 0x205F || ch == 0x3000;
		};
		size_t begin = 0;
		size_t end = opcodeIdentifier.size();
		while (begin < end && isInvariantWhitespace(opcodeIdentifier[begin])) ++begin;
		while (end > begin && isInvariantWhitespace(opcodeIdentifier[end - 1])) --end;
		bool negative = false;
		if (begin < end && (opcodeIdentifier[begin] == L'+' || opcodeIdentifier[begin] == L'-'))
		{
			negative = opcodeIdentifier[begin] == L'-';
			++begin;
		}
		if (begin < end && !negative)
		{
			unsigned int value = 0;
			bool valid = true;
			for (size_t i = begin; i < end; ++i)
			{
				wchar_t ch = opcodeIdentifier[i];
				if (ch < L'0' || ch > L'9') { valid = false; break; }
				value = value * 10u + static_cast<unsigned int>(ch - L'0');
				if (value > 0xFFFFu) { valid = false; break; }
			}
			if (valid)
			{
				opcode = static_cast<unsigned short>(value);
				return true;
			}
		}

		if (opcodeIdentifier.size() >= 2 && opcodeIdentifier[0] == L'0' &&
			(opcodeIdentifier[1] == L'x' || opcodeIdentifier[1] == L'X'))
		{
			const size_t hexBegin = 2;
			const size_t hexEnd = opcodeIdentifier.size();
			// NumberStyles.AllowHexSpecifier does not permit whitespace or a sign.
			if (hexBegin < hexEnd)
			{
				unsigned int value = 0;
				bool valid = true;
				for (size_t i = hexBegin; i < hexEnd; ++i)
				{
					wchar_t ch = opcodeIdentifier[i];
					unsigned int digit;
					if (ch >= L'0' && ch <= L'9') digit = static_cast<unsigned int>(ch - L'0');
					else if (ch >= L'a' && ch <= L'f') digit = static_cast<unsigned int>(ch - L'a' + 10);
					else if (ch >= L'A' && ch <= L'F') digit = static_cast<unsigned int>(ch - L'A' + 10);
					else { valid = false; break; }
					value = value * 16u + digit;
					if (value > 0xFFFFu) { valid = false; break; }
				}
				if (valid)
				{
					opcode = static_cast<unsigned short>(value);
					return true;
				}
			}
		}

		opcode = 0;
		return false;
	}
}
