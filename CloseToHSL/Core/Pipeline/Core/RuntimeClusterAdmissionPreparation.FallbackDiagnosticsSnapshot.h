#pragma once

namespace YAKSys_Hybrid_CPU::Core
{
// C# TO C++ CONVERTER NOTE: The following .NET attribute has no direct equivalent in C++:
// ORIGINAL LINE: [Flags] internal enum RuntimeClusterFallbackReasonMask : byte
	enum class RuntimeClusterFallbackReasonMask : unsigned char
	{
		None = 0,
		ResidualDiagnostics = 1 << 0,
		BlockedScalarCandidates = 1 << 1,
		RegisterHazard = 1 << 2,
		OrderingHazard = 1 << 3
	};

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to the C# readonly struct:
// ORIGINAL LINE: internal readonly struct ClusterFallbackDiagnosticsSnapshot
	class ClusterFallbackDiagnosticsSnapshot
	{
	private:
		unsigned char FallbackDiagnosticsMask;
		bool SuggestsFallbackDiagnostics;
		RuntimeClusterFallbackReasonMask FallbackReasonMask;

	public:
		ClusterFallbackDiagnosticsSnapshot(unsigned char fallbackDiagnosticsMask, bool suggestsFallbackDiagnostics, RuntimeClusterFallbackReasonMask fallbackReasonMask = RuntimeClusterFallbackReasonMask::None);

		/// <summary>
		/// Runtime-readable mirror of the decode-side residual narrow-path mask.
		/// Read-only diagnostics snapshot: it must not become a semantic owner for system/CSR/FSM behavior.
		/// </summary>
		unsigned char getFallbackDiagnosticsMask() const;

		/// <summary>
		/// Advisory decode/runtime hint that some slots still rely on the narrow reference path.
		/// Observability only; runtime legality, retire, and privileged semantics remain explicit elsewhere.
		/// </summary>
		bool getSuggestsFallbackDiagnostics() const;

		/// <summary>
		/// Category mask explaining why a scalar-cluster candidate still degrades to the reference path.
		/// This is derived from decode/runtime diagnostics so semantic fallback accounting can
		/// use an explicit authority instead of reading the diagnostics mirror directly.
		/// </summary>
		RuntimeClusterFallbackReasonMask getFallbackReasonMask() const;

		static RuntimeClusterFallbackReasonMask DeriveReasonMask(unsigned char fallbackDiagnosticsMask, unsigned char blockedScalarCandidateMask, unsigned char registerHazardMask, unsigned char orderingHazardMask);

		ClusterFallbackDiagnosticsSnapshot() = default;
	};
}
