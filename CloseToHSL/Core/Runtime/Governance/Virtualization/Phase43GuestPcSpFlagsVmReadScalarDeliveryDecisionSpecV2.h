#pragma once

#include "VirtualizationDecisionCanonicalEncoderV2.h"
#include <string>
#include <format>
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{

	class Phase43GuestPcSpFlagsVmReadScalarDeliveryDecisionSpecV2 final
	{
	private:
		static std::shared_ptr<VirtualizationDecisionSpecV2> Create();
		static std::shared_ptr<VirtualizationDecisionSpecV2> GetInstance();
		static std::vector<unsigned char> GetCanonicalBytes();

	public:
		static inline const std::wstring ExpectedSpecDigest = L"e67ff2620ff6a1fd193b8303c5b6ae1d532e51241e6b3e405f3c6cedefe2d754";

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
		static bool getProductionCompositionAuthorized();

	private:
		static std::vector<std::shared_ptr<VirtualizationDecisionOwnerMapEntryV2>> CreateOwnerMap();

		static std::shared_ptr<VirtualizationDecisionOwnerMapEntryV2> Entry(const std::wstring &field, const std::wstring &source);
	};

}
