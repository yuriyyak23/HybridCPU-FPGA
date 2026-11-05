#include "Processor.CompilerBridge.h"
#include "../../../CloseToHSL/Core/Pipeline/Safety/YAKSys_Hybrid_CPU.Core.SafetyVerifier.h"

using namespace HybridCPU_ISE::Arch;
using namespace YAKSys_Hybrid_CPU::Core;
using namespace YAKSys_Hybrid_CPU::Core::Contracts;

namespace YAKSys_Hybrid_CPU
{

	bool ProcessorCompilerBridge::getValidateCompilerFacts() const
	{
		return ValidateCompilerFacts;
	}

	void ProcessorCompilerBridge::setValidateCompilerFacts(bool value)
	{
		ValidateCompilerFacts = value;
	}

	TypedSlotBundleFacts ProcessorCompilerBridge::getLastAcceptedFacts() const
	{
		return LastAcceptedFacts;
	}

	void ProcessorCompilerBridge::setLastAcceptedFacts(TypedSlotBundleFacts value)
	{
		LastAcceptedFacts = value;
	}

	std::optional<AgreementViolationReport> ProcessorCompilerBridge::getLastViolation() const
	{
		return LastViolation;
	}

	void ProcessorCompilerBridge::setLastViolation(const std::optional<AgreementViolationReport> &value)
	{
		LastViolation = value;
	}

	int ProcessorCompilerBridge::getInstructionCount() const
	{
		return _instructionCount;
	}

	CompilerTypedSlotPolicy ProcessorCompilerBridge::getTypedSlotPolicy() const
	{
		return _typedSlotPolicy;
	}

	void ProcessorCompilerBridge::setTypedSlotPolicy(CompilerTypedSlotPolicy value)
	{
		if (!value.isRuntimeSelectable())
		{
			throw NotSupportedException(L"Compiler typed-slot required-for-admission policy is a future seam and is not selectable in the current runtime mainline.");
		}

		_typedSlotPolicy = value;
	}

	CompilerTypedSlotIngressAction ProcessorCompilerBridge::getLastTypedSlotIngressAction() const
	{
		return LastTypedSlotIngressAction;
	}

	void ProcessorCompilerBridge::setLastTypedSlotIngressAction(CompilerTypedSlotIngressAction value)
	{
		LastTypedSlotIngressAction = value;
	}

	bool ProcessorCompilerBridge::getHasContractHandshake() const
	{
		return _declaredCompilerContractVersion.has_value();
	}

	std::optional<int> ProcessorCompilerBridge::getDeclaredCompilerContractVersion() const
	{
		return _declaredCompilerContractVersion;
	}

	std::wstring ProcessorCompilerBridge::getContractHandshakeProducerSurface() const
	{
		return ContractHandshakeProducerSurface;
	}

	void ProcessorCompilerBridge::setContractHandshakeProducerSurface(const std::wstring &value)
	{
		ContractHandshakeProducerSurface = value;
	}

	void ProcessorCompilerBridge::DeclareCompilerContractVersion(int contractVersion, const std::wstring &producerSurface)
	{
		std::wstring resolvedSurface = StringHelper::isEmptyOrWhiteSpace(producerSurface) ? L"unspecified producer" : producerSurface;

		CompilerContract::ThrowIfVersionMismatch(contractVersion, producerSurface);

		if (_declaredCompilerContractVersion)
		{
	// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
	// ORIGINAL LINE: string existingSurface = string.IsNullOrWhiteSpace(ContractHandshakeProducerSurface) ? "unspecified producer" : ContractHandshakeProducerSurface!;
			std::wstring existingSurface = StringHelper::isEmptyOrWhiteSpace(getContractHandshakeProducerSurface()) ? L"unspecified producer" : getContractHandshakeProducerSurface();
			throw InvalidOperationException(std::format(L"ProcessorCompilerBridge already has compiler contract handshake version {} from {}. Duplicate declaration from {} is not allowed.", _declaredCompilerContractVersion.value(), existingSurface, resolvedSurface));
		}

		_declaredCompilerContractVersion = contractVersion;
		setContractHandshakeProducerSurface(resolvedSurface);
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public void AcceptTypedSlotFacts(TypedSlotBundleFacts facts, MicroOp?[]? bundle = null, int bundleWidth = 8)
	void ProcessorCompilerBridge::AcceptTypedSlotFacts(TypedSlotBundleFacts facts, std::vector<std::shared_ptr<MicroOp>>* bundle, int bundleWidth)
	{
		EnsureCompilerContractHandshake(L"AcceptTypedSlotFacts");
		setLastAcceptedFacts(facts);
		setLastViolation(std::nullopt);
		setLastTypedSlotIngressAction(CompilerTypedSlotIngressAction::None);

		if (facts.isEmpty())
		{
			setLastTypedSlotIngressAction(CompilerTypedSlotIngressAction::AcceptedMissingFacts);
			return;
		}

		bool mustValidate = getTypedSlotPolicy().getRequiresValidationForPresentFacts() || getValidateCompilerFacts();
		if (!mustValidate)
		{
			setLastTypedSlotIngressAction(CompilerTypedSlotIngressAction::RecordedWithoutValidation);
			return;
		}

		if (bundle == nullptr)
		{
			setLastTypedSlotIngressAction(CompilerTypedSlotIngressAction::RecordedWithoutValidation);
			if (getTypedSlotPolicy().getRequiresRuntimeBundleForPresentFacts())
			{
				setLastTypedSlotIngressAction(CompilerTypedSlotIngressAction::RejectedAgreementFailure);
				throw InvalidOperationException(L"Strict typed-slot verification requires a runtime bundle to verify present compiler facts.");
			}

			return;
		}

		bool valid = SafetyVerifier::ValidateTypedSlotFacts(facts, *bundle, bundleWidth);
		if (valid)
		{
			setLastTypedSlotIngressAction(CompilerTypedSlotIngressAction::RecordedValidatedFacts);
			return;
		}

		RecordTypedSlotViolation(facts, TypedSlotRejectReason::StaticClassOvercommit, L"AcceptTypedSlotFacts");
	}

	void ProcessorCompilerBridge::RecordAgreementViolation(TypedSlotBundleFacts facts, TypedSlotRejectReason runtimeReject)
	{
		EnsureCompilerContractHandshake(L"RecordAgreementViolation");
		RecordTypedSlotViolation(facts, runtimeReject, L"RecordAgreementViolation");
	}

	void ProcessorCompilerBridge::RecordTypedSlotViolation(TypedSlotBundleFacts facts, TypedSlotRejectReason runtimeReject, const std::wstring &operationName)
	{
		bool isStructuralMismatch = AgreementViolationReport::IsStructuralReject(runtimeReject);
		AgreementViolationReport report;
		report.setCompilerFacts(facts);
		report.setRuntimeReject(runtimeReject);
		report.setStructuralMismatch(isStructuralMismatch);
		setLastViolation(report);

		if (isStructuralMismatch)
		{
			setLastTypedSlotIngressAction(getTypedSlotPolicy().getRejectsStructuralAgreementFailures() ? CompilerTypedSlotIngressAction::RejectedAgreementFailure : CompilerTypedSlotIngressAction::QuarantinedAgreementFailure);

			if (getTypedSlotPolicy().getRejectsStructuralAgreementFailures())
			{
				throw InvalidOperationException(std::format(L"Strict typed-slot verification rejected structural agreement failure during {}: {}.", operationName, static_cast<int>(runtimeReject)));
			}

			return;
		}

		setLastTypedSlotIngressAction(CompilerTypedSlotIngressAction::RecordedDynamicRuntimeReject);
	}

	void ProcessorCompilerBridge::RecordInstruction(VLIW_Instruction const instruction)
	{
		EnsureCompilerContractHandshake(L"RecordInstruction");
		EnsureInstructionCapacityForAppend();

		_instructions[_instructionCount] = instruction;
		_instructionCount++;
	}

	void ProcessorCompilerBridge::Add_VLIW_Instruction(unsigned int opCode, unsigned char dataType, unsigned char predicate, unsigned short immediate, unsigned long long destSrc1, unsigned long long src2, unsigned long long streamLength, unsigned short stride)
	{
		unsigned int encodedStreamLength = RequireVliwUInt32(streamLength, L"streamLength");

		VLIW_Instruction temp_var = VLIW_Instruction();
		temp_var.setOpCode(opCode);
		temp_var.setDataType(dataType);
		temp_var.setPredicateMask(predicate);
		temp_var.setImmediate(immediate);
		temp_var.setDestSrc1Pointer(destSrc1);
		temp_var.setSrc2Pointer(src2);
		temp_var.setStreamLength(encodedStreamLength);
		temp_var.setStride(stride);
		RecordInstruction(temp_var);
	}

	std::span<const VLIW_Instruction> ProcessorCompilerBridge::GetRecordedInstructions()
	{
		return std::span<const VLIW_Instruction>(_instructions.data(), static_cast<size_t>(_instructionCount));
	}

	void ProcessorCompilerBridge::ResetInstructionBuffer()
	{
		_instructionCount = 0;
	}

	void ProcessorCompilerBridge::EnsureInstructionCapacityForAppend()
	{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'checked' in this context:
// ORIGINAL LINE: int requiredCapacity = checked(_instructionCount + 1);
		int requiredCapacity = _instructionCount + 1;
		if (requiredCapacity <= _instructions.size())
		{
			return;
		}

		// The bridge is a compat-only recording surface, but it must not silently
		// impose a legacy hard stop on canonical compilation throughput.
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'checked' in this context:
// ORIGINAL LINE: int nextCapacity = _instructions.Length == 0 ? InitialInstructionCapacity : checked(_instructions.Length * 2);
		int nextCapacity = _instructions.empty() ? InitialInstructionCapacity : _instructions.size() * 2;
		if (nextCapacity < requiredCapacity)
		{
			nextCapacity = requiredCapacity;
		}

		_instructions.resize(nextCapacity);
	}

	unsigned int ProcessorCompilerBridge::RequireVliwUInt32(unsigned long long value, const std::wstring &parameterName)
	{
		if (value > std::numeric_limits<unsigned int>::max())
		{
			throw ArgumentOutOfRangeException(std::format(L"{}: {} exceeds the 32-bit VLIW carrier field; silent truncation is forbidden.", parameterName, value));
		}

		return static_cast<unsigned int>(value);
	}

	void ProcessorCompilerBridge::EnsureCompilerContractHandshake(const std::wstring &operationName)
	{
		if (_declaredCompilerContractVersion)
		{
			return;
		}

		throw InvalidOperationException(std::format(L"ProcessorCompilerBridge requires an explicit compiler contract handshake before {}. Call DeclareCompilerContractVersion(CompilerContract.Version, ...) from the compiler/runtime ingress path.", operationName));
	}
}
