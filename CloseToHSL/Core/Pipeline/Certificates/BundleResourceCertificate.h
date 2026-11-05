#pragma once

#include "SafetyMask128.h"
namespace YAKSys_Hybrid_CPU::Core { class MicroOp; }
#include "BundleResourceCertificate4Way.h"
#include "StructuralIdentity/HardwareHash.h"
#include <string>
#include <vector>
#include <limits>
#include <stdexcept>
#include <any>
#include <format>
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{
	/// <summary>
	/// Structural identity of an inter-core bundle resource certificate.
	/// Excludes temporal fields like cycle number and keeps replay/template matching
	/// scoped to structural proof context instead of raw certificate object equality.
	/// </summary>
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to the C# readonly struct:
// ORIGINAL LINE: public readonly struct BundleResourceCertificateIdentity : IEquatable<BundleResourceCertificateIdentity>
	class BundleResourceCertificateIdentity
	{
	private:
		unsigned long long CombinedMaskLow{};
		unsigned long long CombinedMaskHigh{};
		int OwnerThreadId{};
		int OperationCount{};
		unsigned long long BundleHash{};
		unsigned long long AssistStructuralKey{};
		unsigned char DmaStreamComputeIdentityCount{};
		unsigned long long DmaStreamComputeReplayEnvelopeHash{};

	public:
		BundleResourceCertificateIdentity(unsigned long long combinedMaskLow, unsigned long long combinedMaskHigh, int ownerThreadId, int operationCount, unsigned long long bundleHash, unsigned long long assistStructuralKey, unsigned char dmaStreamComputeIdentityCount = 0, unsigned long long dmaStreamComputeReplayEnvelopeHash = 0);

		unsigned long long getCombinedMaskLow() const;

		unsigned long long getCombinedMaskHigh() const;

		int getOwnerThreadId() const;

		int getOperationCount() const;

		unsigned long long getBundleHash() const;

		unsigned long long getAssistStructuralKey() const;

		unsigned char getDmaStreamComputeIdentityCount() const;

		unsigned long long getDmaStreamComputeReplayEnvelopeHash() const;

		bool isValid() const;

		bool Equals(BundleResourceCertificateIdentity other) const ;

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public override bool Equals(Object? obj)
		bool Equals(std::any obj) const;

		int GetHashCode() const;

		public:
        BundleResourceCertificateIdentity() = default;
	};

	/// <summary>
	/// Bundle resource certificate for verifiable non-interference (Phase: Safety Tags & Certificates).
	///
	/// Purpose: Provides a formal proof that a bundle of micro-operations does not conflict
	/// with other operations based on resource usage. This certificate can be used by the
	/// Singularity-style OS to verify isolation guarantees at the hardware level.
	///
	/// Design goals:
	/// - Hardware-agnostic: suitable for HLS synthesis
	/// - Verifiable: can be checked without re-analyzing operations
	/// - Efficient: minimal overhead for certificate generation and validation
	/// - Formal: based on safety mask bit-level analysis
	/// </summary>
	class BundleResourceCertificate : public std::enable_shared_from_this<BundleResourceCertificate>
	{
	private:
		SafetyMask128 CombinedMask;
		int OwnerThreadId = 0;
		unsigned long long CycleNumber = 0;
		int OperationCount = 0;
		bool IsSpeculative = false;
		unsigned long long BundleHash = 0;
		unsigned long long AssistStructuralKey = 0;
		unsigned char DmaStreamComputeIdentityCount = 0;
		unsigned long long DmaStreamComputeReplayEnvelopeHash = 0;

		static constexpr unsigned long long RegisterBitsMaskLow32 = 0xFFFF'FFFFULL;
		static constexpr unsigned long long AssistStructuralKeyOffsetBasis = 14695981039346656037ULL;
		static constexpr unsigned long long AssistStructuralKeyPrime = 1099511628211ULL;

		/// <summary>
		/// Combined safety mask for all operations in the bundle.
		/// Represents the union of all resource requirements.
		/// </summary>
	public:
		SafetyMask128 getCombinedMask() const;
		void setCombinedMask(SafetyMask128 value);

		/// <summary>
		/// Thread ID that owns this bundle.
		/// Used for thread isolation verification.
		/// </summary>
		int getOwnerThreadId() const;
		void setOwnerThreadId(int value);

		/// <summary>
		/// Cycle number when this certificate was created.
		/// Used for temporal ordering and debugging.
		/// </summary>
		unsigned long long getCycleNumber() const;
		void setCycleNumber(unsigned long long value);

		/// <summary>
		/// Number of operations in the bundle.
		/// Used for verification and statistics.
		/// </summary>
		int getOperationCount() const;
		void setOperationCount(int value);

		/// <summary>
		/// Indicates whether this certificate represents a speculative execution.
		/// Speculative operations may be rolled back without committing.
		/// </summary>
		bool isSpeculative() const;
		void setSpeculative(bool value);

		/// <summary>
		/// Hash of the bundle for integrity verification.
		/// Computed from operation opcodes and resource masks.
		/// </summary>
		unsigned long long getBundleHash() const;
		void setBundleHash(unsigned long long value);

		/// <summary>
		/// Assist-owned structural discriminator that keeps replay/template reuse
		/// aware of widened assist owner tuples without reopening retire-visible
		/// bundle semantics.
		/// </summary>
		unsigned long long getAssistStructuralKey() const;
		void setAssistStructuralKey(unsigned long long value);

		/// <summary>
		/// Count of descriptor-backed lane6 compute evidence records carried by this certificate.
		/// </summary>
		unsigned char getDmaStreamComputeIdentityCount() const;
		void setDmaStreamComputeIdentityCount(unsigned char value);

		/// <summary>
		/// Full DmaStreamCompute replay envelope hash. Evidence only; never commit authority.
		/// </summary>
		unsigned long long getDmaStreamComputeReplayEnvelopeHash() const;
		void setDmaStreamComputeReplayEnvelopeHash(unsigned long long value);

		/// <summary>
		/// Structural identity used by replay/template caches.
		/// Keeps certificate reuse scoped to proof context rather than object equality.
		/// </summary>
		BundleResourceCertificateIdentity getStructuralIdentity() const;

		/// <summary>
		/// Create a resource certificate for a bundle of micro-operations.
		/// </summary>
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: public static BundleResourceCertificate Create(System.Collections.Generic.IReadOnlyList<MicroOp?> bundle, int ownerThreadId, ulong cycleNumber)
		static std::shared_ptr<BundleResourceCertificate> Create(const std::vector<std::shared_ptr<MicroOp>> &bundle, int ownerThreadId, unsigned long long cycleNumber);

		/// <summary>
		/// Verify that this certificate does not conflict with another certificate.
		/// Returns true if the two bundles can execute in parallel without conflicts.
		/// </summary>
		bool NoConflictsWith(std::shared_ptr<BundleResourceCertificate> other);

		/// <summary>
		/// Verify that this certificate does not conflict with a candidate operation.
		/// Returns true if the candidate can be injected into the bundle without conflicts.
		/// </summary>
		bool CanInject(std::shared_ptr<MicroOp> candidate);

		/// <summary>
		/// Verify candidate injection while preserving explicit conflict detail for
		/// checker-owned legality decisions.
		/// </summary>
		bool CanInjectWithReason(std::shared_ptr<MicroOp> candidate, CertificateRejectDetail &detail);

		/// <summary>
		/// Create an extended certificate that includes an additional operation.
		/// Returns a new certificate representing the bundle with the operation added.
		/// </summary>
		std::shared_ptr<BundleResourceCertificate> WithOperation(std::shared_ptr<MicroOp> op);

	private:
		static SafetyMask128 GetSharedMask(SafetyMask128 combinedMask);

		static void AppendAssistReplayKey(HardwareHash hasher, std::shared_ptr<MicroOp> op);

		static void AppendDmaStreamComputeReplayKey(HardwareHash hasher, std::shared_ptr<MicroOp> op);

		static unsigned long long PackIntPair(int low, int high);

		static unsigned long long AccumulateAssistStructuralKey(unsigned long long currentKey, std::shared_ptr<MicroOp> op);

		static unsigned char AccumulateDmaStreamComputeIdentityCount(unsigned char currentCount, std::shared_ptr<MicroOp> op);

		static unsigned long long AccumulateDmaStreamComputeReplayEnvelopeHash(unsigned long long currentHash, std::shared_ptr<MicroOp> op);

		static unsigned long long MixAssistStructuralKey(unsigned long long hash, unsigned long long value);

		static bool HasRegisterConflict(unsigned int bundleRegMask, unsigned int candidateRegMask);

		/// <summary>
		/// Validate the integrity of this certificate.
		/// Returns true if the certificate is well-formed and valid.
		/// </summary>
	public:
		bool IsValid();

		std::wstring ToString();

		/// <summary>
		/// Empty certificate (no resources, no operations).
		/// Used as a starting point for certificate construction.
		/// </summary>
		static const std::shared_ptr<BundleResourceCertificate> Empty;
	};
}

inline const std::shared_ptr<YAKSys_Hybrid_CPU::Core::BundleResourceCertificate> YAKSys_Hybrid_CPU::Core::BundleResourceCertificate::Empty = [] {
    auto certificate = std::make_shared<BundleResourceCertificate>();
    certificate->CombinedMask = SafetyMask128::Zero;
    certificate->OwnerThreadId = -1;
    certificate->CycleNumber = 0;
    certificate->OperationCount = 0;
    certificate->IsSpeculative = false;
    certificate->BundleHash = 0;
    certificate->AssistStructuralKey = 0;
    return certificate;
}();
