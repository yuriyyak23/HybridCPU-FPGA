#pragma once
#include "MemoryDomainRuntime.h"
#include "MemoryDomainRuntime.CpuSecondStage.h"

#include "../../Descriptors/MemoryDomain/YAKSys_Hybrid_CPU.Core.MemoryDomainDescriptor.h"
#include "../../../../Virtualization/Compatibility/FrozenAbi/VmcsFieldAliases/VmcsField.h"
#include "MemoryDomainRuntime.AddressSpaceGeneration.h"
#include <string>
#include <vector>
#include <any>
#include <functional>
#include <mutex>
#include <format>
#include "tangible_exception_helper.h"
#include <memory>

using namespace YAKSys_Hybrid_CPU::Memory;

namespace YAKSys_Hybrid_CPU::Core
{
	struct CpuSecondStageSourceBindResult;
    struct CpuSecondStageTranslationSnapshot;
    class MemoryDomainRuntime final : public std::enable_shared_from_this<MemoryDomainRuntime>
	{
	public:
		class SourceCapture final : public std::enable_shared_from_this<SourceCapture>
		{
		private:
			const std::shared_ptr<MemoryDomainDescriptor> SourceOwner;
			const unsigned long long AddressSpaceGeneration;
			const unsigned long long BindingGeneration;
			const unsigned long long DomainTag;
			const unsigned long long RuntimeAddressSpaceTag;
			const VmcsField Field;
			const unsigned long long Value;

			const std::any _issuerSeal;

		public:
			SourceCapture(std::any issuerSeal, std::shared_ptr<MemoryDomainDescriptor> sourceOwner, unsigned long long addressSpaceGeneration, unsigned long long bindingGeneration, unsigned long long domainTag, unsigned long long runtimeAddressSpaceTag, VmcsField field, unsigned long long value);

			std::shared_ptr<MemoryDomainDescriptor> getSourceOwner() const;
			unsigned long long getAddressSpaceGeneration() const;
			unsigned long long getBindingGeneration() const;
			unsigned long long getDomainTag() const;
			unsigned long long getRuntimeAddressSpaceTag() const;
			VmcsField getField() const;
			unsigned long long getValue() const;
			bool getRuntimeAuthorityGranted() const;
			bool isReceipt() const;
			bool WasIssuedBy(std::any seal);
		};

	private:
		mutable std::mutex _translationSourceSync;
		const std::any _translationCaptureIssuerSeal = std::make_shared<const unsigned char>(0);
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private MemoryDomainDescriptor? _currentTranslationSource;
		std::shared_ptr<MemoryDomainDescriptor> _currentTranslationSource;
		unsigned long long _currentAddressSpaceGeneration = 0;
		unsigned long long _addressSpaceGenerationCounter = 0;
		unsigned long long _translationBindingGeneration = 0;
		unsigned long long _currentRuntimeAddressSpaceTag = 0;

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal MemoryDomainDescriptor? CurrentTranslationSource
	public:
		std::shared_ptr<MemoryDomainDescriptor> getCurrentTranslationSource() const;

		unsigned long long getCurrentAddressSpaceGeneration() const;

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal MemoryDomainSourceBindResult BindAuthoritativeTranslationView(MemoryDomainDescriptor? descriptor, ulong runtimeAddressSpaceTag)
		std::shared_ptr<MemoryDomainSourceBindResult> BindAuthoritativeTranslationView(std::shared_ptr<MemoryDomainDescriptor> descriptor, unsigned long long runtimeAddressSpaceTag);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal MemoryDomainSourceBindResult ReplaceAuthoritativeTranslationView(MemoryDomainDescriptor? descriptor, ulong runtimeAddressSpaceTag)
		std::shared_ptr<MemoryDomainSourceBindResult> ReplaceAuthoritativeTranslationView(std::shared_ptr<MemoryDomainDescriptor> descriptor, unsigned long long runtimeAddressSpaceTag);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal MemoryDomainSourceBindResult RebindAuthoritativeTranslationViewAfterRestore(MemoryDomainDescriptor? descriptor, ulong runtimeAddressSpaceTag)
		std::shared_ptr<MemoryDomainSourceBindResult> RebindAuthoritativeTranslationViewAfterRestore(std::shared_ptr<MemoryDomainDescriptor> descriptor, unsigned long long runtimeAddressSpaceTag);

		void UnbindAuthoritativeTranslationView();

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal MemoryDomainSourceCaptureResult CaptureVmReadScalarSource(MemoryDomainDescriptor? presentedDescriptor, ulong runtimeDomainTag, ulong runtimeAddressSpaceTag, VmcsField field)
		std::shared_ptr<MemoryDomainSourceCaptureResult> CaptureVmReadScalarSource(std::shared_ptr<MemoryDomainDescriptor> presentedDescriptor, unsigned long long runtimeDomainTag, unsigned long long runtimeAddressSpaceTag, VmcsField field);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal bool IsAuthenticCapture(SourceCapture? capture)
		bool IsAuthenticCapture(std::shared_ptr<SourceCapture> capture);

	private:
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private MemoryDomainSourceBindResult BindSource(MemoryDomainDescriptor? descriptor, ulong runtimeAddressSpaceTag, string action)
		std::shared_ptr<MemoryDomainSourceBindResult> BindSource(std::shared_ptr<MemoryDomainDescriptor> descriptor, unsigned long long runtimeAddressSpaceTag, const std::wstring &action);

		unsigned long long AdvanceAddressSpaceGeneration();

		unsigned long long AdvanceBindingGeneration();

		static std::shared_ptr<MemoryDomainSourceBindResult> DenyBind(MemoryDomainSourceBindDecision decision, const std::wstring &reason);

		static std::shared_ptr<MemoryDomainSourceCaptureResult> DenyCapture(MemoryDomainSourceCaptureDecision decision, const std::wstring &reason);


	public:
		class CpuSecondStageTranslationBinding final : public std::enable_shared_from_this<CpuSecondStageTranslationBinding>
		{
		private:
			const std::shared_ptr<MemoryDomainRuntime> Issuer;
			const std::shared_ptr<MemoryDomainDescriptor> SourceOwner;
			const unsigned long long RuntimeAddressSpaceIdentity;
			const unsigned long long AddressSpaceGeneration;
			const unsigned long long BindingGeneration;

			const std::any _issuerSeal;

		public:
			CpuSecondStageTranslationBinding(std::any issuerSeal, std::shared_ptr<MemoryDomainRuntime> issuer, std::shared_ptr<MemoryDomainDescriptor> sourceOwner, unsigned long long runtimeAddressSpaceIdentity, unsigned long long addressSpaceGeneration, unsigned long long bindingGeneration);

			std::shared_ptr<MemoryDomainRuntime> getIssuer() const;
			std::shared_ptr<MemoryDomainDescriptor> getSourceOwner() const;
			unsigned long long getRuntimeAddressSpaceIdentity() const;
			unsigned long long getAddressSpaceGeneration() const;
			unsigned long long getBindingGeneration() const;
			bool WasIssuedBy(std::any seal);
		};

	private:
		const std::any _cpuSecondStageIssuerSeal = std::make_shared<const unsigned char>(0);
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly List<System.Action> _cpuSecondStageLifecycleInvalidations = new();
		std::vector<std::function<void()>> _cpuSecondStageLifecycleInvalidations = std::vector<std::function<void()>>();

	public:
		void RegisterCpuSecondStageCompletionInvalidation(std::shared_ptr<CpuSecondStageTranslationBinding> binding, std::function<void()> invalidation);

	private:
		void InvalidateCpuSecondStageLifecycleBeforeSourceChange();

	public:
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal CpuSecondStageSourceBindResult BindCanonicalCpuSecondStageTranslation(MemoryDomainDescriptor? descriptor, ulong runtimeAddressSpaceIdentity, ulong expectedDomainIdentity)
		std::shared_ptr<CpuSecondStageSourceBindResult> BindCanonicalCpuSecondStageTranslation(std::shared_ptr<MemoryDomainDescriptor> descriptor, unsigned long long runtimeAddressSpaceIdentity, unsigned long long expectedDomainIdentity);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal bool TryCaptureCanonicalCpuSecondStageTranslation(CpuSecondStageTranslationBinding? binding, ulong expectedDomainIdentity, out CpuSecondStageTranslationSnapshot snapshot)
		bool TryCaptureCanonicalCpuSecondStageTranslation(std::shared_ptr<CpuSecondStageTranslationBinding> binding, unsigned long long expectedDomainIdentity, std::shared_ptr<CpuSecondStageTranslationSnapshot> &snapshot);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal bool IsCurrentCanonicalCpuSecondStageTranslation(CpuSecondStageTranslationBinding? binding, in CpuSecondStageTranslationSnapshot snapshot)
		bool IsCurrentCanonicalCpuSecondStageTranslation(std::shared_ptr<CpuSecondStageTranslationBinding> binding, std::shared_ptr<CpuSecondStageTranslationSnapshot> snapshot);


		std::shared_ptr<MemoryDomainRuntimeResult> Validate(std::shared_ptr<MemoryDomainRuntimeRequest> request);

		bool CanAdmit(std::shared_ptr<MemoryDomainRuntimeRequest> request);
	};
}

namespace YAKSys_Hybrid_CPU::Core {
struct MemoryDomainSourceCaptureResult final {
    MemoryDomainSourceCaptureDecision Decision;
    std::shared_ptr<MemoryDomainRuntime::SourceCapture> Capture;
    std::wstring Reason;
    bool isCaptured() const noexcept { return Decision == MemoryDomainSourceCaptureDecision::Captured && Capture != nullptr; }
};
}
namespace YAKSys_Hybrid_CPU::Core {
struct CpuSecondStageSourceBindResult final {
    CpuSecondStageSourceBindDecision Decision;
    std::shared_ptr<MemoryDomainRuntime::CpuSecondStageTranslationBinding> Binding;
    std::wstring Reason;
    bool isBound() const noexcept { return Decision == CpuSecondStageSourceBindDecision::Bound && Binding != nullptr; }
};
struct CpuSecondStageTranslationSnapshot final {
    std::shared_ptr<MemoryDomainDescriptor> SourceOwner;
    unsigned long long SecondStageRoot = 0, DomainIdentity = 0, AddressSpaceIdentity = 0, MemoryDomainOwnerEpoch = 0, AddressSpaceGeneration = 0, BindingGeneration = 0;
    std::shared_ptr<AddressSpaceDescriptor> AddressSpace;
    bool isValid() const { return SecondStageRoot != 0 && DomainIdentity != 0 && AddressSpaceIdentity != 0 && MemoryDomainOwnerEpoch != 0 && AddressSpaceGeneration != 0 && BindingGeneration != 0 && AddressSpace && AddressSpace->isRuntimeAuthoritative(); }
};
}
