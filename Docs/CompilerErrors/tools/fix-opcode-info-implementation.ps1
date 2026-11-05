$p='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\NonRTL\Arch\OpcodeInfo.Types.cpp'; $s=[IO.File]::ReadAllText($p)
$s=[regex]::Replace($s,'\(Flags & InstructionFlags::([A-Za-z]+)\) != 0','(static_cast<int>(Flags) & static_cast<int>(InstructionFlags::$1)) != 0')
$pattern='(?s)\tInstructionClass OpcodeInfo::ResolveInstructionClass\(OpcodeCategory category\)\s*\{.*?\r?\n\t\}'
$new=@"
	InstructionClass OpcodeInfo::ResolveInstructionClass(OpcodeCategory category)
	{
		switch (category) {
		case OpcodeCategory::Memory: return InstructionClass::Memory;
		case OpcodeCategory::ControlFlow: return InstructionClass::ControlFlow;
		case OpcodeCategory::System: case OpcodeCategory::Privileged: return InstructionClass::System;
		case OpcodeCategory::Atomic: return InstructionClass::Atomic;
		default: return InstructionClass::ScalarAlu;
		}
	}
"@
if(([regex]::Matches($s,$pattern)).Count -ne 1){throw 'Instruction class method not found'}; $s=[regex]::Replace($s,$pattern,$new)
$pattern='(?s)\tSerializationClass OpcodeInfo::ResolveSerializationClass\(OpcodeCategory category, InstructionFlags flags\)\s*\{.*?\r?\n\t\}'
$new=@"
	SerializationClass OpcodeInfo::ResolveSerializationClass(OpcodeCategory category, InstructionFlags flags)
	{
		if (category == OpcodeCategory::Atomic) return SerializationClass::AtomicSerial;
		if (category == OpcodeCategory::Memory && (static_cast<int>(flags) & static_cast<int>(InstructionFlags::MemoryWrite)) != 0) return SerializationClass::MemoryOrdered;
		if (category == OpcodeCategory::Privileged) return SerializationClass::FullSerial;
		return SerializationClass::Free;
	}
"@
if(([regex]::Matches($s,$pattern)).Count -ne 1){throw 'Serialization method not found'}; $s=[regex]::Replace($s,$pattern,$new); [IO.File]::WriteAllText($p,$s)