#pragma once

#include "../../BurstIO/IBurstBackend.h"
#include "../../BurstIO/IOMMUBurstBackend.h"
#include "../../../Architecture/Common/YAKSys_Hybrid_CPU.Processor.h"
namespace YAKSys_Hybrid_CPU::Memory { class MemorySubsystem; class MemoryRequestToken; }
#include "../../../../Memory/Registers/YAKSys_Hybrid_CPU.Memory.StreamRegisterFile.h"
#include <string>
#include <vector>
#include <span>
#include <limits>
#include <format>
#include <memory>

using namespace YAKSys_Hybrid_CPU::Memory;

namespace YAKSys_Hybrid_CPU::Execution
{
	/// <summary>
	/// Burst I/O operations for stream engine.
	/// Implements AXI4-compliant burst rules:
	/// - Maximum 256 beats (transfers) per burst
	/// - Cannot cross 4KB boundaries
	/// - No early termination
	///
	/// Design philosophy (ref2.md refactoring):
	/// - Separation of concerns: BurstPlanner (math) vs. IBurstBackend (I/O)
	/// - Backend abstraction enables testing, DMA offloading, simulation
	/// - Deterministic: same input → same burst pattern
	/// - Hardware-oriented: models real FPGA memory controller
	///
	/// Persistent DMA progress is owned by the bound memory controller. These
	/// synchronous span helpers remain functional adapters and create no time.
	///
	/// HLS Note: Each burst operation takes MEMORY_LATENCY cycles (4 cycles for L1 cache hit).
	/// Actual hardware latency depends on:
	/// - Cache hit/miss (L1: 4 cycles, L2: 12 cycles, DRAM: 100+ cycles)
	/// - Burst length (longer bursts have higher latency)
	/// - Memory controller congestion
	/// </summary>
	class BurstIO final
	{
	private:
		static constexpr unsigned long long CPU_DEVICE_ID = 0;
		static constexpr int AXI4_MAX_BURST_LENGTH = 256;
		static constexpr unsigned long long AXI4_4KB_BOUNDARY = 4096;

		/// <summary>
		/// DMA threshold: transfers larger than this use DMA Controller (ref2.md)
		/// Default: 4KB (one page). Transfers smaller than this use direct IOMMU path.
		/// </summary>

		/// <summary>
		/// Backend for memory operations.
		/// Defaults to IOMMU backend for virtual address translation.
		/// Can be swapped for testing or alternative implementations (DMA, simulation, etc.)
		///
		/// Updated (ref3.md - PerfModel): Now uses MemorySubsystem if available,
		/// otherwise falls back to IOMMUBurstBackend for backwards compatibility.
		/// </summary>
		static inline std::shared_ptr<IBurstBackend> _backend = std::make_shared<IOMMUBurstBackend>();

		static std::shared_ptr<Processor::MainMemoryArea> CaptureCurrentMainMemory();

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: [MethodImpl(MethodImplOptions.AggressiveInlining)] private static YAKSys_Hybrid_CPU.Memory.MemorySubsystem? ResolveActiveMemorySubsystem(YAKSys_Hybrid_CPU.Memory.MemorySubsystem? memSub)
		static std::shared_ptr<YAKSys_Hybrid_CPU::Memory::MemorySubsystem> ResolveActiveMemorySubsystem(std::shared_ptr<YAKSys_Hybrid_CPU::Memory::MemorySubsystem> memSub);

		/// <summary>
		/// Get the active backend (MemorySubsystem if available, otherwise fallback)
		/// </summary>
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: [MethodImpl(MethodImplOptions.AggressiveInlining)] private static IBurstBackend GetBackend(YAKSys_Hybrid_CPU.Memory.MemorySubsystem? memSub)
		static std::shared_ptr<IBurstBackend> GetBackend(std::shared_ptr<YAKSys_Hybrid_CPU::Memory::MemorySubsystem> memSub);

		static bool HasExactBoundMainMemoryRange(std::shared_ptr<Processor::MainMemoryArea> mainMemory, unsigned long long address, int size);

		static bool TryReadBoundMainMemoryExact(std::shared_ptr<Processor::MainMemoryArea> mainMemory, unsigned long long address, std::span<unsigned char> buffer);

		static bool TryWriteBoundMainMemoryExact(std::shared_ptr<Processor::MainMemoryArea> mainMemory, unsigned long long address, std::span<const unsigned char> buffer);

	public:
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: [MethodImpl(MethodImplOptions.AggressiveInlining)] internal static bool TryReadThroughActiveBackend(ulong address, Span<byte> buffer, YAKSys_Hybrid_CPU.Memory.MemorySubsystem? memSub = null)
		static bool TryReadThroughActiveBackend(unsigned long long address, std::span<unsigned char> buffer, std::shared_ptr<YAKSys_Hybrid_CPU::Memory::MemorySubsystem> memSub = nullptr);

	private:
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: [MethodImpl(MethodImplOptions.AggressiveInlining)] private static bool TryConsumePrefetchedSrfChunk(ulong baseAddr, Span<byte> buffer, ulong elementCount, int elementSize, ushort stride, YAKSys_Hybrid_CPU.Memory.StreamRegisterFile? srf)
		static bool TryConsumePrefetchedSrfChunk(unsigned long long baseAddr, std::span<unsigned char> buffer, unsigned long long elementCount, int elementSize, unsigned short stride, std::shared_ptr<YAKSys_Hybrid_CPU::Memory::StreamRegisterFile> srf);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: [MethodImpl(MethodImplOptions.AggressiveInlining)] private static void InvalidatePrefetchedSrfWindow(ulong address, ulong byteCount, YAKSys_Hybrid_CPU.Memory.StreamRegisterFile? srf)
		static void InvalidatePrefetchedSrfWindow(unsigned long long address, unsigned long long byteCount, std::shared_ptr<YAKSys_Hybrid_CPU::Memory::StreamRegisterFile> srf);

		static void ThrowIfUnsupportedZeroRowLength2DTransferContour(const std::wstring &operationName, unsigned long long elementCount, unsigned int rowLength);

		static void ThrowIfIncompleteTransfer(const std::wstring &operationName, unsigned long long baseAddr, unsigned long long requestedElements, unsigned long long completedElements, int elementSize);

		/// <summary>
		/// Read elements from memory using AXI4-compliant bursts.
		/// Automatically splits at 4KB boundaries and respects burst length limits.
		/// Large transfers use the same functional backend path.
		///
		/// Refactored (ref2.md): Uses BurstPlanner for page-split logic and IBurstBackend for I/O.
		/// </summary>
		/// <param name="baseAddr">Base IO virtual address</param>
		/// <param name="buffer">Destination buffer (must be pre-allocated)</param>
		/// <param name="elementCount">Number of elements to read</param>
		/// <param name="elementSize">Size of each element in bytes</param>
		/// <param name="stride">Byte stride between elements (0 = packed/contiguous)</param>
		/// <returns>Number of elements successfully read</returns>
	public:
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: [MethodImpl(MethodImplOptions.AggressiveInlining)] public static ulong BurstRead(ulong baseAddr, Span<byte> buffer, ulong elementCount, int elementSize, ushort stride, YAKSys_Hybrid_CPU.Memory.MemorySubsystem? memSub = null)
		static unsigned long long BurstRead(unsigned long long baseAddr, std::span<unsigned char> buffer, unsigned long long elementCount, int elementSize, unsigned short stride, std::shared_ptr<YAKSys_Hybrid_CPU::Memory::MemorySubsystem> memSub = nullptr);

		/// <summary>
		/// Write elements to memory using AXI4-compliant bursts.
		/// Automatically splits at 4KB boundaries and respects burst length limits.
		/// Large transfers use the same functional backend path; persistent DMA
		/// progresses only from the bound memory-controller edge.
		///
		/// Refactored (ref2.md): Uses BurstPlanner for page-split logic and IBurstBackend for I/O.
		/// </summary>
		/// <param name="baseAddr">Base IO virtual address</param>
		/// <param name="buffer">Source buffer</param>
		/// <param name="elementCount">Number of elements to write</param>
		/// <param name="elementSize">Size of each element in bytes</param>
		/// <param name="stride">Byte stride between elements (0 = packed/contiguous)</param>
		/// <returns>Number of elements successfully written</returns>
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: [MethodImpl(MethodImplOptions.AggressiveInlining)] public static ulong BurstWrite(ulong baseAddr, ReadOnlySpan<byte> buffer, ulong elementCount, int elementSize, ushort stride, YAKSys_Hybrid_CPU.Memory.MemorySubsystem? memSub = null)
		static unsigned long long BurstWrite(unsigned long long baseAddr, std::span<const unsigned char> buffer, unsigned long long elementCount, int elementSize, unsigned short stride, std::shared_ptr<YAKSys_Hybrid_CPU::Memory::MemorySubsystem> memSub = nullptr);

		/// <summary>
		/// Read elements with 2D addressing pattern.
		/// Optimized: Uses BurstPlanner.Plan2D for efficient row-based burst planning.
		///
		/// Optimization (per problem statement):
		/// - For contiguous rows (colStride == elementSize), uses burst planning per row
		/// - Reduces number of backend calls from O(rows*cols) to O(rows*segments_per_row)
		/// - Fully contiguous 2D regions treated as single 1D burst
		///
		/// Refactored (ref2.md): Uses IBurstBackend for I/O operations.
		/// </summary>
		/// <param name="baseAddr">Base IO virtual address</param>
		/// <param name="buffer">Destination buffer</param>
		/// <param name="elementCount">Total number of elements (rows * cols)</param>
		/// <param name="elementSize">Size of each element in bytes</param>
		/// <param name="rowLength">Number of elements per row</param>
		/// <param name="rowStride">Byte pitch between rows</param>
		/// <param name="colStride">Byte stride between columns</param>
		/// <param name="startOffset">Starting linear element index (for strip-mining)</param>
		/// <returns>Number of elements successfully read</returns>
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: [MethodImpl(MethodImplOptions.AggressiveInlining)] public static ulong BurstRead2D(ulong baseAddr, Span<byte> buffer, ulong elementCount, int elementSize, uint rowLength, ushort rowStride, ushort colStride, ulong startOffset = 0, YAKSys_Hybrid_CPU.Memory.MemorySubsystem? memSub = null)
		static unsigned long long BurstRead2D(unsigned long long baseAddr, std::span<unsigned char> buffer, unsigned long long elementCount, int elementSize, unsigned int rowLength, unsigned short rowStride, unsigned short colStride, unsigned long long startOffset = 0, std::shared_ptr<YAKSys_Hybrid_CPU::Memory::MemorySubsystem> memSub = nullptr);

		/// <summary>
		/// Write elements with 2D addressing pattern.
		/// Optimized: Uses BurstPlanner.Plan2D for efficient row-based burst planning.
		///
		/// Optimization (per problem statement):
		/// - For contiguous rows (colStride == elementSize), uses burst planning per row
		/// - Reduces number of backend calls from O(rows*cols) to O(rows*segments_per_row)
		/// - Fully contiguous 2D regions treated as single 1D burst
		///
		/// Refactored (ref2.md): Uses IBurstBackend for I/O operations.
		/// </summary>
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: [MethodImpl(MethodImplOptions.AggressiveInlining)] public static ulong BurstWrite2D(ulong baseAddr, ReadOnlySpan<byte> buffer, ulong elementCount, int elementSize, uint rowLength, ushort rowStride, ushort colStride, ulong startOffset = 0, YAKSys_Hybrid_CPU.Memory.MemorySubsystem? memSub = null)
		static unsigned long long BurstWrite2D(unsigned long long baseAddr, std::span<const unsigned char> buffer, unsigned long long elementCount, int elementSize, unsigned int rowLength, unsigned short rowStride, unsigned short colStride, unsigned long long startOffset = 0, std::shared_ptr<YAKSys_Hybrid_CPU::Memory::MemorySubsystem> memSub = nullptr);

		/// <summary>
		/// Gather elements from memory using index array (hardware-optimized scatter-gather).
		/// Reads elements from non-contiguous addresses specified by indices.
		///
		/// Optimization (per problem statement):
		/// - For large index arrays with contiguous patterns, uses BurstPlanner.PlanIndexed
		/// - Groups contiguous accesses into bursts, reducing backend calls
		/// - Falls back to element-by-element for small or highly fragmented accesses
		///
		/// Refactored (ref2.md): Uses IBurstBackend for I/O operations.
		/// </summary>
		/// <param name="baseAddr">Base address of source array</param>
		/// <param name="buffer">Destination buffer</param>
		/// <param name="elementCount">Number of elements to gather</param>
		/// <param name="elementSize">Size of each element in bytes</param>
		/// <param name="indices">Buffer containing indices</param>
		/// <param name="indexSize">Size of each index (4 or 8 bytes)</param>
		/// <param name="indexIsByteOffset">True if indices are byte offsets, false if element indices</param>
		/// <returns>Number of elements successfully gathered</returns>
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: [MethodImpl(MethodImplOptions.AggressiveInlining)] public static ulong BurstGather(ulong baseAddr, Span<byte> buffer, ulong elementCount, int elementSize, ReadOnlySpan<byte> indices, int indexSize, byte indexIsByteOffset, YAKSys_Hybrid_CPU.Memory.MemorySubsystem? memSub = null)
		static unsigned long long BurstGather(unsigned long long baseAddr, std::span<unsigned char> buffer, unsigned long long elementCount, int elementSize, std::span<const unsigned char> indices, int indexSize, unsigned char indexIsByteOffset, std::shared_ptr<YAKSys_Hybrid_CPU::Memory::MemorySubsystem> memSub = nullptr);

		/// <summary>
		/// Scatter elements to memory using index array (hardware-optimized scatter-gather).
		/// Writes elements to non-contiguous addresses specified by indices.
		///
		/// Optimization (per problem statement):
		/// - For large index arrays with contiguous patterns, uses BurstPlanner.PlanIndexed
		/// - Groups contiguous accesses into bursts, reducing backend calls
		/// - Falls back to element-by-element for small or highly fragmented accesses
		///
		/// Refactored (ref2.md): Uses IBurstBackend for I/O operations.
		/// </summary>
		/// <param name="baseAddr">Base address of destination array</param>
		/// <param name="buffer">Source buffer</param>
		/// <param name="elementCount">Number of elements to scatter</param>
		/// <param name="elementSize">Size of each element in bytes</param>
		/// <param name="indices">Buffer containing indices</param>
		/// <param name="indexSize">Size of each index (4 or 8 bytes)</param>
		/// <param name="indexIsByteOffset">True if indices are byte offsets, false if element indices</param>
		/// <returns>Number of elements successfully scattered</returns>
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: [MethodImpl(MethodImplOptions.AggressiveInlining)] public static ulong BurstScatter(ulong baseAddr, ReadOnlySpan<byte> buffer, ulong elementCount, int elementSize, ReadOnlySpan<byte> indices, int indexSize, byte indexIsByteOffset, YAKSys_Hybrid_CPU.Memory.MemorySubsystem? memSub = null)
		static unsigned long long BurstScatter(unsigned long long baseAddr, std::span<const unsigned char> buffer, unsigned long long elementCount, int elementSize, std::span<const unsigned char> indices, int indexSize, unsigned char indexIsByteOffset, std::shared_ptr<YAKSys_Hybrid_CPU::Memory::MemorySubsystem> memSub = nullptr);
	};
}
