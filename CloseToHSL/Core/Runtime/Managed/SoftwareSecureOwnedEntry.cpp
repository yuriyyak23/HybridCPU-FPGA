#include "SoftwareSecureOwnedEntry.h"
#include "../../Pipeline/Safety/SafetyVerifier.SecureComputeAdmission.h"
#include "../Completion/Records/SecureComputeRegisterLocalCompletionOwner.h"
#include "../../../../ExternalRuntime/Contracts/ExternalSecureRegisterLocalContracts.h"

using namespace HybridCPU::ExternalRuntime::Contracts;
using namespace YAKSys_Hybrid_CPU;
using namespace YAKSys_Hybrid_CPU::Core;

namespace HybridCPU_ISE::CloseToHSL::Core::Runtime::Managed
{

	Guid SoftwareSecureOwnedEntry::getRunId() const
	{
		return RunId;
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public SoftwareSecureOwnedEntry(ExternalSecureRegisterLocalRequest request, ExternalSecureExecutionReservation reservation, IExternalSecureExecutionConsumptionProvider provider, System.Func<bool>? ownerSourceCurrent = null) : this(request, reservation, provider, ownerSourceCurrent, null, null)
	SoftwareSecureOwnedEntry::SoftwareSecureOwnedEntry(std::shared_ptr<ExternalSecureRegisterLocalRequest> request, std::shared_ptr<ExternalSecureExecutionReservation> reservation, std::shared_ptr<IExternalSecureExecutionConsumptionProvider> provider, std::function<bool()> ownerSourceCurrent) : SoftwareSecureOwnedEntry(request, reservation, provider, ownerSourceCurrent, std::any(), std::any())
	{
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public SoftwareSecureOwnedEntry(ExternalSecureRegisterLocalRequest request, ExternalSecureExecutionReservation reservation, IExternalSecureExecutionConsumptionProvider provider, System.Func<bool>? ownerSourceCurrent, System.Nullable<Object> sourceParentGate, System.Nullable<Object> sourceChildGate)
	SoftwareSecureOwnedEntry::SoftwareSecureOwnedEntry(std::shared_ptr<ExternalSecureRegisterLocalRequest> request, std::shared_ptr<ExternalSecureExecutionReservation> reservation, std::shared_ptr<IExternalSecureExecutionConsumptionProvider> provider, std::function<bool()> ownerSourceCurrent, std::any sourceParentGate, std::any sourceChildGate) : Request(request), Reservation(reservation), Provider(provider), ownerCurrent(std::move(ownerSourceCurrent)), sourceParentGate(std::move(sourceParentGate)), sourceChildGate(std::move(sourceChildGate))
	{
		if (!Request) throw std::invalid_argument("request");
		if (!Reservation) throw std::invalid_argument("reservation");
		if (!Provider) throw std::invalid_argument("provider");
	}

	std::shared_ptr<ExternalSecureRegisterLocalRequest> SoftwareSecureOwnedEntry::getRequest() const
	{
		return Request;
	}

	std::shared_ptr<ExternalSecureExecutionReservation> SoftwareSecureOwnedEntry::getReservation() const
	{
		return Reservation;
	}

	std::shared_ptr<IExternalSecureExecutionConsumptionProvider> SoftwareSecureOwnedEntry::getProvider() const
	{
		return Provider;
	}

	std::shared_ptr<SecureComputeCanonicalAdmissionResult> SoftwareSecureOwnedEntry::getResult() const
	{
		return Result;
	}

	void SoftwareSecureOwnedEntry::setResult(std::shared_ptr<SecureComputeCanonicalAdmissionResult> value)
	{
		Result = value;
	}

	std::optional<unsigned long long> SoftwareSecureOwnedEntry::getCoreValue() const
	{
		return CoreValue;
	}

	void SoftwareSecureOwnedEntry::setCoreValue(const std::optional<unsigned long long> &value)
	{
		CoreValue = value;
	}

	std::optional<unsigned long long> SoftwareSecureOwnedEntry::getRegister3Before() const
	{
		return Register3Before;
	}

	void SoftwareSecureOwnedEntry::setRegister3Before(const std::optional<unsigned long long> &value)
	{
		Register3Before = value;
	}

	std::optional<unsigned long long> SoftwareSecureOwnedEntry::getRegister3After() const
	{
		return Register3After;
	}

	void SoftwareSecureOwnedEntry::setRegister3After(const std::optional<unsigned long long> &value)
	{
		Register3After = value;
	}

	bool SoftwareSecureOwnedEntry::getMachineInvocationStarted() const
	{
		return MachineInvocationStarted;
	}

	void SoftwareSecureOwnedEntry::setMachineInvocationStarted(bool value)
	{
		MachineInvocationStarted = value;
	}

	void SoftwareSecureOwnedEntry::Cancel()
	{
		cancelled.store(true, std::memory_order_release);
		auto core = std::atomic_load_explicit(&currentCore, std::memory_order_acquire);
		if (core == nullptr)
		{
			return;
		}
		// A source-owner reentrant caller must not wait for the verifier while
		// retaining a gate that canonical commit joins. The permanent denial
		// flag already fences that commit; registry invalidation may follow.
		if (sourceParentGate.has_value() || sourceChildGate.has_value())
		{
			auto secureDomain = std::make_shared<ExternalSecureDomainLease>(getRequest()->Binding.SecureDomain);
			std::thread([core = std::move(core), secureDomain]()
			{
				core->InvalidateSoftwareSecureAdmissions(secureDomain);
			}).detach();
		}
		else
		{
			core->InvalidateSoftwareSecureAdmissions(std::make_shared<ExternalSecureDomainLease>(getRequest()->Binding.SecureDomain));
		}
	}

	void SoftwareSecureOwnedEntry::Execute(std::shared_ptr<Processor::CPU_Core> core, int maximumCycles, int virtualThread)
	{
		int expected = 0;
		if (!invoked.compare_exchange_strong(expected, 1, std::memory_order_acq_rel))
		{
			return;
		}
		std::atomic_store_explicit(&currentCore, core, std::memory_order_release);
		auto cleanup = [&]()
		{
			if (getRegister3Before().has_value())
			{
				setRegister3After(core->ReadArch(virtualThread, 3));
			}
			std::atomic_store_explicit(&currentCore, std::shared_ptr<Processor::CPU_Core>{}, std::memory_order_release);
		};
		try
		{
			if (!cancelled.load(std::memory_order_acquire) && getReservation()->Request == getRequest() && core->TryParkSoftwareSecureDecode())
			{
				for (int cycle = 0; cycle < std::min(maximumCycles, 32) && !core->isSoftwareSecureDecodeParked(); cycle++)
				{
					core->ExecuteRuntimePipelineCycle();
				}
				if (core->isSoftwareSecureDecodeParked())
				{
					setRegister3Before(core->ReadArch(virtualThread, 3));
					setMachineInvocationStarted(true);
					setResult(core->ExecuteSoftwareSecureRegisterLocal(getRequest(), getReservation(), getProvider(), [&]()
					{
						return !cancelled.load(std::memory_order_acquire) && (!ownerCurrent || ownerCurrent());
					}, sourceParentGate, sourceChildGate));

					auto retired = getResult()->getRegisterLocalRetirement();
					if (retired)
					{
						auto submission = retired->getSubmission();
						auto attempt = submission ? submission->getAttempt() : nullptr;
						if (attempt && attempt->Request == getRequest())
						{
							unsigned long long value = core->ReadArch(attempt->VirtualThread, 3);
							if (value == retired->getRegister3Value())
							{
								setCoreValue(value);
							}
						}
					}
				}
			}
		}
		catch (...)
		{
			cleanup();
			throw;
		}
		cleanup();
	}
}
