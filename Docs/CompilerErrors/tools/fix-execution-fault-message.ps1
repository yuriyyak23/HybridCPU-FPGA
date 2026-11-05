$ErrorActionPreference='Stop'
$p='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Execution\ExecutionFaultContract.cpp'
$s=[IO.File]::ReadAllText($p)
$start=$s.IndexOf("`tstd::wstring ExecutionFaultContract::GetMessagePrefix(")
$end=$s.IndexOf("`tvoid ExecutionFaultContract::Stamp(",$start)
if($start -lt 0 -or $end -lt 0){throw 'Missing prefix block'}
$replacement=@(
'    std::wstring ExecutionFaultContract::GetMessagePrefix(ExecutionFaultCategory category)',
'    {',
'        const wchar_t* name = nullptr;',
'        switch (category) {',
'        case ExecutionFaultCategory::UnsupportedExecutionSurface: name = L"UnsupportedExecutionSurface"; break;',
'        case ExecutionFaultCategory::InvalidInternalOp: name = L"InvalidInternalOp"; break;',
'        case ExecutionFaultCategory::UnsupportedVectorElementType: name = L"UnsupportedVectorElementType"; break;',
'        case ExecutionFaultCategory::ReferenceModelValidation: name = L"ReferenceModelValidation"; break;',
'        case ExecutionFaultCategory::InvalidPipelineLane: name = L"InvalidPipelineLane"; break;',
'        case ExecutionFaultCategory::InvalidVirtualThread: name = L"InvalidVirtualThread"; break;',
'        default: return PrefixLead + std::to_wstring(static_cast<unsigned int>(category)) + L"]";',
'        }',
'        return PrefixLead + name + L"]";',
'    }',
'',
'    std::wstring ExecutionFaultContract::FormatMessage(ExecutionFaultCategory category, const std::wstring &detail)',
'    {',
'        if (StringHelper::isEmptyOrWhiteSpace(detail))',
'            throw std::invalid_argument("Execution fault detail must not be null or empty.");',
'        return GetMessagePrefix(category) + L" " + detail;',
'    }',
'') -join "`r`n"
$s=$s.Substring(0,$start)+$replacement+$s.Substring($end)
[IO.File]::WriteAllText($p,$s,[Text.UTF8Encoding]::new($false))
