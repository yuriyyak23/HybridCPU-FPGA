#pragma once

#include "IVmxEventSink.h"
#include "VmxEventKind.h"
#include "../Virtualization/Compatibility/FrozenAbi/VmcsFieldAliases/VmExitReason.h"
#include <stdexcept>
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{
	/// <summary>
	/// An <see cref="IVmxEventSink"/> that forwards every event to two underlying
	/// diagnostics sinks in order: <see cref="Primary"/> first, then <see cref="Secondary"/>.
	/// <para>
	/// Typical use: wire a <see cref="DomainEpochTracker"/> alongside a diagnostic
	/// recording sink without changing compatibility event publication contracts.
	/// </para>
	/// </summary>
// C# TO C++ CONVERTER NOTE: The following .NET attribute has no direct equivalent in C++:
// ORIGINAL LINE: [EditorBrowsable(EditorBrowsableState.Never)] internal sealed class CompositeVmxEventSink : IVmxEventSink
	class CompositeVmxEventSink final : public IVmxEventSink
	{
	private:
		const std::shared_ptr<IVmxEventSink> Primary;
		const std::shared_ptr<IVmxEventSink> Secondary;

		/// <summary>The first sink to receive each event.</summary>
	public:
		std::shared_ptr<IVmxEventSink> getPrimary() const;

		/// <summary>The second sink to receive each event.</summary>
		std::shared_ptr<IVmxEventSink> getSecondary() const;

		/// <param name="primary">First sink.</param>
		/// <param name="secondary">Second sink.</param>
		CompositeVmxEventSink(std::shared_ptr<IVmxEventSink> primary, std::shared_ptr<IVmxEventSink> secondary);

		void RecordVmxEvent(VmxEventKind kind, unsigned short vtId, VmExitReason exitReason = VmExitReason::None) override;
	};
}
