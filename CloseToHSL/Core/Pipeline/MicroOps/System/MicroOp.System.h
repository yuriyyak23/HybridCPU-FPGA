#pragma once
#include "SystemEventTypes.h"

#include "../Types/MicroOp.h"
#include "../../../Architecture/Common/YAKSys_Hybrid_CPU.Processor.h"
#include <string>
#include <vector>
#include <optional>
#include <format>
#include "tangible_exception_helper.h"
#include <memory>

using namespace YAKSys_Hybrid_CPU::Core::Memory;
using namespace YAKSys_Hybrid_CPU::Core::Registers;
using namespace YAKSys_Hybrid_CPU::Core::Registers::Retire;


namespace YAKSys_Hybrid_CPU::Core
{
	/// <summary>
	/// System-event micro-operation for serialising instructions (FENCE, ECALL, EBREAK, MRET,
	/// SRET, WFI).
	///
	/// The decoded micro-op stays immutable after decode; the live EX/MEM/WB lane owns
	/// the generated <see cref="Pipeline.PipelineEvent"/> payload that retire forwards
	/// into the FSM/control-flow seam.
	///
	/// Replaces <c>SystemNopMicroOp</c> for canonical system and typed SMT/VT event opcodes
	/// (V6 Phase 3 вЂ" A5).
	/// </summary>
	class SysEventMicroOp final : public MicroOp
	{
	private:
		SystemEventKind EventKind = static_cast<SystemEventKind>(0);
		SystemEventOrderGuarantee OrderGuarantee = static_cast<SystemEventOrderGuarantee>(0);

		static constexpr int EcallCodeRegister = 17;

		/// <summary>Architectural event kind this operation generates.</summary>
	public:
		SystemEventKind getEventKind() const;
		void setEventKind(SystemEventKind value);

		/// <summary>
		/// Pipeline drain / memory-order guarantee required by this event.
		///
		/// C17: Explicit ordering contract so that schedulers and hazard-checkers
		/// can enforce drain/flush requirements without implicit, opcode-specific logic.
		/// </summary>
		SystemEventOrderGuarantee getOrderGuarantee() const;
		void setOrderGuarantee(SystemEventOrderGuarantee value);

		SysEventMicroOp();

		CanonicalDecodePublicationMode getCanonicalDecodePublication() const override;

		bool Execute(std::shared_ptr<Processor::CPU_Core> &core) override;

		/// <summary>
		/// Initializes canonical admission facts for the concrete system-event carrier.
		/// ECALL must truthfully publish its architectural a7/x17 source dependency;
		/// all other current system events remain register-neutral singleton carriers.
		/// </summary>
		void InitializeMetadata();

		void RefreshWriteMetadata() override;

		/// <summary>
		/// Materialises the typed pipeline/FSM event for this instruction.
		/// The returned payload is carried by the pipeline lane, not by the micro-op.
		/// </summary>
		std::optional<std::shared_ptr<Pipeline::PipelineEvent>> CreatePipelineEvent(std::shared_ptr<Processor::CPU_Core> &core, unsigned long long bundleSerial = 0);

		std::wstring GetDescription() override;

		// в"Ђв"Ђв"Ђ Static factory helpers (C17: canonical order guarantees) в"Ђв"Ђв"Ђв"Ђв"Ђв"Ђв"Ђв"Ђв"Ђ

		/// <summary>
		/// Creates a <see cref="SysEventMicroOp"/> for <c>FENCE</c> with a
		/// <see cref="SystemEventOrderGuarantee.DrainMemory"/> order guarantee.
		/// </summary>
		static std::shared_ptr<SysEventMicroOp> ForFence();

		/// <summary>
		/// Creates a <see cref="SysEventMicroOp"/> for <c>FENCE.I</c> with a
		/// <see cref="SystemEventOrderGuarantee.FlushPipeline"/> order guarantee.
		/// </summary>
		static std::shared_ptr<SysEventMicroOp> ForFenceI();

		/// <summary>
		/// Creates a <see cref="SysEventMicroOp"/> for <c>ECALL</c> with a
		/// <see cref="SystemEventOrderGuarantee.FullSerialTrapBoundary"/> order guarantee.
		/// </summary>
		static std::shared_ptr<SysEventMicroOp> ForEcall();

		/// <summary>
		/// Creates a <see cref="SysEventMicroOp"/> for <c>EBREAK</c> with a
		/// <see cref="SystemEventOrderGuarantee.FullSerialTrapBoundary"/> order guarantee.
		/// </summary>
		static std::shared_ptr<SysEventMicroOp> ForEbreak();

		/// <summary>
		/// Creates a <see cref="SysEventMicroOp"/> for <c>MRET</c> with a
		/// <see cref="SystemEventOrderGuarantee.FullSerialTrapBoundary"/> order guarantee.
		/// </summary>
		static std::shared_ptr<SysEventMicroOp> ForMret();

		/// <summary>
		/// Creates a <see cref="SysEventMicroOp"/> for <c>SRET</c> with a
		/// <see cref="SystemEventOrderGuarantee.FullSerialTrapBoundary"/> order guarantee.
		/// </summary>
		static std::shared_ptr<SysEventMicroOp> ForSret();

		/// <summary>
		/// Creates a <see cref="SysEventMicroOp"/> for <c>WFI</c> with a
		/// <see cref="SystemEventOrderGuarantee.DrainMemory"/> order guarantee.
		/// </summary>
		static std::shared_ptr<SysEventMicroOp> ForWfi();

		/// <summary>
		/// Creates a <see cref="SysEventMicroOp"/> for <c>WFE</c>.
		/// Wait-state semantics are owned by the FSM when the retired event is consumed.
		/// </summary>
		static std::shared_ptr<SysEventMicroOp> ForWfe();

		/// <summary>
		/// Creates a <see cref="SysEventMicroOp"/> for <c>SEV</c>.
		/// Wake semantics are owned by the FSM when the retired event is consumed.
		/// </summary>
		static std::shared_ptr<SysEventMicroOp> ForSev();

		/// <summary>
		/// Creates a <see cref="SysEventMicroOp"/> for <c>YIELD</c>.
		/// Current runtime semantics are carried by the retired event/FSM plane
		/// without an additional serializing retire boundary.
		/// </summary>
		static std::shared_ptr<SysEventMicroOp> ForYield();

		/// <summary>
		/// Creates a <see cref="SysEventMicroOp"/> for <c>POD_BARRIER</c>.
		/// Current runtime semantics are carried by the retired event/FSM plane
		/// without an additional serializing retire boundary.
		/// </summary>
		static std::shared_ptr<SysEventMicroOp> ForPodBarrier();

		/// <summary>
		/// Creates a <see cref="SysEventMicroOp"/> for <c>VT_BARRIER</c>.
		/// Current runtime semantics are carried by the retired event/FSM plane
		/// without an additional serializing retire boundary.
		/// </summary>
		static std::shared_ptr<SysEventMicroOp> ForVtBarrier();

	private:
		static std::shared_ptr<SysEventMicroOp> Create(unsigned short opcode, SystemEventKind eventKind, SystemEventOrderGuarantee orderGuarantee);

		void ApplyCanonicalClassificationAndPlacement();

		/// <summary>
		/// Read the ECALL code from the authoritative a7 (x17) architectural source.
		/// Fails closed when the current execution contour cannot publish that value.
		/// </summary>
		static long long ReadEcallCode(std::shared_ptr<Processor::CPU_Core> &core, int ownerThreadId);

	protected:
		std::shared_ptr<SysEventMicroOp> shared_from_this()
		{
			return std::static_pointer_cast<SysEventMicroOp>(MicroOp::shared_from_this());
		}
	};

	/// <summary>
	/// Stream-control micro-operation for STREAM_SETUP, STREAM_START, and STREAM_WAIT.
	///
	/// These carriers preserve canonical decode/materializer facts for the
	/// stream-control opcodes without granting them an implicit execution surface.
	/// <c>STREAM_WAIT</c> currently follows through the serializing-boundary retire
	/// contour, while <c>STREAM_SETUP</c> and <c>STREAM_START</c> are rejected by the
	/// execution-surface contract until an explicit mainline retire/apply path lands.
	///
	/// Replaces <c>SystemNopMicroOp</c> for the three stream-engine opcodes
	/// (V6 Phase 3 вЂ" A5).
	/// </summary>
		class StreamControlMicroOp final : public MicroOp
		{
		public:
			StreamControlMicroOp();

		  CanonicalDecodePublicationMode getCanonicalDecodePublication() const override;

		void InitializeMetadata();

		bool Execute(std::shared_ptr<Processor::CPU_Core> &core) override;
		std::wstring GetDescription() override;

	protected:
		std::shared_ptr<StreamControlMicroOp> shared_from_this()
		{
			return std::static_pointer_cast<StreamControlMicroOp>(MicroOp::shared_from_this());
		}
		};
}
