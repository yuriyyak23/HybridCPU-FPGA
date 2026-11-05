#pragma once
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{
	enum class EventInjectionKind : unsigned char
	{
		None = 0,
		ExternalInterrupt = 1,
		Exception = 2,
		Nmi = 3,
		VirtualTimer = 4,
		PostedCompletion = 5
	};

class EventInjectionDescriptor final {
public:
 EventInjectionKind Kind=EventInjectionKind::None; unsigned char Vector=0, Priority=0, TargetVtId=0; unsigned short ExecutionDomainTag=0, AddressSpaceTag=0; unsigned long long Payload=0, Sequence=0; bool Posted=false;
 bool IsValid() const { return Kind != EventInjectionKind::None; }
 bool Matches(unsigned char vt, unsigned short domain) const { return IsValid() && TargetVtId==vt && ExecutionDomainTag==domain; }
 bool CoalescesWith(const EventInjectionDescriptor& o) const { return IsValid() && o.IsValid() && Posted && o.Posted && Kind==o.Kind && Vector==o.Vector && Priority==o.Priority && TargetVtId==o.TargetVtId && ExecutionDomainTag==o.ExecutionDomainTag && AddressSpaceTag==o.AddressSpaceTag && Payload==o.Payload; }
 static std::shared_ptr<EventInjectionDescriptor> Create(EventInjectionKind kind, unsigned char vector, unsigned char target, unsigned short domain, unsigned short space=0, unsigned char priority=0, unsigned long long payload=0, bool posted=false) { return std::make_shared<EventInjectionDescriptor>(kind,vector,priority,target,domain,space,payload,0,posted); }
 std::shared_ptr<EventInjectionDescriptor> WithSequence(unsigned long long sequence) const { auto e=std::make_shared<EventInjectionDescriptor>(*this); e->Sequence=sequence; return e; }
};
// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct EventInjectionDescriptor(EventInjectionKind Kind, byte Vector, byte Priority, byte TargetVtId, ushort ExecutionDomainTag, ushort AddressSpaceTag, ulong Payload, ulong Sequence, bool Posted)
//	{
//		public bool IsValid
//		{
//			get
//			{
//				return Kind != EventInjectionKind.None;
//			}
//		}
//
//		public bool Matches(byte vtId, ushort executionDomainTag)
//		{
//			return IsValid && TargetVtId == vtId && ExecutionDomainTag == executionDomainTag;
//		}
//
//		public bool CoalescesWith(EventInjectionDescriptor other)
//		{
//			return IsValid && other.IsValid && Posted && other.Posted && Kind == other.Kind && Vector == other.Vector && Priority == other.Priority && TargetVtId == other.TargetVtId && ExecutionDomainTag == other.ExecutionDomainTag && AddressSpaceTag == other.AddressSpaceTag && Payload == other.Payload;
//		}
//
//		public static EventInjectionDescriptor Create(EventInjectionKind kind, byte vector, byte targetVtId, ushort executionDomainTag, ushort addressSpaceTag = 0, byte priority = 0, ulong payload = 0, bool posted = false)
//		{
//			return new(kind, vector, priority, targetVtId, executionDomainTag, addressSpaceTag, payload, 0, posted);
//		}
//
//		internal EventInjectionDescriptor WithSequence(ulong sequence)
//		{
//			return this with { Sequence = sequence };
//		}
//	}
}
