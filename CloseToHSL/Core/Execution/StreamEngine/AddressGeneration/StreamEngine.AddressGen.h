#pragma once

namespace YAKSys_Hybrid_CPU::Execution
{
	/// <summary>
	/// Port type for stream operations - used for widening address calculations.
	/// </summary>
	enum class PortType
	{
		SourceA,
		SourceB,
		Destination
	};

	/// <summary>
	/// Address generation for stream operations.
	/// Supports: 1D (stride), 2D (row/column), and indexed (gather/scatter).
	/// Supports widening operations (FP8 → FP32) via asymmetric addressing.
	///
	/// Design goals:
	/// - HLS-friendly: deterministic, no dynamic allocation
	/// - Compiler-oriented: explicit modes via instruction flags
	/// - AXI4-aware: can split addresses at 4KB boundaries
	/// - Widening-aware: supports asymmetric element sizes for source and destination
	/// </summary>
	class AddressGen final
	{
		/// <summary>
		/// Generate address for 1D strided access.
		/// Simple pattern: base + (element_index * stride)
		/// </summary>
		/// <param name="baseAddr">Base address</param>
		/// <param name="elementIndex">Element index in stream</param>
		/// <param name="stride">Byte stride between elements</param>
		/// <returns>Physical address for element</returns>
	public:
		static unsigned long long Gen1D(unsigned long long baseAddr, unsigned long long elementIndex, unsigned short stride);

		/// <summary>
		/// Generate address for 2D (row-major) access.
		/// Pattern: base + (row * rowStride) + (col * colStride)
		///
		/// Instruction encoding for 2D (when Is2D=1):
		/// - RowStride: byte pitch between rows
		/// - Stride: byte stride between columns (element stride)
		/// - Immediate: number of elements per row (rowLength)
		/// - StreamLength: total elements (rows * cols)
		///
		/// Compute: row = elementIndex / rowLength
		///          col = elementIndex % rowLength
		/// </summary>
		/// <param name="baseAddr">Base address</param>
		/// <param name="elementIndex">Linear element index</param>
		/// <param name="rowLength">Number of elements per row</param>
		/// <param name="rowStride">Byte pitch between rows</param>
		/// <param name="colStride">Byte stride between columns</param>
		/// <returns>Physical address for element</returns>
		static unsigned long long Gen2D(unsigned long long baseAddr, unsigned long long elementIndex, unsigned int rowLength, unsigned short rowStride, unsigned short colStride);

		/// <summary>
		/// Check if address crosses a 4KB boundary.
		/// AXI4 rule: burst cannot cross 4KB boundaries.
		/// Used to split large transfers into multiple bursts.
		/// </summary>
		/// <param name="startAddr">Start address of transfer</param>
		/// <param name="length">Length of transfer in bytes</param>
		/// <returns>True if transfer crosses 4KB boundary</returns>
		static bool Crosses4KBBoundary(unsigned long long startAddr, unsigned long long length);

		/// <summary>
		/// Compute bytes until next 4KB boundary from given address.
		/// Used to determine maximum burst size without crossing boundary.
		/// </summary>
		/// <param name="addr">Starting address</param>
		/// <returns>Bytes remaining in current 4KB page</returns>
		static unsigned long long BytesTo4KBBoundary(unsigned long long addr);

		/// <summary>
		/// Compute maximum number of elements that can be read in one burst
		/// without crossing 4KB boundary.
		/// </summary>
		/// <param name="startAddr">Starting address</param>
		/// <param name="elementSize">Size of each element in bytes</param>
		/// <param name="maxElements">Maximum elements requested</param>
		/// <returns>Actual number of elements to read in this burst</returns>
		static unsigned long long ComputeBurstLength(unsigned long long startAddr, int elementSize, unsigned long long maxElements);

		// ========================================================================
		// Widening Operation Support (FP8 → FP32)
		// ========================================================================

		/// <summary>
		/// Check if opcode is a widening operation (reads narrow elements, writes wide result).
		/// Widening operations: VDOT_FP8 with FP8 DataType (read 1-byte FP8, write 4-byte FP32).
		/// Hardware MUX for HLS synthesis (no branches).
		/// </summary>
		static bool IsWideningOpcode(unsigned int opCode);

		/// <summary>
		/// Get effective element size for address generation based on port type.
		/// For widening operations: source reads 1-byte, destination writes 4-byte.
		/// Returns effective size via pure combinational MUX (HLS-friendly).
		/// </summary>
		/// <param name="opCode">Instruction opcode</param>
		/// <param name="port">Port type (SourceA, SourceB, or Destination)</param>
		/// <param name="baseElementSize">Base element size from data type (1 byte for FP8)</param>
		/// <returns>Effective element size for this port (1 or 4 bytes)</returns>
		static unsigned int GetEffectiveElementSize(unsigned int opCode, PortType port, unsigned char baseElementSize);
	};
}
