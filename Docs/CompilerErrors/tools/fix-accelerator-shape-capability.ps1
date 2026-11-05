$ErrorActionPreference='Stop'
$p='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Execution\ExternalAccelerators\Capabilities\AcceleratorShapeCapability.cpp'
$s=[IO.File]::ReadAllText($p)
$m='AcceleratorShapeCapability::AcceleratorShapeCapability(const std::wstring &shapeKind, unsigned long long minElements, unsigned long long maxElements, unsigned char minRank, unsigned char maxRank)'
if(-not $s.Contains($m)){throw 'Expected constructor missing'}
$s=$s.Replace('#include "AcceleratorShapeCapability.h"',"#include `"AcceleratorShapeCapability.h`"`r`n#define NOMINMAX`r`n#include <Windows.h>`r`n#include <limits>")
$s=$s.Replace($m,$m+' : ShapeKind(shapeKind), MinElements(minElements), MaxElements(maxElements), MinRank(minRank), MaxRank(maxRank)')
$s=$s.Replace('throw ArgumentOutOfRangeException(L"minElements", L"Minimum elements must not exceed maximum elements.");','throw std::out_of_range("Minimum elements must not exceed maximum elements.");').Replace('throw ArgumentOutOfRangeException(L"minRank", L"Minimum rank must not exceed maximum rank.");','throw std::out_of_range("Minimum rank must not exceed maximum rank.");')
foreach($line in @('ShapeKind = shapeKind;','MinElements = minElements;','MaxElements = maxElements;','MinRank = minRank;','MaxRank = maxRank;')){$s=$s.Replace($line,'')}
$m='return std::wstring::Equals(getShapeKind(), shapeKind, StringComparison::OrdinalIgnoreCase) && elementCount >= getMinElements() && elementCount <= getMaxElements() && rank >= getMinRank() && rank <= getMaxRank();'
if(-not $s.Contains($m)){throw 'Expected comparison missing'}
$replace='return ShapeKind.size() <= static_cast<std::size_t>(std::numeric_limits<int>::max()) && shapeKind.size() <= static_cast<std::size_t>(std::numeric_limits<int>::max()) && CompareStringOrdinal(ShapeKind.data(), static_cast<int>(ShapeKind.size()), shapeKind.data(), static_cast<int>(shapeKind.size()), TRUE) == CSTR_EQUAL && elementCount >= MinElements && elementCount <= MaxElements && rank >= MinRank && rank <= MaxRank;'
$s=$s.Replace($m,$replace); [IO.File]::WriteAllText($p,$s,[Text.UTF8Encoding]::new($false))
