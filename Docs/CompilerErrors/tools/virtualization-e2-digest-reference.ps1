$bytes=[Collections.Generic.List[byte]]::new()
function AddNum([UInt64]$value,[int]$n) { for($i=$n-1;$i -ge 0;$i--) { $bytes.Add([byte](($value -shr ($i*8)) -band 255)) } }
function AddText([string]$value) { $b=[Text.Encoding]::UTF8.GetBytes($value); AddNum $b.Length 4; $bytes.AddRange([byte[]]$b) }
$bytes.AddRange([byte[]][Text.Encoding]::ASCII.GetBytes("HCPUE2V2`0"))
AddNum 1 8; AddNum 2 8; AddNum 3 8; AddNum 4294967295 4; AddNum 5 4; AddNum 6 8; AddNum 7 8; AddNum 8 8
AddText 'тест'; $bytes.AddRange([byte[]](0,255,18,171)); AddNum 9 8; AddNum 10 4; AddNum 11 4
AddText 'NS'; AddText 'op'; AddNum 12 2; $bytes.AddRange([byte[]](52,86,120,154)); AddNum 13 8; AddNum 14 8; AddNum 15 8; AddNum 16 8
$sha=[Security.Cryptography.SHA256]::Create(); ([BitConverter]::ToString($sha.ComputeHash($bytes.ToArray()))).Replace('-','').ToLowerInvariant()
