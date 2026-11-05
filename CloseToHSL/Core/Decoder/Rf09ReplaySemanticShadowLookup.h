#pragma once

namespace YAKSys_Hybrid_CPU::Core::Decoder { class ReplayEntry; class CanonicalBundle; }

#include "Rf09ReplayEntry.h"
#include "CanonicalDecodedContracts.h"
#include "ReplayShadowTypes.h"
#include <memory>

namespace YAKSys_Hybrid_CPU::Core::Decoder
{

	enum class ReplaySemanticShadowObservation : unsigned char
	{
		Miss = 0,
		EquivalentHit = 1,
		ContentMismatch = 2
	};

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct ReplaySemanticShadowMetrics(ulong Observations, ulong Misses, ulong EquivalentHits, ulong ContentMismatches, ulong Invalidations)
//	{
//		public ulong AccountedObservations
//		{
//			get
//			{
//				return Misses + EquivalentHits + ContentMismatches;
//			}
//		}
//	}

	/// <summary>
	/// Bounded one-entry RF-09 shadow observer. It never returns cached canonical
	/// content and therefore cannot become a production decode or issue source.
	/// </summary>
	class ReplaySemanticShadowLookup final : public std::enable_shared_from_this<ReplaySemanticShadowLookup>
	{
	private:
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private ReplayEntry? _entry;
		std::shared_ptr<ReplayEntry> _entry;
		unsigned long long _observations = 0;
		unsigned long long _misses = 0;
		unsigned long long _equivalentHits = 0;
		unsigned long long _contentMismatches = 0;
		unsigned long long _invalidations = 0;

	public:
		ReplaySemanticShadowObservation ObserveLiveDecode(std::shared_ptr<CanonicalBundle> liveCanonicalBundle);

		void Invalidate();

		std::shared_ptr<ReplaySemanticShadowMetrics> getMetrics() const;

#if defined(TESTING)
		bool getTestHasEntry() const;
#endif
	};

}
