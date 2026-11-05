#include "BundleResourceCertificate4Way.h"

namespace YAKSys_Hybrid_CPU::Core
{
    const unsigned short BundleResourceCertificateIdentity4Way::CurrentLaneTopologySignature = BundleResourceCertificateIdentity4Way::ComputeLaneTopologySignature();

	BundleResourceCertificateIdentity4Way::BundleResourceCertificateIdentity4Way(unsigned long long bundleShapeId, unsigned int classVector, unsigned short laneTopologySignature, unsigned char resourceProfile, int operationCount, unsigned char dmaStreamComputeIdentityCount, unsigned long long dmaStreamComputeDescriptorIdentityHash, unsigned long long dmaStreamComputeNormalizedFootprintHash, unsigned long long dmaStreamComputeReplayEnvelopeHash)
	{
		BundleShapeId = bundleShapeId;
		ClassVector = classVector;
		LaneTopologySignature = laneTopologySignature;
		ResourceProfile = resourceProfile;
		OperationCount = operationCount;
		DmaStreamComputeIdentityCount = dmaStreamComputeIdentityCount;
		DmaStreamComputeDescriptorIdentityHash = dmaStreamComputeDescriptorIdentityHash;
		DmaStreamComputeNormalizedFootprintHash = dmaStreamComputeNormalizedFootprintHash;
		DmaStreamComputeReplayEnvelopeHash = dmaStreamComputeReplayEnvelopeHash;
	}

	unsigned long long BundleResourceCertificateIdentity4Way::getBundleShapeId() const
	{
		return BundleShapeId;
	}

	unsigned int BundleResourceCertificateIdentity4Way::getClassVector() const
	{
		return ClassVector;
	}

	unsigned short BundleResourceCertificateIdentity4Way::getLaneTopologySignature() const
	{
		return LaneTopologySignature;
	}

	unsigned char BundleResourceCertificateIdentity4Way::getResourceProfile() const
	{
		return ResourceProfile;
	}

	int BundleResourceCertificateIdentity4Way::getOperationCount() const
	{
		return OperationCount;
	}

	unsigned char BundleResourceCertificateIdentity4Way::getDmaStreamComputeIdentityCount() const
	{
		return DmaStreamComputeIdentityCount;
	}

	unsigned long long BundleResourceCertificateIdentity4Way::getDmaStreamComputeDescriptorIdentityHash() const
	{
		return DmaStreamComputeDescriptorIdentityHash;
	}

	unsigned long long BundleResourceCertificateIdentity4Way::getDmaStreamComputeNormalizedFootprintHash() const
	{
		return DmaStreamComputeNormalizedFootprintHash;
	}

	unsigned long long BundleResourceCertificateIdentity4Way::getDmaStreamComputeReplayEnvelopeHash() const
	{
		return DmaStreamComputeReplayEnvelopeHash;
	}

	bool BundleResourceCertificateIdentity4Way::isValid() const
	{
		return getLaneTopologySignature() != 0 && getOperationCount() >= 0;
	}

	BundleResourceCertificateIdentity4Way BundleResourceCertificateIdentity4Way::Create(BundleResourceCertificate4Way const certificate)
	{
		unsigned int classVector = EncodeClassVector(certificate.ClassOccupancy);
		unsigned char resourceProfile = ComputeResourceProfile(certificate);

		HardwareHash hasher = HardwareHash();
		hasher.Initialize();
		hasher.Compress(certificate.SharedMask.Low);
		hasher.Compress(certificate.SharedMask.High);
		hasher.Compress((static_cast<unsigned long long>(certificate.RegMaskVT1) << 32) | certificate.RegMaskVT0);
		hasher.Compress((static_cast<unsigned long long>(certificate.RegMaskVT3) << 32) | certificate.RegMaskVT2);
		hasher.Compress(classVector);
		hasher.Compress((static_cast<unsigned long long>(CurrentLaneTopologySignature) << 32) | resourceProfile);
		hasher.Compress(static_cast<unsigned long long>(certificate.OperationCount));
		if (certificate.DmaStreamComputeIdentityCount != 0)
		{
			hasher.Compress(certificate.DmaStreamComputeIdentityCount);
			hasher.Compress(certificate.DmaStreamComputeDescriptorIdentityHash);
			hasher.Compress(certificate.DmaStreamComputeNormalizedFootprintHash);
			hasher.Compress(certificate.DmaStreamComputeReplayEnvelopeHash);
		}

		return BundleResourceCertificateIdentity4Way(hasher.Finalize(), classVector, CurrentLaneTopologySignature, resourceProfile, certificate.OperationCount, certificate.DmaStreamComputeIdentityCount, certificate.DmaStreamComputeDescriptorIdentityHash, certificate.DmaStreamComputeNormalizedFootprintHash, certificate.DmaStreamComputeReplayEnvelopeHash);
	}

	bool BundleResourceCertificateIdentity4Way::Equals(BundleResourceCertificateIdentity4Way other) const
	{
		return getBundleShapeId() == other.getBundleShapeId() && getClassVector() == other.getClassVector() && getLaneTopologySignature() == other.getLaneTopologySignature() && getResourceProfile() == other.getResourceProfile() && getOperationCount() == other.getOperationCount() && getDmaStreamComputeIdentityCount() == other.getDmaStreamComputeIdentityCount() && getDmaStreamComputeDescriptorIdentityHash() == other.getDmaStreamComputeDescriptorIdentityHash() && getDmaStreamComputeNormalizedFootprintHash() == other.getDmaStreamComputeNormalizedFootprintHash() && getDmaStreamComputeReplayEnvelopeHash() == other.getDmaStreamComputeReplayEnvelopeHash();
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public override bool Equals(Object? obj)
	bool BundleResourceCertificateIdentity4Way::Equals(std::any obj) const
	{
		const auto *other = std::any_cast<BundleResourceCertificateIdentity4Way>(&obj);
		return other != nullptr && Equals(*other);
	}

	int BundleResourceCertificateIdentity4Way::GetHashCode() const
	{
//		unchecked
			int hash = static_cast<int>(0x5B4D8A13);
			hash = (hash * 397) ^ static_cast<int>(getBundleShapeId());
			hash = (hash * 397) ^ static_cast<int>(getClassVector());
			hash = (hash * 397) ^ getLaneTopologySignature();
			hash = (hash * 397) ^ getResourceProfile();
			hash = (hash * 397) ^ getOperationCount();
			hash = (hash * 397) ^ static_cast<int>(getDmaStreamComputeIdentityCount());
			hash = (hash * 397) ^ static_cast<int>(getDmaStreamComputeDescriptorIdentityHash());
			hash = (hash * 397) ^ static_cast<int>(getDmaStreamComputeNormalizedFootprintHash());
			hash = (hash * 397) ^ static_cast<int>(getDmaStreamComputeReplayEnvelopeHash());
			return hash;
// C# TO C++ CONVERTER TASK: End of the original C# 'unchecked' block.
	}

	std::wstring BundleResourceCertificateIdentity4Way::ToString() const
	{
		std::wstring dmaStreamSuffix;
		if (getDmaStreamComputeIdentityCount() != 0)
		{
			dmaStreamSuffix = std::format(L", DmaDesc=0x{:016X}, DmaFootprint=0x{:016X}, DmaIds={}", getDmaStreamComputeDescriptorIdentityHash(), getDmaStreamComputeNormalizedFootprintHash(), getDmaStreamComputeIdentityCount());
		}

		return std::format(L"Cert4WayIdentity(Shape=0x{:016X}, Class=0x{:05X}, Topo=0x{:04X}, Profile=0x{:02X}, Ops={}{})", getBundleShapeId(), getClassVector(), getLaneTopologySignature(), getResourceProfile(), getOperationCount(), dmaStreamSuffix);
	}

	unsigned int BundleResourceCertificateIdentity4Way::EncodeClassVector(SlotClassCapacityState state)
	{
		return static_cast<unsigned int>(state.AluOccupied) | (static_cast<unsigned int>(state.LsuOccupied) << OCCUPANCY_BITS_PER_CLASS) | (static_cast<unsigned int>(state.DmaStreamOccupied) << (OCCUPANCY_BITS_PER_CLASS * 2)) | (static_cast<unsigned int>(state.MatrixTileStreamOccupied) << (OCCUPANCY_BITS_PER_CLASS * 3)) | (static_cast<unsigned int>(state.BranchControlOccupied) << (OCCUPANCY_BITS_PER_CLASS * 4)) | (static_cast<unsigned int>(state.SystemSingletonOccupied) << (OCCUPANCY_BITS_PER_CLASS * 5));
	}

	unsigned char BundleResourceCertificateIdentity4Way::ComputeResourceProfile(BundleResourceCertificate4Way const certificate)
	{
		unsigned char profile = 0;
		if (certificate.SharedMask.isNonZero())
		{
			profile |= 1 << 0;
		}
		if (certificate.RegMaskVT0 != 0)
		{
			profile |= 1 << 1;
		}
		if (certificate.RegMaskVT1 != 0)
		{
			profile |= 1 << 2;
		}
		if (certificate.RegMaskVT2 != 0)
		{
			profile |= 1 << 3;
		}
		if (certificate.RegMaskVT3 != 0)
		{
			profile |= 1 << 4;
		}
		if (certificate.DmaStreamComputeIdentityCount != 0)
		{
			profile |= 1 << 5;
		}
		return profile;
	}

	unsigned short BundleResourceCertificateIdentity4Way::ComputeLaneTopologySignature()
	{
		HardwareHash hasher = HardwareHash();
		hasher.Initialize();
		CompressClass(hasher, SlotClass::AluClass);
		CompressClass(hasher, SlotClass::LsuClass);
		CompressClass(hasher, SlotClass::DmaStreamClass);
		CompressClass(hasher, SlotClass::MatrixTileStreamClass);
		CompressClass(hasher, SlotClass::BranchControl);
		CompressClass(hasher, SlotClass::SystemSingleton);
		return static_cast<unsigned short>(hasher.Finalize() & 0xFFFF);
	}

	void BundleResourceCertificateIdentity4Way::CompressClass(HardwareHash &hasher, SlotClass slotClass)
	{
		hasher.Compress(static_cast<unsigned long long>(static_cast<unsigned char>(slotClass)));
		hasher.Compress(static_cast<unsigned long long>(SlotClassLaneMap::GetLaneMask(slotClass)));
		hasher.Compress(static_cast<unsigned long long>(SlotClassLaneMap::GetClassCapacity(slotClass)));
	}
}
