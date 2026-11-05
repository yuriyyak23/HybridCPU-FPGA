$ErrorActionPreference='Stop'
$p='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Execution\ExecutionFaultContract.cpp'
$s=[IO.File]::ReadAllText($p)
$start=$s.IndexOf("`tbool ExecutionFaultContract::TryParseCategoryFromMessagePrefix(")
if($start -lt 0){throw 'Expected parser not found'}
$replacement=@(
'    bool ExecutionFaultContract::TryParseCategoryFromMessagePrefix(const std::wstring &message, ExecutionFaultCategory &category)',
'    {',
'        category = static_cast<ExecutionFaultCategory>(0);',
'        if (message.compare(0, PrefixLead.size(), PrefixLead) != 0) return false;',
'        const auto end = message.find(L'']'', PrefixLead.size());',
'        if (end == std::wstring::npos || end == PrefixLead.size()) return false;',
'        const auto name = message.substr(PrefixLead.size(), end - PrefixLead.size());',
'        if (name == L"UnsupportedExecutionSurface") category = ExecutionFaultCategory::UnsupportedExecutionSurface;',
'        else if (name == L"InvalidInternalOp") category = ExecutionFaultCategory::InvalidInternalOp;',
'        else if (name == L"UnsupportedVectorElementType") category = ExecutionFaultCategory::UnsupportedVectorElementType;',
'        else if (name == L"ReferenceModelValidation") category = ExecutionFaultCategory::ReferenceModelValidation;',
'        else if (name == L"InvalidPipelineLane") category = ExecutionFaultCategory::InvalidPipelineLane;',
'        else if (name == L"InvalidVirtualThread") category = ExecutionFaultCategory::InvalidVirtualThread;',
'        else {',
'            try {',
'                std::size_t count = 0;',
'                const auto numeric = std::stoul(name, &count);',
'                if (count != name.size() || numeric > 255) return false;',
'                category = static_cast<ExecutionFaultCategory>(numeric);',
'            } catch (const std::exception&) { return false; }',
'        }',
'        return true;',
'    }',
'}',
'') -join "`r`n"
$s=$s.Substring(0,$start)+$replacement
[IO.File]::WriteAllText($p,$s,[Text.UTF8Encoding]::new($false))
