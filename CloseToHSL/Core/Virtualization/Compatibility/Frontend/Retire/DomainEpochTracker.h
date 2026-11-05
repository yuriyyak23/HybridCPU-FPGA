#pragma once

#include "../../../../System/IVmxEventSink.h"
#include "../../../../System/VmxEventKind.h"
#include "../../FrozenAbi/VmcsFieldAliases/VmExitReason.h"
#include <optional>

namespace YAKSys_Hybrid_CPU::Core
{
	/// <summary>
	/// Tracks VM transition epoch boundaries for deterministic replay.
	/// <para>
	/// Each VM-Entry, VM-Exit, VMXON, or VMXOFF constitutes an epoch boundary.
	/// Any replay template or bundle certificate cached with an older epoch serial
	/// must be treated as invalid at the next bundle boundary.
	/// </para>
	/// <para>
	/// Implements <see cref="IVmxEventSink"/> as a diagnostics-only observer of
	/// retired VMX outcomes. It does not own live VMX semantics.
	/// </para>
	/// </summary>
// C# TO C++ CONVERTER NOTE: The following .NET attribute has no direct equivalent in C++:
// ORIGINAL LINE: [EditorBrowsable(EditorBrowsableState.Never)] internal sealed class DomainEpochTracker : IVmxEventSink
	class DomainEpochTracker final : public IVmxEventSink
	{
	private:
		unsigned long long _epoch = 0;
		VmxEventKind _lastTransitionKind = VmxEventKind::VmxOff;
		bool _hasTransitioned = false;

		/// <summary>Monotonically increasing epoch serial. Starts at 0.</summary>
	public:
		unsigned long long getCurrentEpoch() const;

		/// <summary>
		/// The VMX event kind that triggered the most-recent epoch bump,
		/// or <c>null</c> if no VM transition has occurred since construction
		/// or <see cref="Reset"/>.
		/// </summary>
		std::optional<VmxEventKind> getLastTransitionKind() const;

		/// <summary>
		/// Returns <c>true</c> when the given VMX event constitutes an epoch boundary.
		/// </summary>
		static bool IsEpochBoundary(VmxEventKind kind);

		void RecordVmxEvent(VmxEventKind kind, unsigned short vtId, VmExitReason exitReason = VmExitReason::None) override;

		/// <summary>
		/// Returns <c>true</c> when <paramref name="cachedEpoch"/> equals the
		/// current epoch, which means no VM transition has occurred since the
		/// value was cached.
		/// </summary>
		bool IsCurrentEpoch(unsigned long long cachedEpoch);

		/// <summary>
		/// Reset the epoch tracker to its initial state.
		/// </summary>
		void Reset();
	};
}
