#pragma once

namespace YAKSys_Hybrid_CPU::Core
{

	enum class SecureMeasurementState : unsigned char
	{
		Missing = 0,
		Pending = 1,
		Materialized = 2,
		Stale = 3,
		Revoked = 4
	};

	enum class SecureMeasurementDebugClass : unsigned char
	{
		Production = 0,
		MeasuredDebug = 1,
		DebugDenied = 2,
		DebugUnmeasuredDenied = 3
	};


}
