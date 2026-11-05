#pragma once
#include "../../Domains/Validation/YAKSys_Hybrid_CPU.Core.DomainValidationResult.h"
#include "InterruptRemapPolicy.h"

namespace YAKSys_Hybrid_CPU::Core
{

struct EventDeliveryResult final {
 std::shared_ptr<DomainValidationResult> Validation; std::shared_ptr<EventInjectionDescriptor> Event; InterruptPostDisposition Disposition;
 bool IsQueued() const { return Validation && Validation->isValid() && (Disposition==InterruptPostDisposition::Queued || Disposition==InterruptPostDisposition::RemappedQueued || Disposition==InterruptPostDisposition::Coalesced || Disposition==InterruptPostDisposition::RemappedCoalesced); }
 static std::shared_ptr<EventDeliveryResult> Fail(DomainValidationFailureReason reason, std::shared_ptr<EventInjectionDescriptor> event, InterruptPostDisposition disposition, std::wstring message) { return std::make_shared<EventDeliveryResult>(DomainValidationResult::Fail(reason,message),std::move(event),disposition); }
};
// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct EventDeliveryResult(DomainValidationResult Validation, EventInjectionDescriptor Event, InterruptPostDisposition Disposition)
//	{
//		public bool IsQueued
//		{
//			get
//			{
//				return Validation.IsValid && (Disposition == InterruptPostDisposition.Queued || Disposition == InterruptPostDisposition.RemappedQueued || Disposition == InterruptPostDisposition.Coalesced || Disposition == InterruptPostDisposition.RemappedCoalesced);
//			}
//		}
//
//		public static EventDeliveryResult Fail(DomainValidationFailureReason reason, EventInjectionDescriptor evt, InterruptPostDisposition disposition, string message)
//		{
//			return new(DomainValidationResult.Fail(reason, message), evt, disposition);
//		}
//	}


}
