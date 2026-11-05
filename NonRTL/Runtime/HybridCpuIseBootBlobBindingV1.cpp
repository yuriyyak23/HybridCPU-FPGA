#include "HybridCpuIseBootBlobBindingV1.h"

#include <algorithm>
#include <array>
#include <limits>
#include <span>

using namespace HybridCPU::ManagedRuntime;
using namespace HybridCPU::Platform::Contracts;
using namespace HybridCPU::RuntimeKernel;
using namespace HybridCPU_ISE::CloseToHSL::Core::Runtime::Managed;

namespace HybridCPU_ISE::NonRTL::Runtime
{
	std::shared_ptr<HybridCpuManagedBootBlobProviderResultV1> HybridCpuIseBootBlobBindingV1::Resolve(int blobId)
	{
		if (_registry.has_value() && *_registry)
		{
			return (*_registry)->Resolve(blobId);
		}
		return std::make_shared<HybridCpuManagedBootBlobProviderResultV1>(false, 0, 0, L"", L"Boot blob has not been materialized and rooted.");
	}

	bool HybridCpuIseBootBlobBindingV1::TryBind(std::shared_ptr<HybridCpuIseManagedImageLoadResultV1> load,
		int blobId, std::span<const unsigned char> bytes,
		std::shared_ptr<HybridCpuIseManagedImageLoadResultV1> &bound, std::wstring &reason)
	{
		bound = load;
		reason = L"Boot blob binding requires a fresh successful loader, a valid id and bounded nonempty bytes.";
		if (_attempted || !load)
		{
			return false;
		}
		_attempted = true;

		std::shared_ptr<HybridCpuManagedTypeSystemV1> types = load->TypeSystem;
		std::shared_ptr<HybridCpuManagedHeapAllocatorV1> heap = load->Heap;
		if (!load->IsSuccess || !types || !heap || !load->Gc || (load->ProcessRoots != nullptr && !load->ProcessRoots->empty()) ||
			blobId <= 0 || blobId > HybridCpuBootBlobServiceContractV1::MaximumBlobId || bytes.empty() ||
			bytes.size() > HybridCpuBootBlobServiceContractV1::MaximumBlobBytes)
		{
			return false;
		}

		std::vector<std::shared_ptr<HybridCpuManagedTypeDescriptorV1>> matches;
		for (const std::shared_ptr<HybridCpuManagedTypeDescriptorV1> &candidate : types->Descriptors)
		{
			if (candidate && candidate->StableIdentity == L"System.Byte[]")
			{
				matches.push_back(candidate);
			}
		}
		reason = L"Boot blob requires one exact image-owned System.Byte[] descriptor.";
		if (matches.size() != 1)
		{
			return false;
		}

		const std::shared_ptr<HybridCpuManagedTypeDescriptorV1> &type = matches.front();
		const auto shape = type->ArrayShape;
		const auto handle = types->TypeHandle(type->TypeId);
		if (type->Kind != HybridCpuManagedTypeKindV1::SzArray || !shape || !shape->IsSzArray ||
			shape->ElementStorageKind != HybridCpuManagedStorageKindV1::Primitive ||
			shape->ElementSizeBytes != 1 || shape->ElementAlignmentBytes != 1 || !handle.has_value())
		{
			return false;
		}

		if (shape->DataOffsetBytes < 0 || bytes.size() > static_cast<std::size_t>(std::numeric_limits<int>::max()) ||
			bytes.size() > static_cast<std::size_t>(std::numeric_limits<int>::max() - shape->DataOffsetBytes))
		{
			reason = L"Boot blob array extent overflows.";
			return false;
		}
		const int size = std::max(type->InstanceSizeBytes, shape->DataOffsetBytes + static_cast<int>(bytes.size()));
		std::shared_ptr<HybridCpuManagedHeapResultV1> allocation = heap->TryAllocateVariable(*handle, size);
		if (!allocation || !allocation->IsSuccess)
		{
			reason = allocation ? allocation->Reason : L"Boot blob heap allocation returned no result.";
			return false;
		}

		std::array<unsigned char, sizeof(int)> length{};
		const unsigned int byteLength = static_cast<unsigned int>(bytes.size());
		for (std::size_t index = 0; index < length.size(); ++index)
		{
			length[index] = static_cast<unsigned char>((byteLength >> (index * 8)) & 0xffu);
		}
		const auto lengthWrite = heap->WriteObjectBytes(allocation->ObjectReference, shape->LengthOffsetBytes,
			std::span<const unsigned char>(length));
		const auto dataWrite = heap->WriteObjectBytes(allocation->ObjectReference, shape->DataOffsetBytes, bytes);
		if (!lengthWrite.IsSuccess || !dataWrite.IsSuccess)
		{
			reason = L"Boot blob heap copy failed; no reference was published.";
			return false;
		}

		std::shared_ptr<HybridCpuManagedGcRootRegistryV1> roots = std::make_shared<HybridCpuManagedGcRootRegistryV1>();
		std::shared_ptr<HybridCpuManagedBootBlobRegistryV1> registry =
			std::make_shared<HybridCpuManagedBootBlobRegistryV1>(types, heap, roots);
		if (!registry->Register(blobId, allocation->ObjectReference))
		{
			reason = L"Boot blob process root registration failed.";
			return false;
		}

		bound = std::make_shared<HybridCpuIseManagedImageLoadResultV1>(*load);
		auto rootSnapshot = roots->Snapshot();
bound->ProcessRoots = std::make_shared<decltype(rootSnapshot)>(std::move(rootSnapshot));
		_registry = registry;
		reason.clear();
		return true;
	}
}
