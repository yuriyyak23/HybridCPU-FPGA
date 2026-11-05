$p='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Virtualization\Compatibility\Frontend\Projection\VmcsRead\VmcsReadOnlyValueProjectionService.cpp'; $s=[IO.File]::ReadAllText($p)
$s=$s.Replace('request->RuntimeAdmission.IsAllowed','request->RuntimeAdmission->IsAllowed').Replace('request->Memory.TryCreateReadOnlyTranslationView','request->Memory->TryCreateReadOnlyTranslationView').Replace('request->Execution.TryCreateReadOnlyStateView','request->Execution->TryCreateReadOnlyStateView').Replace('AllowWrite: false','false')
$s=$s.Replace('entry->Owner is not (VmcsFieldProjectionOwner::CompletionRecord or VmcsFieldProjectionOwner::ExecutionDomainDescriptor or VmcsFieldProjectionOwner::MemoryDomainDescriptor)','entry->Owner != VmcsFieldProjectionOwner::CompletionRecord && entry->Owner != VmcsFieldProjectionOwner::ExecutionDomainDescriptor && entry->Owner != VmcsFieldProjectionOwner::MemoryDomainDescriptor')
$s=$s.Replace('entry->Field is not (VmcsField::ExitReason or VmcsField::ExitQualification or VmcsField::GuestPhysicalAddress or VmcsField::EptViolationQualification)','entry->Field != VmcsField::ExitReason && entry->Field != VmcsField::ExitQualification && entry->Field != VmcsField::GuestPhysicalAddress && entry->Field != VmcsField::EptViolationQualification')
$s=$s.Replace('entry->Field == VmcsField::HostPc or VmcsField::HostSp or VmcsField::HostFlags or VmcsField::HostCr0','entry->Field == VmcsField::HostPc || entry->Field == VmcsField::HostSp || entry->Field == VmcsField::HostFlags || entry->Field == VmcsField::HostCr0').Replace('entry->Field == VmcsField::GuestCr0 or VmcsField::GuestCr4','entry->Field == VmcsField::GuestCr0 || entry->Field == VmcsField::GuestCr4')
$s=$s.Replace('SecureVisibilityAllowed: alias->IsAllowed, MigrationClassified:(request->PrivilegedExecutionState == nullptr ? nullptr :  request->PrivilegedExecutionState.MigrationClass) == PrivilegedExecutionStateMigrationClass::RevalidatedAfterRestore','alias->IsAllowed, request->PrivilegedExecutionState && request->PrivilegedExecutionState->MigrationClass == PrivilegedExecutionStateMigrationClass::RevalidatedAfterRestore')
$s=$s.Replace('return new(decision, field, entry, alias, VmcsV2ValidationResult::Fail(VmcsV2ValidationCode::AccessDenied, fieldId, reason), Value: 0, reason);','return std::make_shared<VmcsReadOnlyValueProjectionResult>(decision, field, entry, alias, VmcsV2ValidationResult::Fail(VmcsV2ValidationCode::AccessDenied, fieldId, reason), 0, reason);')
$s=[regex]::Replace($s,'std::format\("Generated VMREAD field .*?entry->Field, entry->Owner\)','L"Generated VMREAD field owner has no admitted neutral read-only value source in this slice."')
$s=[regex]::Replace($s,'std::format\("CompletionRecord does not define a VMREAD value projection for field .*?entry->Field\)','L"CompletionRecord does not define a VMREAD value projection for this field."')
$s=[regex]::Replace($s,'std::format\("MemoryDomainDescriptor does not define a VMREAD value projection for field .*?entry->Field\)','L"MemoryDomainDescriptor does not define a VMREAD value projection for this field."')
$i=$s.IndexOf("// C# TO C++ CONVERTER TASK: The following 'switch expression'"); $end=$s.IndexOf("`n`t}",$i); if($i -lt 0 -or $end -lt 0){throw 'switch missing'}
$dispatch=@'
switch (entry->Owner) {
        case VmcsFieldProjectionOwner::CompletionRecord: return ProjectCompletionOwnedValue(request, field, entry, alias);
        case VmcsFieldProjectionOwner::ExecutionDomainDescriptor: return ProjectExecutionOwnedValue(request, field, entry, alias);
        case VmcsFieldProjectionOwner::MemoryDomainDescriptor: return ProjectMemoryOwnedValue(request, field, entry, alias);
        default: return Denied(VmcsReadOnlyValueProjectionDecision::NeutralOwnerValueSourceMissing, field, entry, alias, request->FieldId, L"Generated VMREAD field owner has no admitted neutral read-only value source in this slice.");
        }
'@
$s=$s.Substring(0,$i)+$dispatch+$s.Substring($end)
$pat='(?s)if \(!privilegedProjection->IsAllowed\)\s*\{.*?return temp_var;'
$replacement=@'
if (!privilegedProjection->IsAllowed) {
                auto denied = Denied(VmcsReadOnlyValueProjectionDecision::PrivilegedExecutionStateProjectionDenied, field, entry, alias, request->FieldId, privilegedProjection->Reason);
                denied->PrivilegedProjection = privilegedProjection;
                return denied;
            }
            auto projected = std::make_shared<VmcsReadOnlyValueProjectionResult>(VmcsReadOnlyValueProjectionDecision::ReadOnlyValueProjected, field, entry, alias, VmcsV2ValidationResult::Success(request->FieldId), static_cast<long long>(privilegedProjection->Value), L"Guest control-register VMREAD value came from the neutral privileged execution-state owner.");
            projected->PrivilegedProjection = privilegedProjection;
            return projected;
'@
if(-not [regex]::IsMatch($s,$pat)){throw 'with anchor missing'}; $s=[regex]::Replace($s,$pat,$replacement)
$i=$s.IndexOf('bool VmcsReadOnlyValueProjectionService::TryProjectExecutionField'); $end=$s.IndexOf('std::shared_ptr<VmcsReadOnlyValueProjectionResult> VmcsReadOnlyValueProjectionService::Denied',$i); if($i -lt 0 -or $end -lt 0){throw 'methods anchor missing'}
$methods=@'
bool VmcsReadOnlyValueProjectionService::TryProjectExecutionField(VmcsField field, std::shared_ptr<ExecutionDomainReadOnlyStateView> view, long long &value, std::wstring &missingReason) {
        switch (field) {
        case VmcsField::GuestPc: if (view->HasMaterializedGuestPc) { value=static_cast<long long>(view->GuestPc); missingReason=L""; return true; } break;
        case VmcsField::GuestSp: if (view->HasMaterializedGuestSp) { value=static_cast<long long>(view->GuestSp); missingReason=L""; return true; } break;
        case VmcsField::GuestFlags: if (view->HasMaterializedGuestFlags) { value=static_cast<long long>(view->GuestFlags); missingReason=L""; return true; } break;
        default: value=0; missingReason=L"ExecutionDomainDescriptor does not define a VMREAD value projection for this field."; return false;
        }
        value=0; missingReason=L"ExecutionDomainDescriptor read-only state view does not materialize this field."; return false;
    }
    bool VmcsReadOnlyValueProjectionService::TryProjectMemoryField(VmcsField field, std::shared_ptr<MemoryDomainReadOnlyTranslationView> view, long long &value) {
        switch (field) {
        case VmcsField::GuestCr3: value=static_cast<long long>(view->AddressSpaceRoot); return true;
        case VmcsField::EptPointer: if (view->OwnsSecondStageTranslation) { value=static_cast<long long>(view->SecondStageRoot); return true; } break;
        case VmcsField::Vpid: value=view->AddressSpaceTag; return true;
        case VmcsField::Cr3TargetCount: value=view->AddressSpaceTargetCount; return true;
        default: break;
        }
        value=0; return false;
    }
'@
$s=$s.Substring(0,$i)+$methods+"`r`n`t"+$s.Substring($end); [IO.File]::WriteAllText($p,$s)