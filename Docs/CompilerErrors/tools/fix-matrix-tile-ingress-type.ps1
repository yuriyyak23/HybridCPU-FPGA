$dir = 'C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\ISA\Instructions\NonVmx\Lanes00_03Vector\MatrixTile'
$header = Join-Path $dir 'MatrixTileExecuteCaptureAbi.h'
$source = Join-Path $dir 'MatrixTileExecuteCaptureAbi.cpp'
foreach ($file in @($header, $source)) {
    $s = [IO.File]::ReadAllText($file)
    $s = $s.Replace('std::function<unsigned char[]?(unsigned long long, int)>', 'std::function<std::optional<std::vector<unsigned char>>(unsigned long long, int)>')
    if ($file -eq $source) {
        $s = $s.Replace('unsigned char[]? rowBytesBuffer', 'auto rowBytesBuffer')
        $s = $s.Replace('rowBytesBuffer == nullptr || rowBytesBuffer->Length != rowBytes', '!rowBytesBuffer || rowBytesBuffer->size() != static_cast<size_t>(rowBytes)')
        $s = $s.Replace('Buffer::BlockCopy(rowBytesBuffer, 0, packed, row * rowBytes, rowBytes);', 'std::copy(rowBytesBuffer->begin(), rowBytesBuffer->end(), packed.begin() + row * rowBytes);')
        $s = $s.Replace('#include "MatrixTileExecuteCaptureAbi.h"', "#include `"MatrixTileExecuteCaptureAbi.h`"`r`n#include <algorithm>")
    }
    [IO.File]::WriteAllText($file, $s)
}
foreach ($name in @('MatrixTileStreamTransferAbi.h', 'MatrixTileStreamTransferAbi.cpp')) {
    $file = Join-Path $dir $name
    $s = [IO.File]::ReadAllText($file)
    $s = $s.Replace('unsigned char[]? std::shared_ptr<> ReadIngress', 'std::optional<std::vector<unsigned char>> ReadIngress')
    $s = $s.Replace('unsigned char[]? std::shared_ptr<> MatrixTileStreamTransferSession::ReadIngress', 'std::optional<std::vector<unsigned char>> MatrixTileStreamTransferSession::ReadIngress')
    if ($name -eq 'MatrixTileStreamTransferAbi.cpp') {
        $start = $s.IndexOf('std::optional<std::vector<unsigned char>> MatrixTileStreamTransferSession::ReadIngress')
        $end = $s.IndexOf('std::shared_ptr<MatrixTileTileImage> MatrixTileStreamTransferSession::StageEgress', $start)
        if ($start -lt 0 -or $end -lt 0) { throw 'ReadIngress boundary not found' }
        $body = $s.Substring($start, $end - $start).Replace('return nullptr;', 'return std::nullopt;')
        $s = $s.Substring(0, $start) + $body + $s.Substring($end)
    }
    [IO.File]::WriteAllText($file, $s)
}
