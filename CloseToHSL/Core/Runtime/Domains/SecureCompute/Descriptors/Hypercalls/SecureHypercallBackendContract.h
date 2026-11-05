#pragma once

#include "../../Authority/SecureRevocationEpoch.h"
#include "../../Authority/SecureGrantHandle.h"
#include "../Backend/SecureBackendOwnerDescriptor.h"
#include "../Io/YAKSys_Hybrid_CPU.Core.SecureIoDomainDescriptor.h"
#include <memory>
#include <optional>
#include <vector>
#include <utility>

namespace YAKSys_Hybrid_CPU::Core
{

struct SecureHypercallTransportOpcode final { unsigned short Value = 0; explicit SecureHypercallTransportOpcode(unsigned short value = 0) : Value(value) {} };

struct SecureHypercallDecodedLeaf final { unsigned long long Value = 0; bool IsMaterialized = false; explicit SecureHypercallDecodedLeaf(unsigned long long value = 0) : Value(value), IsMaterialized(value != 0) {} };
struct SecureComputeServiceId final { unsigned long long Value = 0; bool IsMaterialized = false; explicit SecureComputeServiceId(unsigned long long value = 0) : Value(value), IsMaterialized(value != 0) {} };
struct SecureBackendOwnerId final { unsigned long long Value = 0; bool IsMaterialized = false; explicit SecureBackendOwnerId(unsigned long long value = 0) : Value(value), IsMaterialized(value != 0) {} };
struct SecureHypercallContractVersion final { unsigned short Major = 0; unsigned short Minor = 0; bool IsMaterialized = false; SecureHypercallContractVersion(unsigned short major = 0, unsigned short minor = 0) : Major(major), Minor(minor), IsMaterialized(major != 0) {} };

//	public readonly record struct SecureHypercallTransportOpcode(ushort Value);

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct SecureHypercallDecodedLeaf(ulong Value)
//	{
//		public bool IsMaterialized
//		{
//			get
//			{
//				return Value != 0;
//			}
//		}
//	}

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct SecureComputeServiceId(ulong Value)
//	{
//		public bool IsMaterialized
//		{
//			get
//			{
//				return Value != 0;
//			}
//		}
//	}

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct SecureBackendOwnerId(ulong Value)
//	{
//		public bool IsMaterialized
//		{
//			get
//			{
//				return Value != 0;
//			}
//		}
//	}

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct SecureHypercallContractVersion(ushort Major, ushort Minor)
//	{
//		public bool IsMaterialized
//		{
//			get
//			{
//				return Major != 0;
//			}
//		}
//	}

	enum class SecureHypercallArgumentOwnership : unsigned char
	{
		GuestImmediate = 0,
		ExplicitSharedBuffer = 1,
		OpaqueRuntimeHandle = 2,
		RawHostPointerDenied = 3
	};

	enum class SecureHypercallReplayPolicy : unsigned char
	{
		DenyReplay = 0,
		IdempotentRetryWithMatchingToken = 1
	};

	enum class SecureHypercallCancellationPolicy : unsigned char
	{
		DenyBeforeExecution = 0
	};

	enum class SecureHypercallRequestMigrationClass : unsigned char
	{
		NonMigratableInFlight = 0,
		DescriptorOnlyRevalidatedAfterRestore = 1
	};

	enum class SecureHypercallResultMigrationClass : unsigned char
	{
		NoResultBeforeExecution = 0,
		RecomputedAfterRestore = 1
	};

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
struct SecureHypercallBackendArgument final {
    unsigned char Index = 0;
    SecureHypercallArgumentOwnership Ownership = SecureHypercallArgumentOwnership::GuestImmediate;
    unsigned long long Value = 0;
    unsigned long long Length = 0;
    SecureGrantHandle Grant;
    bool IsRawPointer = false;
    bool IsSharedBuffer = false;
    bool IsOpaqueHandle = false;
    SecureHypercallBackendArgument(unsigned char index = 0, SecureHypercallArgumentOwnership ownership = SecureHypercallArgumentOwnership::GuestImmediate,
        unsigned long long value = 0, unsigned long long length = 0, SecureGrantHandle grant = {})
        : Index(index), Ownership(ownership), Value(value), Length(length), Grant(grant),
          IsRawPointer(ownership == SecureHypercallArgumentOwnership::RawHostPointerDenied),
          IsSharedBuffer(ownership == SecureHypercallArgumentOwnership::ExplicitSharedBuffer),
          IsOpaqueHandle(ownership == SecureHypercallArgumentOwnership::OpaqueRuntimeHandle) {}
};

	class SecureHypercallBackendContractDescriptor final : public std::enable_shared_from_this<SecureHypercallBackendContractDescriptor>
	{
	private:
		static inline const std::shared_ptr<SecureHypercallBackendContractDescriptor> Unresolved = std::make_shared<SecureHypercallBackendContractDescriptor>(std::make_shared<SecureHypercallDecodedLeaf>(), std::make_shared<SecureComputeServiceId>(), std::make_shared<SecureBackendOwnerId>(), SecureRevocationEpoch::Unmaterialized, std::make_shared<SecureHypercallContractVersion>(), std::make_shared<SecureGrantHandle>(), SecureHypercallReplayPolicy::DenyReplay, SecureHypercallCancellationPolicy::DenyBeforeExecution, SecureHypercallRequestMigrationClass::NonMigratableInFlight, SecureHypercallResultMigrationClass::NoResultBeforeExecution);
		const std::shared_ptr<SecureHypercallDecodedLeaf> DecodedLeaf;
		const std::shared_ptr<SecureComputeServiceId> ServiceId;
		const std::shared_ptr<SecureBackendOwnerId> OwnerId;
		const std::shared_ptr<SecureRevocationEpoch> OwnerEpoch;
		const std::shared_ptr<SecureHypercallContractVersion> Version;
		const std::shared_ptr<SecureGrantHandle> RequiredGrant;
		const SecureHypercallReplayPolicy ReplayPolicy;
		const SecureHypercallCancellationPolicy CancellationPolicy;
		const SecureHypercallRequestMigrationClass RequestMigrationClass;
		const SecureHypercallResultMigrationClass ResultMigrationClass;

	public:
		SecureHypercallBackendContractDescriptor(std::shared_ptr<SecureHypercallDecodedLeaf> decodedLeaf, std::shared_ptr<SecureComputeServiceId> serviceId, std::shared_ptr<SecureBackendOwnerId> ownerId, std::shared_ptr<SecureRevocationEpoch> ownerEpoch, std::shared_ptr<SecureHypercallContractVersion> version, std::shared_ptr<SecureGrantHandle> requiredGrant, SecureHypercallReplayPolicy replayPolicy, SecureHypercallCancellationPolicy cancellationPolicy, SecureHypercallRequestMigrationClass requestMigrationClass, SecureHypercallResultMigrationClass resultMigrationClass);

		static std::shared_ptr<SecureHypercallBackendContractDescriptor> getUnresolved();

		std::shared_ptr<SecureHypercallDecodedLeaf> getDecodedLeaf() const;

		std::shared_ptr<SecureComputeServiceId> getServiceId() const;

		std::shared_ptr<SecureBackendOwnerId> getOwnerId() const;

		std::shared_ptr<SecureRevocationEpoch> getOwnerEpoch() const;

		std::shared_ptr<SecureHypercallContractVersion> getVersion() const;

		std::shared_ptr<SecureGrantHandle> getRequiredGrant() const;

		SecureHypercallReplayPolicy getReplayPolicy() const;

		SecureHypercallCancellationPolicy getCancellationPolicy() const;

		SecureHypercallRequestMigrationClass getRequestMigrationClass() const;

		SecureHypercallResultMigrationClass getResultMigrationClass() const;

		bool isMaterialized() const;
	};

struct SecureHypercallBackendContractRequest final {
    std::shared_ptr<SecureHypercallTransportOpcode> TransportOpcode;
    std::shared_ptr<SecureHypercallDecodedLeaf> DecodedLeaf;
    std::shared_ptr<SecureComputeServiceId> ServiceId;
    std::shared_ptr<SecureHypercallContractVersion> ContractVersion;
    std::optional<std::shared_ptr<SecureBackendOwnerDescriptor>> Owner;
    std::shared_ptr<SecureRevocationEpoch> CurrentEpoch = SecureRevocationEpoch::Unmaterialized;
    SecureGrantHandle PresentedGrant;
    bool EvidenceValidated = false;
    std::shared_ptr<SecureRevocationEpoch> EvidenceEpoch = SecureRevocationEpoch::Unmaterialized;
    std::shared_ptr<SecureIoDomainDescriptor> IoPolicy;
    unsigned long long ValidatedDomainTag = 0;
    std::vector<std::shared_ptr<SecureHypercallBackendArgument>> Arguments;
    bool IsReplay = false;
    bool IdempotentRetry = false;
    bool ReplayTokenMatches = false;
    bool CancellationRequested = false;
};

}
