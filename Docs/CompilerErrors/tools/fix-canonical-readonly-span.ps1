$root='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Decoder'
foreach ($name in @('CanonicalDecodedContracts.h','CanonicalDecodedContracts.cpp')) {
 $p=Join-Path $root $name
 $s=[IO.File]::ReadAllText($p)
 $s=$s.Replace('ReadOnlySpan<VLIW_Instruction> rawSlots', 'std::span<const VLIW_Instruction> rawSlots')
 $s=$s.Replace('rawSlots.Length', 'rawSlots.size()')
 $s=$s.Replace('std::optional<std::shared_ptr<CanonicalDecodeContext>> &context = std::nullopt', 'std::shared_ptr<CanonicalDecodeContext> context = nullptr')
 $s=$s.Replace('std::optional<std::shared_ptr<CanonicalDecodeContext>> &context)', 'std::shared_ptr<CanonicalDecodeContext> context)')
 if($name -eq 'CanonicalDecodedContracts.h' -and -not $s.Contains('#include <span>')) { $s=$s.Replace('#include <vector>',"#include <vector>`r`n#include <span>") }
 [IO.File]::WriteAllText($p,$s)
}
