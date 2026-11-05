#pragma once

#include "../../Descriptors/Memory/YAKSys_Hybrid_CPU.Core.SecureMemoryDomainDescriptor.h"
#include "../../Descriptors/Io/YAKSys_Hybrid_CPU.Core.SecureIoDomainDescriptor.h"
#include "SecureMemoryAdmissionPolicy.h"
#include <string>
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{
	class SecureMemoryAdmissionPolicy final : public std::enable_shared_from_this<SecureMemoryAdmissionPolicy>
	{
	public:
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public SecureMemoryAdmissionResult Admit(SecureMemoryDomainDescriptor? descriptor, SecureMemoryAccessRequest request)
		std::shared_ptr<SecureMemoryAdmissionResult> Admit(std::shared_ptr<SecureMemoryDomainDescriptor> descriptor, std::shared_ptr<SecureMemoryAccessRequest> request);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public SecureMemoryAdmissionResult Admit(SecureMemoryDomainDescriptor? descriptor, SecureMemoryAccessRequest request, System.Nullable<SecureIoDomainDescriptor> secureIoPolicy)
		std::shared_ptr<SecureMemoryAdmissionResult> Admit(std::shared_ptr<SecureMemoryDomainDescriptor> descriptor, std::shared_ptr<SecureMemoryAccessRequest> request, std::shared_ptr<SecureIoDomainDescriptor> secureIoPolicy);

	private:
		static std::shared_ptr<SecureMemoryAdmissionResult> AdmitHostRead(std::shared_ptr<SecureMemoryRegionDescriptor> region);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private static SecureMemoryAdmissionResult AdmitDma(SecureMemoryDomainDescriptor descriptor, SecureIoDomainDescriptor? secureIoPolicy, SecureMemoryRegionDescriptor region, SecureMemoryAccessRequest request)
		static std::shared_ptr<SecureMemoryAdmissionResult> AdmitDma(std::shared_ptr<SecureMemoryDomainDescriptor> descriptor, std::shared_ptr<SecureIoDomainDescriptor> secureIoPolicy, std::shared_ptr<SecureMemoryRegionDescriptor> region, std::shared_ptr<SecureMemoryAccessRequest> request);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private static bool TryFindSharedBuffer(SecureMemoryDomainDescriptor descriptor, SecureIoDomainDescriptor? secureIoPolicy, SecureMemoryAccessRequest request, out SecureSharedBufferDescriptor sharedBuffer)
		static bool TryFindSharedBuffer(std::shared_ptr<SecureMemoryDomainDescriptor> descriptor, std::shared_ptr<SecureIoDomainDescriptor> secureIoPolicy, std::shared_ptr<SecureMemoryAccessRequest> request, std::shared_ptr<SecureSharedBufferDescriptor> &sharedBuffer);

		static std::shared_ptr<SecureMemoryAdmissionResult> AdmitMeasurement(std::shared_ptr<SecureMemoryRegionDescriptor> region);

		static std::shared_ptr<SecureMemoryAdmissionResult> AdmitRuntimeTouch(std::shared_ptr<SecureMemoryRegionDescriptor> region);

		static std::shared_ptr<SecureMemoryAdmissionResult> Deny(SecureMemoryAdmissionDecision decision, const std::wstring &reason);
	};
}
