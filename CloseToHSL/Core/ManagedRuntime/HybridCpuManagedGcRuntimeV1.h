#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <deque>
#include <set>
#include <optional>
#include <format>
#include <memory>
#include <utility>
#include "../../../tangible_string_helper.h"

namespace HybridCPU::ManagedRuntime
{
	enum class HybridCpuManagedNonMovingGcStatusV1 : unsigned char
	{
		Collected = 0, Disabled = 1, Unsupported = 2, InvalidMetadata = 3,
		InvalidState = 4, BudgetExhausted = 5, HeapFailure = 6
	};

	enum class HybridCpuManagedRetiredSafepointStatusV1 : unsigned char
	{
		NotSafepoint = 0, Collected = 1, Rejected = 2
	};

	struct HybridCpuManagedNonMovingGcResultV1 final
	{
		HybridCpuManagedNonMovingGcStatusV1 Status;
		std::wstring Reason;
		std::vector<unsigned long long> Roots;
		std::vector<unsigned long long> ReachableObjects;
		std::vector<unsigned long long> ReclaimedObjects;
		std::wstring ResultDigest;
		bool IsSuccess;

		HybridCpuManagedNonMovingGcResultV1(HybridCpuManagedNonMovingGcStatusV1 status, std::wstring reason,
			std::vector<unsigned long long> roots, std::vector<unsigned long long> reachable,
			std::vector<unsigned long long> reclaimed, std::wstring digest)
			: Status(status), Reason(std::move(reason)), Roots(std::move(roots)), ReachableObjects(std::move(reachable)),
			  ReclaimedObjects(std::move(reclaimed)), ResultDigest(std::move(digest)),
			  IsSuccess(status == HybridCpuManagedNonMovingGcStatusV1::Collected) {}
	};

	struct HybridCpuManagedRetiredSafepointResultV1 final
	{
		HybridCpuManagedRetiredSafepointStatusV1 Status;
		std::wstring Reason;
		std::shared_ptr<HybridCpuManagedNonMovingGcResultV1> Collection;
		bool IsSafepoint;
		bool IsSuccess;

		HybridCpuManagedRetiredSafepointResultV1(HybridCpuManagedRetiredSafepointStatusV1 status, std::wstring reason,
			std::shared_ptr<HybridCpuManagedNonMovingGcResultV1> collection = nullptr)
			: Status(status), Reason(std::move(reason)), Collection(std::move(collection)),
			  IsSafepoint(status != HybridCpuManagedRetiredSafepointStatusV1::NotSafepoint),
			  IsSuccess(status == HybridCpuManagedRetiredSafepointStatusV1::Collected && Collection && Collection->IsSuccess) {}

		static std::shared_ptr<HybridCpuManagedRetiredSafepointResultV1> Rejected(std::wstring reason)
		{
			return std::make_shared<HybridCpuManagedRetiredSafepointResultV1>(HybridCpuManagedRetiredSafepointStatusV1::Rejected, std::move(reason));
		}
	};

	class HybridCpuManagedNonMovingGcV1 final
	{
	public:
		template<typename... TArgs>
		std::shared_ptr<HybridCpuManagedRetiredSafepointResultV1> CollectRetiredSafepoint(TArgs&&...) const
		{
			return HybridCpuManagedRetiredSafepointResultV1::Rejected(L"Non-moving GC safepoint collection is unavailable in this runtime implementation.");
		}
	};

	enum class HybridCpuManagedGcRuntimeStatusV1 : unsigned char
	{
		Collected = 0,
		Disabled = 1,
		Unsupported = 2,
		InvalidMetadata = 3,
		InvalidState = 4,
		BudgetExhausted = 5
	};

	enum class HybridCpuManagedRootLocationKindV1 : unsigned char
	{
		Register = 0,
		Stack = 1
	};

// Immutable contract value types mirror the corresponding C# records.
struct HybridCpuManagedGcRuntimeBudgetsV1 final
{
    int MaximumFrames, MaximumSafepoints, MaximumRoots, MaximumObjects, MaximumObjectReferences;
    HybridCpuManagedGcRuntimeBudgetsV1(int frames, int safepoints, int roots, int objects, int references)
        : MaximumFrames(frames), MaximumSafepoints(safepoints), MaximumRoots(roots), MaximumObjects(objects), MaximumObjectReferences(references) {}
    static std::shared_ptr<HybridCpuManagedGcRuntimeBudgetsV1> Production();
};
struct HybridCpuManagedGcRuntimeOptionsV1 final
{
    bool EnableMovingCollection;
    std::shared_ptr<HybridCpuManagedGcRuntimeBudgetsV1> Budgets;
    std::wstring OptionsDigest;
    HybridCpuManagedGcRuntimeOptionsV1(bool enabled, std::shared_ptr<HybridCpuManagedGcRuntimeBudgetsV1> budgets, std::wstring digest)
        : EnableMovingCollection(enabled), Budgets(std::move(budgets)), OptionsDigest(std::move(digest)) {}
    static std::shared_ptr<HybridCpuManagedGcRuntimeOptionsV1> Create(bool enabled, std::shared_ptr<HybridCpuManagedGcRuntimeBudgetsV1> budgets);
    static std::shared_ptr<HybridCpuManagedGcRuntimeOptionsV1> Production();
    static std::shared_ptr<HybridCpuManagedGcRuntimeOptionsV1> Qualification();
};
struct HybridCpuManagedMethodRegistrationV1 final
{
    std::wstring MethodIdentity, ManagedAbiDigest, TargetPlatformDigest, NativeAbiDigest, RuntimePackRevision;
    std::shared_ptr<std::vector<unsigned char>> GcInfo, CodeManagerMetadata;
};
struct HybridCpuManagedRuntimeFrameV1 final
{
    std::wstring MethodIdentity;
    int InstructionPointer;
    std::shared_ptr<std::vector<unsigned long long>> Registers;
    std::shared_ptr<std::unordered_map<int, unsigned long long>> StackSlots;
    HybridCpuManagedRuntimeFrameV1(std::wstring method, int ip, std::shared_ptr<std::vector<unsigned long long>> registers, std::shared_ptr<std::unordered_map<int, unsigned long long>> slots)
        : MethodIdentity(std::move(method)), InstructionPointer(ip), Registers(std::move(registers)), StackSlots(std::move(slots)) {}
};
struct HybridCpuManagedThreadStateV1 final
{
    std::shared_ptr<std::vector<std::shared_ptr<HybridCpuManagedRuntimeFrameV1>>> Frames;
    explicit HybridCpuManagedThreadStateV1(std::shared_ptr<std::vector<std::shared_ptr<HybridCpuManagedRuntimeFrameV1>>> frames) : Frames(std::move(frames)) {}
};
struct HybridCpuManagedHeapObjectV1 final
{
    unsigned long long Address;
    std::shared_ptr<std::vector<unsigned char>> Payload;
    std::shared_ptr<std::vector<unsigned long long>> References;
    HybridCpuManagedHeapObjectV1(unsigned long long address, std::shared_ptr<std::vector<unsigned char>> payload, std::shared_ptr<std::vector<unsigned long long>> references)
        : Address(address), Payload(std::move(payload)), References(std::move(references)) {}
};
struct HybridCpuManagedHeapV1 final
{
    std::shared_ptr<std::vector<std::shared_ptr<HybridCpuManagedHeapObjectV1>>> Objects;
    explicit HybridCpuManagedHeapV1(std::shared_ptr<std::vector<std::shared_ptr<HybridCpuManagedHeapObjectV1>>> objects) : Objects(std::move(objects)) {}
};
struct HybridCpuManagedGcCollectionRequestV1 final
{
    std::shared_ptr<std::vector<std::shared_ptr<HybridCpuManagedMethodRegistrationV1>>> Registrations;
    std::shared_ptr<HybridCpuManagedThreadStateV1> Thread;
    std::shared_ptr<HybridCpuManagedHeapV1> Heap;
};
struct HybridCpuManagedGcCollectionResultV1 final
{
    HybridCpuManagedGcRuntimeStatusV1 Status;
    std::wstring Reason;
    std::shared_ptr<HybridCpuManagedThreadStateV1> Thread;
    std::shared_ptr<HybridCpuManagedHeapV1> Heap;
    std::unordered_map<unsigned long long, unsigned long long> ForwardingAddresses;
    int RootCount, ReclaimedObjectCount;
    std::wstring ResultDigest;
    HybridCpuManagedGcCollectionResultV1(HybridCpuManagedGcRuntimeStatusV1 status, std::wstring reason, std::shared_ptr<HybridCpuManagedThreadStateV1> thread, std::shared_ptr<HybridCpuManagedHeapV1> heap, std::unordered_map<unsigned long long, unsigned long long> forwarding, int roots, int reclaimed, std::wstring digest)
        : Status(status), Reason(std::move(reason)), Thread(std::move(thread)), Heap(std::move(heap)), ForwardingAddresses(std::move(forwarding)), RootCount(roots), ReclaimedObjectCount(reclaimed), ResultDigest(std::move(digest)) {}
};
	class HybridCpuManagedGcRuntimeContractV1 final : public std::enable_shared_from_this<HybridCpuManagedGcRuntimeContractV1>
	{
	private:
		const std::wstring ProductionOptionsDigest;
		const std::wstring QualificationOptionsDigest;
		const std::wstring ContractDigest;

	public:
		static inline const std::wstring SchemaId = L"hybridcpu.managed-gc-runtime/v1";
		static inline const std::wstring ManagedAbiDigest = L"2ef28903d8b273f848283c0940899e0390183d8736980ad0c251b27e121704d9";
		static inline const std::wstring TargetPlatformDigest = L"3b3714582295e0a0daa111841b2fe96aae610788ec7e27b222b7552dd5879431";
		static inline const std::wstring NativeAbiDigest = L"3a4d4a4aebed5e5df40a3615b4e40c301181cb71bf281d958470fbee308317a4";
		static inline const std::wstring RuntimePackRevision = L"hybridcpu.runtime-pack/managed-contract-v1.5";
		static constexpr unsigned long long FirstMovedAddress = 0x0000'0000'1000'0000ULL;
		static constexpr unsigned long long ObjectAddressStride = 0x100ULL;

		static std::shared_ptr<HybridCpuManagedGcRuntimeContractV1> getDefault();

	private:
		HybridCpuManagedGcRuntimeContractV1();

	public:
		std::wstring getProductionOptionsDigest() const;
		std::wstring getQualificationOptionsDigest() const;
		std::wstring getContractDigest() const;

		static std::wstring Hash(const std::wstring &text);
	};

	/// <summary>
	/// Runtime-owned, default-disabled moving collector for the qualified Phase 25 object-reference subset.
	/// It consumes compiler metadata but grants no compiler authority over suspension, movement or root validity.
	/// </summary>
	class HybridCpuManagedGcRuntimeV1 final : public std::enable_shared_from_this<HybridCpuManagedGcRuntimeV1>
	{
	private:
		struct RootLocation final { HybridCpuManagedRootLocationKindV1 Kind; int Index; };
		struct RootBinding final { int FrameIndex; HybridCpuManagedRootLocationKindV1 Kind; int Index; unsigned long long Value; };
		struct Registration final { int CodeStart, CodeSize; std::unordered_map<int, std::vector<std::shared_ptr<RootLocation>>> Safepoints; };
		struct RegistrationParse final { HybridCpuManagedGcRuntimeStatusV1 Status; std::wstring Reason; std::shared_ptr<Registration> RegistrationValue; };
		struct SafepointParse final { HybridCpuManagedGcRuntimeStatusV1 Status; std::wstring Reason; std::unordered_map<int, std::vector<std::shared_ptr<RootLocation>>> Safepoints; };
		static constexpr unsigned int GcInfoMagic = 0x474d4348;
		static constexpr unsigned int CodeManagerMagic = 0x4d4d4348;

	public:
		std::shared_ptr<HybridCpuManagedGcCollectionResultV1> Collect(std::shared_ptr<HybridCpuManagedGcCollectionRequestV1> request, std::optional<std::shared_ptr<HybridCpuManagedGcRuntimeOptionsV1>> options = std::nullopt);

	private:
		static std::shared_ptr<RegistrationParse> ParseRegistration(std::shared_ptr<HybridCpuManagedMethodRegistrationV1> item, std::shared_ptr<HybridCpuManagedGcRuntimeBudgetsV1> budgets);

		static std::shared_ptr<SafepointParse> ParseSafepoints(std::vector<unsigned char> &bytes, std::shared_ptr<HybridCpuManagedGcRuntimeBudgetsV1> budgets);

		static bool ValidOptions(std::shared_ptr<HybridCpuManagedGcRuntimeOptionsV1> options);

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private static string DigestResult(IReadOnlyList<HybridCpuManagedRuntimeFrameV1> frames, IReadOnlyList<HybridCpuManagedHeapObjectV1> objects, IReadOnlyDictionary<ulong, ulong> forwarding)
		static std::wstring DigestResult(const std::vector<std::shared_ptr<HybridCpuManagedRuntimeFrameV1>> &frames, const std::vector<std::shared_ptr<HybridCpuManagedHeapObjectV1>> &objects, const std::unordered_map<unsigned long long, unsigned long long> &forwarding);

		static std::shared_ptr<HybridCpuManagedGcCollectionResultV1> Failure(HybridCpuManagedGcRuntimeStatusV1 status, const std::wstring &reason, std::shared_ptr<HybridCpuManagedThreadStateV1> thread, std::shared_ptr<HybridCpuManagedHeapV1> heap);

	};

}
