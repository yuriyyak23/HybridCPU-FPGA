#pragma once
#include "../Architecture/Common/YAKSys_Hybrid_CPU.Processor.h"
#include "../Memory/IAtomicMemoryUnit.h"
#include "../../Memory/Subsystem/YAKSys_Hybrid_CPU.Memory.MemorySubsystem.h"
#include "../Runtime/Memory/Translation/CpuInstructionTranslationContour.h"
#include <functional>
#include <memory>
#include <optional>
#include <algorithm>
#include <cwctype>
#include <limits>
#include <stdexcept>
#include <string>
#include <utility>

namespace YAKSys_Hybrid_CPU::Core { class IHybridCpuIseEcallBridgeV1; class MicroOpScheduler; }

using namespace YAKSys_Hybrid_CPU::Core;
using namespace YAKSys_Hybrid_CPU::Core::Memory;

namespace YAKSys_Hybrid_CPU
{
	struct CpuExactProbeRuntimeConstructionProfile final
	{
		static inline const std::wstring ProvenContourSubjectCommitSha = L"bcd2d7f4654d4dab17c7a6705cb885fdd572510d";
		unsigned long long DomainTag = 0;
		std::wstring ProvenContourCommitSha;
		bool IsConfigured = false;
		CpuExactProbeRuntimeConstructionProfile() = default;
		CpuExactProbeRuntimeConstructionProfile(unsigned long long domainTag, std::wstring subjectSha)
			: DomainTag(domainTag), ProvenContourCommitSha(std::move(subjectSha)), IsConfigured(true)
		{
			if (DomainTag == 0 || DomainTag > std::numeric_limits<unsigned short>::max())
			{
				throw std::out_of_range("Exact probe domain tag must fit a nonzero 16-bit identifier.");
			}
			const bool matchesReviewedSubject = ProvenContourCommitSha.size() == ProvenContourSubjectCommitSha.size() && std::equal(ProvenContourCommitSha.begin(), ProvenContourCommitSha.end(), ProvenContourSubjectCommitSha.begin(), [](wchar_t left, wchar_t right) { return std::towlower(left) == std::towlower(right); });
			if (!matchesReviewedSubject)
			{
				throw std::invalid_argument("Exact probe construction requires the reviewed contour subject SHA.");
			}
		}
	};

	namespace Core { struct CpuCorePlatformContext final {
		std::shared_ptr<Processor::MainMemoryArea> MainMemory;
		std::shared_ptr<YAKSys_Hybrid_CPU::Memory::MemorySubsystem> MemorySubsystem;
		ProcessorMode InitialExecutionMode = ProcessorMode::Emulation;
		bool TrackGlobalExecutionMode = false;
		bool IsConfigured = false;
		std::shared_ptr<CpuInstructionTranslationPolicy> CpuInstructionTranslationPolicy;
		std::shared_ptr<CompatibilityControlPolicyConstructionProfile> CompatibilityControlPolicyProfile;
		bool EnableCompletionReasonQualificationVmRead = false;
		bool EnableCompletionSecondStageVmRead = false;
		bool EnablePinBasedControlsVmRead = false;
		std::optional<CpuExactProbeRuntimeConstructionProfile> ExactProbeRuntimeProfile;
		std::shared_ptr<IHybridCpuIseEcallBridgeV1> EcallBridge;
		std::shared_ptr<MicroOpScheduler> OwnedSoftwareSecureScheduler;
		CpuCorePlatformContext() = default;
		ProcessorMode ResolveExecutionMode() const { return TrackGlobalExecutionMode ? Processor::CurrentProcessorMode : InitialExecutionMode; }
		std::shared_ptr<Memory::IAtomicMemoryUnit> CreateAtomicMemoryUnit() const;
		static CpuCorePlatformContext CreateLegacy(std::shared_ptr<Processor::MainMemoryArea> mainMemory = nullptr);
		static CpuCorePlatformContext CreateFixed(std::shared_ptr<Processor::MainMemoryArea> mainMemory, ProcessorMode executionMode, std::shared_ptr<IHybridCpuIseEcallBridgeV1> ecallBridge = nullptr);
		static CpuCorePlatformContext CreateSoftwareSecureFixed(std::shared_ptr<Processor::MainMemoryArea> mainMemory, ProcessorMode executionMode, std::shared_ptr<IHybridCpuIseEcallBridgeV1> ecallBridge = nullptr);
	}; }
	/// <summary>
	/// Immutable construction request for the one already-proven neutral
	/// PROBE_NO_STATE_V1 runtime contour. The recorded subject SHA identifies
	/// the reviewed contour; it is evidence binding, not runtime authority.
	/// </summary>
// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct CpuExactProbeRuntimeConstructionProfile
//	{
//		public const string ProvenContourSubjectCommitSha = "bcd2d7f4654d4dab17c7a6705cb885fdd572510d";
//
//		public CpuExactProbeRuntimeConstructionProfile(ulong domainTag, string provenContourSubjectCommitSha)
//		{
//			if (domainTag == 0 || domainTag > ushort.MaxValue)
//				throw new ArgumentOutOfRangeException(nameof(domainTag));
//			if (!string.Equals(provenContourSubjectCommitSha, ProvenContourSubjectCommitSha, StringComparison.OrdinalIgnoreCase))
//			{
//				throw new ArgumentException("Exact probe construction requires the reviewed contour subject SHA.", nameof(provenContourSubjectCommitSha));
//			}
//
//			DomainTag = domainTag;
//			ProvenContourCommitSha = ProvenContourSubjectCommitSha;
//			IsConfigured = true;
//		}
//
//		public ulong DomainTag
//		{
//			get;
//		}
//		public System.Nullable<string> ProvenContourCommitSha
//		{
//			get;
//		}
//		public bool IsConfigured
//		{
//			get;
//		}
//	}

	/// <summary>
	/// Immutable construction-only binding for the exact released read-only
	/// PinBasedControls VMREAD profile. These identifiers bind reviewed
	/// compatibility decisions; they are evidence, never runtime authority.
	/// </summary>
// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct PinBasedControlsVmReadProductionConstructionProfile
//	{
//		public const string FrozenAbiDecisionId = "ABI-HV-VMX8-PIN-BASED-CONTROLS-0001";
//		public const string ProjectionDecisionId = "D2-HV-VMREAD-PIN-BASED-CONTROLS-0005";
//		public const string ScalarDeliveryDecisionId = "D2-HV-VMREAD-PIN-BASED-CONTROLS-SCALAR-0006";
//		public const string ReviewedImplementationSubjectSha = "bcb10489762c4a560813be8efac299408d8f2906";
//		public const string ReviewedImplementationSubjectTree = "e5db97f9dcee4cab0d6114fcc85a00f305339011";
//
//		public PinBasedControlsVmReadProductionConstructionProfile(ulong domainIdentity, string frozenAbiDecisionId, string projectionDecisionId, string scalarDeliveryDecisionId, string reviewedImplementationSubjectSha, string reviewedImplementationSubjectTree)
//		{
//			if (domainIdentity == 0)
//				throw new ArgumentOutOfRangeException(nameof(domainIdentity));
//			RequireExact(frozenAbiDecisionId, FrozenAbiDecisionId, nameof(frozenAbiDecisionId));
//			RequireExact(projectionDecisionId, ProjectionDecisionId, nameof(projectionDecisionId));
//			RequireExact(scalarDeliveryDecisionId, ScalarDeliveryDecisionId, nameof(scalarDeliveryDecisionId));
//			RequireExact(reviewedImplementationSubjectSha, ReviewedImplementationSubjectSha, nameof(reviewedImplementationSubjectSha));
//			RequireExact(reviewedImplementationSubjectTree, ReviewedImplementationSubjectTree, nameof(reviewedImplementationSubjectTree));
//
//			DomainIdentity = domainIdentity;
//			IsConfigured = true;
//		}
//
//		public ulong DomainIdentity
//		{
//			get;
//		}
//		public bool IsConfigured
//		{
//			get;
//		}
//		public bool RuntimeAuthorityGranted
//		{
//			get
//			{
//				return false;
//			}
//		}
//
//		public static PinBasedControlsVmReadProductionConstructionProfile CreateExact(ulong domainIdentity)
//		{
//			return new(domainIdentity, FrozenAbiDecisionId, ProjectionDecisionId, ScalarDeliveryDecisionId, ReviewedImplementationSubjectSha, ReviewedImplementationSubjectTree);
//		}
//
//		private static void RequireExact(string actual, string expected, string parameterName)
//		{
//			if (!string.Equals(actual, expected, StringComparison.Ordinal))
//				throw new ArgumentException("Exact PinBasedControls release identity mismatch.", parameterName);
//		}
//	}

	/// <summary>
	/// Explicit runtime dependencies for a live CPU core instance.
	/// New production core construction should go through this context instead
	/// of implicitly reading mutable global Processor state.
	/// </summary>
// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct CpuCorePlatformContext
//	{
//		private readonly System.Nullable<Func<Processor.MainMemoryArea, IAtomicMemoryUnit>> _atomicMemoryUnitFactory;
//
//		public CpuCorePlatformContext(Processor.MainMemoryArea mainMemory, ProcessorMode initialExecutionMode, bool trackGlobalExecutionMode = false, System.Nullable<Func<Processor.MainMemoryArea, IAtomicMemoryUnit>> atomicMemoryUnitFactory = nullptr, System.Nullable<CpuInstructionTranslationPolicy> cpuInstructionTranslationPolicy = nullptr, bool enableCompletionReasonQualificationVmRead = false, bool enableCompletionSecondStageVmRead = false, System.Nullable<CpuExactProbeRuntimeConstructionProfile> exactProbeRuntimeProfile = nullptr, System.Nullable<CompatibilityControlPolicyConstructionProfile> compatibilityControlPolicyProfile = nullptr, System.Nullable<PinBasedControlsVmReadProductionConstructionProfile> pinBasedControlsVmReadProfile = nullptr, System.Nullable<IHybridCpuIseEcallBridgeV1> ecallBridge = nullptr)
//		{
//			MainMemory = mainMemory ?? throw new ArgumentNullException(nameof(mainMemory));
//			InitialExecutionMode = initialExecutionMode;
//			TrackGlobalExecutionMode = trackGlobalExecutionMode;
//			_atomicMemoryUnitFactory = atomicMemoryUnitFactory;
//			CpuInstructionTranslationPolicy = cpuInstructionTranslationPolicy ?? CpuInstructionTranslationPolicy.Identity;
//			EnableCompletionReasonQualificationVmRead = enableCompletionReasonQualificationVmRead;
//			EnableCompletionSecondStageVmRead = enableCompletionSecondStageVmRead;
//			ExactProbeRuntimeProfile = exactProbeRuntimeProfile;
//			CompatibilityControlPolicyProfile = compatibilityControlPolicyProfile;
//			PinBasedControlsVmReadProfile = pinBasedControlsVmReadProfile;
//			EcallBridge = ecallBridge;
//			if (pinBasedControlsVmReadProfile is { IsConfigured: true } exactPinBasedProfile)
//			{
//				if (compatibilityControlPolicyProfile is @not { IsConfigured: true } policyProfile || policyProfile.DomainIdentity != exactPinBasedProfile.DomainIdentity)
//				{
//					throw new ArgumentException("Exact PinBasedControls release construction requires the same canonical compatibility-control policy domain.", nameof(pinBasedControlsVmReadProfile));
//				}
//				EnablePinBasedControlsVmRead = true;
//			}
//			IsConfigured = true;
//		}
//
//		public Processor.MainMemoryArea MainMemory
//		{
//			get;
//		}
//		/// <summary>Construction-bound controller; null selects the original main-memory fallback.</summary>
//		public System.Nullable<YAKSys_Hybrid_CPU.Memory.MemorySubsystem> MemorySubsystem
//		{
//			get;
//			private init;
//		}
//
//		/// <summary>Returns a new construction request; never rebinds an existing core.</summary>
//		public CpuCorePlatformContext WithMemorySubsystem(System.Nullable<YAKSys_Hybrid_CPU.Memory.MemorySubsystem> memorySubsystem)
//		{
//			return this with { MemorySubsystem = memorySubsystem };
//		}
//		internal System.Nullable<MicroOpScheduler> OwnedSoftwareSecureScheduler
//		{
//			get;
//			private init;
//		}
//
//		/// <summary>Private software-model placement scope; grants no semantic or issue authority.</summary>
//		public static CpuCorePlatformContext CreateSoftwareSecureFixed(Processor.MainMemoryArea memory)
//		{
//			return CreateSoftwareSecureFixed(memory, ProcessorMode.Emulation, nullptr);
//		}
//
//		public static CpuCorePlatformContext CreateSoftwareSecureFixed(Processor.MainMemoryArea memory, ProcessorMode executionMode, System.Nullable<IHybridCpuIseEcallBridgeV1> ecallBridge)
//		{
//			return new(memory, executionMode, ecallBridge: ecallBridge)
//		}
//			{
//				OwnedSoftwareSecureScheduler = new MicroOpScheduler { TypedSlotEnabled = true }
//			};
//
//		public ProcessorMode InitialExecutionMode
//		{
//			get;
//		}
//
//		public bool TrackGlobalExecutionMode
//		{
//			get;
//		}
//
//		public bool IsConfigured
//		{
//			get;
//		}
//
//		public CpuInstructionTranslationPolicy CpuInstructionTranslationPolicy
//		{
//			get;
//		}
//		public bool EnableCompletionReasonQualificationVmRead
//		{
//			get;
//		}
//		public bool EnableCompletionSecondStageVmRead
//		{
//			get;
//		}
//		public System.Nullable<CpuExactProbeRuntimeConstructionProfile> ExactProbeRuntimeProfile
//		{
//			get;
//		}
//		public System.Nullable<CompatibilityControlPolicyConstructionProfile> CompatibilityControlPolicyProfile
//		{
//			get;
//		}
//		public System.Nullable<PinBasedControlsVmReadProductionConstructionProfile> PinBasedControlsVmReadProfile
//		{
//			get;
//		}
//		public bool EnablePinBasedControlsVmRead
//		{
//			get;
//		}
//		public System.Nullable<IHybridCpuIseEcallBridgeV1> EcallBridge
//		{
//			get;
//		}
//
//		public ProcessorMode ResolveExecutionMode()
//		{
//			return TrackGlobalExecutionMode ? Processor.CurrentProcessorMode : InitialExecutionMode;
//		}
//
//		public IAtomicMemoryUnit CreateAtomicMemoryUnit()
//		{
//			System.Nullable<Func<Processor.MainMemoryArea, IAtomicMemoryUnit>> atomicMemoryUnitFactory = _atomicMemoryUnitFactory;
//			if (atomicMemoryUnitFactory != nullptr)
//			{
//				return atomicMemoryUnitFactory(MainMemory);
//			}
//
//			return new MainMemoryAtomicMemoryUnit(MainMemory);
//		}
//
//		public static CpuCorePlatformContext CreateFixed(Processor.MainMemoryArea mainMemory, ProcessorMode executionMode, System.Nullable<Func<Processor.MainMemoryArea, IAtomicMemoryUnit>> atomicMemoryUnitFactory = nullptr, System.Nullable<CpuInstructionTranslationPolicy> cpuInstructionTranslationPolicy = nullptr, bool enableCompletionReasonQualificationVmRead = false, bool enableCompletionSecondStageVmRead = false, System.Nullable<CpuExactProbeRuntimeConstructionProfile> exactProbeRuntimeProfile = nullptr, System.Nullable<CompatibilityControlPolicyConstructionProfile> compatibilityControlPolicyProfile = nullptr, System.Nullable<PinBasedControlsVmReadProductionConstructionProfile> pinBasedControlsVmReadProfile = nullptr, System.Nullable<IHybridCpuIseEcallBridgeV1> ecallBridge = nullptr)
//		{
//			return new(mainMemory, executionMode, trackGlobalExecutionMode: false, atomicMemoryUnitFactory: atomicMemoryUnitFactory, cpuInstructionTranslationPolicy: cpuInstructionTranslationPolicy, enableCompletionReasonQualificationVmRead: enableCompletionReasonQualificationVmRead, enableCompletionSecondStageVmRead: enableCompletionSecondStageVmRead, exactProbeRuntimeProfile: exactProbeRuntimeProfile, compatibilityControlPolicyProfile: compatibilityControlPolicyProfile, pinBasedControlsVmReadProfile: pinBasedControlsVmReadProfile, ecallBridge: ecallBridge);
//		}
//
//		public static CpuCorePlatformContext CreateLegacy(System.Nullable<Processor.MainMemoryArea> mainMemory = nullptr, System.Nullable<Func<Processor.MainMemoryArea, IAtomicMemoryUnit>> atomicMemoryUnitFactory = nullptr, System.Nullable<CpuInstructionTranslationPolicy> cpuInstructionTranslationPolicy = nullptr, bool enableCompletionReasonQualificationVmRead = false, bool enableCompletionSecondStageVmRead = false, System.Nullable<CpuExactProbeRuntimeConstructionProfile> exactProbeRuntimeProfile = nullptr, System.Nullable<CompatibilityControlPolicyConstructionProfile> compatibilityControlPolicyProfile = nullptr, System.Nullable<PinBasedControlsVmReadProductionConstructionProfile> pinBasedControlsVmReadProfile = nullptr, System.Nullable<IHybridCpuIseEcallBridgeV1> ecallBridge = nullptr)
//		{
//			return new(mainMemory ?? Processor.MainMemory, Processor.CurrentProcessorMode, trackGlobalExecutionMode: true, atomicMemoryUnitFactory: atomicMemoryUnitFactory, cpuInstructionTranslationPolicy: cpuInstructionTranslationPolicy, enableCompletionReasonQualificationVmRead: enableCompletionReasonQualificationVmRead, enableCompletionSecondStageVmRead: enableCompletionSecondStageVmRead, exactProbeRuntimeProfile: exactProbeRuntimeProfile, compatibilityControlPolicyProfile: compatibilityControlPolicyProfile, pinBasedControlsVmReadProfile: pinBasedControlsVmReadProfile, ecallBridge: ecallBridge)
//		}
//			{
//				MemorySubsystem = Processor.Memory
//			};
//	}
}
