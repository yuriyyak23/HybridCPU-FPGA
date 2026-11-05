#pragma once

#include "../MicroOps/Memory/MicroOp.LoadStore.h"
#include "../../Decoder/Rf06ExecutionContracts.h"
#include <vector>
#include <cmath>
#include <optional>
#include "tangible_exception_helper.h"
#include <memory>

using namespace YAKSys_Hybrid_CPU::Core::Decoder;

namespace YAKSys_Hybrid_CPU::Core
{

	/// <summary>
	/// Read-only dynamic memory contour sampled by ShadowOracle/FSP.  The sample is
	/// deliberately separate from <see cref="ExecutionContract"/>: it describes
	/// current pressure, not an operation capability or an execution outcome.
	/// </summary>
// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct Rf06MemoryShadowState
//	{
//		private Rf06MemoryShadowState(byte remainingMemoryIssueBudget, byte remainingReadIssueBudget, byte remainingWriteIssueBudget, ImmutableArray<byte> memoryBankBudgets, ImmutableArray<byte> readBankBudgets, ImmutableArray<byte> writeBankBudgets, ImmutableArray<byte> consumedMemoryByBank, ImmutableArray<byte> consumedReadByBank, ImmutableArray<byte> consumedWriteByBank, ImmutableArray<byte> outstandingByVirtualThread, ImmutableArray<byte> capacityByVirtualThread, ushort pendingBankMask, ushort outstandingStoreBankMask)
//		{
//			RemainingMemoryIssueBudget = remainingMemoryIssueBudget;
//			RemainingReadIssueBudget = remainingReadIssueBudget;
//			RemainingWriteIssueBudget = remainingWriteIssueBudget;
//			MemoryBankBudgets = memoryBankBudgets;
//			ReadBankBudgets = readBankBudgets;
//			WriteBankBudgets = writeBankBudgets;
//			ConsumedMemoryByBank = consumedMemoryByBank;
//			ConsumedReadByBank = consumedReadByBank;
//			ConsumedWriteByBank = consumedWriteByBank;
//			OutstandingByVirtualThread = outstandingByVirtualThread;
//			CapacityByVirtualThread = capacityByVirtualThread;
//			PendingBankMask = pendingBankMask;
//			OutstandingStoreBankMask = outstandingStoreBankMask;
//		}
//
//		public byte RemainingMemoryIssueBudget
//		{
//			get;
//		}
//		public byte RemainingReadIssueBudget
//		{
//			get;
//		}
//		public byte RemainingWriteIssueBudget
//		{
//			get;
//		}
//		public ImmutableArray<byte> MemoryBankBudgets
//		{
//			get;
//		}
//		public ImmutableArray<byte> ReadBankBudgets
//		{
//			get;
//		}
//		public ImmutableArray<byte> WriteBankBudgets
//		{
//			get;
//		}
//		public ImmutableArray<byte> ConsumedMemoryByBank
//		{
//			get;
//		}
//		public ImmutableArray<byte> ConsumedReadByBank
//		{
//			get;
//		}
//		public ImmutableArray<byte> ConsumedWriteByBank
//		{
//			get;
//		}
//		public ImmutableArray<byte> OutstandingByVirtualThread
//		{
//			get;
//		}
//		public ImmutableArray<byte> CapacityByVirtualThread
//		{
//			get;
//		}
//		public ushort PendingBankMask
//		{
//			get;
//		}
//		public ushort OutstandingStoreBankMask
//		{
//			get;
//		}
//
//		public static Rf06MemoryShadowState Create(byte remainingMemoryIssueBudget, byte remainingReadIssueBudget, byte remainingWriteIssueBudget, IEnumerable<byte> memoryBankBudgets, IEnumerable<byte> readBankBudgets, IEnumerable<byte> writeBankBudgets, IEnumerable<byte> consumedMemoryByBank, IEnumerable<byte> consumedReadByBank, IEnumerable<byte> consumedWriteByBank, IEnumerable<byte> outstandingByVirtualThread, IEnumerable<byte> capacityByVirtualThread, ushort pendingBankMask = 0, ushort outstandingStoreBankMask = 0)
//		{
//			ImmutableArray<byte> memory = Freeze(memoryBankBudgets, MemoryBankId.BankCount, nameof(memoryBankBudgets));
//			ImmutableArray<byte> reads = Freeze(readBankBudgets, MemoryBankId.BankCount, nameof(readBankBudgets));
//			ImmutableArray<byte> writes = Freeze(writeBankBudgets, MemoryBankId.BankCount, nameof(writeBankBudgets));
//			ImmutableArray<byte> consumedMemory = Freeze(consumedMemoryByBank, MemoryBankId.BankCount, nameof(consumedMemoryByBank));
//			ImmutableArray<byte> consumedReads = Freeze(consumedReadByBank, MemoryBankId.BankCount, nameof(consumedReadByBank));
//			ImmutableArray<byte> consumedWrites = Freeze(consumedWriteByBank, MemoryBankId.BankCount, nameof(consumedWriteByBank));
//			ImmutableArray<byte> outstanding = Freeze(outstandingByVirtualThread, 4, nameof(outstandingByVirtualThread));
//			ImmutableArray<byte> capacity = Freeze(capacityByVirtualThread, 4, nameof(capacityByVirtualThread));
//
//			for (int bank = 0; bank < MemoryBankId.BankCount; bank++)
//			{
//				if (consumedMemory[bank] > memory[bank] || consumedReads[bank] > reads[bank] || consumedWrites[bank] > writes[bank])
//				{
//					throw new ArgumentException("A dynamic memory sample cannot exceed a bank budget.", nameof(consumedMemoryByBank));
//				}
//			}
//
//			for (int vt = 0; vt < 4; vt++)
//			{
//				if (outstanding[vt] > capacity[vt])
//				{
//					throw new ArgumentException("A dynamic memory sample cannot exceed a VT scoreboard capacity.", nameof(outstandingByVirtualThread));
//				}
//			}
//
//			return new Rf06MemoryShadowState(remainingMemoryIssueBudget, remainingReadIssueBudget, remainingWriteIssueBudget, memory, reads, writes, consumedMemory, consumedReads, consumedWrites, outstanding, capacity, pendingBankMask, outstandingStoreBankMask);
//		}
//
//		internal Rf06MemoryShadowState Consume(MemoryCapability capability, int virtualThreadId)
//		{
//			if (capability.Kind == MemoryCapabilityKind.NoMemory)
//				return this;
//
//// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
//// ORIGINAL LINE: int bank = capability.Bank!.Value.Value;
//			int bank = capability.Bank.Value.Value;
//			byte[] memory = ConsumedMemoryByBank.ToArray();
//			byte[] reads = ConsumedReadByBank.ToArray();
//			byte[] writes = ConsumedWriteByBank.ToArray();
//			byte[] outstanding = OutstandingByVirtualThread.ToArray();
//			memory[bank]++;
//			outstanding[virtualThreadId]++;
//
//			byte remainingRead = RemainingReadIssueBudget;
//			byte remainingWrite = RemainingWriteIssueBudget;
//			if (capability.Kind == MemoryCapabilityKind.Load)
//			{
//				reads[bank]++;
//				remainingRead--;
//			}
//			else if (capability.Kind == MemoryCapabilityKind.Store)
//			{
//				writes[bank]++;
//				remainingWrite--;
//			}
//
//			ushort stores = OutstandingStoreBankMask;
//			if (capability.Kind == MemoryCapabilityKind.Store)
//				stores |= (ushort)(1 << bank);
//
//			return Create(checked((byte)(RemainingMemoryIssueBudget - 1)), remainingRead, remainingWrite, MemoryBankBudgets, ReadBankBudgets, WriteBankBudgets, memory, reads, writes, outstanding, CapacityByVirtualThread, PendingBankMask, stores);
//		}
//
//		private static ImmutableArray<byte> Freeze(IEnumerable<byte> values, int expectedLength, string parameterName)
//		{
//			ArgumentNullException.ThrowIfNull(values, parameterName);
//			byte[] copy = values.ToArray();
//			if (copy.Length != expectedLength)
//				throw new ArgumentException(string.Format("Expected exactly {0} values.", expectedLength), parameterName);
//			return ImmutableArray.Create(copy);
//		}
//	}

class Rf06MemoryShadowState final
{
public:
	unsigned char RemainingMemoryIssueBudget, RemainingReadIssueBudget, RemainingWriteIssueBudget;
	std::vector<unsigned char> MemoryBankBudgets, ReadBankBudgets, WriteBankBudgets;
	std::vector<unsigned char> ConsumedMemoryByBank, ConsumedReadByBank, ConsumedWriteByBank;
	std::vector<unsigned char> OutstandingByVirtualThread, CapacityByVirtualThread;
	unsigned short PendingBankMask, OutstandingStoreBankMask;
	static std::shared_ptr<Rf06MemoryShadowState> Create(unsigned char m, unsigned char r, unsigned char w, const std::vector<unsigned char> &mb, const std::vector<unsigned char> &rb, const std::vector<unsigned char> &wb, const std::vector<unsigned char> &cm, const std::vector<unsigned char> &cr, const std::vector<unsigned char> &cw, const std::vector<unsigned char> &ov, const std::vector<unsigned char> &cv, unsigned short pending = 0, unsigned short stores = 0);
	std::shared_ptr<Rf06MemoryShadowState> Consume(std::shared_ptr<MemoryCapability> capability, int virtualThreadId) const;
	Rf06MemoryShadowState(unsigned char m, unsigned char r, unsigned char w, std::vector<unsigned char> mb, std::vector<unsigned char> rb, std::vector<unsigned char> wb, std::vector<unsigned char> cm, std::vector<unsigned char> cr, std::vector<unsigned char> cw, std::vector<unsigned char> ov, std::vector<unsigned char> cv, unsigned short pending, unsigned short stores) : RemainingMemoryIssueBudget(m), RemainingReadIssueBudget(r), RemainingWriteIssueBudget(w), MemoryBankBudgets(std::move(mb)), ReadBankBudgets(std::move(rb)), WriteBankBudgets(std::move(wb)), ConsumedMemoryByBank(std::move(cm)), ConsumedReadByBank(std::move(cr)), ConsumedWriteByBank(std::move(cw)), OutstandingByVirtualThread(std::move(ov)), CapacityByVirtualThread(std::move(cv)), PendingBankMask(pending), OutstandingStoreBankMask(stores) {}
};

enum class Rf06MemoryShadowRejectReason : unsigned char
	{
		None = 0,
		NotMemoryCapability,
		InvalidVirtualThread,
		NoIssueBudget,
		ScoreboardCapacity,
		PendingBank,
		OutstandingStoreBank,
		BankBudget,
		DirectionBudget,
		LegacyCarrierNotRepresentable
	};

struct Rf06MemoryShadowDecision { bool IsEligible; Rf06MemoryShadowRejectReason RejectReason; Rf06MemoryShadowDecision(bool eligible, Rf06MemoryShadowRejectReason reason) : IsEligible(eligible), RejectReason(reason) {} };
struct Rf06MemoryDifferentialResult { bool IsEquivalent, StaticEquivalent, DynamicEquivalent; std::wstring MismatchField; std::shared_ptr<Rf06MemoryShadowDecision> LegacyDecision, ContractDecision; Rf06MemoryDifferentialResult(bool e, bool s, bool d, std::wstring f, std::shared_ptr<Rf06MemoryShadowDecision> l, std::shared_ptr<Rf06MemoryShadowDecision> c) : IsEquivalent(e), StaticEquivalent(s), DynamicEquivalent(d), MismatchField(std::move(f)), LegacyDecision(std::move(l)), ContractDecision(std::move(c)) {} };
struct Rf06MemoryFspCandidate { std::shared_ptr<AdmissionRecord> Admission; std::shared_ptr<LoadStoreMicroOp> LegacyCarrier; };
struct Rf06MemoryFspDifferentialResult { int LegacyPacked, ContractPacked; bool AreEquivalent; std::vector<int> DivergentVirtualThreads; Rf06MemoryFspDifferentialResult(int l, int c, bool e, std::vector<int> d) : LegacyPacked(l), ContractPacked(c), AreEquivalent(e), DivergentVirtualThreads(std::move(d)) {} };

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct Rf06MemoryShadowDecision(bool IsEligible, Rf06MemoryShadowRejectReason RejectReason);

	/// <summary>
	/// Immutable memory-contract evaluator used by the RF-06.4b differential
	/// contour. It mirrors only the existing ShadowOracle memory gates. It does
	/// not allocate a scheduler operation, mutate a carrier, or own timing/MSHR
	/// state.
	/// </summary>
	class Rf06MemoryShadowOracle final
	{
	public:
		static std::shared_ptr<Rf06MemoryShadowDecision> EvaluateContract(std::shared_ptr<AdmissionRecord> admission, std::shared_ptr<Rf06MemoryShadowState> state);

		static std::shared_ptr<Rf06MemoryShadowDecision> EvaluateContract(std::shared_ptr<MemoryCapability> capability, int virtualThreadId, std::shared_ptr<Rf06MemoryShadowState> state);

		/// <summary>
		/// Compatibility-boundary projection of the existing carrier facts. This
		/// is the only place where ShadowOracle/FSP reads legacy memory carrier
		/// type/address/size/bank fields during RF-06.4b.
		/// </summary>
		static bool TryProjectLegacyCarrier(std::shared_ptr<LoadStoreMicroOp> carrier, std::shared_ptr<MemoryCapability> &capability);

		static std::shared_ptr<Rf06MemoryShadowDecision> EvaluateLegacyCarrier(std::shared_ptr<LoadStoreMicroOp> carrier, std::shared_ptr<Rf06MemoryShadowState> state);

		static std::shared_ptr<Rf06MemoryDifferentialResult> Compare(std::shared_ptr<AdmissionRecord> admission, std::shared_ptr<LoadStoreMicroOp> carrier, std::shared_ptr<Rf06MemoryShadowState> state);

	private:
		static std::shared_ptr<Rf06MemoryShadowDecision> EvaluateLegacyFacts(std::shared_ptr<LoadStoreMicroOp> carrier, MemoryCapabilityKind kind, int bank, std::shared_ptr<Rf06MemoryShadowState> state);

		static bool AreEquivalent(std::shared_ptr<MemoryCapability> left, std::shared_ptr<MemoryCapability> right);

		static std::shared_ptr<Rf06MemoryShadowDecision> Reject(Rf06MemoryShadowRejectReason reason);
	};

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct Rf06MemoryDifferentialResult(bool IsEquivalent, bool StaticEquivalent, bool DynamicEquivalent, string MismatchField, Rf06MemoryShadowDecision LegacyDecision, Rf06MemoryShadowDecision ContractDecision);

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct Rf06MemoryFspCandidate(AdmissionRecord Admission, LoadStoreMicroOp LegacyCarrier);

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct Rf06MemoryFspDifferentialResult(int LegacyPacked, int ContractPacked, bool AreEquivalent, ImmutableArray<int> DivergentVirtualThreads);

	/// <summary>
	/// Four-way SMT/FSP differential contour. It models the existing SCHED2
	/// ascending VT nomination order and owner exclusion. It intentionally does not
	/// issue operation IDs or alter the scheduler's nomination ports.
	/// </summary>
	class Rf06MemoryFspDifferential final
	{
	public:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: public static Rf06MemoryFspDifferentialResult Evaluate(IReadOnlyList<Rf06MemoryFspCandidate> candidates, int ownerVirtualThreadId, byte readyVirtualThreadMask, Rf06MemoryShadowState state)
		static std::shared_ptr<Rf06MemoryFspDifferentialResult> Evaluate(const std::vector<std::shared_ptr<Rf06MemoryFspCandidate>> &candidates, int ownerVirtualThreadId, unsigned char readyVirtualThreadMask, std::shared_ptr<Rf06MemoryShadowState> state);
	};

}
