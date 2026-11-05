#pragma once

#include "MatrixTileLayoutPolicyAbi.h"
#include <string>
#include <vector>
#include <cmath>
#include <memory>
#include <cstdint>
#include <type_traits>
#include <limits>
#include <algorithm>
#include <bit>
#include <utility>

using namespace HybridCPU_ISE::Arch;
using namespace YAKSys_Hybrid_CPU::Arch;

namespace YAKSys_Hybrid_CPU::CloseToHSL::Core::ISA::Instructions::NonVmx::Lanes00_03Vector::MatrixTile
{

	enum class MatrixTileMaccArithmeticFaultKind : unsigned char
	{
		None = 0,
		InvalidNumericPolicy = 1,
		InvalidLayoutPolicy = 2,
		InvalidSnapshot = 3,
		ArithmeticOverflow = 4
	};

	class MatrixTileMaccArithmeticAbi final
	{
	private:
		// Exact, signed, unbounded integer used by the C# BigInteger contract.
		// A small base-2^32 backend keeps integer dot products exact without
		// imposing a fixed-width overflow limit on matrix dimensions.
		class BigInteger final
		{
			std::vector<std::uint32_t> _limbs;
			bool _negative = false;

			void Normalize()
			{
				while (!_limbs.empty() && _limbs.back() == 0) _limbs.pop_back();
				if (_limbs.empty()) _negative = false;
			}

			static int CompareMagnitude(const BigInteger &left, const BigInteger &right)
			{
				if (left._limbs.size() != right._limbs.size()) return left._limbs.size() < right._limbs.size() ? -1 : 1;
				for (std::size_t i = left._limbs.size(); i-- > 0;)
				{
					if (left._limbs[i] != right._limbs[i]) return left._limbs[i] < right._limbs[i] ? -1 : 1;
				}
				return 0;
			}

			static BigInteger AddMagnitude(const BigInteger &left, const BigInteger &right)
			{
				BigInteger result;
				const std::size_t count = std::max(left._limbs.size(), right._limbs.size());
				result._limbs.resize(count);
				std::uint64_t carry = 0;
				for (std::size_t i = 0; i < count; ++i)
				{
					const std::uint64_t a = i < left._limbs.size() ? left._limbs[i] : 0;
					const std::uint64_t b = i < right._limbs.size() ? right._limbs[i] : 0;
					const std::uint64_t sum = a + b + carry;
					result._limbs[i] = static_cast<std::uint32_t>(sum);
					carry = sum >> 32;
				}
				if (carry) result._limbs.push_back(static_cast<std::uint32_t>(carry));
				return result;
			}

			// Precondition: |left| >= |right|.
			static BigInteger SubtractMagnitude(const BigInteger &left, const BigInteger &right)
			{
				BigInteger result;
				result._limbs.resize(left._limbs.size());
				std::uint64_t borrow = 0;
				for (std::size_t i = 0; i < left._limbs.size(); ++i)
				{
					const std::uint64_t a = left._limbs[i];
					const std::uint64_t b = (i < right._limbs.size() ? right._limbs[i] : 0) + borrow;
					result._limbs[i] = static_cast<std::uint32_t>(a - b);
					borrow = a < b ? 1 : 0;
				}
				result.Normalize();
				return result;
			}

			std::uint64_t LowMagnitude64() const
			{
				std::uint64_t value = _limbs.empty() ? 0 : _limbs[0];
				if (_limbs.size() > 1) value |= static_cast<std::uint64_t>(_limbs[1]) << 32;
				return value;
			}

		public:
			BigInteger() = default;
			template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
			BigInteger(T value)
			{
				using Unsigned = std::make_unsigned_t<T>;
				_negative = std::is_signed_v<T> && value < 0;
				const Unsigned bits = static_cast<Unsigned>(value);
				const Unsigned magnitude = _negative ? Unsigned(0) - bits : bits;
				std::uint64_t remaining = static_cast<std::uint64_t>(magnitude);
				while (remaining)
				{
					_limbs.push_back(static_cast<std::uint32_t>(remaining));
					remaining >>= 32;
				}
				Normalize();
			}

			bool IsZero() const { return _limbs.empty(); }
			bool IsEven() const { return _limbs.empty() || (_limbs[0] & 1U) == 0; }
			int Sign() const { return IsZero() ? 0 : (_negative ? -1 : 1); }
			std::size_t GetBitLength() const
			{
				if (_limbs.empty()) return 0;
				return (_limbs.size() - 1) * 32 + (32 - static_cast<std::size_t>(std::countl_zero(_limbs.back())));
			}
			static BigInteger Abs(BigInteger value) { value._negative = false; return value; }

			friend BigInteger operator-(BigInteger value) { if (!value.IsZero()) value._negative = !value._negative; return value; }
			friend BigInteger operator+(const BigInteger &left, const BigInteger &right)
			{
				if (left._negative == right._negative)
				{
					BigInteger result = AddMagnitude(left, right);
					result._negative = left._negative;
					return result;
				}
				const int order = CompareMagnitude(left, right);
				if (order == 0) return BigInteger();
				BigInteger result = order > 0 ? SubtractMagnitude(left, right) : SubtractMagnitude(right, left);
				result._negative = order > 0 ? left._negative : right._negative;
				return result;
			}
			friend BigInteger operator-(const BigInteger &left, const BigInteger &right) { return left + (-BigInteger(right)); }
			BigInteger &operator+=(const BigInteger &other) { *this = *this + other; return *this; }
			friend BigInteger operator*(const BigInteger &left, const BigInteger &right)
			{
				BigInteger result;
				if (left.IsZero() || right.IsZero()) return result;
				result._limbs.assign(left._limbs.size() + right._limbs.size(), 0);
				for (std::size_t i = 0; i < left._limbs.size(); ++i)
				{
					std::uint64_t carry = 0;
					for (std::size_t j = 0; j < right._limbs.size(); ++j)
					{
						const std::uint64_t current = static_cast<std::uint64_t>(left._limbs[i]) * right._limbs[j] + result._limbs[i + j] + carry;
						result._limbs[i + j] = static_cast<std::uint32_t>(current);
						carry = current >> 32;
					}
					std::size_t at = i + right._limbs.size();
					while (carry)
					{
						if (at == result._limbs.size()) result._limbs.push_back(0);
						const std::uint64_t current = static_cast<std::uint64_t>(result._limbs[at]) + carry;
						result._limbs[at++] = static_cast<std::uint32_t>(current);
						carry = current >> 32;
					}
				}
				result._negative = left._negative != right._negative;
				result.Normalize();
				return result;
			}
			friend BigInteger operator<<(const BigInteger &value, int shift)
			{
				if (shift < 0) return value >> -shift;
				if (value.IsZero() || shift == 0) return value;
				BigInteger result;
				const std::size_t words = static_cast<std::size_t>(shift / 32);
				const unsigned bits = static_cast<unsigned>(shift % 32);
				result._limbs.assign(words, 0);
				std::uint64_t carry = 0;
				for (std::uint32_t limb : value._limbs)
				{
					const std::uint64_t expanded = (static_cast<std::uint64_t>(limb) << bits) | carry;
					result._limbs.push_back(static_cast<std::uint32_t>(expanded));
					carry = expanded >> 32;
				}
				if (carry) result._limbs.push_back(static_cast<std::uint32_t>(carry));
				result._negative = value._negative;
				return result;
			}
			friend BigInteger operator>>(const BigInteger &value, int shift)
			{
				if (shift < 0) return value << -shift;
				if (shift == 0 || value.IsZero()) return value;
				const std::size_t words = static_cast<std::size_t>(shift / 32);
				const unsigned bits = static_cast<unsigned>(shift % 32);
				if (words >= value._limbs.size()) return BigInteger();
				BigInteger result;
				result._limbs.resize(value._limbs.size() - words);
				for (std::size_t i = words; i < value._limbs.size(); ++i)
				{
					std::uint64_t current = value._limbs[i];
					if (bits && i + 1 < value._limbs.size()) current |= static_cast<std::uint64_t>(value._limbs[i + 1]) << 32;
					result._limbs[i - words] = static_cast<std::uint32_t>(current >> bits);
				}
				result._negative = value._negative;
				result.Normalize();
				return result;
			}
			BigInteger &operator>>=(int shift) { *this = *this >> shift; return *this; }

			friend bool operator==(const BigInteger &a, const BigInteger &b) { return a._negative == b._negative && a._limbs == b._limbs; }
			friend bool operator!=(const BigInteger &a, const BigInteger &b) { return !(a == b); }
			friend bool operator<(const BigInteger &a, const BigInteger &b)
			{
				if (a._negative != b._negative) return a._negative;
				const int order = CompareMagnitude(a, b);
				return a._negative ? order > 0 : order < 0;
			}
			friend bool operator>(const BigInteger &a, const BigInteger &b) { return b < a; }
			friend bool operator<=(const BigInteger &a, const BigInteger &b) { return !(b < a); }
			friend bool operator>=(const BigInteger &a, const BigInteger &b) { return !(a < b); }

			template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
			explicit operator T() const
			{
				const std::uint64_t magnitude = LowMagnitude64();
				if constexpr (std::is_signed_v<T>)
				{
					if (_negative)
					{
						const std::uint64_t minimumMagnitude = static_cast<std::uint64_t>(std::numeric_limits<T>::max()) + 1ULL;
						if (magnitude >= minimumMagnitude) return std::numeric_limits<T>::min();
						return static_cast<T>(-static_cast<std::int64_t>(magnitude));
					}
				}
				return static_cast<T>(_negative ? 0ULL - magnitude : magnitude);
			}
		};

		enum class BinaryValueKind : unsigned char { Finite = 0, Infinity = 1, NaN = 2 };
		struct BinaryFormat final
		{
			int TotalBits;
			int ExponentBits;
			int FractionBits;
			int Bias;
			unsigned long long CanonicalNaN;
			BinaryFormat(int totalBits, int exponentBits, int fractionBits, int bias, unsigned long long canonicalNaN)
				: TotalBits(totalBits), ExponentBits(exponentBits), FractionBits(fractionBits), Bias(bias), CanonicalNaN(canonicalNaN) {}
			unsigned long long SignMask() const { return 1ULL << (TotalBits - 1); }
			unsigned long long ExponentMask() const { return (1ULL << ExponentBits) - 1ULL; }
			unsigned long long FractionMask() const { return (1ULL << FractionBits) - 1ULL; }
		};
		struct BinaryValue final
		{
			BinaryValueKind Kind;
			bool Negative;
			BigInteger Significand;
			int Exponent;
			bool IsZero;
			BinaryValue(BinaryValueKind kind, bool negative, BigInteger significand, int exponent)
				: Kind(kind), Negative(negative), Significand(std::move(significand)), Exponent(exponent), IsZero(kind == BinaryValueKind::Finite && Significand.IsZero()) {}
			static std::shared_ptr<BinaryValue> Finite(bool negative, BigInteger significand, int exponent) { return std::make_shared<BinaryValue>(BinaryValueKind::Finite, negative, std::move(significand), exponent); }
			static std::shared_ptr<BinaryValue> Infinity(bool negative) { return std::make_shared<BinaryValue>(BinaryValueKind::Infinity, negative, BigInteger(0), 0); }
			static std::shared_ptr<BinaryValue> NaN() { return std::make_shared<BinaryValue>(BinaryValueKind::NaN, false, BigInteger(0), 0); }
		};

	public:
		static inline const std::wstring FormalFunctionDecision = L"AccumulatorSnapshotThenAscendingKMultiplyAddThenFinalize";
		static inline const std::wstring IntegerDecision = L"ExactIntegerProductAndUnboundedSumWithFinalEncodingTrap";
		static inline const std::wstring FloatingDecision = L"SoftwareIeee754SeparateMultiplyAndAddRoundToNearestTiesToEven";
		static inline const std::wstring ByteOrderDecision = L"CanonicalLittleEndianElementCodec";
		static constexpr bool UsesHostFloatingPointArithmetic = false;
		static constexpr bool UsesFusedMultiplyAdd = false;
		static constexpr bool UsesHostMatrixLibrary = false;

		static bool TryCompute(std::shared_ptr<MatrixTileMaccSemanticContract> contract, std::shared_ptr<MatrixTileTileImage> left, std::shared_ptr<MatrixTileTileImage> right, std::shared_ptr<MatrixTileTileImage> accumulator, std::shared_ptr<MatrixTileTileImage> &resultImage, MatrixTileMaccArithmeticFaultKind &faultKind);

	private:
		static bool TryComputeInteger(std::shared_ptr<MatrixTileMaccSemanticContract> contract, std::vector<unsigned char> &left, std::vector<unsigned char> &right, std::vector<unsigned char> &result);

		static bool TryComputeFloating(std::shared_ptr<MatrixTileMaccSemanticContract> contract, std::vector<unsigned char> &left, std::vector<unsigned char> &right, std::vector<unsigned char> &result);

		static BigInteger ReadInteger(std::vector<unsigned char> &data, MatrixTileCanonicalDescriptorAbi descriptor, unsigned short row, unsigned short column, MatrixTileElementAddressingKind addressing, bool signed_Keyword);

		static bool TryWriteInteger(std::vector<unsigned char> &data, MatrixTileCanonicalDescriptorAbi descriptor, unsigned short row, unsigned short column, MatrixTileElementAddressingKind addressing, BigInteger value, bool signed_Keyword);

		static bool FitsInteger(BigInteger value, unsigned short elementSizeBytes, bool signed_Keyword);

		static unsigned long long ReadBits(std::vector<unsigned char> &data, MatrixTileCanonicalDescriptorAbi descriptor, unsigned short row, unsigned short column, MatrixTileElementAddressingKind addressing, std::shared_ptr<BinaryFormat> format);

		static void WriteBits(std::vector<unsigned char> &data, MatrixTileCanonicalDescriptorAbi descriptor, unsigned short row, unsigned short column, MatrixTileElementAddressingKind addressing, std::shared_ptr<BinaryFormat> format, unsigned long long bits);

		static unsigned long long Multiply(unsigned long long leftBits, unsigned long long rightBits, std::shared_ptr<BinaryFormat> format);

		static unsigned long long Add(unsigned long long leftBits, unsigned long long rightBits, std::shared_ptr<BinaryFormat> format);

		static std::shared_ptr<BinaryValue> Decode(unsigned long long bits, std::shared_ptr<BinaryFormat> format);

		static unsigned long long Round(std::shared_ptr<BinaryValue> value, std::shared_ptr<BinaryFormat> format, int exponentAdjustment = 0);

		static BigInteger RoundShiftRightToEven(BigInteger value, int shift);

		static unsigned long long EncodeInfinity(bool negative, std::shared_ptr<BinaryFormat> format);

		static unsigned long long EncodeZero(bool negative, std::shared_ptr<BinaryFormat> format);

		static unsigned long long Sign(bool negative, std::shared_ptr<BinaryFormat> format);

	};

}
