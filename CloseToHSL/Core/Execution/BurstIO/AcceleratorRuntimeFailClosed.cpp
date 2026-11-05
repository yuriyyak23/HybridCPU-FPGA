#include "AcceleratorRuntimeFailClosed.h"

namespace YAKSys_Hybrid_CPU::Execution
{

	void AcceleratorRuntimeFailClosed::ThrowRegistrationNotSupported()
	{
		throw NotSupportedException(RegistrationMessage);
	}

	std::shared_ptr<DMATransferToken> AcceleratorRuntimeFailClosed::ThrowTransferNotSupported()
	{
		throw NotSupportedException(TransferMessage);
	}
}
