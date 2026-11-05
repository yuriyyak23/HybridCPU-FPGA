#pragma once

#include "AcceleratorCapabilityDescriptor.h"
#include "IAcceleratorCapabilityProvider.h"
#include <string>
#include <unordered_map>
#include <vector>
#include <stdexcept>
#include <format>
#include <memory>
#include <map>
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <Windows.h>
#include "../../../../../tangible_string_helper.h"

// C# TO C++ CONVERTER NOTE: Forward class declarations:
namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Capabilities { class AcceleratorCapabilityQueryResult; }
namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Capabilities { class AcceleratorCapabilityAcceptanceResult; }
namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Auth { class AcceleratorGuardDecision; }
namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Telemetry { class AcceleratorTelemetry; }
namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Descriptors { struct AcceleratorOwnerBinding; }

using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Auth;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Descriptors;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Telemetry;

namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Capabilities
{
	struct OrdinalIgnoreCaseLess {
		bool operator()(const std::wstring& a, const std::wstring& b) const noexcept {
			return CompareStringOrdinal(a.data(), static_cast<int>(a.size()), b.data(), static_cast<int>(b.size()), TRUE) == CSTR_LESS_THAN;
		}
	};

	/// <summary>
	/// Metadata-only registry for future L7-SDC external accelerator capability queries.
	/// </summary>
	class AcceleratorCapabilityRegistry final : public std::enable_shared_from_this<AcceleratorCapabilityRegistry>
	{
	private:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly Dictionary<string, AcceleratorCapabilityDescriptor> _descriptors = new(StringComparer.OrdinalIgnoreCase);
		std::map<std::wstring, std::shared_ptr<AcceleratorCapabilityDescriptor>, OrdinalIgnoreCaseLess> _descriptors;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private readonly AcceleratorTelemetry? _telemetry;
		const std::shared_ptr<AcceleratorTelemetry> _telemetry;

	public:
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public AcceleratorCapabilityRegistry(AcceleratorTelemetry? telemetry = null)
		AcceleratorCapabilityRegistry(std::shared_ptr<AcceleratorTelemetry> telemetry = nullptr);

		int getCount() const;

		void RegisterProvider(std::shared_ptr<IAcceleratorCapabilityProvider> provider);

		void RegisterDescriptor(std::shared_ptr<AcceleratorCapabilityDescriptor> descriptor);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public bool TryGetDescriptor(string acceleratorId, out AcceleratorCapabilityDescriptor? descriptor)
		bool TryGetDescriptor(const std::wstring &acceleratorId, std::shared_ptr<AcceleratorCapabilityDescriptor> &descriptor);

		const std::vector<std::shared_ptr<AcceleratorCapabilityDescriptor>> GetAllDescriptors();

		std::shared_ptr<AcceleratorCapabilityQueryResult> Query(const std::wstring &acceleratorId, AcceleratorCapabilityAdoptionMode adoptionMode = AcceleratorCapabilityAdoptionMode::MetadataOnly, AcceleratorCompatibilityMode compatibilityMode = AcceleratorCompatibilityMode::MetadataOnly);

		std::shared_ptr<AcceleratorCapabilityAcceptanceResult> AcceptCapability(const std::wstring &acceleratorId, std::shared_ptr<AcceleratorOwnerBinding> descriptorOwnerBinding, AcceleratorCapabilityAdoptionMode adoptionMode = AcceleratorCapabilityAdoptionMode::MetadataOnly, AcceleratorCompatibilityMode compatibilityMode = AcceleratorCompatibilityMode::MetadataOnly);

		std::shared_ptr<AcceleratorCapabilityAcceptanceResult> AcceptCapability(const std::wstring &acceleratorId, std::shared_ptr<AcceleratorOwnerBinding> descriptorOwnerBinding, std::shared_ptr<AcceleratorGuardDecision> descriptorGuardDecision, AcceleratorCapabilityAdoptionMode adoptionMode = AcceleratorCapabilityAdoptionMode::MetadataOnly, AcceleratorCompatibilityMode compatibilityMode = AcceleratorCompatibilityMode::MetadataOnly);

	private:
		static void ValidateMetadataOnlyDescriptor(std::shared_ptr<AcceleratorCapabilityDescriptor> descriptor);

		static bool IsKnownMetadataOnlyMode(AcceleratorCapabilityAdoptionMode mode);

		static bool IsKnownMetadataOnlyMode(AcceleratorCompatibilityMode mode);

		std::shared_ptr<AcceleratorCapabilityQueryResult> RecordQueryResult(std::shared_ptr<AcceleratorCapabilityQueryResult> result);
	};

	class AcceleratorCapabilityAcceptanceResult final : public std::enable_shared_from_this<AcceleratorCapabilityAcceptanceResult>
	{
	private:
		const bool IsAccepted;
		const std::shared_ptr<AcceleratorCapabilityQueryResult> QueryResult;
		const std::shared_ptr<AcceleratorGuardDecision> GuardDecision;
		const std::wstring RejectReason;

		AcceleratorCapabilityAcceptanceResult(bool isAccepted, std::shared_ptr<AcceleratorCapabilityQueryResult> queryResult, std::shared_ptr<AcceleratorGuardDecision> guardDecision, const std::wstring &rejectReason);

	public:
		bool isAccepted() const;

		bool isRejected() const;

		std::shared_ptr<AcceleratorCapabilityQueryResult> getQueryResult() const;

		std::shared_ptr<AcceleratorGuardDecision> getGuardDecision() const;

		std::wstring getRejectReason() const;

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public AcceleratorCapabilityDescriptor? Descriptor
		std::shared_ptr<AcceleratorCapabilityDescriptor> getDescriptor() const;

		bool getGrantsDecodeAuthority() const;

		bool getGrantsCommandSubmissionAuthority() const;

		bool getGrantsExecutionAuthority() const;

		bool getGrantsCommitAuthority() const;

		bool getGrantsTopologyQueryAuthority() const;

		bool getGrantsQueueOpenAuthority() const;

		bool getGrantsQueueBindAuthority() const;

		bool getGrantsQueueLifecycleAuthority() const;

		static std::shared_ptr<AcceleratorCapabilityAcceptanceResult> Accepted(std::shared_ptr<AcceleratorCapabilityQueryResult> queryResult, std::shared_ptr<AcceleratorGuardDecision> guardDecision);

		static std::shared_ptr<AcceleratorCapabilityAcceptanceResult> Reject(std::shared_ptr<AcceleratorCapabilityQueryResult> queryResult, std::shared_ptr<AcceleratorGuardDecision> guardDecision);
	};

	class AcceleratorCapabilityQueryResult final : public std::enable_shared_from_this<AcceleratorCapabilityQueryResult>
	{
	private:
		const bool IsMetadataAvailable;
		const std::shared_ptr<AcceleratorCapabilityDescriptor> Descriptor;
		const std::wstring RejectReason;

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private AcceleratorCapabilityQueryResult(bool isMetadataAvailable, AcceleratorCapabilityDescriptor? descriptor, string rejectReason)
		AcceleratorCapabilityQueryResult(bool isMetadataAvailable, std::shared_ptr<AcceleratorCapabilityDescriptor> descriptor, const std::wstring &rejectReason);

	public:
		bool isMetadataAvailable() const;

		bool isRejected() const;

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public AcceleratorCapabilityDescriptor? Descriptor {get;}
		std::shared_ptr<AcceleratorCapabilityDescriptor> getDescriptor() const;

		std::wstring getRejectReason() const;

		bool getGrantsDecodeAuthority() const;

		bool getGrantsCommandSubmissionAuthority() const;

		bool getGrantsExecutionAuthority() const;

		bool getGrantsCommitAuthority() const;

		bool getGrantsTopologyQueryAuthority() const;

		bool getGrantsQueueOpenAuthority() const;

		bool getGrantsQueueBindAuthority() const;

		bool getGrantsQueueLifecycleAuthority() const;

		static std::shared_ptr<AcceleratorCapabilityQueryResult> MetadataAvailable(std::shared_ptr<AcceleratorCapabilityDescriptor> descriptor);

		static std::shared_ptr<AcceleratorCapabilityQueryResult> Reject(const std::wstring &rejectReason);
	};

}

