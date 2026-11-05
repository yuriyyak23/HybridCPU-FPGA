#include "OwnedCoreObservationSession.h"
#include "../../Machine/HybridCPU_ISE.IseObservationService.h"

#include <algorithm>
#include <memory>

using namespace YAKSys_Hybrid_CPU;

namespace HybridCPU_ISE::NonRTL::Runtime
{

	std::wstring OwnedCoreObservationSession::getFailure() const
	{
		return Failure;
	}

	void OwnedCoreObservationSession::setFailure(const std::wstring &value)
	{
		Failure = value;
	}

	OwnedCoreObservationSession::OwnedCoreObservationSession(std::shared_ptr<Processor::CPU_Core> core, unsigned long long entryAddress, std::function<std::shared_ptr<IOwnedCoreObservationConsumer>(OwnedCoreObservationBinding)> attach, const std::wstring &segmentKind) : source(std::make_shared<OwnedCoreObservationSource>(core)), entryAddress(entryAddress), segmentKind(segmentKind)
	{
		if (!attach)
		{
			throw std::invalid_argument("attach");
		}
		try
		{
		OwnedCoreObservationBinding binding;
		binding.SegmentId = segmentId;
		binding.EntryAddress = entryAddress;
		binding.Observer = std::make_shared<IseObservationService>(source, std::make_shared<std::recursive_mutex>());
		binding.SegmentKind = segmentKind;
		binding.ReadOriginalRetire = [source = source, segmentId = segmentId, entryAddress = entryAddress]()
		{
			return source->ReadOriginalRetire(segmentId, entryAddress);
		};
		std::shared_ptr<IOwnedCoreObservationConsumer> attachedConsumer = attach(std::move(binding));
		if (attachedConsumer != nullptr)
		{
			consumer = std::move(attachedConsumer);
		}
		else
		{
			throw std::runtime_error("Observation attach returned no consumer.");
		}
		}
		catch (const std::runtime_error &error)
		{
			const char *message = error.what();
			RecordFailure(std::wstring(message, message + std::char_traits<char>::length(message)));
			source->dispose();
			closed = true;
		}
	}

	void OwnedCoreObservationSession::OnBoundary()
	{
		if (closed || consumer == nullptr)
		{
			return;
		}
		try
		{
			boundaries++;
			consumer->OnBoundary();
		}
		catch (const std::runtime_error &error)
		{
			const char *message = error.what();
			RecordFailure(std::wstring(message, message + std::char_traits<char>::length(message)));
			CloseConsumer();
			source->dispose();
			closed = true;
		}
	}

	void OwnedCoreObservationSession::CloseConsumer()
	{
		std::shared_ptr<IOwnedCoreObservationConsumer> prior = consumer;
		consumer = nullptr;
		try
		{
			if (prior != nullptr)
			{
				prior->dispose();
			}
		}
		catch (const std::runtime_error &error)
		{
			const char *message = error.what();
			RecordFailure(std::wstring(message, message + std::char_traits<char>::length(message)), true);
		}
	}

	void OwnedCoreObservationSession::RecordFailure(const std::wstring &message, bool closing)
	{
		const std::wstring prefix = closing && !Failure.empty() ? L" | close: " : L"";
		const std::size_t available = static_cast<std::size_t>(MaximumFailureCharacters) - std::min(Failure.size(), static_cast<std::size_t>(MaximumFailureCharacters));
		const std::wstring bounded = prefix + message.substr(0, static_cast<std::size_t>(MaximumFailureCharacters));
		failureTruncated = failureTruncated || message.size() > static_cast<std::size_t>(MaximumFailureCharacters) || bounded.size() > available;
		setFailure(Failure + bounded.substr(0, available));
	}

	std::shared_ptr<OwnedCoreObservationSegmentReport> OwnedCoreObservationSession::getReport() const
	{
		return std::make_shared<OwnedCoreObservationSegmentReport>(segmentId, entryAddress, segmentKind, boundaries, closed, getFailure(), failureTruncated);
	}

	void OwnedCoreObservationSession::dispose()
	{
		if (closed)
		{
			return;
		}
		CloseConsumer();
		source->dispose();
		closed = true;
	}
}
