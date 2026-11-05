$dir='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Runtime\Completion\Records'; $h=Join-Path $dir 'SecureComputeRegisterLocalCompletionOwner.h'; $c=Join-Path $dir 'SecureComputeRegisterLocalCompletionOwner.cpp'; $s=[IO.File]::ReadAllText($h); $cs=[IO.File]::ReadAllText($c)
$marker='using namespace HybridCPU::ExternalRuntime::Contracts;'; $forward='namespace HybridCPU::ExternalRuntime::Contracts { class ExternalSecureRegisterLocalSubmission; class ExternalSecureRegisterLocalExecutionReceipt; class ExternalSecureRegisterLocalExecutionResult; class IExternalSecureRegisterLocalExecutionProvider; class ExternalSecureDomainLease; class ExternalSecureExecutionAttempt; class ExternalSecureExecutionConsumptionReceipt; }'; if(-not $s.Contains($marker)){throw 'Namespace marker missing'}; $s=$s.Replace($marker,$forward+"`r`n"+$marker)
$old='class Pending(SecureComputePreparedMachineAdmission prepared, ExternalSecureRegisterLocalSubmission submission, ulong domainScope, IExternalSecureRegisterLocalExecutionProvider provider, PostStageBIssuedAttempt issuedAttempt) final'; if(-not $s.Contains($old)){throw 'Pending missing'}; $s=$s.Replace($old,'class Pending final')
foreach($x in @('Prepared = prepared','Submission = submission','DomainScope = domainScope','Provider = provider','IssuedAttempt = issuedAttempt')){$s=$s.Replace($x,$x.Split('=')[0].TrimEnd())}
$start=$s.IndexOf("`t`t`tconst std::vector<std::shared_ptr<ArchContextState>> Contexts ="); $end=$s.IndexOf('})->ToArray();',$start); if($start -lt 0 -or $end -lt 0){throw 'Contexts initializer missing'}; $s=$s.Remove($start,$end+14-$start).Insert($start,"`t`t`tconst std::vector<std::shared_ptr<ArchContextState>> Contexts;")
$s=$s.Replace('std::shared_ptr<RetireCoordinator> retire;','mutable std::shared_ptr<RetireCoordinator> retire;')
$pos=$s.IndexOf('class Pending final'); $public=$s.IndexOf("`t`tpublic:",$pos); $s=$s.Insert($public+"`t`tpublic:".Length,"`r`n`t`t`tPending(std::shared_ptr<SecureComputePreparedMachineAdmission> prepared, std::shared_ptr<ExternalSecureRegisterLocalSubmission> submission, unsigned long long domainScope, std::shared_ptr<IExternalSecureRegisterLocalExecutionProvider> provider, std::shared_ptr<PostStageBIssuedAttempt> issuedAttempt);")
$old='class Acceptance(Pending pending, ExternalSecureRegisterLocalExecutionReceipt receipt, ulong sequence) final'; if(-not $s.Contains($old)){throw 'Acceptance missing'}; $s=$s.Replace($old,'class Acceptance final').Replace('Pending = pending;','Pending;').Replace('Receipt = receipt;','Receipt;').Replace('Sequence = sequence;','Sequence;')
$pos=$s.IndexOf('class Acceptance final'); $public=$s.IndexOf("`t`tpublic:",$pos); $s=$s.Insert($public+"`t`tpublic:".Length,"`r`n`t`t`tAcceptance(std::shared_ptr<SecureComputeRegisterLocalCompletionOwner::Pending> pending, std::shared_ptr<ExternalSecureRegisterLocalExecutionReceipt> receipt, unsigned long long sequence) : Pending(std::move(pending)), Receipt(std::move(receipt)), Sequence(sequence) {}")
$pos=$s.IndexOf('class Pending final'); $insert=$s.LastIndexOf("`tpublic:",$pos); $s=$s.Insert($insert+"`tpublic:".Length,"`r`n`t`tclass Acceptance;")
$s=$s.Replace(' = [];',' = {};').Replace('#include <memory>',"#include <memory>`r`n#include <utility>")
$marker="`tstd::shared_ptr<SecureComputePreparedMachineAdmission> SecureComputeRegisterLocalCompletionOwner::Pending::getPrepared() const"
if(-not $cs.Contains($marker)){throw 'Implementation marker missing'}
$constructor=@"
    SecureComputeRegisterLocalCompletionOwner::Pending::Pending(
        std::shared_ptr<SecureComputePreparedMachineAdmission> prepared,
        std::shared_ptr<ExternalSecureRegisterLocalSubmission> submission, unsigned long long domainScope,
        std::shared_ptr<IExternalSecureRegisterLocalExecutionProvider> provider,
        std::shared_ptr<PostStageBIssuedAttempt> issuedAttempt)
        : Prepared(std::move(prepared)), Submission(std::move(submission)), DomainScope(domainScope),
          Provider(std::move(provider)), IssuedAttempt(std::move(issuedAttempt)), Contexts([this] {
              auto target = Prepared->getCertificate()->getLiveCoreTarget();
              if (target) return target->getContexts();
              std::vector<std::shared_ptr<ArchContextState>> contexts;
              for (int vt = 0; vt < 4; ++vt) contexts.push_back(std::make_shared<ArchContextState>(32, vt));
              return contexts;
          }()) {}

"@
$cs=$cs.Replace($marker,$constructor+$marker)
$start=$cs.IndexOf("`tstd::shared_ptr<RetireCoordinator> SecureComputeRegisterLocalCompletionOwner::Pending::getRetire() const"); $end=$cs.IndexOf("`tstd::shared_ptr<YAKSys_Hybrid_CPU::Core::SecureComputeRegisterLocalCompletionOwner::Pending>",$start); if($start -lt 0 -or $end -lt 0){throw 'Retire getter boundaries missing'}
$getter=@"
    std::shared_ptr<RetireCoordinator> SecureComputeRegisterLocalCompletionOwner::Pending::getRetire() const {
        auto target = Prepared->getCertificate()->getLiveCoreTarget();
        if (target) return target->getRetire();
        if (!retire) retire = std::make_shared<RetireCoordinator>(std::make_shared<PhysicalRegisterFile>(),
            std::make_shared<RenameMap>(4), std::make_shared<CommitMap>(4), std::make_shared<ArchContextCollection>(Contexts));
        return retire;
    }

"@
$cs=$cs.Remove($start,$end-$start).Insert($start,$getter)
[IO.File]::WriteAllText($h,$s,[Text.UTF8Encoding]::new($false));[IO.File]::WriteAllText($c,$cs,[Text.UTF8Encoding]::new($false))
