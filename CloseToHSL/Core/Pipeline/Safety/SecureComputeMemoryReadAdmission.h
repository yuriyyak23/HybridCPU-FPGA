#pragma once

#include "../../Decoder/CanonicalDecodedContracts.h"
#include "../../Decoder/Rf06ExecutionContracts.h"
#include "../../Architecture/Registers/Retire/Rf081ScalarRegisterWriteRetireEffect.h"
#include "../Scheduling/PostStageBIssuedAttempt.h"
#include <any>
#include <functional>
#include <optional>
#include <memory>
#include <utility>

// C# TO C++ CONVERTER NOTE: Forward class declarations:
namespace YAKSys_Hybrid_CPU::Core { class SecureComputeMemoryReadRetirement; }

namespace HybridCPU::ExternalRuntime::Contracts { class ExternalSecureMemoryReadRequest; class ExternalSecureMemoryReadAttempt; class ExternalSecureMemoryReadConsumptionResult; class ExternalSecureMemoryReadSubmission; class ExternalSecureMemoryReadResult; class ExternalSecureMemoryReadReceipt; class ExternalSecureMemoryReadReservation; class IExternalSecureMemoryReadOrigin; }
using namespace HybridCPU::ExternalRuntime::Contracts;
using namespace HybridCPU_ISE::Arch;
using namespace YAKSys_Hybrid_CPU::Core::Decoder;
using namespace YAKSys_Hybrid_CPU::Core::Registers::Retire;

namespace YAKSys_Hybrid_CPU::Core
{

	enum class SecureComputeMemoryReadDecision : unsigned char
	{
		Unknown = 0,
		Denied = 1,
		CleanupRequired = 2,
		LoadRetired = 3
	};

	class SecureComputeMemoryReadMachineResult final : public std::enable_shared_from_this<SecureComputeMemoryReadMachineResult>
	{
	private:
		const std::shared_ptr<ExternalSecureMemoryReadRequest> Request;
		const SecureComputeMemoryReadDecision Decision;
		const std::shared_ptr<CanonicalBundle> Canonical;
		const std::shared_ptr<ScheduledOperation> Placement;
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly System.Nullable<ExternalSecureMemoryReadAttempt> Attempt;
		std::optional<std::shared_ptr<ExternalSecureMemoryReadAttempt>> Attempt;
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly System.Nullable<ExternalSecureMemoryReadConsumptionResult> Consumption;
		std::optional<std::shared_ptr<ExternalSecureMemoryReadConsumptionResult>> Consumption;
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly System.Nullable<ExternalSecureMemoryReadSubmission> Submission;
		std::optional<std::shared_ptr<ExternalSecureMemoryReadSubmission>> Submission;
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly System.Nullable<ExternalSecureMemoryReadResult> Outcome;
		std::optional<std::shared_ptr<ExternalSecureMemoryReadResult>> Outcome;
		const std::shared_ptr<SecureComputeMemoryReadRetirement> Retirement;
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly System.Nullable<ulong> Register3Value;
		std::optional<unsigned long long> Register3Value;

	public:
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal SecureComputeMemoryReadMachineResult(ExternalSecureMemoryReadRequest request, SecureComputeMemoryReadDecision decision, CanonicalBundle? canonical = null, System.Nullable<ScheduledOperation> placement = null, System.Nullable<ExternalSecureMemoryReadAttempt> attempt = null, System.Nullable<ExternalSecureMemoryReadConsumptionResult> consumption = null, System.Nullable<ExternalSecureMemoryReadSubmission> submission = null, System.Nullable<ExternalSecureMemoryReadResult> outcome = null, System.Nullable<SecureComputeMemoryReadRetirement> retirement = null, System.Nullable<ulong> register3Value = null)
		SecureComputeMemoryReadMachineResult(std::shared_ptr<ExternalSecureMemoryReadRequest> request, SecureComputeMemoryReadDecision decision, std::shared_ptr<CanonicalBundle> canonical = nullptr, std::shared_ptr<ScheduledOperation> placement = nullptr, std::optional<std::shared_ptr<ExternalSecureMemoryReadAttempt>> attempt = std::nullopt, std::optional<std::shared_ptr<ExternalSecureMemoryReadConsumptionResult>> consumption = std::nullopt, std::optional<std::shared_ptr<ExternalSecureMemoryReadSubmission>> submission = std::nullopt, std::optional<std::shared_ptr<ExternalSecureMemoryReadResult>> outcome = std::nullopt, std::shared_ptr<SecureComputeMemoryReadRetirement> retirement = nullptr, std::optional<unsigned long long> register3Value = std::nullopt);
		std::shared_ptr<ExternalSecureMemoryReadRequest> getRequest() const;
		SecureComputeMemoryReadDecision getDecision() const;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public CanonicalBundle? Canonical {get;}
		std::shared_ptr<CanonicalBundle> getCanonical() const;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public ScheduledOperation? Placement {get;}
		std::shared_ptr<ScheduledOperation> getPlacement() const;
		std::optional<std::shared_ptr<ExternalSecureMemoryReadAttempt>> getAttempt() const;
		std::optional<std::shared_ptr<ExternalSecureMemoryReadConsumptionResult>> getConsumption() const;
		std::optional<std::shared_ptr<ExternalSecureMemoryReadSubmission>> getSubmission() const;
		std::optional<std::shared_ptr<ExternalSecureMemoryReadResult>> getOutcome() const;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public SecureComputeMemoryReadRetirement? Retirement {get;}
		std::shared_ptr<SecureComputeMemoryReadRetirement> getRetirement() const;
		std::optional<unsigned long long> getRegister3Value() const;
	};

	class SecureComputeMemoryReadRetirement final : public std::enable_shared_from_this<SecureComputeMemoryReadRetirement>
	{
	private:
		const std::shared_ptr<ExternalSecureMemoryReadReceipt> Receipt;
		const unsigned long long AcceptanceSequence;
		const unsigned long long CommitSequence;
		const unsigned long long Register3Value;
		const std::shared_ptr<ScalarRegisterWriteRetireEffect> Effect;

	public:
		SecureComputeMemoryReadRetirement(std::shared_ptr<ExternalSecureMemoryReadReceipt> receipt, unsigned long long acceptance, unsigned long long commit, unsigned long long value, std::shared_ptr<ScalarRegisterWriteRetireEffect> effect);
		std::shared_ptr<ExternalSecureMemoryReadReceipt> getReceipt() const;
		unsigned long long getAcceptanceSequence() const;
		unsigned long long getCommitSequence() const;
		unsigned long long getRegister3Value() const;
		std::shared_ptr<ScalarRegisterWriteRetireEffect> getEffect() const;
		int getEffectCount() const;
	};

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER TASK: C++ has no equivalent to C# primary constructors:
// ORIGINAL LINE: internal sealed class SecureComputeMemoryReadCertificate(ExternalSecureMemoryReadRequest request, ExternalSecureMemoryReadReservation reservation, CanonicalBundle canonical, PostStageBIssuedAttempt issued, IExternalSecureMemoryReadOrigin origin, Object sourceGate, Object providerGate, ulong generation, System.Func<bool>? ownerCurrent)
	class SecureComputeMemoryReadCertificate final : public std::enable_shared_from_this<SecureComputeMemoryReadCertificate>
	{
	private:
		const std::shared_ptr<ExternalSecureMemoryReadRequest> Request;
		const std::shared_ptr<ExternalSecureMemoryReadReservation> Reservation;
		const std::shared_ptr<CanonicalBundle> Canonical;
		const std::shared_ptr<PostStageBIssuedAttempt> Issued;
		const std::shared_ptr<IExternalSecureMemoryReadOrigin> Origin;
		const std::any SourceGate;
		const std::any ProviderGate;
		const unsigned long long Generation;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private readonly System.Func<bool>? OwnerCurrent;
		const std::function<bool()> OwnerCurrent;

	public:
		SecureComputeMemoryReadCertificate(std::shared_ptr<ExternalSecureMemoryReadRequest> request, std::shared_ptr<ExternalSecureMemoryReadReservation> reservation, std::shared_ptr<CanonicalBundle> canonical, std::shared_ptr<PostStageBIssuedAttempt> issued, std::shared_ptr<IExternalSecureMemoryReadOrigin> origin, std::any sourceGate, std::any providerGate, unsigned long long generation, std::function<bool()> ownerCurrent) : Request(std::move(request)), Reservation(std::move(reservation)), Canonical(std::move(canonical)), Issued(std::move(issued)), Origin(std::move(origin)), SourceGate(std::move(sourceGate)), ProviderGate(std::move(providerGate)), Generation(generation), OwnerCurrent(std::move(ownerCurrent)) {}
		std::shared_ptr<ExternalSecureMemoryReadRequest> getRequest() const
		{
			return Request;
		}
		std::shared_ptr<ExternalSecureMemoryReadReservation> getReservation() const
		{
			return Reservation;
		}
		std::shared_ptr<CanonicalBundle> getCanonical() const
		{
			return Canonical;
		}
		std::shared_ptr<PostStageBIssuedAttempt> getIssued() const
		{
			return Issued;
		}
		std::shared_ptr<IExternalSecureMemoryReadOrigin> getOrigin() const
		{
			return Origin;
		}
		std::any getSourceGate() const
		{
			return SourceGate;
		}
		std::any getProviderGate() const
		{
			return ProviderGate;
		}
		unsigned long long getGeneration() const
		{
			return Generation;
		}
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal System.Func<bool>? OwnerCurrent {get;} = ownerCurrent;
		std::function<bool()> getOwnerCurrent() const
		{
			return OwnerCurrent;
		}
		std::optional<std::shared_ptr<ExternalSecureMemoryReadAttempt>> Attempt;
	};

// C# TO C++ CONVERTER TASK: C++ has no equivalent to C# primary constructors:
// ORIGINAL LINE: internal sealed class SecureComputeMemoryReadIssuePermit(SecureComputeMemoryReadCertificate certificate)
	class SecureComputeMemoryReadIssuePermit final : public std::enable_shared_from_this<SecureComputeMemoryReadIssuePermit>
	{
	private:
		const std::shared_ptr<SecureComputeMemoryReadCertificate> Certificate;

	public:
		explicit SecureComputeMemoryReadIssuePermit(std::shared_ptr<SecureComputeMemoryReadCertificate> certificate) : Certificate(std::move(certificate)) {}
		std::shared_ptr<SecureComputeMemoryReadCertificate> getCertificate() const;
	};



}
