#include "PhysicalRegisterFile.h"

namespace YAKSys_Hybrid_CPU::Core::Registers
{

	PhysicalRegisterFile::PhysicalRegisterFile() : PhysicalRegisterFile(nullptr)
	{
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal PhysicalRegisterFile(System.Func<CanonicalStateAccessKind, IDisposable>? reserveAccess)
	PhysicalRegisterFile::PhysicalRegisterFile(std::function<std::shared_ptr<Disposable>(CanonicalStateAccessKind)> reserveAccess) : _reserveAccess(reserveAccess)
	{
	}

	unsigned long long PhysicalRegisterFile::Read(int physId)
	{
// C# TO C++ CONVERTER NOTE: The following 'using' block is replaced by its C++ equivalent:
// ORIGINAL LINE: using var access = _reserveAccess == null ? null : _reserveAccess.Invoke(CanonicalStateAccessKind.Read);
		{
			auto access = _reserveAccess == nullptr ? nullptr : _reserveAccess(CanonicalStateAccessKind::Read);
			ValidateId(physId);
			return physId == 0 ? 0ULL : _physRegs[physId];
		}
	}

	unsigned long long PhysicalRegisterFile::Read(std::shared_ptr<PhysRegId> physId)
	{
		return Read(physId->getValue());
	}

	void PhysicalRegisterFile::Write(int physId, unsigned long long value)
	{
// C# TO C++ CONVERTER NOTE: The following 'using' block is replaced by its C++ equivalent:
// ORIGINAL LINE: using var access = _reserveAccess == null ? null : _reserveAccess.Invoke(CanonicalStateAccessKind.Write);
		{
			auto access = _reserveAccess == nullptr ? nullptr : _reserveAccess(CanonicalStateAccessKind::Write);
			ValidateId(physId);
			if (physId == 0)
			{
				return; // p0 is hardwired zero
			}
			_physRegs[physId] = value;
		}
	}

	void PhysicalRegisterFile::Write(std::shared_ptr<PhysRegId> physId, unsigned long long value)
	{
		Write(physId->getValue(), value);
	}

	void PhysicalRegisterFile::ValidateId(int physId)
	{
		if (static_cast<unsigned int>(physId) >= TotalPhysRegs)
		{
			throw ArgumentOutOfRangeException(std::format("Physical register index must be in [0, {}].", TotalPhysRegs - 1));
		}
	}

	RenameMap::RenameMap(int smtWays) : RenameMap(smtWays, nullptr)
	{
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal RenameMap(int smtWays, System.Func<CanonicalStateAccessKind, IDisposable>? reserveAccess)
	RenameMap::RenameMap(int smtWays, std::function<std::shared_ptr<Disposable>(CanonicalStateAccessKind)> reserveAccess)
		: _smtWays(smtWays), _map(RectangularVectors::rectangularIntVector(smtWays, ArchRegs)), _reserveAccess(std::move(reserveAccess))
	{
		if (smtWays < 1)
		{
			throw ArgumentOutOfRangeException("SMT ways must be positive.");
		}
// C# TO C++ CONVERTER NOTE: The following call to the 'RectangularVectors' helper class reproduces the rectangular array initialization that is automatic in C#:
// ORIGINAL LINE: _map = new int[smtWays, ArchRegs];
		Reset();
	}

	int RenameMap::Lookup(int vtId, int archReg)
	{
// C# TO C++ CONVERTER NOTE: The following 'using' block is replaced by its C++ equivalent:
// ORIGINAL LINE: using var access = _reserveAccess == null ? null : _reserveAccess.Invoke(CanonicalStateAccessKind.Read);
		{
			auto access = _reserveAccess == nullptr ? nullptr : _reserveAccess(CanonicalStateAccessKind::Read);
			ValidateVt(vtId);
			ValidateArch(archReg);
			return _map[vtId][archReg];
		}
	}

	std::shared_ptr<PhysRegId> RenameMap::Lookup(std::shared_ptr<VtId> vtId, std::shared_ptr<ArchRegId> archReg)
	{
		return std::make_shared<PhysRegId>(PhysRegId::Create(Lookup(vtId->getValue(), archReg->getValue())));
	}

	void RenameMap::Remap(int vtId, int archReg, int physReg)
	{
// C# TO C++ CONVERTER NOTE: The following 'using' block is replaced by its C++ equivalent:
// ORIGINAL LINE: using var access = _reserveAccess == null ? null : _reserveAccess.Invoke(CanonicalStateAccessKind.Write);
		{
			auto access = _reserveAccess == nullptr ? nullptr : _reserveAccess(CanonicalStateAccessKind::Write);
			ValidateVt(vtId);
			ValidateArch(archReg);
			_map[vtId][archReg] = physReg;
		}
	}

	void RenameMap::Remap(std::shared_ptr<VtId> vtId, std::shared_ptr<ArchRegId> archReg, std::shared_ptr<PhysRegId> physReg)
	{
		Remap(vtId->getValue(), archReg->getValue(), physReg->getValue());
	}

	void RenameMap::Reset()
	{
// C# TO C++ CONVERTER NOTE: The following 'using' block is replaced by its C++ equivalent:
// ORIGINAL LINE: using var access = _reserveAccess == null ? null : _reserveAccess.Invoke(CanonicalStateAccessKind.Write);
		{
			auto access = _reserveAccess == nullptr ? nullptr : _reserveAccess(CanonicalStateAccessKind::Write);
			for (int vt = 0; vt < _smtWays; vt++)
			{
			for (int r = 0; r < ArchRegs; r++)
			{
				_map[vt][r] = r; // identity: arch r → phys r
			}
			}
		}
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal IDisposable? ReserveRestore(int sourceWays)
	std::shared_ptr<Disposable> RenameMap::ReserveRestore(int sourceWays)
	{
		if (sourceWays > _smtWays)
		{
// C# TO C++ CONVERTER TASK: This exception's constructor requires only one argument:
// ORIGINAL LINE: throw new ArgumentException("Restore source has more virtual threads than the target rename map.", "renameMap");
			throw std::invalid_argument("Restore source has more virtual threads than the target rename map.");
		}
		return (_reserveAccess == nullptr ? nullptr : _reserveAccess(CanonicalStateAccessKind::Write));
	}

	void RenameMap::ValidateVt(int vtId)
	{
		if (static_cast<unsigned int>(vtId) >= static_cast<unsigned int>(_smtWays))
		{
			throw ArgumentOutOfRangeException(std::format("VT ID must be in [0, {}].", _smtWays - 1));
		}
	}

	void RenameMap::ValidateArch(int archReg)
	{
		if (static_cast<unsigned int>(archReg) >= ArchRegs)
		{
			throw ArgumentOutOfRangeException(std::format("Architectural register index must be in [0, {}].", ArchRegs - 1));
		}
	}

	CommitMap::CommitMap(int smtWays) : CommitMap(smtWays, nullptr)
	{
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal CommitMap(int smtWays, System.Func<CanonicalStateAccessKind, IDisposable>? reserveAccess)
	CommitMap::CommitMap(int smtWays, std::function<std::shared_ptr<Disposable>(CanonicalStateAccessKind)> reserveAccess)
		: _smtWays(smtWays), _map(RectangularVectors::rectangularIntVector(smtWays, RenameMap::ArchRegs)), _reserveAccess(std::move(reserveAccess))
	{
		if (smtWays < 1)
		{
			throw ArgumentOutOfRangeException("SMT ways must be positive.");
		}
// C# TO C++ CONVERTER NOTE: The following call to the 'RectangularVectors' helper class reproduces the rectangular array initialization that is automatic in C#:
// ORIGINAL LINE: _map = new int[smtWays, RenameMap.ArchRegs];
		Reset();
	}

	int CommitMap::Lookup(int vtId, int archReg)
	{
// C# TO C++ CONVERTER NOTE: The following 'using' block is replaced by its C++ equivalent:
// ORIGINAL LINE: using var access = _reserveAccess == null ? null : _reserveAccess.Invoke(CanonicalStateAccessKind.Read);
		{
			auto access = _reserveAccess == nullptr ? nullptr : _reserveAccess(CanonicalStateAccessKind::Read);
			return _map[vtId][archReg];
		}
	}

	std::shared_ptr<PhysRegId> CommitMap::Lookup(std::shared_ptr<VtId> vtId, std::shared_ptr<ArchRegId> archReg)
	{
		return std::make_shared<PhysRegId>(PhysRegId::Create(Lookup(vtId->getValue(), archReg->getValue())));
	}

	void CommitMap::Commit(int vtId, int archReg, int physReg)
	{
// C# TO C++ CONVERTER NOTE: The following 'using' block is replaced by its C++ equivalent:
// ORIGINAL LINE: using var access = _reserveAccess == null ? null : _reserveAccess.Invoke(CanonicalStateAccessKind.Write);
		{
			auto access = _reserveAccess == nullptr ? nullptr : _reserveAccess(CanonicalStateAccessKind::Write);
			_map[vtId][archReg] = physReg;
		}
	}

	void CommitMap::Commit(std::shared_ptr<VtId> vtId, std::shared_ptr<ArchRegId> archReg, std::shared_ptr<PhysRegId> physReg)
	{
		Commit(vtId->getValue(), archReg->getValue(), physReg->getValue());
	}

	void CommitMap::RestoreInto(std::shared_ptr<RenameMap> renameMap)
	{
		ArgumentNullException::ThrowIfNull(renameMap);
		// Whole target publication precedes the first Remap. The original
		// target/source core callbacks use nonblocking gate acquisition, so
		// cross-core restoration cannot wait for the other core's gate.
// C# TO C++ CONVERTER NOTE: The following 'using' block is replaced by its C++ equivalent:
// ORIGINAL LINE: using var targetAccess = renameMap.ReserveRestore(_smtWays);
		{
			auto targetAccess = renameMap->ReserveRestore(_smtWays);
// C# TO C++ CONVERTER NOTE: The following 'using' block is replaced by its C++ equivalent:
// ORIGINAL LINE: using var access = _reserveAccess == null ? null : _reserveAccess.Invoke(CanonicalStateAccessKind.Read);
			{
				auto access = _reserveAccess == nullptr ? nullptr : _reserveAccess(CanonicalStateAccessKind::Read);
				for (int vt = 0; vt < _smtWays; vt++)
				{
				for (int r = 0; r < RenameMap::ArchRegs; r++)
				{
					renameMap->Remap(vt, r, _map[vt][r]);
	#if defined(TESTING)
					if (renameMap->TestAfterRestoreEntry != nullptr)
					{
						renameMap->TestAfterRestoreEntry->Invoke(vt, r);
					}
	#endif
				}
				}
			}
		}
	}

	void CommitMap::Reset()
	{
// C# TO C++ CONVERTER NOTE: The following 'using' block is replaced by its C++ equivalent:
// ORIGINAL LINE: using var access = _reserveAccess == null ? null : _reserveAccess.Invoke(CanonicalStateAccessKind.Write);
		{
			auto access = _reserveAccess == nullptr ? nullptr : _reserveAccess(CanonicalStateAccessKind::Write);
			for (int vt = 0; vt < _smtWays; vt++)
			{
			for (int r = 0; r < RenameMap::ArchRegs; r++)
			{
				_map[vt][r] = r;
			}
			}
		}
	}

	int FreeList::getAvailable() const
	{
		return _head;
	}

	FreeList::FreeList() : _entries(std::vector<int>(PhysicalRegisterFile::TotalPhysRegs - RenameMap::ArchRegs))
	{
		// Physical registers [ArchRegs .. TotalPhysRegs-1] are available for renaming.
		// Registers [0 .. ArchRegs-1] are pre-committed to the identity map.
		int freeCount = PhysicalRegisterFile::TotalPhysRegs - RenameMap::ArchRegs;
		for (int i = 0; i < freeCount; i++)
		{
			_entries[i] = RenameMap::ArchRegs + i;
		}
		_head = freeCount;
	}

	int FreeList::Allocate()
	{
		if (_head == 0)
		{
			return -1;
		}
		return _entries[--_head];
	}

	bool FreeList::TryAllocate(std::shared_ptr<PhysRegId> &physReg)
	{
		int allocated = Allocate();
		if (allocated < 0)
		{
			physReg = nullptr;
			return false;
		}

		physReg = std::make_shared<PhysRegId>(PhysRegId::Create(allocated));
		return true;
	}

	void FreeList::Release(int physReg)
	{
		if (physReg <= 0 || physReg >= PhysicalRegisterFile::TotalPhysRegs)
		{
			return; // p0 is never released; out-of-range is silently ignored
		}
		if (_head < _entries.size())
		{
			_entries[_head++] = physReg;
		}
	}

	void FreeList::Release(std::shared_ptr<PhysRegId> physReg)
	{
		Release(physReg->getValue());
	}
}
