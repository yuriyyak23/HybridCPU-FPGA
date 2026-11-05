#pragma once

#include "../Pipeline/Assist/InterCore/YAKSys_Hybrid_CPU.Core.MicroOpScheduler.h"
#include "../Execution/ExternalAccelerators/Telemetry/AcceleratorTelemetry.h"
#include "../Architecture/Common/YAKSys_Hybrid_CPU.Processor.h"
#include "../Pipeline/Certificates/ReplayPhaseSubstrate.Implementations.h"
#include "TypedSlotTelemetryProfile.h"
#include "../Execution/DmaStreamCompute/DmaStreamComputeTelemetry.h"
#include "../Pipeline/Scheduling/SlotLegality/SlotClassDefinitions.h"
#include <string>
#include <unordered_map>
#include <vector>
#include <optional>
#include <concepts>
#include "tangible_exception_helper.h"
#include <memory>
#include "../../../tangible_string_helper.h"

using namespace YAKSys_Hybrid_CPU::Core::Execution::DmaStreamCompute;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Telemetry;

namespace YAKSys_Hybrid_CPU::Core::Diagnostics
{

	/// <summary>
	/// Collects existing runtime telemetry counters from <see cref="MicroOpScheduler"/>
	/// and serializes them as <see cref="TypedSlotTelemetryProfile"/> JSON.
	/// <para>
	/// This is an <b>additive-only</b> ISE-side class — no existing scheduling,
	/// admission, or fairness logic is modified.
	/// </para>
	/// </summary>
	class TelemetryExporter final
	{
	private:
		static constexpr int SmtWays = 4;

		/// <summary>
		/// Builds a <see cref="TypedSlotTelemetryProfile"/> from the current scheduler state.
		/// </summary>
		/// <param name="scheduler">The micro-op scheduler with accumulated counters.</param>
		/// <param name="programHash">Hash of the compiled program for staleness detection.</param>
		/// <param name="workerMetrics">Optional per-worker metrics from Phase 07 parallel execution.</param>
	public:
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: public static TypedSlotTelemetryProfile BuildProfile(MicroOpScheduler scheduler, string programHash, IReadOnlyDictionary<string, WorkerPerformanceMetrics>? workerMetrics = null)
		static std::shared_ptr<TypedSlotTelemetryProfile> BuildProfile(std::shared_ptr<MicroOpScheduler> scheduler, const std::wstring &programHash, std::optional<std::unordered_map<std::wstring, WorkerPerformanceMetrics>> workerMetrics = std::nullopt);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: public static TypedSlotTelemetryProfile BuildProfile(MicroOpScheduler scheduler, string programHash, DmaStreamComputeTelemetrySnapshot dmaStreamComputeTelemetry, IReadOnlyDictionary<string, WorkerPerformanceMetrics>? workerMetrics = null)
		static std::shared_ptr<TypedSlotTelemetryProfile> BuildProfile(std::shared_ptr<MicroOpScheduler> scheduler, const std::wstring &programHash, std::shared_ptr<DmaStreamComputeTelemetrySnapshot> dmaStreamComputeTelemetry, std::optional<std::unordered_map<std::wstring, WorkerPerformanceMetrics>> workerMetrics = std::nullopt);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: public static TypedSlotTelemetryProfile BuildProfile(MicroOpScheduler scheduler, string programHash, AcceleratorTelemetrySnapshot acceleratorTelemetry, IReadOnlyDictionary<string, WorkerPerformanceMetrics>? workerMetrics = null)
		static std::shared_ptr<TypedSlotTelemetryProfile> BuildProfile(std::shared_ptr<MicroOpScheduler> scheduler, const std::wstring &programHash, std::shared_ptr<AcceleratorTelemetrySnapshot> acceleratorTelemetry, std::optional<std::unordered_map<std::wstring, WorkerPerformanceMetrics>> workerMetrics = std::nullopt);

		static std::shared_ptr<AcceleratorTelemetrySnapshot> ExportAcceleratorTelemetry(std::shared_ptr<AcceleratorTelemetry> telemetry);

		/// <summary>
		/// Builds a <see cref="TypedSlotTelemetryProfile"/> from the current scheduler state and optional pipeline telemetry.
		/// </summary>
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: public static TypedSlotTelemetryProfile BuildProfile(MicroOpScheduler scheduler, string programHash, System.Nullable<Processor.CPU_Core.PipelineControl> pipelineControl, IReadOnlyDictionary<string, WorkerPerformanceMetrics>? workerMetrics = null, System.Nullable<DmaStreamComputeTelemetrySnapshot> dmaStreamComputeTelemetry = null, System.Nullable<AcceleratorTelemetrySnapshot> acceleratorTelemetry = null)
		static std::shared_ptr<TypedSlotTelemetryProfile> BuildProfile(std::shared_ptr<MicroOpScheduler> scheduler, const std::wstring &programHash, std::optional<Processor::CPU_Core::PipelineControl> pipelineControl, std::optional<std::unordered_map<std::wstring, WorkerPerformanceMetrics>> workerMetrics = std::nullopt, std::optional<std::shared_ptr<DmaStreamComputeTelemetrySnapshot>> dmaStreamComputeTelemetry = std::nullopt, std::optional<std::shared_ptr<AcceleratorTelemetrySnapshot>> acceleratorTelemetry = std::nullopt);

	private:
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private static Dictionary<SlotClass, long>? BuildCertificateRejectsPerClass(SchedulerPhaseMetrics metrics)
		static std::unordered_map<SlotClass, long long> BuildCertificateRejectsPerClass(SchedulerPhaseMetrics metrics);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private static Dictionary<SlotClass, long>? BuildSmtLegalityRejectsPerClass(SchedulerPhaseMetrics metrics)
		static std::unordered_map<SlotClass, long long> BuildSmtLegalityRejectsPerClass(SchedulerPhaseMetrics metrics);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private static Dictionary<int, long>? BuildPerVtRejectionCounts(SchedulerPhaseMetrics metrics)
		static std::unordered_map<int, long long> BuildPerVtRejectionCounts(SchedulerPhaseMetrics metrics);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private static Dictionary<int, long>? BuildPerVtRegGroupConflicts(SchedulerPhaseMetrics metrics)
		static std::unordered_map<int, long long> BuildPerVtRegGroupConflicts(SchedulerPhaseMetrics metrics);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private static Dictionary<int, long>? BuildBankPendingRejectsPerBank(SchedulerPhaseMetrics metrics)
		static std::unordered_map<int, long long> BuildBankPendingRejectsPerBank(SchedulerPhaseMetrics metrics);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private static long GetValueOrDefault(IReadOnlyDictionary<SlotClass, long>? values, SlotClass slotClass)
		static long long GetValueOrDefault(const std::unordered_map<SlotClass, long long> &values, SlotClass slotClass);

		template<typename TKey>
		static void AddIfPositive(std::unordered_map<TKey, long long> &dictionary, TKey key, long long value)
		{
			if (value > 0)
			{
				dictionary[key] = value;
			}
		}

		static std::optional<double> NullIfZero(double value);

		/// <summary>
		/// Serializes a telemetry profile to a JSON string.
		/// </summary>
	public:
		static std::wstring SerializeToJson(std::shared_ptr<TypedSlotTelemetryProfile> profile);

		/// <summary>
		/// Deserializes a telemetry profile from a JSON string.
		/// Returns <c>null</c> if the JSON is invalid.
		/// </summary>
		static std::optional<std::shared_ptr<TypedSlotTelemetryProfile>> DeserializeFromJson(const std::wstring &json);
	};

}
