$ErrorActionPreference='Stop'
$p='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Architecture\ExceptionsAndTraps\InvalidVirtualThreadException.cpp'
$s=[IO.File]::ReadAllText($p)
$start=$s.IndexOf("`tInvalidVirtualThreadException::InvalidVirtualThreadException(")
$end=$s.IndexOf("`tstd::wstring InvalidVirtualThreadException::NormalizeOperation(",$start)
if($start -lt 0 -or $end -lt 0) { throw 'Expected constructor block not found' }
$replace=@(
'    InvalidVirtualThreadException::InvalidVirtualThreadException(const std::wstring &operation, int vtId, int vtCount)',
'        : std::runtime_error(std::wstring_convert<std::codecvt_utf8<wchar_t>>{}.to_bytes(',
'            ExecutionFaultContract::FormatMessage(ExecutionFaultCategory::InvalidVirtualThread, FormatDetail(operation, vtId, vtCount)))),',
'          Operation(NormalizeOperation(operation)), VtId(vtId), VtCount(vtCount)',
'    {',
'        // Category is exposed by getCategory and the message prefix; Exception.Data needs a native equivalent.',
'    }',
'',
'    std::wstring InvalidVirtualThreadException::FormatDetail(const std::wstring &operation, int vtId, int vtCount)',
'    {',
'        const std::wstring validRange = vtCount > 0 ? std::format(L"0..{}", vtCount - 1) : L"<none>";',
'        return std::format(L"{} attempted to access invalid virtual thread {}. Valid VT range is {}.",',
'            NormalizeOperation(operation), vtId, validRange);',
'    }',
'') -join "`r`n"
$s=$s.Substring(0,$start)+$replace+$s.Substring($end)
$s=$s.Replace('#include "InvalidVirtualThreadException.h"',"#include `"InvalidVirtualThreadException.h`"`r`n#include <codecvt>`r`n#include <locale>")
[IO.File]::WriteAllText($p,$s,[Text.UTF8Encoding]::new($false))
