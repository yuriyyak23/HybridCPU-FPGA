$root=Join-Path (Split-Path -Parent (Split-Path -Parent $PSScriptRoot)) 'Arch\'
$h=Join-Path $root 'DataType.h'; $c=Join-Path $root 'DataType.cpp'
$header=[IO.File]::ReadAllText($h); $cpp=[IO.File]::ReadAllText($c)
if(-not $cpp.Contains('#include "../tangible_exception_helper.h"')) { $cpp=$cpp.Replace('#include "DataType.h"', '#include "DataType.h"' + "`r`n" + '#include "../tangible_exception_helper.h"') }
$header=$header.Replace('inline static ', 'static ')
$cpp=[regex]::Replace($cpp,'(?m)^(\s*)inline (int|bool) DataTypeUtils::','$1$2 DataTypeUtils::')
$old='throw ArgumentOutOfRangeException(L"dataType", dataType, std::format("Unknown data type: {0:s}", dataType));'
$new='throw ArgumentOutOfRangeException(std::format("Unknown data type: {} (dataType)", static_cast<unsigned int>(dataType)));'
$cpp=$cpp.Replace($old,$new)
$old='return Enum::IsDefined(dataType);'
$new='switch (dataType) {' + "`r`n" + '        case DataTypeEnum::INT8: case DataTypeEnum::UINT8: case DataTypeEnum::INT16: case DataTypeEnum::UINT16:' + "`r`n" + '        case DataTypeEnum::INT32: case DataTypeEnum::UINT32: case DataTypeEnum::FLOAT32: case DataTypeEnum::INT64:' + "`r`n" + '        case DataTypeEnum::UINT64: case DataTypeEnum::FLOAT64: case DataTypeEnum::FLOAT16: case DataTypeEnum::BFLOAT16:' + "`r`n" + '        case DataTypeEnum::FLOAT8_E4M3: case DataTypeEnum::FLOAT8_E5M2: return true;' + "`r`n" + '        default: return false;' + "`r`n`t`t" + '}'
$cpp=$cpp.Replace($old,$new)
[IO.File]::WriteAllText($h,$header,[Text.UTF8Encoding]::new($false)); [IO.File]::WriteAllText($c,$cpp,[Text.UTF8Encoding]::new($false))