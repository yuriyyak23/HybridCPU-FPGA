$root=Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
$h=Join-Path $root 'CloseToHSL\\Core\Decoder\Rf06ExecutionContracts.h';$c=Join-Path $root 'CloseToHSL\\Core\Decoder\Rf06ExecutionContracts.cpp'
$ht=[IO.File]::ReadAllText($h);$ct=[IO.File]::ReadAllText($c)
if (!$ht.Contains('#include "FreezeRegisters.h"')) {$ht=$ht.Replace('#include "ExecutionBindingIdentity.h"','#include "ExecutionBindingIdentity.h"'+"`r`n"+'#include "FreezeRegisters.h"')}
$ht=$ht.Replace('ImmutableArray<int>','std::vector<int>').Replace('std::vector<int> &readRegisters = std::vector<int>()','std::vector<int> readRegisters = {}').Replace('std::vector<int> &writeRegisters = std::vector<int>()','std::vector<int> writeRegisters = {}').Replace('FreezeRegisters(std::vector<int> &registers','FreezeRegisters(const std::vector<int> &registers')
$ct=$ct.Replace('ImmutableArray<int>','std::vector<int>').Replace('std::vector<int> &readRegisters, std::vector<int> &writeRegisters','std::vector<int> readRegisters, std::vector<int> writeRegisters').Replace('FreezeRegisters(std::vector<int> &registers','FreezeRegisters(const std::vector<int> &registers')
$pattern='(?s)(std::vector<int> ExecutionContract::FreezeRegisters\(const std::vector<int> &registers, const std::wstring &parameterName\))\s*\{.*?\n\t\}'
if([regex]::Matches($ct,$pattern).Count -eq 1){$ct=[regex]::Replace($ct,$pattern,'$1 { (void)parameterName; return FreezeRegisterIds(registers); }')}
elseif(!$ct.Contains('return FreezeRegisterIds(registers);')){throw 'FreezeRegisters definition not found'}
$pattern='(?s)(ExecutionContract::ExecutionContract\([^\r\n]+\))\s*\{\s*GeneratedBinding = generatedBinding;.*?IsAssist = isAssist;\s*\}'
$init='$1 : GeneratedBinding(std::move(generatedBinding)), RuntimeProvider(std::move(runtimeProvider)), InstructionClass(instructionClass), SerializationClass(serializationClass), Placement(std::move(placement)), StaticEffectContract(staticEffectContract), Memory(std::move(memory)), StaticMemoryPlan(std::move(staticMemoryPlan)), ReadRegisters(std::move(readRegisters)), WriteRegisters(std::move(writeRegisters)), ResourceMask(resourceMask), IsStealable(isStealable), IsRetireVisible(isRetireVisible), IsAssist(isAssist) {}'
if([regex]::Matches($ct,$pattern).Count -eq 1){$ct=[regex]::Replace($ct,$pattern,$init)}elseif(!$ct.Contains('ReadRegisters(std::move(readRegisters))')){throw 'ExecutionContract constructor not found'}
[IO.File]::WriteAllText($h,$ht,[Text.UTF8Encoding]::new($false));[IO.File]::WriteAllText($c,$ct,[Text.UTF8Encoding]::new($false));Write-Output 'Converted RF06 register collections'
