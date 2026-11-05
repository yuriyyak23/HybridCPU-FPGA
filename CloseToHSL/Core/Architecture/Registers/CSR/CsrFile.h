#pragma once

#include "CsrAccessPolicy.h"
#include <unordered_map>
#include <stdexcept>
#include <functional>
#include <format>
#include <memory>

// ─────────────────────────────────────────────────────────────────────────────
// HybridCPU ISA v4 — CSR Register File
// Phase 08: CSR Layer
// ─────────────────────────────────────────────────────────────────────────────

class Disposable;

namespace YAKSys_Hybrid_CPU::Core::Registers
{
	enum class CanonicalStateAccessKind
	{
		Read,
		Write
	};
	// ─────────────────────────────────────────────────────────────────────────
	// CsrPrivilegeFaultException
	// ─────────────────────────────────────────────────────────────────────────

	/// <summary>
	/// Thrown when a CSR access is denied due to insufficient privilege.
	/// </summary>
	class CsrPrivilegeFaultException final : public std::runtime_error, public std::enable_shared_from_this<CsrPrivilegeFaultException>
	{
	private:
		const unsigned short CsrAddress;
		const PrivilegeLevel RequiredLevel;
		const PrivilegeLevel ActualLevel;
		const bool IsWrite;

	public:
		unsigned short getCsrAddress() const;
		PrivilegeLevel getRequiredLevel() const;
		PrivilegeLevel getActualLevel() const;
		bool isWrite() const;

		CsrPrivilegeFaultException(unsigned short csrAddress, PrivilegeLevel required, PrivilegeLevel actual, bool isWrite);
	};

	// ─────────────────────────────────────────────────────────────────────────
	// CsrUnknownAddressException
	// ─────────────────────────────────────────────────────────────────────────

	/// <summary>
	/// Thrown when an unregistered CSR address is accessed.
	/// </summary>
	class CsrUnknownAddressException final : public std::runtime_error, public std::enable_shared_from_this<CsrUnknownAddressException>
	{
	private:
		const unsigned short CsrAddress;

	public:
		unsigned short getCsrAddress() const;

		CsrUnknownAddressException(unsigned short csrAddress);
	};

	// ─────────────────────────────────────────────────────────────────────────
	// CsrFile
	// ─────────────────────────────────────────────────────────────────────────

	/// <summary>
	/// ISA v4 CSR register file — the canonical typed CSR storage and access control layer.
	/// <para>
	/// Supports all <see cref="CsrAddresses"/> groups: privilege/system, VT/SMT,
	/// capability/configuration, VMX, and performance counters.
	/// </para>
	/// <para>
	/// Access control is enforced per-register via <see cref="CsrAccessPolicy"/>.
	/// Hardware-updated registers (e.g., VtId) can only be written through
	/// <see cref="HardwareWrite"/> — software writes via CSRRW/CSRRS/CSRRC raise
	/// a privilege fault.
	/// </para>
	/// </summary>
	class CsrFile final : public std::enable_shared_from_this<CsrFile>
	{
	private:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly Dictionary<ushort, ulong> _values = new();
		std::unordered_map<unsigned short, unsigned long long> _values = std::unordered_map<unsigned short, unsigned long long>();
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly Dictionary<ushort, CsrAccessPolicy> _policies = new();
		std::unordered_map<unsigned short, CsrAccessPolicy> _policies = std::unordered_map<unsigned short, CsrAccessPolicy>();
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private readonly System.Func<CanonicalStateAccessKind, IDisposable>? _reserveAccess;
		const std::function<std::shared_ptr<Disposable>(CanonicalStateAccessKind)> _reserveAccess;

		/// <summary>
		/// Create a CSR file and register all canonical v4 CSR addresses with
		/// their default values and access policies.
		/// </summary>
	public:
		CsrFile();

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal CsrFile(System.Func<CanonicalStateAccessKind, IDisposable>? reserveAccess)
		CsrFile(std::function<std::shared_ptr<Disposable>(CanonicalStateAccessKind)> reserveAccess);

		// ── Public read/write (software-facing, privilege-checked) ────────

		/// <summary>
		/// Read a CSR value with privilege checking.
		/// </summary>
		/// <param name="address">12-bit CSR address.</param>
		/// <param name="privilege">Current privilege level of the hart.</param>
		/// <returns>64-bit CSR value.</returns>
		unsigned long long Read(unsigned short address, PrivilegeLevel privilege);

		/// <summary>
		/// Write a CSR value with privilege checking.
		/// Hardware-updated registers reject software writes.
		/// </summary>
		/// <param name="address">12-bit CSR address.</param>
		/// <param name="value">64-bit value to write.</param>
		/// <param name="privilege">Current privilege level of the hart.</param>
		void Write(unsigned short address, unsigned long long value, PrivilegeLevel privilege);

		// ── Hardware-only write (bypasses privilege checks) ───────────────

		/// <summary>
		/// Write a CSR value from hardware/firmware context.
		/// This bypasses privilege checks and is used for:
		/// <list type="bullet">
		///   <item>VT dispatch: writing VtId/VtMask on context switch</item>
		///   <item>Hardware counter updates: Cycle, InstrRet, etc.</item>
		///   <item>Firmware reset: initialising read-only CSRs</item>
		/// </list>
		/// </summary>
		void HardwareWrite(unsigned short address, unsigned long long value);

		// ── Direct read (no privilege check — for hardware/internal use) ──

		/// <summary>
		/// Read a CSR value without privilege checking.
		/// Used by hardware subsystems that need direct access (e.g., pipeline
		/// reading VtId for thread dispatch).
		/// </summary>
		unsigned long long DirectRead(unsigned short address);

		// ── Query ─────────────────────────────────────────────────────────

		/// <summary>Returns true if <paramref name="address"/> is a registered CSR.</summary>
		bool IsRegistered(unsigned short address);


		// ── Access control helpers ────────────────────────────────────────

	private:
		static void CheckReadAccess(unsigned short address, CsrAccessPolicy policy, PrivilegeLevel privilege);

		static void CheckWriteAccess(unsigned short address, CsrAccessPolicy policy, PrivilegeLevel privilege);

		// ── Default registration ──────────────────────────────────────────

		void RegisterDefaults();

		void Register(unsigned short address, CsrAccessPolicy policy, unsigned long long defaultValue = 0ULL);
	};
}
