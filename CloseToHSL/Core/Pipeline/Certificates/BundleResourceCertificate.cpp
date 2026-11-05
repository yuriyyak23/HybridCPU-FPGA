#include "BundleResourceCertificate.h"
#include "../MicroOps/Assist/AssistMicroOp.h"
#include "../Assist/Runtime/AssistRuntime.h"
#include "../MicroOps/Lane6DmaStream/DmaStreamComputeMicroOp.h"
#include "../MicroOps/Types/MicroOp.h"
#include "../Metadata/MicroOpAdmissionMetadata.h"
#include <functional>

namespace YAKSys_Hybrid_CPU::Core
{
	namespace
	{
		constexpr unsigned long long AdmissionRegisterBitsMaskLow32 = 0xFFFF'FFFFULL;

		SafetyMask128 GetAdmissionCertificateMask(const MicroOpAdmissionMetadata &admission)
		{
			return SafetyMask128((admission.StructuralSafetyMask.Low & ~AdmissionRegisterBitsMaskLow32) | admission.RegisterHazardMask, admission.StructuralSafetyMask.High);
		}

		SafetyMask128 GetAdmissionSharedStructuralMask(const MicroOpAdmissionMetadata &admission)
		{
			return SafetyMask128(admission.StructuralSafetyMask.Low & ~AdmissionRegisterBitsMaskLow32, admission.StructuralSafetyMask.High);
		}
	}

	BundleResourceCertificateIdentity::BundleResourceCertificateIdentity(unsigned long long combinedMaskLow, unsigned long long combinedMaskHigh, int ownerThreadId, int operationCount, unsigned long long bundleHash, unsigned long long assistStructuralKey, unsigned char dmaStreamComputeIdentityCount, unsigned long long dmaStreamComputeReplayEnvelopeHash)
	{
		CombinedMaskLow = combinedMaskLow;
		CombinedMaskHigh = combinedMaskHigh;
		OwnerThreadId = ownerThreadId;
		OperationCount = operationCount;
		BundleHash = bundleHash;
		AssistStructuralKey = assistStructuralKey;
		DmaStreamComputeIdentityCount = dmaStreamComputeIdentityCount;
		DmaStreamComputeReplayEnvelopeHash = dmaStreamComputeReplayEnvelopeHash;
	}

	unsigned long long BundleResourceCertificateIdentity::getCombinedMaskLow() const
	{
		return CombinedMaskLow;
	}

	unsigned long long BundleResourceCertificateIdentity::getCombinedMaskHigh() const
	{
		return CombinedMaskHigh;
	}

	int BundleResourceCertificateIdentity::getOwnerThreadId() const
	{
		return OwnerThreadId;
	}

	int BundleResourceCertificateIdentity::getOperationCount() const
	{
		return OperationCount;
	}

	unsigned long long BundleResourceCertificateIdentity::getBundleHash() const
	{
		return BundleHash;
	}

	unsigned long long BundleResourceCertificateIdentity::getAssistStructuralKey() const
	{
		return AssistStructuralKey;
	}

	unsigned char BundleResourceCertificateIdentity::getDmaStreamComputeIdentityCount() const
	{
		return DmaStreamComputeIdentityCount;
	}

	unsigned long long BundleResourceCertificateIdentity::getDmaStreamComputeReplayEnvelopeHash() const
	{
		return DmaStreamComputeReplayEnvelopeHash;
	}

	bool BundleResourceCertificateIdentity::isValid() const
	{
		return (getCombinedMaskLow() != 0 || getCombinedMaskHigh() != 0) && getOperationCount() > 0 && getOwnerThreadId() >= 0 && getOwnerThreadId() < 16;
	}

	bool BundleResourceCertificateIdentity::Equals(BundleResourceCertificateIdentity other) const
	{
		return getCombinedMaskLow() == other.getCombinedMaskLow() && getCombinedMaskHigh() == other.getCombinedMaskHigh() && getOwnerThreadId() == other.getOwnerThreadId() && getOperationCount() == other.getOperationCount() && getBundleHash() == other.getBundleHash() && getAssistStructuralKey() == other.getAssistStructuralKey() && getDmaStreamComputeIdentityCount() == other.getDmaStreamComputeIdentityCount() && getDmaStreamComputeReplayEnvelopeHash() == other.getDmaStreamComputeReplayEnvelopeHash();
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public override bool Equals(Object? obj)
	bool BundleResourceCertificateIdentity::Equals(std::any obj) const
	{
		const auto *other = std::any_cast<BundleResourceCertificateIdentity>(&obj);
		return other != nullptr && Equals(*other);
	}

	int BundleResourceCertificateIdentity::GetHashCode() const
	{
		std::size_t hash = 0;
		auto combine = [&hash](std::size_t value)
		{
			hash ^= value + static_cast<std::size_t>(0x9E3779B97F4A7C15ULL) + (hash << 6) + (hash >> 2);
		};
		combine(std::hash<unsigned long long>{}(getCombinedMaskLow()));
		combine(std::hash<unsigned long long>{}(getCombinedMaskHigh()));
		combine(std::hash<int>{}(getOwnerThreadId()));
		combine(std::hash<int>{}(getOperationCount()));
		combine(std::hash<unsigned long long>{}(getBundleHash()));
		combine(std::hash<unsigned long long>{}(getAssistStructuralKey()));
		combine(std::hash<unsigned char>{}(getDmaStreamComputeIdentityCount()));
		combine(std::hash<unsigned long long>{}(getDmaStreamComputeReplayEnvelopeHash()));
		return static_cast<int>(hash);
	}

	SafetyMask128 BundleResourceCertificate::getCombinedMask() const
	{
		return CombinedMask;
	}

	void BundleResourceCertificate::setCombinedMask(SafetyMask128 value)
	{
		CombinedMask = value;
	}

	int BundleResourceCertificate::getOwnerThreadId() const
	{
		return OwnerThreadId;
	}

	void BundleResourceCertificate::setOwnerThreadId(int value)
	{
		OwnerThreadId = value;
	}

	unsigned long long BundleResourceCertificate::getCycleNumber() const
	{
		return CycleNumber;
	}

	void BundleResourceCertificate::setCycleNumber(unsigned long long value)
	{
		CycleNumber = value;
	}

	int BundleResourceCertificate::getOperationCount() const
	{
		return OperationCount;
	}

	void BundleResourceCertificate::setOperationCount(int value)
	{
		OperationCount = value;
	}

	bool BundleResourceCertificate::isSpeculative() const
	{
		return IsSpeculative;
	}

	void BundleResourceCertificate::setSpeculative(bool value)
	{
		IsSpeculative = value;
	}

	unsigned long long BundleResourceCertificate::getBundleHash() const
	{
		return BundleHash;
	}

	void BundleResourceCertificate::setBundleHash(unsigned long long value)
	{
		BundleHash = value;
	}

	unsigned long long BundleResourceCertificate::getAssistStructuralKey() const
	{
		return AssistStructuralKey;
	}

	void BundleResourceCertificate::setAssistStructuralKey(unsigned long long value)
	{
		AssistStructuralKey = value;
	}

	unsigned char BundleResourceCertificate::getDmaStreamComputeIdentityCount() const
	{
		return DmaStreamComputeIdentityCount;
	}

	void BundleResourceCertificate::setDmaStreamComputeIdentityCount(unsigned char value)
	{
		DmaStreamComputeIdentityCount = value;
	}

	unsigned long long BundleResourceCertificate::getDmaStreamComputeReplayEnvelopeHash() const
	{
		return DmaStreamComputeReplayEnvelopeHash;
	}

	void BundleResourceCertificate::setDmaStreamComputeReplayEnvelopeHash(unsigned long long value)
	{
		DmaStreamComputeReplayEnvelopeHash = value;
	}

	BundleResourceCertificateIdentity BundleResourceCertificate::getStructuralIdentity() const
	{
		return BundleResourceCertificateIdentity(getCombinedMask().Low, getCombinedMask().High, getOwnerThreadId(), getOperationCount(), getBundleHash(), getAssistStructuralKey(), getDmaStreamComputeIdentityCount(), getDmaStreamComputeReplayEnvelopeHash());
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: public static BundleResourceCertificate Create(System.Collections.Generic.IReadOnlyList<MicroOp?> bundle, int ownerThreadId, ulong cycleNumber)
	std::shared_ptr<BundleResourceCertificate> BundleResourceCertificate::Create(const std::vector<std::shared_ptr<MicroOp>> &bundle, int ownerThreadId, unsigned long long cycleNumber)
	{
		if (bundle.empty() || bundle.empty())
		{
// C# TO C++ CONVERTER TASK: This exception's constructor requires only one argument:
// ORIGINAL LINE: throw new ArgumentException("Bundle cannot be null or empty", nameof(bundle));
			throw std::invalid_argument("Bundle cannot be null or empty");
		}

		SafetyMask128 combinedMask = SafetyMask128::Zero;
		int operationCount = 0;
		bool isSpeculative = false;
		unsigned long long assistStructuralKey = 0;
		unsigned char dmaStreamComputeIdentityCount = 0;
		unsigned long long dmaStreamComputeReplayEnvelopeHash = 0;

		// SipHash-2-4: collision-resistant integrity hash (HLS: 4-cycle latency)
		HardwareHash hasher = HardwareHash();
		hasher.Initialize();

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: foreach (MicroOp? op in bundle)
		for (auto op : bundle)
		{
			if (op == nullptr)
			{
				continue;
			}

			std::shared_ptr<MicroOpAdmissionMetadata> admission = op->getAdmissionMetadata();
			SafetyMask128 opMask = GetAdmissionCertificateMask(*admission);
			combinedMask = combinedMask | opMask;

			operationCount++;
			isSpeculative |= op->isSpeculative();
			assistStructuralKey = AccumulateAssistStructuralKey(assistStructuralKey, op);
			dmaStreamComputeIdentityCount = AccumulateDmaStreamComputeIdentityCount(dmaStreamComputeIdentityCount, op);
			dmaStreamComputeReplayEnvelopeHash = AccumulateDmaStreamComputeReplayEnvelopeHash(dmaStreamComputeReplayEnvelopeHash, op);

			hasher.Compress(static_cast<unsigned long long>(op->getOpCode()));
			hasher.Compress(opMask.Low);
			hasher.Compress(opMask.High);
			AppendAssistReplayKey(hasher, op);
			AppendDmaStreamComputeReplayKey(hasher, op);
		}

		unsigned long long bundleHash = hasher.Finalize();

		std::shared_ptr<BundleResourceCertificate> temp_var = std::make_shared<BundleResourceCertificate>();
		temp_var->setCombinedMask(combinedMask);
		temp_var->setOwnerThreadId(ownerThreadId);
		temp_var->setCycleNumber(cycleNumber);
		temp_var->setOperationCount(operationCount);
		temp_var->setSpeculative(isSpeculative);
		temp_var->setBundleHash(bundleHash);
		temp_var->setAssistStructuralKey(assistStructuralKey);
		temp_var->setDmaStreamComputeIdentityCount(dmaStreamComputeIdentityCount);
		temp_var->setDmaStreamComputeReplayEnvelopeHash(dmaStreamComputeReplayEnvelopeHash);
		return temp_var;
	}

	bool BundleResourceCertificate::NoConflictsWith(std::shared_ptr<BundleResourceCertificate> other)
	{
		if (other == nullptr)
		{
			throw std::invalid_argument("other");
		}

		// Check for resource conflicts using 128-bit mask
		return getCombinedMask().NoConflictsWith(other->getCombinedMask());
	}

	bool BundleResourceCertificate::CanInject(std::shared_ptr<MicroOp> candidate)
	{
		if (candidate == nullptr)
		{
			throw std::invalid_argument("candidate");
		}

		std::shared_ptr<MicroOpAdmissionMetadata> admission = candidate->getAdmissionMetadata();
		if ((GetSharedMask(getCombinedMask()) & GetAdmissionSharedStructuralMask(*admission)).isNonZero())
		{
			return false;
		}

		return !HasRegisterConflict(static_cast<unsigned int>(getCombinedMask().Low & RegisterBitsMaskLow32), admission->RegisterHazardMask);
	}

	bool BundleResourceCertificate::CanInjectWithReason(std::shared_ptr<MicroOp> candidate, CertificateRejectDetail &detail)
	{
		if (candidate == nullptr)
		{
			throw std::invalid_argument("candidate");
		}

		std::shared_ptr<MicroOpAdmissionMetadata> admission = candidate->getAdmissionMetadata();
		if ((GetSharedMask(getCombinedMask()) & GetAdmissionSharedStructuralMask(*admission)).isNonZero())
		{
			detail = CertificateRejectDetail::SharedResourceConflict;
			return false;
		}

		if (HasRegisterConflict(static_cast<unsigned int>(getCombinedMask().Low & RegisterBitsMaskLow32), admission->RegisterHazardMask))
		{
			detail = CertificateRejectDetail::RegisterGroupConflict;
			return false;
		}

		detail = CertificateRejectDetail::None;
		return true;
	}

	std::shared_ptr<BundleResourceCertificate> BundleResourceCertificate::WithOperation(std::shared_ptr<MicroOp> op)
	{
		if (op == nullptr)
		{
			throw std::invalid_argument("op");
		}

		std::shared_ptr<MicroOpAdmissionMetadata> admission = op->getAdmissionMetadata();
		SafetyMask128 opMask = GetAdmissionCertificateMask(*admission);

		// Chain-hash: compress previous digest with new op data
		HardwareHash hasher = HardwareHash();
		hasher.Initialize();
		hasher.Compress(getBundleHash());
		hasher.Compress(static_cast<unsigned long long>(op->getOpCode()));
		hasher.Compress(opMask.Low);
		hasher.Compress(opMask.High);
		AppendAssistReplayKey(hasher, op);
		AppendDmaStreamComputeReplayKey(hasher, op);

		std::shared_ptr<BundleResourceCertificate> temp_var = std::make_shared<BundleResourceCertificate>();
		temp_var->setCombinedMask(getCombinedMask() | opMask);
		temp_var->setOwnerThreadId(getOwnerThreadId());
		temp_var->setCycleNumber(getCycleNumber());
		temp_var->setOperationCount(getOperationCount() + 1);
		temp_var->setSpeculative(isSpeculative() || op->isSpeculative());
		temp_var->setBundleHash(hasher.Finalize());
		temp_var->setAssistStructuralKey(AccumulateAssistStructuralKey(getAssistStructuralKey(), op));
		temp_var->setDmaStreamComputeIdentityCount(AccumulateDmaStreamComputeIdentityCount(getDmaStreamComputeIdentityCount(), op));
		temp_var->setDmaStreamComputeReplayEnvelopeHash(AccumulateDmaStreamComputeReplayEnvelopeHash(getDmaStreamComputeReplayEnvelopeHash(), op));
		return temp_var;
	}

	SafetyMask128 BundleResourceCertificate::GetSharedMask(SafetyMask128 combinedMask)
	{
		return SafetyMask128(combinedMask.Low & ~RegisterBitsMaskLow32, combinedMask.High);
	}

	void BundleResourceCertificate::AppendAssistReplayKey(HardwareHash hasher, std::shared_ptr<MicroOp> op)
	{
		std::shared_ptr<AssistMicroOp> assistMicroOp = std::dynamic_pointer_cast<AssistMicroOp>(op);
		if (assistMicroOp == nullptr)
		{
			return;
		}

		// Keep the stable ownership signature in the template hash, but also
		// materialize the concrete assist tuple so same-VT and donor-VT
		// widened owners cannot collapse into one certificate identity.
		hasher.Compress(AssistOwnershipFingerprint::Compute(assistMicroOp));
		hasher.Compress(static_cast<unsigned long long>(assistMicroOp->getKind()));
		hasher.Compress(static_cast<unsigned long long>(assistMicroOp->getExecutionMode()));
		hasher.Compress(static_cast<unsigned long long>(assistMicroOp->getCarrierKind()));
		hasher.Compress(static_cast<unsigned long long>(assistMicroOp->getCarrierSlotClass()));
		hasher.Compress(static_cast<unsigned long long>(assistMicroOp->getDonorSource().getKind()));
		hasher.Compress(PackIntPair(assistMicroOp->getCarrierVirtualThreadId(), assistMicroOp->getDonorVirtualThreadId()));
		hasher.Compress(PackIntPair(assistMicroOp->getTargetVirtualThreadId(), assistMicroOp->getOwnerContextId()));
		hasher.Compress(assistMicroOp->getPlacement().DomainTag);
		hasher.Compress(PackIntPair(assistMicroOp->getDonorSource().getSourceCoreId(), assistMicroOp->getDonorSource().getOwnerContextId()));
		hasher.Compress((static_cast<unsigned long long>(assistMicroOp->getDonorSource().getSourcePodId()) << 48) | (static_cast<unsigned long long>(static_cast<unsigned short>(assistMicroOp->getPodId())) << 32) | static_cast<unsigned int>(assistMicroOp->getTargetCoreId()));
	}

	void BundleResourceCertificate::AppendDmaStreamComputeReplayKey(HardwareHash hasher, std::shared_ptr<MicroOp> op)
	{
		std::shared_ptr<DmaStreamComputeMicroOp> dmaStreamCompute = std::dynamic_pointer_cast<DmaStreamComputeMicroOp>(op);
		if (dmaStreamCompute == nullptr)
		{
			return;
		}

		hasher.Compress(dmaStreamCompute->getReplayEvidence()->EnvelopeHash);
	}

	unsigned long long BundleResourceCertificate::PackIntPair(int low, int high)
	{
//		unchecked
			return static_cast<unsigned int>(low) | (static_cast<unsigned long long>(static_cast<unsigned int>(high)) << 32);
// C# TO C++ CONVERTER TASK: End of the original C# 'unchecked' block.
	}

	unsigned long long BundleResourceCertificate::AccumulateAssistStructuralKey(unsigned long long currentKey, std::shared_ptr<MicroOp> op)
	{
		std::shared_ptr<AssistMicroOp> assistMicroOp = std::dynamic_pointer_cast<AssistMicroOp>(op);
		if (assistMicroOp == nullptr)
		{
			return currentKey;
		}

		unsigned long long hash = currentKey == 0 ? AssistStructuralKeyOffsetBasis : currentKey;
		hash = MixAssistStructuralKey(hash, AssistOwnershipFingerprint::Compute(assistMicroOp));
		hash = MixAssistStructuralKey(hash, PackIntPair(static_cast<int>(assistMicroOp->getKind()), static_cast<int>(assistMicroOp->getDonorSource().getKind())));
		hash = MixAssistStructuralKey(hash, PackIntPair(assistMicroOp->getDonorVirtualThreadId(), assistMicroOp->getTargetVirtualThreadId()));
		hash = MixAssistStructuralKey(hash, PackIntPair(assistMicroOp->getCarrierCoreId(), assistMicroOp->getTargetCoreId()));
		hash = MixAssistStructuralKey(hash, assistMicroOp->getPlacement().DomainTag);
		hash = MixAssistStructuralKey(hash, PackIntPair(assistMicroOp->getDonorSource().getSourceCoreId(), assistMicroOp->getOwnerContextId()));
		hash = MixAssistStructuralKey(hash, (static_cast<unsigned long long>(assistMicroOp->getDonorSource().getSourcePodId()) << 48) | (static_cast<unsigned long long>(static_cast<unsigned short>(assistMicroOp->getPodId())) << 32) | static_cast<unsigned int>(assistMicroOp->getDonorSource().getOwnerContextId()));
		return hash;
	}

	unsigned char BundleResourceCertificate::AccumulateDmaStreamComputeIdentityCount(unsigned char currentCount, std::shared_ptr<MicroOp> op)
	{
		if (std::dynamic_pointer_cast<DmaStreamComputeMicroOp>(op) == nullptr)
		{
			return currentCount;
		}

		return currentCount == std::numeric_limits<unsigned char>::max() ? std::numeric_limits<unsigned char>::max() : static_cast<unsigned char>(currentCount + 1);
	}

	unsigned long long BundleResourceCertificate::AccumulateDmaStreamComputeReplayEnvelopeHash(unsigned long long currentHash, std::shared_ptr<MicroOp> op)
	{
		std::shared_ptr<DmaStreamComputeMicroOp> dmaStreamCompute = std::dynamic_pointer_cast<DmaStreamComputeMicroOp>(op);
		if (dmaStreamCompute == nullptr)
		{
			return currentHash;
		}

		HardwareHash hasher = HardwareHash();
		hasher.Initialize();
		hasher.Compress(currentHash);
		hasher.Compress(dmaStreamCompute->getReplayEvidence()->EnvelopeHash);
		unsigned long long hash = hasher.Finalize();
		return hash == 0 ? 0xD5C0'BA5EULL : hash;
	}

	unsigned long long BundleResourceCertificate::MixAssistStructuralKey(unsigned long long hash, unsigned long long value)
	{
		for (int shift = 0; shift < 64; shift += 8)
		{
			hash ^= static_cast<unsigned char>((value >> shift) & 0xFF);
			hash *= AssistStructuralKeyPrime;
		}

		return hash;
	}

	bool BundleResourceCertificate::HasRegisterConflict(unsigned int bundleRegMask, unsigned int candidateRegMask)
	{
		unsigned short candRead = static_cast<unsigned short>(candidateRegMask & 0xFFFF);
		unsigned short candWrite = static_cast<unsigned short>(candidateRegMask >> 16);
		unsigned short bundleRead = static_cast<unsigned short>(bundleRegMask & 0xFFFF);
		unsigned short bundleWrite = static_cast<unsigned short>(bundleRegMask >> 16);

		return ((candRead & bundleWrite) | (candWrite & bundleRead) | (candWrite & bundleWrite)) != 0;
	}

	bool BundleResourceCertificate::IsValid()
	{
		// Certificate is valid if:
		// 1. Combined mask is non-zero (at least one resource used)
		// 2. Operation count is positive
		// 3. Owner thread ID is in valid range [0, 15]
		return getCombinedMask().isNonZero() && getOperationCount() > 0 && getOwnerThreadId() >= 0 && getOwnerThreadId() < 16;
	}

	std::wstring BundleResourceCertificate::ToString()
	{
		return std::format(L"BundleResourceCertificate(Thread={}, Ops={}, Mask={}, Cycle={}, Hash=0x{:016X}, Speculative={})", getOwnerThreadId(), getOperationCount(), getCombinedMask().ToString(), getCycleNumber(), getBundleHash(), isSpeculative());
	}
}
