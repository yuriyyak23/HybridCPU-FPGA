#include "YAKSys_Hybrid_CPU.Core.TrapPolicyBitmap.h"

namespace YAKSys_Hybrid_CPU::Core
{

	unsigned long long TrapPolicyBitmap::getPolicyEpoch() const
	{
		return PolicyEpoch;
	}

	void TrapPolicyBitmap::setPolicyEpoch(unsigned long long value)
	{
		PolicyEpoch = value;
	}

	TrapPolicyClass TrapPolicyBitmap::getEnabledClasses() const
	{
		return static_cast<TrapPolicyClass>(_enabledClasses);
	}

	void TrapPolicyBitmap::Clear()
	{
		_instructionOpcodes.clear();
		_csrAddresses.clear();
		_memoryRanges.clear();
		_laneOperations.clear();
		_enabledClasses = 0;
		_compatibilityOperationMask = 0;
		_laneWideMask = 0;
		AdvanceEpoch();
	}

	void TrapPolicyBitmap::EnableInstruction(unsigned short opcode)
	{
		if (_instructionOpcodes.insert(opcode).second)
		{
			_enabledClasses |= static_cast<unsigned long long>(TrapPolicyClass::Instruction);
			AdvanceEpoch();
		}
	}

	void TrapPolicyBitmap::EnableCsr(unsigned short csrAddress)
	{
		if (_csrAddresses.insert(csrAddress).second)
		{
			_enabledClasses |= static_cast<unsigned long long>(TrapPolicyClass::Csr);
			AdvanceEpoch();
		}
	}

	void TrapPolicyBitmap::EnableMemoryRange(unsigned long long baseAddress, unsigned long long length, TrapAccessMask accessMask)
	{
		std::shared_ptr<MemoryTrapRange> range = std::make_shared<MemoryTrapRange>(baseAddress, length, accessMask);
		if (!range->IsValid)
		{
			return;
		}

		_memoryRanges.push_back(range);
		_enabledClasses |= static_cast<unsigned long long>(TrapPolicyClass::Memory);
		AdvanceEpoch();
	}

	void TrapPolicyBitmap::EnableCompatibilityOperation(unsigned char operation)
	{
		unsigned long long mask = OperationBit(operation);
		if (mask == 0 || (_compatibilityOperationMask & mask) != 0)
		{
			return;
		}

		_compatibilityOperationMask |= mask;
		_enabledClasses |= static_cast<unsigned long long>(TrapPolicyClass::CompatibilityOperation);
		AdvanceEpoch();
	}

	bool TrapPolicyBitmap::InterceptsCompatibilityOperation(unsigned char operation)
	{
		unsigned long long mask = OperationBit(operation);
		return mask != 0 && (_compatibilityOperationMask & mask) != 0;
	}

	void TrapPolicyBitmap::EnableLaneOperation(unsigned char laneId, unsigned short opcode)
	{
// C# TO C++ CONVERTER TASK: The following 'is' operator pattern is not converted by C# to C++ Converter:
		if ((laneId != 6 && laneId != 7))
		{
			return;
		}

		unsigned int key = LaneOperationKey(laneId, opcode);
		if (_laneOperations.insert(key).second)
		{
			_enabledClasses |= static_cast<unsigned long long>(TrapPolicyClass::LaneOperation);
			AdvanceEpoch();
		}
	}

	void TrapPolicyBitmap::EnableLane(unsigned char laneId)
	{
// C# TO C++ CONVERTER TASK: The following 'is' operator pattern is not converted by C# to C++ Converter:
		if ((laneId != 6 && laneId != 7))
		{
			return;
		}

		unsigned int mask = 1u << laneId;
		if ((_laneWideMask & mask) != 0)
		{
			return;
		}

		_laneWideMask |= mask;
		_enabledClasses |= static_cast<unsigned long long>(TrapPolicyClass::LaneOperation);
		AdvanceEpoch();
	}

	std::shared_ptr<NeutralTrapResult> TrapPolicyBitmap::Evaluate(std::shared_ptr<TrapRequest> request)
	{
	switch (request->TargetKind) {
        case TrapTargetKind::InstructionOpcode:
            if ((_enabledClasses & static_cast<unsigned long long>(TrapPolicyClass::Instruction)) && _instructionOpcodes.contains(static_cast<unsigned short>(request->Target))) return NeutralTrapResult::Trap(request, NeutralTrapResultKind::InstructionIntercept); break;
        case TrapTargetKind::CsrAddress:
            if ((_enabledClasses & static_cast<unsigned long long>(TrapPolicyClass::Csr)) && _csrAddresses.contains(static_cast<unsigned short>(request->Target))) return NeutralTrapResult::Trap(request, NeutralTrapResultKind::CsrIntercept); break;
        case TrapTargetKind::CompatibilityOperation:
            if ((_enabledClasses & static_cast<unsigned long long>(TrapPolicyClass::CompatibilityOperation)) && InterceptsCompatibilityOperation(static_cast<unsigned char>(request->Target))) return NeutralTrapResult::Trap(request, NeutralTrapResultKind::CompatibilityOperationIntercept); break;
        case TrapTargetKind::MemoryRange:
            if ((_enabledClasses & static_cast<unsigned long long>(TrapPolicyClass::Memory)) && InterceptsMemory(request->Target, request->Auxiliary, request->AccessType)) return NeutralTrapResult::Trap(request, NeutralTrapResultKind::MemoryIntercept); break;
        case TrapTargetKind::LaneOperation:
            if ((_enabledClasses & static_cast<unsigned long long>(TrapPolicyClass::LaneOperation)) && InterceptsLaneOperation(static_cast<unsigned char>(request->Target), static_cast<unsigned short>(request->Auxiliary))) return NeutralTrapResult::Trap(request, NeutralTrapResultKind::LaneOperationIntercept); break;
        default: break;
        }
        return NeutralTrapResult::Continue(request);
	}

	void TrapPolicyBitmap::AdvanceEpoch()
	{
//		unchecked
			setPolicyEpoch(getPolicyEpoch() + 1);
// C# TO C++ CONVERTER TASK: End of the original C# 'unchecked' block.
	}

	unsigned long long TrapPolicyBitmap::OperationBit(unsigned char operation)
	{
		return operation == 0 || operation >= 64 ? 0 : 1ULL << operation;
	}

	bool TrapPolicyBitmap::InterceptsMemory(unsigned long long address, unsigned long long length, TrapAccessType accessType)
	{
		for (int index = 0; index < _memoryRanges.size(); index++)
		{
			if (_memoryRanges[index]->Matches(address, length, accessType))
			{
				return true;
			}
		}

		return false;
	}

	bool TrapPolicyBitmap::InterceptsLaneOperation(unsigned char laneId, unsigned short opcode)
	{
// C# TO C++ CONVERTER TASK: The following 'is' operator pattern is not converted by C# to C++ Converter:
		if ((laneId != 6 && laneId != 7))
		{
			return false;
		}

		return (_laneWideMask & (1u << laneId)) != 0 || _laneOperations.find(LaneOperationKey(laneId, opcode)) != _laneOperations.end();
	}

	unsigned int TrapPolicyBitmap::LaneOperationKey(unsigned char laneId, unsigned short opcode)
	{
		return (static_cast<unsigned int>(laneId) << 16) | opcode;
	}

	void TrapPolicyBitmap::EnableVmxOperation(VmxOperationKind operation)
	{
		EnableCompatibilityOperation(static_cast<unsigned char>(operation));
	}

	bool TrapPolicyBitmap::InterceptsVmxOperation(VmxOperationKind operation)
	{
		return InterceptsCompatibilityOperation(static_cast<unsigned char>(operation));
	}
}
