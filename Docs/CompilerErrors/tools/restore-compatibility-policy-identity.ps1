$p='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Runtime\Capabilities\CompatibilityControls\CompatibilityControlPolicyOwner.h'; $s=[IO.File]::ReadAllText($p); $anchor='class CompatibilityControlPolicySnapshot final'; $def=@"
struct CompatibilityControlPolicyConstructionProfile final {
    const unsigned long long DomainIdentity;
    const CompatibilityEventRoutingPolicy EventRoutingPolicy;
    const bool IsConfigured;
    CompatibilityControlPolicyConstructionProfile(unsigned long long domain, CompatibilityEventRoutingPolicy policy) : DomainIdentity(domain), EventRoutingPolicy(policy), IsConfigured(true) {
        const auto bits = static_cast<unsigned long long>(policy);
        if (!domain || bits == 0 || (bits & ~7ULL) != 0) throw std::invalid_argument("Invalid compatibility control profile");
    }
    static CompatibilityControlPolicyConstructionProfile FailClosed(unsigned long long domain) { return {domain, static_cast<CompatibilityEventRoutingPolicy>(7)}; }
};
struct CompatibilityControlPolicyIdentity final {
    unsigned long long OwnerIdentity = 0, OwnerEpoch = 0, DomainIdentity = 0, PolicyGeneration = 0;
    bool isMaterialized() const noexcept { return OwnerIdentity != 0 && OwnerEpoch != 0 && DomainIdentity != 0 && PolicyGeneration != 0; }
    bool operator==(const CompatibilityControlPolicyIdentity&) const = default;
};

"@; if(-not $s.Contains('struct CompatibilityControlPolicyIdentity final')) {$s=$s.Replace($anchor,$def+$anchor)}; $s=$s.Replace('CompatibilityEventRoutingPolicy::RuntimeTrapPolicyRequired | CompatibilityEventRoutingPolicy::NeutralTrapResultRequired | CompatibilityEventRoutingPolicy::PublicationFenceRequired','static_cast<CompatibilityEventRoutingPolicy>(7)'); [IO.File]::WriteAllText($p,$s)
$p='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Runtime\Capabilities\CompatibilityControls\CompatibilityControlPolicyOwner.cpp'; $s=[IO.File]::ReadAllText($p).Replace('getIdentity()->IsMaterialized','getIdentity()->isMaterialized()'); [IO.File]::WriteAllText($p,$s)
