$base='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Pipeline\Safety\'; $p=Join-Path $base 'ConditionalWeakTable.h'; $s=[IO.File]::ReadAllText($p)
$extra=@"

// Membership-only weak set for detecting repeated issuance for an operand.
template<class Key> class WeakIdentitySet {
    using WeakKey = std::weak_ptr<typename Key::element_type>;
    std::map<WeakKey, bool, std::owner_less<WeakKey>> entries_;
    std::mutex mutex_;
    void prune() { for (auto it = entries_.begin(); it != entries_.end();) { if (it->first.expired()) it = entries_.erase(it); else ++it; } }
public:
    void Add(const Key& key) {
        if (!key) throw std::invalid_argument("WeakIdentitySet requires a non-null key");
        std::lock_guard lock(mutex_); prune();
        if (!entries_.emplace(WeakKey(key), true).second) throw std::invalid_argument("Duplicate key");
    }
    bool Contains(const Key& key) {
        if (!key) throw std::invalid_argument("WeakIdentitySet requires a non-null key");
        std::lock_guard lock(mutex_); prune(); return entries_.find(WeakKey(key)) != entries_.end();
    }
};
"@
if($s.Contains('class WeakIdentitySet')){throw 'Already applied'}; [IO.File]::WriteAllText($p,$s+$extra)
$p=Join-Path $base 'YAKSys_Hybrid_CPU.Core.SafetyVerifier.h'; $s=[IO.File]::ReadAllText($p)
$old='std::shared_ptr<ConditionalWeakTable<std::shared_ptr<VirtualizationOperandSnapshot>, std::shared_ptr<VirtualizationOperationAdmissionCertificate>>> _issuedVirtualizationE2ByOperand = std::make_shared<ConditionalWeakTable<std::shared_ptr<VirtualizationOperandSnapshot>, std::shared_ptr<VirtualizationOperationAdmissionCertificate>>>()'
$new='std::shared_ptr<WeakIdentitySet<std::shared_ptr<VirtualizationOperandSnapshot>>> _issuedVirtualizationE2ByOperand = std::make_shared<WeakIdentitySet<std::shared_ptr<VirtualizationOperandSnapshot>>>()'
if(!$s.Contains($old)){throw 'Operand table not found'}; $s=$s.Replace($old,$new); [IO.File]::WriteAllText($p,$s)
$p=Join-Path $base 'YAKSys_Hybrid_CPU.Core.SafetyVerifier.cpp'; $s=[IO.File]::ReadAllText($p); $s=$s.Replace('_issuedVirtualizationE2ByOperand->TryGetValue(request->Operand, _)','_issuedVirtualizationE2ByOperand->Contains(request->Operand)').Replace('_issuedVirtualizationE2ByOperand->Add(request->Operand, certificate)','_issuedVirtualizationE2ByOperand->Add(request->Operand)'); [IO.File]::WriteAllText($p,$s)