#pragma once

#include "../Types/MicroOp.h"
#include "../../../../../NonRTL/Arch/Compat/HybridCPU_ISE.Arch.VLIW_Instruction.h"
#include "../../../../../NonRTL/Arch/SerializationClass.h"
#include "../../../Architecture/Common/YAKSys_Hybrid_CPU.Processor.h"
#include "../../../Architecture/Registers/Retire/RetireRecord.h"
#include "../../../Execution/ExternalAccelerators/ExternalAcceleratorRuntime.h"
#include "../../Safety/ResourceBitset.h"
#include <string>
#include <vector>
#include <optional>
#include <format>
#include "tangible_exception_helper.h"
#include <memory>

using namespace HybridCPU_ISE::Arch;
using namespace YAKSys_Hybrid_CPU::Arch;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Auth;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Backends;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Capabilities;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Descriptors;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Tokens;
using namespace YAKSys_Hybrid_CPU::Core::Registers;
using namespace YAKSys_Hybrid_CPU::Core::Registers::Retire;

namespace YAKSys_Hybrid_CPU::Core
{
	enum class SystemDeviceCommandKind : unsigned char
	{
		QueryCaps,
		Submit,
		Poll,
		Status,
		Wait,
		Cancel,
		Fence
	};

	/// <summary>
	/// Runtime-owned L7-SDC lane7 system-device command carrier for the Phase 08
	/// current production contour plus the Phase 08A ACCEL_STATUS contour.
	/// It remains a control-plane SystemSingleton op:
	/// no scalar/vector arithmetic plane and no legacy custom-accelerator fallback.
	/// </summary>
	class SystemDeviceCommandMicroOp : public MicroOp
	{
	private:
		const SystemDeviceCommandKind CommandKind;
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly System.Nullable<AcceleratorCommandDescriptor> CommandDescriptor;
		const std::optional<std::shared_ptr<AcceleratorCommandDescriptor>> CommandDescriptor;
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly System.Nullable<AcceleratorDescriptorReference> CommandDescriptorReference;
		const std::optional<std::shared_ptr<AcceleratorDescriptorReference>> CommandDescriptorReference;
		const unsigned short DestinationRegister;
		const unsigned short TokenRegister;

		std::optional<std::shared_ptr<ExternalAcceleratorRuntimeCommandResult>> _lastCommandResult;
		unsigned long long _capturedWriteBackValue = 0;
		std::shared_ptr<AcceleratorTokenHandle> _capturedFenceHandle;

	protected:
		SystemDeviceCommandMicroOp(unsigned short opCode, SystemDeviceCommandKind commandKind, Arch::SerializationClass serializationClass, unsigned short destinationRegister = VLIW_Instruction::NoReg, unsigned short tokenRegister = VLIW_Instruction::NoReg, std::optional<std::shared_ptr<AcceleratorCommandDescriptor>> commandDescriptor = std::nullopt);

	public:
		SystemDeviceCommandKind getCommandKind() const;

		std::optional<std::shared_ptr<AcceleratorCommandDescriptor>> getCommandDescriptor() const;

		std::optional<std::shared_ptr<AcceleratorDescriptorReference>> getCommandDescriptorReference() const;

		unsigned short getDestinationRegister() const;

		unsigned short getTokenRegister() const;

		std::optional<std::shared_ptr<ExternalAcceleratorRuntimeCommandResult>> getLastCommandResult() const;

		std::optional<std::shared_ptr<AcceleratorTokenAdmissionResult>> getLastSubmitAdmission() const;

		std::optional<std::shared_ptr<AcceleratorTokenLookupResult>> getLastTokenLookup() const;

		bool getUsedLegacyCustomAcceleratorFallback() const;

		bool getUsedArithmeticExecutionPlane() const;

		CanonicalDecodePublicationMode getCanonicalDecodePublication() const override;

		void InitializeMetadata();

		void RefreshWriteMetadata() override;

		bool Execute(std::shared_ptr<Processor::CPU_Core> &core) override;

		void EmitWriteBackRetireRecords(std::shared_ptr<Processor::CPU_Core> &core, std::span<RetireRecord> retireRecords, int &retireRecordCount) override;

		bool TryGetPrimaryWriteBackResult(unsigned long long &value) override;

		void CapturePrimaryWriteBackResult(unsigned long long value) override;

	private:
		std::shared_ptr<ExternalAcceleratorRuntimeCommandResult> ExecuteSubmit(std::shared_ptr<ExternalAcceleratorRuntime> runtime);

		std::shared_ptr<ExternalAcceleratorRuntimeCommandResult> ExecuteFenceObserve(std::shared_ptr<ExternalAcceleratorRuntime> runtime, std::shared_ptr<AcceleratorTokenHandle> handle);

		std::shared_ptr<ExternalAcceleratorRuntimeCommandResult> ResolveRetireResult(std::shared_ptr<Processor::CPU_Core> &core);

		static void RejectGuestCompatibilityExecution(std::shared_ptr<Processor::CPU_Core> &core, int vtId);

		unsigned long long ReadTokenRegisterValue(std::shared_ptr<Processor::CPU_Core> &core, int ownerVirtualThreadId);

		std::shared_ptr<AcceleratorTokenHandle> ReadTokenHandle(std::shared_ptr<Processor::CPU_Core> &core, int vtId);

		std::shared_ptr<AcceleratorOwnerBinding> BuildOwnerBinding(std::shared_ptr<Processor::CPU_Core> &core, int vtId);

		std::shared_ptr<AcceleratorGuardEvidence> BuildGuardEvidence(std::shared_ptr<Processor::CPU_Core> &core, int vtId);

	public:
		std::wstring GetDescription() override;

	private:
		static bool UsesTokenRegister(SystemDeviceCommandKind commandKind);

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private static ResourceBitset BuildResourceMask(ulong ownerDomainTag, System.Collections.Generic.IReadOnlyList<int> readRegisters, System.Collections.Generic.IReadOnlyList<int> writeRegisters)
		static ResourceBitset BuildResourceMask(unsigned long long ownerDomainTag, const std::vector<int> &readRegisters, const std::vector<int> &writeRegisters);

		static unsigned short NormalizeOptionalArchRegister(unsigned short rawRegister, const std::wstring &operandName);

	protected:
		std::shared_ptr<SystemDeviceCommandMicroOp> shared_from_this()
		{
			return std::static_pointer_cast<SystemDeviceCommandMicroOp>(std::enable_shared_from_this<MicroOp>::shared_from_this());
		}
	};

	class AcceleratorQueryCapsMicroOp final : public SystemDeviceCommandMicroOp
	{
	public:
		AcceleratorQueryCapsMicroOp(unsigned short destinationRegister = VLIW_Instruction::NoReg);

	protected:
		std::shared_ptr<AcceleratorQueryCapsMicroOp> shared_from_this()
		{
			return std::static_pointer_cast<AcceleratorQueryCapsMicroOp>(std::enable_shared_from_this<MicroOp>::shared_from_this());
		}
	};

	class AcceleratorSubmitMicroOp final : public SystemDeviceCommandMicroOp
	{
	private:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly System.Nullable<AcceleratorGuardDecision> SubmitGuardDecision;
		std::optional<std::shared_ptr<AcceleratorGuardDecision>> SubmitGuardDecision;

	public:
		std::optional<std::shared_ptr<AcceleratorGuardDecision>> getSubmitGuardDecision() const;

		AcceleratorSubmitMicroOp(unsigned short destinationRegister = VLIW_Instruction::NoReg);

		AcceleratorSubmitMicroOp(unsigned short destinationRegister, std::shared_ptr<AcceleratorCommandDescriptor> commandDescriptor);

		AcceleratorSubmitMicroOp(std::shared_ptr<AcceleratorCommandDescriptor> commandDescriptor);

	protected:
		std::shared_ptr<AcceleratorSubmitMicroOp> shared_from_this()
		{
			return std::static_pointer_cast<AcceleratorSubmitMicroOp>(std::enable_shared_from_this<MicroOp>::shared_from_this());
		}
	};

	class AcceleratorPollMicroOp final : public SystemDeviceCommandMicroOp
	{
	public:
		AcceleratorPollMicroOp(unsigned short destinationRegister = VLIW_Instruction::NoReg, unsigned short tokenRegister = VLIW_Instruction::NoReg);

	protected:
		std::shared_ptr<AcceleratorPollMicroOp> shared_from_this()
		{
			return std::static_pointer_cast<AcceleratorPollMicroOp>(std::enable_shared_from_this<MicroOp>::shared_from_this());
		}
	};

	class AcceleratorWaitMicroOp final : public SystemDeviceCommandMicroOp
	{
	public:
		AcceleratorWaitMicroOp(unsigned short destinationRegister = VLIW_Instruction::NoReg, unsigned short tokenRegister = VLIW_Instruction::NoReg);

	protected:
		std::shared_ptr<AcceleratorWaitMicroOp> shared_from_this()
		{
			return std::static_pointer_cast<AcceleratorWaitMicroOp>(std::enable_shared_from_this<MicroOp>::shared_from_this());
		}
	};

	class AcceleratorStatusMicroOp final : public SystemDeviceCommandMicroOp
	{
	public:
		AcceleratorStatusMicroOp(unsigned short destinationRegister = VLIW_Instruction::NoReg, unsigned short tokenRegister = VLIW_Instruction::NoReg);

	protected:
		std::shared_ptr<AcceleratorStatusMicroOp> shared_from_this()
		{
			return std::static_pointer_cast<AcceleratorStatusMicroOp>(std::enable_shared_from_this<MicroOp>::shared_from_this());
		}
	};

	class AcceleratorCancelMicroOp final : public SystemDeviceCommandMicroOp
	{
	public:
		AcceleratorCancelMicroOp(unsigned short destinationRegister = VLIW_Instruction::NoReg, unsigned short tokenRegister = VLIW_Instruction::NoReg);

	protected:
		std::shared_ptr<AcceleratorCancelMicroOp> shared_from_this()
		{
			return std::static_pointer_cast<AcceleratorCancelMicroOp>(std::enable_shared_from_this<MicroOp>::shared_from_this());
		}
	};

	class AcceleratorFenceMicroOp final : public SystemDeviceCommandMicroOp
	{
	public:
		AcceleratorFenceMicroOp(unsigned short destinationRegister = VLIW_Instruction::NoReg, unsigned short tokenRegister = VLIW_Instruction::NoReg);

	protected:
		std::shared_ptr<AcceleratorFenceMicroOp> shared_from_this()
		{
			return std::static_pointer_cast<AcceleratorFenceMicroOp>(std::enable_shared_from_this<MicroOp>::shared_from_this());
		}
	};
}

