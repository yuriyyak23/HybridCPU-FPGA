#include "ExecutionEngine.h"
#include "../InvalidInternalOpException.h"

using namespace YAKSys_Hybrid_CPU::Core::Pipeline::MicroOps;

namespace YAKSys_Hybrid_CPU::Core::Execution
{

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// C# TO C++ CONVERTER TASK: Throw expressions are not converted by C# to C++ Converter:
// ORIGINAL LINE: public ExecutionEngine(IReadOnlyDictionary<InternalOpKind, IExecutionUnit> units)
	ExecutionEngine::ExecutionEngine(const std::unordered_map<InternalOpKind, std::shared_ptr<IExecutionUnit>> &units) : _units(units)
	{
	}

	ExecutionResult ExecutionEngine::Execute(std::shared_ptr<InternalOp> op, std::shared_ptr<ICanonicalCpuState> state)
	{
		if (op == nullptr)
		{
			throw std::invalid_argument("op");
		}

		auto unitIterator = _units.find(op->Kind);
		if (unitIterator == _units.end())
		{
			throw InvalidInternalOpException(op->Kind);
		}

		return unitIterator->second->Execute(op, state);
	}
}

