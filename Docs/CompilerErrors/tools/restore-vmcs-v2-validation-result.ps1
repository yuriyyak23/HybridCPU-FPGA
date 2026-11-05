$p='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\System\Vmcs\V2\VmcsV2FieldModel.h'; $s=[IO.File]::ReadAllText($p)
$s=$s.Replace('#pragma once',"#pragma once`r`n#include <memory>`r`n#include <string>`r`n#include <utility>")
$needle="public readonly record struct VmcsV2ValidationResult"; $i=$s.IndexOf($needle); if($i -lt 0){throw 'anchor missing'}; $begin=$s.LastIndexOf('// C# TO C++ CONVERTER TASK:', $i); $end=$s.IndexOf("// C# TO C++ CONVERTER TASK:",$i); if($end -lt 0){throw 'next anchor missing'}
$def=@'
struct VmcsV2ValidationResult {
    const bool Succeeded; const VmcsV2ValidationCode Code; const unsigned short FieldId; const std::wstring Message;
    VmcsV2ValidationResult(bool succeeded, VmcsV2ValidationCode code, unsigned short fieldId, std::wstring message) : Succeeded(succeeded), Code(code), FieldId(fieldId), Message(std::move(message)) {}
    static std::shared_ptr<VmcsV2ValidationResult> Success(unsigned short fieldId = 0) { return std::make_shared<VmcsV2ValidationResult>(true, VmcsV2ValidationCode::Success, fieldId, L""); }
    static std::shared_ptr<VmcsV2ValidationResult> Fail(VmcsV2ValidationCode code, unsigned short fieldId, std::wstring message) { return std::make_shared<VmcsV2ValidationResult>(false, code, fieldId, std::move(message)); }
};
'@
$s=$s.Substring(0,$begin)+$def+"`r`n`r`n"+$s.Substring($end); [IO.File]::WriteAllText($p,$s)