$dir = 'C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Execution\ExternalAccelerators\Queues'
foreach ($name in @('AcceleratorCommandQueue.h', 'AcceleratorCommandQueue.cpp')) {
    $p = Join-Path $dir $name
    $s = [IO.File]::ReadAllText($p)
    $s = [regex]::Replace($s, '(Reject\(AcceleratorTokenFaultCode faultCode, )std::optional<std::shared_ptr<AcceleratorGuardDecision>> &guardDecision', '$1std::optional<std::shared_ptr<AcceleratorGuardDecision>> guardDecision')
    if ($name -eq 'AcceleratorCommandQueue.cpp') {
        $s = $s.Replace('return reject;', 'return reject.value();')
        $s = $s.Replace('token->MarkValidated(currentGuardEvidence)', 'token->MarkValidated(currentGuardEvidence.value_or(nullptr))')
        $s = $s.Replace('token->MarkQueued(currentGuardEvidence)', 'token->MarkQueued(currentGuardEvidence.value_or(nullptr))')
        $s = $s.Replace('next->Token.SubmitGuardDecision', 'next->Token->getSubmitGuardDecision()')
        $s = $s.Replace('command = _commands.pop_front();', "command = next;`r`n`t`t_commands.pop_front();")
        $s = $s.Replace('AcceleratorQueueAdmissionResult::Accepted(command, guardDecision', 'AcceleratorQueueAdmissionResult::Accepted(command.value(), guardDecision')
        $s = $s.Replace('AcceleratorQueueAdmissionResult::Reject(faultCode, guardDecision,', 'AcceleratorQueueAdmissionResult::Reject(faultCode, guardDecision.value_or(nullptr),')
        $s = $s.Replace('std::format("L7-SDC queue admission rejected because device ''{0:s}'' is busy after guard-backed admission checks.", getDevice()->getDeviceId())', 'std::format(L"L7-SDC queue admission rejected because device ''{}'' is busy after guard-backed admission checks.", getDevice()->getDeviceId())')
        $s = $s.Replace('std::format("L7-SDC queue admission requires token Created or Validated, but token is {0:s}.", token->getState())', 'std::format(L"L7-SDC queue admission requires token Created or Validated, but token is {}.", static_cast<int>(token->getState()))')
    }
    [IO.File]::WriteAllText($p, $s)
}
