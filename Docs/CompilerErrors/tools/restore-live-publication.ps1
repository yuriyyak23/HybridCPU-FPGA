$p='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Runtime\Completion\Records\DomainHypercallCompletionOwner.h'; $s=[IO.File]::ReadAllText($p); $anchor='class DomainHypercallCompletionOwner final'; $decl="class DomainHypercallCompletionPublicationResult;`r`n    class DomainHypercallRetireOwner;`r`n    "
if(-not $s.Contains('class DomainHypercallCompletionPublicationResult;')) {$s=$s.Replace($anchor,$decl+$anchor)}
$anchor='// C# TO C++ CONVERTER TASK: C# ''records'' are not converted by C# to C++ Converter:' + "`r`n" + '//`t`tprivate sealed record LivePublication(CompletionRecord Completion, DomainHypercallLifecycleGate LifecycleGate);'
$def=@"
        struct LivePublication final {
            const std::shared_ptr<CompletionRecord> Completion;
            const std::shared_ptr<DomainHypercallLifecycleGate> LifecycleGate;
            LivePublication(std::shared_ptr<CompletionRecord> completion, std::shared_ptr<DomainHypercallLifecycleGate> gate) : Completion(std::move(completion)), LifecycleGate(std::move(gate)) {}
        };
"@
if(-not $s.Contains('struct LivePublication final')) { $pattern='(?m)^\s*//\s*private sealed record LivePublication\(CompletionRecord Completion, DomainHypercallLifecycleGate LifecycleGate\);'; if(-not [regex]::IsMatch($s,$pattern)){throw 'LivePublication anchor missing'}; $s=[regex]::Replace($s,$pattern,$def) }; if(-not $s.Contains('#include <utility>')) {$s=$s.Replace('#include <memory>',"#include <memory>`r`n#include <utility>")}; [IO.File]::WriteAllText($p,$s)
