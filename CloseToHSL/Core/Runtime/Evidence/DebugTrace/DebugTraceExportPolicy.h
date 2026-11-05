#pragma once

#include <memory>
#include <string>
#include <utility>

namespace YAKSys_Hybrid_CPU::Core
{
	enum class DebugTraceExportDecision : unsigned char
	{
		Allowed = 0,
		GuestExportDenied = 1,
		HostLocalExportDenied = 2,
		EvidencePolicyDenied = 3,
		RedactionRequired = 4
	};

	struct DebugTraceExportResult final
	{
		const DebugTraceExportDecision Decision;
		const std::wstring Reason;
		const bool IsAllowed;

		DebugTraceExportResult(DebugTraceExportDecision decision, std::wstring reason)
			: Decision(decision), Reason(std::move(reason)), IsAllowed(decision == DebugTraceExportDecision::Allowed) {}

		static inline const std::shared_ptr<DebugTraceExportResult> Allowed =
			std::make_shared<DebugTraceExportResult>(DebugTraceExportDecision::Allowed, L"");

		static std::shared_ptr<DebugTraceExportResult> Denied(DebugTraceExportDecision decision, const std::wstring &reason)
		{
			return std::make_shared<DebugTraceExportResult>(decision, reason);
		}
	};
}
