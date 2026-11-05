$ErrorActionPreference='Stop'
$p=Join-Path (Split-Path -Parent (Split-Path -Parent $PSScriptRoot)) 'CloseToHSL\\Core\Pipeline\Certificates\ReplayPhaseSubstrate.Interfaces.cpp'
$s=Get-Content $p -Raw
$s=$s.Replace('return InterCoreLegalityCertificateCache();','return std::make_shared<InterCoreLegalityCertificateCache>();').Replace('return SmtLegalityCertificateCache4Way();','return std::make_shared<SmtLegalityCertificateCache4Way>();')
Set-Content $p $s -Encoding UTF8 -NoNewline
