#pragma once

namespace YAKSys_Hybrid_CPU::Core
{

	/// <summary>
	/// Checked representation of one post-Stage-B physical lane.
	/// Representation does not grant typed-slot admission, placement, execution,
	/// completion, replay, retirement or publication authority.
	/// </summary>
// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct LaneId
//	{
//		public const int LaneCount = 8;
//		public const byte MinValue = 0;
//		public const byte MaxValue = LaneCount - 1;
//
//		/// <summary>Valid physical lane zero; never flexible placement or absence.</summary>
//		public static LaneId Zero
//		{
//			get;
//		} = new(0, skipValidation: true);
//
//		public byte Value
//		{
//			get;
//		}
//
//		[System.Text.Json.Serialization.JsonConstructor] public LaneId(byte value)
//		{
//			if (value > MaxValue)
//			{
//				throw new ArgumentOutOfRangeException(nameof(value), value, string.Format("Physical lane id must be in [0, {0}].", MaxValue));
//			}
//
//			Value = value;
//		}
//
//		private LaneId(byte value, bool skipValidation)
//		{
//			Value = value;
//		}
//
//		/// <summary>
//		/// Returns whether <paramref name="value"/> is representable as a physical
//		/// lane. This is not a pinning, legality, occupancy or placement check.
//		/// </summary>
//		public static bool IsRepresentable(int value)
//		{
//			return (uint)value < LaneCount;
//		}
//
//		/// <summary>
//		/// Reconstructs a checked physical lane from its retained byte form.
//		/// </summary>
//		public static LaneId FromRawValue(byte value)
//		{
//			return new(value);
//		}
//
//		/// <summary>
//		/// Projects this valid physical lane to its retained byte form.
//		/// </summary>
//		public byte ToRawValue()
//		{
//			return Value;
//		}
//
//		public static LaneId Create(int value)
//		{
//			if (!TryCreate(value, out LaneId laneId))
//			{
//				throw new ArgumentOutOfRangeException(nameof(value), value, string.Format("Physical lane id must be in [0, {0}].", MaxValue));
//			}
//
//			return laneId;
//		}
//
//		public static bool TryCreate(int value, out LaneId laneId)
//		{
//			if (IsRepresentable(value))
//			{
//				laneId = new LaneId((byte)value, skipValidation: true);
//				return true;
//			}
//
//			laneId = default;
//			return false;
//		}
//
//		public override string ToString()
//		{
//			return string.Format("lane{0}", Value);
//		}
//
//		public static implicit operator int(LaneId laneId)
//		{
//			return laneId.Value;
//		}
//		public static explicit operator byte(LaneId laneId)
//		{
//			return laneId.Value;
//		}
//		public static explicit operator LaneId(int value)
//		{
//			return Create(value);
//		}
//	}

}

#include <stdexcept>
#include <string>
namespace YAKSys_Hybrid_CPU::Core {
class LaneId final {
    unsigned char value_ = 0;
public:
    static constexpr int LaneCount = 8;
    static constexpr unsigned char MinValue = 0, MaxValue = 7;
    LaneId() = default;
    explicit LaneId(unsigned char value) : value_(value) { if(value > MaxValue) throw std::out_of_range("Physical lane id must be in [0, 7]."); }
    static bool IsRepresentable(int value) { return value >= 0 && value < LaneCount; }
    static LaneId FromRawValue(unsigned char value) { return LaneId(value); }
    unsigned char ToRawValue() const { return value_; }
    unsigned char getValue() const { return value_; }
    static LaneId Create(int value) { if(!IsRepresentable(value)) throw std::out_of_range("Physical lane id must be in [0, 7]."); return LaneId(static_cast<unsigned char>(value)); }
    static bool TryCreate(int value, LaneId &out) { if(!IsRepresentable(value)){out=LaneId();return false;} out=LaneId(static_cast<unsigned char>(value));return true; }
    std::wstring ToString() const { return L"lane" + std::to_wstring(value_); }
    operator int() const { return value_; }
    explicit operator unsigned char() const { return value_; }
    bool operator==(const LaneId &) const = default;
    static const LaneId Zero;
};
inline const LaneId LaneId::Zero{};
}