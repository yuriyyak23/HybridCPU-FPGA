#pragma once

#include "../../Decoder/ClusterIssuePreparation.h"
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{
	/// <summary>
	/// Internal runtime-readable preparation snapshot derived from the decoder-side cluster seam.
	/// This object is advisory only and exists to prepare future runtime admission logic without
	/// changing the reference narrow execution mode.
	/// </summary>
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to the C# readonly struct:
// ORIGINAL LINE: internal readonly struct RuntimeClusterAdmissionPreparation
	class RuntimeClusterAdmissionPreparation
	{
	private:
		YAKSys_Hybrid_CPU::Core::DecodeMode DecodeMode;
		unsigned char PreparedScalarMask;
		unsigned char FallbackDiagnosticsMask;
		int PreparedScalarCount;
		int AuxiliaryReservationCount;
		bool ShouldConsiderClusterAdmission;
		bool SuggestsFallbackDiagnostics;

	public:
		RuntimeClusterAdmissionPreparation(YAKSys_Hybrid_CPU::Core::DecodeMode decodeMode, unsigned char preparedScalarMask, unsigned char fallbackDiagnosticsMask, int preparedScalarCount, int auxiliaryReservationCount, bool shouldConsiderClusterAdmission, bool suggestsFallbackDiagnostics);

		YAKSys_Hybrid_CPU::Core::DecodeMode getDecodeMode() const;
		unsigned char getPreparedScalarMask() const;
		unsigned char getFallbackDiagnosticsMask() const;
		int getPreparedScalarCount() const;
		int getAuxiliaryReservationCount() const;
		bool getShouldConsiderClusterAdmission() const;
		bool getSuggestsFallbackDiagnostics() const;

		static RuntimeClusterAdmissionPreparation Create(std::shared_ptr<ClusterIssuePreparation> clusterPreparation);

		static RuntimeClusterAdmissionPreparation CreateEmpty();

		RuntimeClusterAdmissionPreparation() = default;
	};
}
