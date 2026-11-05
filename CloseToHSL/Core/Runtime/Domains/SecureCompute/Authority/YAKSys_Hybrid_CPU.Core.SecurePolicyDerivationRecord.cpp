#include "YAKSys_Hybrid_CPU.Core.SecurePolicyDerivationRecord.h"

namespace YAKSys_Hybrid_CPU::Core
{
	const std::shared_ptr<SecurePolicyDerivationRecord> SecurePolicyDerivationRecord::None =
		std::make_shared<SecurePolicyDerivationRecord>(0, 0, 0, 0, 0);
}
