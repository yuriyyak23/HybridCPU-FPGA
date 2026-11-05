#include "MicroOp.LoadStore.h"
#include "../Replay/ReplayToken.h"
#include "../../../Runtime/Memory/Translation/CpuInstructionTranslationContour.h"
#include "../../../Execution/Memory/LoadStore/MemoryBankRouting.h"
#include "../../../../../NonRTL/Arch/Compat/HybridCPU_ISE.Arch.VLIW_Instruction.h"
#include "../../../../../NonRTL/Arch/YAKSys_Hybrid_CPU.Arch.OpcodeRegistry.h"
#include "../../../../../NonRTL/Arch/InstructionClass.h"
#include "../../../../../NonRTL/Arch/SerializationClass.h"
#include "../../Scheduling/SlotLegality/SlotClassDefinitions.h"
#include "../../Safety/ResourceBitset.h"
#include "../../../../Memory/Subsystem/YAKSys_Hybrid_CPU.Memory.MemorySubsystem.h"
#include "../Types/MicroOp.Exceptions.h"
#include <cstddef>

using namespace HybridCPU_ISE::Arch;
using namespace HybridCPU_ISE::CloseToHSL::Memory::Timing;
using namespace HybridCPU_ISE::Core;
using namespace YAKSys_Hybrid_CPU::Arch;
using namespace YAKSys_Hybrid_CPU::Core::Memory;
using namespace YAKSys_Hybrid_CPU::Core::Registers;
using namespace YAKSys_Hybrid_CPU::Core::Registers::Retire;
using namespace YAKSys_Hybrid_CPU::Memory;
using OpcodeValues = YAKSys_Hybrid_CPU::Processor::CPU_Core::IsaOpcodeValues;

namespace YAKSys_Hybrid_CPU::Core
{
	namespace
	{
		unsigned long long ReadLittleEndianUnsigned(const std::vector<unsigned char> &buffer, std::size_t byteCount)
		{
			unsigned long long value = 0;
			for (std::size_t index = 0; index < byteCount; ++index)
			{
				value |= static_cast<unsigned long long>(buffer[index]) << (index * 8);
			}
			return value;
		}

		std::vector<unsigned char> WriteLittleEndianUnsigned(unsigned long long value, std::size_t byteCount)
		{
			std::vector<unsigned char> bytes(byteCount);
			for (std::size_t index = 0; index < byteCount; ++index)
			{
				bytes[index] = static_cast<unsigned char>((value >> (index * 8)) & 0xFFu);
			}
			return bytes;
		}
	}

	void LoadStoreMicroOp::BindStandaloneSecureReadBank(unsigned long long offset, unsigned long long sourceLength)
	{
		auto loadMicroOp = std::dynamic_pointer_cast<LoadMicroOp>(shared_from_this());
		if (loadMicroOp == nullptr || loadMicroOp->getSize() != 8 || standaloneSecureReadBank.has_value())
		{
			throw InvalidOperationException(L"One original bounded source-bank binding required.");
		}
		standaloneSecureReadBank = Core::Memory::MemoryBankRouting::ResolveStandaloneReadOnlyBank(offset, sourceLength);
	}

	std::shared_ptr<PhysicalReadBinding> LoadStoreMicroOp::getCpuPhysicalReadBinding() const
	{
		return CpuPhysicalReadBinding;
	}

	void LoadStoreMicroOp::setCpuPhysicalReadBinding(std::shared_ptr<PhysicalReadBinding> value)
	{
		CpuPhysicalReadBinding = value;
	}

	void LoadStoreMicroOp::BindCpuPhysicalRead(std::shared_ptr<PhysicalReadBinding> binding)
	{
		if (getCpuPhysicalReadBinding() != nullptr)
		{
			throw InvalidOperationException(L"CPU read translation already bound.");
		}
		setCpuPhysicalReadBinding(binding);
	}

	bool LoadStoreMicroOp::getUsesBaseRegisterAddressing() const
	{
		return UsesBaseRegisterAddressing;
	}

	void LoadStoreMicroOp::setUsesBaseRegisterAddressing(bool value)
	{
		UsesBaseRegisterAddressing = value;
	}

	long long LoadStoreMicroOp::getAddressDisplacement() const
	{
		return AddressDisplacement;
	}

	void LoadStoreMicroOp::setAddressDisplacement(long long value)
	{
		AddressDisplacement = value;
	}

	int LoadStoreMicroOp::getMemoryBankId() const
	{
		return standaloneSecureReadBank.value_or(
			Core::Memory::MemoryBankRouting::ResolveSchedulerVisibleBankId(getMemoryAddress()));
	}

	void LoadStoreMicroOp::ResolveBaseRelativeAddressForPipeline(std::shared_ptr<Processor::CPU_Core> core)
	{
	}

	void LoadStoreMicroOp::ResolveBaseRelativeAddressForPipeline(unsigned long long baseValue)
	{
	}

	unsigned long long LoadStoreMicroOp::ResolveBaseRelativeAddress(std::shared_ptr<Processor::CPU_Core> core, unsigned short baseRegister)
	{
		if (!getUsesBaseRegisterAddressing())
		{
			return getMemoryAddress();
		}
		if (_baseRelativeAddressResolved)
		{
			return _baseRelativeAddress;
		}
		if (baseRegister == VLIW_Instruction::NoReg)
		{
			throw InvalidOperationException(L"Base-relative scalar memory execution requires an architectural base register.");
		}

		VtId checkedOwner;
		int vtId = TryGetArchitecturalOwnerVtId(checkedOwner) ? checkedOwner.getValue() : NormalizeExecutionVtId(getOwnerThreadId());
		unsigned long long baseValue = core->ReadArch(vtId, baseRegister);
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: _baseRelativeAddress = unchecked(baseValue + (ulong)AddressDisplacement);
		_baseRelativeAddress = baseValue + static_cast<unsigned long long>(getAddressDisplacement());
		_baseRelativeAddressResolved = true;
		return _baseRelativeAddress;
	}

	unsigned long long LoadStoreMicroOp::ResolveCpuTranslatedAddress(std::shared_ptr<Processor::CPU_Core> &core, CpuInstructionTranslationAccessKind accessKind, unsigned char accessSize)
	{
		if (_cpuTranslationResolved)
		{
			return _cpuTranslatedAddress;
		}

		std::shared_ptr<CpuInstructionTranslationResult> result = core->TranslateScalarInstructionMemoryAddress(shared_from_this(), accessKind, accessSize);
		if (!result->Succeeded)
		{
			if (!result->Fault)
			{
				throw InvalidOperationException(L"Failed CPU scalar translation omitted typed fault facts.");
			}
			auto fault = std::make_shared<CpuInstructionTranslationFault>(result->Fault.value());
			throw CpuInstructionTranslationFaultException(fault);
		}

		_cpuTranslatedAddress = result->PhysicalAddress;
		_cpuTranslationOwnerEpoch = result->OwnerEpoch;
		_cpuTranslationResolved = true;
		return _cpuTranslatedAddress;
	}

	bool LoadStoreMicroOp::TryGetCpuTranslatedAddress(unsigned long long &physicalAddress, unsigned long long &ownerEpoch) const
	{
		physicalAddress = _cpuTranslatedAddress;
		ownerEpoch = _cpuTranslationOwnerEpoch;
		return _cpuTranslationResolved;
	}

	unsigned long long LoadStoreMicroOp::ResolveCpuTranslatedAddressForPipeline(std::shared_ptr<Processor::CPU_Core> &core, CpuInstructionTranslationAccessKind accessKind, unsigned char accessSize)
	{
		return ResolveCpuTranslatedAddress(core, accessKind, accessSize);
	}

	CanonicalDecodePublicationMode LoadStoreMicroOp::getCanonicalDecodePublication() const
	{
		return CanonicalDecodePublicationMode::SelfPublishes;
	}

	bool LoadStoreMicroOp::HasExactMainMemoryRange(std::shared_ptr<Processor::CPU_Core> &core, unsigned long long address, int size)
	{
		return core->HasExactBoundMainMemoryRange(address, size);
	}

	void LoadStoreMicroOp::ThrowIfMainMemoryRangeUnavailable(std::shared_ptr<Processor::CPU_Core> &core, unsigned long long address, int size, const std::wstring &executionSurface)
	{
		core->ThrowIfBoundMainMemoryRangeUnavailable(address, size, executionSurface);
	}

	void LoadStoreMicroOp::ReadMainMemoryExact(std::shared_ptr<Processor::CPU_Core> &core, unsigned long long address, std::vector<unsigned char> &buffer, const std::wstring &executionSurface)
	{
		core->ReadBoundMainMemoryExact(address, buffer, executionSurface);
	}

	unsigned long long LoadStoreMicroOp::DecodeLoadValue(unsigned int opcode, std::vector<unsigned char> &buffer, unsigned char accessSize, const std::wstring &executionSurface)
	{
		int requiredSize;
		switch (accessSize)
		{
		case 1: requiredSize = 1; break;
		case 2: requiredSize = 2; break;
		case 4: requiredSize = 4; break;
		case 0:
		case 8: requiredSize = 8; break;
		default:
			throw InvalidOperationException(std::format(
				L"{} reached unsupported scalar load access size {}.", executionSurface,
				static_cast<unsigned int>(accessSize)));
		}

		if (buffer.size() < static_cast<std::size_t>(requiredSize))
		{
			throw InvalidOperationException(std::format(
				L"{} reached a partial load buffer ({} byte(s)) for an access size of {} byte(s). The authoritative memory lane must fail closed instead of decoding a partial load image across the boundary contour.",
				executionSurface, buffer.size(), requiredSize));
		}

		const auto normalizedOpcode = static_cast<unsigned short>(opcode);
		unsigned char typedAccessSize = 0;
		if (TryResolveTypedLoadAccessSize(normalizedOpcode, typedAccessSize) &&
			typedAccessSize != static_cast<unsigned char>(requiredSize))
		{
			throw InvalidOperationException(std::format(
				L"{} reached typed load opcode {} with access size {}, but the published runtime contour requires {} byte(s).",
				executionSurface, OpcodeRegistry::GetMnemonicOrHex(opcode), requiredSize,
				static_cast<unsigned int>(typedAccessSize)));
		}

		switch (normalizedOpcode)
		{
		case OpcodeValues::LB:
			return static_cast<unsigned long long>(static_cast<long long>(static_cast<signed char>(buffer[0])));
		case OpcodeValues::LH:
			return static_cast<unsigned long long>(static_cast<long long>(static_cast<short>(ReadLittleEndianUnsigned(buffer, 2))));
		case OpcodeValues::LW:
			return static_cast<unsigned long long>(static_cast<long long>(static_cast<int>(ReadLittleEndianUnsigned(buffer, 4))));
		case OpcodeValues::LBU:
			return buffer[0];
		case OpcodeValues::LHU:
			return ReadLittleEndianUnsigned(buffer, 2);
		case OpcodeValues::LWU:
			return ReadLittleEndianUnsigned(buffer, 4);
		case OpcodeValues::LD:
			return ReadLittleEndianUnsigned(buffer, 8);
		default:
			return DecodeUntypedLoadValue(buffer, requiredSize);
		}
	}

	unsigned long long LoadStoreMicroOp::DecodeUntypedLoadValue(std::vector<unsigned char> &buffer, int requiredSize)
	{
		return ReadLittleEndianUnsigned(buffer, static_cast<std::size_t>(requiredSize));
	}
	bool LoadStoreMicroOp::TryResolveTypedLoadAccessSize(unsigned short opcode, unsigned char &accessSize)
	{
		int temp_var123;
		switch (opcode)
		{
			case OpcodeValues::LB:
			case OpcodeValues::LBU:
				temp_var123 = 1;
				break;
			case OpcodeValues::LH:
			case OpcodeValues::LHU:
				temp_var123 = 2;
				break;
			case OpcodeValues::LW:
			case OpcodeValues::LWU:
				temp_var123 = 4;
				break;
			case OpcodeValues::LD:
				temp_var123 = 8;
				break;
			default:
				temp_var123 = static_cast<unsigned char>(0);
				break;
		}
		accessSize = temp_var123;

		return accessSize != 0;
	}

	void LoadStoreMicroOp::MarkSpeculative()
	{
		setSpeculative(true);
	}

	void LoadStoreMicroOp::ClearSpeculative()
	{
		setSpeculative(false);
	}

	void LoadStoreMicroOp::MarkFaulted()
	{
		setFaulted(true);
	}

	void LoadStoreMicroOp::ClearFaulted()
	{
		setFaulted(false);
	}

	unsigned long long LoadMicroOp::getAddress() const
	{
		return Address;
	}

	void LoadMicroOp::setAddress(unsigned long long value)
	{
		Address = value;
	}

	unsigned char LoadMicroOp::getSize() const
	{
		return Size;
	}

	void LoadMicroOp::setSize(unsigned char value)
	{
		Size = value;
	}

	unsigned short LoadMicroOp::getBaseRegID() const
	{
		return BaseRegID;
	}

	void LoadMicroOp::setBaseRegID(unsigned short value)
	{
		BaseRegID = value;
	}

	bool LoadMicroOp::getOwnsPendingMemoryCompletion() const
	{
		unsigned long long acceptedAddress;
		unsigned long long ownerEpoch;
		return _requestController != nullptr && _controllerRequestId &&
			TryGetCpuTranslatedAddress(acceptedAddress, ownerEpoch) &&
			_requestController->OwnsOutstandingSingleLaneScalarLoad(
				_controllerRequestId.value(), 0, acceptedAddress, getSize());
	}

	bool LoadMicroOp::getHasControllerAdmissionBackpressure() const
	{
		return _controllerAdmissionBackpressured && !_controllerRequestId;
	}

	bool LoadMicroOp::isSpeculativeFaultSuppressed() const
	{
		return isSpeculative() && getFaulted();
	}

	bool LoadMicroOp::HasNonSpeculativeFallbackBackendDenial(std::shared_ptr<Processor::CPU_Core> core)
	{
		return !_controllerRequestId && _requestController == nullptr && !isSpeculative() && !getFaulted() && core->GetBoundMemorySubsystem() == nullptr && !core->HasExactBoundMainMemoryRange(getAddress(), getSize());
	}

	unsigned long long LoadMicroOp::getMemoryAddress() const
	{
		return getAddress();
	}

	void LoadMicroOp::ResolveBaseRelativeAddressForPipeline(std::shared_ptr<Processor::CPU_Core> core)
	{
		if (!getUsesBaseRegisterAddressing())
		{
			return;
		}
		setAddress(ResolveBaseRelativeAddress(core, getBaseRegID()));
		setReadMemoryRanges(std::vector<MemoryRange>{{getAddress(), static_cast<unsigned long long>(getSize())}});
	}

	void LoadMicroOp::ResolveBaseRelativeAddressForPipeline(unsigned long long baseValue)
	{
		if (!getUsesBaseRegisterAddressing())
		{
			return;
		}
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: Address = unchecked(baseValue + (ulong)AddressDisplacement);
		setAddress(baseValue + static_cast<unsigned long long>(getAddressDisplacement()));
		setReadMemoryRanges(std::vector<MemoryRange>{{getAddress(), static_cast<unsigned long long>(getSize())}});
	}

	LoadMicroOp::LoadMicroOp()
	{
		setMemoryOp(true);
		setClass(MicroOpClass::Lsu);

		// ISA v4 Phase 02: loads are Memory class, free ordering (non-destructive)
		setInstructionClass(Arch::InstructionClass::Memory);
		setSerializationClass(Arch::SerializationClass::Free);

		// Phase 01: Typed-slot taxonomy
		SetClassFlexiblePlacement(SlotClass::LsuClass);
	}

	void LoadMicroOp::InitializeMetadata()
	{
		if (getUsesBaseRegisterAddressing())
		{
			setStealable(false);
			setSerializationClass(Arch::SerializationClass::MemoryOrdered);
		}
		// FSP MMIO Security Policy (Phase: Safety Tags & Certificates):
		// MMIO reads are often state-mutating (e.g., clear-on-read, FIFO pop).
		// Speculative or FSP-stolen execution of MMIO breaks system state.
		// Mark operations targeting MMIO space as strictly non-stealable.
		constexpr unsigned long long MMIO_BASE = 0xFFFF000000000000ULL;
		if (getAddress() >= MMIO_BASE)
		{
			setStealable(false);
		}

		// Blueprint §3.69 / §5: guard against NoReg sentinels (0xFFFF)
		constexpr unsigned short noReg = VLIW_Instruction::NoReg;

		if (getBaseRegID() != noReg)
		{
			setReadRegisters(std::vector<int> {static_cast<int>(getBaseRegID())});
		}
		else
		{
			setReadRegisters(std::vector<int>{});
		}

		// Write to destination register — skip NoReg sentinels
		if (getWritesRegister() && getDestRegID() != noReg)
		{
			setWriteRegisters(std::vector<int> {static_cast<int>(getDestRegID())});
		}

		// Memory range will be updated during Execute
		setReadMemoryRanges(std::vector<MemoryRange>{{getAddress(), static_cast<unsigned long long>(getSize())}});

		// Phase 8: Initialize ResourceMask for GRLB
		setResourceMask(ResourceBitset::Zero);
		// Add register read (base address register)
		if (getBaseRegID() != noReg)
		{
			ArchRegId baseRegister;
			const auto baseResource = ArchRegId::TryCreate(getBaseRegID(), baseRegister)
				? ResourceMaskBuilder::ForArchitecturalRegisterRead(std::make_shared<ArchRegId>(baseRegister))
				: ResourceMaskBuilder::ForRegisterRead(getBaseRegID());
			setResourceMask(getResourceMask() | baseResource);
		}
		// Add register write (destination register)
		if (getWritesRegister() && getDestRegID() != noReg)
		{
			ArchRegId destinationRegister;
			const auto destinationResource = ArchRegId::TryCreate(getDestRegID(), destinationRegister)
				? ResourceMaskBuilder::ForArchitecturalRegisterWrite(std::make_shared<ArchRegId>(destinationRegister))
				: ResourceMaskBuilder::ForRegisterWrite(getDestRegID());
			setResourceMask(getResourceMask() | destinationResource);
		}
		// Add LSU load channel
		setResourceMask(getResourceMask() | ResourceMaskBuilder::ForLoad());
		// Add memory domain (use owner thread ID as domain)
		setResourceMask(getResourceMask() | ResourceMaskBuilder::ForMemoryDomain(getOwnerThreadId()));

		PublishExplicitStructuralSafetyMask();
		RefreshAdmissionMetadata(shared_from_this());
	}

	void LoadMicroOp::RefreshWriteMetadata()
	{
		InitializeMetadata();
	}

	bool LoadMicroOp::Execute(std::shared_ptr<Processor::CPU_Core> &core)
	{
		_controllerAdmissionBackpressured = false;
		try
		{
			if (getUsesBaseRegisterAddressing())
			{
				setAddress(ResolveBaseRelativeAddress(core, getBaseRegID()));
				setReadMemoryRanges(std::vector<MemoryRange>{{getAddress(), static_cast<unsigned long long>(getSize())}});
			}
			unsigned long long translatedAddress = ResolveCpuTranslatedAddress(core, CpuInstructionTranslationAccessKind::ScalarLoad, getSize());
			// If MemorySubsystem is available, use the RF-10 controller
			// request/completion interface.
			std::shared_ptr<MemorySubsystem> memSub = core->GetBoundMemorySubsystem();
			if (memSub != nullptr)
			{
				std::shared_ptr<MemoryCycleController> controller = memSub->getCycleController();
				if (!_controllerRequestId)
				{
					std::shared_ptr<MemoryAdmissionResult> admission = controller->TryAcceptCpuSingleLaneScalarLoad(0, translatedAddress, getSize(), getCpuPhysicalReadBinding());
					if (admission->Status == MemoryAdmissionStatus::Backpressured)
					{
						_controllerAdmissionBackpressured = true;
						return false;
					}

					if (admission->Status == MemoryAdmissionStatus::Rejected)
					{
						throw InvalidOperationException(admission->Reason.value_or(
							L"MemoryCycleController rejected a single-lane scalar load without a reason."));
					}

					_requestController = controller;
					_controllerRequestId = std::make_shared<MemoryRequestId>(admission->RequestId);
					return false;
				}

				if (_requestController != controller)
				{
					if (_requestController != nullptr)
					{
						_requestController->TryCancel(_controllerRequestId.value());
					}
					ClearControllerRequestState();
					throw InvalidOperationException(L"LoadMicroOp controller binding changed while an accepted request was outstanding.");
				}

				std::shared_ptr<MemoryCompletion> completion;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: if (!controller.TryTakeCompletion(_controllerRequestId.Value, out MemoryCompletion? completion))
				if (!controller->TryTakeCompletion(_controllerRequestId.value(), completion))
				{
					return false;
				}

				ClearControllerRequestState();
				if (completion == nullptr || !completion->getSucceeded())
				{
					const std::wstring failureReason = completion != nullptr
						? completion->getFailureReason()
						: std::wstring{};
					throw InvalidOperationException(
						L"LoadMicroOp.Execute(): accepted memory request did not materialize successfully. " +
						(failureReason.empty()
							? L"MemoryCycleController returned an invalid single-lane scalar-load completion."
							: failureReason));
				}

				const auto data = completion->getData();
				std::vector<unsigned char> resultBuffer(data.begin(), data.end());
				_loadedValue = DecodeLoadValue(getOpCode(), resultBuffer, getSize(), L"LoadMicroOp.Execute()");
				return true; // Operation complete
			}

			// Fallback to synchronous implementation if MemorySubsystem is not available
			if (HasExactMainMemoryRange(core, translatedAddress, getSize()))
			{
				std::vector<unsigned char> bytes(getSize());
				ReadMainMemoryExact(core, translatedAddress, bytes, L"LoadMicroOp.Execute()");
				_loadedValue = DecodeLoadValue(getOpCode(), bytes, getSize(), L"LoadMicroOp.Execute()");
				return true;
			}

			// Phase 7: Out-of-bounds access handling for speculative operations
			if (this->isSpeculative())
			{
				this->MarkFaulted();
				return false; // Not ready to commit
			}

			return false;
		}
		catch (const CpuInstructionTranslationFaultException &e1)
		{
			if (this->isSpeculative())
			{
				this->MarkFaulted();
				return false;
			}
			throw;
		}
		catch (const PageFaultException &e2)
		{
			// Phase 7: Speculative FSP with Silent Squash
			// If this is a speculative operation, suppress the exception and mark as faulted
			if (this->isSpeculative())
			{
				this->MarkFaulted();
				return false; // Not ready to commit
			}
			else
			{
				// Non-speculative operation: propagate exception normally
				throw;
			}
		}
		catch (const std::runtime_error &ex)
		{
			// Phase 7: Convert other memory exceptions to PageFaultException for speculative handling
			// This handles cases like IndexOutOfRangeException, ArgumentOutOfRangeException, etc.
			if (this->isSpeculative())
			{
				this->MarkFaulted();
				return false; // Not ready to commit
			}
			else
			{
				// Wrap and propagate for non-speculative operations
				throw PageFaultException(
					std::format(L"Memory access error at 0x{:X}", getAddress()), ex, getAddress(), false);
			}
		}
	}

	bool LoadMicroOp::CancelPendingControllerRequest()
	{
		if (getCpuPhysicalReadBinding() != nullptr)
		{
			getCpuPhysicalReadBinding()->RevokeUnclaimed();
		}
		bool canceled = _requestController != nullptr && _controllerRequestId && _requestController->TryCancel(_controllerRequestId.value());
		ClearControllerRequestState();
		return canceled;
	}

	void LoadMicroOp::ClearControllerRequestState()
	{
		_requestController = nullptr;
		_controllerRequestId = std::nullopt;
		_controllerAdmissionBackpressured = false;
	}

	void LoadMicroOp::EmitWriteBackRetireRecords(std::shared_ptr<Processor::CPU_Core> &core, std::span<RetireRecord> retireRecords, int &retireRecordCount)
	{
		if (this->isSpeculative() && this->getFaulted())
		{
			return;
		}

		if (getWritesRegister() && getDestRegID() != VLIW_Instruction::NoReg)
		{
			VtId checkedOwner;
			int vtId = TryGetArchitecturalOwnerVtId(checkedOwner) ? checkedOwner.getValue() : NormalizeExecutionVtId(getOwnerThreadId());
			AppendWriteBackRetireRecord(retireRecords, retireRecordCount, RetireRecord::RegisterWrite(vtId, getDestRegID(), _loadedValue));
		}
	}

	bool LoadMicroOp::TryGetPrimaryWriteBackResult(unsigned long long &value)
	{
		value = _loadedValue;
		return getWritesRegister() && getDestRegID() != VLIW_Instruction::NoReg;
	}

	void LoadMicroOp::CapturePrimaryWriteBackResult(unsigned long long value)
	{
		_loadedValue = value;
	}

	std::wstring LoadMicroOp::GetDescription()
	{
		return std::format(L"Load: Addr=0x{:X}, Size={}, Dest=R{}",
			getAddress(), static_cast<unsigned int>(getSize()), getDestRegID());
	}

	unsigned long long StoreMicroOp::getAddress() const
	{
		return Address;
	}

	void StoreMicroOp::setAddress(unsigned long long value)
	{
		Address = value;
	}

	unsigned long long StoreMicroOp::getValue() const
	{
		return Value;
	}

	void StoreMicroOp::setValue(unsigned long long value)
	{
		Value = value;
	}

	unsigned char StoreMicroOp::getSize() const
	{
		return Size;
	}

	void StoreMicroOp::setSize(unsigned char value)
	{
		Size = value;
	}

	unsigned short StoreMicroOp::getSrcRegID() const
	{
		return SrcRegID;
	}

	void StoreMicroOp::setSrcRegID(unsigned short value)
	{
		SrcRegID = value;
	}

	unsigned short StoreMicroOp::getBaseRegID() const
	{
		return BaseRegID;
	}

	void StoreMicroOp::setBaseRegID(unsigned short value)
	{
		BaseRegID = value;
	}

	bool StoreMicroOp::getOwnsPendingWriteCompletion() const
	{
		std::vector<unsigned char> buffer;
		if (_requestController == nullptr || !_controllerRequestId || !TryCreateStoreBuffer(buffer))
		{
			return false;
		}

		return _requestController->OwnsOutstandingSingleLaneScalarStore(_controllerRequestId.value(), 0, getAddress(), getSize(), buffer);
	}

	bool StoreMicroOp::getHasControllerAdmissionBackpressure() const
	{
		return _controllerAdmissionBackpressured && _requestController == nullptr && !_controllerRequestId;
	}

	bool StoreMicroOp::getHasInvalidTransferSize() const
	{
// C# TO C++ CONVERTER TASK: The following 'is' operator pattern is not converted by C# to C++ Converter:
		return getSize() != 1 && getSize() != 2 && getSize() != 4 && getSize() != 8;
	}

	bool StoreMicroOp::isSpeculativeFaultSuppressed() const
	{
		return !getHasInvalidTransferSize() && isSpeculative() && getFaulted();
	}

	bool StoreMicroOp::HasNonSpeculativeFallbackBackendDenial(std::shared_ptr<Processor::CPU_Core> core)
	{
		const bool supportedSize = getSize() == 1 || getSize() == 2 || getSize() == 4 || getSize() == 8;
		return _requestController == nullptr && !_controllerRequestId && supportedSize &&
			!isSpeculative() && !getFaulted() && core->GetBoundMemorySubsystem() == nullptr &&
			!core->HasExactBoundMainMemoryRange(getAddress(), getSize());
	}

	unsigned long long StoreMicroOp::getMemoryAddress() const
	{
		return getAddress();
	}

	void StoreMicroOp::ResolveBaseRelativeAddressForPipeline(std::shared_ptr<Processor::CPU_Core> core)
	{
		if (!getUsesBaseRegisterAddressing())
		{
			return;
		}
		setAddress(ResolveBaseRelativeAddress(core, getBaseRegID()));
		setWriteMemoryRanges(std::vector<MemoryRange>{{getAddress(), static_cast<unsigned long long>(getSize())}});
	}

	void StoreMicroOp::ResolveBaseRelativeAddressForPipeline(unsigned long long baseValue)
	{
		if (!getUsesBaseRegisterAddressing())
		{
			return;
		}
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: Address = unchecked(baseValue + (ulong)AddressDisplacement);
		setAddress(baseValue + static_cast<unsigned long long>(getAddressDisplacement()));
		setWriteMemoryRanges(std::vector<MemoryRange>{{getAddress(), static_cast<unsigned long long>(getSize())}});
	}

	StoreMicroOp::StoreMicroOp()
	{
		setMemoryOp(true);
		setHasSideEffects(true); // Memory writes have side effects
		setClass(MicroOpClass::Lsu);

		// ISA v4 Phase 02: stores are Memory class, MemoryOrdered serialization
		setInstructionClass(Arch::InstructionClass::Memory);
		setSerializationClass(Arch::SerializationClass::MemoryOrdered);

		// Phase 01: Typed-slot taxonomy
		SetClassFlexiblePlacement(SlotClass::LsuClass);
	}

	void StoreMicroOp::InitializeMetadata()
	{
		if (getUsesBaseRegisterAddressing())
		{
			setStealable(false);
			setSerializationClass(Arch::SerializationClass::MemoryOrdered);
		}
		// FSP MMIO Security Policy (Phase: Safety Tags & Certificates):
		// MMIO writes interact directly with hardware states.
		// Mark operations targeting MMIO space as strictly non-stealable.
		constexpr unsigned long long MMIO_BASE = 0xFFFF000000000000ULL;
		if (getAddress() >= MMIO_BASE)
		{
			setStealable(false);
		}

		// Blueprint §3.69 / §5: guard against NoReg sentinels (0xFFFF)
		constexpr unsigned short noReg = VLIW_Instruction::NoReg;

		// Read from source register (value) and base register (address)
		std::vector<int> readRegs;
		if (getSrcRegID() != noReg)
		{
			readRegs.push_back(getSrcRegID());
		}
		if (getBaseRegID() != noReg)
		{
			readRegs.push_back(getBaseRegID());
		}
		setReadRegisters(readRegs);

		// No register writes for store operations
		setWriteRegisters(std::vector<int>{});

		// Memory range to write
		setWriteMemoryRanges(std::vector<MemoryRange>{{getAddress(), static_cast<unsigned long long>(getSize())}});

		// Phase 8: Initialize ResourceMask for GRLB
		setResourceMask(ResourceBitset::Zero);
		// Add register reads (source and base address registers)
		if (getSrcRegID() != noReg)
		{
			ArchRegId sourceRegister;
			const auto sourceResource = ArchRegId::TryCreate(getSrcRegID(), sourceRegister)
				? ResourceMaskBuilder::ForArchitecturalRegisterRead(std::make_shared<ArchRegId>(sourceRegister))
				: ResourceMaskBuilder::ForRegisterRead(getSrcRegID());
			setResourceMask(getResourceMask() | sourceResource);
		}
		if (getBaseRegID() != noReg)
		{
			ArchRegId baseRegister;
			const auto baseResource = ArchRegId::TryCreate(getBaseRegID(), baseRegister)
				? ResourceMaskBuilder::ForArchitecturalRegisterRead(std::make_shared<ArchRegId>(baseRegister))
				: ResourceMaskBuilder::ForRegisterRead(getBaseRegID());
			setResourceMask(getResourceMask() | baseResource);
		}
		// Add LSU store channel
		setResourceMask(getResourceMask() | ResourceMaskBuilder::ForStore());
		// Add memory domain (use owner thread ID as domain)
		setResourceMask(getResourceMask() | ResourceMaskBuilder::ForMemoryDomain(getOwnerThreadId()));

		PublishExplicitStructuralSafetyMask();
		RefreshAdmissionMetadata(shared_from_this());
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public override HybridCPU_ISE.Core.ReplayToken CreateRollbackToken(int ownerThreadId, Processor.MainMemoryArea? mainMemory = null)
	std::shared_ptr<HybridCPU_ISE::Core::ReplayToken> StoreMicroOp::CreateRollbackToken(int ownerThreadId, std::shared_ptr<Processor::MainMemoryArea> mainMemory)
	{
		auto token = std::make_shared<ReplayToken>(mainMemory);
		token->setOwnerThreadId(ownerThreadId);
		token->setHasSideEffects(true);

		// Capture memory state before write
		token->CaptureMemoryState(getAddress(), getSize());

		return token;
	}

	bool StoreMicroOp::Execute(std::shared_ptr<Processor::CPU_Core> &core)
	{
		_controllerAdmissionBackpressured = false;
		try
		{
			if (getUsesBaseRegisterAddressing())
			{
				setAddress(ResolveBaseRelativeAddress(core, getBaseRegID()));
				setWriteMemoryRanges(std::vector<MemoryRange>{{getAddress(), static_cast<unsigned long long>(getSize())}});
			}
			unsigned long long translatedAddress = ResolveCpuTranslatedAddress(core, CpuInstructionTranslationAccessKind::ScalarStore, getSize());
			// A bound subsystem uses RF-10 controller readiness. The
			// controller never publishes the store bytes; selected retire
			// remains the physical mutation owner.
			std::shared_ptr<MemorySubsystem> memSub = core->GetBoundMemorySubsystem();
			if (memSub != nullptr)
			{
				std::vector<unsigned char> buffer;
				if (!TryCreateStoreBuffer(buffer))
				{
					return false;
				}

				std::shared_ptr<MemoryCycleController> controller = memSub->getCycleController();
				if (!_controllerRequestId)
				{
					std::shared_ptr<MemoryAdmissionResult> admission = controller->TryAcceptSingleLaneScalarStore(0, translatedAddress, getSize(), buffer);
					if (admission->Status == MemoryAdmissionStatus::Backpressured)
					{
						_controllerAdmissionBackpressured = true;
						return false;
					}
					if (admission->Status == MemoryAdmissionStatus::Rejected)
					{
						throw InvalidOperationException(admission->Reason.value_or(
							L"MemoryCycleController rejected a single-lane scalar store without a reason."));
					}

					_requestController = controller;
					_controllerRequestId = std::make_shared<MemoryRequestId>(admission->RequestId);
					return false;
				}

				if (_requestController != controller)
				{
					if (_requestController != nullptr)
					{
						_requestController->TryCancel(_controllerRequestId.value());
					}
					ClearControllerRequestState();
					throw InvalidOperationException(L"StoreMicroOp controller binding changed while an accepted request was outstanding.");
				}

				std::shared_ptr<MemoryCompletion> completion;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: if (!controller.TryTakeCompletion(_controllerRequestId.Value, out MemoryCompletion? completion))
				if (!controller->TryTakeCompletion(_controllerRequestId.value(), completion))
				{
					return false;
				}

				ClearControllerRequestState();
				if (completion == nullptr || !completion->getSucceeded())
				{
					const std::wstring failureReason = completion != nullptr
						? completion->getFailureReason()
						: std::wstring{};
					throw InvalidOperationException(
						L"StoreMicroOp.Execute(): accepted readiness request did not complete successfully. " +
						(failureReason.empty()
							? L"MemoryCycleController returned an invalid single-lane scalar-store completion."
							: failureReason));
				}

				return true;
			}

			// Fallback to synchronous implementation if MemorySubsystem is not available
			if (HasExactMainMemoryRange(core, translatedAddress, getSize()))
			{
				std::vector<unsigned char> buffer;
				if (!TryCreateStoreBuffer(buffer))
				{
					return false;
				}

				core->WriteBoundMainMemoryExact(translatedAddress, buffer, L"StoreMicroOp.Execute()");
				return true;
			}

			// Phase 7: Out-of-bounds access handling for speculative operations
			if (this->isSpeculative())
			{
				this->MarkFaulted();
				return false; // Not ready to commit
			}

			return false;
		}
		catch (const CpuInstructionTranslationFaultException &e1)
		{
			if (this->isSpeculative())
			{
				this->MarkFaulted();
				return false;
			}
			throw;
		}
		catch (const PageFaultException &e2)
		{
			// Phase 7: Speculative FSP with Silent Squash
			// If this is a speculative operation, suppress the exception and mark as faulted
			if (this->isSpeculative())
			{
				this->MarkFaulted();
				return false; // Not ready to commit
			}
			else
			{
				// Non-speculative operation: propagate exception normally
				throw;
			}
		}
		catch (const std::runtime_error &ex)
		{
			// Phase 7: Convert other memory exceptions to PageFaultException for speculative handling
			// This handles cases like IndexOutOfRangeException, ArgumentOutOfRangeException, etc.
			if (this->isSpeculative())
			{
				this->MarkFaulted();
				return false; // Not ready to commit
			}
			else
			{
				// Wrap and propagate for non-speculative operations
				throw PageFaultException(std::format(L"Memory write error at 0x{:X}", getAddress()), ex, getAddress(), true);
			}
		}
	}

	bool StoreMicroOp::CancelPendingControllerRequest()
	{
		bool canceled = _requestController != nullptr && _controllerRequestId && _requestController->TryCancel(_controllerRequestId.value());
		ClearControllerRequestState();
		return canceled;
	}

	void StoreMicroOp::ClearControllerRequestState()
	{
		_requestController = nullptr;
		_controllerRequestId = std::nullopt;
		_controllerAdmissionBackpressured = false;
	}

	bool StoreMicroOp::TryCreateStoreBuffer(std::vector<unsigned char> &buffer) const
	{
		if (getSize() != 1 && getSize() != 2 && getSize() != 4 && getSize() != 8)
		{
			buffer.clear();
			return false;
		}
		buffer = WriteLittleEndianUnsigned(getValue(), getSize());
		return !buffer.empty();
	}
	std::wstring StoreMicroOp::GetDescription()
	{
		return std::format(L"Store: Addr=0x{:X}, Value=0x{:X}, Size={}", getAddress(), getValue(), static_cast<unsigned int>(getSize()));
	}
}
