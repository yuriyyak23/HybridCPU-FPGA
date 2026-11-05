#pragma once

#include "../../Safety/ResourceBitset.h"
#include "../../Certificates/SafetyMask128.h"

#include "../../../../../NonRTL/Arch/InstructionClass.h"
#include "../../../../../NonRTL/Arch/SerializationClass.h"
#include "../../Metadata/LocalityHint.h"
#include "../../Certificates/ArchLegalityMask.h"
#include "../../Scheduling/SlotPlacementMetadata.h"
#include "../../../Architecture/Registers/Architectural/RegisterIdentity.h"
#include <string>
#include <vector>
#include <format>
#include <memory>
#include <span>
#include <stdexcept>
#include "../../../Architecture/Registers/Retire/RetireRecord.h"
#include "MicroOpMemoryRange.h"

namespace HybridCPU_ISE::Arch {} 
namespace YAKSys_Hybrid_CPU::Core::Memory {} 
namespace YAKSys_Hybrid_CPU::Core::Registers { class ArchRegId; namespace Retire {} }
#include "../../../Architecture/Common/YAKSys_Hybrid_CPU.Processor.h"
namespace YAKSys_Hybrid_CPU::Core { class StreamEngineId; struct MicroOpAdmissionMetadata; }
using namespace HybridCPU_ISE::Arch;

using namespace YAKSys_Hybrid_CPU::Core::Memory;
using namespace YAKSys_Hybrid_CPU::Core::Registers;
using namespace YAKSys_Hybrid_CPU::Core::Registers::Retire;

namespace YAKSys_Hybrid_CPU::Core
{
    class PostStageBIdentityTemplate;
    class PostStageBIssuedAttempt;
	/// <summary>
	/// MicroOp classification for resource quota tracking
	/// </summary>
	enum class MicroOpClass
	{
		/// <summary>
		/// ALU operation (arithmetic, logical, shift, etc.)
		/// </summary>
		Alu,

		/// <summary>
		/// LSU operation (load, store, atomic)
		/// </summary>
		Lsu,

		/// <summary>
		/// DMA operation (block transfer)
		/// </summary>
		Dma,

		/// <summary>
		/// MatrixTile memory transport. ISA memory semantics remain separate from LSU ownership.
		/// </summary>
		MatrixTileMemory,

		/// <summary>
		/// MatrixTile compute operation.
		/// </summary>
		MatrixTileCompute,

		/// <summary>
		/// Vector/Stream operation
		/// </summary>
		Vector,

		/// <summary>
		/// Control flow operation
		/// </summary>
		Control,

		/// <summary>
		/// Other/unclassified operation
		/// </summary>
		Other
	};

	/// <summary>
	/// Declares which layer is authoritative for canonical decode-fact publication.
	/// <see cref="Unspecified"/> is fail-closed: decode/projector paths must reject
	/// materialized MicroOps that never made an explicit publication decision.
	/// </summary>
	enum class CanonicalDecodePublicationMode
	{
		/// <summary>
		/// No explicit canonical decode-publication policy has been declared.
		/// This is illegal on decoder/projector materialization paths.
		/// </summary>
		Unspecified = 0,

		/// <summary>
		/// Canonical decode facts must be projected by the decode-to-runtime projector.
		/// The materialized MicroOp may still publish execution-specific metadata.
		/// </summary>
		ProjectorPublishes = 1,

		/// <summary>
		/// The MicroOp family/factory already publishes authoritative canonical decode facts.
		/// The projector must not overlay a second manual compatibility projection.
		/// </summary>
		SelfPublishes = 2
	};

	/// <summary>
	/// Helper class for building Global Resource Lock Bitset (GRLB) masks - Phase 8.
	/// Provides static methods to construct resource masks for different operation types.
	/// Now supports both 64-bit (legacy) and 128-bit (extended) safety masks.
	/// </summary>
	class ResourceMaskBuilder final
	{
		// Resource bit layout constants (Low 64 bits)
	private:
		static constexpr int REG_READ_BASE = 0; // Bits 0-15: Register read groups
		static constexpr int REG_WRITE_BASE = 16; // Bits 16-31: Register write groups
		static constexpr int MEM_DOMAIN_BASE = 32; // Bits 32-47: Memory domain IDs
		static constexpr int LSU_LOAD_BIT = 48; // Bit 48: Load operation
		static constexpr int LSU_STORE_BIT = 49; // Bit 49: Store operation
		static constexpr int LSU_ATOMIC_BIT = 50; // Bit 50: Atomic operation
		static constexpr int DMA_CHANNEL_BASE = 51; // Bits 51-54: DMA channels (4 channels)
		static constexpr int STREAM_ENGINE_BASE = 55; // Bits 55-58: Stream engines (4 engines)
		static constexpr int ACCEL_BASE = 59; // Bits 59-62: Custom accelerators (4 accelerators)

		// Extended resource bit layout constants (High 64 bits, bits 64-127)
		static constexpr int EXT_GRLB_BASE = 0; // High bits 0-31: Extended GRLB channels
		static constexpr int MATRIX_TILE_BASE = 16; // High bits 16-23: MatrixTile structural channels
		static constexpr int EXT_MEM_DOMAIN_BASE = 32; // High bits 32-47: Extended memory domains
		static constexpr int EXT_MEM_BANK_BASE = 48; // High bits 48-63: Memory Banks occupancy (Phase Refactoring Pt. 1)

		static int RequireResourceId(int resourceId, int resourceCount, const std::wstring &parameterName, const std::wstring &resourceName);

		/// <summary>
		/// Build resource mask for memory bank occupancy.
		/// </summary>
	public:
		static ResourceBitset ForMemoryBank(int bankId);

		/// <summary>
		/// Build resource mask for register read operations with virtual-thread isolation.
		///
		/// Group layout (per-VT bank): <c>group = vtId * 4 + regId / 8</c>.
		/// With 4 VTs (0–3) and 4 groups per VT this maps onto the same 16-group
		/// window as the non-VT overload but keeps each thread's register traffic
		/// in a distinct bank slot — preventing false resource conflicts across VTs.
		///
		/// Blueprint §1 / §15: "Ввести банки: ForRegisterRead(regId, vtId)".
		/// </summary>
		static ResourceBitset ForRegisterRead(int regId, int vtId);

		/// <summary>
		/// Build resource mask for register write operations with virtual-thread isolation.
		///
		/// Group layout (per-VT bank): <c>group = vtId * 4 + regId / 8</c>.
		/// Mirrors <see cref="ForRegisterRead(int, int)"/> for the write path.
		///
		/// Blueprint §1 / §15: "Ввести банки: ForRegisterWrite(regId, vtId)".
		/// </summary>
		static ResourceBitset ForRegisterWrite(int regId, int vtId);

		/// <summary>
		/// Build the existing per-VT register-read mask from a checked
		/// architectural-register identity while retaining the raw VT boundary.
		/// </summary>
		static ResourceBitset ForArchitecturalRegisterRead(std::shared_ptr<ArchRegId> regId, int vtId);

		/// <summary>
		/// Build the existing per-VT register-write mask from a checked
		/// architectural-register identity while retaining the raw VT boundary.
		/// </summary>
		static ResourceBitset ForArchitecturalRegisterWrite(std::shared_ptr<ArchRegId> regId, int vtId);


		/// <summary>
		/// Build resource mask for register read operations.
		/// Maps register ID to group ID (4 registers per group).
		/// Returns a ResourceBitset with the appropriate bit set in the Low component.
		/// </summary>
		static ResourceBitset ForRegisterRead(int regId);

		/// <summary>
		/// Build resource mask for register write operations.
		/// Maps register ID to group ID (4 registers per group).
		/// Returns a ResourceBitset with the appropriate bit set in the Low component.
		/// </summary>
		static ResourceBitset ForRegisterWrite(int regId);

		/// <summary>
		/// Build the existing register-read mask from a checked architectural
		/// register identity.
		/// </summary>
		static ResourceBitset ForArchitecturalRegisterRead(std::shared_ptr<ArchRegId> regId);

		/// <summary>
		/// Build the existing register-write mask from a checked architectural
		/// register identity.
		/// </summary>
		static ResourceBitset ForArchitecturalRegisterWrite(std::shared_ptr<ArchRegId> regId);

		/// <summary>
		/// Build resource mask for memory domain access.
		/// </summary>
		static ResourceBitset ForMemoryDomain(int domainId);

		/// <summary>
		/// Build resource mask for load operation (LSU read channel).
		/// </summary>
		static ResourceBitset ForLoad();

		/// <summary>
		/// Build resource mask for store operation (LSU write channel).
		/// </summary>
		static ResourceBitset ForStore();

		/// <summary>
		/// Build resource mask for atomic operation (LSU atomic channel).
		/// </summary>
		static ResourceBitset ForAtomic();

		/// <summary>
		/// Build resource mask for DMA channel.
		/// </summary>
		static ResourceBitset ForDMAChannel(int channelId);

		/// <summary>
		/// Build resource mask for stream engine.
		/// </summary>
		static ResourceBitset ForStreamEngine(int engineId);

		/// <summary>
		/// Builds the existing stream-engine resource mask from a checked
		/// selector. Representation does not grant scheduling authority.
		/// </summary>
		static ResourceBitset ForStreamEngine(std::shared_ptr<StreamEngineId> engineId);

		static ResourceBitset ForMatrixTileStreamWindow();

		static ResourceBitset ForMatrixTileIngress();

		static ResourceBitset ForMatrixTileEgress();

		static ResourceBitset ForMatrixTileStateRead();

		static ResourceBitset ForMatrixTileStateWrite();

		static ResourceBitset ForMatrixTileAccumulatorRead();

		static ResourceBitset ForMatrixTileAccumulatorWrite();

		static ResourceBitset ForMatrixTileTransposePolicy();

		/// <summary>
		/// Build resource mask for custom accelerator.
		/// </summary>
		static ResourceBitset ForAccelerator(int accelId);

		// ===== 128-bit Safety Mask Builders (Phase: Safety Tags & Certificates) =====

		/// <summary>
		/// Build 128-bit safety mask for register read operations.
		/// </summary>
		static SafetyMask128 ForRegisterRead128(int regId);

		/// <summary>
		/// Build 128-bit safety mask for register write operations.
		/// </summary>
		static SafetyMask128 ForRegisterWrite128(int regId);

		/// <summary>
		/// Build the existing 128-bit register-read safety mask from a checked
		/// architectural-register identity.
		/// </summary>
		static SafetyMask128 ForArchitecturalRegisterRead128(std::shared_ptr<ArchRegId> regId);

		/// <summary>
		/// Build the existing 128-bit register-write safety mask from a checked
		/// architectural-register identity.
		/// </summary>
		static SafetyMask128 ForArchitecturalRegisterWrite128(std::shared_ptr<ArchRegId> regId);

		/// <summary>
		/// Build 128-bit safety mask for memory domain access.
		/// </summary>
		static SafetyMask128 ForMemoryDomain128(int domainId);

		/// <summary>
		/// Build 128-bit safety mask for memory bank occupancy (Refactoring: Hardware MSHR/Banks).
		/// </summary>
		static SafetyMask128 ForMemoryBank128(int bankId);

		/// <summary>
		/// Build 128-bit safety mask for load operation.
		/// </summary>
		static SafetyMask128 ForLoad128();

		/// <summary>
		/// Build 128-bit safety mask for store operation.
		/// </summary>
		static SafetyMask128 ForStore128();

		/// <summary>
		/// Build 128-bit safety mask for atomic operation.
		/// </summary>
		static SafetyMask128 ForAtomic128();

		/// <summary>
		/// Build 128-bit safety mask for DMA channel.
		/// </summary>
		static SafetyMask128 ForDMAChannel128(int channelId);

		/// <summary>
		/// Build 128-bit safety mask for stream engine.
		/// </summary>
		static SafetyMask128 ForStreamEngine128(int engineId);

		/// <summary>
		/// Builds the existing 128-bit stream-engine safety mask from a
		/// checked selector. Representation does not grant scheduling authority.
		/// </summary>
		static SafetyMask128 ForStreamEngine128(std::shared_ptr<StreamEngineId> engineId);

		/// <summary>
		/// Build 128-bit safety mask for custom accelerator.
		/// </summary>
		static SafetyMask128 ForAccelerator128(int accelId);

		/// <summary>
		/// Build 128-bit safety mask for extended GRLB channel (bits 64-95).
		/// Supports up to 32 additional channels for future hardware expansions.
		/// </summary>
		static SafetyMask128 ForExtendedGRLBChannel(int channelId);

		/// <summary>
		/// Build 128-bit safety mask for extended memory domain (bits 96-111).
		/// Supports 16 additional memory domains beyond the base 16.
		/// </summary>
		static SafetyMask128 ForExtendedMemoryDomain(int domainId);
	};

	/// </summary>
	class MicroOp : public std::enable_shared_from_this<MicroOp>
	{
	private:
		std::shared_ptr<YAKSys_Hybrid_CPU::Core::PostStageBIdentityTemplate> PostStageBIdentityTemplate;
		std::shared_ptr<YAKSys_Hybrid_CPU::Core::PostStageBIssuedAttempt> PostStageBIssuedAttempt;
		bool IsSoftwareSecureSource = false;
		unsigned int OpCode = 0;
		unsigned short PredicateMask = 0;
		unsigned short DestRegID = 0;
		bool WritesRegister = false;
		unsigned char Latency = 0;
		bool IsMemoryOp = false;
		bool IsControlFlow = false;
		Arch::InstructionClass InstructionClass = Arch::InstructionClass::ScalarAlu;
		Arch::SerializationClass SerializationClass = Arch::SerializationClass::Free;
		LocalityHint MemoryLocalityHint = LocalityHint::None;
		bool IsFspInjected = false;
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private IReadOnlyList<int> ReadRegisters = Array.Empty<int>();
		std::vector<int> ReadRegisters{};
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private IReadOnlyList<int> WriteRegisters = Array.Empty<int>();
		std::vector<int> WriteRegisters{};
		int OwnerThreadId = 0;
		int OwnerContextId = 0;
		int VirtualThreadId = 0;
		bool HasSideEffects = false;
		SafetyMask128 SafetyMask = SafetyMask128(0, 0);
		ArchLegalityMask ArchLegality = ArchLegalityMask::Zero;
		BackendHazardMask BackendHazard = BackendHazardMask::Zero;
		int QueueIndex = 0;
		MicroOpClass Class = MicroOpClass::Other;
		SlotPlacementMetadata Placement = SlotPlacementMetadata::getDefault();
		ResourceBitset ResourceMask = ResourceBitset::Zero;
		unsigned long long ResourceToken = 0;
		ResourceBitset OriginalResourceMask = ResourceBitset::Zero;
		bool ScoreboardPending = false;
		bool IsSpeculative = false;
		bool Faulted = false;

		std::shared_ptr<MicroOpAdmissionMetadata> _admissionMetadata;
		bool _hasAdmissionMetadata = false;
		bool _isStealable = true;

		// RF-08.3d: optional exact issued-attempt sideband. It is assigned only
		// by the successful existing Stage-B commit arm and follows this same
		// live MicroOp reference through FSP-derived packet/latch transport.
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal PostStageBIdentityTemplate? PostStageBIdentityTemplate {get;set;}
	public:
		std::shared_ptr<YAKSys_Hybrid_CPU::Core::PostStageBIdentityTemplate> getPostStageBIdentityTemplate() const;
		void setPostStageBIdentityTemplate(std::shared_ptr<YAKSys_Hybrid_CPU::Core::PostStageBIdentityTemplate> value);
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal PostStageBIssuedAttempt? PostStageBIssuedAttempt {get;set;}
		std::shared_ptr<YAKSys_Hybrid_CPU::Core::PostStageBIssuedAttempt> getPostStageBIssuedAttempt() const;
		void setPostStageBIssuedAttempt(std::shared_ptr<YAKSys_Hybrid_CPU::Core::PostStageBIssuedAttempt> value);
		// Deny-only marker: this source must never fall through ordinary
		// dispatch while its explicit secure owner performs admission/retire.
		bool isSoftwareSecureSource() const;
	private:
		void setSoftwareSecureSource(bool value);
	public:
		void BindSoftwareSecureSource();

	protected:
		static void RefreshAdmissionMetadata(std::shared_ptr<MicroOp> microOp);

		/// <summary>
		/// Opcode of the original instruction
		/// </summary>
	public:
		unsigned int getOpCode() const;
		void setOpCode(unsigned int value);

		/// <summary>
		/// Predicate mask for conditional execution (0 = all lanes active)
		/// </summary>
		unsigned short getPredicateMask() const;
		void setPredicateMask(unsigned short value);

		/// <summary>
		/// Destination register ID (if applicable)
		/// </summary>
		unsigned short getDestRegID() const;
		void setDestRegID(unsigned short value);

		/// <summary>
		/// Does this MicroOp write to a register?
		/// </summary>
		bool getWritesRegister() const;
		void setWritesRegister(bool value);

		/// <summary>
		/// Execution latency in cycles (minimum, may vary by operands)
		/// </summary>
		unsigned char getLatency() const;
		void setLatency(unsigned char value);

		/// <summary>
		/// Is this a memory operation?
		/// </summary>
		bool isMemoryOp() const;
		void setMemoryOp(bool value);

		/// <summary>
		/// Is this a control flow operation?
		/// </summary>
		bool isControlFlow() const;
		void setControlFlow(bool value);

		// ===== ISA v4 Classification (Phase 02) =====

		/// <summary>
		/// Canonical ISA v4 instruction class for this micro-operation.
		/// Determines pipeline routing, slot class assignment, and serialization behavior.
		/// Populated from <see cref="Arch.InstructionClassifier"/> during construction.
		/// </summary>
		virtual Arch::InstructionClass getInstructionClass() const;
	protected:
		virtual void setInstructionClass(Arch::InstructionClass value);

		/// <summary>
		/// Canonical ISA v4 serialization class for this micro-operation.
		/// Determines ordering and side-effect isolation requirements in the pipeline.
		/// Populated from <see cref="Arch.InstructionClassifier"/> during construction.
		/// </summary>
	public:
		virtual Arch::SerializationClass getSerializationClass() const;
	protected:
		virtual void setSerializationClass(Arch::SerializationClass value);

		/// <summary>
		/// Declares whether canonical decode facts are published by the materialized
		/// MicroOp itself or must be projected by the decode-to-runtime transport layer.
		/// New MicroOp families must override this explicitly; <see cref="Unspecified"/>
		/// is a fail-closed contract rather than a permissive default.
		/// </summary>
	public:
		virtual CanonicalDecodePublicationMode getCanonicalDecodePublication() const;

		/// <summary>
		/// Explicit compatibility/test opt-in for the retired structural fallback surface.
		/// Canonical runtime micro-ops must publish authoritative structural safety masks
		/// and leave this disabled.
		/// </summary>
	protected:
		virtual bool getAllowsStructuralSafetyFallback() const;

		// ===== Legacy FSP Compatibility Metadata =====
		// These properties retain compatibility with older slot-admission terminology
		// while the canonical runtime story speaks in typed-slot bundle densification,
		// donor provenance, and assist legality.

		/// <summary>
		/// Explicit producer-side stealability fact for this materialized micro-op.
		/// Runtime admission and legacy FSP-compatibility candidate checks must
		/// consume the cached <see cref="AdmissionMetadata"/> snapshot instead of
		/// recovering stealability from legacy per-slot shells.
		/// </summary>
	public:
		bool isStealable() const;
		void setStealable(bool value);

		/// <summary>
		/// Explicit producer-side locality hint carried on the materialized micro-op.
		/// Runtime assist-seed shaping must consume this field directly instead of
		/// recovering locality policy from detached slot metadata containers.
		/// </summary>
		LocalityHint getMemoryLocalityHint() const;
		void setMemoryLocalityHint(LocalityHint value);

		/// <summary>
		/// Snapshot of correctness-relevant admission facts extracted from the producer-side
		/// micro-op state. This is the authoritative legality input for decoded-slot and
		/// certificate consumers; <see cref="SafetyMask"/> remains only a structural primitive.
		/// </summary>
		std::shared_ptr<MicroOpAdmissionMetadata> getAdmissionMetadata() const;
		bool allowsStructuralSafetyFallback() const { return getAllowsStructuralSafetyFallback(); }

		// Validate that the published destination flag and canonical write set agree.
		void ValidatePublishedWriteRegisterContract(const std::wstring &producerSurface) const;

		/// <summary>
		/// Re-synchronises the cached producer-side admission snapshot after the micro-op's
		/// scheduling or hazard-relevant fields are updated.
		/// </summary>
		void RefreshAdmissionMetadata();

		/// <summary>
		/// Publishes the explicit structural safety mask for the micro-op's current
		/// producer-side resource, placement, and serialization shape.
		/// Canonical runtime producers must call this before refreshing admission
		/// metadata on high-risk contours.
		/// </summary>
	protected:
		void PublishExplicitStructuralSafetyMask();

		/// <summary>
		/// Retained compatibility flag for a micro-operation injected by typed-slot
		/// bundle densification / slot admission rather than by its owning VT in
		/// normal program order.
		/// Used at ID stage to distinguish silent speculative squash from
		/// architectural domain fault (precise exception). See §5.4.
		/// </summary>
	public:
		bool isFspInjected() const;
		void setFspInjected(bool value);

		/// <summary>
		/// True only for the post-phase-08 assist plane.
		/// Assist micro-ops are architecturally invisible, retire-invisible,
		/// replay-discardable, and non-retiring.
		/// They remain boundedly observable through carrier-memory effects plus
		/// replay and telemetry evidence.
		/// </summary>
		virtual bool isAssist() const;

		/// <summary>
		/// True when this micro-op participates in architectural retire truth.
		/// Assist micro-ops override this to keep retire authority explicit.
		/// </summary>
		virtual bool isRetireVisible() const;

		/// <summary>
		/// True when this micro-op may be discarded across replay / trap / fence / VM boundaries
		/// without changing ISA-visible state.
		/// </summary>
		virtual bool isReplayDiscardable() const;

		/// <summary>
		/// True when runtime faults must be suppressed and converted into a deterministic kill.
		/// Used by the assist plane to prevent ISA-visible traps or results.
		/// </summary>
		virtual bool getSuppressesArchitecturalFaults() const;

		/// <summary>
		/// Register IDs from which this micro-operation reads.
		/// Used by scheduler to detect RAW/WAW hazards.
		/// </summary>
		virtual const std::vector<int> getReadRegisters() const;
	protected:
		virtual void setReadRegisters(const std::vector<int> &value);

		/// <summary>
		/// Register IDs to which this micro-operation writes.
		/// Used by scheduler to detect WAW/WAR hazards.
		/// </summary>
	public:
		virtual const std::vector<int> getWriteRegisters() const;
	protected:
		virtual void setWriteRegisters(const std::vector<int> &value);

		/// <summary>
		/// Memory address ranges from which this operation reads.
		/// Format: (Address, Length) tuples after IOMMU translation.
		/// Populated during construction or in Execute stage for stream operations.
		/// </summary>
	public:
		using MemoryRange = MicroOpMemoryRange;
    private:
        std::vector<MemoryRange> ReadMemoryRanges{};
        std::vector<MemoryRange> WriteMemoryRanges{};
    public:
        virtual const std::vector<MemoryRange>& getReadMemoryRanges() const { return ReadMemoryRanges; }
        virtual const std::vector<MemoryRange>& getWriteMemoryRanges() const { return WriteMemoryRanges; }
    protected:
        virtual void setReadMemoryRanges(const std::vector<MemoryRange>& value) { ReadMemoryRanges = value; }
        virtual void setWriteMemoryRanges(const std::vector<MemoryRange>& value) { WriteMemoryRanges = value; }
    public:
        int getOwnerThreadId() const { return OwnerThreadId; }
        bool TryGetArchitecturalOwnerVtId(VtId& ownerVtId) const { return VtId::TryCreate(OwnerThreadId, ownerVtId); }
        void setOwnerThreadId(int value) { OwnerThreadId = value; }
        int getOwnerContextId() const { return OwnerContextId; }
        void setOwnerContextId(int value) { OwnerContextId = value; }
        int getVirtualThreadId() const { return VirtualThreadId; }
        void setVirtualThreadId(int value) { VirtualThreadId = value; }
        virtual bool getHasSideEffects() const { return HasSideEffects; }
    protected:
        static int NormalizeExecutionVtId(int ownerThreadId);
        static bool TryReadUnifiedArchValue(const std::shared_ptr<Processor::CPU_Core>& core, int vtId, int archReg, unsigned long long& value);
        static unsigned long long ReadUnifiedScalarSourceOperand(const std::shared_ptr<Processor::CPU_Core>& core, int vtId, unsigned short rawRegId);
        virtual void setHasSideEffects(bool value) { HasSideEffects = value; }
    public:
        virtual SafetyMask128 getSafetyMask() const { return SafetyMask; }
        virtual void setSafetyMask(SafetyMask128 value) { SafetyMask = value; }
        ArchLegalityMask getArchLegality() const { return ArchLegality; }
        void setArchLegality(ArchLegalityMask value) { ArchLegality = value; }
        BackendHazardMask getBackendHazard() const { return BackendHazard; }
        void setBackendHazard(BackendHazardMask value) { BackendHazard = value; }
        ResourceBitset getResourceMask() const { return ResourceMask; }
        void setResourceMask(ResourceBitset value) { ResourceMask = value; }
        ResourceBitset getOriginalResourceMask() const { return OriginalResourceMask; }
        void setOriginalResourceMask(ResourceBitset value) { OriginalResourceMask = value; }
        int getQueueIndex() const { return QueueIndex; }
        void setQueueIndex(int value) { QueueIndex = value; }
        virtual MicroOpClass getClass() const { return Class; }
    protected:
        virtual void setClass(MicroOpClass value) { Class = value; }
    public:
        SlotPlacementMetadata getPlacement() const { return Placement; }
        void setPlacement(SlotPlacementMetadata value) { Placement = value; }
        unsigned long long getResourceToken() const { return ResourceToken; }
        void setResourceToken(unsigned long long value) { ResourceToken = value; }
        bool getScoreboardPending() const { return ScoreboardPending; }
        void setScoreboardPending(bool value) { ScoreboardPending = value; }
        bool isSpeculative() const { return IsSpeculative; }
        void setSpeculative(bool value) { IsSpeculative = value; }
        bool getFaulted() const { return Faulted; }
        void setFaulted(bool value) { Faulted = value; }

        void ApplyCanonicalDecodeProjection(Arch::InstructionClass instructionClass,
            Arch::SerializationClass serializationClass, SlotPlacementMetadata placement,
            bool isMemoryOp, bool isControlFlow, bool writesRegister,
            const std::vector<int>& readRegisters, const std::vector<int>& writeRegisters)
        {
            InstructionClass = instructionClass;
            SerializationClass = serializationClass;
            Placement = placement;
            IsMemoryOp = isMemoryOp;
            IsControlFlow = isControlFlow;
            WritesRegister = writesRegister;
            ReadRegisters = readRegisters;
            WriteRegisters = writeRegisters;
            _hasAdmissionMetadata = false;
        }
    protected:
        void SetPlacement(SlotClass requiredSlotClass, SlotPinningKind pinningKind,
            unsigned char pinnedLaneId = 0)
        {
            Placement.RequiredSlotClass = requiredSlotClass;
            Placement.PinningKind = pinningKind;
            Placement.PinnedLaneId = pinningKind == SlotPinningKind::HardPinned ? pinnedLaneId : 0;
        }
        void SetClassFlexiblePlacement(SlotClass requiredSlotClass)
        {
            SetPlacement(requiredSlotClass, SlotPinningKind::ClassFlexible);
        }
    public:
        virtual std::shared_ptr<HybridCPU_ISE::Core::ReplayToken> CreateRollbackToken(int ownerThreadId, std::shared_ptr<Processor::MainMemoryArea> mainMemory = nullptr);
        virtual bool Execute(std::shared_ptr<YAKSys_Hybrid_CPU::Processor::CPU_Core>& core) = 0;
        virtual void Commit(std::shared_ptr<YAKSys_Hybrid_CPU::Processor::CPU_Core>& core) {}
        virtual void EmitWriteBackRetireRecords(std::shared_ptr<YAKSys_Hybrid_CPU::Processor::CPU_Core>& core, std::span<RetireRecord> retireRecords, int& retireRecordCount) {}
        static void AppendWriteBackRetireRecord(std::span<RetireRecord> retireRecords, int& retireRecordCount, const RetireRecord& record)
        { if (retireRecordCount < 0 || static_cast<size_t>(retireRecordCount) >= retireRecords.size()) throw std::out_of_range("WB retire record buffer exhausted."); std::construct_at(retireRecords.data() + retireRecordCount++, record); }
        virtual bool TryGetPrimaryWriteBackResult(unsigned long long& value)
        {
            value = 0;
            return false;
        }
        virtual void CapturePrimaryWriteBackResult(unsigned long long) {}
        virtual std::wstring GetDescription() = 0;
        virtual void RefreshWriteMetadata() {}
    protected:
        static bool TryNormalizeFlatArchRegId(unsigned short rawRegId, int& archRegId)
        {
            ArchRegId regId;
            if (!ArchRegId::TryCreate(rawRegId, regId)) { archRegId = 0; return false; }
            archRegId = regId.getValue();
            return true;
        }
        void SetHardPinnedPlacement(SlotClass requiredSlotClass, unsigned char pinnedLaneId)
        {
            SetPlacement(requiredSlotClass, SlotPinningKind::HardPinned, pinnedLaneId);
        }

    // Remaining execution and retire methods require conversion from MicroOp.cs.
};
}


