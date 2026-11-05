#pragma once

#include "ExternalSecureRegisterLocalContracts.h"
#include <any>
#include <memory>
#include <limits>
#include <stdexcept>
#include <utility>

namespace HybridCPU::ExternalRuntime::Contracts
{
    // Host-owned memory object identity. Binding itself grants no read or CPU authority.
    class ExternalSecureMemoryRegionBinding final
    {
    public:
        const ExternalSecureExecutionBinding Execution;
        const Guid Region;
        const unsigned long long Generation;
        const unsigned long long Address;
        const unsigned long long Length;
        const unsigned long long AddressSpaceIdentity;
        static constexpr unsigned int SchemaVersion = 1;

        ExternalSecureMemoryRegionBinding(ExternalSecureExecutionBinding execution, Guid region,
            unsigned long long generation, unsigned long long address, unsigned long long length,
            unsigned long long addressSpaceIdentity, unsigned int schemaVersion = 1)
            : Execution(std::move(execution)), Region(std::move(region)), Generation(generation),
              Address(address), Length(length), AddressSpaceIdentity(addressSpaceIdentity)
        {
            if (schemaVersion != SchemaVersion || Region == Guid::Empty || Generation == 0 || Length == 0 ||
                Address > (std::numeric_limits<unsigned long long>::max)() - Length || AddressSpaceIdentity == 0)
                throw std::invalid_argument("Exact bounded memory object required.");
        }
    };

    class ExternalSecureMemoryReadRequest final
    {
    public:
        const std::shared_ptr<ExternalSecureMemoryRegionBinding> Region;
        const ExternalSecureExecutionBinding Binding;
        const ExternalOperationIdentity Operation;
        const unsigned long long Address;
        static constexpr unsigned int SchemaVersion = 1;
        ExternalSecureMemoryReadRequest(std::shared_ptr<ExternalSecureMemoryRegionBinding> region,
            ExternalOperationIdentity operation, unsigned long long address, unsigned int schemaVersion = 1)
            : Region(std::move(region)), Binding(Region ? Region->Execution : ExternalSecureExecutionBinding{}),
              Operation(std::move(operation)), Address(address)
        {
            if (!Region) throw std::invalid_argument("region");
            if (schemaVersion != SchemaVersion || Operation.Handle.Value == Guid::Empty || Operation.Generation.Value == 0)
                throw std::invalid_argument("Original memory read operation required.");
        }
        int getSize() const noexcept { return 8; }
    };

    class ExternalSecureMemoryReadReservation final
    {
    public:
        const std::shared_ptr<ExternalSecureMemoryReadRequest> Request;
        const Guid Handle;
        ExternalSecureMemoryReadReservation(std::shared_ptr<ExternalSecureMemoryReadRequest> request, Guid handle)
            : Request(std::move(request)), Handle(std::move(handle))
        {
            if (!Request) throw std::invalid_argument("request");
            if (Handle == Guid::Empty) throw std::invalid_argument("Original hold required.");
        }
    };

    class ExternalSecureMemoryReadReservationResult final
    {
    public:
        const ExternalSecureDomainProviderStatus Status;
        const std::shared_ptr<ExternalSecureMemoryReadReservation> Reservation;
        const std::shared_ptr<ExternalSecureMemoryReadRequest> RefusedRequest;
        ExternalSecureMemoryReadReservationResult(ExternalSecureDomainProviderStatus status,
            std::shared_ptr<ExternalSecureMemoryReadReservation> reservation = nullptr,
            std::shared_ptr<ExternalSecureMemoryReadRequest> refusedRequest = nullptr)
            : Status(status), Reservation(std::move(reservation)), RefusedRequest(std::move(refusedRequest))
        {
            if (!IsDefined(Status) || (Reservation && RefusedRequest) ||
                (RefusedRequest && Status != ExternalSecureDomainProviderStatus::Rejected))
                throw std::invalid_argument("Exact memory reservation outcome required.");
        }
    };

    class ExternalSecureMemoryReadAttempt final
    {
    public:
        const std::shared_ptr<ExternalSecureMemoryReadReservation> Reservation;
        const std::shared_ptr<ExternalSecureMemoryReadRequest> Request;
        const Guid MachineScope;
        const unsigned long long MachineGeneration, CpuAttempt, WorkingBundle;
        const int VirtualThread, OwnerContext, SourceSlot, WorkingSlot, PhysicalLane;
        static constexpr unsigned int SchemaVersion = 1;
        ExternalSecureMemoryReadAttempt(std::shared_ptr<ExternalSecureMemoryReadReservation> reservation,
            Guid machineScope, unsigned long long machineGeneration, unsigned long long cpuAttempt,
            unsigned long long workingBundle, int virtualThread, int ownerContext, int sourceSlot,
            int workingSlot, int physicalLane, unsigned int schemaVersion = 1)
            : Reservation(std::move(reservation)), Request(Reservation ? Reservation->Request : nullptr),
              MachineScope(std::move(machineScope)), MachineGeneration(machineGeneration), CpuAttempt(cpuAttempt),
              WorkingBundle(workingBundle), VirtualThread(virtualThread), OwnerContext(ownerContext),
              SourceSlot(sourceSlot), WorkingSlot(workingSlot), PhysicalLane(physicalLane)
        {
            if (!Reservation) throw std::invalid_argument("reservation");
            if (schemaVersion != SchemaVersion || MachineScope == Guid::Empty || MachineGeneration == 0 ||
                CpuAttempt == 0 || WorkingBundle == 0 || VirtualThread < 0 || VirtualThread >= 4 ||
                OwnerContext < 0 || SourceSlot < 0 || SourceSlot >= 8 || WorkingSlot < 0 || WorkingSlot >= 8 ||
                PhysicalLane < 0 || PhysicalLane >= 8)
                throw std::invalid_argument("Exact memory attempt required.");
        }
    };

    class ExternalSecureMemoryReadConsumptionReceipt final
    {
    public:
        const std::shared_ptr<ExternalSecureMemoryReadAttempt> Attempt;
        const Guid ProviderScope;
        ExternalSecureMemoryReadConsumptionReceipt(std::shared_ptr<ExternalSecureMemoryReadAttempt> attempt, Guid providerScope)
            : Attempt(std::move(attempt)), ProviderScope(std::move(providerScope))
        {
            if (!Attempt) throw std::invalid_argument("attempt");
            if (ProviderScope == Guid::Empty) throw std::invalid_argument("Original provider required.");
        }
    };

    class ExternalSecureMemoryReadConsumptionResult final
    {
    public:
        const std::shared_ptr<ExternalSecureMemoryReadAttempt> Attempt;
        const ExternalSecureDomainProviderStatus Status;
        const std::shared_ptr<ExternalSecureMemoryReadConsumptionReceipt> Receipt;
        ExternalSecureMemoryReadConsumptionResult(std::shared_ptr<ExternalSecureMemoryReadAttempt> attempt,
            ExternalSecureDomainProviderStatus status, std::shared_ptr<ExternalSecureMemoryReadConsumptionReceipt> receipt = nullptr)
            : Attempt(std::move(attempt)), Status(status), Receipt(std::move(receipt))
        {
            if (!Attempt) throw std::invalid_argument("attempt");
            if (!IsDefined(Status) || (Status == ExternalSecureDomainProviderStatus::Receipt) != static_cast<bool>(Receipt) ||
                (Receipt && Receipt->Attempt != Attempt))
                throw std::invalid_argument("Original memory consume outcome required.");
        }
    };

    class ExternalSecureMemoryReadSubmission final
    {
    public:
        const std::shared_ptr<ExternalSecureMemoryReadConsumptionReceipt> Consumption;
        const std::shared_ptr<ExternalSecureMemoryReadAttempt> Attempt;
        explicit ExternalSecureMemoryReadSubmission(std::shared_ptr<ExternalSecureMemoryReadConsumptionReceipt> consumption)
            : Consumption(std::move(consumption)), Attempt(Consumption ? Consumption->Attempt : nullptr)
        {
            if (!Consumption) throw std::invalid_argument("consumption");
        }
    };

    class ExternalSecureMemoryReadReceipt final
    {
    public:
        const std::shared_ptr<ExternalSecureMemoryReadSubmission> Submission;
        const unsigned long long Value, SourceEpoch;
        ExternalSecureMemoryReadReceipt(std::shared_ptr<ExternalSecureMemoryReadSubmission> submission,
            unsigned long long value, unsigned long long sourceEpoch)
            : Submission(std::move(submission)), Value(value), SourceEpoch(sourceEpoch)
        {
            if (!Submission) throw std::invalid_argument("submission");
            if (SourceEpoch == 0) throw std::invalid_argument("Original source epoch required.");
        }
    };

    class ExternalSecureMemoryReadResult final
    {
    public:
        const std::shared_ptr<ExternalSecureMemoryReadSubmission> Submission;
        const ExternalSecureDomainProviderStatus Status;
        const std::shared_ptr<ExternalSecureMemoryReadReceipt> Receipt;
        ExternalSecureMemoryReadResult(std::shared_ptr<ExternalSecureMemoryReadSubmission> submission,
            ExternalSecureDomainProviderStatus status, std::shared_ptr<ExternalSecureMemoryReadReceipt> receipt = nullptr)
            : Submission(std::move(submission)), Status(status), Receipt(std::move(receipt))
        {
            if (!Submission) throw std::invalid_argument("submission");
            if (!IsDefined(Status) || (Status == ExternalSecureDomainProviderStatus::Receipt) != static_cast<bool>(Receipt) ||
                (Receipt && Receipt->Submission != Submission))
                throw std::invalid_argument("Original memory read outcome required.");
        }
    };

    class IExternalSecureMemoryReadProvider
    {
    public:
        virtual ~IExternalSecureMemoryReadProvider() = default;
        virtual std::shared_ptr<ExternalSecureMemoryReadReservationResult> ReserveMemoryRead(
            std::shared_ptr<ExternalSecureMemoryReadRequest> request) = 0;
        virtual std::shared_ptr<ExternalSecureMemoryReadReservationResult> QueryMemoryReadReservation(
            std::shared_ptr<ExternalSecureMemoryReadRequest> request) = 0;
        virtual std::shared_ptr<ExternalSecureMemoryReadConsumptionResult> ConsumeMemoryRead(
            std::shared_ptr<ExternalSecureMemoryReadAttempt> attempt) = 0;
        virtual std::shared_ptr<ExternalSecureMemoryReadConsumptionResult> QueryMemoryReadConsumption(
            std::shared_ptr<ExternalSecureMemoryReadAttempt> attempt) = 0;
        virtual std::shared_ptr<ExternalSecureMemoryReadResult> SubmitMemoryRead(
            std::shared_ptr<ExternalSecureMemoryReadSubmission> submission) = 0;
        virtual std::shared_ptr<ExternalSecureMemoryReadResult> QueryMemoryRead(
            std::shared_ptr<ExternalSecureMemoryReadSubmission> submission) = 0;
        virtual std::shared_ptr<ExternalSecureMemoryReadReservationResult> ReleaseMemoryRead(
            std::shared_ptr<ExternalSecureMemoryReadReservation> reservation) = 0;
        virtual std::shared_ptr<ExternalSecureMemoryReadReservationResult> QueryMemoryReadRelease(
            std::shared_ptr<ExternalSecureMemoryReadReservation> reservation) = 0;
    };

    class IExternalSecureMemoryReadOrigin
    {
    public:
        std::shared_ptr<ExternalSecureMemoryRegionBinding> Region;
        std::any SynchronizationRoot, ProviderSynchronizationRoot, MemorySynchronizationRoot;
        virtual ~IExternalSecureMemoryReadOrigin() = default;
        virtual bool IsCurrent(std::shared_ptr<ExternalSecureMemoryReadReservation> reservation) = 0;
        virtual bool IsOriginalRead(std::shared_ptr<ExternalSecureMemoryReadReceipt> receipt) = 0;
    };

    class IExternalSecureMemoryReadOriginProvider
    {
    public:
        virtual ~IExternalSecureMemoryReadOriginProvider() = default;
        virtual std::shared_ptr<IExternalSecureMemoryReadOrigin> ResolveMemoryReadOrigin(
            std::shared_ptr<ExternalSecureMemoryReadReservation> reservation) = 0;
    };
}


