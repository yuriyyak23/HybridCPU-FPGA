#pragma once

namespace YAKSys_Hybrid_CPU::Core
{

	/// <summary>
	/// Checked representation of the fixed stream-resource selector (0..3).
	/// This value proves representation only; it does not grant scheduler
	/// admission, execution, completion, replay, retirement or publication
	/// authority, and it is not a DMA channel, lane, slot, device or queue.
	/// </summary>
// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct StreamEngineId
//	{
//		public const int EngineCount = 4;
//		public const byte MinValue = 0;
//		public const byte MaxValue = EngineCount - 1;
//
//		public static StreamEngineId Zero
//		{
//			get;
//		} = new(0, skipValidation: true);
//
//		public byte Value
//		{
//			get;
//		}
//
//		[System.Text.Json.Serialization.JsonConstructor] public StreamEngineId(byte value)
//		{
//			if (value > MaxValue)
//			{
//				throw new ArgumentOutOfRangeException(nameof(value), value, string.Format("Stream engine id must be in [0, {0}].", MaxValue));
//			}
//
//			Value = value;
//		}
//
//		private StreamEngineId(byte value, bool skipValidation)
//		{
//			Value = value;
//		}
//
//		/// <summary>Returns whether a raw integer is representable as a stream selector.</summary>
//		public static bool IsRepresentable(int value)
//		{
//			return (uint)value < EngineCount;
//		}
//
//		/// <summary>Reconstructs a checked selector from its retained byte form.</summary>
//		public static StreamEngineId FromRawValue(byte value)
//		{
//			return new(value);
//		}
//
//		/// <summary>Projects this valid selector to its retained byte form.</summary>
//		public byte ToRawValue()
//		{
//			return Value;
//		}
//
//		public static StreamEngineId Create(int value)
//		{
//			if (!TryCreate(value, out StreamEngineId streamEngineId))
//			{
//				throw new ArgumentOutOfRangeException(nameof(value), value, string.Format("Stream engine id must be in [0, {0}].", MaxValue));
//			}
//
//			return streamEngineId;
//		}
//
//		public static bool TryCreate(int value, out StreamEngineId streamEngineId)
//		{
//			if (IsRepresentable(value))
//			{
//				streamEngineId = new StreamEngineId((byte)value, skipValidation: true);
//				return true;
//			}
//
//			streamEngineId = default;
//			return false;
//		}
//
//		public override string ToString()
//		{
//			return string.Format("stream{0}", Value);
//		}
//
//		public static implicit operator int(StreamEngineId streamEngineId)
//		{
//			return streamEngineId.Value;
//		}
//		public static explicit operator byte(StreamEngineId streamEngineId)
//		{
//			return streamEngineId.Value;
//		}
//		public static explicit operator StreamEngineId(int value)
//		{
//			return Create(value);
//		}
//	}

}

#include <stdexcept>
#include <string>
namespace YAKSys_Hybrid_CPU::Core {
class StreamEngineId final {
    unsigned char value_ = 0;
public:
    static constexpr int Count = 4;
    static constexpr int EngineCount = Count;
    static constexpr unsigned char MinValue = 0, MaxValue = Count - 1;
    static const StreamEngineId Zero;
    explicit StreamEngineId(unsigned char value = 0) : value_(value) { if (value >= Count) throw std::out_of_range("StreamEngineId"); }
    unsigned char getValue() const noexcept { return value_; }
    unsigned char ToRawValue() const noexcept { return value_; }
    static bool IsRepresentable(int value) noexcept { return value >= 0 && value < Count; }
    static StreamEngineId FromRawValue(unsigned char value) { return StreamEngineId(value); }
    static StreamEngineId Create(int value) { if (!IsRepresentable(value)) throw std::out_of_range("StreamEngineId"); return StreamEngineId(static_cast<unsigned char>(value)); }
    static bool TryCreate(int value, StreamEngineId &out) noexcept { if (!IsRepresentable(value)) { out = StreamEngineId(); return false; } out = StreamEngineId(static_cast<unsigned char>(value)); return true; }
    std::wstring ToString() const { return L"stream" + std::to_wstring(value_); }
    operator int() const noexcept { return value_; }
    explicit operator unsigned char() const noexcept { return value_; }
    bool operator==(const StreamEngineId&) const = default;
};
inline const StreamEngineId StreamEngineId::Zero{};
}