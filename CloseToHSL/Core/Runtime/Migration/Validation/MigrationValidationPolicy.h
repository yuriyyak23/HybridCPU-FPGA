#pragma once

#include <memory>
#include <string>
#include <utility>

namespace YAKSys_Hybrid_CPU::Core
{

	enum class MigrationValidationDecision : unsigned char
	{
		Allowed = 0,
		PayloadClassDenied = 1,
		EvidencePolicyDenied = 2,
		HostOwnedEvidenceRejected = 3,
		RestorePolicyRejected = 4
	};

	struct MigrationValidationResult final
	{
		const MigrationValidationDecision Decision;
		const std::wstring Message;
		const bool IsAllowed;

		MigrationValidationResult(MigrationValidationDecision decision, std::wstring message)
			: Decision(decision), Message(std::move(message)),
			  IsAllowed(decision == MigrationValidationDecision::Allowed) {}

		static const std::shared_ptr<MigrationValidationResult> Allowed;

		static std::shared_ptr<MigrationValidationResult> Denied(
			MigrationValidationDecision decision, const std::wstring &message)
		{
			return std::make_shared<MigrationValidationResult>(decision, message);
		}
	};

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct MigrationValidationResult(MigrationValidationDecision Decision, string Message)
//	{
//		public bool IsAllowed
//		{
//			get
//			{
//				return Decision == MigrationValidationDecision.Allowed;
//			}
//		}
//
//		public static MigrationValidationResult Allowed
//		{
//			get;
//		} = new(MigrationValidationDecision.Allowed, string.Empty);
//
//		public static MigrationValidationResult Denied(MigrationValidationDecision decision, string message)
//		{
//			return new(decision, message);
//		}
//	}


}




