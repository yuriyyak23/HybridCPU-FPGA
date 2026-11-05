#pragma once
#include "ExecutionDomainRuntime.h"

#include "../../Descriptors/ExecutionDomain/YAKSys_Hybrid_CPU.Core.ExecutionDomainDescriptor.h"
#include "ExecutionDomainRuntime.StateEpoch.h"
#include "../../../../Virtualization/Compatibility/FrozenAbi/VmcsFieldAliases/VmcsField.h"
#include <string>
#include <mutex>
#include <memory>
#include <utility>

namespace YAKSys_Hybrid_CPU::Core
{
	class ExecutionDomainRuntime final : public std::enable_shared_from_this<ExecutionDomainRuntime>
	{
	public:
		std::shared_ptr<ExecutionDomainRuntimeResult> Validate(std::shared_ptr<ExecutionDomainRuntimeRequest> request);

		bool CanAdmit(std::shared_ptr<ExecutionDomainRuntimeRequest> request);


		/// <summary>
		/// Opaque immutable capture issued only while the existing execution-domain runtime
		/// owns the matching descriptor, epoch, domain and address-space binding. It is a
		/// freshness proof for one source read, not a capability or continuing source lease.
		/// </summary>
	public:
		class SourceCapture final : public std::enable_shared_from_this<SourceCapture>
		{
		private:
			const std::shared_ptr<ExecutionDomainDescriptor> SourceOwner;
			const ExecutionDomainStateEpoch SourceEpoch;
			const unsigned long long BindingGeneration;
			const unsigned long long DomainTag;
			const unsigned long long AddressSpaceTag;
			const VmcsField Field;
			const unsigned long long Value;

			const std::shared_ptr<const unsigned char> _issuerSeal;

		public:
			SourceCapture(std::shared_ptr<const unsigned char> issuerSeal, std::shared_ptr<ExecutionDomainDescriptor> sourceOwner, ExecutionDomainStateEpoch sourceEpoch, unsigned long long bindingGeneration, unsigned long long domainTag, unsigned long long addressSpaceTag, VmcsField field, unsigned long long value);

			std::shared_ptr<ExecutionDomainDescriptor> getSourceOwner() const;
			ExecutionDomainStateEpoch getSourceEpoch() const;
			unsigned long long getBindingGeneration() const;
			unsigned long long getDomainTag() const;
			unsigned long long getAddressSpaceTag() const;
			VmcsField getField() const;
			unsigned long long getValue() const;
			bool isMaterialized() const;
			bool getRuntimeAuthorityGranted() const;
			bool isCapability() const;
			bool isReceipt() const;

			bool WasIssuedBy(const std::shared_ptr<const unsigned char> &issuerSeal) const;
		};

	private:
		mutable std::mutex _sourceStateSync;
		const std::shared_ptr<const unsigned char> _sourceCaptureIssuerSeal = std::make_shared<const unsigned char>(0);
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private ExecutionDomainDescriptor? _currentSourceDescriptor;
		std::shared_ptr<ExecutionDomainDescriptor> _currentSourceDescriptor;
		ExecutionDomainStateEpoch _currentSourceEpoch;
		unsigned long long _sourceEpochCounter = 0;
		unsigned long long _sourceBindingGeneration = 0;
		unsigned long long _currentSourceAddressSpaceTag = 0;

	public:
		ExecutionDomainStateEpoch getCurrentSourceEpoch() const;

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal ExecutionDomainDescriptor? CurrentSourceDescriptor
		std::shared_ptr<ExecutionDomainDescriptor> getCurrentSourceDescriptor() const;

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal ExecutionDomainSourceBindResult BindAuthoritativeReadOnlyState(ExecutionDomainDescriptor? descriptor, ulong addressSpaceTag)
		std::shared_ptr<ExecutionDomainSourceBindResult> BindAuthoritativeReadOnlyState(std::shared_ptr<ExecutionDomainDescriptor> descriptor, unsigned long long addressSpaceTag);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal ExecutionDomainSourceBindResult ReplaceAuthoritativeReadOnlyState(ExecutionDomainDescriptor? descriptor, ulong addressSpaceTag)
		std::shared_ptr<ExecutionDomainSourceBindResult> ReplaceAuthoritativeReadOnlyState(std::shared_ptr<ExecutionDomainDescriptor> descriptor, unsigned long long addressSpaceTag);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal ExecutionDomainSourceBindResult RebindAuthoritativeReadOnlyStateAfterRestore(ExecutionDomainDescriptor? descriptor, ulong addressSpaceTag)
		std::shared_ptr<ExecutionDomainSourceBindResult> RebindAuthoritativeReadOnlyStateAfterRestore(std::shared_ptr<ExecutionDomainDescriptor> descriptor, unsigned long long addressSpaceTag);

		void UnbindAuthoritativeReadOnlyState();

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal ExecutionDomainSourceCaptureResult CaptureVmReadScalarSource(ExecutionDomainDescriptor? presentedDescriptor, ulong runtimeDomainTag, ulong runtimeAddressSpaceTag, VmcsField field)
		std::shared_ptr<ExecutionDomainSourceCaptureResult> CaptureVmReadScalarSource(std::shared_ptr<ExecutionDomainDescriptor> presentedDescriptor, unsigned long long runtimeDomainTag, unsigned long long runtimeAddressSpaceTag, VmcsField field);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal bool IsAuthenticCapture(SourceCapture? capture)
		bool IsAuthenticCapture(std::shared_ptr<SourceCapture> capture);

	private:
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private ExecutionDomainSourceBindResult BindSource(ExecutionDomainDescriptor? descriptor, ulong addressSpaceTag, string action)
		std::shared_ptr<ExecutionDomainSourceBindResult> BindSource(std::shared_ptr<ExecutionDomainDescriptor> descriptor, unsigned long long addressSpaceTag, const std::wstring &action);

		ExecutionDomainStateEpoch AdvanceSourceEpoch();

		unsigned long long AdvanceBindingGeneration();

		inline static bool TryReadExactField(std::shared_ptr<ExecutionDomainReadOnlyStateView> view, VmcsField field, unsigned long long &value, bool &knownField);

		static std::shared_ptr<ExecutionDomainSourceBindResult> DenyBind(ExecutionDomainSourceBindDecision decision, const std::wstring &reason);

		static std::shared_ptr<ExecutionDomainSourceCaptureResult> DenyCapture(ExecutionDomainSourceCaptureDecision decision, const std::wstring &reason);
	};

	struct ExecutionDomainSourceCaptureResult final {
		ExecutionDomainSourceCaptureDecision Decision;
		std::shared_ptr<ExecutionDomainRuntime::SourceCapture> Capture;
		std::wstring Reason;
		ExecutionDomainSourceCaptureResult(ExecutionDomainSourceCaptureDecision decision, std::shared_ptr<ExecutionDomainRuntime::SourceCapture> capture, std::wstring reason)
			: Decision(decision), Capture(std::move(capture)), Reason(std::move(reason)) {}
		bool isCaptured() const noexcept { return Decision == ExecutionDomainSourceCaptureDecision::Captured && Capture != nullptr; }
	};
}
