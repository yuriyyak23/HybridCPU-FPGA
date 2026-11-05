$p='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\ISA\Instructions\NonVmx\Lanes00_03Vector\MatrixTile\MatrixTileMemoryShapeAndFaultAbi.h'; $s=[IO.File]::ReadAllText($p).Replace('#pragma once','#pragma once'+"`r`n"+'#include "MatrixTileArchitecturalTileStateAndDescriptorAbi.h"'); $i=$s.IndexOf("`tclass MatrixTileMemoryShapeAndFaultAbi final"); if($i -lt 0){throw 'ABI marker missing'}
$block=@"
	struct MatrixTileMemoryFaultPoint final { unsigned short Row = 0, Column = 0; unsigned int ByteOffsetInRow = 0; unsigned long long Address = 0; bool IsStore = false; };
	struct MatrixTileMemoryShapeContract final {
		MatrixTileMemoryOperationKind Operation;
		MatrixTileEffectiveAddressSourceKind EffectiveAddressSource;
		std::shared_ptr<MatrixTileCanonicalDescriptorAbi> Descriptor;
		unsigned long long BaseAddress;
		unsigned short PageSizeBytes;
	};
	struct MatrixTileMemoryShapeValidationResult final {
		bool IsValid = false;
		MatrixTileMemoryFaultKind FaultKind = MatrixTileMemoryFaultKind::None;
		MatrixTileMemoryFaultPoint FaultPoint{};
		bool HasFaultPoint = false;
		unsigned long long FirstByteAddress = 0, LastByteAddress = 0, TotalByteFootprint = 0;
		unsigned int RowByteCount = 0;
		bool CrossesPageBoundary = false;
		MatrixTileMemoryPublicationPolicyKind PublicationPolicy = MatrixTileMemoryPublicationPolicyKind::Unspecified;
		MatrixTileMemoryOrderingPolicyKind OrderingPolicy = MatrixTileMemoryOrderingPolicyKind::Unspecified;
		bool isMemoryShapeAbiAccepted() const noexcept { return IsValid; }
		static std::shared_ptr<MatrixTileMemoryShapeValidationResult> Valid(unsigned long long first, unsigned long long last, unsigned long long total, unsigned int rowBytes, bool crosses, MatrixTileMemoryPublicationPolicyKind policy) { return std::make_shared<MatrixTileMemoryShapeValidationResult>(MatrixTileMemoryShapeValidationResult{true, MatrixTileMemoryFaultKind::None, {}, false, first, last, total, rowBytes, crosses, policy, MatrixTileMemoryOrderingPolicyKind::RetireOrderedAllOrNone}); }
		static std::shared_ptr<MatrixTileMemoryShapeValidationResult> Fault(MatrixTileMemoryFaultKind kind, MatrixTileMemoryFaultPoint point = {}, bool hasPoint = false) { return std::make_shared<MatrixTileMemoryShapeValidationResult>(MatrixTileMemoryShapeValidationResult{false, kind, point, hasPoint}); }
	};

"@
$s=$s.Insert($i,$block); [IO.File]::WriteAllText($p,$s)