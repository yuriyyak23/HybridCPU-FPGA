#pragma once

#include "OwnedCoreObservationSource.h"
#include "OwnedCoreObservationDiagnostics.h"
#include "../../CloseToHSL/Core/Architecture/Common/YAKSys_Hybrid_CPU.Processor.h"
#include <string>
#include <stdexcept>
#include <any>
#include <functional>
#include "tangible_exception_helper.h"
#include <memory>
#include "../../tangible_string_helper.h"
#include "../../tangible_disposable_interface.h"

using namespace YAKSys_Hybrid_CPU;

namespace HybridCPU_ISE::NonRTL::Runtime
{

	/// <summary>Optional host diagnostics. All methods execute on the existing CPU-owner thread.</summary>
	class IOwnedCoreObservationConsumer : public Disposable
	{
	public:
		virtual void OnBoundary() = 0;
	};

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public sealed record OwnedCoreObservationBinding(Guid SegmentId, ulong EntryAddress, IseObservationService Observer)
//	{
//		// Pure owner-thread read of the bound core's existing retire record, not
//		// reconstructed from diagnostic cycles, current PC or an exit projection.
//		public System.Nullable<Func<System.Nullable<OwnedCoreRetireObservation>>> ReadOriginalRetire
//		{
//			get;
//			set;
//		}
//		public string SegmentKind
//		{
//			get;
//			set;
//		} = "Unavailable";
//		public System.Nullable<OwnedCoreObservationRunIdentity> RunIdentity
//		{
//			get;
//			set;
//		}
//	}

	/// <summary>Supplied by the package/loader owner, never inferred from an observation snapshot.</summary>
// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public sealed record OwnedCoreObservationRunIdentity(Guid RunId, string PackageSha256, string LoaderStatus);

	/// <summary>Contains diagnostic failures so they cannot replace the guest execution outcome.</summary>
	class OwnedCoreObservationSession final : public Disposable
	{
	private:
		std::wstring Failure;

		const std::shared_ptr<OwnedCoreObservationSource> source;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private IOwnedCoreObservationConsumer? consumer;
		std::shared_ptr<IOwnedCoreObservationConsumer> consumer;
		bool closed = false;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public string? Failure {get;private set;}
	public:
		std::wstring getFailure() const;
	private:
		void setFailure(const std::wstring &value);
		const Guid segmentId = Guid::NewGuid();
		const unsigned long long entryAddress;
		const std::wstring segmentKind;
		long long boundaries = 0;
		bool failureTruncated = false;
		static constexpr int MaximumFailureCharacters = 2048;

	public:
		OwnedCoreObservationSession(std::shared_ptr<Processor::CPU_Core> core, unsigned long long entryAddress, std::function<std::shared_ptr<IOwnedCoreObservationConsumer>(OwnedCoreObservationBinding)> attach, const std::wstring &segmentKind = L"Unavailable");

		void OnBoundary();

	private:
		void CloseConsumer();

		void RecordFailure(const std::wstring &message, bool closing = false);

	public:
		std::shared_ptr<OwnedCoreObservationSegmentReport> getReport() const;

		void dispose() override;
	};

}
