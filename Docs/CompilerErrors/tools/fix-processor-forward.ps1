$p='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Architecture\Common\YAKSys_Hybrid_CPU.Processor.h'
$s=[IO.File]::ReadAllText($p)
$s=$s.Replace('namespace YAKSys_Hybrid_CPU { class CPU_Core; }','namespace YAKSys_Hybrid_CPU { class CPU_Core; }'+"
"+'namespace YAKSys_Hybrid_CPU::Arch::Generated {}'+"`r`n"+'namespace HybridCPU_ISE::CloseToHSL::Memory::Timing {}')
$s=$s.Replace('class Processor'+"`r`n`t"+'{'+"`r`n`t"+'private:'+"`r`n`t`t"+'static inline std::vector<std::shared_ptr<CPU_Core>> CPU_Cores = Array::Empty<std::shared_ptr<CPU_Core>>();', 'class Processor'+"`r`n`t"+'{'+"`r`n`t"+'public:'+"`r`n`t`t"+'class CPU_Core;'+"`r`n`t"+'private:'+"`r`n`t`t"+'static inline std::vector<std::shared_ptr<CPU_Core>> CPU_Cores;')
[IO.File]::WriteAllText($p,$s)