#pragma once

#include "BundleLegalityDescriptor.h"
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{
	class BundleLegalityDescriptor final : public std::enable_shared_from_this<BundleLegalityDescriptor>
	{
	private:
		const BundleLegalityAuthority Authority;
		const bool RequiresRuntimeValidation;
		const bool AcceptsCompilerEvidence;
		const bool AllowsCompatibilityProjection;

	public:
		BundleLegalityDescriptor();

		BundleLegalityDescriptor(BundleLegalityAuthority authority, bool requiresRuntimeValidation, bool acceptsCompilerEvidence, bool allowsCompatibilityProjection);

		BundleLegalityAuthority getAuthority() const;

		bool getRequiresRuntimeValidation() const;

		bool getAcceptsCompilerEvidence() const;

		bool getAllowsCompatibilityProjection() const;

		bool isRuntimeAuthoritative() const;

		bool getCanUseCompilerEvidenceAsAuthority() const;

		bool getCanProjectToCompatibilityFrontend() const;

		std::shared_ptr<BundleLegalityDescriptor> WithCompatibilityProjection(bool enabled);
	};
}
