#pragma once

#include "../../../CloseToHSL/Core/Pipeline/Scheduling/SlotLegality/SlotClassDefinitions.h"
#include "../../../CloseToHSL/Core/Contracts/CompilerContract.h"
#include "../../Arch/Compat/HybridCPU_ISE.Arch.VLIW_Instruction.h"
#include <string>
#include <vector>
#include <span>
#include <limits>
#include <optional>
#include <format>
#include "tangible_exception_helper.h"
#include <memory>
#include "../../../tangible_string_helper.h"

using namespace HybridCPU_ISE::Arch;
using namespace YAKSys_Hybrid_CPU::Core;
using namespace YAKSys_Hybrid_CPU::Core::Contracts;

namespace YAKSys_Hybrid_CPU
{
	/// <summary>
	/// Runtime-side bridge for compiler-mode instruction recording and typed-slot fact acceptance.
	/// <para>
	/// Records <see cref="VLIW_Instruction"/> entries emitted by compiler-facing seams such as
	/// canonical emitters and processor-level compat recording helpers when
	/// <see cref="Processor.CurrentProcessorMode"/> is <see cref="ProcessorMode.Compiler"/>.
	/// The recorded buffer can be retrieved via <see cref="GetRecordedInstructions"/> for
	/// canonical compilation.
	/// </para>
	/// <para>
	/// Also accepts <see cref="TypedSlotBundleFacts"/> from the compiler with optional
	/// validation against the Stage 7 agreement.
	/// </para>
	/// </summary>
	class ProcessorCompilerBridge final : public std::enable_shared_from_this<ProcessorCompilerBridge>
	{
	private:
		bool ValidateCompilerFacts = true;
		TypedSlotBundleFacts LastAcceptedFacts;
		std::optional<AgreementViolationReport> LastViolation;
		CompilerTypedSlotIngressAction LastTypedSlotIngressAction = static_cast<CompilerTypedSlotIngressAction>(0);
		std::wstring ContractHandshakeProducerSurface;

		static constexpr int InitialInstructionCapacity = 64;
		std::vector<VLIW_Instruction> _instructions;
		int _instructionCount = 0;
		std::optional<int> _declaredCompilerContractVersion;
		CompilerTypedSlotPolicy _typedSlotPolicy = CompilerContract::getCurrentTypedSlotPolicy();

		/// <summary>
		/// When <see langword="true"/>, <see cref="AcceptTypedSlotFacts"/> validates
		/// incoming compiler facts via <see cref="SafetyVerifier.ValidateTypedSlotFacts"/>.
		/// Under the current compatibility policy, validation failures are recorded
		/// and quarantine-logged rather than rejected. <see cref="TypedSlotPolicy"/>
		/// selects stricter behavior when a proof surface requires it.
		/// </summary>
	public:
		bool getValidateCompilerFacts() const;
		void setValidateCompilerFacts(bool value);

		/// <summary>
		/// Last <see cref="TypedSlotBundleFacts"/> accepted from the compiler.
		/// <c>default</c> (IsEmpty == true) if no facts have been supplied.
		/// </summary>
		TypedSlotBundleFacts getLastAcceptedFacts() const;
	private:
		void setLastAcceptedFacts(TypedSlotBundleFacts value);

		/// <summary>
		/// Last <see cref="AgreementViolationReport"/> recorded, if any.
		/// Only meaningful when <see cref="ValidateCompilerFacts"/> is <see langword="true"/>
		/// and validation failed.
		/// </summary>
	public:
		std::optional<AgreementViolationReport> getLastViolation() const;
	private:
		void setLastViolation(const std::optional<AgreementViolationReport> &value);

		/// <summary>Number of instructions recorded since last reset.</summary>
	public:
		int getInstructionCount() const;

		/// <summary>
		/// Gets or sets the active typed-slot policy for this bridge.
		/// The future required-for-admission policy is intentionally not selectable
		/// in the current mainline.
		/// </summary>
		CompilerTypedSlotPolicy getTypedSlotPolicy() const;
		void setTypedSlotPolicy(CompilerTypedSlotPolicy value);

		/// <summary>
		/// Gets the last policy action taken by typed-slot fact ingress.
		/// </summary>
		CompilerTypedSlotIngressAction getLastTypedSlotIngressAction() const;
	private:
		void setLastTypedSlotIngressAction(CompilerTypedSlotIngressAction value);

		/// <summary>
		/// Gets whether this bridge has observed an explicit compiler/runtime contract handshake.
		/// </summary>
	public:
		bool getHasContractHandshake() const;

		/// <summary>
		/// Gets the compiler contract version declared by the active producer, if any.
		/// </summary>
		std::optional<int> getDeclaredCompilerContractVersion() const;

		/// <summary>
		/// Gets the producer surface that declared the current compiler contract handshake.
		/// </summary>
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public string? ContractHandshakeProducerSurface {get;private set;}
		std::wstring getContractHandshakeProducerSurface() const;
	private:
		void setContractHandshakeProducerSurface(const std::wstring &value);

		/// <summary>
		/// Declares the compiler contract version expected by the producer feeding this bridge.
		/// Canonical ingress paths must publish the handshake exactly once before
		/// any instruction recording or typed-slot fact publication occurs.
		/// </summary>
	public:
		void DeclareCompilerContractVersion(int contractVersion, const std::wstring &producerSurface = L"unspecified producer");

		/// <summary>
		/// Accept typed-slot facts emitted by the compiler for a compiled bundle.
		/// Optionally validates the facts against the bundle if <see cref="ValidateCompilerFacts"/> is enabled.
		/// </summary>
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public void AcceptTypedSlotFacts(TypedSlotBundleFacts facts, MicroOp?[]? bundle = null, int bundleWidth = 8)
		void AcceptTypedSlotFacts(TypedSlotBundleFacts facts, std::vector<std::shared_ptr<MicroOp>>* bundle = nullptr, int bundleWidth = 8);

		/// <summary>
		/// Records an agreement violation when runtime rejects an op
		/// that compiler declared structurally admissible.
		/// </summary>
		void RecordAgreementViolation(TypedSlotBundleFacts facts, TypedSlotRejectReason runtimeReject);

	private:
		void RecordTypedSlotViolation(TypedSlotBundleFacts facts, TypedSlotRejectReason runtimeReject, const std::wstring &operationName);

		/// <summary>
		/// Records a standard VLIW instruction into the bridge buffer.
		/// Compat-only bit-container recording: opcode semantics remain authoritative
		/// only in canonical IR / OpcodeRegistry layers, not in this bridge buffer.
		/// </summary>
	public:
		void RecordInstruction(VLIW_Instruction const instruction);

		/// <summary>
		/// Legacy compat wrapper that appends a raw VLIW container built from scalar fields.
		/// New production code should prefer <see cref="RecordInstruction"/> with an emitter-built
		/// <see cref="VLIW_Instruction"/> so helper layers do not duplicate bridge append logic.
		/// </summary>
		void Add_VLIW_Instruction(unsigned int opCode, unsigned char dataType, unsigned char predicate, unsigned short immediate, unsigned long long destSrc1, unsigned long long src2, unsigned long long streamLength, unsigned short stride);

		/// <summary>
		/// Returns the instructions recorded since the last reset.
		/// </summary>
		std::span<const VLIW_Instruction> GetRecordedInstructions();

		/// <summary>
		/// Clears the recorded instruction buffer.
		/// </summary>
		void ResetInstructionBuffer();

	private:
		void EnsureInstructionCapacityForAppend();

		static unsigned int RequireVliwUInt32(unsigned long long value, const std::wstring &parameterName);

		void EnsureCompilerContractHandshake(const std::wstring &operationName);
	};
}
