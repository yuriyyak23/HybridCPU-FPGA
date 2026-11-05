#pragma once

#include "MicroOp.h"
#include "../../PipelineEvent.h"
#include "../../../Architecture/Common/YAKSys_Hybrid_CPU.Processor.h"
#include "../../Safety/YAKSys_Hybrid_CPU.Core.SafetyVerifier.h"
#include "../../../Runtime/Events/Hypercalls/VirtualizationOperandSnapshot.h"
#include "../../../Runtime/Completion/Records/DomainHypercallRetireOwner.h"
#include "../../../Virtualization/Compatibility/Frontend/Retire/VmxRetireModel.h"
#include "../../../Runtime/Events/Hypercalls/DomainHypercallCanonicalComposition.h"
#include "../../../Runtime/Events/VmRead/VmReadScalarDeliveryCanonicalComposition.h"
#include "../../../Runtime/Events/Hypercalls/DomainHypercallRuntimeExecutor.h"
#include "../../../Architecture/Registers/Retire/RetireRecord.h"
#include <string>
#include <vector>
#include <span>
#include <optional>
#include <format>
#include "tangible_exception_helper.h"
#include <memory>

using namespace HybridCPU_ISE::Arch;

using namespace YAKSys_Hybrid_CPU::Core::Memory;
using namespace YAKSys_Hybrid_CPU::Core::Pipeline::MicroOps;
using namespace YAKSys_Hybrid_CPU::Core::Registers;
using namespace YAKSys_Hybrid_CPU::Core::Registers::Retire;


namespace YAKSys_Hybrid_CPU::Core
{
	class VirtualizationAdmissionCertificate;
	// ─────────────────────────────────────────────────────────────────────────
	// TrapMicroOp — Blueprint §7 (replaces SystemNopMicroOp/NopMicroOp fallback)
	// ─────────────────────────────────────────────────────────────────────────



	/// <summary>
	/// Trap micro-operation: encodes an unrecognised or privileged-fault instruction.
	/// <para>
	/// Replaces the legacy <c>SystemNopMicroOp</c>/<c>NopMicroOp</c> fallback for
	/// unregistered opcodes. On <see cref="Execute"/> the trap is recorded; the pipeline
	/// FSM is responsible for redirecting control flow to the trap-handler vector.
	/// </para>
	/// Blueprint §7: "Удалить SystemNopMicroOp класс как placeholder,
	/// заменив его на TrapMicroOp или реальное исполнение системных инструкций."
	/// Blueprint §5 (decoder): "legacyOp (для незарегистрированных) переделать в нормальную
	/// ошибку/исключение, а не SystemNop."
	/// </summary>
	class TrapMicroOp final : public MicroOp
	{
	private:
		unsigned int UndecodedOpCode = 0;
		std::wstring TrapReason;
		int ProjectedMemoryBankIntent = -1;

		static constexpr unsigned long long IllegalInstructionCauseCode = 2;

		/// <summary>The original undecoded opcode that triggered the trap.</summary>
	public:
		unsigned int getUndecodedOpCode() const;
		void setUndecodedOpCode(unsigned int value);

		/// <summary>Optional human-readable reason for the trap (e.g., decode exception message).</summary>
		std::wstring getTrapReason() const;
		void setTrapReason(const std::wstring &value);

		/// <summary>
		/// Projected memory-bank intent captured from the canonical memory materialization
		/// attempt before the carrier fail-closed trapped. Non-memory trap contours keep -1.
		/// </summary>
		int getProjectedMemoryBankIntent() const;
		void setProjectedMemoryBankIntent(int value);

		TrapMicroOp();

		CanonicalDecodePublicationMode getCanonicalDecodePublication() const override;

		bool Execute(std::shared_ptr<Processor::CPU_Core> &core) override;

		std::shared_ptr<Pipeline::TrapEntryEvent> CreatePipelineEvent(unsigned long long bundleSerial = 0);

		std::wstring GetDescription() override;

	protected:
		std::shared_ptr<TrapMicroOp> shared_from_this()
		{
			return std::static_pointer_cast<TrapMicroOp>(MicroOp::shared_from_this());
		}
	};

	// VmxMicroOp — Blueprint Step 2 (VMX instruction plane)
	// ─────────────────────────────────────────────────────────────────────────

	/// <summary>
	/// MicroOp that publishes a fail-closed typed effect for a frozen VMX opcode.
	/// <para>
	/// All VMX instructions are fully serialised (<see cref="Arch.SerializationClass.VmxSerial"/>)
	/// and pinned to the system-singleton slot. They require Machine-mode privilege.
	/// </para>
	/// <para>
	/// The removed legacy frontend has no replacement; opcode admission must
	/// be restored through generic runtime descriptors in a future layer.
	/// </para>
	/// </summary>
	class VmxMicroOp final : public MicroOp
	{
	private:
		std::shared_ptr<VirtualizationAdmissionCertificate> VirtualizationAdmission;
		std::shared_ptr<YAKSys_Hybrid_CPU::Core::VirtualizationOperandSnapshot> VirtualizationOperandSnapshot;
		std::optional<std::shared_ptr<DomainHypercallExecutionResult>> ExactHypercallExecutionResult;
		std::optional<std::shared_ptr<DomainHypercallCompletionPublicationResult>> ExactHypercallCompletionPublication;
		std::shared_ptr<DomainHypercallRetireOwner::VirtualizationRetireGrant> ExactHypercallRetireGrant;
		unsigned long long ExactHypercallRetireWindowIdentity = 0;
		unsigned long long ExactHypercallRetireOrderEpoch = 0;
		std::optional<std::shared_ptr<Pipeline::MicroOps::InstructionIR>> Instruction;
		unsigned char Rd = 0;
		unsigned char Rs1 = 0;
		unsigned char Rs2 = 0;

		std::optional<VmxRetireEffect> _resolvedRetireEffect;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private DomainHypercallCanonicalComposition.ExecutionDispatch? _exactHypercallDispatch;
		std::shared_ptr<DomainHypercallCanonicalComposition::ExecutionDispatch> _exactHypercallDispatch;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private VmReadScalarResultReceipt? _vmReadScalarResultReceipt;
		std::shared_ptr<VmReadScalarResultReceipt> _vmReadScalarResultReceipt;
		unsigned long long _vmReadScalarResult = 0;
		bool _vmReadScalarResultReady = false;

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal SafetyVerifier.VirtualizationAdmissionCertificate? VirtualizationAdmission {get;private set;}
	public:
		std::shared_ptr<VirtualizationAdmissionCertificate> getVirtualizationAdmission() const;
	private:
		void setVirtualizationAdmission(std::shared_ptr<VirtualizationAdmissionCertificate> value);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal VirtualizationOperandSnapshot? VirtualizationOperandSnapshot {get;private set;}
	public:
		std::shared_ptr<YAKSys_Hybrid_CPU::Core::VirtualizationOperandSnapshot> getVirtualizationOperandSnapshot() const;
	private:
		void setVirtualizationOperandSnapshot(std::shared_ptr<YAKSys_Hybrid_CPU::Core::VirtualizationOperandSnapshot> value);

	public:
		std::optional<std::shared_ptr<DomainHypercallExecutionResult>> getExactHypercallExecutionResult() const;
	private:
		void setExactHypercallExecutionResult(const std::optional<std::shared_ptr<DomainHypercallExecutionResult>> &value);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal DomainHypercallRuntimeExecutor.ExecutionReceipt? ExactHypercallExecutionReceipt
	public:
		std::shared_ptr<DomainHypercallRuntimeExecutor::ExecutionReceipt> getExactHypercallExecutionReceipt() const;

		std::optional<std::shared_ptr<DomainHypercallCompletionPublicationResult>> getExactHypercallCompletionPublication() const;
	private:
		void setExactHypercallCompletionPublication(const std::optional<std::shared_ptr<DomainHypercallCompletionPublicationResult>> &value);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal DomainHypercallRetireOwner.VirtualizationRetireGrant? ExactHypercallRetireGrant {get;private set;}
	public:
		std::shared_ptr<DomainHypercallRetireOwner::VirtualizationRetireGrant> getExactHypercallRetireGrant() const;
	private:
		void setExactHypercallRetireGrant(std::shared_ptr<DomainHypercallRetireOwner::VirtualizationRetireGrant> value);

	public:
		unsigned long long getExactHypercallRetireWindowIdentity() const;
	private:
		void setExactHypercallRetireWindowIdentity(unsigned long long value);
	public:
		unsigned long long getExactHypercallRetireOrderEpoch() const;
	private:
		void setExactHypercallRetireOrderEpoch(unsigned long long value);
	public:
		bool getHasVmReadScalarResultReceipt() const;

		/// <summary>Decoded instruction IR forwarded to the VMX execution unit.</summary>
		std::optional<std::shared_ptr<Pipeline::MicroOps::InstructionIR>> getInstruction() const;
		void setInstruction(const std::optional<std::shared_ptr<Pipeline::MicroOps::InstructionIR>> &value);

		/// <summary>Destination register index (Rd) — used for VMREAD result writeback.</summary>
		unsigned char getRd() const;
		void setRd(unsigned char value);

		/// <summary>Source register 1 index (Rs1) — VMCS field selector / pointer register.</summary>
		unsigned char getRs1() const;
		void setRs1(unsigned char value);

		/// <summary>Source register 2 index (Rs2) — VMWRITE value register.</summary>
		unsigned char getRs2() const;
		void setRs2(unsigned char value);

		VmxMicroOp();

		CanonicalDecodePublicationMode getCanonicalDecodePublication() const override;

		void RefreshWriteMetadata() override;

		bool Execute(std::shared_ptr<Processor::CPU_Core> &core) override;

		std::wstring GetDescription() override;

		// ── Helper ────────────────────────────────────────────────────────────

		VmxRetireEffect CreateRetireEffect();

		bool TryResolveFrozenOperation(VmxOperationKind &operation);

		void AttachVirtualizationAdmission(std::shared_ptr<VirtualizationAdmissionCertificate> certificate);

		void AttachVirtualizationOperandSnapshot(std::shared_ptr<YAKSys_Hybrid_CPU::Core::VirtualizationOperandSnapshot> snapshot);

		void AttachExactHypercallExecutionDispatch(std::shared_ptr<DomainHypercallCanonicalComposition::ExecutionDispatch> dispatch);

		void AttachExactHypercallRetireGrant(std::shared_ptr<DomainHypercallRetireOwner::VirtualizationRetireGrant> grant, unsigned long long retireWindowIdentity, unsigned long long orderEpoch);

		void AttachVmReadScalarResultReceipt(std::shared_ptr<VmReadScalarResultReceipt> receipt, unsigned long long fieldSelector);

		bool TryGetPrimaryWriteBackResult(unsigned long long &value) override;

		void CapturePrimaryWriteBackResult(unsigned long long value) override;

		void EmitWriteBackRetireRecords(std::shared_ptr<Processor::CPU_Core> &core, std::span<RetireRecord> retireRecords, int &retireRecordCount) override;

	private:
		static bool HasArchitecturalRegister(unsigned char registerId);

		static unsigned char NormalizeInstructionIrRegister(unsigned char registerId);

		void InitializeMetadata();

		std::shared_ptr<Pipeline::MicroOps::InstructionIR> BuildInstructionIR();

	protected:
		std::shared_ptr<VmxMicroOp> shared_from_this()
		{
			return std::static_pointer_cast<VmxMicroOp>(MicroOp::shared_from_this());
		}
	};
}
