$p='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\ISA\Instructions\NonVmx\Lanes00_03Vector\MatrixTile\MatrixTileAccumulatorAndTransposePolicyAbi.h'; $s=[IO.File]::ReadAllText($p).Replace('#pragma once','#pragma once'+"`r`n"+'#include "MatrixTileArchitecturalTileStateAndDescriptorAbi.h"'); $i=$s.IndexOf("`tclass MatrixTileCanonicalDescriptorAbi;"); if($i -lt 0){throw 'Record insertion point missing'}
$block=@"
	struct MatrixTileNumericPolicy;
	struct MatrixTileLayoutPolicy;
	struct MatrixTileMaccSemanticContract final {
		MatrixTileCanonicalDescriptorAbi Left, Right, Accumulator;
		MatrixTileNumericElementKind ElementKind;
		MatrixTileAccumulatorPolicyKind AccumulatorPolicy;
		std::shared_ptr<MatrixTileNumericPolicy> NumericPolicy;
		bool HasExplicitNumericPolicy = false;
		std::shared_ptr<MatrixTileLayoutPolicy> LayoutPolicy;
		bool HasExplicitLayoutPolicy = false;
		MatrixTileMaccSemanticContract(const std::shared_ptr<MatrixTileCanonicalDescriptorAbi>& left, const std::shared_ptr<MatrixTileCanonicalDescriptorAbi>& right, const std::shared_ptr<MatrixTileCanonicalDescriptorAbi>& accumulator, MatrixTileNumericElementKind elementKind, MatrixTileAccumulatorPolicyKind policy) : Left(*left), Right(*right), Accumulator(*accumulator), ElementKind(elementKind), AccumulatorPolicy(policy) {}
	};
	struct MatrixTileTransposeSemanticContract final {
		MatrixTileCanonicalDescriptorAbi Source, Destination;
		unsigned short SourceTileId, DestinationTileId;
		MatrixTileTransposeAliasPolicyKind AliasPolicy;
		std::shared_ptr<MatrixTileLayoutPolicy> LayoutPolicy;
		bool HasExplicitLayoutPolicy = false;
		MatrixTileTransposeSemanticContract(const std::shared_ptr<MatrixTileCanonicalDescriptorAbi>& source, const std::shared_ptr<MatrixTileCanonicalDescriptorAbi>& destination, unsigned short sourceTileId, unsigned short destinationTileId, MatrixTileTransposeAliasPolicyKind aliasPolicy) : Source(*source), Destination(*destination), SourceTileId(sourceTileId), DestinationTileId(destinationTileId), AliasPolicy(aliasPolicy) {}
	};

"@
$s=$s.Insert($i,$block); [IO.File]::WriteAllText($p,$s)