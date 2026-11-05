#include "LoopBuffer.h"
#include "../../MicroOps/Vector/VectorMicroOps.h"

namespace YAKSys_Hybrid_CPU::Core
{

	int LoopBuffer::getValidSlotCount() const
	{
		return ValidSlotCount;
	}

	void LoopBuffer::setValidSlotCount(int value)
	{
		ValidSlotCount = value;
	}

	LoopBuffer::BufferState LoopBuffer::getState() const
	{
		return State;
	}

	void LoopBuffer::setState(BufferState value)
	{
		State = value;
	}

	unsigned long long LoopBuffer::getCachedPC() const
	{
		return CachedPC;
	}

	void LoopBuffer::setCachedPC(unsigned long long value)
	{
		CachedPC = value;
	}

	unsigned long long LoopBuffer::getRemainingIterations() const
	{
		return RemainingIterations;
	}

	void LoopBuffer::setRemainingIterations(unsigned long long value)
	{
		RemainingIterations = value;
	}

	unsigned long long LoopBuffer::getTotalReplays() const
	{
		return TotalReplays;
	}

	void LoopBuffer::setTotalReplays(unsigned long long value)
	{
		TotalReplays = value;
	}

	unsigned long long LoopBuffer::getFetchSavings() const
	{
		return FetchSavings;
	}

	void LoopBuffer::setFetchSavings(unsigned long long value)
	{
		FetchSavings = value;
	}

	unsigned long long LoopBuffer::getReplayEpochCount() const
	{
		return ReplayEpochCount;
	}

	void LoopBuffer::setReplayEpochCount(unsigned long long value)
	{
		ReplayEpochCount = value;
	}

	unsigned long long LoopBuffer::getTotalEpochLength() const
	{
		return TotalEpochLength;
	}

	void LoopBuffer::setTotalEpochLength(unsigned long long value)
	{
		TotalEpochLength = value;
	}

	unsigned long long LoopBuffer::getStableDonorSlotSamples() const
	{
		return StableDonorSlotSamples;
	}

	void LoopBuffer::setStableDonorSlotSamples(unsigned long long value)
	{
		StableDonorSlotSamples = value;
	}

	unsigned long long LoopBuffer::getTotalReplaySlotSamples() const
	{
		return TotalReplaySlotSamples;
	}

	void LoopBuffer::setTotalReplaySlotSamples(unsigned long long value)
	{
		TotalReplaySlotSamples = value;
	}

	unsigned long long LoopBuffer::getDeterministicTransitionCount() const
	{
		return DeterministicTransitionCount;
	}

	void LoopBuffer::setDeterministicTransitionCount(unsigned long long value)
	{
		DeterministicTransitionCount = value;
	}

	void LoopBuffer::Initialize()
	{
	#if defined(TESTING)
		_slots = YAKSys_Hybrid_CPU::Core::LoopBuffer::MicroOpSlotBuffer();
		_slotValid = YAKSys_Hybrid_CPU::Core::LoopBuffer::SlotValidBuffer();
	#endif
		_replayEntry = nullptr;
		setState(BufferState::Empty);
		setCachedPC(0);
		setRemainingIterations(0);
		setValidSlotCount(0);
		setTotalReplays(0);
		setFetchSavings(0);
		setReplayEpochCount(0);
		setTotalEpochLength(0);
		setStableDonorSlotSamples(0);
		setTotalReplaySlotSamples(0);
		setDeterministicTransitionCount(0);
		_currentEpochId = 0;
		_currentEpochReplayCount = 0;
		_currentEpochLength = 0;
		_stableDonorMask = 0;
		_lastInvalidationReason = ReplayPhaseInvalidationReason::None;
		_loadedDecodeContext = std::nullopt;
		_aluDonorCapacity = 0;
		_lsuDonorCapacity = 0;
		_dmaStreamDonorCapacity = 0;
		_branchControlDonorCapacity = 0;
		_systemSingletonDonorCapacity = 0;
	}

#if defined(TESTING)
	void LoopBuffer::BeginLoad(unsigned long long pc, unsigned long long totalIterations, std::optional<std::shared_ptr<Decoder::CanonicalDecodeContext>> &decodeContext)
	{
		if (totalIterations <= 1)
		{
			return; // No point caching a single iteration
		}

		setCachedPC(pc);
		setRemainingIterations(totalIterations);
		setValidSlotCount(0);
		setState(BufferState::Loading);
		_currentEpochReplayCount = 0;
		_currentEpochLength = 0;
		_stableDonorMask = 0;
		_lastInvalidationReason = ReplayPhaseInvalidationReason::None;
		_loadedDecodeContext = decodeContext;
		_replayEntry = nullptr;

		for (int i = 0; i < MAX_SLOTS; i++)
		{
			_slots[i] = nullptr;
			_slotValid[i] = false;
		}
	}
#endif

#if defined(TESTING)
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: [MethodImpl(MethodImplOptions.AggressiveInlining)] public void StoreSlot(int slotIndex, MicroOp? op)
	void LoopBuffer::StoreSlot(int slotIndex, std::shared_ptr<MicroOp> op)
	{
		if (getState() != BufferState::Loading)
		{
			return;
		}

		if (static_cast<unsigned int>(slotIndex) >= MAX_SLOTS)
		{
			return;
		}

		_slots[slotIndex] = op;
		_slotValid[slotIndex] = op != nullptr;
		if (op != nullptr)
		{
			setValidSlotCount(getValidSlotCount() + 1);
		}
	}
#endif

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: [MethodImpl(MethodImplOptions.AggressiveInlining)] public void BeginSemanticLoad(ulong pc, ulong totalIterations, Decoder.ReplayEntry replayEntry, System.Collections.Generic.IReadOnlyList<MicroOp?> liveCarriers, Decoder.CanonicalDecodeContext decodeContext)
	void LoopBuffer::BeginSemanticLoad(unsigned long long pc, unsigned long long totalIterations, std::shared_ptr<Decoder::ReplayEntry> replayEntry, const std::vector<std::shared_ptr<MicroOp>> &liveCarriers, std::shared_ptr<Decoder::CanonicalDecodeContext> decodeContext)
	{
		ArgumentNullException::ThrowIfNull(replayEntry);
		ArgumentNullException::ThrowIfNull(liveCarriers);
		ArgumentNullException::ThrowIfNull(decodeContext);
		if (totalIterations <= 1)
		{
			return;
		}
		if (!replayEntry->HasValidFrozenContent() || !replayEntry->getCanonicalBundle()->isReplayEligible() || replayEntry->getCanonicalBundle()->getBundleAddress() != pc || replayEntry->getCanonicalBundle()->getSlots().size() != MAX_SLOTS || liveCarriers.size() != MAX_SLOTS)
		{
			throw InvalidOperationException(L"Production loop replay requires one valid eight-slot immutable semantic entry.");
		}

		setCachedPC(pc);
		setRemainingIterations(totalIterations);
		setValidSlotCount(0);
		setState(BufferState::Loading);
		_currentEpochReplayCount = 0;
		_currentEpochLength = 0;
		_stableDonorMask = ComputeStableDonorMask(liveCarriers);
		_lastInvalidationReason = ReplayPhaseInvalidationReason::None;
		_loadedDecodeContext = decodeContext;
		_replayEntry = replayEntry;
		for (int slotIndex = 0; slotIndex < MAX_SLOTS; slotIndex++)
		{
			if (replayEntry->getCanonicalBundle()->getSlots()[slotIndex]->IsOccupied)
			{
				setValidSlotCount(getValidSlotCount() + 1);
			}
		}
	}

	void LoopBuffer::CommitLoad()
	{
		if (getState() != BufferState::Loading)
		{
			return;
		}

		_currentEpochId++;
		_currentEpochReplayCount = 0;
		_currentEpochLength = getRemainingIterations();
	#if defined(TESTING)
		if (_replayEntry == nullptr)
		{
			_stableDonorMask = ComputeStableDonorMask();
		}
	#endif
		ComputeClassDonorCapacity();
		setReplayEpochCount(getReplayEpochCount() + 1);
		setTotalEpochLength(getTotalEpochLength() + _currentEpochLength);
		setDeterministicTransitionCount(getDeterministicTransitionCount() + 1);
		setState(BufferState::Active);
	}

#if defined(TESTING)
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: [MethodImpl(MethodImplOptions.AggressiveInlining)] public bool TryReplay(ulong requestedPC, MicroOp?[] target, System.Nullable<Decoder.CanonicalDecodeContext> currentDecodeContext = null)
	bool LoopBuffer::TryReplay(unsigned long long requestedPC, std::vector<std::shared_ptr<MicroOp>> &target, std::optional<std::shared_ptr<Decoder::CanonicalDecodeContext>> &currentDecodeContext)
	{
		if (getState() != BufferState::Active && getState() != BufferState::Draining)
		{
			return false;
		}

		if (requestedPC != getCachedPC())
		{
			// PC mismatch — invalidate buffer (branch out of loop)
			Invalidate(ReplayPhaseInvalidationReason::PcMismatch);
			return false;
		}

	// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
	// ORIGINAL LINE: if (_loadedDecodeContext != null && (_loadedDecodeContext != currentDecodeContext || !currentDecodeContext!.IsReplayEligible))
		if (_loadedDecodeContext && (_loadedDecodeContext != currentDecodeContext || !currentDecodeContext.IsReplayEligible))
		{
			Invalidate(ReplayPhaseInvalidationReason::CertificateMutation);
			return false;
		}

		// Loop buffer hit — copy cached MicroOps to caller buffer
		for (int i = 0; i < MAX_SLOTS; i++)
		{
			target[i] = _slots[i];
		}

		ConsumeReplayHit();

		return true;
	}
#endif

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: [MethodImpl(MethodImplOptions.AggressiveInlining)] public bool TryGetReplayEntry(ulong requestedPC, Decoder.CanonicalDecodeContext currentDecodeContext, out Decoder.ReplayEntry? replayEntry)
	bool LoopBuffer::TryGetReplayEntry(unsigned long long requestedPC, std::shared_ptr<Decoder::CanonicalDecodeContext> currentDecodeContext, std::shared_ptr<Decoder::ReplayEntry> &replayEntry)
	{
		ArgumentNullException::ThrowIfNull(currentDecodeContext);
		replayEntry = nullptr;
		if (getState() != BufferState::Active && getState() != BufferState::Draining)
		{
			return false;
		}
		if (requestedPC != getCachedPC())
		{
			Invalidate(ReplayPhaseInvalidationReason::PcMismatch);
			return false;
		}
		if (_replayEntry == nullptr || !_replayEntry->HasValidFrozenContent() || _loadedDecodeContext != currentDecodeContext || !currentDecodeContext->IsReplayEligible)
		{
			Invalidate(ReplayPhaseInvalidationReason::CertificateMutation);
			return false;
		}

		replayEntry = _replayEntry;
		ConsumeReplayHit();
		return true;
	}

	void LoopBuffer::EndCycle()
	{
		if (getState() == BufferState::Draining)
		{
			Invalidate(ReplayPhaseInvalidationReason::Completed);
		}
	}

	void LoopBuffer::Invalidate(ReplayPhaseInvalidationReason reason)
	{
		if (getState() != BufferState::Empty)
		{
		_lastInvalidationReason = reason;
		}

		setState(BufferState::Empty);
		setCachedPC(0);
		setRemainingIterations(0);
		setValidSlotCount(0);
		_currentEpochLength = 0;
		_currentEpochReplayCount = 0;
		_stableDonorMask = 0;
		_loadedDecodeContext = std::nullopt;
		_replayEntry = nullptr;
		_aluDonorCapacity = 0;
		_lsuDonorCapacity = 0;
		_dmaStreamDonorCapacity = 0;
		_branchControlDonorCapacity = 0;
		_systemSingletonDonorCapacity = 0;

	#if defined(TESTING)
		_slots = YAKSys_Hybrid_CPU::Core::LoopBuffer::MicroOpSlotBuffer();
		_slotValid = YAKSys_Hybrid_CPU::Core::LoopBuffer::SlotValidBuffer();
	#endif
	}

	ReplayPhaseContext LoopBuffer::getCurrentReplayPhase() const
	{
		return ReplayPhaseContext(getState() == BufferState::Active || getState() == BufferState::Draining, _currentEpochId, getCachedPC(), _currentEpochLength, _currentEpochReplayCount, getValidSlotCount(), _stableDonorMask, _lastInvalidationReason);
	}

	ReplayPhaseMetrics LoopBuffer::GetReplayPhaseMetrics()
	{
		ReplayPhaseMetrics temp_var = ReplayPhaseMetrics();
		temp_var.setReplayEpochCount(getReplayEpochCount());
		temp_var.setTotalEpochLength(getTotalEpochLength());
		temp_var.setStableDonorSlotSamples(getStableDonorSlotSamples());
		temp_var.setTotalReplaySlotSamples(getTotalReplaySlotSamples());
		temp_var.setDeterministicTransitionCount(getDeterministicTransitionCount());
		return temp_var;
	}

	bool LoopBuffer::IsSlotDonorEligible(int slotIndex)
	{
		if (getState() != BufferState::Active)
		{
			return false;
		}

		if (static_cast<unsigned int>(slotIndex) >= MAX_SLOTS)
		{
			return false;
		}

		return (_stableDonorMask & (1 << slotIndex)) != 0;
	}

	int LoopBuffer::getDonorEligibleSlotCount() const
	{
		if (getState() != BufferState::Active)
		{
			return 0;
		}

		return CountBits(_stableDonorMask);
	}

	bool LoopBuffer::isReplaying() const
	{
		return getState() == BufferState::Active;
	}

	int LoopBuffer::GetClassDonorCapacity(SlotClass slotClass) const
	{
		if (getState() != BufferState::Active)
		{
			return 0;
		}

		int temp_var143;
		switch (slotClass)
		{
			case SlotClass::AluClass:
				temp_var143 = _aluDonorCapacity;
				break;
			case SlotClass::LsuClass:
				temp_var143 = _lsuDonorCapacity;
				break;
			case SlotClass::DmaStreamClass:
				temp_var143 = _dmaStreamDonorCapacity;
				break;
			case SlotClass::MatrixTileStreamClass:
				temp_var143 = _dmaStreamDonorCapacity;
				break;
			case SlotClass::BranchControl:
				temp_var143 = _branchControlDonorCapacity;
				break;
			case SlotClass::SystemSingleton:
				temp_var143 = _systemSingletonDonorCapacity;
				break;
			default:
				temp_var143 = 0;
				break;
		}
		return temp_var143;
	}

#if defined(TESTING)
	unsigned char LoopBuffer::ComputeStableDonorMask()
	{
		unsigned char mask = 0;
		for (int i = 0; i < MAX_SLOTS; i++)
		{
			if (IsReplayStableDonorSlot(i))
			{
				mask |= static_cast<unsigned char>(1 << i);
			}
		}

		return mask;
	}
#endif

#if defined(TESTING)
	bool LoopBuffer::IsReplayStableDonorSlot(int slotIndex)
	{
		if (!_slotValid[slotIndex])
		{
			return true;
		}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: MicroOp? op = _slots[slotIndex];
		std::shared_ptr<MicroOp> op = _slots[slotIndex];
		return op != nullptr && std::dynamic_pointer_cast<VectorMicroOp>(op) == nullptr;
	}
#endif

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private static byte ComputeStableDonorMask(System.Collections.Generic.IReadOnlyList<MicroOp?> carriers)
	unsigned char LoopBuffer::ComputeStableDonorMask(const std::vector<std::shared_ptr<MicroOp>> &carriers)
	{
		unsigned char mask = 0;
		for (int slotIndex = 0; slotIndex < MAX_SLOTS; slotIndex++)
		{
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: MicroOp? carrier = carriers[slotIndex];
			std::shared_ptr<MicroOp> carrier = carriers[slotIndex];
			if (carrier == nullptr || std::dynamic_pointer_cast<VectorMicroOp>(carrier) == nullptr)
			{
				mask |= static_cast<unsigned char>(1 << slotIndex);
			}
		}

		return mask;
	}

	void LoopBuffer::ConsumeReplayHit()
	{
		setTotalReplays(getTotalReplays() + 1);
		setFetchSavings(getFetchSavings() + 1);
		_currentEpochReplayCount++;
		setTotalReplaySlotSamples(getTotalReplaySlotSamples() + MAX_SLOTS);
		setStableDonorSlotSamples(getStableDonorSlotSamples() + static_cast<unsigned long long>(CountBits(_stableDonorMask)));

		if (getRemainingIterations() > 0)
		{
			setRemainingIterations(getRemainingIterations() - 1);
		}
		if (getRemainingIterations() == 0)
		{
			setState(BufferState::Draining);
		}
	}

	void LoopBuffer::ComputeClassDonorCapacity()
	{
		_aluDonorCapacity = 0;
		_lsuDonorCapacity = 0;
		_dmaStreamDonorCapacity = 0;
		_branchControlDonorCapacity = 0;
		_systemSingletonDonorCapacity = 0;

		for (int i = 0; i < MAX_SLOTS; i++)
		{
			if ((_stableDonorMask & (1 << i)) == 0)
			{
				continue;
			}

			// Map lane index to primary class using default W=8 topology:
			// lanes 0-3 → AluClass, lanes 4-5 → LsuClass,
			// lane 6 → DmaStreamClass, lane 7 → BranchControl (aliased with SystemSingleton)
			switch (i)
			{
				case 0:
			case 1:
		case 2:
	case 3:
					_aluDonorCapacity++;
					break;
				case 4:
			case 5:
					_lsuDonorCapacity++;
					break;
				case 6:
					_dmaStreamDonorCapacity++;
					break;
				case 7:
					// Lane 7 is aliased: count for both BranchControl and SystemSingleton
					_branchControlDonorCapacity++;
					_systemSingletonDonorCapacity++;
					break;
			}
		}
	}

	int LoopBuffer::CountBits(unsigned char value)
	{
		int count = 0;
		unsigned char remaining = value;
		while (remaining != 0)
		{
			count += remaining & 1;
			remaining >>= 1;
		}

		return count;
	}
}
