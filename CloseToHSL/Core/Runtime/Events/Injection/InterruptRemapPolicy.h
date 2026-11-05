#pragma once

#include "EventInjectionDescriptor.h"
#include <limits>
#include <vector>
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{
class InterruptRemapEntry; class InterruptRemapKey; class EventInjectionDescriptor;
	enum class InterruptPostDisposition : unsigned char
	{
		None = 0,
		Queued = 1,
		RemappedQueued = 2,
		Coalesced = 3,
		RemappedCoalesced = 4,
		DroppedInvalid = 5,
		DroppedQueueFull = 6,
		DroppedByRemap = 7
	};

	enum class InterruptRemapAction : unsigned char
	{
		Route = 0,
		Drop = 1
	};

class InterruptRemapKey final {
public:
 EventInjectionKind Kind=EventInjectionKind::None; unsigned char SourceVector=0; unsigned short SourceExecutionDomainTag=0;
 static constexpr unsigned short AnyExecutionDomainTag=std::numeric_limits<unsigned short>::max();
 bool Matches(const std::shared_ptr<EventInjectionDescriptor>& d) const { return d && d->IsValid() && Kind==d->Kind && SourceVector==d->Vector && (SourceExecutionDomainTag==AnyExecutionDomainTag || SourceExecutionDomainTag==d->ExecutionDomainTag); }
 bool operator==(const InterruptRemapKey&) const = default;
};
class InterruptRemapEntry final {
public:
 std::shared_ptr<InterruptRemapKey> Key; InterruptRemapAction Action=InterruptRemapAction::Route; unsigned char TargetVector=0, TargetVtId=0; unsigned short TargetExecutionDomainTag=0, TargetAddressSpaceTag=0; unsigned char Priority=0; bool OverridePriority=false, CoalescePosted=false;
 static std::shared_ptr<InterruptRemapEntry> Route(EventInjectionKind kind, unsigned char source, unsigned char target, unsigned char vt, unsigned short domain, unsigned short space=0, unsigned char priority=0, bool overridePriority=true, bool coalesce=true, unsigned short sourceDomain=InterruptRemapKey::AnyExecutionDomainTag) { auto e=std::make_shared<InterruptRemapEntry>(); e->Key=std::make_shared<InterruptRemapKey>(kind,source,sourceDomain); e->Action=InterruptRemapAction::Route; e->TargetVector=target; e->TargetVtId=vt; e->TargetExecutionDomainTag=domain; e->TargetAddressSpaceTag=space; e->Priority=priority; e->OverridePriority=overridePriority; e->CoalescePosted=coalesce; return e; }
 static std::shared_ptr<InterruptRemapEntry> Drop(EventInjectionKind kind, unsigned char source, unsigned short sourceDomain=InterruptRemapKey::AnyExecutionDomainTag) { auto e=std::make_shared<InterruptRemapEntry>(); e->Key=std::make_shared<InterruptRemapKey>(kind,source,sourceDomain); e->Action=InterruptRemapAction::Drop; return e; }
 bool IsDrop() const { return Action==InterruptRemapAction::Drop; }
 bool Matches(const std::shared_ptr<EventInjectionDescriptor>& d) const { return Key && Key->Matches(d); }
 std::shared_ptr<EventInjectionDescriptor> Apply(const std::shared_ptr<EventInjectionDescriptor>& d) const { if (IsDrop() || !d) return nullptr; auto r=std::make_shared<EventInjectionDescriptor>(*d); r->Vector=TargetVector; r->TargetVtId=TargetVtId; r->ExecutionDomainTag=TargetExecutionDomainTag; r->AddressSpaceTag=TargetAddressSpaceTag; if (OverridePriority) r->Priority=Priority; return r; }
};
// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct InterruptRemapKey(EventInjectionKind Kind, byte SourceVector, ushort SourceExecutionDomainTag)
//	{
//		public const ushort AnyExecutionDomainTag = ushort.MaxValue;
//
//		public bool Matches(EventInjectionDescriptor descriptor)
//		{
//			return descriptor.IsValid && Kind == descriptor.Kind && SourceVector == descriptor.Vector && (SourceExecutionDomainTag == AnyExecutionDomainTag || SourceExecutionDomainTag == descriptor.ExecutionDomainTag);
//		}
//	}

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct InterruptRemapEntry(InterruptRemapKey Key, InterruptRemapAction Action, byte TargetVector, byte TargetVtId, ushort TargetExecutionDomainTag, ushort TargetAddressSpaceTag, byte Priority, bool OverridePriority, bool CoalescePosted)
//	{
//		public bool IsDrop
//		{
//			get
//			{
//				return Action == InterruptRemapAction.Drop;
//			}
//		}
//
//		public static InterruptRemapEntry Route(EventInjectionKind kind, byte sourceVector, byte targetVector, byte targetVtId, ushort targetExecutionDomainTag, ushort targetAddressSpaceTag = 0, byte priority = 0, bool overridePriority = true, bool coalescePosted = true, ushort sourceExecutionDomainTag = InterruptRemapKey.AnyExecutionDomainTag)
//		{
//			return new(new InterruptRemapKey(kind, sourceVector, sourceExecutionDomainTag), InterruptRemapAction.Route, targetVector, targetVtId, targetExecutionDomainTag, targetAddressSpaceTag, priority, overridePriority, coalescePosted);
//		}
//
//		public static InterruptRemapEntry Drop(EventInjectionKind kind, byte sourceVector, ushort sourceExecutionDomainTag = InterruptRemapKey.AnyExecutionDomainTag)
//		{
//			return new(new InterruptRemapKey(kind, sourceVector, sourceExecutionDomainTag), InterruptRemapAction.Drop, TargetVector: 0, TargetVtId: 0, TargetExecutionDomainTag: 0, TargetAddressSpaceTag: 0, Priority: 0, OverridePriority: false, CoalescePosted: false);
//		}
//
//		public bool Matches(EventInjectionDescriptor descriptor)
//		{
//			return Key.Matches(descriptor);
//		}
//
//		public EventInjectionDescriptor Apply(EventInjectionDescriptor descriptor)
//		{
//			if (IsDrop)
//			{
//				return default;
//			}
//
//			return descriptor with { Vector = TargetVector, TargetVtId = TargetVtId, ExecutionDomainTag = TargetExecutionDomainTag, AddressSpaceTag = TargetAddressSpaceTag, Priority = OverridePriority ? Priority : descriptor.Priority};
//		}
//	}

	class InterruptRemapTable final : public std::enable_shared_from_this<InterruptRemapTable>
	{
	private:
		unsigned long long PolicyEpoch = 0;

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly List<InterruptRemapEntry> _entries = new();
		std::vector<std::shared_ptr<InterruptRemapEntry>> _entries = std::vector<std::shared_ptr<InterruptRemapEntry>>();

	public:
		int getCount() const;

		unsigned long long getPolicyEpoch() const;
	private:
		void setPolicyEpoch(unsigned long long value);

	public:
		void Configure(std::shared_ptr<InterruptRemapEntry> entry);

		bool Remove(std::shared_ptr<InterruptRemapKey> key);

		void Clear();

		bool TryRoute(std::shared_ptr<EventInjectionDescriptor> descriptor, std::shared_ptr<EventInjectionDescriptor> &routed, bool &remapped, bool &coalescePosted, InterruptPostDisposition &disposition);

	private:
		bool TryFindEntry(std::shared_ptr<EventInjectionDescriptor> descriptor, std::shared_ptr<InterruptRemapEntry> &entry);

		void AdvancePolicyEpoch();
	};
}
