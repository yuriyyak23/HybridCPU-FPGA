#include "StreamRegisterFile.Telemetry.h"

namespace YAKSys_Hybrid_CPU::Memory
{

	StreamIngressWarmTelemetry::StreamIngressWarmTelemetry(unsigned long long foregroundWarmAttempts, unsigned long long foregroundWarmSuccesses, unsigned long long foregroundWarmReuseHits, unsigned long long foregroundBypassHits, unsigned long long assistWarmAttempts, unsigned long long assistWarmSuccesses, unsigned long long assistWarmReuseHits, unsigned long long assistBypassHits, unsigned long long translationRejects, unsigned long long backendRejects, unsigned long long assistResidentBudgetRejects, unsigned long long assistLoadingBudgetRejects, unsigned long long assistNoVictimRejects)
		: ForegroundWarmAttempts(foregroundWarmAttempts),
		  ForegroundWarmSuccesses(foregroundWarmSuccesses),
		  ForegroundWarmReuseHits(foregroundWarmReuseHits),
		  ForegroundBypassHits(foregroundBypassHits),
		  AssistWarmAttempts(assistWarmAttempts),
		  AssistWarmSuccesses(assistWarmSuccesses),
		  AssistWarmReuseHits(assistWarmReuseHits),
		  AssistBypassHits(assistBypassHits),
		  TranslationRejects(translationRejects),
		  BackendRejects(backendRejects),
		  AssistResidentBudgetRejects(assistResidentBudgetRejects),
		  AssistLoadingBudgetRejects(assistLoadingBudgetRejects),
		  AssistNoVictimRejects(assistNoVictimRejects)
	{}

	unsigned long long StreamIngressWarmTelemetry::getForegroundWarmAttempts() const
	{
		return ForegroundWarmAttempts;
	}

	unsigned long long StreamIngressWarmTelemetry::getForegroundWarmSuccesses() const
	{
		return ForegroundWarmSuccesses;
	}

	unsigned long long StreamIngressWarmTelemetry::getForegroundWarmReuseHits() const
	{
		return ForegroundWarmReuseHits;
	}

	unsigned long long StreamIngressWarmTelemetry::getForegroundBypassHits() const
	{
		return ForegroundBypassHits;
	}

	unsigned long long StreamIngressWarmTelemetry::getAssistWarmAttempts() const
	{
		return AssistWarmAttempts;
	}

	unsigned long long StreamIngressWarmTelemetry::getAssistWarmSuccesses() const
	{
		return AssistWarmSuccesses;
	}

	unsigned long long StreamIngressWarmTelemetry::getAssistWarmReuseHits() const
	{
		return AssistWarmReuseHits;
	}

	unsigned long long StreamIngressWarmTelemetry::getAssistBypassHits() const
	{
		return AssistBypassHits;
	}

	unsigned long long StreamIngressWarmTelemetry::getTranslationRejects() const
	{
		return TranslationRejects;
	}

	unsigned long long StreamIngressWarmTelemetry::getBackendRejects() const
	{
		return BackendRejects;
	}

	unsigned long long StreamIngressWarmTelemetry::getAssistResidentBudgetRejects() const
	{
		return AssistResidentBudgetRejects;
	}

	unsigned long long StreamIngressWarmTelemetry::getAssistLoadingBudgetRejects() const
	{
		return AssistLoadingBudgetRejects;
	}

	unsigned long long StreamIngressWarmTelemetry::getAssistNoVictimRejects() const
	{
		return AssistNoVictimRejects;
	}
}
