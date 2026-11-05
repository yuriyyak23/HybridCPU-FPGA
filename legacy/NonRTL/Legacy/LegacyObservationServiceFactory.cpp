#include "LegacyObservationServiceFactory.h"
#include "LegacyProcessorMachineStateSource.h"

namespace HybridCPU_ISE::Legacy
{

	std::shared_ptr<IseObservationService> LegacyObservationServiceFactory::CreateLegacyGlobalCompat(std::shared_ptr<std::recursive_mutex> syncLock)
	{
		ArgumentNullException::ThrowIfNull(syncLock);
		return std::make_shared<IseObservationService>(std::make_shared<LegacyProcessorMachineStateSource>(LegacyMemoryReadPolicy::ZeroPaddedCompatibility), syncLock);
	}

	std::shared_ptr<IseObservationService> LegacyObservationServiceFactory::CreateLegacyGlobalStrict(std::shared_ptr<std::recursive_mutex> syncLock)
	{
		ArgumentNullException::ThrowIfNull(syncLock);
		return std::make_shared<IseObservationService>(std::make_shared<LegacyProcessorMachineStateSource>(LegacyMemoryReadPolicy::FailClosed), syncLock);
	}

	std::shared_ptr<IseObservationService> LegacyObservationServiceFactory::CreateProcessorBacked(std::shared_ptr<std::recursive_mutex> syncLock)
	{
		return CreateLegacyGlobalCompat(syncLock);
	}
}
