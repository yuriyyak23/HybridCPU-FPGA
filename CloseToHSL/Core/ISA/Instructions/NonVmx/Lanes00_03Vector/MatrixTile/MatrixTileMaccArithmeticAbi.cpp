#include "MatrixTileMaccArithmeticAbi.h"
#include "MatrixTileExecuteCaptureAbi.h"
#include "MatrixTileNumericPolicyAbi.h"
#include "MatrixTileIrProjectionAndMaterializer.h"
#include "../../../../../../../Arch/DataType.h"
#include <optional>
#include <stdexcept>

using namespace HybridCPU_ISE::Arch;
using namespace YAKSys_Hybrid_CPU::Arch;

namespace YAKSys_Hybrid_CPU::CloseToHSL::Core::ISA::Instructions::NonVmx::Lanes00_03Vector::MatrixTile
{

	bool MatrixTileMaccArithmeticAbi::TryCompute(std::shared_ptr<MatrixTileMaccSemanticContract> contract, std::shared_ptr<MatrixTileTileImage> left, std::shared_ptr<MatrixTileTileImage> right, std::shared_ptr<MatrixTileTileImage> accumulator, std::shared_ptr<MatrixTileTileImage> &resultImage, MatrixTileMaccArithmeticFaultKind &faultKind)
	{
		resultImage = nullptr;
		faultKind = MatrixTileMaccArithmeticFaultKind::None;
		if (!contract || !contract->NumericPolicy)
		{
			faultKind = MatrixTileMaccArithmeticFaultKind::InvalidNumericPolicy;
			return false;
		}
		if (!contract->LayoutPolicy)
		{
			faultKind = MatrixTileMaccArithmeticFaultKind::InvalidLayoutPolicy;
			return false;
		}

		std::optional<std::shared_ptr<MatrixTileNumericPolicy>> numericPolicy = contract->NumericPolicy;
		if (!MatrixTileNumericPolicyAbi::Validate(numericPolicy)->isRuntimeOwnedNumericPolicyAccepted())
		{
			faultKind = MatrixTileMaccArithmeticFaultKind::InvalidNumericPolicy;
			return false;
		}

		std::vector<MatrixTileCanonicalDescriptorAbi> descriptors{contract->Left, contract->Right, contract->Accumulator};
		std::optional<std::shared_ptr<MatrixTileLayoutPolicy>> layoutPolicy = contract->LayoutPolicy;
		if (!MatrixTileLayoutPolicyAbi::Validate(layoutPolicy, MatrixTileProjectedOperationKind::Macc)->isRuntimeOwnedLayoutPolicyAccepted() || !MatrixTileLayoutPolicyAbi::ValidateDescriptors(contract->LayoutPolicy, descriptors)->isRuntimeOwnedLayoutPolicyAccepted())
		{
			faultKind = MatrixTileMaccArithmeticFaultKind::InvalidLayoutPolicy;
			return false;
		}

		if (!left || !right || !accumulator || !left->IsCanonicalPacked() || !right->IsCanonicalPacked() || !accumulator->IsCanonicalPacked() || left->Descriptor != contract->Left || right->Descriptor != contract->Right || accumulator->Descriptor != contract->Accumulator)
		{
			faultKind = MatrixTileMaccArithmeticFaultKind::InvalidSnapshot;
			return false;
		}

		std::vector<unsigned char> result = accumulator->Data;
		bool success = DataTypeUtils::IsFloatingPoint(contract->NumericPolicy->ElementType) ? TryComputeFloating(contract, left->Data, right->Data, result) : TryComputeInteger(contract, left->Data, right->Data, result);
		if (!success)
		{
			faultKind = MatrixTileMaccArithmeticFaultKind::ArithmeticOverflow;
			return false;
		}

		auto resultDescriptor = std::make_shared<MatrixTileCanonicalDescriptorAbi>(contract->Accumulator);
		resultImage = MatrixTileTileImage::Create(accumulator->TileId, resultDescriptor, std::span<const unsigned char>(result.data(), result.size()));
		return true;
	}

	bool MatrixTileMaccArithmeticAbi::TryComputeInteger(std::shared_ptr<MatrixTileMaccSemanticContract> contract, std::vector<unsigned char> &left, std::vector<unsigned char> &right, std::vector<unsigned char> &result)
	{
		bool signed_Keyword = contract->NumericPolicy->Signedness == MatrixTileNumericSignedness::Signed;
		std::shared_ptr<MatrixTileLayoutPolicy> layout = contract->LayoutPolicy;

		for (unsigned short row = 0; row < contract->Left.Rows; row++)
		{
			for (unsigned short column = 0; column < contract->Right.Columns; column++)
			{
				BigInteger sum = ReadInteger(result, contract->Accumulator, row, column, layout->DestinationAddressing, signed_Keyword);

				for (unsigned short k = 0; k < contract->Left.Columns; k++)
				{
					BigInteger lhs = ReadInteger(left, contract->Left, row, k, layout->SourceAddressing, signed_Keyword);
					BigInteger rhs = ReadInteger(right, contract->Right, k, column, layout->SecondaryAddressing, signed_Keyword);
					sum += lhs * rhs;
				}

				if (!TryWriteInteger(result, contract->Accumulator, row, column, layout->DestinationAddressing, sum, signed_Keyword))
				{
					return false;
				}
			}
		}

		return true;
	}

	bool MatrixTileMaccArithmeticAbi::TryComputeFloating(std::shared_ptr<MatrixTileMaccSemanticContract> contract, std::vector<unsigned char> &left, std::vector<unsigned char> &right, std::vector<unsigned char> &result)
	{
		std::shared_ptr<BinaryFormat> format;
		switch (contract->NumericPolicy->ElementType)
		{
		case DataTypeEnum::FLOAT32:
			format = std::make_shared<BinaryFormat>(32, 8, 23, 127, 0x7FC00000ULL);
			break;
		case DataTypeEnum::FLOAT64:
			format = std::make_shared<BinaryFormat>(64, 11, 52, 1023, 0x7FF8000000000000ULL);
			break;
		default:
			return false;
		}
		if (format->TotalBits == 0)
		{
			return false;
		}

		std::shared_ptr<MatrixTileLayoutPolicy> layout = contract->LayoutPolicy;
		for (unsigned short row = 0; row < contract->Left.Rows; row++)
		{
			for (unsigned short column = 0; column < contract->Right.Columns; column++)
			{
				unsigned long long accumulatorBits = ReadBits(result, contract->Accumulator, row, column, layout->DestinationAddressing, format);

				for (unsigned short k = 0; k < contract->Left.Columns; k++)
				{
					unsigned long long lhsBits = ReadBits(left, contract->Left, row, k, layout->SourceAddressing, format);
					unsigned long long rhsBits = ReadBits(right, contract->Right, k, column, layout->SecondaryAddressing, format);
					unsigned long long productBits = Multiply(lhsBits, rhsBits, format);
					accumulatorBits = Add(accumulatorBits, productBits, format);
				}

				WriteBits(result, contract->Accumulator, row, column, layout->DestinationAddressing, format, accumulatorBits);
			}
		}

		return true;
	}

	MatrixTileMaccArithmeticAbi::BigInteger MatrixTileMaccArithmeticAbi::ReadInteger(std::vector<unsigned char> &data, MatrixTileCanonicalDescriptorAbi descriptor, unsigned short row, unsigned short column, MatrixTileElementAddressingKind addressing, bool signed_Keyword)
	{
		int offset = MatrixTileLayoutPolicyAbi::GetPackedOffset(std::make_shared<MatrixTileCanonicalDescriptorAbi>(descriptor), row, column, addressing);
		const unsigned short size = descriptor.ElementSizeBytes;
		if ((size != 1 && size != 2 && size != 4 && size != 8) || offset < 0 || static_cast<std::size_t>(offset) + size > data.size())
		{
			throw std::invalid_argument("Unsupported or out-of-range MatrixTile integer element.");
		}
		unsigned long long raw = 0;
		for (unsigned short i = 0; i < size; ++i) raw |= static_cast<unsigned long long>(data[static_cast<std::size_t>(offset) + i]) << (8 * i);
		if (!signed_Keyword || (raw & (1ULL << (size * 8 - 1))) == 0) return BigInteger(raw);
		return BigInteger(raw) - (BigInteger(1) << (size * 8));
	}

	bool MatrixTileMaccArithmeticAbi::TryWriteInteger(std::vector<unsigned char> &data, MatrixTileCanonicalDescriptorAbi descriptor, unsigned short row, unsigned short column, MatrixTileElementAddressingKind addressing, BigInteger value, bool signed_Keyword)
	{
		if (!FitsInteger(value, descriptor.ElementSizeBytes, signed_Keyword))
		{
			return false;
		}

		int offset = MatrixTileLayoutPolicyAbi::GetPackedOffset(std::make_shared<MatrixTileCanonicalDescriptorAbi>(descriptor), row, column, addressing);
		if (offset < 0 || static_cast<std::size_t>(offset) + descriptor.ElementSizeBytes > data.size()) return false;
		const unsigned long long raw = static_cast<unsigned long long>(value);
		for (unsigned short i = 0; i < descriptor.ElementSizeBytes; ++i) data[static_cast<std::size_t>(offset) + i] = static_cast<unsigned char>(raw >> (8 * i));
		return true;
	}

	bool MatrixTileMaccArithmeticAbi::FitsInteger(BigInteger value, unsigned short elementSizeBytes, bool signed_Keyword)
	{
		int bitCount = elementSizeBytes * 8;
		if (signed_Keyword)
		{
			BigInteger min = -(BigInteger(1) << (bitCount - 1));
			BigInteger max = (BigInteger(1) << (bitCount - 1)) - 1;
			return value >= min && value <= max;
		}

		BigInteger unsignedMax = (BigInteger(1) << bitCount) - 1;
		return value >= BigInteger(0) && value <= unsignedMax;
	}

	unsigned long long MatrixTileMaccArithmeticAbi::ReadBits(std::vector<unsigned char> &data, MatrixTileCanonicalDescriptorAbi descriptor, unsigned short row, unsigned short column, MatrixTileElementAddressingKind addressing, std::shared_ptr<BinaryFormat> format)
	{
		int offset = MatrixTileLayoutPolicyAbi::GetPackedOffset(std::make_shared<MatrixTileCanonicalDescriptorAbi>(descriptor), row, column, addressing);
		const int size = format->TotalBits / 8;
		if (offset < 0 || static_cast<std::size_t>(offset) + size > data.size()) throw std::out_of_range("MatrixTile floating element offset is outside packed data.");
		unsigned long long bits = 0;
		for (int i = 0; i < size; ++i) bits |= static_cast<unsigned long long>(data[static_cast<std::size_t>(offset) + i]) << (8 * i);
		return bits;
	}

	void MatrixTileMaccArithmeticAbi::WriteBits(std::vector<unsigned char> &data, MatrixTileCanonicalDescriptorAbi descriptor, unsigned short row, unsigned short column, MatrixTileElementAddressingKind addressing, std::shared_ptr<BinaryFormat> format, unsigned long long bits)
	{
		int offset = MatrixTileLayoutPolicyAbi::GetPackedOffset(std::make_shared<MatrixTileCanonicalDescriptorAbi>(descriptor), row, column, addressing);
		const int size = format->TotalBits / 8;
		if (offset < 0 || static_cast<std::size_t>(offset) + size > data.size()) throw std::out_of_range("MatrixTile floating element offset is outside packed data.");
		for (int i = 0; i < size; ++i) data[static_cast<std::size_t>(offset) + i] = static_cast<unsigned char>(bits >> (8 * i));
	}

	unsigned long long MatrixTileMaccArithmeticAbi::Multiply(unsigned long long leftBits, unsigned long long rightBits, std::shared_ptr<BinaryFormat> format)
	{
		std::shared_ptr<BinaryValue> left = Decode(leftBits, format);
		std::shared_ptr<BinaryValue> right = Decode(rightBits, format);
		if (left->Kind == BinaryValueKind::NaN || right->Kind == BinaryValueKind::NaN)
		{
			return format->CanonicalNaN;
		}

		if ((left->Kind == BinaryValueKind::Infinity && right->IsZero) || (right->Kind == BinaryValueKind::Infinity && left->IsZero))
		{
			return format->CanonicalNaN;
		}

		bool negative = left->Negative ^ right->Negative;
		if (left->Kind == BinaryValueKind::Infinity || right->Kind == BinaryValueKind::Infinity)
		{
			return EncodeInfinity(negative, format);
		}

		if (left->IsZero || right->IsZero)
		{
			return EncodeZero(negative, format);
		}

		return Round(BinaryValue::Finite(negative, left->Significand * right->Significand, left->Exponent), format, right->Exponent);
	}

	unsigned long long MatrixTileMaccArithmeticAbi::Add(unsigned long long leftBits, unsigned long long rightBits, std::shared_ptr<BinaryFormat> format)
	{
		std::shared_ptr<BinaryValue> left = Decode(leftBits, format);
		std::shared_ptr<BinaryValue> right = Decode(rightBits, format);
		if (left->Kind == BinaryValueKind::NaN || right->Kind == BinaryValueKind::NaN)
		{
			return format->CanonicalNaN;
		}

		if (left->Kind == BinaryValueKind::Infinity || right->Kind == BinaryValueKind::Infinity)
		{
			if (left->Kind == BinaryValueKind::Infinity && right->Kind == BinaryValueKind::Infinity && left->Negative != right->Negative)
			{
				return format->CanonicalNaN;
			}

			std::shared_ptr<BinaryValue> infinity = left->Kind == BinaryValueKind::Infinity ? left : right;
			return EncodeInfinity(infinity->Negative, format);
		}

		if (left->IsZero && right->IsZero)
		{
			return EncodeZero(left->Negative && right->Negative, format);
		}

		int commonExponent = std::min(left->Exponent, right->Exponent);
		BigInteger leftInteger = left->Significand << (left->Exponent - commonExponent);
		BigInteger rightInteger = right->Significand << (right->Exponent - commonExponent);
		if (left->Negative)
		{
			leftInteger = -leftInteger;
		}

		if (right->Negative)
		{
			rightInteger = -rightInteger;
		}

		BigInteger exact = leftInteger + rightInteger;
		if (exact.IsZero())
		{
			return EncodeZero(false, format);
		}

		return Round(BinaryValue::Finite(exact.Sign() < 0, BigInteger::Abs(exact), commonExponent), format);
	}

	std::shared_ptr<MatrixTileMaccArithmeticAbi::BinaryValue> MatrixTileMaccArithmeticAbi::Decode(unsigned long long bits, std::shared_ptr<BinaryFormat> format)
	{
		bool negative = (bits & format->SignMask()) != 0;
		unsigned long long exponentField = (bits >> format->FractionBits) & format->ExponentMask();
		unsigned long long fraction = bits & format->FractionMask();
		if (exponentField == format->ExponentMask())
		{
			return fraction == 0 ? BinaryValue::Infinity(negative) : BinaryValue::NaN();
		}

		if (exponentField == 0)
		{
			return BinaryValue::Finite(negative, BigInteger(fraction), 1 - format->Bias - format->FractionBits);
		}

// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'checked' in this context:
// ORIGINAL LINE: return BinaryValue.Finite(negative, (BigInteger.One << format.FractionBits) + fraction, checked((int)exponentField) - format.Bias - format.FractionBits);
		return BinaryValue::Finite(negative, (BigInteger(1) << format->FractionBits) + BigInteger(fraction), static_cast<int>(exponentField) - format->Bias - format->FractionBits);
	}

	unsigned long long MatrixTileMaccArithmeticAbi::Round(std::shared_ptr<BinaryValue> value, std::shared_ptr<BinaryFormat> format, int exponentAdjustment)
	{
		if (value->Significand.IsZero())
		{
			return EncodeZero(value->Negative, format);
		}

// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'checked' in this context:
// ORIGINAL LINE: int exponent = checked(value.Exponent + exponentAdjustment);
		int exponent = value->Exponent + exponentAdjustment;
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'checked' in this context:
// ORIGINAL LINE: int bitLength = checked((int)value.Significand.GetBitLength());
		int bitLength = static_cast<int>(value->Significand.GetBitLength());
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'checked' in this context:
// ORIGINAL LINE: int topExponent = checked(bitLength - 1 + exponent);
		int topExponent = bitLength - 1 + exponent;
		int minimumNormalExponent = 1 - format->Bias;
		int maximumNormalExponent = format->Bias;

		if (topExponent >= minimumNormalExponent)
		{
			int precision = format->FractionBits + 1;
			int shift = bitLength - precision;
			BigInteger rounded = shift > 0 ? RoundShiftRightToEven(value->Significand, shift) : value->Significand << -shift;

			if (rounded.GetBitLength() > static_cast<std::size_t>(precision))
			{
				rounded >>= 1;
				topExponent++;
			}

			if (topExponent > maximumNormalExponent)
			{
				return EncodeInfinity(value->Negative, format);
			}

// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'checked' in this context:
// ORIGINAL LINE: ulong exponentField = checked((ulong)(topExponent + format.Bias));
			unsigned long long exponentField = static_cast<unsigned long long>(topExponent + format->Bias);
			BigInteger hiddenBit = BigInteger(1) << format->FractionBits;
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'checked' in this context:
// ORIGINAL LINE: ulong fraction = checked((ulong)(rounded - hiddenBit));
			unsigned long long fraction = static_cast<unsigned long long>(rounded - hiddenBit);
			return Sign(value->Negative, format) | (exponentField << format->FractionBits) | fraction;
		}

		int subnormalQuantumExponent = minimumNormalExponent - format->FractionBits;
		int subnormalShift = subnormalQuantumExponent - exponent;
		BigInteger subnormal = subnormalShift > 0 ? RoundShiftRightToEven(value->Significand, subnormalShift) : value->Significand << -subnormalShift;
		if (subnormal.IsZero())
		{
			return EncodeZero(value->Negative, format);
		}

		BigInteger smallestNormal = BigInteger(1) << format->FractionBits;
		if (subnormal >= smallestNormal)
		{
			return Sign(value->Negative, format) | (1ULL << format->FractionBits);
		}

// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'checked' in this context:
// ORIGINAL LINE: return Sign(value.Negative, format) | checked((ulong)subnormal);
		return Sign(value->Negative, format) | static_cast<unsigned long long>(subnormal);
	}

	MatrixTileMaccArithmeticAbi::BigInteger MatrixTileMaccArithmeticAbi::RoundShiftRightToEven(MatrixTileMaccArithmeticAbi::BigInteger value, int shift)
	{
		if (shift <= 0)
		{
			return value << -shift;
		}

		BigInteger quotient = value >> shift;
		BigInteger remainder = value - (quotient << shift);
		BigInteger halfway = BigInteger(1) << (shift - 1);
		if (remainder > halfway || (remainder == halfway && !quotient.IsEven()))
		{
			quotient += BigInteger(1);
		}

		return quotient;
	}

	unsigned long long MatrixTileMaccArithmeticAbi::EncodeInfinity(bool negative, std::shared_ptr<BinaryFormat> format)
	{
		return Sign(negative, format) | (format->ExponentMask() << format->FractionBits);
	}

	unsigned long long MatrixTileMaccArithmeticAbi::EncodeZero(bool negative, std::shared_ptr<BinaryFormat> format)
	{
		return Sign(negative, format);
	}

	unsigned long long MatrixTileMaccArithmeticAbi::Sign(bool negative, std::shared_ptr<BinaryFormat> format)
	{
		return negative ? format->SignMask() : 0ULL;
	}
}
