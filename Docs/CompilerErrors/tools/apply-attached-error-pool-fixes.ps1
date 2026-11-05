$ErrorActionPreference = 'Stop'
$workspace = (Resolve-Path (Join-Path $PSScriptRoot '..\..')).Path
$vmxPath = Join-Path $workspace 'NonRTL\\Arch\VmxSpecTable.h'
$content = [System.IO.File]::ReadAllText($vmxPath)

# Restore the converter-damaged C# collection expressions as standard C++ vectors.
$types = @('VmxOpcodeSpec', 'VmcsFieldSpec', 'VmxCsrSpec', 'VmExitReasonSpec')
$lines = $content -split "`r?`n"
for ($i = 0; $i -lt $lines.Count; $i++) {
    $pattern = '^(\s*static inline const std::vector<std::shared_ptr<(?<type>VmxOpcodeSpec|VmcsFieldSpec|VmxCsrSpec|VmExitReasonSpec)>>\s+\w+\s*=\s*)\[const std::vector<std::shared_ptr<\w+>>\((?<body>.*)\)\];$'
    $m = [regex]::Match($lines[$i], $pattern)
    if (-not $m.Success) { continue }
    $type = $m.Groups['type'].Value
    $body = $m.Groups['body'].Value
    if ($type -eq 'VmxOpcodeSpec') {
        $body = [regex]::Replace($body, '\b(?:Opcode|Mnemonic|OperandCount|Flags|OperandForm|OperationKindName|InternalOpKindName|RegisterRoleMask|CompatibilityNote):\s*', '')
    }
    $body = [regex]::Replace($body, ',\s*new\(', ", std::make_shared<$type>(")
    $lines[$i] = $m.Groups[1].Value + '{ std::make_shared<' + $type + '>(' + $body + ') };'
}
$content = $lines -join "`r`n"

if ($content -match '\[const std::vector<std::shared_ptr<') {
    throw 'One or more VMX collection initializers could not be converted.'
}
[System.IO.File]::WriteAllText($vmxPath, $content, [System.Text.UTF8Encoding]::new($false))
Write-Output "Converted VMX static specification collections: $vmxPath"

function Add-BeforeFinalBrace([string]$Path, [string]$Code, [string]$Marker) {
    $full = Join-Path $workspace $Path
    $text = [System.IO.File]::ReadAllText($full)
    if ($text.Contains($Marker)) { return }
    $index = $text.LastIndexOf('}')
    if ($index -lt 0) { throw "No namespace close brace in $Path" }
    $text = $text.Insert($index, $Code + "`r`n")
    [System.IO.File]::WriteAllText($full, $text, [System.Text.UTF8Encoding]::new($false))
    Write-Output "Restored record type in $Path"
}

function Add-HeaderLine([string]$Path, [string]$Header) {
    $full = Join-Path $workspace $Path
    $text = [System.IO.File]::ReadAllText($full)
    if (-not $text.Contains($Header)) {
        $text = [regex]::Replace($text, '(?m)^#pragma once\s*', "#pragma once`r`n$Header`r`n", 1)
        [System.IO.File]::WriteAllText($full, $text, [System.Text.UTF8Encoding]::new($false))
    }
}

Add-HeaderLine 'CloseToHSL\\Core\System\VectorStreamExceptionInfo.h' '#include "../Runtime/Lanes/VectorStream/State/VectorStreamState.h"'
Add-BeforeFinalBrace 'CloseToHSL\\Core\System\VectorStreamExceptionInfo.h' @'
struct VectorStreamExceptionInfo final {
    unsigned short ExecutionDomainTag, AddressSpaceTag, OwnerVirtualThreadId;
    unsigned int ExceptionMask, ExceptionPriority; unsigned char HighestExceptionIndex;
    unsigned long long FaultingPc, FaultingLane; unsigned int FaultingOpcode;
    VectorExceptionAction Action; unsigned long long Sequence;
    bool IsValid() const { return ExecutionDomainTag != 0 && HighestExceptionIndex < 5; }
    unsigned long long EncodeCompatibilityQualification() const { return (static_cast<unsigned long long>(HighestExceptionIndex) & 0x7ULL) | ((static_cast<unsigned long long>(Action) & 0x3ULL) << 8) | ((static_cast<unsigned long long>(OwnerVirtualThreadId) & 0xFFFFULL) << 16) | ((static_cast<unsigned long long>(AddressSpaceTag) & 0xFFFFULL) << 32); }
    bool RequiresCompatibilityExitProjection() const { return Action == VectorExceptionAction::ReflectAsCompatibilityExit; }
};
'@ 'struct VectorStreamExceptionInfo final'

Add-HeaderLine 'CloseToHSL\\Core\System\VectorStreamDescriptorFaultInfo.h' '#include "../Runtime/Lanes/VectorStream/State/VectorStreamState.h"'
Add-HeaderLine 'CloseToHSL\\Core\System\VectorStreamDescriptorFaultInfo.h' '#include <string>'
Add-HeaderLine 'CloseToHSL\\Core\System\VectorStreamDescriptorFaultInfo.h' '#include <utility>'
Add-BeforeFinalBrace 'CloseToHSL\\Core\System\VectorStreamDescriptorFaultInfo.h' @'
struct VectorStreamDescriptorFaultInfo final {
    unsigned short ExecutionDomainTag, AddressSpaceTag, OwnerVirtualThreadId;
    StreamDescriptorFaultKind Kind; unsigned long long GuestDescriptorAddress; unsigned int DescriptorLength;
    unsigned long long StreamReplayEpoch, Sequence; std::wstring Message;
    bool IsFaulted() const { return Kind != StreamDescriptorFaultKind::None; }
    unsigned long long EncodeCompatibilityQualification() const { return (static_cast<unsigned long long>(Kind) & 0xFFULL) | ((static_cast<unsigned long long>(OwnerVirtualThreadId) & 0xFFFFULL) << 16) | ((static_cast<unsigned long long>(AddressSpaceTag) & 0xFFFFULL) << 32); }
    VectorStreamDescriptorFaultInfo(unsigned short domain, unsigned short addressSpace, unsigned short vt, StreamDescriptorFaultKind kind, unsigned long long address, unsigned int length, unsigned long long replayEpoch, unsigned long long sequence, std::wstring message)
        : ExecutionDomainTag(domain), AddressSpaceTag(addressSpace), OwnerVirtualThreadId(vt), Kind(kind), GuestDescriptorAddress(address), DescriptorLength(length), StreamReplayEpoch(replayEpoch), Sequence(sequence), Message(std::move(message)) {}
    static inline const VectorStreamDescriptorFaultInfo None{0, 0, 0, StreamDescriptorFaultKind::None, 0, 0, 0, 0, L""};
};
'@ 'struct VectorStreamDescriptorFaultInfo final'

$directoryPath = Join-Path $workspace 'CloseToHSL\\Core\System\Vmcs\V2\VmcsV2BlockDirectory.cpp'
$directoryText = [System.IO.File]::ReadAllText($directoryPath)
$directoryText = $directoryText.Replace('InvalidationEpoch: 0', '0')
$directoryText = $directoryText.Replace('field.ToString()', 'std::to_wstring(static_cast<unsigned short>(field))')
[System.IO.File]::WriteAllText($directoryPath, $directoryText, [System.Text.UTF8Encoding]::new($false))
Write-Output "Converted VMCSv2 descriptor argument syntax: $directoryPath"
