$ErrorActionPreference='Stop'
$p='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\Frontend\Invalidation\FreshnessState.h'; $s=[IO.File]::ReadAllText($p)
$m='static inline int publicationDepth = 0;'
if(-not $s.Contains($m)){throw 'Expected publication depth declaration missing'}
$s=$s.Replace($m,'static inline thread_local int publicationDepth = 0;')
[IO.File]::WriteAllText($p,$s,[Text.UTF8Encoding]::new($false))
