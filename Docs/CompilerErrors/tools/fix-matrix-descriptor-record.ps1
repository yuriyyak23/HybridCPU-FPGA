$p='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\ISA\Instructions\NonVmx\Lanes00_03Vector\MatrixTile\MatrixTileArchitecturalTileStateAndDescriptorAbi.h'; $s=[IO.File]::ReadAllText($p); $marker='// C# TO C++ CONVERTER TASK: C# ''records'' are not converted by C# to C++ Converter:'; $i=$s.IndexOf($marker); if($i -lt 0){throw 'Descriptor marker missing'}
$block=@"
	struct MatrixTileCanonicalDescriptorAbi final {
		unsigned short Rows = 0, Columns = 0, ElementSizeBytes = 0;
		unsigned int StrideBytes = 0;
		MatrixTileDescriptorLayoutKind Layout = MatrixTileDescriptorLayoutKind::Unspecified;
		static MatrixTileCanonicalDescriptorAbi Create(unsigned short rows, unsigned short columns, unsigned short elementSizeBytes, unsigned int strideBytes, MatrixTileDescriptorLayoutKind layout = MatrixTileDescriptorLayoutKind::RowMajor) { return {rows, columns, elementSizeBytes, strideBytes, layout}; }
		bool isZeroEncoding() const noexcept { return Rows == 0 && Columns == 0 && ElementSizeBytes == 0 && StrideBytes == 0 && Layout == MatrixTileDescriptorLayoutKind::Unspecified; }
		bool hasValidShape() const noexcept { return Rows != 0 && Columns != 0 && StrideBytes != 0; }
		bool hasValidElementSize() const noexcept { return ElementSizeBytes != 0; }
		bool hasCanonicalLayout() const noexcept { return Layout == MatrixTileDescriptorLayoutKind::RowMajor; }
		bool isCanonical() const noexcept { return hasValidShape() && hasValidElementSize() && hasCanonicalLayout(); }
		bool isReservedEncoding() const noexcept { return !isZeroEncoding() && !isCanonical(); }
		bool operator==(const MatrixTileCanonicalDescriptorAbi&) const = default;
	};

"@
$s=$s.Insert($i,$block); [IO.File]::WriteAllText($p,$s)