#pragma once
#include <span>

#include "../../../Architecture/Common/YAKSys_Hybrid_CPU.Processor.h"
#include "../Types/MicroOp.h"
namespace YAKSys_Hybrid_CPU::Core::Memory { class PhysicalReadBinding; enum class CpuInstructionTranslationAccessKind : unsigned char; }
#include "../../../../Memory/Timing/MemoryCycleController.h"
#include "../../../Architecture/Registers/Retire/RetireRecord.h"
namespace HybridCPU_ISE::Core { class ReplayToken; }
#include <string>
#include <vector>
#include <stdexcept>
#include <any>
#include <optional>
#include <format>
#include "tangible_exception_helper.h"
#include <memory>

using namespace HybridCPU_ISE::Arch;
using namespace HybridCPU_ISE::CloseToHSL::Memory::Timing;
using namespace HybridCPU_ISE::Core;
using namespace YAKSys_Hybrid_CPU::Arch;
using namespace YAKSys_Hybrid_CPU::Core::Memory;
using namespace YAKSys_Hybrid_CPU::Core::Registers;
using namespace YAKSys_Hybrid_CPU::Core::Registers::Retire;
using namespace YAKSys_Hybrid_CPU::Memory;


namespace YAKSys_Hybrid_CPU::Core
{
	class LoadStoreMicroOp : public MicroOp
	{
	private:
		std::shared_ptr<PhysicalReadBinding> CpuPhysicalReadBinding;
		bool UsesBaseRegisterAddressing = false;
		long long AddressDisplacement = 0;

		std::optional<int> standaloneSecureReadBank;
		// Only the checked single-page standalone read producer uses this local
		// scheduling resource. It never substitutes a global uninitialized bank.
	public:
		void BindStandaloneSecureReadBank(unsigned long long offset, unsigned long long sourceLength);
	private:
		bool _cpuTranslationResolved = false;
		unsigned long long _cpuTranslatedAddress = 0;
		unsigned long long _cpuTranslationOwnerEpoch = 0;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal CpuInstructionTranslationOwner.PhysicalReadBinding? CpuPhysicalReadBinding {get;private set;}
	public:
		std::shared_ptr<PhysicalReadBinding> getCpuPhysicalReadBinding() const;
	private:
		void setCpuPhysicalReadBinding(std::shared_ptr<PhysicalReadBinding> value);
	public:
		void BindCpuPhysicalRead(std::shared_ptr<PhysicalReadBinding> binding);
	private:
		bool _baseRelativeAddressResolved = false;
		unsigned long long _baseRelativeAddress = 0;

		/// <summary>
		/// Runtime-owned scalar base-register plus signed-displacement contour.
		/// Decode publishes the shape; execution resolves the effective address.
		/// </summary>
	public:
		bool getUsesBaseRegisterAddressing() const;
		void setUsesBaseRegisterAddressing(bool value);
		long long getAddressDisplacement() const;
		void setAddressDisplacement(long long value);

		/// <summary>
		/// Memory bank ID computed from the operation's address (Refactoring Pt. 3).
		/// Used by per-VT scoreboard for bank-level conflict detection during FSP scheduling.
		/// The bank geometry follows the live runtime memory subsystem when present.
		/// If runtime memory geometry has not been materialized yet, this returns the
		/// explicit uninitialized contour instead of a synthetic legacy bank id.
		/// </summary>
		int getMemoryBankId() const;

		/// <summary>
		/// Abstract property for the memory address of this operation.
		/// Implemented by LoadMicroOp (Address) and StoreMicroOp (Address).
		/// </summary>
		virtual unsigned long long getMemoryAddress() const = 0;
		virtual void ResolveBaseRelativeAddressForPipeline(std::shared_ptr<Processor::CPU_Core> core);
		virtual void ResolveBaseRelativeAddressForPipeline(unsigned long long baseValue);

	protected:
		unsigned long long ResolveBaseRelativeAddress(std::shared_ptr<Processor::CPU_Core> core, unsigned short baseRegister);

		unsigned long long ResolveCpuTranslatedAddress(std::shared_ptr<Processor::CPU_Core> &core, CpuInstructionTranslationAccessKind accessKind, unsigned char accessSize);

	public:
		bool TryGetCpuTranslatedAddress(unsigned long long &physicalAddress, unsigned long long &ownerEpoch) const;

		unsigned long long ResolveCpuTranslatedAddressForPipeline(std::shared_ptr<Processor::CPU_Core> &core, CpuInstructionTranslationAccessKind accessKind, unsigned char accessSize);

		CanonicalDecodePublicationMode getCanonicalDecodePublication() const override;

	protected:
		static bool HasExactMainMemoryRange(std::shared_ptr<Processor::CPU_Core> &core, unsigned long long address, int size);

		static void ThrowIfMainMemoryRangeUnavailable(std::shared_ptr<Processor::CPU_Core> &core, unsigned long long address, int size, const std::wstring &executionSurface);

		static void ReadMainMemoryExact(std::shared_ptr<Processor::CPU_Core> &core, unsigned long long address, std::vector<unsigned char> &buffer, const std::wstring &executionSurface);

	public:
		static unsigned long long DecodeLoadValue(unsigned int opcode, std::vector<unsigned char> &buffer, unsigned char accessSize, const std::wstring &executionSurface);

	private:
		static unsigned long long DecodeUntypedLoadValue(std::vector<unsigned char> &buffer, int requiredSize);

		static bool TryResolveTypedLoadAccessSize(unsigned short opcode, unsigned char &accessSize);

		/// <summary>
		/// Mark this operation as speculative (stolen from another thread).
		/// Encapsulates speculative state management and allows for future extensions.
		/// </summary>
	public:
		void MarkSpeculative();

		/// <summary>
		/// Clear speculative flag (operation is now executing in owner thread).
		/// </summary>
		void ClearSpeculative();

		/// <summary>
		/// Mark this operation as faulted during speculative execution.
		/// </summary>
		void MarkFaulted();

		/// <summary>
		/// Clear faulted flag (operation ready to retry).
		/// </summary>
		void ClearFaulted();

	protected:
		std::shared_ptr<LoadStoreMicroOp> shared_from_this()
		{
			return std::static_pointer_cast<LoadStoreMicroOp>(MicroOp::shared_from_this());
		}
	};

	/// <summary>
	/// Load micro-operation (memory read)
	/// Updated to use asynchronous memory subsystem
	/// </summary>
	class LoadMicroOp : public LoadStoreMicroOp
	{
	private:
		unsigned long long Address = 0;
		unsigned char Size = 0;
		unsigned short BaseRegID = 0;

	public:
		unsigned long long getAddress() const;
		void setAddress(unsigned long long value);
		unsigned char getSize() const;
		void setSize(unsigned char value);
		unsigned short getBaseRegID() const;
		void setBaseRegID(unsigned short value);
	private:
		unsigned long long _loadedValue = 0;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private MemoryCycleController? _requestController;
		std::shared_ptr<MemoryCycleController> _requestController;
		std::optional<std::shared_ptr<MemoryRequestId>> _controllerRequestId;
		bool _controllerAdmissionBackpressured = false;

		// RF-07.2g runtime evidence for the one scalar-load false contour that
		// is unambiguously a resource wait. The controller-qualified request
		// identity remains owned by this
		// MicroOp; this property does not publish readiness or architectural
		// completion and is not a replacement execution-state model.
	public:
		bool getOwnsPendingMemoryCompletion() const;

		// RF-10.3 no-ID admission wait. This is consumer-local observation of
		// a controller decision; it grants no request identity or queue state.
		bool getHasControllerAdmissionBackpressure() const;

		// RF-07.2o observes, but does not own, the existing FSP silent-squash
		// carrier. Scheduler ProcessFaultedOperations retains cleanup authority.
		bool isSpeculativeFaultSuppressed() const;

		// RF-07.2h evidence for the distinct non-speculative fallback denial.
		// This query owns no readiness/outcome state: it only rechecks the exact
		// backend/range facts that made this concrete Execute call return false.
		bool HasNonSpeculativeFallbackBackendDenial(std::shared_ptr<Processor::CPU_Core> core);

		/// <inheritdoc />
		unsigned long long getMemoryAddress() const override;

		void ResolveBaseRelativeAddressForPipeline(std::shared_ptr<Processor::CPU_Core> core) override;

		void ResolveBaseRelativeAddressForPipeline(unsigned long long baseValue) override;

		LoadMicroOp();

		/// <summary>
		/// Initialize FSP metadata after register IDs and address are set.
		/// Phase: Safety Tags & Certificates - Mandatory safety mask computation.
		/// </summary>
		void InitializeMetadata();

		/// <inheritdoc/>
		void RefreshWriteMetadata() override;

		bool Execute(std::shared_ptr<Processor::CPU_Core> &core) override;

		bool CancelPendingControllerRequest();

	private:
		void ClearControllerRequestState();

	public:
		void EmitWriteBackRetireRecords(std::shared_ptr<Processor::CPU_Core> &core, std::span<RetireRecord> retireRecords, int &retireRecordCount) override;

		bool TryGetPrimaryWriteBackResult(unsigned long long &value) override;

		void CapturePrimaryWriteBackResult(unsigned long long value) override;

		std::wstring GetDescription() override;

	protected:
		std::shared_ptr<LoadMicroOp> shared_from_this()
		{
			return std::static_pointer_cast<LoadMicroOp>(LoadStoreMicroOp::shared_from_this());
		}
	};

	/// <summary>
	/// Store micro-operation (memory write)
	/// Updated to use asynchronous memory subsystem
	/// </summary>
	class StoreMicroOp : public LoadStoreMicroOp
	{
	private:
		unsigned long long Address = 0;
		unsigned long long Value = 0;
		unsigned char Size = 0;
		unsigned short SrcRegID = 0;
		unsigned short BaseRegID = 0;

	public:
		unsigned long long getAddress() const;
		void setAddress(unsigned long long value);
		unsigned long long getValue() const;
		void setValue(unsigned long long value);
		unsigned char getSize() const;
		void setSize(unsigned char value);
		unsigned short getSrcRegID() const;
		void setSrcRegID(unsigned short value);
		unsigned short getBaseRegID() const;
		void setBaseRegID(unsigned short value);
	private:
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private MemoryCycleController? _requestController;
		std::shared_ptr<MemoryCycleController> _requestController;
		std::optional<std::shared_ptr<MemoryRequestId>> _controllerRequestId;
		bool _controllerAdmissionBackpressured = false;

		// RF-07.2i runtime evidence for the one scalar-store false contour that
		// is unambiguously an owned pending readiness request. Exact controller,
		// device/address/size/data identity prevents a mutated MicroOp from
		// borrowing the retry disposition. This is evidence only, not mutable
		// readiness or retirement authority.
	public:
		bool getOwnsPendingWriteCompletion() const;

		bool getHasControllerAdmissionBackpressure() const;

		// RF-07.2k evidence for the malformed scalar-store carrier. This is a
		// structural MicroOp invariant, not a backend/range observation and not
		// a speculative fault-suppression signal. It owns no mutable outcome,
		// readiness, scheduler or retirement state.
		bool getHasInvalidTransferSize() const;

		// RF-07.2l evidence for the existing FSP silent-suppression carrier.
		// This query does not select a squash, release a reservation or mutate
		// completion state: MicroOpScheduler.ProcessFaultedOperations remains
		// the sole FSP owner. Invalid size is intentionally excluded because
		// RF-07.2k owns that malformed carrier as FatalInvariantViolation.
		bool isSpeculativeFaultSuppressed() const;

		// RF-07.2j evidence for the distinct non-speculative fallback denial.
		// This query owns no readiness/outcome state: it only rechecks the exact
		// backend/range facts that made this concrete Execute call return false.
		bool HasNonSpeculativeFallbackBackendDenial(std::shared_ptr<Processor::CPU_Core> core);

		/// <inheritdoc />
		unsigned long long getMemoryAddress() const override;

		void ResolveBaseRelativeAddressForPipeline(std::shared_ptr<Processor::CPU_Core> core) override;

		void ResolveBaseRelativeAddressForPipeline(unsigned long long baseValue) override;

		StoreMicroOp();

		/// <summary>
		/// Initialize FSP metadata after register IDs and address are set.
		/// Phase: Safety Tags & Certificates - Mandatory safety mask computation.
		/// </summary>
		void InitializeMetadata();

		/// <summary>
		/// Create rollback token for store operation.
		/// Captures pre-execution memory state.
		/// </summary>
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public override HybridCPU_ISE.Core.ReplayToken CreateRollbackToken(int ownerThreadId, Processor.MainMemoryArea? mainMemory = null)
		std::shared_ptr<HybridCPU_ISE::Core::ReplayToken> CreateRollbackToken(int ownerThreadId, std::shared_ptr<Processor::MainMemoryArea> mainMemory = nullptr) override;

		bool Execute(std::shared_ptr<Processor::CPU_Core> &core) override;

		bool CancelPendingControllerRequest();

	private:
		void ClearControllerRequestState();

		bool TryCreateStoreBuffer(std::vector<unsigned char> &buffer) const;

	public:
		std::wstring GetDescription() override;

	protected:
		std::shared_ptr<StoreMicroOp> shared_from_this()
		{
			return std::static_pointer_cast<StoreMicroOp>(LoadStoreMicroOp::shared_from_this());
		}
	};

	/// <summary>
	/// Branch scheduling micro-operation (jumps, branches, calls, returns).
	/// V6-final scheduling-only carrier; execution is routed through
	/// <c>ExecutionDispatcherV4.ExecuteControlFlow()</c> via <c>InstructionIR</c>.
	/// Do NOT add execution logic (Execute / Commit) to this class.
}
