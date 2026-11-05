$dir = 'C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\ISA\Instructions\NonVmx\Lanes00_03Vector\MatrixTile'
$header = Join-Path $dir 'MatrixTileExecuteCaptureAbi.h'
$source = Join-Path $dir 'MatrixTileExecuteCaptureAbi.cpp'
$h = [IO.File]::ReadAllText($header)
$h = $h.Replace('#include <unordered_map>', '#include <map>')
$h = $h.Replace('std::unordered_map<(int OwnerThreadId, unsigned short TileId), std::shared_ptr<MatrixTileTileImage>> _tiles = std::unordered_map<(int OwnerThreadId, unsigned short TileId), std::shared_ptr<MatrixTileTileImage>>();', 'std::map<std::pair<int, unsigned short>, std::shared_ptr<MatrixTileTileImage>> _tiles;')
$h = $h.Replace('ReadOnlySpan<unsigned char>', 'std::span<const unsigned char>')
$h = $h.Replace('#include <map>', "#include <map>`r`n#include <span>`r`n#include <utility>")
[IO.File]::WriteAllText($header, $h)
$s = [IO.File]::ReadAllText($source)
$s = $s.Replace('ReadOnlySpan<unsigned char>', 'std::span<const unsigned char>')
$s = $s.Replace('(ValidateOwnerThreadId(ownerThreadId), tileId)', 'std::make_pair(ValidateOwnerThreadId(ownerThreadId), tileId)')
$s = $s.Replace('std::unordered_map<(int OwnerThreadId, ushort TileId)*, MatrixTileTileImage*>::const_iterator _tiles_iterator', 'auto _tiles_iterator')
$s = $s.Replace("if (_tiles_iterator == _tiles.end())`r`n`t`t{`r`n`t`t`timage = _tiles_iterator->second;`r`n`t`t`treturn false;", "if (_tiles_iterator == _tiles.end())`r`n`t`t{`r`n`t`t`treturn false;")
$s = $s.Replace("if (_tiles_iterator == _tiles.end() || !image->IsCanonicalPacked)`r`n`t`t{`r`n`t`t`timage = _tiles_iterator->second;`r`n`t`t`treturn false;", "if (_tiles_iterator == _tiles.end() || !_tiles_iterator->second->IsCanonicalPacked)`r`n`t`t{`r`n`t`t`treturn false;")
[IO.File]::WriteAllText($source, $s)
