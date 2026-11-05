#pragma once
#include <memory>
#include <string>
#include <utility>

namespace YAKSys_Hybrid_CPU::Core::Vmcs::V2
{

	enum class VmcsV2BlockKind : unsigned char
	{
		VmxRootControl = 0,
		VirtualCpu = 1,
		BundleExecution = 2,
		SchedulingBudgetTimer = 3,
		VirtualInterruptFabric = 4,
		VectorStreamState = 5,
		SecurityIsolation = 6,
		CapabilityNegotiation = 7,
		NestedTranslation = 8,
		ExitInformation = 9,
		InterceptBitmap = 10,
		EventInjection = 11,
		LaneCompletionRouting = 12,
		IoVirt = 13,
		Lane6State = 14,
		Lane7State = 15,
		ShadowVmcs = 16,
		DirtyLog = 17,
		DebugTrace = 18
	};

	enum class VmcsV2FieldValueType : unsigned char
	{
		UInt64 = 0,
		Boolean = 1,
		Epoch = 2,
		GuestIntegerRegisterFile = 3,
		DescriptorReference = 4
	};

	enum class VmcsV2FieldAccessKind : unsigned char
	{
		GuestArchitecturalState = 0,
		HostArchitecturalState = 1,
		RootOwnedControl = 2,
		ExitInformation = 3,
		HostRuntimeEvidence = 4
	};

	enum class VmcsV2ValidationCode : unsigned char
	{
		Success = 0,
		UnknownField = 1,
		AccessDenied = 2,
		TypeMismatch = 3,
		ReservedBitsSet = 4,
		GuestGprPersistenceIncomplete = 5,
		HostEvidenceNotGuestVisible = 6,
		VectorStreamStateIncomplete = 7,
		Lane7StateIncomplete = 8,
		NestedVmxDisabled = 9,
		NestedCapabilityMissing = 10,
		NestedPolicyGateFailed = 11,
		InvalidVmcs12 = 12,
		DirtyLogDisabled = 13,
		DirtyLogOverflow = 14,
		CheckpointFormatMismatch = 15,
		CheckpointCapabilityMismatch = 16,
		CheckpointSecurityPolicyMismatch = 17,
		CheckpointGenerationMismatch = 18
	};

	enum class VmcsV2ValidationPolicyKind : unsigned char
	{
		None = 0,
		Boolean = 1,
		ReservedMustBeZero = 2,
		GuestGprPersistenceRequired = 3,
		HostEvidenceForbidden = 4
	};

	enum class VmcsV2MigrationPolicyKind : unsigned char
	{
		MigratableGuestState = 0,
		RootLocalState = 1,
		ReconstructableRuntimeState = 2,
		HostOnlyEvidence = 3
	};

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct VmcsV2AccessPolicy(VmcsV2FieldAccessKind Kind, bool CanReadViaVmRead, bool CanWriteViaVmWrite, bool GuestVisible, bool HostRuntimeEvidence)
//	{
//		public static VmcsV2AccessPolicy GuestState
//		{
//			get;
//		} = new(VmcsV2FieldAccessKind.GuestArchitecturalState, true, true, true, false);
//
//		public static VmcsV2AccessPolicy HostState
//		{
//			get;
//		} = new(VmcsV2FieldAccessKind.HostArchitecturalState, true, true, false, false);
//
//		public static VmcsV2AccessPolicy RootControl
//		{
//			get;
//		} = new(VmcsV2FieldAccessKind.RootOwnedControl, true, true, false, false);
//
//		public static VmcsV2AccessPolicy ExitInfo
//		{
//			get;
//		} = new(VmcsV2FieldAccessKind.ExitInformation, true, true, true, false);
//
//		public static VmcsV2AccessPolicy HostEvidence
//		{
//			get;
//		} = new(VmcsV2FieldAccessKind.HostRuntimeEvidence, false, false, false, true);
//	}
	struct VmcsV2AccessPolicy final
	{
		VmcsV2FieldAccessKind Kind; bool CanReadViaVmRead; bool CanWriteViaVmWrite; bool GuestVisible; bool HostRuntimeEvidence;
		VmcsV2AccessPolicy(VmcsV2FieldAccessKind kind, bool read, bool write, bool guest, bool host) : Kind(kind), CanReadViaVmRead(read), CanWriteViaVmWrite(write), GuestVisible(guest), HostRuntimeEvidence(host) {}
		static const std::shared_ptr<VmcsV2AccessPolicy> GuestState, HostState, RootControl, ExitInfo, HostEvidence;
	};
	inline const std::shared_ptr<VmcsV2AccessPolicy> VmcsV2AccessPolicy::GuestState = std::make_shared<VmcsV2AccessPolicy>(VmcsV2FieldAccessKind::GuestArchitecturalState, true, true, true, false);
	inline const std::shared_ptr<VmcsV2AccessPolicy> VmcsV2AccessPolicy::HostState = std::make_shared<VmcsV2AccessPolicy>(VmcsV2FieldAccessKind::HostArchitecturalState, true, true, false, false);
	inline const std::shared_ptr<VmcsV2AccessPolicy> VmcsV2AccessPolicy::RootControl = std::make_shared<VmcsV2AccessPolicy>(VmcsV2FieldAccessKind::RootOwnedControl, true, true, false, false);
	inline const std::shared_ptr<VmcsV2AccessPolicy> VmcsV2AccessPolicy::ExitInfo = std::make_shared<VmcsV2AccessPolicy>(VmcsV2FieldAccessKind::ExitInformation, true, true, true, false);
	inline const std::shared_ptr<VmcsV2AccessPolicy> VmcsV2AccessPolicy::HostEvidence = std::make_shared<VmcsV2AccessPolicy>(VmcsV2FieldAccessKind::HostRuntimeEvidence, false, false, false, true);

	struct VmcsV2ValidationPolicy final
	{
		VmcsV2ValidationPolicyKind Kind; unsigned long long ReservedMask;
		VmcsV2ValidationPolicy(VmcsV2ValidationPolicyKind kind, unsigned long long mask = 0) : Kind(kind), ReservedMask(mask) {}
		static const std::shared_ptr<VmcsV2ValidationPolicy> None, Boolean, GuestGprPersistenceRequired, HostEvidenceForbidden;
	};
	inline const std::shared_ptr<VmcsV2ValidationPolicy> VmcsV2ValidationPolicy::None = std::make_shared<VmcsV2ValidationPolicy>(VmcsV2ValidationPolicyKind::None);
	inline const std::shared_ptr<VmcsV2ValidationPolicy> VmcsV2ValidationPolicy::Boolean = std::make_shared<VmcsV2ValidationPolicy>(VmcsV2ValidationPolicyKind::Boolean);
	inline const std::shared_ptr<VmcsV2ValidationPolicy> VmcsV2ValidationPolicy::GuestGprPersistenceRequired = std::make_shared<VmcsV2ValidationPolicy>(VmcsV2ValidationPolicyKind::GuestGprPersistenceRequired);
	inline const std::shared_ptr<VmcsV2ValidationPolicy> VmcsV2ValidationPolicy::HostEvidenceForbidden = std::make_shared<VmcsV2ValidationPolicy>(VmcsV2ValidationPolicyKind::HostEvidenceForbidden);

	struct VmcsV2MigrationPolicy final
	{
		VmcsV2MigrationPolicyKind Kind; bool RequiresMaterializedGuestState;
		VmcsV2MigrationPolicy(VmcsV2MigrationPolicyKind kind, bool materialized) : Kind(kind), RequiresMaterializedGuestState(materialized) {}
		static const std::shared_ptr<VmcsV2MigrationPolicy> MigratableGuestState, RootLocalState, ReconstructableRuntimeState, HostOnlyEvidence;
	};
	inline const std::shared_ptr<VmcsV2MigrationPolicy> VmcsV2MigrationPolicy::MigratableGuestState = std::make_shared<VmcsV2MigrationPolicy>(VmcsV2MigrationPolicyKind::MigratableGuestState, true);
	inline const std::shared_ptr<VmcsV2MigrationPolicy> VmcsV2MigrationPolicy::RootLocalState = std::make_shared<VmcsV2MigrationPolicy>(VmcsV2MigrationPolicyKind::RootLocalState, false);
	inline const std::shared_ptr<VmcsV2MigrationPolicy> VmcsV2MigrationPolicy::ReconstructableRuntimeState = std::make_shared<VmcsV2MigrationPolicy>(VmcsV2MigrationPolicyKind::ReconstructableRuntimeState, false);
	inline const std::shared_ptr<VmcsV2MigrationPolicy> VmcsV2MigrationPolicy::HostOnlyEvidence = std::make_shared<VmcsV2MigrationPolicy>(VmcsV2MigrationPolicyKind::HostOnlyEvidence, false);

	struct VmcsV2FieldDescriptor final
	{
		unsigned short FieldId; std::wstring Name; VmcsV2BlockKind Block; VmcsV2FieldValueType ValueType;
		VmcsV2AccessPolicy AccessPolicy; VmcsV2ValidationPolicy ValidationPolicy; VmcsV2MigrationPolicy MigrationPolicy; unsigned long long InvalidationEpoch;
		VmcsV2FieldDescriptor(unsigned short id, std::wstring name, VmcsV2BlockKind block, VmcsV2FieldValueType valueType, std::shared_ptr<VmcsV2AccessPolicy> access, std::shared_ptr<VmcsV2ValidationPolicy> validation, std::shared_ptr<VmcsV2MigrationPolicy> migration, unsigned long long epoch)
			: FieldId(id), Name(std::move(name)), Block(block), ValueType(valueType), AccessPolicy(*access), ValidationPolicy(*validation), MigrationPolicy(*migration), InvalidationEpoch(epoch) {}
		bool IsVmReadVisible() const { return AccessPolicy.CanReadViaVmRead; }
		bool IsVmWriteVisible() const { return AccessPolicy.CanWriteViaVmWrite; }
	};

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct VmcsV2ValidationPolicy(VmcsV2ValidationPolicyKind Kind, ulong ReservedMask = 0)
//	{
//		public static VmcsV2ValidationPolicy None
//		{
//			get;
//		} = new(VmcsV2ValidationPolicyKind.None);
//
//		public static VmcsV2ValidationPolicy Boolean
//		{
//			get;
//		} = new(VmcsV2ValidationPolicyKind.Boolean);
//
//		public static VmcsV2ValidationPolicy GuestGprPersistenceRequired
//		{
//			get;
//		} = new(VmcsV2ValidationPolicyKind.GuestGprPersistenceRequired);
//
//		public static VmcsV2ValidationPolicy HostEvidenceForbidden
//		{
//			get;
//		} = new(VmcsV2ValidationPolicyKind.HostEvidenceForbidden);
//	}

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct VmcsV2MigrationPolicy(VmcsV2MigrationPolicyKind Kind, bool RequiresMaterializedGuestState)
//	{
//		public static VmcsV2MigrationPolicy MigratableGuestState
//		{
//			get;
//		} = new(VmcsV2MigrationPolicyKind.MigratableGuestState, true);
//
//		public static VmcsV2MigrationPolicy RootLocalState
//		{
//			get;
//		} = new(VmcsV2MigrationPolicyKind.RootLocalState, false);
//
//		public static VmcsV2MigrationPolicy ReconstructableRuntimeState
//		{
//			get;
//		} = new(VmcsV2MigrationPolicyKind.ReconstructableRuntimeState, false);
//
//		public static VmcsV2MigrationPolicy HostOnlyEvidence
//		{
//			get;
//		} = new(VmcsV2MigrationPolicyKind.HostOnlyEvidence, false);
//	}

struct VmcsV2ValidationResult {
    const bool Succeeded; const VmcsV2ValidationCode Code; const unsigned short FieldId; const std::wstring Message;
    VmcsV2ValidationResult(bool succeeded, VmcsV2ValidationCode code, unsigned short fieldId, std::wstring message) : Succeeded(succeeded), Code(code), FieldId(fieldId), Message(std::move(message)) {}
    static std::shared_ptr<VmcsV2ValidationResult> Success(unsigned short fieldId = 0) { return std::make_shared<VmcsV2ValidationResult>(true, VmcsV2ValidationCode::Success, fieldId, L""); }
    static std::shared_ptr<VmcsV2ValidationResult> Fail(VmcsV2ValidationCode code, unsigned short fieldId, std::wstring message) { return std::make_shared<VmcsV2ValidationResult>(false, code, fieldId, std::move(message)); }
};

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct VmcsV2FieldDescriptor(ushort FieldId, string Name, VmcsV2BlockKind Block, VmcsV2FieldValueType ValueType, VmcsV2AccessPolicy AccessPolicy, VmcsV2ValidationPolicy ValidationPolicy, VmcsV2MigrationPolicy MigrationPolicy, ulong InvalidationEpoch)
//	{
//		public bool IsVmReadVisible
//		{
//			get
//			{
//				return AccessPolicy.CanReadViaVmRead;
//			}
//		}
//
//		public bool IsVmWriteVisible
//		{
//			get
//			{
//				return AccessPolicy.CanWriteViaVmWrite;
//			}
//		}
//	}

}
