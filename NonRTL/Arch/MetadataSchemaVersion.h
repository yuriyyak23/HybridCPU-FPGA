#pragma once

namespace YAKSys_Hybrid_CPU::Core
{
	/// <summary>
	/// Canonical metadata schema version constant for HybridCPU v4.
	/// Referenced by <see cref="YAKSys_Hybrid_CPU.Core.SlotMetadata"/> and
	/// <see cref="YAKSys_Hybrid_CPU.Core.BundleMetadata"/> to stamp emitted metadata.
	/// </summary>
	class MetadataSchemaVersion final
	{
		/// <summary>
		/// Current ISE metadata schema version.
		/// Incremented when the schema is extended in a breaking way.
		/// </summary>
	public:
		static constexpr unsigned char Current = 4;
	};
}
