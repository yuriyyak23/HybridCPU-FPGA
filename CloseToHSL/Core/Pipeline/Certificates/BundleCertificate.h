#pragma once

#include <chrono>
#include <string>
#include <vector>
#include <format>
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{
	class MicroOp;
	/// <summary>
	/// Bundle resource proof structure for Singularity/SIP-style formal verification.
	/// Provides a certificate that demonstrates resource isolation and safety guarantees.
	/// (Phase 2: Formal Resource Proofs - Verification Readiness)
	/// </summary>
	class BundleResourceProof
	{
		/// <summary>
		/// Cycle at which this proof was generated
		/// </summary>
	public:
		long long Cycle = 0;

		/// <summary>
		/// Deterministic hash of the VLIW bundle
		/// Ensures proof corresponds to specific bundle state
		/// </summary>
		unsigned int BundleHash = 0;

		/// <summary>
		/// Start address of allowed memory range for this bundle's thread
		/// </summary>
		unsigned long long AllowedMemoryRangeStart = 0;

		/// <summary>
		/// End address of allowed memory range for this bundle's thread
		/// </summary>
		unsigned long long AllowedMemoryRangeEnd = 0;

		/// <summary>
		/// Bitmask indicating which hardware threads are active in this bundle
		/// Bit i = 1 means thread i has operations in this bundle
		/// </summary>
		unsigned int ThreadMask = 0;

		/// <summary>
		/// Simulated signature bytes for the ISE proof fixture.
		/// This is not a hardware root-of-trust claim.
		/// </summary>
		std::vector<unsigned char> Signature;

		/// <summary>
		/// Timestamp when proof was generated (for audit trail)
		/// </summary>
		std::chrono::system_clock::time_point Timestamp;

		/// <summary>
		/// Whether this proof passed all verification checks
		/// </summary>
		bool IsValid = false;

		/// <summary>
		/// Human-readable description of verification status
		/// </summary>
		std::wstring VerificationStatus;

		/// <summary>
		/// Create a new bundle resource proof
		/// </summary>
		BundleResourceProof();

		/// <summary>
		/// Calculate deterministic hash of VLIW bundle
		/// Uses FNV-1a hash algorithm for determinism
		/// </summary>
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: public static uint CalculateBundleHash(System.Collections.Generic.IReadOnlyList<MicroOp?> bundle)
		static unsigned int CalculateBundleHash(const std::vector<std::shared_ptr<MicroOp>> &bundle);

		/// <summary>
		/// Verify proof-fixture signature bytes.
		/// This is simulated for ISE and is not a hardware root-of-trust claim.
		/// </summary>
		bool VerifySignature();

		/// <summary>
		/// Get human-readable string representation
		/// </summary>
		std::wstring ToString();
	};
}
