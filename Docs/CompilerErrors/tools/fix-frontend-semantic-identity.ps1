$ErrorActionPreference='Stop'
$p='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\Frontend\Contracts\Contracts.cpp'; $s=[IO.File]::ReadAllText($p)
$m='SemanticIdentity::SemanticIdentity(std::shared_ptr<DecodeContext> context, std::shared_ptr<DecodeResult> result)'
$start=$s.IndexOf($m); $end=$s.IndexOf('std::shared_ptr<DecodeContext> SemanticIdentity::getContext()',$start)
if($start -lt 0 -or $end -lt 0){throw 'Expected identity constructor missing'}
$s=$s.Substring(0,$start)+$m+' : Context(std::move(context)), Result(std::move(result)) {}'+"`r`n`r`n`t"+$s.Substring($end)
$s=$s.Replace('getContext() == other->getContext() && getResult()->SameFacts(other->getResult())','getContext() && other->getContext() && *getContext() == *other->getContext() && getResult() && getResult()->SameFacts(other->getResult())')
$s=$s.Replace('std::shared_ptr<SemanticIdentity> other = std::dynamic_pointer_cast<SemanticIdentity>(obj);', 'const auto* ptr = std::any_cast<std::shared_ptr<SemanticIdentity>>(&obj); std::shared_ptr<SemanticIdentity> other = ptr ? *ptr : nullptr;')
$s=$s.Replace('return getContext()->GetHashCode();','return 0; // Valid conservative hash; equal contexts must share a hash.')
[IO.File]::WriteAllText($p,$s,[Text.UTF8Encoding]::new($false))
