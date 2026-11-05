#pragma once
#include <span>

#include "../../../Architecture/Common/YAKSys_Hybrid_CPU.Processor.h"
#include "../../../../../Arch/DataTypeEnum.h"
#include <cmath>
#include <limits>
#include <optional>
#include <format>
#include <memory>
#include "../../../../../tangible_bankers_rounding.h"

using namespace HybridCPU_ISE::Arch;

using namespace YAKSys_Hybrid_CPU::Arch;





using namespace YAKSys_Hybrid_CPU::Core;


namespace YAKSys_Hybrid_CPU::Execution
{
	/// <summary>
	/// Typed vector ALU operations for all data types.
	/// Provides element-wise operations respecting type semantics (signed/unsigned/float).
	///
	/// Design principles:
	/// - No dynamic allocation (HLS-compatible)
	/// - Uses ElementCodec for uniform load/store
	/// - Respects predicate masks (inactive lanes skip computation)
	/// - RVV semantics: masked-off elements don't write, don't trap
	/// - Handles all data types: INT8-INT64, UINT8-UINT64, FLOAT32-FLOAT64
	/// </summary>
	class VectorALU final
	{
		/// <summary>
		/// Mark vector unit as dirty (used for context switching optimization).
		/// Should be called at the start of any vector operation.
		/// </summary>
	private:
		static void MarkVectorDirty(std::shared_ptr<Processor::CPU_Core> &core);

		/// <summary>
		/// Track divide-by-zero exception.
		/// </summary>
		static void TrackDivByZero(std::shared_ptr<Processor::CPU_Core> &core);

		/// <summary>
		/// Track invalid operation exception (e.g., sqrt of negative, NaN operations).
		/// </summary>
		static void TrackInvalidOp(std::shared_ptr<Processor::CPU_Core> &core);

		/// <summary>
		/// Track overflow exception (integer/fixed-point overflow).
		/// </summary>
		static void TrackOverflow(std::shared_ptr<Processor::CPU_Core> &core);

		/// <summary>
		/// Track underflow exception (floating-point underflow).
		/// </summary>
		static void TrackUnderflow(std::shared_ptr<Processor::CPU_Core> &core);

		/// <summary>
		/// Track inexact result exception (floating-point rounding).
		/// </summary>
		static void TrackInexact(std::shared_ptr<Processor::CPU_Core> &core);

		/// <summary>
		/// Apply rounding mode to a floating-point value.
		/// Implements IEEE 754 rounding modes as specified in VectorExceptionStatus.RoundingMode.
		/// </summary>
		static double ApplyRounding(double value, unsigned char roundingMode, std::shared_ptr<Processor::CPU_Core> &core);

		/// <summary>
		/// Check for underflow in floating-point result.
		/// Detects denormal/subnormal values that indicate underflow.
		/// </summary>
		static void CheckUnderflow(double result, std::shared_ptr<Processor::CPU_Core> &core);

		/// <summary>
		/// Check exception mode and handle trap if necessary.
		/// Called after tracking any exception.
		/// Implements mask and priority-based exception handling.
		/// </summary>
		static void CheckExceptionMode(std::shared_ptr<Processor::CPU_Core> &core);

		/// <summary>
		/// Generate a trap/interrupt for vector exceptions.
		/// Saves full vector context before dispatching to the interrupt handler,
		/// so InterruptReturn can restore the vector unit to its pre-exception state.
		/// </summary>
		/// <param name="core">CPU core reference</param>
		/// <param name="exceptionType">Type of exception that triggered the trap</param>
		static void GenerateVectorExceptionTrap(std::shared_ptr<Processor::CPU_Core> &core, Processor::CPU_Core::VectorException exceptionType);

		/// <summary>
		/// Apply binary operation element-wise across vectors.
		/// Supports all arithmetic and logical operations.
		///
		/// RVV semantics:
		/// - Inactive lanes (predicate mask bit = 0) are not processed
		/// - No exceptions raised for masked-off elements
		/// - Results written only for active lanes
		/// - Tail/mask policy respected (undisturbed = preserve, agnostic = may overwrite)
		/// </summary>
		/// <param name="op">Operation code (InstructionsEnum value)</param>
		/// <param name="dt">Data type enum</param>
		/// <param name="a">First source operand buffer</param>
		/// <param name="b">Second source operand buffer</param>
		/// <param name="dst">Destination buffer (may alias 'a' for destructive ops)</param>
		/// <param name="elemSize">Size of each element in bytes</param>
		/// <param name="vl">Vector length (number of active elements)</param>
		/// <param name="predIndex">Predicate register index (0 = no masking)</param>
		/// <param name="tailAgnostic">Per-instruction tail-agnostic bit (ta): tail elements may be overwritten</param>
		/// <param name="maskAgnostic">Per-instruction mask-agnostic bit (ma): masked-off elements may be overwritten</param>
		/// <param name="core">Reference to CPU core for predicate access</param>
	public:
		static void ApplyBinary(unsigned int op, DataTypeEnum dt, std::span<const unsigned char> a, std::span<const unsigned char> b, std::span<unsigned char> dst, int elemSize, unsigned long long vl, unsigned char predIndex, bool tailAgnostic, bool maskAgnostic, std::shared_ptr<Processor::CPU_Core> &core);

		/// <summary>
		/// Apply binary operation with immediate value element-wise across vectors.
		/// Used for operations like shift-by-immediate (VSLL.VI, VSRL.VI, VSRA.VI).
		///
		/// RVV semantics:
		/// - Inactive lanes (predicate mask bit = 0) are not processed
		/// - Immediate value is used as the second operand for all elements
		/// </summary>
		/// <param name="op">Operation code (InstructionsEnum value)</param>
		/// <param name="dt">Data type enum</param>
		/// <param name="a">Source operand buffer</param>
		/// <param name="immediate">Immediate value (used as second operand)</param>
		/// <param name="dst">Destination buffer</param>
		/// <param name="elemSize">Size of each element in bytes</param>
		/// <param name="vl">Vector length (number of active elements)</param>
		/// <param name="predIndex">Predicate register index (0 = no masking)</param>
		/// <param name="tailAgnostic">Per-instruction tail-agnostic bit (ta): tail elements may be overwritten</param>
		/// <param name="maskAgnostic">Per-instruction mask-agnostic bit (ma): masked-off elements may be overwritten</param>
		/// <param name="core">Reference to CPU core for predicate access</param>
		static void ApplyBinaryImmediate(unsigned int op, DataTypeEnum dt, std::span<const unsigned char> a, unsigned short immediate, std::span<unsigned char> dst, int elemSize, unsigned long long vl, unsigned char predIndex, bool tailAgnostic, bool maskAgnostic, std::shared_ptr<Processor::CPU_Core> &core);

		/// <summary>
		/// Apply unary operation element-wise across vector.
		/// Handles operations like NOT, SQRT, NEG that take single operand.
		/// Respects RVV tail/mask policy semantics.
		/// </summary>
		static void ApplyUnary(unsigned int op, DataTypeEnum dt, std::span<const unsigned char> src, std::span<unsigned char> dst, int elemSize, unsigned long long vl, unsigned char predIndex, bool tailAgnostic, bool maskAgnostic, std::shared_ptr<Processor::CPU_Core> &core);

		/// <summary>
		/// Apply ternary FMA operation element-wise across vectors.
		/// FMA (Fused Multiply-Add): performs multiplication and addition as single operation.
		///
		/// Operations:
		/// - VFMADD: vd[i] = (vd[i] * vs1[i]) + vs2[i]
		/// - VFMSUB: vd[i] = (vd[i] * vs1[i]) - vs2[i]
		/// - VFNMADD: vd[i] = -(vd[i] * vs1[i]) + vs2[i]
		/// - VFNMSUB: vd[i] = -(vd[i] * vs1[i]) - vs2[i]
		///
		/// Note: vd is both source and destination (destructive operation).
		/// </summary>
		static void ApplyFMA(unsigned int op, DataTypeEnum dt, std::span<unsigned char> vd, std::span<const unsigned char> vs1, std::span<const unsigned char> vs2, int elemSize, unsigned long long vl, unsigned char predIndex, bool tailAgnostic, bool maskAgnostic, std::shared_ptr<Processor::CPU_Core> &core);

		/// <summary>
		/// Apply reduction operation to collapse vector to scalar.
		/// Reduces all active elements in a vector to a single result.
		///
		/// Reduction operations:
		/// - VREDSUM: sum of all elements
		/// - VREDMAX/VREDMIN: maximum/minimum of all elements
		/// - VREDAND/VREDOR/VREDXOR: bitwise AND/OR/XOR of all elements
		///
		/// RVV semantics:
		/// - Only active lanes (predicate mask) participate in reduction
		/// - Result written to element 0 of destination
		/// - Inactive lanes don't affect the result
		/// </summary>
		static void ApplyReduction(unsigned int op, DataTypeEnum dt, std::span<const unsigned char> src, std::span<unsigned char> dst, int elemSize, unsigned long long vl, unsigned char predIndex, std::shared_ptr<Processor::CPU_Core> &core);

		/// <summary>
		/// Apply dot-product operation to compute scalar result from two vectors.
		/// Computes sum(vs1[i] * vs2[i]) for all active elements.
		///
		/// Dot-product operations (ML/DSP optimized):
		/// - VDOT: signed integer dot product
		/// - VDOTU: unsigned integer dot product
		/// - VDOTF: floating-point dot product
		///
		/// RVV semantics:
		/// - Only active lanes (predicate mask) contribute to sum
		/// - Result written to element 0 of destination
		/// - Inactive lanes don't affect the result
		/// </summary>
		static void ApplyDotProduct(unsigned int op, DataTypeEnum dt, std::span<const unsigned char> a, std::span<const unsigned char> b, std::span<unsigned char> dst, int elemSize, unsigned long long vl, unsigned char predIndex, std::shared_ptr<Processor::CPU_Core> &core);

		/// <summary>
		/// Apply compress operation (predicative movement - ARM SVE style).
		/// Packs active elements to the left, skipping masked-off elements.
		///
		/// Operation: VCOMPRESS
		/// - Collect all elements where mask bit is 1
		/// - Write them sequentially starting from destination[0]
		/// - Inactive elements are skipped (not copied)
		///
		/// Example: src = [A, B, C, D, E], mask = 0b10110 (0,2,3 active)
		///          dst = [A, C, D, ?, ?] (3 elements packed left)
		/// </summary>
		static void ApplyCompress(DataTypeEnum dt, std::span<const unsigned char> src, std::span<unsigned char> dst, int elemSize, unsigned long long vl, unsigned char predIndex, std::shared_ptr<Processor::CPU_Core> &core);

		/// <summary>
		/// Apply expand operation (predicative movement - ARM SVE style).
		/// Unpacks elements from packed source according to mask pattern.
		///
		/// Operation: VEXPAND
		/// - For each active lane (mask bit = 1), read next element from packed source
		/// - For inactive lanes (mask bit = 0), leave destination unchanged unless
		///   mask-agnostic policy allows deterministic overwrite on that lane
		///
		/// Example: src = [A, B, C], mask = 0b10110 (0,2,3 active)
		///          dst = [A, ?, B, C, ?] (elements expanded by mask)
		///
		/// Inverse operation of VCOMPRESS.
		/// </summary>
		static void ApplyExpand(DataTypeEnum dt, std::span<const unsigned char> src, std::span<unsigned char> dst, int elemSize, unsigned long long vl, unsigned char predIndex, bool tailAgnostic, bool maskAgnostic, std::shared_ptr<Processor::CPU_Core> &core);

		/// <summary>
		/// Apply permutation operation (gather with index vector).
		/// Performs indexed access: vd[i] = vs1[vs2[i]]
		///
		/// Operations: VPERMUTE, VRGATHER
		/// - vs2[i] contains index into vs1
		/// - Out-of-bounds indices produce zero
		/// - Supports arbitrary element reordering
		///
		/// Example: vs1 = [A, B, C, D], vs2 = [2, 0, 3, 1]
		///          vd = [C, A, D, B] (permuted by indices)
		/// </summary>
		static void ApplyPermute(DataTypeEnum dt, std::span<const unsigned char> src, std::span<const unsigned char> indices, std::span<unsigned char> dst, int elemSize, unsigned long long vl, unsigned char predIndex, bool tailAgnostic, bool maskAgnostic, std::shared_ptr<Processor::CPU_Core> &core);

		/// <summary>
		/// Apply slide-up operation (shift elements up by immediate offset).
		///
		/// Operation: VSLIDEUP
		/// - Shifts elements towards higher indices by offset
		/// - vd[i+offset] = vs1[i] for valid indices
		/// - Lower offset elements are preserved from vd (slide into gap)
		///
		/// Example: vs1 = [A, B, C, D], offset = 2
		///          vd = [vd[0], vd[1], A, B] (C,D out of range)
		/// </summary>
		static void ApplySlideUp(DataTypeEnum dt, std::span<const unsigned char> src, std::span<unsigned char> dst, int elemSize, unsigned long long vl, unsigned short offset, unsigned char predIndex, bool tailAgnostic, bool maskAgnostic, std::shared_ptr<Processor::CPU_Core> &core);

		/// <summary>
		/// Apply slide-down operation (shift elements down by immediate offset).
		///
		/// Operation: VSLIDEDOWN
		/// - Shifts elements towards lower indices by offset
		/// - vd[i] = vs1[i+offset] for valid source indices
		/// - Upper elements that slide past end are set to zero
		///
		/// Example: vs1 = [A, B, C, D], offset = 2
		///          vd = [C, D, 0, 0]
		/// </summary>
		static void ApplySlideDown(DataTypeEnum dt, std::span<const unsigned char> src, std::span<unsigned char> dst, int elemSize, unsigned long long vl, unsigned short offset, unsigned char predIndex, bool tailAgnostic, bool maskAgnostic, std::shared_ptr<Processor::CPU_Core> &core);

		// --- Private operation execution helpers ---

		/// <summary>
		/// Check if operation is bitwise (XOR/OR/AND/NOT).
		/// Bitwise operations on floats require raw bit manipulation.
		/// </summary>
	private:
		static bool IsBitwiseOp(unsigned int op);

		/// <summary>
		/// Execute binary bitwise operation on raw bits.
		/// Used for bitwise operations on floating-point types.
		/// </summary>
		static unsigned long long ExecuteBitwiseRaw(unsigned int op, unsigned long long x, unsigned long long y);

		/// <summary>
		/// Execute unary bitwise operation on raw bits.
		/// Used for bitwise operations on floating-point types.
		/// </summary>
		static unsigned long long ExecuteUnaryRaw(unsigned int op, unsigned long long x);

		/// <summary>
		/// Execute binary immediate operation on raw bits (for floating-point types).
		/// </summary>
		static unsigned long long ExecuteBinaryImmediateRaw(unsigned int op, unsigned long long x, unsigned short immediate);

		/// <summary>
		/// Execute binary immediate operation for signed integers.
		/// </summary>
		static long long ExecuteBinaryImmediateSignedInt(unsigned int op, long long x, unsigned short immediate);

		/// <summary>
		/// Execute binary immediate operation for unsigned integers.
		/// </summary>
		static unsigned long long ExecuteBinaryImmediateUnsignedInt(unsigned int op, unsigned long long x, unsigned short immediate);

		static double ExecuteBinaryFloat(unsigned int op, double x, double y, std::shared_ptr<Processor::CPU_Core> &core);

		static double DivideFloat(double x, double y, std::shared_ptr<Processor::CPU_Core> &core);

		static double ModuloFloat(double x, double y, std::shared_ptr<Processor::CPU_Core> &core);

		static long long ExecuteBinarySignedInt(unsigned int op, long long x, long long y, std::shared_ptr<Processor::CPU_Core> &core);

		static long long DivideSignedInt(long long x, long long y, std::shared_ptr<Processor::CPU_Core> &core);

		static long long ModuloSignedInt(long long x, long long y, std::shared_ptr<Processor::CPU_Core> &core);

		static unsigned long long ExecuteBinaryUnsignedInt(unsigned int op, unsigned long long x, unsigned long long y, std::shared_ptr<Processor::CPU_Core> &core);

		static unsigned long long DivideUnsignedInt(unsigned long long x, unsigned long long y, std::shared_ptr<Processor::CPU_Core> &core);

		static unsigned long long ModuloUnsignedInt(unsigned long long x, unsigned long long y, std::shared_ptr<Processor::CPU_Core> &core);

		static double ExecuteUnaryFloat(unsigned int op, double x, std::shared_ptr<Processor::CPU_Core> &core);

		static double SqrtFloat(double x, std::shared_ptr<Processor::CPU_Core> &core);

		static long long ExecuteUnarySignedInt(unsigned int op, long long x);

		static unsigned long long ExecuteUnaryUnsignedInt(unsigned int op, unsigned long long x);

		// ========================================================================
		// Vector Comparison Instructions - Generate Predicate Masks
		// ========================================================================

		/// <summary>
		/// Apply vector comparison operation element-wise and generate predicate mask.
		/// Compares two vectors element-by-element and produces a bit mask result.
		///
		/// RVV semantics:
		/// - Each element comparison produces 1 bit in the result mask
		/// - Result bit = 1 if comparison is true, 0 if false
		/// - Respects predicate masking (input mask controls which comparisons execute)
		/// - Output is written to a predicate register (not a vector register)
		/// - Type-aware: handles signed/unsigned integers and floating-point correctly
		///
		/// Comparison operations:
		/// - VCMPEQ: vd[i] = (vs1[i] == vs2[i])
		/// - VCMPNE: vd[i] = (vs1[i] != vs2[i])
		/// - VCMPLT: vd[i] = (vs1[i] < vs2[i])
		/// - VCMPLE: vd[i] = (vs1[i] <= vs2[i])
		/// - VCMPGT: vd[i] = (vs1[i] > vs2[i])
		/// - VCMPGE: vd[i] = (vs1[i] >= vs2[i])
		/// </summary>
		/// <param name="op">Comparison operation code</param>
		/// <param name="dt">Data type for comparison</param>
		/// <param name="a">First source vector buffer</param>
		/// <param name="b">Second source vector buffer</param>
		/// <param name="elemSize">Size of each element in bytes</param>
		/// <param name="vl">Vector length (number of elements to compare)</param>
		/// <param name="predIndex">Input predicate mask index (0 = all active)</param>
		/// <param name="core">CPU core reference for predicate access</param>
		/// <returns>64-bit predicate mask where bit[i]=1 if comparison is true</returns>


		/// </summary>
		static double ExecuteFMAFloat(unsigned int op, double d, double s1, double s2);

		/// <summary>
		/// Execute FMA operation for 32-bit floating-point values (FP32).
		/// Uses MathF.FusedMultiplyAdd for single-precision FMA with one rounding step.
		/// Ensures IEEE 754 compliance for binary32 format (24-bit mantissa).
		/// </summary>
		static float ExecuteFMAFloat32(unsigned int op, float d, float s1, float s2);

		/// <summary>
		/// Execute FMA operation for 64-bit floating-point values (FP64).
		/// Uses Math.FusedMultiplyAdd for double-precision FMA with one rounding step.
		/// Ensures IEEE 754 compliance for binary64 format (53-bit mantissa).
		/// </summary>
		static double ExecuteFMAFloat64(unsigned int op, double d, double s1, double s2);

		/// <summary>
		/// Apply rounding mode to a 32-bit floating-point value.
		/// Implements IEEE 754 rounding modes for binary32 format (24-bit mantissa).
		/// </summary>
		static float ApplyRoundingFloat32(float value, unsigned char roundingMode, std::shared_ptr<Processor::CPU_Core> &core);

		/// <summary>
		/// Execute FMA operation for signed integer values.
		/// </summary>
		static long long ExecuteFMASignedInt(unsigned int op, long long d, long long s1, long long s2);

		/// <summary>
		/// Execute FMA operation for unsigned integer values.
		/// </summary>
		static unsigned long long ExecuteFMAUnsignedInt(unsigned int op, unsigned long long d, unsigned long long s1, unsigned long long s2);

		// ========================================================================
		// Reduction Helper Functions
		// ========================================================================

		/// <summary>
		/// Get identity value for reduction operations (floating-point).
		/// Identity is the neutral element for the operation.
		/// </summary>


	public:
		static unsigned long long ApplyComparison(unsigned int op, DataTypeEnum dt, std::span<const unsigned char> a, std::span<const unsigned char> b, int elemSize, unsigned long long vl, unsigned char predIndex, std::shared_ptr<Processor::CPU_Core> &core);

		/// <summary>
		/// Execute comparison for floating-point values.
		/// </summary>
	private:
		static bool ExecuteComparisonFloat(unsigned int op, double x, double y);

		/// <summary>
		/// Execute comparison for signed integer values.
		/// </summary>
		static bool ExecuteComparisonSignedInt(unsigned int op, long long x, long long y);

		/// <summary>
		/// Execute comparison for unsigned integer values.
		/// </summary>
		static bool ExecuteComparisonUnsignedInt(unsigned int op, unsigned long long x, unsigned long long y);

		// ========================================================================
		// Predicate Mask Manipulation Instructions
		// ========================================================================

		/// <summary>
		/// Apply mask-to-mask logical operation.
		/// Operates on predicate registers directly (bit-level operations).
		///
		/// RVV-style mask operations:
		/// - VMAND: md = ms1 & ms2 (mask AND)
		/// - VMOR: md = ms1 | ms2 (mask OR)
		/// - VMXOR: md = ms1 ^ ms2 (mask XOR)
		///
		/// Use cases:
		/// - Combining multiple comparison results
		/// - Building complex predicates
		/// - Mask register manipulation
		/// </summary>
		/// <param name="op">Mask operation code</param>
		/// <param name="mask1">First source mask (64-bit)</param>
		/// <param name="mask2">Second source mask (64-bit)</param>
		/// <returns>Result mask after operation</returns>
	public:
		static unsigned long long ApplyMaskBinary(unsigned int op, unsigned long long mask1, unsigned long long mask2);

		/// <summary>
		/// Apply unary mask operation.
		/// Operates on a single predicate register.
		///
		/// RVV-style unary mask operations:
		/// - VMNOT: md = ~ms (mask NOT / invert)
		/// </summary>
		/// <param name="op">Mask operation code</param>
		/// <param name="mask">Source mask (64-bit)</param>
		/// <returns>Result mask after operation</returns>
		static unsigned long long ApplyMaskUnary(unsigned int op, unsigned long long mask);

		/// <summary>
		/// Apply VMSBF set-before-first semantics within the active vector length.
		/// Bits before the first set source bit are one; the first set bit and all
		/// later active bits are zero. Tail bits outside VL are always zero.
		/// </summary>
		static unsigned long long ApplyMaskSetBeforeFirst(unsigned long long mask, unsigned long long vl);

		/// <summary>
		/// Count number of set bits in mask (population count).
		/// VPOPC instruction: counts active lanes in predicate register.
		///
		/// Use cases:
		/// - Determine how many elements passed a filter
		/// - Dynamic workload calculation
		/// - Conditional reduction operations
		/// </summary>
		/// <param name="mask">64-bit predicate mask</param>
		/// <param name="vl">Vector length (limits count to first VL bits)</param>
		/// <returns>Number of set bits in mask[0:vl-1]</returns>
		static unsigned long long MaskPopCount(unsigned long long mask, unsigned long long vl);

	private:
		static unsigned long long CreateActiveLaneMask(unsigned long long vl);

		// ========================================================================
		// FMA (Fused Multiply-Add) Helper Functions
		// ========================================================================

		/// <summary>
		/// Execute FMA operation for floating-point values.
		/// Uses Math.FusedMultiplyAdd when available for better accuracy.


		static double GetReductionIdentityFloat(unsigned int op);

		/// <summary>
		/// Get identity value for reduction operations (signed integer).
		/// </summary>
		static long long GetReductionIdentitySignedInt(unsigned int op);

		/// <summary>
		/// Get identity value for reduction operations (unsigned integer).
		/// </summary>
		static unsigned long long GetReductionIdentityUnsignedInt(unsigned int op);

		/// <summary>
		/// Execute reduction operation for floating-point values.
		/// </summary>
		static double ExecuteReductionFloat(unsigned int op, double acc, double value);

		/// <summary>
		/// Execute reduction operation for signed integer values.
		/// </summary>
		static long long ExecuteReductionSignedInt(unsigned int op, long long acc, long long value);

		/// <summary>
		/// Execute reduction operation for unsigned integer values.
		/// </summary>
		static unsigned long long ExecuteReductionUnsignedInt(unsigned int op, unsigned long long acc, unsigned long long value);

		// ========================================================================
		// Saturating Arithmetic Operations (DSP/ML optimized)
		// ========================================================================

		/// <summary>
		/// Saturating addition for signed integers - clamps to min/max on overflow
		/// </summary>


	public:
		static void ApplySaturatingAddElement(unsigned int op, DataTypeEnum dataType, std::span<const unsigned char> sourceA, std::span<const unsigned char> sourceB, std::span<unsigned char> destination, std::shared_ptr<Processor::CPU_Core> &core);

	private:
		static long long SaturatingAddSignedForElementWidth(long long x, long long y, DataTypeEnum dataType, std::shared_ptr<Processor::CPU_Core> &core);

		static unsigned long long SaturatingAddUnsignedForElementWidth(unsigned long long x, unsigned long long y, DataTypeEnum dataType, std::shared_ptr<Processor::CPU_Core> &core);

		static long long SignedMin(DataTypeEnum dataType);

		static long long SignedMax(DataTypeEnum dataType);

		static unsigned long long UnsignedMax(DataTypeEnum dataType);

		static long long SaturatingAddSigned(long long x, long long y, std::shared_ptr<Processor::CPU_Core> &core);

		/// <summary>
		/// Saturating subtraction for signed integers
		/// </summary>
		static long long SaturatingSubSigned(long long x, long long y, std::shared_ptr<Processor::CPU_Core> &core);

		/// <summary>
		/// Saturating multiplication for signed integers
		/// </summary>
		static long long SaturatingMulSigned(long long x, long long y, std::shared_ptr<Processor::CPU_Core> &core);

		/// <summary>
		/// Saturating addition for unsigned integers
		/// </summary>
		static unsigned long long SaturatingAddUnsigned(unsigned long long x, unsigned long long y, std::shared_ptr<Processor::CPU_Core> &core);

		/// <summary>
		/// Saturating subtraction for unsigned integers
		/// </summary>
		static unsigned long long SaturatingSubUnsigned(unsigned long long x, unsigned long long y, std::shared_ptr<Processor::CPU_Core> &core);

		/// <summary>
		/// Saturating multiplication for unsigned integers
		/// </summary>
		static unsigned long long SaturatingMulUnsigned(unsigned long long x, unsigned long long y, std::shared_ptr<Processor::CPU_Core> &core);

		// ========================================================================
		// Bit Manipulation Operations
		// ========================================================================

		/// <summary>
		/// Count leading zeros in a 64-bit value
		/// </summary>
		static int CountLeadingZeros(unsigned long long value);

		/// <summary>
		/// Count trailing zeros in a 64-bit value
		/// </summary>
		static int CountTrailingZeros(unsigned long long value);

		/// <summary>
		/// Count population (number of set bits) in a 64-bit value
		/// </summary>
		static int PopCount(unsigned long long value);

		/// <summary>
		/// Reverse bits in a 64-bit value
		/// </summary>
		static unsigned long long ReverseBits(unsigned long long value);

		/// <summary>
		/// Reverse byte order (endianness swap) in a 64-bit value
		/// </summary>
		static unsigned long long ReverseBytes(unsigned long long value);


	public:
		static void ApplyScanSumElement(unsigned int op, DataTypeEnum dataType, std::span<const unsigned char> sourceElement, std::span<unsigned char> destinationElement, long long &signedAccumulator, unsigned long long &unsignedAccumulator, std::shared_ptr<Processor::CPU_Core> &core);
	};
}
