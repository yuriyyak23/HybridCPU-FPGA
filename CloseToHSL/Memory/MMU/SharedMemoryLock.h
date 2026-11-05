#pragma once

#include <unordered_map>
#include <vector>
#include <mutex>
#include <memory>

namespace HybridCPU_ISE::CloseToHSL::Memory::MMU
{
	/// <summary>
	/// Hardware-agnostic mutex/lock for shared memory regions (Phase 5 extension)
	///
	/// Design principles:
	/// - HLS-compatible: simple spin-lock implementation
	/// - Deterministic: no OS-level primitives
	/// - Thread-safe: uses atomic operations for lock state
	/// - Singularity-style: explicit locking required for shared regions
	///
	/// Usage:
	/// 1. Create lock for shared memory region
	/// 2. Each thread calls AcquireLock(threadId) before accessing
	/// 3. Thread calls ReleaseLock(threadId) after access
	/// 4. Lock tracks current owner for verification
	/// </summary>
	class SharedMemoryLock : public std::enable_shared_from_this<SharedMemoryLock>
	{
	private:
// C# TO C++ CONVERTER TASK: 'volatile' has a different meaning in C++:
// ORIGINAL LINE: private volatile int _lockOwner = -1;
		int _lockOwner = -1; // -1 = unlocked, 0-15 = thread ID
		std::mutex _syncRoot;
		const int _sharedRegionId;
		const unsigned long long _baseAddress;
		const unsigned long long _size;

	public:
		int getSharedRegionId() const;
		unsigned long long getBaseAddress() const;
		unsigned long long getSize() const;
		int getLockOwner() const;
		bool isLocked() const;

		SharedMemoryLock(int sharedRegionId, unsigned long long baseAddress, unsigned long long size);

		/// <summary>
		/// Acquire lock for thread (blocking)
		/// In hardware synthesis: would translate to test-and-set operation
		/// </summary>
		/// <param name="threadId">Thread ID requesting lock (0-15)</param>
		/// <returns>True if lock acquired</returns>
		bool AcquireLock(int threadId);

		/// <summary>
		/// Try to acquire lock (non-blocking)
		/// Returns immediately if lock unavailable
		/// </summary>
		/// <param name="threadId">Thread ID requesting lock (0-15)</param>
		/// <returns>True if lock acquired, false if already held</returns>
		bool TryAcquireLock(int threadId);

		/// <summary>
		/// Release lock held by thread
		/// Only the owning thread can release the lock
		/// </summary>
		/// <param name="threadId">Thread ID releasing lock (0-15)</param>
		/// <returns>True if lock released, false if not owner</returns>
		bool ReleaseLock(int threadId);

		/// <summary>
		/// Check if thread owns this lock
		/// </summary>
		bool IsOwnedBy(int threadId);

		/// <summary>
		/// Force unlock (for debugging/recovery)
		/// </summary>
		void ForceUnlock();
	};

	/// <summary>
	/// Manager for shared memory locks (Phase 5 extension)
	/// Maintains locks for all shared memory regions
	/// </summary>
	class SharedMemoryLockManager final
	{
	private:
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private static Dictionary<int, SharedMemoryLock>? _locks;
		static inline std::unordered_map<int, std::shared_ptr<SharedMemoryLock>> _locks;

	public:
		static void Initialize();

		/// <summary>
		/// Create lock for shared memory region
		/// </summary>
		static bool CreateLock(int sharedRegionId, unsigned long long baseAddress, unsigned long long size);

		/// <summary>
		/// Get lock for shared region
		/// </summary>
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public static SharedMemoryLock? GetLock(int sharedRegionId)
		static std::shared_ptr<SharedMemoryLock> GetLock(int sharedRegionId);

		/// <summary>
		/// Acquire lock for thread (blocking)
		/// </summary>
		static bool AcquireLock(int sharedRegionId, int threadId);

		/// <summary>
		/// Release lock for thread
		/// </summary>
		static bool ReleaseLock(int sharedRegionId, int threadId);

		/// <summary>
		/// Check if region is locked
		/// </summary>
		static bool IsRegionLocked(int sharedRegionId);

		/// <summary>
		/// Get current owner of lock
		/// </summary>
		static int GetLockOwner(int sharedRegionId);

		/// <summary>
		/// Remove lock for shared region
		/// </summary>
		static bool RemoveLock(int sharedRegionId);

		/// <summary>
		/// Get all active locks (for debugging)
		/// </summary>
		static std::vector<std::shared_ptr<SharedMemoryLock>> GetAllLocks();
	};
}
