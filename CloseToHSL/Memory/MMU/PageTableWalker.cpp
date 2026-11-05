#include "PageTableWalker.h"

namespace HybridCPU_ISE::CloseToHSL::Memory::MMU
{
	const PageTableWalker::WalkResult PageTableWalker::WalkResult::NoEvent(false, false, -1, 0, 0, 0, 0);


	unsigned long long PageTableWalker::getWalksCompleted() const
	{
		return WalksCompleted;
	}

	void PageTableWalker::setWalksCompleted(unsigned long long value)
	{
		WalksCompleted = value;
	}

	unsigned long long PageTableWalker::getWalksFaulted() const
	{
		return WalksFaulted;
	}

	void PageTableWalker::setWalksFaulted(unsigned long long value)
	{
		WalksFaulted = value;
	}

	unsigned long long PageTableWalker::getTotalWalkCycles() const
	{
		return TotalWalkCycles;
	}

	void PageTableWalker::setTotalWalkCycles(unsigned long long value)
	{
		TotalWalkCycles = value;
	}

	bool PageTableWalker::isBusy() const
	{
		return _state != PtwState::Idle;
	}

	PageTableWalker::PtwState PageTableWalker::getCurrentState() const
	{
		return _state;
	}

	int PageTableWalker::getStalledThreadId() const
	{
		return isBusy() ? _currentRequest.ThreadId : -1;
	}

	int PageTableWalker::getPendingCount() const
	{
		return _pendingCount;
	}

	PageTableWalker::WalkResult::WalkResult(bool done, bool faulted, int threadId, unsigned long long physAddr, unsigned char perms, unsigned long long virtualAddress, int domainId) : Done(done), Faulted(faulted), ThreadId(threadId), PhysicalAddress(physAddr), Permissions(perms), VirtualAddress(virtualAddress), DomainId(domainId)
	{
	}

	bool PageTableWalker::SubmitWalk(unsigned long long virtualAddress, int threadId, int domainId, bool isWrite)
	{
		if (_state == PtwState::Idle)
		{
			_currentRequest = {.VirtualAddress = virtualAddress, .ThreadId = threadId, .DomainId = domainId, .IsWrite = isWrite, .Valid = true};
			BeginWalk();
			return true;
		}

		// Enqueue if space available
		if (_pendingCount < MAX_PENDING_WALKS)
		{
			int tail = (_pendingHead + _pendingCount) % MAX_PENDING_WALKS;
			WalkRequest temp_var = WalkRequest();
			temp_var.VirtualAddress = virtualAddress;
			temp_var.ThreadId = threadId;
			temp_var.DomainId = domainId;
			temp_var.IsWrite = isWrite;
			temp_var.Valid = true;
			_pendingQueue[tail] = temp_var;
			_pendingCount++;
			return true;
		}

		return false; // Queue full — caller must retry next cycle
	}

	PageTableWalker::WalkResult PageTableWalker::AdvanceCycle(std::shared_ptr<std::vector<std::shared_ptr<std::vector<unsigned long long>>>> pageDirectory)
	{
		if (_state != PtwState::Idle)
		{
			setTotalWalkCycles(getTotalWalkCycles() + 1);
		}

		switch (_state)
		{
			case PtwState::Idle:
				return TryDequeue();

			case PtwState::ReadPDE:
				// Issue DMA read for PDE (dedicated PTW port)
				_state = PtwState::WaitPDE;
				_waitCycles = 0;
				return WalkResult::NoEvent;

			case PtwState::WaitPDE:
				_waitCycles++;
				if (_waitCycles >= MEMORY_READ_LATENCY)
				{
					// Read PDE from page directory
					unsigned int dirIndex = static_cast<unsigned int>((_currentRequest.VirtualAddress >> 22) & 0x3FF);

					if (pageDirectory == nullptr || dirIndex >= pageDirectory->size() || (*pageDirectory)[dirIndex] == nullptr)
					{
						// PDE not present — fault
						_state = PtwState::Fault;
						return WalkResult::NoEvent;
					}

					// PDE is the page table base for this directory entry.
					// In our 2-level model the page directory array entry at dirIndex
					// IS the page table array — we move to PTE read.
					_state = PtwState::ReadPTE;
					_waitCycles = 0;
				}
				return WalkResult::NoEvent;

			case PtwState::ReadPTE:
				// Issue DMA read for PTE (second level)
				_state = PtwState::WaitPTE;
				_waitCycles = 0;
				return WalkResult::NoEvent;

			case PtwState::WaitPTE:
				_waitCycles++;
				if (_waitCycles >= MEMORY_READ_LATENCY)
				{
					// Read PTE from page table
					unsigned int dirIdx = static_cast<unsigned int>((_currentRequest.VirtualAddress >> 22) & 0x3FF);
					unsigned int tblIdx = static_cast<unsigned int>((_currentRequest.VirtualAddress >> 12) & 0x3FF);

					if (pageDirectory == nullptr || dirIdx >= pageDirectory->size() || (*pageDirectory)[dirIdx] == nullptr || tblIdx >= (*pageDirectory)[dirIdx]->size())
					{
						_state = PtwState::Fault;
						return WalkResult::NoEvent;
					}

					unsigned long long pte = (*(*pageDirectory)[dirIdx])[tblIdx];

					// Check present bit
					if ((pte & 0x1ULL) == 0)
					{
						_state = PtwState::Fault;
						return WalkResult::NoEvent;
					}

					// Keep PTW permission truth aligned with the synchronous IOMMU walk.
					if (_currentRequest.IsWrite && (pte & 0x4ULL) == 0)
					{
						_state = PtwState::Fault;
						return WalkResult::NoEvent;
					}

					if (!_currentRequest.IsWrite && (pte & 0x2ULL) == 0)
					{
						_state = PtwState::Fault;
						return WalkResult::NoEvent;
					}

					// Successful — extract physical address and permissions
					unsigned long long ppn = pte & 0xFFFFFFFFFFFFF000ULL;
					unsigned long long offset = _currentRequest.VirtualAddress & 0xFFF;
					unsigned long long physAddr = ppn | offset;
					unsigned char perms = static_cast<unsigned char>((pte >> 1) & 0x06);

					setWalksCompleted(getWalksCompleted() + 1);
					_state = PtwState::Idle;

					WalkResult result = WalkResult(true, false, _currentRequest.ThreadId, physAddr, perms, _currentRequest.VirtualAddress, _currentRequest.DomainId);

					// Immediately try to start next pending walk
					TryDequeue();

					return result;
				}
				return WalkResult::NoEvent;

			case PtwState::Complete:
				// Should not normally reach here (Complete is handled inline in WaitPTE),
				// but included for FSM completeness.
				_state = PtwState::Idle;
				return WalkResult::NoEvent;

			case PtwState::Fault:
			{
				setWalksFaulted(getWalksFaulted() + 1);
				int faultedThread = _currentRequest.ThreadId;
				_state = PtwState::Idle;

					WalkResult result = WalkResult(true, true, faultedThread, 0, 0, _currentRequest.VirtualAddress, _currentRequest.DomainId);

				// Try to start next pending walk
				TryDequeue();

				return result;
			}
		}

		return WalkResult::NoEvent;
	}

	void PageTableWalker::Reset()
	{
		_state = PtwState::Idle;
		_currentRequest = HybridCPU_ISE::CloseToHSL::Memory::MMU::PageTableWalker::WalkRequest();
		_pdeAddress = 0;
		_pdeValue = 0;
		_pteAddress = 0;
		_waitCycles = 0;
		_pendingHead = 0;
		_pendingCount = 0;
		_pendingQueue = HybridCPU_ISE::CloseToHSL::Memory::MMU::PageTableWalker::PendingWalkArray();
		setWalksCompleted(0);
		setWalksFaulted(0);
		setTotalWalkCycles(0);
	}

	void PageTableWalker::BeginWalk()
	{
		_state = PtwState::ReadPDE;
		_waitCycles = 0;
	}

	PageTableWalker::WalkResult PageTableWalker::TryDequeue()
	{
		if (_pendingCount > 0 && _state == PtwState::Idle)
		{
			_currentRequest = _pendingQueue[_pendingHead];
			_pendingQueue[_pendingHead].Valid = false;
			_pendingHead = (_pendingHead + 1) % MAX_PENDING_WALKS;
			_pendingCount--;
			BeginWalk();
		}

		return WalkResult::NoEvent;
	}
}
