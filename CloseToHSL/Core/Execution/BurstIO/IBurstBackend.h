#pragma once

#include <memory>
#include <span>

namespace YAKSys_Hybrid_CPU::Execution
{
	/// <summary>
	/// DMA capabilities for custom accelerator (Phase 4)
	/// </summary>
	class AcceleratorDMACapabilities
	{
	public:
		int MaxBurstLength = 0; // Maximum burst size
		bool Supports2D = false; // Can do 2D transfers
		bool SupportsScatter = false; // Can do scatter-gather
		int BandwidthMBps = 0; // Peak bandwidth
	};

	/// <summary>
	/// Token representing a DMA transfer operation (Phase 4)
	/// </summary>
	class DMATransferToken : public std::enable_shared_from_this<DMATransferToken>
	{
	private:
		unsigned long long DeviceId = 0;
		unsigned long long SourceAddress = 0;
		unsigned long long DestinationAddress = 0;
		int Size = 0;
		bool IsComplete = false;
		int CyclesRemaining = 0;

	public:
		unsigned long long getDeviceId() const;
		void setDeviceId(unsigned long long value);
		unsigned long long getSourceAddress() const;
		void setSourceAddress(unsigned long long value);
		unsigned long long getDestinationAddress() const;
		void setDestinationAddress(unsigned long long value);
		int getSize() const;
		void setSize(int value);
		bool isComplete() const;
		void setComplete(bool value);
		int getCyclesRemaining() const;
		void setCyclesRemaining(int value);
	};

	/// <summary>
	/// Backend abstraction for burst memory operations.
	///
	/// Purpose:
	/// - Decouple burst planning (BurstPlanner) from memory access mechanism
	/// - Enable multiple backend implementations (IOMMU, simulation, testing)
	/// - Simplify testing: mock backends for unit tests
	///
	/// Implementations:
	/// - IOMMUBurstBackend: Uses IOMMU for virtual address translation
	/// - SimulatedBackend: In-memory simulation for testing (future)
	///
	/// Design philosophy:
	/// - Minimal interface: only Read/Write, no configuration
	/// - Span-based: zero-copy semantics
	/// - Boolean return: success/failure (simple error handling)
	/// </summary>
	class IBurstBackend
	{
		/// <summary>
		/// Read burst of bytes from memory.
		/// </summary>
		/// <param name="deviceID">Device ID for IOMMU translation (0 for CPU)</param>
		/// <param name="address">Virtual address to read from</param>
		/// <param name="buffer">Destination buffer (caller-allocated)</param>
		/// <returns>True if read succeeded, false on error</returns>
	public:
		virtual bool Read(unsigned long long deviceID, unsigned long long address, std::span<unsigned char> buffer) = 0;

		/// <summary>
		/// Write burst of bytes to memory.
		/// </summary>
		/// <param name="deviceID">Device ID for IOMMU translation (0 for CPU)</param>
		/// <param name="address">Virtual address to write to</param>
		/// <param name="buffer">Source buffer</param>
		/// <returns>True if write succeeded, false on error</returns>
		virtual bool Write(unsigned long long deviceID, unsigned long long address, std::span<const unsigned char> buffer) = 0;

		// ========== Phase 4: Accelerator DMA Support ==========

		/// <summary>
		/// Register accelerator as DMA-capable device (Phase 4)
		/// </summary>
		/// <param name="deviceId">Unique device ID for accelerator</param>
		/// <param name="capabilities">DMA capabilities</param>
		virtual void RegisterAcceleratorDevice(unsigned long long deviceId, AcceleratorDMACapabilities capabilities) = 0;

		/// <summary>
		/// Initiate DMA transfer for accelerator (Phase 4)
		/// </summary>
		/// <param name="deviceId">Accelerator device ID</param>
		/// <param name="srcAddr">Source address</param>
		/// <param name="dstAddr">Destination address</param>
		/// <param name="size">Transfer size</param>
		/// <returns>DMA transfer token</returns>
		virtual std::shared_ptr<DMATransferToken> InitiateAcceleratorDMA(unsigned long long deviceId, unsigned long long srcAddr, unsigned long long dstAddr, int size) = 0;
	};
}
