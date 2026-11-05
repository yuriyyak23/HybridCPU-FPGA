#include "ReplayPhaseSubstrate.Interfaces.h"
#include "ReplayPhaseSubstrate.Implementations.h"

using namespace YAKSys_Hybrid_CPU::Core::Diagnostics;

namespace YAKSys_Hybrid_CPU::Core
{

	std::shared_ptr<IInterCoreLegalityCertificateCache> RuntimeLegalityCertificateCacheFactory::CreateInterCore()
	{
		return std::make_shared<InterCoreLegalityCertificateCache>();
	}

	std::shared_ptr<ISmtLegalityCertificateCache4Way> RuntimeLegalityCertificateCacheFactory::CreateSmt4Way()
	{
		return std::make_shared<SmtLegalityCertificateCache4Way>();
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal static ILegalityCertificateCacheCoordinator CreateDefault(ILegalityCertificateCacheTelemetrySink telemetrySink, IInterCoreLegalityCertificateCache? interCoreCache = null, System.Nullable<ISmtLegalityCertificateCache4Way> smtCache = null)
	std::shared_ptr<ILegalityCertificateCacheCoordinator> RuntimeLegalityCertificateCacheCoordinatorFactory::CreateDefault(std::shared_ptr<ILegalityCertificateCacheTelemetrySink> telemetrySink, std::shared_ptr<IInterCoreLegalityCertificateCache> interCoreCache, std::shared_ptr<ISmtLegalityCertificateCache4Way> smtCache)
	{
		return std::make_shared<LegalityCertificateCacheCoordinator>(telemetrySink, (interCoreCache != nullptr) ? interCoreCache : RuntimeLegalityCertificateCacheFactory::CreateInterCore(), (smtCache != nullptr) ? smtCache : RuntimeLegalityCertificateCacheFactory::CreateSmt4Way());
	}
}
