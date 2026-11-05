#pragma once
#include <optional>

namespace YAKSys_Hybrid_CPU::Core::Pipeline::MicroOps
{
	// ─────────────────────────────────────────────────────────────────────────
	// InternalOpDataType — scalar operand width
	// ─────────────────────────────────────────────────────────────────────────

	/// <summary>
	/// Operand width for a scalar <see cref="InternalOp"/>.
	/// Determines how many bytes the operation reads/writes.
	/// </summary>
	enum class InternalOpDataType : unsigned char
	{
		Byte = 0,
		Half = 1,
		Word = 2,
		DWord = 3
	};

	// ─────────────────────────────────────────────────────────────────────────
	// InternalOpFlags — atomic ordering, signed/unsigned, shift type
	// ─────────────────────────────────────────────────────────────────────────

	/// <summary>
	/// Modifier flags that qualify an <see cref="InternalOp"/> without changing
	/// which architectural state it reads or writes.
	/// </summary>
// C# TO C++ CONVERTER NOTE: The following .NET attribute has no direct equivalent in C++:
// ORIGINAL LINE: [Flags] public enum InternalOpFlags : byte
	enum class InternalOpFlags : unsigned char
	{
		None = 0,
		/// <summary>Arithmetic operation treats operands as signed integers.</summary>
		Signed = 1 << 0,
		/// <summary>Atomic operation carries acquire ordering (A-flag in RISC-V AMO).</summary>
		AcquireOrdering = 1 << 1,
		/// <summary>Atomic operation carries release ordering (R-flag in RISC-V AMO).</summary>
		ReleaseOrdering = 1 << 2,
		/// <summary>Shift is arithmetic (sign-extending) rather than logical.</summary>
		ArithmeticShift = 1 << 3
	};
	constexpr InternalOpFlags operator|(InternalOpFlags left, InternalOpFlags right) noexcept
	{
		return static_cast<InternalOpFlags>(static_cast<unsigned char>(left) | static_cast<unsigned char>(right));
	}
	constexpr InternalOpFlags &operator|=(InternalOpFlags &left, InternalOpFlags right) noexcept
	{
		left = left | right;
		return left;
	}

	// ─────────────────────────────────────────────────────────────────────────
	// InternalOpKind — the operation taxonomy
	// ─────────────────────────────────────────────────────────────────────────

	/// <summary>
	/// Classifies an <see cref="InternalOp"/> into an abstract operation kind,
	/// independent of the originating ISA opcode.
	/// </summary>
	enum class InternalOpKind : unsigned char
	{
		// ── ALU reg-reg ───────────────────────────────────────────────────────
		Add = 0,
		Sub = 1,
		And = 2,
		Or = 3,
		Xor = 4,
		Sll = 5,
		Srl = 6,
		Sra = 7,
		Slt = 8,
		Sltu = 9,

		// ── ALU M-extension ───────────────────────────────────────────────────
		Mul = 10,
		MulH = 11,
		MulHu = 12,
		MulHsu = 13,
		Div = 14,
		Divu = 15,
		Rem = 16,
		Remu = 17,
		AddW = 18,
		SubW = 19,

		// ── ALU immediate ─────────────────────────────────────────────────────
		AddI = 20,
		AndI = 21,
		OrI = 22,
		XorI = 23,
		SllI = 24,
		SrlI = 25,
		SraI = 26,
		SltI = 27,
		SltiU = 28,
		Lui = 29,
		Auipc = 30,
		AddIW = 31,
		SllW = 32,
		SrlW = 33,
		SraW = 34,
		SllIW = 35,
		SrlIW = 36,
		SraIW = 37,
		MulW = 38,
		DivW = 39,

		// ── Memory ────────────────────────────────────────────────────────────
		Load = 40,
		Store = 41,

		// ── Word M-extension continuation; values are explicit to preserve existing InternalOpKind values.
		DivuW = 42,
		RemW = 43,
		RemuW = 44,
		SextW = 45,
		ZextW = 46,
		CzeroEqz = 47,
		Clz = 48,
		Sh1Add = 49,
		ClMul = 75,
		Ctz = 76,
		SextB = 77,
		SextH = 78,
		ZextH = 79,

		// ── Control flow ──────────────────────────────────────────────────────
		Jal = 50,
		Jalr = 51,
		Branch = 52,

		// ── Atomics ───────────────────────────────────────────────────────────
		LrW = 60,
		ScW = 61,
		LrD = 62,
		ScD = 63,
		AmoWord = 64,
		AmoDword = 65,

		// Non-VMX CloseToHSL scalar bitmanip continuation.
		Rol = 66,
		Ror = 67,
		AndN = 115,
		OrN = 116,
		Xnor = 117,
		Min = 118,
		Max = 119,
		MinU = 120,
		MaxU = 121,
		Rev8 = 122,
		Brev8 = 123,
		CzeroNez = 124,
		Cpop = 125,
		RolI = 126,
		RorI = 127,
		Bset = 128,
		Bclr = 129,
		Binv = 130,
		Bext = 131,
		BsetI = 132,
		BclrI = 133,
		BinvI = 134,
		BextI = 135,
		Sh2Add = 136,
		Sh3Add = 137,
		AddUw = 138,
		Sh1AddUw = 139,
		Sh2AddUw = 140,
		Sh3AddUw = 141,
		SlliUw = 142,
		ClMulH = 143,
		ClMulR = 144,

		// ── CSR ───────────────────────────────────────────────────────────────
		CsrReadWrite = 70,
		CsrReadSet = 71,
		CsrReadClear = 72,
		CsrClear = 73,
		CsrReadCounter = 74,

		// ── SMT/VT ────────────────────────────────────────────────────────────
		Yield = 80,
		Wfe = 81,
		Sev = 82,
		PodBarrier = 83,
		VtBarrier = 84,
		Fence = 85,
		FenceI = 86,
		Ecall = 87,
		Ebreak = 88,
		Mret = 89,
		Sret = 90,
		Wfi = 91,
		Interrupt = 92,
		InterruptReturn = 93,

		// ── VMX compute (explicit VMX instruction surface) ───────────────────
		VmxOn = 100,
		VmxOff = 101,
		VmLaunch = 102,
		VmResume = 103,
		VmRead = 104,
		VmWrite = 105,
		VmClear = 106,
		VmPtrLd = 107,
		VmPtrSt = 108,
		VmCall = 109,
		Invept = 110,
		Invvpid = 111,
		VmFunc = 112,
		VmSaveX = 113,
		VmRestX = 114
	};

	// ─────────────────────────────────────────────────────────────────────────
	// E25/E26 (Checklist): InternalOpCategory — operation group taxonomy
	// Separates computation / memory / control-flow / atomics / CSR from
	// the architectural event categories (SysEvent / TrapEvent / VmxEvent).
	// ─────────────────────────────────────────────────────────────────────────

	/// <summary>
	/// High-level category of an <see cref="InternalOp"/>.
	///
	/// E25: Canonical InternalOp model independent of <c>InstructionEnum</c>.
	/// E26: Explicit split between computation ops (ALU / Memory / Branch /
	///      Atomic / CSR) and architectural event categories (SysEvent /
	///      TrapEvent / VmxEvent).
	/// </summary>
	enum class InternalOpCategory : unsigned char
	{
		/// <summary>Integer ALU computation (reg-reg, reg-imm, M-extension).</summary>
		Computation = 0,

		/// <summary>Memory load or store (scalar or atomic).</summary>
		MemoryAccess = 1,

		/// <summary>Control-flow transfer (branch, JAL, JALR).</summary>
		ControlFlow = 2,

		/// <summary>Atomic read-modify-write operation (LR/SC, AMO).</summary>
		Atomic = 3,

		/// <summary>Control/status-register access.</summary>
		Csr = 4,

		/// <summary>
		/// SMT/VT scheduling event (YIELD, WFE, SEV, POD_BARRIER, VT_BARRIER).
		/// E26: Explicitly separate from computation categories.
		/// </summary>
		SysEvent = 5,

		/// <summary>
		/// VMX control / VMCS / VM-entry / VM-exit operation.
		/// E26: Explicitly separate from computation categories.
		/// </summary>
		VmxEvent = 6
	};

	// ─────────────────────────────────────────────────────────────────────────
	// InternalOp — canonical contract (V5 Phase 1 / V6 E25-E28)
	// ─────────────────────────────────────────────────────────────────────────

	/// <summary>
	/// Canonical InternalOp contract — Phase 1 / V6 audit (E25–E28).
	///
	/// Represents a single unit of computation for the execution engine.
	/// No scheduling policy. No hints. No ISA-origin information.
	///
	/// CONTRACT RULE: execution result is 100% determined by <see cref="InternalOp"/>
	/// fields plus architectural state.  No metadata field may affect the result.
	///
	/// DOES NOT carry:
	/// <list type="bullet">
	///   <item>Scheduling hints (branch prediction, stealability, locality, thermal)</item>
	///   <item>FSP stealability</item>
	///   <item>Preferred VT</item>
	///   <item>Any policy field</item>
	/// </list>
	///
	/// Hint/policy information belongs exclusively in <c>SlotMetadata</c> /
	/// <c>BundleMetadata</c>.
	///
	/// V6 additions (E25–E28):
	/// <list type="bullet">
	///   <item>E25/E26: <see cref="Category"/> — group taxonomy independent of ISA opcode.</item>
	///   <item>E27: <see cref="IsMemoryRead"/>, <see cref="IsMemoryWrite"/>,
	///         <see cref="IsControlTransfer"/>, <see cref="IsExceptionPotential"/> — explicit effect descriptors.</item>
	///   <item>E28: <see cref="IsSerializing"/>, <see cref="RequiresPipelineFlush"/>,
	///         <see cref="ForbidsFsp"/>, <see cref="ForbidsSmtInjection"/> — serialisation semantics.</item>
	/// </list>
	/// </summary>
// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public sealed record InternalOp
//	{
//		/// <summary>The abstract operation class — determines which execution unit handles this op.</summary>
//		public required InternalOpKind Kind
//		{
//			get;
//			set;
//		}
//
//		/// <summary>Source register 1 index. -1 = unused.</summary>
//		public int Rs1
//		{
//			get;
//			set;
//		} = -1;
//
//		/// <summary>Source register 2 index. -1 = unused.</summary>
//		public int Rs2
//		{
//			get;
//			set;
//		} = -1;
//
//		/// <summary>Destination register index. -1 = no destination.</summary>
//		public int Rd
//		{
//			get;
//			set;
//		} = -1;
//
//		/// <summary>Immediate value (sign-extended). 0 when not applicable.</summary>
//		public long Immediate
//		{
//			get;
//			set;
//		}
//
//		/// <summary>Operand width. Defaults to <see cref="InternalOpDataType.DWord"/>.</summary>
//		public InternalOpDataType DataType
//		{
//			get;
//			set;
//		} = InternalOpDataType.DWord;
//
//		/// <summary>
//		/// CSR address for CSR operations.
//		/// <see langword="null"/> for all non-CSR operations.
//		/// </summary>
//		public System.Nullable<ushort> CsrTarget
//		{
//			get;
//			set;
//		}
//
//		/// <summary>Atomic ordering, signed/unsigned, shift-type qualifiers.</summary>
//		public InternalOpFlags Flags
//		{
//			get;
//			set;
//		}
//
//		// ─────────────────────────────────────────────────────────────────────
//		// E25/E26: Category — computed from Kind (no ISA opcode dependency)
//		// ─────────────────────────────────────────────────────────────────────
//
//		/// <summary>
//		/// High-level operation category derived solely from <see cref="Kind"/>.
//		/// Never depends on the originating ISA opcode.
//		///
//		/// E25/E26: Canonical category independent of <c>InstructionEnum</c>;
//		/// explicitly separates computation from architectural-event categories.
//		/// </summary>
//		public InternalOpCategory Category
//		{
//// C# TO C++ CONVERTER TASK: The following 'switch expression' was not converted by C# to C++ Converter:
////			return Kind switch
////		{
////			// ── Computation (ALU, M-ext, immediate) ──────────────────────────
////			InternalOpKind.Add  or InternalOpKind.Sub  or InternalOpKind.And  or InternalOpKind.Or   or InternalOpKind.Xor  or InternalOpKind.Sll  or InternalOpKind.Srl  or InternalOpKind.Sra  or InternalOpKind.Slt  or InternalOpKind.Sltu or InternalOpKind.AddW or InternalOpKind.SubW or InternalOpKind.SllW or InternalOpKind.SrlW or InternalOpKind.SraW or InternalOpKind.Mul  or InternalOpKind.MulH or InternalOpKind.MulHu or InternalOpKind.MulHsu or InternalOpKind.Div or InternalOpKind.Divu or InternalOpKind.Rem  or InternalOpKind.Remu or InternalOpKind.AddI or InternalOpKind.AndI or InternalOpKind.OrI  or InternalOpKind.XorI or InternalOpKind.SllI or InternalOpKind.SrlI or InternalOpKind.SraI or InternalOpKind.SltI or InternalOpKind.SltiU or InternalOpKind.Lui  or InternalOpKind.Auipc or InternalOpKind.AddIW or InternalOpKind.SllIW or InternalOpKind.SrlIW or InternalOpKind.SraIW or InternalOpKind.MulW or InternalOpKind.DivW or InternalOpKind.DivuW or InternalOpKind.RemW or InternalOpKind.RemuW or InternalOpKind.SextW or InternalOpKind.ZextW or InternalOpKind.CzeroEqz or InternalOpKind.CzeroNez or InternalOpKind.Clz or InternalOpKind.Ctz or InternalOpKind.Cpop or InternalOpKind.SextB or InternalOpKind.SextH or InternalOpKind.ZextH or InternalOpKind.Rol or InternalOpKind.Ror or InternalOpKind.RolI or InternalOpKind.RorI or InternalOpKind.Bset or InternalOpKind.Bclr or InternalOpKind.Binv or InternalOpKind.Bext or InternalOpKind.BsetI or InternalOpKind.BclrI or InternalOpKind.BinvI or InternalOpKind.BextI or InternalOpKind.AndN or InternalOpKind.OrN or InternalOpKind.Xnor or InternalOpKind.Min or InternalOpKind.Max or InternalOpKind.MinU or InternalOpKind.MaxU or InternalOpKind.Rev8 or InternalOpKind.Brev8 or InternalOpKind.Sh1Add or InternalOpKind.Sh2Add or InternalOpKind.Sh3Add or InternalOpKind.AddUw or InternalOpKind.Sh1AddUw or InternalOpKind.Sh2AddUw or InternalOpKind.Sh3AddUw or InternalOpKind.SlliUw or InternalOpKind.ClMul or InternalOpKind.ClMulH or InternalOpKind.ClMulR => InternalOpCategory.Computation,
////ignore
////ignore
////ignore
////ignore
////ignore
////ignore
////ignore
////ignore
////ignore
////ignore
////ignore
////ignore
////ignore
////ignore
////ignore
////ignore
////ignore
////ignore
////ignore
////ignore
////ignore
////ignore
////ignore
////ignore
////ignore
////ignore
////ignore
////
////			// ── Memory access ────────────────────────────────────────────────
////			InternalOpKind.Load or InternalOpKind.Store => InternalOpCategory.MemoryAccess,
////
////			// ── Control flow ─────────────────────────────────────────────────
////			InternalOpKind.Jal or InternalOpKind.Jalr or InternalOpKind.Branch => InternalOpCategory.ControlFlow,
////
////			// ── Atomics ──────────────────────────────────────────────────────
////			InternalOpKind.LrW  or InternalOpKind.ScW  or InternalOpKind.LrD  or InternalOpKind.ScD  or InternalOpKind.AmoWord or InternalOpKind.AmoDword => InternalOpCategory.Atomic,
////ignore
////ignore
////
////			// ── CSR ──────────────────────────────────────────────────────────
////			InternalOpKind.CsrReadWrite or InternalOpKind.CsrReadSet or InternalOpKind.CsrReadClear or InternalOpKind.CsrClear or InternalOpKind.CsrReadCounter => InternalOpCategory.Csr,
////ignore
////
////			// ── SMT/VT system events ─────────────────────────────────────────
////			InternalOpKind.Yield or InternalOpKind.Wfe or InternalOpKind.Sev or InternalOpKind.PodBarrier or InternalOpKind.VtBarrier or InternalOpKind.Fence or InternalOpKind.FenceI or InternalOpKind.Ecall or InternalOpKind.Ebreak or InternalOpKind.Mret or InternalOpKind.Sret or InternalOpKind.Wfi or InternalOpKind.Interrupt or InternalOpKind.InterruptReturn => InternalOpCategory.SysEvent,
////ignore
////ignore
////ignore
////ignore
////
////			// ── VMX data-plane events ────────────────────────────────────────
////			InternalOpKind.VmxOn or InternalOpKind.VmxOff or InternalOpKind.VmLaunch or InternalOpKind.VmResume or InternalOpKind.VmRead or InternalOpKind.VmWrite or InternalOpKind.VmClear or InternalOpKind.VmPtrLd or InternalOpKind.VmPtrSt or InternalOpKind.VmCall or InternalOpKind.Invept or InternalOpKind.Invvpid or InternalOpKind.VmFunc or InternalOpKind.VmSaveX or InternalOpKind.VmRestX => InternalOpCategory.VmxEvent,
////ignore
////ignore
////ignore
////ignore
////ignore
////ignore
////
////			_ => InternalOpCategory.Computation,
////		};
//		}
//
//		// ─────────────────────────────────────────────────────────────────────
//		// E27: Explicit effect descriptors
//		// ─────────────────────────────────────────────────────────────────────
//
//		/// <summary>
//		/// <see langword="true"/> if this operation reads from memory.
//		///
//		/// E27: Explicit effect descriptor; does not require opcode-range inspection.
//		/// </summary>
//		public bool IsMemoryRead
//		{
//			get
//			{
//				return Kind is InternalOpKind.Load or InternalOpKind.LrW or InternalOpKind.LrD or InternalOpKind.AmoWord or InternalOpKind.AmoDword;
//			}
//		}
//
//		/// <summary>
//		/// <see langword="true"/> if this operation writes to memory.
//		///
//		/// E27: Explicit effect descriptor; does not require opcode-range inspection.
//		/// </summary>
//		public bool IsMemoryWrite
//		{
//			get
//			{
//				return Kind is InternalOpKind.Store or InternalOpKind.ScW or InternalOpKind.ScD or InternalOpKind.AmoWord or InternalOpKind.AmoDword;
//			}
//		}
//
//		/// <summary>
//		/// <see langword="true"/> if this operation may change the program counter
//		/// to a non-sequential address (branch, jump, call, return).
//		///
//		/// E27: Explicit ControlTransfer effect descriptor.
//		/// </summary>
//		public bool IsControlTransfer
//		{
//			get
//			{
//				return Category == InternalOpCategory.ControlFlow;
//			}
//		}
//
//		/// <summary>
//		/// <see langword="true"/> if this operation may raise an exception (e.g. divide-by-zero,
//		/// misaligned access, page fault) or generates a trap boundary event.
//		///
//		/// E27: Explicit ExceptionPotential effect descriptor.
//		/// </summary>
//		public bool IsExceptionPotential
//		{
//			get
//			{
//				return Kind is InternalOpKind.Div or InternalOpKind.Divu or InternalOpKind.Rem or InternalOpKind.Remu or InternalOpKind.Load or InternalOpKind.Store or InternalOpKind.LrW or InternalOpKind.LrD or InternalOpKind.ScW or InternalOpKind.ScD or InternalOpKind.AmoWord or InternalOpKind.AmoDword or InternalOpKind.Jal or InternalOpKind.Jalr or InternalOpKind.CsrReadWrite or InternalOpKind.CsrReadSet or InternalOpKind.CsrReadClear or InternalOpKind.CsrClear or InternalOpKind.CsrReadCounter or InternalOpKind.Ecall or InternalOpKind.Ebreak or InternalOpKind.Mret or InternalOpKind.Sret or InternalOpKind.Interrupt or InternalOpKind.InterruptReturn;
//			}
//		}
//
//		/// <summary>
//		/// <see langword="true"/> if this operation has memory ordering semantics.
//		/// Atomics with Acquire/Release flags always have ordering.
//		///
//		/// E27: Explicit HasOrdering effect descriptor.
//		/// </summary>
//		public bool HasOrdering
//		{
//			get
//			{
//				return Category == InternalOpCategory.Atomic && (Flags & (InternalOpFlags.AcquireOrdering | InternalOpFlags.ReleaseOrdering)) != 0;
//			}
//		}
//
//		// ─────────────────────────────────────────────────────────────────────
//		// E28: Serialisation semantics (pipeline scheduling contract)
//		// ─────────────────────────────────────────────────────────────────────
//
//		/// <summary>
//		/// <see langword="true"/> if this operation is fully serialising:
//		/// all preceding ops must retire and all following ops must wait
//		/// until this op completes.
//		///
//		/// E28: Serialisation is part of InternalOp semantics, not implicit.
//		/// </summary>
//		public bool IsSerializing
//		{
//			get
//			{
//				return Kind is InternalOpKind.CsrReadWrite or InternalOpKind.CsrReadSet or InternalOpKind.CsrReadClear or InternalOpKind.CsrClear or InternalOpKind.CsrReadCounter or InternalOpKind.Wfe or InternalOpKind.Sev or InternalOpKind.PodBarrier or InternalOpKind.VtBarrier or InternalOpKind.Fence or InternalOpKind.FenceI or InternalOpKind.Ecall or InternalOpKind.Ebreak or InternalOpKind.Mret or InternalOpKind.Sret or InternalOpKind.Wfi or InternalOpKind.Interrupt or InternalOpKind.InterruptReturn or InternalOpKind.VmxOn or InternalOpKind.VmxOff or InternalOpKind.VmLaunch or InternalOpKind.VmResume or InternalOpKind.VmRead or InternalOpKind.VmWrite or InternalOpKind.VmClear or InternalOpKind.VmPtrLd or InternalOpKind.VmPtrSt or InternalOpKind.VmCall or InternalOpKind.Invept or InternalOpKind.Invvpid or InternalOpKind.VmFunc or InternalOpKind.VmSaveX or InternalOpKind.VmRestX;
//			}
//		}
//
//		/// <summary>
//		/// <see langword="true"/> if pipeline must be fully flushed before
//		/// this operation can retire (instruction cache included for I-fence class).
//		///
//		/// E28: RequiresPipelineFlush is part of InternalOp semantics.
//		/// </summary>
//		public bool RequiresPipelineFlush
//		{
//			get
//			{
//				return Kind is InternalOpKind.PodBarrier or InternalOpKind.VtBarrier or InternalOpKind.FenceI or InternalOpKind.Ecall or InternalOpKind.Ebreak or InternalOpKind.Mret or InternalOpKind.Sret or InternalOpKind.Interrupt or InternalOpKind.InterruptReturn or InternalOpKind.VmxOn or InternalOpKind.VmxOff or InternalOpKind.VmLaunch or InternalOpKind.VmResume or InternalOpKind.VmRead or InternalOpKind.VmWrite or InternalOpKind.VmClear or InternalOpKind.VmPtrLd or InternalOpKind.VmPtrSt or InternalOpKind.VmCall or InternalOpKind.Invept or InternalOpKind.Invvpid or InternalOpKind.VmFunc or InternalOpKind.VmSaveX or InternalOpKind.VmRestX;
//			}
//		}
//
//		/// <summary>
//		/// <see langword="true"/> if the retained compatibility alias for
//		/// bundle densification / cross-thread slot injection is forbidden
//		/// for any slot in the same bundle as this operation.
//		///
//		/// E28: <see cref="ForbidsFsp"/> is a retained compatibility alias
//		/// over the canonical serialization boundary.
//		/// </summary>
//		public bool ForbidsFsp
//		{
//			get
//			{
//				return IsSerializing;
//			}
//		}
//
//		/// <summary>
//		/// <see langword="true"/> if SMT cross-thread injection into empty slots
//		/// in the same bundle is forbidden.
//		///
//		/// E28: ForbidsSmtInjection is part of InternalOp semantics.
//		/// </summary>
//		public bool ForbidsSmtInjection
//		{
//			get
//			{
//				return IsSerializing;
//			}
//		}
//	}

	// Active C++ contract for the C# sealed record above. The field names and
	// defaults intentionally match the builder and execution-unit interfaces.
	class InternalOp final
	{
	public:
		InternalOpKind Kind{};
		int Rs1 = -1;
		int Rs2 = -1;
		int Rd = -1;
		long long Immediate = 0;
		InternalOpDataType DataType = InternalOpDataType::DWord;
		std::optional<unsigned short> CsrTarget;
		InternalOpFlags Flags = InternalOpFlags::None;

		InternalOpCategory getCategory() const
		{
switch (Kind) {
			case InternalOpKind::Add:
			case InternalOpKind::Sub:
			case InternalOpKind::And:
			case InternalOpKind::Or:
			case InternalOpKind::Xor:
			case InternalOpKind::Sll:
			case InternalOpKind::Srl:
			case InternalOpKind::Sra:
			case InternalOpKind::Slt:
			case InternalOpKind::Sltu:
			case InternalOpKind::AddW:
			case InternalOpKind::SubW:
			case InternalOpKind::SllW:
			case InternalOpKind::SrlW:
			case InternalOpKind::SraW:
			case InternalOpKind::Mul:
			case InternalOpKind::MulH:
			case InternalOpKind::MulHu:
			case InternalOpKind::MulHsu:
			case InternalOpKind::Div:
			case InternalOpKind::Divu:
			case InternalOpKind::Rem:
			case InternalOpKind::Remu:
			case InternalOpKind::AddI:
			case InternalOpKind::AndI:
			case InternalOpKind::OrI:
			case InternalOpKind::XorI:
			case InternalOpKind::SllI:
			case InternalOpKind::SrlI:
			case InternalOpKind::SraI:
			case InternalOpKind::SltI:
			case InternalOpKind::SltiU:
			case InternalOpKind::Lui:
			case InternalOpKind::Auipc:
			case InternalOpKind::AddIW:
			case InternalOpKind::SllIW:
			case InternalOpKind::SrlIW:
			case InternalOpKind::SraIW:
			case InternalOpKind::MulW:
			case InternalOpKind::DivW:
			case InternalOpKind::DivuW:
			case InternalOpKind::RemW:
			case InternalOpKind::RemuW:
			case InternalOpKind::SextW:
			case InternalOpKind::ZextW:
			case InternalOpKind::CzeroEqz:
			case InternalOpKind::CzeroNez:
			case InternalOpKind::Clz:
			case InternalOpKind::Ctz:
			case InternalOpKind::Cpop:
			case InternalOpKind::SextB:
			case InternalOpKind::SextH:
			case InternalOpKind::ZextH:
			case InternalOpKind::Rol:
			case InternalOpKind::Ror:
			case InternalOpKind::RolI:
			case InternalOpKind::RorI:
			case InternalOpKind::Bset:
			case InternalOpKind::Bclr:
			case InternalOpKind::Binv:
			case InternalOpKind::Bext:
			case InternalOpKind::BsetI:
			case InternalOpKind::BclrI:
			case InternalOpKind::BinvI:
			case InternalOpKind::BextI:
			case InternalOpKind::AndN:
			case InternalOpKind::OrN:
			case InternalOpKind::Xnor:
			case InternalOpKind::Min:
			case InternalOpKind::Max:
			case InternalOpKind::MinU:
			case InternalOpKind::MaxU:
			case InternalOpKind::Rev8:
			case InternalOpKind::Brev8:
			case InternalOpKind::Sh1Add:
			case InternalOpKind::Sh2Add:
			case InternalOpKind::Sh3Add:
			case InternalOpKind::AddUw:
			case InternalOpKind::Sh1AddUw:
			case InternalOpKind::Sh2AddUw:
			case InternalOpKind::Sh3AddUw:
			case InternalOpKind::SlliUw:
			case InternalOpKind::ClMul:
			case InternalOpKind::ClMulH:
			case InternalOpKind::ClMulR:
				return InternalOpCategory::Computation;
			case InternalOpKind::Load:
			case InternalOpKind::Store:
				return InternalOpCategory::MemoryAccess;
			case InternalOpKind::Jal:
			case InternalOpKind::Jalr:
			case InternalOpKind::Branch:
				return InternalOpCategory::ControlFlow;
			case InternalOpKind::LrW:
			case InternalOpKind::ScW:
			case InternalOpKind::LrD:
			case InternalOpKind::ScD:
			case InternalOpKind::AmoWord:
			case InternalOpKind::AmoDword:
				return InternalOpCategory::Atomic;
			case InternalOpKind::CsrReadWrite:
			case InternalOpKind::CsrReadSet:
			case InternalOpKind::CsrReadClear:
			case InternalOpKind::CsrClear:
			case InternalOpKind::CsrReadCounter:
				return InternalOpCategory::Csr;
			case InternalOpKind::Yield:
			case InternalOpKind::Wfe:
			case InternalOpKind::Sev:
			case InternalOpKind::PodBarrier:
			case InternalOpKind::VtBarrier:
			case InternalOpKind::Fence:
			case InternalOpKind::FenceI:
			case InternalOpKind::Ecall:
			case InternalOpKind::Ebreak:
			case InternalOpKind::Mret:
			case InternalOpKind::Sret:
			case InternalOpKind::Wfi:
			case InternalOpKind::Interrupt:
			case InternalOpKind::InterruptReturn:
				return InternalOpCategory::SysEvent;
			case InternalOpKind::VmxOn:
			case InternalOpKind::VmxOff:
			case InternalOpKind::VmLaunch:
			case InternalOpKind::VmResume:
			case InternalOpKind::VmRead:
			case InternalOpKind::VmWrite:
			case InternalOpKind::VmClear:
			case InternalOpKind::VmPtrLd:
			case InternalOpKind::VmPtrSt:
			case InternalOpKind::VmCall:
			case InternalOpKind::Invept:
			case InternalOpKind::Invvpid:
			case InternalOpKind::VmFunc:
			case InternalOpKind::VmSaveX:
			case InternalOpKind::VmRestX:
				return InternalOpCategory::VmxEvent;
				return InternalOpCategory::Computation;
			default: return InternalOpCategory::Computation;
		}
		}

		bool isMemoryRead() const
		{
			return Kind == InternalOpKind::Load || Kind == InternalOpKind::LrW || Kind == InternalOpKind::LrD || Kind == InternalOpKind::AmoWord || Kind == InternalOpKind::AmoDword;
		}
		bool isMemoryWrite() const
		{
			return Kind == InternalOpKind::Store || Kind == InternalOpKind::ScW || Kind == InternalOpKind::ScD || Kind == InternalOpKind::AmoWord || Kind == InternalOpKind::AmoDword;
		}
		bool isControlTransfer() const { return getCategory() == InternalOpCategory::ControlFlow; }
		bool isExceptionPotential() const
		{
			return Kind == InternalOpKind::Div || Kind == InternalOpKind::Divu || Kind == InternalOpKind::Rem || Kind == InternalOpKind::Remu ||
				isMemoryRead() || isMemoryWrite() || Kind == InternalOpKind::Jal || Kind == InternalOpKind::Jalr ||
				Kind == InternalOpKind::CsrReadWrite || Kind == InternalOpKind::CsrReadSet || Kind == InternalOpKind::CsrReadClear || Kind == InternalOpKind::CsrClear || Kind == InternalOpKind::CsrReadCounter ||
				Kind == InternalOpKind::Ecall || Kind == InternalOpKind::Ebreak || Kind == InternalOpKind::Mret || Kind == InternalOpKind::Sret || Kind == InternalOpKind::Interrupt || Kind == InternalOpKind::InterruptReturn;
		}
		bool hasOrdering() const
		{
			const auto mask = static_cast<unsigned char>(InternalOpFlags::AcquireOrdering) | static_cast<unsigned char>(InternalOpFlags::ReleaseOrdering);
			return getCategory() == InternalOpCategory::Atomic && (static_cast<unsigned char>(Flags) & mask) != 0;
		}
		bool isSerializing() const
		{
			return Kind == InternalOpKind::CsrReadWrite || Kind == InternalOpKind::CsrReadSet || Kind == InternalOpKind::CsrReadClear || Kind == InternalOpKind::CsrClear || Kind == InternalOpKind::CsrReadCounter ||
				Kind == InternalOpKind::Wfe || Kind == InternalOpKind::Sev || Kind == InternalOpKind::PodBarrier || Kind == InternalOpKind::VtBarrier || Kind == InternalOpKind::Fence || Kind == InternalOpKind::FenceI ||
				Kind == InternalOpKind::Ecall || Kind == InternalOpKind::Ebreak || Kind == InternalOpKind::Mret || Kind == InternalOpKind::Sret || Kind == InternalOpKind::Wfi || Kind == InternalOpKind::Interrupt || Kind == InternalOpKind::InterruptReturn ||
				Kind == InternalOpKind::VmxOn || Kind == InternalOpKind::VmxOff || Kind == InternalOpKind::VmLaunch || Kind == InternalOpKind::VmResume || Kind == InternalOpKind::VmRead || Kind == InternalOpKind::VmWrite || Kind == InternalOpKind::VmClear ||
				Kind == InternalOpKind::VmPtrLd || Kind == InternalOpKind::VmPtrSt || Kind == InternalOpKind::VmCall || Kind == InternalOpKind::Invept || Kind == InternalOpKind::Invvpid || Kind == InternalOpKind::VmFunc || Kind == InternalOpKind::VmSaveX || Kind == InternalOpKind::VmRestX;
		}
		bool requiresPipelineFlush() const
		{
			return Kind == InternalOpKind::PodBarrier || Kind == InternalOpKind::VtBarrier || Kind == InternalOpKind::FenceI || Kind == InternalOpKind::Ecall || Kind == InternalOpKind::Ebreak || Kind == InternalOpKind::Mret || Kind == InternalOpKind::Sret ||
				Kind == InternalOpKind::Interrupt || Kind == InternalOpKind::InterruptReturn || Kind == InternalOpKind::VmxOn || Kind == InternalOpKind::VmxOff || Kind == InternalOpKind::VmLaunch || Kind == InternalOpKind::VmResume ||
				Kind == InternalOpKind::VmRead || Kind == InternalOpKind::VmWrite || Kind == InternalOpKind::VmClear || Kind == InternalOpKind::VmPtrLd || Kind == InternalOpKind::VmPtrSt || Kind == InternalOpKind::VmCall ||
				Kind == InternalOpKind::Invept || Kind == InternalOpKind::Invvpid || Kind == InternalOpKind::VmFunc || Kind == InternalOpKind::VmSaveX || Kind == InternalOpKind::VmRestX;
		}
		bool forbidsFsp() const { return isSerializing(); }
		bool forbidsSmtInjection() const { return isSerializing(); }
	};
}
