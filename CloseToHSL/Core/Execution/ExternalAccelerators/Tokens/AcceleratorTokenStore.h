#pragma once

#include "AcceleratorToken.h"
#include "../ExternalAcceleratorFeatureSwitch.h"
#include "../Telemetry/AcceleratorTelemetry.h"
#include "../Capabilities/AcceleratorCapabilityRegistry.h"
#include "../Conflicts/ExternalAcceleratorConflictManager.h"
#include "AcceleratorTokenResults.h"
#include "../Auth/AcceleratorOwnerDomainGuard.h"
#include "AcceleratorTokenStatusWord.h"
#include <string>
#include <unordered_map>
#include <optional>
#include <format>
#include <memory>
#include "../../../../../tangible_string_helper.h"

using namespace YAKSys_Hybrid_CPU::Core;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Auth;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Capabilities;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Conflicts;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Descriptors;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Telemetry;

namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Tokens
{
	class AcceleratorTokenAdmissionResult; class AcceleratorTokenLookupResult; struct AcceleratorWaitPolicy; struct AcceleratorCancelPolicy;

	class AcceleratorTokenStore final : public std::enable_shared_from_this<AcceleratorTokenStore>
	{
	private:
		static constexpr unsigned long long HandleSalt = 0xA7C3'5D19'E2B4'6F80ULL;
		static constexpr unsigned long long HandleStride = 0x9E37'79B9'7F4A'7C15ULL;

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly Dictionary<ulong, AcceleratorToken> _tokens = new();
		std::unordered_map<unsigned long long, std::shared_ptr<AcceleratorToken>> _tokens = std::unordered_map<unsigned long long, std::shared_ptr<AcceleratorToken>>();
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private readonly AcceleratorTelemetry? _telemetry;
		const std::shared_ptr<AcceleratorTelemetry> _telemetry;
		const std::shared_ptr<ExternalAcceleratorFeatureSwitch> _featureSwitch;
		unsigned long long _nextTokenId = 1;
		unsigned long long _handleGeneration = 1;

	public:
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public AcceleratorTokenStore(AcceleratorTelemetry? telemetry = null, System.Nullable<ExternalAcceleratorFeatureSwitch> featureSwitch = null)
		AcceleratorTokenStore(std::shared_ptr<AcceleratorTelemetry> telemetry = nullptr, std::optional<std::shared_ptr<ExternalAcceleratorFeatureSwitch>> featureSwitch = std::nullopt);

		int getCount() const;

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public AcceleratorTokenAdmissionResult Create(AcceleratorCommandDescriptor descriptor, AcceleratorCapabilityAcceptanceResult capabilityAcceptance, System.Nullable<AcceleratorGuardEvidence> submitGuardEvidence, AcceleratorTokenAdmissionRejectPolicy rejectPolicy = AcceleratorTokenAdmissionRejectPolicy.NonTrappingReject, ExternalAcceleratorConflictManager? conflictManager = null)
		std::shared_ptr<AcceleratorTokenAdmissionResult> Create(std::shared_ptr<AcceleratorCommandDescriptor> descriptor, std::shared_ptr<AcceleratorCapabilityAcceptanceResult> capabilityAcceptance, std::optional<std::shared_ptr<AcceleratorGuardEvidence>> &submitGuardEvidence, AcceleratorTokenAdmissionRejectPolicy rejectPolicy = AcceleratorTokenAdmissionRejectPolicy::NonTrappingReject, std::shared_ptr<ExternalAcceleratorConflictManager> conflictManager = nullptr);

		std::shared_ptr<AcceleratorTokenLookupResult> TryLookup(std::shared_ptr<AcceleratorTokenHandle> handle, std::optional<std::shared_ptr<AcceleratorGuardEvidence>> &currentGuardEvidence, AcceleratorTokenLookupIntent intent = AcceleratorTokenLookupIntent::Observe);

		std::shared_ptr<AcceleratorTokenLookupResult> TryPoll(std::shared_ptr<AcceleratorTokenHandle> handle, std::optional<std::shared_ptr<AcceleratorGuardEvidence>> &currentGuardEvidence);

		std::shared_ptr<AcceleratorTokenLookupResult> Poll(std::shared_ptr<AcceleratorTokenHandle> handle, std::optional<std::shared_ptr<AcceleratorGuardEvidence>> &currentGuardEvidence);

		std::shared_ptr<AcceleratorTokenLookupResult> TryStatus(std::shared_ptr<AcceleratorTokenHandle> handle, std::optional<std::shared_ptr<AcceleratorGuardEvidence>> &currentGuardEvidence);

		std::shared_ptr<AcceleratorTokenLookupResult> Status(std::shared_ptr<AcceleratorTokenHandle> handle, std::optional<std::shared_ptr<AcceleratorGuardEvidence>> &currentGuardEvidence);

		std::shared_ptr<AcceleratorTokenLookupResult> TryWait(std::shared_ptr<AcceleratorTokenHandle> handle, std::optional<std::shared_ptr<AcceleratorGuardEvidence>> &currentGuardEvidence, std::shared_ptr<AcceleratorWaitPolicy> waitPolicy);

		std::shared_ptr<AcceleratorTokenLookupResult> Wait(std::shared_ptr<AcceleratorTokenHandle> handle, std::optional<std::shared_ptr<AcceleratorGuardEvidence>> &currentGuardEvidence);

		std::shared_ptr<AcceleratorTokenLookupResult> Fence(std::shared_ptr<AcceleratorTokenHandle> handle, std::optional<std::shared_ptr<AcceleratorGuardEvidence>> &currentGuardEvidence);

		std::shared_ptr<AcceleratorTokenLookupResult> TryCancel(std::shared_ptr<AcceleratorTokenHandle> handle, std::optional<std::shared_ptr<AcceleratorGuardEvidence>> &currentGuardEvidence, std::shared_ptr<AcceleratorCancelPolicy> cancelPolicy);

		std::shared_ptr<AcceleratorTokenLookupResult> TryCancel(std::shared_ptr<AcceleratorTokenHandle> handle, std::optional<std::shared_ptr<AcceleratorGuardEvidence>> &currentGuardEvidence);

		std::shared_ptr<AcceleratorTokenLookupResult> Cancel(std::shared_ptr<AcceleratorTokenHandle> handle, std::optional<std::shared_ptr<AcceleratorGuardEvidence>> &currentGuardEvidence);

		std::shared_ptr<AcceleratorTokenLookupResult> TryCommitPublication(std::shared_ptr<AcceleratorTokenHandle> handle, std::optional<std::shared_ptr<AcceleratorGuardEvidence>> &currentGuardEvidence);

		static AcceleratorTokenFaultCode MapGuardFault(AcceleratorGuardFault guardFault);

	private:
		static bool IsCapabilityGuardBackedForDescriptor(std::shared_ptr<AcceleratorCapabilityAcceptanceResult> capabilityAcceptance, std::shared_ptr<AcceleratorCommandDescriptor> descriptor, std::wstring &message);

		static bool IsCurrentRuntimeTaxonomyForTokenAdmission(std::shared_ptr<AcceleratorCommandDescriptor> descriptor, std::shared_ptr<AcceleratorCapabilityAcceptanceResult> capabilityAcceptance, std::wstring &message);

		unsigned long long AllocateTokenId();

		std::shared_ptr<AcceleratorTokenHandle> AllocateHandle(unsigned long long tokenId);

		std::shared_ptr<AcceleratorTokenAdmissionResult> RecordSubmitResult(std::shared_ptr<AcceleratorTokenAdmissionResult> result);

		std::shared_ptr<AcceleratorTokenLookupResult> RecordLookupResult(std::shared_ptr<AcceleratorTokenLookupResult> result);
	};

}
