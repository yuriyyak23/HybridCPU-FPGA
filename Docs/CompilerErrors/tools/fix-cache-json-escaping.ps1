$ErrorActionPreference = 'Stop'
$p = Join-Path (Split-Path -Parent (Split-Path -Parent $PSScriptRoot)) 'Frontend\\Cache\SemanticBundleCache.cpp'
$s = Get-Content $p -Raw
if (!$s.Contains('#define NOMINMAX')) { $s = $s.Replace('#include <windows.h>', "#define NOMINMAX`r`n#include <windows.h>") }
$a = $s.IndexOf('    static void AppendJsonString(')
$b = $s.IndexOf('    static std::string BindingJson(', $a)
if ($a -lt 0 -or $b -lt 0) { throw 'JSON method boundaries missing' }
$body = @'
    static void AppendJsonString(std::string& out, const std::wstring& text) {
        constexpr char hex[] = "0123456789ABCDEF"; out.push_back('"');
        for (wchar_t ch : text) {
            unsigned int c = static_cast<unsigned int>(ch);
            if (c == 92) { out.push_back(92); out.push_back(92); continue; }
            char esc = 0;
            switch (c) { case 8: esc = 'b'; break; case 9: esc = 't'; break; case 10: esc = 'n'; break; case 12: esc = 'f'; break; case 13: esc = 'r'; break; }
            if (esc) { out.push_back(92); out.push_back(esc); continue; }
            if (c < 32 || c > 126 || c == 34 || c == 38 || c == 39 || c == 43 || c == 60 || c == 62 || c == 96) {
                out.push_back(92); out.push_back('u'); for (int shift = 12; shift >= 0; shift -= 4) out.push_back(hex[(c >> shift) & 15]);
            } else out.push_back(static_cast<char>(c));
        }
        out.push_back('"');
    }
'@
Set-Content $p ($s.Substring(0, $a) + $body + "`r`n" + $s.Substring($b)) -Encoding UTF8 -NoNewline
