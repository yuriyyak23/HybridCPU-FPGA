#pragma once

namespace YAKSys_Hybrid_CPU::Core
{

	/// <summary>
	/// Checked representation of a canonical or working bundle position.
	/// Representation does not grant decode legality, admission, placement,
	/// execution, replay, retirement or publication authority.
	/// </summary>
// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct SlotId
//	{
//		public const int SlotCount = 8;
//		public const byte MinValue = 0;
//		public const byte MaxValue = SlotCount - 1;
//
//		public static SlotId Zero
//		{
//			get;
//		} = new(0, skipValidation: true);
//
//		public byte Value
//		{
//			get;
//		}
//
//		[System.Text.Json.Serialization.JsonConstructor] public SlotId(byte value)
//		{
//			if (value > MaxValue)
//			{
//				throw new ArgumentOutOfRangeException(nameof(value), value, string.Format("Bundle slot id must be in [0, {0}].", MaxValue));
//			}
//
//			Value = value;
//		}
//
//		private SlotId(byte value, bool skipValidation)
//		{
//			Value = value;
//		}
//
//		/// <summary>
//		/// Returns whether <paramref name="value"/> is representable as a bundle
//		/// position. This is not a legality, occupancy or placement check.
//		/// </summary>
//		public static bool IsRepresentable(int value)
//		{
//			return (uint)value < SlotCount;
//		}
//
//		/// <summary>
//		/// Reconstructs a checked bundle position from its retained byte form.
//		/// </summary>
//		public static SlotId FromRawValue(byte value)
//		{
//			return new(value);
//		}
//
//		/// <summary>
//		/// Projects this valid bundle position to its retained byte form.
//		/// </summary>
//		public byte ToRawValue()
//		{
//			return Value;
//		}
//
//		public static SlotId Create(int value)
//		{
//			if (!TryCreate(value, out SlotId slotId))
//			{
//				throw new ArgumentOutOfRangeException(nameof(value), value, string.Format("Bundle slot id must be in [0, {0}].", MaxValue));
//			}
//
//			return slotId;
//		}
//
//		public static bool TryCreate(int value, out SlotId slotId)
//		{
//			if (IsRepresentable(value))
//			{
//				slotId = new SlotId((byte)value, skipValidation: true);
//				return true;
//			}
//
//			slotId = default;
//			return false;
//		}
//
//		public override string ToString()
//		{
//			return string.Format("slot{0}", Value);
//		}
//
//		public static implicit operator int(SlotId slotId)
//		{
//			return slotId.Value;
//		}
//		public static explicit operator byte(SlotId slotId)
//		{
//			return slotId.Value;
//		}
//		public static explicit operator SlotId(int value)
//		{
//			return Create(value);
//		}
//	}

}

#include "../../../Pipeline/Metadata/BundleWidth.h"
#include <stdexcept>
#include <string>
namespace YAKSys_Hybrid_CPU::Core {
class SlotId final {
    unsigned char value_ = 0;
public:
    static constexpr int SlotCount = BundleSlotCount;
    static constexpr unsigned char MinValue = 0, MaxValue = SlotCount - 1;
    static const SlotId Zero;
    explicit SlotId(unsigned char value = 0) : value_(value) {
        if (!IsRepresentable(value)) throw std::out_of_range("SlotId");
    }
    unsigned char getValue() const noexcept { return value_; }
    unsigned char ToRawValue() const noexcept { return value_; }
    static bool IsRepresentable(int value) noexcept { return value >= 0 && value < SlotCount; }
    static SlotId FromRawValue(unsigned char value) { return SlotId(value); }
    static SlotId Create(int value) {
        if (!IsRepresentable(value)) throw std::out_of_range("SlotId");
        return SlotId(static_cast<unsigned char>(value));
    }
    static bool TryCreate(int value, SlotId& out) noexcept {
        if (!IsRepresentable(value)) { out = SlotId(); return false; }
        out = SlotId(static_cast<unsigned char>(value));
        return true;
    }
    std::wstring ToString() const { return L"slot" + std::to_wstring(value_); }
    operator int() const noexcept { return value_; }
    explicit operator unsigned char() const noexcept { return value_; }
    bool operator==(const SlotId&) const = default;
};
inline const SlotId SlotId::Zero{};
}