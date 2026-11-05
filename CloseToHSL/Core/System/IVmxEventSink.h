#pragma once

#include "VmxEventKind.h"
#include "../Virtualization/Compatibility/FrozenAbi/VmcsFieldAliases/VmExitReason.h"
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{
	/// <summary>
	/// Sink for VMX diagnostic events.
	/// <para>
	/// This is an observability mirror of retired VMX outcomes. It is not a
	/// production semantic owner for VMX execution, VMCS lifecycle, or VM-entry /
	/// VM-exit control-flow.
	/// </para>
	/// </summary>
// C# TO C++ CONVERTER NOTE: The following .NET attribute has no direct equivalent in C++:
// ORIGINAL LINE: [EditorBrowsable(EditorBrowsableState.Never)] internal interface IVmxEventSink
	class IVmxEventSink
	{
		/// <summary>
		/// Record a VMX event for diagnostic trace and replay observability.
		/// </summary>
		/// <param name="kind">The kind of VMX event.</param>
		/// <param name="vtId">Virtual thread ID that triggered the event.</param>
		/// <param name="exitReason">VM-Exit reason when <paramref name="kind"/> is <see cref="VmxEventKind.VmExit"/>.</param>
	public:
		virtual void RecordVmxEvent(VmxEventKind kind, unsigned short vtId, VmExitReason exitReason = VmExitReason::None) = 0;
	};

	/// <summary>
	/// No-op VMX event sink that discards all events.
	/// Used when no diagnostic trace projection is configured.
	/// </summary>
// C# TO C++ CONVERTER NOTE: The following .NET attribute has no direct equivalent in C++:
// ORIGINAL LINE: [EditorBrowsable(EditorBrowsableState.Never)] internal sealed class NullVmxEventSink : IVmxEventSink
	class NullVmxEventSink final : public IVmxEventSink
	{
	public:
		static inline const std::shared_ptr<NullVmxEventSink> Instance = std::make_shared<NullVmxEventSink>();

		void RecordVmxEvent(VmxEventKind kind, unsigned short vtId, VmExitReason exitReason = VmExitReason::None) override;
	};
}
