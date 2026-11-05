#pragma once

namespace HybridCPU_ISE::CloseToHSL::Memory::Timing
{

	/// <summary>
	/// Additive, observation-only counters for the RF-10 timed-memory domain.
	/// Classified request counts are properties of a request and are therefore
	/// not a partition: a canonical vector transfer is both a data read and a
	/// future data write, while total counts still count its identity once.
	/// </summary>
// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct MemoryCycleTelemetrySnapshot(ulong ControllerCycles, ulong ReadServiceCycles, ulong StoreReadinessServiceCycles, ulong CompletionPublicationCycles, ulong AcceptedRequests, ulong CompletedRequests, ulong DataReadAcceptedRequests, ulong DataReadCompletedRequests, ulong DataWriteAcceptedRequests, ulong DataWriteCompletedRequests, ulong DataReadBytes, ulong CommittedDataWriteBytes, ulong InstructionFetchReadBytes, ulong QueueFullRejects, ulong TelemetryBaselineOutstandingRequests = 0, ulong CanceledRequests = 0, ulong ConsumedCompletions = 0, ulong OutstandingRequests = 0)
//	{
//		public const string SchemaVersion = "memory-cycle-telemetry-v2";
//		public const string PreviousSchemaVersion = "memory-cycle-telemetry-v1";
//
//		// Fetch remains a synchronous cache/main-memory contour and has no
//		// controller request-identity protocol to count truthfully.
//		public const bool InstructionFetchRequestTelemetryAvailable = false;
//
//		// Controller-native admission currently has capacity backpressure but no
//		// bank-conflict rejection result. Scheduler bank conflicts and legacy
//		// MemorySubsystem conflicts are different counters and must not be merged.
//		public const bool BankConflictRejectTelemetryAvailable = false;
//	}

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
}
