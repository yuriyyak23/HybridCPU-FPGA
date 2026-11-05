#pragma once

#include "../../../../../System/Vmcs/V2/VmcsV2BlockDirectory.h"
#include "../../../../../System/Vmcs/V2/VmcsV2HostEvidenceKind.h"
#include "../../../FrozenAbi/VmcsFieldAliases/VmExitReason.h"
#include <string>
#include <vector>
#include <memory>
#include <algorithm>

using namespace YAKSys_Hybrid_CPU::Core::Vmcs::V2;

namespace YAKSys_Hybrid_CPU::Core::Nested
{

	class ChildDomainIntentFieldIds final
	{
	public:
		static constexpr unsigned short GuestProgramCounter = 0;
		static constexpr unsigned short GuestStackPointer = 1;
		static constexpr unsigned short GuestFlags = 2;
		static constexpr unsigned short GuestControl0 = 3;
		static constexpr unsigned short GuestPageTableRoot = 4;
		static constexpr unsigned short GuestControl4 = 5;
		static constexpr unsigned short HostProgramCounter = 32;
		static constexpr unsigned short HostStackPointer = 33;
		static constexpr unsigned short HostFlags = 34;
		static constexpr unsigned short HostControl0 = 35;
		static constexpr unsigned short HostPageTableRoot = 36;
		static constexpr unsigned short PinExecutionControls = 64;
		static constexpr unsigned short ProcessorExecutionControls = 65;
		static constexpr unsigned short ExitControls = 66;
		static constexpr unsigned short EntryControls = 67;
		static constexpr unsigned short SecondStageRootPointer = 80;
		static constexpr unsigned short AddressSpaceTag = 81;
		static constexpr unsigned short SecondaryExecutionControls = 82;
		static constexpr unsigned short GuestPageTableTargetCount = 83;
		static constexpr unsigned short ExitReason = 96;
		static constexpr unsigned short ExitQualification = 97;
		static constexpr unsigned short GuestPhysicalAddress = 112;
		static constexpr unsigned short SecondStageViolationQualification = 113;
	};

	enum class ChildDomainIntentAccessDisposition : unsigned char
	{
		Allowed = 0,
		VmFail = 1,
		VmExitToL0 = 2
	};

	struct ChildDomainIntentAccessResult final
	{
		const bool Succeeded;
		const ChildDomainIntentAccessDisposition Disposition;
		const unsigned short FieldId;
		const long long Value;
		const VmExitReason ExitReason;
		const std::wstring Message;
		ChildDomainIntentAccessResult(bool succeeded, ChildDomainIntentAccessDisposition disposition, unsigned short fieldId, long long value, VmExitReason exitReason, std::wstring message)
			: Succeeded(succeeded), Disposition(disposition), FieldId(fieldId), Value(value), ExitReason(exitReason), Message(std::move(message)) {}
		static std::shared_ptr<ChildDomainIntentAccessResult> VmFail(unsigned short fieldId, const std::wstring &message) { return std::make_shared<ChildDomainIntentAccessResult>(false, ChildDomainIntentAccessDisposition::VmFail, fieldId, 0, VmExitReason::None, message); }
		static std::shared_ptr<ChildDomainIntentAccessResult> L0Exit(unsigned short fieldId, VmExitReason reason, const std::wstring &message) { return std::make_shared<ChildDomainIntentAccessResult>(false, ChildDomainIntentAccessDisposition::VmExitToL0, fieldId, 0, reason, message); }
	};

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct ChildDomainIntentAccessResult(bool Succeeded, ChildDomainIntentAccessDisposition Disposition, ushort FieldId, long Value, VmExitReason ExitReason, string Message)
//	{
//		public static ChildDomainIntentAccessResult Allowed(ushort fieldId, long value)
//		{
//			return new(true, ChildDomainIntentAccessDisposition.Allowed, fieldId, value, VmExitReason.None, string.Empty);
//		}
//
//		public static ChildDomainIntentAccessResult VmFail(ushort fieldId, string message)
//		{
//			return new(false, ChildDomainIntentAccessDisposition.VmFail, fieldId, 0, VmExitReason.None, message);
//		}
//
//		public static ChildDomainIntentAccessResult L0Exit(ushort fieldId, VmExitReason reason, string message)
//		{
//			return new(false, ChildDomainIntentAccessDisposition.VmExitToL0, fieldId, 0, reason, message);
//		}
//	}

	class ChildDomainIntentAccessPolicy final : public std::enable_shared_from_this<ChildDomainIntentAccessPolicy>
	{
	private:
		static inline const std::vector<unsigned short> DefaultL1VisibleFields = {ChildDomainIntentFieldIds::GuestProgramCounter, ChildDomainIntentFieldIds::GuestStackPointer, ChildDomainIntentFieldIds::GuestFlags, ChildDomainIntentFieldIds::GuestControl0, ChildDomainIntentFieldIds::GuestPageTableRoot, ChildDomainIntentFieldIds::GuestControl4, ChildDomainIntentFieldIds::HostProgramCounter, ChildDomainIntentFieldIds::HostStackPointer, ChildDomainIntentFieldIds::HostFlags, ChildDomainIntentFieldIds::HostControl0, ChildDomainIntentFieldIds::HostPageTableRoot, ChildDomainIntentFieldIds::PinExecutionControls, ChildDomainIntentFieldIds::ProcessorExecutionControls, ChildDomainIntentFieldIds::ExitControls, ChildDomainIntentFieldIds::EntryControls, ChildDomainIntentFieldIds::SecondStageRootPointer, ChildDomainIntentFieldIds::AddressSpaceTag, ChildDomainIntentFieldIds::SecondaryExecutionControls, ChildDomainIntentFieldIds::GuestPageTableTargetCount, ChildDomainIntentFieldIds::ExitReason, ChildDomainIntentFieldIds::ExitQualification, ChildDomainIntentFieldIds::GuestPhysicalAddress, ChildDomainIntentFieldIds::SecondStageViolationQualification};

		const std::vector<unsigned short> _readAllow;
		const std::vector<unsigned short> _writeAllow;
		const std::vector<unsigned short> _readDeny;
		const std::vector<unsigned short> _writeDeny;
		const std::vector<unsigned short> _readExit;
		const std::vector<unsigned short> _writeExit;

	public:
		ChildDomainIntentAccessPolicy();

	private:
		ChildDomainIntentAccessPolicy(std::vector<unsigned short> readAllow, std::vector<unsigned short> writeAllow, std::vector<unsigned short> readDeny, std::vector<unsigned short> writeDeny, std::vector<unsigned short> readExit, std::vector<unsigned short> writeExit);

	public:
		static std::shared_ptr<ChildDomainIntentAccessPolicy> DefaultNestedL1Visible();

		ChildDomainIntentAccessDisposition EvaluateRead(unsigned short fieldId);

		ChildDomainIntentAccessDisposition EvaluateWrite(unsigned short fieldId);

	private:
		static ChildDomainIntentAccessDisposition Evaluate(unsigned short fieldId, const std::vector<unsigned short> &allow, const std::vector<unsigned short> &deny, const std::vector<unsigned short> &exit);
	};

	class ChildDomainIntentDescriptor final : public std::enable_shared_from_this<ChildDomainIntentDescriptor>
	{
	private:
		const unsigned long long ChildIntentPointer;
		const std::shared_ptr<ChildDomainIntentAccessPolicy> AccessBitmaps;

	public:
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public ChildDomainIntentDescriptor(ulong childIntentPointer, ChildDomainIntentAccessPolicy? bitmaps = null)
		ChildDomainIntentDescriptor(unsigned long long childIntentPointer, std::shared_ptr<ChildDomainIntentAccessPolicy> bitmaps = nullptr);

		unsigned short getVersion() const;

		unsigned long long getChildIntentPointer() const;

		std::shared_ptr<ChildDomainIntentAccessPolicy> getAccessBitmaps() const;

		bool isReadOnlyCompatibilityProjection() const;

		bool TryReadIntentField(std::shared_ptr<VmcsV2BlockDirectory> directory, unsigned short fieldId, long long &value, std::shared_ptr<ChildDomainIntentAccessResult> &result);

		bool ContainsHostEvidence(VmcsV2HostEvidenceKind evidence);

	private:
		static bool IsL1VisibleField(std::shared_ptr<VmcsV2BlockDirectory> directory, unsigned short fieldId, std::wstring &message);
	};

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public sealed record ChildDomainIntentSnapshot(ushort Version, ulong ChildIntentPointer);

}
