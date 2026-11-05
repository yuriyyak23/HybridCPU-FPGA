$dir='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Runtime\Domains\Descriptors'
$cases=@(@('ExecutionDomain','ExecutionDomainDescriptor'),@('MemoryDomain','MemoryDomainDescriptor'),@('IoDomain','IoDomainDescriptor'))
foreach($item in $cases) {
 $name=$item[1]; $p=Join-Path $dir ($item[0]+'\YAKSys_Hybrid_CPU.Core.'+$name+'.cpp'); $s=[IO.File]::ReadAllText($p)
 $pat='(?s)('+$name+'::'+$name+'\([^\r\n]*\))\s*\{\s*((?:(?:[A-Za-z]+ = [^;]+;)\s*)+)\}'
 $match=[regex]::Match($s,$pat); if(-not $match.Success){throw "constructor missing $name"}
 $fields=[regex]::Matches($match.Groups[2].Value,'([A-Za-z]+) = ([^;]+);'); $initializers=foreach($f in $fields){$f.Groups[1].Value+'('+ $f.Groups[2].Value +')'}
 $s=$s.Replace($match.Value,$match.Groups[1].Value+' : '+($initializers -join ', ')+' {}')
 $s=$s.Replace('return new(',('return std::make_shared<'+$name+'>(')); if($name -eq 'ExecutionDomainDescriptor'){$s=$s.Replace('true, std::nullopt)','true, nullptr)')}; [IO.File]::WriteAllText($p,$s)
}
