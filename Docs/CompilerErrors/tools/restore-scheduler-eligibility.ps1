$ErrorActionPreference='Stop'
$root=Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
$p=Join-Path $root 'CloseToHSL\\Core\Pipeline\Assist\InterCore\YAKSys_Hybrid_CPU.Core.MicroOpScheduler.h';$s=Get-Content $p -Raw
$s=$s.Replace('#include <array>',"#include <array>`r`n#include <span>`r`n#include <bit>")
$marker="`tclass MicroOpScheduler : public ILegalityCertificateCacheTelemetrySink`r`n`t{";if(!$s.Contains($marker)){$marker="`tclass MicroOpScheduler : public ILegalityCertificateCacheTelemetrySink`n`t{"};if(!$s.Contains($marker)){throw 'scheduler class missing'}
$body=@"
    public:
        struct SmtEligibilitySnapshot final {
            unsigned char RequestedMask = 0, NormalizedMask = 0, ReadyPortMask = 0, VisibleReadyMask = 0, MaskedReadyMask = 0;
            int getEligibleCount() const noexcept { return std::popcount(static_cast<unsigned int>(NormalizedMask)); }
            int getReadyPortCount() const noexcept { return std::popcount(static_cast<unsigned int>(ReadyPortMask)); }
            int getVisibleReadyCount() const noexcept { return std::popcount(static_cast<unsigned int>(VisibleReadyMask)); }
            int getMaskedReadyCount() const noexcept { return std::popcount(static_cast<unsigned int>(MaskedReadyMask)); }
            bool getHasMaskedReadyCandidates() const noexcept { return MaskedReadyMask != 0; }
        };
    private:
        enum class RejectionType;
"@
$s=$s.Replace($marker,$marker+"`r`n"+$body)
$s=$s.Replace('std::shared_ptr<> workingBundle','std::shared_ptr<std::vector<std::shared_ptr<MicroOp>>> workingBundle').Replace('Span<int> ranked','std::span<int> ranked').Replace('ReadOnlySpan<bool> wasInjected','std::span<const bool> wasInjected').Replace('std::vector<(unsigned long long Address, unsigned long long Length)>','std::vector<MicroOp::MemoryRange>')
Set-Content $p $s -Encoding UTF8 -NoNewline
$p=Join-Path $root 'CloseToHSL\\Core\Pipeline\Assist\InterCore\YAKSys_Hybrid_CPU.Core.MicroOpScheduler.cpp';$s=Get-Content $p -Raw
$s=$s.Replace('std::shared_ptr<> workingBundle','std::shared_ptr<std::vector<std::shared_ptr<MicroOp>>> workingBundle').Replace('Span<int> ranked','std::span<int> ranked').Replace('ReadOnlySpan<bool> wasInjected','std::span<const bool> wasInjected').Replace('std::vector<(unsigned long long Address, unsigned long long Length)>','std::vector<MicroOp::MemoryRange>').Replace('ranges[index]->Address','ranges[index].Address').Replace('ranges[index]->Length','ranges[index].Length')
Set-Content $p $s -Encoding UTF8 -NoNewline
