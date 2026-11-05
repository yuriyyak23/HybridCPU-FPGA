#pragma once
#include <stdexcept>

namespace HybridCPU_ISE::CloseToHSL::Memory::DMA
{

	/// <summary>
	/// Checked representation of one persistent DMA-controller channel position.
	/// This value proves only the fixed 0..7 representation. It does not grant
	/// controller ownership, transfer admission, channel availability, execution,
	/// completion, interrupt, replay, retirement or publication authority.
	/// </summary>
// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct DmaChannelId
//	{
//		public const byte MinValue = 0;
//		public const byte MaxValue = 7;
//
//		public static DmaChannelId Zero
//		{
//			get;
//		} = new(0, skipValidation: true);
//
//		public byte Value
//		{
//			get;
//		}
//
//		[System.Text.Json.Serialization.JsonConstructor] public DmaChannelId(byte value)
//		{
//			if (!IsRepresentable(value))
//			{
//				throw new ArgumentOutOfRangeException(nameof(value), value, "A persistent DMA channel identity must be in the range [0, 7].");
//			}
//
//			Value = value;
//		}
//
//		private DmaChannelId(byte value, bool skipValidation)
//		{
//			Value = value;
//		}
//
//		public static bool IsRepresentable(byte value)
//		{
//			return value <= MaxValue;
//		}
//
//		public static DmaChannelId Create(byte value)
//		{
//			if (!TryCreate(value, out DmaChannelId channel))
//			{
//				throw new ArgumentOutOfRangeException(nameof(value), value, "A persistent DMA channel identity must be in the range [0, 7].");
//			}
//
//			return channel;
//		}
//
//		public static bool TryCreate(byte value, out DmaChannelId channel)
//		{
//			if (IsRepresentable(value))
//			{
//				channel = new DmaChannelId(value, skipValidation: true);
//				return true;
//			}
//
//			channel = default;
//			return false;
//		}
//
//		public static DmaChannelId FromRawValue(byte value)
//		{
//			return Create(value);
//		}
//
//		public byte ToRawValue()
//		{
//			return Value;
//		}
//
//		public override string ToString()
//		{
//			return string.Format("dma-channel{0}", Value);
//		}
//
//		public static implicit operator byte(DmaChannelId channel)
//		{
//			return channel.Value;
//		}
//
//		public static explicit operator DmaChannelId(byte value)
//		{
//			return Create(value);
//		}
//	}

	class DmaChannelId final {
		unsigned char value_ = 0;
	public:
		static constexpr unsigned char MinValue = 0;
		static constexpr unsigned char MaxValue = 7;
		DmaChannelId() = default;
		explicit DmaChannelId(unsigned char value) : value_(value) { if (!IsRepresentable(value)) throw std::out_of_range("DMA channel must be in range [0, 7]"); }
		static constexpr bool IsRepresentable(unsigned char value) noexcept { return value <= MaxValue; }
		static DmaChannelId Create(unsigned char value) { return DmaChannelId(value); }
		static bool TryCreate(unsigned char value, DmaChannelId& result) { if (!IsRepresentable(value)) { result = DmaChannelId(); return false; } result = DmaChannelId(value); return true; }
		static DmaChannelId FromRawValue(unsigned char value) { return Create(value); }
		unsigned char ToRawValue() const noexcept { return value_; }
		operator unsigned char() const noexcept { return value_; }
	};
}
