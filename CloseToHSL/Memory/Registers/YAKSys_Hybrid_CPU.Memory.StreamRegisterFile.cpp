#include "YAKSys_Hybrid_CPU.Memory.StreamRegisterFile.h"
#include <algorithm>
#include "../../Core/Architecture/MemoryModel/Addressing/MemoryRangeOverlap.h"

using namespace YAKSys_Hybrid_CPU::Core;
using namespace YAKSys_Hybrid_CPU::Core::Memory;

namespace YAKSys_Hybrid_CPU::Memory
{

	bool StreamRegisterFile::CanAllocateAssistRegister(unsigned long long sourceAddr, unsigned char elementSize, AssistStreamRegisterPartitionPolicy policy, AssistStreamRegisterRejectKind &rejectKind)
	{
		rejectKind = AssistStreamRegisterRejectKind::None;
		int unusedRegisterIndex = 0;
		if (TryFindAssistReuseRegister(sourceAddr, elementSize, 1, unusedRegisterIndex))
		{
			return true;
		}

		return TrySelectAssistRegisterSlot(policy, unusedRegisterIndex, rejectKind);
	}

	bool StreamRegisterFile::TryAllocateAssistRegister(unsigned long long sourceAddr, unsigned char elementSize, unsigned int elementCount, AssistStreamRegisterPartitionPolicy policy, int &registerIndex, AssistStreamRegisterRejectKind &rejectKind)
	{
		rejectKind = AssistStreamRegisterRejectKind::None;
		if (TryFindAssistReuseRegister(sourceAddr, elementSize, elementCount, registerIndex))
		{
			return true;
		}

		if (!TrySelectAssistRegisterSlot(policy, registerIndex, rejectKind))
		{
			return false;
		}

// C# TO C++ CONVERTER TASK: 'ref locals' are not converted by C# to C++ Converter:
// ORIGINAL LINE: ref RegisterEntry reg = ref registers[registerIndex];
		RegisterEntry& reg = registers[registerIndex];
		if (reg.State == RegisterState::Dirty)
		{
			totalEvictions++;
		}

		reg.State = RegisterState::Invalid;
		reg.SourceAddress = sourceAddr;
		reg.ElementSize = elementSize;
		reg.ElementCount = elementCount;
		reg.ValidBytes = 0;
		reg.LastAccessTime = ++accessCounter;
		reg.AssistOwned = true;
		return true;
	}

	void StreamRegisterFile::MarkAssistLoading(int regIndex)
	{
		if (regIndex < 0 || regIndex >= registerCount)
		{
			return;
		}

		if (registers[regIndex].State == RegisterState::Invalid)
		{
			registers[regIndex].State = RegisterState::Loading;
		}

		registers[regIndex].AssistOwned = true;
	}

	int StreamRegisterFile::CountAssistOwnedRegisters()
	{
		int count = 0;
		for (int i = 0; i < registerCount; i++)
		{
			if (registers[i].AssistOwned && registers[i].State != RegisterState::Invalid)
			{
				count++;
			}
		}

		return count;
	}

	int StreamRegisterFile::CountAssistOwnedRegisters(RegisterState state)
	{
		int count = 0;
		for (int i = 0; i < registerCount; i++)
		{
			if (registers[i].AssistOwned && registers[i].State == state)
			{
				count++;
			}
		}

		return count;
	}

	bool StreamRegisterFile::TryFindAssistReuseRegister(unsigned long long sourceAddr, unsigned char elementSize, unsigned int elementCount, int &registerIndex)
	{
		registerIndex = -1;
		unsigned int requestedBytes = ComputeRequestedByteCount(elementSize, elementCount);

		for (int i = 0; i < registerCount; i++)
		{
			unsigned int trackedBytes = GetTrackedByteCount(registers[i]);
			if ((registers[i].State == RegisterState::Valid || registers[i].State == RegisterState::Loading) && registers[i].SourceAddress == sourceAddr && registers[i].ElementSize == elementSize && trackedBytes >= requestedBytes)
			{
				registerIndex = i;
				return true;
			}
		}

		return false;
	}

	bool StreamRegisterFile::TrySelectAssistRegisterSlot(AssistStreamRegisterPartitionPolicy policy, int &registerIndex, AssistStreamRegisterRejectKind &rejectKind)
	{
		registerIndex = -1;
		rejectKind = AssistStreamRegisterRejectKind::None;

		if (CountAssistOwnedRegisters(RegisterState::Loading) >= policy.getLoadingRegisterBudget())
		{
			rejectKind = AssistStreamRegisterRejectKind::LoadingBudget;
			return false;
		}

		int invalidIndex = FindEmptyRegisterSlot();
		if (invalidIndex >= 0)
		{
			if (CountAssistOwnedRegisters() >= policy.getResidentRegisterBudget())
			{
				rejectKind = AssistStreamRegisterRejectKind::ResidentBudget;
				return false;
			}

			registerIndex = invalidIndex;
			return true;
		}

		int victimIndex = FindAssistVictimRegister();
		if (victimIndex >= 0)
		{
			registerIndex = victimIndex;
			return true;
		}

		rejectKind = CountAssistOwnedRegisters() >= policy.getResidentRegisterBudget() ? AssistStreamRegisterRejectKind::ResidentBudget : AssistStreamRegisterRejectKind::NoAssistVictim;
		return false;
	}

	int StreamRegisterFile::FindEmptyRegisterSlot()
	{
		for (int i = 0; i < registerCount; i++)
		{
			if (registers[i].State == RegisterState::Invalid)
			{
				return i;
			}
		}

		return -1;
	}

	int StreamRegisterFile::FindAssistVictimRegister()
	{
		int victimIndex = -1;
		unsigned long long oldestTime = std::numeric_limits<unsigned long long>::max();

		for (int i = 0; i < registerCount; i++)
		{
			if (!registers[i].AssistOwned || registers[i].State == RegisterState::Loading)
			{
				continue;
			}

			if (registers[i].State != RegisterState::Invalid && registers[i].LastAccessTime < oldestTime)
			{
				oldestTime = registers[i].LastAccessTime;
				victimIndex = i;
			}
		}

		return victimIndex;
	}

	StreamRegisterFile::StreamRegisterFile(int numRegisters) : registerCount(numRegisters)
	{
		registers = std::vector<RegisterEntry>(numRegisters);
		accessCounter = 0;
		totalHits = 0;
		totalMisses = 0;
		totalEvictions = 0;

		for (int i = 0; i < numRegisters; i++)
		{
			registers[i].Data = std::vector<unsigned char>(REGISTER_SIZE);
			registers[i].State = RegisterState::Invalid;
			registers[i].SourceAddress = 0;
			registers[i].ValidBytes = 0;
			registers[i].LastAccessTime = 0;
			registers[i].ElementSize = 0;
			registers[i].ElementCount = 0;
			registers[i].AssistOwned = false;
		}
	}

	int StreamRegisterFile::AllocateRegister(unsigned long long sourceAddr, unsigned char elementSize, unsigned int elementCount)
	{
		unsigned int requestedBytes = ComputeRequestedByteCount(elementSize, elementCount);
		int matchingIndexNeedingReload = -1;

		for (int i = 0; i < registerCount; i++)
		{
			if (registers[i].State == RegisterState::Valid && registers[i].SourceAddress == sourceAddr && registers[i].ElementSize == elementSize)
			{
				if (registers[i].ValidBytes < requestedBytes)
				{
					matchingIndexNeedingReload = i;
					break;
				}

				registers[i].LastAccessTime = ++accessCounter;
				registers[i].AssistOwned = false;
				totalHits++;
				return i;
			}
		}

		totalMisses++;
		int victimIndex = matchingIndexNeedingReload >= 0 ? matchingIndexNeedingReload : FindVictimRegister();

		if (victimIndex == -1)
		{
			return -1;
		}

		if (registers[victimIndex].State == RegisterState::Dirty)
		{
			totalEvictions++;
		}

		registers[victimIndex].State = RegisterState::Invalid;
		registers[victimIndex].SourceAddress = sourceAddr;
		registers[victimIndex].ElementSize = elementSize;
		registers[victimIndex].ElementCount = elementCount;
		registers[victimIndex].ValidBytes = 0;
		registers[victimIndex].LastAccessTime = ++accessCounter;
		registers[victimIndex].AssistOwned = false;

		return victimIndex;
	}

	int StreamRegisterFile::FindVictimRegister()
	{
		int victimIndex = -1;
		unsigned long long oldestTime = std::numeric_limits<unsigned long long>::max();

		for (int i = 0; i < registerCount; i++)
		{
			if (registers[i].State == RegisterState::Invalid)
			{
				return i;
			}
		}

		for (int i = 0; i < registerCount; i++)
		{
			if (registers[i].State == RegisterState::Loading)
			{
				continue;
			}

			if (registers[i].LastAccessTime < oldestTime)
			{
				oldestTime = registers[i].LastAccessTime;
				victimIndex = i;
			}
		}

		return victimIndex;
	}

	bool StreamRegisterFile::LoadRegister(int regIndex, std::vector<unsigned char> &memory)
	{
		if (regIndex < 0 || regIndex >= registerCount)
		{
			return false;
		}

// C# TO C++ CONVERTER TASK: 'ref locals' are not converted by C# to C++ Converter:
// ORIGINAL LINE: ref RegisterEntry reg = ref registers[regIndex];
		RegisterEntry& reg = registers[regIndex];
		unsigned int bytesToLoad = ComputeRequestedByteCount(reg.ElementSize, reg.ElementCount);
		if (reg.SourceAddress + bytesToLoad > static_cast<unsigned long long>(memory.size()))
		{
			return false;
		}

		return LoadRegister(regIndex, std::span<const unsigned char>(memory).subspan(static_cast<std::size_t>(reg.SourceAddress), bytesToLoad));
	}

	bool StreamRegisterFile::LoadRegister(int regIndex, std::span<const unsigned char> sourceData)
	{
		if (regIndex < 0 || regIndex >= registerCount)
		{
			return false;
		}

// C# TO C++ CONVERTER TASK: 'ref locals' are not converted by C# to C++ Converter:
// ORIGINAL LINE: ref RegisterEntry reg = ref registers[regIndex];
		RegisterEntry& reg = registers[regIndex];
		unsigned int bytesToLoad = ComputeRequestedByteCount(reg.ElementSize, reg.ElementCount);
		if (bytesToLoad == 0 || bytesToLoad > sourceData.size())
		{
			return false;
		}

		reg.State = RegisterState::Loading;
		std::copy_n(sourceData.begin(), bytesToLoad, reg.Data.begin());
		reg.State = RegisterState::Valid;
		reg.ValidBytes = bytesToLoad;
		reg.LastAccessTime = ++accessCounter;
		return true;
	}

	bool StreamRegisterFile::ReadRegister(int regIndex, std::span<unsigned char> destination, unsigned int byteCount)
	{
		if (regIndex < 0 || regIndex >= registerCount)
		{
			return false;
		}

// C# TO C++ CONVERTER TASK: 'ref locals' are not converted by C# to C++ Converter:
// ORIGINAL LINE: ref RegisterEntry reg = ref registers[regIndex];
		RegisterEntry& reg = registers[regIndex];
		if (reg.State != RegisterState::Valid)
		{
			return false;
		}

		if (byteCount > reg.ValidBytes || byteCount > destination.size())
		{
			return false;
		}

		std::copy_n(reg.Data.begin(), byteCount, destination.begin());
		reg.LastAccessTime = ++accessCounter;
		RecordBypassHit(reg.AssistOwned);
		return true;
	}

	bool StreamRegisterFile::TryReadPrefetchedChunk(unsigned long long sourceAddr, unsigned char elementSize, unsigned int elementCount, std::span<unsigned char> destination)
	{
		unsigned int requestedBytes = ComputeRequestedByteCount(elementSize, elementCount);
		if (requestedBytes == 0 || requestedBytes > destination.size())
		{
			return false;
		}

		for (int i = 0; i < registerCount; i++)
		{
// C# TO C++ CONVERTER TASK: 'ref locals' are not converted by C# to C++ Converter:
// ORIGINAL LINE: ref RegisterEntry reg = ref registers[i];
			RegisterEntry& reg = registers[i];
			if (reg.State != RegisterState::Valid || reg.SourceAddress != sourceAddr || reg.ElementSize != elementSize || reg.ValidBytes < requestedBytes)
			{
				continue;
			}

			return ReadRegister(i, destination, requestedBytes);
		}

		return false;
	}

	bool StreamRegisterFile::WriteRegister(int regIndex, std::span<const unsigned char> source, unsigned int byteCount)
	{
		if (regIndex < 0 || regIndex >= registerCount)
		{
			return false;
		}

// C# TO C++ CONVERTER TASK: 'ref locals' are not converted by C# to C++ Converter:
// ORIGINAL LINE: ref RegisterEntry reg = ref registers[regIndex];
		RegisterEntry& reg = registers[regIndex];
		if (byteCount > REGISTER_SIZE || byteCount > source.size())
		{
			return false;
		}

		std::copy_n(source.begin(), byteCount, reg.Data.begin());
		reg.State = RegisterState::Dirty;
		reg.ValidBytes = byteCount;
		reg.AssistOwned = false;
		reg.LastAccessTime = ++accessCounter;
		return true;
	}

	StreamRegisterFile::RegisterState StreamRegisterFile::GetRegisterState(int regIndex)
	{
		if (regIndex < 0 || regIndex >= registerCount)
		{
			return RegisterState::Invalid;
		}

		return registers[regIndex].State;
	}

	void StreamRegisterFile::InvalidateRegister(int regIndex)
	{
		if (regIndex < 0 || regIndex >= registerCount)
		{
			return;
		}

		registers[regIndex].State = RegisterState::Invalid;
		registers[regIndex].ValidBytes = 0;
		registers[regIndex].AssistOwned = false;
	}

	void StreamRegisterFile::InvalidateAll()
	{
		for (int i = 0; i < registerCount; i++)
		{
			registers[i].State = RegisterState::Invalid;
			registers[i].ValidBytes = 0;
			registers[i].AssistOwned = false;
		}
	}

	void StreamRegisterFile::MarkLoading(int regIndex)
	{
		if (regIndex < 0 || regIndex >= registerCount)
		{
			return;
		}

		if (registers[regIndex].State == RegisterState::Invalid)
		{
			registers[regIndex].State = RegisterState::Loading;
		}
	}

	void StreamRegisterFile::InvalidateOverlappingRange(unsigned long long address, unsigned int byteCount)
	{
		(void)InvalidateOverlappingRangeAndCount(address, byteCount);
	}

	int StreamRegisterFile::InvalidateOverlappingRangeAndCount(unsigned long long address, unsigned long long byteCount)
	{
		if (byteCount == 0)
		{
			return 0;
		}

		int invalidated = 0;
		for (int i = 0; i < registerCount; i++)
		{
// C# TO C++ CONVERTER TASK: 'ref locals' are not converted by C# to C++ Converter:
// ORIGINAL LINE: ref RegisterEntry reg = ref registers[i];
			RegisterEntry& reg = registers[i];
			if (reg.State == RegisterState::Invalid)
			{
				continue;
			}

			unsigned int trackedBytes = GetTrackedByteCount(reg);
			if (trackedBytes == 0)
			{
				continue;
			}

			if (MemoryRangeOverlap::RangesOverlap(address, byteCount, reg.SourceAddress, trackedBytes))
			{
				InvalidateRegister(i);
				invalidated++;
			}
		}

		return invalidated;
	}

	unsigned int StreamRegisterFile::ComputeRequestedByteCount(unsigned char elementSize, unsigned int elementCount)
	{
		if (elementSize == 0 || elementCount == 0)
		{
			return 0;
		}

		unsigned long long requestedBytes = static_cast<unsigned long long>(elementSize) * elementCount;
		return static_cast<unsigned int>(std::min(static_cast<unsigned long long>(REGISTER_SIZE), requestedBytes));
	}

	unsigned int StreamRegisterFile::GetTrackedByteCount(RegisterEntry const reg)
	{
		return reg.State == RegisterState::Valid ? reg.ValidBytes : ComputeRequestedByteCount(reg.ElementSize, reg.ElementCount);
	}

	void StreamRegisterFile::RecordWarmAttempt(bool assistOwned)
	{
		if (assistOwned)
		{
			_assistWarmAttempts++;
		}
		else
		{
			_foregroundWarmAttempts++;
		}
	}

	void StreamRegisterFile::RecordWarmSuccess(bool assistOwned)
	{
		if (assistOwned)
		{
			_assistWarmSuccesses++;
		}
		else
		{
			_foregroundWarmSuccesses++;
		}
	}

	void StreamRegisterFile::RecordWarmReuse(bool assistOwned)
	{
		RecordWarmSuccess(assistOwned);
		if (assistOwned)
		{
			_assistWarmReuseHits++;
		}
		else
		{
			_foregroundWarmReuseHits++;
		}
	}

	void StreamRegisterFile::RecordBypassHit(bool assistOwned)
	{
		_l1BypassHits++;
		if (assistOwned)
		{
			_assistBypassHits++;
		}
		else
		{
			_foregroundBypassHits++;
		}
	}

	void StreamRegisterFile::RecordWarmTranslationReject()
	{
		_translationRejects++;
	}

	void StreamRegisterFile::RecordWarmBackendReject()
	{
		_backendRejects++;
	}

	void StreamRegisterFile::RecordAssistScheduleReject(AssistStreamRegisterRejectKind rejectKind)
	{
		switch (rejectKind)
		{
			case AssistStreamRegisterRejectKind::ResidentBudget:
				_assistResidentBudgetRejects++;
				break;
			case AssistStreamRegisterRejectKind::LoadingBudget:
				_assistLoadingBudgetRejects++;
				break;
			case AssistStreamRegisterRejectKind::NoAssistVictim:
				_assistNoVictimRejects++;
				break;
		}
	}

	StreamIngressWarmTelemetry StreamRegisterFile::GetIngressWarmTelemetry()
	{
		return StreamIngressWarmTelemetry(_foregroundWarmAttempts, _foregroundWarmSuccesses, _foregroundWarmReuseHits, _foregroundBypassHits, _assistWarmAttempts, _assistWarmSuccesses, _assistWarmReuseHits, _assistBypassHits, _translationRejects, _backendRejects, _assistResidentBudgetRejects, _assistLoadingBudgetRejects, _assistNoVictimRejects);
	}
}
