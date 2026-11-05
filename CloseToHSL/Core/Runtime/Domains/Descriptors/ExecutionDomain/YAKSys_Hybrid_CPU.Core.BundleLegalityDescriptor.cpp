#include "YAKSys_Hybrid_CPU.Core.BundleLegalityDescriptor.h"

namespace YAKSys_Hybrid_CPU::Core
{

	BundleLegalityDescriptor::BundleLegalityDescriptor() : BundleLegalityDescriptor(BundleLegalityAuthority::Runtime, true, true, true)
	{
	}

	BundleLegalityDescriptor::BundleLegalityDescriptor(BundleLegalityAuthority authority, bool requiresRuntimeValidation, bool acceptsCompilerEvidence, bool allowsCompatibilityProjection)
		: Authority(authority), RequiresRuntimeValidation(requiresRuntimeValidation),
		  AcceptsCompilerEvidence(acceptsCompilerEvidence), AllowsCompatibilityProjection(allowsCompatibilityProjection)
	{
	}

	BundleLegalityAuthority BundleLegalityDescriptor::getAuthority() const
	{
		return Authority;
	}

	bool BundleLegalityDescriptor::getRequiresRuntimeValidation() const
	{
		return RequiresRuntimeValidation;
	}

	bool BundleLegalityDescriptor::getAcceptsCompilerEvidence() const
	{
		return AcceptsCompilerEvidence;
	}

	bool BundleLegalityDescriptor::getAllowsCompatibilityProjection() const
	{
		return AllowsCompatibilityProjection;
	}

	bool BundleLegalityDescriptor::isRuntimeAuthoritative() const
	{
		return getAuthority() == BundleLegalityAuthority::Runtime;
	}

	bool BundleLegalityDescriptor::getCanUseCompilerEvidenceAsAuthority() const
	{
		return false;
	}

	bool BundleLegalityDescriptor::getCanProjectToCompatibilityFrontend() const
	{
		return getAllowsCompatibilityProjection() && isRuntimeAuthoritative();
	}

	std::shared_ptr<BundleLegalityDescriptor> BundleLegalityDescriptor::WithCompatibilityProjection(bool enabled)
	{
		return std::make_shared<BundleLegalityDescriptor>(getAuthority(), getRequiresRuntimeValidation(), getAcceptsCompilerEvidence(), enabled);
	}
}
