#pragma once

#include "CompatibilityControlDescriptor.h"
#include <string>
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{
	class CompatibilityControlDescriptor final : public std::enable_shared_from_this<CompatibilityControlDescriptor>
	{
	private:
		static const std::shared_ptr<CompatibilityControlDescriptor> NotMaterialized;
		static const std::shared_ptr<CompatibilityControlDescriptor> FailClosedProjectionOnly;
		const CompatibilityControlMaterializationState MaterializationState;
		const std::shared_ptr<CompatibilityControlReadOnlyView> ReadOnlyView;

	public:
		CompatibilityControlDescriptor();

		CompatibilityControlDescriptor(CompatibilityControlMaterializationState materializationState, std::shared_ptr<CompatibilityControlReadOnlyView> readOnlyView);

		static std::shared_ptr<CompatibilityControlDescriptor> getNotMaterialized();

		static std::shared_ptr<CompatibilityControlDescriptor> getFailClosedProjectionOnly();

		CompatibilityControlMaterializationState getMaterializationState() const;

		std::shared_ptr<CompatibilityControlReadOnlyView> getReadOnlyView() const;

		bool isRuntimeAuthoritativeControlOwner() const;

		bool getRequiresOwnerIssuedPolicySnapshot() const;

		bool getHasMaterializedReadOnlyProjection() const;

		static std::shared_ptr<CompatibilityControlDescriptor> FromNeutralSemantics(std::shared_ptr<CompatibilityControlReadOnlyView> readOnlyView);

		bool TryCreateReadOnlyControlView(std::shared_ptr<CompatibilityControlReadOnlyView> &view, std::wstring &reason);
	};
}
