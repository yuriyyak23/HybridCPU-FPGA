#pragma once

#include "../../../Arch/DataTypeEnum.h"
#include <span>
#include <string>
#include <limits>
#include <format>







namespace HybridCPU_ISE::Arch
{
// C# TO C++ CONVERTER NOTE: The following .NET attribute has no direct equivalent in C++:
// ORIGINAL LINE: [StructLayout(LayoutKind.Sequential, Pack = 1)] public partial struct VLIW_Instruction
	class VLIW_Instruction
	{
	public:
		static constexpr unsigned long long ReservedWord0Mask = 0x0000FF0000000000ULL;
		static constexpr unsigned long long RetiredPolicyGapMask = 1ULL << 50;

		static std::wstring GetReservedWord0ViolationMessage(int slotIndex = -1);

		static std::wstring GetRetiredPolicyGapViolationMessage(int slotIndex = -1);

		static unsigned long long ValidateWord0ForProductionIngress(unsigned long long rawWord0);

		static unsigned long long ValidateWord3ForProductionIngress(unsigned long long rawWord3);

		void ValidateRetiredPolicyGapBitForProductionIngress();


			// --- Flags in Word 0 [23:16] ---

			/// <summary>
			/// Per-instruction mask-agnostic policy bit (RVV ma).
			/// When false (0): undisturbed — masked-off elements preserve previous value.
			/// When true (1): agnostic — masked-off elements are UNDEFINED (allows optimization).
			/// Effective policy = instBit OR global VectorConfig.MaskAgnostic.
			/// </summary>
			bool getMaskAgnostic() const;
			void setMaskAgnostic(bool value);

			/// <summary>
			/// Per-instruction tail-agnostic policy bit (RVV ta).
			/// When false (0): undisturbed — tail elements (beyond VL) preserve previous value.
			/// When true (1): agnostic — tail elements are UNDEFINED (allows optimization).
			/// Effective policy = instBit OR global VectorConfig.TailAgnostic.
			/// In memory-to-memory model tail is naturally undisturbed (BurstWrite uses VL, not VLMAX).
			/// </summary>
			bool getTailAgnostic() const;
			void setTailAgnostic(bool value);

			bool getIndexed() const;
			void setIndexed(bool value);

			bool getIs2D() const;
			void setIs2D(bool value);

			bool getReduction() const;
			void setReduction(bool value);

			// --- New v2 flags [18:16] ---

			/// <summary>
			/// Acquire memory ordering fence for atomic operations (bit 18).
			/// </summary>
			bool getAcquire() const;
			void setAcquire(bool value);

			/// <summary>
			/// Release memory ordering fence for atomic operations (bit 17).
			/// </summary>
			bool getRelease() const;
			void setRelease(bool value);

			/// <summary>
			/// Saturating arithmetic mode hint (bit 16).
			/// </summary>
			bool getSaturating() const;
			void setSaturating(bool value);


			// V6 B10: VLIW_Instruction is a fixed-width bitfield container (4 × 64-bit
			// words) that encodes the opcode as an opaque 16-bit field in word0[63:48].  Adding new
			// opcodes to InstructionsEnum never requires changes to this struct — the struct is
			// fully decoupled from opcode-space evolution.  Architectural classification
			// (IsControlFlow, IsMathOrVector, …) lives exclusively in OpcodeInfo/OpcodeRegistry.
			//
			// V6 B11: word3[50] is a retired legacy scheduling-policy bit, NOT an
			// architectural flag; word3[49:48] = VirtualThreadId is a transport hint only,
			// NOT ISA state. These bits must not influence functional decode. Architectural flags
			// (Acquire/Release/Saturating/MaskAgnostic/…) are encoded in word0[23:16].
		private:
			unsigned long long word0 = 0; // [63:48] OpCode (16 bit) | [47:40] Reserved (8 bit) | [39:32] DataType (8 bit) | [31:24] PredicateMask (8 bit) | [23:16] Flags (8 bit) | [15:0] Immediate (16 bit)
			unsigned long long word1 = 0; // Dest / Src1 Pointer
			unsigned long long word2 = 0; // Src2 Pointer / Index Array Pointer (if Indexed)
			unsigned long long word3 = 0; // [63:51] RowStride (13 bit) | [50] Reserved legacy policy gap (must be zero on production ingress) | [49:48] VirtualThreadId (transport hint only) | [47:16] StreamLength (32 bit) | [15:0] Stride (16 bit)

		public:
			unsigned long long getWord0() const;
			void setWord0(unsigned long long value);

			unsigned long long getWord1() const;
			void setWord1(unsigned long long value);

			unsigned long long getWord2() const;
			void setWord2(unsigned long long value);

			unsigned long long getWord3() const;
			void setWord3(unsigned long long value);

			// --- HLS-friendly Word 0 decoders ---
			unsigned int getOpCode() const;
			void setOpCode(unsigned int value);

			unsigned char getDataType() const;
			void setDataType(unsigned char value);

			// Helper property to get/set DataType as enum
			DataTypeEnum getDataTypeValue() const;
			void setDataTypeValue(DataTypeEnum value);

			unsigned char getPredicateMask() const;
			void setPredicateMask(unsigned char value);

			unsigned short getImmediate() const;
			void setImmediate(unsigned short value);

			// --- Reserved [47:40] ---

			/// <summary>
			/// Reserved 8-bit future metadata field (bits [47:40]).
			/// </summary>
			unsigned char getReserved() const;
			void setReserved(unsigned char value);

			// --- Addressing (Pointer Swapping) ---
			unsigned long long getDestSrc1Pointer() const;
			void setDestSrc1Pointer(unsigned long long value);

			unsigned long long getSrc2Pointer() const;
			void setSrc2Pointer(unsigned long long value);

			// --- 32-bit Addressing for MemPtr_MemPtr_MemPtr_DataLength ---
			unsigned int getSourceAPointer() const;
			void setSourceAPointer(unsigned int value);

			unsigned int getSourceBPointer() const;
			void setSourceBPointer(unsigned int value);

			unsigned int getDestinationPointer() const;
			void setDestinationPointer(unsigned int value);

			unsigned int getVectorDataLength() const;
			void setVectorDataLength(unsigned int value);

			// --- Extended Word 3: RowStride (13 bit) + SMT hint fields (3 bit) + StreamLength (32 bit) + Stride (16 bit) ---
			// Word3 layout (PHASE 4 SMT EXTENSION):
			// [63:51] RowStride (13 bits, reduced from 16 bits - max 8191, sufficient for 2D operations)
			// [50]    Reserved legacy policy bit (1 bit) - must be zero on production ingress
			// [49:48] VirtualThreadId (2 bits, 0-3) - transport hint only, not an execution binding
			// [47:16] StreamLength (32 bits) - unchanged
			// [15:0]  Stride (16 bits) - unchanged

			unsigned short getRowStride() const;
			void setRowStride(unsigned short value);

			/// <summary>
			/// Phase 4 SMT Extension: Virtual Thread ID (0-3) transport hint only.
			/// Preserved for compat ingress/egress and diagnostics; does not bind execution
			/// to a specific SMT context and does not participate in functional decode.
			/// </summary>
			unsigned char getVirtualThreadId() const;
			void setVirtualThreadId(unsigned char value);

			unsigned int getStreamLength() const;
			void setStreamLength(unsigned int value);

			unsigned short getStride() const;
			void setStride(unsigned short value);

			void Clear();


			// --- HLS-compatible register packing for the flat architectural Word1 contour ---
			// Canonical decode decides whether Word1 is interpreted through this packed ABI
			// from published opcode semantics, not from StreamLength-derived scalar heuristics.
			//   Word1: [15:0] Rd | [31:16] Rs1 | [47:32] Rs2
			// Word2 is used for memory address or auxiliary data (jump target, immediate, etc.)

			/// <summary>
			/// Sentinel register ID meaning "no register" / "register field unused".
			/// <para>
			/// Blueprint §3.69 / §5: "Возможно, явно запретить RegID=-1, а генерировать NoReg."
			/// Callers that formerly passed <c>-1</c> to <see cref="PackRegisters"/> to indicate
			/// an absent operand now receive this value from <see cref="Reg1ID"/> / <see cref="Reg2ID"/>
			/// / <see cref="Reg3ID"/> and should guard with <c>!= NoReg</c> before treating the
			/// field as an architectural register index.
			/// </para>
			/// </summary>
			static constexpr unsigned short NoReg = (std::numeric_limits<unsigned short>::max)(); // 0xFFFF

			/// <summary>
			/// Sentinel architectural register field meaning "operand not present".
			/// Encoded as <see cref="NoReg"/> in Word1.
			/// </summary>
			static constexpr unsigned char NoArchReg = (std::numeric_limits<unsigned char>::max)(); // 0xFF

			/// <summary>
			/// Packs up to 3 flat architectural register ids into Word1 for the packed-arch register contour.
			/// Register operands must be in [0, 31], or <see cref="NoArchReg"/> for an unused field.
			/// No VT/global-register information is permitted in this encoding.
			/// </summary>
			static unsigned long long PackArchRegs(unsigned char rd, unsigned char rs1, unsigned char rs2);

			/// <summary>
			/// Attempts to unpack flat architectural register ids from Word1.
			/// Returns false if any field contains a legacy/global register id or other invalid value.
			/// Unused fields decode as <see cref="NoArchReg"/>.
			/// </summary>
			static bool TryUnpackArchRegs(unsigned long long packedRegisters, unsigned char &rd, unsigned char &rs1, unsigned char &rs2);

			/// <summary>
			/// Extracts the raw Word1 destination field.
			/// Use <see cref="TryUnpackArchRegs"/> to validate that it contains a flat architectural register id.
			/// </summary>
			unsigned short getReg1ID() const;

			/// <summary>
			/// Extracts the raw Word1 source-1 field.
			/// Use <see cref="TryUnpackArchRegs"/> to validate that it contains a flat architectural register id.
			/// </summary>
			unsigned short getReg2ID() const;

			/// <summary>
			/// Extracts the raw Word1 source-2 field.
			/// Use <see cref="TryUnpackArchRegs"/> to validate that it contains a flat architectural register id.
			/// </summary>
			unsigned short getReg3ID() const;


		private:
			bool TryReadBytesCore(std::span<const unsigned char> source, int offset);

			/// <summary>
			/// HLS-compatible: Write instruction to pre-allocated buffer.
			/// No dynamic allocation, uses Span for zero-copy operation.
			/// The canonical VLIW carrier byte order is little-endian.
			/// </summary>
			/// <param name="destination">Pre-allocated buffer (must be >= 32 bytes)</param>
			/// <returns>True if successful, false if buffer too small</returns>
		public:
			bool TryWriteBytes(std::span<unsigned char> destination);

			/// <summary>
			/// HLS-compatible: Read instruction from buffer.
			/// Returns false for short buffers. Invalid production-ingress carrier bits
			/// fail closed with <see cref="YAKSys_Hybrid_CPU.Arch.InvalidOpcodeException"/>.
			/// The canonical VLIW carrier byte order is little-endian.
			/// </summary>
			/// <param name="source">Source buffer (must be >= 32 bytes from offset)</param>
			/// <param name="offset">Offset in buffer</param>
			/// <returns>True if successful, false if buffer too small</returns>
			bool TryReadBytes(std::span<const unsigned char> source, int offset = 0);
	};
}
