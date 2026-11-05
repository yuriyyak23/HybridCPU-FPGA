#pragma once

#include "../../Core/Architecture/Common/YAKSys_Hybrid_CPU.Processor.h"
#include "DmaChannelId.h"
#include <span>
#include <tuple>
namespace YAKSys_Hybrid_CPU::Core::Memory { class MemoryCoherencyObserver; }
#include <vector>
#include <cmath>
#include <functional>
#include <optional>
#include <memory>
#include "../../../tangible_event.h"

using namespace YAKSys_Hybrid_CPU;
using namespace YAKSys_Hybrid_CPU::Core::Memory;
using namespace YAKSys_Hybrid_CPU::Memory;

namespace HybridCPU_ISE::CloseToHSL::Memory::DMA
{
	/// <summary>
	/// Standalone DMA Controller for HybridCPU.
	/// Models persistent memory-to-memory channel transfers. Production
	/// progress is supplied only by the bound MemoryCycleController edge.
	///
	/// Features:
	/// - Separate channel state from CPU_Core execution
	/// - AXI4-compliant burst transfers
	/// - Scatter/gather support via descriptor chains
	/// - Multi-channel support (up to 8 channels)
	/// - Integration with IOMMU for address translation
	/// - Hardware prefetch for stream operations
	/// - Priority-based channel arbitration
	/// - Interrupt signaling on completion
	/// - Completion events and callbacks when ExecuteCycle-driven work finishes
	///
	/// Design goals:
	/// - Allow multiple StreamEngine instances to run concurrently
	/// - Reduce CPU_Core memory bandwidth contention
	/// - Provide a base for future overlapped computation and I/O decisions
	///
	/// StreamEngine BurstIO remains a functional adapter and does not submit or
	/// advance this agent. This class-level API should not be read as a
	/// DmaStreamCompute request protocol or a complete overlap model.
	/// </summary>
	class DMAController : public std::enable_shared_from_this<DMAController>
	{
		/// <summary>
		/// Event arguments for DMA transfer completion
		/// </summary>
	public:
		class TransferCompletedEventArgs : public EventArgs
		{
		private:
			unsigned char ChannelID = 0;
			bool IsError = false;
			unsigned long long ErrorCode = 0;
			unsigned int BytesTransferred = 0;

		public:
			unsigned char getChannelID() const;
			void setChannelID(unsigned char value);
			bool isError() const;
			void setError(bool value);
			unsigned long long getErrorCode() const;
			void setErrorCode(unsigned long long value);
			unsigned int getBytesTransferred() const;
			void setBytesTransferred(unsigned int value);

		protected:
			std::shared_ptr<TransferCompletedEventArgs> shared_from_this()
			{
				return std::static_pointer_cast<TransferCompletedEventArgs>(EventArgs::shared_from_this());
			}
		};

		/// <summary>
		/// Callback delegate for async transfer completion
		/// </summary>
		using TransferCompletionCallback = std::function<void (unsigned char channelID, bool success, unsigned long long errorCode)>;

		/// <summary>
		/// Event fired when a DMA transfer completes (success or error)
		/// </summary>
	public:
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public event std::function<void(void*, std::shared_ptr<TransferCompletedEventArgs>)>? TransferCompleted;
		std::shared_ptr<TangibleEvent<std::function<void(void*, std::shared_ptr<TransferCompletedEventArgs>)>>> TransferCompleted = std::make_shared<TangibleEvent<std::function<void(void*, std::shared_ptr<TransferCompletedEventArgs>)>>>();
		/// <summary>
		/// Maximum number of DMA channels supported
		/// </summary>
	private:
		static constexpr int MAX_CHANNELS = 8;

		/// <summary>
		/// Maximum burst length (AXI4 compliance: 256 beats)
		/// </summary>
		static constexpr int MAX_BURST_LENGTH = 256;

		/// <summary>
		/// AXI4 4KB boundary alignment requirement
		/// </summary>
		static constexpr int BOUNDARY_4KB = 4096;

		/// <summary>
		/// DMA channel state
		/// </summary>
	public:
		enum class ChannelState : unsigned char
		{
			Idle = 0, // Channel is idle, ready for new transfer
			Configured = 1, // Transfer configured, not started
			Active = 2, // Transfer in progress
			Paused = 3, // Transfer paused (can be resumed)
			Completed = 4, // Transfer completed successfully
			Error = 5 // Transfer error occurred
		};

		/// <summary>
		/// DMA transfer descriptor
		/// Describes a single memory-to-memory transfer operation
		/// </summary>
	public:
		class TransferDescriptor
		{
		public:
			unsigned long long SourceAddress = 0; // Physical source address
			unsigned long long DestAddress = 0; // Physical destination address
			unsigned int TransferSize = 0; // Number of bytes to transfer
			unsigned short SourceStride = 0; // Stride for source (0 = packed)
			unsigned short DestStride = 0; // Stride for destination (0 = packed)
			unsigned char ElementSize = 0; // Size of each element in bytes (1, 2, 4, 8)
			bool UseIOMMU = false; // Whether to use IOMMU translation
			unsigned long long NextDescriptor = 0; // Address of next descriptor (0 = none)
			unsigned char ChannelID = 0; // DMA channel ID (0-7)
			unsigned char Priority = 0; // Transfer priority (0 = lowest, 255 = highest)
		};

		/// <summary>
		/// DMA channel control block
		/// </summary>
	private:
		class ChannelControl
		{
		public:
			ChannelState State = static_cast<ChannelState>(0); // Current channel state
			TransferDescriptor CurrentDesc; // Current transfer descriptor
			unsigned int BytesTransferred = 0; // Bytes transferred so far
			unsigned int TotalBytes = 0; // Total bytes for this transfer
			unsigned long long ErrorCode = 0; // Error code if State == Error
			std::optional<TransferCompletionCallback> Callback; // Optional completion callback
		};

		// DMA channel state (one per channel)
	private:
		std::vector<ChannelControl> channels;

		// Reference to processor memory
		Processor processor;

		/// <summary>
		/// Captured interrupt-dispatch surface. When non-null, interrupt delivery
		/// routes through this delegate instead of the global
		/// <see cref="Processor.InterruptData.CallInterrupt"/>.
		/// Signature: (DeviceType deviceId, ushort interruptId, ulong coreId).
		/// </summary>
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private readonly System.Action<Processor.DeviceType, ushort, ulong>? _interruptDispatch;
		const std::function<void(Processor::DeviceType, unsigned short, unsigned long long)> _interruptDispatch;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private readonly MemoryCoherencyObserver? _coherencyObserver;
		const std::shared_ptr<MemoryCoherencyObserver> _coherencyObserver;

		// Performance counters
		unsigned long long totalBytesTransferred = 0;
		unsigned long long totalTransfers = 0;
		unsigned long long totalErrors = 0;

		/// <summary>
		/// Initialize DMA controller.
		/// </summary>
		/// <param name="proc">Reference to processor for memory access.</param>
		/// <param name="interruptDispatch">Optional interrupt-dispatch delegate.
		/// When null, falls back to the global <see cref="Processor.InterruptData.CallInterrupt"/>.</param>
	public:
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public DMAController(ref Processor proc, System.Action<Processor.DeviceType, ushort, ulong>? interruptDispatch = null, System.Nullable<MemoryCoherencyObserver> coherencyObserver = null)
		DMAController(Processor &proc, std::function<void(Processor::DeviceType, unsigned short, unsigned long long)> interruptDispatch = nullptr, std::shared_ptr<MemoryCoherencyObserver> coherencyObserver = nullptr);

		/// <summary>
		/// Configure a DMA transfer
		/// </summary>
		/// <param name="desc">Transfer descriptor</param>
		/// <param name="callback">Optional callback to invoke on completion</param>
		/// <returns>True if channel configured successfully, false if channel busy</returns>
		inline bool ConfigureTransfer(TransferDescriptor desc, const std::optional<TransferCompletionCallback> &callback = std::nullopt);

		/// <summary>
		/// Start a configured DMA transfer
		/// </summary>
		/// <param name="channelID">Channel ID to start</param>
		/// <returns>True if started successfully</returns>
		inline bool StartTransfer(unsigned char channelID);

		/// <summary>
		/// Execute one DMA cycle (processes active channels)
		/// Call this periodically to advance DMA transfers
		/// Uses priority-based arbitration: highest priority active channel is processed first
		/// </summary>
		void ExecuteCycle();

		/// <summary>
		/// Process a single DMA channel transfer
		/// </summary>
	private:
		inline void ProcessChannel(DmaChannelId channel);

		/// <summary>
		/// Complete a DMA channel transfer
		/// </summary>
		inline void CompleteChannel(DmaChannelId channel);

		/// <summary>
		/// Raise an interrupt for DMA channel completion or error.
		/// Routes through the captured interrupt-dispatch delegate when available;
		/// falls back to the global <see cref="Processor.InterruptData.CallInterrupt"/>.
		/// </summary>
		inline void RaiseInterrupt(DmaChannelId channel, bool isError);

		/// <summary>
		/// Perform memory-to-memory burst transfer
		/// </summary>
		inline bool PerformBurst(unsigned long long srcAddr, unsigned long long dstAddr, unsigned int byteCount, bool useIOMMU);

		/// <summary>
		/// Calculate bytes remaining until 4KB boundary
		/// </summary>
		inline unsigned int BytesTo4KBBoundary(unsigned long long address);

		inline static std::shared_ptr<Processor::MainMemoryArea> CaptureCurrentMainMemory();

		inline static bool HasExactMainMemoryRange(std::shared_ptr<Processor::MainMemoryArea> mainMemory, unsigned long long address, int size);

		inline static bool TryReadDirectMainMemoryExact(std::shared_ptr<Processor::MainMemoryArea> mainMemory, unsigned long long address, std::span<unsigned char> buffer);

		inline static bool TryWriteDirectMainMemoryExact(std::shared_ptr<Processor::MainMemoryArea> mainMemory, unsigned long long address, std::span<const unsigned char> buffer);

		/// <summary>
		/// Get channel state
		/// </summary>
	public:
		ChannelState GetChannelState(unsigned char channelID);

		/// <summary>
		/// Get channel progress
		/// </summary>
// C# TO C++ CONVERTER TASK: Methods returning tuples are not converted by C# to C++ Converter:
//		public (uint transferred, uint total) GetChannelProgress(byte channelID)
//		{
//			if (channelID >= MAX_CHANNELS)
//				return (0, 0);
//
//			DmaChannelId channel = DmaChannelId.Create(channelID);
//			return (channels[channel].BytesTransferred, channels[channel].TotalBytes);
//		}

		/// <summary>
		/// Reset a channel to idle state
		/// </summary>
		void ResetChannel(unsigned char channelID);

		/// <summary>
		/// Get performance statistics
		/// </summary>
		std::tuple<unsigned long long, unsigned long long, unsigned long long> GetStatistics() const
		{
			return {totalBytesTransferred, totalTransfers, totalErrors};
		}

		/// <summary>
		/// Get channel descriptor information (source/destination addresses)
		/// </summary>
// C# TO C++ CONVERTER TASK: Methods returning tuples are not converted by C# to C++ Converter:
//		public (ulong srcAddr, ulong dstAddr) GetChannelAddresses(byte channelID)
//		{
//			if (channelID >= MAX_CHANNELS)
//				return (0, 0);
//
//			DmaChannelId channel = DmaChannelId.Create(channelID);
//			return (channels[channel].CurrentDesc.SourceAddress, channels[channel].CurrentDesc.DestAddress);
//		}

		/// <summary>
		/// Pause a running DMA transfer
		/// </summary>
		bool PauseTransfer(unsigned char channelID);

		/// <summary>
		/// Resume a paused DMA transfer
		/// </summary>
		bool ResumeTransfer(unsigned char channelID);

		/// <summary>
		/// Cancel a DMA transfer
		/// </summary>
		bool CancelTransfer(unsigned char channelID);

		/// <summary>
		/// Fire TransferCompleted event
		/// </summary>
	private:
		void OnTransferCompleted(unsigned char channelID, bool isError, unsigned long long errorCode, unsigned int bytesTransferred);
	};
}
