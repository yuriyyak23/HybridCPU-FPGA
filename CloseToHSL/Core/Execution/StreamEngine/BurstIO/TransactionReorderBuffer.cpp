#include "TransactionReorderBuffer.h"

namespace YAKSys_Hybrid_CPU::Execution
{

	unsigned long long TransactionReorderBuffer::getTotalTransactions() const
	{
		return TotalTransactions;
	}

	void TransactionReorderBuffer::setTotalTransactions(unsigned long long value)
	{
		TotalTransactions = value;
	}

	unsigned long long TransactionReorderBuffer::getReorderedCompletions() const
	{
		return ReorderedCompletions;
	}

	void TransactionReorderBuffer::setReorderedCompletions(unsigned long long value)
	{
		ReorderedCompletions = value;
	}

	unsigned long long TransactionReorderBuffer::getBankConflictStalls() const
	{
		return BankConflictStalls;
	}

	void TransactionReorderBuffer::setBankConflictStalls(unsigned long long value)
	{
		BankConflictStalls = value;
	}

	TransactionReorderBuffer::TransactionReorderBuffer(int _) : _entries(std::vector<TrbEntry>(TRB_DEPTH))
	{
		_nextTransactionId = 0;
		_head = 0;
		_tail = 0;
		_count = 0;
		setTotalTransactions(0);
		setReorderedCompletions(0);
		setBankConflictStalls(0);
	}

	int TransactionReorderBuffer::Allocate(int threadId, unsigned long long address, int size, bool isRead, int targetBank, unsigned long long currentCycle)
	{
		if (_count >= TRB_DEPTH)
		{
			return -1;
		}

		int idx = _tail;
		TrbEntry temp_var = TrbEntry();
		temp_var.TransactionId = _nextTransactionId++;
		temp_var.ThreadId = threadId;
		temp_var.Address = address;
		temp_var.Size = size;
		temp_var.IsRead = isRead;
		temp_var.TargetBank = targetBank;
		temp_var.IssueCycle = currentCycle;
		temp_var.Valid = true;
		temp_var.Complete = false;
		temp_var.CompletionSlot = -1;
		_entries[idx] = temp_var;

		_tail = (_tail + 1) % TRB_DEPTH;
		_count++;
		setTotalTransactions(getTotalTransactions() + 1);

		return idx;
	}

	void TransactionReorderBuffer::Complete(int entryIndex, unsigned long long currentCycle, int completionSlot)
	{
		if (static_cast<unsigned int>(entryIndex) >= TRB_DEPTH)
		{
			return;
		}

// C# TO C++ CONVERTER TASK: 'ref locals' are not converted by C# to C++ Converter:
// ORIGINAL LINE: ref TrbEntry entry = ref _entries[entryIndex];
		TrbEntry& entry = _entries[entryIndex];
		if (!entry.Valid)
		{
			return;
		}

		entry.Complete = true;
		entry.CompleteCycle = currentCycle;
		entry.CompletionSlot = completionSlot;

		if (entryIndex != _head)
		{
			setReorderedCompletions(getReorderedCompletions() + 1);
		}
	}

	bool TransactionReorderBuffer::TryRetire(TrbEntry &retired)
	{
		retired = YAKSys_Hybrid_CPU::Execution::TransactionReorderBuffer::TrbEntry();
		if (_count == 0)
		{
			return false;
		}

// C# TO C++ CONVERTER TASK: 'ref locals' are not converted by C# to C++ Converter:
// ORIGINAL LINE: ref TrbEntry head = ref _entries[_head];
		TrbEntry& head = _entries[_head];
		if (!head.Valid || !head.Complete)
		{
			return false;
		}

		retired = head;
		head.Valid = false;
		head.Complete = false;

		_head = (_head + 1) % TRB_DEPTH;
		_count--;
		return true;
	}

	    int TransactionReorderBuffer::FindNextIssuable(std::span<const bool> bankBusy) { return FindNextIssuableImpl(bankBusy); }
    int TransactionReorderBuffer::FindNextIssuable(const std::vector<bool>& bankBusy) { return FindNextIssuableImpl(bankBusy); }
    template<class Flags>
    int TransactionReorderBuffer::FindNextIssuableImpl(const Flags& bankBusy)
	{
		for (int i = 0; i < _count; i++)
		{
			int idx = (_head + i) % TRB_DEPTH;
// C# TO C++ CONVERTER TASK: 'ref locals' are not converted by C# to C++ Converter:
// ORIGINAL LINE: ref TrbEntry entry = ref _entries[idx];
			TrbEntry entry = _entries[idx];

			if (!entry.Valid || entry.Complete)
			{
				continue;
			}

			if (static_cast<unsigned int>(entry.TargetBank) < static_cast<unsigned int>(bankBusy.size()) && bankBusy[entry.TargetBank])
			{
				continue;
			}

			return idx;
		}

		if (_count > 0)
		{
			setBankConflictStalls(getBankConflictStalls() + 1);
		}

		return -1;
	}

	bool TransactionReorderBuffer::getHasSpace() const
	{
		return _count < TRB_DEPTH;
	}

	int TransactionReorderBuffer::getOutstandingCount() const
	{
		return _count;
	}

	TransactionReorderBuffer::TrbEntry TransactionReorderBuffer::GetEntry(int index) const
	{
		if (static_cast<unsigned int>(index) >= TRB_DEPTH)
		{
			return YAKSys_Hybrid_CPU::Execution::TransactionReorderBuffer::TrbEntry();
		}
		return _entries[index];
	}
}
