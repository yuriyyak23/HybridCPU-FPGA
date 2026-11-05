$ErrorActionPreference='Stop'
$root='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1'
$p=Join-Path $root 'CloseToHSL\\Core\Architecture\Common\YAKSys_Hybrid_CPU.Processor.cpp'
$s=[IO.File]::ReadAllText($p)
$s=$s.Replace('if (!ingress || ingress.Facts == nullptr)','if (!ingress || !*ingress || !(**ingress).Facts)')
$s=$s.Replace('ShadowSemanticProjection::Project(canonical)->SameFacts(ingress.Facts)','ShadowSemanticProjection::Project(canonical)->SameFacts((**ingress).Facts)')
$s=$s.Replace('if ((!ingress ? nullptr : ingress.Facts) == nullptr)','if (!ingress || !*ingress || !(**ingress).Facts)')
$s=$s.Replace('pipeIF.VLIWBundle == nullptr','pipeIF.VLIWBundle.empty()').Replace('pipeIF.BundleAnnotations == nullptr','!pipeIF.BundleAnnotations')
[IO.File]::WriteAllText($p,$s,[Text.UTF8Encoding]::new($false))
