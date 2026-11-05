#pragma once

#include <string>
#include <vector>
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{
	class EventInjectionDescriptor;
	struct TrapRequest;

	class EventTrapDomainIdentityAuthorityRemovalContract final : public std::enable_shared_from_this<EventTrapDomainIdentityAuthorityRemovalContract>
	{
	public:
		static inline const std::wstring EventDescriptorPath = L"Core/Runtime/Events/Injection/EventInjectionDescriptor.cs";

		static inline const std::wstring InterruptRemapPath = L"Core/Runtime/Events/Injection/InterruptRemapPolicy.cs";

		static inline const std::wstring PostedEventQueuePath = L"Core/Runtime/Events/Injection/PostedEventQueue.cs";

		static inline const std::wstring VirtualInterruptFabricPath = L"Core/Runtime/Events/Injection/VirtualInterruptFabric.cs";

		static inline const std::wstring TrapRequestPath = L"Core/Runtime/Events/Traps/TrapRequest.cs";

		static inline const std::wstring VirtualTimerPath = L"Core/Runtime/Events/Traps/VirtualTimerState.cs";

		static inline const std::wstring VirtualTimerExpirationPath = L"Core/Runtime/Events/Traps/VirtualTimerState.Expiration.partial.cs";

		static inline const std::wstring CompatibilityEventProjectionPath = L"Core/VMX/Compatibility/Generated/VmcsProjection/VmcsV2Blocks.cs";

	private:
		static inline const std::vector<std::wstring> ExecutableEventTrapPaths = {EventDescriptorPath, InterruptRemapPath, PostedEventQueuePath, VirtualInterruptFabricPath, TrapRequestPath, VirtualTimerPath, VirtualTimerExpirationPath};
		static inline const std::vector<std::wstring> ForbiddenExecutableIdentityMarkers = {L"Vmid", L"Vpid", L"vmid", L"vpid", L"VMID", L"VPID"};
		static inline const std::vector<std::wstring> ForbiddenCompatibilityEventHelperMarkers = {L"ushort vmid", L"ushort vpid", L"Vmid", L"Vpid", L"TryQueue(", L"TryDeliver(", L"ConfigureInterruptRemap", L"RemoveInterruptRemap", L"ClearInterruptRemaps", L"RestoreSnapshot"};

	public:
		static std::vector<std::wstring> getExecutableEventTrapPaths();

		static std::vector<std::wstring> getForbiddenExecutableIdentityMarkers();

		static std::vector<std::wstring> getForbiddenCompatibilityEventHelperMarkers();

		bool IsNeutralIdentity(std::shared_ptr<EventInjectionDescriptor> descriptor, std::shared_ptr<TrapRequest> request, unsigned short executionDomainTag, unsigned short addressSpaceTag);
	};
}
