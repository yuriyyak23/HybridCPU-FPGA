#pragma once
#include "../../../Architecture/Common/CanonicalIsaOpcode.h"
#include "../../../../../NonRTL/Arch/InstructionClass.h"
#include "../../../../../NonRTL/Arch/SerializationClass.h"
#include "../Internal/InternalOp.h"
#include "../../../Execution/DmaStreamCompute/DmaStreamComputeDescriptor.h"
#include "../../../Execution/ExternalAccelerators/Descriptors/AcceleratorCommandDescriptor.h"
#include "../../../Virtualization/Compatibility/Frontend/Decode/VmxInstructionPayload.h"
#include "../../../ISA/Instructions/NonVmx/Lanes00_03Vector/MatrixTile/MatrixTileIrProjectionAndMaterializer.h"
#include <memory>
#include <optional>
#include <stdexcept>

using namespace YAKSys_Hybrid_CPU::Arch;
using namespace YAKSys_Hybrid_CPU::Core::Execution::DmaStreamCompute;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Descriptors;
using namespace YAKSys_Hybrid_CPU::CloseToHSL::Core::ISA::Instructions::NonVmx::Lanes00_03Vector::MatrixTile;

namespace YAKSys_Hybrid_CPU::Core::Pipeline::MicroOps
{
	struct VectorInstructionPayload final {
		unsigned long long PrimaryPointer = 0, SecondaryPointer = 0;
		unsigned int StreamLength = 0;
		unsigned short Stride = 0, RowStride = 0;
		bool Indexed = false, Is2D = false, TailAgnostic = false, MaskAgnostic = false, Saturating = false;
		unsigned char PredicateMask = 0, DataType = 0;
		std::shared_ptr<YAKSys_Hybrid_CPU::CloseToHSL::Core::ISA::Instructions::NonVmx::Lanes00_03Vector::MatrixTile::MatrixTileNumericPolicy> MatrixTileNumericPolicy;
		std::shared_ptr<YAKSys_Hybrid_CPU::CloseToHSL::Core::ISA::Instructions::NonVmx::Lanes00_03Vector::MatrixTile::MatrixTileLayoutPolicy> MatrixTileLayoutPolicy;
	};
	class InstructionIR final {
	public:
		std::shared_ptr<YAKSys_Hybrid_CPU::CanonicalIsaOpcode> CanonicalOpcode;
		YAKSys_Hybrid_CPU::Arch::InstructionClass Class{};
		YAKSys_Hybrid_CPU::Arch::SerializationClass SerializationClass{};
		unsigned char Rd = 0, Rs1 = 0, Rs2 = 0;
		long long Imm = 0;
		bool HasAbsoluteAddressing = false, AcquireOrdering = false, ReleaseOrdering = false;
		InternalOpDataType DataType = InternalOpDataType::DWord;
		std::optional<std::shared_ptr<VectorInstructionPayload>> VectorPayload;
		std::optional<std::shared_ptr<YAKSys_Hybrid_CPU::CloseToHSL::Core::ISA::Instructions::NonVmx::Lanes00_03Vector::MatrixTile::MatrixTileInstructionIrProjection>> MatrixTileProjection;
		std::shared_ptr<YAKSys_Hybrid_CPU::Core::VmxInstructionPayload> VmxPayload;
		std::optional<unsigned short> CsrAddress;
		std::optional<std::shared_ptr<YAKSys_Hybrid_CPU::Core::Execution::DmaStreamCompute::DmaStreamComputeDescriptorReference>> DmaStreamComputeDescriptorReference;
		std::shared_ptr<YAKSys_Hybrid_CPU::Core::Execution::DmaStreamCompute::DmaStreamComputeDescriptor> DmaStreamComputeDescriptor;
		std::optional<std::shared_ptr<YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Descriptors::AcceleratorDescriptorReference>> AcceleratorCommandDescriptorReference;
		std::shared_ptr<YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Descriptors::AcceleratorCommandDescriptor> AcceleratorCommandDescriptor;
		std::shared_ptr<InstructionIR> SetAcceleratorCommandDescriptor(std::shared_ptr<YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Descriptors::AcceleratorCommandDescriptor> descriptor) {
			if (!descriptor) throw std::invalid_argument("descriptor");
			auto copy = std::make_shared<InstructionIR>(*this);
			copy->AcceleratorCommandDescriptor = std::move(descriptor);
			copy->AcceleratorCommandDescriptorReference = std::make_shared<YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Descriptors::AcceleratorDescriptorReference>(copy->AcceleratorCommandDescriptor->DescriptorReference);
			return copy;
		}
	};
	/// <summary>
	/// Canonical typed vector payload carried by decode for vector memory/compute opcodes.
	/// Raw VLIW pointer and addressing bits are projected into this IR sideband before
	/// runtime materialization, so opcode metadata alone never reopens a vector surface.
	/// </summary>
// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct VectorInstructionPayload(ulong PrimaryPointer, ulong SecondaryPointer, uint StreamLength, ushort Stride, ushort RowStride, bool Indexed, bool Is2D, bool TailAgnostic, bool MaskAgnostic, bool Saturating, byte PredicateMask, byte DataType)
//	{
//		public System.Nullable<MatrixTileNumericPolicy> MatrixTileNumericPolicy
//		{
//			get;
//			set;
//		}
//
//		public System.Nullable<MatrixTileLayoutPolicy> MatrixTileLayoutPolicy
//		{
//			get;
//			set;
//		}
//	}

	/// <summary>
	/// Internal IR record for a single HybridCPU ISA v4 instruction.
	/// This is the canonical IR form used by both the ISE runtime (MicroOp)
	/// and the compiler (IR node).
	/// <para>
	/// INVARIANT: No hint or scheduling policy fields.
	/// All hint/policy information belongs in <c>SlotMetadata</c> / <c>BundleMetadata</c>.
	/// </para>
	/// <para>
	/// Stores canonical opcode identity as <see cref="Processor.CPU_Core.IsaOpcode"/>.
	/// Legacy enum publication is intentionally not part of the IR surface.
	/// </para>
	/// </summary>
// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public sealed record InstructionIR
//	{
//		private Processor.CPU_Core.IsaOpcode _canonicalOpcode;
//
//		/// <summary>Canonical opcode identifier carried by the IR.</summary>
//		public Processor.CPU_Core.IsaOpcode CanonicalOpcode
//		{
//			get
//			{
//				return _canonicalOpcode;
//			}
//			set
//			{
//				_canonicalOpcode = value;
//			}
//		}
//
//		/// <summary>Instruction class — determines pipeline routing.</summary>
//		public required InstructionClass Class
//		{
//			get;
//			set;
//		}
//
//		/// <summary>Serialization class — determines ordering requirements.</summary>
//		public required SerializationClass SerializationClass
//		{
//			get;
//			set;
//		}
//
//		/// <summary>Destination register (0 = no destination / x0).</summary>
//		public required byte Rd
//		{
//			get;
//			set;
//		}
//
//		/// <summary>Source register 1 (0 = x0 or not used).</summary>
//		public required byte Rs1
//		{
//			get;
//			set;
//		}
//
//		/// <summary>Source register 2 (0 = x0 or not used).</summary>
//		public required byte Rs2
//		{
//			get;
//			set;
//		}
//
//		/// <summary>
//		/// Immediate value (sign-extended).
//		/// 0 for register-register instructions.
//		/// When <see cref="HasAbsoluteAddressing"/> is <see langword="true"/>,
//		/// carries the absolute memory address or branch target instead of a
//		/// relative offset.
//		/// </summary>
//		public required long Imm
//		{
//			get;
//			set;
//		}
//
//		/// <summary>
//		/// When <see langword="true"/>, <see cref="Imm"/> carries an absolute
//		/// address (memory address or branch target) rather than a base+offset
//		/// displacement or PC-relative offset. Set by the active decoder boundary
//		/// when it projects an absolute memory address or an explicit compat
//		/// wrapper target into canonical IR.
//		/// </summary>
//		public bool HasAbsoluteAddressing
//		{
//			get;
//			set;
//		}
//
//		/// <summary>Atomic acquire ordering bit captured from canonical decode.</summary>
//		public bool AcquireOrdering
//		{
//			get;
//			set;
//		}
//
//		/// <summary>Atomic release ordering bit captured from canonical decode.</summary>
//		public bool ReleaseOrdering
//		{
//			get;
//			set;
//		}
//
//		/// <summary>
//		/// Operand data type for typed memory and vector operations.
//		/// Defaults to <see cref="InternalOpDataType.DWord"/> for most scalar instructions.
//		/// </summary>
//		public InternalOpDataType DataType
//		{
//			get;
//			set;
//		} = InternalOpDataType.DWord;
//
//		/// <summary>
//		/// Canonical vector payload for published vector opcodes. This payload is
//		/// decode-owned transport data only; executable authority still requires the
//		/// registry, materializer, MicroOp, execute, retire, and replay proof chain.
//		/// </summary>
//		public System.Nullable<VectorInstructionPayload> VectorPayload
//		{
//			get;
//			set;
//		}
//
//		/// <summary>
//		/// Typed matrix/tile runtime projection for MTILE decode. This sideband is
//		/// materializer evidence only; it does not publish a MicroOp, scheduler lane,
//		/// execution path, retire behavior, replay behavior, compiler helper, or
//		/// fallback lowering authority.
//		/// </summary>
//		public System.Nullable<MatrixTileInstructionIrProjection> MatrixTileProjection
//		{
//			get;
//			set;
//		}
//
//		/// <summary>
//		/// Typed VMX-v2 operand/qualification sideband. VMX execution remains
//		/// capability-gated at runtime; this payload does not grant legality.
//		/// </summary>
//		public System.Nullable<YAKSys_Hybrid_CPU.Core.VmxInstructionPayload> VmxPayload
//		{
//			get;
//			set;
//		}
//
//		/// <summary>
//		/// CSR address for CSR-class instructions (CSRRW/CSRRS/CSRRC and immediate variants).
//		/// <see langword="null"/> for all non-CSR instructions.
//		/// </summary>
//		public System.Nullable<ushort> CsrAddress
//		{
//			get;
//			set;
//		}
//
//		/// <summary>
//		/// Typed decoded sideband for descriptor-backed lane6 memory-memory compute.
//		/// The raw VLIW slot layout is not extended to carry this ABI. Phase 10 native
//		/// decode requires the guard-accepted descriptor payload before projector
//		/// admission can produce the lane6 DmaStreamComputeMicroOp carrier.
//		/// </summary>
//		public System.Nullable<DmaStreamComputeDescriptorReference> DmaStreamComputeDescriptorReference
//		{
//			get;
//			set;
//		}
//
//		/// <summary>
//		/// Guard-accepted descriptor payload that survived compiler annotation,
//		/// VLIW decode, InstructionIR, and projector transport without using raw
//		/// reserved VLIW fields as ABI storage.
//		/// </summary>
//		public System.Nullable<DmaStreamComputeDescriptor> DmaStreamComputeDescriptor
//		{
//			get;
//			set;
//		}
//
//		/// <summary>
//		/// Typed descriptor reference for lane7 L7-SDC external accelerator commands.
//		/// This is sideband evidence only; raw reserved VLIW fields and raw VT hints are
//		/// never descriptor ABI or owner/domain authority.
//		/// </summary>
//		public System.Nullable<AcceleratorDescriptorReference> AcceleratorCommandDescriptorReference
//		{
//			get;
//			set;
//		}
//
//		/// <summary>
//		/// Parsed L7-SDC command descriptor payload that survived native carrier
//		/// cleanliness validation at decode. The L7-SDC transport uses this only to
//		/// materialize a guarded lane7 carrier; it does not grant backend execution,
//		/// queue admission, token lifecycle, staged writes, or commit authority.
//		/// </summary>
//		public System.Nullable<AcceleratorCommandDescriptor> AcceleratorCommandDescriptor
//		{
//			get;
//			set;
//		}
//
//		public InstructionIR SetAcceleratorCommandDescriptor(AcceleratorCommandDescriptor descriptor)
//		{
//			System.ArgumentNullException.ThrowIfNull(descriptor);
//			return this with { AcceleratorCommandDescriptor = descriptor, AcceleratorCommandDescriptorReference = descriptor.DescriptorReference };
//		}
//
//		// NOTE: No hint fields here.
//		// Scheduling hints (branch prediction, stealability, locality, thermal) belong
//		// in SlotMetadata attached to the instruction's slot position in the bundle.
//	}
}
