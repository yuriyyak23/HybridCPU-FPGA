$p='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\ISA\Instructions\NonVmx\Lanes00_03Vector\MatrixTile\MatrixTileNumericPolicyAbi.h'; $s=[IO.File]::ReadAllText($p).Replace('using namespace YAKSys_Hybrid_CPU::Arch;',''); $i=$s.IndexOf("`tclass MatrixTileNumericPolicyAbi final"); if($i -lt 0){throw 'Numeric ABI marker missing'}
$block=@"
	struct MatrixTileNumericPolicy final {
		unsigned short AbiVersion = 0;
		MatrixTileNumericProfileId ProfileId = MatrixTileNumericProfileId::Unspecified;
		DataTypeEnum ElementType{}, AccumulatorType{}, PublishFormat{};
		MatrixTileNumericSignedness Signedness{};
		MatrixTileNumericWideningRule WideningRule{};
		MatrixTileNumericMultiplyRule MultiplyRule{};
		MatrixTileNumericAddRule AddRule{};
		MatrixTileNumericRoundingMode RoundingMode{};
		MatrixTileNumericSaturationMode SaturationMode{};
		MatrixTileNumericOverflowMode OverflowMode{};
		MatrixTileNumericNaNPolicy NaNPolicy{};
		MatrixTileNumericInfinityPolicy InfinityPolicy{};
		MatrixTileNumericDenormalPolicy DenormalPolicy{};
		MatrixTileNumericReproducibilityMode ReproducibilityMode{};
		MatrixTileNumericExceptionPolicy ExceptionPolicy{};
		unsigned long long Fingerprint = 0;
	};
	struct MatrixTileNumericProfileRow final {
		MatrixTileNumericProfileId ProfileId{};
		DataTypeEnum ElementType{}, AccumulatorType{}, PublishFormat{};
		bool IsExecutionSupported = false;
		std::wstring SupportDecision;
	};
	struct MatrixTileNumericPolicyValidationResult final {
		bool IsValid = false;
		MatrixTileNumericPolicyFaultKind FaultKind = MatrixTileNumericPolicyFaultKind::None;
		std::shared_ptr<MatrixTileNumericProfileRow> Profile;
		bool isRuntimeOwnedNumericPolicyAccepted() const noexcept { return IsValid; }
		static std::shared_ptr<MatrixTileNumericPolicyValidationResult> Valid(std::shared_ptr<MatrixTileNumericProfileRow> profile) { return std::make_shared<MatrixTileNumericPolicyValidationResult>(MatrixTileNumericPolicyValidationResult{true, MatrixTileNumericPolicyFaultKind::None, std::move(profile)}); }
		static std::shared_ptr<MatrixTileNumericPolicyValidationResult> Fault(MatrixTileNumericPolicyFaultKind kind, std::shared_ptr<MatrixTileNumericProfileRow> profile = nullptr) { return std::make_shared<MatrixTileNumericPolicyValidationResult>(MatrixTileNumericPolicyValidationResult{false, kind, std::move(profile)}); }
	};

"@
$s=$s.Insert($i,$block); $s=$s.Replace('static inline const std::vector<std::shared_ptr<MatrixTileNumericProfileRow>> ProfileTable = {','static const std::vector<std::shared_ptr<MatrixTileNumericProfileRow>> ProfileTable;'+"`r`n"+'/* ORIGINAL ProfileTable = {'); $start=$s.IndexOf('/* ORIGINAL ProfileTable = {'); $end=$s.IndexOf(';',$start); if($start -lt 0 -or $end -lt 0){throw 'Table not found'}; $s=$s.Insert($end+1,' */'); [IO.File]::WriteAllText($p,$s)
$p2='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\ISA\Instructions\NonVmx\Lanes00_03Vector\MatrixTile\MatrixTileNumericPolicyAbi.cpp'; $c=[IO.File]::ReadAllText($p2); $table=$s.Substring($start+'/* ORIGINAL '.Length,$end-($start+'/* ORIGINAL '.Length)); $table=$table.Replace('ProfileTable = {','const std::vector<std::shared_ptr<MatrixTileNumericProfileRow>> MatrixTileNumericPolicyAbi::ProfileTable = {')+';'; $marker='namespace YAKSys_Hybrid_CPU::CloseToHSL::Core::ISA::Instructions::NonVmx::Lanes00_03Vector::MatrixTile'+"`r`n"+'{'; $idx=$c.IndexOf($marker); if($idx -lt 0){throw 'Namespace missing'}; $idx += $marker.Length; $c=$c.Insert($idx,"`r`n`t"+$table+"`r`n"); [IO.File]::WriteAllText($p2,$c)