#pragma once

#include <string>
#include <memory>

namespace YAKSys_Hybrid_CPU::Core::Vmcs::V2
{

	class VmcsV2Header final : public std::enable_shared_from_this<VmcsV2Header>
	{
	private:
		const unsigned int RevisionId;
		const unsigned int SizeBytes;
		const std::wstring Baseline;

	public:
		static constexpr unsigned int CurrentRevisionId = 2;
		static constexpr unsigned int CurrentSizeBytes = 256;
		static inline const std::wstring CompatibilityBaseline = L"VMX8";

		VmcsV2Header(unsigned int revisionId = CurrentRevisionId, unsigned int sizeBytes = CurrentSizeBytes, const std::wstring &compatibilityBaseline = CompatibilityBaseline);

		unsigned int getRevisionId() const;

		unsigned int getSizeBytes() const;

		std::wstring getBaseline() const;

		bool isReadOnlyCompatibilityProjection() const;

		bool isLaunched() const;

		unsigned long long getInvalidationEpoch() const;
	};

}
