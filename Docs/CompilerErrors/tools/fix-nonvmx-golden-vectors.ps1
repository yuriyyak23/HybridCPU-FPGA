$ErrorActionPreference = 'Stop'
$workspaceRoot = (Resolve-Path (Join-Path $PSScriptRoot '..\..')).Path
$instructionRoot = Join-Path $workspaceRoot 'CloseToHSL\\Core\ISA\Instructions\NonVmx\Lanes00_03Scalar'
$excluded = @('AddressGeneration', 'BitManipulation\BitCount', 'BitManipulation\BitExtract', 'BitManipulation\BitSetClearInvert')
$edited = [System.Collections.Generic.List[string]]::new()

foreach ($source in Get-ChildItem -LiteralPath $instructionRoot -Recurse -Filter '*.cpp') {
	$relative = [System.IO.Path]::GetRelativePath($instructionRoot, $source.FullName)
	if ($excluded | Where-Object { $relative.StartsWith($_, [System.StringComparison]::OrdinalIgnoreCase) }) { continue }

	$text = [System.IO.File]::ReadAllText($source.FullName)
	$vectorReturn = [regex]::Match($text, 'std::vector<(?<type>\w+)> (?<class>\w+)::GetLocalGoldenVectors\(\)')
	if (-not $vectorReturn.Success) { continue }
	$vectorType = $vectorReturn.Groups['type'].Value
	$className = $vectorReturn.Groups['class'].Value
	$headerPath = [System.IO.Path]::ChangeExtension($source.FullName, '.h')
	if (-not (Test-Path -LiteralPath $headerPath)) { throw "Missing header for $($source.FullName)" }
	$headerText = [System.IO.File]::ReadAllText($headerPath)
	$changed = $false

	# C# converter emitted namespace-level forward declarations even though the type is nested.
	$declaration = "(?m)(class\s+$([regex]::Escape($className))\b[^\r\n]*\r?\n\s*\{\r?\n)"
	$declMatch = [regex]::Match($headerText, $declaration)
	if (-not $declMatch.Success) { throw "Could not locate class body for $className in $headerPath" }
	$methodIndex = $headerText.IndexOf('GetLocalGoldenVectors', $declMatch.Index, [System.StringComparison]::Ordinal)
	$nestedIndex = $headerText.IndexOf("class $vectorType;", $declMatch.Index, [System.StringComparison]::Ordinal)
	$nestedDefinitionMatch = [regex]::Match($headerText.Substring($declMatch.Index + $declMatch.Length), "class\s+$([regex]::Escape($vectorType))\s*\{")
	if (($nestedIndex -lt 0 -or $nestedIndex -gt $methodIndex) -and $nestedDefinitionMatch.Success) {
		$newline = if ($headerText.Contains("`r`n")) { "`r`n" } else { "`n" }
		$insertion = "`tpublic:$newline`t`tclass $vectorType;$newline$newline"
		$headerText = $headerText.Insert($declMatch.Index + $declMatch.Length, $insertion)
		$changed = $true
	}

	$qualifiedReturn = "std::vector<$className`::$vectorType> $className::GetLocalGoldenVectors()"
	$unqualifiedReturn = "std::vector<$vectorType> $className::GetLocalGoldenVectors()"
	if ($text.Contains($unqualifiedReturn)) {
		$text = $text.Replace($unqualifiedReturn, $qualifiedReturn)
		$changed = $true
	}

	# Translate C# target-typed `new(args)` elements to aggregate/value initialization.
	$valuesBlock = [regex]::Match($text, "(?s)(return\s+std::vector<$([regex]::Escape($className))::$([regex]::Escape($vectorType))>\s*\{)(.*?)(\};)")
	if (-not $valuesBlock.Success) {
		$valuesBlock = [regex]::Match($text, "(?s)(return\s+std::vector<$([regex]::Escape($vectorType))>\s*\{)(.*?)(\};)")
	}
	if ($valuesBlock.Success) {
		$values = $valuesBlock.Groups[2].Value
		$convertedValues = [regex]::Replace($values, '\bnew\s*\(([^()]*)\)', '{$1}')
		if ($convertedValues -ne $values) {
			$text = $text.Remove($valuesBlock.Groups[2].Index, $valuesBlock.Groups[2].Length).Insert($valuesBlock.Groups[2].Index, $convertedValues)
			$changed = $true
		}
	}

	$ctorStart = "$className`::$vectorType`::$vectorType"
	$ctorPattern = "(?s)(?<signature>$([regex]::Escape($ctorStart))\s*\((?<args>[^)]*)\))\s*\{(?<body>[^{}]*)\}"
	$ctorMatch = [regex]::Match($text, $ctorPattern)
	if ($ctorMatch.Success) {
		$body = $ctorMatch.Groups['body'].Value
		$assignments = [regex]::Matches($body, '(?<field>\w+)\s*=\s*(?<value>\w+)\s*;')
		$remainder = [regex]::Replace($body, '(?<field>\w+)\s*=\s*(?<value>\w+)\s*;', '').Trim()
		if ($assignments.Count -gt 0 -and $remainder.Length -eq 0) {
			$newline = if ($text.Contains("`r`n")) { "`r`n" } else { "`n" }
			$initializers = @($assignments | ForEach-Object { "$($_.Groups['field'].Value)($($_.Groups['value'].Value))" }) -join ', '
			$replacement = $ctorMatch.Groups['signature'].Value + $newline + "`t`t: $initializers$newline`t{" + $newline + "`t}" 
			$text = $text.Remove($ctorMatch.Index, $ctorMatch.Length).Insert($ctorMatch.Index, $replacement)
			$changed = $true
		}
	}

	if ($changed) {
		$sourceBytes = [System.IO.File]::ReadAllBytes($source.FullName)
		$headerBytes = [System.IO.File]::ReadAllBytes($headerPath)
		$sourceHasBom = $sourceBytes.Length -ge 3 -and $sourceBytes[0] -eq 0xEF -and $sourceBytes[1] -eq 0xBB -and $sourceBytes[2] -eq 0xBF
		$headerHasBom = $headerBytes.Length -ge 3 -and $headerBytes[0] -eq 0xEF -and $headerBytes[1] -eq 0xBB -and $headerBytes[2] -eq 0xBF
		[System.IO.File]::WriteAllText($source.FullName, $text, [System.Text.UTF8Encoding]::new($sourceHasBom))
		[System.IO.File]::WriteAllText($headerPath, $headerText, [System.Text.UTF8Encoding]::new($headerHasBom))
		$edited.Add($relative)
	}
}

$edited | ForEach-Object { Write-Output $_ }
Write-Output "EDITED_SOURCE_COUNT=$($edited.Count)"
