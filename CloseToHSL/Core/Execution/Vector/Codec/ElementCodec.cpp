#include "ElementCodec.h"
#include <bit>
#include <cstdint>
#include <stdexcept>
#include "../../../../../Arch/DataType.h"

using namespace HybridCPU_ISE::Arch;

namespace YAKSys_Hybrid_CPU::Execution
{

	int ElementCodec::SizeOf(DataTypeEnum dt)
	{
		return Arch::DataTypeUtils::SizeOf(dt);
	}

    namespace {
        unsigned long long ReadLE(std::span<const unsigned char> mem, int off, int size) {
            if (off < 0 || static_cast<size_t>(off) > mem.size() || static_cast<size_t>(size) > mem.size() - static_cast<size_t>(off)) throw std::out_of_range("element read");
            unsigned long long bits = 0;
            for (int i = 0; i < size; ++i) bits |= static_cast<unsigned long long>(mem[off + i]) << (i * 8);
            return bits;
        }
        void WriteLE(std::span<unsigned char> mem, int off, int size, unsigned long long bits) {
            if (off < 0 || static_cast<size_t>(off) > mem.size() || static_cast<size_t>(size) > mem.size() - static_cast<size_t>(off)) throw std::out_of_range("element write");
            for (int i = 0; i < size; ++i) mem[off + i] = static_cast<unsigned char>(bits >> (i * 8));
        }
    }
    long long ElementCodec::LoadI(std::span<const unsigned char> mem, int off, DataTypeEnum dt) {
        switch (dt) {
        case DataTypeEnum::INT8: return std::bit_cast<std::int8_t>(static_cast<std::uint8_t>(ReadLE(mem, off, 1)));
        case DataTypeEnum::INT16: return std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(ReadLE(mem, off, 2)));
        case DataTypeEnum::INT32: return std::bit_cast<std::int32_t>(static_cast<std::uint32_t>(ReadLE(mem, off, 4)));
        case DataTypeEnum::INT64: return std::bit_cast<std::int64_t>(static_cast<std::uint64_t>(ReadLE(mem, off, 8)));
        default: throw std::invalid_argument("LoadI unsupported datatype");
        }
    }
    unsigned long long ElementCodec::LoadU(std::span<const unsigned char> mem, int off, DataTypeEnum dt) {
        switch (dt) {
        case DataTypeEnum::UINT8: return ReadLE(mem, off, 1);
        case DataTypeEnum::UINT16: return ReadLE(mem, off, 2);
        case DataTypeEnum::UINT32: return ReadLE(mem, off, 4);
        case DataTypeEnum::UINT64: return ReadLE(mem, off, 8);
        default: throw std::invalid_argument("LoadU unsupported datatype");
        }
    }
    double ElementCodec::LoadF(std::span<const unsigned char> mem, int off, DataTypeEnum dt) {
        switch (dt) {
        case DataTypeEnum::FLOAT8_E4M3: return DecodeE4M3(static_cast<unsigned char>(ReadLE(mem, off, 1)));
        case DataTypeEnum::FLOAT8_E5M2: return DecodeE5M2(static_cast<unsigned char>(ReadLE(mem, off, 1)));
        case DataTypeEnum::FLOAT16: return ConvertFP16ToFloat(static_cast<unsigned short>(ReadLE(mem, off, 2)));
        case DataTypeEnum::BFLOAT16: return ConvertBF16ToFloat(static_cast<unsigned short>(ReadLE(mem, off, 2)));
        case DataTypeEnum::FLOAT32: return std::bit_cast<float>(static_cast<std::uint32_t>(ReadLE(mem, off, 4)));
        case DataTypeEnum::FLOAT64: return std::bit_cast<double>(static_cast<std::uint64_t>(ReadLE(mem, off, 8)));
        default: throw std::invalid_argument("LoadF unsupported datatype");
        }
    }
    void ElementCodec::StoreI(std::span<unsigned char> mem, int off, DataTypeEnum dt, long long value) {
        switch (dt) {
        case DataTypeEnum::INT8: WriteLE(mem, off, 1, value); break;
        case DataTypeEnum::INT16: WriteLE(mem, off, 2, value); break;
        case DataTypeEnum::INT32: WriteLE(mem, off, 4, value); break;
        case DataTypeEnum::INT64: WriteLE(mem, off, 8, value); break;
        default: throw std::invalid_argument("StoreI unsupported datatype");
        }
    }
    void ElementCodec::StoreU(std::span<unsigned char> mem, int off, DataTypeEnum dt, unsigned long long value) {
        switch (dt) {
        case DataTypeEnum::UINT8: WriteLE(mem, off, 1, value); break;
        case DataTypeEnum::UINT16: WriteLE(mem, off, 2, value); break;
        case DataTypeEnum::UINT32: WriteLE(mem, off, 4, value); break;
        case DataTypeEnum::UINT64: WriteLE(mem, off, 8, value); break;
        default: throw std::invalid_argument("StoreU unsupported datatype");
        }
    }
    void ElementCodec::StoreF(std::span<unsigned char> mem, int off, DataTypeEnum dt, double value) {
        switch (dt) {
        case DataTypeEnum::FLOAT8_E4M3: WriteLE(mem, off, 1, EncodeE4M3(value)); break;
        case DataTypeEnum::FLOAT8_E5M2: WriteLE(mem, off, 1, EncodeE5M2(value)); break;
        case DataTypeEnum::FLOAT16: WriteLE(mem, off, 2, ConvertFloatToFP16(value)); break;
        case DataTypeEnum::BFLOAT16: WriteLE(mem, off, 2, ConvertFloatToBF16(value)); break;
        case DataTypeEnum::FLOAT32: WriteLE(mem, off, 4, std::bit_cast<std::uint32_t>(static_cast<float>(value))); break;
        case DataTypeEnum::FLOAT64: WriteLE(mem, off, 8, std::bit_cast<std::uint64_t>(value)); break;
        default: throw std::invalid_argument("StoreF unsupported datatype");
        }
    }
    unsigned long long ElementCodec::LoadRaw(std::span<const unsigned char> mem, int off, DataTypeEnum dt) {
        int size = SizeOf(dt); if (size != 1 && size != 2 && size != 4 && size != 8) throw std::invalid_argument("unexpected element size");
        return ReadLE(mem, off, size);
    }
    void ElementCodec::StoreRaw(std::span<unsigned char> mem, int off, DataTypeEnum dt, unsigned long long value) {
        int size = SizeOf(dt); if (size != 1 && size != 2 && size != 4 && size != 8) throw std::invalid_argument("unexpected element size");
        WriteLE(mem, off, size, value);
    }
	float ElementCodec::ConvertFP16ToFloat(unsigned short fp16)
	{
		unsigned int sign = static_cast<unsigned int>(fp16 >> 15) << 31;
		unsigned int exponent = static_cast<unsigned int>((fp16 >> 10) & 0x1F);
		unsigned int mantissa = static_cast<unsigned int>(fp16 & 0x3FF);

		if (exponent == 0)
		{
			if (mantissa == 0)
			{
				// Zero (positive or negative)
				return std::bit_cast<float>(sign);
			}
			else
			{
				// Subnormal number
				// Convert to normalized FP32
				while ((mantissa & 0x400) == 0)
				{
					mantissa <<= 1;
					exponent--;
				}
				exponent++;
				mantissa &= 0x3FF;
				unsigned int fp32 = sign | ((exponent + (127 - 15)) << 23) | (mantissa << 13);
				return std::bit_cast<float>(fp32);
			}
		}
		else if (exponent == 0x1F)
		{
			// Infinity or NaN
			unsigned int fp32 = sign | (0xFFU << 23) | (mantissa << 13);
			return std::bit_cast<float>(fp32);
		}
		else
		{
			// Normalized number
			unsigned int fp32 = sign | ((exponent + (127 - 15)) << 23) | (mantissa << 13);
			return std::bit_cast<float>(fp32);
		}
	}

	unsigned short ElementCodec::ConvertFloatToFP16(double value)
	{
		float f = static_cast<float>(value);
		unsigned int bits = std::bit_cast<unsigned int>(f);
		unsigned int sign = (bits >> 31) & 1;
		int exponent = static_cast<int>((bits >> 23) & 0xFF) - 127;
		unsigned int mantissa = bits & 0x7FFFFF;

		if (exponent == 128)
		{
			// Infinity or NaN
			return static_cast<unsigned short>((sign << 15) | 0x7C00 | (mantissa != 0 ? 1 : 0));
		}

		if (exponent > 15)
		{
			// Overflow to infinity
			return static_cast<unsigned short>((sign << 15) | 0x7C00);
		}

		if (exponent < -14)
		{
			// Underflow or subnormal
			if (exponent < -24)
			{
				// Too small, flush to zero
				return static_cast<unsigned short>(sign << 15);
			}
			// Subnormal FP16
			mantissa |= 0x800000;
			int shift = -exponent - 14;
			mantissa >>= shift;
			return static_cast<unsigned short>((sign << 15) | (mantissa >> 13));
		}

		// Normalized FP16
		unsigned int fp16Exp = static_cast<unsigned int>(exponent + 15);
		unsigned int fp16Mantissa = mantissa >> 13;
		return static_cast<unsigned short>((sign << 15) | (fp16Exp << 10) | fp16Mantissa);
	}

	float ElementCodec::ConvertBF16ToFloat(unsigned short bf16)
	{
		// BF16 is just the upper 16 bits of FP32, so shift left by 16
		unsigned int fp32 = static_cast<unsigned int>(bf16) << 16;
		return std::bit_cast<float>(fp32);
	}

	unsigned short ElementCodec::ConvertFloatToBF16(double value)
	{
		float f = static_cast<float>(value);
		unsigned int bits = std::bit_cast<unsigned int>(f);
		// BF16 is upper 16 bits of FP32
		// For proper rounding: add 0x7FFF before truncating (round to nearest)
		unsigned int rounded = bits + 0x7FFF + ((bits >> 16) & 1); // Round to nearest even
		return static_cast<unsigned short>(rounded >> 16);
	}

	unsigned char ElementCodec::EncodeE4M3(double value)
	{
		float f = static_cast<float>(value);
		unsigned int bits = std::bit_cast<unsigned int>(f);
		unsigned int sign = (bits >> 31) & 1;
		int f32_exp = static_cast<int>((bits >> 23) & 0xFF);
		unsigned int f32_mantissa = bits & 0x7FFFFF;

		// Zero or FP32 subnormal → E4M3 zero (preserving sign)
		if (f32_exp == 0)
		{
			return static_cast<unsigned char>(sign << 7);
		}

		// FP32 NaN → E4M3 NaN (exp=15, mantissa=7)
		if (f32_exp == 255 && f32_mantissa != 0)
		{
			return static_cast<unsigned char>((sign << 7) | 0x7F);
		}

		// FP32 Infinity → saturate to max finite E4M3 (exp=15, mantissa=6)
		// E4M3 has no infinity representation
		if (f32_exp == 255)
		{
			return static_cast<unsigned char>((sign << 7) | 0x7E);
		}

		// Bias adjustment: FP32 bias (127) - E4M3 bias (7) = 120
		int e4m3_exp = f32_exp - 120;

		// Underflow → flush to zero
		if (e4m3_exp <= 0)
		{
			return static_cast<unsigned char>(sign << 7);
		}

		// Overflow → saturate to max finite (exp=15, mantissa=6)
		if (e4m3_exp > 15)
		{
			return static_cast<unsigned char>((sign << 7) | 0x7E);
		}

		// Truncate FP32 mantissa to 3 bits
		unsigned int e4m3_mantissa = f32_mantissa >> 20;

		// Guard against accidental NaN encoding (exp=15, mantissa=7)
		if (e4m3_exp == 15 && e4m3_mantissa == 7)
		{
			e4m3_mantissa = 6;
		}

		return static_cast<unsigned char>((sign << 7) | (static_cast<unsigned int>(e4m3_exp) << 3) | e4m3_mantissa);
	}

	unsigned char ElementCodec::EncodeE5M2(double value)
	{
		float f = static_cast<float>(value);
		unsigned int bits = std::bit_cast<unsigned int>(f);
		unsigned int sign = (bits >> 31) & 1;
		int f32_exp = static_cast<int>((bits >> 23) & 0xFF);
		unsigned int f32_mantissa = bits & 0x7FFFFF;

		// Zero or FP32 subnormal → E5M2 zero (preserving sign)
		if (f32_exp == 0)
		{
			return static_cast<unsigned char>(sign << 7);
		}

		// FP32 NaN → E5M2 NaN (exp=31, mantissa=3)
		if (f32_exp == 255 && f32_mantissa != 0)
		{
			return static_cast<unsigned char>((sign << 7) | 0x7F);
		}

		// FP32 Infinity → E5M2 Infinity (exp=31, mantissa=0)
		if (f32_exp == 255)
		{
			return static_cast<unsigned char>((sign << 7) | 0x7C);
		}

		// Bias adjustment: FP32 bias (127) - E5M2 bias (15) = 112
		int e5m2_exp = f32_exp - 112;

		// Underflow → flush to zero
		if (e5m2_exp <= 0)
		{
			return static_cast<unsigned char>(sign << 7);
		}

		// Overflow → infinity
		if (e5m2_exp >= 31)
		{
			return static_cast<unsigned char>((sign << 7) | 0x7C);
		}

		// Truncate FP32 mantissa to 2 bits
		unsigned int e5m2_mantissa = f32_mantissa >> 21;

		return static_cast<unsigned char>((sign << 7) | (static_cast<unsigned int>(e5m2_exp) << 2) | e5m2_mantissa);
	}

	float ElementCodec::DecodeE4M3(unsigned char value)
	{
		unsigned int sign = static_cast<unsigned int>((value & 0x80) << 24); // Bit 7: sign
		unsigned int exp = static_cast<unsigned int>((value & 0x78) >> 3); // Bits 6-3: exponent (4 bits)
		unsigned int mantissa = static_cast<unsigned int>(value & 0x07); // Bits 2-0: mantissa (3 bits)

		// Flush denormals to zero (exp == 0)
		// Bias adjustment: FP32 bias (127) - E4M3 bias (7) = +120
		unsigned int f32_exp = (exp == 0) ? 0 : (exp + 120);

		// Handle special values (NaN/Inf): exp == 15 and mantissa == 7
		bool isSpecial = (exp == 15 && mantissa == 7);
		f32_exp = isSpecial ? 255 : f32_exp;
		unsigned int f32_mantissa = isSpecial ? 0x007FFFFFu : (exp == 0 ? 0 : (mantissa << 20));

		unsigned int bits = sign | (f32_exp << 23) | f32_mantissa;
		return std::bit_cast<float>(bits);
	}

	float ElementCodec::DecodeE5M2(unsigned char value)
	{
		unsigned int sign = static_cast<unsigned int>((value & 0x80) << 24); // Bit 7: sign
		unsigned int exp = static_cast<unsigned int>((value & 0x7C) >> 2); // Bits 6-2: exponent (5 bits)
		unsigned int mantissa = static_cast<unsigned int>(value & 0x03); // Bits 1-0: mantissa (2 bits)

		// Flush denormals to zero (exp == 0)
		// Bias adjustment: FP32 bias (127) - E5M2 bias (15) = +112
		unsigned int f32_exp = (exp == 0) ? 0 : (exp + 112);

		// Handle special values: exp == 31 (NaN/Inf)
		bool isSpecial = (exp == 31);
		f32_exp = isSpecial ? 255 : f32_exp;
		unsigned int f32_mantissa = isSpecial ? (mantissa != 0 ? 0x007FFFFFu : 0u) : (exp == 0 ? 0 : (mantissa << 21));

		unsigned int bits = sign | (f32_exp << 23) | f32_mantissa;
		return std::bit_cast<float>(bits);
	}
}
