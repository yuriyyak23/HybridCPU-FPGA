#pragma once

namespace YAKSys_Hybrid_CPU::Core
{

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct CompletionRoutingAdmissionResult(DomainValidationResult Validation, LaneCompletionRoutingResult Routing)
//	{
//		public bool IsRouted
//		{
//			get
//			{
//				return Validation.IsValid && Routing.Routed;
//			}
//		}
//
//		public static CompletionRoutingAdmissionResult Fail(DomainValidationFailureReason reason, LaneCompletionDescriptor completion, LaneCompletionRouteStatus status, string message)
//		{
//			return new(DomainValidationResult.Fail(reason, message), LaneCompletionRoutingResult.Create(status, completion, default, InterruptPostDisposition.None, routingEpoch: 0, message));
//		}
//	}


}
