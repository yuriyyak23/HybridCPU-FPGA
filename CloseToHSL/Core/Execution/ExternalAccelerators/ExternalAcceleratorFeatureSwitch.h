#pragma once
#include <memory>

namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators
{

	/// <summary>
	/// Rollback switch surface for L7-SDC validation. Disabling execution must not
	/// disable parser, carrier, guard, telemetry, token observation, or fail-closed
	/// rejection surfaces.
	/// </summary>
// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public sealed record ExternalAcceleratorFeatureSwitch
//	{
//		public static ExternalAcceleratorFeatureSwitch Enabled
//		{
//			get;
//		} = new();
//
//		public static ExternalAcceleratorFeatureSwitch SubmitAndBackendExecutionDisabled
//		{
//			get;
//		} = new() { SubmitAdmissionEnabled = false, BackendExecutionEnabled = false };
//
//		public static ExternalAcceleratorFeatureSwitch BackendExecutionDisabled
//		{
//			get;
//		} = new() { BackendExecutionEnabled = false };
//
//		public bool SubmitAdmissionEnabled
//		{
//			get;
//			set;
//		} = true;
//
//		public bool BackendExecutionEnabled
//		{
//			get;
//			set;
//		} = true;
//
//		public bool ParserAndCarrierValidationEnabled
//		{
//			get
//			{
//				return true;
//			}
//		}
//
//		public bool GuardValidationEnabled
//		{
//			get
//			{
//				return true;
//			}
//		}
//
//		public bool TokenObservationEnabled
//		{
//			get
//			{
//				return true;
//			}
//		}
//
//		public bool TelemetryEvidenceEnabled
//		{
//			get
//			{
//				return true;
//			}
//		}
//	}

	struct ExternalAcceleratorFeatureSwitch final {
		bool SubmitAdmissionEnabled = true;
		bool BackendExecutionEnabled = true;
		bool ParserAndCarrierValidationEnabled() const noexcept { return true; }
		bool GuardValidationEnabled() const noexcept { return true; }
		bool TokenObservationEnabled() const noexcept { return true; }
		bool TelemetryEvidenceEnabled() const noexcept { return true; }
		static const std::shared_ptr<ExternalAcceleratorFeatureSwitch> Enabled;
		static const std::shared_ptr<ExternalAcceleratorFeatureSwitch> SubmitAndBackendExecutionDisabled;
		static const std::shared_ptr<ExternalAcceleratorFeatureSwitch> BackendExecutionDisabled;
	};
	inline const std::shared_ptr<ExternalAcceleratorFeatureSwitch> ExternalAcceleratorFeatureSwitch::Enabled = std::make_shared<ExternalAcceleratorFeatureSwitch>();
	inline const std::shared_ptr<ExternalAcceleratorFeatureSwitch> ExternalAcceleratorFeatureSwitch::SubmitAndBackendExecutionDisabled = std::make_shared<ExternalAcceleratorFeatureSwitch>(false, false);
	inline const std::shared_ptr<ExternalAcceleratorFeatureSwitch> ExternalAcceleratorFeatureSwitch::BackendExecutionDisabled = std::make_shared<ExternalAcceleratorFeatureSwitch>(true, false);
}
