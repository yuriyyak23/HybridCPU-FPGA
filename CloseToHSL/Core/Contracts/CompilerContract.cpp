#include "CompilerContract.h"
#include "../../../tangible_exception_helper.h"

namespace YAKSys_Hybrid_CPU::Core::Contracts
{

	CompilerTypedSlotPolicy::CompilerTypedSlotPolicy(CompilerTypedSlotPolicyMode mode) : Mode(mode)
	{}

	CompilerTypedSlotPolicyMode CompilerTypedSlotPolicy::getMode() const
	{
		return Mode;
	}

	bool CompilerTypedSlotPolicy::getAllowsMissingFacts() const
	{
		return getMode() != CompilerTypedSlotPolicyMode::RequiredForAdmission;
	}

	bool CompilerTypedSlotPolicy::getRequiresValidationForPresentFacts() const
	{
		return getMode() == CompilerTypedSlotPolicyMode::StrictVerification || getMode() == CompilerTypedSlotPolicyMode::RequiredForAdmission;
	}

	bool CompilerTypedSlotPolicy::getRejectsStructuralAgreementFailures() const
	{
		return getMode() == CompilerTypedSlotPolicyMode::StrictVerification || getMode() == CompilerTypedSlotPolicyMode::RequiredForAdmission;
	}

	bool CompilerTypedSlotPolicy::getRequiresRuntimeBundleForPresentFacts() const
	{
		return getRequiresValidationForPresentFacts();
	}

	bool CompilerTypedSlotPolicy::getQuarantineLogsAgreementFailures() const
	{
		return true;
	}

	bool CompilerTypedSlotPolicy::isRuntimeSelectable() const
	{
		return getMode() != CompilerTypedSlotPolicyMode::RequiredForAdmission;
	}

	CompilerTypedSlotPolicy CompilerTypedSlotPolicy::getCompatibilityValidation()
	{
		return CompatibilityValidation;
	}

	CompilerTypedSlotPolicy CompilerTypedSlotPolicy::getStrictVerification()
	{
		return StrictVerification;
	}

	CompilerTypedSlotPolicy CompilerTypedSlotPolicy::getRequiredForAdmissionFuture()
	{
		return RequiredForAdmissionFuture;
	}

	CompilerTypedSlotPolicy CompilerContract::getCurrentTypedSlotPolicy()
	{
		return CompilerTypedSlotPolicy::getCompatibilityValidation();
	}

	void CompilerContract::ThrowIfVersionMismatch(int producerVersion, const std::wstring &producerSurface)
	{
		if (producerVersion == Version)
		{
			return;
		}

		std::wstring resolvedSurface = StringHelper::isEmptyOrWhiteSpace(producerSurface) ? L"unknown compiler surface" : producerSurface;

		throw InvalidOperationException(std::format(L"Compiler contract mismatch at {}: producer version {} does not match runtime version {}. Re-run compilation against the current HybridCPU compiler/runtime contract.", resolvedSurface, producerVersion, Version));
	}
}
