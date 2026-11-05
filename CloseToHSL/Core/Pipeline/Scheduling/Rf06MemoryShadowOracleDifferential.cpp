#include "Rf06MemoryShadowOracleDifferential.h"
#include "../../Decoder/MemoryBankResolution.h"
#include "../../Architecture/Common/YAKSys_Hybrid_CPU.Processor.h"
#include "../MicroOps/Types/MicroOp.Misc.h"

using namespace YAKSys_Hybrid_CPU::Core::Decoder;

namespace YAKSys_Hybrid_CPU::Core
{
	std::shared_ptr<Rf06MemoryShadowState> Rf06MemoryShadowState::Create(unsigned char m, unsigned char r, unsigned char w, const std::vector<unsigned char> &mb, const std::vector<unsigned char> &rb, const std::vector<unsigned char> &wb, const std::vector<unsigned char> &cm, const std::vector<unsigned char> &cr, const std::vector<unsigned char> &cw, const std::vector<unsigned char> &ov, const std::vector<unsigned char> &cv, unsigned short pending, unsigned short stores)
	{
		const size_t banks = MemoryBankId::BankCount;
		if (mb.size() != banks || rb.size() != banks || wb.size() != banks || cm.size() != banks || cr.size() != banks || cw.size() != banks || ov.size() != 4 || cv.size() != 4)
			throw std::invalid_argument("RF-06 memory shadow sample has an invalid vector length.");
		for (size_t i = 0; i < banks; ++i) if (cm[i] > mb[i] || cr[i] > rb[i] || cw[i] > wb[i]) throw std::invalid_argument("A dynamic memory sample cannot exceed a bank budget.");
		for (size_t i = 0; i < 4; ++i) if (ov[i] > cv[i]) throw std::invalid_argument("A dynamic memory sample cannot exceed a VT scoreboard capacity.");
		return std::make_shared<Rf06MemoryShadowState>(m, r, w, mb, rb, wb, cm, cr, cw, ov, cv, pending, stores);
	}
	std::shared_ptr<Rf06MemoryShadowState> Rf06MemoryShadowState::Consume(std::shared_ptr<MemoryCapability> capability, int vt) const
	{
		if (!capability || capability->Kind == MemoryCapabilityKind::NoMemory) return std::make_shared<Rf06MemoryShadowState>(*this);
		if (static_cast<unsigned int>(vt) >= 4 || !capability->Bank || !capability->Bank->Value) throw std::out_of_range("invalid RF-06 memory shadow consumption");
		const int bank = capability->Bank->Value;
		if (static_cast<unsigned int>(bank) >= MemoryBankId::BankCount) throw std::out_of_range("invalid memory bank");
		auto cm = ConsumedMemoryByBank, cr = ConsumedReadByBank, cw = ConsumedWriteByBank, ov = OutstandingByVirtualThread;
		++cm[bank]; ++ov[vt]; unsigned char reads = RemainingReadIssueBudget, writes = RemainingWriteIssueBudget;
		if (capability->Kind == MemoryCapabilityKind::Load) { ++cr[bank]; --reads; }
		else if (capability->Kind == MemoryCapabilityKind::Store) { ++cw[bank]; --writes; }
		unsigned short storeMask = OutstandingStoreBankMask;
		if (capability->Kind == MemoryCapabilityKind::Store) storeMask |= static_cast<unsigned short>(1u << bank);
		return Create(static_cast<unsigned char>(RemainingMemoryIssueBudget - 1), reads, writes, MemoryBankBudgets, ReadBankBudgets, WriteBankBudgets, cm, cr, cw, ov, CapacityByVirtualThread, PendingBankMask, storeMask);
	}

	std::shared_ptr<Rf06MemoryShadowDecision> Rf06MemoryShadowOracle::EvaluateContract(std::shared_ptr<AdmissionRecord> admission, std::shared_ptr<Rf06MemoryShadowState> state)
	{
		ArgumentNullException::ThrowIfNull(admission);
		return EvaluateContract(admission->Contract->getMemory(), admission->VirtualThreadId, state);
	}

	std::shared_ptr<Rf06MemoryShadowDecision> Rf06MemoryShadowOracle::EvaluateContract(std::shared_ptr<MemoryCapability> capability, int virtualThreadId, std::shared_ptr<Rf06MemoryShadowState> state)
	{
		ArgumentNullException::ThrowIfNull(capability);
		if (capability->Kind == MemoryCapabilityKind::NoMemory)
		{
			return Reject(Rf06MemoryShadowRejectReason::NotMemoryCapability);
		}
		if (static_cast<unsigned int>(virtualThreadId) >= 4)
		{
			return Reject(Rf06MemoryShadowRejectReason::InvalidVirtualThread);
		}
		if (state->RemainingMemoryIssueBudget == 0)
		{
			return Reject(Rf06MemoryShadowRejectReason::NoIssueBudget);
		}
		if (state->OutstandingByVirtualThread[virtualThreadId] >= state->CapacityByVirtualThread[virtualThreadId])
		{
			return Reject(Rf06MemoryShadowRejectReason::ScoreboardCapacity);
		}

	// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
	// ORIGINAL LINE: int bank = capability.Bank!.Value.Value;
		int bank = capability->Bank->Value;
		if ((state->PendingBankMask & (1 << bank)) != 0)
		{
			return Reject(Rf06MemoryShadowRejectReason::PendingBank);
		}
		if (capability->Kind == MemoryCapabilityKind::Store && (state->OutstandingStoreBankMask & (1 << bank)) != 0)
		{
			return Reject(Rf06MemoryShadowRejectReason::OutstandingStoreBank);
		}

		if (state->ConsumedMemoryByBank[bank] >= state->MemoryBankBudgets[bank])
		{
			return Reject(Rf06MemoryShadowRejectReason::BankBudget);
		}

		// The current legacy oracle applies directional budgets to Load and
		// Store, while Atomic follows the existing general-bank contour only.
		// Keeping this distinction is required for behavior-preserving shadow
		// migration; Atomic direction remains ReadWrite in the immutable ISA
		// capability and is not reinterpreted as mutable state here.
		if (capability->Kind == MemoryCapabilityKind::Load && (state->RemainingReadIssueBudget == 0 || state->ConsumedReadByBank[bank] >= state->ReadBankBudgets[bank]))
		{
			return Reject(Rf06MemoryShadowRejectReason::DirectionBudget);
		}
		if (capability->Kind == MemoryCapabilityKind::Store && (state->RemainingWriteIssueBudget == 0 || state->ConsumedWriteByBank[bank] >= state->WriteBankBudgets[bank]))
		{
			return Reject(Rf06MemoryShadowRejectReason::DirectionBudget);
		}

		return std::make_shared<Rf06MemoryShadowDecision>(true, Rf06MemoryShadowRejectReason::None);
	}

	bool Rf06MemoryShadowOracle::TryProjectLegacyCarrier(std::shared_ptr<LoadStoreMicroOp> carrier, std::shared_ptr<MemoryCapability> &capability)
	{
		ArgumentNullException::ThrowIfNull(carrier);
		capability = MemoryCapability::None;

		int bank = carrier->getMemoryBankId();
		if (static_cast<unsigned int>(bank) >= MemoryBankId::BankCount)
		{
			MemoryBankResolution nonResolvedBank = Processor::Memory == nullptr ? MemoryBankResolution::UnavailableTopology : MemoryBankResolution::InvalidGeometry;
			if (!nonResolvedBank.IsResolved())
			{
				return false;
			}
		}

		MemoryCapabilityKind kind;
		unsigned long long length;
		std::shared_ptr<LoadMicroOp> load = std::dynamic_pointer_cast<LoadMicroOp>(carrier);
		if (load != nullptr)
		{
			kind = MemoryCapabilityKind::Load;
			length = load->getSize();
		}
		else
		{
			std::shared_ptr<StoreMicroOp> store = std::dynamic_pointer_cast<StoreMicroOp>(carrier);
			if (store != nullptr)
			{
				kind = MemoryCapabilityKind::Store;
				length = store->getSize();
			}
			else
			{
				std::shared_ptr<AtomicMicroOp> atomic = std::dynamic_pointer_cast<AtomicMicroOp>(carrier);
				if (atomic != nullptr)
				{
					kind = MemoryCapabilityKind::Atomic;
					length = std::max(atomic->getSize(), static_cast<unsigned char>(4));
				}
				else
				{
					return false;
				}
			}
		}

		if (length == 0)
		{
			return false;
		}
		MemoryBankResolution bankResolution = MemoryBankResolution::Resolved(MemoryBankId(bank));

	// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
	// ORIGINAL LINE: capability = MemoryCapability.Create( kind, new[] { new FrozenMemoryRange(carrier.MemoryAddress, length) }, bankResolution.Bank!.Value);
		capability = MemoryCapability::Create(kind, std::vector<FrozenMemoryRange>{FrozenMemoryRange(carrier->getMemoryAddress(), length)}, bankResolution.Bank);
		return true;
	}

	std::shared_ptr<Rf06MemoryShadowDecision> Rf06MemoryShadowOracle::EvaluateLegacyCarrier(std::shared_ptr<LoadStoreMicroOp> carrier, std::shared_ptr<Rf06MemoryShadowState> state)
	{
		ArgumentNullException::ThrowIfNull(carrier);
		std::shared_ptr<MemoryCapability> capability;
		if (!TryProjectLegacyCarrier(carrier, capability))
		{
			return Reject(Rf06MemoryShadowRejectReason::LegacyCarrierNotRepresentable);
		}

	// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
	// ORIGINAL LINE: return EvaluateLegacyFacts(carrier, capability.Kind, capability.Bank!.Value.Value, state);
		return EvaluateLegacyFacts(carrier, capability->Kind, capability->Bank->Value, state);
	}

	std::shared_ptr<Rf06MemoryDifferentialResult> Rf06MemoryShadowOracle::Compare(std::shared_ptr<AdmissionRecord> admission, std::shared_ptr<LoadStoreMicroOp> carrier, std::shared_ptr<Rf06MemoryShadowState> state)
	{
		ArgumentNullException::ThrowIfNull(admission);
		ArgumentNullException::ThrowIfNull(carrier);
		std::shared_ptr<MemoryCapability> projected;
		if (!TryProjectLegacyCarrier(carrier, projected))
		{
			return std::make_shared<Rf06MemoryDifferentialResult>(false, false, false, L"LegacyCarrierNotRepresentable", std::make_shared<Rf06MemoryShadowDecision>(false, Rf06MemoryShadowRejectReason::LegacyCarrierNotRepresentable), EvaluateContract(admission, state));
		}

		bool staticEquivalent = AreEquivalent(projected, admission->Contract->getMemory());
	// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
	// ORIGINAL LINE: Rf06MemoryShadowDecision legacy = EvaluateLegacyFacts( carrier, projected.Kind, projected.Bank!.Value.Value, state);
		std::shared_ptr<Rf06MemoryShadowDecision> legacy = EvaluateLegacyFacts(carrier, projected->Kind, projected->Bank->Value, state);
		std::shared_ptr<Rf06MemoryShadowDecision> contract = EvaluateContract(admission, state);
		bool dynamicEquivalent = legacy == contract;
		return std::make_shared<Rf06MemoryDifferentialResult>(staticEquivalent && dynamicEquivalent, staticEquivalent, dynamicEquivalent, staticEquivalent ? (dynamicEquivalent ? L"" : L"DynamicDecision") : L"MemoryCapability", legacy, contract);
	}

	std::shared_ptr<Rf06MemoryShadowDecision> Rf06MemoryShadowOracle::EvaluateLegacyFacts(std::shared_ptr<LoadStoreMicroOp> carrier, MemoryCapabilityKind kind, int bank, std::shared_ptr<Rf06MemoryShadowState> state)
	{
		if (static_cast<unsigned int>(carrier->getVirtualThreadId()) >= 4)
		{
			return Reject(Rf06MemoryShadowRejectReason::InvalidVirtualThread);
		}
		if (state->RemainingMemoryIssueBudget == 0)
		{
			return Reject(Rf06MemoryShadowRejectReason::NoIssueBudget);
		}
		if (state->OutstandingByVirtualThread[carrier->getVirtualThreadId()] >= state->CapacityByVirtualThread[carrier->getVirtualThreadId()])
		{
			return Reject(Rf06MemoryShadowRejectReason::ScoreboardCapacity);
		}
		if ((state->PendingBankMask & (1 << bank)) != 0)
		{
			return Reject(Rf06MemoryShadowRejectReason::PendingBank);
		}
		if (std::dynamic_pointer_cast<StoreMicroOp>(carrier) != nullptr && (state->OutstandingStoreBankMask & (1 << bank)) != 0)
		{
			return Reject(Rf06MemoryShadowRejectReason::OutstandingStoreBank);
		}
		if (state->ConsumedMemoryByBank[bank] >= state->MemoryBankBudgets[bank])
		{
			return Reject(Rf06MemoryShadowRejectReason::BankBudget);
		}
		if (std::dynamic_pointer_cast<LoadMicroOp>(carrier) != nullptr && (state->RemainingReadIssueBudget == 0 || state->ConsumedReadByBank[bank] >= state->ReadBankBudgets[bank]))
		{
			return Reject(Rf06MemoryShadowRejectReason::DirectionBudget);
		}
		if (std::dynamic_pointer_cast<StoreMicroOp>(carrier) != nullptr && (state->RemainingWriteIssueBudget == 0 || state->ConsumedWriteByBank[bank] >= state->WriteBankBudgets[bank]))
		{
			return Reject(Rf06MemoryShadowRejectReason::DirectionBudget);
		}

		return std::make_shared<Rf06MemoryShadowDecision>(true, Rf06MemoryShadowRejectReason::None);
	}

	bool Rf06MemoryShadowOracle::AreEquivalent(std::shared_ptr<MemoryCapability> left, std::shared_ptr<MemoryCapability> right)
	{
		if (left->Kind != right->Kind || left->Direction != right->Direction || left->Bank != right->Bank || left->Footprint.size() != right->Footprint.size())
		{
			return false;
		}

		for (int index = 0; index < left->Footprint.size(); index++)
		{
			if (left->Footprint[index] != right->Footprint[index])
			{
				return false;
			}
		}

		return true;
	}

	std::shared_ptr<Rf06MemoryShadowDecision> Rf06MemoryShadowOracle::Reject(Rf06MemoryShadowRejectReason reason)
	{
		return std::make_shared<Rf06MemoryShadowDecision>(false, reason);
	}

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: public static Rf06MemoryFspDifferentialResult Evaluate(IReadOnlyList<Rf06MemoryFspCandidate> candidates, int ownerVirtualThreadId, byte readyVirtualThreadMask, Rf06MemoryShadowState state)
	std::shared_ptr<Rf06MemoryFspDifferentialResult> Rf06MemoryFspDifferential::Evaluate(const std::vector<std::shared_ptr<Rf06MemoryFspCandidate>> &candidates, int ownerVirtualThreadId, unsigned char readyVirtualThreadMask, std::shared_ptr<Rf06MemoryShadowState> state)
	{
		ArgumentNullException::ThrowIfNull(candidates);
		if (static_cast<unsigned int>(ownerVirtualThreadId) >= 4)
		{
			throw ArgumentOutOfRangeException(L"ownerVirtualThreadId");
		}

		int legacyPacked = 0;
		int contractPacked = 0;
		std::vector<int> divergent;
		std::shared_ptr<Rf06MemoryShadowState> legacyState = state;
		std::shared_ptr<Rf06MemoryShadowState> contractState = state;

		for (int vt = 0; vt < 4; vt++)
		{
			if (vt == ownerVirtualThreadId || (readyVirtualThreadMask & (1 << vt)) == 0)
			{
				continue;
			}

			auto candidate = std::find_if(candidates.begin(), candidates.end(), [vt](const auto &item)
			{
				return item && item->Admission && item->Admission->VirtualThreadId == vt;
			});
			if (candidate == candidates.end())
			{
				continue;
			}

			std::shared_ptr<Rf06MemoryFspCandidate> selected = *candidate;
			std::shared_ptr<Rf06MemoryShadowDecision> legacy = Rf06MemoryShadowOracle::EvaluateLegacyCarrier(selected->LegacyCarrier, legacyState);
			std::shared_ptr<Rf06MemoryShadowDecision> contract = Rf06MemoryShadowOracle::EvaluateContract(selected->Admission, contractState);
			if (legacy->IsEligible != contract->IsEligible || legacy->RejectReason != contract->RejectReason)
			{
				divergent.push_back(vt);
			}

			if (legacy->IsEligible)
			{
				legacyPacked++;
				std::shared_ptr<MemoryCapability> legacyCapability;
				if (Rf06MemoryShadowOracle::TryProjectLegacyCarrier(selected->LegacyCarrier, legacyCapability))
				{
					legacyState = legacyState->Consume(legacyCapability, vt);
				}
			}

			if (contract->IsEligible)
			{
				contractPacked++;
				contractState = contractState->Consume(selected->Admission->Contract->getMemory(), selected->Admission->VirtualThreadId);
			}
		}

		return std::make_shared<Rf06MemoryFspDifferentialResult>(legacyPacked, contractPacked, divergent.empty() && legacyPacked == contractPacked, std::move(divergent));
	}
}


