#pragma once

#include <algorithm>
#include <cwctype>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>
#include "ExecutionBindingIdentity.h"

using namespace YAKSys_Hybrid_CPU::Arch::Generated;

namespace YAKSys_Hybrid_CPU::Core::Decoder
{

	/// <summary>Failure classes owned by the RF-06.1 generated-binding shadow.</summary>
	enum class ProviderBindingShadowFailureKind : unsigned char
	{
		Missing = 0,
		Duplicate = 1,
		Inactive = 2,
		Unreferenced = 3,
		SchemaMismatch = 4
	};

	/// <summary>One immutable shadow entry; it is not a runtime provider instance.</summary>
	class GeneratedProviderBindingShadowEntry final
	{
	private:
		static std::shared_ptr<GeneratedStaticBinding> RequireBinding(std::shared_ptr<GeneratedStaticBinding> binding)
		{
			if (!binding)
				throw std::invalid_argument("Generated provider-binding shadow entry requires a binding.");
			return binding;
		}

		static std::wstring RequireSchema(std::wstring value)
		{
			auto whitespace = [](wchar_t c) { return std::iswspace(c) != 0; };
			auto first = std::find_if_not(value.begin(), value.end(), whitespace);
			if (first == value.end())
				throw std::invalid_argument("Provider-binding schema version must not be empty or whitespace.");
			auto last = std::find_if_not(value.rbegin(), value.rend(), whitespace).base();
			return std::wstring(first, last);
		}

	public:
		static constexpr const wchar_t *ExpectedSchemaVersion = L"rf06.generated-provider-binding.v1";
		const std::shared_ptr<GeneratedStaticBinding> Binding;
		const std::wstring SchemaVersion;
		const bool IsActive;

		explicit GeneratedProviderBindingShadowEntry(std::shared_ptr<GeneratedStaticBinding> binding,
			std::wstring schemaVersion = ExpectedSchemaVersion, bool isActive = true)
			: Binding(RequireBinding(std::move(binding))), SchemaVersion(RequireSchema(std::move(schemaVersion))), IsActive(isActive) {}
		bool operator==(const GeneratedProviderBindingShadowEntry &other) const
		{
			return *Binding == *other.Binding && SchemaVersion == other.SchemaVersion && IsActive == other.IsActive;
		}
	};

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public sealed record GeneratedProviderBindingShadowEntry
//	{
//		public const string ExpectedSchemaVersion = "rf06.generated-provider-binding.v1";
//
//		public GeneratedProviderBindingShadowEntry(GeneratedStaticBinding binding, string schemaVersion = ExpectedSchemaVersion, bool isActive = true)
//		{
//			Binding = binding;
//			SchemaVersion = RequireSchema(schemaVersion);
//			IsActive = isActive;
//		}
//
//		public GeneratedStaticBinding Binding
//		{
//			get;
//		}
//		public string SchemaVersion
//		{
//			get;
//		}
//		public bool IsActive
//		{
//			get;
//		}
//
//		private static string RequireSchema(string value)
//		{
//			ArgumentException.ThrowIfNullOrWhiteSpace(value);
//			return value.Trim();
//		}
//	}

	/// <summary>
	/// Deterministic generated-binding shadow. RF-06.1 validates this shadow only;
	/// it does not register or resolve a production execution provider.
	/// </summary>
	class GeneratedProviderBindingShadowRegistry final : public std::enable_shared_from_this<GeneratedProviderBindingShadowRegistry>
	{
	private:
		const std::vector<std::shared_ptr<GeneratedProviderBindingShadowEntry>> Entries;

	public:
		GeneratedProviderBindingShadowRegistry(std::vector<std::shared_ptr<GeneratedProviderBindingShadowEntry>> &entries);

		std::vector<std::shared_ptr<GeneratedProviderBindingShadowEntry>> getEntries() const;

		static std::shared_ptr<GeneratedProviderBindingShadowRegistry> FromGeneratedCatalog();

		std::shared_ptr<GeneratedProviderBindingShadowRegistry> ReorderedForTest();

		std::wstring CanonicalFingerprint();
	};

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public sealed record ProviderBindingShadowFailure(ProviderBindingShadowFailureKind Kind, string Identity, string Detail);
	struct ProviderBindingShadowFailure final
	{
		const ProviderBindingShadowFailureKind Kind;
		const std::wstring Identity;
		const std::wstring Detail;
		ProviderBindingShadowFailure(ProviderBindingShadowFailureKind kind, std::wstring identity, std::wstring detail)
			: Kind(kind), Identity(std::move(identity)), Detail(std::move(detail)) {}
		bool operator==(const ProviderBindingShadowFailure &) const = default;
	};

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public sealed record ProviderBindingShadowValidationReport(bool IsValid, ImmutableArray<ProviderBindingShadowFailure> Failures)
//	{
//		public static ProviderBindingShadowValidationReport Valid
//		{
//			get;
//		} = new(true, ImmutableArray<ProviderBindingShadowFailure>.Empty);
//	}
	struct ProviderBindingShadowValidationReport final
	{
		const bool IsValid;
		const std::vector<std::shared_ptr<ProviderBindingShadowFailure>> Failures;
		ProviderBindingShadowValidationReport(bool isValid, std::vector<std::shared_ptr<ProviderBindingShadowFailure>> failures)
			: IsValid(isValid), Failures(std::move(failures)) {}
		bool operator==(const ProviderBindingShadowValidationReport &other) const
		{
			if (IsValid != other.IsValid || Failures.size() != other.Failures.size())
				return false;
			for (std::size_t index = 0; index < Failures.size(); ++index)
			{
				if (!Failures[index] || !other.Failures[index])
				{
					if (Failures[index] != other.Failures[index])
						return false;
				}
				else if (*Failures[index] != *other.Failures[index])
				{
					return false;
				}
			}
			return true;
		}
		static const std::shared_ptr<ProviderBindingShadowValidationReport> Valid;
	};

	/// <summary>
	/// Compares generated static references with the deterministic shadow registry.
	/// Ordering is deliberately excluded from validity and canonical identity.
	/// </summary>
	class GeneratedProviderBindingShadowValidator final
	{
	public:
		static std::shared_ptr<ProviderBindingShadowValidationReport> Validate(std::shared_ptr<GeneratedProviderBindingShadowRegistry> registry);

	private:
		static std::wstring Describe(std::shared_ptr<GeneratedStaticBinding> binding);
	};

}
