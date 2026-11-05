$ErrorActionPreference='Stop'
$root=Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
$dir=Join-Path $root 'CloseToHSL\\Core\Virtualization\Compatibility\Frontend\Projection\Completion'
$p=Join-Path $dir 'NeutralCompletionReasonQualificationProjection.h'; $s=Get-Content $p -Raw
$marker="`tclass NeutralCompletionReasonQualificationProjectionOwner final"
$body=@"
    enum class VmExitReason : int;
    struct NeutralCompletionReasonQualificationProjection final {
        NeutralCompletionCompatibilityProjectionDecision Decision;
        VmExitReason ExitReason;
        unsigned long long ExitQualification;
        std::wstring Reason;
        bool isProjected() const noexcept { return Decision == NeutralCompletionCompatibilityProjectionDecision::Projected; }
    };

"@
if(!$s.Contains('struct NeutralCompletionReasonQualificationProjection final')){$s=$s.Replace($marker,$body+$marker)};Set-Content $p $s -Encoding UTF8 -NoNewline
$p=Join-Path $dir 'NeutralCompletionSecondStageProjection.h';$s=Get-Content $p -Raw
$marker="`tclass NeutralCompletionSecondStageProjectionOwner final"
$body=@"
    struct NeutralCompletionSecondStageProjection final {
        NeutralCompletionCompatibilityProjectionDecision Decision;
        unsigned long long GuestPhysicalAddress, EptViolationQualification;
        std::wstring Reason;
        bool isProjected() const noexcept { return Decision == NeutralCompletionCompatibilityProjectionDecision::Projected; }
    };

"@
if(!$s.Contains('struct NeutralCompletionSecondStageProjection final')){$s=$s.Replace($marker,$body+$marker)};Set-Content $p $s -Encoding UTF8 -NoNewline
$p=Join-Path $root 'CloseToHSL\\Core\Pipeline\Scheduling\Smt\MicroOpScheduler.ResearchVirtualizationCanonicalComposition.h';$s=Get-Content $p -Raw
$s=[regex]::Replace($s,'#endif\s*\}\s*$', "}`r`n#endif`r`n");Set-Content $p $s -Encoding UTF8 -NoNewline
$p=Join-Path $root 'tangible_\rectangular_vectors.h';$s=Get-Content $p -Raw
if(!$s.Contains('namespace YAKSys_Hybrid_CPU::Execution { class NoC_XY_Router; }')){$s=$s.Replace('#include <vector>',"#include <vector>`r`n#include <memory>`r`nnamespace YAKSys_Hybrid_CPU::Execution { class NoC_XY_Router; }`r`nnamespace YAKSys_Hybrid_CPU::Core { enum class ScoreboardEntryType : unsigned char; }")}
$s=$s.Replace('std::shared_ptr<NoC_XY_Router>','std::shared_ptr<YAKSys_Hybrid_CPU::Execution::NoC_XY_Router>').Replace('std::vector<ScoreboardEntryType>','std::vector<YAKSys_Hybrid_CPU::Core::ScoreboardEntryType>')
Set-Content $p $s -Encoding UTF8 -NoNewline
