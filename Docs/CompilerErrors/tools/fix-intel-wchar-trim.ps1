$path=Join-Path (Split-Path -Parent (Split-Path -Parent $PSScriptRoot)) 'tangible_\string_helper.h'
$text=[IO.File]::ReadAllText($path)
$old='return source.erase(0, source.find_first_not_of(trimChars));'
$new=@'
std::size_t first = 0;
        while (first < source.size()) {
            bool matched = false;
            for (wchar_t ch : trimChars) if (source[first] == ch) { matched = true; break; }
            if (!matched) break;
            ++first;
        }
        return source.erase(0, first);
'@
$old2='return source.erase(source.find_last_not_of(trimChars) + 1);'
$new2=@'
std::size_t last = source.size();
        while (last > 0) {
            bool matched = false;
            for (wchar_t ch : trimChars) if (source[last - 1] == ch) { matched = true; break; }
            if (!matched) break;
            --last;
        }
        return source.erase(last);
'@
if($text.Contains($old) -and $text.Contains($old2)) {$text=$text.Replace($old,$new).Replace($old2,$new2);[IO.File]::WriteAllText($path,$text,[Text.UTF8Encoding]::new($false));Write-Output 'Fixed trim methods'}
elseif(!$text.Contains($old) -and !$text.Contains($old2)){Write-Output 'Already fixed'} else {throw 'Only one expected trim method found'}
