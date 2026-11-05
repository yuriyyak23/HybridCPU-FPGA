#pragma once

#include "ExternalOperationIdentity.h"
#include "ExternalSecureExecutionConsumptionProvider.h"
#include "../../Guid.h"
#include <memory>
#include <stdexcept>
#include <utility>

namespace HybridCPU::ExternalRuntime::Contracts
{
	enum class ExternalSecureDomainProviderStatus : unsigned char
	{
		Receipt = 1,
		Unavailable = 2,
		Faulted = 3,
		Stale = 4,
		Rejected = 5
	};

	inline bool IsDefined(ExternalSecureDomainProviderStatus status) noexcept
	{
		return status == ExternalSecureDomainProviderStatus::Receipt ||
			status == ExternalSecureDomainProviderStatus::Unavailable ||
			status == ExternalSecureDomainProviderStatus::Faulted ||
			status == ExternalSecureDomainProviderStatus::Stale ||
			status == ExternalSecureDomainProviderStatus::Rejected;
	}

	struct ExternalSecureDomainHandle { Guid Value; };
	struct ExternalSecureDomainGeneration { unsigned long long Value = 0; };
	struct ExternalSecureDomainLease
	{
		ExternalSecureDomainHandle Handle;
		ExternalSecureDomainGeneration Generation;
		bool Equals(const ExternalSecureDomainLease &other) const { return Handle.Value == other.Handle.Value && Generation.Value == other.Generation.Value; }
		bool operator==(const ExternalSecureDomainLease &other) const { return Equals(other); }
	};
	struct ExternalSecureExecutionBinding { ExternalSecureDomainLease SecureDomain; };
	class ExternalSecureRegisterLocalRequest
	{
	public:
		ExternalSecureExecutionBinding Binding;
		ExternalOperationIdentity Operation;
		unsigned long long Left = 0, Right = 0;
		unsigned int SchemaVersion = 1;
	};
	class ExternalSecureExecutionReservation
	{
	public:
		const std::shared_ptr<ExternalSecureRegisterLocalRequest> Request;
		const Guid Handle;

		ExternalSecureExecutionReservation(std::shared_ptr<ExternalSecureRegisterLocalRequest> request, Guid handle)
			: Request(std::move(request)), Handle(std::move(handle))
		{
			if (!Request) throw std::invalid_argument("request");
			if (Handle == Guid::Empty) throw std::invalid_argument("Nonempty reservation handle required.");
		}

		std::shared_ptr<ExternalSecureRegisterLocalRequest> getRequest() const { return Request; }
		Guid getHandle() const { return Handle; }
	};

	/// <summary>Exact correlation of the original hold with a bounded machine placement attempt.</summary>
	class ExternalSecureExecutionAttempt final
	{
	public:
		const std::shared_ptr<ExternalSecureExecutionReservation> Reservation;
		const std::shared_ptr<ExternalSecureRegisterLocalRequest> Request;
		const Guid MachineScope;
		const unsigned long long MachineGeneration;
		const unsigned long long CpuAttempt;
		const unsigned long long WorkingBundle;
		const int VirtualThread;
		const int OwnerContext;
		const int SourceSlot;
		const int WorkingSlot;
		const int PhysicalLane;
		const unsigned int SchemaVersion;

		ExternalSecureExecutionAttempt(std::shared_ptr<ExternalSecureExecutionReservation> reservation,
			Guid machineScope, unsigned long long machineGeneration, unsigned long long cpuAttempt,
			unsigned long long workingBundle, int virtualThread, int ownerContext,
			int sourceSlot, int workingSlot, int physicalLane, unsigned int schemaVersion = 1)
			: Reservation(std::move(reservation)),
			  Request(Reservation ? Reservation->Request : nullptr), MachineScope(std::move(machineScope)),
			  MachineGeneration(machineGeneration), CpuAttempt(cpuAttempt), WorkingBundle(workingBundle),
			  VirtualThread(virtualThread), OwnerContext(ownerContext), SourceSlot(sourceSlot),
			  WorkingSlot(workingSlot), PhysicalLane(physicalLane), SchemaVersion(schemaVersion)
		{
			if (!Reservation) throw std::invalid_argument("reservation");
			if (SchemaVersion != 1) throw std::out_of_range("schemaVersion");
			if (MachineScope == Guid::Empty || MachineGeneration == 0 || CpuAttempt == 0 || WorkingBundle == 0 ||
				VirtualThread < 0 || VirtualThread >= 4 || OwnerContext < 0 ||
				SourceSlot < 0 || SourceSlot >= 8 || WorkingSlot < 0 || WorkingSlot >= 8 ||
				PhysicalLane < 0 || PhysicalLane >= 8)
				throw std::invalid_argument("Exact bounded machine attempt required.");
		}

		std::shared_ptr<ExternalSecureExecutionReservation> getReservation() const { return Reservation; }
		std::shared_ptr<ExternalSecureRegisterLocalRequest> getRequest() const { return Request; }
		Guid getMachineScope() const { return MachineScope; }
		unsigned long long getMachineGeneration() const noexcept { return MachineGeneration; }
		unsigned long long getCpuAttempt() const noexcept { return CpuAttempt; }
		unsigned long long getWorkingBundle() const noexcept { return WorkingBundle; }
		int getVirtualThread() const noexcept { return VirtualThread; }
		int getOwnerContext() const noexcept { return OwnerContext; }
		int getSourceSlot() const noexcept { return SourceSlot; }
		int getWorkingSlot() const noexcept { return WorkingSlot; }
		int getPhysicalLane() const noexcept { return PhysicalLane; }
		unsigned int getSchemaVersion() const noexcept { return SchemaVersion; }
	};

	/// <summary>Historical consumption of provider-local capacity; no work is submitted.</summary>
	class ExternalSecureExecutionConsumptionReceipt final
	{
	public:
		const std::shared_ptr<ExternalSecureExecutionAttempt> Attempt;
		const Guid ProviderScope;

		ExternalSecureExecutionConsumptionReceipt(std::shared_ptr<ExternalSecureExecutionAttempt> attempt, Guid providerScope)
			: Attempt(std::move(attempt)), ProviderScope(std::move(providerScope))
		{
			if (!Attempt) throw std::invalid_argument("attempt");
			if (ProviderScope == Guid::Empty) throw std::invalid_argument("Exact provider scope required.");
		}

		std::shared_ptr<ExternalSecureExecutionAttempt> getAttempt() const { return Attempt; }
		Guid getProviderScope() const { return ProviderScope; }
	};

	class ExternalSecureExecutionConsumptionResult final
	{
	public:
		const std::shared_ptr<ExternalSecureExecutionAttempt> Attempt;
		const ExternalSecureDomainProviderStatus Status;
		const std::shared_ptr<ExternalSecureExecutionConsumptionReceipt> Receipt;

		ExternalSecureExecutionConsumptionResult(std::shared_ptr<ExternalSecureExecutionAttempt> attempt,
			ExternalSecureDomainProviderStatus status,
			std::shared_ptr<ExternalSecureExecutionConsumptionReceipt> receipt = nullptr)
			: Attempt(std::move(attempt)), Status(status), Receipt(std::move(receipt))
		{
			if (!Attempt) throw std::invalid_argument("attempt");
			if (!IsDefined(Status)) throw std::out_of_range("status");
			if ((Status == ExternalSecureDomainProviderStatus::Receipt) != static_cast<bool>(Receipt) ||
				(Receipt && Receipt->Attempt != Attempt))
				throw std::invalid_argument("Exact receipt and original attempt must agree.");
		}

		std::shared_ptr<ExternalSecureExecutionAttempt> getAttempt() const { return Attempt; }
		ExternalSecureDomainProviderStatus getStatus() const noexcept { return Status; }
		std::shared_ptr<ExternalSecureExecutionConsumptionReceipt> getReceipt() const { return Receipt; }
	};

	/// <summary>One-shot bounded register-local submission; it carries no architectural sink.</summary>
	class ExternalSecureRegisterLocalSubmission final
	{
	public:
		const std::shared_ptr<ExternalSecureExecutionConsumptionReceipt> Consumption;
		const std::shared_ptr<ExternalSecureExecutionAttempt> Attempt;
		const int CompletionSlot;
		const unsigned long long CompletionGeneration;
		static constexpr int ExpectedDestination = 3;
		static constexpr unsigned int SchemaVersion = 1;

		ExternalSecureRegisterLocalSubmission(std::shared_ptr<ExternalSecureExecutionConsumptionReceipt> consumption,
			int completionSlot, unsigned long long completionGeneration, unsigned int schemaVersion = 1)
			: Consumption(std::move(consumption)),
			  Attempt(Consumption ? Consumption->Attempt : nullptr), CompletionSlot(completionSlot),
			  CompletionGeneration(completionGeneration)
		{
			if (!Consumption) throw std::invalid_argument("consumption");
			if (schemaVersion != 1 || CompletionSlot < 0 || CompletionSlot >= 8 || CompletionGeneration == 0)
				throw std::invalid_argument("Bounded schema1 slot/generation required.");
		}

		std::shared_ptr<ExternalSecureExecutionConsumptionReceipt> getConsumption() const { return Consumption; }
		std::shared_ptr<ExternalSecureExecutionAttempt> getAttempt() const { return Attempt; }
		int getCompletionSlot() const noexcept { return CompletionSlot; }
		unsigned long long getCompletionGeneration() const noexcept { return CompletionGeneration; }
		int getExpectedDestination() const noexcept { return ExpectedDestination; }
		unsigned int getSchemaVersion() const noexcept { return SchemaVersion; }
	};

	/// <summary>Untrusted bounded facts; the issuer validates source and arithmetic before acceptance.</summary>
	class ExternalSecureRegisterLocalExecutionReceipt final
	{
	public:
		const std::shared_ptr<ExternalSecureRegisterLocalSubmission> Submission;
		const unsigned long long Value;
		const int Destination;
		const int EffectCount;
		const int CompletionSlot;
		const unsigned long long CompletionGeneration;

		ExternalSecureRegisterLocalExecutionReceipt(std::shared_ptr<ExternalSecureRegisterLocalSubmission> submission,
			unsigned long long value, int destination, int effectCount, int completionSlot,
			unsigned long long completionGeneration)
			: Submission(std::move(submission)), Value(value), Destination(destination), EffectCount(effectCount),
			  CompletionSlot(completionSlot), CompletionGeneration(completionGeneration)
		{
			if (!Submission) throw std::invalid_argument("submission");
			if (Destination < 0 || Destination >= 32 || EffectCount < 0 || EffectCount > 1 ||
				CompletionSlot < 0 || CompletionSlot >= 8 || CompletionGeneration == 0)
				throw std::invalid_argument("Bounded receipt facts required.");
		}

		std::shared_ptr<ExternalSecureRegisterLocalSubmission> getSubmission() const { return Submission; }
		unsigned long long getValue() const noexcept { return Value; }
		int getDestination() const noexcept { return Destination; }
		int getEffectCount() const noexcept { return EffectCount; }
		int getCompletionSlot() const noexcept { return CompletionSlot; }
		unsigned long long getCompletionGeneration() const noexcept { return CompletionGeneration; }
	};

	class ExternalSecureRegisterLocalExecutionResult final
	{
	public:
		const std::shared_ptr<ExternalSecureRegisterLocalSubmission> Submission;
		const ExternalSecureDomainProviderStatus Status;
		const std::shared_ptr<ExternalSecureRegisterLocalExecutionReceipt> Receipt;

		ExternalSecureRegisterLocalExecutionResult(std::shared_ptr<ExternalSecureRegisterLocalSubmission> submission,
			ExternalSecureDomainProviderStatus status,
			std::shared_ptr<ExternalSecureRegisterLocalExecutionReceipt> receipt = nullptr)
			: Submission(std::move(submission)), Status(status), Receipt(std::move(receipt))
		{
			if (!Submission) throw std::invalid_argument("submission");
			if (!IsDefined(Status) ||
				(Status == ExternalSecureDomainProviderStatus::Receipt) != static_cast<bool>(Receipt) ||
				(Receipt && Receipt->Submission != Submission))
				throw std::invalid_argument("Original submission/receipt status required.");
		}

		std::shared_ptr<ExternalSecureRegisterLocalSubmission> getSubmission() const { return Submission; }
		ExternalSecureDomainProviderStatus getStatus() const noexcept { return Status; }
		std::shared_ptr<ExternalSecureRegisterLocalExecutionReceipt> getReceipt() const { return Receipt; }
	};

	class IExternalSecureRegisterLocalExecutionProvider : public IExternalSecureExecutionConsumptionProvider
	{
	public:
		~IExternalSecureRegisterLocalExecutionProvider() override = default;
		virtual std::shared_ptr<ExternalSecureRegisterLocalExecutionResult> SubmitRegisterLocal(
			std::shared_ptr<ExternalSecureRegisterLocalSubmission> submission) = 0;
		virtual std::shared_ptr<ExternalSecureRegisterLocalExecutionResult> QueryRegisterLocal(
			std::shared_ptr<ExternalSecureRegisterLocalSubmission> submission) = 0;
	};
}
