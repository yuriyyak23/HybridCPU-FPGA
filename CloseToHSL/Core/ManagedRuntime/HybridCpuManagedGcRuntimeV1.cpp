#include "HybridCpuManagedGcRuntimeV1.h"
#include "../Decoder/Sha256Digest.h"
#include <algorithm>
#include <charconv>
#include <cctype>
#include <cstdint>
#include <cwctype>
#include <span>
#include <codecvt>
#include <limits>
#include <locale>
#include <sstream>
#include <stdexcept>

namespace HybridCPU::ManagedRuntime
{
    namespace
    {
        std::vector<unsigned char> Utf8(const std::wstring &text)
        {
            std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
            const std::string encoded = converter.to_bytes(text);
            return {encoded.begin(), encoded.end()};
        }
        std::wstring HashBytes(const std::vector<unsigned char> &bytes)
        {
            return YAKSys_Hybrid_CPU::Core::Decoder::Sha256Hex(std::span<const unsigned char>(bytes));
        }
        std::wstring Join(const std::vector<std::wstring> &parts, wchar_t separator)
        {
            std::wstring out;
            for (size_t i = 0; i < parts.size(); ++i) { if (i) out.push_back(separator); out += parts[i]; }
            return out;
        }
        template <typename T>
        std::wstring Decimal(T value)
        {
            char buffer[32];
            const auto result = std::to_chars(buffer, buffer + sizeof(buffer), value);
            if (result.ec != std::errc{}) throw std::runtime_error("integer formatting failed");
            std::wstring out; out.reserve(static_cast<size_t>(result.ptr - buffer));
            for (const char *p = buffer; p != result.ptr; ++p) out.push_back(static_cast<wchar_t>(*p));
            return out;
        }
        std::wstring HexUpper(const std::vector<unsigned char> &bytes)
        {
            constexpr wchar_t digits[] = L"0123456789ABCDEF";
            std::wstring out; out.reserve(bytes.size() * 2);
            for (unsigned char b : bytes) { out.push_back(digits[b >> 4]); out.push_back(digits[b & 15]); }
            return out;
        }
        std::vector<unsigned char> HexBytes(const std::wstring &hex)
        {
            auto digit = [](wchar_t c) -> unsigned char {
                if (c >= L'0' && c <= L'9') return static_cast<unsigned char>(c - L'0');
                if (c >= L'a' && c <= L'f') return static_cast<unsigned char>(c - L'a' + 10);
                if (c >= L'A' && c <= L'F') return static_cast<unsigned char>(c - L'A' + 10);
                throw std::invalid_argument("invalid hexadecimal digest");
            };
            if (hex.size() % 2) throw std::invalid_argument("odd hexadecimal digest length");
            std::vector<unsigned char> out; out.reserve(hex.size() / 2);
            for (size_t i = 0; i < hex.size(); i += 2) out.push_back(static_cast<unsigned char>((digit(hex[i]) << 4) | digit(hex[i + 1])));
            return out;
        }
        uint16_t ReadU16(const std::vector<unsigned char> &b, size_t o)
        {
            if (o > b.size() || b.size() - o < 2) throw std::out_of_range("truncated little-endian value");
            return static_cast<uint16_t>(b[o]) | static_cast<uint16_t>(b[o + 1] << 8);
        }
        uint32_t ReadU32(const std::vector<unsigned char> &b, size_t o)
        {
            if (o > b.size() || b.size() - o < 4) throw std::out_of_range("truncated little-endian value");
            return static_cast<uint32_t>(b[o]) | (static_cast<uint32_t>(b[o + 1]) << 8) | (static_cast<uint32_t>(b[o + 2]) << 16) | (static_cast<uint32_t>(b[o + 3]) << 24);
        }
        int32_t ReadI32(const std::vector<unsigned char> &b, size_t o) { return static_cast<int32_t>(ReadU32(b, o)); }
        int16_t ReadI16(const std::vector<unsigned char> &b, size_t o) { return static_cast<int16_t>(ReadU16(b, o)); }
        bool SameBytes(const std::vector<unsigned char> &a, size_t offset, const std::vector<unsigned char> &b, size_t count)
        {
            return offset <= a.size() && count <= a.size() - offset && count <= b.size() && std::equal(a.begin() + offset, a.begin() + offset + count, b.begin());
        }
        bool EqualsOrdinal(const std::wstring &left, const std::wstring &right)
        {
            return left == right;
        }
        bool IsWhiteSpace(const std::wstring &s)
        {
            return s.empty() || std::all_of(s.begin(), s.end(), [](wchar_t c) {
                const unsigned int value = static_cast<unsigned int>(c);
                return (value >= 0x0009 && value <= 0x000d) || value == 0x0020 || value == 0x0085 || value == 0x00a0 ||
                    value == 0x1680 || (value >= 0x2000 && value <= 0x200a) || value == 0x2028 || value == 0x2029 ||
                    value == 0x202f || value == 0x205f || value == 0x3000;
            });
        }
        std::wstring StatusName(HybridCpuManagedGcRuntimeStatusV1 status)
        {
            switch (status) {
            case HybridCpuManagedGcRuntimeStatusV1::Collected: return L"Collected";
            case HybridCpuManagedGcRuntimeStatusV1::Disabled: return L"Disabled";
            case HybridCpuManagedGcRuntimeStatusV1::Unsupported: return L"Unsupported";
            case HybridCpuManagedGcRuntimeStatusV1::InvalidMetadata: return L"InvalidMetadata";
            case HybridCpuManagedGcRuntimeStatusV1::InvalidState: return L"InvalidState";
            case HybridCpuManagedGcRuntimeStatusV1::BudgetExhausted: return L"BudgetExhausted";
            }
            return L"";
        }
    }

    std::shared_ptr<HybridCpuManagedGcRuntimeBudgetsV1> HybridCpuManagedGcRuntimeBudgetsV1::Production()
    {
        static auto value = std::make_shared<HybridCpuManagedGcRuntimeBudgetsV1>(1024, 4096, 16384, 65536, 262144);
        return value;
    }
    std::shared_ptr<HybridCpuManagedGcRuntimeOptionsV1> HybridCpuManagedGcRuntimeOptionsV1::Create(bool enabled, std::shared_ptr<HybridCpuManagedGcRuntimeBudgetsV1> budgets)
    {
        if (!budgets) throw std::invalid_argument("budgets must not be null");
        const std::wstring canonical = Join({L"hybridcpu.managed-gc-runtime-options/v1", enabled ? L"True" : L"False", Decimal(budgets->MaximumFrames), Decimal(budgets->MaximumSafepoints), Decimal(budgets->MaximumRoots), Decimal(budgets->MaximumObjects), Decimal(budgets->MaximumObjectReferences)}, L'|');
        return std::make_shared<HybridCpuManagedGcRuntimeOptionsV1>(enabled, std::move(budgets), HybridCpuManagedGcRuntimeContractV1::Hash(canonical));
    }
    std::shared_ptr<HybridCpuManagedGcRuntimeOptionsV1> HybridCpuManagedGcRuntimeOptionsV1::Production()
    {
        static auto value = Create(false, HybridCpuManagedGcRuntimeBudgetsV1::Production()); return value;
    }
    std::shared_ptr<HybridCpuManagedGcRuntimeOptionsV1> HybridCpuManagedGcRuntimeOptionsV1::Qualification()
    {
        static auto value = Create(true, HybridCpuManagedGcRuntimeBudgetsV1::Production()); return value;
    }

    std::shared_ptr<HybridCpuManagedGcRuntimeContractV1> HybridCpuManagedGcRuntimeContractV1::getDefault()
    {
        static const std::shared_ptr<HybridCpuManagedGcRuntimeContractV1> value(new HybridCpuManagedGcRuntimeContractV1());
        return value;
    }
    HybridCpuManagedGcRuntimeContractV1::HybridCpuManagedGcRuntimeContractV1()
        : ProductionOptionsDigest(HybridCpuManagedGcRuntimeOptionsV1::Production()->OptionsDigest),
          QualificationOptionsDigest(HybridCpuManagedGcRuntimeOptionsV1::Qualification()->OptionsDigest),
          ContractDigest(Hash(Join({SchemaId, ManagedAbiDigest, TargetPlatformDigest, NativeAbiDigest, RuntimePackRevision,
              ProductionOptionsDigest, QualificationOptionsDigest, L"hcmg=1.0", L"hcmm=1.0", L"object-reference-only", L"transactional", L"publication-authority=false"}, L'|')))
    {
    }
    std::wstring HybridCpuManagedGcRuntimeContractV1::getProductionOptionsDigest() const { return ProductionOptionsDigest; }
    std::wstring HybridCpuManagedGcRuntimeContractV1::getQualificationOptionsDigest() const { return QualificationOptionsDigest; }
    std::wstring HybridCpuManagedGcRuntimeContractV1::getContractDigest() const { return ContractDigest; }
    std::wstring HybridCpuManagedGcRuntimeContractV1::Hash(const std::wstring &text) { return HashBytes(Utf8(text)); }

    std::shared_ptr<HybridCpuManagedGcCollectionResultV1> HybridCpuManagedGcRuntimeV1::Collect(std::shared_ptr<HybridCpuManagedGcCollectionRequestV1> request, std::optional<std::shared_ptr<HybridCpuManagedGcRuntimeOptionsV1>> options)
    {
        if (!request) throw std::invalid_argument("request must not be null");
        if (!options || !*options) options = HybridCpuManagedGcRuntimeOptionsV1::Production();
        auto activeOptions = *options;
        auto originalThread = request->Thread ? request->Thread : std::make_shared<HybridCpuManagedThreadStateV1>(std::make_shared<std::vector<std::shared_ptr<HybridCpuManagedRuntimeFrameV1>>>());
        auto originalHeap = request->Heap ? request->Heap : std::make_shared<HybridCpuManagedHeapV1>(std::make_shared<std::vector<std::shared_ptr<HybridCpuManagedHeapObjectV1>>>());
        if (!ValidOptions(activeOptions)) return Failure(HybridCpuManagedGcRuntimeStatusV1::InvalidState, L"Managed GC runtime options do not bind valid deterministic budgets.", originalThread, originalHeap);
        if (!activeOptions->EnableMovingCollection) return Failure(HybridCpuManagedGcRuntimeStatusV1::Disabled, L"Production moving collection is disabled.", originalThread, originalHeap);
        if (!request->Registrations || !request->Thread || !request->Heap || !request->Thread->Frames || !request->Heap->Objects) return Failure(HybridCpuManagedGcRuntimeStatusV1::InvalidState, L"Registrations, thread frames and heap objects are required.", originalThread, originalHeap);

        const auto budgets = activeOptions->Budgets;
        if (request->Thread->Frames->size() > static_cast<size_t>(budgets->MaximumFrames) || request->Heap->Objects->size() > static_cast<size_t>(budgets->MaximumObjects))
            return Failure(HybridCpuManagedGcRuntimeStatusV1::BudgetExhausted, L"Managed GC runtime deterministic budgets were exceeded.", originalThread, originalHeap);
        size_t referenceCount = 0;
        for (const auto &item : *request->Heap->Objects) if (item && item->References && item->References->size() > std::numeric_limits<size_t>::max() - referenceCount) return Failure(HybridCpuManagedGcRuntimeStatusV1::BudgetExhausted, L"Managed GC runtime deterministic budgets were exceeded.", originalThread, originalHeap); else if (item && item->References) referenceCount += item->References->size();
        if (referenceCount > static_cast<size_t>(budgets->MaximumObjectReferences)) return Failure(HybridCpuManagedGcRuntimeStatusV1::BudgetExhausted, L"Managed GC runtime deterministic budgets were exceeded.", originalThread, originalHeap);

        std::unordered_map<std::wstring, std::shared_ptr<Registration>> registrations;
        for (const auto &item : *request->Registrations) {
            auto parsed = ParseRegistration(item, budgets);
            if (parsed->Status != HybridCpuManagedGcRuntimeStatusV1::Collected || !parsed->RegistrationValue) return Failure(parsed->Status, parsed->Reason, originalThread, originalHeap);
            if (!registrations.emplace(item->MethodIdentity, parsed->RegistrationValue).second) return Failure(HybridCpuManagedGcRuntimeStatusV1::InvalidMetadata, L"Managed method registration identities must be unique.", originalThread, originalHeap);
        }
        std::vector<RootBinding> rootBindings;
        for (size_t frameIndex = 0; frameIndex < request->Thread->Frames->size(); ++frameIndex) {
            const auto &frame = (*request->Thread->Frames)[frameIndex];
            if (!frame || IsWhiteSpace(frame->MethodIdentity) || !frame->Registers || !frame->StackSlots || frame->Registers->size() != 64 || registrations.find(frame->MethodIdentity) == registrations.end())
                return Failure(HybridCpuManagedGcRuntimeStatusV1::InvalidState, L"Every frame requires 64 registers and one matching method registration.", originalThread, originalHeap);
            const auto registration = registrations.at(frame->MethodIdentity);
            if (frame->InstructionPointer < registration->CodeStart) return Failure(HybridCpuManagedGcRuntimeStatusV1::InvalidState, L"A frame instruction pointer is not a registered safepoint.", originalThread, originalHeap);
            const int relativeIp = frame->InstructionPointer - registration->CodeStart;
            const auto safepoint = registration->Safepoints.find(relativeIp);
            if (relativeIp >= registration->CodeSize || safepoint == registration->Safepoints.end()) return Failure(HybridCpuManagedGcRuntimeStatusV1::InvalidState, L"A frame instruction pointer is not a registered safepoint.", originalThread, originalHeap);
            for (const auto &location : safepoint->second) {
                unsigned long long value = 0;
                if (location->Kind == HybridCpuManagedRootLocationKindV1::Register) {
                    if (location->Index < 0 || location->Index >= 64) return Failure(HybridCpuManagedGcRuntimeStatusV1::InvalidMetadata, L"GC metadata names an invalid register.", originalThread, originalHeap);
                    value = (*frame->Registers)[static_cast<size_t>(location->Index)];
                } else {
                    const auto slot = frame->StackSlots->find(location->Index);
                    if (slot == frame->StackSlots->end()) return Failure(HybridCpuManagedGcRuntimeStatusV1::InvalidState, L"A GC stack root location is absent from the runtime frame.", originalThread, originalHeap);
                    value = slot->second;
                }
                rootBindings.push_back({static_cast<int>(frameIndex), location->Kind, location->Index, value});
            }
        }
        if (rootBindings.size() > static_cast<size_t>(budgets->MaximumRoots)) return Failure(HybridCpuManagedGcRuntimeStatusV1::BudgetExhausted, L"The runtime root budget was exceeded.", originalThread, originalHeap);

        std::unordered_map<unsigned long long, std::shared_ptr<HybridCpuManagedHeapObjectV1>> objects;
        for (const auto &item : *request->Heap->Objects) {
            if (!item || item->Address == 0 || item->Address % 8 != 0 || !item->Payload || !item->References || !objects.emplace(item->Address, item).second)
                return Failure(HybridCpuManagedGcRuntimeStatusV1::InvalidState, L"Heap object addresses and payload/reference collections must be valid and unique.", originalThread, originalHeap);
        }
        std::set<unsigned long long> reachable;
        std::deque<unsigned long long> pending;
        for (const auto &root : rootBindings) if (root.Value != 0) pending.push_back(root.Value);
        while (!pending.empty()) {
            const auto address = pending.front(); pending.pop_front();
            if (!reachable.insert(address).second) continue;
            const auto object = objects.find(address);
            if (object == objects.end()) return Failure(HybridCpuManagedGcRuntimeStatusV1::InvalidState, L"A root or object field references an address outside the managed heap.", originalThread, originalHeap);
            for (auto child : *object->second->References) if (child != 0) pending.push_back(child);
        }
        std::unordered_map<unsigned long long, unsigned long long> forwarding;
        size_t moveIndex = 0;
        for (auto address : reachable) {
            if (moveIndex > (std::numeric_limits<unsigned long long>::max() - HybridCpuManagedGcRuntimeContractV1::FirstMovedAddress) / HybridCpuManagedGcRuntimeContractV1::ObjectAddressStride)
                return Failure(HybridCpuManagedGcRuntimeStatusV1::BudgetExhausted, L"The runtime root budget was exceeded.", originalThread, originalHeap);
            forwarding.emplace(address, HybridCpuManagedGcRuntimeContractV1::FirstMovedAddress + static_cast<unsigned long long>(moveIndex++) * HybridCpuManagedGcRuntimeContractV1::ObjectAddressStride);
        }
        std::vector<std::shared_ptr<HybridCpuManagedHeapObjectV1>> movedObjects;
        for (auto address : reachable) {
            const auto &item = objects.at(address);
            std::vector<unsigned long long> refs; refs.reserve((item->References ? item->References->size() : 0));
            for (auto reference : *item->References) refs.push_back(reference == 0 ? 0 : forwarding.at(reference));
            movedObjects.push_back(std::make_shared<HybridCpuManagedHeapObjectV1>(forwarding.at(address), std::make_shared<std::vector<unsigned char>>(*item->Payload), std::make_shared<std::vector<unsigned long long>>(std::move(refs))));
        }
        std::vector<std::shared_ptr<HybridCpuManagedRuntimeFrameV1>> movedFrames;
        movedFrames.reserve(request->Thread->Frames->size());
        for (const auto &frame : *request->Thread->Frames) movedFrames.push_back(std::make_shared<HybridCpuManagedRuntimeFrameV1>(frame->MethodIdentity, frame->InstructionPointer, std::make_shared<std::vector<unsigned long long>>(*frame->Registers), std::make_shared<std::unordered_map<int, unsigned long long>>(*frame->StackSlots)));
        for (const auto &root : rootBindings) {
            const auto moved = root.Value == 0 ? 0 : forwarding.at(root.Value);
            auto &frame = movedFrames[static_cast<size_t>(root.FrameIndex)];
            if (root.Kind == HybridCpuManagedRootLocationKindV1::Register) (*frame->Registers)[static_cast<size_t>(root.Index)] = moved;
            else (*frame->StackSlots)[root.Index] = moved;
        }
        const auto digest = DigestResult(movedFrames, movedObjects, forwarding);
        return std::make_shared<HybridCpuManagedGcCollectionResultV1>(HybridCpuManagedGcRuntimeStatusV1::Collected, L"Runtime stack walking and deterministic moving collection completed.",
            std::make_shared<HybridCpuManagedThreadStateV1>(std::make_shared<std::vector<std::shared_ptr<HybridCpuManagedRuntimeFrameV1>>>(std::move(movedFrames))), std::make_shared<HybridCpuManagedHeapV1>(std::make_shared<std::vector<std::shared_ptr<HybridCpuManagedHeapObjectV1>>>(std::move(movedObjects))), std::move(forwarding), static_cast<int>(rootBindings.size()), static_cast<int>(objects.size() - reachable.size()), digest);
    }

    std::shared_ptr<HybridCpuManagedGcRuntimeV1::RegistrationParse> HybridCpuManagedGcRuntimeV1::ParseRegistration(std::shared_ptr<HybridCpuManagedMethodRegistrationV1> item, std::shared_ptr<HybridCpuManagedGcRuntimeBudgetsV1> budgets)
    {
        auto fail = [](HybridCpuManagedGcRuntimeStatusV1 status, std::wstring reason) { return std::make_shared<RegistrationParse>(RegistrationParse{status, std::move(reason), nullptr}); };
        if (!item || IsWhiteSpace(item->MethodIdentity) || !EqualsOrdinal(item->ManagedAbiDigest, HybridCpuManagedGcRuntimeContractV1::ManagedAbiDigest) || !EqualsOrdinal(item->TargetPlatformDigest, HybridCpuManagedGcRuntimeContractV1::TargetPlatformDigest) || !EqualsOrdinal(item->NativeAbiDigest, HybridCpuManagedGcRuntimeContractV1::NativeAbiDigest) || !EqualsOrdinal(item->RuntimePackRevision, HybridCpuManagedGcRuntimeContractV1::RuntimePackRevision))
            return fail(HybridCpuManagedGcRuntimeStatusV1::Unsupported, L"Managed ABI, target, native ABI or runtime-pack registration does not match the runtime.");
        if (!item->CodeManagerMetadata || !item->GcInfo) return fail(HybridCpuManagedGcRuntimeStatusV1::Unsupported, L"Managed ABI, target, native ABI or runtime-pack registration does not match the runtime.");
        const auto &metadata = *item->CodeManagerMetadata;
        if (metadata.size() != 92 || ReadU32(metadata, 0) != CodeManagerMagic || ReadU16(metadata, 4) != 1 || ReadU16(metadata, 6) != 0 || ReadI32(metadata, 8) != 1)
            return fail(HybridCpuManagedGcRuntimeStatusV1::InvalidMetadata, L"Code-manager metadata must contain one HCMM 1.0 method record.");
        std::vector<unsigned char> record(metadata.begin() + 12, metadata.end());
        const auto identityDigest = HexBytes(HashBytes(Utf8(item->MethodIdentity)));
        if (!SameBytes(record, 0, identityDigest, 8)) return fail(HybridCpuManagedGcRuntimeStatusV1::InvalidMetadata, L"Code-manager method identity does not match the registration.");
        const int codeStart = ReadI32(record, 8), codeSize = ReadI32(record, 12);
        if (codeStart < 0 || codeSize <= 0 || std::any_of(record.begin() + 48, record.begin() + 80, [](unsigned char b) { return b != 0; }))
            return fail(HybridCpuManagedGcRuntimeStatusV1::Unsupported, L"Code range is invalid or unwind metadata is not supported by this GC runtime gate.");
        const std::wstring gcCanonical = L"gc-info|" + HybridCpuManagedGcRuntimeContractV1::ManagedAbiDigest + L"|" + HexUpper(*item->GcInfo);
        const auto gcDigest = HexBytes(HybridCpuManagedGcRuntimeContractV1::Hash(gcCanonical));
        if (!SameBytes(record, 16, gcDigest, 32)) return fail(HybridCpuManagedGcRuntimeStatusV1::InvalidMetadata, L"Code-manager GC-info digest does not match the registered HCMG bytes.");
        auto safepoints = ParseSafepoints(*item->GcInfo, budgets);
        if (safepoints->Status != HybridCpuManagedGcRuntimeStatusV1::Collected) return fail(safepoints->Status, safepoints->Reason);
        auto registration = std::make_shared<Registration>(Registration{codeStart, codeSize, std::move(safepoints->Safepoints)});
        return std::make_shared<RegistrationParse>(RegistrationParse{HybridCpuManagedGcRuntimeStatusV1::Collected, L"", std::move(registration)});
    }

    std::shared_ptr<HybridCpuManagedGcRuntimeV1::SafepointParse> HybridCpuManagedGcRuntimeV1::ParseSafepoints(std::vector<unsigned char> &bytes, std::shared_ptr<HybridCpuManagedGcRuntimeBudgetsV1> budgets)
    {
        auto fail = [](std::wstring reason) { return std::make_shared<SafepointParse>(SafepointParse{HybridCpuManagedGcRuntimeStatusV1::InvalidMetadata, std::move(reason), {}}); };
        if (bytes.size() < 12 || ReadU32(bytes, 0) != GcInfoMagic || ReadU16(bytes, 4) != 1 || ReadU16(bytes, 6) != 0) return fail(L"GC metadata is not HCMG 1.0.");
        const int count = ReadI32(bytes, 8);
        if (count < 0 || count > budgets->MaximumSafepoints) return std::make_shared<SafepointParse>(SafepointParse{HybridCpuManagedGcRuntimeStatusV1::BudgetExhausted, L"GC safepoint count exceeds the runtime budget.", {}});
        std::unordered_map<int, std::vector<std::shared_ptr<RootLocation>>> result;
        size_t offset = 12; int roots = 0;
        for (int index = 0; index < count; ++index) {
            if (offset > bytes.size() || bytes.size() - offset < 8) return fail(L"GC safepoint header is truncated.");
            const int codeOffset = ReadI32(bytes, offset); const unsigned char category = bytes[offset + 4]; const int referenceCount = ReadU16(bytes, offset + 6); offset += 8;
            if (referenceCount > std::numeric_limits<int>::max() - roots) return fail(L"GC safepoint or root collection is malformed.");
            roots += referenceCount;
            if (codeOffset < 0 || category > 1 || roots > budgets->MaximumRoots || static_cast<size_t>(referenceCount) > (bytes.size() - offset) / 16) return fail(L"GC safepoint or root collection is malformed.");
            std::vector<std::shared_ptr<RootLocation>> locations; locations.reserve(static_cast<size_t>(referenceCount));
            for (int reference = 0; reference < referenceCount; ++reference) {
                const unsigned char referenceKind = bytes[offset + 8], locationKind = bytes[offset + 9];
                const int reg = ReadI16(bytes, offset + 10), stack = ReadI32(bytes, offset + 12);
                if (referenceKind != 0 || locationKind > 1 || (locationKind == 0 && (reg < 0 || reg >= 64 || stack != -1)) || (locationKind == 1 && (reg != -1 || stack < 0 || stack % 8 != 0))) return fail(L"GC root location is unsupported or malformed.");
                locations.push_back(std::make_shared<RootLocation>(RootLocation{locationKind == 0 ? HybridCpuManagedRootLocationKindV1::Register : HybridCpuManagedRootLocationKindV1::Stack, locationKind == 0 ? reg : stack}));
                offset += 16;
            }
            if (!result.emplace(codeOffset, std::move(locations)).second) return fail(L"GC safepoint offsets must be unique.");
        }
        if (offset != bytes.size()) return fail(L"GC metadata has trailing bytes.");
        return std::make_shared<SafepointParse>(SafepointParse{HybridCpuManagedGcRuntimeStatusV1::Collected, L"valid", std::move(result)});
    }

    bool HybridCpuManagedGcRuntimeV1::ValidOptions(std::shared_ptr<HybridCpuManagedGcRuntimeOptionsV1> options)
    {
        if (!options || !options->Budgets) return false;
        const auto &b = options->Budgets;
        if (b->MaximumFrames <= 0 || b->MaximumSafepoints <= 0 || b->MaximumRoots <= 0 || b->MaximumObjects <= 0 || b->MaximumObjectReferences <= 0) return false;
        return EqualsOrdinal(options->OptionsDigest, HybridCpuManagedGcRuntimeOptionsV1::Create(options->EnableMovingCollection, b)->OptionsDigest);
    }
    std::wstring HybridCpuManagedGcRuntimeV1::DigestResult(const std::vector<std::shared_ptr<HybridCpuManagedRuntimeFrameV1>> &frames, const std::vector<std::shared_ptr<HybridCpuManagedHeapObjectV1>> &objects, const std::unordered_map<unsigned long long, unsigned long long> &forwarding)
    {
        std::vector<std::wstring> frameParts;
        for (const auto &frame : frames) {
            std::vector<std::wstring> registers; for (auto value : *frame->Registers) registers.push_back(Decimal(value));
            std::vector<int> stackKeys; for (const auto &entry : *frame->StackSlots) stackKeys.push_back(entry.first); std::sort(stackKeys.begin(), stackKeys.end());
            std::vector<std::wstring> stack; for (int key : stackKeys) stack.push_back(Decimal(key) + L"=" + Decimal(frame->StackSlots->at(key)));
            frameParts.push_back(frame->MethodIdentity + L":" + Decimal(frame->InstructionPointer) + L":" + Join(registers, L',') + L":" + Join(stack, L','));
        }
        std::vector<std::shared_ptr<HybridCpuManagedHeapObjectV1>> orderedObjects = objects;
        std::sort(orderedObjects.begin(), orderedObjects.end(), [](const auto &a, const auto &b) { return a->Address < b->Address; });
        std::vector<std::wstring> objectParts;
        for (const auto &item : orderedObjects) { std::vector<std::wstring> refs; for (auto ref : *item->References) refs.push_back(Decimal(ref)); objectParts.push_back(Decimal(item->Address) + L":" + HexUpper(*item->Payload) + L":" + Join(refs, L',')); }
        std::vector<unsigned long long> forwardKeys; for (const auto &entry : forwarding) forwardKeys.push_back(entry.first); std::sort(forwardKeys.begin(), forwardKeys.end());
        std::vector<std::wstring> forwardParts; for (auto key : forwardKeys) forwardParts.push_back(Decimal(key) + L">" + Decimal(forwarding.at(key)));
        return HybridCpuManagedGcRuntimeContractV1::Hash(Join({HybridCpuManagedGcRuntimeContractV1::getDefault()->getContractDigest(), Join(frameParts, L';'), Join(objectParts, L';'), Join(forwardParts, L';')}, L'|'));
    }
    std::shared_ptr<HybridCpuManagedGcCollectionResultV1> HybridCpuManagedGcRuntimeV1::Failure(HybridCpuManagedGcRuntimeStatusV1 status, const std::wstring &reason, std::shared_ptr<HybridCpuManagedThreadStateV1> thread, std::shared_ptr<HybridCpuManagedHeapV1> heap)
    {
        return std::make_shared<HybridCpuManagedGcCollectionResultV1>(status, reason, std::move(thread), std::move(heap), std::unordered_map<unsigned long long, unsigned long long>{}, 0, 0,
            HybridCpuManagedGcRuntimeContractV1::Hash(L"failure|" + StatusName(status) + L"|" + reason));
    }
}
