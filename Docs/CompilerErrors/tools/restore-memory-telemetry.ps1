$ErrorActionPreference='Stop'
$dir=Join-Path (Split-Path -Parent (Split-Path -Parent $PSScriptRoot)) 'CloseToHSL\\Memory\Timing'
$p=Join-Path $dir 'MemoryCycleTelemetrySnapshot.h'
$s=Get-Content $p -Raw
if (!$s.Contains('struct MemoryCycleTelemetrySnapshot final')) {
 $i=$s.LastIndexOf('}')
 $body=@"
    struct MemoryCycleTelemetrySnapshot final {
        unsigned long long ControllerCycles = 0, ReadServiceCycles = 0, StoreReadinessServiceCycles = 0, CompletionPublicationCycles = 0;
        unsigned long long AcceptedRequests = 0, CompletedRequests = 0, DataReadAcceptedRequests = 0, DataReadCompletedRequests = 0;
        unsigned long long DataWriteAcceptedRequests = 0, DataWriteCompletedRequests = 0, DataReadBytes = 0, CommittedDataWriteBytes = 0;
        unsigned long long InstructionFetchReadBytes = 0, QueueFullRejects = 0, TelemetryBaselineOutstandingRequests = 0, CanceledRequests = 0, ConsumedCompletions = 0, OutstandingRequests = 0;
        static constexpr const wchar_t* SchemaVersion = L"memory-cycle-telemetry-v2";
        static constexpr const wchar_t* PreviousSchemaVersion = L"memory-cycle-telemetry-v1";
        static constexpr bool InstructionFetchRequestTelemetryAvailable = false;
        static constexpr bool BankConflictRejectTelemetryAvailable = false;
        bool operator==(const MemoryCycleTelemetrySnapshot&) const = default;
    };
"@
 $s=$s.Insert($i,$body+"`r`n"); Set-Content $p $s -Encoding UTF8 -NoNewline
}
$p=Join-Path $dir 'MemoryCycleController.h'; $s=Get-Content $p -Raw
if (!$s.Contains('#include "MemoryCycleTelemetrySnapshot.h"')) { $s=$s.Replace('#include "MemoryAdmissionTypes.h"', "#include `"MemoryAdmissionTypes.h`"`r`n#include `"MemoryCycleTelemetrySnapshot.h`"") }
Set-Content $p $s -Encoding UTF8 -NoNewline
