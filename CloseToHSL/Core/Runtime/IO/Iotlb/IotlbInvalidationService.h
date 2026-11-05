#pragma once

#include <memory>
#include <string>
#include <utility>

namespace YAKSys_Hybrid_CPU::Core
{

	enum class IotlbInvalidationScope : unsigned char
	{
		All = 0,
		IoDomainTag = 1,
		Domain = 2,
		Device = 3
	};

	enum class IotlbInvalidationDecision : unsigned char
	{
		Allowed = 0,
		MissingIoDomain = 1,
		MissingVirtualizationBlock = 2,
		RuntimeAuthorityMissing = 3,
		InvalidDescriptor = 4
	};

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct IotlbInvalidationResult(IotlbInvalidationDecision Decision, int InvalidatedEntries, string Message)
//	{
//		public bool IsAllowed
//		{
//			get
//			{
//				return Decision == IotlbInvalidationDecision.Allowed;
//			}
//		}
//
//		public static IotlbInvalidationResult Allowed(int invalidatedEntries)
//		{
//			return new(IotlbInvalidationDecision.Allowed, invalidatedEntries, string.Empty);
//		}
//
//		public static IotlbInvalidationResult Denied(IotlbInvalidationDecision decision, string message)
//		{
//			return new(decision, 0, message);
//		}
//	}


	struct IotlbInvalidationResult final
	{
		const IotlbInvalidationDecision Decision;
		const int InvalidatedEntries;
		const std::wstring Message;
		const bool IsAllowed;

		IotlbInvalidationResult(IotlbInvalidationDecision decision, int invalidatedEntries, std::wstring message)
			: Decision(decision),
			  InvalidatedEntries(invalidatedEntries),
			  Message(std::move(message)),
			  IsAllowed(decision == IotlbInvalidationDecision::Allowed)
		{
		}

		static std::shared_ptr<IotlbInvalidationResult> Allowed(int invalidatedEntries)
		{
			return std::make_shared<IotlbInvalidationResult>(IotlbInvalidationDecision::Allowed, invalidatedEntries, L"");
		}

		static std::shared_ptr<IotlbInvalidationResult> Denied(IotlbInvalidationDecision decision, std::wstring message)
		{
			return std::make_shared<IotlbInvalidationResult>(decision, 0, std::move(message));
		}
	};
}
