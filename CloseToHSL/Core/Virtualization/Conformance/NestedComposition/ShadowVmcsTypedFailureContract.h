#pragma once

namespace YAKSys_Hybrid_CPU {
namespace Core {

	enum class ShadowVmcsTypedFailureViolation : unsigned char
	{
		None = 0,
		MissingFailure = 1,
		NonGenericFailureCode = 2,
		MissingFailureMessage = 3
	};


}
}
