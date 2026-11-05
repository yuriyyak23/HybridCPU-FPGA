#pragma once

#include "../../../Execution/Rf07ExecutionOutcomeContracts.h"
#include "../../../Decoder/Rf06ExecutionContracts.h"
#include "RetireRecord.h"
#include <string>
#include <unordered_set>
#include <vector>
#include <optional>
#include <format>
#include <memory>

namespace YAKSys_Hybrid_CPU::Core::Decoder { class ScheduledOperation; }
// C# TO C++ CONVERTER NOTE: Forward class declarations:
namespace YAKSys_Hybrid_CPU::Core::Registers::Retire { class RetireEffectIdentityContractViolationException; }
namespace YAKSys_Hybrid_CPU::Core::Decoder { class SourceOperationProvenance; }

using namespace YAKSys_Hybrid_CPU::Core::Decoder;
using namespace YAKSys_Hybrid_CPU::Core::Execution;

namespace YAKSys_Hybrid_CPU::Core::Registers::Retire
{

	/// <summary>
	/// Closed RF-08 effect-family vocabulary. This classifies an existing retire-visible
	/// effect; it does not select retirement or publish architectural state.
	/// </summary>
	enum class RetireVisibleEffectKind : unsigned char
	{
		RegisterWrite = 1,
		PcWrite = 2,
		CsrWrite = 3,
		VectorConfigWrite = 4,
		DeferredStoreCommit = 5,
		ScalarMemoryStoreCommit = 6,
		AtomicCommit = 7,
		SystemCommit = 8,
		VmxCommit = 9,
		TrapCommit = 10,
		PipelineEventPublication = 11,
		PredicateStateWrite = 12,
		VectorStreamDirty = 13,
		MatrixTileCommit = 14,
		AcceleratorCommit = 15
	};

	/// <summary>
	/// Immutable identity projection for one retire-visible effect of one existing issued
	/// attempt. It creates no operation ID and owns no retire selection, publication,
	/// physical-register, rename, commit-map, free-list, checkpoint, squash, or recovery
	/// authority.
	/// </summary>
	class RetireVisibleEffectIdentity final : public std::enable_shared_from_this<RetireVisibleEffectIdentity>
	{
	private:
		const std::shared_ptr<YAKSys_Hybrid_CPU::Core::Execution::ExecutionRecord> ExecutionRecord;
		const std::shared_ptr<YAKSys_Hybrid_CPU::Core::Decoder::ScheduledOperation> ScheduledOperation;
		const std::shared_ptr<VliwOperationId> OperationId;
		const std::shared_ptr<GeneratedStaticBinding> GeneratedBinding;
		const std::shared_ptr<SourceOperationProvenance> SourceProvenance;
		const RetireVisibleEffectKind EffectKind;
		const int EffectOrdinal;
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly System.Nullable<int> ArchitecturalRegisterId;
		std::optional<int> ArchitecturalRegisterId;

		RetireVisibleEffectIdentity(std::shared_ptr<YAKSys_Hybrid_CPU::Core::Execution::ExecutionRecord> executionRecord, RetireVisibleEffectKind effectKind, int effectOrdinal, std::optional<int> architecturalRegisterId);

	public:
		std::shared_ptr<YAKSys_Hybrid_CPU::Core::Execution::ExecutionRecord> getExecutionRecord() const;
		std::shared_ptr<YAKSys_Hybrid_CPU::Core::Decoder::ScheduledOperation> getScheduledOperation() const;
		std::shared_ptr<VliwOperationId> getOperationId() const;
		std::shared_ptr<GeneratedStaticBinding> getGeneratedBinding() const;
		std::shared_ptr<SourceOperationProvenance> getSourceProvenance() const;
		RetireVisibleEffectKind getEffectKind() const;
		int getEffectOrdinal() const;
		std::optional<int> getArchitecturalRegisterId() const;
		int getVirtualThreadId() const;
		unsigned long long getWorkingBundleSequence() const;
		int getWorkingSlotIndex() const;
		unsigned long long getOperationAttempt() const;
		int getPhysicalLaneIndex() const;
		unsigned long long getSourceBundleSerial() const;
		int getSourceSlotIndex() const;

		static std::shared_ptr<RetireVisibleEffectIdentity> Freeze(std::shared_ptr<YAKSys_Hybrid_CPU::Core::Execution::ExecutionRecord> executionRecord, RetireVisibleEffectKind effectKind, int effectOrdinal, int effectVirtualThreadId, std::optional<int> architecturalRegisterId = std::nullopt);

	private:
		static RetireEffectIdentityContractViolationException Violation(const std::wstring &message);
		static RetireEffectIdentityContractViolationException Violation(const std::string &message);
	};

	/// <summary>
	/// Additive projection binding the existing RetireRecord payload to one frozen effect
	/// identity. Production retirement does not consume this projection in RF-08.0.
	/// </summary>
	class RetireRecordIdentityProjection final : public std::enable_shared_from_this<RetireRecordIdentityProjection>
	{
	private:
		const YAKSys_Hybrid_CPU::Core::Registers::Retire::RetireRecord RetireRecord;
		const std::shared_ptr<RetireVisibleEffectIdentity> Identity;

		RetireRecordIdentityProjection(YAKSys_Hybrid_CPU::Core::Registers::Retire::RetireRecord retireRecord, std::shared_ptr<RetireVisibleEffectIdentity> identity);

	public:
		YAKSys_Hybrid_CPU::Core::Registers::Retire::RetireRecord getRetireRecord() const;
		std::shared_ptr<RetireVisibleEffectIdentity> getIdentity() const;

		static std::shared_ptr<RetireRecordIdentityProjection> Create(YAKSys_Hybrid_CPU::Core::Registers::Retire::RetireRecord const retireRecord, std::shared_ptr<RetireVisibleEffectIdentity> identity);
	};

	/// <summary>
	/// Evidence-only RF-08.0 coherence checks. They validate a proposed publication
	/// transition but do not perform prevalidation, mutate the backend, or publish effects.
	/// </summary>
	class RetireVisibleEffectCoherence final
	{
	public:
		static void ValidatePublicationClaim(std::shared_ptr<RetireVisibleEffectIdentity> identity, std::shared_ptr<ScheduledOperation> scheduledOperation, std::shared_ptr<ExecutionRecord> executionRecord, std::shared_ptr<GeneratedStaticBinding> generatedBinding, bool prevalidationComplete, bool selectedByRetireProtocol, bool requestsStoreVisibility);

		static void ValidateDistinctTerminalEffects(std::vector<std::shared_ptr<RetireVisibleEffectIdentity>> &identities);

	private:
		static RetireEffectIdentityContractViolationException Violation(const std::wstring &message);
		static RetireEffectIdentityContractViolationException Violation(const std::string &message);
	};

	/// <summary>
	/// Fail-closed programming/invariant error. It is not an architectural fault and
	/// cannot be converted into retry/not-ready state.
	/// </summary>
	class RetireEffectIdentityContractViolationException final : public InvalidOperationException
	{
	public:
		RetireEffectIdentityContractViolationException(const std::wstring &message);
		RetireEffectIdentityContractViolationException(const std::string &message);
	};

}
