#pragma once

#include "../../Pipeline/Safety/SafetyVerifier.SecureComputeAdmission.h"
#include "../../Architecture/Common/YAKSys_Hybrid_CPU.Processor.h"
#include "../../../../Guid.h"
#include <cmath>
#include <atomic>
#include <stdexcept>
#include <any>
#include <functional>
#include <optional>
#include <memory>
#include <thread>

using namespace HybridCPU::ExternalRuntime::Contracts;
using namespace YAKSys_Hybrid_CPU;
using namespace YAKSys_Hybrid_CPU::Core;

namespace HybridCPU_ISE::CloseToHSL::Core::Runtime::Managed
{

	// Explicit machine control, deliberately separate from the diagnostic observer.
	// Semantic authorization and original reservation lifecycle remain host-owned.
	class SoftwareSecureOwnedEntry final : public std::enable_shared_from_this<SoftwareSecureOwnedEntry>
	{
	private:
		const Guid RunId = Guid::NewGuid();
		const std::shared_ptr<ExternalSecureRegisterLocalRequest> Request;
		const std::shared_ptr<ExternalSecureExecutionReservation> Reservation;
		const std::shared_ptr<IExternalSecureExecutionConsumptionProvider> Provider;
		std::shared_ptr<SecureComputeCanonicalAdmissionResult> Result;
		std::optional<unsigned long long> CoreValue;
		std::optional<unsigned long long> Register3Before;
		std::optional<unsigned long long> Register3After;
		bool MachineInvocationStarted = false;

		std::atomic<int> invoked{0};
// C# TO C++ CONVERTER TASK: 'volatile' has a different meaning in C++:
// ORIGINAL LINE: private volatile bool cancelled;
		std::atomic<bool> cancelled{false};
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private Processor.CPU_Core? currentCore;
		std::shared_ptr<Processor::CPU_Core> currentCore;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER TASK: Comparisons involving nullable type instances will need to be rewritten since comparison rules are different between C++ optional and System.Nullable:
// ORIGINAL LINE: private readonly System.Func<bool>? ownerCurrent;
		const std::function<bool()> ownerCurrent;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private readonly Object? sourceParentGate, sourceChildGate;
		const std::any sourceParentGate, sourceChildGate;
	public:
		Guid getRunId() const;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public SoftwareSecureOwnedEntry(ExternalSecureRegisterLocalRequest request, ExternalSecureExecutionReservation reservation, IExternalSecureExecutionConsumptionProvider provider, System.Func<bool>? ownerSourceCurrent = null) : this(request, reservation, provider, ownerSourceCurrent, null, null)
		SoftwareSecureOwnedEntry(std::shared_ptr<ExternalSecureRegisterLocalRequest> request, std::shared_ptr<ExternalSecureExecutionReservation> reservation, std::shared_ptr<IExternalSecureExecutionConsumptionProvider> provider, std::function<bool()> ownerSourceCurrent = nullptr);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public SoftwareSecureOwnedEntry(ExternalSecureRegisterLocalRequest request, ExternalSecureExecutionReservation reservation, IExternalSecureExecutionConsumptionProvider provider, System.Func<bool>? ownerSourceCurrent, System.Nullable<Object> sourceParentGate, System.Nullable<Object> sourceChildGate)
		SoftwareSecureOwnedEntry(std::shared_ptr<ExternalSecureRegisterLocalRequest> request, std::shared_ptr<ExternalSecureExecutionReservation> reservation, std::shared_ptr<IExternalSecureExecutionConsumptionProvider> provider, std::function<bool()> ownerSourceCurrent, std::any sourceParentGate, std::any sourceChildGate);
		std::shared_ptr<ExternalSecureRegisterLocalRequest> getRequest() const;
		std::shared_ptr<ExternalSecureExecutionReservation> getReservation() const;
		std::shared_ptr<IExternalSecureExecutionConsumptionProvider> getProvider() const;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public SecureComputeCanonicalAdmissionResult? Result {get;private set;}
		std::shared_ptr<SecureComputeCanonicalAdmissionResult> getResult() const;
	private:
		void setResult(std::shared_ptr<SecureComputeCanonicalAdmissionResult> value);
	public:
		std::optional<unsigned long long> getCoreValue() const;
	private:
		void setCoreValue(const std::optional<unsigned long long> &value);
	public:
		std::optional<unsigned long long> getRegister3Before() const;
	private:
		void setRegister3Before(const std::optional<unsigned long long> &value);
	public:
		std::optional<unsigned long long> getRegister3After() const;
	private:
		void setRegister3After(const std::optional<unsigned long long> &value);
	public:
		bool getMachineInvocationStarted() const;
	private:
		void setMachineInvocationStarted(bool value);
	public:
		void Cancel();
		void Execute(std::shared_ptr<Processor::CPU_Core> core, int maximumCycles, int virtualThread);
	};

}
