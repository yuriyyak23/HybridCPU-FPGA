#pragma once

#include <vector>
#include <memory>
#include <optional>
#include <string>
#include <utility>
#include <functional>
#include "../../Guid.h"

namespace HybridCPU_ISE { class IseObservationService; }

namespace HybridCPU_ISE::NonRTL::Runtime
{
	struct OwnedCoreRetireObservation;

	struct OwnedCoreObservationRunIdentity final
	{
		Guid RunId;
		std::wstring PackageSha256;
		std::wstring LoaderStatus;
	};

	struct OwnedCoreObservationBinding final
	{
		Guid SegmentId;
		unsigned long long EntryAddress{};
		std::shared_ptr<::HybridCPU_ISE::IseObservationService> Observer;
		std::function<std::optional<std::shared_ptr<OwnedCoreRetireObservation>>()> ReadOriginalRetire;
		std::wstring SegmentKind = L"Unavailable";
		std::optional<OwnedCoreObservationRunIdentity> RunIdentity;
	};

struct OwnedCoreObservationSegmentReport final
{
	const Guid SegmentId;
	const unsigned long long EntryAddress;
	const std::wstring SegmentKind;
	const long long BoundariesObserved;
	const bool Closed;
	const std::optional<std::wstring> Failure;
	const bool FailureTruncated;

	OwnedCoreObservationSegmentReport(const Guid &segmentId, unsigned long long entryAddress,
		const std::wstring &segmentKind, long long boundariesObserved, bool closed,
		std::optional<std::wstring> failure, bool failureTruncated)
		: SegmentId(segmentId), EntryAddress(entryAddress), SegmentKind(segmentKind),
		  BoundariesObserved(boundariesObserved), Closed(closed),
		  Failure(std::move(failure)), FailureTruncated(failureTruncated)
	{
	}

	OwnedCoreObservationSegmentReport(const Guid &segmentId, unsigned long long entryAddress,
		const std::wstring &segmentKind, long long boundariesObserved, bool closed,
		const std::wstring &failure, bool failureTruncated)
		: OwnedCoreObservationSegmentReport(segmentId, entryAddress, segmentKind, boundariesObserved,
			closed, failure.empty() ? std::nullopt : std::optional<std::wstring>(failure), failureTruncated)
	{
	}
};

	/// <summary>Terminal host diagnostic evidence only; never GC or guest completion evidence.</summary>
struct OwnedCoreObservationDiagnostics final
{
	const std::vector<std::shared_ptr<OwnedCoreObservationSegmentReport>> Segments;
	const long long SegmentsObserved;
	const long long FailedSegments;
	const long long DroppedSegments;

	OwnedCoreObservationDiagnostics(std::vector<std::shared_ptr<OwnedCoreObservationSegmentReport>> segments,
		long long segmentsObserved, long long failedSegments, long long droppedSegments)
		: Segments(std::move(segments)), SegmentsObserved(segmentsObserved),
		  FailedSegments(failedSegments), DroppedSegments(droppedSegments)
	{
	}

	std::wstring getSchema() const { return L"hybridcpu.owned-core-observation-terminal/v1"; }
};

	class OwnedCoreObservationLedger final : public std::enable_shared_from_this<OwnedCoreObservationLedger>
	{
	public:
		static constexpr int Capacity = 256;
	private:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly List<OwnedCoreObservationSegmentReport> segments = new();
		std::vector<std::shared_ptr<OwnedCoreObservationSegmentReport>> segments = std::vector<std::shared_ptr<OwnedCoreObservationSegmentReport>>();
		long long observed = 0;
		long long failed = 0;

	public:
		void Record(std::shared_ptr<OwnedCoreObservationSegmentReport> report);

		std::shared_ptr<OwnedCoreObservationDiagnostics> Snapshot();
	};

}
