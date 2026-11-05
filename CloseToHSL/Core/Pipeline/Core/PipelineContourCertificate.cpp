#include "PipelineContourCertificate.h"

#include "../../../../tangible_exception_helper.h"

namespace YAKSys_Hybrid_CPU::Core
{

	PipelineContourCertificate::PipelineContourCertificate(PipelineContourKind kind, PipelineContourOwner owner, PipelineContourVisibilityStage visibilityStage, unsigned long long pc, unsigned char slotMask, bool isPublished)
		: Kind(kind), Owner(owner), VisibilityStage(visibilityStage), Pc(pc), SlotMask(slotMask), IsPublished(isPublished)
	{
	}

	PipelineContourKind PipelineContourCertificate::getKind() const
	{
		return Kind;
	}

	PipelineContourOwner PipelineContourCertificate::getOwner() const
	{
		return Owner;
	}

	PipelineContourVisibilityStage PipelineContourCertificate::getVisibilityStage() const
	{
		return VisibilityStage;
	}

	unsigned long long PipelineContourCertificate::getPc() const
	{
		return Pc;
	}

	unsigned char PipelineContourCertificate::getSlotMask() const
	{
		return SlotMask;
	}

	bool PipelineContourCertificate::isPublished() const
	{
		return IsPublished;
	}

	PipelineContourCertificate PipelineContourCertificate::CreateEmpty(PipelineContourKind kind)
	{
		if (kind == PipelineContourKind::None)
		{
			throw ArgumentOutOfRangeException(L"kind", kind, L"Contour certificate empties must still name the contour kind.");
		}

		return PipelineContourCertificate(kind, PipelineContourOwner::None, PipelineContourVisibilityStage::None, 0, 0, false);
	}

	PipelineContourCertificate PipelineContourCertificate::CreatePublished(PipelineContourKind kind, PipelineContourOwner owner, PipelineContourVisibilityStage visibilityStage, unsigned long long pc, unsigned char slotMask)
	{
		if (kind == PipelineContourKind::None)
		{
			throw ArgumentOutOfRangeException(L"kind", kind, L"Published contour certificates must name the contour kind.");
		}

		if (owner == PipelineContourOwner::None)
		{
			throw ArgumentOutOfRangeException(L"owner", owner, L"Published contour certificates must name an explicit owner.");
		}

		if (visibilityStage == PipelineContourVisibilityStage::None)
		{
			throw ArgumentOutOfRangeException(L"visibilityStage", visibilityStage, L"Published contour certificates must name the publication stage.");
		}

		return PipelineContourCertificate(kind, owner, visibilityStage, pc, slotMask, true);
	}
}
