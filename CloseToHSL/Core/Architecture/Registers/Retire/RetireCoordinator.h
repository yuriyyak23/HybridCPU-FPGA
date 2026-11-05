#pragma once

#include "../Physical/PhysicalRegisterFile.h"
#include "../Architectural/ArchContextState.h"
#include "../CSR/CsrFile.h"
#include "RetireRecord.h"
#include <vector>
#include <span>
#include <cmath>
#include <stdexcept>
#include <functional>
#include <format>
#include "tangible_exception_helper.h"
#include <memory>
#include "../../../../../tangible_disposable_interface.h"

// C# TO C++ CONVERTER NOTE: Forward class declarations:


namespace YAKSys_Hybrid_CPU::Core::Registers::Retire
{
	/// <summary>
	/// Applies retire records to the unified committed architectural state.
	/// </summary>
	class RetireCoordinator final : public std::enable_shared_from_this<RetireCoordinator>
	{
	private:
		const int VtCount;

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private readonly PhysicalRegisterFile? _physicalRegisters;
		const std::shared_ptr<PhysicalRegisterFile> _physicalRegisters;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private readonly RenameMap? _archRenameMap;
		const std::shared_ptr<RenameMap> _archRenameMap;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private readonly CommitMap? _archCommitMap;
		const std::shared_ptr<CommitMap> _archCommitMap;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly IReadOnlyList<ArchContextState>? _archContexts;
		const std::shared_ptr<ArchContextCollection> _archContexts;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private readonly System.Func<CanonicalStateAccessKind, IDisposable>? _reserveAccess;
		const std::function<std::shared_ptr<Disposable>(CanonicalStateAccessKind)> _reserveAccess;

	public:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: public RetireCoordinator(PhysicalRegisterFile physicalRegisters, RenameMap archRenameMap, CommitMap archCommitMap, IReadOnlyList<ArchContextState> archContexts) : this(physicalRegisters, archRenameMap, archCommitMap, archContexts, null)
		RetireCoordinator(std::shared_ptr<PhysicalRegisterFile> physicalRegisters, std::shared_ptr<RenameMap> archRenameMap, std::shared_ptr<CommitMap> archCommitMap, std::shared_ptr<ArchContextCollection> archContexts);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// C# TO C++ CONVERTER TASK: Throw expressions are not converted by C# to C++ Converter:
// ORIGINAL LINE: internal RetireCoordinator(PhysicalRegisterFile physicalRegisters, RenameMap archRenameMap, CommitMap archCommitMap, IReadOnlyList<ArchContextState> archContexts, System.Func<CanonicalStateAccessKind, IDisposable>? reserveAccess)
		RetireCoordinator(std::shared_ptr<PhysicalRegisterFile> physicalRegisters, std::shared_ptr<RenameMap> archRenameMap, std::shared_ptr<CommitMap> archCommitMap, std::shared_ptr<ArchContextCollection> archContexts, std::function<std::shared_ptr<Disposable>(CanonicalStateAccessKind)> reserveAccess);

		int getVtCount() const;

#if defined(TESTING)
		// Observation after the original effect; never a publication decision.
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal System.Action<RetireRecord>? TestAfterRetireRecord;
		std::function<void(RetireRecord)> TestAfterRetireRecord;
#endif

		// Checked by the original core reservation before any committed array
		// write, including when a retained coordinator survives a raw rebind.
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: internal bool HasBacking(PhysicalRegisterFile registers, RenameMap rename, CommitMap commit, IReadOnlyList<ArchContextState> contexts)
		bool HasBacking(std::shared_ptr<PhysicalRegisterFile> registers, std::shared_ptr<RenameMap> rename, std::shared_ptr<CommitMap> commit, std::shared_ptr<ArchContextCollection> contexts);

		void Retire(RetireRecord const record);

		void Retire(std::span<const RetireRecord> records);

		/// <summary>
		/// Validates the complete bounded register/PC subspan without publishing it.
		/// The WB retire-window owner uses this before it releases resources or
		/// mutates retire counters for the selected older prefix.
		/// </summary>
		void Prevalidate(std::span<const RetireRecord> records);

		// Bind standalone source targets before any prefix. These leases only
		// prevent late core attachment; they never grant access to core-owned state.
	private:
        class StandaloneTargets final {
            std::vector<std::shared_ptr<ArchContextState>> contexts_;
            std::vector<std::shared_ptr<Disposable>> leases_;
        public:
            StandaloneTargets(std::vector<std::shared_ptr<ArchContextState>> contexts,
                              std::vector<std::shared_ptr<Disposable>> leases)
                : contexts_(std::move(contexts)), leases_(std::move(leases)) {}
            StandaloneTargets(const StandaloneTargets&) = delete;
            ~StandaloneTargets() {
                for (auto it = leases_.rbegin(); it != leases_.rend(); ++it)
                    if (*it) { try { (*it)->dispose(); } catch (...) {} }
            }
            const std::vector<std::shared_ptr<ArchContextState>>& getContexts() const { return contexts_; }
        };
        std::shared_ptr<StandaloneTargets> ReserveTargets();
	private:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private void ApplyRetireRecord(in RetireRecord record, IReadOnlyList<ArchContextState> contexts)
		void ApplyRetireRecord(RetireRecord const record, const std::vector<std::shared_ptr<ArchContextState>> &contexts);

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private void ApplyRegisterWrite(in RetireRecord record, IReadOnlyList<ArchContextState> contexts)
		void ApplyRegisterWrite(RetireRecord const record, const std::vector<std::shared_ptr<ArchContextState>> &contexts);

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private void ApplyPcWrite(in RetireRecord record, IReadOnlyList<ArchContextState> contexts)
		void ApplyPcWrite(RetireRecord const record, const std::vector<std::shared_ptr<ArchContextState>> &contexts);

		void ValidateRecord(RetireRecord const record);
	};
}
