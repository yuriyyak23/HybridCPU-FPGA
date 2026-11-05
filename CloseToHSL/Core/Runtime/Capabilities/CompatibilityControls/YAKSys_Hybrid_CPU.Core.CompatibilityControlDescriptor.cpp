#include "YAKSys_Hybrid_CPU.Core.CompatibilityControlDescriptor.h"

namespace YAKSys_Hybrid_CPU::Core
{
    const std::shared_ptr<CompatibilityControlDescriptor> CompatibilityControlDescriptor::NotMaterialized = std::make_shared<CompatibilityControlDescriptor>(CompatibilityControlMaterializationState::NotMaterialized, nullptr);
    const std::shared_ptr<CompatibilityControlDescriptor> CompatibilityControlDescriptor::FailClosedProjectionOnly = FromNeutralSemantics(CompatibilityControlReadOnlyView::FailClosedProjectionOnly);

	CompatibilityControlDescriptor::CompatibilityControlDescriptor() : CompatibilityControlDescriptor(CompatibilityControlMaterializationState::NotMaterialized, nullptr)
	{
	}

	CompatibilityControlDescriptor::CompatibilityControlDescriptor(CompatibilityControlMaterializationState materializationState, std::shared_ptr<CompatibilityControlReadOnlyView> readOnlyView) : MaterializationState(materializationState), ReadOnlyView(std::move(readOnlyView)) {}

	std::shared_ptr<CompatibilityControlDescriptor> CompatibilityControlDescriptor::getNotMaterialized()
	{
		return NotMaterialized;
	}

	std::shared_ptr<CompatibilityControlDescriptor> CompatibilityControlDescriptor::getFailClosedProjectionOnly()
	{
		return FailClosedProjectionOnly;
	}

	CompatibilityControlMaterializationState CompatibilityControlDescriptor::getMaterializationState() const
	{
		return MaterializationState;
	}

	std::shared_ptr<CompatibilityControlReadOnlyView> CompatibilityControlDescriptor::getReadOnlyView() const
	{
		return ReadOnlyView;
	}

	bool CompatibilityControlDescriptor::isRuntimeAuthoritativeControlOwner() const
	{
		return false;
	}

	bool CompatibilityControlDescriptor::getRequiresOwnerIssuedPolicySnapshot() const
	{
		return true;
	}

	bool CompatibilityControlDescriptor::getHasMaterializedReadOnlyProjection() const
	{
		return getMaterializationState() == CompatibilityControlMaterializationState::ReadOnlyProjectionAvailable && getReadOnlyView()->isSemanticallyComplete();
	}

	std::shared_ptr<CompatibilityControlDescriptor> CompatibilityControlDescriptor::FromNeutralSemantics(std::shared_ptr<CompatibilityControlReadOnlyView> readOnlyView)
	{
		return std::make_shared<CompatibilityControlDescriptor>(readOnlyView->isSemanticallyComplete() ? CompatibilityControlMaterializationState::ReadOnlyProjectionAvailable : CompatibilityControlMaterializationState::NotMaterialized, std::move(readOnlyView));
	}

	bool CompatibilityControlDescriptor::TryCreateReadOnlyControlView(std::shared_ptr<CompatibilityControlReadOnlyView> &view, std::wstring &reason)
	{
		if (!getHasMaterializedReadOnlyProjection())
		{
			view = nullptr;
			reason = L"Neutral compatibility control owner has no materialized read-only control view.";
			return false;
		}

		view = getReadOnlyView();
		reason = L"Neutral compatibility control owner exposed a materialized read-only control view.";
		return true;
	}
}
