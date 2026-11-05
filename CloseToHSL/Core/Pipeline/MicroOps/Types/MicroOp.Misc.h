#pragma once

#include "MicroOp.h"
#include "../../../Architecture/Common/YAKSys_Hybrid_CPU.Processor.h"
#include "../../../../../NonRTL/Arch/Compat/HybridCPU_ISE.Arch.VLIW_Instruction.h"
#include "../Memory/MicroOp.LoadStore.h"
#include "../Control/MicroOp.Control.h"
#include "../../../Diagnostics/InstructionRegistry.Types.h"
#include "../../../Architecture/Registers/Retire/RetireRecord.h"
#include <string>
#include <vector>
#include <cmath>
#include <any>
#include <format>
#include <memory>
#include <optional>

using namespace HybridCPU_ISE::Arch;

using namespace YAKSys_Hybrid_CPU::Core::Memory;
using namespace YAKSys_Hybrid_CPU::Core::Registers;
using namespace YAKSys_Hybrid_CPU::Core::Registers::Retire;


namespace YAKSys_Hybrid_CPU::Core
{
	class NopMicroOp : public MicroOp
	{
	public:
		NopMicroOp();

		CanonicalDecodePublicationMode getCanonicalDecodePublication() const override;

		bool Execute(std::shared_ptr<Processor::CPU_Core> &core) override;

		std::wstring GetDescription() override;

	protected:
		std::shared_ptr<NopMicroOp> shared_from_this()
		{
			return std::static_pointer_cast<NopMicroOp>(MicroOp::shared_from_this());
		}
	};

	/// <summary>
	/// Generic micro-operation for instructions that need custom execution
	/// </summary>
	class GenericMicroOp : public MicroOp
	{
	private:
		VLIW_Instruction Instruction;

	public:
		VLIW_Instruction getInstruction() const;
		void setInstruction(VLIW_Instruction value);

		GenericMicroOp();

		CanonicalDecodePublicationMode getCanonicalDecodePublication() const override;

		bool Execute(std::shared_ptr<Processor::CPU_Core> &core) override;

		std::wstring GetDescription() override;

	protected:
		std::shared_ptr<GenericMicroOp> shared_from_this()
		{
			return std::static_pointer_cast<GenericMicroOp>(MicroOp::shared_from_this());
		}
	};

	/// <summary>
	/// Retire-authoritative Atomic micro-operation.
	/// Execute resolves a non-mutating retire effect; memory mutation happens only
	/// when the WB retire contour applies that effect.

	class AtomicMicroOp final : public LoadStoreMicroOp
	{
	private:
		unsigned long long Address = 0;
		unsigned short BaseRegID = 0;
		unsigned short SrcRegID = 0;
		unsigned char Size = 0;
		bool AcquireOrdering = false;
		bool ReleaseOrdering = false;

		std::optional<AtomicRetireEffect> _resolvedRetireEffect;

	public:
		unsigned long long getAddress() const;
		void setAddress(unsigned long long value);

		unsigned short getBaseRegID() const;
		void setBaseRegID(unsigned short value);

		unsigned short getSrcRegID() const;
		void setSrcRegID(unsigned short value);

		unsigned char getSize() const;
		void setSize(unsigned char value);

		bool getAcquireOrdering() const;
		void setAcquireOrdering(bool value);

		bool getReleaseOrdering() const;
		void setReleaseOrdering(bool value);

		unsigned long long getMemoryAddress() const override;

		AtomicMicroOp();

		void InitializeMetadata();

		void RefreshWriteMetadata() override;

		bool Execute(std::shared_ptr<Processor::CPU_Core> &core) override;

		AtomicRetireEffect CreateRetireEffect();

		std::wstring GetDescription() override;

	private:
		bool getUsesSourceRegister() const;

	protected:
		std::shared_ptr<AtomicMicroOp> shared_from_this()
		{
			return std::static_pointer_cast<AtomicMicroOp>(LoadStoreMicroOp::shared_from_this());
		}
	};

	/// <summary>
	/// Fail-closed placeholder for retained custom-accelerator descriptor contours.
	/// The executable runtime carrier is not implemented truthfully yet, so any
	/// attempt to execute this MicroOp must trap at the API boundary instead of
	/// publishing success.

	class CustomAcceleratorMicroOp : public MicroOp
	{
	private:
		std::shared_ptr<ICustomAccelerator> Accelerator = nullptr;
		std::vector<unsigned long long> Operands = {};
		std::vector<unsigned char> Config = {};

	public:
		std::shared_ptr<ICustomAccelerator> getAccelerator() const;
		void setAccelerator(std::shared_ptr<ICustomAccelerator> value);
		std::vector<unsigned long long> getOperands() const;
		void setOperands(const std::vector<unsigned long long> &value);
		std::vector<unsigned char> getConfig() const;
		void setConfig(const std::vector<unsigned char> &value);
	private:
		std::vector<unsigned long long> _results = {};

	public:
		CustomAcceleratorMicroOp();

		CanonicalDecodePublicationMode getCanonicalDecodePublication() const override;

		/// <summary>
		/// Initialize retained compatibility metadata for a custom accelerator operation.
		/// Call this after setting up Accelerator and operand register IDs.
		/// Phase: Safety Tags & Certificates - retained fail-closed contour metadata.
		/// </summary>
		void InitializeMetadata(int acceleratorId, std::vector<int> &inputRegIds, int outputRegId);

		bool Execute(std::shared_ptr<Processor::CPU_Core> &core) override;

		void EmitWriteBackRetireRecords(std::shared_ptr<Processor::CPU_Core> &core, std::span<RetireRecord> retireRecords, int &retireRecordCount) override;

		bool TryGetPrimaryWriteBackResult(unsigned long long &value) override;

		void CapturePrimaryWriteBackResult(unsigned long long value) override;

		std::wstring GetDescription() override;

	protected:
		std::shared_ptr<CustomAcceleratorMicroOp> shared_from_this()
		{
			return std::static_pointer_cast<CustomAcceleratorMicroOp>(MicroOp::shared_from_this());
		}
	};

	/// <summary>
	/// Halt micro-operation — stops pipeline execution (CPU_Stop).
	/// Sets the active live PC past memory end so the pipeline loop terminates.
	/// </summary>
	class HaltMicroOp : public MicroOp
	{
	public:
		HaltMicroOp();

		CanonicalDecodePublicationMode getCanonicalDecodePublication() const override;

		bool Execute(std::shared_ptr<Processor::CPU_Core> &core) override;

		std::wstring GetDescription() override;

	protected:
		std::shared_ptr<HaltMicroOp> shared_from_this()
		{
			return std::static_pointer_cast<HaltMicroOp>(MicroOp::shared_from_this());
		}
	};

	/// <summary>
	/// Retained move-family micro-operation.
	/// Handles canonical single-destination register Move (DT=0) and
	/// Move_Num (DT=1). Retired DT=2/3 memory shapes must be canonicalized
	/// to Load/Store before runtime execution, while DT=4/5 fail closed.
	/// </summary>
	class MoveMicroOp : public MicroOp
	{
	private:
		VLIW_Instruction Instruction;

	public:
		VLIW_Instruction getInstruction() const;
		void setInstruction(VLIW_Instruction value);
	private:
		unsigned char _projectedDataType = 0;
		unsigned short _projectedReg1Id = 0;
		unsigned short _projectedReg2Id = 0;
		unsigned long long _projectedPrimaryPayload = 0;
		bool _hasProjectedMoveShape = false;
		unsigned long long _loadedValue = 0;
		unsigned long long _primaryWriteValue = 0;
	public:
		MoveMicroOp();

		CanonicalDecodePublicationMode getCanonicalDecodePublication() const override;

		void RefreshWriteMetadata() override;

		void ApplyCanonicalRuntimeMoveShapeProjection(unsigned char dataType, unsigned short reg1Id, unsigned short reg2Id, unsigned long long primaryPayload);

	private:
		unsigned char ResolveMoveDataType();

		unsigned short ResolveMoveReg1Id();

		unsigned short ResolveMoveReg2Id();

		unsigned long long ResolveMovePrimaryPayload();

		void InitializeMetadata();

		void ApplyCanonicalDirectFactoryProjection();

		void ResetStagedRegisterWrites();

		static std::shared_ptr<InvalidOperationException> CreateRetiredMemoryContourException(unsigned char dataType, unsigned long long address);

		static std::shared_ptr<InvalidOperationException> CreateUnsupportedRetainedRegisterContourException(unsigned char dataType);

		static void ThrowIfUnsupportedRetainedRegisterContour(unsigned char dataType);

		void StagePrimaryRegisterWrite(unsigned short destRegId, unsigned long long value);

	public:
		bool Execute(std::shared_ptr<Processor::CPU_Core> &core) override;

		void EmitWriteBackRetireRecords(std::shared_ptr<Processor::CPU_Core> &core, std::span<RetireRecord> retireRecords, int &retireRecordCount) override;

		bool TryGetPrimaryWriteBackResult(unsigned long long &value) override;

		void CapturePrimaryWriteBackResult(unsigned long long value) override;

		std::wstring GetDescription() override;

	protected:
		std::shared_ptr<MoveMicroOp> shared_from_this()
		{
			return std::static_pointer_cast<MoveMicroOp>(MicroOp::shared_from_this());
		}
	};

	/// <summary>
	/// Increment/Decrement micro-operation.
	/// </summary>
	class IncrDecrMicroOp : public MicroOp
	{
	private:
		bool IsDecrement = false;

	public:
		bool isDecrement() const;
		void setDecrement(bool value);
	private:
		unsigned long long _result = 0;

	public:
		IncrDecrMicroOp();

		CanonicalDecodePublicationMode getCanonicalDecodePublication() const override;

		bool Execute(std::shared_ptr<Processor::CPU_Core> &core) override;

		void EmitWriteBackRetireRecords(std::shared_ptr<Processor::CPU_Core> &core, std::span<RetireRecord> retireRecords, int &retireRecordCount) override;

		bool TryGetPrimaryWriteBackResult(unsigned long long &value) override;

		void CapturePrimaryWriteBackResult(unsigned long long value) override;

		std::wstring GetDescription() override;

	protected:
		std::shared_ptr<IncrDecrMicroOp> shared_from_this()
		{
			return std::static_pointer_cast<IncrDecrMicroOp>(MicroOp::shared_from_this());
		}
	};

	// ─────────────────────────────────────────────────────────────────────────
	// SystemEventKind — V6 Phase 3 (A5)
	// ─────────────────────────────────────────────────────────────────────────

	/// <summary>
	/// Identifies the architectural event kind produced by a <see cref="SysEventMicroOp"/>.
	/// One value per canonical system or typed SMT/VT event instruction that flows
	/// through the explicit lane-generated event retire contour.
}

