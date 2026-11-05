$dir='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Pipeline\MicroOps\Control'
foreach($file in @('MicroOp.Control.h','MicroOp.Control.cpp')) {
 $path=Join-Path $dir $file; $text=[IO.File]::ReadAllText($path)
 $old='Span<RetireRecord> retireRecords'; $n=[regex]::Matches($text,[regex]::Escape($old)).Count
 if($n -ne 2){throw "Expected two signatures in $file, found $n"}
 $text=$text.Replace($old,'std::span<RetireRecord> retireRecords')
 if($file -eq 'MicroOp.Control.h' -and -not $text.Contains('#include <span>')){$text=$text.Replace('#pragma once',"#pragma once`r`n#include <span>")}
 [IO.File]::WriteAllText($path,$text,[Text.UTF8Encoding]::new($false))
}
