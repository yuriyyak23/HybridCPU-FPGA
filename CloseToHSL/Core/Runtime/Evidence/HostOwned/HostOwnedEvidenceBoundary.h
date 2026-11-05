#pragma once
#include <memory>
#include <string>
#include <utility>

namespace YAKSys_Hybrid_CPU::Core
{

	enum class HostOwnedEvidenceBoundaryDecision : unsigned char
	{
		Allowed = 0,
		GuestProjectionDenied = 1,
		HostCaptureDenied = 2,
		RestoreRequiresRecompute = 3,
		RestoreDenied = 4
	};

struct HostOwnedEvidenceBoundaryResult final {
 HostOwnedEvidenceBoundaryDecision Decision;
 std::wstring Reason;
 bool IsAllowed() const noexcept { return Decision == HostOwnedEvidenceBoundaryDecision::Allowed; }
 static std::shared_ptr<HostOwnedEvidenceBoundaryResult> Denied(HostOwnedEvidenceBoundaryDecision decision, std::wstring reason) { return std::make_shared<HostOwnedEvidenceBoundaryResult>(decision, std::move(reason)); }
 static const std::shared_ptr<HostOwnedEvidenceBoundaryResult> Allowed;
};
inline const std::shared_ptr<HostOwnedEvidenceBoundaryResult> HostOwnedEvidenceBoundaryResult::Allowed = std::make_shared<HostOwnedEvidenceBoundaryResult>(HostOwnedEvidenceBoundaryDecision::Allowed, L"");
// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct HostOwnedEvidenceBoundaryResult(HostOwnedEvidenceBoundaryDecision Decision, string Reason)
//	{
//		public bool IsAllowed
//		{
//			get
//			{
//				return Decision == HostOwnedEvidenceBoundaryDecision.Allowed;
//			}
//		}
//
//		public static HostOwnedEvidenceBoundaryResult Allowed
//		{
//			get;
//		} = new(HostOwnedEvidenceBoundaryDecision.Allowed, string.Empty);
//
//		public static HostOwnedEvidenceBoundaryResult Denied(HostOwnedEvidenceBoundaryDecision decision, string reason)
//		{
//			return new(decision, reason);
//		}
//	}


}
