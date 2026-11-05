#include "Rf09ReplaySemanticShadowLookup.h"
#include <limits>
#include <stdexcept>

namespace YAKSys_Hybrid_CPU::Core::Decoder
{
	namespace
	{
		void IncrementChecked(unsigned long long &counter)
		{
			if (counter == (std::numeric_limits<unsigned long long>::max)())
			{
				throw std::overflow_error("Replay semantic shadow counter overflow.");
			}
			++counter;
		}
	}

	ReplaySemanticShadowObservation ReplaySemanticShadowLookup::ObserveLiveDecode(std::shared_ptr<CanonicalBundle> liveCanonicalBundle)
	{
		ArgumentNullException::ThrowIfNull(liveCanonicalBundle);
		std::shared_ptr<ReplayEntry> liveEntry = ReplayEntry::Create(liveCanonicalBundle);
		IncrementChecked(_observations);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: ReplayEntry? cachedEntry = _entry;
		std::shared_ptr<ReplayEntry> cachedEntry = _entry;
		if (cachedEntry == nullptr || !cachedEntry->getSemanticKey().Equals(liveEntry->getSemanticKey()))
		{
			IncrementChecked(_misses);
			_entry = liveEntry;
			return ReplaySemanticShadowObservation::Miss;
		}

		if (cachedEntry->HasSameFrozenSemanticContent(liveEntry))
		{
			IncrementChecked(_equivalentHits);
			return ReplaySemanticShadowObservation::EquivalentHit;
		}

		IncrementChecked(_contentMismatches);
		_entry = liveEntry;
		return ReplaySemanticShadowObservation::ContentMismatch;
	}

	void ReplaySemanticShadowLookup::Invalidate()
	{
		if (_entry != nullptr)
		{
			_entry = nullptr;
			IncrementChecked(_invalidations);
		}
	}

	std::shared_ptr<ReplaySemanticShadowMetrics> ReplaySemanticShadowLookup::getMetrics() const
	{
		return std::make_shared<ReplaySemanticShadowMetrics>(_observations, _misses, _equivalentHits, _contentMismatches, _invalidations);
	}

#if defined(TESTING)
	bool ReplaySemanticShadowLookup::getTestHasEntry() const
	{
		return _entry != nullptr;
	}
#endif
}
