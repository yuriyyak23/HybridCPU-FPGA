$root=Join-Path (Split-Path -Parent (Split-Path -Parent $PSScriptRoot)) '2'
$fixes=@(
 @{File='CloseToHSL\Core\Runtime\Managed\HybridCpuIseManagedGuestExecutionV1.cpp';Old='return register_Keyword is >= 0 and_Keyword <= 31;';New='return register_Keyword >= 0 && register_Keyword <= 31;'},
 @{File='CloseToHSL\Core\Architecture\Common\YAKSys_Hybrid_CPU.Processor.cpp';Old='return issueLane.getPhysicalLaneIndex() is >= 4 and_Keyword < 8;';New='const auto laneIndex = issueLane.getPhysicalLaneIndex(); return laneIndex >= 4 && laneIndex < 8;'},
 @{File='CloseToHSL\Core\Runtime\Completion\Commit\ArchitecturalCompletionCommitOwner.cpp';Old='candidate->VirtualThreadId is >= 0 and_Keyword < Processor::CPU_Core::SmtWays';New='(candidate->VirtualThreadId >= 0 && candidate->VirtualThreadId < Processor::CPU_Core::SmtWays)'}
)
foreach($f in $fixes){$path=Join-Path $root $f.File; $text=[IO.File]::ReadAllText($path); if($text.Contains($f.Old)){ $text=$text.Replace($f.Old,$f.New); [IO.File]::WriteAllText($path,$text,[Text.UTF8Encoding]::new($false)); Write-Output "Fixed $($f.File)" } elseif($text.Contains($f.New)){Write-Output "Already fixed $($f.File)"}else{throw "Expected source not found $path"}}