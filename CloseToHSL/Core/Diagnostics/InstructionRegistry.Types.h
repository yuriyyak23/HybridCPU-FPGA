#pragma once

#include "../Pipeline/MicroOps/Types/MicroOp.h"
#include "../Pipeline/Metadata/ISlotMetadataBuilder.h"
#include "../Pipeline/Metadata/ReadRangeMetadataHelper.h"
#include <string>
#include <vector>
#include <optional>
#include <memory>

using namespace YAKSys_Hybrid_CPU::Arch;
using namespace YAKSys_Hybrid_CPU::CloseToHSL::Core::ISA::Instructions::NonVmx::Lanes00_03Vector::MatrixTile;
using namespace YAKSys_Hybrid_CPU::Core::Pipeline::Metadata;

namespace YAKSys_Hybrid_CPU::Core
{
	/// <summary>
	/// Context information passed to MicroOp factories during decoding
	/// </summary>
	class DecoderContext
	{
	public:
		unsigned int OpCode = 0;
		unsigned short Immediate = 0;
		bool HasImmediate = false;
		unsigned char DataType = 0;
		bool HasDataType = false;
		bool IndexedAddressing = false;
		bool Is2DAddressing = false;
		bool HasVectorAddressingContour = false;
		unsigned long long VectorPrimaryPointer = 0;
		unsigned long long VectorSecondaryPointer = 0;
		unsigned int VectorStreamLength = 0;
		unsigned short VectorStride = 0;
		unsigned short VectorRowStride = 0;
		bool TailAgnostic = false;
		bool MaskAgnostic = false;
		bool Saturating = false;
		std::optional<std::shared_ptr<MatrixTileNumericPolicy>> MatrixTileNumericPolicy;
		std::optional<std::shared_ptr<MatrixTileLayoutPolicy>> MatrixTileLayoutPolicy;
		bool HasVectorPayload = false;
		unsigned long long MemoryAddress = 0;
		bool HasMemoryAddress = false;
		unsigned long long PackedRegisterTriplet = 0;
		bool HasPackedRegisterTriplet = false;
		unsigned short Reg1ID = 0;
		unsigned short Reg2ID = 0;
		unsigned short Reg3ID = 0;
		unsigned long long AuxData = 0;
		unsigned short PredicateMask = 0;
		bool AcquireOrdering = false;
		bool ReleaseOrdering = false;
		int OwnerThreadId = 0;
		int MemoryDomainId = 0;
	};

	/// <summary>
	/// Safety mask generation context
	/// </summary>
	class SafetyMaskContext
	{
	public:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: public IReadOnlyList<int> ReadRegisters;
		const std::vector<int> ReadRegisters;
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: public IReadOnlyList<int> WriteRegisters;
		const std::vector<int> WriteRegisters;
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: public IReadOnlyList<(ulong Address, ulong Length)> ReadMemoryRanges;
		const std::vector<std::pair<unsigned long long, unsigned long long>> ReadMemoryRanges;
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: public IReadOnlyList<(ulong Address, ulong Length)> NormalizedReadMemoryRanges;
		const std::vector<std::pair<unsigned long long, unsigned long long>> NormalizedReadMemoryRanges;
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: public IReadOnlyList<(ulong Address, ulong Length)> WriteMemoryRanges;
		const std::vector<std::pair<unsigned long long, unsigned long long>> WriteMemoryRanges;
		std::shared_ptr<AssistCoalescingDescriptor> AssistCoalescingDescriptor;
		int MemoryDomainId = 0;
		bool IsMemoryOp = false;
		bool IsLoad = false;
		bool IsStore = false;
		bool IsAtomic = false;
		/// Virtual thread that owns this operation.
		/// Used by <see cref="InstructionRegistry.DefaultSafetyMask128Generator"/> to keep
		/// 128-bit masks aware of per-VT resource ownership.
		/// Blueprint section 2: generate 128-bit masks with per-VT awareness.
		/// </summary>
		int VirtualThreadId = 0;
	};

	/// <summary>
	/// Resource footprint for retained custom-accelerator descriptor fixtures (Phase 4).
	/// This is compatibility metadata only; it is not current typed-slot authority.
	/// </summary>
	class AcceleratorResourceFootprint
	{
	public:
		int MemoryBandwidthMBps = 0; // MB/s required
		int RegisterFileAccesses = 0; // Number of register reads/writes
		bool RequiresExclusiveAccess = false; // Can't share with other ops
	};

	/// <summary>
	/// Interface for retained custom-accelerator descriptor fixtures (Phase 4).
	/// Registration through this interface records legacy opcode families for tests and
	/// fail-closed diagnostics. It does not publish canonical decode, DMA, replay, retire,
	/// or scheduler authority.
	///
	/// Fixture philosophy:
	/// - keep legacy registration surfaces available for negative controls;
	/// - report shape, latency, and resource metadata without enabling runtime success;
	/// - fail closed at runtime until a truthful carrier contract exists.
	/// </summary>
	class ICustomAccelerator
	{
		/// Accelerator name/identifier
		/// </summary>
	public:
		virtual std::wstring getName() const = 0;

		/// Supported legacy opcode family recorded for fail-closed checks.
		/// </summary>
		virtual const std::vector<unsigned int> getSupportedOpcodes() const = 0;

		/// Execute the local descriptor-fixture model with given operands.
		/// This method is not canonical runtime publication authority.
		/// </summary>
		/// <param name="opcode">Operation code</param>
		/// <param name="operands">Input operands</param>
		/// <param name="config">Accelerator-specific configuration</param>
		/// <returns>Result values</returns>
		virtual std::vector<unsigned long long> Execute(unsigned int opcode, std::vector<unsigned long long> &operands, std::vector<unsigned char> &config) = 0;

		/// Query execution latency for operation (may be data-dependent)
		/// </summary>
		/// <param name="opcode">Operation code</param>
		/// <param name="operands">Input operands (for data-dependent latency)</param>
		/// <returns>Estimated latency in cycles</returns>
		virtual int GetLatency(unsigned int opcode, std::vector<unsigned long long> &operands) = 0;

		/// <summary>
		/// Get retained resource metadata for compatibility tests.
		/// </summary>
		/// <param name="opcode">Operation code</param>
		/// <returns>Resource footprint (memory, registers, etc.)</returns>
		virtual AcceleratorResourceFootprint GetResourceFootprint(unsigned int opcode) = 0;

		/// <summary>
		/// Check if operation can be pipelined
		/// </summary>
		virtual bool IsPipelined(unsigned int opcode) = 0;

		/// <summary>
		/// Reset descriptor-fixture state.
		/// </summary>
		virtual void Reset() = 0;
	};

	/// <summary>
	/// Factory function type for creating MicroOps from decoded instructions
	/// </summary>
	using MicroOpFactory = std::function<std::shared_ptr<MicroOp> (DecoderContext context)>;

	/// <summary>
	/// Descriptor for MicroOp attributes used by FSP (Fine-Grained Slot Pilfering).
	///
	/// Purpose: Define FSP-relevant properties of micro-operations at registration time.
	/// Benefits:
	/// - Centralized attribute specification (avoid per-instance assignment)
	/// - Clear documentation of instruction characteristics
	/// - Enables scheduler to make informed packing decisions
	/// - Supports latency-aware and memory-aware scheduling
	///
	/// Usage:
	///   var desc = new MicroOpDescriptor {
	///       Latency = 2,
	///       MemFootprintClass = 2,  // Vector memory access
	///   };
	///   InstructionRegistry.RegisterSemanticFactory(opcode, factory);
	///   InstructionRegistry.RegisterOpAttributes(opcode, desc);
	/// </summary>
	class MicroOpDescriptor : public std::enable_shared_from_this<MicroOpDescriptor>
	{
	private:
		int Latency = 1;
		int MemFootprintClass = 0;
		std::optional<bool> WritesRegister;
		std::optional<bool> IsMemoryOp;

		/// <summary>
		/// Execution latency in cycles (minimum).
		/// Default: 1 cycle
		/// Examples: ADD=1, MUL=3, DIV=16, SQRT=8
		/// Used by scheduler for latency-hiding policies.
		/// </summary>
	public:
		int getLatency() const;
		void setLatency(int value);

		/// <summary>
		/// Memory footprint classification.
		/// 0 = No memory access (compute-only)
		/// 1 = Scalar memory (single load/store)
		/// 2 = Vector memory (multiple elements)
		/// 3 = Stream memory (large burst transactions via StreamEngine)
		/// Used by scheduler to avoid memory bank conflicts.
		/// </summary>
		int getMemFootprintClass() const;
		void setMemFootprintClass(int value);

		// Blueprint sections 2 and 4: semantic flags live in descriptors, not in factories.
		// These overrides let registration keep construction and per-op policy separate.

		/// <summary>
		/// Whether this instruction writes to a destination register.
		/// When non-null, overrides the value set by the factory lambda.
		/// </summary>
		std::optional<bool> getWritesRegister() const;
		void setWritesRegister(const std::optional<bool> &value);

		/// <summary>
		/// Whether this instruction performs a memory access (load or store).
		/// When non-null, overrides the value set by the factory lambda.
		/// </summary>
		std::optional<bool> isMemoryOp() const;
		void setMemoryOp(const std::optional<bool> &value);

		/// <summary>
		/// Create descriptor with default values (compute-only, 1-cycle latency)
		/// </summary>
		MicroOpDescriptor();

		/// <summary>
		/// Create descriptor with specified values (V6 B11: IsControlFlow removed -
		/// use <see cref="Arch.OpcodeRegistry.IsControlFlowOp"/> for architectural classification).
		/// </summary>
		MicroOpDescriptor(int latency, int memFootprintClass);
	};
}



