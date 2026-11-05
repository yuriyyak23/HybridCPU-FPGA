#pragma once

#include <memory>
#include <string>
#include <utility>
#include "VirtualizationAdmissionCertificate.h"

namespace YAKSys_Hybrid_CPU { namespace Core
{

	enum class VirtualizationAdmissionIssueDecision : unsigned char
	{
		IssuedForFaultOnlyTransport = 0,
		NotVmxMicroOp = 1,
		UnknownFrozenOpcode = 2,
		InvalidVirtualThread = 3,
		OwnerVirtualThreadMismatch = 4,
		OwnerContextMismatch = 5,
		DomainMismatch = 6,
		InvalidSourceSlot = 7,
		WorkingSlotMismatch = 8,
		DuplicateAttempt = 9
	};

	enum class VirtualizationAdmissionValidationDecision : unsigned char
	{
		ValidForFaultOnlyTransport = 0,
		MissingCertificate = 1,
		IssuerMismatch = 2,
		IssuanceNotLive = 3,
		IssuerGenerationMismatch = 4,
		OpcodeOrOperationMismatch = 5,
		VirtualThreadMismatch = 6,
		OwnerContextMismatch = 7,
		DomainMismatch = 8,
		SourceOrWorkingSlotMismatch = 9,
		BundleOrReplayMismatch = 10
	};

	struct VirtualizationAdmissionIssueResult final
	{
		const VirtualizationAdmissionIssueDecision Decision;
		const std::shared_ptr<VirtualizationAdmissionCertificate> Certificate;
		const std::wstring Reason;
		const bool IsIssued;
		VirtualizationAdmissionIssueResult(VirtualizationAdmissionIssueDecision decision, std::shared_ptr<VirtualizationAdmissionCertificate> certificate, std::wstring reason)
			: Decision(decision), Certificate(std::move(certificate)), Reason(std::move(reason)), IsIssued(Decision == VirtualizationAdmissionIssueDecision::IssuedForFaultOnlyTransport && Certificate != nullptr) {}
	};

	struct VirtualizationAdmissionValidationResult final
	{
		const VirtualizationAdmissionValidationDecision Decision;
		const std::wstring Reason;
		const bool IsValidForFaultOnlyTransport;
		VirtualizationAdmissionValidationResult(VirtualizationAdmissionValidationDecision decision, std::wstring reason)
			: Decision(decision), Reason(std::move(reason)), IsValidForFaultOnlyTransport(Decision == VirtualizationAdmissionValidationDecision::ValidForFaultOnlyTransport) {}
	};

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
// ORIGINAL LINE: internal readonly record struct VirtualizationAdmissionIssueResult(VirtualizationAdmissionIssueDecision Decision, SafetyVerifier.VirtualizationAdmissionCertificate? Certificate, string Reason)
//	internal readonly record struct VirtualizationAdmissionIssueResult(VirtualizationAdmissionIssueDecision Decision, SafetyVerifier.VirtualizationAdmissionCertificate Certificate, string Reason)
//	{
//		internal bool IsIssued
//		{
//			get
//			{
//				return Decision == VirtualizationAdmissionIssueDecision.IssuedForFaultOnlyTransport && Certificate != nullptr;
//			}
//		}
//	}

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	internal readonly record struct VirtualizationAdmissionValidationResult(VirtualizationAdmissionValidationDecision Decision, string Reason)
//	{
//		internal bool IsValidForFaultOnlyTransport
//		{
//			get
//			{
//				return Decision == VirtualizationAdmissionValidationDecision.ValidForFaultOnlyTransport;
//			}
//		}
//	}


}}
