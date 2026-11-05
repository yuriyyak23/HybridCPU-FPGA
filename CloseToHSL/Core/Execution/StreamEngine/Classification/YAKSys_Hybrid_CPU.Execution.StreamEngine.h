#pragma once

#include "../StreamDescriptors.h"
#include "../../../../../NonRTL/Arch/Compat/HybridCPU_ISE.Arch.VLIW_Instruction.h"
#include "../../../Architecture/Common/YAKSys_Hybrid_CPU.Processor.h"
#include "../StreamExecutionRequest.h"
#include "../../../../../Arch/DataTypeEnum.h"
#include "../../../Architecture/Registers/Retire/RetireRecord.h"
namespace YAKSys_Hybrid_CPU::Memory { class MemorySubsystem; class MemoryRequestToken; }
#include "../../../Pipeline/Assist/Runtime/AssistRuntime.h"
#include "../../../../Memory/Registers/YAKSys_Hybrid_CPU.Memory.StreamRegisterFile.h"
#include <string>
#include <vector>
#include <cmath>
#include <limits>
#include <optional>
#include <format>
#include "tangible_exception_helper.h"
#include <memory>

using namespace YAKSys_Hybrid_CPU::Arch;
using namespace YAKSys_Hybrid_CPU::Memory;
using namespace YAKSys_Hybrid_CPU::Core::Registers::Retire;

using namespace YAKSys_Hybrid_CPU::Core;
using namespace HybridCPU_ISE::Arch;



using namespace HybridCPU_ISE::CloseToHSL::Memory::MMU;

namespace YAKSys_Hybrid_CPU::Execution
{
	/// <summary>
	/// Central execution engine for stream/vector instructions.
	/// Implements RVV-inspired strip-mining FSM with type-agnostic operation dispatch.
	///
	/// Architecture goals:
	/// - Single point of truth for stream instruction semantics
	/// - Type-agnostic: handles all INT/UINT/FLOAT widths (8-64 bits)
	/// - Address mode agnostic: unit-stride, strided, 2D, indexed (via descriptors)
	/// - Strip-mining: chunks large streams into VLMAX-sized hardware iterations
	/// - Burst I/O: uses AXI4-compliant BurstIO for memory transfers
	/// - Zero allocation: all buffers pre-allocated in CPU_Core scratch space
	/// - RVV semantics: predicate masks, inactive lanes don't write/trap
	/// </summary>
	class StreamEngine final
	{
	private:
		static bool IsBinaryOp(unsigned int opCode);

		inline static bool IsGenericInPlaceVectorComputeOp(unsigned int opCode);

		/// <summary>
		/// Check if operation is a comparison instruction that generates a predicate mask.
		/// </summary>
		inline static bool IsComparisonOp(unsigned int opCode);

		/// <summary>
		/// Check if operation is a mask manipulation instruction.
		/// </summary>
		inline static bool IsMaskManipOp(unsigned int opCode);

		/// <summary>
		/// Read a TriOpDesc descriptor from memory at the given address.
		/// TriOpDesc provides base addresses and strides for FMA tri-operand operations.
		/// Layout (20 bytes): SrcA(8) + SrcB(8) + StrideA(2) + StrideB(2)
		/// </summary>
		inline static TriOpDesc ReadTriOpDesc(unsigned long long descriptorAddr);

		/// <summary>
		/// Check if operation is a FMA (Fused Multiply-Add) instruction.
		/// FMA operations require 3 source operands (ternary operation).
		/// </summary>
		inline static bool IsFMAOp(unsigned int opCode);

		/// <summary>
		/// Check if operation is a reduction instruction.
		/// Reduction operations collapse a vector to a scalar result.
		/// </summary>
		inline static bool IsReductionOp(unsigned int opCode);

		/// <summary>
		/// Check if operation is a prefix scan instruction.
		/// </summary>
		inline static bool IsScanOp(unsigned int opCode);

		/// <summary>
		/// Check if operation is a dot-product instruction.
		/// Dot-product operations: VDOT_VV (signed), VDOTU_VV (unsigned), VDOTF_VV (float)
		/// </summary>
		inline static bool IsDotProductOp(unsigned int opCode);

		/// <summary>
		/// Check if operation is a predicative movement instruction.
		/// Predicative movement operations: VCOMPRESS, VEXPAND (ARM SVE-style)
		/// </summary>
		inline static bool IsPredicativeMovementOp(unsigned int opCode);

		/// <summary>
		/// Check if operation is a permutation/gather instruction.
		/// Permutation operations: VPERMUTE, VRGATHER (indexed reordering)
		/// </summary>
		inline static bool IsPermutationOp(unsigned int opCode);

		/// <summary>
		/// Check if operation is a slide instruction.
		/// Slide operations: VSLIDEUP, VSLIDEDOWN (element shifting)
		/// </summary>
		inline static bool IsSlideOp(unsigned int opCode);


		/// <summary>
		/// Execute a stream instruction.
		/// Main entry point from the explicit direct-stream compat/test seam and
		/// materialized vector micro-op execution.
		///
		/// Execution flow:
		/// 1. Extract instruction parameters (opcode, type, addresses, stride, length)
		/// 2. Strip-mine: loop over chunks of VLMAX elements
		/// 3. For each chunk:
		///    a. Read operands via BurstIO (respecting AXI4 constraints)
		///    b. Execute operation via VectorALU (with predicate masking)
		///    c. Write results via BurstIO
		/// 4. Update pointers for next chunk
		/// </summary>
		/// <param name="core">Reference to CPU core (for predicates, scratch buffers)</param>
		/// <param name="inst">Instruction structure (parsed VLIW slot)</param>
	public:
		static void Execute(std::shared_ptr<Processor::CPU_Core> &core, VLIW_Instruction const inst, int ownerThreadId = -1);

		static void Execute(std::shared_ptr<Processor::CPU_Core> &core, StreamExecutionRequest const request, int ownerThreadId = -1);

		static void CaptureRetireWindowPublications(std::shared_ptr<Processor::CPU_Core> &core, VLIW_Instruction const inst, Processor::CPU_Core::RetireWindowBatch &retireBatch, int ownerThreadId = -1);

		static void CaptureRetireWindowPublications(std::shared_ptr<Processor::CPU_Core> &core, StreamExecutionRequest const request, Processor::CPU_Core::RetireWindowBatch &retireBatch, int ownerThreadId = -1);

	private:
		inline static void CaptureRetireWindowPublicationsCore(std::shared_ptr<Processor::CPU_Core> &core, StreamExecutionRequest const request, Processor::CPU_Core::RetireWindowBatch &retireBatch, int ownerThreadId, const std::wstring &executionContour);

		/// <summary>
		/// Execute scalar operation on registers.
		/// StreamLength = 1, operands are register IDs packed in Word1.
		/// </summary>
		inline static void ExecuteScalarRegister(std::shared_ptr<Processor::CPU_Core> &core, StreamExecutionRequest const request, unsigned int opCode, DataTypeEnum dataType, unsigned char predIndex, int executionVtId);

		/// <summary>
		/// Execute vector operation on memory buffers.
		/// Implements strip-mining with burst I/O.
		/// Supports 1D, 2D, and indexed addressing modes.
		/// </summary>
		inline static void ExecuteVectorMemory(std::shared_ptr<Processor::CPU_Core> &core, StreamExecutionRequest const request, unsigned int opCode, DataTypeEnum dataType, unsigned char predIndex, int elemSize, unsigned long long streamLength, int executionVtId);

		inline static int ResolveRequiredElementSizeOrThrow(unsigned int opCode, DataTypeEnum dataType);

		inline static void ExecuteMaskManipulation(std::shared_ptr<Processor::CPU_Core> &core, StreamExecutionRequest const request, unsigned int opCode, int executionVtId);

		inline static void ThrowIfUnsupportedNonRepresentableVectorAddressing(StreamExecutionRequest const request, unsigned int opCode);

		inline static void ThrowIfUnsupportedRawTransferVectorContour(StreamExecutionRequest const request, unsigned int opCode);

		inline static void ThrowIfUnsupportedDirectRetireVectorAddressing(StreamExecutionRequest const request, unsigned int opCode, const std::wstring &executionContour);

		inline static void ThrowIfUnsupportedRawDescriptorlessFmaContour(StreamExecutionRequest const request, unsigned int opCode);

		inline static void ThrowIfUnsupportedRawWidenNarrowConvertContour(StreamExecutionRequest const request, unsigned int opCode);

		inline static void ThrowIfUnsupportedRawScanContour(StreamExecutionRequest const request, unsigned int opCode);

		inline static void ThrowIfUnsupportedRawSlideOneContour(StreamExecutionRequest const request, unsigned int opCode);

		inline static void ThrowIfUnsupportedRawWideDotContour(StreamExecutionRequest const request, unsigned int opCode);

		inline static void ThrowIfUnsupportedRawPerm2Contour(StreamExecutionRequest const request, unsigned int opCode);

		inline static void ThrowIfUnsupportedRawTransposeContour(StreamExecutionRequest const request, unsigned int opCode);

		inline static void ThrowIfUnsupportedRawSaturatingPolicyContour(StreamExecutionRequest const request, unsigned int opCode);

		inline static void ThrowIfUnavailableRawScratchContour(unsigned int opCode, const std::wstring &executeContour, bool requiresScratchB, int scratchALength, int scratchBLength, int scratchDstLength, int scratchIndexLength = -1);

		inline static void ThrowIfUnsupportedZeroRowLength2DContour(unsigned int opCode, unsigned long long streamLength, unsigned int rowLength, const std::wstring &executeContour);

		inline static std::wstring DescribeNonRepresentableVectorAddressingContour(StreamExecutionRequest const request);


		static void ExecutePredicativeMovement1D(std::shared_ptr<Processor::CPU_Core> &core, StreamExecutionRequest const request, unsigned int opCode, DataTypeEnum dataType, unsigned char predIndex, int elemSize, unsigned long long streamLength);

		static void ThrowIfUnsupportedRawPredicativeMovementBufferFootprint(unsigned int opCode, unsigned long long streamLength, int elemSize, int scratchALength, int scratchDstLength);

		static void Execute1D(std::shared_ptr<Processor::CPU_Core> &core, StreamExecutionRequest const request, unsigned int opCode, DataTypeEnum dataType, unsigned char predIndex, int elemSize, unsigned long long streamLength, int executionVtId);

		inline static void PublishScalarRetireRegisterWrite(std::shared_ptr<Processor::CPU_Core> &core, int executionVtId, unsigned short archRegId, unsigned long long value);

		inline static void PublishScalarRetireRegisterWrite(std::shared_ptr<Processor::CPU_Core> &core, RetireRecord const retireRecord);

		inline static bool TryResolveScalarRegisterRetireRecord(std::shared_ptr<Processor::CPU_Core> &core, StreamExecutionRequest const request, unsigned int opCode, unsigned char predIndex, int executionVtId, RetireRecord &retireRecord);

		inline static void CaptureScalarRegisterRetireWindowPublication(std::shared_ptr<Processor::CPU_Core> &core, StreamExecutionRequest const request, Processor::CPU_Core::RetireWindowBatch &retireBatch, unsigned int opCode, unsigned char predIndex, int executionVtId);

		inline static unsigned long long ResolveScalarRegisterResult(std::shared_ptr<Processor::CPU_Core> &core, StreamExecutionRequest const request, unsigned int opCode, int executionVtId, unsigned char sourceRegister1, unsigned char sourceRegister2);

		inline static void ResolveRequiredScalarRegisterOperandsOrThrow(StreamExecutionRequest const request, unsigned int opCode, unsigned char &destinationRegister, unsigned char &sourceRegister1, unsigned char &sourceRegister2);

		inline static void ThrowIfUnsupportedScalarRegisterStreamOpcode(unsigned int opCode);

		inline static bool IsSupportedScalarRegisterStreamOpcode(unsigned int opCode);

		inline static std::shared_ptr<InvalidOperationException> CreateUnsupportedScalarRegisterStreamOpcodeException(unsigned int opCode);

		inline static void CaptureVpopcRetireWindowPublication(std::shared_ptr<Processor::CPU_Core> &core, StreamExecutionRequest const request, Processor::CPU_Core::RetireWindowBatch &retireBatch, int executionVtId);

		inline static bool TryResolveVpopcRetireRecord(std::shared_ptr<Processor::CPU_Core> &core, StreamExecutionRequest const request, int executionVtId, RetireRecord &retireRecord);

		inline static void CapturePredicateStateRetireWindowPublication(std::shared_ptr<Processor::CPU_Core> &core, StreamExecutionRequest const request, Processor::CPU_Core::RetireWindowBatch &retireBatch, unsigned int opCode, DataTypeEnum dataType, unsigned char predIndex, int executionVtId);

		inline static unsigned long long ResolveComparisonPredicateMask(std::shared_ptr<Processor::CPU_Core> &core, StreamExecutionRequest const request, unsigned int opCode, DataTypeEnum dataType, unsigned char predIndex, int elemSize);

		inline static unsigned char ResolveComparisonDestinationPredicateRegister(StreamExecutionRequest const request);

		inline static unsigned char ResolveMaskManipulationDestinationPredicateRegister(StreamExecutionRequest const request);

		inline static unsigned long long ResolveMaskManipulationResultMask(std::shared_ptr<Processor::CPU_Core> &core, StreamExecutionRequest const request, unsigned int opCode);

		inline static unsigned long long ResolveMaskSetBeforeFirstResult(unsigned long long mask, StreamExecutionRequest const request);

		inline static void AccumulateComparisonPredicateChunk(unsigned long long &accumulatedResultMask, unsigned long long chunkResultMask, unsigned long long elementsProcessed);


		static int ResolveExecutionVtId(std::shared_ptr<Processor::CPU_Core> &core, int ownerThreadId);

		inline static unsigned long long ResolvePreferredStreamChunkShape(unsigned long long remaining, int elementSize, int scratchByteCapacity, unsigned short stride);

		inline static unsigned long long ResolvePreferredStreamChunkShape(unsigned long long remaining, int elementSize, int scratchByteCapacity, unsigned int rowLength, unsigned short rowStride, unsigned short colStride, unsigned long long elementsProcessed);

		inline static unsigned long long ResolvePreferredStreamChunkShape(unsigned long long remaining, int elementSize, int scratchByteCapacity, int indexElementSize, int scratchIndexByteCapacity, unsigned short indexStride);

		inline static void TryApplyIngressWarmPolicy(unsigned long long destPtr, unsigned long long src2Ptr, int elementSize, unsigned long long chunkElements, unsigned short stride, bool isBinary);

		inline static void TryApplyIngressWarmPolicy(unsigned long long baseAddr, int elementSize, unsigned long long chunkElements, unsigned int rowLength, unsigned short rowStride, unsigned short colStride, unsigned long long startOffset);

		inline static void TryApplyIngressWarmPolicy(unsigned long long destBase, unsigned long long indexBase, int elementSize, int indexElementSize, unsigned short indexStride, unsigned long long startElement, unsigned long long chunkElements);

		/// <summary>
		/// Execute binary vector operation with double-buffered scratch.
		/// Models overlapped load→compute→store pipeline:
		///   Cycle N  : LOAD chunk[N] → active buffer
		///   Cycle N+1: COMPUTE active | LOAD chunk[N+1] → inactive
		///   Cycle N+2: STORE active   | COMPUTE inactive | LOAD chunk[N+2] → active
		/// In single-threaded emulation the overlap is modelled by counter arithmetic:
		///   OverlappedCycles += min(loadLatency, computeLatency) per iteration.
		/// Result is bit-identical to Execute1D (correctness invariant).
		/// Only binary ops go through this path; special ops (FMA, reduction, etc.)
		/// continue to use the single-buffer Execute1D.
		/// </summary>
		inline static void Execute1D_DoubleBuffered(std::shared_ptr<Processor::CPU_Core> &core, StreamExecutionRequest const request, unsigned int opCode, DataTypeEnum dataType, unsigned char predIndex, int elemSize, unsigned long long streamLength);

		/// <summary>
		/// Execute vector operation with 2D addressing pattern.
		/// Uses BurstRead2D/Write2D for row-major access with row and column strides.
		/// </summary>
		inline static void Execute2D(std::shared_ptr<Processor::CPU_Core> &core, StreamExecutionRequest const request, unsigned int opCode, DataTypeEnum dataType, unsigned char predIndex, int elemSize, unsigned long long streamLength);

		/// <summary>
		/// Execute vector operation with indexed addressing (gather/scatter).
		/// Reads descriptor from memory to get source addresses and index array.
		/// </summary>
		inline static void ExecuteIndexed(std::shared_ptr<Processor::CPU_Core> &core, StreamExecutionRequest const request, unsigned int opCode, DataTypeEnum dataType, unsigned char predIndex, int elemSize, unsigned long long streamLength);


		static constexpr unsigned int MaxStreamRegisterBytes = 32u * 8u;

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: [MethodImpl(MethodImplOptions.AggressiveInlining)] private static MemorySubsystem? ResolveActiveMemorySubsystem(MemorySubsystem? memSub)
		inline static std::shared_ptr<MemorySubsystem> ResolveActiveMemorySubsystem(std::shared_ptr<MemorySubsystem> memSub);

	public:
		static unsigned int ResolveSrfResidentChunkBudget(int elementSize, unsigned long long requestedElementCount);

		/// <summary>
		/// Prefetch the next strip-mined chunk into the SRF using the active
		/// runtime memory backend, so the later BurstRead can truthfully bypass
		/// backend traffic instead of relying on a loading marker only.
		/// </summary>
	private:
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: [MethodImpl(MethodImplOptions.AggressiveInlining)] private static void PrefetchToStreamRegister(ulong sourceAddr, byte elemSize, uint elemCount, YAKSys_Hybrid_CPU.Memory.MemorySubsystem? memSub = null)
		inline static void PrefetchToStreamRegister(unsigned long long sourceAddr, unsigned char elemSize, unsigned int elemCount, std::shared_ptr<YAKSys_Hybrid_CPU::Memory::MemorySubsystem> memSub = nullptr);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: [MethodImpl(MethodImplOptions.AggressiveInlining)] private static bool TryPrefetchToAssistStreamRegister(ulong sourceAddr, byte elemSize, uint elemCount, YAKSys_Hybrid_CPU.Core.AssistStreamRegisterPartitionPolicy partitionPolicy, YAKSys_Hybrid_CPU.Memory.MemorySubsystem? memSub = null)
		inline static bool TryPrefetchToAssistStreamRegister(unsigned long long sourceAddr, unsigned char elemSize, unsigned int elemCount, YAKSys_Hybrid_CPU::Core::AssistStreamRegisterPartitionPolicy partitionPolicy, std::shared_ptr<YAKSys_Hybrid_CPU::Memory::MemorySubsystem> memSub = nullptr);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: [MethodImpl(MethodImplOptions.AggressiveInlining)] private static bool TryWarmNextStreamChunk(StreamRegisterFile srf, int registerIndex, ulong sourceAddr, byte elemSize, uint elemCount, bool assistOwned, YAKSys_Hybrid_CPU.Memory.MemorySubsystem? memSub = null)
		inline static bool TryWarmNextStreamChunk(std::shared_ptr<StreamRegisterFile> srf, int registerIndex, unsigned long long sourceAddr, unsigned char elemSize, unsigned int elemCount, bool assistOwned, std::shared_ptr<YAKSys_Hybrid_CPU::Memory::MemorySubsystem> memSub = nullptr);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: [MethodImpl(MethodImplOptions.AggressiveInlining)] private static void TryWarmPlanned2DChunk(ulong baseAddr, int elementSize, ulong elementCount, uint rowLength, ushort rowStride, ushort colStride, ulong startOffset, YAKSys_Hybrid_CPU.Memory.MemorySubsystem? memSub = null)
		inline static void TryWarmPlanned2DChunk(unsigned long long baseAddr, int elementSize, unsigned long long elementCount, unsigned int rowLength, unsigned short rowStride, unsigned short colStride, unsigned long long startOffset, std::shared_ptr<YAKSys_Hybrid_CPU::Memory::MemorySubsystem> memSub = nullptr);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: [MethodImpl(MethodImplOptions.AggressiveInlining)] private static void TryWarmIndexedIngressChunk(ulong destBase, ulong indexBase, int elementSize, int indexElementSize, ushort indexStride, ulong startElement, ulong elementCount, YAKSys_Hybrid_CPU.Memory.MemorySubsystem? memSub = null)
		inline static void TryWarmIndexedIngressChunk(unsigned long long destBase, unsigned long long indexBase, int elementSize, int indexElementSize, unsigned short indexStride, unsigned long long startElement, unsigned long long elementCount, std::shared_ptr<YAKSys_Hybrid_CPU::Memory::MemorySubsystem> memSub = nullptr);

		/// <summary>
		/// Lane6/DMA-backed assist seam for landed stream-register-prefetch assists.
		/// </summary>
	public:
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: [MethodImpl(MethodImplOptions.AggressiveInlining)] public static bool ScheduleLane6AssistPrefetch(ulong sourceAddr, byte elemSize, uint elemCount, YAKSys_Hybrid_CPU.Core.AssistStreamRegisterPartitionPolicy partitionPolicy, YAKSys_Hybrid_CPU.Memory.MemorySubsystem? memSub = null)
		static bool ScheduleLane6AssistPrefetch(unsigned long long sourceAddr, unsigned char elemSize, unsigned int elemCount, YAKSys_Hybrid_CPU::Core::AssistStreamRegisterPartitionPolicy partitionPolicy, std::shared_ptr<YAKSys_Hybrid_CPU::Memory::MemorySubsystem> memSub = nullptr);

		/// <summary>
		/// Compatibility alias for older narrow assist callsites.
		/// </summary>
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: [MethodImpl(MethodImplOptions.AggressiveInlining)] public static bool ScheduleAssistPrefetch(ulong sourceAddr, byte elemSize, uint elemCount, YAKSys_Hybrid_CPU.Core.AssistStreamRegisterPartitionPolicy partitionPolicy, YAKSys_Hybrid_CPU.Memory.MemorySubsystem? memSub = null)
		inline static bool ScheduleAssistPrefetch(unsigned long long sourceAddr, unsigned char elemSize, unsigned int elemCount, YAKSys_Hybrid_CPU::Core::AssistStreamRegisterPartitionPolicy partitionPolicy, std::shared_ptr<YAKSys_Hybrid_CPU::Memory::MemorySubsystem> memSub = nullptr);
	};
}
