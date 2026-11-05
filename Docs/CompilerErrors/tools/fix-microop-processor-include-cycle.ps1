$ErrorActionPreference = 'Stop'
$projectRoot = 'C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1'
$utf8 = [Text.UTF8Encoding]::new($false)

function Replace-Once([string]$text, [string]$old, [string]$new, [string]$label) {
    $count = ([regex]::Matches($text, [regex]::Escape($old))).Count
    if ($count -eq 0) {
        if ($text.Contains($new)) { return $text }
        throw "Missing expected source block: $label"
    }
    if ($count -ne 1) { throw "Expected one source block for $label; found $count" }
    return $text.Replace($old, $new)
}

$processorPath = Join-Path $projectRoot 'CloseToHSL\\Core\Architecture\Common\YAKSys_Hybrid_CPU.Processor.h'
$processorCppPath = Join-Path $projectRoot 'CloseToHSL\\Core\Architecture\Common\YAKSys_Hybrid_CPU.Processor.cpp'
$schedulerPath = Join-Path $projectRoot 'CloseToHSL\\Core\Pipeline\Assist\InterCore\YAKSys_Hybrid_CPU.Core.MicroOpScheduler.h'
$schedulerCppPath = Join-Path $projectRoot 'CloseToHSL\\Core\Pipeline\Assist\InterCore\YAKSys_Hybrid_CPU.Core.MicroOpScheduler.cpp'
$bundleCertificatePath = Join-Path $projectRoot 'CloseToHSL\\Core\Pipeline\Certificates\BundleResourceCertificate.h'
$bundleCertificate4WayPath = Join-Path $projectRoot 'CloseToHSL\\Core\Pipeline\Certificates\YAKSys_Hybrid_CPU.Core.BundleResourceCertificate4Way.h'
$decodedBundleDescriptorPath = Join-Path $projectRoot 'CloseToHSL\\Core\Decoder\DecodedBundleDescriptor.h'
$clusterIssuePreparationPath = Join-Path $projectRoot 'CloseToHSL\\Core\Decoder\ClusterIssuePreparation.h'
$issuePacketLanePath = Join-Path $projectRoot 'CloseToHSL\\Core\Pipeline\Core\RuntimeClusterAdmissionPreparation.IssuePacketLane.h'
$replayImplementationsPath = Join-Path $projectRoot 'CloseToHSL\\Core\Pipeline\Certificates\ReplayPhaseSubstrate.Implementations.h'
$safetyVerifierTypesPath = Join-Path $projectRoot 'CloseToHSL\\Core\Pipeline\Safety\SafetyVerifier.Types.h'
$postStageAttemptPath = Join-Path $projectRoot 'CloseToHSL\\Core\Pipeline\Scheduling\PostStageBIssuedAttempt.h'
$safetyVerifierPath = Join-Path $projectRoot 'CloseToHSL\\Core\Pipeline\Safety\YAKSys_Hybrid_CPU.Core.SafetyVerifier.h'
$microOpHeaderPath = Join-Path $projectRoot 'CloseToHSL\\Core\Pipeline\MicroOps\Types\MicroOp.h'
$decodedBundleHeaderPath = Join-Path $projectRoot 'CloseToHSL\\Core\Decoder\DecodedBundleDescriptor.h'

$p = [IO.File]::ReadAllText($processorPath)
$p = Replace-Once $p '#include "../../Pipeline/MicroOps/Types/MicroOp.h"' 'namespace YAKSys_Hybrid_CPU::Core { class MicroOp; }' 'Processor MicroOp include'
$p = Replace-Once $p '#include "../../../Processor/Core/YAKSys_Hybrid_CPU.PodController.h"' 'namespace YAKSys_Hybrid_CPU { class PodController; }' 'Processor PodController include'
$p = Replace-Once $p '#include "../../Pipeline/Assist/InterCore/YAKSys_Hybrid_CPU.Core.MicroOpScheduler.h"' 'namespace YAKSys_Hybrid_CPU::Core { class MicroOpScheduler; class InterCoreAssistNomination; }' 'Processor scheduler include'
$p = Replace-Once $p 'Core::MicroOpScheduler::InterCoreAssistNomination' 'Core::InterCoreAssistNomination' 'Processor nomination type'
[IO.File]::WriteAllText($processorPath, $p, $utf8)

$bc = [IO.File]::ReadAllText($bundleCertificatePath)
$bc = Replace-Once $bc '#include "../MicroOps/Types/MicroOp.h"' 'namespace YAKSys_Hybrid_CPU::Core { class MicroOp; }' 'bundle certificate MicroOp include'
[IO.File]::WriteAllText($bundleCertificatePath, $bc, $utf8)

$bc4 = [IO.File]::ReadAllText($bundleCertificate4WayPath)
$bc4 = Replace-Once $bc4 '#include "../MicroOps/Types/MicroOp.h"' 'namespace YAKSys_Hybrid_CPU::Core { class MicroOp; }' '4-way bundle certificate MicroOp include'
[IO.File]::WriteAllText($bundleCertificate4WayPath, $bc4, $utf8)

$dbd = [IO.File]::ReadAllText($decodedBundleDescriptorPath)
$dbd = Replace-Once $dbd '#include "../Pipeline/MicroOps/Types/MicroOp.h"' 'namespace YAKSys_Hybrid_CPU::Core { class MicroOp; }' 'decoded bundle descriptor MicroOp include'
[IO.File]::WriteAllText($decodedBundleDescriptorPath, $dbd, $utf8)

$cip = [IO.File]::ReadAllText($clusterIssuePreparationPath)
$cip = Replace-Once $cip '#include "../Pipeline/MicroOps/Types/MicroOp.h"' 'namespace YAKSys_Hybrid_CPU::Core { class MicroOp; }' 'cluster issue preparation MicroOp include'
[IO.File]::WriteAllText($clusterIssuePreparationPath, $cip, $utf8)

$ipl = [IO.File]::ReadAllText($issuePacketLanePath)
$ipl = Replace-Once $ipl '#include "../MicroOps/Types/MicroOp.h"' 'namespace YAKSys_Hybrid_CPU::Core { class MicroOp; }' 'issue packet lane MicroOp include'
[IO.File]::WriteAllText($issuePacketLanePath, $ipl, $utf8)

$rpi = [IO.File]::ReadAllText($replayImplementationsPath)
$rpi = Replace-Once $rpi '#include "../MicroOps/Types/MicroOp.h"' 'namespace YAKSys_Hybrid_CPU::Core { class MicroOp; }' 'replay substrate implementations MicroOp include'
[IO.File]::WriteAllText($replayImplementationsPath, $rpi, $utf8)

$svt = [IO.File]::ReadAllText($safetyVerifierTypesPath)
$svt = Replace-Once $svt '#include "../MicroOps/Types/MicroOp.h"' 'namespace YAKSys_Hybrid_CPU::Core { class MicroOp; }' 'safety verifier types MicroOp include'
[IO.File]::WriteAllText($safetyVerifierTypesPath, $svt, $utf8)

$psa = [IO.File]::ReadAllText($postStageAttemptPath)
$psa = Replace-Once $psa '#include "../MicroOps/Types/MicroOp.h"' 'namespace YAKSys_Hybrid_CPU::Core { class MicroOp; }' 'post-stage attempt MicroOp include'
[IO.File]::WriteAllText($postStageAttemptPath, $psa, $utf8)

$sv = [IO.File]::ReadAllText($safetyVerifierPath)
$sv = Replace-Once $sv '#include "../MicroOps/Types/MicroOp.h"' 'namespace YAKSys_Hybrid_CPU::Core { class MicroOp; }' 'safety verifier MicroOp include'
[IO.File]::WriteAllText($safetyVerifierPath, $sv, $utf8)
$sv = [IO.File]::ReadAllText($safetyVerifierPath)
if (-not $sv.Contains('#include "../MicroOps/Types/MicroOpMemoryRange.h"')) {
    $sv = Replace-Once $sv '#include "../Certificates/BundleResourceCertificate.h"' "#include ""../Certificates/BundleResourceCertificate.h""`r`n#include ""../MicroOps/Types/MicroOpMemoryRange.h""" 'standalone memory range include'
}
$sv = $sv.Replace('MicroOp::MemoryRange', 'MicroOpMemoryRange')
[IO.File]::WriteAllText($safetyVerifierPath, $sv, $utf8)

$mh = [IO.File]::ReadAllText($microOpHeaderPath)
if (-not $mh.Contains('#include "MicroOpMemoryRange.h"')) {
    $mh = Replace-Once $mh '#include "../../../Architecture/Registers/Retire/RetireRecord.h"' "#include ""../../../Architecture/Registers/Retire/RetireRecord.h""`r`n#include ""MicroOpMemoryRange.h""" 'MicroOp memory range include'
}
$rangePattern = '(?s)\t\tstruct MemoryRange\s*\{\s*unsigned long long Address;\s*unsigned long long Length;\s*\};'
if ([regex]::IsMatch($mh, $rangePattern)) { $mh = [regex]::Replace($mh, $rangePattern, "`t`tusing MemoryRange = MicroOpMemoryRange;", 1) }
[IO.File]::WriteAllText($microOpHeaderPath, $mh, $utf8)

$dbdh = [IO.File]::ReadAllText($decodedBundleHeaderPath)
if (-not $dbdh.Contains('class MicroOpAdmissionMetadata;')) {
    $dbdh = Replace-Once $dbdh 'namespace YAKSys_Hybrid_CPU::Core { class TrapMicroOp; }' "namespace YAKSys_Hybrid_CPU::Core { class TrapMicroOp; }`r`nnamespace YAKSys_Hybrid_CPU::Core { class MicroOpAdmissionMetadata; }" 'descriptor metadata forward declaration'
}
[IO.File]::WriteAllText($decodedBundleHeaderPath, $dbdh, $utf8)

$h = [IO.File]::ReadAllText($schedulerPath)
$opening = @'
namespace YAKSys_Hybrid_CPU::Core
{
    class MicroOpScheduler;
    class InterCoreAssistNomination final : public std::enable_shared_from_this<InterCoreAssistNomination>
    {
    private:
        const std::shared_ptr<MicroOpScheduler> owner;
        const int coreId;
    public:
        InterCoreAssistNomination(std::shared_ptr<MicroOpScheduler> owner, int coreId);
        void Withdraw();
    };
'@
if (-not $h.Contains('class InterCoreAssistNomination final : public std::enable_shared_from_this<InterCoreAssistNomination>')) {
$h = Replace-Once $h "namespace YAKSys_Hybrid_CPU::Core`r`n{" $opening 'scheduler namespace opening'
}
$nestedPattern = '(?s)\t\tclass InterCoreAssistNomination final : public std::enable_shared_from_this<InterCoreAssistNomination>\s*\{.*?\t\t\};'
$nestedCount = ([regex]::Matches($h, $nestedPattern)).Count
if ($nestedCount -eq 1) {
    $h = [regex]::Replace($h, $nestedPattern, "`t`tusing InterCoreAssistNomination = ::YAKSys_Hybrid_CPU::Core::InterCoreAssistNomination;", 1)
} elseif ($nestedCount -ne 0 -or -not $h.Contains('using InterCoreAssistNomination = ::YAKSys_Hybrid_CPU::Core::InterCoreAssistNomination;')) {
    throw "Expected one nested nomination declaration or its replacement; found $nestedCount"
}
[IO.File]::WriteAllText($schedulerPath, $h, $utf8)

$cpp = [IO.File]::ReadAllText($schedulerCppPath)
$cppCount = ([regex]::Matches($cpp, [regex]::Escape('MicroOpScheduler::InterCoreAssistNomination::'))).Count
if ($cppCount -gt 0) { $cpp = $cpp.Replace('MicroOpScheduler::InterCoreAssistNomination::', 'InterCoreAssistNomination::') }
elseif (-not $cpp.Contains('InterCoreAssistNomination::')) { throw 'Missing nomination member definitions.' }
[IO.File]::WriteAllText($schedulerCppPath, $cpp, $utf8)

$pcpp = [IO.File]::ReadAllText($processorCppPath)
$pcppCount = ([regex]::Matches($pcpp, [regex]::Escape('Core::MicroOpScheduler::InterCoreAssistNomination'))).Count
if ($pcppCount -gt 0) { $pcpp = $pcpp.Replace('Core::MicroOpScheduler::InterCoreAssistNomination', 'Core::InterCoreAssistNomination') }
elseif (-not $pcpp.Contains('Core::InterCoreAssistNomination')) { throw 'Missing Processor nomination definitions.' }
[IO.File]::WriteAllText($processorCppPath, $pcpp, $utf8)
