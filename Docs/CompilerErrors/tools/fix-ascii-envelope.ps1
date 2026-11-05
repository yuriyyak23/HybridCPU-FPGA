$root='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2'
$pattern='Encoding::ASCII->GetBytes\(L"([A-Za-z0-9]+)\\0"\)'
$files=Get-ChildItem -LiteralPath $root -Recurse -File -Filter '*.h'
foreach($file in $files) {
    $s=[IO.File]::ReadAllText($file.FullName)
    $t=[regex]::Replace($s,$pattern,[System.Text.RegularExpressions.MatchEvaluator]{ param($m)
        $bytes = @($m.Groups[1].Value.ToCharArray() | ForEach-Object { "'$_'" })
        $bytes += '0'
        return '{' + ($bytes -join ', ') + '}'
    })
    if($s -ne $t) { [IO.File]::WriteAllText($file.FullName,$t); Write-Output $file.FullName }
}
