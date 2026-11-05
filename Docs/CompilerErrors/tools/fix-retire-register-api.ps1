$root='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Architecture\Registers'
$h=Join-Path $root 'Physical\PhysicalRegisterFile.h'; $c=Join-Path $root 'Retire\RetireCoordinator.cpp'
$hs=[IO.File]::ReadAllText($h); $cs=[IO.File]::ReadAllText($c)
$old="#endif`r`n`r`n`t`t/// <summary>`r`n`t`t/// Initialise rename maps"; $new="#endif`r`n`tpublic:`r`n`r`n`t`t/// <summary>`r`n`t`t/// Initialise rename maps"
if(-not $hs.Contains($old)){throw 'RenameMap access marker missing'}; $hs=$hs.Replace($old,$new)
$old='archContext->getCommittedRegs()[record.getArchReg()] = record.getValue();'; if(-not $cs.Contains($old)){throw 'Register assignment missing'}
$cs=$cs.Replace($old,'archContext->getCommittedRegs()->setIndexer(record.getArchReg(), record.getValue());')
$cs=[regex]::Replace($cs,'throw ArgumentOutOfRangeException\(L"record", record\.getKind\(\), L"Unsupported retire record kind\."\);','throw std::out_of_range("Unsupported retire record kind.");')
$cs=[regex]::Replace($cs,'throw ArgumentOutOfRangeException\(L"record", record\.getVtId\(\), std::format\([^\r\n]+\)\);','throw std::out_of_range("Retire record VT identifier outside range.");')
$cs=[regex]::Replace($cs,'throw ArgumentOutOfRangeException\(L"record", record\.getArchReg\(\), std::format\([^\r\n]+\)\);','throw std::out_of_range("Retire record architectural register outside range.");')
[IO.File]::WriteAllText($h,$hs,[Text.UTF8Encoding]::new($false));[IO.File]::WriteAllText($c,$cs,[Text.UTF8Encoding]::new($false))
