$path=Join-Path (Split-Path -Parent (Split-Path -Parent $PSScriptRoot)) 'CloseToHSL\\Core\Diagnostics\DifferentialTraceCompare.h'
$text=[IO.File]::ReadAllText($path)
$pattern='(?s)\t\ttemplate<typename T> requires std::derived_from<T, IEquatable<T>>.*?\n\t\t\}\s*(?=\n\t\};)'
$replacement=@"
        template<typename T>
            requires (std::integral<T> || requires(T value) {
                { value.Equals(value) } -> std::convertible_to<bool>;
                { value.ToString() } -> std::convertible_to<std::wstring>;
            })
        static void CheckField(int index, unsigned long long pc, const std::wstring &fieldName, T legacyValue, T clusterValue, std::vector<DifferentialTraceDiscrepancy> &discrepancies)
        {
            bool equal;
            if constexpr (std::integral<T>) {
                equal = legacyValue == clusterValue;
            } else {
                equal = legacyValue.Equals(clusterValue);
            }
            if (!equal) {
                auto stringify = [](const T &value) -> std::wstring {
                    if constexpr (std::same_as<T, bool>) return value ? L"True" : L"False";
                    else if constexpr (std::integral<T>) return std::to_wstring(value);
                    else return value.ToString();
                };
                discrepancies.emplace_back(index, pc, fieldName, stringify(legacyValue), stringify(clusterValue));
            }
        }
"@
$m=[regex]::Matches($text,$pattern)
if($m.Count -eq 1){$text=[regex]::Replace($text,$pattern,$replacement);[IO.File]::WriteAllText($path,$text,[Text.UTF8Encoding]::new($false));Write-Output 'Converted CheckField'} elseif($m.Count -eq 0 -and $text.Contains('std::integral<T>')) {Write-Output 'Already fixed'} else{throw "Unexpected matches: $($m.Count)"}