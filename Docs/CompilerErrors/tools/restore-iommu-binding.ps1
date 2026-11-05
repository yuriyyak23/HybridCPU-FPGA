$r='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Runtime'
$p=Join-Path $r 'IO\Dma\DmaDomainBinding.h'; $s=[IO.File]::ReadAllText($p); $s=$s.Replace('using namespace HybridCPU_ISE::CloseToHSL::Memory::MMU;',"namespace HybridCPU_ISE::CloseToHSL::Memory::MMU { enum class IOMMUAccessPermissions : int; }`r`nusing namespace HybridCPU_ISE::CloseToHSL::Memory::MMU;")
$anchor='enum class DmaFaultKind'; $def=@"
struct IommuDomainBinding final {
    unsigned short IoDomainTag = 0; unsigned int DomainId = 0; unsigned long long DomainTag = 0; unsigned int DeviceId = 0;
    IOMMUAccessPermissions Permissions = static_cast<IOMMUAccessPermissions>(0);
    unsigned long long DomainEpoch = 0; bool NonCoherent = false; bool RequiresFence = false;
    bool isValid() const noexcept { return IoDomainTag != 0 && DomainId != 0 && DomainTag != 0 && DeviceId != 0 && static_cast<int>(Permissions) != 0 && DomainEpoch != 0; }
    bool Allows(IOMMUAccessPermissions requested) const noexcept { auto r = static_cast<int>(requested); return r != 0 && (static_cast<int>(Permissions) & r) == r; }
    IommuDomainBinding WithEpoch(unsigned long long epoch) const { auto copy = *this; copy.DomainEpoch = epoch ? epoch : 1; return copy; }
    static IommuDomainBinding Create(unsigned short ioTag, unsigned int domainId, unsigned long long domainTag, unsigned int deviceId, IOMMUAccessPermissions permissions, unsigned long long epoch = 1, bool nonCoherent = false, bool requiresFence = false) { return {ioTag, domainId, domainTag, deviceId, permissions, epoch ? epoch : 1, nonCoherent, requiresFence}; }
    bool operator==(const IommuDomainBinding&) const = default;
};

"@
if(-not $s.Contains('struct IommuDomainBinding final')) {$s=$s.Replace($anchor,$def+$anchor)}; [IO.File]::WriteAllText($p,$s)
$p=Join-Path $r 'Domains\Descriptors\IoDomain\IoVirtualizationBlock.h'; $s=[IO.File]::ReadAllText($p); $s=$s.Replace('#include <unordered_map>',"#include <map>`r`n#include <tuple>`r`n#include `"../../../IO/Dma/DmaDomainBinding.h`""); $s=$s.Replace('using namespace YAKSys_Hybrid_CPU::Core::Execution::DmaStreamCompute;',"namespace YAKSys_Hybrid_CPU::Core::Execution::DmaStreamCompute { class DmaStreamComputeOwnerBinding; }`r`nusing namespace YAKSys_Hybrid_CPU::Core::Execution::DmaStreamCompute;")
$pat='std::unordered_map<\(unsigned short IoDomainTag, unsigned long long DomainTag, unsigned int DeviceId\), std::shared_ptr<IommuDomainBinding>> _bindings = std::unordered_map<\(unsigned short IoDomainTag, unsigned long long DomainTag, unsigned int DeviceId\), std::shared_ptr<IommuDomainBinding>>\(\);'; $s=[regex]::Replace($s,$pat,'std::map<std::tuple<unsigned short, unsigned long long, unsigned int>, std::shared_ptr<IommuDomainBinding>> _bindings;'); [IO.File]::WriteAllText($p,$s)
