#pragma once

#include "../../../../../Runtime/Domains/Descriptors/EvidencePolicy/YAKSys_Hybrid_CPU.Core.EvidencePolicyDescriptor.h"
#include "../../../../../Runtime/Capabilities/Grants/YAKSys_Hybrid_CPU.Core.CapabilityGrant.h"
#include "../../../../../Virtualization/Compatibility/Frontend/Decode/VmxInstructionPayload.h"
#include "../../../../../Runtime/Capabilities/Grants/CapabilityGrant.h"
#include <memory>
#include <utility>
#include <vector>

namespace YAKSys_Hybrid_CPU::Core::Nested
{

// C# TO C++ CONVERTER NOTE: The following .NET attribute has no direct equivalent in C++:
// ORIGINAL LINE: [Flags] public enum NestedEnablementGate : ulong
	enum class NestedEnablementGate : int
	{
		None = 0,
		NestedCompatibilityPolicyEnabled = 1ULL << 0,
		CompleteGprPersistence = 1ULL << 1,
		HostEvidenceExcluded = 1ULL << 2,
		BundleLegalityReady = 1ULL << 3,
		InterruptFabricReady = 1ULL << 4,
		SecurityIsolationReady = 1ULL << 5,
		NestedMemoryCompositionReady = 1ULL << 6,
		InterceptTranslationReady = 1ULL << 7,
		Lane6Lane7PassthroughBlocked = 1ULL << 8,

		RequiredForPhase7 = NestedCompatibilityPolicyEnabled | CompleteGprPersistence | HostEvidenceExcluded | BundleLegalityReady | InterruptFabricReady | SecurityIsolationReady | NestedMemoryCompositionReady | InterceptTranslationReady | Lane6Lane7PassthroughBlocked
	};

// C# TO C++ CONVERTER NOTE: The following .NET attribute has no direct equivalent in C++:
// ORIGINAL LINE: [Flags] public enum NestedCapabilityGrantMask : ulong
	enum class NestedCapabilityGrantMask : int
	{
		None = 0,
		ChildDomainIntentDescriptor = 1ULL << 0,
		ComposedDomainProjection = 1ULL << 1,
		VmReadVmWriteBitmaps = 1ULL << 2,
		InterceptTranslation = 1ULL << 3,
		ExitMapping = 1ULL << 4,
		NestedMemoryComposition = 1ULL << 5,
		VirtualInterruptReflection = 1ULL << 6,
		PreemptionTimerReflection = 1ULL << 7,
		LanePassthroughBlocked = 1ULL << 8,

		RequiredForPhase7 = ChildDomainIntentDescriptor | ComposedDomainProjection | VmReadVmWriteBitmaps | InterceptTranslation | ExitMapping | NestedMemoryComposition | VirtualInterruptReflection | PreemptionTimerReflection | LanePassthroughBlocked
	};

		struct PublishedCapabilityProjection final
	{
		const unsigned long long CompatibilityWord;
		const bool DescriptorBacked;
		const bool PolicyFiltered;
		const bool IsProjectionOnly;
		PublishedCapabilityProjection(unsigned long long compatibilityWord, bool descriptorBacked, bool policyFiltered)
			: CompatibilityWord(compatibilityWord), DescriptorBacked(descriptorBacked), PolicyFiltered(policyFiltered), IsProjectionOnly(descriptorBacked && policyFiltered) {}
		bool Contains(unsigned long long capabilityMask) const { return capabilityMask != 0 && (CompatibilityWord & capabilityMask) == capabilityMask; }
		static PublishedCapabilityProjection FromCompatibilityAlias(unsigned long long compatibilityWord) { return PublishedCapabilityProjection(compatibilityWord, false, false); }
		static PublishedCapabilityProjection FromTypedGrant(const YAKSys_Hybrid_CPU::Core::CapabilityGrant &grant) { return PublishedCapabilityProjection(grant.isPublishableCompatibilityGrant() ? grant.getCapabilityMask() : 0, true, true); }
	};

	enum class NestedDomainCapability : unsigned short { None = 0, NestedCompatibilityProjection = 1 };

	struct NestedDomainCapabilityProjection final
	{
		const NestedDomainCapability Capability;
		const unsigned long long FrontendCapabilityMask;
		const bool IsValid;
		NestedDomainCapabilityProjection(NestedDomainCapability capability, unsigned long long frontendCapabilityMask)
			: Capability(capability), FrontendCapabilityMask(frontendCapabilityMask), IsValid(capability != NestedDomainCapability::None && frontendCapabilityMask != 0) {}
		static const NestedDomainCapabilityProjection VmxCompatibility;
	};

	struct NestedCompatibilityCapabilityRequirement final
	{
		const NestedDomainCapabilityProjection Projection;
		const YAKSys_Hybrid_CPU::Core::CapabilityGrant RequiredGrant;
		const NestedDomainCapability Capability;
		const unsigned long long CapabilityMask;
		const bool IsTypedAuthority;
		NestedCompatibilityCapabilityRequirement(NestedDomainCapabilityProjection projection, YAKSys_Hybrid_CPU::Core::CapabilityGrant requiredGrant)
			: Projection(projection), RequiredGrant(std::move(requiredGrant)), Capability(Projection.Capability), CapabilityMask(Projection.FrontendCapabilityMask),
			  IsTypedAuthority(Projection.IsValid && RequiredGrant.getHasTypedAuthority() && RequiredGrant.isPublishableCompatibilityGrant() && CapabilityMask != 0 && (RequiredGrant.getCapabilityMask() & CapabilityMask) == CapabilityMask) {}
		static NestedCompatibilityCapabilityRequirement Create(unsigned long long ownerDomainId)
		{
			return NestedCompatibilityCapabilityRequirement(NestedDomainCapabilityProjection::VmxCompatibility,
				YAKSys_Hybrid_CPU::Core::CapabilityGrant(VmxV2InstructionCaps::NestedVmx, YAKSys_Hybrid_CPU::Core::CapabilityGrantScope::CompatibilityProjection,
					ownerDomainId != YAKSys_Hybrid_CPU::Core::CapabilityGrant::NoOwnerDomainId, ownerDomainId,
					YAKSys_Hybrid_CPU::Core::CapabilityDelegationPolicy::NonDelegable, YAKSys_Hybrid_CPU::Core::CapabilityRevocationPolicy::RuntimeRevocable,
					YAKSys_Hybrid_CPU::Core::CapabilityMigrationClass::DomainLocal, YAKSys_Hybrid_CPU::Core::CapabilityEvidenceVisibility::GuestVisibleProjection,
					YAKSys_Hybrid_CPU::Core::CapabilityFrontendProjectionPolicy::ProjectIfCompatible));
		}
	};

	struct NestedCapabilityPublication final
	{
		const PublishedCapabilityProjection Projection;
		const NestedCompatibilityCapabilityRequirement Requirement;
		const bool IsDescriptorBacked;
		const bool CanPublishRequiredCapability;
		NestedCapabilityPublication(PublishedCapabilityProjection projection, NestedCompatibilityCapabilityRequirement requirement)
			: Projection(projection), Requirement(std::move(requirement)), IsDescriptorBacked(Projection.IsProjectionOnly && Requirement.IsTypedAuthority),
			  CanPublishRequiredCapability(IsDescriptorBacked && Projection.Contains(Requirement.CapabilityMask)) {}
		static std::shared_ptr<NestedCapabilityPublication> FromCompatibilityAlias(unsigned long long compatibilityWord, unsigned long long ownerDomainId)
		{
			return std::make_shared<NestedCapabilityPublication>(PublishedCapabilityProjection::FromCompatibilityAlias(compatibilityWord), NestedCompatibilityCapabilityRequirement::Create(ownerDomainId));
		}
		static std::shared_ptr<NestedCapabilityPublication> FromTypedGrant(unsigned long long ownerDomainId)
		{
			auto requirement = NestedCompatibilityCapabilityRequirement::Create(ownerDomainId);
			return std::make_shared<NestedCapabilityPublication>(PublishedCapabilityProjection::FromTypedGrant(requirement.RequiredGrant), std::move(requirement));
		}
	};
constexpr NestedEnablementGate operator|(NestedEnablementGate left, NestedEnablementGate right)
	{
		return static_cast<NestedEnablementGate>(static_cast<unsigned int>(left) | static_cast<unsigned int>(right));
	}

	constexpr NestedEnablementGate operator&(NestedEnablementGate left, NestedEnablementGate right)
	{
		return static_cast<NestedEnablementGate>(static_cast<unsigned int>(left) & static_cast<unsigned int>(right));
	}

	constexpr NestedCapabilityGrantMask operator|(NestedCapabilityGrantMask left, NestedCapabilityGrantMask right)
	{
		return static_cast<NestedCapabilityGrantMask>(static_cast<unsigned int>(left) | static_cast<unsigned int>(right));
	}

	constexpr NestedCapabilityGrantMask operator&(NestedCapabilityGrantMask left, NestedCapabilityGrantMask right)
	{
		return static_cast<NestedCapabilityGrantMask>(static_cast<unsigned int>(left) & static_cast<unsigned int>(right));
	}

	struct NestedCapabilityGrantDescriptor final
	{
		const NestedCapabilityGrantMask Capability;
		const YAKSys_Hybrid_CPU::Core::CapabilityGrantScope Scope;
		const bool DescriptorBacked;
		const bool EvidencePolicyBound;
		const bool IsAuthoritative;

		NestedCapabilityGrantDescriptor(NestedCapabilityGrantMask capability,
			YAKSys_Hybrid_CPU::Core::CapabilityGrantScope scope, bool descriptorBacked, bool evidencePolicyBound)
			: Capability(capability), Scope(scope), DescriptorBacked(descriptorBacked),
			  EvidencePolicyBound(evidencePolicyBound),
			  IsAuthoritative(capability != NestedCapabilityGrantMask::None &&
				scope == YAKSys_Hybrid_CPU::Core::CapabilityGrantScope::DomainGranted &&
				descriptorBacked && evidencePolicyBound) {}
	};

	struct NestedProofAuthoritySource final
	{
		const bool DescriptorBacked;
		const bool EvidencePolicyBound;
		const EvidenceVisibilityClass GateEvidenceClass;
		const bool RuntimeValidated;
		const bool CanAuthorizeCapabilityProof;
		const bool CanAuthorizeGateProof;

		NestedProofAuthoritySource() : NestedProofAuthoritySource(false, false, EvidenceVisibilityClass::GuestArchitecturalState, false) {}

		NestedProofAuthoritySource(bool descriptorBacked, bool evidencePolicyBound,
			EvidenceVisibilityClass gateEvidenceClass, bool runtimeValidated)
			: DescriptorBacked(descriptorBacked), EvidencePolicyBound(evidencePolicyBound),
			  GateEvidenceClass(gateEvidenceClass), RuntimeValidated(runtimeValidated),
			  CanAuthorizeCapabilityProof(descriptorBacked && evidencePolicyBound),
			  CanAuthorizeGateProof(runtimeValidated && gateEvidenceClass == EvidenceVisibilityClass::GuestArchitecturalState) {}

		static const NestedProofAuthoritySource FailClosed;
		static const NestedProofAuthoritySource RuntimePolicyValidated;
	};

	struct NestedEnablementGateDescriptor final
	{
		const NestedEnablementGate Gate;
		const EvidenceVisibilityClass EvidenceClass;
		const bool RuntimeValidated;
		const bool IsAuthoritative;

		NestedEnablementGateDescriptor(NestedEnablementGate gate, EvidenceVisibilityClass evidenceClass,
			bool runtimeValidated)
			: Gate(gate), EvidenceClass(evidenceClass), RuntimeValidated(runtimeValidated),
			  IsAuthoritative(gate != NestedEnablementGate::None && runtimeValidated &&
				evidenceClass == EvidenceVisibilityClass::GuestArchitecturalState) {}
	};

	struct NestedEnablementProof final
	{
		const std::vector<NestedCapabilityGrantDescriptor> CapabilityGrants;
		const std::vector<NestedEnablementGateDescriptor> GateProofs;
		const bool HasRequiredCapabilities;
		const bool HasRequiredGates;
		const bool IsAuthoritative;

		NestedEnablementProof(std::vector<NestedCapabilityGrantDescriptor> capabilityGrants,
			std::vector<NestedEnablementGateDescriptor> gateProofs)
			: CapabilityGrants(std::move(capabilityGrants)), GateProofs(std::move(gateProofs)),
			  HasRequiredCapabilities(HasCapabilities()), HasRequiredGates(HasGates()),
			  IsAuthoritative(HasRequiredCapabilities && HasRequiredGates) {}

		bool HasCapabilities() const
		{
			unsigned int granted = 0;
			for (const auto &grant : CapabilityGrants)
				if (grant.IsAuthoritative) granted |= static_cast<unsigned int>(grant.Capability);
			const auto required = static_cast<unsigned int>(NestedCapabilityGrantMask::RequiredForPhase7);
			return (granted & required) == required;
		}

		bool HasGates() const
		{
			unsigned int proven = 0;
			for (const auto &gate : GateProofs)
				if (gate.IsAuthoritative) proven |= static_cast<unsigned int>(gate.Gate);
			const auto required = static_cast<unsigned int>(NestedEnablementGate::RequiredForPhase7);
			return (proven & required) == required;
		}

		static std::shared_ptr<NestedEnablementProof> FromTypedDescriptorProofs(
			NestedCapabilityGrantMask capabilities, NestedEnablementGate gates,
			NestedProofAuthoritySource authoritySource)
		{
			const NestedCapabilityGrantMask grants[] = {
				NestedCapabilityGrantMask::ChildDomainIntentDescriptor,
				NestedCapabilityGrantMask::ComposedDomainProjection,
				NestedCapabilityGrantMask::VmReadVmWriteBitmaps,
				NestedCapabilityGrantMask::InterceptTranslation,
				NestedCapabilityGrantMask::ExitMapping,
				NestedCapabilityGrantMask::NestedMemoryComposition,
				NestedCapabilityGrantMask::VirtualInterruptReflection,
				NestedCapabilityGrantMask::PreemptionTimerReflection,
				NestedCapabilityGrantMask::LanePassthroughBlocked};
			const NestedEnablementGate gateKinds[] = {
				NestedEnablementGate::NestedCompatibilityPolicyEnabled,
				NestedEnablementGate::CompleteGprPersistence,
				NestedEnablementGate::HostEvidenceExcluded,
				NestedEnablementGate::BundleLegalityReady,
				NestedEnablementGate::InterruptFabricReady,
				NestedEnablementGate::SecurityIsolationReady,
				NestedEnablementGate::NestedMemoryCompositionReady,
				NestedEnablementGate::InterceptTranslationReady,
				NestedEnablementGate::Lane6Lane7PassthroughBlocked};
			std::vector<NestedCapabilityGrantDescriptor> capabilityProofs;
			std::vector<NestedEnablementGateDescriptor> gateProofs;
			for (auto grant : grants)
			{
				const bool present = (capabilities & grant) == grant;
				capabilityProofs.emplace_back(grant, YAKSys_Hybrid_CPU::Core::CapabilityGrantScope::DomainGranted,
					present && authoritySource.CanAuthorizeCapabilityProof,
					present && authoritySource.CanAuthorizeCapabilityProof);
			}
			for (auto gate : gateKinds)
			{
				const bool present = (gates & gate) == gate;
				gateProofs.emplace_back(gate, authoritySource.GateEvidenceClass,
					present && authoritySource.CanAuthorizeGateProof);
			}
			return std::make_shared<NestedEnablementProof>(std::move(capabilityProofs), std::move(gateProofs));
		}
	};
// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct PublishedCapabilityProjection(ulong CompatibilityWord, bool DescriptorBacked, bool PolicyFiltered)
//	{
//		public bool IsProjectionOnly
//		{n
//			get
//			{
//				return DescriptorBacked && PolicyFiltered;
//			}
//		}
//
//		public bool Contains(ulong capabilityMask)
//		{
//			return capabilityMask != 0 && (CompatibilityWord & capabilityMask) == capabilityMask;
//		}
//
//		public static PublishedCapabilityProjection FromCompatibilityAlias(ulong compatibilityWord)
//		{
//			return new(compatibilityWord, DescriptorBacked: false, PolicyFiltered: false);
//		}
//
//		public static PublishedCapabilityProjection FromTypedGrant(CapabilityGrant grant)
//		{
//			return new(grant.IsPublishableCompatibilityGrant ? grant.CapabilityMask : 0, DescriptorBacked: true, PolicyFiltered: true);
//		}
//	}


// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct NestedDomainCapabilityProjection(NestedDomainCapability Capability, ulong FrontendCapabilityMask)
//	{
//		public bool IsValid
//		{
//			get
//			{
//				return Capability != NestedDomainCapability.None && FrontendCapabilityMask != 0;
//			}
//		}
//
//		public static NestedDomainCapabilityProjection VmxCompatibility
//		{
//			get;
//		} = new(NestedDomainCapability.NestedCompatibilityProjection, VmxV2InstructionCaps.NestedVmx);
//	}

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct NestedCompatibilityCapabilityRequirement(NestedDomainCapabilityProjection Projection, CapabilityGrant RequiredGrant)
//	{
//		public NestedDomainCapability Capability
//		{
//			get
//			{
//				return Projection.Capability;
//			}
//		}
//
//		public ulong CapabilityMask
//		{
//			get
//			{
//				return Projection.FrontendCapabilityMask;
//			}
//		}
//
//		public bool IsTypedAuthority
//		{
//			get
//			{
//				return Projection.IsValid && RequiredGrant.HasTypedAuthority && RequiredGrant.IsPublishableCompatibilityGrant && RequiredGrant.Grants(CapabilityMask);
//			}
//		}
//
//		public static NestedCompatibilityCapabilityRequirement Create(ulong ownerDomainId)
//		{
//			return new(NestedDomainCapabilityProjection.VmxCompatibility, new CapabilityGrant(VmxV2InstructionCaps.NestedVmx, CapabilityGrantScope.CompatibilityProjection, isGranted: ownerDomainId != CapabilityGrant.NoOwnerDomainId, ownerDomainId, CapabilityDelegationPolicy.NonDelegable, CapabilityRevocationPolicy.RuntimeRevocable, CapabilityMigrationClass.DomainLocal, CapabilityEvidenceVisibility.GuestVisibleProjection, CapabilityFrontendProjectionPolicy.ProjectIfCompatible));
//		}
//	}

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct NestedCapabilityPublication(PublishedCapabilityProjection Projection, NestedCompatibilityCapabilityRequirement Requirement)
//	{
//		public bool IsDescriptorBacked
//		{
//			get
//			{
//				return Projection.IsProjectionOnly && Requirement.IsTypedAuthority;
//			}
//		}
//
//		public bool CanPublishRequiredCapability
//		{
//			get
//			{
//				return IsDescriptorBacked && Projection.Contains(Requirement.CapabilityMask);
//			}
//		}
//
//		public static NestedCapabilityPublication FromCompatibilityAlias(ulong compatibilityWord, ulong ownerDomainId)
//		{
//			return new(PublishedCapabilityProjection.FromCompatibilityAlias(compatibilityWord), NestedCompatibilityCapabilityRequirement.Create(ownerDomainId));
//		}
//
//		public static NestedCapabilityPublication FromTypedGrant(ulong ownerDomainId)
//		{
//			NestedCompatibilityCapabilityRequirement requirement = NestedCompatibilityCapabilityRequirement.Create(ownerDomainId);
//
//			return new(PublishedCapabilityProjection.FromTypedGrant(requirement.RequiredGrant), requirement);
//		}
//	}

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct NestedCapabilityGrantDescriptor(NestedCapabilityGrantMask Capability, CapabilityGrantScope Scope, bool DescriptorBacked, bool EvidencePolicyBound)
//	{
//		public bool IsAuthoritative
//		{
//			get
//			{
//				return Capability != NestedCapabilityGrantMask.None && Scope == CapabilityGrantScope.DomainGranted && DescriptorBacked && EvidencePolicyBound;
//			}
//		}
//	}

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct NestedProofAuthoritySource(bool DescriptorBacked, bool EvidencePolicyBound, EvidenceVisibilityClass GateEvidenceClass, bool RuntimeValidated)
//	{
//		public bool CanAuthorizeCapabilityProof
//		{
//			get
//			{
//				return DescriptorBacked && EvidencePolicyBound;
//			}
//		}
//
//		public bool CanAuthorizeGateProof
//		{
//			get
//			{
//				return RuntimeValidated && GateEvidenceClass == EvidenceVisibilityClass.GuestArchitecturalState;
//			}
//		}
//
//		public static NestedProofAuthoritySource FailClosed
//		{
//			get;
//		} = new(DescriptorBacked: false, EvidencePolicyBound: false, EvidenceVisibilityClass.HostOwnedRuntimeEvidence, RuntimeValidated: false);
//
//		public static NestedProofAuthoritySource RuntimePolicyValidated
//		{
//			get;
//		} = new(DescriptorBacked: true, EvidencePolicyBound: true, EvidenceVisibilityClass.GuestArchitecturalState, RuntimeValidated: true);
//	}

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct NestedEnablementGateDescriptor(NestedEnablementGate Gate, EvidenceVisibilityClass EvidenceClass, bool RuntimeValidated)
//	{
//		public bool IsAuthoritative
//		{
//			get
//			{
//				return Gate != NestedEnablementGate.None && RuntimeValidated && EvidenceClass == EvidenceVisibilityClass.GuestArchitecturalState;
//			}
//		}
//	}

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct NestedEnablementProof(NestedCapabilityGrantDescriptor[] CapabilityGrants, NestedEnablementGateDescriptor[] GateProofs)
//	{
//		public bool HasRequiredCapabilities
//		{
//			get
//			{
//				NestedCapabilityGrantMask granted = NestedCapabilityGrantMask.None;
//
//				foreach (NestedCapabilityGrantDescriptor grant in CapabilityGrants ?? Array.Empty<NestedCapabilityGrantDescriptor>())
//				{
//					if (grant.IsAuthoritative)
//					{
//						granted |= grant.Capability;
//					}
//				}
//
//				return (granted & NestedCapabilityGrantMask.RequiredForPhase7) == NestedCapabilityGrantMask.RequiredForPhase7;
//			}
//		}
//
//		public bool HasRequiredGates
//		{
//			get
//			{
//				NestedEnablementGate proven = NestedEnablementGate.None;
//
//				foreach (NestedEnablementGateDescriptor gate in GateProofs ?? Array.Empty<NestedEnablementGateDescriptor>())
//				{
//					if (gate.IsAuthoritative)
//					{
//						proven |= gate.Gate;
//					}
//				}
//
//				return (proven & NestedEnablementGate.RequiredForPhase7) == NestedEnablementGate.RequiredForPhase7;
//			}
//		}
//
//		public bool IsAuthoritative
//		{
//			get
//			{
//				return HasRequiredCapabilities && HasRequiredGates;
//			}
//		}
//
//		public static NestedEnablementProof FromTypedDescriptorProofs(NestedCapabilityGrantMask capabilities, NestedEnablementGate gates, NestedProofAuthoritySource authoritySource)
//		{
//			NestedCapabilityGrantDescriptor[] capabilityGrants = { CreateGrant(capabilities, NestedCapabilityGrantMask.ChildDomainIntentDescriptor, authoritySource), CreateGrant(capabilities, NestedCapabilityGrantMask.ComposedDomainProjection, authoritySource), CreateGrant(capabilities, NestedCapabilityGrantMask.VmReadVmWriteBitmaps, authoritySource), CreateGrant(capabilities, NestedCapabilityGrantMask.InterceptTranslation, authoritySource), CreateGrant(capabilities, NestedCapabilityGrantMask.ExitMapping, authoritySource), CreateGrant(capabilities, NestedCapabilityGrantMask.NestedMemoryComposition, authoritySource), CreateGrant(capabilities, NestedCapabilityGrantMask.VirtualInterruptReflection, authoritySource), CreateGrant(capabilities, NestedCapabilityGrantMask.PreemptionTimerReflection, authoritySource), CreateGrant(capabilities, NestedCapabilityGrantMask.LanePassthroughBlocked, authoritySource)};
//
//			NestedEnablementGateDescriptor[] gateProofs = { CreateGate(gates, NestedEnablementGate.NestedCompatibilityPolicyEnabled, authoritySource), CreateGate(gates, NestedEnablementGate.CompleteGprPersistence, authoritySource), CreateGate(gates, NestedEnablementGate.HostEvidenceExcluded, authoritySource), CreateGate(gates, NestedEnablementGate.BundleLegalityReady, authoritySource), CreateGate(gates, NestedEnablementGate.InterruptFabricReady, authoritySource), CreateGate(gates, NestedEnablementGate.SecurityIsolationReady, authoritySource), CreateGate(gates, NestedEnablementGate.NestedMemoryCompositionReady, authoritySource), CreateGate(gates, NestedEnablementGate.InterceptTranslationReady, authoritySource), CreateGate(gates, NestedEnablementGate.Lane6Lane7PassthroughBlocked, authoritySource)};
//
//			return new NestedEnablementProof(capabilityGrants, gateProofs);
//		}
//
//		private static NestedCapabilityGrantDescriptor CreateGrant(NestedCapabilityGrantMask source, NestedCapabilityGrantMask capability, NestedProofAuthoritySource authoritySource)
//		{
//			return new(capability, CapabilityGrantScope.DomainGranted, DescriptorBacked: (source & capability) == capability && authoritySource.CanAuthorizeCapabilityProof, EvidencePolicyBound: (source & capability) == capability && authoritySource.CanAuthorizeCapabilityProof);
//		}
//
//		private static NestedEnablementGateDescriptor CreateGate(NestedEnablementGate source, NestedEnablementGate gate, NestedProofAuthoritySource authoritySource)
//		{
//			return new(gate, authoritySource.GateEvidenceClass, RuntimeValidated: (source & gate) == gate && authoritySource.CanAuthorizeGateProof);
//		}
//	}
	struct NestedEnablementRequest final
	{
		const unsigned long long CompatibilityCapabilityProjection;
		const NestedCapabilityGrantMask NestedCapabilities;
		const NestedEnablementGate ProvenGates;
		const unsigned long long OwnerDomainId;
		const NestedProofAuthoritySource AuthoritySource;
		const unsigned long long CompatibilityProjectionWord;
		const PublishedCapabilityProjection CompatibilityCapabilities;
		const std::shared_ptr<NestedCapabilityPublication> CapabilityPublication;
		const std::shared_ptr<NestedEnablementProof> EnablementProof;
		const bool HasExplicitVmxCapability;
		const bool HasRequiredNestedCapabilities;
		const bool HasRequiredGates;

		NestedEnablementRequest(unsigned long long compatibilityCapabilityProjection = 0,
			NestedCapabilityGrantMask nestedCapabilities = NestedCapabilityGrantMask::None,
			NestedEnablementGate provenGates = NestedEnablementGate::None,
			unsigned long long ownerDomainId = YAKSys_Hybrid_CPU::Core::CapabilityGrant::NoOwnerDomainId,
			NestedProofAuthoritySource authoritySource = NestedProofAuthoritySource())
			: CompatibilityCapabilityProjection(compatibilityCapabilityProjection), NestedCapabilities(nestedCapabilities),
			  ProvenGates(provenGates), OwnerDomainId(ownerDomainId), AuthoritySource(std::move(authoritySource)),
			  CompatibilityProjectionWord(CompatibilityCapabilityProjection),
			  CompatibilityCapabilities(PublishedCapabilityProjection::FromCompatibilityAlias(CompatibilityProjectionWord)),
			  CapabilityPublication(NestedCapabilityPublication::FromTypedGrant(OwnerDomainId)),
			  EnablementProof(NestedEnablementProof::FromTypedDescriptorProofs(NestedCapabilities, ProvenGates, AuthoritySource)),
			  HasExplicitVmxCapability(CapabilityPublication->CanPublishRequiredCapability),
			  HasRequiredNestedCapabilities(EnablementProof->HasRequiredCapabilities),
			  HasRequiredGates(EnablementProof->HasRequiredGates) {}
	};


// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct NestedEnablementRequest(ulong CompatibilityCapabilityProjection, NestedCapabilityGrantMask NestedCapabilities, NestedEnablementGate ProvenGates, ulong OwnerDomainId = CapabilityGrant.NoOwnerDomainId, NestedProofAuthoritySource AuthoritySource = default)
//	{
//		public ulong CompatibilityProjectionWord
//		{
//			get
//			{
//				return CompatibilityCapabilityProjection;
//			}
//		}
//
//		public PublishedCapabilityProjection CompatibilityCapabilities
//		{
//			get
//			{
//				return PublishedCapabilityProjection.FromCompatibilityAlias(CompatibilityProjectionWord);
//			}
//		}
//
//		public NestedCapabilityPublication CapabilityPublication
//		{
//			get
//			{
//				return NestedCapabilityPublication.FromTypedGrant(OwnerDomainId);
//			}
//		}
//
//		public NestedEnablementProof EnablementProof
//		{
//			get
//			{
//				return NestedEnablementProof.FromTypedDescriptorProofs(NestedCapabilities, ProvenGates, AuthoritySource);
//			}
//		}
//
//		public bool HasExplicitVmxCapability
//		{
//			get
//			{
//				return CapabilityPublication.CanPublishRequiredCapability;
//			}
//		}
//
//		public bool HasRequiredNestedCapabilities
//		{
//			get
//			{
//				return EnablementProof.HasRequiredCapabilities;
//			}
//		}
//
//		public bool HasRequiredGates
//		{
//			get
//			{
//				return EnablementProof.HasRequiredGates;
//			}
//		}
//	}


}





