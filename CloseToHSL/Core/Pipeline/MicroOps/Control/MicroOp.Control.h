#pragma once
#include "ControlRetireTypes.h"
#include <span>

#include "../Types/MicroOp.h"
#include "../../../Architecture/Common/YAKSys_Hybrid_CPU.Processor.h"
#include "../../../Architecture/Registers/Retire/RetireRecord.h"
#include "../../../Diagnostics/TraceEventKind.h"
#include <string>
#include <vector>
#include <format>
#include "tangible_exception_helper.h"
#include <memory>

using namespace HybridCPU_ISE::Arch;

using namespace YAKSys_Hybrid_CPU::Arch;
using namespace YAKSys_Hybrid_CPU::Core::Memory;
using namespace YAKSys_Hybrid_CPU::Core::Registers;
using namespace YAKSys_Hybrid_CPU::Core::Registers::Retire;


namespace YAKSys_Hybrid_CPU::Core
{
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to the C# readonly struct:
// ORIGINAL LINE: internal readonly struct BranchExecutionPayload
	class BranchExecutionPayload
	{
	private:
		const bool IsExecutable;
		const bool ConditionMet;
		const bool RedirectsControlFlow;
		const unsigned long long ResolvedRetireTargetAddress;
		const unsigned long long MaterializedResultValue;

		BranchExecutionPayload(bool isExecutable, bool conditionMet, bool redirectsControlFlow, unsigned long long resolvedRetireTargetAddress, unsigned long long materializedResultValue);

	public:
		bool isExecutable() const;

		bool getConditionMet() const;

		bool getRedirectsControlFlow() const;

		unsigned long long getResolvedRetireTargetAddress() const;

		unsigned long long getMaterializedResultValue() const;

		static BranchExecutionPayload Invalid();

		static BranchExecutionPayload CreateConditional(bool conditionMet, unsigned long long resolvedRetireTargetAddress, unsigned long long materializedResultValue);

		static BranchExecutionPayload CreateUnconditional(unsigned long long resolvedRetireTargetAddress, unsigned long long materializedResultValue);

		BranchExecutionPayload();
	};

	class BranchMicroOp final : public MicroOp
	{
	private:
		unsigned long long TargetAddress = 0;
		bool IsConditional = false;
		bool ConditionMet = false;
		unsigned short Reg1ID = 0;
		unsigned short Reg2ID = 0;
		short RelativeTargetDisplacement = 0;
		bool HasRelativeTargetDisplacement = false;

	public:
		unsigned long long getTargetAddress() const;
		void setTargetAddress(unsigned long long value);
		bool isConditional() const;
		void setConditional(bool value);
		bool getConditionMet() const;
		void setConditionMet(bool value);
		unsigned short getReg1ID() const;
		void setReg1ID(unsigned short value);
		unsigned short getReg2ID() const;
		void setReg2ID(unsigned short value);
		short getRelativeTargetDisplacement() const;
	private:
		void setRelativeTargetDisplacement(short value);
	public:
		bool getHasRelativeTargetDisplacement() const;
	private:
		void setHasRelativeTargetDisplacement(bool value);
		bool _hasCapturedPrimaryWriteBackResult = false;
		unsigned long long _capturedPrimaryWriteBackResult = 0;
		bool _hasResolvedRetireTargetAddress = false;
		unsigned long long _resolvedRetireTargetAddress = 0;

	public:
		BranchMicroOp();

		CanonicalDecodePublicationMode getCanonicalDecodePublication() const override;

		void ApplyCanonicalRuntimeOperandProjection(unsigned char rd, unsigned char rs1, unsigned char rs2);

		void ApplyCanonicalRuntimeTargetProjection(unsigned long long targetAddress);

		void ApplyCanonicalRuntimeRelativeTargetProjection(short relativeTargetDisplacement);

	private:
		short ResolveRelativeTargetDisplacement();

	public:
		unsigned long long ResolveConditionalTargetAddress(unsigned long long executionPc);

		unsigned long long ResolveUnconditionalTargetAddress(unsigned long long executionPc, unsigned long long baseValue = 0);

		unsigned long long ResolveLinkRegisterValue(unsigned long long executionPc);

		void CaptureResolvedRetireTargetAddress(unsigned long long targetAddress);

	private:
		void ResetResolvedRetireTargetAddress();

	public:
		BranchExecutionPayload ResolveExecutionPayload(unsigned short opcode, unsigned long long executionPc, unsigned long long operand1 = 0, unsigned long long operand2 = 0, unsigned long long baseValue = 0);

	private:
		BranchExecutionPayload ResolveConditionalExecutionPayload(unsigned short opcode, unsigned long long executionPc, unsigned long long operand1, unsigned long long operand2);

		BranchExecutionPayload ResolveUnconditionalExecutionPayload(unsigned short opcode, unsigned long long executionPc, unsigned long long baseValue);

		static bool EvaluateBranchCondition(unsigned short opcode, unsigned long long operand1, unsigned long long operand2);

		/// <summary>
		/// Initialize FSP metadata after register IDs are set.
		/// Phase: Safety Tags & Certificates - Mandatory safety mask computation.
		/// </summary>
	public:
		void InitializeMetadata();

		void RefreshWriteMetadata() override;

		bool Execute(std::shared_ptr<Processor::CPU_Core> &core) override;

		std::wstring GetDescription() override;

		void EmitWriteBackRetireRecords(std::shared_ptr<Processor::CPU_Core> &core, std::span<RetireRecord> retireRecords, int &retireRecordCount) override;

		bool TryGetPrimaryWriteBackResult(unsigned long long &value) override;

		void CapturePrimaryWriteBackResult(unsigned long long value) override;

	protected:
		std::shared_ptr<BranchMicroOp> shared_from_this()
		{
			return std::static_pointer_cast<BranchMicroOp>(MicroOp::shared_from_this());
		}
	};

	/// <summary>
	/// CSR (Control & Status Register) micro-operation
	/// </summary>
	class CSRMicroOp : public MicroOp
	{
	private:
		unsigned long long CSRAddress = 0;
		unsigned long long WriteValue = 0;
		unsigned short SrcRegID = 0;

		unsigned long long _readValue = 0;
		bool _registerWritebackConfigured = false;
		bool _registerWritebackCapabilitySeeded = false;

	public:
		unsigned long long getCSRAddress() const;
		void setCSRAddress(unsigned long long value);
		unsigned long long getWriteValue() const;
		void setWriteValue(unsigned long long value);
		unsigned short getSrcRegID() const;
		void setSrcRegID(unsigned short value);

	protected:
		virtual bool getReadsCsr() const;
		virtual bool getWritesCsr() const;
		virtual bool getWritesFromSourceRegister() const;
		virtual bool getUsesSourceRegisterWriteValue() const;
		virtual bool getClearsArchitecturalExceptionState() const;
		virtual std::wstring getDescriptionVerb() const;
	private:
		static bool HasArchitecturalRegister(unsigned short registerId);
		bool getHasArchitecturalSourceRegister() const;
		bool getHasArchitecturalDestinationRegister() const;

	protected:
		CSRMicroOp();

	public:
		CanonicalDecodePublicationMode getCanonicalDecodePublication() const override;

		/// <summary>
		/// Initialize FSP metadata after register IDs are set.
		/// Phase: Safety Tags & Certificates - Mandatory safety mask computation.
		/// </summary>
		void InitializeMetadata();

		/// <inheritdoc/>
		void RefreshWriteMetadata() override;

		bool Execute(std::shared_ptr<Processor::CPU_Core> &core) override;

		CsrRetireEffect CreateRetireEffect(std::shared_ptr<Processor::CPU_Core> &core);

		void EmitWriteBackRetireRecords(std::shared_ptr<Processor::CPU_Core> &core, std::span<RetireRecord> retireRecords, int &retireRecordCount) override;

		bool TryGetPrimaryWriteBackResult(unsigned long long &value) override;

		void CapturePrimaryWriteBackResult(unsigned long long value) override;

		std::wstring GetDescription() override;

	protected:
		virtual unsigned long long ResolveWriteValue(std::shared_ptr<Processor::CPU_Core> &core, unsigned long long priorValue);

	private:
		bool IsRegisterWritebackConfigured();

	protected:
		unsigned long long ResolveConfiguredWriteValue(std::shared_ptr<Processor::CPU_Core> &core);

	public:
		static CsrStorageSurface ResolveStorageSurface(std::shared_ptr<Processor::CPU_Core> &core, unsigned long long addr);

		static unsigned long long ReadCsr(std::shared_ptr<Processor::CPU_Core> &core, CsrStorageSurface storageSurface, unsigned long long addr);

		static void WriteCsr(std::shared_ptr<Processor::CPU_Core> &core, CsrStorageSurface storageSurface, unsigned long long addr, unsigned long long value);

	private:
		static unsigned long long ReadVectorPodPlaneCsr(std::shared_ptr<Processor::CPU_Core> &core, int csrAddr);

		static void WriteVectorPodPlaneCsr(std::shared_ptr<Processor::CPU_Core> &core, int csrAddr, unsigned long long value);

		static unsigned long long ReadPackedExceptionPriorities(std::shared_ptr<Processor::CPU_Core> &core);



	protected:
		std::shared_ptr<CSRMicroOp> shared_from_this()
		{
			return std::static_pointer_cast<CSRMicroOp>(MicroOp::shared_from_this());
		}
	};

	class CsrReadWriteMicroOp final : public CSRMicroOp
	{
	protected:
		bool getWritesCsr() const override;
		bool getWritesFromSourceRegister() const override;
		bool getUsesSourceRegisterWriteValue() const override;
		std::wstring getDescriptionVerb() const override;

		unsigned long long ResolveWriteValue(std::shared_ptr<Processor::CPU_Core> &core, unsigned long long priorValue) override;

	protected:
		std::shared_ptr<CsrReadWriteMicroOp> shared_from_this()
		{
			return std::static_pointer_cast<CsrReadWriteMicroOp>(CSRMicroOp::shared_from_this());
		}
	};

	class CsrReadSetMicroOp final : public CSRMicroOp
	{
	protected:
		bool getWritesCsr() const override;
		bool getWritesFromSourceRegister() const override;
		bool getUsesSourceRegisterWriteValue() const override;
		std::wstring getDescriptionVerb() const override;

		unsigned long long ResolveWriteValue(std::shared_ptr<Processor::CPU_Core> &core, unsigned long long priorValue) override;

	protected:
		std::shared_ptr<CsrReadSetMicroOp> shared_from_this()
		{
			return std::static_pointer_cast<CsrReadSetMicroOp>(CSRMicroOp::shared_from_this());
		}
	};

	class CsrReadClearMicroOp final : public CSRMicroOp
	{
	protected:
		bool getWritesCsr() const override;
		bool getWritesFromSourceRegister() const override;
		bool getUsesSourceRegisterWriteValue() const override;
		std::wstring getDescriptionVerb() const override;

		unsigned long long ResolveWriteValue(std::shared_ptr<Processor::CPU_Core> &core, unsigned long long priorValue) override;

	protected:
		std::shared_ptr<CsrReadClearMicroOp> shared_from_this()
		{
			return std::static_pointer_cast<CsrReadClearMicroOp>(CSRMicroOp::shared_from_this());
		}
	};

	class CsrReadWriteImmediateMicroOp final : public CSRMicroOp
	{
	protected:
		bool getWritesCsr() const override;
		bool getWritesFromSourceRegister() const override;
		bool getUsesSourceRegisterWriteValue() const override;
		std::wstring getDescriptionVerb() const override;

	protected:
		std::shared_ptr<CsrReadWriteImmediateMicroOp> shared_from_this()
		{
			return std::static_pointer_cast<CsrReadWriteImmediateMicroOp>(CSRMicroOp::shared_from_this());
		}
	};

	class CsrReadSetImmediateMicroOp final : public CSRMicroOp
	{
	protected:
		bool getWritesCsr() const override;
		bool getWritesFromSourceRegister() const override;
		bool getUsesSourceRegisterWriteValue() const override;
		std::wstring getDescriptionVerb() const override;

		unsigned long long ResolveWriteValue(std::shared_ptr<Processor::CPU_Core> &core, unsigned long long priorValue) override;

	protected:
		std::shared_ptr<CsrReadSetImmediateMicroOp> shared_from_this()
		{
			return std::static_pointer_cast<CsrReadSetImmediateMicroOp>(CSRMicroOp::shared_from_this());
		}
	};

	class CsrReadClearImmediateMicroOp final : public CSRMicroOp
	{
	protected:
		bool getWritesCsr() const override;
		bool getWritesFromSourceRegister() const override;
		bool getUsesSourceRegisterWriteValue() const override;
		std::wstring getDescriptionVerb() const override;

		unsigned long long ResolveWriteValue(std::shared_ptr<Processor::CPU_Core> &core, unsigned long long priorValue) override;

	protected:
		std::shared_ptr<CsrReadClearImmediateMicroOp> shared_from_this()
		{
			return std::static_pointer_cast<CsrReadClearImmediateMicroOp>(CSRMicroOp::shared_from_this());
		}
	};

	class CsrReadCounterMicroOp final : public CSRMicroOp
	{
	protected:
		std::wstring getDescriptionVerb() const override;

	protected:
		std::shared_ptr<CsrReadCounterMicroOp> shared_from_this()
		{
			return std::static_pointer_cast<CsrReadCounterMicroOp>(CSRMicroOp::shared_from_this());
		}
	};

	class CsrClearMicroOp final : public CSRMicroOp
	{
	protected:
		bool getReadsCsr() const override;
		bool getClearsArchitecturalExceptionState() const override;
		std::wstring getDescriptionVerb() const override;

	protected:
		std::shared_ptr<CsrClearMicroOp> shared_from_this()
		{
			return std::static_pointer_cast<CsrClearMicroOp>(CSRMicroOp::shared_from_this());
		}
	};

	/// <summary>
	/// NOP (No Operation) micro-operation
}
