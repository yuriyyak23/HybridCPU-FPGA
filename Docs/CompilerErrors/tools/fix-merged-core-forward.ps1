$h='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Architecture\Common\YAKSys_Hybrid_CPU.Processor.h'
$c='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Architecture\Common\YAKSys_Hybrid_CPU.Processor.cpp'
$s=[IO.File]::ReadAllText($h)
$s=$s.Replace('class MatrixTileTileImage; class MatrixTileCapturedMemoryWrite;', 'class MatrixTileTileImage; class MatrixTileCapturedMemoryWrite; class MatrixTileArchitecturalTileRegisterFile; class MatrixTileCanonicalDescriptorAbi;')
$s=$s.Replace('namespace YAKSys_Hybrid_CPU::Core { class VectorTransferMicroOp; class CoreRuntimeState; }','namespace YAKSys_Hybrid_CPU::Core { class VectorTransferMicroOp; class CoreRuntimeState; class CpuCorePlatformContext; class VmxRetireOutcome; }')
$s=$s.Replace('class CPU_Core;'+"`r`n`t"+'private:', 'class CPU_Core;'+"`r`n`t`t"+'class MainMemoryArea;'+"`r`n`t`t"+'enum class DeviceType : int;'+"`r`n`t"+'private:')
$s=$s.Replace('class CPU_Core final : public std::enable_shared_from_this<CPU_Core>'+"`r`n`t`t"+'{', 'class CPU_Core final : public std::enable_shared_from_this<CPU_Core>'+"`r`n`t`t"+'{'+"`r`n`t`t"+'public:'+"`r`n`t`t`t"+'enum class VectorException : unsigned char;')
$s=$s.Replace('ReadOnlySpan<unsigned char> packedData','std::span<const unsigned char> packedData')
[IO.File]::WriteAllText($h,$s)
$s=[IO.File]::ReadAllText($c).Replace('ReadOnlySpan<unsigned char> packedData','std::span<const unsigned char> packedData')
[IO.File]::WriteAllText($c,$s)