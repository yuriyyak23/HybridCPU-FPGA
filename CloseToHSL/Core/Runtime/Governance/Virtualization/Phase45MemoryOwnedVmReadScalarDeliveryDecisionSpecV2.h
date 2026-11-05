#pragma once

#include "VirtualizationDecisionCanonicalEncoderV2.h"
#include <string>
#include <format>
#include "tangible_exception_helper.h"
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{

	class Phase45MemoryOwnedVmReadScalarDeliveryDecisionSpecV2 final
	{
	private:
		static std::shared_ptr<VirtualizationDecisionSpecV2> GetInstance();
		static std::vector<unsigned char> GetCanonicalBytes();

	public:
		static inline constexpr wchar_t ExpectedSpecDigest[] = L"7cc2ad6bca9cc808aa6d42767dba5c7eaefed1a34180cb6caf3b34384662df21";
		static std::shared_ptr<VirtualizationDecisionSpecV2> getInstance();
		static std::vector<unsigned char> getCanonicalBytes();
		static bool getRuntimeAuthorityGranted();
		static bool getSourceValueAvailable();
		static bool getResultReceiptIssued();
		static bool getProductionCompositionAuthorized();

	private:
		static std::shared_ptr<VirtualizationDecisionSpecV2> Create();

		static std::vector<std::shared_ptr<VirtualizationDecisionOwnerMapEntryV2>> CreateOwnerMap();

		static std::shared_ptr<VirtualizationDecisionOwnerMapEntryV2> Entry(const std::wstring &field, const std::wstring &source, const std::wstring &evidence);
	};

}
