#pragma once

#include "VirtualizationDecisionCanonicalEncoderV2.h"
#include <string>
#include "tangible_exception_helper.h"
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{

	/// <summary>
	/// Immutable governance specification for exact GuestCr0/GuestCr4 scalar
	/// delivery. It does not issue a result receipt or authorize runtime writeback.
	/// </summary>
	class Phase41VmReadScalarDeliveryDecisionSpecV2 final
	{
	private:
		static std::shared_ptr<VirtualizationDecisionSpecV2> Create();
		static std::shared_ptr<VirtualizationDecisionSpecV2> GetInstance();
		static std::vector<unsigned char> GetCanonicalBytes();

	public:
		static inline const std::wstring ExpectedSpecDigest = L"ccda8698dbeb3f6eef1b4f13e22a3fb7607e939f493138fb7e3373674e234309";

		static std::shared_ptr<VirtualizationDecisionSpecV2> getInstance();

		static std::vector<unsigned char> getCanonicalBytes();

		static bool getRuntimeAuthorityGranted();
		static bool getSourceValueAvailable();
		static bool getResultReceiptIssued();
		static bool getRegisterWritebackAuthorized();
		static bool getRetireCommitAuthorized();
		static bool getBackendExecutionAuthorized();
		static bool getUnderlyingVirtualizationMutationAuthorized();
		static bool getCompletionPublicationAuthorized();

	private:
		static std::vector<std::shared_ptr<VirtualizationDecisionOwnerMapEntryV2>> CreateOwnerMap();

		static std::shared_ptr<VirtualizationDecisionOwnerMapEntryV2> Entry(const std::wstring &field, const std::wstring &valueSource);
	};

}
