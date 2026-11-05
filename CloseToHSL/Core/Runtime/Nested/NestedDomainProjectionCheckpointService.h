#pragma once

#include "Projection/YAKSys_Hybrid_CPU.Core.NestedProjectionService.h"
#include "../Migration/Restore/YAKSys_Hybrid_CPU.Core.RestoreValidationService.h"
#include <memory>
#include <utility>

namespace YAKSys_Hybrid_CPU::Core
{

	enum class NestedDomainProjectionCheckpointDecision : unsigned char
	{
		Allowed = 0,
		ProjectionDenied = 1,
		RestoreDenied = 2
	};

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
// ORIGINAL LINE: public readonly record struct NestedDomainProjectionCheckpointRequest(NestedProjectionRequest ProjectionRequest, DomainCheckpointImage? Checkpoint, System.Nullable<MigrationValidationPolicy> MigrationPolicy, EvidenceRestorePolicy RestorePolicy, ulong ExpectedCheckpointEpoch);
//	public readonly record struct NestedDomainProjectionCheckpointRequest(NestedProjectionRequest ProjectionRequest, DomainCheckpointImage Checkpoint, MigrationValidationPolicy MigrationPolicy, EvidenceRestorePolicy RestorePolicy, ulong ExpectedCheckpointEpoch);
	struct NestedDomainProjectionCheckpointRequest final
	{
		const std::shared_ptr<NestedProjectionRequest> ProjectionRequest;
		const std::shared_ptr<DomainCheckpointImage> Checkpoint;
		const std::shared_ptr<MigrationValidationPolicy> MigrationPolicy;
		const EvidenceRestorePolicy RestorePolicy;
		const unsigned long long ExpectedCheckpointEpoch;

		NestedDomainProjectionCheckpointRequest(std::shared_ptr<NestedProjectionRequest> projectionRequest,
			std::shared_ptr<DomainCheckpointImage> checkpoint, std::shared_ptr<MigrationValidationPolicy> migrationPolicy,
			EvidenceRestorePolicy restorePolicy, unsigned long long expectedCheckpointEpoch)
			: ProjectionRequest(std::move(projectionRequest)), Checkpoint(std::move(checkpoint)),
			  MigrationPolicy(std::move(migrationPolicy)), RestorePolicy(restorePolicy),
			  ExpectedCheckpointEpoch(expectedCheckpointEpoch) {}
	};

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct NestedDomainProjectionCheckpointResult(NestedDomainProjectionCheckpointDecision Decision, NestedProjectionResult ProjectionResult, RestoreValidationResult RestoreResult, string Reason)
//	{
//		public bool IsAllowed
//		{
//			get
//			{
//				return Decision == NestedDomainProjectionCheckpointDecision.Allowed;
//			}
//		}
//
//		public static NestedDomainProjectionCheckpointResult Allowed(NestedProjectionResult projection, RestoreValidationResult restore)
//		{
//			return new(NestedDomainProjectionCheckpointDecision.Allowed, projection, restore, string.Empty);
//		}
//
//		public static NestedDomainProjectionCheckpointResult DeniedProjection(NestedProjectionResult projection)
//		{
//			return new(NestedDomainProjectionCheckpointDecision.ProjectionDenied, projection, default, projection.Reason);
//		}
//
//		public static NestedDomainProjectionCheckpointResult DeniedRestore(NestedProjectionResult projection, RestoreValidationResult restore)
//		{
//			return new(NestedDomainProjectionCheckpointDecision.RestoreDenied, projection, restore, restore.Message);
//		}
//	}

	struct NestedDomainProjectionCheckpointResult final
	{
		const NestedDomainProjectionCheckpointDecision Decision;
		const std::shared_ptr<NestedProjectionResult> ProjectionResult;
		const std::shared_ptr<RestoreValidationResult> RestoreResult;
		const std::wstring Reason;
		const bool IsAllowed;

		NestedDomainProjectionCheckpointResult(NestedDomainProjectionCheckpointDecision decision,
			std::shared_ptr<NestedProjectionResult> projection, std::shared_ptr<RestoreValidationResult> restore,
			std::wstring reason)
			: Decision(decision), ProjectionResult(std::move(projection)), RestoreResult(std::move(restore)),
			  Reason(std::move(reason)), IsAllowed(decision == NestedDomainProjectionCheckpointDecision::Allowed) {}

		static std::shared_ptr<NestedDomainProjectionCheckpointResult> Allowed(
			std::shared_ptr<NestedProjectionResult> projection, std::shared_ptr<RestoreValidationResult> restore)
		{
			return std::make_shared<NestedDomainProjectionCheckpointResult>(NestedDomainProjectionCheckpointDecision::Allowed,
				std::move(projection), std::move(restore), L"");
		}
		static std::shared_ptr<NestedDomainProjectionCheckpointResult> DeniedProjection(std::shared_ptr<NestedProjectionResult> projection)
		{
			const std::wstring reason = projection ? projection->Reason : L"Nested compatibility projection was denied.";
			return std::make_shared<NestedDomainProjectionCheckpointResult>(NestedDomainProjectionCheckpointDecision::ProjectionDenied,
				std::move(projection), nullptr, reason);
		}
		static std::shared_ptr<NestedDomainProjectionCheckpointResult> DeniedRestore(
			std::shared_ptr<NestedProjectionResult> projection, std::shared_ptr<RestoreValidationResult> restore)
		{
			const std::wstring reason = restore ? restore->Message : L"Nested checkpoint restore was denied.";
			return std::make_shared<NestedDomainProjectionCheckpointResult>(NestedDomainProjectionCheckpointDecision::RestoreDenied,
				std::move(projection), std::move(restore), reason);
		}
	};

	class NestedDomainProjectionCheckpointService final : public std::enable_shared_from_this<NestedDomainProjectionCheckpointService>
	{
	private:
		const std::shared_ptr<NestedProjectionService> _projectionService = std::make_shared<NestedProjectionService>();
		const std::shared_ptr<RestoreValidationService> _restoreService = std::make_shared<RestoreValidationService>();

	public:
		std::shared_ptr<NestedDomainProjectionCheckpointResult> Validate(std::shared_ptr<NestedDomainProjectionCheckpointRequest> request);
	};

}
