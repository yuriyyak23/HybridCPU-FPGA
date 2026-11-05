#pragma once

namespace YAKSys_Hybrid_CPU::Core
{

	enum class DomainRuntimeOperationKind : unsigned char
	{
		ActivateCompatibilityFrontend = 0,
		DeactivateCompatibilityFrontend = 1,
		EnterDomain = 2,
		ResumeDomain = 3,
		ReadCompatibilityProjection = 4,
		WriteCompatibilityProjection = 5,
		InvalidateTranslation = 6,
		InvokeCapability = 7,
		SaveDomainState = 8,
		RestoreDomainState = 9,
		ProjectCompatibilityTrap = 10,
		InvokeHypercall = 11
	};

	enum class DomainRuntimeOperationSource : unsigned char
	{
		CompatibilityFrontend = 0,
		RuntimeService = 1,
		MigrationReplay = 2
	};

	enum class DomainRuntimeOperationAuthorityClass : unsigned char
	{
		ProjectionOnly = 0,
		NoStateExecution = 1,
		AuthoritativeMutation = 2
	};


}
