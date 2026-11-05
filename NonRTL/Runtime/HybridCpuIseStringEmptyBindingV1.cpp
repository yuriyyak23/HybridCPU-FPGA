#include "HybridCpuIseStringEmptyBindingV1.h"

#include <algorithm>
#include <limits>
#include <stdexcept>

namespace HybridCPU::ManagedRuntime
{
	HybridCpuManagedStringRuntimeV1::HybridCpuManagedStringRuntimeV1(
		std::shared_ptr<HybridCpuManagedTypeSystemV1> types,
		std::shared_ptr<HybridCpuManagedHeapAllocatorV1> heap)
		: _types(std::move(types)), _heap(std::move(heap))
	{
		if (!_types || !_heap)
			throw std::invalid_argument("types and heap are required");
	}

	std::shared_ptr<HybridCpuManagedShapeResultV1> HybridCpuManagedStringRuntimeV1::MaterializeLiteral(
		unsigned long long typeHandle, const std::wstring& value)
	{
		const std::wstring key = HybridCpuManagedGcRuntimeContractV1::Hash(
			L"utf16-literal/v1|" + std::to_wstring(typeHandle) + L"|" + value);
		if (const auto existing = _literals.find(key); existing != _literals.end())
			return std::make_shared<HybridCpuManagedShapeResultV1>(
				HybridCpuManagedShapeStatusV1::Success, L"", existing->second,
				static_cast<long long>(value.size()));

		auto type = _types->ResolveTypeHandle(typeHandle);
		if (!type || type->Kind != HybridCPU::Platform::Contracts::HybridCpuManagedTypeKindV1::String ||
			!type->StringShape || type->StringShape->CharacterSizeBytes != 2 || !type->StringShape->IsImmutable)
			return std::make_shared<HybridCpuManagedShapeResultV1>(HybridCpuManagedShapeStatusV1::InvalidType,
				L"ldstr requires an exact immutable UTF-16 string descriptor.");

		const auto& shape = *type->StringShape;
		if (shape.DataOffsetBytes < 0 || shape.LengthOffsetBytes < 0 ||
			value.size() > static_cast<std::size_t>((std::numeric_limits<int>::max)() / 2))
			return std::make_shared<HybridCpuManagedShapeResultV1>(HybridCpuManagedShapeStatusV1::SizeOverflow,
				L"String byte size overflowed checked V1 bounds.");
		const std::size_t payloadSize = value.size() * 2;
		if (payloadSize > static_cast<std::size_t>((std::numeric_limits<int>::max)() - shape.DataOffsetBytes))
			return std::make_shared<HybridCpuManagedShapeResultV1>(HybridCpuManagedShapeStatusV1::SizeOverflow,
				L"String byte size overflowed checked V1 bounds.");

		std::vector<unsigned char> payload(payloadSize);
		for (std::size_t index = 0; index < value.size(); ++index)
		{
			const auto codeUnit = static_cast<unsigned short>(value[index]);
			payload[index * 2] = static_cast<unsigned char>(codeUnit & 0xffu);
			payload[index * 2 + 1] = static_cast<unsigned char>((codeUnit >> 8) & 0xffu);
		}

		const int objectSize = (std::max)(type->InstanceSizeBytes,
			shape.DataOffsetBytes + static_cast<int>(payloadSize));
		auto allocation = _heap->TryAllocateStringVariable(typeHandle, objectSize);
		if (!allocation || !allocation->IsSuccess)
			return std::make_shared<HybridCpuManagedShapeResultV1>(HybridCpuManagedShapeStatusV1::HeapFailure,
				allocation ? allocation->Reason : L"String heap allocation returned no result.");

		std::array<unsigned char, sizeof(int)> encodedLength{};
		const unsigned int length = static_cast<unsigned int>(value.size());
		for (std::size_t index = 0; index < encodedLength.size(); ++index)
			encodedLength[index] = static_cast<unsigned char>((length >> (index * 8)) & 0xffu);
		const auto lengthWrite = _heap->WriteObjectBytes(allocation->ObjectReference, shape.LengthOffsetBytes,
			std::span<const unsigned char>(encodedLength));
		const auto payloadWrite = _heap->WriteObjectBytes(allocation->ObjectReference, shape.DataOffsetBytes,
			std::span<const unsigned char>(payload));
		if (!lengthWrite.IsSuccess || !payloadWrite.IsSuccess)
			return std::make_shared<HybridCpuManagedShapeResultV1>(HybridCpuManagedShapeStatusV1::HeapFailure,
				L"String payload write failed.");

		_literals.emplace(key, allocation->ObjectReference);
		return std::make_shared<HybridCpuManagedShapeResultV1>(HybridCpuManagedShapeStatusV1::Success,
			L"", allocation->ObjectReference, static_cast<long long>(value.size()));
	}
}

namespace HybridCPU_ISE::NonRTL::Runtime
{
using HybridCPU::ManagedRuntime::HybridCpuManagedShapeResultV1;
using HybridCPU::ManagedRuntime::HybridCpuManagedStringRuntimeV1;
using HybridCPU::ManagedRuntime::HybridCpuManagedTypeSystemV1;
using HybridCPU::Platform::Contracts::HybridCpuManagedTypeDescriptorV1;
using HybridCPU::Platform::Contracts::HybridCpuManagedTypeInitializationStateV1;
using HybridCPU::Platform::Contracts::HybridCpuManagedTypeKindV1;
using HybridCPU::Platform::Contracts::HybridCpuManagedStorageKindV1;
using HybridCPU::Platform::Contracts::HybridCpuModuleInitializerRegistrationV1;

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: public static bool TryInitialize(HybridCpuManagedTypeSystemV1 types, HybridCpuManagedStringRuntimeV1 strings, IReadOnlyList<HybridCpuModuleInitializerRegistrationV1> initializers, out string reason)
	bool HybridCpuIseStringEmptyBindingV1::TryInitialize(
		std::shared_ptr<HybridCpuManagedTypeSystemV1> types,
		std::shared_ptr<HybridCpuManagedStringRuntimeV1> strings,
		const std::vector<std::shared_ptr<HybridCpuModuleInitializerRegistrationV1>>& initializers,
		std::wstring& reason)
	{
		if (!types || !strings)
			throw std::invalid_argument("types and strings are required");
		reason.clear();
		std::vector<std::shared_ptr<HybridCpuManagedTypeDescriptorV1>> candidates;
		for (const auto& candidate : types->Descriptors)
			if (candidate && candidate->StableIdentity == L"System.String")
				candidates.push_back(candidate);
		if (candidates.empty())
		{
			return true;
		}
		if (candidates.size() != 1)
		{
			reason = L"String.Empty requires one exact image-owned System.String descriptor.";
			return false;
		}
		std::shared_ptr<HybridCpuManagedTypeDescriptorV1> type = candidates[0];
		const auto& layout = type->StaticLayout;
		if (!layout || !layout->Fields)
		{
			reason = L"System.String descriptor is missing its static layout.";
			return false;
		}
		if (layout->Fields->empty())
		{
			return true;
		}
		const auto& field = layout->Fields->front();
		const auto handle = types->TypeHandle(type->TypeId);
		if (type->Kind != HybridCpuManagedTypeKindV1::String || !type->StringShape ||
			type->StringShape->LengthOffsetBytes != 16 || type->StringShape->DataOffsetBytes != 20 ||
			type->StringShape->CharacterSizeBytes != 2 || !type->StringShape->IsImmutable ||
			layout->SizeBytes != 8 || layout->AlignmentBytes != 8 || layout->Fields->size() != 1 || !field ||
			field->Identity != L"Empty" || field->StorageKind != HybridCpuManagedStorageKindV1::ObjectReference ||
			field->OffsetBytes != 0 || field->SizeBytes != 8 || field->AlignmentBytes != 8 ||
			!layout->ObjectReferenceOffsets || layout->ObjectReferenceOffsets->size() != 1 ||
			layout->ObjectReferenceOffsets->front() != 0 || !handle)
		{
			reason = L"String.Empty requires the exact immutable UTF-16 shape and single pointer-sized static root layout.";
			return false;
		}
		if (std::any_of(initializers.begin(), initializers.end(), [&](const auto& row) {
			return row && row->TypeId && *row->TypeId == type->TypeId;
		}))
		{
			reason = L"Runtime-preinitialized String.Empty cannot bypass an image initializer for System.String.";
			return false;
		}
		if (types->InitializationState(type->TypeId) != HybridCpuManagedTypeInitializationStateV1::Uninitialized)
		{
			reason = L"String.Empty binding requires a fresh uninitialized runtime type state.";
			return false;
		}

		if (!types->TryBeginInitialization(type->TypeId))
		{
			reason = L"String.Empty type initialization transition was rejected.";
			return false;
		}

		bool initialized = false;
		try
		{
			auto empty = strings->MaterializeLiteral(*handle, L"");
			if (!empty || !empty->IsSuccess || empty->ObjectReference == 0)
				reason = L"String.Empty materialization failed: " + (empty ? empty->Reason : L"no result");
			else if (!types->TryStoreStaticReference(*handle, 0, empty->ObjectReference))
				reason = L"String.Empty static root binding failed: exact static reference field was rejected.";
			else
				initialized = true;
		}
		catch (const std::exception& exception)
		{
			reason = L"String.Empty initialization threw: ";
			const std::string message = exception.what();
			reason.append(message.begin(), message.end());
		}

		if (!types->TryCompleteInitialization(type->TypeId, initialized))
		{
			reason = L"String.Empty type initialization completion transition was rejected.";
			return false;
		}
		return initialized;
	}
}
