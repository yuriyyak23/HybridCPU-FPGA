#include "OwnedCoreObservationDiagnostics.h"

namespace HybridCPU_ISE::NonRTL::Runtime
{

	void OwnedCoreObservationLedger::Record(std::shared_ptr<OwnedCoreObservationSegmentReport> report)
	{
		observed++;
		if (report != nullptr && report->Failure.has_value())
		{
			failed++;
		}
		if (segments.size() < Capacity)
		{
			segments.push_back(report);
		}
	}

	std::shared_ptr<OwnedCoreObservationDiagnostics> OwnedCoreObservationLedger::Snapshot()
	{
		return std::make_shared<OwnedCoreObservationDiagnostics>(
			segments, observed, failed, observed - static_cast<long long>(segments.size()));
	}
}
