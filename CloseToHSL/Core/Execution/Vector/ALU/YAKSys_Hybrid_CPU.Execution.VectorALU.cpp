#include "YAKSys_Hybrid_CPU.Execution.VectorALU.h"
#include <bit>
#include <cstring>
#include <algorithm>
#include "../../../State/CoreRuntimeState.h"
#include "../../../State/ArchitecturalState.h"
#include "../../../../../Arch/DataType.h"
#include "../Codec/ElementCodec.h"
#include "../../StreamEngine/AddressGeneration/StreamEngine.AddressGen.h"
#include "../../../Diagnostics/DecodeProjectionFaultException.h"
#include "../../ExecutionFaultContract.h"

using namespace HybridCPU_ISE::Arch;
using namespace YAKSys_Hybrid_CPU::Arch;
using namespace YAKSys_Hybrid_CPU::Core;

namespace YAKSys_Hybrid_CPU::Execution
{

	void VectorALU::MarkVectorDirty(std::shared_ptr<Processor::CPU_Core> &core)
	{
		core->MarkCanonicalVectorDirty();
	}

	void VectorALU::TrackDivByZero(std::shared_ptr<Processor::CPU_Core> &core)
	{
		core->RecordCanonicalVectorException(Processor::CPU_Core::VectorException::DivByZero);
		CheckExceptionMode(core);
	}

	void VectorALU::TrackInvalidOp(std::shared_ptr<Processor::CPU_Core> &core)
	{
		core->RecordCanonicalVectorException(Processor::CPU_Core::VectorException::InvalidOp);
		CheckExceptionMode(core);
	}

	void VectorALU::TrackOverflow(std::shared_ptr<Processor::CPU_Core> &core)
	{
		core->RecordCanonicalVectorException(Processor::CPU_Core::VectorException::Overflow);
		CheckExceptionMode(core);
	}

	void VectorALU::TrackUnderflow(std::shared_ptr<Processor::CPU_Core> &core)
	{
		core->RecordCanonicalVectorException(Processor::CPU_Core::VectorException::Underflow);
		CheckExceptionMode(core);
	}

	void VectorALU::TrackInexact(std::shared_ptr<Processor::CPU_Core> &core)
	{
		core->RecordCanonicalVectorException(Processor::CPU_Core::VectorException::Inexact);
		CheckExceptionMode(core);
	}

	inline double VectorALU::ApplyRounding(double value, unsigned char roundingMode, std::shared_ptr<Processor::CPU_Core> &core)
	{
		// Store original value to detect inexact results
		double original = value;

		// RoundingMode:
		// 0 = RNE (Round to Nearest, ties to Even) - IEEE 754 default
		// 1 = RTZ (Round Towards Zero) - truncate
		// 2 = RDN (Round Down) - towards -infinity
		// 3 = RUP (Round Up) - towards +infinity
		// 4 = RMM (Round to Nearest, ties to Max Magnitude)

	// C# TO C++ CONVERTER TASK: The following 'switch expression' was not converted by C# to C++ Converter:
	//			double rounded = roundingMode switch
	//			{
	//				0 => Math.Round(value, MidpointRounding.ToEven), // RNE
	//				1 => Math.Truncate(value), // RTZ
	//				2 => Math.Floor(value), // RDN
	//				3 => Math.Ceiling(value), // RUP
	//				4 => Math.Round(value, MidpointRounding.AwayFromZero), // RMM
	//				_ => value // Default: no rounding
	//			};

		        double rounded;
        switch (roundingMode) {
        case 0: {
            double lower = std::floor(value);
            double fraction = value - lower;
            rounded = fraction < 0.5 ? lower : fraction > 0.5 ? lower + 1.0 : (std::fmod(lower, 2.0) == 0.0 ? lower : lower + 1.0);
            if (!std::isfinite(value)) rounded = value;
            break;
        }
        case 1: rounded = std::trunc(value); break;
        case 2: rounded = std::floor(value); break;
        case 3: rounded = std::ceil(value); break;
        case 4: rounded = std::round(value); break;
        default: rounded = value; break;
        }

        // Track inexact result if rounding changed the value
		if (rounded != original && !std::isnan(rounded) && !std::isinf(rounded))
		{
			TrackInexact(core);
		}

		return rounded;
	}

	void VectorALU::CheckUnderflow(double result, std::shared_ptr<Processor::CPU_Core> &core)
	{
		// Check for underflow: result is non-zero but smaller than the smallest normal double
		// double.MinValue is the most negative value, not the smallest positive value
		// The smallest positive normal value is approximately 2.2250738585072014e-308
		if (result != 0.0 && !std::isinf(result) && !std::isnan(result))
		{
			double absValue = std::abs(result);
			// Check if result is denormal/subnormal (smaller than smallest normal double)
			if (absValue < 2.2250738585072014e-308)
			{
				TrackUnderflow(core);
			}
		}
	}

	void VectorALU::CheckExceptionMode(std::shared_ptr<Processor::CPU_Core> &core)
	{
		// ExceptionMode:
		// 0 = Accumulate (default) - count exceptions, don't interrupt
		// 1 = Trap on first - raise interrupt on first exception
		// 2 = Trap on any - raise interrupt if any lane has exception

		unsigned char exceptionMode = core->getExceptionStatus()->getExceptionMode();

		// Mode 0: Accumulate only, no interrupt
		if (exceptionMode == 0)
		{
			return;
		}

		// Check for highest priority unmasked exception
		std::optional<Processor::CPU_Core::VectorException> highestException = core->getExceptionStatus()->GetHighestPriorityException();

		// If no unmasked exceptions, don't trap
		if (!highestException)
		{
			return;
		}

		// Mode 1 or 2: Generate trap/interrupt
		// Check if this is the first exception (for mode 1: trap on first)
		unsigned int totalExceptions = core->getExceptionStatus()->TotalExceptions();
		bool shouldTrap = false;

		if (exceptionMode == 1)
		{
			// Trap on first: only trap if this is the first exception
			shouldTrap = (totalExceptions == 1);
		}
		else if (exceptionMode == 2)
		{
			// Trap on any: always trap when any exception occurs
			shouldTrap = true;
		}

		if (shouldTrap)
		{
			// Generate vector exception interrupt with exception type
			GenerateVectorExceptionTrap(core, highestException.value());
		}
	}

	void VectorALU::GenerateVectorExceptionTrap(std::shared_ptr<Processor::CPU_Core> &core, Processor::CPU_Core::VectorException exceptionType)
	{
		// Base vector exception IRQ: 0x80
		// Exception type encoded in low bits: 0x80 + exception type
		constexpr unsigned short VECTOR_EXCEPTION_IRQ_BASE = 0x80;
		unsigned short exceptionIRQ = static_cast<unsigned short>(VECTOR_EXCEPTION_IRQ_BASE + static_cast<unsigned char>(exceptionType));

		// Save vector context BEFORE dispatching to handler
		core->getRuntime()->getArchitectural()->SavedVectorContext = core->SaveVectorContext();

		// Store exception type in saved context for handler inspection
		core->getRuntime()->getArchitectural()->SavedVectorContext.FaultingOpCode = static_cast<unsigned int>(exceptionType);

		unsigned char result = core->DispatchInterrupt(Processor::DeviceType::VectorUnit, exceptionIRQ);

		// Interrupt delivery failures still force the active VT through the
		// canonical interrupt wake transition rather than the legacy core FSM.
		if (result != 0)
		{
			core->ApplyInterruptTransitionToVirtualThread(core->ReadActiveVirtualThreadId());
		}
	}

	void VectorALU::ApplyBinary(unsigned int op, DataTypeEnum dt, std::span<const unsigned char> a, std::span<const unsigned char> b, std::span<unsigned char> dst, int elemSize, unsigned long long vl, unsigned char predIndex, bool tailAgnostic, bool maskAgnostic, std::shared_ptr<Processor::CPU_Core> &core)
	{
		// Mark vector unit as dirty (for context switching)
		MarkVectorDirty(core);

		for (unsigned long long lane = 0; lane < vl; lane++)
		{
			// Check predicate mask: skip inactive lanes for undisturbed policy
			// Effective mask-agnostic = per-instruction bit OR global config
			bool laneActive = core->LaneActive(predIndex, static_cast<int>(lane));
			bool effectiveMaskAgnostic = maskAgnostic || (core->getVectorConfig()->getMaskAgnostic() != 0);
			if (!laneActive && !effectiveMaskAgnostic)
			{
				// Undisturbed: preserve destination value, skip computation
				continue;
			}

			int off = static_cast<int>(lane * static_cast<unsigned long long>(elemSize));

			// Dispatch based on data type category
			if (DataTypeUtils::IsFloatingPoint(dt))
			{
				// Check if operation is bitwise (requires raw bit manipulation)
				if (IsBitwiseOp(op))
				{
					// Use raw bit operations for bitwise ops on floats
					unsigned long long x = ElementCodec::LoadRaw(a, off, dt);
					unsigned long long y = ElementCodec::LoadRaw(b, off, dt);
					unsigned long long r = ExecuteBitwiseRaw(op, x, y);
					ElementCodec::StoreRaw(dst, off, dt, r);
				}
				else
				{
					// Use floating-point arithmetic for non-bitwise ops
					double x = ElementCodec::LoadF(a, off, dt);
					double y = ElementCodec::LoadF(b, off, dt);
					double r = ExecuteBinaryFloat(op, x, y, core);

					// Check for underflow in result
					CheckUnderflow(r, core);

					// Apply rounding mode before storing
					r = ApplyRounding(r, core->getExceptionStatus()->getRoundingMode(), core);

					ElementCodec::StoreF(dst, off, dt, r);
				}
			}
			else if (DataTypeUtils::IsSignedInteger(dt))
			{
				long long x = ElementCodec::LoadI(a, off, dt);
				long long y = ElementCodec::LoadI(b, off, dt);
				long long r = ExecuteBinarySignedInt(op, x, y, core);
				ElementCodec::StoreI(dst, off, dt, r);
			}
			else // Unsigned integer
			{
				unsigned long long x = ElementCodec::LoadU(a, off, dt);
				unsigned long long y = ElementCodec::LoadU(b, off, dt);
				unsigned long long r = ExecuteBinaryUnsignedInt(op, x, y, core);
				ElementCodec::StoreU(dst, off, dt, r);
			}
		}
	}

	void VectorALU::ApplyBinaryImmediate(unsigned int op, DataTypeEnum dt, std::span<const unsigned char> a, unsigned short immediate, std::span<unsigned char> dst, int elemSize, unsigned long long vl, unsigned char predIndex, bool tailAgnostic, bool maskAgnostic, std::shared_ptr<Processor::CPU_Core> &core)
	{
		// Mark vector unit as dirty (for context switching)
		MarkVectorDirty(core);

		for (unsigned long long lane = 0; lane < vl; lane++)
		{
			// Check predicate mask: skip inactive lanes for undisturbed policy
			bool laneActive = core->LaneActive(predIndex, static_cast<int>(lane));
			bool effectiveMaskAgnostic = maskAgnostic || (core->getVectorConfig()->getMaskAgnostic() != 0);
			if (!laneActive && !effectiveMaskAgnostic)
			{
				// Undisturbed: preserve destination value, skip computation
				continue;
			}

			int off = static_cast<int>(lane * static_cast<unsigned long long>(elemSize));

			// Dispatch based on data type category
			if (DataTypeUtils::IsFloatingPoint(dt))
			{
				// Shift operations on floating point are bitwise (raw bit manipulation)
				unsigned long long x = ElementCodec::LoadRaw(a, off, dt);
				unsigned long long r = ExecuteBinaryImmediateRaw(op, x, immediate);
				ElementCodec::StoreRaw(dst, off, dt, r);
			}
			else if (DataTypeUtils::IsSignedInteger(dt))
			{
				long long x = ElementCodec::LoadI(a, off, dt);
				long long r = ExecuteBinaryImmediateSignedInt(op, x, immediate);
				ElementCodec::StoreI(dst, off, dt, r);
			}
			else // Unsigned integer
			{
				unsigned long long x = ElementCodec::LoadU(a, off, dt);
				unsigned long long r = ExecuteBinaryImmediateUnsignedInt(op, x, immediate);
				ElementCodec::StoreU(dst, off, dt, r);
			}
		}
	}

	void VectorALU::ApplyUnary(unsigned int op, DataTypeEnum dt, std::span<const unsigned char> src, std::span<unsigned char> dst, int elemSize, unsigned long long vl, unsigned char predIndex, bool tailAgnostic, bool maskAgnostic, std::shared_ptr<Processor::CPU_Core> &core)
	{
		// Mark vector unit as dirty (for context switching)
		MarkVectorDirty(core);

		for (unsigned long long lane = 0; lane < vl; lane++)
		{
			// Check predicate mask: skip inactive lanes for undisturbed policy
			bool laneActive = core->LaneActive(predIndex, static_cast<int>(lane));
			bool effectiveMaskAgnostic = maskAgnostic || (core->getVectorConfig()->getMaskAgnostic() != 0);
			if (!laneActive && !effectiveMaskAgnostic)
			{
				// Undisturbed: preserve destination value, skip computation
				continue;
			}

			int off = static_cast<int>(lane * static_cast<unsigned long long>(elemSize));

			if (DataTypeUtils::IsFloatingPoint(dt))
			{
				// Check if operation is bitwise (requires raw bit manipulation)
				if (IsBitwiseOp(op))
				{
					// Use raw bit operations for bitwise ops on floats
					unsigned long long x = ElementCodec::LoadRaw(src, off, dt);
					unsigned long long r = ExecuteUnaryRaw(op, x);
					ElementCodec::StoreRaw(dst, off, dt, r);
				}
				else
				{
					// Use floating-point arithmetic for non-bitwise ops
					double x = ElementCodec::LoadF(src, off, dt);
					double r = ExecuteUnaryFloat(op, x, core);

					// Check for underflow in result
					CheckUnderflow(r, core);

					// Apply rounding mode before storing
					r = ApplyRounding(r, core->getExceptionStatus()->getRoundingMode(), core);

					ElementCodec::StoreF(dst, off, dt, r);
				}
			}
			else if (DataTypeUtils::IsSignedInteger(dt))
			{
				long long x = ElementCodec::LoadI(src, off, dt);
				long long r = ExecuteUnarySignedInt(op, x);
				ElementCodec::StoreI(dst, off, dt, r);
			}
			else
			{
				unsigned long long x = ElementCodec::LoadU(src, off, dt);
				unsigned long long r = ExecuteUnaryUnsignedInt(op, x);
				ElementCodec::StoreU(dst, off, dt, r);
			}
		}
	}

	void VectorALU::ApplyFMA(unsigned int op, DataTypeEnum dt, std::span<unsigned char> vd, std::span<const unsigned char> vs1, std::span<const unsigned char> vs2, int elemSize, unsigned long long vl, unsigned char predIndex, bool tailAgnostic, bool maskAgnostic, std::shared_ptr<Processor::CPU_Core> &core)
	{
		// Mark vector unit as dirty (for context switching)
		MarkVectorDirty(core);

		for (unsigned long long lane = 0; lane < vl; lane++)
		{
			// Check predicate mask: skip inactive lanes for undisturbed policy
			bool laneActive = core->LaneActive(predIndex, static_cast<int>(lane));
			bool effectiveMaskAgnostic = maskAgnostic || (core->getVectorConfig()->getMaskAgnostic() != 0);
			if (!laneActive && !effectiveMaskAgnostic)
			{
				// Undisturbed: preserve destination value, skip computation
				continue;
			}

			int off = static_cast<int>(lane * static_cast<unsigned long long>(elemSize));

			// FMA operations: separate FP32 and FP64 paths for IEEE 754 compliance
			if (DataTypeUtils::IsFloatingPoint(dt))
			{
				// Determine if this is FP32 or FP64
				if (dt == DataTypeEnum::FLOAT32)
				{
					// FP32 path: use MathF.FusedMultiplyAdd for proper float precision
                    float fD = static_cast<float>(ElementCodec::LoadF(vd, off, dt));
                    float fS1 = static_cast<float>(ElementCodec::LoadF(vs1, off, dt));
                    float fS2 = static_cast<float>(ElementCodec::LoadF(vs2, off, dt));
					// Execute FMA with single-precision (one rounding step)
					float result = ExecuteFMAFloat32(op, fD, fS1, fS2);

					// Check for invalid operation (NaN inputs/outputs)
					if (std::isnan(result) && !(std::isnan(fD) || std::isnan(fS1) || std::isnan(fS2)))
					{
						TrackInvalidOp(core);
					}

					// Check for underflow (denormal result)
					if (result != 0.0f && !std::isinf(result) && !std::isnan(result))
					{
						float absValue = std::abs(result);
						if (absValue < 1.175494351e-38f) // Smallest normal float
						{
							TrackUnderflow(core);
						}
					}

					// Apply rounding mode (FP32 has 24-bit mantissa)
					result = ApplyRoundingFloat32(result, core->getExceptionStatus()->getRoundingMode(), core);

					// Store back to destination
					ElementCodec::StoreF(vd, off, dt, result);
				}
				else if (dt == DataTypeEnum::FLOAT64)
				{
					// FP64 path: use Math.FusedMultiplyAdd for proper double precision
					double d = ElementCodec::LoadF(vd, off, dt);
					double s1 = ElementCodec::LoadF(vs1, off, dt);
					double s2 = ElementCodec::LoadF(vs2, off, dt);

					// Execute FMA with double-precision (one rounding step)
					double result = ExecuteFMAFloat64(op, d, s1, s2);

					// Check for invalid operation (NaN inputs/outputs)
					if (std::isnan(result) && !(std::isnan(d) || std::isnan(s1) || std::isnan(s2)))
					{
						TrackInvalidOp(core);
					}

					// Check for underflow (denormal result)
					CheckUnderflow(result, core);

					// Apply rounding mode (FP64 has 53-bit mantissa)
					result = ApplyRounding(result, core->getExceptionStatus()->getRoundingMode(), core);

					ElementCodec::StoreF(vd, off, dt, result);
				}
				else
				{
					// Fallback for other float types (shouldn't happen)
					double d = ElementCodec::LoadF(vd, off, dt);
					double s1 = ElementCodec::LoadF(vs1, off, dt);
					double s2 = ElementCodec::LoadF(vs2, off, dt);
					double r = ExecuteFMAFloat(op, d, s1, s2);
					CheckUnderflow(r, core);
					r = ApplyRounding(r, core->getExceptionStatus()->getRoundingMode(), core);
					ElementCodec::StoreF(vd, off, dt, r);
				}
			}
			else if (DataTypeUtils::IsSignedInteger(dt))
			{
				long long d = ElementCodec::LoadI(vd, off, dt);
				long long s1 = ElementCodec::LoadI(vs1, off, dt);
				long long s2 = ElementCodec::LoadI(vs2, off, dt);
				long long r = ExecuteFMASignedInt(op, d, s1, s2);
				ElementCodec::StoreI(vd, off, dt, r);
			}
			else // Unsigned integer
			{
				unsigned long long d = ElementCodec::LoadU(vd, off, dt);
				unsigned long long s1 = ElementCodec::LoadU(vs1, off, dt);
				unsigned long long s2 = ElementCodec::LoadU(vs2, off, dt);
				unsigned long long r = ExecuteFMAUnsignedInt(op, d, s1, s2);
				ElementCodec::StoreU(vd, off, dt, r);
			}
		}
	}

	void VectorALU::ApplyReduction(unsigned int op, DataTypeEnum dt, std::span<const unsigned char> src, std::span<unsigned char> dst, int elemSize, unsigned long long vl, unsigned char predIndex, std::shared_ptr<Processor::CPU_Core> &core)
	{
		// Mark vector unit as dirty (for context switching)
		MarkVectorDirty(core);

		// Initialize accumulator based on operation type
		bool hasValue = false;

		if (DataTypeUtils::IsFloatingPoint(dt))
		{
			double accumulator = GetReductionIdentityFloat(op);

			for (unsigned long long lane = 0; lane < vl; lane++)
			{
				bool laneActive = core->LaneActive(predIndex, static_cast<int>(lane));
				if (!laneActive)
				{
					continue;
				}

				int off = static_cast<int>(lane * static_cast<unsigned long long>(elemSize));
				double value = ElementCodec::LoadF(src, off, dt);

				if (!hasValue)
				{
					accumulator = value;
					hasValue = true;
				}
				else
				{
					accumulator = ExecuteReductionFloat(op, accumulator, value);
				}
			}

			// Write result to element 0 of destination
			ElementCodec::StoreF(dst, 0, dt, accumulator);
		}
		else if (DataTypeUtils::IsSignedInteger(dt))
		{
			long long accumulator = GetReductionIdentitySignedInt(op);

			for (unsigned long long lane = 0; lane < vl; lane++)
			{
				bool laneActive = core->LaneActive(predIndex, static_cast<int>(lane));
				if (!laneActive)
				{
					continue;
				}

				int off = static_cast<int>(lane * static_cast<unsigned long long>(elemSize));
				long long value = ElementCodec::LoadI(src, off, dt);

				if (!hasValue)
				{
					accumulator = value;
					hasValue = true;
				}
				else
				{
					accumulator = ExecuteReductionSignedInt(op, accumulator, value);
				}
			}

			// Write result to element 0 of destination
			ElementCodec::StoreI(dst, 0, dt, accumulator);
		}
		else // Unsigned integer
		{
			unsigned long long accumulator = GetReductionIdentityUnsignedInt(op);

			for (unsigned long long lane = 0; lane < vl; lane++)
			{
				bool laneActive = core->LaneActive(predIndex, static_cast<int>(lane));
				if (!laneActive)
				{
					continue;
				}

				int off = static_cast<int>(lane * static_cast<unsigned long long>(elemSize));
				unsigned long long value = ElementCodec::LoadU(src, off, dt);

				if (!hasValue)
				{
					accumulator = value;
					hasValue = true;
				}
				else
				{
					accumulator = ExecuteReductionUnsignedInt(op, accumulator, value);
				}
			}

			// Write result to element 0 of destination
			ElementCodec::StoreU(dst, 0, dt, accumulator);
		}
	}

	void VectorALU::ApplyDotProduct(unsigned int op, DataTypeEnum dt, std::span<const unsigned char> a, std::span<const unsigned char> b, std::span<unsigned char> dst, int elemSize, unsigned long long vl, unsigned char predIndex, std::shared_ptr<Processor::CPU_Core> &core)
	{
		// Mark vector unit as dirty (for context switching)
		MarkVectorDirty(core);

		// Reuse the shared widening classification so the active dot-product execute contour
		// follows the same source/destination width truth already published by metadata.
		bool isWideningDotProduct = AddressGen::IsWideningOpcode(op);
		bool isWideningE4M3 = isWideningDotProduct && dt == DataTypeEnum::FLOAT8_E4M3;
		bool isWideningE5M2 = isWideningDotProduct && dt == DataTypeEnum::FLOAT8_E5M2;
		bool isWideHalfOrBFloat = op == static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VDOT_WIDE) && (dt == DataTypeEnum::FLOAT16 || dt == DataTypeEnum::BFLOAT16);
		bool isWideSignedByteInteger = op == static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VDOT_WIDE) && dt == DataTypeEnum::INT8;
		bool isWideUnsignedByteInteger = op == static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VDOT_WIDE) && dt == DataTypeEnum::UINT8;

		if (isWideningE4M3 || isWideningE5M2)
		{
			// Widening dot product: FP8 (1-byte) > FP32 (4-byte)
			// Read elements as 1-byte FP8, decode on-the-fly, accumulate in FP32
			float accumulator = 0.0f;

			for (unsigned long long lane = 0; lane < vl; lane++)
			{
				bool laneActive = core->LaneActive(predIndex, static_cast<int>(lane));
				if (!laneActive)
				{
					continue;
				}

				// Read 1-byte FP8 elements
				int off = static_cast<int>(lane); // 1-byte stride
				unsigned char rawA = a[off];
				unsigned char rawB = b[off];

				// Decode FP8 > FP32 on-the-fly (hardware decoder)
				float valA = isWideningE4M3 ? ElementCodec::DecodeE4M3(rawA) : ElementCodec::DecodeE5M2(rawA);
				float valB = isWideningE4M3 ? ElementCodec::DecodeE4M3(rawB) : ElementCodec::DecodeE5M2(rawB);

				// Accumulate in FP32
				accumulator += (valA * valB);
			}

			// Check for underflow/overflow in final result
			CheckUnderflow(accumulator, core);

			// Apply rounding mode before storing
			double rounded = ApplyRounding(accumulator, core->getExceptionStatus()->getRoundingMode(), core);

			// Write 4-byte FP32 result to destination (asymmetric write)
			ElementCodec::StoreF(dst, 0, DataTypeEnum::FLOAT32, rounded);
		}
		else if (isWideHalfOrBFloat)
		{
			double accumulator = 0.0;

			for (unsigned long long lane = 0; lane < vl; lane++)
			{
				bool laneActive = core->LaneActive(predIndex, static_cast<int>(lane));
				if (!laneActive)
				{
					continue;
				}

				int off = static_cast<int>(lane * static_cast<unsigned long long>(elemSize));
				double x = ElementCodec::LoadF(a, off, dt);
				double y = ElementCodec::LoadF(b, off, dt);
				accumulator += x * y;
			}

			CheckUnderflow(accumulator, core);
			accumulator = ApplyRounding(accumulator, core->getExceptionStatus()->getRoundingMode(), core);

			ElementCodec::StoreF(dst, 0, DataTypeEnum::FLOAT32, accumulator);
		}
		else if (isWideSignedByteInteger)
		{
			long long accumulator = 0;

			for (unsigned long long lane = 0; lane < vl; lane++)
			{
				bool laneActive = core->LaneActive(predIndex, static_cast<int>(lane));
				if (!laneActive)
				{
					continue;
				}

				int off = static_cast<int>(lane);
				long long x = ElementCodec::LoadI(a, off, dt);
				long long y = ElementCodec::LoadI(b, off, dt);
				accumulator += x * y;
			}

			ElementCodec::StoreI(dst, 0, DataTypeEnum::INT32, accumulator);
		}
		else if (isWideUnsignedByteInteger)
		{
			unsigned long long accumulator = 0;

			for (unsigned long long lane = 0; lane < vl; lane++)
			{
				bool laneActive = core->LaneActive(predIndex, static_cast<int>(lane));
				if (!laneActive)
				{
					continue;
				}

				int off = static_cast<int>(lane);
				unsigned long long x = ElementCodec::LoadU(a, off, dt);
				unsigned long long y = ElementCodec::LoadU(b, off, dt);
				accumulator += x * y;
			}

			ElementCodec::StoreU(dst, 0, DataTypeEnum::UINT32, accumulator);
		}
		else if (DataTypeUtils::IsFloatingPoint(dt))
		{
			double accumulator = 0.0;

			for (unsigned long long lane = 0; lane < vl; lane++)
			{
				bool laneActive = core->LaneActive(predIndex, static_cast<int>(lane));
				if (!laneActive)
				{
					continue;
				}

				int off = static_cast<int>(lane * static_cast<unsigned long long>(elemSize));
				double x = ElementCodec::LoadF(a, off, dt);
				double y = ElementCodec::LoadF(b, off, dt);
				accumulator += x * y;
			}

			// Check for underflow in final result
			CheckUnderflow(accumulator, core);

			// Apply rounding mode before storing
			accumulator = ApplyRounding(accumulator, core->getExceptionStatus()->getRoundingMode(), core);

			// Write result to element 0 of destination
			ElementCodec::StoreF(dst, 0, dt, accumulator);
		}
		else if (DataTypeUtils::IsSignedInteger(dt))
		{
			long long accumulator = 0;

			for (unsigned long long lane = 0; lane < vl; lane++)
			{
				bool laneActive = core->LaneActive(predIndex, static_cast<int>(lane));
				if (!laneActive)
				{
					continue;
				}

				int off = static_cast<int>(lane * static_cast<unsigned long long>(elemSize));
				long long x = ElementCodec::LoadI(a, off, dt);
				long long y = ElementCodec::LoadI(b, off, dt);
				accumulator += x * y;
			}

			// Write result to element 0 of destination
			ElementCodec::StoreI(dst, 0, dt, accumulator);
		}
		else // Unsigned integer
		{
			unsigned long long accumulator = 0;

			for (unsigned long long lane = 0; lane < vl; lane++)
			{
				bool laneActive = core->LaneActive(predIndex, static_cast<int>(lane));
				if (!laneActive)
				{
					continue;
				}

				int off = static_cast<int>(lane * static_cast<unsigned long long>(elemSize));
				unsigned long long x = ElementCodec::LoadU(a, off, dt);
				unsigned long long y = ElementCodec::LoadU(b, off, dt);
				accumulator += x * y;
			}

			// Write result to element 0 of destination
			ElementCodec::StoreU(dst, 0, dt, accumulator);
		}
	}

	void VectorALU::ApplyCompress(DataTypeEnum dt, std::span<const unsigned char> src, std::span<unsigned char> dst, int elemSize, unsigned long long vl, unsigned char predIndex, std::shared_ptr<Processor::CPU_Core> &core)
	{
		// Mark vector unit as dirty (for context switching)
		MarkVectorDirty(core);

		int dstIndex = 0;

		for (unsigned long long lane = 0; lane < vl; lane++)
		{
			bool laneActive = core->LaneActive(predIndex, static_cast<int>(lane));
			if (!laneActive)
			{
				continue;
			}

			int srcOff = static_cast<int>(lane * static_cast<unsigned long long>(elemSize));
			int dstOff = dstIndex * elemSize;

			// Copy element from source to compressed destination
			std::memmove(dst.subspan(dstOff, elemSize).data(), src.subspan(srcOff, elemSize).data(), elemSize);
			dstIndex++;
		}
	}

	void VectorALU::ApplyExpand(DataTypeEnum dt, std::span<const unsigned char> src, std::span<unsigned char> dst, int elemSize, unsigned long long vl, unsigned char predIndex, bool tailAgnostic, bool maskAgnostic, std::shared_ptr<Processor::CPU_Core> &core)
	{
		// Mark vector unit as dirty (for context switching)
		MarkVectorDirty(core);

		int srcIndex = 0;

		for (unsigned long long lane = 0; lane < vl; lane++)
		{
			bool laneActive = core->LaneActive(predIndex, static_cast<int>(lane));
			bool effectiveMaskAgnostic = maskAgnostic || (core->getVectorConfig()->getMaskAgnostic() != 0);
			bool laneMayOverwrite = laneActive || effectiveMaskAgnostic;

			if (!laneMayOverwrite)
			{
				// Undisturbed: preserve destination value, skip
				continue;
			}

			int dstOff = static_cast<int>(lane * static_cast<unsigned long long>(elemSize));

			if (laneActive)
			{
				// Active lane: read next element from packed source
				int srcOff = srcIndex * elemSize;
				std::memmove(dst.subspan(dstOff, elemSize).data(), src.subspan(srcOff, elemSize).data(), elemSize);
				srcIndex++;
			}
			else
			{
				// Deterministic overwrite keeps the packed-source cursor aligned with
				// active lanes while still honoring agnostic follow-through.
				std::fill(dst.subspan(dstOff, elemSize).begin(), dst.subspan(dstOff, elemSize).end(), 0);
			}
		}
	}

	void VectorALU::ApplyPermute(DataTypeEnum dt, std::span<const unsigned char> src, std::span<const unsigned char> indices, std::span<unsigned char> dst, int elemSize, unsigned long long vl, unsigned char predIndex, bool tailAgnostic, bool maskAgnostic, std::shared_ptr<Processor::CPU_Core> &core)
	{
		// Mark vector unit as dirty (for context switching)
		MarkVectorDirty(core);

		for (unsigned long long lane = 0; lane < vl; lane++)
		{
			bool laneActive = core->LaneActive(predIndex, static_cast<int>(lane));
			bool effectiveMaskAgnostic = maskAgnostic || (core->getVectorConfig()->getMaskAgnostic() != 0);
			bool laneMayOverwrite = laneActive || effectiveMaskAgnostic;

			if (!laneMayOverwrite)
			{
				// Undisturbed: preserve destination value
				continue;
			}

			int dstOff = static_cast<int>(lane * static_cast<unsigned long long>(elemSize));

			// Agnostic masked-off lanes are allowed to take the same deterministic
			// gather result as active lanes instead of silently preserving stale data.
			unsigned long long index = ElementCodec::LoadU(indices, dstOff, dt);

			if (index < vl)
			{
				// Valid index: gather from source
				int srcOff = static_cast<int>(index * static_cast<unsigned long long>(elemSize));
				std::memmove(dst.subspan(dstOff, elemSize).data(), src.subspan(srcOff, elemSize).data(), elemSize);
			}
			else
			{
				// Out-of-bounds: write zero
				std::fill(dst.subspan(dstOff, elemSize).begin(), dst.subspan(dstOff, elemSize).end(), 0);
			}
		}
	}

	void VectorALU::ApplySlideUp(DataTypeEnum dt, std::span<const unsigned char> src, std::span<unsigned char> dst, int elemSize, unsigned long long vl, unsigned short offset, unsigned char predIndex, bool tailAgnostic, bool maskAgnostic, std::shared_ptr<Processor::CPU_Core> &core)
	{
		// Mark vector unit as dirty (for context switching)
		MarkVectorDirty(core);

		for (unsigned long long lane = 0; lane < vl; lane++)
		{
			bool laneActive = core->LaneActive(predIndex, static_cast<int>(lane));
			bool effectiveMaskAgnostic = maskAgnostic || (core->getVectorConfig()->getMaskAgnostic() != 0);
			bool laneMayOverwrite = laneActive || effectiveMaskAgnostic;

			if (!laneMayOverwrite)
			{
				// Undisturbed: preserve destination value
				continue;
			}

			int dstOff = static_cast<int>(lane * static_cast<unsigned long long>(elemSize));

			if (lane >= offset)
			{
				// Copy from source at (lane - offset)
				unsigned long long srcLane = lane - offset;
				int srcOff = static_cast<int>(srcLane * static_cast<unsigned long long>(elemSize));
				std::memmove(dst.subspan(dstOff, elemSize).data(), src.subspan(srcOff, elemSize).data(), elemSize);
			}
			// else: slide-up gap lanes preserve destination; agnostic policy does not
			// require every inactive lane to overwrite.
		}
	}

	void VectorALU::ApplySlideDown(DataTypeEnum dt, std::span<const unsigned char> src, std::span<unsigned char> dst, int elemSize, unsigned long long vl, unsigned short offset, unsigned char predIndex, bool tailAgnostic, bool maskAgnostic, std::shared_ptr<Processor::CPU_Core> &core)
	{
		// Mark vector unit as dirty (for context switching)
		MarkVectorDirty(core);

		for (unsigned long long lane = 0; lane < vl; lane++)
		{
			bool laneActive = core->LaneActive(predIndex, static_cast<int>(lane));
			bool effectiveMaskAgnostic = maskAgnostic || (core->getVectorConfig()->getMaskAgnostic() != 0);
			bool laneMayOverwrite = laneActive || effectiveMaskAgnostic;

			if (!laneMayOverwrite)
			{
				// Undisturbed: preserve destination value
				continue;
			}

			int dstOff = static_cast<int>(lane * static_cast<unsigned long long>(elemSize));

			unsigned long long srcLane = lane + offset;
			if (srcLane < vl)
			{
				// Valid source index: copy element
				int srcOff = static_cast<int>(srcLane * static_cast<unsigned long long>(elemSize));
				std::memmove(dst.subspan(dstOff, elemSize).data(), src.subspan(srcOff, elemSize).data(), elemSize);
			}
			else
			{
				// Out of bounds: write zero
				std::fill(dst.subspan(dstOff, elemSize).begin(), dst.subspan(dstOff, elemSize).end(), 0);
			}
		}
	}

	inline bool VectorALU::IsBitwiseOp(unsigned int op)
	{
		switch (op) {
		case Processor::CPU_Core::IsaOpcodeValues::VXOR: return true;
		case Processor::CPU_Core::IsaOpcodeValues::VAND: return true;
		default: return false;
		}
	}

	unsigned long long VectorALU::ExecuteBitwiseRaw(unsigned int op, unsigned long long x, unsigned long long y)
	{
		switch (op) {
		case Processor::CPU_Core::IsaOpcodeValues::VXOR: return x ^ y;
		case Processor::CPU_Core::IsaOpcodeValues::VAND: return x & y;
		default: return 0;
		}
	}

	unsigned long long VectorALU::ExecuteUnaryRaw(unsigned int op, unsigned long long x)
	{
		switch (op) {
		case Processor::CPU_Core::IsaOpcodeValues::VNOT: return ~x;
		default: return 0;
		}
	}

	unsigned long long VectorALU::ExecuteBinaryImmediateRaw(unsigned int op, unsigned long long x, unsigned short immediate)
	{
		int shiftAmount = immediate & 0x3F;
		switch (op) {
		case Processor::CPU_Core::IsaOpcodeValues::VSLL: return x << shiftAmount;
		case Processor::CPU_Core::IsaOpcodeValues::VSRA: return x >> shiftAmount;
		default: return 0;
		}
	}

	inline long long VectorALU::ExecuteBinaryImmediateSignedInt(unsigned int op, long long x, unsigned short immediate)
	{
        auto ux = static_cast<unsigned long long>(x); auto si = static_cast<long long>(static_cast<short>(immediate)); auto ui = static_cast<unsigned long long>(si); int sh = immediate & 63;
        switch(op) {
        case Processor::CPU_Core::IsaOpcodeValues::VSLL: return std::bit_cast<long long>(ux << sh);
        case Processor::CPU_Core::IsaOpcodeValues::VSRA: return x >> sh;
        case Processor::CPU_Core::IsaOpcodeValues::VSRL: return std::bit_cast<long long>(ux >> sh);
        case Processor::CPU_Core::IsaOpcodeValues::VADD: return std::bit_cast<long long>(ux + ui);
        case Processor::CPU_Core::IsaOpcodeValues::VSUB: return std::bit_cast<long long>(ux - ui);
        case Processor::CPU_Core::IsaOpcodeValues::VMUL: return std::bit_cast<long long>(ux * ui);
        case Processor::CPU_Core::IsaOpcodeValues::VAND: return x & immediate;
        case Processor::CPU_Core::IsaOpcodeValues::VOR: return x | immediate;
        case Processor::CPU_Core::IsaOpcodeValues::VXOR: return x ^ immediate;
        default: return 0;
        }
	}

	unsigned long long VectorALU::ExecuteBinaryImmediateUnsignedInt(unsigned int op, unsigned long long x, unsigned short immediate)
	{
        int sh = immediate & 63; switch(op) {
        case Processor::CPU_Core::IsaOpcodeValues::VSLL: return x << sh;
        case Processor::CPU_Core::IsaOpcodeValues::VSRL: case Processor::CPU_Core::IsaOpcodeValues::VSRA: return x >> sh;
        case Processor::CPU_Core::IsaOpcodeValues::VADD: return x + immediate;
        case Processor::CPU_Core::IsaOpcodeValues::VSUB: return x - immediate;
        case Processor::CPU_Core::IsaOpcodeValues::VMUL: return x * immediate;
        case Processor::CPU_Core::IsaOpcodeValues::VAND: return x & immediate;
        case Processor::CPU_Core::IsaOpcodeValues::VOR: return x | immediate;
        case Processor::CPU_Core::IsaOpcodeValues::VXOR: return x ^ immediate;
        default: return 0;
        }
	}

	inline double VectorALU::ExecuteBinaryFloat(unsigned int op, double x, double y, std::shared_ptr<Processor::CPU_Core> &core)
	{
		switch (op) {
		case Processor::CPU_Core::IsaOpcodeValues::VADD: return x + y;
		case Processor::CPU_Core::IsaOpcodeValues::VMUL: return x * y;
		case Processor::CPU_Core::IsaOpcodeValues::VMOD: return ModuloFloat(x, y, core);
		case Processor::CPU_Core::IsaOpcodeValues::VMIN: return std::min(x, y);
		default: return 0.0;
		}
	}

	inline double VectorALU::DivideFloat(double x, double y, std::shared_ptr<Processor::CPU_Core> &core)
	{
		if (y != 0.0)
		{
			return x / y;
		}
		TrackDivByZero(core);
		return 0.0;
	}

	inline double VectorALU::ModuloFloat(double x, double y, std::shared_ptr<Processor::CPU_Core> &core)
	{
		if (y != 0.0)
		{
			return std::fmod(x, y);
		}
		TrackDivByZero(core);
		return 0.0;
	}

	inline long long VectorALU::ExecuteBinarySignedInt(unsigned int op, long long x, long long y, std::shared_ptr<Processor::CPU_Core> &core)
	{
        auto ux = static_cast<unsigned long long>(x), uy = static_cast<unsigned long long>(y); int sh = static_cast<int>(uy & 63);
        switch(op) {
        case Processor::CPU_Core::IsaOpcodeValues::VADD: return std::bit_cast<long long>(ux + uy);
        case Processor::CPU_Core::IsaOpcodeValues::VSUB: return std::bit_cast<long long>(ux - uy);
        case Processor::CPU_Core::IsaOpcodeValues::VMUL: return std::bit_cast<long long>(ux * uy);
        case Processor::CPU_Core::IsaOpcodeValues::VDIV: return DivideSignedInt(x,y,core);
        case Processor::CPU_Core::IsaOpcodeValues::VMOD: return ModuloSignedInt(x,y,core);
        case Processor::CPU_Core::IsaOpcodeValues::VXOR: return x ^ y;
        case Processor::CPU_Core::IsaOpcodeValues::VOR: return x | y;
        case Processor::CPU_Core::IsaOpcodeValues::VAND: return x & y;
        case Processor::CPU_Core::IsaOpcodeValues::VSLL: return std::bit_cast<long long>(ux << sh);
        case Processor::CPU_Core::IsaOpcodeValues::VSRA: return x >> sh;
        case Processor::CPU_Core::IsaOpcodeValues::VSRL: return std::bit_cast<long long>(ux >> sh);
        case Processor::CPU_Core::IsaOpcodeValues::VMIN: return std::min(x,y);
        case Processor::CPU_Core::IsaOpcodeValues::VMAX: return std::max(x,y);
        default: return 0;
        }
	}

	inline long long VectorALU::DivideSignedInt(long long x, long long y, std::shared_ptr<Processor::CPU_Core> &core)
	{
		if (y != 0)
		{
			return x / y;
		}
		TrackDivByZero(core);
		return 0LL;
	}

	inline long long VectorALU::ModuloSignedInt(long long x, long long y, std::shared_ptr<Processor::CPU_Core> &core)
	{
		if (y != 0)
		{
			return x % y;
		}
		TrackDivByZero(core);
		return 0LL;
	}

	unsigned long long VectorALU::ExecuteBinaryUnsignedInt(unsigned int op, unsigned long long x, unsigned long long y, std::shared_ptr<Processor::CPU_Core> &core)
	{
		switch (op) {
		case Processor::CPU_Core::IsaOpcodeValues::VADD: return x + y;
		case Processor::CPU_Core::IsaOpcodeValues::VMUL: return x * y;
		case Processor::CPU_Core::IsaOpcodeValues::VMOD: return ModuloUnsignedInt(x, y, core);
		case Processor::CPU_Core::IsaOpcodeValues::VOR: return x | y;
		case Processor::CPU_Core::IsaOpcodeValues::VSLL: return x << static_cast<int>(y & 0x3F);
		case Processor::CPU_Core::IsaOpcodeValues::VSRA: return x >> static_cast<int>(y & 0x3F);
		case Processor::CPU_Core::IsaOpcodeValues::VMINU: return std::min(x, y);
		default: return 0;
		}
	}

	unsigned long long VectorALU::DivideUnsignedInt(unsigned long long x, unsigned long long y, std::shared_ptr<Processor::CPU_Core> &core)
	{
		if (y != 0)
		{
			return x / y;
		}
		TrackDivByZero(core);
		return 0ULL;
	}

	unsigned long long VectorALU::ModuloUnsignedInt(unsigned long long x, unsigned long long y, std::shared_ptr<Processor::CPU_Core> &core)
	{
		if (y != 0)
		{
			return x % y;
		}
		TrackDivByZero(core);
		return 0ULL;
	}

	inline double VectorALU::ExecuteUnaryFloat(unsigned int op, double x, std::shared_ptr<Processor::CPU_Core> &core)
	{
		switch (op) {
		case Processor::CPU_Core::IsaOpcodeValues::VSQRT: return SqrtFloat(x, core);
		default: return 0.0;
		}
	}

	inline double VectorALU::SqrtFloat(double x, std::shared_ptr<Processor::CPU_Core> &core)
	{
		if (x >= 0.0)
		{
			return std::sqrt(x);
		}
		TrackInvalidOp(core);
		return NAN;
	}

	inline long long VectorALU::ExecuteUnarySignedInt(unsigned int op, long long x)
	{
        switch(op) {
        case Processor::CPU_Core::IsaOpcodeValues::VSQRT: return static_cast<long long>(std::sqrt(static_cast<double>(x)));
        case Processor::CPU_Core::IsaOpcodeValues::VNOT: return ~x;
        case Processor::CPU_Core::IsaOpcodeValues::VREVERSE: return static_cast<long long>(ReverseBits(static_cast<unsigned long long>(x)));
        case Processor::CPU_Core::IsaOpcodeValues::VPOPCNT: return static_cast<long long>(PopCount(static_cast<unsigned long long>(x)));
        case Processor::CPU_Core::IsaOpcodeValues::VCLZ: return static_cast<long long>(CountLeadingZeros(static_cast<unsigned long long>(x)));
        case Processor::CPU_Core::IsaOpcodeValues::VCTZ: return static_cast<long long>(CountTrailingZeros(static_cast<unsigned long long>(x)));
        case Processor::CPU_Core::IsaOpcodeValues::VBREV8: return static_cast<long long>(ReverseBytes(static_cast<unsigned long long>(x)));
        default: return 0;
        }
	}

	unsigned long long VectorALU::ExecuteUnaryUnsignedInt(unsigned int op, unsigned long long x)
	{
        switch(op) {
        case Processor::CPU_Core::IsaOpcodeValues::VSQRT: return static_cast<unsigned long long>(std::sqrt(static_cast<double>(x)));
        case Processor::CPU_Core::IsaOpcodeValues::VNOT: return ~x;
        case Processor::CPU_Core::IsaOpcodeValues::VREVERSE: return static_cast<unsigned long long>(ReverseBits(static_cast<unsigned long long>(x)));
        case Processor::CPU_Core::IsaOpcodeValues::VPOPCNT: return static_cast<unsigned long long>(PopCount(static_cast<unsigned long long>(x)));
        case Processor::CPU_Core::IsaOpcodeValues::VCLZ: return static_cast<unsigned long long>(CountLeadingZeros(static_cast<unsigned long long>(x)));
        case Processor::CPU_Core::IsaOpcodeValues::VCTZ: return static_cast<unsigned long long>(CountTrailingZeros(static_cast<unsigned long long>(x)));
        case Processor::CPU_Core::IsaOpcodeValues::VBREV8: return static_cast<unsigned long long>(ReverseBytes(static_cast<unsigned long long>(x)));
        default: return 0;
        }
	}

	inline double VectorALU::ExecuteFMAFloat(unsigned int op, double d, double s1, double s2)
	{
		switch (op) {
		case Processor::CPU_Core::IsaOpcodeValues::VFMADD: return std::fma(d, s1, s2);
		case Processor::CPU_Core::IsaOpcodeValues::VFMSUB: return std::fma(d, s1, -s2);
		case Processor::CPU_Core::IsaOpcodeValues::VFNMADD: return std::fma(-d, s1, s2);
		case Processor::CPU_Core::IsaOpcodeValues::VFNMSUB: return std::fma(-d, s1, -s2);
		default: return 0.0;
		}
	}

	inline float VectorALU::ExecuteFMAFloat32(unsigned int op, float d, float s1, float s2)
	{
		switch (op) {
		case Processor::CPU_Core::IsaOpcodeValues::VFMADD: return std::fma(d, s1, s2);
		case Processor::CPU_Core::IsaOpcodeValues::VFMSUB: return std::fma(d, s1, -s2);
		case Processor::CPU_Core::IsaOpcodeValues::VFNMADD: return std::fma(-d, s1, s2);
		case Processor::CPU_Core::IsaOpcodeValues::VFNMSUB: return std::fma(-d, s1, -s2);
		default: return 0.0f;
		}
	}

	inline double VectorALU::ExecuteFMAFloat64(unsigned int op, double d, double s1, double s2)
	{
		switch (op) {
		case Processor::CPU_Core::IsaOpcodeValues::VFMADD: return std::fma(d, s1, s2);
		case Processor::CPU_Core::IsaOpcodeValues::VFMSUB: return std::fma(d, s1, -s2);
		case Processor::CPU_Core::IsaOpcodeValues::VFNMADD: return std::fma(-d, s1, s2);
		case Processor::CPU_Core::IsaOpcodeValues::VFNMSUB: return std::fma(-d, s1, -s2);
		default: return 0.0;
		}
	}

	inline float VectorALU::ApplyRoundingFloat32(float value, unsigned char roundingMode, std::shared_ptr<Processor::CPU_Core> &core)
	{
		// Store original value to detect inexact results
		float original = value;

		// RoundingMode:
		// 0 = RNE (Round to Nearest, ties to Even) - IEEE 754 default
		// 1 = RTZ (Round Towards Zero) - truncate
		// 2 = RDN (Round Down) - towards -infinity
		// 3 = RUP (Round Up) - towards +infinity
		// 4 = RMM (Round to Nearest, ties to Max Magnitude)

		float temp_var58;
		switch (roundingMode)
		{
			case 0: // RNE
				temp_var58 = BankersRounding::round(value);
				break;
			case 1: // RTZ
				temp_var58 = std::truncf(value);
				break;
			case 2: // RDN
				temp_var58 = std::floorf(value);
				break;
			case 3: // RUP
				temp_var58 = std::ceilf(value);
				break;
			case 4: // RMM
				temp_var58 = std::roundf(value);
				break;
			default: // Default: no rounding
				temp_var58 = value;
				break;
		}
		float rounded = temp_var58;

		        // Track inexact result if rounding changed the value
		if (rounded != original && !std::isnan(rounded) && !std::isinf(rounded))
		{
			TrackInexact(core);
		}

		return rounded;
	}

	inline long long VectorALU::ExecuteFMASignedInt(unsigned int op, long long d, long long s1, long long s2)
	{
        auto product = static_cast<unsigned long long>(d) * static_cast<unsigned long long>(s1), u = static_cast<unsigned long long>(s2);
        switch(op) {
        case Processor::CPU_Core::IsaOpcodeValues::VFMADD: return std::bit_cast<long long>(product + u);
        case Processor::CPU_Core::IsaOpcodeValues::VFMSUB: return std::bit_cast<long long>(product - u);
        case Processor::CPU_Core::IsaOpcodeValues::VFNMADD: return std::bit_cast<long long>(0ULL - product + u);
        case Processor::CPU_Core::IsaOpcodeValues::VFNMSUB: return std::bit_cast<long long>(0ULL - product - u);
        default: return 0;
        }
	}

	unsigned long long VectorALU::ExecuteFMAUnsignedInt(unsigned int op, unsigned long long d, unsigned long long s1, unsigned long long s2)
	{
        auto product = d * s1; switch(op) {
        case Processor::CPU_Core::IsaOpcodeValues::VFMADD: return product + s2;
        case Processor::CPU_Core::IsaOpcodeValues::VFMSUB: return product - s2;
        case Processor::CPU_Core::IsaOpcodeValues::VFNMADD: return 0ULL - product + s2;
        case Processor::CPU_Core::IsaOpcodeValues::VFNMSUB: return 0ULL - product - s2;
        default: return 0;
        }
	}

	unsigned long long VectorALU::ApplyComparison(unsigned int op, DataTypeEnum dt, std::span<const unsigned char> a, std::span<const unsigned char> b, int elemSize, unsigned long long vl, unsigned char predIndex, std::shared_ptr<Processor::CPU_Core> &core)
	{
		// Mark vector unit as dirty (for context switching)
		MarkVectorDirty(core);

		unsigned long long resultMask = 0;

		// Limit comparison to 64 lanes (predicate register width)
		unsigned long long effectiveVL = std::min(vl, 64ULL);

		for (unsigned long long lane = 0; lane < effectiveVL; lane++)
		{
			// Check input predicate mask: skip inactive lanes
			bool laneActive = core->LaneActive(predIndex, static_cast<int>(lane));
			if (!laneActive)
			{
				// Masked-off lane: result bit = 0
				continue;
			}

			int off = static_cast<int>(lane * static_cast<unsigned long long>(elemSize));
			bool compResult = false;

			// Perform type-specific comparison
			if (DataTypeUtils::IsFloatingPoint(dt))
			{
				double x = ElementCodec::LoadF(a, off, dt);
				double y = ElementCodec::LoadF(b, off, dt);
				compResult = ExecuteComparisonFloat(op, x, y);
			}
			else if (DataTypeUtils::IsSignedInteger(dt))
			{
				long long x = ElementCodec::LoadI(a, off, dt);
				long long y = ElementCodec::LoadI(b, off, dt);
				compResult = ExecuteComparisonSignedInt(op, x, y);
			}
			else // Unsigned integer
			{
				unsigned long long x = ElementCodec::LoadU(a, off, dt);
				unsigned long long y = ElementCodec::LoadU(b, off, dt);
				compResult = ExecuteComparisonUnsignedInt(op, x, y);
			}

			// Set result bit if comparison is true
			if (compResult)
			{
				resultMask |= (1ULL << static_cast<int>(lane));
			}
		}

		return resultMask;
	}

	inline bool VectorALU::ExecuteComparisonFloat(unsigned int op, double x, double y)
	{
		switch (op) {
		case Processor::CPU_Core::IsaOpcodeValues::VCMPEQ: return x == y;
		case Processor::CPU_Core::IsaOpcodeValues::VCMPLT: return x < y;
		case Processor::CPU_Core::IsaOpcodeValues::VCMPGT: return x > y;
		default: return false;
		}
	}

	inline bool VectorALU::ExecuteComparisonSignedInt(unsigned int op, long long x, long long y)
	{
		switch (op) {
		case Processor::CPU_Core::IsaOpcodeValues::VCMPEQ: return x == y;
		case Processor::CPU_Core::IsaOpcodeValues::VCMPLT: return x < y;
		case Processor::CPU_Core::IsaOpcodeValues::VCMPGT: return x > y;
		default: return false;
		}
	}

	inline bool VectorALU::ExecuteComparisonUnsignedInt(unsigned int op, unsigned long long x, unsigned long long y)
	{
		switch (op) {
		case Processor::CPU_Core::IsaOpcodeValues::VCMPEQ: return x == y;
		case Processor::CPU_Core::IsaOpcodeValues::VCMPLT: return x < y;
		case Processor::CPU_Core::IsaOpcodeValues::VCMPGT: return x > y;
		default: return false;
		}
	}

	unsigned long long VectorALU::ApplyMaskBinary(unsigned int op, unsigned long long mask1, unsigned long long mask2)
	{
		switch (op) {
		case Processor::CPU_Core::IsaOpcodeValues::VMAND: return mask1 & mask2;
		case Processor::CPU_Core::IsaOpcodeValues::VMXOR: return mask1 ^ mask2;
		default: return 0;
		}
	}

	unsigned long long VectorALU::ApplyMaskUnary(unsigned int op, unsigned long long mask)
	{
		switch (op) {
		case Processor::CPU_Core::IsaOpcodeValues::VMNOT: return ~mask;
		default: return 0;
		}
	}

	unsigned long long VectorALU::ApplyMaskSetBeforeFirst(unsigned long long mask, unsigned long long vl)
	{
		unsigned long long activeMask = CreateActiveLaneMask(vl);
		unsigned long long activeSource = mask & activeMask;
		if (activeSource == 0)
		{
			return activeMask;
		}

		int firstSetLane = std::countr_zero(activeSource);
		return firstSetLane == 0 ? 0ULL : (1ULL << firstSetLane) - 1ULL;
	}

	unsigned long long VectorALU::MaskPopCount(unsigned long long mask, unsigned long long vl)
	{
		// Limit to VL bits (mask off bits beyond VL)
		unsigned long long maskedValue = mask & CreateActiveLaneMask(vl);

		// Use hardware popcount if available, fallback to software
		return static_cast<unsigned long long>(std::popcount(maskedValue));
	}

	unsigned long long VectorALU::CreateActiveLaneMask(unsigned long long vl)
	{
		unsigned long long effectiveVL = std::min(vl, 64ULL);
		if (effectiveVL == 0)
		{
			return 0ULL;
		}

		return effectiveVL == 64 ? std::numeric_limits<unsigned long long>::max() : (1ULL << static_cast<int>(effectiveVL)) - 1ULL;
	}

	inline double VectorALU::GetReductionIdentityFloat(unsigned int op)
	{
		switch (op) {
		case Processor::CPU_Core::IsaOpcodeValues::VREDSUM: return 0.0;
		case Processor::CPU_Core::IsaOpcodeValues::VREDMIN: return std::numeric_limits<double>::max();
		default: return 0.0;
		}
	}

	inline long long VectorALU::GetReductionIdentitySignedInt(unsigned int op)
	{
		switch (op) {
		case Processor::CPU_Core::IsaOpcodeValues::VREDSUM: return 0;
		case Processor::CPU_Core::IsaOpcodeValues::VREDMIN: return std::numeric_limits<long long>::max();
		case Processor::CPU_Core::IsaOpcodeValues::VREDOR: return 0;
		default: return 0;
		}
	}

	unsigned long long VectorALU::GetReductionIdentityUnsignedInt(unsigned int op)
	{
		switch (op) {
		case Processor::CPU_Core::IsaOpcodeValues::VREDSUM: return 0;
		case Processor::CPU_Core::IsaOpcodeValues::VREDMAXU: return 0ULL;
		case Processor::CPU_Core::IsaOpcodeValues::VREDMINU: return std::numeric_limits<unsigned long long>::max();
		case Processor::CPU_Core::IsaOpcodeValues::VREDOR: return 0;
		default: return 0;
		}
	}

	inline double VectorALU::ExecuteReductionFloat(unsigned int op, double acc, double value)
	{
		switch (op) {
		case Processor::CPU_Core::IsaOpcodeValues::VREDSUM: return acc + value;
		case Processor::CPU_Core::IsaOpcodeValues::VREDMIN: return std::min(acc, value);
		default: return acc;
		}
	}

	inline long long VectorALU::ExecuteReductionSignedInt(unsigned int op, long long acc, long long value)
	{
        switch(op) {
        case Processor::CPU_Core::IsaOpcodeValues::VREDSUM: return std::bit_cast<long long>(static_cast<unsigned long long>(acc) + static_cast<unsigned long long>(value));
        case Processor::CPU_Core::IsaOpcodeValues::VREDMAX: return std::max(acc,value);
        case Processor::CPU_Core::IsaOpcodeValues::VREDMIN: return std::min(acc,value);
        case Processor::CPU_Core::IsaOpcodeValues::VREDAND: return acc & value;
        case Processor::CPU_Core::IsaOpcodeValues::VREDOR: return acc | value;
        case Processor::CPU_Core::IsaOpcodeValues::VREDXOR: return acc ^ value;
        default: return acc;
        }
	}

	unsigned long long VectorALU::ExecuteReductionUnsignedInt(unsigned int op, unsigned long long acc, unsigned long long value)
	{
		switch (op) {
		case Processor::CPU_Core::IsaOpcodeValues::VREDSUM: return acc + value;
		case Processor::CPU_Core::IsaOpcodeValues::VREDMAXU: return std::max(acc, value);
		case Processor::CPU_Core::IsaOpcodeValues::VREDMINU: return std::min(acc, value);
		case Processor::CPU_Core::IsaOpcodeValues::VREDOR: return acc | value;
		default: return acc;
		}
	}

	void VectorALU::ApplySaturatingAddElement(unsigned int op, DataTypeEnum dataType, std::span<const unsigned char> sourceA, std::span<const unsigned char> sourceB, std::span<unsigned char> destination, std::shared_ptr<Processor::CPU_Core> &core)
	{
		if (op != static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VADD))
		{
			throw InvalidOperationException(std::format("VectorALU.ApplySaturatingAddElement rejected opcode 0x{0:0>X}; Phase 05B opens only VADD.SAT.", op));
		}

		MarkVectorDirty(core);

		if (DataTypeUtils::IsSignedInteger(dataType))
		{
			long long x = ElementCodec::LoadI(sourceA, 0, dataType);
			long long y = ElementCodec::LoadI(sourceB, 0, dataType);
			long long result = SaturatingAddSignedForElementWidth(x, y, dataType, core);
			ElementCodec::StoreI(destination, 0, dataType, result);
			return;
		}

		if (DataTypeUtils::IsUnsignedInteger(dataType))
		{
			unsigned long long x = ElementCodec::LoadU(sourceA, 0, dataType);
			unsigned long long y = ElementCodec::LoadU(sourceB, 0, dataType);
			unsigned long long result = SaturatingAddUnsignedForElementWidth(x, y, dataType, core);
			ElementCodec::StoreU(destination, 0, dataType, result);
			return;
		}

		throw DecodeProjectionFaultException(std::format(L"VectorALU.ApplySaturatingAddElement rejected unsupported VADD.SAT DataType {}. ", static_cast<unsigned int>(dataType)) + L"Phase 05B opens saturating add only for integer signed/unsigned element types.");
	}

	long long VectorALU::SaturatingAddSignedForElementWidth(long long x, long long y, DataTypeEnum dataType, std::shared_ptr<Processor::CPU_Core> &core)
	{
        const long long min = SignedMin(dataType);
        const long long max = SignedMax(dataType);
        if ((y > 0 && x > max - y) || (y < 0 && x < min - y))
        {
            TrackOverflow(core);
            return y > 0 ? max : min;
        }
        return x + y;
	}

	unsigned long long VectorALU::SaturatingAddUnsignedForElementWidth(unsigned long long x, unsigned long long y, DataTypeEnum dataType, std::shared_ptr<Processor::CPU_Core> &core)
	{
        const unsigned long long max = UnsignedMax(dataType);
        if (x > max || y > max - x)
        {
            TrackOverflow(core);
            return max;
        }
        return x + y;
	}

	long long VectorALU::SignedMin(DataTypeEnum dataType)
	{
		switch (dataType) {
		case DataTypeEnum::INT8: return std::numeric_limits<signed char>::min();
		case DataTypeEnum::INT16: return std::numeric_limits<short>::min();
		case DataTypeEnum::INT32: return std::numeric_limits<int>::min();
		case DataTypeEnum::INT64: return std::numeric_limits<long long>::min();
		default: throw DecodeProjectionFaultException(L"VADD.SAT rejected unsupported datatype for clamp.");
		}
	}

	long long VectorALU::SignedMax(DataTypeEnum dataType)
	{
		switch (dataType) {
		case DataTypeEnum::INT8: return std::numeric_limits<signed char>::max();
		case DataTypeEnum::INT16: return std::numeric_limits<short>::max();
		case DataTypeEnum::INT32: return std::numeric_limits<int>::max();
		case DataTypeEnum::INT64: return std::numeric_limits<long long>::max();
		default: throw DecodeProjectionFaultException(L"VADD.SAT rejected unsupported datatype for clamp.");
		}
	}

	unsigned long long VectorALU::UnsignedMax(DataTypeEnum dataType)
	{
		switch (dataType) {
		case DataTypeEnum::UINT8: return std::numeric_limits<unsigned char>::max();
		case DataTypeEnum::UINT16: return std::numeric_limits<unsigned short>::max();
		case DataTypeEnum::UINT32: return std::numeric_limits<unsigned int>::max();
		case DataTypeEnum::UINT64: return std::numeric_limits<unsigned long long>::max();
		default: throw DecodeProjectionFaultException(L"VADD.SAT rejected unsupported datatype for clamp.");
		}
	}

	inline long long VectorALU::SaturatingAddSigned(long long x, long long y, std::shared_ptr<Processor::CPU_Core> &core)
	{
		long long result = x + y;
		// Check for overflow: if signs of x and y are same, and result sign differs, overflow occurred
		if (((x ^ result) & (y ^ result)) < 0)
		{
			// Track overflow exception
			TrackOverflow(core);
			// Overflow: return max if positive, min if negative
			return x >= 0 ? std::numeric_limits<long long>::max() : std::numeric_limits<long long>::min();
		}
		return result;
	}

	inline long long VectorALU::SaturatingSubSigned(long long x, long long y, std::shared_ptr<Processor::CPU_Core> &core)
	{
		long long result = x - y;
		// Check for overflow: if signs of x and y differ, and result sign differs from x
		if (((x ^ y) & (x ^ result)) < 0)
		{
			TrackOverflow(core);
			return x >= 0 ? std::numeric_limits<long long>::max() : std::numeric_limits<long long>::min();
		}
		return result;
	}

	inline long long VectorALU::SaturatingMulSigned(long long x, long long y, std::shared_ptr<Processor::CPU_Core> &core)
	{
        const long long max = std::numeric_limits<long long>::max();
        const long long min = std::numeric_limits<long long>::min();
        bool overflow = false;
        if (x > 0) overflow = y > 0 ? x > max / y : (y < 0 && y < min / x);
        else if (x < 0) overflow = y > 0 ? x < min / y : (y < 0 && x < max / y);
        if (overflow)
        {
            TrackOverflow(core);
            return ((x < 0) != (y < 0)) ? min : max;
        }
        return x * y;
	}

	unsigned long long VectorALU::SaturatingAddUnsigned(unsigned long long x, unsigned long long y, std::shared_ptr<Processor::CPU_Core> &core)
	{
		unsigned long long result = x + y;
		// Overflow if result < x (wraparound occurred)
		if (result < x)
		{
			TrackOverflow(core);
			return std::numeric_limits<unsigned long long>::max();
		}
		return result;
	}

	unsigned long long VectorALU::SaturatingSubUnsigned(unsigned long long x, unsigned long long y, std::shared_ptr<Processor::CPU_Core> &core)
	{
		// Underflow if y > x
		if (y > x)
		{
			TrackOverflow(core);
			return 0;
		}
		return x - y;
	}

	unsigned long long VectorALU::SaturatingMulUnsigned(unsigned long long x, unsigned long long y, std::shared_ptr<Processor::CPU_Core> &core)
	{
        if (y != 0 && x > std::numeric_limits<unsigned long long>::max() / y)
        {
            TrackOverflow(core);
            return std::numeric_limits<unsigned long long>::max();
        }
        return x * y;
	}

	inline int VectorALU::CountLeadingZeros(unsigned long long value)
	{
		if (value == 0)
		{
			return 64;
		}
		return std::countl_zero(value);
	}

	inline int VectorALU::CountTrailingZeros(unsigned long long value)
	{
		if (value == 0)
		{
			return 64;
		}
		return std::countr_zero(value);
	}

	inline int VectorALU::PopCount(unsigned long long value)
	{
		return std::popcount(value);
	}

	unsigned long long VectorALU::ReverseBits(unsigned long long value)
	{
		// Swap bit pairs, then nibbles, then bytes, etc.
		value = ((value & 0x5555555555555555ULL) << 1) | ((value & 0xAAAAAAAAAAAAAAAAULL) >> 1);
		value = ((value & 0x3333333333333333ULL) << 2) | ((value & 0xCCCCCCCCCCCCCCCCULL) >> 2);
		value = ((value & 0x0F0F0F0F0F0F0F0FULL) << 4) | ((value & 0xF0F0F0F0F0F0F0F0ULL) >> 4);
		value = ((value & 0x00FF00FF00FF00FFULL) << 8) | ((value & 0xFF00FF00FF00FF00ULL) >> 8);
		value = ((value & 0x0000FFFF0000FFFFULL) << 16) | ((value & 0xFFFF0000FFFF0000ULL) >> 16);
		value = (value << 32) | (value >> 32);
		return value;
	}

	unsigned long long VectorALU::ReverseBytes(unsigned long long value)
	{
		return ((value & 0x00000000000000FFULL) << 56) | ((value & 0x000000000000FF00ULL) << 40) | ((value & 0x0000000000FF0000ULL) << 24) | ((value & 0x00000000FF000000ULL) << 8) | ((value & 0x000000FF00000000ULL) >> 8) | ((value & 0x0000FF0000000000ULL) >> 24) | ((value & 0x00FF000000000000ULL) >> 40) | ((value & 0xFF00000000000000ULL) >> 56);
	}

	void VectorALU::ApplyScanSumElement(unsigned int op, DataTypeEnum dataType, std::span<const unsigned char> sourceElement, std::span<unsigned char> destinationElement, long long &signedAccumulator, unsigned long long &unsignedAccumulator, std::shared_ptr<Processor::CPU_Core> &core)
	{
		if (op != static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VSCAN_SUM))
		{
			throw InvalidOperationException(std::format("VectorALU.ApplyScanSumElement rejected opcode 0x{0:0>X}; Phase 05A opens only VSCAN.SUM.", op));
		}

		MarkVectorDirty(core);

		if (DataTypeUtils::IsSignedInteger(dataType))
		{
			long long value = ElementCodec::LoadI(sourceElement, 0, dataType);
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: signedAccumulator = unchecked(signedAccumulator + value);
			signedAccumulator = signedAccumulator + value;
			ElementCodec::StoreI(destinationElement, 0, dataType, signedAccumulator);
			return;
		}

		if (DataTypeUtils::IsUnsignedInteger(dataType))
		{
			unsigned long long value = ElementCodec::LoadU(sourceElement, 0, dataType);
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: unsignedAccumulator = unchecked(unsignedAccumulator + value);
			unsignedAccumulator = unsignedAccumulator + value;
			ElementCodec::StoreU(destinationElement, 0, dataType, unsignedAccumulator);
			return;
		}

		throw ExecutionFaultContract::CreateUnsupportedVectorElementTypeException(std::format(L"VectorALU.ApplyScanSumElement rejected unsupported VSCAN.SUM DataType {}. ", static_cast<unsigned int>(dataType)) + L"Phase 05A publishes only integer prefix-sum semantics.");
	}
}
