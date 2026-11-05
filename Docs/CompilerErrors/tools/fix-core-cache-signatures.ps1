$root='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Architecture\Common\'
$h=Join-Path $root 'YAKSys_Hybrid_CPU.Processor.h'; $c=Join-Path $root 'YAKSys_Hybrid_CPU.Processor.cpp'
$s=[IO.File]::ReadAllText($h)
$s=$s.Replace('static void InvalidateVliwBundleCacheLine(std::shared_ptr<> cache,', 'class Cache_VLIWBundle_Object;'+"`r`n`t`t"+'private:'+"`r`n`t`t`t"+'static void InvalidateVliwBundleCacheLine(std::shared_ptr<std::vector<Cache_VLIWBundle_Object>> cache,').Replace('static void ClearVliwBundleCache(std::shared_ptr<> cache);','static void ClearVliwBundleCache(std::shared_ptr<std::vector<Cache_VLIWBundle_Object>> cache);')
# The definition is public, so its forward declaration must also be public.
$s=$s.Replace('private:'+"`r`n`t`t`t"+'class Cache_VLIWBundle_Object;','public:'+"`r`n`t`t`t"+'class Cache_VLIWBundle_Object;')
[IO.File]::WriteAllText($h,$s)
$s=[IO.File]::ReadAllText($c)
$s=$s.Replace('Processor::CPU_Core::InvalidateVliwBundleCacheLine(std::shared_ptr<> cache,','Processor::CPU_Core::InvalidateVliwBundleCacheLine(std::shared_ptr<std::vector<Cache_VLIWBundle_Object>> cache,').Replace('Processor::CPU_Core::ClearVliwBundleCache(std::shared_ptr<> cache)','Processor::CPU_Core::ClearVliwBundleCache(std::shared_ptr<std::vector<Cache_VLIWBundle_Object>> cache)')
$start=$s.IndexOf('void Processor::CPU_Core::InvalidateVliwBundleCacheLine('); $end=$s.IndexOf('void Processor::CPU_Core::SyncSMPChache()', $start)
if($start -lt 0 -or $end -lt 0){throw 'Cache methods not found'}
$part=$s.Substring($start,$end-$start).Replace('cache->Length','cache->size()').Replace('cache[index]->VLIWCache_MemoryAddress','(*cache)[index].VLIWCache_MemoryAddress').Replace('cache[index] = default;','(*cache)[index] = Cache_VLIWBundle_Object{};').Replace('Array::Clear(cache, 0, cache->size());','std::fill(cache->begin(), cache->end(), Cache_VLIWBundle_Object{});')
$s=$s.Remove($start,$end-$start).Insert($start,$part); [IO.File]::WriteAllText($c,$s)