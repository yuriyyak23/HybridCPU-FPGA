#include "MicroOp.h"
#include "../../../Architecture/Common/StreamEngineId.h"
#include <algorithm>
#include "../Replay/ReplayToken.h"
#include "../../Scheduling/PostStageBIssuedAttempt.h"
#include "../../../Architecture/Common/YAKSys_Hybrid_CPU.Processor.h"
#include "../../../Diagnostics/YAKSys_Hybrid_CPU.Core.InstructionRegistry.h"
#include "../../Metadata/MicroOpAdmissionMetadata.h"

using namespace HybridCPU_ISE::Arch;
using namespace YAKSys_Hybrid_CPU::Core::Memory;
using namespace YAKSys_Hybrid_CPU::Core::Registers;
using namespace YAKSys_Hybrid_CPU::Core::Registers::Retire;

namespace YAKSys_Hybrid_CPU::Core
{
    const MicroOpAdmissionMetadata MicroOpAdmissionMetadata::Default{};

    std::shared_ptr<MicroOpAdmissionMetadata> MicroOpAdmissionMetadata::Create(
        std::shared_ptr<const MicroOp> microOp, SafetyMask128 structuralSafetyMask)
    {
        ArgumentNullException::ThrowIfNull(microOp);

        auto readRegisters = microOp->getReadRegisters();
        auto writeRegisters = microOp->getWriteRegisters();
        std::vector<std::pair<unsigned long long, unsigned long long>> readMemoryRanges;
        std::vector<std::pair<unsigned long long, unsigned long long>> writeMemoryRanges;

        const auto &microOpReadRanges = microOp->getReadMemoryRanges();
        readMemoryRanges.reserve(microOpReadRanges.size());
        for (const auto &range : microOpReadRanges)
        {
            readMemoryRanges.emplace_back(range.Address, range.Length);
        }

        const auto &microOpWriteRanges = microOp->getWriteMemoryRanges();
        writeMemoryRanges.reserve(microOpWriteRanges.size());
        for (const auto &range : microOpWriteRanges)
        {
            writeMemoryRanges.emplace_back(range.Address, range.Length);
        }

        auto normalizedReadMemoryRanges = readMemoryRanges;
        Pipeline::Metadata::ReadRangeMetadataHelper::TryNormalizeContiguousReadRanges(
            readMemoryRanges, normalizedReadMemoryRanges);

        const auto assistDescriptor = Pipeline::Metadata::ReadRangeMetadataHelper::BuildAssistCoalescingDescriptor(
            readMemoryRanges, normalizedReadMemoryRanges);
        Pipeline::Metadata::ReadRangeMetadataHelper::ValidateCoalescedRangeMetadata(
            readMemoryRanges, normalizedReadMemoryRanges, assistDescriptor);
        if (!assistDescriptor)
        {
            throw std::invalid_argument("assistDescriptor");
        }
        auto metadata = std::make_shared<MicroOpAdmissionMetadata>(*assistDescriptor);
        metadata->ReadRegisters = std::move(readRegisters);
        metadata->WriteRegisters = std::move(writeRegisters);
        metadata->ReadMemoryRanges = std::move(readMemoryRanges);
        metadata->NormalizedReadMemoryRanges = std::move(normalizedReadMemoryRanges);
        metadata->WriteMemoryRanges = std::move(writeMemoryRanges);
        metadata->RegisterHazardMask = BuildRegisterHazardMask(
            metadata->ReadRegisters, metadata->WriteRegisters);

        const auto registryMicroOp = std::const_pointer_cast<MicroOp>(microOp);
        if (structuralSafetyMask.isZero() &&
            !microOp->allowsStructuralSafetyFallback() &&
            InstructionRegistry::RequiresExplicitStructuralSafetyMask(registryMicroOp))
        {
            throw InstructionRegistry::CreateMissingExplicitStructuralSafetyMaskException(
                registryMicroOp, L"MicroOpAdmissionMetadata.Create()");
        }

        const auto placement = microOp->getPlacement();
        metadata->IsStealable = microOp->isStealable();
        metadata->IsControlFlow = microOp->isControlFlow();
        metadata->IsMemoryOp = microOp->isMemoryOp();
        metadata->WritesRegister = microOp->getWritesRegister();
        metadata->HasSideEffects = microOp->getHasSideEffects();
        metadata->OwnerContextId = microOp->getOwnerContextId();
        metadata->DomainTag = placement.DomainTag;
        metadata->Placement = placement;
        metadata->StructuralSafetyMask = structuralSafetyMask;
        metadata->HasStructuralSafetyMask = structuralSafetyMask.isNonZero();
        metadata->SharedStructuralMask = SafetyMask128(
            structuralSafetyMask.Low & ~0xFFFF'FFFFULL, structuralSafetyMask.High);
        metadata->CertificateMask = SafetyMask128(
            (structuralSafetyMask.Low & ~0xFFFF'FFFFULL) | metadata->RegisterHazardMask,
            structuralSafetyMask.High);
        return metadata;
    }

    unsigned int MicroOpAdmissionMetadata::BuildRegisterHazardMask(
        const std::vector<int> &readRegisters, const std::vector<int> &writeRegisters)
    {
        unsigned int mask = 0;
        const auto clampRegisterGroup = [](int registerId)
        {
            if (registerId < 0)
            {
                return 0;
            }
            const int group = registerId / 4;
            return group < 16 ? group : 15;
        };

        for (const int registerId : readRegisters)
        {
            mask |= 1U << clampRegisterGroup(registerId);
        }
        for (const int registerId : writeRegisters)
        {
            mask |= 1U << (16 + clampRegisterGroup(registerId));
        }
        return mask;
    }

    std::shared_ptr<HybridCPU_ISE::Core::ReplayToken> MicroOp::CreateRollbackToken(int ownerThreadId, std::shared_ptr<Processor::MainMemoryArea> mainMemory)
    {
        auto token = std::make_shared<HybridCPU_ISE::Core::ReplayToken>(mainMemory);
        token->setOwnerThreadId(ownerThreadId);
        token->setHasSideEffects(getHasSideEffects());
        return token;
    }

	int ResourceMaskBuilder::RequireResourceId(int resourceId, int resourceCount, const std::wstring &parameterName, const std::wstring &resourceName)
	{
		if (static_cast<unsigned int>(resourceId) >= static_cast<unsigned int>(resourceCount))
		{
			throw ArgumentOutOfRangeException(parameterName, resourceId, std::format(
				L"Invalid {} id; expected [0, {}]. Resource 0 is valid and must not be used as a fallback.",
				resourceName, resourceCount - 1));
		}

		return resourceId;
	}

	ResourceBitset ResourceMaskBuilder::ForMemoryBank(int bankId)
	{
		bankId = RequireResourceId(bankId, 16, L"bankId", L"memory-bank");
		return ResourceBitset(0, 1ULL << (EXT_MEM_BANK_BASE + bankId));
	}

	ResourceBitset ResourceMaskBuilder::ForRegisterRead(int regId, int vtId)
	{
		vtId = RequireResourceId(vtId, Processor::CPU_Core::SmtWays, L"vtId", L"virtual-thread");
		int group = (vtId * 4) + (regId / 8);
		if (group >= 16)
		{
			group = 15;
		}
		return ResourceBitset(1ULL << (REG_READ_BASE + group), 0);
	}

	ResourceBitset ResourceMaskBuilder::ForRegisterWrite(int regId, int vtId)
	{
		vtId = RequireResourceId(vtId, Processor::CPU_Core::SmtWays, L"vtId", L"virtual-thread");
		int group = (vtId * 4) + (regId / 8);
		if (group >= 16)
		{
			group = 15;
		}
		return ResourceBitset(1ULL << (REG_WRITE_BASE + group), 0);
	}

	ResourceBitset ResourceMaskBuilder::ForArchitecturalRegisterRead(std::shared_ptr<ArchRegId> regId, int vtId)
	{
		return ForRegisterRead(regId->getValue(), vtId);
	}

	ResourceBitset ResourceMaskBuilder::ForArchitecturalRegisterWrite(std::shared_ptr<ArchRegId> regId, int vtId)
	{
		return ForRegisterWrite(regId->getValue(), vtId);
	}

	ResourceBitset ResourceMaskBuilder::ForRegisterRead(int regId)
	{
		int group = regId / 4;
		if (group >= 16)
		{
			group = 15; // Clamp to 16 groups
		}
		return ResourceBitset(1ULL << (REG_READ_BASE + group), 0);
	}

	ResourceBitset ResourceMaskBuilder::ForRegisterWrite(int regId)
	{
		int group = regId / 4;
		if (group >= 16)
		{
			group = 15; // Clamp to 16 groups
		}
		return ResourceBitset(1ULL << (REG_WRITE_BASE + group), 0);
	}

	ResourceBitset ResourceMaskBuilder::ForArchitecturalRegisterRead(std::shared_ptr<ArchRegId> regId)
	{
		return ForRegisterRead(regId->getValue());
	}

	ResourceBitset ResourceMaskBuilder::ForArchitecturalRegisterWrite(std::shared_ptr<ArchRegId> regId)
	{
		return ForRegisterWrite(regId->getValue());
	}

	ResourceBitset ResourceMaskBuilder::ForMemoryDomain(int domainId)
	{
		domainId = RequireResourceId(domainId, 16, L"domainId", L"memory-domain");
		return ResourceBitset(1ULL << (MEM_DOMAIN_BASE + domainId), 0);
	}

	ResourceBitset ResourceMaskBuilder::ForLoad()
	{
		return ResourceBitset(1ULL << LSU_LOAD_BIT, 0);
	}

	ResourceBitset ResourceMaskBuilder::ForStore()
	{
		return ResourceBitset(1ULL << LSU_STORE_BIT, 0);
	}

	ResourceBitset ResourceMaskBuilder::ForAtomic()
	{
		return ResourceBitset(1ULL << LSU_ATOMIC_BIT, 0);
	}

	ResourceBitset ResourceMaskBuilder::ForDMAChannel(int channelId)
	{
		channelId = RequireResourceId(channelId, 4, L"channelId", L"DMA-channel");
		return ResourceBitset(1ULL << (DMA_CHANNEL_BASE + channelId), 0);
	}

	ResourceBitset ResourceMaskBuilder::ForStreamEngine(int engineId)
	{
		engineId = RequireResourceId(engineId, 4, L"engineId", L"stream-engine");
		return ResourceBitset(1ULL << (STREAM_ENGINE_BASE + engineId), 0);
	}

	ResourceBitset ResourceMaskBuilder::ForStreamEngine(std::shared_ptr<StreamEngineId> engineId)
	{
		if (engineId == nullptr) { throw std::invalid_argument("engineId"); }
		return ForStreamEngine(engineId->getValue());
	}

	ResourceBitset ResourceMaskBuilder::ForMatrixTileStreamWindow()
	{
		return ResourceBitset(0ULL, 1ULL << (MATRIX_TILE_BASE + 0));
	}

	ResourceBitset ResourceMaskBuilder::ForMatrixTileIngress()
	{
		return ResourceBitset(0ULL, 1ULL << (MATRIX_TILE_BASE + 1));
	}

	ResourceBitset ResourceMaskBuilder::ForMatrixTileEgress()
	{
		return ResourceBitset(0ULL, 1ULL << (MATRIX_TILE_BASE + 2));
	}

	ResourceBitset ResourceMaskBuilder::ForMatrixTileStateRead()
	{
		return ResourceBitset(0ULL, 1ULL << (MATRIX_TILE_BASE + 3));
	}

	ResourceBitset ResourceMaskBuilder::ForMatrixTileStateWrite()
	{
		return ResourceBitset(0ULL, 1ULL << (MATRIX_TILE_BASE + 4));
	}

	ResourceBitset ResourceMaskBuilder::ForMatrixTileAccumulatorRead()
	{
		return ResourceBitset(0ULL, 1ULL << (MATRIX_TILE_BASE + 5));
	}

	ResourceBitset ResourceMaskBuilder::ForMatrixTileAccumulatorWrite()
	{
		return ResourceBitset(0ULL, 1ULL << (MATRIX_TILE_BASE + 6));
	}

	ResourceBitset ResourceMaskBuilder::ForMatrixTileTransposePolicy()
	{
		return ResourceBitset(0ULL, 1ULL << (MATRIX_TILE_BASE + 7));
	}

	ResourceBitset ResourceMaskBuilder::ForAccelerator(int accelId)
	{
		accelId = RequireResourceId(accelId, 4, L"accelId", L"accelerator");
		return ResourceBitset(1ULL << (ACCEL_BASE + accelId), 0);
	}

	SafetyMask128 ResourceMaskBuilder::ForRegisterRead128(int regId)
	{
		ResourceBitset res = ForRegisterRead(regId);
		return SafetyMask128(res.Low, res.High);
	}

	SafetyMask128 ResourceMaskBuilder::ForRegisterWrite128(int regId)
	{
		ResourceBitset res = ForRegisterWrite(regId);
		return SafetyMask128(res.Low, res.High);
	}

	SafetyMask128 ResourceMaskBuilder::ForArchitecturalRegisterRead128(std::shared_ptr<ArchRegId> regId)
	{
		ResourceBitset res = ForArchitecturalRegisterRead(regId);
		return SafetyMask128(res.Low, res.High);
	}

	SafetyMask128 ResourceMaskBuilder::ForArchitecturalRegisterWrite128(std::shared_ptr<ArchRegId> regId)
	{
		ResourceBitset res = ForArchitecturalRegisterWrite(regId);
		return SafetyMask128(res.Low, res.High);
	}

	SafetyMask128 ResourceMaskBuilder::ForMemoryDomain128(int domainId)
	{
		ResourceBitset res = ForMemoryDomain(domainId);
		return SafetyMask128(res.Low, res.High);
	}

	SafetyMask128 ResourceMaskBuilder::ForMemoryBank128(int bankId)
	{
		ResourceBitset res = ForMemoryBank(bankId);
		return SafetyMask128(res.Low, res.High);
	}

	SafetyMask128 ResourceMaskBuilder::ForLoad128()
	{
		ResourceBitset res = ForLoad();
		return SafetyMask128(res.Low, res.High);
	}

	SafetyMask128 ResourceMaskBuilder::ForStore128()
	{
		ResourceBitset res = ForStore();
		return SafetyMask128(res.Low, res.High);
	}

	SafetyMask128 ResourceMaskBuilder::ForAtomic128()
	{
		ResourceBitset res = ForAtomic();
		return SafetyMask128(res.Low, res.High);
	}

	SafetyMask128 ResourceMaskBuilder::ForDMAChannel128(int channelId)
	{
		ResourceBitset res = ForDMAChannel(channelId);
		return SafetyMask128(res.Low, res.High);
	}

	SafetyMask128 ResourceMaskBuilder::ForStreamEngine128(int engineId)
	{
		ResourceBitset res = ForStreamEngine(engineId);
		return SafetyMask128(res.Low, res.High);
	}

	SafetyMask128 ResourceMaskBuilder::ForStreamEngine128(std::shared_ptr<StreamEngineId> engineId)
	{
		ResourceBitset res = ForStreamEngine(engineId);
		return SafetyMask128(res.Low, res.High);
	}

	SafetyMask128 ResourceMaskBuilder::ForAccelerator128(int accelId)
	{
		ResourceBitset res = ForAccelerator(accelId);
		return SafetyMask128(res.Low, res.High);
	}

	SafetyMask128 ResourceMaskBuilder::ForExtendedGRLBChannel(int channelId)
	{
		channelId = RequireResourceId(channelId, 32, L"channelId", L"extended-GRLB-channel");
		return SafetyMask128(0, 1ULL << (EXT_GRLB_BASE + channelId));
	}

	SafetyMask128 ResourceMaskBuilder::ForExtendedMemoryDomain(int domainId)
	{
		domainId = RequireResourceId(domainId, 16, L"domainId", L"extended-memory-domain");
		return SafetyMask128(0, 1ULL << (EXT_MEM_DOMAIN_BASE + domainId));
	}

	std::shared_ptr<YAKSys_Hybrid_CPU::Core::PostStageBIdentityTemplate> MicroOp::getPostStageBIdentityTemplate() const
	{
		return PostStageBIdentityTemplate;
	}

	void MicroOp::setPostStageBIdentityTemplate(std::shared_ptr<YAKSys_Hybrid_CPU::Core::PostStageBIdentityTemplate> value)
	{
		PostStageBIdentityTemplate = value;
	}

	std::shared_ptr<YAKSys_Hybrid_CPU::Core::PostStageBIssuedAttempt> MicroOp::getPostStageBIssuedAttempt() const
	{
		return PostStageBIssuedAttempt;
	}

	void MicroOp::setPostStageBIssuedAttempt(std::shared_ptr<YAKSys_Hybrid_CPU::Core::PostStageBIssuedAttempt> value)
	{
		PostStageBIssuedAttempt = value;
	}

	bool MicroOp::isSoftwareSecureSource() const
	{
		return IsSoftwareSecureSource;
	}

	void MicroOp::setSoftwareSecureSource(bool value)
	{
		IsSoftwareSecureSource = value;
	}

	void MicroOp::BindSoftwareSecureSource()
	{
		setSoftwareSecureSource(true);
	}

	void MicroOp::RefreshAdmissionMetadata(std::shared_ptr<MicroOp> microOp)
	{
		ArgumentNullException::ThrowIfNull(microOp);
		microOp->_admissionMetadata = MicroOpAdmissionMetadata::Create(microOp, microOp->getSafetyMask());
		microOp->_hasAdmissionMetadata = true;
	}

	unsigned int MicroOp::getOpCode() const
	{
		return OpCode;
	}

	void MicroOp::setOpCode(unsigned int value)
	{
		OpCode = value;
	}

	unsigned short MicroOp::getPredicateMask() const
	{
		return PredicateMask;
	}

	void MicroOp::setPredicateMask(unsigned short value)
	{
		PredicateMask = value;
	}

	unsigned short MicroOp::getDestRegID() const
	{
		return DestRegID;
	}

	void MicroOp::setDestRegID(unsigned short value)
	{
		DestRegID = value;
	}

	bool MicroOp::getWritesRegister() const
	{
		return WritesRegister;
	}

	void MicroOp::setWritesRegister(bool value)
	{
		WritesRegister = value;
	}

	unsigned char MicroOp::getLatency() const
	{
		return Latency;
	}

	void MicroOp::setLatency(unsigned char value)
	{
		Latency = value;
	}

	bool MicroOp::isMemoryOp() const
	{
		return IsMemoryOp;
	}

	void MicroOp::setMemoryOp(bool value)
	{
		IsMemoryOp = value;
	}

	bool MicroOp::isControlFlow() const
	{
		return IsControlFlow;
	}

	void MicroOp::setControlFlow(bool value)
	{
		IsControlFlow = value;
	}

	Arch::InstructionClass MicroOp::getInstructionClass() const
	{
		return InstructionClass;
	}

	void MicroOp::setInstructionClass(Arch::InstructionClass value)
	{
		InstructionClass = value;
	}

	Arch::SerializationClass MicroOp::getSerializationClass() const
	{
		return SerializationClass;
	}

	void MicroOp::setSerializationClass(Arch::SerializationClass value)
	{
		SerializationClass = value;
	}

	CanonicalDecodePublicationMode MicroOp::getCanonicalDecodePublication() const
	{
		return CanonicalDecodePublicationMode::Unspecified;
	}

	bool MicroOp::getAllowsStructuralSafetyFallback() const
	{
		return false;
	}

	bool MicroOp::isStealable() const
	{
		return _isStealable;
	}

	void MicroOp::setStealable(bool value)
	{
		_isStealable = value;
		_hasAdmissionMetadata = false;
	}

	LocalityHint MicroOp::getMemoryLocalityHint() const
	{
		return MemoryLocalityHint;
	}

	void MicroOp::setMemoryLocalityHint(LocalityHint value)
	{
		MemoryLocalityHint = value;
	}

	std::shared_ptr<MicroOpAdmissionMetadata> MicroOp::getAdmissionMetadata() const
	{
		return _hasAdmissionMetadata ? _admissionMetadata : MicroOpAdmissionMetadata::Create(shared_from_this(), getSafetyMask());
	}

	void MicroOp::RefreshAdmissionMetadata()
	{
		_admissionMetadata.reset();
		_hasAdmissionMetadata = false;
	}

	void MicroOp::ValidatePublishedWriteRegisterContract(const std::wstring &producerSurface) const
	{
		const auto admissionMetadata = getAdmissionMetadata();
		const auto &writeRegisters = admissionMetadata->WriteRegisters;
		const auto hasPublishedDestination = DestRegID != 0 &&
			DestRegID != VLIW_Instruction::NoReg &&
			DestRegID != ArchRegisterTripletEncoding::NoArchReg;

		auto formatRegisters = [&writeRegisters]()
		{
			std::string result;
			for (std::size_t index = 0; index < writeRegisters.size(); ++index)
			{
				if (index != 0) result += ", ";
				result += std::to_string(writeRegisters[index]);
			}
			return result;
		};

		if (admissionMetadata->WritesRegister)
		{
			if (writeRegisters.empty())
			{
				throw InvalidOperationException(std::format(L"{} observed MicroOp (opcode 0x{:X}) with WritesRegister=true but an empty canonical WriteRegisters list. DestRegID is convenience only; decode/registration must publish authoritative write-register facts before runtime transport.", producerSurface, OpCode));
			}

			if (hasPublishedDestination && std::find(writeRegisters.begin(), writeRegisters.end(), static_cast<int>(DestRegID)) == writeRegisters.end())
			{
				const auto registers = formatRegisters();
				throw InvalidOperationException(std::format(L"{} observed MicroOp (opcode 0x{:X}) with DestRegID={} missing from canonical WriteRegisters [{}]. DestRegID is convenience only; canonical dependency truth must live in WriteRegisters.", producerSurface, OpCode, DestRegID, std::wstring(registers.begin(), registers.end())));
			}
			return;
		}

		if (!writeRegisters.empty())
		{
			const auto registers = formatRegisters();
			throw InvalidOperationException(std::format(L"{} observed MicroOp (opcode 0x{:X}) with WritesRegister=false but canonical WriteRegisters [{}].", producerSurface, OpCode, std::wstring(registers.begin(), registers.end())));
		}
	}

	void MicroOp::PublishExplicitStructuralSafetyMask()
	{
		setSafetyMask(InstructionRegistry::BuildExplicitStructuralSafetyMask(shared_from_this()));
	}

	bool MicroOp::isFspInjected() const
	{
		return IsFspInjected;
	}

	void MicroOp::setFspInjected(bool value)
	{
		IsFspInjected = value;
	}

	bool MicroOp::isAssist() const
	{
		return false;
	}

	bool MicroOp::isRetireVisible() const
	{
		return true;
	}

	bool MicroOp::isReplayDiscardable() const
	{
		return false;
	}

	bool MicroOp::getSuppressesArchitecturalFaults() const
	{
		return false;
	}

	const std::vector<int> MicroOp::getReadRegisters() const
	{
		return ReadRegisters;
	}

	void MicroOp::setReadRegisters(const std::vector<int> &value)
	{
		ReadRegisters = value;
	}

	const std::vector<int> MicroOp::getWriteRegisters() const
	{
		return WriteRegisters;
	}

	void MicroOp::setWriteRegisters(const std::vector<int> &value)
	{
		WriteRegisters = value;
	}

	int MicroOp::NormalizeExecutionVtId(int ownerThreadId)
	{
		VtId checkedOwner;
		if (!VtId::TryCreate(ownerThreadId, checkedOwner))
		{
			throw std::out_of_range(std::format("Execution owner thread id is invalid; expected [0, {}].", Processor::CPU_Core::SmtWays - 1));
		}
		return checkedOwner.getValue();
	}

	bool MicroOp::TryReadUnifiedArchValue(const std::shared_ptr<Processor::CPU_Core>& core, int vtId, int archReg, unsigned long long& value)
	{
		value = 0;
		if (archReg == 0)
		{
			return true;
		}
		if (static_cast<unsigned int>(archReg) >= static_cast<unsigned int>(RenameMap::ArchRegs))
		{
			return false;
		}
		if (!core || !core->getArchRenameMap() || !core->getPhysicalRegisters() || !core->getArchContexts())
		{
			return false;
		}
		value = core->ReadArch(vtId, archReg);
		return true;
	}

	unsigned long long MicroOp::ReadUnifiedScalarSourceOperand(const std::shared_ptr<Processor::CPU_Core>& core, int vtId, unsigned short rawRegId)
	{
		if (rawRegId == VLIW_Instruction::NoReg)
		{
			return 0;
		}

		int archRegId;
		unsigned long long value;
		return TryNormalizeFlatArchRegId(rawRegId, archRegId) && TryReadUnifiedArchValue(core, vtId, archRegId, value) ? value : 0;
	}

	
}
