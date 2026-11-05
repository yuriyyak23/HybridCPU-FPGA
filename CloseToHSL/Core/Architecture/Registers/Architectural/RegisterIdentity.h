#pragma once

namespace YAKSys_Hybrid_CPU::Core::Registers
{
// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct ArchRegId
//	{
//		/// <summary>
//		/// Decode-side dependency summaries currently encode architectural register usage
//		/// into 64-bit read/write masks. If the architectural register space grows beyond
//		/// this boundary, transport/dependency summaries must migrate to a wider bitset
//		/// rather than silently truncating published register facts.
//		/// </summary>
//		public const int DependencyMaskBitCount = 64;
//
//		public const int RegisterCount = 32;
//		public const byte MinValue = 0;
//		public const byte MaxValue = RegisterCount - 1;
//
//		public static ArchRegId Zero
//		{
//			get;
//		} = new(0, skipValidation: true);
//
//		public byte Value
//		{
//			get;
//		}
//
//		public ArchRegId(byte value)
//		{
//			if (value > MaxValue)
//				throw new ArgumentOutOfRangeException(nameof(value), value, string.Format("Architectural register id must be in [0, {0}].", MaxValue));
//
//			Value = value;
//		}
//
//		private ArchRegId(byte value, bool skipValidation)
//		{
//			Value = value;
//		}
//
//		/// <summary>
//		/// Returns whether <paramref name="value"/> is representable as an
//		/// architectural-register identity. This is not an operand-legality check.
//		/// </summary>
//		public static bool IsRepresentable(int value)
//		{
//			return (uint)value < RegisterCount;
//		}
//
//		/// <summary>
//		/// Reconstructs a checked architectural-register identity from its
//		/// retained byte representation.
//		/// </summary>
//		public static ArchRegId FromRawValue(byte value)
//		{
//			return new(value);
//		}
//
//		/// <summary>
//		/// Projects this valid architectural-register identity to its retained
//		/// byte representation.
//		/// </summary>
//		public byte ToRawValue()
//		{
//			return Value;
//		}
//
//		public static ArchRegId Create(int value)
//		{
//			if (!TryCreate(value, out ArchRegId regId))
//				throw new ArgumentOutOfRangeException(nameof(value), value, string.Format("Architectural register id must be in [0, {0}].", MaxValue));
//
//			return regId;
//		}
//
//		public static bool TryCreate(int value, out ArchRegId regId)
//		{
//			if (IsRepresentable(value))
//			{
//				regId = new ArchRegId((byte)value, skipValidation: true);
//				return true;
//			}
//
//			regId = default;
//			return false;
//		}
//
//		public override string ToString()
//		{
//			return string.Format("x{0}", Value);
//		}
//
//		public static implicit operator int(ArchRegId regId)
//		{
//			return regId.Value;
//		}
//		public static explicit operator byte(ArchRegId regId)
//		{
//			return regId.Value;
//		}
//		public static explicit operator ArchRegId(int value)
//		{
//			return Create(value);
//		}
//	}

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct VtId
//	{
//		public const int SmtWayCount = 4;
//		public const byte MinValue = 0;
//		public const byte MaxValue = SmtWayCount - 1;
//
//		public byte Value
//		{
//			get;
//		}
//
//		[System.Text.Json.Serialization.JsonConstructor] public VtId(byte value)
//		{
//			if (value > MaxValue)
//				throw new ArgumentOutOfRangeException(nameof(value), value, string.Format("VT id must be in [0, {0}].", MaxValue));
//
//			Value = value;
//		}
//
//		private VtId(byte value, bool skipValidation)
//		{
//			Value = value;
//		}
//
//		/// <summary>
//		/// Returns whether <paramref name="value"/> is representable as an SMT
//		/// virtual-thread identity. This is not a legality or ownership check.
//		/// </summary>
//		public static bool IsRepresentable(int value)
//		{
//			return (uint)value < SmtWayCount;
//		}
//
//		/// <summary>
//		/// Reconstructs a checked SMT identity from its retained byte wire form.
//		/// </summary>
//		public static VtId FromRawValue(byte value)
//		{
//			return new(value);
//		}
//
//		/// <summary>
//		/// Projects this valid SMT identity to its retained byte wire form.
//		/// </summary>
//		public byte ToRawValue()
//		{
//			return Value;
//		}
//
//		public static VtId Create(int value)
//		{
//			if (!TryCreate(value, out VtId vtId))
//				throw new ArgumentOutOfRangeException(nameof(value), value, string.Format("VT id must be in [0, {0}].", MaxValue));
//
//			return vtId;
//		}
//
//		public static bool TryCreate(int value, out VtId vtId)
//		{
//			if (IsRepresentable(value))
//			{
//				vtId = new VtId((byte)value, skipValidation: true);
//				return true;
//			}
//
//			vtId = default;
//			return false;
//		}
//
//		public override string ToString()
//		{
//			return string.Format("vt{0}", Value);
//		}
//
//		public static implicit operator int(VtId vtId)
//		{
//			return vtId.Value;
//		}
//		public static explicit operator byte(VtId vtId)
//		{
//			return vtId.Value;
//		}
//		public static explicit operator VtId(int value)
//		{
//			return Create(value);
//		}
//	}

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct PhysRegId
//	{
//		public const int RegisterCount = PhysicalRegisterFile.TotalPhysRegs;
//		public const ushort MinValue = 0;
//		public const ushort MaxValue = RegisterCount - 1;
//		public static readonly PhysRegId Zero = new(0, skipValidation: true);
//
//		public ushort Value
//		{
//			get;
//		}
//
//		public PhysRegId(ushort value)
//		{
//			if (value >= RegisterCount)
//				throw new ArgumentOutOfRangeException(nameof(value), value, string.Format("Physical register id must be in [0, {0}].", RegisterCount - 1));
//
//			Value = value;
//		}
//
//		private PhysRegId(ushort value, bool skipValidation)
//		{
//			Value = value;
//		}
//
//		/// <summary>
//		/// Returns whether <paramref name="value"/> is representable as a
//		/// physical-register identity. This is not an allocation or mapping check.
//		/// </summary>
//		public static bool IsRepresentable(int value)
//		{
//			return (uint)value < RegisterCount;
//		}
//
//		/// <summary>
//		/// Reconstructs a checked physical-register identity from its retained
//		/// unsigned-short representation.
//		/// </summary>
//		public static PhysRegId FromRawValue(ushort value)
//		{
//			return new(value);
//		}
//
//		/// <summary>
//		/// Projects this valid physical-register identity to its retained
//		/// unsigned-short representation.
//		/// </summary>
//		public ushort ToRawValue()
//		{
//			return Value;
//		}
//
//		public static PhysRegId Create(int value)
//		{
//			if (!TryCreate(value, out PhysRegId regId))
//				throw new ArgumentOutOfRangeException(nameof(value), value, string.Format("Physical register id must be in [0, {0}].", RegisterCount - 1));
//
//			return regId;
//		}
//
//		public static bool TryCreate(int value, out PhysRegId regId)
//		{
//			if (IsRepresentable(value))
//			{
//				regId = new PhysRegId((ushort)value, skipValidation: true);
//				return true;
//			}
//
//			regId = default;
//			return false;
//		}
//
//		public override string ToString()
//		{
//			return string.Format("p{0}", Value);
//		}
//
//		public static implicit operator int(PhysRegId regId)
//		{
//			return regId.Value;
//		}
//		public static explicit operator ushort(PhysRegId regId)
//		{
//			return regId.Value;
//		}
//		public static explicit operator PhysRegId(int value)
//		{
//			return Create(value);
//		}
//	}
}

#include <stdexcept>
#include <string>
namespace YAKSys_Hybrid_CPU::Core::Registers {
class ArchRegId final {
    unsigned char value_ = 0;
public:
    static constexpr int DependencyMaskBitCount = 64; static constexpr int Count = 32;
    static constexpr int RegisterCount = Count;
    static constexpr unsigned char MinValue = 0, MaxValue = Count - 1;
    static const ArchRegId Zero;
    explicit ArchRegId(unsigned char value = 0) : value_(value) { if (value >= Count) throw std::out_of_range("ArchRegId"); }
    unsigned char getValue() const noexcept { return value_; }
    unsigned char ToRawValue() const noexcept { return value_; }
    static bool IsRepresentable(int value) noexcept { return value >= 0 && value < Count; }
    static ArchRegId FromRawValue(unsigned char value) { return ArchRegId(value); }
    static ArchRegId Create(int value) { if (!IsRepresentable(value)) throw std::out_of_range("ArchRegId"); return ArchRegId(static_cast<unsigned char>(value)); }
    static bool TryCreate(int value, ArchRegId &out) noexcept { if (!IsRepresentable(value)) { out = ArchRegId(); return false; } out = ArchRegId(static_cast<unsigned char>(value)); return true; }
    std::wstring ToString() const { return L"x" + std::to_wstring(value_); }
    operator int() const noexcept { return value_; }
    explicit operator unsigned char() const noexcept { return value_; }
    bool operator==(const ArchRegId&) const = default;
};
inline const ArchRegId ArchRegId::Zero{};
class PhysRegId final {
    unsigned short value_ = 0;
public:
    static constexpr int RegisterCount = 128;
    static constexpr unsigned short MinValue = 0, MaxValue = RegisterCount - 1;
    static const PhysRegId Zero;
    explicit PhysRegId(unsigned short value = 0) : value_(value) {
        if (!IsRepresentable(value)) throw std::out_of_range("PhysRegId");
    }
    unsigned short getValue() const noexcept { return value_; }
    unsigned short ToRawValue() const noexcept { return value_; }
    static bool IsRepresentable(int value) noexcept { return value >= 0 && value < RegisterCount; }
    static PhysRegId FromRawValue(unsigned short value) { return PhysRegId(value); }
    static PhysRegId Create(int value) {
        if (!IsRepresentable(value)) throw std::out_of_range("PhysRegId");
        return PhysRegId(static_cast<unsigned short>(value));
    }
    static bool TryCreate(int value, PhysRegId& out) noexcept {
        if (!IsRepresentable(value)) { out = PhysRegId(); return false; }
        out = PhysRegId(static_cast<unsigned short>(value)); return true;
    }
    std::wstring ToString() const { return L"p" + std::to_wstring(value_); }
    operator int() const noexcept { return value_; }
    explicit operator unsigned short() const noexcept { return value_; }
    bool operator==(const PhysRegId&) const = default;
};
inline const PhysRegId PhysRegId::Zero{};
class VtId final {
    unsigned char value_ = 0;
public:
    static constexpr int SmtWayCount = 4;
    static constexpr unsigned char MinValue = 0, MaxValue = SmtWayCount - 1;
    explicit VtId(unsigned char value = 0) : value_(value) {
        if (!IsRepresentable(value)) throw std::out_of_range("VtId");
    }
    unsigned char getValue() const noexcept { return value_; }
    unsigned char ToRawValue() const noexcept { return value_; }
    static bool IsRepresentable(int value) noexcept { return value >= 0 && value < SmtWayCount; }
    static VtId FromRawValue(unsigned char value) { return VtId(value); }
    static VtId Create(int value) {
        if (!IsRepresentable(value)) throw std::out_of_range("VtId");
        return VtId(static_cast<unsigned char>(value));
    }
    static bool TryCreate(int value, VtId& out) noexcept {
        if (!IsRepresentable(value)) { out = VtId(); return false; }
        out = VtId(static_cast<unsigned char>(value));
        return true;
    }
    std::wstring ToString() const { return L"vt" + std::to_wstring(value_); }
    operator int() const noexcept { return value_; }
    explicit operator unsigned char() const noexcept { return value_; }
    bool operator==(const VtId&) const = default;
};
}