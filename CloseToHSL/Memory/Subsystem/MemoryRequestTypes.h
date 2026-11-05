#pragma once
#include <memory>
#include <string>
#include <vector>
#include "../../../tangible_event.h"
namespace YAKSys_Hybrid_CPU::Memory {
class PhysicalMemoryBankBinding;
		class BurstEventArgs : public std::enable_shared_from_this<BurstEventArgs>
		{
		private:
			unsigned long long Address = 0;
			int Length = 0;
			bool IsRead = false;
			int BankId = 0;
			long long Timestamp = 0;

		public:
			unsigned long long getAddress() const;
			void setAddress(unsigned long long value);
			int getLength() const;
			void setLength(int value);
			bool isRead() const;
			void setRead(bool value);
			int getBankId() const;
			void setBankId(int value);
			long long getTimestamp() const;
			void setTimestamp(long long value);

		};

		class MemoryRequestToken : public std::enable_shared_from_this<MemoryRequestToken>
		{
		private:
			unsigned long long RequestID = 0;
			bool IsComplete = false;
			bool Succeeded = false;
			unsigned long long DeviceID = 0;
			unsigned long long Address = 0;
			int Size = 0;
			bool IsRead = false;
			bool DefersPhysicalWriteUntilRetire = false;
			long long EnqueueCycle = 0;
			long long CompleteCycle = 0;
			std::wstring FailureReason;

			/// <summary>
			/// Unique identifier for this request
			/// </summary>
		public:
			unsigned long long getRequestID() const;
			void setRequestID(unsigned long long value);

			/// <summary>
			/// Whether the request has completed
			/// </summary>
			bool isComplete() const;
			void setComplete(bool value);

			/// <summary>
			/// Whether the completed request successfully materialized its data/effect.
			/// Only meaningful once <see cref="IsComplete"/> is true.
			/// </summary>
			bool getSucceeded() const;
			void setSucceeded(bool value);

			/// <summary>
			/// Buffer containing read data (for reads) or write data (for writes)
			/// </summary>
		private:
			std::vector<unsigned char> buffer;

			// Owner-only physical-location evidence captured at legacy request
			// acceptance. The public compatibility constructor leaves it absent.
			const std::shared_ptr<PhysicalMemoryBankBinding> physicalBankBinding;

			/// <summary>
			/// Device ID that initiated this request
			/// </summary>
		public:
			unsigned long long getDeviceID() const;
			void setDeviceID(unsigned long long value);

			/// <summary>
			/// Memory address for this request
			/// </summary>
			unsigned long long getAddress() const;
			void setAddress(unsigned long long value);

			/// <summary>
			/// Size of the request in bytes
			/// </summary>
			int getSize() const;
			void setSize(int value);

			/// <summary>
			/// Whether this is a read request (false = write)
			/// </summary>
			bool isRead() const;
			void setRead(bool value);

			/// <summary>
			/// Whether the request consumed subsystem timing/resources but must not
			/// physically update memory until the core retires the corresponding store.
			/// </summary>
			bool getDefersPhysicalWriteUntilRetire() const;
			void setDefersPhysicalWriteUntilRetire(bool value);

			/// <summary>
			/// Cycle when this request was enqueued
			/// </summary>
			long long getEnqueueCycle() const;
			void setEnqueueCycle(long long value);

			/// <summary>
			/// Cycle when this request completed (0 if not complete)
			/// </summary>
			long long getCompleteCycle() const;
			void setCompleteCycle(long long value);

			/// <summary>
			/// Truthful failure reason for completed-but-unsuccessful requests.
			/// </summary>
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public string? FailureReason {get;internal set;}
			std::wstring getFailureReason() const;
			void setFailureReason(const std::wstring &value);

			MemoryRequestToken(unsigned long long requestID, unsigned long long deviceID, unsigned long long address, int size, std::vector<unsigned char> &dataBuffer, bool isRead, long long enqueueCycle, bool defersPhysicalWriteUntilRetire, std::shared_ptr<PhysicalMemoryBankBinding> physicalBankBinding);

			/// <summary>
			/// Get the buffer containing the data
			/// </summary>
			std::vector<unsigned char> GetBuffer();

			// Internal owner-only read for the legacy cancellation location
			// consumer. This does not expose a public token location carrier.
			std::shared_ptr<PhysicalMemoryBankBinding> GetPhysicalBankBindingForOwner();

			/// <summary>
			/// Fail-closed guard for runtime surfaces that were previously treating
			/// completed async requests as implicit success.
			/// </summary>
			void ThrowIfFailed(const std::wstring &executionSurface);
		
		};
}
