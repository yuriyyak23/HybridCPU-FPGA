#include "../../CloseToHSL/Core/Contracts/CompilerContract.h"
#include "../../tangible_exception_helper.h"
#include <cassert>
#include <string>
using namespace YAKSys_Hybrid_CPU::Core::Contracts;
int main() {
 auto a=CompilerTypedSlotPolicy::getCompatibilityValidation();
 auto b=CompilerTypedSlotPolicy::getStrictVerification();
 auto c=CompilerTypedSlotPolicy::getRequiredForAdmissionFuture();
 assert(a.getAllowsMissingFacts() && !a.getRequiresValidationForPresentFacts() && !a.getRejectsStructuralAgreementFailures());
 assert(b.getAllowsMissingFacts() && b.getRequiresValidationForPresentFacts() && b.getRejectsStructuralAgreementFailures());
 assert(!c.getAllowsMissingFacts() && c.getRequiresValidationForPresentFacts() && c.getRejectsStructuralAgreementFailures() && !c.isRuntimeSelectable());
 CompilerContract::ThrowIfVersionMismatch(CompilerContract::Version, L"surface");
 try { CompilerContract::ThrowIfVersionMismatch(0, L" "); assert(false); }
 catch (const InvalidOperationException &e) { assert(std::string(e.what()).find("unknown compiler surface") != std::string::npos); }
 return 0;
}
