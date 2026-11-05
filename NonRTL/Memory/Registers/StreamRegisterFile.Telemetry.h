#pragma once

namespace YAKSys_Hybrid_CPU::Memory
{
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to the C# readonly struct:
// ORIGINAL LINE: public readonly struct StreamIngressWarmTelemetry
	class StreamIngressWarmTelemetry
	{
	private:
		const unsigned long long ForegroundWarmAttempts;
		const unsigned long long ForegroundWarmSuccesses;
		const unsigned long long ForegroundWarmReuseHits;
		const unsigned long long ForegroundBypassHits;
		const unsigned long long AssistWarmAttempts;
		const unsigned long long AssistWarmSuccesses;
		const unsigned long long AssistWarmReuseHits;
		const unsigned long long AssistBypassHits;
		const unsigned long long TranslationRejects;
		const unsigned long long BackendRejects;
		const unsigned long long AssistResidentBudgetRejects;
		const unsigned long long AssistLoadingBudgetRejects;
		const unsigned long long AssistNoVictimRejects;

	public:
		StreamIngressWarmTelemetry(unsigned long long foregroundWarmAttempts, unsigned long long foregroundWarmSuccesses, unsigned long long foregroundWarmReuseHits, unsigned long long foregroundBypassHits, unsigned long long assistWarmAttempts, unsigned long long assistWarmSuccesses, unsigned long long assistWarmReuseHits, unsigned long long assistBypassHits, unsigned long long translationRejects, unsigned long long backendRejects, unsigned long long assistResidentBudgetRejects, unsigned long long assistLoadingBudgetRejects, unsigned long long assistNoVictimRejects);

		unsigned long long getForegroundWarmAttempts() const;

		unsigned long long getForegroundWarmSuccesses() const;

		unsigned long long getForegroundWarmReuseHits() const;

		unsigned long long getForegroundBypassHits() const;

		unsigned long long getAssistWarmAttempts() const;

		unsigned long long getAssistWarmSuccesses() const;

		unsigned long long getAssistWarmReuseHits() const;

		unsigned long long getAssistBypassHits() const;

		unsigned long long getTranslationRejects() const;

		unsigned long long getBackendRejects() const;

		unsigned long long getAssistResidentBudgetRejects() const;

		unsigned long long getAssistLoadingBudgetRejects() const;

		unsigned long long getAssistNoVictimRejects() const;

		StreamIngressWarmTelemetry() = default;
	};

}
