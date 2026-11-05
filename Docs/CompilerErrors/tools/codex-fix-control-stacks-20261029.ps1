$ErrorActionPreference = 'Stop'
$p = 'C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Architecture\Common\YAKSys_Hybrid_CPU.Processor.cpp'
$s = [IO.File]::ReadAllText($p)
$changes = [ordered]@{
 'RedirectActiveExecutionForControlFlow(handlerAddress, !pipeCtrl->Enabled);' = 'RedirectActiveExecutionForControlFlow(handlerAddress, !pipeCtrl.Enabled);'
 'Core_FlagsRegisters_Stack->Add(CoreFlagsRegister);' = 'Core_FlagsRegisters_Stack.push_back(CoreFlagsRegister);'
 'if (Core_FlagsRegisters_Stack->Count > 0)' = 'if (!Core_FlagsRegisters_Stack.empty())'
 'CoreFlagsRegister = Core_FlagsRegisters_Stack[Core_FlagsRegisters_Stack->Count - 1];' = 'CoreFlagsRegister = Core_FlagsRegisters_Stack.back();'
 'Core_FlagsRegisters_Stack->RemoveAt(Core_FlagsRegisters_Stack->Count - 1);' = 'Core_FlagsRegisters_Stack.pop_back();'
 'Call_Callback_Addresses->Add(ReadActiveLivePc() + 32);' = 'Call_Callback_Addresses.push_back(ReadActiveLivePc() + 32);'
 'if (Call_Callback_Addresses != nullptr && Call_Callback_Addresses->Count > 0)' = 'if (!Call_Callback_Addresses.empty())'
 'Call_Callback_Addresses[Call_Callback_Addresses->Count - 1]' = 'Call_Callback_Addresses.back()'
 'Call_Callback_Addresses->RemoveAt(Call_Callback_Addresses->Count - 1);' = 'Call_Callback_Addresses.pop_back();'
 'Interrupt_Callback_Addresses->Add(ReadActiveLivePc() + 32);' = 'Interrupt_Callback_Addresses.push_back(ReadActiveLivePc() + 32);'
 'if (Interrupt_Callback_Addresses != nullptr && Interrupt_Callback_Addresses->Count > 0)' = 'if (!Interrupt_Callback_Addresses.empty())'
 'Interrupt_Callback_Addresses[Interrupt_Callback_Addresses->Count - 1]' = 'Interrupt_Callback_Addresses.back()'
 'Interrupt_Callback_Addresses->RemoveAt(Interrupt_Callback_Addresses->Count - 1);' = 'Interrupt_Callback_Addresses.pop_back();'
}
foreach ($key in $changes.Keys) { if (-not $s.Contains($key)) { throw "Expected text absent: $key" }; $s = $s.Replace($key, $changes[$key]) }
[IO.File]::WriteAllText($p, $s, [Text.UTF8Encoding]::new($false))