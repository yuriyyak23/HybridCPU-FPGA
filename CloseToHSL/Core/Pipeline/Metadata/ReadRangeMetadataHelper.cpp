#include "ReadRangeMetadataHelper.h"
#include <algorithm>
#include <stdexcept>

namespace YAKSys_Hybrid_CPU::Core::Pipeline::Metadata
{
    using Range = std::pair<unsigned long long, unsigned long long>;
    using Ranges = std::vector<Range>;

    bool ReadRangeMetadataHelper::TryNormalizeContiguousReadRanges(const Ranges& ranges, Ranges& normalized)
    {
        // Work on a copy so that callers may use the same vector for both arguments.
        const Ranges source = ranges;
        normalized = source;
        if (source.size() <= 1) return false;
        auto sorted = CollectOrderedNonEmptyRanges(source);
        if (sorted.size() <= 1)
        {
            normalized = sorted;
            return source.size() != sorted.size();
        }
        Ranges merged;
        merged.reserve(sorted.size());
        auto start = sorted[0].first;
        auto end = SaturatingAdd(start, sorted[0].second);
        for (size_t i = 1; i < sorted.size(); ++i)
        {
            const auto [nextStart, nextLength] = sorted[i];
            const auto nextEnd = SaturatingAdd(nextStart, nextLength);
            if (nextStart <= end) { end = std::max(end, nextEnd); continue; }
            merged.emplace_back(start, end - start);
            start = nextStart;
            end = nextEnd;
        }
        merged.emplace_back(start, end - start);
        if (merged.size() == sorted.size() && HasSameRangeSequence(sorted, source)) return false;
        normalized = std::move(merged);
        return true;
    }

    std::shared_ptr<AssistCoalescingDescriptor> ReadRangeMetadataHelper::BuildAssistCoalescingDescriptor(const Ranges& ranges, const Ranges& normalized)
    {
        auto none = [] { return std::make_shared<AssistCoalescingDescriptor>(AssistCoalescingDescriptor::None); };
        if (ranges.size() <= 1) return none();
        const auto sorted = CollectOrderedNonEmptyRanges(ranges);
        if (sorted.size() <= 1) return none();
        const auto start = sorted[0].first;
        auto end = SaturatingAdd(start, sorted[0].second);
        unsigned long long largestGap = 0;
        for (size_t i = 1; i < sorted.size(); ++i)
        {
            const auto [nextStart, nextLength] = sorted[i];
            const auto nextEnd = SaturatingAdd(nextStart, nextLength);
            const auto gap = nextStart > end ? nextStart - end : 0;
            if (gap > AssistNearContiguousGapBytes) return none();
            largestGap = std::max(largestGap, gap);
            end = std::max(end, nextEnd);
        }
        const auto normalizedCount = normalized.empty() ? sorted.size() : normalized.size();
        return std::make_shared<AssistCoalescingDescriptor>(start, end - start, static_cast<int>(sorted.size()), static_cast<int>(normalizedCount), largestGap);
    }

    void ReadRangeMetadataHelper::ValidateCoalescedRangeMetadata(const Ranges& raw, const Ranges& normalized, std::shared_ptr<AssistCoalescingDescriptor> descriptor)
    {
        if (!descriptor) throw std::invalid_argument("descriptor");
        if (!descriptor->getIsValid()) return;
        const auto end = SaturatingAdd(descriptor->BaseAddress, descriptor->PrefetchLength);
        ValidateRangesStayInsideWindow(raw, descriptor->BaseAddress, end, L"rawReadMemoryRanges");
        ValidateRangesStayInsideWindow(normalized, descriptor->BaseAddress, end, L"normalizedReadMemoryRanges");
    }

    Ranges ReadRangeMetadataHelper::CollectOrderedNonEmptyRanges(const Ranges& ranges)
    {
        Ranges result;
        result.reserve(ranges.size());
        for (const auto& [address, length] : ranges)
            if (length != 0) result.emplace_back(address, SaturatingAdd(address, length) - address);
        std::stable_sort(result.begin(), result.end(), [](const Range& a, const Range& b) { return a.first < b.first; });
        return result;
    }

    bool ReadRangeMetadataHelper::HasSameRangeSequence(const Ranges& ordered, const Ranges& raw)
    {
        return ordered == raw;
    }

    void ReadRangeMetadataHelper::ValidateRangesStayInsideWindow(const Ranges& ranges, unsigned long long start, unsigned long long end, const std::wstring& name)
    {
        for (const auto& [address, length] : ranges)
        {
            if (length != 0 && (address < start || SaturatingAdd(address, length) > end))
                throw std::logic_error(std::string("Coalesced read-range metadata escaped its advisory window while validating ") + std::string(name.begin(), name.end()) + ".");
        }
    }

    unsigned long long ReadRangeMetadataHelper::SaturatingAdd(unsigned long long address, unsigned long long length)
    {
        const auto remaining = std::numeric_limits<unsigned long long>::max() - address;
        return length > remaining ? std::numeric_limits<unsigned long long>::max() : address + length;
    }
}
