#pragma once

#include "../CSR/CsrFile.h"
#include "../Architectural/RegisterIdentity.h"
#include <vector>
#include <stdexcept>
#include <functional>
#include <format>
#include "tangible_exception_helper.h"
#include <memory>
#include "../../../../../tangible_rectangular_vectors.h"

// ─────────────────────────────────────────────────────────────────────────────
// HybridCPU ISA v4 — Physical Register File
// Blueprint Refactoring Step 1: PRF + Rename
// ─────────────────────────────────────────────────────────────────────────────


namespace YAKSys_Hybrid_CPU::Core::Registers
{
	/// <summary>
	/// Flat physical register file shared across all virtual threads.
	/// <para>
	/// Holds <see cref="TotalPhysRegs"/> × 64-bit physical registers.
	/// Physical register 0 is reserved as the zero register (always reads 0).
	/// Allocation/deallocation is managed by <see cref="FreeList"/>.
	/// </para>
	/// </summary>
	class PhysicalRegisterFile final : public std::enable_shared_from_this<PhysicalRegisterFile>
	{
		/// <summary>Total number of physical registers (shared PRF, 128 entries).</summary>
	public:
		static constexpr int TotalPhysRegs = 128;
		static_assert(TotalPhysRegs == PhysRegId::RegisterCount);

	private:
		std::vector<unsigned long long> _physRegs = std::vector<unsigned long long>(TotalPhysRegs);
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private readonly System.Func<CanonicalStateAccessKind, IDisposable>? _reserveAccess;
		const std::function<std::shared_ptr<Disposable>(CanonicalStateAccessKind)> _reserveAccess;

	public:
		PhysicalRegisterFile();

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal PhysicalRegisterFile(System.Func<CanonicalStateAccessKind, IDisposable>? reserveAccess)
		PhysicalRegisterFile(std::function<std::shared_ptr<Disposable>(CanonicalStateAccessKind)> reserveAccess);

		/// <summary>Read the value of physical register <paramref name="physId"/>.</summary>
		/// <param name="physId">Physical register index [0, <see cref="TotalPhysRegs"/>).</param>
		unsigned long long Read(int physId);

		unsigned long long Read(std::shared_ptr<PhysRegId> physId);

		/// <summary>Write <paramref name="value"/> to physical register <paramref name="physId"/>.</summary>
		/// <param name="physId">Physical register index [0, <see cref="TotalPhysRegs"/>).</param>
		/// <param name="value">64-bit value to store.</param>
		void Write(int physId, unsigned long long value);

		void Write(std::shared_ptr<PhysRegId> physId, unsigned long long value);

	private:
		static void ValidateId(int physId);
	};

	/// <summary>
	/// Per-VT architectural→physical register rename map.
	/// <para>
	/// Maintains one mapping table per virtual thread:
	/// <c>RenameMap[vtId][archRegId]</c> → physical register index.
	/// At initialization each architectural register <c>i</c> maps to physical register <c>i</c>
	/// (identity map), reserving the first <see cref="ArchRegs"/> physical registers.
	/// </para>
	/// </summary>
	class RenameMap final : public std::enable_shared_from_this<RenameMap>
	{
		/// <summary>Number of architectural integer registers (x0–x31).</summary>
	public:
		static constexpr int ArchRegs = 32;

	private:
		const int _smtWays;
		std::vector<std::vector<int>> _map;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private readonly System.Func<CanonicalStateAccessKind, IDisposable>? _reserveAccess;
		const std::function<std::shared_ptr<Disposable>(CanonicalStateAccessKind)> _reserveAccess;
#if defined(TESTING)
	public:
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal System.Action<int, int>? TestAfterRestoreEntry;
		std::function<void(int, int)> TestAfterRestoreEntry;
#endif
	public:

		/// <summary>
		/// Initialise rename maps for <paramref name="smtWays"/> virtual threads.
		/// Each VT starts with the identity mapping: arch[i] → phys[i].
		/// </summary>
		/// <param name="smtWays">Number of virtual threads (typically 4).</param>
		RenameMap(int smtWays);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal RenameMap(int smtWays, System.Func<CanonicalStateAccessKind, IDisposable>? reserveAccess)
		RenameMap(int smtWays, std::function<std::shared_ptr<Disposable>(CanonicalStateAccessKind)> reserveAccess);

		/// <summary>
		/// Return the physical register index that architectural register
		/// <paramref name="archReg"/> maps to for virtual thread <paramref name="vtId"/>.
		/// </summary>
		int Lookup(int vtId, int archReg);

		std::shared_ptr<PhysRegId> Lookup(std::shared_ptr<VtId> vtId, std::shared_ptr<ArchRegId> archReg);

		/// <summary>
		/// Update the rename mapping: arch register <paramref name="archReg"/> for VT
		/// <paramref name="vtId"/> now maps to physical register <paramref name="physReg"/>.
		/// </summary>
		void Remap(int vtId, int archReg, int physReg);

		void Remap(std::shared_ptr<VtId> vtId, std::shared_ptr<ArchRegId> archReg, std::shared_ptr<PhysRegId> physReg);

		/// <summary>Reset all VT rename maps to the identity mapping.</summary>
		void Reset();

		// The target owns its geometry and original core access callback.
		// Validate before entering the write scope so an impossible copy cannot
		// invalidate captures or publish a prefix. Larger targets retain their
		// untouched VT rows, as in the original RestoreInto implementation.
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal IDisposable? ReserveRestore(int sourceWays)
		std::shared_ptr<Disposable> ReserveRestore(int sourceWays);

	private:
		void ValidateVt(int vtId);

		static void ValidateArch(int archReg);
	};

	/// <summary>
	/// Commit map — a snapshot of the rename map at the commit boundary.
	/// Used to restore the precise architectural state on a branch mispredict or
	/// precise exception: the commit map represents the last architecturally retired
	/// register assignments.
	/// </summary>
	class CommitMap final : public std::enable_shared_from_this<CommitMap>
	{
	private:
		const int _smtWays;
		std::vector<std::vector<int>> _map;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private readonly System.Func<CanonicalStateAccessKind, IDisposable>? _reserveAccess;
		const std::function<std::shared_ptr<Disposable>(CanonicalStateAccessKind)> _reserveAccess;

		/// <summary>Initialise a commit map for <paramref name="smtWays"/> virtual threads.</summary>
	public:
		CommitMap(int smtWays);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal CommitMap(int smtWays, System.Func<CanonicalStateAccessKind, IDisposable>? reserveAccess)
		CommitMap(int smtWays, std::function<std::shared_ptr<Disposable>(CanonicalStateAccessKind)> reserveAccess);

		/// <summary>Retrieve the committed physical register for arch register <paramref name="archReg"/> of VT <paramref name="vtId"/>.</summary>
		int Lookup(int vtId, int archReg);

		std::shared_ptr<PhysRegId> Lookup(std::shared_ptr<VtId> vtId, std::shared_ptr<ArchRegId> archReg);

		/// <summary>Record that arch register <paramref name="archReg"/> of VT <paramref name="vtId"/> has been committed to physical <paramref name="physReg"/>.</summary>
		void Commit(int vtId, int archReg, int physReg);

		void Commit(std::shared_ptr<VtId> vtId, std::shared_ptr<ArchRegId> archReg, std::shared_ptr<PhysRegId> physReg);

		/// <summary>Restore the <see cref="RenameMap"/> from the commit snapshot (rollback).</summary>
		void RestoreInto(std::shared_ptr<RenameMap> renameMap);

		/// <summary>Reset all entries to the identity mapping.</summary>
		void Reset();
	};

	/// <summary>
	/// Free-list of available physical register indices.
	/// <para>
	/// At start-up the free list is seeded with all physical registers above the
	/// identity-mapped region (<see cref="RenameMap.ArchRegs"/> … <see cref="PhysicalRegisterFile.TotalPhysRegs"/>−1).
	/// <see cref="Allocate"/> pops a free register; <see cref="Release"/> pushes one back.
	/// Physical register 0 is never returned — it is the hardwired-zero register.
	/// </para>
	/// </summary>
	class FreeList final : public std::enable_shared_from_this<FreeList>
	{
	private:
		std::vector<int> _entries;
		int _head = 0;

		/// <summary>
		/// Number of registers currently available for allocation.
		/// </summary>
	public:
		int getAvailable() const;

		/// <summary>Initialise the free list, seeding it with the speculative physical registers.</summary>
		FreeList();

		/// <summary>
		/// Allocate a free physical register.
		/// </summary>
		/// <returns>Physical register index, or -1 if the free list is exhausted.</returns>
		int Allocate();

		bool TryAllocate(std::shared_ptr<PhysRegId> &physReg);

		/// <summary>
		/// Return physical register <paramref name="physReg"/> to the free list.
		/// </summary>
		void Release(int physReg);

		void Release(std::shared_ptr<PhysRegId> physReg);
	};
}
