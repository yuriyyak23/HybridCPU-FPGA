#include "RetireCoordinator.h"

namespace YAKSys_Hybrid_CPU::Core::Registers::Retire
{

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: public RetireCoordinator(PhysicalRegisterFile physicalRegisters, RenameMap archRenameMap, CommitMap archCommitMap, IReadOnlyList<ArchContextState> archContexts) : this(physicalRegisters, archRenameMap, archCommitMap, archContexts, null)
	RetireCoordinator::RetireCoordinator(std::shared_ptr<PhysicalRegisterFile> physicalRegisters, std::shared_ptr<RenameMap> archRenameMap, std::shared_ptr<CommitMap> archCommitMap, std::shared_ptr<ArchContextCollection> archContexts) : RetireCoordinator(physicalRegisters, archRenameMap, archCommitMap, archContexts, nullptr)
	{
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// C# TO C++ CONVERTER TASK: Throw expressions are not converted by C# to C++ Converter:
// ORIGINAL LINE: internal RetireCoordinator(PhysicalRegisterFile physicalRegisters, RenameMap archRenameMap, CommitMap archCommitMap, IReadOnlyList<ArchContextState> archContexts, System.Func<CanonicalStateAccessKind, IDisposable>? reserveAccess)
	RetireCoordinator::RetireCoordinator(std::shared_ptr<PhysicalRegisterFile> physicalRegisters, std::shared_ptr<RenameMap> archRenameMap, std::shared_ptr<CommitMap> archCommitMap, std::shared_ptr<ArchContextCollection> archContexts, std::function<std::shared_ptr<Disposable>(CanonicalStateAccessKind)> reserveAccess) : VtCount(archContexts ? archContexts->getCount() : throw std::invalid_argument("archContexts")), _physicalRegisters((physicalRegisters != nullptr) ? physicalRegisters : throw std::invalid_argument("physicalRegisters")), _archRenameMap((archRenameMap != nullptr) ? archRenameMap : throw std::invalid_argument("archRenameMap")), _archCommitMap((archCommitMap != nullptr) ? archCommitMap : throw std::invalid_argument("archCommitMap")), _archContexts(archContexts), _reserveAccess(reserveAccess)
	{
	}

	int RetireCoordinator::getVtCount() const
	{
		return VtCount;
	}

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: internal bool HasBacking(PhysicalRegisterFile registers, RenameMap rename, CommitMap commit, IReadOnlyList<ArchContextState> contexts)
	bool RetireCoordinator::HasBacking(std::shared_ptr<PhysicalRegisterFile> registers, std::shared_ptr<RenameMap> rename, std::shared_ptr<CommitMap> commit, std::shared_ptr<ArchContextCollection> contexts)
	{
		// Entries remain current through the original array owner. Only a
		// captured adapter binds individual entry incarnations.
// C# TO C++ CONVERTER TASK: Cannot test for object identity between 2 non-pointer objects in C++:
		return registers == _physicalRegisters && rename == _archRenameMap && commit == _archCommitMap && contexts == _archContexts;
	}

    namespace {
        struct RetireAccessGuard {
            std::shared_ptr<Disposable> lease;
            ~RetireAccessGuard() { if (lease) { try { lease->dispose(); } catch (...) {} } }
        };
    }
    void RetireCoordinator::Retire(RetireRecord const record) {
        RetireAccessGuard access{_reserveAccess ? _reserveAccess(CanonicalStateAccessKind::Write) : nullptr};
        ValidateRecord(record);
        auto targets = ReserveTargets();
        ApplyRetireRecord(record, targets->getContexts());
    }
    void RetireCoordinator::Retire(std::span<const RetireRecord> records) {
        RetireAccessGuard access{_reserveAccess ? _reserveAccess(CanonicalStateAccessKind::Write) : nullptr};
        for (const auto& record : records) ValidateRecord(record);
        auto targets = ReserveTargets();
        for (const auto& record : records) ApplyRetireRecord(record, targets->getContexts());
    }
    void RetireCoordinator::Prevalidate(std::span<const RetireRecord> records) {
        RetireAccessGuard access{_reserveAccess ? _reserveAccess(CanonicalStateAccessKind::Read) : nullptr};
        for (const auto& record : records) ValidateRecord(record);
    }
    std::shared_ptr<RetireCoordinator::StandaloneTargets> RetireCoordinator::ReserveTargets() {
        auto snapshot = _archContexts->Clone();
        if (_reserveAccess) return std::make_shared<StandaloneTargets>(std::move(snapshot), std::vector<std::shared_ptr<Disposable>>{});
        if (snapshot.size() != static_cast<std::size_t>(getVtCount())) throw std::logic_error("Retire target geometry changed.");
        std::vector<std::shared_ptr<Disposable>> leases;
        leases.reserve(snapshot.size());
        try {
            for (const auto& context : snapshot) if (context) leases.push_back(context->ReserveStandaloneRetirement());
            return std::make_shared<StandaloneTargets>(std::move(snapshot), std::move(leases));
        } catch (...) {
            for (auto it = leases.rbegin(); it != leases.rend(); ++it) if (*it) (*it)->dispose();
            throw;
        }
    }
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private void ApplyRetireRecord(in RetireRecord record, IReadOnlyList<ArchContextState> contexts)
	void RetireCoordinator::ApplyRetireRecord(RetireRecord const record, const std::vector<std::shared_ptr<ArchContextState>> &contexts)
	{
		switch (record.getKind())
		{
			case RetireRecordKind::RegisterWrite:
				ApplyRegisterWrite(record, contexts);
				break;

			case RetireRecordKind::PcWrite:
				ApplyPcWrite(record, contexts);
				break;

			default:
				throw std::out_of_range("Unsupported retire record kind.");
		}
	#if defined(TESTING)
		if (TestAfterRetireRecord != nullptr)
		{
			TestAfterRetireRecord->Invoke(record);
		}
	#endif
	}

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private void ApplyRegisterWrite(in RetireRecord record, IReadOnlyList<ArchContextState> contexts)
	void RetireCoordinator::ApplyRegisterWrite(RetireRecord const record, const std::vector<std::shared_ptr<ArchContextState>> &contexts)
	{
		if (_archContexts->empty() || _archRenameMap == nullptr || _archCommitMap == nullptr || _physicalRegisters == nullptr)
		{
			return;
		}

		if (record.getArchReg() == 0)
		{
			return;
		}

		int vtId = record.getVtId();
		std::shared_ptr<ArchContextState> archContext = contexts[vtId];
		archContext->getCommittedRegs()->setIndexer(record.getArchReg(), record.getValue());

		int physReg = _archRenameMap->Lookup(vtId, record.getArchReg());
		if (physReg != 0)
		{
			_physicalRegisters->Write(physReg, record.getValue());
		}

		_archCommitMap->Commit(vtId, record.getArchReg(), physReg == 0 ? record.getArchReg() : physReg);
	}

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private void ApplyPcWrite(in RetireRecord record, IReadOnlyList<ArchContextState> contexts)
	void RetireCoordinator::ApplyPcWrite(RetireRecord const record, const std::vector<std::shared_ptr<ArchContextState>> &contexts)
	{
		if (_archContexts->empty())
		{
			return;
		}

		contexts[record.getVtId()]->setCommittedPc(record.getValue());
	}

	void RetireCoordinator::ValidateRecord(RetireRecord const record)
	{
		if (static_cast<unsigned int>(record.getVtId()) >= static_cast<unsigned int>(getVtCount()))
		{
			int maxVtId = std::max(getVtCount() - 1, 0);
			throw std::out_of_range("Retire record VT identifier outside range.");
		}

		if (record.getKind() == RetireRecordKind::RegisterWrite && static_cast<unsigned int>(record.getArchReg()) >= static_cast<unsigned int>(RenameMap::ArchRegs))
		{
			throw std::out_of_range("Retire record architectural register outside range.");
		}
	}
}
