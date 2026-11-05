$p='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Pipeline\MicroOps\Vector\VectorMicroOps.Memory.h'; $s=[IO.File]::ReadAllText($p)
foreach($outer in @('GatherMicroOp','StoreScatterMicroOp')) {
 $outerStart=$s.IndexOf("`tclass $outer : public VectorMicroOp"); if($outerStart -lt 0){throw "Missing $outer"}
 $start=$s.IndexOf("`t`tclass Indexed2SrcDescriptor`r`n`t`t{",$outerStart); if($start -lt 0){throw "Missing descriptor in $outer"}
 $end=$s.IndexOf("`t`t};",$start); if($end -lt 0){throw 'Missing descriptor end'}; $end+=5
 $block=$s.Substring($start,$end-$start); $s=$s.Remove($start,$end-$start)
 $outerStart=$s.IndexOf("`tclass $outer : public VectorMicroOp"); $field=$s.IndexOf("`t`tIndexed2SrcDescriptor _descriptor;",$outerStart); if($field -lt 0){throw 'Missing descriptor field'}
 $s=$s.Insert($field,$block+"`r`n`r`n`tprivate:`r`n")
}
[IO.File]::WriteAllText($p,$s)