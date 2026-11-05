$root = Join-Path (Split-Path -Parent (Split-Path -Parent $PSScriptRoot)) 'CloseToHSL\\Core\Pipeline'
$comment = '// C# TO C++ CONVERTER TASK: C++ cast operators must convert from the enclosing type to another type:'
$cases = @(
    @{ Path='Safety\ResourceBitset'; Type='ResourceBitset'; Param='unsigned long long mask'; Inline='inline '; Explicit=$false },
    @{ Path='Certificates\SafetyMask128'; Type='SafetyMask128'; Param='unsigned long long mask'; Inline=''; Explicit=$false },
    @{ Path='Certificates\ArchLegalityMask'; Type='ArchLegalityMask'; Param='SafetyMask128 m'; Inline=''; Explicit=$true },
    @{ Path='Certificates\ArchLegalityMask'; Type='BackendHazardMask'; Param='SafetyMask128 m'; Inline=''; Explicit=$true }
)
foreach ($case in $cases) {
    $h = Join-Path $root ($case.Path+'.h'); $cpp = Join-Path $root ($case.Path+'.cpp')
    $ht = [IO.File]::ReadAllText($h); $ct = [IO.File]::ReadAllText($cpp)
    $modifier = if ($case.Explicit) { 'explicit ' } else { '' }
    $decl = "static $modifier" + 'operator ' + $case.Type + '(' + $case.Param + ');'
    $def = $modifier + $case.Type + '::operator ' + $case.Type + '(' + $case.Param + ')'
    $hpattern = '(?m)^' + [regex]::Escape($comment) + '\r?\n\s*' + [regex]::Escape($case.Inline + $decl) + '\r?\n'
    $cpattern = '(?m)^' + [regex]::Escape($comment) + '\r?\n\s*' + [regex]::Escape($case.Inline + $def) + '\r?\n\s*\{\r?\n\s*return [^;]+;\r?\n\s*\}\r?\n'
    $hm=[regex]::Matches($ht,$hpattern).Count; $cm=[regex]::Matches($ct,$cpattern).Count
    if ($hm -eq 0 -and $cm -eq 0) { Write-Output "$($case.Type): already fixed"; continue }
    if ($hm -ne 1 -or $cm -ne 1) { throw "Unexpected source for $($case.Type): header=$hm cpp=$cm" }
    $ht=[regex]::Replace($ht,$hpattern,''); $ct=[regex]::Replace($ct,$cpattern,'')
    if ($case.Explicit) {
        $old=$case.Type + '(SafetyMask128 mask);'; $new='explicit ' + $old
        if (-not $ht.Contains($old)) { throw "Missing constructor: $old" }; $ht=$ht.Replace($old,$new)
    }
    if ($case.Type -eq 'ResourceBitset') { $ct=$ct.Replace('inline explicit ResourceBitset::operator unsigned long long()', 'inline ResourceBitset::operator unsigned long long()') }
    [IO.File]::WriteAllText($h,$ht,[Text.UTF8Encoding]::new($false)); [IO.File]::WriteAllText($cpp,$ct,[Text.UTF8Encoding]::new($false))
    Write-Output "Fixed $($case.Type)"
}