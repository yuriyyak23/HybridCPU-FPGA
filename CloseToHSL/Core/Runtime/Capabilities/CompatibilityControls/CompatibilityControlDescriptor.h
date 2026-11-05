#pragma once
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{

	enum class CompatibilityControlMaterializationState : unsigned char
	{
		NotMaterialized = 0,
		ReadOnlyProjectionAvailable = 1
	};

// C# TO C++ CONVERTER NOTE: The following .NET attribute has no direct equivalent in C++:
// ORIGINAL LINE: [System.Flags] public enum CompatibilityEventRoutingPolicy : ulong
	enum class CompatibilityEventRoutingPolicy : int
	{
		None = 0,
		RuntimeTrapPolicyRequired = 1ULL << 0,
		NeutralTrapResultRequired = 1ULL << 1,
		PublicationFenceRequired = 1ULL << 2
	};
	constexpr CompatibilityEventRoutingPolicy operator&(CompatibilityEventRoutingPolicy left, CompatibilityEventRoutingPolicy right) noexcept
	{
		return static_cast<CompatibilityEventRoutingPolicy>(static_cast<int>(left) & static_cast<int>(right));
	}
	constexpr CompatibilityEventRoutingPolicy operator~(CompatibilityEventRoutingPolicy value) noexcept
	{
		return static_cast<CompatibilityEventRoutingPolicy>(~static_cast<int>(value));
	}

// C# TO C++ CONVERTER NOTE: The following .NET attribute has no direct equivalent in C++:
// ORIGINAL LINE: [System.Flags] public enum CompatibilityExecutionPolicy : ulong
	enum class CompatibilityExecutionPolicy : int
	{
		None = 0,
		RuntimeBoundaryAdmissionRequired = 1ULL << 0,
		ReadProjectionOnly = 1ULL << 1,
		WritesDenied = 1ULL << 2,
		BackendExecutionDenied = 1ULL << 3,
		AuthoritativeMutationDenied = 1ULL << 4
	};

// C# TO C++ CONVERTER NOTE: The following .NET attribute has no direct equivalent in C++:
// ORIGINAL LINE: [System.Flags] public enum CompatibilityExitPublicationPolicy : ulong
	enum class CompatibilityExitPublicationPolicy : int
	{
		None = 0,
		NeutralCompletionSourceRequired = 1ULL << 0,
		PublicationFenceRequired = 1ULL << 1,
		RetirePublicationRequiresNeutralPermit = 1ULL << 2,
		PublicationDeniedUntilRouteMaterialized = 1ULL << 3
	};

// C# TO C++ CONVERTER NOTE: The following .NET attribute has no direct equivalent in C++:
// ORIGINAL LINE: [System.Flags] public enum CompatibilityEntryAdmissionPolicy : ulong
	enum class CompatibilityEntryAdmissionPolicy : int
	{
		None = 0,
		RootAuthorityRequired = 1ULL << 0,
		DescriptorValidationRequired = 1ULL << 1,
		CapabilityValidationRequired = 1ULL << 2,
		SchedulingValidationRequired = 1ULL << 3,
		NoEmissionValidationRequired = 1ULL << 4,
		ProjectionEvidenceRequired = 1ULL << 5
	};

// C# TO C++ CONVERTER NOTE: The following .NET attribute has no direct equivalent in C++:
// ORIGINAL LINE: [System.Flags] public enum CompatibilitySecondaryExecutionPolicy : ulong
	enum class CompatibilitySecondaryExecutionPolicy : int
	{
		None = 0,
		NestedIntentRequiresNeutralOwner = 1ULL << 0,
		SecondStageTranslationRequiresMemoryOwner = 1ULL << 1,
		AddressSpaceTagRequiresMemoryOwner = 1ULL << 2,
		ControlValueProjectionDenied = 1ULL << 3
	};

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct CompatibilityControlReadOnlyView(bool IsMaterialized, CompatibilityEventRoutingPolicy EventRoutingPolicy, CompatibilityExecutionPolicy ExecutionPolicy, CompatibilityExitPublicationPolicy ExitPublicationPolicy, CompatibilityEntryAdmissionPolicy EntryAdmissionPolicy, CompatibilitySecondaryExecutionPolicy SecondaryExecutionPolicy)
//	{
//		public static CompatibilityControlReadOnlyView FailClosedProjectionOnly
//		{
//			get;
//		} = Create(CompatibilityEventRoutingPolicy.RuntimeTrapPolicyRequired | CompatibilityEventRoutingPolicy.NeutralTrapResultRequired | CompatibilityEventRoutingPolicy.PublicationFenceRequired, CompatibilityExecutionPolicy.RuntimeBoundaryAdmissionRequired | CompatibilityExecutionPolicy.ReadProjectionOnly | CompatibilityExecutionPolicy.WritesDenied | CompatibilityExecutionPolicy.BackendExecutionDenied | CompatibilityExecutionPolicy.AuthoritativeMutationDenied, CompatibilityExitPublicationPolicy.NeutralCompletionSourceRequired | CompatibilityExitPublicationPolicy.PublicationFenceRequired | CompatibilityExitPublicationPolicy.RetirePublicationRequiresNeutralPermit | CompatibilityExitPublicationPolicy.PublicationDeniedUntilRouteMaterialized, CompatibilityEntryAdmissionPolicy.RootAuthorityRequired | CompatibilityEntryAdmissionPolicy.DescriptorValidationRequired | CompatibilityEntryAdmissionPolicy.CapabilityValidationRequired | CompatibilityEntryAdmissionPolicy.SchedulingValidationRequired | CompatibilityEntryAdmissionPolicy.NoEmissionValidationRequired | CompatibilityEntryAdmissionPolicy.ProjectionEvidenceRequired, CompatibilitySecondaryExecutionPolicy.NestedIntentRequiresNeutralOwner | CompatibilitySecondaryExecutionPolicy.SecondStageTranslationRequiresMemoryOwner | CompatibilitySecondaryExecutionPolicy.AddressSpaceTagRequiresMemoryOwner | CompatibilitySecondaryExecutionPolicy.ControlValueProjectionDenied);
//
//		public static CompatibilityControlReadOnlyView Create(CompatibilityEventRoutingPolicy eventRoutingPolicy, CompatibilityExecutionPolicy executionPolicy, CompatibilityExitPublicationPolicy exitPublicationPolicy, CompatibilityEntryAdmissionPolicy entryAdmissionPolicy, CompatibilitySecondaryExecutionPolicy secondaryExecutionPolicy)
//		{
//			return new(IsMaterialized: true, eventRoutingPolicy, executionPolicy, exitPublicationPolicy, entryAdmissionPolicy, secondaryExecutionPolicy);
//		}
//
//		public bool IsSemanticallyComplete
//		{
//			get
//			{
//				return IsMaterialized && EventRoutingPolicy != CompatibilityEventRoutingPolicy.None && ExecutionPolicy != CompatibilityExecutionPolicy.None && ExitPublicationPolicy != CompatibilityExitPublicationPolicy.None && EntryAdmissionPolicy != CompatibilityEntryAdmissionPolicy.None && SecondaryExecutionPolicy != CompatibilitySecondaryExecutionPolicy.None;
//			}
//		}
//
//		public bool RequiresRuntimeBoundaryAdmission
//		{
//			get
//			{
//				return (ExecutionPolicy & CompatibilityExecutionPolicy.RuntimeBoundaryAdmissionRequired) != 0;
//			}
//		}
//
//		public bool DeniesWrites
//		{
//			get
//			{
//				return (ExecutionPolicy & CompatibilityExecutionPolicy.WritesDenied) != 0;
//			}
//		}
//
//		public bool DeniesBackendExecution
//		{
//			get
//			{
//				return (ExecutionPolicy & CompatibilityExecutionPolicy.BackendExecutionDenied) != 0;
//			}
//		}
//
//		public bool DeniesAuthoritativeMutation
//		{
//			get
//			{
//				return (ExecutionPolicy & CompatibilityExecutionPolicy.AuthoritativeMutationDenied) != 0;
//			}
//		}
//
//		public bool RequiresNeutralPublicationFence
//		{
//			get
//			{
//				return (ExitPublicationPolicy & CompatibilityExitPublicationPolicy.PublicationFenceRequired) != 0;
//			}
//		}
//
//		public bool KeepsControlValuesUnprojected
//		{
//			get
//			{
//				return (SecondaryExecutionPolicy & CompatibilitySecondaryExecutionPolicy.ControlValueProjectionDenied) != 0;
//			}
//		}
//	}


struct CompatibilityControlReadOnlyView final {
    bool IsMaterialized = false;
    CompatibilityEventRoutingPolicy EventRoutingPolicy = CompatibilityEventRoutingPolicy::None;
    CompatibilityExecutionPolicy ExecutionPolicy = CompatibilityExecutionPolicy::None;
    CompatibilityExitPublicationPolicy ExitPublicationPolicy = CompatibilityExitPublicationPolicy::None;
    CompatibilityEntryAdmissionPolicy EntryAdmissionPolicy = CompatibilityEntryAdmissionPolicy::None;
    CompatibilitySecondaryExecutionPolicy SecondaryExecutionPolicy = CompatibilitySecondaryExecutionPolicy::None;
    bool isSemanticallyComplete() const noexcept { return IsMaterialized && EventRoutingPolicy != CompatibilityEventRoutingPolicy::None && ExecutionPolicy != CompatibilityExecutionPolicy::None && ExitPublicationPolicy != CompatibilityExitPublicationPolicy::None && EntryAdmissionPolicy != CompatibilityEntryAdmissionPolicy::None && SecondaryExecutionPolicy != CompatibilitySecondaryExecutionPolicy::None; }
    bool requiresRuntimeBoundaryAdmission() const noexcept { return (static_cast<int>(ExecutionPolicy) & static_cast<int>(CompatibilityExecutionPolicy::RuntimeBoundaryAdmissionRequired)) != 0; }
    bool deniesWrites() const noexcept { return (static_cast<int>(ExecutionPolicy) & static_cast<int>(CompatibilityExecutionPolicy::WritesDenied)) != 0; }
    bool deniesBackendExecution() const noexcept { return (static_cast<int>(ExecutionPolicy) & static_cast<int>(CompatibilityExecutionPolicy::BackendExecutionDenied)) != 0; }
    bool deniesAuthoritativeMutation() const noexcept { return (static_cast<int>(ExecutionPolicy) & static_cast<int>(CompatibilityExecutionPolicy::AuthoritativeMutationDenied)) != 0; }
    bool requiresNeutralPublicationFence() const noexcept { return (static_cast<int>(ExitPublicationPolicy) & static_cast<int>(CompatibilityExitPublicationPolicy::PublicationFenceRequired)) != 0; }
    bool keepsControlValuesUnprojected() const noexcept { return (static_cast<int>(SecondaryExecutionPolicy) & static_cast<int>(CompatibilitySecondaryExecutionPolicy::ControlValueProjectionDenied)) != 0; }
    static const std::shared_ptr<CompatibilityControlReadOnlyView> FailClosedProjectionOnly;
};
inline const std::shared_ptr<CompatibilityControlReadOnlyView> CompatibilityControlReadOnlyView::FailClosedProjectionOnly = std::make_shared<CompatibilityControlReadOnlyView>(CompatibilityControlReadOnlyView{true, static_cast<CompatibilityEventRoutingPolicy>(7), static_cast<CompatibilityExecutionPolicy>(31), static_cast<CompatibilityExitPublicationPolicy>(15), static_cast<CompatibilityEntryAdmissionPolicy>(63), static_cast<CompatibilitySecondaryExecutionPolicy>(15)});
}
