#include "ExecutionDomainRuntime.StateEpoch.h"

namespace YAKSys_Hybrid_CPU::Core
{

	ExecutionDomainStateEpoch::ExecutionDomainStateEpoch(unsigned long long value)
	{
		Value = value;
	}

	unsigned long long ExecutionDomainStateEpoch::getValue() const
	{
		return Value;
	}

	bool ExecutionDomainStateEpoch::isMaterialized() const
	{
		return getValue() != 0;
	}

	bool ExecutionDomainStateEpoch::Equals(ExecutionDomainStateEpoch other) const
	{
		return getValue() == other.getValue();
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public override bool Equals(Object? obj)
	bool ExecutionDomainStateEpoch::Equals(std::any obj) const
	{
		const auto *other = std::any_cast<ExecutionDomainStateEpoch>(&obj);
		return other != nullptr && Equals(*other);
	}

	int ExecutionDomainStateEpoch::GetHashCode() const
	{
		// Match Int64.GetHashCode: XOR the low and high 32-bit halves.
		const unsigned long long value = getValue();
		return static_cast<int>(static_cast<unsigned int>(value) ^ static_cast<unsigned int>(value >> 32));
	}

	bool ExecutionDomainStateEpoch::operator == (ExecutionDomainStateEpoch right)
	{
		return this->Equals(right);
	}

	bool ExecutionDomainStateEpoch::operator != (ExecutionDomainStateEpoch right)
	{
		return !this->Equals(right);
	}

	std::wstring ExecutionDomainStateEpoch::ToString() const
	{
		return std::to_wstring(getValue());
	}
}
