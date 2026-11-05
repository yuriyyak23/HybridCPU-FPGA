#include "Rf06ScalarLegacyProjection.h"
#include "../../../../CloseToHSL/Core/Pipeline/Scheduling/SlotLegality/SlotClassDefinitions.h"
#include "../../../../NonRTL/Arch/InstructionClass.h"
#include "../../../../NonRTL/Arch/SerializationClass.h"
#include "../../../../CloseToHSL/Core/Pipeline/MicroOps/Types/MicroOp.h"
#include "../../../../CloseToHSL/Core/Decoder/Sha256Digest.h"
#include <sstream>
#include <span>
#include <codecvt>
#include <locale>

using namespace HybridCPU_ISE::Arch;
using namespace YAKSys_Hybrid_CPU::Arch;
using namespace YAKSys_Hybrid_CPU::Core::Pipeline::MicroOps;
using namespace YAKSys_Hybrid_CPU::Core::Registers;
using OpcodeValues = YAKSys_Hybrid_CPU::Processor::CPU_Core::IsaOpcodeValues;

namespace YAKSys_Hybrid_CPU::Core::Decoder
{
    namespace {
        template<typename T, size_t N>
        std::wstring EnumName(T value, const wchar_t* const (&names)[N]) {
            const auto index = static_cast<unsigned int>(value);
            return index < N && names[index] ? names[index] : std::to_wstring(index);
        }
        const wchar_t* const instructionNames[] = { L"ScalarAlu", L"Memory", L"ControlFlow", L"Atomic", L"System", L"Csr", L"SmtVt", L"Vmx" };
        const wchar_t* const serializationNames[] = { L"Free", L"MemoryOrdered", L"FullSerial", L"AtomicSerial", L"CsrOrdered", L"VmxSerial" };
        const wchar_t* const slotNames[] = { L"AluClass", L"LsuClass", L"DmaStreamClass", L"BranchControl", L"SystemSingleton", L"MatrixTileStreamClass", nullptr, L"Unclassified" };
        const wchar_t* const pinNames[] = { L"ClassFlexible", L"HardPinned" };
        const wchar_t* const memoryNames[] = { L"NoMemory", L"Load", L"Store", L"Atomic" };
        const wchar_t* BoolName(bool value) { return value ? L"True" : L"False"; }
        void AppendRegisters(std::wostream& out, const std::vector<int>& registers) {
            bool first = true;
            for (int value : registers) { if (!first) out << L','; out << value; first = false; }
        }
    }
	std::shared_ptr<ScalarLegacyProjectionFingerprint> ScalarLegacyProjectionFingerprint::Capture(std::shared_ptr<ExecutionContract> contract, std::shared_ptr<ScalarALUMicroOp> carrier)
	{
		if (!contract || !carrier)
		{
			throw std::invalid_argument("RF-06.2 fingerprint requires both contract and carrier.");
		}
		const auto binding = contract->getGeneratedBinding();
		const auto placement = contract->getPlacement();
		const auto memory = contract->getMemory();
		std::wostringstream canonical;
		canonical << binding->Opcode << L'|' << binding->MaterializerId.Value << L'|'
			<< binding->RuntimeExecutionProviderId.Value << L'|' << binding->LatencyModelId.Value << L'|'
			<< binding->DescriptorFingerprint << L'|' << contract->getRuntimeProvider()->Id.Value << L'|'
			<< contract->getRuntimeProvider()->PayloadSchema << L'|'
			<< EnumName(contract->getInstructionClass(), instructionNames) << L'|'
			<< EnumName(contract->getSerializationClass(), serializationNames) << L'|'
			<< EnumName(placement->RequiredSlotClass, slotNames) << L'|'
			<< EnumName(placement->PinningKind, pinNames) << L'|'
			<< static_cast<unsigned int>(placement->PinnedLaneId) << L'|' << placement->DomainTag << L'|'
			<< contract->getStaticEffectContract() << L'|'
			<< EnumName(memory->Kind, memoryNames) << L'|'
			<< BoolName(contract->getStaticMemoryPlan() != nullptr) << L'|';
		AppendRegisters(canonical, contract->getReadRegisters());
		canonical << L'|';
		AppendRegisters(canonical, contract->getWriteRegisters());
		const auto contractMask = contract->getResourceMask();
		canonical << L'|' << contractMask.Low << L'|' << contractMask.High << L'|'
			<< carrier->getOpCode() << L'|' << carrier->getDestRegID() << L'|'
			<< carrier->getSrc1RegID() << L'|' << carrier->getSrc2RegID() << L'|'
			<< carrier->getImmediate() << L'|' << BoolName(carrier->getUsesImmediate()) << L'|'
			<< BoolName(carrier->getWritesRegister()) << L'|' << BoolName(carrier->isMemoryOp()) << L'|' << static_cast<unsigned int>(carrier->getLatency()) << L'|'
			<< EnumName(carrier->getInstructionClass(), instructionNames) << L'|'
			<< EnumName(carrier->getSerializationClass(), serializationNames) << L'|';
		AppendRegisters(canonical, carrier->getReadRegisters());
		canonical << L'|';
		AppendRegisters(canonical, carrier->getWriteRegisters());
		const auto carrierMask = carrier->getResourceMask();
		canonical << L'|' << carrierMask.Low << L'|' << carrierMask.High;
		const std::string payload = std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>>{}.to_bytes(canonical.str());
		const auto bytes = std::span<const unsigned char>(reinterpret_cast<const unsigned char*>(payload.data()), payload.size());
		return std::make_shared<ScalarLegacyProjectionFingerprint>(ScalarLegacyProjectionFingerprint{Sha256Hex(bytes)});
	}

	void ScalarLegacyProjectionFingerprint::EnsureCurrent(std::shared_ptr<ScalarLegacyProjectionFingerprint> expected, std::shared_ptr<ExecutionContract> contract, std::shared_ptr<ScalarALUMicroOp> carrier)
	{
		if (!expected || Capture(contract, carrier)->Value != expected->Value)
		{
			throw std::runtime_error("RF-06.2 scalar legacy projection is stale after carrier mutation.");
		}
	}

	bool Rf06ScalarLegacyProjection::IsSupportedOpcode(unsigned int opcode)
	{
		return (opcode == OpcodeValues::ADD || opcode == OpcodeValues::SUB || opcode == OpcodeValues::AND || opcode == OpcodeValues::OR || opcode == OpcodeValues::XOR);
	}

	std::shared_ptr<ExecutionContract> Rf06ScalarLegacyProjection::CreateContract(std::shared_ptr<CanonicalDecodedInstruction> canonical)
	{
		std::shared_ptr<GeneratedStaticBinding> binding;
		ValidateCanonicalFamily(canonical, binding);

		std::vector<int> reads = {canonical->Rs1, canonical->Rs2};
		std::vector<int> writes = {canonical->Rd};

		auto runtimeProvider = std::make_shared<RuntimeExecutionProviderBinding>(binding->RuntimeExecutionProviderId, L"scalar-reg-reg-v1");
		return ExecutionContract::Create(binding, runtimeProvider, InstructionClass::ScalarAlu, SerializationClass::Free, ExecutionPlacement::Create(SlotClass::AluClass, SlotPinningKind::ClassFlexible), L"RegisterWrite", MemoryCapability::None, reads, writes, BuildRegisterResourceMask(canonical), true, true, false);
	}

	std::shared_ptr<CheckedScalarLegacyProjection> Rf06ScalarLegacyProjection::Project(std::shared_ptr<CanonicalDecodedInstruction> canonical, std::shared_ptr<ExecutionContract> contract)
	{
		std::shared_ptr<GeneratedStaticBinding> binding;
		ValidateCanonicalFamily(canonical, binding);
		if (!contract) throw std::invalid_argument("contract is null");

// C# TO C++ CONVERTER TASK: The following .NET 'String.Equals' reference is not converted:
		if ((!contract->getGeneratedBinding() || *contract->getGeneratedBinding() != *binding) || contract->getRuntimeProvider()->Id != binding->RuntimeExecutionProviderId || contract->getInstructionClass() != InstructionClass::ScalarAlu || contract->getSerializationClass() != SerializationClass::Free || contract->getPlacement()->RequiredSlotClass != SlotClass::AluClass || contract->getPlacement()->PinningKind != SlotPinningKind::ClassFlexible || contract->getMemory()->Kind != MemoryCapabilityKind::NoMemory || contract->getStaticMemoryPlan() != nullptr || contract->getRuntimeProvider()->PayloadSchema != L"scalar-reg-reg-v1" || contract->getReadRegisters() != std::vector<int>{canonical->Rs1, canonical->Rs2} || contract->getWriteRegisters() != std::vector<int>{canonical->Rd} || contract->getResourceMask() != BuildRegisterResourceMask(canonical) || !contract->isStealable() || !contract->isRetireVisible() || contract->isAssist() || contract->getStaticEffectContract() != L"RegisterWrite")
		{
			throw InvalidOperationException(L"The scalar legacy projection requires the matching frozen scalar execution contract.");
		}

		auto carrier = std::make_shared<ScalarALUMicroOp>();
		carrier->setOpCode(canonical->Opcode);
		carrier->setDestRegID(canonical->Rd);
		carrier->setSrc1RegID(canonical->Rs1);
		carrier->setSrc2RegID(canonical->Rs2);
		carrier->setImmediate(0);
		carrier->setUsesImmediate(false);
		carrier->setWritesRegister(true);
		carrier->InitializeMetadata();

		return std::make_shared<CheckedScalarLegacyProjection>(canonical, contract, carrier);
	}

	YAKSys_Hybrid_CPU::Core::ResourceBitset Rf06ScalarLegacyProjection::BuildRegisterResourceMask(std::shared_ptr<CanonicalDecodedInstruction> canonical)
	{
		std::shared_ptr<ArchRegId> rs1 = std::make_shared<ArchRegId>(ArchRegId::FromRawValue(canonical->Rs1));
		std::shared_ptr<ArchRegId> rs2 = std::make_shared<ArchRegId>(ArchRegId::FromRawValue(canonical->Rs2));
		std::shared_ptr<ArchRegId> rd = std::make_shared<ArchRegId>(ArchRegId::FromRawValue(canonical->Rd));

		return ResourceMaskBuilder::ForArchitecturalRegisterRead(rs1) | ResourceMaskBuilder::ForArchitecturalRegisterRead(rs2) | ResourceMaskBuilder::ForArchitecturalRegisterWrite(rd);
	}

	void Rf06ScalarLegacyProjection::ValidateCanonicalFamily(std::shared_ptr<CanonicalDecodedInstruction> canonical, std::shared_ptr<GeneratedStaticBinding> &binding)
	{
		if (!canonical) throw std::invalid_argument("canonical is null");
		if (!canonical->IsOccupied || !IsSupportedOpcode(canonical->Opcode) || canonical->InstructionClass != InstructionClass::ScalarAlu || canonical->SerializationClass != SerializationClass::Free)
		{
			throw InvalidOperationException(L"RF-06.2 scalar projection accepts only occupied, free, register-register scalar ALU slots.");
		}

	if (canonical->StaticBinding != nullptr)
	{
		binding = canonical->StaticBinding;
	}
	else
	{
		throw InvalidOperationException(L"RF-06.2 scalar projection requires the generated static binding carried by canonical decode.");
	}
// C# TO C++ CONVERTER TASK: The following .NET 'String.Equals' reference is not converted:
		if (binding->Opcode != canonical->Opcode || binding->MaterializerId.Value != MaterializerIdentity || binding->RuntimeExecutionProviderId.Value != ProviderIdentity || binding->LatencyModelId.Value != LatencyIdentity)
		{
			throw InvalidOperationException(L"RF-06.2 scalar projection received a generated binding outside the approved scalar family.");
		}

		if (!IsPresentArchitecturalRegister(canonical->Rd) || !IsPresentArchitecturalRegister(canonical->Rs1) || !IsPresentArchitecturalRegister(canonical->Rs2))
		{
			throw InvalidOperationException(std::format(L"RF-06.2 scalar projection requires present rd, rs1 and rs2 architectural registers in x0..x31; received rd={0}, rs1={1}, rs2={2}.", canonical->Rd, canonical->Rs1, canonical->Rs2));
		}
	}

	bool Rf06ScalarLegacyProjection::IsPresentArchitecturalRegister(unsigned char rawValue)
	{
		return ArchRegId::IsRepresentable(rawValue);
	}

	CheckedScalarLegacyProjection::CheckedScalarLegacyProjection(std::shared_ptr<CanonicalDecodedInstruction> canonical, std::shared_ptr<ExecutionContract> contract, std::shared_ptr<ScalarALUMicroOp> carrier) : CanonicalInstruction(std::move(canonical)), Contract(std::move(contract)), Carrier(std::move(carrier)), _fingerprint(ScalarLegacyProjectionFingerprint::Capture(Contract, Carrier))
	{

	}

	std::shared_ptr<CanonicalDecodedInstruction> CheckedScalarLegacyProjection::getCanonicalInstruction() const
	{
		return CanonicalInstruction;
	}

	std::shared_ptr<ExecutionContract> CheckedScalarLegacyProjection::getContract() const
	{
		return Contract;
	}

	std::shared_ptr<ScalarALUMicroOp> CheckedScalarLegacyProjection::getCarrier() const
	{
		return Carrier;
	}

	void CheckedScalarLegacyProjection::EnsureCurrent()
	{
		ScalarLegacyProjectionFingerprint::EnsureCurrent(_fingerprint, getContract(), getCarrier());
	}
}
