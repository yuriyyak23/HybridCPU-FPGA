#pragma once

#include "AcceleratorTokenStatusWord.h"
#include "AcceleratorToken.h"
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>

namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Tokens
{

	enum class AcceleratorTokenAdmissionResultKind : unsigned char
	{
		Accepted = 0,
		NonTrappingReject = 1,
		PreciseFault = 2
	};

	enum class AcceleratorTokenAdmissionRejectPolicy : unsigned char
	{
		NonTrappingReject = 0,
		PreciseFault = 1
	};

	enum class AcceleratorTokenLookupIntent : unsigned char
	{
		Observe = 0,
		Wait = 1,
		Cancel = 2,
		Fence = 3,
		CommitPublication = 4
	};

	class AcceleratorTokenAdmissionResult final {
	public:
		const AcceleratorTokenAdmissionResultKind Kind;
		const std::shared_ptr<AcceleratorToken> Token;
		const AcceleratorTokenFaultCode FaultCode;
		const std::shared_ptr<YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Auth::AcceleratorGuardDecision> GuardDecision;
		const std::wstring Message;
		const bool IsAccepted, IsNonTrappingReject, RequiresPreciseFault;
	private:
		AcceleratorTokenAdmissionResult(AcceleratorTokenAdmissionResultKind kind, std::shared_ptr<AcceleratorToken> token, AcceleratorTokenFaultCode fault, std::shared_ptr<YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Auth::AcceleratorGuardDecision> guard, std::wstring message)
			: Kind(kind), Token(std::move(token)), FaultCode(fault), GuardDecision(std::move(guard)), Message(std::move(message)), IsAccepted(kind == AcceleratorTokenAdmissionResultKind::Accepted && Token != nullptr), IsNonTrappingReject(kind == AcceleratorTokenAdmissionResultKind::NonTrappingReject), RequiresPreciseFault(kind == AcceleratorTokenAdmissionResultKind::PreciseFault) {}
	public:
		std::shared_ptr<AcceleratorTokenHandle> getHandle() const { return Token ? Token->getHandle() : AcceleratorTokenHandle::Invalid; }
		static std::shared_ptr<AcceleratorTokenAdmissionResult> Accepted(std::shared_ptr<AcceleratorToken> token) {
			if (!token) throw std::invalid_argument("token");
			auto guard = token->getSubmitGuardDecision();
			return std::shared_ptr<AcceleratorTokenAdmissionResult>(new AcceleratorTokenAdmissionResult(AcceleratorTokenAdmissionResultKind::Accepted, std::move(token), AcceleratorTokenFaultCode::None, std::move(guard), L"ACCEL_SUBMIT accepted by descriptor, capability, and submit guards; token handle is evidence only."));
		}
		static std::shared_ptr<AcceleratorTokenAdmissionResult> Reject(AcceleratorTokenFaultCode fault, const std::wstring& message, std::shared_ptr<YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Auth::AcceleratorGuardDecision> guard = nullptr, AcceleratorTokenAdmissionRejectPolicy policy = AcceleratorTokenAdmissionRejectPolicy::NonTrappingReject) {
			if (fault == AcceleratorTokenFaultCode::None) throw std::invalid_argument("Rejected token admission requires a fault code.");
			return std::shared_ptr<AcceleratorTokenAdmissionResult>(new AcceleratorTokenAdmissionResult(policy == AcceleratorTokenAdmissionRejectPolicy::PreciseFault ? AcceleratorTokenAdmissionResultKind::PreciseFault : AcceleratorTokenAdmissionResultKind::NonTrappingReject, nullptr, fault, std::move(guard), message));
		}
	};

	class AcceleratorTokenTransition final {
	public:
		const bool Succeeded;
		const bool Rejected;
		const AcceleratorTokenState FromState, ToState;
		const AcceleratorTokenFaultCode FaultCode;
		const std::wstring Message;
	private:
		AcceleratorTokenTransition(bool succeeded, AcceleratorTokenState from, AcceleratorTokenState to, AcceleratorTokenFaultCode fault, std::wstring message)
			: Succeeded(succeeded), Rejected(!succeeded), FromState(from), ToState(to), FaultCode(fault), Message(std::move(message)) {}
	public:
		static std::shared_ptr<AcceleratorTokenTransition> Success(AcceleratorTokenState from, AcceleratorTokenState to, const std::wstring& message) {
			return std::shared_ptr<AcceleratorTokenTransition>(new AcceleratorTokenTransition(true, from, to, AcceleratorTokenFaultCode::None, message));
		}
		static std::shared_ptr<AcceleratorTokenTransition> Reject(AcceleratorTokenState from, AcceleratorTokenState to, AcceleratorTokenFaultCode fault, const std::wstring& message) {
			if (fault == AcceleratorTokenFaultCode::None) throw std::invalid_argument("Rejected L7-SDC token transitions require a fault code.");
			return std::shared_ptr<AcceleratorTokenTransition>(new AcceleratorTokenTransition(false, from, to, fault, message));
		}
	};

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public sealed record AcceleratorTokenTransition
//	{
//		private AcceleratorTokenTransition(bool succeeded, AcceleratorTokenState fromState, AcceleratorTokenState toState, AcceleratorTokenFaultCode faultCode, string message)
//		{
//			Succeeded = succeeded;
//			FromState = fromState;
//			ToState = toState;
//			FaultCode = faultCode;
//			Message = message;
//		}
//
//		public bool Succeeded
//		{
//			get;
//		}
//
//		public bool Rejected
//		{
//			get
//			{
//				return !Succeeded;
//			}
//		}
//
//		public AcceleratorTokenState FromState
//		{
//			get;
//		}
//
//		public AcceleratorTokenState ToState
//		{
//			get;
//		}
//
//		public AcceleratorTokenFaultCode FaultCode
//		{
//			get;
//		}
//
//		public string Message
//		{
//			get;
//		}
//
//		public static AcceleratorTokenTransition Success(AcceleratorTokenState fromState, AcceleratorTokenState toState, string message)
//		{
//			return new(succeeded: true, fromState, toState, AcceleratorTokenFaultCode.None, message);
//		}
//
//		public static AcceleratorTokenTransition Reject(AcceleratorTokenState fromState, AcceleratorTokenState toState, AcceleratorTokenFaultCode faultCode, string message)
//		{
//			if (faultCode == AcceleratorTokenFaultCode.None)
//			{
//				throw new ArgumentException("Rejected L7-SDC token transitions require a fault code.", nameof(faultCode));
//			}
//
//			return new AcceleratorTokenTransition(succeeded: false, fromState, toState, faultCode, message);
//		}
//	}

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public sealed record AcceleratorTokenAdmissionResult
//	{
//		private AcceleratorTokenAdmissionResult(AcceleratorTokenAdmissionResultKind kind, System.Nullable<AcceleratorToken> token, AcceleratorTokenFaultCode faultCode, System.Nullable<AcceleratorGuardDecision> guardDecision, string message)
//		{
//			Kind = kind;
//			Token = token;
//			FaultCode = faultCode;
//			GuardDecision = guardDecision;
//			Message = message;
//		}
//
//		public AcceleratorTokenAdmissionResultKind Kind
//		{
//			get;
//		}
//
//		public bool IsAccepted
//		{
//			get
//			{
//				return Kind == AcceleratorTokenAdmissionResultKind.Accepted && Token != nullptr;
//			}
//		}
//
//		public bool IsNonTrappingReject
//		{
//			get
//			{
//				return Kind == AcceleratorTokenAdmissionResultKind.NonTrappingReject;
//			}
//		}
//
//		public bool RequiresPreciseFault
//		{
//			get
//			{
//				return Kind == AcceleratorTokenAdmissionResultKind.PreciseFault;
//			}
//		}
//
//		public System.Nullable<AcceleratorToken> Token
//		{
//			get;
//		}
//
//		public AcceleratorTokenHandle Handle
//		{
//			get
//			{
//				return (Token == nullptr ? nullptr : Token.Handle) ?? AcceleratorTokenHandle.Invalid;
//			}
//		}
//
//		public AcceleratorTokenFaultCode FaultCode
//		{
//			get;
//		}
//
//		public System.Nullable<AcceleratorGuardDecision> GuardDecision
//		{
//			get;
//		}
//
//		public string Message
//		{
//			get;
//		}
//
//		public static AcceleratorTokenAdmissionResult Accepted(AcceleratorToken token)
//		{
//			ArgumentNullException.ThrowIfNull(token);
//			return new AcceleratorTokenAdmissionResult(AcceleratorTokenAdmissionResultKind.Accepted, token, AcceleratorTokenFaultCode.None, token.SubmitGuardDecision, "ACCEL_SUBMIT accepted by descriptor, capability, and submit guards; token handle is evidence only.");
//		}
//
//		public static AcceleratorTokenAdmissionResult Reject(AcceleratorTokenFaultCode faultCode, string message, System.Nullable<AcceleratorGuardDecision> guardDecision = nullptr, AcceleratorTokenAdmissionRejectPolicy rejectPolicy = AcceleratorTokenAdmissionRejectPolicy.NonTrappingReject)
//		{
//			if (faultCode == AcceleratorTokenFaultCode.None)
//			{
//				throw new ArgumentException("Rejected token admission requires a fault code.", nameof(faultCode));
//			}
//
//			return new AcceleratorTokenAdmissionResult(rejectPolicy == AcceleratorTokenAdmissionRejectPolicy.PreciseFault ? AcceleratorTokenAdmissionResultKind.PreciseFault : AcceleratorTokenAdmissionResultKind.NonTrappingReject, token: nullptr, faultCode, guardDecision, message);
//		}
//	}

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public sealed record AcceleratorTokenLookupResult
//	{
//		private AcceleratorTokenLookupResult(bool isAllowed, AcceleratorTokenLookupIntent intent, System.Nullable<AcceleratorToken> token, AcceleratorTokenStatusWord statusWord, AcceleratorTokenFaultCode faultCode, System.Nullable<AcceleratorGuardDecision> guardDecision, bool userVisiblePublicationAllowed, bool sideEffectsAllowed, string message)
//		{
//			IsAllowed = isAllowed;
//			Intent = intent;
//			Token = token;
//			StatusWord = statusWord;
//			FaultCode = faultCode;
//			GuardDecision = guardDecision;
//			UserVisiblePublicationAllowed = userVisiblePublicationAllowed;
//			SideEffectsAllowed = sideEffectsAllowed;
//			Message = message;
//		}
//
//		public bool IsAllowed
//		{
//			get;
//		}
//
//		public bool IsRejected
//		{
//			get
//			{
//				return !IsAllowed;
//			}
//		}
//
//		public AcceleratorTokenLookupIntent Intent
//		{
//			get;
//		}
//
//		public System.Nullable<AcceleratorToken> Token
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
//				return StatusWord.Pack();
//			}
//		}
//
//		public AcceleratorTokenFaultCode FaultCode
//		{
//			get;
//		}
//
//		public System.Nullable<AcceleratorGuardDecision> GuardDecision
//		{
//			get;
//		}
//
//		public bool UserVisiblePublicationAllowed
//		{
//			get;
//		}
//
//		public bool SideEffectsAllowed
//		{
//			get;
//		}
//
//		public string Message
//		{
//			get;
//		}
//
//		public static AcceleratorTokenLookupResult Allowed(AcceleratorTokenLookupIntent intent, AcceleratorToken token, AcceleratorGuardDecision guardDecision, string message)
//		{
//			ArgumentNullException.ThrowIfNull(token);
//			return AllowedWithStatus(intent, token, guardDecision, AcceleratorTokenStatusWord.FromToken(token), AcceleratorTokenFaultCode.None, sideEffectsAllowed: intent == AcceleratorTokenLookupIntent.Cancel, message);
//		}
//
//		public static AcceleratorTokenLookupResult AllowedWithStatus(AcceleratorTokenLookupIntent intent, AcceleratorToken token, AcceleratorGuardDecision guardDecision, AcceleratorTokenStatusWord statusWord, AcceleratorTokenFaultCode faultCode, bool sideEffectsAllowed, string message)
//		{
//			ArgumentNullException.ThrowIfNull(token);
//			return new AcceleratorTokenLookupResult(isAllowed: true, intent, token, statusWord, faultCode, guardDecision, userVisiblePublicationAllowed: false, sideEffectsAllowed, message);
//		}
//
//		public static AcceleratorTokenLookupResult Rejected(AcceleratorTokenLookupIntent intent, AcceleratorTokenState visibleState, AcceleratorTokenFaultCode faultCode, string message, System.Nullable<AcceleratorToken> token = nullptr, System.Nullable<AcceleratorGuardDecision> guardDecision = nullptr)
//		{
//			if (faultCode == AcceleratorTokenFaultCode.None)
//			{
//				throw new ArgumentException("Rejected token lookup requires a fault code.", nameof(faultCode));
//			}
//
//			AcceleratorTokenStatusFlags flags = AcceleratorTokenStatusFlags.GuardRejected | AcceleratorTokenStatusFlags.ModelOnly;
//			if (IsTerminalState(visibleState))
//			{
//				flags |= AcceleratorTokenStatusFlags.Terminal;
//			}
//
//			if (intent == AcceleratorTokenLookupIntent.CommitPublication)
//			{
//				flags |= AcceleratorTokenStatusFlags.CommitPublicationForbidden;
//			}
//
//			return new AcceleratorTokenLookupResult(isAllowed: false, intent, token, new AcceleratorTokenStatusWord(visibleState, faultCode, flags, (token == nullptr ? nullptr : token.StatusSequence) ?? 0), faultCode, guardDecision, userVisiblePublicationAllowed: false, sideEffectsAllowed: false, message);
//		}
//
//		private static bool IsTerminalState(AcceleratorTokenState state)
//		{
//			return state is AcceleratorTokenState.Committed or AcceleratorTokenState.Faulted or AcceleratorTokenState.Canceled or AcceleratorTokenState.TimedOut or AcceleratorTokenState.Abandoned;
//		}
//	}

	class AcceleratorTokenLookupResult final {
	public:
		const bool IsAllowed;
		const AcceleratorTokenLookupIntent Intent;
		const std::shared_ptr<AcceleratorToken> Token;
		const AcceleratorTokenStatusWord StatusWord;
		const AcceleratorTokenFaultCode FaultCode;
		const std::shared_ptr<YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Auth::AcceleratorGuardDecision> GuardDecision;
		const bool UserVisiblePublicationAllowed = false;
		const bool SideEffectsAllowed;
		const std::wstring Message;
		const bool IsRejected;
		const unsigned long long PackedStatusWord;
	private:
		AcceleratorTokenLookupResult(bool allowed, AcceleratorTokenLookupIntent intent, std::shared_ptr<AcceleratorToken> token, AcceleratorTokenStatusWord status, AcceleratorTokenFaultCode fault, std::shared_ptr<YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Auth::AcceleratorGuardDecision> guard, bool sideEffects, const std::wstring& message)
			: IsAllowed(allowed), Intent(intent), Token(std::move(token)), StatusWord(status), FaultCode(fault), GuardDecision(std::move(guard)), SideEffectsAllowed(sideEffects), Message(message), IsRejected(!allowed), PackedStatusWord(StatusWord.Pack()) {}
	public:
		static std::shared_ptr<AcceleratorTokenLookupResult> Allowed(AcceleratorTokenLookupIntent intent, std::shared_ptr<AcceleratorToken> token, std::shared_ptr<YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Auth::AcceleratorGuardDecision> guard, const std::wstring& message) {
			if (!token) throw std::invalid_argument("token");
			return std::shared_ptr<AcceleratorTokenLookupResult>(new AcceleratorTokenLookupResult(true, intent, token, AcceleratorTokenStatusWord::FromToken(*token), AcceleratorTokenFaultCode::None, guard, intent == AcceleratorTokenLookupIntent::Cancel, message));
		}
		static std::shared_ptr<AcceleratorTokenLookupResult> AllowedWithStatus(AcceleratorTokenLookupIntent intent, std::shared_ptr<AcceleratorToken> token, std::shared_ptr<YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Auth::AcceleratorGuardDecision> guard, AcceleratorTokenStatusWord status, AcceleratorTokenFaultCode fault, bool sideEffects, const std::wstring& message) {
			if (!token) throw std::invalid_argument("token");
			return std::shared_ptr<AcceleratorTokenLookupResult>(new AcceleratorTokenLookupResult(true, intent, std::move(token), status, fault, std::move(guard), sideEffects, message));
		}
		static std::shared_ptr<AcceleratorTokenLookupResult> Rejected(AcceleratorTokenLookupIntent intent, AcceleratorTokenState state, AcceleratorTokenFaultCode fault, const std::wstring& message, std::shared_ptr<AcceleratorToken> token = nullptr, std::shared_ptr<YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Auth::AcceleratorGuardDecision> guard = nullptr) {
			if (fault == AcceleratorTokenFaultCode::None) throw std::invalid_argument("Rejected token lookup requires a fault code.");
			AcceleratorTokenStatusFlags flags = AcceleratorTokenStatusFlags::GuardRejected | AcceleratorTokenStatusFlags::ModelOnly;
			if (state == AcceleratorTokenState::Committed || state == AcceleratorTokenState::Faulted || state == AcceleratorTokenState::Canceled || state == AcceleratorTokenState::TimedOut || state == AcceleratorTokenState::Abandoned) flags = flags | AcceleratorTokenStatusFlags::Terminal;
			if (intent == AcceleratorTokenLookupIntent::CommitPublication) flags = flags | AcceleratorTokenStatusFlags::CommitPublicationForbidden;
			return std::shared_ptr<AcceleratorTokenLookupResult>(new AcceleratorTokenLookupResult(false, intent, token, AcceleratorTokenStatusWord(state, fault, flags, token ? token->getStatusSequence() : 0), fault, std::move(guard), false, message));
		}
	};
}




