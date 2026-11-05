$ErrorActionPreference='Stop'
$root=Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
$jobs=@(
 @{path='CloseToHSL\\Core\Pipeline\Certificates\BundleResourceCertificate.h'; names=@('BundleResourceCertificateIdentity')},
 @{path='CloseToHSL\\Core\Pipeline\Certificates\BundleResourceCertificate4Way.h'; names=@('BundleResourceCertificateIdentity4Way')},
 @{path='CloseToHSL\\Core\Pipeline\Certificates\ReplayPhaseSubstrate.h'; names=@('ClassCapacityTemplate','ReplayPhaseKey','PhaseCertificateTemplateKey','PhaseCertificateTemplate')},
 @{path='CloseToHSL\\Core\Pipeline\Certificates\ReplayPhaseSubstrate.Implementations.h'; names=@('BoundaryGuardState','SmtBundleMetadata4Way','PhaseCertificateTemplateKey4Way','PhaseCertificateTemplate4Way')}
)
foreach($job in $jobs){
 $p=Join-Path $root $job.path; $s=Get-Content $p -Raw
 foreach($name in $job.names){
  $a=$s.IndexOf("class $name`r`n"); if($a -lt 0){$a=$s.IndexOf("class $name`n")}; $needle="$name() = default;"; $end=$s.IndexOf($needle,$a)
  if($a -lt 0 -or $end -lt 0){throw "Missing class or default constructor: $name"}; $end += $needle.Length
  $block=$s.Substring($a,$end-$a)
  $block=[regex]::Replace($block,'(?m)^(\s*)const ((?:unsigned long long|unsigned int|unsigned short|unsigned char|long long|int|bool|[A-Za-z_][\w:<>]*|std::shared_ptr<[^>]+>)) (\w+);','$1$2 $3{};')
  $block=$block.Replace($needle,"public:`r`n        $needle")
  $s=$s.Substring(0,$a)+$block+$s.Substring($end)
 }
 Set-Content $p $s -Encoding UTF8 -NoNewline
}
