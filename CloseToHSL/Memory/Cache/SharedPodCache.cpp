#include "SharedPodCache.h"
#include <algorithm>
namespace HybridCPU_ISE::CloseToHSL::Memory::Cache {
long long SharedPodCache::getTotalLookups() const { return TotalLookups; }
void SharedPodCache::setTotalLookups(long long v) { TotalLookups=v; }
long long SharedPodCache::getHits() const { return Hits; }
void SharedPodCache::setHits(long long v) { Hits=v; }
long long SharedPodCache::getMisses() const { return Misses; }
void SharedPodCache::setMisses(long long v) { Misses=v; }
SharedPodCache::SharedPodCache() : _lines(NUM_LINES), _data(NUM_LINES, std::vector<unsigned char>(LINE_SIZE_BYTES)), _domainAccessCounts(MAX_DOMAIN_COUNTERS), _domainHitCounts(MAX_DOMAIN_COUNTERS), _domainMissCounts(MAX_DOMAIN_COUNTERS), _coreMissPending(16) { for (auto& line : _lines) line.OwnerCoreId=-1; }
unsigned short SharedPodCache::getMissPendingMask() const { unsigned short mask=0; for(int i=0;i<16;++i) if(_coreMissPending[i]) mask |= static_cast<unsigned short>(1u << i); return mask; }
bool SharedPodCache::Lookup(unsigned long long address, int core, unsigned long long domain) {
 ++TotalLookups; auto slot=static_cast<int>(domain % MAX_DOMAIN_COUNTERS); ++_domainAccessCounts[slot]; auto& line=_lines[GetIndex(address)];
 if(line.Valid && line.Tag==GetTag(address) && (domain==0 || line.DomainTag==0 || line.DomainTag==domain)) { ++Hits; ++_domainHitCounts[slot]; line.LastAccessCycle=_currentCycle; SetCoreMissPending(core,false); return true; }
 ++Misses; ++_domainMissCounts[slot]; SetCoreMissPending(core,true); return false;
}
void SharedPodCache::Fill(unsigned long long address, std::span<const unsigned char> data, int core, unsigned long long domain) {
 auto index=GetIndex(address); auto& line=_lines[index]; line.Tag=GetTag(address); line.Valid=true; line.Dirty=false; line.DomainTag=domain; line.OwnerCoreId=core; line.LastAccessCycle=_currentCycle;
 std::copy_n(data.begin(),std::min(data.size(),static_cast<std::size_t>(LINE_SIZE_BYTES)),_data[index].begin()); SetCoreMissPending(core,false);
}
void SharedPodCache::Invalidate(unsigned long long address) { auto& line=_lines[GetIndex(address)]; if(line.Valid && line.Tag==GetTag(address)) line.Valid=false; }
bool SharedPodCache::IsCoreMissPending(int core) { return core>=0 && core<16 && _coreMissPending[core]; }
void SharedPodCache::SetCoreMissPending(int core,bool pending) { if(core>=0 && core<16) _coreMissPending[core]=pending; }
void SharedPodCache::ClearAllMissPending() { std::fill(_coreMissPending.begin(),_coreMissPending.end(),false); }
void SharedPodCache::AdvanceCycle() { ++_currentCycle; }
void SharedPodCache::Reset() { for(auto& line:_lines) { line.Valid=false; line.Dirty=false; line.Tag=0; line.DomainTag=0; line.OwnerCoreId=-1; } ClearAllMissPending(); TotalLookups=Hits=Misses=0; ResetDomainCounters(); }
std::tuple<long long,long long,long long> SharedPodCache::GetDomainAccessCounts(int slot) const { if(slot<0 || slot>=MAX_DOMAIN_COUNTERS) return {0,0,0}; return {_domainAccessCounts[slot],_domainHitCounts[slot],_domainMissCounts[slot]}; }
void SharedPodCache::ResetDomainCounters() { std::fill(_domainAccessCounts.begin(),_domainAccessCounts.end(),0); std::fill(_domainHitCounts.begin(),_domainHitCounts.end(),0); std::fill(_domainMissCounts.begin(),_domainMissCounts.end(),0); }
int SharedPodCache::GetIndex(unsigned long long address) { return static_cast<int>((address >> OFFSET_BITS) & (NUM_LINES-1)); }
unsigned long long SharedPodCache::GetTag(unsigned long long address) { return address >> (OFFSET_BITS+INDEX_BITS); }
}
