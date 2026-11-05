#include "Rf06MemoryCapabilityAdmission.h"
#include "Rf06ExecutionContracts.h"
#include <algorithm>
#include <stdexcept>

namespace YAKSys_Hybrid_CPU::Core::Decoder
{
	std::shared_ptr<MemoryAdmissionPolicy> MemoryAdmissionPolicy::Create(
		std::vector<MemoryBankId> allowedBanks,
		MemoryAccessDirection allowedDirections,
		std::vector<FrozenMemoryRange> allowedFootprint)
	{
		if (allowedDirections == MemoryAccessDirection::None)
			throw std::invalid_argument("A memory admission policy must allow at least one direction.");

		std::sort(allowedBanks.begin(), allowedBanks.end(), [](const auto &left, const auto &right) {
			return left.Value < right.Value;
		});
		allowedBanks.erase(std::unique(allowedBanks.begin(), allowedBanks.end()), allowedBanks.end());
		if (allowedBanks.empty())
			throw std::invalid_argument("A memory admission policy must allow at least one bank.");

		std::sort(allowedFootprint.begin(), allowedFootprint.end(), [](const auto &left, const auto &right) {
			return left.Address < right.Address || (left.Address == right.Address && left.Length < right.Length);
		});
		if (allowedFootprint.empty())
			throw std::invalid_argument("A memory admission policy must declare at least one allowed range.");
		for (std::size_t index = 1; index < allowedFootprint.size(); ++index)
		{
			if (allowedFootprint[index].Address <= allowedFootprint[index - 1].getLastAddress())
				throw std::invalid_argument("A memory admission policy cannot contain overlapping allowed ranges.");
		}

		return std::shared_ptr<MemoryAdmissionPolicy>(new MemoryAdmissionPolicy(
			std::move(allowedBanks), allowedDirections, std::move(allowedFootprint)));
	}

	std::shared_ptr<Rf06MemoryAdmissionResult> Rf06MemoryAdmissionResult::Accepted(
		std::shared_ptr<AdmissionRecord> admission)
	{
		if (!admission) throw std::invalid_argument("admission");
		return std::shared_ptr<Rf06MemoryAdmissionResult>(new Rf06MemoryAdmissionResult(
			true, Rf06MemoryAdmissionRejectReason::None, std::move(admission)));
	}

	std::shared_ptr<Rf06MemoryAdmissionResult> Rf06MemoryAdmissionResult::Rejected(
		std::shared_ptr<AdmissionRecord> admission, Rf06MemoryAdmissionRejectReason reason)
	{
		if (!admission) throw std::invalid_argument("admission");
		return std::shared_ptr<Rf06MemoryAdmissionResult>(new Rf06MemoryAdmissionResult(
			false, reason, std::move(admission)));
	}

	std::shared_ptr<Rf06MemoryAdmissionResult> Rf06MemoryCapabilityAdmission::AdmitStageA(
		std::shared_ptr<AdmissionRecord> admission,
		std::shared_ptr<MemoryAdmissionPolicy> policy)
	{
		if (!admission) throw std::invalid_argument("admission");
		if (!policy) throw std::invalid_argument("policy");
		if (!admission->Contract) throw std::invalid_argument("admission execution contract");

		std::shared_ptr<MemoryCapability> memory = admission->Contract->getMemory();
		if (!memory || memory->Kind == MemoryCapabilityKind::NoMemory || !memory->Bank.has_value() ||
			memory->Direction == MemoryAccessDirection::None || memory->Footprint.empty())
		{
			return Rf06MemoryAdmissionResult::Rejected(admission, Rf06MemoryAdmissionRejectReason::NotMemoryCapability);
		}

		if (std::find(policy->AllowedBanks.begin(), policy->AllowedBanks.end(), *memory->Bank) == policy->AllowedBanks.end())
			return Rf06MemoryAdmissionResult::Rejected(admission, Rf06MemoryAdmissionRejectReason::BankNotAllowed);

		const auto requestedDirections = static_cast<unsigned char>(memory->Direction);
		const auto allowedDirections = static_cast<unsigned char>(policy->AllowedDirections);
		if ((requestedDirections & static_cast<unsigned char>(~allowedDirections)) != 0)
			return Rf06MemoryAdmissionResult::Rejected(admission, Rf06MemoryAdmissionRejectReason::DirectionNotAllowed);

		const bool outsideAllowedFootprint = std::any_of(
			memory->Footprint.begin(), memory->Footprint.end(), [&](const FrozenMemoryRange &requested) {
				return std::none_of(policy->AllowedFootprint.begin(), policy->AllowedFootprint.end(),
					[&](const FrozenMemoryRange &allowed) { return Contains(allowed, requested); });
			});
		if (outsideAllowedFootprint)
			return Rf06MemoryAdmissionResult::Rejected(admission, Rf06MemoryAdmissionRejectReason::FootprintOutsidePolicy);

		return Rf06MemoryAdmissionResult::Accepted(std::move(admission));
	}

	bool Rf06MemoryCapabilityAdmission::Contains(const FrozenMemoryRange &allowed, const FrozenMemoryRange &requested)
	{
		return requested.Address >= allowed.Address && requested.getLastAddress() <= allowed.getLastAddress();
	}
}
