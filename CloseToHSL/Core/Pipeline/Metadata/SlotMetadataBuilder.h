#pragma once

#include "ISlotMetadataBuilder.h"
#include <optional>
#include <memory>

// ─────────────────────────────────────────────────────────────────────────────
// HybridCPU ISA v4 — Pipeline Metadata
// V5 Phase 2: Decoder Disentangling
// ─────────────────────────────────────────────────────────────────────────────

using namespace YAKSys_Hybrid_CPU::Core::Pipeline::MicroOps;

namespace YAKSys_Hybrid_CPU::Core::Pipeline::Metadata
{
	/// <summary>
	/// Default <see cref="ISlotMetadataBuilder"/> implementation.
	///
	/// Mapping rules:
	/// <list type="bullet">
	///   <item>When <c>annotation</c> is <see langword="null"/> — all <see cref="SlotMetadata"/> fields
	///         take their default values.</item>
	///   <item>Otherwise — every non-default field in <c>annotation</c> is forwarded
	///         verbatim into the corresponding <see cref="SlotMetadata"/> field.</item>
	/// </list>
	///
	/// The <paramref name="instruction"/> parameter is accepted by the interface contract for
	/// future structural defaults but is not currently consulted for any policy decision.
	/// </summary>
	class SlotMetadataBuilder final : public ISlotMetadataBuilder
	{
		/// <inheritdoc />
	public:
		std::shared_ptr<SlotMetadata> Build(std::shared_ptr<InstructionIR> instruction, std::optional<std::shared_ptr<CompilerAnnotation>> &annotation) override;
	};
}
