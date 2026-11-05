$path='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Architecture\Common\YAKSys_Hybrid_CPU.Processor.cpp'
$s=[IO.File]::ReadAllText($path)
$old='[&] (int arg1, ArchContextState* arg2) {PublishArchitecturalContext(arg1, arg2);}'
$new='[this] (int vtId, std::shared_ptr<ArchContextState> context) { PublishArchitecturalContext(vtId, std::move(context)); }'
if(-not $s.Contains($old)){throw 'Expected publisher callback missing'}
$s=$s.Replace($old,$new); [IO.File]::WriteAllText($path,$s,[Text.UTF8Encoding]::new($false))
