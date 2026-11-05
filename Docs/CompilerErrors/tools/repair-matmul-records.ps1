$ErrorActionPreference='Stop'
$root='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1'
$h=Join-Path $root 'CloseToHSL\\Core\Execution\ExternalAccelerators\Descriptors\MatMulDescriptor.h'
$c=Join-Path $root 'CloseToHSL\\Core\Execution\ExternalAccelerators\Descriptors\MatMulDescriptor.cpp'
$hs=[IO.File]::ReadAllText($h)
$marker='enum class MatMulLayoutFlags : unsigned short'
$at=$hs.IndexOf($marker); if($at -lt 0){throw 'MatMul enum marker absent'}
$end=$hs.IndexOf('};',$at)+2
$hs=$hs.Insert($end,"`r`n`tstruct MatMulDatatypeTriple final { AcceleratorDatatype InputDatatype{}; AcceleratorDatatype AccumulatorDatatype{}; AcceleratorDatatype OutputDatatype{}; };")
$hs=$hs.Replace('};constexpr MatMulLayoutFlags operator|','};' + "`r`n" + '`tconstexpr MatMulLayoutFlags operator|')
[IO.File]::WriteAllText($h,$hs,[Text.UTF8Encoding]::new($false))
$cs=[IO.File]::ReadAllText($c)
foreach($label in @('shape accepted.','strides accepted.','datatype triple accepted.')) {
 $pattern='(?s)\s*MatMulFootprint temp_var = std::make_shared<MatMulFootprint\(\), L"'+[regex]::Escape($label)+'"\);\s*temp_var->ARange = default;\s*temp_var->BRange = default;\s*temp_var->CRange = default;'
 $cs=[regex]::Replace($cs,$pattern,"`r`n`t`tauto temp_var = std::make_shared<MatMulFootprint>();")
}
$cs=$cs.Replace('!(descriptor->LayoutFlags & MatMulLayoutFlags::RowMajorA) != MatMulLayoutFlags::None','(descriptor->LayoutFlags & MatMulLayoutFlags::RowMajorA) == MatMulLayoutFlags::None').Replace('!(descriptor->LayoutFlags & MatMulLayoutFlags::RowMajorB) != MatMulLayoutFlags::None','(descriptor->LayoutFlags & MatMulLayoutFlags::RowMajorB) == MatMulLayoutFlags::None').Replace('!(descriptor->LayoutFlags & MatMulLayoutFlags::RowMajorC) != MatMulLayoutFlags::None','(descriptor->LayoutFlags & MatMulLayoutFlags::RowMajorC) == MatMulLayoutFlags::None')
[IO.File]::WriteAllText($c,$cs,[Text.UTF8Encoding]::new($false))
