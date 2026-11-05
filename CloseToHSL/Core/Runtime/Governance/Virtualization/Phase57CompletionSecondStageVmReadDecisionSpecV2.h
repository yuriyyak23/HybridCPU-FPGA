#pragma once

#include "VirtualizationDecisionCanonicalEncoderV2.h"
#include <string>
#include <format>
#include "tangible_exception_helper.h"
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{

	class Phase57CompletionSecondStageVmReadDecisionSpecV2 final
	{
	private:
		static std::shared_ptr<VirtualizationDecisionSpecV2> GetInstance();
		static std::vector<unsigned char> GetCanonicalBytes();

	public:
		static inline constexpr wchar_t ExpectedSpecDigest[] = L"c68adf194f0452c15f4d11e3a44199b50a0a6168923915e280c62fcbe683c4ab";
		static std::shared_ptr<VirtualizationDecisionSpecV2> getInstance();
		static std::vector<unsigned char> getCanonicalBytes();
		static bool getRuntimeAuthorityGranted();
		static bool getProductionCompositionAuthorized();

	private:
		static std::shared_ptr<VirtualizationDecisionSpecV2> Create();

		static std::vector<std::shared_ptr<VirtualizationDecisionOwnerMapEntryV2>> CreateOwnerMap();

		static std::shared_ptr<VirtualizationDecisionOwnerMapEntryV2> Entry(const std::wstring &field, const std::wstring &source, const std::wstring &evidence);
	};

}
