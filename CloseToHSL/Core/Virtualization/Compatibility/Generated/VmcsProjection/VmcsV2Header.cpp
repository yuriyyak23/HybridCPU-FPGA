#include "VmcsV2Header.h"

namespace YAKSys_Hybrid_CPU::Core::Vmcs::V2
{

	VmcsV2Header::VmcsV2Header(unsigned int revisionId, unsigned int sizeBytes, const std::wstring &compatibilityBaseline)
		: RevisionId(revisionId), SizeBytes(sizeBytes), Baseline(compatibilityBaseline)
	{
	}

	unsigned int VmcsV2Header::getRevisionId() const
	{
		return RevisionId;
	}

	unsigned int VmcsV2Header::getSizeBytes() const
	{
		return SizeBytes;
	}

	std::wstring VmcsV2Header::getBaseline() const
	{
		return Baseline;
	}

	bool VmcsV2Header::isReadOnlyCompatibilityProjection() const
	{
		return true;
	}

	bool VmcsV2Header::isLaunched() const
	{
		return false;
	}

	unsigned long long VmcsV2Header::getInvalidationEpoch() const
	{
		return 0;
	}
}
