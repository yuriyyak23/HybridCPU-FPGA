#pragma once

namespace YAKSys_Hybrid_CPU::Core
{

	enum class SecureMigrationMode : unsigned char
	{
		Disabled = 0,
		LocalOnly = 1,
		ReattestRequired = 2,
		PolicyCompatible = 3
	};

	enum class SecurePrivateMemoryMigrationPolicy : unsigned char
	{
		Denied = 0,
		ReinitializeAfterRestore = 1,
		SealedEncryptedPayloadRequired = 2
	};

	enum class SecureMeasurementRestorePolicy : unsigned char
	{
		Denied = 0,
		ReuseMaterializedHandle = 1,
		Revalidate = 2,
		Remeasure = 3,
		Reattest = 4
	};

	enum class SecureGrantRestorePolicy : unsigned char
	{
		Denied = 0,
		PreserveTypedGrants = 1,
		Rederive = 2
	};


}
