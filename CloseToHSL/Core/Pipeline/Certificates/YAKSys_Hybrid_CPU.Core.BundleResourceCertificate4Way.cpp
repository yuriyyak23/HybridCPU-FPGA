#include "YAKSys_Hybrid_CPU.Core.BundleResourceCertificate4Way.h"
#include "BundleResourceCertificate4Way.h"
#include "../MicroOps/Lane6DmaStream/DmaStreamComputeMicroOp.h"
#include "../Metadata/MicroOpAdmissionMetadata.h"
#include "StructuralIdentity/HardwareHash.h"

namespace YAKSys_Hybrid_CPU::Core
{
	namespace
	{
		constexpr unsigned long long AdmissionRegisterBitsMaskLow32 = 0xFFFF'FFFFULL;

		SafetyMask128 GetSharedStructuralMask(const MicroOpAdmissionMetadata &admission)
		{
			return SafetyMask128(admission.StructuralSafetyMask.Low & ~AdmissionRegisterBitsMaskLow32, admission.StructuralSafetyMask.High);
		}
	}

	BundleResourceCertificateIdentity4Way BundleResourceCertificate4Way::getStructuralIdentity() const
	{
		return BundleResourceCertificateIdentity4Way::Create(*this);
	}

	bool BundleResourceCertificate4Way::CanInject(std::shared_ptr<MicroOp> candidate)
	{
		ArgumentNullException::ThrowIfNull(candidate);

		std::shared_ptr<MicroOpAdmissionMetadata> admission = candidate->getAdmissionMetadata();

		// Stage 1: Shared resource check (bits 32–127).
		SafetyMask128 candidateShared = GetSharedStructuralMask(*admission);

		if ((SharedMask & candidateShared).isNonZero())
		{
			return false;
		}

		// Stage 2: Per-thread register group check (bits 0–31) — RAR-aware.
		unsigned int candidateReg = admission->RegisterHazardMask;
		const unsigned int bundleReg = GetRegMask(candidate->getVirtualThreadId());

		// Split 32-bit mask into read-half (bits 0–15) and write-half (bits 16–31).
		// Conflict only on true hazards: RAW, WAR, WAW. RAR is not a hazard.
		unsigned short candRead = static_cast<unsigned short>(candidateReg & 0xFFFF);
		unsigned short candWrite = static_cast<unsigned short>(candidateReg >> 16);
		unsigned short bndRead = static_cast<unsigned short>(bundleReg & 0xFFFF);
		unsigned short bndWrite = static_cast<unsigned short>(bundleReg >> 16);

		return ((candRead & bndWrite) | (candWrite & bndRead) | (candWrite & bndWrite)) == 0; // WAW: both write same group
	}

	void BundleResourceCertificate4Way::AddOperation(std::shared_ptr<MicroOp> op)
	{
		ArgumentNullException::ThrowIfNull(op);

		std::shared_ptr<MicroOpAdmissionMetadata> admission = op->getAdmissionMetadata();

		// Accumulate shared resources (bits 32–127).
		SafetyMask128 opShared = GetSharedStructuralMask(*admission);
		SharedMask = SharedMask | opShared;

		// Accumulate per-thread register groups (bits 0–31).
		unsigned int opReg = admission->RegisterHazardMask;
		switch (op->getVirtualThreadId())
		{
			case 0:
				RegMaskVT0 |= opReg;
				break;
			case 1:
				RegMaskVT1 |= opReg;
				break;
			case 2:
				RegMaskVT2 |= opReg;
				break;
			default:
				RegMaskVT3 |= opReg;
				break;
		}

		// Phase 05: update class occupancy
		ClassOccupancy.IncrementOccupancy(op->getPlacement().RequiredSlotClass);
		AddDmaStreamComputeIdentityIfNeeded(op);

		OperationCount++;
	}

	unsigned int BundleResourceCertificate4Way::GetRegMask(int virtualThreadId)
	{
		switch (virtualThreadId)
		{
		case 0: return RegMaskVT0;
		case 1: return RegMaskVT1;
		case 2: return RegMaskVT2;
		default: return RegMaskVT3;
		}
	}

	bool BundleResourceCertificate4Way::CanInjectWithReason(std::shared_ptr<MicroOp> candidate, CertificateRejectDetail &detail)
	{
		ArgumentNullException::ThrowIfNull(candidate);

		std::shared_ptr<MicroOpAdmissionMetadata> admission = candidate->getAdmissionMetadata();

		detail = CertificateRejectDetail::None;

		// Stage 1: Shared resource check (bits 32–127).
		SafetyMask128 candidateShared = GetSharedStructuralMask(*admission);

		if ((SharedMask & candidateShared).isNonZero())
		{
			detail = CertificateRejectDetail::SharedResourceConflict;
			return false;
		}

		// Stage 2: Per-thread register group check (bits 0–31) — RAR-aware.
		unsigned int candidateReg = admission->RegisterHazardMask;
		const unsigned int bundleReg = GetRegMask(candidate->getVirtualThreadId());

		unsigned short candRead = static_cast<unsigned short>(candidateReg & 0xFFFF);
		unsigned short candWrite = static_cast<unsigned short>(candidateReg >> 16);
		unsigned short bndRead = static_cast<unsigned short>(bundleReg & 0xFFFF);
		unsigned short bndWrite = static_cast<unsigned short>(bundleReg >> 16);

		if (((candRead & bndWrite) | (candWrite & bndRead) | (candWrite & bndWrite)) != 0)
		{
			detail = CertificateRejectDetail::RegisterGroupConflict;
			return false;
		}

		return true;
	}

	void BundleResourceCertificate4Way::AddDmaStreamComputeIdentityIfNeeded(std::shared_ptr<MicroOp> op)
	{
		std::shared_ptr<DmaStreamComputeMicroOp> dmaStreamCompute = std::dynamic_pointer_cast<DmaStreamComputeMicroOp>(op);
		if (dmaStreamCompute == nullptr)
		{
			return;
		}

		if (DmaStreamComputeIdentityCount == 0)
		{
			DmaStreamComputeIdentityCount = 1;
			DmaStreamComputeDescriptorIdentityHash = dmaStreamCompute->getDescriptorIdentityHash();
			DmaStreamComputeNormalizedFootprintHash = dmaStreamCompute->getNormalizedFootprintHash();
			DmaStreamComputeReplayEnvelopeHash = dmaStreamCompute->getReplayEvidence()->EnvelopeHash;
			return;
		}

		HardwareHash hasher = HardwareHash();
		hasher.Initialize();
		hasher.Compress(DmaStreamComputeDescriptorIdentityHash);
		hasher.Compress(DmaStreamComputeNormalizedFootprintHash);
		hasher.Compress(DmaStreamComputeReplayEnvelopeHash);
		hasher.Compress(dmaStreamCompute->getDescriptorIdentityHash());
		hasher.Compress(dmaStreamCompute->getNormalizedFootprintHash());
		hasher.Compress(dmaStreamCompute->getReplayEvidence()->EnvelopeHash);

		DmaStreamComputeIdentityCount = DmaStreamComputeIdentityCount == std::numeric_limits<unsigned char>::max() ? std::numeric_limits<unsigned char>::max() : static_cast<unsigned char>(DmaStreamComputeIdentityCount + 1);
		DmaStreamComputeDescriptorIdentityHash = hasher.Finalize();
		DmaStreamComputeNormalizedFootprintHash ^= dmaStreamCompute->getNormalizedFootprintHash();
		DmaStreamComputeReplayEnvelopeHash ^= dmaStreamCompute->getReplayEvidence()->EnvelopeHash;
	}

	std::wstring BundleResourceCertificate4Way::ToString()
	{
		return std::format(L"Cert4Way(Ops={}, Shared={}, VT0=0x{:08X}, VT1=0x{:08X}, VT2=0x{:08X}, VT3=0x{:08X})",
			OperationCount, SharedMask.ToString(), RegMaskVT0, RegMaskVT1, RegMaskVT2, RegMaskVT3);
	}
}
