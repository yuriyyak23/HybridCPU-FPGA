#pragma once

#include "../../../Virtualization/Compatibility/FrozenAbi/VmcsFieldAliases/VmcsField.h"
#include "VmcsV2FieldModel.h"
#include <string>
#include <vector>
#include <memory>

using namespace YAKSys_Hybrid_CPU::Core;

namespace YAKSys_Hybrid_CPU::Core::Vmcs::V2
{

	class VmcsV2BlockDirectory final : public std::enable_shared_from_this<VmcsV2BlockDirectory>
	{
	public:
		static constexpr unsigned short GuestIntegerRegisterFileFieldId = 0x1000;
		static constexpr unsigned short VmxRootControlBlockFieldId = 0x1100;
		static constexpr unsigned short VirtualCpuBlockFieldId = 0x1101;
		static constexpr unsigned short BundleExecutionBlockFieldId = 0x1102;
		static constexpr unsigned short VmxPreemptionTimerBlockFieldId = 0x1103;
		static constexpr unsigned short VirtualInterruptFabricBlockFieldId = 0x1104;
		static constexpr unsigned short VectorStreamStateBlockFieldId = 0x1105;
		static constexpr unsigned short SecurityIsolationBlockFieldId = 0x1106;
		static constexpr unsigned short CapabilityNegotiationBlockFieldId = 0x1107;
		static constexpr unsigned short NptBlockFieldId = 0x1108;
		static constexpr unsigned short ExitInfoBlockFieldId = 0x1109;
		static constexpr unsigned short InterceptBitmapBlockFieldId = 0x110A;
		static constexpr unsigned short EventInjectionBlockFieldId = 0x110B;
		static constexpr unsigned short LaneCompletionRoutingBlockFieldId = 0x110C;
		static constexpr unsigned short IoVirtBlockFieldId = 0x110D;
		static constexpr unsigned short Lane6StateBlockFieldId = 0x110E;
		static constexpr unsigned short Lane7StateBlockFieldId = 0x110F;
		static constexpr unsigned short ShadowVmcsBlockFieldId = 0x1110;
		static constexpr unsigned short DirtyLogBlockFieldId = 0x1111;
		static constexpr unsigned short DebugTraceBlockFieldId = 0x1112;

	private:
		const std::vector<std::shared_ptr<VmcsV2FieldDescriptor>> _fields;

	public:
// C# TO C++ CONVERTER TASK: C++ has no equivalent to C# range operator:
		VmcsV2BlockDirectory(const std::vector<std::shared_ptr<VmcsV2FieldDescriptor>> &fields);

		const std::vector<std::shared_ptr<VmcsV2FieldDescriptor>> getFields() const;

		static std::shared_ptr<VmcsV2BlockDirectory> CreateDefault();

		bool TryGetField(unsigned short fieldId, std::shared_ptr<VmcsV2FieldDescriptor> &descriptor);

		bool TryGetLegacyAlias(VmcsField field, std::shared_ptr<VmcsV2FieldDescriptor> &descriptor);

	private:
		static void AddLegacyVmcsAliases(std::vector<std::shared_ptr<VmcsV2FieldDescriptor>> &fields);

		static void AddDescriptorFoundationFields(std::vector<std::shared_ptr<VmcsV2FieldDescriptor>> &fields);

		static void AddBlockDescriptor(std::vector<std::shared_ptr<VmcsV2FieldDescriptor>> &fields, unsigned short fieldId, const std::wstring &name, VmcsV2BlockKind block);

		static VmcsV2BlockKind ResolveBlock(VmcsField field);

		static std::shared_ptr<VmcsV2AccessPolicy> ResolveAccessPolicy(VmcsField field);

		static std::shared_ptr<VmcsV2MigrationPolicy> ResolveMigrationPolicy(VmcsField field);
	};

}
