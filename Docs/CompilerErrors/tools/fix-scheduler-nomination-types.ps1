$ErrorActionPreference='Stop'
$root=Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
$p=Join-Path $root 'CloseToHSL\\Core\Pipeline\Assist\InterCore\YAKSys_Hybrid_CPU.Core.MicroOpScheduler.h';$s=Get-Content $p -Raw
$a=$s.IndexOf("`t`tclass FspSnapshotFingerprint`r`n");if($a -lt 0){$a=$s.IndexOf("`t`tclass FspSnapshotFingerprint`n")};$b=$s.IndexOf('FspSnapshotFingerprint() = default;',$a);$end=$s.IndexOf('};',$b)+2;$dest=$s.IndexOf("`t`tclass FspPipelineRegister")
if($a -lt 0 -or $b -lt 0 -or $end -lt 2 -or $dest -lt 0){throw 'Fingerprint boundaries missing'}
$block=$s.Substring($a,$end-$a).Replace('const unsigned long long Low;', 'unsigned long long Low = 0;').Replace('const unsigned long long High;', 'unsigned long long High = 0;')
$s=$s.Remove($a,$end-$a);$dest=$s.IndexOf("`t`tclass FspPipelineRegister");$s=$s.Insert($dest,$block+"`r`n`r`n")
$s=$s.Replace('readonly MicroOp[]? _candidates;', 'std::vector<std::shared_ptr<MicroOp>> _candidates;').Replace('std::vector<std::deque<std::shared_ptr<MicroOp>>&>', 'std::vector<std::deque<std::shared_ptr<MicroOp>>>')
Set-Content $p $s -Encoding UTF8 -NoNewline
$p=Join-Path $root 'CloseToHSL\\Core\Pipeline\Assist\InterCore\YAKSys_Hybrid_CPU.Core.MicroOpScheduler.cpp';$s=Get-Content $p -Raw;$s=$s.Replace('std::vector<std::deque<std::shared_ptr<MicroOp>>&>', 'std::vector<std::deque<std::shared_ptr<MicroOp>>>'); Set-Content $p $s -Encoding UTF8 -NoNewline
