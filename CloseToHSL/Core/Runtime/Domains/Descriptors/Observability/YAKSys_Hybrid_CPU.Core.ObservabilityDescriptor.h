#pragma once

#include "../EvidencePolicy/EvidencePolicyDescriptor.h"
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{
	class ObservabilityDescriptor final : public std::enable_shared_from_this<ObservabilityDescriptor>
	{
	private:
		static inline const std::shared_ptr<ObservabilityDescriptor> FailClosed = std::make_shared<ObservabilityDescriptor>();
		const bool AllowGuestArchitecturalObservability;
		const bool AllowCompatibilityAliasObservability;
		const bool AllowHostLocalRuntimeCapture;

	public:
		ObservabilityDescriptor();

		ObservabilityDescriptor(bool allowGuestArchitecturalObservability, bool allowCompatibilityAliasObservability, bool allowHostLocalRuntimeCapture);

		static std::shared_ptr<ObservabilityDescriptor> getFailClosed();

		bool getAllowGuestArchitecturalObservability() const;

		bool getAllowCompatibilityAliasObservability() const;

		bool getAllowHostLocalRuntimeCapture() const;

		bool CanPublishToGuest(EvidenceVisibilityClass evidenceClass);

		bool CanCaptureHostLocal(EvidenceVisibilityClass evidenceClass);

		bool MustRedactForGuest(EvidenceVisibilityClass evidenceClass);
	};
}
