#include "AcceleratorCapabilityRegistry.h"
#include "../Telemetry/AcceleratorTelemetry.h"
#include "../Auth/AcceleratorOwnerDomainGuard.h"
#include "../../../Pipeline/Safety/SafetyVerifier.Types.h"
#include "../Descriptors/AcceleratorDescriptorTaxonomyCatalog.h"

using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Auth;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Descriptors;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Telemetry;

namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Capabilities
{

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public AcceleratorCapabilityRegistry(AcceleratorTelemetry? telemetry = null)
	AcceleratorCapabilityRegistry::AcceleratorCapabilityRegistry(std::shared_ptr<AcceleratorTelemetry> telemetry) : _telemetry(telemetry)
	{
	}

	int AcceleratorCapabilityRegistry::getCount() const
	{
		return _descriptors.size();
	}

	void AcceleratorCapabilityRegistry::RegisterProvider(std::shared_ptr<IAcceleratorCapabilityProvider> provider)
	{
		if (!provider) throw std::invalid_argument("provider");

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: IReadOnlyList<AcceleratorCapabilityDescriptor> capabilities = provider.GetCapabilities();
		const std::vector<std::shared_ptr<AcceleratorCapabilityDescriptor>> capabilities = provider->GetCapabilities();
		

		for (auto descriptor : capabilities)
		{
			RegisterDescriptor(descriptor);
		}
	}

	void AcceleratorCapabilityRegistry::RegisterDescriptor(std::shared_ptr<AcceleratorCapabilityDescriptor> descriptor)
	{
		if (!descriptor) throw std::invalid_argument("descriptor");
		ValidateMetadataOnlyDescriptor(descriptor);

		if (!_descriptors.emplace(descriptor->getAcceleratorId(), descriptor).second)
		{
			throw std::logic_error("Accelerator capability metadata is already registered.");
		}
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public bool TryGetDescriptor(string acceleratorId, out AcceleratorCapabilityDescriptor? descriptor)
	bool AcceleratorCapabilityRegistry::TryGetDescriptor(const std::wstring &acceleratorId, std::shared_ptr<AcceleratorCapabilityDescriptor> &descriptor)
	{
		descriptor = nullptr;
		if (StringHelper::isEmptyOrWhiteSpace(acceleratorId))
		{
			return false;
		}

		auto it = _descriptors.find(acceleratorId);
		if (it == _descriptors.end()) return false;
		descriptor = it->second;
		return true;
	}

	const std::vector<std::shared_ptr<AcceleratorCapabilityDescriptor>> AcceleratorCapabilityRegistry::GetAllDescriptors()
	{
		std::vector<std::shared_ptr<AcceleratorCapabilityDescriptor>> result;
		result.reserve(_descriptors.size());
		for (const auto& [id, descriptor] : _descriptors) result.push_back(descriptor);
		return result;
	}

	std::shared_ptr<AcceleratorCapabilityQueryResult> AcceleratorCapabilityRegistry::Query(const std::wstring &acceleratorId, AcceleratorCapabilityAdoptionMode adoptionMode, AcceleratorCompatibilityMode compatibilityMode)
	{
		if (!IsKnownMetadataOnlyMode(adoptionMode))
		{
			return RecordQueryResult(AcceleratorCapabilityQueryResult::Reject(std::format(L"Unknown or unsupported accelerator capability adoption mode '{0}'.", static_cast<int>(adoptionMode))));
		}

		if (!IsKnownMetadataOnlyMode(compatibilityMode))
		{
			return RecordQueryResult(AcceleratorCapabilityQueryResult::Reject(std::format(L"Unknown or unsupported accelerator compatibility mode '{0}'.", static_cast<int>(compatibilityMode))));
		}

		std::shared_ptr<AcceleratorCapabilityDescriptor> descriptor;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: if (!TryGetDescriptor(acceleratorId, out AcceleratorCapabilityDescriptor? descriptor))
		if (!TryGetDescriptor(acceleratorId, descriptor))
		{
			return RecordQueryResult(AcceleratorCapabilityQueryResult::Reject(std::format(L"Unknown accelerator id '{0:s}'.", acceleratorId)));
		}

	// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
	// ORIGINAL LINE: return RecordQueryResult( AcceleratorCapabilityQueryResult.MetadataAvailable(descriptor!));
		return RecordQueryResult(AcceleratorCapabilityQueryResult::MetadataAvailable(descriptor));
	}

	std::shared_ptr<AcceleratorCapabilityAcceptanceResult> AcceleratorCapabilityRegistry::AcceptCapability(const std::wstring &acceleratorId, std::shared_ptr<AcceleratorOwnerBinding> descriptorOwnerBinding, AcceleratorCapabilityAdoptionMode adoptionMode, AcceleratorCompatibilityMode compatibilityMode)
	{
		ArgumentNullException::ThrowIfNull(descriptorOwnerBinding);
		std::shared_ptr<AcceleratorCapabilityQueryResult> query = Query(acceleratorId, adoptionMode, compatibilityMode);
		return AcceleratorCapabilityAcceptanceResult::Reject(query, AcceleratorGuardDecision::Reject(AcceleratorGuardSurface::CapabilityAcceptance, AcceleratorGuardFault::CapabilityGuardMissing, descriptorOwnerBinding, nullptr, YAKSys_Hybrid_CPU::Core::RejectKind::OwnerMismatch, L"L7-SDC capability acceptance requires guard-plane owner/domain evidence; registry metadata is not authority."));
	}

	std::shared_ptr<AcceleratorCapabilityAcceptanceResult> AcceleratorCapabilityRegistry::AcceptCapability(const std::wstring &acceleratorId, std::shared_ptr<AcceleratorOwnerBinding> descriptorOwnerBinding, std::shared_ptr<AcceleratorGuardDecision> descriptorGuardDecision, AcceleratorCapabilityAdoptionMode adoptionMode, AcceleratorCompatibilityMode compatibilityMode)
	{
		ArgumentNullException::ThrowIfNull(descriptorOwnerBinding);

		std::shared_ptr<AcceleratorGuardDecision> capabilityGuard = AcceleratorOwnerDomainGuard::getDefault()->EnsureBeforeCapabilityAcceptance(descriptorOwnerBinding, descriptorGuardDecision);
		std::shared_ptr<AcceleratorCapabilityQueryResult> query = Query(acceleratorId, adoptionMode, compatibilityMode);

		if (!capabilityGuard->IsAllowed)
		{
			return AcceleratorCapabilityAcceptanceResult::Reject(query, capabilityGuard);
		}

		return query->isMetadataAvailable() ? AcceleratorCapabilityAcceptanceResult::Accepted(query, capabilityGuard) : AcceleratorCapabilityAcceptanceResult::Reject(query, capabilityGuard);
	}

	void AcceleratorCapabilityRegistry::ValidateMetadataOnlyDescriptor(std::shared_ptr<AcceleratorCapabilityDescriptor> descriptor)
	{
		if (!IsKnownMetadataOnlyMode(descriptor->getAdoptionMode()))
		{
// C# TO C++ CONVERTER TASK: This exception's constructor requires only one argument:
// ORIGINAL LINE: throw new ArgumentException(string.Format("Unknown or unsupported accelerator capability adoption mode '{0}'.", (int)descriptor.AdoptionMode), nameof(descriptor));
			throw std::invalid_argument(std::format("Unknown or unsupported accelerator capability adoption mode '{0}'.", static_cast<int>(descriptor->getAdoptionMode())));
		}

		if (!IsKnownMetadataOnlyMode(descriptor->getCompatibilityMode()))
		{
// C# TO C++ CONVERTER TASK: This exception's constructor requires only one argument:
// ORIGINAL LINE: throw new ArgumentException(string.Format("Unknown or unsupported accelerator compatibility mode '{0}'.", (int)descriptor.CompatibilityMode), nameof(descriptor));
			throw std::invalid_argument(std::format("Unknown or unsupported accelerator compatibility mode '{0}'.", static_cast<int>(descriptor->getCompatibilityMode())));
		}

		std::shared_ptr<AcceleratorDescriptorTaxonomyEntry> taxonomyEntry;
		if (!AcceleratorDescriptorTaxonomyCatalog::TryGetEntry(descriptor->getTaxonomyKey(), taxonomyEntry) || taxonomyEntry->getStatus() != descriptor->getTaxonomyStatus())
		{
// C# TO C++ CONVERTER TASK: This exception's constructor requires only one argument:
// ORIGINAL LINE: throw new ArgumentException("Accelerator capability metadata must reference a known Lane7 descriptor taxonomy entry with the catalog-pinned status.", nameof(descriptor));
			throw std::invalid_argument("Accelerator capability metadata must reference a known Lane7 descriptor taxonomy entry with the catalog-pinned status.");
		}
	}

	bool AcceleratorCapabilityRegistry::IsKnownMetadataOnlyMode(AcceleratorCapabilityAdoptionMode mode)
	{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'typeof' operator:
		return mode == AcceleratorCapabilityAdoptionMode::MetadataOnly;
	}

	bool AcceleratorCapabilityRegistry::IsKnownMetadataOnlyMode(AcceleratorCompatibilityMode mode)
	{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'typeof' operator:
		return mode == AcceleratorCompatibilityMode::MetadataOnly;
	}

	std::shared_ptr<AcceleratorCapabilityQueryResult> AcceleratorCapabilityRegistry::RecordQueryResult(std::shared_ptr<AcceleratorCapabilityQueryResult> result)
	{
		if (_telemetry != nullptr)
		{
			_telemetry->RecordCapabilityQuery(result->isMetadataAvailable(), result->isMetadataAvailable() ? L"L7-SDC capability metadata query succeeded." : result->getRejectReason());
		}
		return result;
	}

	AcceleratorCapabilityAcceptanceResult::AcceleratorCapabilityAcceptanceResult(bool isAccepted, std::shared_ptr<AcceleratorCapabilityQueryResult> queryResult, std::shared_ptr<AcceleratorGuardDecision> guardDecision, const std::wstring &rejectReason) : IsAccepted(isAccepted), QueryResult(std::move(queryResult)), GuardDecision(std::move(guardDecision)), RejectReason(rejectReason) {}

	bool AcceleratorCapabilityAcceptanceResult::isAccepted() const
	{
		return IsAccepted;
	}

	bool AcceleratorCapabilityAcceptanceResult::isRejected() const
	{
		return !isAccepted();
	}

	std::shared_ptr<AcceleratorCapabilityQueryResult> AcceleratorCapabilityAcceptanceResult::getQueryResult() const
	{
		return QueryResult;
	}

	std::shared_ptr<AcceleratorGuardDecision> AcceleratorCapabilityAcceptanceResult::getGuardDecision() const
	{
		return GuardDecision;
	}

	std::wstring AcceleratorCapabilityAcceptanceResult::getRejectReason() const
	{
		return RejectReason;
	}

	std::shared_ptr<AcceleratorCapabilityDescriptor> AcceleratorCapabilityAcceptanceResult::getDescriptor() const
	{
		return getQueryResult()->getDescriptor();
	}

	bool AcceleratorCapabilityAcceptanceResult::getGrantsDecodeAuthority() const
	{
		return false;
	}

	bool AcceleratorCapabilityAcceptanceResult::getGrantsCommandSubmissionAuthority() const
	{
		return false;
	}

	bool AcceleratorCapabilityAcceptanceResult::getGrantsExecutionAuthority() const
	{
		return false;
	}

	bool AcceleratorCapabilityAcceptanceResult::getGrantsCommitAuthority() const
	{
		return false;
	}

	bool AcceleratorCapabilityAcceptanceResult::getGrantsTopologyQueryAuthority() const
	{
		return false;
	}

	bool AcceleratorCapabilityAcceptanceResult::getGrantsQueueOpenAuthority() const
	{
		return false;
	}

	bool AcceleratorCapabilityAcceptanceResult::getGrantsQueueBindAuthority() const
	{
		return false;
	}

	bool AcceleratorCapabilityAcceptanceResult::getGrantsQueueLifecycleAuthority() const
	{
		return false;
	}

	std::shared_ptr<AcceleratorCapabilityAcceptanceResult> AcceleratorCapabilityAcceptanceResult::Accepted(std::shared_ptr<AcceleratorCapabilityQueryResult> queryResult, std::shared_ptr<AcceleratorGuardDecision> guardDecision)
	{
		ArgumentNullException::ThrowIfNull(queryResult);
		if (!queryResult->isMetadataAvailable())
		{
// C# TO C++ CONVERTER TASK: This exception's constructor requires only one argument:
// ORIGINAL LINE: throw new ArgumentException("Accepted capability results require metadata to be available.", nameof(queryResult));
			throw std::invalid_argument("Accepted capability results require metadata to be available.");
		}

		if (!guardDecision->IsAllowed)
		{
// C# TO C++ CONVERTER TASK: This exception's constructor requires only one argument:
// ORIGINAL LINE: throw new ArgumentException("Accepted capability results require an allowed guard decision.", nameof(guardDecision));
			throw std::invalid_argument("Accepted capability results require an allowed guard decision.");
		}

		return std::shared_ptr<AcceleratorCapabilityAcceptanceResult>(new AcceleratorCapabilityAcceptanceResult(true, queryResult, guardDecision, L""));
	}

	std::shared_ptr<AcceleratorCapabilityAcceptanceResult> AcceleratorCapabilityAcceptanceResult::Reject(std::shared_ptr<AcceleratorCapabilityQueryResult> queryResult, std::shared_ptr<AcceleratorGuardDecision> guardDecision)
	{
		ArgumentNullException::ThrowIfNull(queryResult);
		std::wstring rejectReason = !StringHelper::isEmptyOrWhiteSpace(queryResult->getRejectReason()) ? queryResult->getRejectReason() : guardDecision->Message;
		return std::shared_ptr<AcceleratorCapabilityAcceptanceResult>(new AcceleratorCapabilityAcceptanceResult(false, queryResult, guardDecision, rejectReason));
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private AcceleratorCapabilityQueryResult(bool isMetadataAvailable, AcceleratorCapabilityDescriptor? descriptor, string rejectReason)
	AcceleratorCapabilityQueryResult::AcceleratorCapabilityQueryResult(bool isMetadataAvailable, std::shared_ptr<AcceleratorCapabilityDescriptor> descriptor, const std::wstring &rejectReason) : IsMetadataAvailable(isMetadataAvailable), Descriptor(std::move(descriptor)), RejectReason(rejectReason) {}

	bool AcceleratorCapabilityQueryResult::isMetadataAvailable() const
	{
		return IsMetadataAvailable;
	}

	bool AcceleratorCapabilityQueryResult::isRejected() const
	{
		return !isMetadataAvailable();
	}

	std::shared_ptr<AcceleratorCapabilityDescriptor> AcceleratorCapabilityQueryResult::getDescriptor() const
	{
		return Descriptor;
	}

	std::wstring AcceleratorCapabilityQueryResult::getRejectReason() const
	{
		return RejectReason;
	}

	bool AcceleratorCapabilityQueryResult::getGrantsDecodeAuthority() const
	{
		return false;
	}

	bool AcceleratorCapabilityQueryResult::getGrantsCommandSubmissionAuthority() const
	{
		return false;
	}

	bool AcceleratorCapabilityQueryResult::getGrantsExecutionAuthority() const
	{
		return false;
	}

	bool AcceleratorCapabilityQueryResult::getGrantsCommitAuthority() const
	{
		return false;
	}

	bool AcceleratorCapabilityQueryResult::getGrantsTopologyQueryAuthority() const
	{
		return false;
	}

	bool AcceleratorCapabilityQueryResult::getGrantsQueueOpenAuthority() const
	{
		return false;
	}

	bool AcceleratorCapabilityQueryResult::getGrantsQueueBindAuthority() const
	{
		return false;
	}

	bool AcceleratorCapabilityQueryResult::getGrantsQueueLifecycleAuthority() const
	{
		return false;
	}

	std::shared_ptr<AcceleratorCapabilityQueryResult> AcceleratorCapabilityQueryResult::MetadataAvailable(std::shared_ptr<AcceleratorCapabilityDescriptor> descriptor)
	{
		if (!descriptor) throw std::invalid_argument("descriptor");
		return std::shared_ptr<AcceleratorCapabilityQueryResult>(new AcceleratorCapabilityQueryResult(true, descriptor, L""));
	}

	std::shared_ptr<AcceleratorCapabilityQueryResult> AcceleratorCapabilityQueryResult::Reject(const std::wstring &rejectReason)
	{
		if (StringHelper::isEmptyOrWhiteSpace(rejectReason))
		{
// C# TO C++ CONVERTER TASK: This exception's constructor requires only one argument:
// ORIGINAL LINE: throw new ArgumentException("Reject reason is required.", nameof(rejectReason));
			throw std::invalid_argument("Reject reason is required.");
		}

		return std::shared_ptr<AcceleratorCapabilityQueryResult>(new AcceleratorCapabilityQueryResult(false, nullptr, rejectReason));
	}
}
