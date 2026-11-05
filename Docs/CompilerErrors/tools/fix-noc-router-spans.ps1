$dir='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Execution\StreamEngine\BurstIO'
foreach($name in @('NoC_XY_Router.h','NoC_XY_Router.cpp')) {
 $path=Join-Path $dir $name; $s=[IO.File]::ReadAllText($path)
 foreach($pair in @(@('ReadOnlySpan<unsigned char>','std::span<const unsigned char>'),@('Span<unsigned char>','std::span<unsigned char>'))) {
   if(-not $s.Contains($pair[0])) { throw "Expected $($pair[0]) missing in $name" }
   $s=$s.Replace($pair[0],$pair[1])
 }
 if($name -eq 'NoC_XY_Router.h') { $s=$s.Replace('#pragma once',"#pragma once`r`n#include <span>") }
 [IO.File]::WriteAllText($path,$s,[Text.UTF8Encoding]::new($false))
}
