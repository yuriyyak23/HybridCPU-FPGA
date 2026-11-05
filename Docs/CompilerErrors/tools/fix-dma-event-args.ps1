$root='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2'
$path=Join-Path $root 'tangible_event.h'
$s=[IO.File]::ReadAllText($path)
$marker='template<typename T>'
if(-not $s.Contains($marker) -or $s.Contains('class EventArgs')) { throw 'Unexpected event header state' }
$s=$s.Replace($marker,"// Base for event payloads converted from System.EventArgs.`r`nclass EventArgs { public: virtual ~EventArgs() = default; };`r`n`r`n"+$marker)
[IO.File]::WriteAllText($path,$s,[Text.UTF8Encoding]::new($false))
