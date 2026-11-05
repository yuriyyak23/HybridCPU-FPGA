#pragma once

#include "../../Events/Injection/InterruptRemapPolicy.h"
#include <string>
#include <vector>
#include <deque>
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{
	enum class LaneCompletionSourceKind : unsigned char
	{
		None = 0,
		DmaStreamComputeLane6 = 1,
		ExternalAcceleratorLane7 = 2
	};

	enum class LaneCompletionRouteStatus : unsigned char
	{
		None = 0,
		Routed = 1,
		IgnoredRouteDisabled = 2,
		DroppedInvalid = 3,
		DroppedQueueFull = 4,
		DroppedEventQueue = 5
	};

class EventInjectionDescriptor;

class LaneCompletionDescriptor final {
public:
 LaneCompletionSourceKind SourceKind=LaneCompletionSourceKind::None; unsigned char LaneIndex=0; unsigned short SourceOpcode=0, OwnerVirtualThreadId=0, ExecutionDomainTag=0, AddressSpaceTag=0;
 unsigned long long TokenId=0, TokenGeneration=0, DescriptorIdentityHash=0, NormalizedFootprintHash=0, OwnerDomainTag=0, RuntimeQueueSequence=0, CompletionStatus=0; bool Succeeded=false; unsigned long long Sequence=0;
 LaneCompletionDescriptor(LaneCompletionSourceKind kind, unsigned char lane, unsigned short opcode, unsigned short owner, unsigned short domain, unsigned short space, unsigned long long token, unsigned long long generation, unsigned long long identity, unsigned long long footprint, unsigned long long tag, unsigned long long queue, unsigned long long status, bool succeeded, unsigned long long sequence) : SourceKind(kind), LaneIndex(lane), SourceOpcode(opcode), OwnerVirtualThreadId(owner), ExecutionDomainTag(domain), AddressSpaceTag(space), TokenId(token), TokenGeneration(generation), DescriptorIdentityHash(identity), NormalizedFootprintHash(footprint), OwnerDomainTag(tag), RuntimeQueueSequence(queue), CompletionStatus(status), Succeeded(succeeded), Sequence(sequence) {}
 bool IsValid() const { return SourceKind != LaneCompletionSourceKind::None && (LaneIndex == 6 || LaneIndex == 7) && SourceOpcode != 0 && TokenId != 0; }
 unsigned long long EncodePayload() const { auto p=TokenId ^ (TokenGeneration<<7) ^ (DescriptorIdentityHash<<13) ^ (NormalizedFootprintHash>>11) ^ (OwnerDomainTag<<3) ^ (RuntimeQueueSequence<<17) ^ (CompletionStatus<<41) ^ (static_cast<unsigned long long>(LaneIndex)<<56) ^ (static_cast<unsigned long long>(SourceOpcode)<<32); if (Succeeded) p ^= 1ULL<<63; return p ? p : 1ULL; }
 std::shared_ptr<LaneCompletionDescriptor> WithSequence(unsigned long long sequence) const { auto p=std::make_shared<LaneCompletionDescriptor>(*this); p->Sequence=sequence; return p; }
};
class LaneCompletionRoute final {
public:
 bool Enabled=false; unsigned char LaneIndex=0, TargetVtId=0; unsigned short ExecutionDomainTag=0, AddressSpaceTag=0; unsigned char Vector=0, Priority=0; bool CoalescePosted=true;
 bool Matches(const std::shared_ptr<LaneCompletionDescriptor>& d) const { return Enabled && d && d->IsValid() && LaneIndex==d->LaneIndex && (!d->ExecutionDomainTag || ExecutionDomainTag==d->ExecutionDomainTag) && (!d->AddressSpaceTag || AddressSpaceTag==d->AddressSpaceTag) && (TargetVtId==d->OwnerVirtualThreadId || TargetVtId==0xff); }
 static std::shared_ptr<LaneCompletionRoute> EnabledForOwner(unsigned char lane, unsigned char owner, unsigned short domain, unsigned short space, unsigned char vector, unsigned char priority=0, bool coalesce=true) { auto r=std::make_shared<LaneCompletionRoute>(); r->Enabled=true; r->LaneIndex=lane; r->TargetVtId=owner; r->ExecutionDomainTag=domain; r->AddressSpaceTag=space; r->Vector=vector; r->Priority=priority; r->CoalescePosted=coalesce; return r; }
 static std::shared_ptr<LaneCompletionRoute> Disabled(unsigned char lane) { auto r=std::make_shared<LaneCompletionRoute>(); r->LaneIndex=lane; return r; }
};
// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct LaneCompletionDescriptor(LaneCompletionSourceKind SourceKind, byte LaneIndex, ushort SourceOpcode, ushort OwnerVirtualThreadId, ushort ExecutionDomainTag, ushort AddressSpaceTag, ulong TokenId, ulong TokenGeneration, ulong DescriptorIdentityHash, ulong NormalizedFootprintHash, ulong OwnerDomainTag, ulong RuntimeQueueSequence, ulong CompletionStatus, bool Succeeded, ulong Sequence)
//	{
//		public bool IsValid
//		{
//			get
//			{
//				return SourceKind != LaneCompletionSourceKind.None && (LaneIndex == 6 || LaneIndex == 7) && SourceOpcode != 0 && TokenId != 0;
//			}
//		}
//
//		public ulong EncodePayload()
//		{
//			ulong payload = TokenId;
//			payload ^= TokenGeneration << 7;
//			payload ^= DescriptorIdentityHash << 13;
//			payload ^= NormalizedFootprintHash >> 11;
//			payload ^= OwnerDomainTag << 3;
//			payload ^= RuntimeQueueSequence << 17;
//			payload ^= CompletionStatus << 41;
//			payload ^= (ulong)LaneIndex << 56;
//			payload ^= (ulong)SourceOpcode << 32;
//			if (Succeeded)
//			{
//				payload ^= 1UL << 63;
//			}
//
//			return payload == 0 ? 1UL : payload;
//		}
//
//		public LaneCompletionDescriptor WithSequence(ulong sequence)
//		{
//			return this with { Sequence = sequence };
//		}
//	}

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct LaneCompletionRoute(bool Enabled, byte LaneIndex, byte TargetVtId, ushort ExecutionDomainTag, ushort AddressSpaceTag, byte Vector, byte Priority, bool CoalescePosted)
//	{
//		public bool Matches(LaneCompletionDescriptor descriptor)
//		{
//			return Enabled && descriptor.IsValid && LaneIndex == descriptor.LaneIndex && (descriptor.ExecutionDomainTag == 0 || ExecutionDomainTag == descriptor.ExecutionDomainTag) && (descriptor.AddressSpaceTag == 0 || AddressSpaceTag == descriptor.AddressSpaceTag) && (TargetVtId == descriptor.OwnerVirtualThreadId || TargetVtId == 0xFF);
//		}
//
//		public static LaneCompletionRoute Disabled(byte laneIndex)
//		{
//			return new(Enabled: false, LaneIndex: laneIndex, TargetVtId: 0, ExecutionDomainTag: 0, AddressSpaceTag: 0, Vector: 0, Priority: 0, CoalescePosted: true);
//		}
//
//		public static LaneCompletionRoute EnabledForOwner(byte laneIndex, byte targetVtId, ushort executionDomainTag, ushort addressSpaceTag, byte vector, byte priority = 0, bool coalescePosted = true)
//		{
//			return new(Enabled: true, LaneIndex: laneIndex, TargetVtId: targetVtId, ExecutionDomainTag: executionDomainTag, AddressSpaceTag: addressSpaceTag, Vector: vector, Priority: priority, CoalescePosted: coalescePosted);
//		}
//	}

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct LaneCompletionRoutingResult(LaneCompletionRouteStatus Status, LaneCompletionDescriptor Completion, EventInjectionDescriptor Event, InterruptPostDisposition EventDisposition, ulong RoutingEpoch, string Message)
//	{
//		public bool Routed
//		{
//			get
//			{
//				return Status == LaneCompletionRouteStatus.Routed;
//			}
//		}
//
//		public static LaneCompletionRoutingResult Create(LaneCompletionRouteStatus status, LaneCompletionDescriptor completion, EventInjectionDescriptor evt, InterruptPostDisposition eventDisposition, ulong routingEpoch, string message)
//		{
//			return new(status, completion, evt, eventDisposition, routingEpoch, message);
//		}
//	}

	class LaneCompletionRoutingResult final {
public:
 LaneCompletionRouteStatus Status; std::shared_ptr<LaneCompletionDescriptor> Completion; std::shared_ptr<EventInjectionDescriptor> Event; InterruptPostDisposition EventDisposition; unsigned long long RoutingEpoch; std::wstring Message;
 LaneCompletionRoutingResult(LaneCompletionRouteStatus status, std::shared_ptr<LaneCompletionDescriptor> completion, std::shared_ptr<EventInjectionDescriptor> event, InterruptPostDisposition disposition, unsigned long long epoch, std::wstring message) : Status(status), Completion(std::move(completion)), Event(std::move(event)), EventDisposition(disposition), RoutingEpoch(epoch), Message(std::move(message)) {}
 bool Routed() const { return Status == LaneCompletionRouteStatus::Routed; }
 static std::shared_ptr<LaneCompletionRoutingResult> Create(LaneCompletionRouteStatus status, std::shared_ptr<LaneCompletionDescriptor> completion, std::shared_ptr<EventInjectionDescriptor> event, InterruptPostDisposition disposition, unsigned long long epoch, std::wstring message) { return std::make_shared<LaneCompletionRoutingResult>(status,std::move(completion),std::move(event),disposition,epoch,std::move(message)); }
};
class LaneCompletionRouterSnapshot final : public std::enable_shared_from_this<LaneCompletionRouterSnapshot>
	{
	private:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly IReadOnlyList<LaneCompletionRoute> Routes;
		const std::vector<std::shared_ptr<LaneCompletionRoute>> Routes;
		const unsigned long long RoutingEpoch;
		const unsigned long long RoutedCount;
		const unsigned long long IgnoredDisabledCount;
		const unsigned long long InvalidDroppedCount;
		const unsigned long long CapacityDroppedCount;
		const unsigned long long EventQueueDroppedCount;

	public:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: public LaneCompletionRouterSnapshot(IReadOnlyList<LaneCompletionRoute> routes, ulong routingEpoch, ulong routedCount, ulong ignoredDisabledCount, ulong invalidDroppedCount, ulong capacityDroppedCount, ulong eventQueueDroppedCount)
		LaneCompletionRouterSnapshot(const std::vector<std::shared_ptr<LaneCompletionRoute>> &routes, unsigned long long routingEpoch, unsigned long long routedCount, unsigned long long ignoredDisabledCount, unsigned long long invalidDroppedCount, unsigned long long capacityDroppedCount, unsigned long long eventQueueDroppedCount);

		const std::vector<std::shared_ptr<LaneCompletionRoute>> getRoutes() const;

		unsigned long long getRoutingEpoch() const;

		unsigned long long getRoutedCount() const;

		unsigned long long getIgnoredDisabledCount() const;

		unsigned long long getInvalidDroppedCount() const;

		unsigned long long getCapacityDroppedCount() const;

		unsigned long long getEventQueueDroppedCount() const;
	};

	class LaneCompletionQueue final : public std::enable_shared_from_this<LaneCompletionQueue>
	{
	private:
		unsigned long long InvalidDroppedCount = 0;
		unsigned long long CapacityDroppedCount = 0;

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly Queue<LaneCompletionDescriptor> _queue = new();
		std::deque<std::shared_ptr<LaneCompletionDescriptor>> _queue = std::deque<std::shared_ptr<LaneCompletionDescriptor>>();
		const int _capacity;
		unsigned long long _nextSequence = 0;

	public:
		LaneCompletionQueue(int capacity = 64);

		int getCount() const;

		unsigned long long getInvalidDroppedCount() const;
	private:
		void setInvalidDroppedCount(unsigned long long value);

	public:
		unsigned long long getCapacityDroppedCount() const;
	private:
		void setCapacityDroppedCount(unsigned long long value);

	public:
		bool TryEnqueue(std::shared_ptr<LaneCompletionDescriptor> descriptor, std::shared_ptr<LaneCompletionDescriptor> &queued, LaneCompletionRouteStatus &status);

		bool TryDequeue(std::shared_ptr<LaneCompletionDescriptor> &descriptor);

		void RestoreCounters(unsigned long long invalidDroppedCount, unsigned long long capacityDroppedCount);
	};

	class LaneCompletionRouter final : public std::enable_shared_from_this<LaneCompletionRouter>
	{
	private:
		unsigned long long RoutingEpoch = 0;
		unsigned long long RoutedCount = 0;
		unsigned long long IgnoredDisabledCount = 0;
		unsigned long long EventQueueDroppedCount = 0;

		const std::shared_ptr<LaneCompletionQueue> _queue;
		std::vector<std::shared_ptr<LaneCompletionRoute>> _routes = {LaneCompletionRoute::Disabled(0), LaneCompletionRoute::Disabled(1), LaneCompletionRoute::Disabled(2), LaneCompletionRoute::Disabled(3), LaneCompletionRoute::Disabled(4), LaneCompletionRoute::Disabled(5), LaneCompletionRoute::Disabled(6), LaneCompletionRoute::Disabled(7),};

	public:
		LaneCompletionRouter(int capacity = 64);

		int getPendingCount() const;

		unsigned long long getRoutingEpoch() const;
	private:
		void setRoutingEpoch(unsigned long long value);

	public:
		unsigned long long getRoutedCount() const;
	private:
		void setRoutedCount(unsigned long long value);

	public:
		unsigned long long getIgnoredDisabledCount() const;
	private:
		void setIgnoredDisabledCount(unsigned long long value);

	public:
		unsigned long long getInvalidDroppedCount() const;

		unsigned long long getCapacityDroppedCount() const;

		unsigned long long getEventQueueDroppedCount() const;
	private:
		void setEventQueueDroppedCount(unsigned long long value);

	public:
		void ConfigureRoute(std::shared_ptr<LaneCompletionRoute> route);

		void DisableRoute(unsigned char laneIndex);

		bool TryBuildPostedCompletion(std::shared_ptr<LaneCompletionDescriptor> descriptor, std::shared_ptr<EventInjectionDescriptor> &evt, std::shared_ptr<LaneCompletionRoutingResult> &result);

		void RecordEventQueueDrop();

		void RecordRouted();

		std::shared_ptr<LaneCompletionRouterSnapshot> CreateSnapshot();

		void RestoreSnapshot(std::shared_ptr<LaneCompletionRouterSnapshot> snapshot);

	private:
		void AdvanceRoutingEpoch();
	};
}
