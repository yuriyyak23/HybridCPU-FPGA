#pragma once

#include "AcceleratorToken.h"
#include "AcceleratorTokenStatusWord.h"
#include <optional>
#include <format>
#include <memory>

using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Auth;

namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Tokens
{

	enum class AcceleratorFaultPublicationDisposition : unsigned char
	{
		Published = 0,
		BlockedInvalidOwner = 1,
		DiagnosticOnly = 2,
		Rejected = 3
	};

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public sealed record AcceleratorFaultPublicationResult
//	{
//		private AcceleratorFaultPublicationResult(AcceleratorFaultPublicationDisposition disposition, System.Nullable<AcceleratorToken> token, AcceleratorTokenFaultCode faultCode, AcceleratorTokenStatusWord statusWord, System.Nullable<AcceleratorGuardDecision> guardDecision, bool hasUserVisibleStatusWord, bool userVisiblePublished, bool privilegedDiagnosticRecorded, string message)
//		{
//			Disposition = disposition;
//			Token = token;
//			FaultCode = faultCode;
//			StatusWord = statusWord;
//			GuardDecision = guardDecision;
//			HasUserVisibleStatusWord = hasUserVisibleStatusWord;
//			UserVisiblePublished = userVisiblePublished;
//			PrivilegedDiagnosticRecorded = privilegedDiagnosticRecorded;
//			Message = message;
//		}
//
//		public AcceleratorFaultPublicationDisposition Disposition
//		{
//			get;
//		}
//
//		public bool IsPublished
//		{
//			get
//			{
//				return Disposition == AcceleratorFaultPublicationDisposition.Published;
//			}
//		}
//
//		public bool IsRejected
//		{
//			get
//			{
//				return Disposition == AcceleratorFaultPublicationDisposition.Rejected;
//			}
//		}
//
//		public System.Nullable<AcceleratorToken> Token
//		{
//			get;
//		}
//
//		public AcceleratorTokenFaultCode FaultCode
//		{
//			get;
//		}
//
//		public AcceleratorTokenStatusWord StatusWord
//		{
//			get;
//		}
//
//		public ulong PackedStatusWord
//		{
//			get
//			{
//				return HasUserVisibleStatusWord ? StatusWord.Pack() : 0;
//			}
//		}
//
//		public System.Nullable<AcceleratorGuardDecision> GuardDecision
//		{
//			get;
//		}
//
//		public bool HasUserVisibleStatusWord
//		{
//			get;
//		}
//
//		public bool UserVisiblePublished
//		{
//			get;
//		}
//
//		public bool PrivilegedDiagnosticRecorded
//		{
//			get;
//		}
//
//		public bool CanAuthorizeCommit
//		{
//			get
//			{
//				return false;
//			}
//		}
//
//		public bool CanAuthorizeCancel
//		{
//			get
//			{
//				return false;
//			}
//		}
//
//		public bool CanAuthorizeFence
//		{
//			get
//			{
//				return false;
//			}
//		}
//
//		public string Message
//		{
//			get;
//		}
//
//		public static AcceleratorFaultPublicationResult Published(AcceleratorToken token, AcceleratorTokenFaultCode faultCode, AcceleratorGuardDecision guardDecision, string message)
//		{
//			ArgumentNullException.ThrowIfNull(token);
//			AcceleratorTokenStatusWord statusWord = AcceleratorTokenStatusWord.FromToken(token);
//			AcceleratorTokenFaultCode effectiveFaultCode = statusWord.FaultCode == AcceleratorTokenFaultCode.None ? faultCode : statusWord.FaultCode;
//			ThrowIfFaultCodeNone(effectiveFaultCode, nameof(faultCode));
//			return new AcceleratorFaultPublicationResult(AcceleratorFaultPublicationDisposition.Published, token, effectiveFaultCode, statusWord, guardDecision, hasUserVisibleStatusWord: true, userVisiblePublished: true, privilegedDiagnosticRecorded: false, message);
//		}
//
//		public static AcceleratorFaultPublicationResult Blocked(AcceleratorToken token, AcceleratorTokenFaultCode faultCode, AcceleratorGuardDecision guardDecision, bool privilegedDiagnosticRecorded, string message)
//		{
//			ArgumentNullException.ThrowIfNull(token);
//			ThrowIfFaultCodeNone(faultCode, nameof(faultCode));
//			return new AcceleratorFaultPublicationResult(privilegedDiagnosticRecorded ? AcceleratorFaultPublicationDisposition.DiagnosticOnly : AcceleratorFaultPublicationDisposition.BlockedInvalidOwner, token, faultCode, default, guardDecision, hasUserVisibleStatusWord: false, userVisiblePublished: false, privilegedDiagnosticRecorded, message);
//		}
//
//		public static AcceleratorFaultPublicationResult Rejected(System.Nullable<AcceleratorToken> token, AcceleratorTokenFaultCode faultCode, System.Nullable<AcceleratorGuardDecision> guardDecision, string message)
//		{
//			ThrowIfFaultCodeNone(faultCode, nameof(faultCode));
//			return new AcceleratorFaultPublicationResult(AcceleratorFaultPublicationDisposition.Rejected, token, faultCode, default, guardDecision, hasUserVisibleStatusWord: false, userVisiblePublished: false, privilegedDiagnosticRecorded: false, message);
//		}
//
//		private static void ThrowIfFaultCodeNone(AcceleratorTokenFaultCode faultCode, string paramName)
//		{
//			if (faultCode == AcceleratorTokenFaultCode.None)
//			{
//				throw new ArgumentException("L7-SDC fault publication result requires a non-None fault code.", paramName);
//			}
//		}
//	}

	class AcceleratorFaultPublicationResult final {
	public:
		const AcceleratorFaultPublicationDisposition Disposition;
		const std::shared_ptr<AcceleratorToken> Token;
		const AcceleratorTokenFaultCode FaultCode;
		const AcceleratorTokenStatusWord StatusWord;
		const std::shared_ptr<AcceleratorGuardDecision> GuardDecision;
		const bool HasUserVisibleStatusWord, UserVisiblePublished, PrivilegedDiagnosticRecorded;
		const std::wstring Message;
		const bool IsPublished, IsRejected, CanAuthorizeCommit = false, CanAuthorizeCancel = false, CanAuthorizeFence = false;
		const unsigned long long PackedStatusWord;
	private:
		AcceleratorFaultPublicationResult(AcceleratorFaultPublicationDisposition disposition, std::shared_ptr<AcceleratorToken> token, AcceleratorTokenFaultCode fault, AcceleratorTokenStatusWord status, std::shared_ptr<AcceleratorGuardDecision> guard, bool hasStatus, bool published, bool diagnostic, std::wstring message)
			: Disposition(disposition), Token(std::move(token)), FaultCode(fault), StatusWord(status), GuardDecision(std::move(guard)), HasUserVisibleStatusWord(hasStatus), UserVisiblePublished(published), PrivilegedDiagnosticRecorded(diagnostic), Message(std::move(message)), IsPublished(disposition == AcceleratorFaultPublicationDisposition::Published), IsRejected(disposition == AcceleratorFaultPublicationDisposition::Rejected), PackedStatusWord(hasStatus ? status.Pack() : 0) { if (fault == AcceleratorTokenFaultCode::None) throw std::invalid_argument("faultCode"); }
	public:
		static std::shared_ptr<AcceleratorFaultPublicationResult> Published(std::shared_ptr<AcceleratorToken> token, AcceleratorTokenFaultCode fault, std::shared_ptr<AcceleratorGuardDecision> guard, const std::wstring& message) {
			if (!token) throw std::invalid_argument("token"); auto status = AcceleratorTokenStatusWord::FromToken(*token); auto effective = status.FaultCode == AcceleratorTokenFaultCode::None ? fault : status.FaultCode;
			return std::shared_ptr<AcceleratorFaultPublicationResult>(new AcceleratorFaultPublicationResult(AcceleratorFaultPublicationDisposition::Published, token, effective, status, guard, true, true, false, message));
		}
		static std::shared_ptr<AcceleratorFaultPublicationResult> Blocked(std::shared_ptr<AcceleratorToken> token, AcceleratorTokenFaultCode fault, std::shared_ptr<AcceleratorGuardDecision> guard, bool diagnostic, const std::wstring& message) {
			if (!token) throw std::invalid_argument("token"); return std::shared_ptr<AcceleratorFaultPublicationResult>(new AcceleratorFaultPublicationResult(diagnostic ? AcceleratorFaultPublicationDisposition::DiagnosticOnly : AcceleratorFaultPublicationDisposition::BlockedInvalidOwner, token, fault, {AcceleratorTokenState{}, AcceleratorTokenFaultCode{}, AcceleratorTokenStatusFlags{}}, guard, false, false, diagnostic, message));
		}
		static std::shared_ptr<AcceleratorFaultPublicationResult> Rejected(std::shared_ptr<AcceleratorToken> token, AcceleratorTokenFaultCode fault, std::shared_ptr<AcceleratorGuardDecision> guard, const std::wstring& message) {
			return std::shared_ptr<AcceleratorFaultPublicationResult>(new AcceleratorFaultPublicationResult(AcceleratorFaultPublicationDisposition::Rejected, token, fault, {AcceleratorTokenState{}, AcceleratorTokenFaultCode{}, AcceleratorTokenStatusFlags{}}, guard, false, false, false, message));
		}
	};
	class AcceleratorExceptionPublication final : public std::enable_shared_from_this<AcceleratorExceptionPublication>
	{
	public:
		std::shared_ptr<AcceleratorFaultPublicationResult> TryPublish(std::shared_ptr<AcceleratorToken> token, AcceleratorTokenFaultCode faultCode, std::optional<std::shared_ptr<AcceleratorGuardEvidence>> &currentGuardEvidence, bool recordPrivilegedDiagnostic = false);
	};

}
