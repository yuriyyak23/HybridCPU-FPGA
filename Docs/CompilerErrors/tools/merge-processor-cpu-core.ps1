$p='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Architecture\Common\YAKSys_Hybrid_CPU.Processor.h'
$backup='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\CompilerErrors\docs\snapshots\Processor.h.before-merge-cpu-core'
$s=[IO.File]::ReadAllText($p)
$pattern='(?ms)^\t\tclass CPU_Core final : public std::enable_shared_from_this<CPU_Core>\r?\n\t\t\{\r?\n(.*?)^\t\t\};'
$matches=[regex]::Matches($s,$pattern)
if($matches.Count -ne 78){throw "Expected 78 partial declarations; found $($matches.Count). No changes made."}
if(Test-Path -LiteralPath $backup){throw 'Backup already exists; refusing to overwrite.'}
[IO.File]::WriteAllText($backup,$s)
$bodies=foreach($m in $matches){$m.Groups[1].Value}
# The converter produced an empty declaration of this class before its complete definition.
$bodies[0]=[regex]::Replace($bodies[0], '(?m)^\t\t\tclass IsaOpcodeValues final\r?\n\t\t\t\{\r?\n\t\t\t\};\r?\n', '')
$combined="`t`tclass CPU_Core final : public std::enable_shared_from_this<CPU_Core>`r`n`t`t{`r`n"+($bodies -join "`r`n")+"`t`t};"
for($i=$matches.Count-1;$i -ge 0;$i--){$m=$matches[$i]; $replacement=if($i -eq 0){$combined}else{''}; $s=$s.Remove($m.Index,$m.Length).Insert($m.Index,$replacement)}
[IO.File]::WriteAllText($p,$s)