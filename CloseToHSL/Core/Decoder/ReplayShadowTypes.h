#pragma once
#include <string>
#include <utility>
namespace YAKSys_Hybrid_CPU::Core::Decoder {
struct ReplayEntryValidationFingerprint final {
    static constexpr unsigned short CurrentSchemaVersion = 1;
    unsigned short SchemaVersion = 0;
    std::wstring CanonicalContentSha256;
    ReplayEntryValidationFingerprint() = default;
    ReplayEntryValidationFingerprint(unsigned short schema, std::wstring digest) : SchemaVersion(schema), CanonicalContentSha256(std::move(digest)) {}
    bool isValid() const noexcept { return SchemaVersion == CurrentSchemaVersion && CanonicalContentSha256.size() == 64; }
    bool operator==(const ReplayEntryValidationFingerprint&) const = default;
};
struct ReplaySemanticShadowMetrics final {
    unsigned long long Observations = 0, Misses = 0, EquivalentHits = 0, ContentMismatches = 0, Invalidations = 0;
    ReplaySemanticShadowMetrics() = default;
    ReplaySemanticShadowMetrics(unsigned long long observations, unsigned long long misses, unsigned long long hits, unsigned long long mismatches, unsigned long long invalidations)
        : Observations(observations), Misses(misses), EquivalentHits(hits), ContentMismatches(mismatches), Invalidations(invalidations) {}
    unsigned long long getAccountedObservations() const noexcept { return Misses + EquivalentHits + ContentMismatches; }
};
}
