#pragma once

#include "../Types/MicroOp.h"
#include "../../../Architecture/Common/YAKSys_Hybrid_CPU.Processor.h"
#include "../../../Architecture/Registers/Retire/RetireRecord.h"
#include "../../../../../NonRTL/Arch/Compat/HybridCPU_ISE.Arch.VLIW_Instruction.h"
#include <string>
#include <vector>
#include <format>
#include <memory>

using namespace HybridCPU_ISE::Arch;

using namespace YAKSys_Hybrid_CPU::Core::Registers;
using namespace YAKSys_Hybrid_CPU::Core::Registers::Retire;


namespace YAKSys_Hybrid_CPU::Core
{
	/// <summary>
	/// Scalar ALU micro-operation (register-to-register arithmetic)
	/// </summary>
	class ScalarALUMicroOp : public MicroOp
	{
	private:
		unsigned short Src1RegID = 0;
		unsigned short Src2RegID = 0;
		unsigned long long Immediate = 0;
		bool UsesImmediate = false;

	public:
		unsigned short getSrc1RegID() const;
		void setSrc1RegID(unsigned short value);
		unsigned short getSrc2RegID() const;
		void setSrc2RegID(unsigned short value);
		unsigned long long getImmediate() const;
		void setImmediate(unsigned long long value);
		bool getUsesImmediate() const;
		void setUsesImmediate(bool value);

	private:
		unsigned long long _result = 0;

	public:
		ScalarALUMicroOp();

		CanonicalDecodePublicationMode getCanonicalDecodePublication() const override;

		/// <summary>
		/// Initialize FSP metadata after register IDs are set.
		/// Phase: Safety Tags & Certificates - Mandatory safety mask computation.
		/// </summary>
		void InitializeMetadata();

		/// <inheritdoc/>
		/// Calls <see cref="InitializeMetadata"/> so that <see cref="MicroOp.WriteRegisters"/>
		/// and <see cref="MicroOp.ResourceMask"/> are re-populated after <see cref="MicroOp.WritesRegister"/>
		/// is promoted by a descriptor override in <see cref="InstructionRegistry.CreateMicroOp"/>.
		void RefreshWriteMetadata() override;

		bool Execute(std::shared_ptr<Processor::CPU_Core> &core) override;

		bool ExecuteWithResolvedPipelineOperands(unsigned long long source1, unsigned long long source2, unsigned long long executionPc);

		void EmitWriteBackRetireRecords(std::shared_ptr<Processor::CPU_Core> &core, std::span<RetireRecord> retireRecords, int &retireRecordCount) override;

		bool TryGetPrimaryWriteBackResult(unsigned long long &value) override;

		void CapturePrimaryWriteBackResult(unsigned long long value) override;

		/// <summary>
		/// Delegates all scalar ALU computation to the canonical <see cref="ScalarAluOps.Compute"/>
		/// helper (Blueprint §7: unified ALU, removes duplicated switch).
		///
		/// When <see cref="UsesImmediate"/> is <see langword="true"/>, <paramref name="op2"/>
		/// already contains the immediate value sourced from the instruction's <c>Immediate</c>
		/// field (see the <c>Execute()</c> method above).  In that case we pass <paramref name="op2"/>
		/// as <em>both</em> the second register operand <em>and</em> the <c>immediate</c>
		/// parameter so that shift instructions and <c>Move_Num</c> use the correct value on
		/// both the MicroOp-path and the pipeline-path.
		/// </summary>
	private:
		unsigned long long ExecuteScalarOp(unsigned int opCode, unsigned long long op1, unsigned long long op2, unsigned long long executionPc);

	public:
		std::wstring GetDescription() override;

	protected:
		std::shared_ptr<ScalarALUMicroOp> shared_from_this()
		{
			return std::static_pointer_cast<ScalarALUMicroOp>(MicroOp::shared_from_this());
		}
	};

	/// <summary>
	/// Vector ALU micro-operation (memory-to-memory stream operations)
	/// </summary>
	class VectorALUMicroOp : public MicroOp
	{
	private:
		VLIW_Instruction Instruction;

	public:
		VLIW_Instruction getInstruction() const;
		void setInstruction(VLIW_Instruction value);

		VectorALUMicroOp();

		CanonicalDecodePublicationMode getCanonicalDecodePublication() const override;

		/// <summary>
		/// Initialize FSP metadata for vector operations.
		/// Vector operations typically work on memory ranges, not individual registers.
		/// Phase: Safety Tags & Certificates - Mandatory safety mask computation.
		/// </summary>
		void InitializeMetadata();

		bool Execute(std::shared_ptr<Processor::CPU_Core> &core) override;

	private:
		void ResetStagedRegisterWrites();

	public:
		std::wstring GetDescription() override;

	protected:
		std::shared_ptr<VectorALUMicroOp> shared_from_this()
		{
			return std::static_pointer_cast<VectorALUMicroOp>(MicroOp::shared_from_this());
		}
	};

	/// <summary>
	/// Abstract base class for Load/Store micro-operations.
	/// Provides explicit methods for managing speculative execution state.
	/// Phase 7: Speculative FSP with Silent Squash - Refactored for clarity and extensibility.
}
