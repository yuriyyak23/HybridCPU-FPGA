$ErrorActionPreference='Stop'
$r='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Decoder'
$p=Join-Path $r 'ReplayShadowTypes.h'
if(Test-Path -LiteralPath $p){throw 'Replay types header exists'}
$lines=@(
'#pragma once',
'#include <string>',
'#include <utility>',
'namespace YAKSys_Hybrid_CPU::Core::Decoder {',
'struct ReplayEntryValidationFingerprint final {',
'    static constexpr unsigned short CurrentSchemaVersion = 1;',
'    unsigned short SchemaVersion = 0;',
'    std::wstring CanonicalContentSha256;',
'    ReplayEntryValidationFingerprint() = default;',
'    ReplayEntryValidationFingerprint(unsigned short schema, std::wstring digest) : SchemaVersion(schema), CanonicalContentSha256(std::move(digest)) {}',
'    bool isValid() const noexcept { return SchemaVersion == CurrentSchemaVersion && CanonicalContentSha256.size() == 64; }',
'    bool operator==(const ReplayEntryValidationFingerprint&) const = default;',
'};',
'struct ReplaySemanticShadowMetrics final {',
'    unsigned long long Observations = 0, Misses = 0, EquivalentHits = 0, ContentMismatches = 0, Invalidations = 0;',
'    ReplaySemanticShadowMetrics() = default;',
'    ReplaySemanticShadowMetrics(unsigned long long observations, unsigned long long misses, unsigned long long hits, unsigned long long mismatches, unsigned long long invalidations)',
'        : Observations(observations), Misses(misses), EquivalentHits(hits), ContentMismatches(mismatches), Invalidations(invalidations) {}',
'    unsigned long long getAccountedObservations() const noexcept { return Misses + EquivalentHits + ContentMismatches; }',
'};',
'}',
'') -join "`r`n"
[IO.File]::WriteAllText($p,$lines,[Text.UTF8Encoding]::new($false))
$p=Join-Path $r 'Rf09ReplayEntry.h'; $s=[IO.File]::ReadAllText($p); $m='#include "CanonicalDecodedContracts.h"'
if(-not $s.Contains($m)){throw 'Expected replay header include missing'}
$s=$s.Replace($m,$m+"`r`n"+'#include "ReplayShadowTypes.h"'); [IO.File]::WriteAllText($p,$s,[Text.UTF8Encoding]::new($false))
$p=Join-Path $r 'Rf09ReplayEntry.cpp'; $s=[IO.File]::ReadAllText($p)
$s=$s.Replace('getValidationFingerprint()->IsValid','getValidationFingerprint()->isValid()').Replace('getValidationFingerprint()->Equals(ComputeValidationFingerprint(getCanonicalBundle()))','(*getValidationFingerprint() == *ComputeValidationFingerprint(getCanonicalBundle()))')
[IO.File]::WriteAllText($p,$s,[Text.UTF8Encoding]::new($false))
$p=Join-Path $r 'Rf09ReplaySemanticShadowLookup.cpp'; $s=[IO.File]::ReadAllText($p)
$s=$s.Replace('return new(_observations, _misses, _equivalentHits, _contentMismatches, _invalidations);','return std::make_shared<ReplaySemanticShadowMetrics>(_observations, _misses, _equivalentHits, _contentMismatches, _invalidations);')
[IO.File]::WriteAllText($p,$s,[Text.UTF8Encoding]::new($false))
