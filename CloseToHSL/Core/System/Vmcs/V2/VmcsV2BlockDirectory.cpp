#include "VmcsV2BlockDirectory.h"
#include <array>

using namespace YAKSys_Hybrid_CPU::Core;

namespace YAKSys_Hybrid_CPU::Core::Vmcs::V2
{
	namespace
	{
		constexpr std::array VmcsFieldValues{
			VmcsField::GuestPc, VmcsField::GuestSp, VmcsField::GuestFlags, VmcsField::GuestCr0,
			VmcsField::GuestCr3, VmcsField::GuestCr4, VmcsField::HostPc, VmcsField::HostSp,
			VmcsField::HostFlags, VmcsField::HostCr0, VmcsField::HostCr3, VmcsField::PinBasedControls,
			VmcsField::ProcBasedControls, VmcsField::ExitControls, VmcsField::EntryControls,
			VmcsField::EptPointer, VmcsField::Vpid, VmcsField::SecondaryProcControls,
			VmcsField::Cr3TargetCount, VmcsField::ExitReason, VmcsField::ExitQualification,
			VmcsField::GuestPhysicalAddress, VmcsField::EptViolationQualification};

		bool IsDefinedVmcsField(VmcsField field)
		{
			for (const auto candidate : VmcsFieldValues)
				if (candidate == field) return true;
			return false;
		}

		std::wstring VmcsFieldName(VmcsField field)
		{
			switch (field)
			{
			case VmcsField::GuestPc: return L"GuestPc";
			case VmcsField::GuestSp: return L"GuestSp";
			case VmcsField::GuestFlags: return L"GuestFlags";
			case VmcsField::GuestCr0: return L"GuestCr0";
			case VmcsField::GuestCr3: return L"GuestCr3";
			case VmcsField::GuestCr4: return L"GuestCr4";
			case VmcsField::HostPc: return L"HostPc";
			case VmcsField::HostSp: return L"HostSp";
			case VmcsField::HostFlags: return L"HostFlags";
			case VmcsField::HostCr0: return L"HostCr0";
			case VmcsField::HostCr3: return L"HostCr3";
			case VmcsField::PinBasedControls: return L"PinBasedControls";
			case VmcsField::ProcBasedControls: return L"ProcBasedControls";
			case VmcsField::ExitControls: return L"ExitControls";
			case VmcsField::EntryControls: return L"EntryControls";
			case VmcsField::EptPointer: return L"EptPointer";
			case VmcsField::Vpid: return L"Vpid";
			case VmcsField::SecondaryProcControls: return L"SecondaryProcControls";
			case VmcsField::Cr3TargetCount: return L"Cr3TargetCount";
			case VmcsField::ExitReason: return L"ExitReason";
			case VmcsField::ExitQualification: return L"ExitQualification";
			case VmcsField::GuestPhysicalAddress: return L"GuestPhysicalAddress";
			case VmcsField::EptViolationQualification: return L"EptViolationQualification";
			default: return L"";
			}
		}
	}

	VmcsV2BlockDirectory::VmcsV2BlockDirectory(const std::vector<std::shared_ptr<VmcsV2FieldDescriptor>> &fields) : _fields(fields)
	{
	}

	const std::vector<std::shared_ptr<VmcsV2FieldDescriptor>> VmcsV2BlockDirectory::getFields() const
	{
		return _fields;
	}

	std::shared_ptr<VmcsV2BlockDirectory> VmcsV2BlockDirectory::CreateDefault()
	{
		std::vector<std::shared_ptr<VmcsV2FieldDescriptor>> fields;
		AddLegacyVmcsAliases(fields);
		AddDescriptorFoundationFields(fields);
		return std::make_shared<VmcsV2BlockDirectory>(fields);
	}

	bool VmcsV2BlockDirectory::TryGetField(unsigned short fieldId, std::shared_ptr<VmcsV2FieldDescriptor> &descriptor)
	{
		for (auto candidate : _fields)
		{
			if (candidate->FieldId == fieldId)
			{
				descriptor = candidate;
				return true;
			}
		}

		descriptor = nullptr;
		return false;
	}

	bool VmcsV2BlockDirectory::TryGetLegacyAlias(VmcsField field, std::shared_ptr<VmcsV2FieldDescriptor> &descriptor)
	{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'typeof' operator:
		if (!IsDefinedVmcsField(field))
		{
			descriptor = nullptr;
			return false;
		}

		return TryGetField(static_cast<unsigned short>(field), descriptor);
	}

	void VmcsV2BlockDirectory::AddLegacyVmcsAliases(std::vector<std::shared_ptr<VmcsV2FieldDescriptor>> &fields)
	{
		for (const auto field : VmcsFieldValues)
		{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'ToString':
			fields.push_back(std::make_shared<VmcsV2FieldDescriptor>(static_cast<unsigned short>(field), VmcsFieldName(field), ResolveBlock(field), VmcsV2FieldValueType::UInt64, ResolveAccessPolicy(field), VmcsV2ValidationPolicy::None, ResolveMigrationPolicy(field), 0));
		}
	}

	void VmcsV2BlockDirectory::AddDescriptorFoundationFields(std::vector<std::shared_ptr<VmcsV2FieldDescriptor>> &fields)
	{
		fields.push_back(std::make_shared<VmcsV2FieldDescriptor>(GuestIntegerRegisterFileFieldId, L"GuestIntegerRegisterFile", VmcsV2BlockKind::VirtualCpu, VmcsV2FieldValueType::GuestIntegerRegisterFile, VmcsV2AccessPolicy::GuestState, VmcsV2ValidationPolicy::GuestGprPersistenceRequired, VmcsV2MigrationPolicy::MigratableGuestState, 0));

		AddBlockDescriptor(fields, VmxRootControlBlockFieldId, L"VmxRootControlBlock", VmcsV2BlockKind::VmxRootControl);
		AddBlockDescriptor(fields, VirtualCpuBlockFieldId, L"VirtualCpuBlock", VmcsV2BlockKind::VirtualCpu);
		AddBlockDescriptor(fields, BundleExecutionBlockFieldId, L"BundleExecutionBlock", VmcsV2BlockKind::BundleExecution);
		AddBlockDescriptor(fields, VmxPreemptionTimerBlockFieldId, L"VmxPreemptionTimerBlock", VmcsV2BlockKind::SchedulingBudgetTimer);
		AddBlockDescriptor(fields, VirtualInterruptFabricBlockFieldId, L"VirtualInterruptFabricBlock", VmcsV2BlockKind::VirtualInterruptFabric);
		AddBlockDescriptor(fields, VectorStreamStateBlockFieldId, L"VectorStreamStateBlock", VmcsV2BlockKind::VectorStreamState);
		AddBlockDescriptor(fields, SecurityIsolationBlockFieldId, L"SecurityIsolationBlock", VmcsV2BlockKind::SecurityIsolation);
		AddBlockDescriptor(fields, CapabilityNegotiationBlockFieldId, L"CapabilityNegotiationBlock", VmcsV2BlockKind::CapabilityNegotiation);
		AddBlockDescriptor(fields, NptBlockFieldId, L"NptBlock", VmcsV2BlockKind::NestedTranslation);
		AddBlockDescriptor(fields, ExitInfoBlockFieldId, L"ExitInfoBlock", VmcsV2BlockKind::ExitInformation);
		AddBlockDescriptor(fields, InterceptBitmapBlockFieldId, L"InterceptBitmapBlock", VmcsV2BlockKind::InterceptBitmap);
		AddBlockDescriptor(fields, EventInjectionBlockFieldId, L"EventInjectionBlock", VmcsV2BlockKind::EventInjection);
		AddBlockDescriptor(fields, LaneCompletionRoutingBlockFieldId, L"LaneCompletionRoutingBlock", VmcsV2BlockKind::LaneCompletionRouting);
		AddBlockDescriptor(fields, IoVirtBlockFieldId, L"IoVirtBlock", VmcsV2BlockKind::IoVirt);
		AddBlockDescriptor(fields, Lane6StateBlockFieldId, L"Lane6StateBlock", VmcsV2BlockKind::Lane6State);
		AddBlockDescriptor(fields, Lane7StateBlockFieldId, L"Lane7StateBlock", VmcsV2BlockKind::Lane7State);
		AddBlockDescriptor(fields, ShadowVmcsBlockFieldId, L"ShadowVmcsBlock", VmcsV2BlockKind::ShadowVmcs);
		AddBlockDescriptor(fields, DirtyLogBlockFieldId, L"DirtyLogBlock", VmcsV2BlockKind::DirtyLog);
		AddBlockDescriptor(fields, DebugTraceBlockFieldId, L"DebugTraceBlock", VmcsV2BlockKind::DebugTrace);
	}

	void VmcsV2BlockDirectory::AddBlockDescriptor(std::vector<std::shared_ptr<VmcsV2FieldDescriptor>> &fields, unsigned short fieldId, const std::wstring &name, VmcsV2BlockKind block)
	{
		fields.push_back(std::make_shared<VmcsV2FieldDescriptor>(fieldId, name, block, VmcsV2FieldValueType::DescriptorReference, VmcsV2AccessPolicy::HostEvidence, VmcsV2ValidationPolicy::HostEvidenceForbidden, VmcsV2MigrationPolicy::ReconstructableRuntimeState, 0));
	}

	VmcsV2BlockKind VmcsV2BlockDirectory::ResolveBlock(VmcsField field)
	{
		const auto value = static_cast<unsigned short>(field);
		if (value < 0x20) return VmcsV2BlockKind::VirtualCpu;
		if (value < 0x60) return VmcsV2BlockKind::VmxRootControl;
		if (value < 0x80) return VmcsV2BlockKind::ExitInformation;
		return VmcsV2BlockKind::CapabilityNegotiation;
	}

	std::shared_ptr<VmcsV2AccessPolicy> VmcsV2BlockDirectory::ResolveAccessPolicy(VmcsField field)
	{
		const auto value = static_cast<unsigned short>(field);
		if (value < 0x20) return VmcsV2AccessPolicy::GuestState;
		if (value < 0x40) return VmcsV2AccessPolicy::HostState;
		if (value >= 0x60 && value < 0x80) return VmcsV2AccessPolicy::ExitInfo;
		return VmcsV2AccessPolicy::RootControl;
	}

	std::shared_ptr<VmcsV2MigrationPolicy> VmcsV2BlockDirectory::ResolveMigrationPolicy(VmcsField field)
	{
		const auto value = static_cast<unsigned short>(field);
		if (value < 0x20) return VmcsV2MigrationPolicy::MigratableGuestState;
		if (value < 0x40) return VmcsV2MigrationPolicy::RootLocalState;
		return VmcsV2MigrationPolicy::ReconstructableRuntimeState;
	}
}
