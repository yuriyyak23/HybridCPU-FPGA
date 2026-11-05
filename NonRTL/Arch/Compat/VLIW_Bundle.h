#pragma once

#include "HybridCPU_ISE.Arch.VLIW_Instruction.h"

namespace HybridCPU_ISE::Arch
{
// C# TO C++ CONVERTER NOTE: The following .NET attribute has no direct equivalent in C++:
// ORIGINAL LINE: [StructLayout(LayoutKind.Sequential, Pack = 1)] public struct VLIW_Bundle
	class VLIW_Bundle
	{
	private:
		VLIW_Instruction inst0;
		VLIW_Instruction inst1;
		VLIW_Instruction inst2;
		VLIW_Instruction inst3;
		VLIW_Instruction inst4;
		VLIW_Instruction inst5;
		VLIW_Instruction inst6;
		VLIW_Instruction inst7;

	public:
		VLIW_Instruction GetInstruction(int index);

		void SetInstruction(int index, VLIW_Instruction instruction);

	private:
		void SetInstructionUnchecked(int index, VLIW_Instruction instruction);

		/// <summary>
		/// HLS-compatible: Write bundle to pre-allocated buffer.
		/// No dynamic allocation, uses Span for zero-copy operation.
		/// </summary>
		/// <param name="destination">Pre-allocated buffer (must be >= 256 bytes)</param>
		/// <returns>True if successful, false if buffer too small</returns>
	public:
		bool TryWriteBytes(std::span<unsigned char> destination);

		/// <summary>
		/// HLS-compatible: Read bundle from buffer.
		/// No exceptions thrown, returns success flag.
		/// </summary>
		/// <param name="source">Source buffer (must be >= 256 bytes from offset)</param>
		/// <param name="offset">Offset in buffer</param>
		/// <returns>True if successful, false if buffer too small</returns>
		bool TryReadBytes(std::span<const unsigned char> source, int offset = 0);
	};
}
