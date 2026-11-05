#pragma once

namespace YAKSys_Hybrid_CPU::Core
{
// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct VmxRootDescriptorReference(ulong Address, bool IsLegacyDefault)
//	{
//		public bool IsExplicit
//		{
//			get
//			{
//				return !IsLegacyDefault && Address != 0;
//			}
//		}
//
//		public bool IsValid
//		{
//			get
//			{
//				return IsLegacyDefault || Address != 0;
//			}
//		}
//
//		public static VmxRootDescriptorReference CompatibilityDefault
//		{
//			get;
//		} = new(Address: 0, IsLegacyDefault: true);
//
//		public static VmxRootDescriptorReference LegacyDefault
//		{
//			get;
//		} = CompatibilityDefault;
//
//		public static VmxRootDescriptorReference FromOperand(ulong address)
//		{
//			return address == 0 ? CompatibilityDefault : new VmxRootDescriptorReference(address, IsLegacyDefault: false);
//		}
//	}
struct VmxRootDescriptorReference final {
    unsigned long long Address = 0;
    bool IsLegacyDefault = false;
    bool isExplicit() const noexcept { return !IsLegacyDefault && Address != 0; }
    bool isValid() const noexcept { return IsLegacyDefault || Address != 0; }
    static VmxRootDescriptorReference FromOperand(unsigned long long address) noexcept { return address == 0 ? VmxRootDescriptorReference{0, true} : VmxRootDescriptorReference{address, false}; }
    static const VmxRootDescriptorReference CompatibilityDefault;
    static const VmxRootDescriptorReference LegacyDefault;
    bool operator==(const VmxRootDescriptorReference&) const = default;
};
inline const VmxRootDescriptorReference VmxRootDescriptorReference::CompatibilityDefault{0, true};
inline const VmxRootDescriptorReference VmxRootDescriptorReference::LegacyDefault = VmxRootDescriptorReference::CompatibilityDefault;
}
