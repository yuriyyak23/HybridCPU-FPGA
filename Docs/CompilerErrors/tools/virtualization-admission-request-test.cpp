#include "../../CloseToHSL/Core/Pipeline/Safety/SafetyVerifier.VirtualizationOperationAdmission.h"
#include <cassert>
using namespace YAKSys_Hybrid_CPU::Core;
int main(){VirtualizationOperationAdmissionRequest a; a.DecisionManifest.DecisionId=L"d2";a.DecisionManifest.RequiredReviewers.push_back(L"reviewer");a.Attribution.CodeOwnersRuleMatched=true;auto b=a;assert(a==b);b.DecisionManifest.RequiredReviewers[0]=L"other";assert(!(a==b) && a.DecisionManifest.RequiredReviewers[0]==L"reviewer");assert(!a.AddressSpaceIdentityPresent);}
