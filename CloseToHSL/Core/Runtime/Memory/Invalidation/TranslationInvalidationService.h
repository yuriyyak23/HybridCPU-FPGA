#pragma once

#include <memory>
#include <string>
#include <utility>

namespace YAKSys_Hybrid_CPU::Core
{

	enum class TranslationInvalidationScope : unsigned char
	{
		Address = 0,
		AddressSpace = 1,
		Global = 2
	};

	enum class TranslationInvalidationDecision : unsigned char
	{
		Allowed = 0,
		MissingMemoryDomain = 1,
		MissingAddressSpace = 2,
		MissingTranslationPolicy = 3,
		RuntimeAuthorityMissing = 4,
		PermissionDenied = 5,
		RangeDenied = 6,
		FenceRequired = 7
	};

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct TranslationInvalidationResult(TranslationInvalidationDecision Decision, int InvalidatedEntries, string Message)
//	{
//		public bool IsAllowed
//		{
//			get
//			{
//				return Decision == TranslationInvalidationDecision.Allowed;
//			}
//		}
//
//		public static TranslationInvalidationResult Allowed(int invalidatedEntries)
//		{
//			return new(TranslationInvalidationDecision.Allowed, invalidatedEntries, string.Empty);
//		}
//
//		public static TranslationInvalidationResult Denied(TranslationInvalidationDecision decision, string message)
//		{
//			return new(decision, 0, message);
//		}
//	}
	struct TranslationInvalidationResult final {
		const TranslationInvalidationDecision Decision;
		const int InvalidatedEntries;
		const std::wstring Message;
		const bool IsAllowed;
		TranslationInvalidationResult(TranslationInvalidationDecision decision, int invalidatedEntries, std::wstring message)
			: Decision(decision), InvalidatedEntries(invalidatedEntries), Message(std::move(message)), IsAllowed(decision == TranslationInvalidationDecision::Allowed) {}
		static std::shared_ptr<TranslationInvalidationResult> Allowed(int invalidatedEntries) {
			return std::make_shared<TranslationInvalidationResult>(TranslationInvalidationDecision::Allowed, invalidatedEntries, L"");
		}
		static std::shared_ptr<TranslationInvalidationResult> Denied(TranslationInvalidationDecision decision, std::wstring message) {
			return std::make_shared<TranslationInvalidationResult>(decision, 0, std::move(message));
		}
	};

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct TranslationInvalidationBackendRequest(TranslationInvalidationScope Scope, ulong Descriptor, ulong GuestPhysicalAddress, ulong SizeBytes, bool IsSecondStageRoot);

	struct TranslationInvalidationBackendRequest final {
		TranslationInvalidationScope Scope;
		unsigned long long Descriptor, GuestPhysicalAddress, SizeBytes;
		bool IsSecondStageRoot;
		TranslationInvalidationBackendRequest(TranslationInvalidationScope scope, unsigned long long descriptor, unsigned long long guestPhysicalAddress, unsigned long long sizeBytes, bool isSecondStageRoot) : Scope(scope), Descriptor(descriptor), GuestPhysicalAddress(guestPhysicalAddress), SizeBytes(sizeBytes), IsSecondStageRoot(isSecondStageRoot) {}
	};
	class ITranslationInvalidationBackend
	{
	public:
		virtual int Invalidate(std::shared_ptr<TranslationInvalidationBackendRequest> request) = 0;
	};


}
