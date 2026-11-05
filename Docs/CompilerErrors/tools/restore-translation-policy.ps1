$ErrorActionPreference='Stop'
$root=Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
$p=Join-Path $root 'CloseToHSL\\Core\Runtime\Memory\Translation\CpuInstructionTranslationContour.h'; $s=Get-Content $p -Raw
$s=$s.Replace('ImmutableArray<std::shared_ptr<CpuInstructionTranslationRegion>>','std::vector<std::shared_ptr<CpuInstructionTranslationRegion>>').Replace('ImmutableArray<std::shared_ptr<CpuInstructionStageOneRegion>>','std::vector<std::shared_ptr<CpuInstructionStageOneRegion>>').Replace('System::Collections::Immutable::ImmutableArray<CpuInstructionStageOneRegion>()','{}')
$a=$s.IndexOf('static inline const std::shared_ptr<CpuInstructionTranslationPolicy> Identity ='); $b=$s.IndexOf(';',$a)
if($a -lt 0 -or $b -lt 0){throw 'Identity initializer not found'}
$s=$s.Remove($a,$b-$a+1)
Set-Content $p $s -Encoding UTF8 -NoNewline
$p=Join-Path $root 'CloseToHSL\\Core\Runtime\Memory\Translation\CpuInstructionTranslationContour.cpp'; $s=Get-Content $p -Raw
$a=$s.IndexOf("`tCpuInstructionTranslationPolicy::CpuInstructionTranslationPolicy("); $b=$s.IndexOf("`tCpuInstructionTranslationFaultException::CpuInstructionTranslationFaultException(",$a)
if($a -lt 0 -or $b -lt 0){throw 'Policy block boundaries missing'}
$body=@"
    template<class T> static std::vector<std::shared_ptr<T>> CopyRegions(const std::vector<std::shared_ptr<T>>& regions) {
        std::vector<std::shared_ptr<T>> result; result.reserve(regions.size());
        for (const auto& region : regions) { if (!region) throw std::invalid_argument("null region"); result.push_back(std::make_shared<T>(*region)); }
        return result;
    }
    template<class T> static std::vector<std::shared_ptr<T>> ValidatedRegions(const std::vector<std::shared_ptr<T>>& input) {
        auto ordered=CopyRegions(input);
        if (ordered.empty()) throw std::invalid_argument("mapped translation requires at least one region");
        std::sort(ordered.begin(),ordered.end(),[](const auto& a,const auto& b){ return a->VirtualBase < b->VirtualBase; });
        unsigned long long previousEnd=0; bool first=true;
        for (const auto& r : ordered) {
            if (!r->isValid()) throw std::invalid_argument("invalid translation region");
            if (!first && r->VirtualBase <= previousEnd) throw std::invalid_argument("overlapping translation regions");
            previousEnd=r->VirtualBase + r->Length - 1; first=false;
        }
        return ordered;
    }
    CpuInstructionTranslationPolicy::CpuInstructionTranslationPolicy(CpuInstructionTranslationMode mode, std::shared_ptr<CpuInstructionTranslationScope> scope, std::vector<std::shared_ptr<CpuInstructionTranslationRegion>> regions, std::vector<std::shared_ptr<CpuInstructionStageOneRegion>> stageOneRegions, std::shared_ptr<Core::MemoryDomainRuntime::CpuSecondStageTranslationBinding> binding)
        : Mode(mode), Scope(scope ? std::make_shared<CpuInstructionTranslationScope>(*scope) : std::make_shared<CpuInstructionTranslationScope>()), Regions(CopyRegions(regions)), StageOneRegions(CopyRegions(stageOneRegions)), SecondStageBinding(std::move(binding)) {}
    CpuInstructionTranslationMode CpuInstructionTranslationPolicy::getMode() const { return Mode; }
    std::shared_ptr<CpuInstructionTranslationScope> CpuInstructionTranslationPolicy::getScope() const { return std::make_shared<CpuInstructionTranslationScope>(*Scope); }
    std::vector<std::shared_ptr<CpuInstructionTranslationRegion>> CpuInstructionTranslationPolicy::getRegions() const { return CopyRegions(Regions); }
    std::vector<std::shared_ptr<CpuInstructionStageOneRegion>> CpuInstructionTranslationPolicy::getStageOneRegions() const { return CopyRegions(StageOneRegions); }
    std::shared_ptr<Core::MemoryDomainRuntime::CpuSecondStageTranslationBinding> CpuInstructionTranslationPolicy::getSecondStageBinding() const { return SecondStageBinding; }
    std::shared_ptr<CpuInstructionTranslationPolicy> CpuInstructionTranslationPolicy::getIdentity() {
        static auto identity=std::shared_ptr<CpuInstructionTranslationPolicy>(new CpuInstructionTranslationPolicy(CpuInstructionTranslationMode::Identity, nullptr, {})); return identity;
    }
    std::shared_ptr<CpuInstructionTranslationPolicy> CpuInstructionTranslationPolicy::CreateBoundedRegions(std::shared_ptr<CpuInstructionTranslationScope> scope, std::vector<std::shared_ptr<CpuInstructionTranslationRegion>>& regions) {
        if (!scope || !scope->isValid()) throw std::out_of_range("scope");
        auto ordered=ValidatedRegions(regions);
        return std::shared_ptr<CpuInstructionTranslationPolicy>(new CpuInstructionTranslationPolicy(CpuInstructionTranslationMode::BoundedRegions,scope,std::move(ordered)));
    }
    std::shared_ptr<CpuInstructionTranslationPolicy> CpuInstructionTranslationPolicy::CreateTwoStageBoundedRegions(std::shared_ptr<CpuInstructionTranslationScope> scope, std::vector<std::shared_ptr<CpuInstructionStageOneRegion>>& regions, std::shared_ptr<Core::MemoryDomainRuntime> runtime, std::shared_ptr<Core::MemoryDomainDescriptor> descriptor, unsigned long long identity) {
        if (!scope || !scope->isValid()) throw std::out_of_range("scope");
        if (!runtime || !descriptor) throw std::invalid_argument("runtime or descriptor");
        auto ordered=ValidatedRegions(regions);
        auto bound=runtime->BindCanonicalCpuSecondStageTranslation(descriptor, identity, scope->DomainId);
        if (!bound || !bound->isBound()) throw std::logic_error("second-stage source binding failed");
        return std::shared_ptr<CpuInstructionTranslationPolicy>(new CpuInstructionTranslationPolicy(CpuInstructionTranslationMode::TwoStageBoundedRegions,scope,{},std::move(ordered),bound->Binding));
    }

"@
$s=$s.Substring(0,$a)+$body+$s.Substring($b)
$s=$s.Replace('#include "CpuInstructionTranslationContour.h"', "#include `"CpuInstructionTranslationContour.h`"`r`n#include <algorithm>`r`n#include <stdexcept>`r`n#include <utility>")
Set-Content $p $s -Encoding UTF8 -NoNewline
