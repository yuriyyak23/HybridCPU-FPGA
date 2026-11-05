$p = 'C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\ISA\Instructions\NonVmx\Lanes00_03Vector\MatrixTile\MatrixTileLayoutPolicyAbi.cpp'
$s = [IO.File]::ReadAllText($p)
$s = $s.Replace('Fingerprint: 0)', '0)')
$s = $s.Replace('//'+"`t`t"+'return policy with { Fingerprint = ComputeFingerprint(policy) };', '//'+"`t`t"+'return policy with { Fingerprint = ComputeFingerprint(policy) };'+"`r`n`t`tpolicy->Fingerprint = ComputeFingerprint(policy);`r`n`t`treturn policy;")
$s = $s.Replace('if (expected->Equals(nullptr))', 'std::shared_ptr<MatrixTileLayoutPolicy> expected;'+"`r`n`t`t"+'switch (expectedOperationKind) {'+"`r`n`t`t"+'case MatrixTileProjectedOperationKind::Macc: expected = CreateMaccPolicy(); break;'+"`r`n`t`t"+'case MatrixTileProjectedOperationKind::Transpose: expected = CreateTransposePolicy(); break;'+"`r`n`t`t"+'default: break;'+"`r`n`t`t}"+"`r`n`t`t"+'if (!expected)')
$s = $s.Replace('return value == expected ?', 'return *value == *expected ?')
$s = $s.Replace('index < descriptors->Length', 'static_cast<size_t>(index) < descriptors.size()')
[IO.File]::WriteAllText($p, $s)
