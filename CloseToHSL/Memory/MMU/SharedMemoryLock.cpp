#include "SharedMemoryLock.h"

namespace HybridCPU_ISE::CloseToHSL::Memory::MMU
{

	int SharedMemoryLock::getSharedRegionId() const
	{
		return _sharedRegionId;
	}

	unsigned long long SharedMemoryLock::getBaseAddress() const
	{
		return _baseAddress;
	}

	unsigned long long SharedMemoryLock::getSize() const
	{
		return _size;
	}

	int SharedMemoryLock::getLockOwner() const
	{
		return _lockOwner;
	}

	bool SharedMemoryLock::isLocked() const
	{
		return _lockOwner >= 0;
	}

	SharedMemoryLock::SharedMemoryLock(int sharedRegionId, unsigned long long baseAddress, unsigned long long size) : _sharedRegionId(sharedRegionId), _baseAddress(baseAddress), _size(size)
	{
	}

	bool SharedMemoryLock::AcquireLock(int threadId)
	{
		if (threadId < 0 || threadId > 15)
		{
			return false;
		}

		{
			std::scoped_lock<std::mutex> lock(_syncRoot);
			if (_lockOwner == -1)
			{
				_lockOwner = threadId;
				return true;
			}
			return false; // Already locked
		}
	}

	bool SharedMemoryLock::TryAcquireLock(int threadId)
	{
		return AcquireLock(threadId);
	}

	bool SharedMemoryLock::ReleaseLock(int threadId)
	{
		{
			std::scoped_lock<std::mutex> lock(_syncRoot);
			if (_lockOwner == threadId)
			{
				_lockOwner = -1;
				return true;
			}
			return false; // Not owner
		}
	}

	bool SharedMemoryLock::IsOwnedBy(int threadId)
	{
		return _lockOwner == threadId;
	}

	void SharedMemoryLock::ForceUnlock()
	{
		{
			std::scoped_lock<std::mutex> lock(_syncRoot);
			_lockOwner = -1;
		}
	}

	void SharedMemoryLockManager::Initialize()
	{
		_locks = std::unordered_map<int, std::shared_ptr<SharedMemoryLock>>();
	}

	bool SharedMemoryLockManager::CreateLock(int sharedRegionId, unsigned long long baseAddress, unsigned long long size)
	{
		if (_locks.empty())
		{
			Initialize();
		}

	// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
	// ORIGINAL LINE: if (_locks!.ContainsKey(sharedRegionId))
		if (_locks.contains(sharedRegionId))
		{
			return false; // Lock already exists
		}

		_locks[sharedRegionId] = std::make_shared<SharedMemoryLock>(sharedRegionId, baseAddress, size);
		return true;
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public static SharedMemoryLock? GetLock(int sharedRegionId)
	std::shared_ptr<SharedMemoryLock> SharedMemoryLockManager::GetLock(int sharedRegionId)
	{
		if (_locks.empty() || !_locks.contains(sharedRegionId))
		{
			return nullptr;
		}

		return _locks[sharedRegionId];
	}

	bool SharedMemoryLockManager::AcquireLock(int sharedRegionId, int threadId)
	{
		std::shared_ptr<SharedMemoryLock> lock_ = GetLock(sharedRegionId);
		if (lock_ == nullptr)
		{
			return false;
		}

		return lock_->AcquireLock(threadId);
	}

	bool SharedMemoryLockManager::ReleaseLock(int sharedRegionId, int threadId)
	{
		std::shared_ptr<SharedMemoryLock> lock_ = GetLock(sharedRegionId);
		if (lock_ == nullptr)
		{
			return false;
		}

		return lock_->ReleaseLock(threadId);
	}

	bool SharedMemoryLockManager::IsRegionLocked(int sharedRegionId)
	{
		const std::shared_ptr<SharedMemoryLock> lock_ = GetLock(sharedRegionId);
		return lock_ != nullptr && lock_->isLocked();
	}

	int SharedMemoryLockManager::GetLockOwner(int sharedRegionId)
	{
		const std::shared_ptr<SharedMemoryLock> lock_ = GetLock(sharedRegionId);
		return lock_ != nullptr ? lock_->getLockOwner() : -1;
	}

	bool SharedMemoryLockManager::RemoveLock(int sharedRegionId)
	{
		if (_locks.empty() || !_locks.contains(sharedRegionId))
		{
			return false;
		}

		_locks.erase(sharedRegionId);
		return true;
	}

	std::vector<std::shared_ptr<SharedMemoryLock>> SharedMemoryLockManager::GetAllLocks()
	{
		if (_locks.empty())
		{
			return {};
		}

		std::vector<std::shared_ptr<SharedMemoryLock>> locks;
		locks.reserve(_locks.size());
		for (const auto& [sharedRegionId, lock] : _locks)
		{
			locks.push_back(lock);
		}
		return locks;
	}
}
