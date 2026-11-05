#pragma once

#include "../Pipeline/Assist/Runtime/AssistRuntime.h"
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{

	/// <summary>
	/// Reference-owned storage for core-local assist epoch and invalidation identity.
	/// Scheduler nomination, replay invalidation and retirement authorities remain
	/// with their existing owners.
	/// </summary>
	class AssistState final : public std::enable_shared_from_this<AssistState>
	{
	public:
		unsigned long long RuntimeEpoch = 0;
		AssistInvalidationReason LastInvalidationReason = static_cast<AssistInvalidationReason>(0);
	};

}
