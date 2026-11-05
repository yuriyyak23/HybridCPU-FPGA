#pragma once

#include "IBurstBackend.h"
#include <memory>

namespace YAKSys_Hybrid_CPU::Execution
{
	/// <summary>
	/// IOMMU-based burst backend implementation.
	///
	/// Purpose:
	/// - Provides memory access through IOMMU virtual address translation
	/// - Simplest backend: direct delegation to IOMMU.ReadBurst/WriteBurst
	/// - Used for small transfers and default memory access path
	///
	/// Characteristics:
	/// - No state (stateless, thread-safe by design)
	/// - Minimal overhead: thin wrapper around IOMMU
	/// - Synchronous: operations block until completion
	///
	/// Alternative backends (future):
	/// - CachingBackend: Adds L2/L3 cache simulation
	/// - TracingBackend: Logs all memory operations for debugging
	/// </summary>
	class IOMMUBurstBackend final : public IBurstBackend
	{
		/// <summary>
		/// Read burst through IOMMU.
		/// Delegates directly to Memory.IOMMU.ReadBurst with virtual address translation.
		/// </summary>
	public:
		bool Read(unsigned long long deviceID, unsigned long long address, std::span<unsigned char> buffer) override;

		/// <summary>
		/// Write burst through IOMMU.
		/// Delegates directly to Memory.IOMMU.WriteBurst with virtual address translation.
		/// </summary>
		bool Write(unsigned long long deviceID, unsigned long long address, std::span<const unsigned char> buffer) override;

		// Phase 4: Accelerator DMA support remains fail-closed until a truthful runtime exists.

		/// <summary>
		/// Register accelerator device.
		/// </summary>
		void RegisterAcceleratorDevice(unsigned long long deviceId, AcceleratorDMACapabilities capabilities) override;

		/// <summary>
		/// Initiate accelerator DMA transfer.
		/// </summary>
		std::shared_ptr<DMATransferToken> InitiateAcceleratorDMA(unsigned long long deviceId, unsigned long long srcAddr, unsigned long long dstAddr, int size) override;
	};
}
