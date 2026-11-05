#pragma once

#include <string>
#include <format>
#include "../../../tangible_string_helper.h"

namespace YAKSys_Hybrid_CPU::Core::Contracts
{
	/// <summary>
	/// Runtime bridge policy for compiler-emitted typed-slot facts.
	/// </summary>
	enum class CompilerTypedSlotPolicyMode : unsigned char
	{
		/// <summary>
		/// Current mainline: validate facts when possible, accept missing facts,
		/// and quarantine-log structural disagreement without rejecting execution.
		/// </summary>
		CompatibilityValidation = 0,

		/// <summary>
		/// Optional stricter verification: missing facts remain compatible, but
		/// present facts must be verifiable and structural disagreement is rejected.
		/// </summary>
		StrictVerification = 1,

		/// <summary>
		/// Reserved future seam: missing or invalid facts would become admission
		/// blockers. This is not a selectable runtime policy in the current mainline.
		/// </summary>
		RequiredForAdmission = 2
	};

	/// <summary>
	/// Last runtime action taken by the compiler bridge for typed-slot ingress.
	/// This is diagnostic state, not runtime legality authority.
	/// </summary>
	enum class CompilerTypedSlotIngressAction : unsigned char
	{
		None = 0,
		AcceptedMissingFacts,
		RecordedWithoutValidation,
		RecordedValidatedFacts,
		QuarantinedAgreementFailure,
		RejectedAgreementFailure,
		RecordedDynamicRuntimeReject
	};

	/// <summary>
	/// Explicit staged runtime policy for compiler-emitted typed-slot facts.
	/// Current default is <see cref="CompatibilityValidation"/>.
	/// </summary>
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to the C# readonly struct:
// ORIGINAL LINE: public readonly struct CompilerTypedSlotPolicy
	class CompilerTypedSlotPolicy
	{
	private:
		CompilerTypedSlotPolicyMode Mode;
		static const CompilerTypedSlotPolicy CompatibilityValidation;
		static const CompilerTypedSlotPolicy StrictVerification;
		static const CompilerTypedSlotPolicy RequiredForAdmissionFuture;

		CompilerTypedSlotPolicy(CompilerTypedSlotPolicyMode mode);

	public:
		CompilerTypedSlotPolicyMode getMode() const;

		bool getAllowsMissingFacts() const;

		bool getRequiresValidationForPresentFacts() const;

		bool getRejectsStructuralAgreementFailures() const;

		bool getRequiresRuntimeBundleForPresentFacts() const;

		bool getQuarantineLogsAgreementFailures() const;

		bool isRuntimeSelectable() const;

		static CompilerTypedSlotPolicy getCompatibilityValidation();

		static CompilerTypedSlotPolicy getStrictVerification();

		static CompilerTypedSlotPolicy getRequiredForAdmissionFuture();

		CompilerTypedSlotPolicy() = default;
	};

	/// <summary>
	/// Compiler/ISE contract version sentinel.
	/// Incremented each time the ISE pipeline contract changes in a way that
	/// requires the compiler to be re-run against updated metadata.
	/// Consumed by the compiler front-end and the ISE runtime to detect
	/// stale compiled programs at load time.
	/// </summary>
	class CompilerContract final
	{
		/// <summary>
		/// Current ISE contract version.
		/// <list type="bullet">
		///   <item>V5 Phase 1–4: deprecated fields carried forward.</item>
		///   <item>V5 Phase 5: <c>InstructionIR.SafetyMask</c> removed;
		///     <c>MicroOp.CanBeStolen</c> removed; <c>LegacyExecutionShim</c> deleted.</item>
		///   <item>V6 Phase 1: <c>MicroOp.OwnerContextId</c> added (distinct from <c>VirtualThreadId</c>);
		///     <c>ControlFlowMicroOp</c> marked <c>[Obsolete]</c> — scheduling placeholder only;
		///     <c>VLIW_Instruction.IsControlFlow</c> / <c>IsMathOrVector</c> marked <c>[Obsolete]</c>;
		///     <c>InstructionRegistry</c> system-op registrations annotated with migration comments.
		///   </item>
		///   <item>V6 Phase 3 (ISA Table-Driven Decode):
		///     <c>VLIW_Instruction.IsControlFlow</c> / <c>IsMathOrVector</c> removed — replaced by
		///     <c>OpcodeRegistry.IsControlFlowOp()</c> / <c>IsMathOrVectorOp()</c>;
		///     <c>MicroOpDescriptor.IsControlFlow</c> removed; <c>SysEventMicroOp</c> and
		///     <c>StreamControlMicroOp</c> replace <c>SystemNopMicroOp</c> for all canonical
		///     system opcodes; typed system events now flow through pipeline lane-state
		///     rather than mutable MicroOp-owned event state.
		///   </item>
		///   <item>V6 Phase 5 (Metadata Extraction – H39–H43, I44–I46):
		///     legacy <c>word3[50]</c> scheduling policy retired from correctness path and
		///     cleared on production bundle ingress; direct compat decode rejects the bit if it
		///     survives past ingress. <c>VirtualThreadId</c> (<c>word3[49:48]</c>) remains a
		///     transport hint only and does not bind execution context or correctness.
		///     All scheduling policy lives in
		///     <see cref="YAKSys_Hybrid_CPU.Core.Pipeline.Metadata.SlotMetadata"/> /
		///     <see cref="YAKSys_Hybrid_CPU.Core.Pipeline.Metadata.CompilerAnnotation"/> only;
		///     pipeline runs correctly with null or empty <c>SlotMetadata</c> (H43);
		///     no compiler-emitted field is required for correct execution of any canonical opcode (I44).
		///   </item>
		///   <item><b>V6 Phase 6 (VMX subsystem — J47–J52):</b>
		///     typed VMX compatibility retire effects introduced; <c>PipelineState.VmEntry</c> / <c>PipelineState.VmExit</c>
		///     added; VMX instructions serialised via <c>SerializationClass.VmxSerial</c>;
		///     VMCS identifiers are frozen compatibility projection vocabulary; <c>VmExitReason</c>
		///     populated on VM_EXIT.
		///   </item>
		///   <item><b>V6 Phase 7 (Final Freeze):</b>
		///     <c>ControlFlowMicroOp</c> deleted → replaced by <c>BranchMicroOp</c>;
		///     <c>SystemNopMicroOp</c> deleted → unregistered-opcode fallback uses <c>NopMicroOp</c>;
		///     all migration annotations closed; contract version bumped to 6.
		///   </item>
		/// </list>
		/// </summary>
	public:
		static constexpr int Version = 6;

		/// <summary>
		/// Current mainline typed-slot bridge policy.
		/// Missing typed-slot facts remain compatible; present facts are
		/// validation/quarantine evidence unless a stricter policy is selected.
		/// </summary>
		static CompilerTypedSlotPolicy getCurrentTypedSlotPolicy();

		/// <summary>
		/// Fail-closed compiler/runtime contract compatibility check for ingress surfaces.
		/// </summary>
		static void ThrowIfVersionMismatch(int producerVersion, const std::wstring &producerSurface);
	};
inline const CompilerTypedSlotPolicy CompilerTypedSlotPolicy::CompatibilityValidation = CompilerTypedSlotPolicy(CompilerTypedSlotPolicyMode::CompatibilityValidation);
inline const CompilerTypedSlotPolicy CompilerTypedSlotPolicy::StrictVerification = CompilerTypedSlotPolicy(CompilerTypedSlotPolicyMode::StrictVerification);
inline const CompilerTypedSlotPolicy CompilerTypedSlotPolicy::RequiredForAdmissionFuture = CompilerTypedSlotPolicy(CompilerTypedSlotPolicyMode::RequiredForAdmission);
}
