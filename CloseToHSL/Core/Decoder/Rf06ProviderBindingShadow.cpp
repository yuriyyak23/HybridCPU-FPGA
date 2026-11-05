#include "Rf06ProviderBindingShadow.h"
#include "../../../NonRTL/Arch/Generated/GeneratedIsaShadowCatalog.g.h"
#include "../../../tangible_exception_helper.h"
#include <algorithm>
#include <format>
#include <tuple>

using namespace YAKSys_Hybrid_CPU::Arch::Generated;

namespace YAKSys_Hybrid_CPU::Core::Decoder
{

	GeneratedProviderBindingShadowRegistry::GeneratedProviderBindingShadowRegistry(std::vector<std::shared_ptr<GeneratedProviderBindingShadowEntry>> &entries)
		: Entries(entries)
	{
		if (std::any_of(Entries.begin(), Entries.end(), [](const auto &entry) { return entry == nullptr; }))
		{
			throw std::invalid_argument("A generated provider-binding shadow cannot contain null entries.");
		}
	}

	std::vector<std::shared_ptr<GeneratedProviderBindingShadowEntry>> GeneratedProviderBindingShadowRegistry::getEntries() const
	{
		return Entries;
	}

	std::shared_ptr<GeneratedProviderBindingShadowRegistry> GeneratedProviderBindingShadowRegistry::FromGeneratedCatalog()
	{
		std::vector<std::shared_ptr<GeneratedStaticBinding>> uniqueBindings;
		std::vector<std::shared_ptr<GeneratedProviderBindingShadowEntry>> entries;
		for (const auto &descriptor : GeneratedIsaCatalog::Descriptors)
		{
			if (!descriptor)
				continue;

			auto binding = GeneratedStaticBinding::FromDescriptor(*descriptor);
			const bool alreadyPresent = std::any_of(uniqueBindings.begin(), uniqueBindings.end(),
				[&](const auto &existing) { return *existing == *binding; });
			if (!alreadyPresent)
			{
				uniqueBindings.push_back(binding);
				entries.push_back(std::make_shared<GeneratedProviderBindingShadowEntry>(binding));
			}
		}

		return std::make_shared<GeneratedProviderBindingShadowRegistry>(entries);
	}

	std::shared_ptr<GeneratedProviderBindingShadowRegistry> GeneratedProviderBindingShadowRegistry::ReorderedForTest()
	{
		const auto current = getEntries();
		std::vector<std::shared_ptr<GeneratedProviderBindingShadowEntry>> reordered(current.begin(), current.end());
		std::reverse(reordered.begin(), reordered.end());
		return std::make_shared<GeneratedProviderBindingShadowRegistry>(reordered);
	}

	std::wstring GeneratedProviderBindingShadowRegistry::CanonicalFingerprint()
	{
		const auto current = getEntries();
		std::vector<std::shared_ptr<GeneratedProviderBindingShadowEntry>> ordered(current.begin(), current.end());
		std::sort(ordered.begin(), ordered.end(), [](const auto &left, const auto &right)
		{
			const auto &leftBinding = left->Binding;
			const auto &rightBinding = right->Binding;
			return std::tie(leftBinding->Opcode, leftBinding->MaterializerId.Value,
				leftBinding->RuntimeExecutionProviderId.Value, leftBinding->LatencyModelId.Value) <
				std::tie(rightBinding->Opcode, rightBinding->MaterializerId.Value,
					rightBinding->RuntimeExecutionProviderId.Value, rightBinding->LatencyModelId.Value);
		});

		std::wstring fingerprint;
		for (const auto &entry : ordered)
		{
			if (!fingerprint.empty())
				fingerprint += L'|';
			const auto &binding = entry->Binding;
			fingerprint += std::format(L"{}:{}:{}:{}:{}:{}", binding->Opcode,
				binding->MaterializerId.Value, binding->RuntimeExecutionProviderId.Value,
				binding->LatencyModelId.Value, entry->SchemaVersion,
				entry->IsActive ? L"active" : L"inactive");
		}
		return fingerprint;
	}

	std::shared_ptr<ProviderBindingShadowValidationReport> GeneratedProviderBindingShadowValidator::Validate(std::shared_ptr<GeneratedProviderBindingShadowRegistry> registry)
	{
		ArgumentNullException::ThrowIfNull(registry);

		struct ExpectedBinding final
		{
			std::shared_ptr<GeneratedStaticBinding> Binding;
			int ReferenceCount;
		};
		std::vector<ExpectedBinding> expected;
		for (const auto &descriptor : GeneratedIsaCatalog::Descriptors)
		{
			if (!descriptor)
				continue;

			auto binding = GeneratedStaticBinding::FromDescriptor(*descriptor);
			auto found = std::find_if(expected.begin(), expected.end(), [&](const auto &entry)
			{
				return *entry.Binding == *binding;
			});
			if (found == expected.end())
				expected.push_back({std::move(binding), 1});
			else
				++found->ReferenceCount;
		}

		struct BindingGroup final
		{
			std::shared_ptr<GeneratedStaticBinding> Binding;
			std::vector<std::shared_ptr<GeneratedProviderBindingShadowEntry>> Entries;
		};
		std::vector<BindingGroup> grouped;
		const auto registryEntries = registry->getEntries();
		for (const auto &entry : registryEntries)
		{
			if (!entry || !entry->Binding)
				continue;
			auto group = std::find_if(grouped.begin(), grouped.end(), [&](const auto &candidate)
			{
				return *candidate.Binding == *entry->Binding;
			});
			if (group == grouped.end())
			{
				grouped.push_back({entry->Binding, {entry}});
			}
			else
			{
				group->Entries.push_back(entry);
			}
		}

		std::vector<std::shared_ptr<ProviderBindingShadowFailure>> failures;
		for (const auto &expectedEntry : expected)
		{
			const auto &binding = expectedEntry.Binding;
			auto group = std::find_if(grouped.begin(), grouped.end(), [&](const auto &candidate)
			{
				return *candidate.Binding == *binding;
			});
			if (group == grouped.end())
			{
				failures.push_back(std::make_shared<ProviderBindingShadowFailure>(ProviderBindingShadowFailureKind::Missing,
					Describe(binding), std::format(L"Generated catalog references the binding {} time(s), but the shadow has no entry.", expectedEntry.ReferenceCount)));
				continue;
			}

			if (group->Entries.size() != 1)
			{
				failures.push_back(std::make_shared<ProviderBindingShadowFailure>(ProviderBindingShadowFailureKind::Duplicate,
					Describe(binding), std::format(L"Expected exactly one shadow entry, found {}.", group->Entries.size())));
			}

			for (const auto &candidate : group->Entries)
			{
				if (candidate->SchemaVersion != GeneratedProviderBindingShadowEntry::ExpectedSchemaVersion)
				{
					failures.push_back(std::make_shared<ProviderBindingShadowFailure>(ProviderBindingShadowFailureKind::SchemaMismatch,
						Describe(binding), std::format(L"Expected schema '{}', got '{}'.", GeneratedProviderBindingShadowEntry::ExpectedSchemaVersion, candidate->SchemaVersion)));
				}

				if (!candidate->IsActive)
				{
					failures.push_back(std::make_shared<ProviderBindingShadowFailure>(ProviderBindingShadowFailureKind::Inactive,
						Describe(binding), L"The generated binding is present but inactive."));
				}
			}
		}

		for (const auto &entry : registryEntries)
		{
			if (!entry || !entry->Binding)
				continue;
			const bool isReferenced = std::any_of(expected.begin(), expected.end(), [&](const auto &expectedEntry)
			{
				return *expectedEntry.Binding == *entry->Binding;
			});
			if (!isReferenced)
			{
				failures.push_back(std::make_shared<ProviderBindingShadowFailure>(ProviderBindingShadowFailureKind::Unreferenced,
					Describe(entry->Binding), L"The shadow entry is not referenced by the generated catalog."));
			}
		}

		return failures.empty()
			? ProviderBindingShadowValidationReport::Valid
			: std::make_shared<ProviderBindingShadowValidationReport>(false, std::move(failures));
	}

	std::wstring GeneratedProviderBindingShadowValidator::Describe(std::shared_ptr<GeneratedStaticBinding> binding)
	{
		return std::format(L"opcode={};materializer={};provider={};latency={}", binding->Opcode,
			binding->MaterializerId.Value, binding->RuntimeExecutionProviderId.Value, binding->LatencyModelId.Value);
	}

	const std::shared_ptr<ProviderBindingShadowValidationReport> ProviderBindingShadowValidationReport::Valid =
		std::make_shared<ProviderBindingShadowValidationReport>(true, std::vector<std::shared_ptr<ProviderBindingShadowFailure>>{});
}
