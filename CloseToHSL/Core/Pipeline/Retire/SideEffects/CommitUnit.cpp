#include "CommitUnit.h"

using namespace YAKSys_Hybrid_CPU::Core::Registers::Retire;

namespace YAKSys_Hybrid_CPU::Core::Pipeline
{

// C# TO C++ CONVERTER TASK: Throw expressions are not converted by C# to C++ Converter:
// ORIGINAL LINE: private CommitUnit(RetireCoordinator retireCoordinator)
	CommitUnit::CommitUnit(std::shared_ptr<RetireCoordinator> retireCoordinator) : _retireCoordinator((retireCoordinator != nullptr) ? retireCoordinator : throw std::invalid_argument("retireCoordinator"))
	{
	}

	std::shared_ptr<CommitUnit> CommitUnit::FromRetireCoordinator(std::shared_ptr<RetireCoordinator> retireCoordinator)
	{
		return std::shared_ptr<CommitUnit>(new CommitUnit(std::move(retireCoordinator)));
	}

	void CommitUnit::Commit(std::span<const RetireRecord> records, unsigned char vtId)
	{
		ValidateVirtualThreadId(vtId);

		for (std::size_t i = 0; i < records.size(); i++)
		{
			RetireRecord record = records[i];
			if (record.getVtId() != vtId)
			{
			throw InvalidOperationException(std::format(L"CommitUnit retire batch for VT{} cannot include record[{}] owned by VT{}.", static_cast<unsigned int>(vtId), i, static_cast<unsigned int>(record.getVtId())));
			}
		}

		_retireCoordinator->Retire(records);
	}

	void CommitUnit::ValidateVirtualThreadId(unsigned char vtId)
	{
		if (vtId >= _retireCoordinator->getVtCount())
		{
			int maxVtId = std::max(_retireCoordinator->getVtCount() - 1, 0);
			throw ArgumentOutOfRangeException(L"vtId", vtId, std::format(L"VT identifier {} is outside the range [0, {}].", static_cast<unsigned int>(vtId), maxVtId));
		}
	}
}
