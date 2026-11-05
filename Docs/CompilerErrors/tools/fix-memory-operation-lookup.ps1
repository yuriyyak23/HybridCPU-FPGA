$path='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Pipeline\Safety\YAKSys_Hybrid_CPU.Core.SafetyVerifier.cpp';$s=[IO.File]::ReadAllText($path);$start=$s.IndexOf("`t`t`tstd::unordered_map<ExternalOperationIdentity*, ExternalSecureMemoryReadRequest*>::const_iterator memoryReadOperations_iterator");$end=$s.IndexOf("`t`t`tif (memoryReadOperations.size() >= 1024)",$start);if($start -lt 0 -or $end -lt 0){throw 'Lookup block missing'}
$block=@"
            if (memoryReadOperations.find(request->Operation) != memoryReadOperations.end())
                return std::make_shared<SecureComputeMemoryReadMachineResult>(request, SecureComputeMemoryReadDecision::Denied);
"@
$s=$s.Remove($start,$end-$start).Insert($start,$block+"`r`n");[IO.File]::WriteAllText($path,$s,[Text.UTF8Encoding]::new($false))
