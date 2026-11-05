$root=Join-Path (Split-Path -Parent (Split-Path -Parent $PSScriptRoot)) 'NonRTL\\Arch'
$cpp=Join-Path $root 'InstructionClassifier.cpp'; $header=Join-Path $root 'InstructionClassifier.h'
$text=[IO.File]::ReadAllText($cpp)
$pattern='(?s)// C# TO C\+\+ CONVERTER TASK: The following ''switch expression'' was not converted by C# to C\+\+ Converter:\s*//\s*return opcode switch\s*//\s*\{(?<body>.*?)//\s*\};'
$blocks=[regex]::Matches($text,$pattern)
if($blocks.Count -eq 0 -and $text.Contains('if (opcode == Processor::CPU_Core::InstructionsEnum::')) {Write-Output 'Already fixed';exit 0}
if($blocks.Count -ne 2){throw "Expected 2 switches; found $($blocks.Count)"}
$expected=New-Object System.Collections.Generic.List[string]
$replacement=[Text.RegularExpressions.MatchEvaluator]{ param($m)
 $body=$m.Groups['body'].Value
 $rows=[regex]::Matches($body,'(?m)^\s*//\s*Processor\.CPU_Core\.InstructionsEnum\.(\w+)\s*=>\s*(InstructionClass|SerializationClass)\.(\w+),')
 $fallback=[regex]::Match($body,'(?m)^\s*//\s*_\s*=>\s*(InstructionClass|SerializationClass)\.(\w+),')
 if($rows.Count -lt 1 -or !$fallback.Success){throw 'Switch row or default missing'}
 $allArrows=[regex]::Matches($body,'=>').Count
 if($allArrows -ne $rows.Count+1){throw "Unrecognized switch cases: arrows=$allArrows rows=$($rows.Count)"}
 $out=New-Object System.Collections.Generic.List[string]
 foreach($row in $rows){$op=$row.Groups[1].Value;$type=$row.Groups[2].Value;$val=$row.Groups[3].Value; $out.Add("if (opcode == Processor::CPU_Core::InstructionsEnum::$op) return $($type)::$val;");$expected.Add("$type,$op,$val")}
 $type=$fallback.Groups[1].Value;$val=$fallback.Groups[2].Value;$out.Add("return $($type)::$val;");$expected.Add("$type,__DEFAULT__,$val")
 [string]::Join("`r`n`t`t",$out)
}
$text=[regex]::Replace($text,$pattern,$replacement)
$text=[regex]::Replace($text,'(?m)^(\s*)inline (InstructionClass|SerializationClass) InstructionClassifier::','$1$2 InstructionClassifier::')
$h=[IO.File]::ReadAllText($header).Replace('inline static InstructionClass GetClass','static InstructionClass GetClass').Replace('inline static SerializationClass GetSerializationClass','static SerializationClass GetSerializationClass')
[IO.File]::WriteAllText($cpp,$text,[Text.UTF8Encoding]::new($false));[IO.File]::WriteAllText($header,$h,[Text.UTF8Encoding]::new($false));[IO.File]::WriteAllLines((Join-Path $PSScriptRoot 'classifier-expected.csv'),$expected,[Text.Encoding]::ASCII)
Write-Output "Restored $($expected.Count - 2) mappings and 2 defaults"