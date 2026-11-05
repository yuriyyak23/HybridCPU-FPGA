#include "DMAController.h"
#include "../../Core/Memory/MemoryCoherencyObserver.h"
#include "../../Core/Virtualization/Compatibility/Adapters/IO/YAKSys_Hybrid_CPU.Memory.IOMMU.h"

using namespace YAKSys_Hybrid_CPU;
using namespace YAKSys_Hybrid_CPU::Core::Memory;
using namespace YAKSys_Hybrid_CPU::Memory;

namespace HybridCPU_ISE::CloseToHSL::Memory::DMA
{

	unsigned char DMAController::TransferCompletedEventArgs::getChannelID() const
	{
		return ChannelID;
	}

	void DMAController::TransferCompletedEventArgs::setChannelID(unsigned char value)
	{
		ChannelID = value;
	}

	bool DMAController::TransferCompletedEventArgs::isError() const
	{
		return IsError;
	}

	void DMAController::TransferCompletedEventArgs::setError(bool value)
	{
		IsError = value;
	}

	unsigned long long DMAController::TransferCompletedEventArgs::getErrorCode() const
	{
		return ErrorCode;
	}

	void DMAController::TransferCompletedEventArgs::setErrorCode(unsigned long long value)
	{
		ErrorCode = value;
	}

	unsigned int DMAController::TransferCompletedEventArgs::getBytesTransferred() const
	{
		return BytesTransferred;
	}

	void DMAController::TransferCompletedEventArgs::setBytesTransferred(unsigned int value)
	{
		BytesTransferred = value;
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public DMAController(ref Processor proc, System.Action<Processor.DeviceType, ushort, ulong>? interruptDispatch = null, System.Nullable<MemoryCoherencyObserver> coherencyObserver = null)
	DMAController::DMAController(Processor &proc, std::function<void(Processor::DeviceType, unsigned short, unsigned long long)> interruptDispatch, std::shared_ptr<MemoryCoherencyObserver> coherencyObserver) : _interruptDispatch(interruptDispatch), _coherencyObserver(coherencyObserver)
	{
		this->processor = proc;
		this->channels = std::vector<ChannelControl>(MAX_CHANNELS);

		// Initialize all channels to idle state
		for (int i = 0; i < MAX_CHANNELS; i++)
		{
			DmaChannelId channel = DmaChannelId::Create(static_cast<unsigned char>(i));
			channels[channel].State = ChannelState::Idle;
			channels[channel].BytesTransferred = 0;
			channels[channel].TotalBytes = 0;
			channels[channel].ErrorCode = 0;
		}

		totalBytesTransferred = 0;
		totalTransfers = 0;
		totalErrors = 0;
	}

	inline bool DMAController::ConfigureTransfer(TransferDescriptor desc, const std::optional<TransferCompletionCallback> &callback)
	{
		// Validate channel ID
		if (desc.ChannelID >= MAX_CHANNELS)
		{
			return false;
		}

		DmaChannelId channel = DmaChannelId::Create(desc.ChannelID);

		// Check if channel is available
		if (channels[channel].State != ChannelState::Idle && channels[channel].State != ChannelState::Completed)
		{
			return false; // Channel busy
		}

		// Validate transfer parameters
		if (desc.TransferSize == 0 || desc.ElementSize == 0)
		{
			return false;
		}

		// Configure channel
		channels[channel].CurrentDesc = desc;
		channels[channel].BytesTransferred = 0;
		channels[channel].TotalBytes = desc.TransferSize;
		channels[channel].State = ChannelState::Configured;
		channels[channel].ErrorCode = 0;
		channels[channel].Callback = callback;

		return true;
	}

	inline bool DMAController::StartTransfer(unsigned char channelID)
	{
		if (channelID >= MAX_CHANNELS)
		{
			return false;
		}

		DmaChannelId channel = DmaChannelId::Create(channelID);

		if (channels[channel].State != ChannelState::Configured)
		{
			return false;
		}

		channels[channel].State = ChannelState::Active;
		totalTransfers++;

		return true;
	}

	void DMAController::ExecuteCycle()
	{
		// Priority-based arbitration: find highest priority active channel
		int selectedChannel = -1;
		unsigned char highestPriority = 0;

		for (int ch = 0; ch < MAX_CHANNELS; ch++)
		{
			DmaChannelId channel = DmaChannelId::Create(static_cast<unsigned char>(ch));
			if (channels[channel].State == ChannelState::Active)
			{
				unsigned char channelPriority = channels[channel].CurrentDesc.Priority;
				if (selectedChannel == -1 || channelPriority > highestPriority)
				{
					selectedChannel = ch;
					highestPriority = channelPriority;
				}
			}
		}

		// Process the highest priority channel
		if (selectedChannel >= 0)
		{
			DmaChannelId channel = DmaChannelId::Create(static_cast<unsigned char>(selectedChannel));
			ProcessChannel(channel);
		}
	}

	inline void DMAController::ProcessChannel(DmaChannelId channel)
	{
		unsigned char channelID = channel;
// C# TO C++ CONVERTER TASK: 'ref locals' are not converted by C# to C++ Converter:
// ORIGINAL LINE: ref ChannelControl ch = ref channels[channel];
		ChannelControl ch = channels[channel];
// C# TO C++ CONVERTER TASK: 'ref locals' are not converted by C# to C++ Converter:
// ORIGINAL LINE: ref TransferDescriptor desc = ref ch.CurrentDesc;
		TransferDescriptor desc = ch.CurrentDesc;

		// Calculate remaining bytes
		unsigned int remaining = ch.TotalBytes - ch.BytesTransferred;
		if (remaining == 0)
		{
			CompleteChannel(channel);
			return;
		}

		// Calculate burst size (limited by MAX_BURST_LENGTH and 4KB boundary)
		unsigned int burstBytes = std::min(remaining, static_cast<unsigned int>(MAX_BURST_LENGTH * desc.ElementSize));
		unsigned int srcOffset = ch.BytesTransferred;
		unsigned int dstOffset = ch.BytesTransferred;

		unsigned long long srcAddr = desc.SourceAddress + srcOffset;
		unsigned long long dstAddr = desc.DestAddress + dstOffset;

		// Check 4KB boundary crossing for source
		unsigned int srcTo4KB = BytesTo4KBBoundary(srcAddr);
		if (burstBytes > srcTo4KB && srcTo4KB > 0)
		{
			burstBytes = srcTo4KB;
		}

		// Check 4KB boundary crossing for destination
		unsigned int dstTo4KB = BytesTo4KBBoundary(dstAddr);
		if (burstBytes > dstTo4KB && dstTo4KB > 0)
		{
			burstBytes = dstTo4KB;
		}

		// Perform memory-to-memory transfer
		bool success = PerformBurst(srcAddr, dstAddr, burstBytes, desc.UseIOMMU);

		if (success)
		{
			ch.BytesTransferred += burstBytes;
			totalBytesTransferred += burstBytes;
		}
		else
		{
			// Transfer error
			ch.State = ChannelState::Error;
			ch.ErrorCode = 0x1; // Generic transfer error
			totalErrors++;

			// Invoke error callback if registered
			if (ch.Callback)
			{
				(*ch.Callback)(channelID, false, ch.ErrorCode);
			}

			// Fire completion event with error
			OnTransferCompleted(channelID, true, ch.ErrorCode, ch.BytesTransferred);

			// Signal error interrupt
			RaiseInterrupt(channel, true);
		}
	}

	inline void DMAController::CompleteChannel(DmaChannelId channel)
	{
		unsigned char channelID = channel;
// C# TO C++ CONVERTER TASK: 'ref locals' are not converted by C# to C++ Converter:
// ORIGINAL LINE: ref ChannelControl ch = ref channels[channel];
		ChannelControl ch = channels[channel];

		// Check if there's a chained descriptor
		if (ch.CurrentDesc.NextDescriptor != 0)
		{
			// Load next descriptor (placeholder - would read from memory)
			// For now, just mark as completed
			ch.State = ChannelState::Completed;
		}
		else
		{
			ch.State = ChannelState::Completed;
		}

		// Invoke completion callback if registered
		if (ch.Callback)
		{
			(*ch.Callback)(channelID, true, 0);
		}

		// Fire completion event
		OnTransferCompleted(channelID, false, 0, ch.BytesTransferred);

		// Signal interrupt on completion
		RaiseInterrupt(channel, false);
	}

	inline void DMAController::RaiseInterrupt(DmaChannelId channel, bool isError)
	{
		unsigned char channelID = channel;
		// Calculate interrupt ID: base (0x90) + channel offset + error flag
		unsigned short interruptID = static_cast<unsigned short>(0x90 + channelID);
		if (isError)
		{
			interruptID = static_cast<unsigned short>(0xA0 + channelID); // Error interrupts use 0xA0 base
		}

		if (_interruptDispatch != nullptr)
		{
			_interruptDispatch(Processor::DeviceType::DMAController, interruptID, 0);
		}
		else
		{
			Processor::InterruptData::CallInterrupt(Processor::DeviceType::DMAController, interruptID, 0);
		}
	}

	inline bool DMAController::PerformBurst(unsigned long long srcAddr, unsigned long long dstAddr, unsigned int byteCount, bool useIOMMU)
	{
		try
		{
			std::shared_ptr<Processor::MainMemoryArea> mainMemory = CaptureCurrentMainMemory();

			// Allocate temporary buffer for transfer
			std::vector<unsigned char> tempBuffer(byteCount);
			std::span<unsigned char> bufferSpan = tempBuffer;

			// Read from source
			if (useIOMMU)
			{
				// Use IOMMU for translation and bounds checking
				if (!IOMMU::ReadBurst(0ULL, srcAddr, bufferSpan))
				{
					return false;
				}
			}
			else
			{
				// Direct memory access
				if (!TryReadDirectMainMemoryExact(mainMemory, srcAddr, bufferSpan))
				{
					return false;
				}
			}

			// Write to destination
			if (useIOMMU)
			{
				// Use IOMMU for translation and bounds checking
				if (!IOMMU::WriteBurst(0ULL, dstAddr, bufferSpan))
				{
					return false;
				}
			}
			else
			{
				// Direct memory access
				if (!TryWriteDirectMainMemoryExact(mainMemory, dstAddr, bufferSpan))
				{
					return false;
				}
			}

			if (_coherencyObserver != nullptr)
			{
				_coherencyObserver->NotifyWrite(std::make_shared<MemoryCoherencyWriteNotification>(dstAddr, byteCount, 0, MemoryCoherencyWriteSourceKind::DmaControllerWrite));
			}
			return true;
		}
		catch (...)
		{
			return false;
		}
	}

	inline unsigned int DMAController::BytesTo4KBBoundary(unsigned long long address)
	{
		unsigned long long nextBoundary = (address + BOUNDARY_4KB) & ~static_cast<unsigned long long>(BOUNDARY_4KB - 1);
		return static_cast<unsigned int>(nextBoundary - address);
	}

	inline std::shared_ptr<Processor::MainMemoryArea> DMAController::CaptureCurrentMainMemory()
	{
		return Processor::MainMemory;
	}

	inline bool DMAController::HasExactMainMemoryRange(std::shared_ptr<Processor::MainMemoryArea> mainMemory, unsigned long long address, int size)
	{
		if (size <= 0)
		{
			return false;
		}

		unsigned long long memoryLength = static_cast<unsigned long long>(mainMemory->getLength());
		unsigned long long requestSize = static_cast<unsigned long long>(size);
		return requestSize <= memoryLength && address <= memoryLength - requestSize;
	}

	inline bool DMAController::TryReadDirectMainMemoryExact(std::shared_ptr<Processor::MainMemoryArea> mainMemory, unsigned long long address, std::span<unsigned char> buffer)
	{
		return HasExactMainMemoryRange(mainMemory, address, buffer.size()) && mainMemory->TryReadPhysicalRange(address, buffer);
	}

	inline bool DMAController::TryWriteDirectMainMemoryExact(std::shared_ptr<Processor::MainMemoryArea> mainMemory, unsigned long long address, std::span<const unsigned char> buffer)
	{
		return HasExactMainMemoryRange(mainMemory, address, buffer.size()) && mainMemory->TryWritePhysicalRange(address, buffer);
	}

	DMAController::ChannelState DMAController::GetChannelState(unsigned char channelID)
	{
		if (channelID >= MAX_CHANNELS)
		{
			return ChannelState::Error;
		}

		DmaChannelId channel = DmaChannelId::Create(channelID);
		return channels[channel].State;
	}

	void DMAController::ResetChannel(unsigned char channelID)
	{
		if (channelID >= MAX_CHANNELS)
		{
			return;
		}

		DmaChannelId channel = DmaChannelId::Create(channelID);
		channels[channel].State = ChannelState::Idle;
		channels[channel].BytesTransferred = 0;
		channels[channel].TotalBytes = 0;
		channels[channel].ErrorCode = 0;
	}

	bool DMAController::PauseTransfer(unsigned char channelID)
	{
		if (channelID >= MAX_CHANNELS)
		{
			return false;
		}

		DmaChannelId channel = DmaChannelId::Create(channelID);
		if (channels[channel].State == ChannelState::Active)
		{
			channels[channel].State = ChannelState::Paused;
			return true;
		}

		return false;
	}

	bool DMAController::ResumeTransfer(unsigned char channelID)
	{
		if (channelID >= MAX_CHANNELS)
		{
			return false;
		}

		DmaChannelId channel = DmaChannelId::Create(channelID);
		if (channels[channel].State == ChannelState::Paused)
		{
			channels[channel].State = ChannelState::Active;
			return true;
		}

		return false;
	}

	bool DMAController::CancelTransfer(unsigned char channelID)
	{
		if (channelID >= MAX_CHANNELS)
		{
			return false;
		}

		DmaChannelId channel = DmaChannelId::Create(channelID);
// C# TO C++ CONVERTER TASK: 'ref locals' are not converted by C# to C++ Converter:
// ORIGINAL LINE: ref ChannelControl ch = ref channels[channel];
		ChannelControl ch = channels[channel];

		if (ch.State == ChannelState::Active || ch.State == ChannelState::Paused || ch.State == ChannelState::Configured)
		{
			// Invoke callback with error
			if (ch.Callback)
			{
				(*ch.Callback)(channelID, false, 0xFF);
			}

			// Fire completion event
			OnTransferCompleted(channelID, true, 0xFF, ch.BytesTransferred);

			// Reset channel
			ResetChannel(channelID);
			return true;
		}

		return false;
	}

	void DMAController::OnTransferCompleted(unsigned char channelID, bool isError, unsigned long long errorCode, unsigned int bytesTransferred)
	{
		if (TransferCompleted != nullptr)
		{
			std::shared_ptr<TransferCompletedEventArgs> temp_var = std::make_shared<TransferCompletedEventArgs>();
			temp_var->setChannelID(channelID);
			temp_var->setError(isError);
			temp_var->setErrorCode(errorCode);
			temp_var->setBytesTransferred(bytesTransferred);
			for (auto listener : TransferCompleted->listeners())
			{
				listener(this, temp_var);
			}
		}
	}
}
