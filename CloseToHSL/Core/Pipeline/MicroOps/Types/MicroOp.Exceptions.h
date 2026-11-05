#pragma once

#include <string>
#include <stdexcept>
#include <format>
#include <memory>
#include <exception>






namespace YAKSys_Hybrid_CPU::Core
{
	/// <summary>
	/// Exception thrown when a memory access encounters a page fault or MMU error.
	/// Used in speculative FSP to suppress interrupts for stolen memory operations.
	/// </summary>
	class PageFaultException : public std::runtime_error, public std::enable_shared_from_this<PageFaultException>
	{
	private:
		const unsigned long long FaultAddress;
		const bool IsWrite;
        const std::exception_ptr InnerException;

		/// <summary>
		/// Address that caused the page fault
		/// </summary>
	public:
		unsigned long long getFaultAddress() const;

		/// <summary>
		/// Whether the fault occurred during a read or write operation
		/// </summary>
		bool isWrite() const;
        std::exception_ptr getInnerException() const noexcept;

		PageFaultException(unsigned long long address, bool isWrite);

		PageFaultException(const std::wstring &message, unsigned long long address, bool isWrite);

		PageFaultException(const std::wstring &message, std::runtime_error innerException, unsigned long long address, bool isWrite);
	};

	/// <summary>
	/// Precise architectural exception raised when a non-speculative micro-operation
	/// (issued by its owning VT in program order) fails the DomainTag capability check.
	/// Corresponds to FAULT_DOMAIN_VT[i] in the architectural spec (В§5.4).
	/// FSP-injected ops that fail domain checks are silently squashed to NOP instead.
	/// </summary>
	class DomainFaultException final : public std::runtime_error, public std::enable_shared_from_this<DomainFaultException>
	{
	private:
		const int VirtualThreadId;
		const unsigned long long FaultingPC;
		const unsigned long long OperationDomainTag;
		const unsigned long long ActiveCert;

		/// <summary>Virtual thread index that caused the fault.</summary>
	public:
		int getVirtualThreadId() const;

		/// <summary>Program counter of the faulting instruction.</summary>
		unsigned long long getFaultingPC() const;

		/// <summary>DomainTag of the faulting micro-operation.</summary>
		unsigned long long getOperationDomainTag() const;

		/// <summary>Active domain certificate at fault time (CsrMemDomainCert).</summary>
		unsigned long long getActiveCert() const;

		DomainFaultException(int vtId, unsigned long long pc, unsigned long long opTag, unsigned long long cert);
	};

	/// <summary>
	/// Abstract base class for micro-operations.
	///
	/// Purpose: Decouple decoding from execution by introducing an intermediate representation.
	/// Benefits:
	/// - Easier to add new instructions without modifying pipeline logic
	/// - Clear separation of concerns (decode, execute, commit)
	/// - Facilitates instruction scheduling and optimization
	/// - Simplifies testing (can test MicroOps independently)
	///
	/// Design pattern: Decode в†' MicroOp в†' Execute в†' Commit
}
