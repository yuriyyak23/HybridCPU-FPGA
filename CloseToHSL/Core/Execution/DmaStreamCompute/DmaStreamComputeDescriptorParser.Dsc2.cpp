#include "DmaStreamComputeDescriptorParser.Dsc2.h"

namespace YAKSys_Hybrid_CPU::Core::Execution::DmaStreamCompute
{
	const std::shared_ptr<DmaStreamComputeDsc2CapabilitySet> DmaStreamComputeDsc2CapabilitySet::Empty =
		std::shared_ptr<DmaStreamComputeDsc2CapabilitySet>(new DmaStreamComputeDsc2CapabilitySet(std::vector<DmaStreamComputeDsc2CapabilityGrant>{}));

	const std::shared_ptr<DmaStreamComputeDsc2CapabilitySet> DmaStreamComputeDsc2CapabilitySet::ParserOnlyFoundation =
		DmaStreamComputeDsc2CapabilitySet::Create(std::vector<DmaStreamComputeDsc2CapabilityGrant>{
			DmaStreamComputeDsc2CapabilityGrant(DmaStreamComputeDsc2CapabilityId::CapabilityProfile,
				DmaStreamComputeDsc2CapabilityStage::ParserKnown | DmaStreamComputeDsc2CapabilityStage::Validation),
			DmaStreamComputeDsc2CapabilityGrant(DmaStreamComputeDsc2CapabilityId::AddressSpace,
				DmaStreamComputeDsc2CapabilityStage::ParserKnown | DmaStreamComputeDsc2CapabilityStage::Validation | DmaStreamComputeDsc2CapabilityStage::BackendAddressSpace),
			DmaStreamComputeDsc2CapabilityGrant(DmaStreamComputeDsc2CapabilityId::StridedRange,
				DmaStreamComputeDsc2CapabilityStage::ParserKnown | DmaStreamComputeDsc2CapabilityStage::Validation | DmaStreamComputeDsc2CapabilityStage::FootprintNormalization),
			DmaStreamComputeDsc2CapabilityGrant(DmaStreamComputeDsc2CapabilityId::Tile2D,
				DmaStreamComputeDsc2CapabilityStage::ParserKnown | DmaStreamComputeDsc2CapabilityStage::Validation | DmaStreamComputeDsc2CapabilityStage::FootprintNormalization),
			DmaStreamComputeDsc2CapabilityGrant(DmaStreamComputeDsc2CapabilityId::ScatterGather,
				DmaStreamComputeDsc2CapabilityStage::ParserKnown | DmaStreamComputeDsc2CapabilityStage::Validation | DmaStreamComputeDsc2CapabilityStage::FootprintNormalization),
			DmaStreamComputeDsc2CapabilityGrant(DmaStreamComputeDsc2CapabilityId::FootprintSummary,
				DmaStreamComputeDsc2CapabilityStage::ParserKnown | DmaStreamComputeDsc2CapabilityStage::Validation | DmaStreamComputeDsc2CapabilityStage::FootprintNormalization),
			DmaStreamComputeDsc2CapabilityGrant(DmaStreamComputeDsc2CapabilityId::NonSemanticMetadata,
				DmaStreamComputeDsc2CapabilityStage::ParserKnown)});

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private DmaStreamComputeDsc2CapabilitySet(IReadOnlyList<DmaStreamComputeDsc2CapabilityGrant> grants)
	DmaStreamComputeDsc2CapabilitySet::DmaStreamComputeDsc2CapabilitySet(const std::vector<DmaStreamComputeDsc2CapabilityGrant> &grants) : _grants(std::unordered_map<DmaStreamComputeDsc2CapabilityId, DmaStreamComputeDsc2CapabilityStage>())
	{
		if (grants.empty())
		{
			return;
		}

		for (const auto& grant : grants)
		{
			if (grant.CapabilityId == DmaStreamComputeDsc2CapabilityId::None || grant.Stages == DmaStreamComputeDsc2CapabilityStage::None)
			{
				continue;
			}

			auto existing = _grants.find(grant.CapabilityId);
			if (existing == _grants.end())
			{
				_grantOrder.push_back(grant.CapabilityId);
				_grants.emplace(grant.CapabilityId, grant.Stages);
			}
			else
			{
				existing->second |= grant.Stages;
			}
		}
	}

	std::shared_ptr<DmaStreamComputeDsc2CapabilitySet> DmaStreamComputeDsc2CapabilitySet::getEmpty()
	{
		return Empty;
	}

	std::shared_ptr<DmaStreamComputeDsc2CapabilitySet> DmaStreamComputeDsc2CapabilitySet::getParserOnlyFoundation()
	{
		return ParserOnlyFoundation;
	}

	std::vector<DmaStreamComputeDsc2CapabilityGrant> DmaStreamComputeDsc2CapabilitySet::getGrants() const
	{
		std::vector<DmaStreamComputeDsc2CapabilityGrant> grants;
		grants.reserve(_grantOrder.size());
		for (const auto capabilityId : _grantOrder)
		{
			const auto found = _grants.find(capabilityId);
			if (found != _grants.end())
			{
				grants.emplace_back(found->first, found->second);
			}
		}

		return grants;
	}

	bool DmaStreamComputeDsc2CapabilitySet::getGrantsExecution() const
	{
		return HasAnyStage(DmaStreamComputeDsc2CapabilityStage::Execution);
	}

	bool DmaStreamComputeDsc2CapabilitySet::getGrantsCompilerLowering() const
	{
		return HasAnyStage(DmaStreamComputeDsc2CapabilityStage::CompilerLowering);
	}

	std::shared_ptr<DmaStreamComputeDsc2CapabilitySet> DmaStreamComputeDsc2CapabilitySet::Create(std::vector<DmaStreamComputeDsc2CapabilityGrant> grants)
	{
		return std::shared_ptr<DmaStreamComputeDsc2CapabilitySet>(new DmaStreamComputeDsc2CapabilitySet(grants));
	}

	bool DmaStreamComputeDsc2CapabilitySet::Has(DmaStreamComputeDsc2CapabilityId capabilityId, DmaStreamComputeDsc2CapabilityStage requiredStages)
	{
		const auto found = _grants.find(capabilityId);
		return found != _grants.end() && (found->second & requiredStages) == requiredStages;
	}

	bool DmaStreamComputeDsc2CapabilitySet::HasAnyStage(DmaStreamComputeDsc2CapabilityStage stage) const
	{
		for (const auto& entry : _grants)
		{
			if ((entry.second & stage) != DmaStreamComputeDsc2CapabilityStage::None)
			{
				return true;
			}
		}

		return false;
	}
}
