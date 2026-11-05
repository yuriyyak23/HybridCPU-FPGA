$ErrorActionPreference='Stop'
$p='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\Frontend\Contracts\Contracts.h'; $s=[IO.File]::ReadAllText($p)
$m="    struct AccessDecision final {`r`n        bool Allowed;`r`n        std::optional<std::wstring> Fault;"
if(-not $s.Contains($m)){$m="    struct AccessDecision final {`n        bool Allowed;`n        std::optional<std::wstring> Fault;"}
if(-not $s.Contains($m)){throw 'Expected AccessDecision definition missing'}
$s=$s.Replace($m,$m.Replace('bool Allowed;','const bool Allowed;').Replace('std::optional<std::wstring> Fault;','const std::optional<std::wstring> Fault;'))
[IO.File]::WriteAllText($p,$s,[Text.UTF8Encoding]::new($false))
