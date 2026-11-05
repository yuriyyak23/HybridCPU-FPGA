$p='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\NonRTL\Arch\Generated\GeneratedIsaShadowCatalog.g.h'; $s=[IO.File]::ReadAllText($p)
$record=@"
	struct GeneratedIsaDescriptor {
		unsigned int Opcode;
		std::wstring Mnemonic;
		std::vector<std::wstring> Aliases;
		YAKSys_Hybrid_CPU::Arch::OpcodeCategory OpcodeCategory;
		YAKSys_Hybrid_CPU::Arch::InstructionFlags InstructionFlags;
		unsigned char ExecutionLatency;
		unsigned char MemoryBandwidth;
		std::wstring EncodingForm, OperandSchema;
		InstructionClass StaticClass;
		std::wstring SlotConstraints;
		SerializationClass Serialization;
		std::wstring Privilege, Extension, ProviderId, MaterializerId, StaticEffectContract, LatencyModelId;
	};

"@
$idx=$s.IndexOf("`tclass GeneratedIsaCatalog final"); if($idx -lt 0){throw 'Catalog class not found'}; $s=$s.Insert($idx,$record)
$start=$s.IndexOf('static inline const std::vector<std::shared_ptr<GeneratedIsaDescriptor>> Descriptors = {'); $end=$s.IndexOf(';',$start); if($start -lt 0 -or $end -lt 0){throw 'Descriptor array not found'}
$line=$s.Substring($start,$end-$start); $count=[regex]::Matches($line,'\bnew\(').Count; if($count -lt 100){throw "Unexpected descriptor count: $count"}
$line=[regex]::Replace($line,'\bnew\((.*?)\)(?=, new\(|,?\}$)', 'std::make_shared<GeneratedIsaDescriptor>(GeneratedIsaDescriptor{$1})'); if($line.Contains('new(')){throw 'Some descriptors were not converted'}
$s=$s.Remove($start,$end-$start).Insert($start,$line); [IO.File]::WriteAllText($p,$s); Write-Output "Converted $count descriptors"