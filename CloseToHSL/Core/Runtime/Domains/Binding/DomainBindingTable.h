#pragma once

#include <memory>
#include <optional>
#include <string>
#include <utility>

namespace YAKSys_Hybrid_CPU::Core { class DomainRuntimeContext; }

namespace YAKSys_Hybrid_CPU::Core
{

	enum class DomainBindingAuthority : unsigned char
	{
		Runtime = 0,
		DescriptorSet = 1,
		CompatibilityProjection = 2
	};

	enum class DomainBindingDecision : unsigned char
	{
		Allowed = 0,
		MissingBinding = 1,
		RuntimeAuthorityRequired = 2,
		MissingRuntimeContext = 3,
		MissingRequiredDomains = 4,
		DomainIdMismatch = 5,
		CompatibilityProjectionDenied = 6
	};

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct DomainBindingEntry(DomainBindingAuthority Authority, ulong DomainId, DomainRuntimeContext Context, bool AllowsCompatibilityProjection)
//	{
//		public bool IsRuntimeAuthoritative
//		{
//			get
//			{
//				return Authority == DomainBindingAuthority.Runtime;
//			}
//		}
//
//		public bool HasDomainId
//		{
//			get
//			{
//				return DomainId != 0;
//			}
//		}
//
//		public bool HasRuntimeContext
//		{
//			get
//			{
//				return Context != nullptr;
//			}
//		}
//
//		public bool HasRequiredDomains
//		{
//			get
//			{
//				return HasRuntimeContext && Context.HasRequiredDomains;
//			}
//		}
//
//		public bool CanProjectToCompatibilityFrontend
//		{
//			get
//			{
//				return AllowsCompatibilityProjection && IsRuntimeAuthoritative && HasDomainId;
//			}
//		}
//	}

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct DomainBindingRequest(System.Nullable<DomainBindingEntry> Binding, ulong ExpectedDomainId, bool RequiresCompatibilityProjection);

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct DomainBindingResult(DomainBindingDecision Decision, string Reason)
//	{
//		public bool IsAllowed
//		{
//			get
//			{
//				return Decision == DomainBindingDecision.Allowed;
//			}
//		}
//
//		public static DomainBindingResult Allowed
//		{
//			get;
//		} = new(DomainBindingDecision.Allowed, "Domain binding allowed.");
//
//		public static DomainBindingResult Denied(DomainBindingDecision decision, string reason)
//		{
//			return new(decision, reason);
//		}
//	}


	struct DomainBindingEntry final
	{
		const DomainBindingAuthority Authority;
		const unsigned long long DomainId;
		const std::shared_ptr<DomainRuntimeContext> Context;
		const bool AllowsCompatibilityProjection;

		DomainBindingEntry(DomainBindingAuthority authority, unsigned long long domainId,
			std::shared_ptr<DomainRuntimeContext> context, bool allowsCompatibilityProjection)
			: Authority(authority), DomainId(domainId), Context(std::move(context)),
			  AllowsCompatibilityProjection(allowsCompatibilityProjection) {}
		bool IsRuntimeAuthoritative() const noexcept { return Authority == DomainBindingAuthority::Runtime; }
		bool HasDomainId() const noexcept { return DomainId != 0; }
		bool HasRuntimeContext() const noexcept { return Context != nullptr; }
		bool HasRequiredDomains() const;
		bool CanProjectToCompatibilityFrontend() const noexcept { return AllowsCompatibilityProjection && IsRuntimeAuthoritative() && HasDomainId(); }
	};

	struct DomainBindingRequest final
	{
		const std::optional<DomainBindingEntry> Binding;
		const unsigned long long ExpectedDomainId;
		const bool RequiresCompatibilityProjection;
		DomainBindingRequest(std::optional<DomainBindingEntry> binding, unsigned long long expectedDomainId, bool requiresCompatibilityProjection)
			: Binding(std::move(binding)), ExpectedDomainId(expectedDomainId), RequiresCompatibilityProjection(requiresCompatibilityProjection) {}
	};

	struct DomainBindingResult final
	{
		const DomainBindingDecision Decision;
		const std::wstring Reason;
		const bool IsAllowed;
		DomainBindingResult(DomainBindingDecision decision, std::wstring reason)
			: Decision(decision), Reason(std::move(reason)), IsAllowed(decision == DomainBindingDecision::Allowed) {}
		static std::shared_ptr<DomainBindingResult> Allowed()
		{
			return std::make_shared<DomainBindingResult>(DomainBindingDecision::Allowed, L"Domain binding allowed.");
		}
		static std::shared_ptr<DomainBindingResult> Denied(DomainBindingDecision decision, std::wstring reason)
		{
			return std::make_shared<DomainBindingResult>(decision, std::move(reason));
		}
	};
}
