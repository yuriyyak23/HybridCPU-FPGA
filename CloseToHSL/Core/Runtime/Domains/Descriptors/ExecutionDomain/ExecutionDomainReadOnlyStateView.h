#pragma once

namespace YAKSys_Hybrid_CPU::Core
{

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct ExecutionDomainReadOnlyStateView(ulong GuestPc, ulong GuestSp, ulong GuestFlags, bool HasMaterializedGuestPc, bool HasMaterializedGuestSp, bool HasMaterializedGuestFlags)
//	{
//		public ulong StateEpoch
//		{
//			get;
//			set;
//		}
//
//		public static ExecutionDomainReadOnlyStateView Unmaterialized
//		{
//			get;
//		} = new(GuestPc: 0, GuestSp: 0, GuestFlags: 0, HasMaterializedGuestPc: false, HasMaterializedGuestSp: false, HasMaterializedGuestFlags: false)
//			{
//				StateEpoch = 0,
//			};
//
//		public static ExecutionDomainReadOnlyStateView FromGuestPcSpFlags(ulong guestPc, ulong guestSp, ulong guestFlags, ulong stateEpoch = 0)
//		{
//			return new(guestPc, guestSp, guestFlags, HasMaterializedGuestPc: true, HasMaterializedGuestSp: true, HasMaterializedGuestFlags: true)
//		}
//			{
//				StateEpoch = stateEpoch,
//			};
//
//		public bool HasAnyMaterializedGuestArchitecturalState
//		{
//			get
//			{
//				return HasMaterializedGuestPc || HasMaterializedGuestSp || HasMaterializedGuestFlags;
//			}
//		}
//
//		public bool HasCompleteGuestPcSpFlags
//		{
//			get
//			{
//				return HasMaterializedGuestPc && HasMaterializedGuestSp && HasMaterializedGuestFlags;
//			}
//		}
//
//		public bool IsMaterialized
//		{
//			get
//			{
//				return HasAnyMaterializedGuestArchitecturalState;
//			}
//		}
//	}

}

#include <memory>
namespace YAKSys_Hybrid_CPU::Core {
class ExecutionDomainReadOnlyStateView final {
public:
    const unsigned long long GuestPc, GuestSp, GuestFlags;
    const bool HasMaterializedGuestPc, HasMaterializedGuestSp, HasMaterializedGuestFlags;
    const unsigned long long StateEpoch;
    ExecutionDomainReadOnlyStateView(unsigned long long pc = 0, unsigned long long sp = 0, unsigned long long flags = 0, bool hasPc = false, bool hasSp = false, bool hasFlags = false, unsigned long long epoch = 0)
        : GuestPc(pc), GuestSp(sp), GuestFlags(flags), HasMaterializedGuestPc(hasPc), HasMaterializedGuestSp(hasSp), HasMaterializedGuestFlags(hasFlags), StateEpoch(epoch) {}
    static const std::shared_ptr<ExecutionDomainReadOnlyStateView> Unmaterialized;
    static std::shared_ptr<ExecutionDomainReadOnlyStateView> FromGuestPcSpFlags(unsigned long long pc, unsigned long long sp, unsigned long long flags, unsigned long long epoch = 0) { return std::make_shared<ExecutionDomainReadOnlyStateView>(pc, sp, flags, true, true, true, epoch); }
    bool getHasAnyMaterializedGuestArchitecturalState() const noexcept { return HasMaterializedGuestPc || HasMaterializedGuestSp || HasMaterializedGuestFlags; }
    bool getHasCompleteGuestPcSpFlags() const noexcept { return HasMaterializedGuestPc && HasMaterializedGuestSp && HasMaterializedGuestFlags; }
    bool isMaterialized() const noexcept { return getHasAnyMaterializedGuestArchitecturalState(); }
};
inline const std::shared_ptr<ExecutionDomainReadOnlyStateView> ExecutionDomainReadOnlyStateView::Unmaterialized = std::make_shared<ExecutionDomainReadOnlyStateView>();
}