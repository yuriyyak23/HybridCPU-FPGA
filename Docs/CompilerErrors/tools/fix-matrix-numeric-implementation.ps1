$p = 'C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\ISA\Instructions\NonVmx\Lanes00_03Vector\MatrixTile\MatrixTileNumericPolicyAbi.cpp'
$s = [IO.File]::ReadAllText($p)
$s = $s.Replace('#include "MatrixTileNumericPolicyAbi.h"', "#include `"MatrixTileNumericPolicyAbi.h`"`r`n#include <stdexcept>")
$s = $s.Replace('static_cast<std::vector<std::shared_ptr<MatrixTileNumericProfileRow>>>(ProfileTable.Clone())', 'ProfileTable')
$s = $s.Replace('throw ArgumentOutOfRangeException(L"profileId", profileId, std::format("MatrixTile numeric profile is not executable: {0:s}.", row->SupportDecision));', 'throw std::out_of_range("MatrixTile numeric profile is not executable");')
$s = $s.Replace('//'+"`t`t"+'return policy with { Fingerprint = ComputeFingerprint(policy) };', '//'+"`t`t"+'return policy with { Fingerprint = ComputeFingerprint(policy) };'+"`r`n`t`tpolicy->Fingerprint = ComputeFingerprint(policy);`r`n`t`treturn policy;")
$marker = "`t`tif (profileId == MatrixTileNumericProfileId::Unspecified)"
$insert = @'
		MatrixTileNumericProfileId profileId = MatrixTileNumericProfileId::Unspecified;
		switch (elementType) {
		case DataTypeEnum::INT8: profileId = MatrixTileNumericProfileId::SignedInt8ToInt32; break;
		case DataTypeEnum::UINT8: profileId = MatrixTileNumericProfileId::UnsignedInt8ToUInt32; break;
		case DataTypeEnum::INT16: profileId = MatrixTileNumericProfileId::SignedInt16ToInt32; break;
		case DataTypeEnum::UINT16: profileId = MatrixTileNumericProfileId::UnsignedInt16ToUInt32; break;
		case DataTypeEnum::INT32: profileId = MatrixTileNumericProfileId::SignedInt32ToInt64; break;
		case DataTypeEnum::UINT32: profileId = MatrixTileNumericProfileId::UnsignedInt32ToUInt64; break;
		case DataTypeEnum::INT64: profileId = MatrixTileNumericProfileId::SignedInt64ToInt64; break;
		case DataTypeEnum::UINT64: profileId = MatrixTileNumericProfileId::UnsignedInt64ToUInt64; break;
		default: break;
		}
'@
$s = $s.Replace($marker, $insert + "`r`n" + $marker)
$s = $s.Replace('Fingerprint: 0)', '0)')
$s = $s.Replace('throw ArgumentOutOfRangeException(L"profileId", profileId, L"Unknown MatrixTile numeric profile.");', 'throw std::out_of_range("Unknown MatrixTile numeric profile");')
$s = $s.Replace('return new(profileId, elementType, accumulatorType, accumulatorType, IsExecutionSupported: true, SupportDecision: L"Phase15IntegerProfileSupported");', 'return std::make_shared<MatrixTileNumericProfileRow>(profileId, elementType, accumulatorType, accumulatorType, true, L"Phase15IntegerProfileSupported");')
$s = $s.Replace('return new(profileId, elementType, accumulatorType, accumulatorType, IsExecutionSupported: false, SupportDecision: reason);', 'return std::make_shared<MatrixTileNumericProfileRow>(profileId, elementType, accumulatorType, accumulatorType, false, reason);')
[IO.File]::WriteAllText($p, $s)
