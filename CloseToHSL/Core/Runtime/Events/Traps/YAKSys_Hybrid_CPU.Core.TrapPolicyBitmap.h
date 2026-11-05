#pragma once

#include "TrapPolicyBitmap.h"
#include "TrapRequest.h"
#include "NeutralTrapResult.h"
#include <unordered_set>
#include <vector>
#include <algorithm>
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{
	class TrapPolicyBitmap final : public std::enable_shared_from_this<TrapPolicyBitmap>
	{
	private:
		unsigned long long PolicyEpoch = 0;

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly HashSet<ushort> _instructionOpcodes = new();
		std::unordered_set<unsigned short> _instructionOpcodes = std::unordered_set<unsigned short>();
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly HashSet<ushort> _csrAddresses = new();
		std::unordered_set<unsigned short> _csrAddresses = std::unordered_set<unsigned short>();
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly List<MemoryTrapRange> _memoryRanges = new();
		std::vector<std::shared_ptr<MemoryTrapRange>> _memoryRanges = std::vector<std::shared_ptr<MemoryTrapRange>>();
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly HashSet<uint> _laneOperations = new();
		std::unordered_set<unsigned int> _laneOperations = std::unordered_set<unsigned int>();
		unsigned long long _enabledClasses = 0;
		unsigned long long _compatibilityOperationMask = 0;
		unsigned int _laneWideMask = 0;

	public:
		unsigned long long getPolicyEpoch() const;
	private:
		void setPolicyEpoch(unsigned long long value);

	public:
		TrapPolicyClass getEnabledClasses() const;

		void Clear();

		void EnableInstruction(unsigned short opcode);

		void EnableCsr(unsigned short csrAddress);

		void EnableMemoryRange(unsigned long long baseAddress, unsigned long long length, TrapAccessMask accessMask);

		void EnableCompatibilityOperation(unsigned char operation);

		bool InterceptsCompatibilityOperation(unsigned char operation);

		void EnableLaneOperation(unsigned char laneId, unsigned short opcode);

		void EnableLane(unsigned char laneId);

		std::shared_ptr<NeutralTrapResult> Evaluate(std::shared_ptr<TrapRequest> request);

	private:
		void AdvanceEpoch();

		static unsigned long long OperationBit(unsigned char operation);

		bool InterceptsMemory(unsigned long long address, unsigned long long length, TrapAccessType accessType);

		bool InterceptsLaneOperation(unsigned char laneId, unsigned short opcode);

		static unsigned int LaneOperationKey(unsigned char laneId, unsigned short opcode);


	public:
		void EnableVmxOperation(VmxOperationKind operation);

		bool InterceptsVmxOperation(VmxOperationKind operation);
	};
}
