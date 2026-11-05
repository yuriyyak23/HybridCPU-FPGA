$ErrorActionPreference='Stop'
$p='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\Frontend\Contracts\Contracts.h'; $s=[IO.File]::ReadAllText($p)
$m="`tclass ICanonicalSource"; if(-not $s.Contains($m) -or $s.Contains('struct AccessDecision final')){throw 'Unexpected contracts state'}
$add=@(
'    struct AccessDecision final {',
'        bool Allowed;',
'        std::optional<std::wstring> Fault;',
'        static const std::shared_ptr<AccessDecision> Permit;',
'    };',
'    inline const std::shared_ptr<AccessDecision> AccessDecision::Permit = std::make_shared<AccessDecision>(AccessDecision{true, std::nullopt});',
'    struct CanonicalFetchResult final {',
'        std::shared_ptr<FrozenBytes> Raw;',
'        std::optional<std::wstring> Fault;',
'    };',
'') -join "`r`n"
$s=$s.Replace($m,$add+$m)
[IO.File]::WriteAllText($p,$s,[Text.UTF8Encoding]::new($false))
