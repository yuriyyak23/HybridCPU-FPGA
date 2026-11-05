#include "ChildDomainIntentDescriptor.h"
#include "../../../FrozenAbi/VmcsFieldAliases/VmExitReason.h"
#include "../../../../../System/Vmcs/V2/VmcsV2FieldModel.h"
#include <stdexcept>

using namespace YAKSys_Hybrid_CPU::Core::Vmcs::V2;

namespace YAKSys_Hybrid_CPU::Core::Nested
{

	ChildDomainIntentAccessPolicy::ChildDomainIntentAccessPolicy() : ChildDomainIntentAccessPolicy({}, {}, {}, {}, {}, {})
	{
	}

	ChildDomainIntentAccessPolicy::ChildDomainIntentAccessPolicy(std::vector<unsigned short> readAllow, std::vector<unsigned short> writeAllow, std::vector<unsigned short> readDeny, std::vector<unsigned short> writeDeny, std::vector<unsigned short> readExit, std::vector<unsigned short> writeExit) : _readAllow(std::move(readAllow)), _writeAllow(std::move(writeAllow)), _readDeny(std::move(readDeny)), _writeDeny(std::move(writeDeny)), _readExit(std::move(readExit)), _writeExit(std::move(writeExit))
	{
	}

	std::shared_ptr<ChildDomainIntentAccessPolicy> ChildDomainIntentAccessPolicy::DefaultNestedL1Visible()
	{
		return std::shared_ptr<ChildDomainIntentAccessPolicy>(new ChildDomainIntentAccessPolicy(DefaultL1VisibleFields, DefaultL1VisibleFields, {}, {}, {}, {}));
	}

	ChildDomainIntentAccessDisposition ChildDomainIntentAccessPolicy::EvaluateRead(unsigned short fieldId)
	{
		return Evaluate(fieldId, _readAllow, _readDeny, _readExit);
	}

	ChildDomainIntentAccessDisposition ChildDomainIntentAccessPolicy::EvaluateWrite(unsigned short fieldId)
	{
		return Evaluate(fieldId, _writeAllow, _writeDeny, _writeExit);
	}

	ChildDomainIntentAccessDisposition ChildDomainIntentAccessPolicy::Evaluate(unsigned short fieldId, const std::vector<unsigned short> &allow, const std::vector<unsigned short> &deny, const std::vector<unsigned short> &exit)
	{
		if (std::find(exit.begin(), exit.end(), fieldId) != exit.end())
		{
			return ChildDomainIntentAccessDisposition::VmExitToL0;
		}

		if (std::find(deny.begin(), deny.end(), fieldId) != deny.end() || std::find(allow.begin(), allow.end(), fieldId) == allow.end())
		{
			return ChildDomainIntentAccessDisposition::VmFail;
		}

		return ChildDomainIntentAccessDisposition::Allowed;
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public ChildDomainIntentDescriptor(ulong childIntentPointer, ChildDomainIntentAccessPolicy? bitmaps = null)
	ChildDomainIntentDescriptor::ChildDomainIntentDescriptor(unsigned long long childIntentPointer, std::shared_ptr<ChildDomainIntentAccessPolicy> bitmaps)
		: ChildIntentPointer(childIntentPointer), AccessBitmaps(bitmaps != nullptr ? std::move(bitmaps) : ChildDomainIntentAccessPolicy::DefaultNestedL1Visible())
	{
	}

	unsigned short ChildDomainIntentDescriptor::getVersion() const
	{
		return 1;
	}

	unsigned long long ChildDomainIntentDescriptor::getChildIntentPointer() const
	{
		return ChildIntentPointer;
	}

	std::shared_ptr<ChildDomainIntentAccessPolicy> ChildDomainIntentDescriptor::getAccessBitmaps() const
	{
		return AccessBitmaps;
	}

	bool ChildDomainIntentDescriptor::isReadOnlyCompatibilityProjection() const
	{
		return true;
	}

	bool ChildDomainIntentDescriptor::TryReadIntentField(std::shared_ptr<VmcsV2BlockDirectory> directory, unsigned short fieldId, long long &value, std::shared_ptr<ChildDomainIntentAccessResult> &result)
	{
		value = 0;
		ChildDomainIntentAccessDisposition disposition = getAccessBitmaps()->EvaluateRead(fieldId);
		if (disposition == ChildDomainIntentAccessDisposition::VmExitToL0)
		{
			result = ChildDomainIntentAccessResult::L0Exit(fieldId, VmExitReason::SecurityPolicyViolation, L"Child-domain intent field read is configured to exit to L0.");
			return false;
		}

		if (disposition == ChildDomainIntentAccessDisposition::VmFail)
		{
			result = ChildDomainIntentAccessResult::VmFail(fieldId, L"Child-domain intent field read is denied by the nested access bitmap.");
			return false;
		}

		std::wstring message;
		if (!IsL1VisibleField(directory, fieldId, message))
		{
			result = ChildDomainIntentAccessResult::VmFail(fieldId, message);
			return false;
		}

		result = ChildDomainIntentAccessResult::VmFail(fieldId, L"Child-domain intent field read requires neutral runtime-owned nested intent state.");
		return false;
	}

	bool ChildDomainIntentDescriptor::ContainsHostEvidence(VmcsV2HostEvidenceKind evidence)
	{
		return false;
	}

	bool ChildDomainIntentDescriptor::IsL1VisibleField(std::shared_ptr<VmcsV2BlockDirectory> directory, unsigned short fieldId, std::wstring &message)
	{
		if (directory == nullptr)
		{
			throw std::invalid_argument("VMCSv2 field directory cannot be null.");
		}
		std::shared_ptr<VmcsV2FieldDescriptor> descriptor;
		if (!directory->TryGetField(fieldId, descriptor))
		{
			message = L"Child-domain intent field is not present in the VMCSv2 field directory.";
			return false;
		}

		if (descriptor->AccessPolicy.HostRuntimeEvidence || descriptor->ValueType == VmcsV2FieldValueType::DescriptorReference)
		{
			message = L"Host-owned VMCSv2 descriptor/evidence fields are not visible in child-domain intent projection.";
			return false;
		}

		message = L"";
		return true;
	}
}
