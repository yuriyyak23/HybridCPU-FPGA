#include "MultiBankMemoryArea.h"
#include "../../Core/Execution/Memory/AtomicMemory/YAKSys_Hybrid_CPU.Core.Memory.MainMemoryAtomicMemoryUnit.h"
#include <algorithm>

using namespace YAKSys_Hybrid_CPU;

namespace HybridCPU_ISE::CloseToHSL::Memory::Banks
{

	MultiBankMemoryArea::MultiBankMemoryArea(int bankCount, unsigned long long bankSize)
	{
		if (bankCount <= 0 || bankCount > 16)
		{
			throw ArgumentOutOfRangeException(L"Bank count must be between 1 and 16.");
		}

		if (bankSize == 0 || bankSize > static_cast<unsigned long long>(std::numeric_limits<int>::max()))
		{
			throw ArgumentOutOfRangeException(L"Bank size must be > 0 and <= int.MaxValue.");
		}

		_bankCount = bankCount;
		_bankSize = bankSize;
		_totalSize = static_cast<unsigned long long>(_bankCount) * _bankSize;
		_banks.resize(_bankCount);
		_bankStates.resize(_bankCount);
		_bankLocks.resize(_bankCount);
		_bankDomainCapabilities.resize(_bankCount, 0);

		// Initialize all banks and their states
		for (int i = 0; i < _bankCount; i++)
		{
			_banks[i] = std::make_shared<MemoryStream>();
			_banks[i]->setCapacity(static_cast<int>(bankSize));
			_banks[i]->SetLength(static_cast<long long>(bankSize));
			_bankLocks[i] = std::make_shared<std::mutex>();
			_bankStates[i] = BankState{};
			_bankStates[i].PortId = -1;
		}
	}

	std::pair<std::shared_ptr<MemoryStream>, unsigned long long> MultiBankMemoryArea::GetBank(unsigned long long physicalAddress) const
	{
		const int bankIndex = static_cast<int>((physicalAddress / _bankSize) % static_cast<unsigned long long>(_bankCount));
		return {_banks[bankIndex], physicalAddress % _bankSize};
	}

	long long MultiBankMemoryArea::getLength() const
	{
		long long total = 0;
		for (const auto &bank : _banks)
		{
			total += bank->getLength();
		}
		return total;
	}

	int MultiBankMemoryArea::getCapacity() const
	{
		// Return aggregate capacity
		long long total = 0;
		for (int i = 0; i < _bankCount; i++)
		{
			total += _banks[i]->getCapacity();
		}
		return static_cast<int>((std::min)(total, static_cast<long long>((std::numeric_limits<int>::max)())));
	}

	void MultiBankMemoryArea::setCapacity(int value)
	{
		if (value < 0)
		{
			throw ArgumentOutOfRangeException(L"Capacity must be non-negative.");
		}
		// Distribute capacity across banks
		int perBank = value / _bankCount;
		int remainder = value % _bankCount;
		for (int i = 0; i < _bankCount; i++)
		{
			std::scoped_lock<std::mutex> bankLock(*_bankLocks[i]);
			_banks[i]->setCapacity(perBank + (i < remainder ? 1 : 0));
		}
	}

	long long MultiBankMemoryArea::getSilentSquashCount() const
	{
		return SilentSquashCount;
	}

	void MultiBankMemoryArea::setSilentSquashCount(long long value)
	{
		SilentSquashCount = value;
	}

	int MultiBankMemoryArea::Read(std::vector<unsigned char> &buffer, int offset, int count)
	{
		if (buffer.empty())
		{
			throw std::invalid_argument("buffer");
		}
		if (offset < 0 || static_cast<std::size_t>(offset) > buffer.size())
		{
			throw ArgumentOutOfRangeException(L"Invalid memory buffer offset.");
		}
		if (count < 0)
		{
			throw ArgumentOutOfRangeException(L"Invalid memory buffer count.");
		}
		if (static_cast<std::size_t>(count) > buffer.size() - static_cast<std::size_t>(offset))
		{
			throw std::invalid_argument("Invalid offset length.");
		}

		// Check if this is physical context (direct bank access)
		if (_isPhysicalAccessContext)
		{
			{
				std::scoped_lock<std::mutex> lock(getInstructionFetchCommitMutex());
				unsigned long long physAddr = static_cast<unsigned long long>(getPosition());

				// Silent Squash: check domain access before physical read (req.md §4)
				if (_currentDomainTag != 0)
				{
					int bankId = static_cast<int>((physAddr / _bankSize) % static_cast<unsigned long long>(_bankCount));
					if (!CheckBankDomainAccess(bankId, _currentDomainTag))
					{
						setSilentSquashCount(getSilentSquashCount() + 1);
						_lastAccessSilentlySquashed = true;
						return 0; // Silent squash — return zero bytes read
					}
				}

// C# TO C++ CONVERTER TASK: C++ has no equivalent to C# deconstruction declarations:
				auto [bank, bankOffset] = GetBank(physAddr);
				const int bankId = static_cast<int>((physAddr / _bankSize) % static_cast<unsigned long long>(_bankCount));

				{
					std::scoped_lock<std::mutex> bankLock(*_bankLocks[bankId]);
					bank->setPosition(static_cast<long long>(bankOffset));
					return bank->Read(buffer, offset, count);
				}
			}
		}
		else
		{
			// Virtual context - use base IOMMU-aware read
			return Processor::MainMemoryArea::Read(buffer, offset, count);
		}
	}

	void MultiBankMemoryArea::Write(std::vector<unsigned char> &buffer, int offset, int count)
	{
		if (buffer.empty())
		{
			throw std::invalid_argument("buffer");
		}
		if (offset < 0 || static_cast<std::size_t>(offset) > buffer.size())
		{
			throw ArgumentOutOfRangeException(L"Invalid memory buffer offset.");
		}
		if (count < 0)
		{
			throw ArgumentOutOfRangeException(L"Invalid memory buffer count.");
		}
		if (static_cast<std::size_t>(count) > buffer.size() - static_cast<std::size_t>(offset))
		{
			throw std::invalid_argument("Invalid offset length.");
		}

		// Check if this is physical context (direct bank access)
		if (_isPhysicalAccessContext)
		{
			{
				std::scoped_lock<std::mutex> lock(getInstructionFetchCommitMutex());
				unsigned long long physAddr = static_cast<unsigned long long>(getPosition());

				// Silent Squash: check domain access before physical write (req.md §4)
				if (_currentDomainTag != 0)
				{
					int bankId = static_cast<int>((physAddr / _bankSize) % static_cast<unsigned long long>(_bankCount));
					if (!CheckBankDomainAccess(bankId, _currentDomainTag))
					{
						setSilentSquashCount(getSilentSquashCount() + 1);
						_lastAccessSilentlySquashed = true;
						return; // Silent squash — discard write silently
					}
				}

// C# TO C++ CONVERTER TASK: C++ has no equivalent to C# deconstruction declarations:
				auto [bank, bankOffset] = GetBank(physAddr);
				const int bankId = static_cast<int>((physAddr / _bankSize) % static_cast<unsigned long long>(_bankCount));

				{
					std::scoped_lock<std::mutex> bankLock(*_bankLocks[bankId]);
					bank->setPosition(static_cast<long long>(bankOffset));
					bank->Write(buffer, offset, count);
					if (count != 0)
					{
						NotifyReplayRelevantMutation();
					}
				}
				// Keep reservation publication outside the bank lock. Neither
				// publication enters cache/freshness/verifier/retire callbacks.
				YAKSys_Hybrid_CPU::Core::Memory::MainMemoryAtomicMemoryUnit::NotifyPhysicalWrite(physAddr, count);
			}
		}
		else
		{
			// Virtual context - use base IOMMU-aware write
			Processor::MainMemoryArea::Write(buffer, offset, count);
		}
	}

	int MultiBankMemoryArea::ReadByte()
	{
		std::vector<unsigned char> value(1);
		return Read(value, 0, 1) == 1 ? value[0] : -1;
	}

	void MultiBankMemoryArea::WriteByte(unsigned char value)
	{
		std::vector<unsigned char> byte{value};
		Write(byte, 0, 1);
	}

	void MultiBankMemoryArea::SetAccessDomainTag(unsigned long long domainTag)
	{
		_currentDomainTag = domainTag;
	}

	void MultiBankMemoryArea::ResetLastAccessSilentSquashFlag()
	{
		_lastAccessSilentlySquashed = false;
	}

	bool MultiBankMemoryArea::ConsumeLastAccessSilentSquashFlag()
	{
		bool result = _lastAccessSilentlySquashed;
		_lastAccessSilentlySquashed = false;
		return result;
	}

	void MultiBankMemoryArea::SetLength(long long value)
	{
		if (value < 0)
		{
			throw ArgumentOutOfRangeException(L"Length must be non-negative.");
		}

		{
			std::scoped_lock<std::mutex> lock(getInstructionFetchCommitMutex());
			// Distribute new length across banks
			long long perBank = value / _bankCount;

			for (int i = 0; i < _bankCount; i++)
			{
				{
					std::scoped_lock<std::mutex> bankLock(*_bankLocks[i]);
					long long previousLength = _banks[i]->getLength();
					_banks[i]->SetLength(perBank);
					// Publish each completed bank mutation, including when a
					// later bank resize fails. Unchanged/failed-before-mutation
					// operations do not fabricate an executable generation.
					if (previousLength != perBank)
					{
						NotifyReplayRelevantMutation();
					}
				}
			}
		}
	}

	void MultiBankMemoryArea::Flush()
	{
		for (int i = 0; i < _bankCount; i++)
		{
			{
				std::scoped_lock<std::mutex> bankLock(*_bankLocks[i]);
				_banks[i]->Flush();
			}
		}
	}

	long long MultiBankMemoryArea::GetBankLength(int bankIndex)
	{
		if (bankIndex < 0 || bankIndex >= _bankCount)
		{
			throw ArgumentOutOfRangeException(L"Bank index is outside the configured bank range.");
		}

		{
			std::scoped_lock<std::mutex> bankLock(*_bankLocks[bankIndex]);
			return _banks[bankIndex]->getLength();
		}
	}

	void MultiBankMemoryArea::SetBankDomainCapability(int bankId, unsigned long long domainMask)
	{
		if (bankId < 0 || bankId >= _bankCount)
		{
			throw ArgumentOutOfRangeException(L"Bank id is outside the configured bank range.");
		}

		_bankDomainCapabilities[bankId] = domainMask;
	}

	bool MultiBankMemoryArea::CheckBankDomainAccess(int bankId, unsigned long long domainTag)
	{
		if (static_cast<unsigned int>(bankId) >= static_cast<unsigned int>(_bankCount))
		{
			return false;
		}

		unsigned long long capability = _bankDomainCapabilities[bankId];

		// Zero capability = unrestricted (backward compat / kernel mode)
		if (capability == 0)
		{
			return true;
		}

		// Zero domain tag = trusted kernel — always allowed
		if (domainTag == 0)
		{
			return true;
		}

		// Hardware AND gate: domain must match bank capability
		return (capability & domainTag) != 0;
	}

	int MultiBankMemoryArea::CalculateAccessLatency(unsigned long long address, int size, bool isSequential)
	{
		int bankId = static_cast<int>((address / _bankSize) % static_cast<unsigned long long>(_bankCount));
		constexpr unsigned long long RowBufferSize = 1024; // 1KB row buffer per bank

		{
			std::scoped_lock<std::mutex> lock(*_bankLocks[bankId]);
// C# TO C++ CONVERTER TASK: 'ref locals' are not converted by C# to C++ Converter:
// ORIGINAL LINE: ref BankState state = ref _bankStates[bankId];
			BankState &state = _bankStates[bankId];

			// Calculate row address (address / row buffer size)
			unsigned long long rowAddress = address / RowBufferSize;
			unsigned long long lastRowAddress = state.LastAccessAddress / RowBufferSize;

			// Row buffer hit if accessing same row
			if (rowAddress == lastRowAddress && state.LastAccessAddress != 0)
			{
				return 10; // Row buffer hit latency
			}

			// Row buffer miss - need to load new row
			return 30; // Row buffer miss latency
		}
	}

	int MultiBankMemoryArea::AllocatePort(int bankId, long long currentCycle)
	{
		if (bankId < 0 || bankId >= _bankCount)
		{
			throw ArgumentOutOfRangeException(L"Bank id is outside the configured bank range.");
		}

		{
			std::scoped_lock<std::mutex> lock(*_bankLocks[bankId]);
// C# TO C++ CONVERTER TASK: 'ref locals' are not converted by C# to C++ Converter:
// ORIGINAL LINE: ref BankState state = ref _bankStates[bankId];
			BankState &state = _bankStates[bankId];

			// Simple port allocation - use bank ID modulo number of ports
			// In real hardware this would be more sophisticated
			int portId = bankId % 2; // Assuming 2 ports (configurable in MemorySubsystem)

			state.PortId = portId;
			return portId;
		}
	}

	void MultiBankMemoryArea::ReleasePort(int portId, int bankId, long long currentCycle)
	{
		if (bankId < 0 || bankId >= _bankCount)
		{
			throw ArgumentOutOfRangeException(L"Bank id is outside the configured bank range.");
		}

		{
			std::scoped_lock<std::mutex> lock(*_bankLocks[bankId]);
// C# TO C++ CONVERTER TASK: 'ref locals' are not converted by C# to C++ Converter:
// ORIGINAL LINE: ref BankState state = ref _bankStates[bankId];
			BankState &state = _bankStates[bankId];

			if (state.PortId == portId)
			{
				state.PortId = -1; // Release port
			}
		}
	}

	void MultiBankMemoryArea::MarkBankBusy(int bankId, unsigned long long address, long long completionCycle)
	{
		if (bankId < 0 || bankId >= _bankCount)
		{
			throw ArgumentOutOfRangeException(L"Bank id is outside the configured bank range.");
		}

		{
			std::scoped_lock<std::mutex> lock(*_bankLocks[bankId]);
// C# TO C++ CONVERTER TASK: 'ref locals' are not converted by C# to C++ Converter:
// ORIGINAL LINE: ref BankState state = ref _bankStates[bankId];
			BankState &state = _bankStates[bankId];

			state.Busy = true;
			state.BusyUntilCycle = completionCycle;
			state.LastAccessAddress = address;
			state.QueueDepth++;
		}
	}

	void MultiBankMemoryArea::MarkBankAvailable(int bankId, long long currentCycle)
	{
		if (bankId < 0 || bankId >= _bankCount)
		{
			throw ArgumentOutOfRangeException(L"Bank id is outside the configured bank range.");
		}

		{
			std::scoped_lock<std::mutex> lock(*_bankLocks[bankId]);
// C# TO C++ CONVERTER TASK: 'ref locals' are not converted by C# to C++ Converter:
// ORIGINAL LINE: ref BankState state = ref _bankStates[bankId];
			BankState &state = _bankStates[bankId];

			if (currentCycle >= state.BusyUntilCycle)
			{
				state.Busy = false;
				state.QueueDepth = (std::max)(0, state.QueueDepth - 1);
			}
		}
	}

	BankArbitrator::BankArbitrator(int bankCount, unsigned long long bankSize) : _busyBanks(std::vector<bool>(bankCount)), _bankCount(bankCount), _bankSize(bankSize)
	{
	}

	int BankArbitrator::CalculateBank(unsigned long long address)
	{
		return static_cast<int>((address / _bankSize) % static_cast<unsigned long long>(_bankCount));
	}

	bool BankArbitrator::TryReserveBank(unsigned long long address, int &bankId)
	{
		bankId = CalculateBank(address);
		if (_busyBanks[bankId])
		{
			return false;
		}

		_busyBanks[bankId] = true;
		_busyCount++;
		return true;
	}

	void BankArbitrator::ResetCycle()
	{
		for (int i = 0; i < _bankCount; i++)
		{
			_busyBanks[i] = false;
		}
		_busyCount = 0;
	}

	int BankArbitrator::GetBusyBankCount()
	{
		return _busyCount;
	}
}
