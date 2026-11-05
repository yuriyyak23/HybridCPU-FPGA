$p='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\ISA\Instructions\NonVmx\Lanes00_03Vector\MatrixTile\MatrixTileAccumulatorAndTransposePolicyAbi.h'; $s=[IO.File]::ReadAllText($p); $marker="`tclass MatrixTileAccumulatorAndTransposePolicyAbi"; $idx=$s.IndexOf($marker); if($idx -lt 0){throw 'ABI class not found'}
$block=@"
	class MatrixTileCanonicalDescriptorAbi;
	struct MatrixTileSemanticValidationResult final {
		bool IsValid = false;
		MatrixTileSemanticFaultKind FaultKind = MatrixTileSemanticFaultKind::None;
		std::shared_ptr<MatrixTileCanonicalDescriptorAbi> ResultDescriptor;
		unsigned short ResultElementSizeBytes = 0;
		bool RequiresRetirePublication = false, RequiresReplayIdentity = false, UsesFallbackPath = false;
		bool isSemanticAbiAccepted() const noexcept { return IsValid; }
		static std::shared_ptr<MatrixTileSemanticValidationResult> Valid(std::shared_ptr<MatrixTileCanonicalDescriptorAbi> descriptor, unsigned short size) {
			return std::make_shared<MatrixTileSemanticValidationResult>(MatrixTileSemanticValidationResult{true, MatrixTileSemanticFaultKind::None, std::move(descriptor), size, true, true, false});
		}
		static std::shared_ptr<MatrixTileSemanticValidationResult> Fault(MatrixTileSemanticFaultKind fault) {
			return std::make_shared<MatrixTileSemanticValidationResult>(MatrixTileSemanticValidationResult{false, fault, nullptr, 0, false, false, false});
		}
	};

"@
$s=$s.Insert($idx,$block); [IO.File]::WriteAllText($p,$s)