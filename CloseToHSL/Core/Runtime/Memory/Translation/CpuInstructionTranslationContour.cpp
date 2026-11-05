#include "CpuInstructionTranslationContour.h"
#include <algorithm>
#include <stdexcept>
#include <utility>
#include <sstream>
#include <iomanip>
#include "CpuSecondStageTranslationMechanism.h"

namespace YAKSys_Hybrid_CPU::Core::Memory
{
	namespace
	{
		const char *TranslationAccessName(CpuInstructionTranslationAccessKind accessKind)
		{
		switch (accessKind)
		{
		case CpuInstructionTranslationAccessKind::InstructionFetch: return "InstructionFetch";
		case CpuInstructionTranslationAccessKind::ScalarLoad: return "ScalarLoad";
		case CpuInstructionTranslationAccessKind::ScalarStore: return "ScalarStore";
		default: return "None";
		}
		}

		const char *TranslationFaultName(CpuInstructionTranslationFaultReason reason)
		{
		switch (reason)
		{
		case CpuInstructionTranslationFaultReason::UnmappedAddress: return "UnmappedAddress";
		case CpuInstructionTranslationFaultReason::AccessDenied: return "AccessDenied";
		case CpuInstructionTranslationFaultReason::AddressOverflow: return "AddressOverflow";
		case CpuInstructionTranslationFaultReason::OwnerScopeMismatch: return "OwnerScopeMismatch";
		case CpuInstructionTranslationFaultReason::SecondStageViolation: return "SecondStageViolation";
		case CpuInstructionTranslationFaultReason::SecondStageMisconfiguration: return "SecondStageMisconfiguration";
		case CpuInstructionTranslationFaultReason::SecondStageSourceStale: return "SecondStageSourceStale";
		default: return "None";
		}
		}

		std::string DescribeTranslationFault(const std::shared_ptr<CpuInstructionTranslationFault> &fault)
		{
			if (!fault)
			{
				return "CPU instruction translation fault.";
			}
			std::ostringstream message;
			message << "CPU " << TranslationAccessName(fault->Request.AccessKind)
				<< " translation fault (" << TranslationFaultName(fault->Reason)
				<< ") at virtual address 0x" << std::uppercase << std::hex << fault->Request.VirtualAddress << '.';
			return message.str();
		}
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private CpuInstructionTranslationPolicy(CpuInstructionTranslationMode mode, CpuInstructionTranslationScope scope, ImmutableArray<CpuInstructionTranslationRegion> regions, ImmutableArray<CpuInstructionStageOneRegion> stageOneRegions = default, Core.MemoryDomainRuntime.CpuSecondStageTranslationBinding? secondStageBinding = null)
    template<class T> static std::vector<std::shared_ptr<T>> CopyRegions(const std::vector<std::shared_ptr<T>>& regions) {
        std::vector<std::shared_ptr<T>> result; result.reserve(regions.size());
        for (const auto& region : regions) { if (!region) throw std::invalid_argument("null region"); result.push_back(std::make_shared<T>(*region)); }
        return result;
    }
    template<class T> static std::vector<std::shared_ptr<T>> ValidatedRegions(const std::vector<std::shared_ptr<T>>& input) {
        auto ordered=CopyRegions(input);
        if (ordered.empty()) throw std::invalid_argument("mapped translation requires at least one region");
        std::sort(ordered.begin(),ordered.end(),[](const auto& a,const auto& b){ return a->VirtualBase < b->VirtualBase; });
        unsigned long long previousEnd=0; bool first=true;
        for (const auto& r : ordered) {
            if (!r->isValid()) throw std::invalid_argument("invalid translation region");
            if (!first && r->VirtualBase <= previousEnd) throw std::invalid_argument("overlapping translation regions");
            previousEnd=r->VirtualBase + r->Length - 1; first=false;
        }
        return ordered;
    }
    CpuInstructionTranslationPolicy::CpuInstructionTranslationPolicy(CpuInstructionTranslationMode mode, std::shared_ptr<CpuInstructionTranslationScope> scope, std::vector<std::shared_ptr<CpuInstructionTranslationRegion>> regions, std::vector<std::shared_ptr<CpuInstructionStageOneRegion>> stageOneRegions, std::shared_ptr<Core::MemoryDomainRuntime::CpuSecondStageTranslationBinding> binding)
        : Mode(mode), Scope(scope ? std::make_shared<CpuInstructionTranslationScope>(*scope) : std::make_shared<CpuInstructionTranslationScope>()), Regions(CopyRegions(regions)), StageOneRegions(CopyRegions(stageOneRegions)), SecondStageBinding(std::move(binding)) {}
    CpuInstructionTranslationMode CpuInstructionTranslationPolicy::getMode() const { return Mode; }
    std::shared_ptr<CpuInstructionTranslationScope> CpuInstructionTranslationPolicy::getScope() const { return std::make_shared<CpuInstructionTranslationScope>(*Scope); }
    std::vector<std::shared_ptr<CpuInstructionTranslationRegion>> CpuInstructionTranslationPolicy::getRegions() const { return CopyRegions(Regions); }
    std::vector<std::shared_ptr<CpuInstructionStageOneRegion>> CpuInstructionTranslationPolicy::getStageOneRegions() const { return CopyRegions(StageOneRegions); }
    std::shared_ptr<Core::MemoryDomainRuntime::CpuSecondStageTranslationBinding> CpuInstructionTranslationPolicy::getSecondStageBinding() const { return SecondStageBinding; }
    std::shared_ptr<CpuInstructionTranslationPolicy> CpuInstructionTranslationPolicy::getIdentity() {
        static auto identity=std::shared_ptr<CpuInstructionTranslationPolicy>(new CpuInstructionTranslationPolicy(CpuInstructionTranslationMode::Identity, nullptr, {})); return identity;
    }
    std::shared_ptr<CpuInstructionTranslationPolicy> CpuInstructionTranslationPolicy::CreateBoundedRegions(std::shared_ptr<CpuInstructionTranslationScope> scope, std::vector<std::shared_ptr<CpuInstructionTranslationRegion>>& regions) {
        if (!scope || !scope->isValid()) throw std::out_of_range("scope");
        auto ordered=ValidatedRegions(regions);
        return std::shared_ptr<CpuInstructionTranslationPolicy>(new CpuInstructionTranslationPolicy(CpuInstructionTranslationMode::BoundedRegions,scope,std::move(ordered)));
    }
    std::shared_ptr<CpuInstructionTranslationPolicy> CpuInstructionTranslationPolicy::CreateTwoStageBoundedRegions(std::shared_ptr<CpuInstructionTranslationScope> scope, std::vector<std::shared_ptr<CpuInstructionStageOneRegion>>& regions, std::shared_ptr<Core::MemoryDomainRuntime> runtime, std::shared_ptr<Core::MemoryDomainDescriptor> descriptor, unsigned long long identity) {
        if (!scope || !scope->isValid()) throw std::out_of_range("scope");
        if (!runtime || !descriptor) throw std::invalid_argument("runtime or descriptor");
        auto ordered=ValidatedRegions(regions);
        auto bound=runtime->BindCanonicalCpuSecondStageTranslation(descriptor, identity, scope->DomainId);
        if (!bound || !bound->isBound()) throw std::logic_error("second-stage source binding failed");
        return std::shared_ptr<CpuInstructionTranslationPolicy>(new CpuInstructionTranslationPolicy(CpuInstructionTranslationMode::TwoStageBoundedRegions,scope,{},std::move(ordered),bound->Binding));
    }
	CpuInstructionTranslationFaultException::CpuInstructionTranslationFaultException(std::shared_ptr<CpuInstructionTranslationFault> fault)
		: std::runtime_error(DescribeTranslationFault(fault)), Fault(std::move(fault))
	{
		if (!Fault || !Fault->isValid())
		{
			throw std::invalid_argument("fault");
		}
	}

	std::shared_ptr<CpuInstructionTranslationFault> CpuInstructionTranslationFaultException::getFault() const
	{
		return Fault;
	}

	CpuInstructionTranslationSourceStaleException::CpuInstructionTranslationSourceStaleException(std::shared_ptr<CpuInstructionTranslationFault> fault)
		: std::runtime_error("CPU second-stage translation source changed; the in-flight operation must be replayed."), Fault(std::move(fault))
	{}

	std::shared_ptr<CpuInstructionTranslationFault> CpuInstructionTranslationSourceStaleException::getFault() const
	{
		return Fault;
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal PhysicalReadBinding(CpuInstructionTranslationOwner owner, Object? controller = null)
	CpuInstructionTranslationOwner::PhysicalReadBinding::PhysicalReadBinding(std::shared_ptr<CpuInstructionTranslationOwner> owner, std::shared_ptr<HybridCPU_ISE::CloseToHSL::Memory::Timing::MemoryCycleController> controller) : owner(owner)
	{
		this->controller = controller;
	}

	bool CpuInstructionTranslationOwner::PhysicalReadBinding::TryClaim(std::shared_ptr<HybridCPU_ISE::CloseToHSL::Memory::Timing::MemoryCycleController> target, unsigned long long id, unsigned long long address, int size)
	{
		{
		std::scoped_lock<std::mutex> lock(gate);
// C# TO C++ CONVERTER TASK: Cannot test for object identity between 2 non-pointer objects in C++:
			if (phase != 0 || id == 0 || controller != target || !owner->HasPhysicalRead(shared_from_this(), address, size))
			{
				return false;
			}
			controller = target;
			requestId = id;
			phase = 1;
			return true;
		}
	}

	void CpuInstructionTranslationOwner::PhysicalReadBinding::Close(std::shared_ptr<HybridCPU_ISE::CloseToHSL::Memory::Timing::MemoryCycleController> target, unsigned long long id)
	{
		{
		std::scoped_lock<std::mutex> lock(gate);
// C# TO C++ CONVERTER TASK: Cannot test for object identity between 2 non-pointer objects in C++:
// C# TO C++ CONVERTER TASK: The following 'is' operator pattern is not converted by C# to C++ Converter:
			if (controller != target || requestId != id || (phase != 1 && phase != 2))
			{
				throw InvalidOperationException(L"CPU physical read terminal owner mismatch.");
			}
			phase = 3;
		}
	}

	void CpuInstructionTranslationOwner::PhysicalReadBinding::RevokeUnclaimed()
	{
		// Admission and cancellation serialize on this gate. A claimed
		// request remains under the original controller's terminal owner.
		{
		std::scoped_lock<std::mutex> lock(gate);
			if (phase == 0)
			{
				phase = 3;
			}
		}
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal bool Read(Processor.MainMemoryArea? backing, ulong address, byte[] destination)
	bool CpuInstructionTranslationOwner::PhysicalReadBinding::Read(std::shared_ptr<Processor::MainMemoryArea> backing, unsigned long long address, std::vector<unsigned char> &destination)
	{
		{
		std::scoped_lock<std::mutex> lock(gate);
			if (phase != 1)
			{
				throw InvalidOperationException(L"CPU physical read is unclaimed, duplicate or terminal.");
			}
			phase = 2;
			return owner->ReadPhysical(shared_from_this(), backing, address, destination);
		}
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal PhysicalReadBinding? BindPhysicalRead(in CpuInstructionTranslationRequest request, in CpuInstructionTranslationResult result, System.Nullable<Object> controller)
	std::shared_ptr<PhysicalReadBinding> CpuInstructionTranslationOwner::BindPhysicalRead(std::shared_ptr<CpuInstructionTranslationRequest> request, std::shared_ptr<CpuInstructionTranslationResult> result, std::shared_ptr<HybridCPU_ISE::CloseToHSL::Memory::Timing::MemoryCycleController> controller)
	{
		// Identity retains the existing IOVA compatibility route. Mutable
		// two-stage adoption requires its own request lifetime protocol.
		if (_policy->getMode() != CpuInstructionTranslationMode::BoundedRegions || request->AccessKind != CpuInstructionTranslationAccessKind::ScalarLoad || !result->Succeeded)
		{
			return nullptr;
		}
		if (*Translate(request) != *result)
		{
			throw InvalidOperationException(L"CPU physical binding result was not produced by its original translation owner.");
		}
		auto binding = std::make_shared<PhysicalReadBinding>(shared_from_this(), controller);
		        {
            std::lock_guard lock(physicalReadsGate);
            for (auto it = physicalReads.begin(); it != physicalReads.end();) { if (it->first.expired()) it = physicalReads.erase(it); else ++it; }
            physicalReads.emplace(std::weak_ptr<PhysicalReadBinding>(binding), PhysicalReadRequest{*request, result->PhysicalAddress});
        }
		return binding;
	}

    bool CpuInstructionTranslationOwner::HasPhysicalRead(std::shared_ptr<PhysicalReadBinding> binding, unsigned long long address, int size) {
        std::lock_guard lock(physicalReadsGate);
        auto it = physicalReads.find(std::weak_ptr<PhysicalReadBinding>(binding));
        return it != physicalReads.end() && it->second.Address == address && it->second.Request.AccessSize == size;
    }
    bool CpuInstructionTranslationOwner::ReadPhysical(std::shared_ptr<PhysicalReadBinding> binding, std::shared_ptr<Processor::MainMemoryArea> backing, unsigned long long address, std::vector<unsigned char>& destination) {
        if (backing != _physicalMemory || !HasPhysicalRead(binding, address, static_cast<int>(destination.size()))) throw std::logic_error("CPU physical read lost its original translation/backing/range binding.");
        return _physicalMemory->TryReadPhysicalRange(address, destination);
    }
// C# TO C++ CONVERTER TASK: Throw expressions are not converted by C# to C++ Converter:
// ORIGINAL LINE: internal CpuInstructionTranslationOwner(CpuInstructionTranslationPolicy policy, Processor.MainMemoryArea physicalMemory)
	CpuInstructionTranslationOwner::CpuInstructionTranslationOwner(std::shared_ptr<CpuInstructionTranslationPolicy> policy, std::shared_ptr<Processor::MainMemoryArea> physicalMemory)
		: OwnerEpoch(1), _policy((policy != nullptr) ? policy : throw std::invalid_argument("policy")),
		  _physicalMemory((physicalMemory != nullptr) ? physicalMemory : throw std::invalid_argument("physicalMemory")) {}

	unsigned long long CpuInstructionTranslationOwner::getOwnerEpoch() const
	{
		return OwnerEpoch;
	}

	bool CpuInstructionTranslationOwner::isIdentity() const
	{
		return _policy->getMode() == CpuInstructionTranslationMode::Identity;
	}

	bool CpuInstructionTranslationOwner::getHasImmutableInstructionMapping() const
	{
		return ([&] { const auto codexPatternValue = _policy->getMode(); return codexPatternValue == CpuInstructionTranslationMode::Identity || codexPatternValue == CpuInstructionTranslationMode::BoundedRegions; }());
	}

	std::shared_ptr<CpuInstructionTranslationScope> CpuInstructionTranslationOwner::getConfiguredScope() const
	{
		return _policy->getScope();
	}

	void CpuInstructionTranslationOwner::BindCompletionLifecycle(std::shared_ptr<Core::ArchitecturalCompletionCommitOwner> completionOwner)
	{
		ArgumentNullException::ThrowIfNull(completionOwner);
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: Core.MemoryDomainRuntime.CpuSecondStageTranslationBinding? binding = _policy.SecondStageBinding;
		std::shared_ptr<Core::MemoryDomainRuntime::CpuSecondStageTranslationBinding> binding = _policy->getSecondStageBinding();
		if (binding == nullptr)
		{
			return;
		}

		std::shared_ptr<CompletionObservationScope> scope = std::make_shared<Core::CompletionObservationScope>(_policy->getScope()->DomainId, _policy->getScope()->ContextId, _policy->getScope()->VirtualThreadId);
		binding->getIssuer()->RegisterCpuSecondStageCompletionInvalidation(binding, [&] ()
		{
			completionOwner->InvalidateAfterMemoryDomainRebind(scope);
		});
	}

	std::shared_ptr<CpuInstructionTranslationOperationIdentity> CpuInstructionTranslationOwner::IssueFetchIdentity(unsigned long long virtualAddress)
	{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'checked' in this context:
// ORIGINAL LINE: ulong sequence = checked(++_nextFetchIdentity);
		unsigned long long sequence = ++_nextFetchIdentity;
		return std::make_shared<CpuInstructionTranslationOperationIdentity>(CpuInstructionTranslationOperationIdentity{sequence, MixNonZero(sequence, virtualAddress ^ 0x464554434845564EULL), MixNonZero(sequence, virtualAddress ^ 0x46455443484F5049ULL)});
	}

	std::shared_ptr<CpuInstructionTranslationOperationIdentity> CpuInstructionTranslationOwner::IssueFallbackMemoryIdentity(CpuInstructionTranslationAccessKind accessKind, unsigned long long virtualAddress)
	{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'checked' in this context:
// ORIGINAL LINE: ulong sequence = checked(++_nextFallbackMemoryIdentity);
		unsigned long long sequence = ++_nextFallbackMemoryIdentity;
		return std::make_shared<CpuInstructionTranslationOperationIdentity>(CpuInstructionTranslationOperationIdentity{sequence, MixNonZero(sequence, virtualAddress ^ (static_cast<unsigned long long>(static_cast<unsigned char>(accessKind)) << 56)), MixNonZero(sequence, virtualAddress ^ 0x4D454D4F5046414CULL)});
	}

	std::shared_ptr<CpuInstructionTranslationResult> CpuInstructionTranslationOwner::Translate(std::shared_ptr<CpuInstructionTranslationRequest> request)
	{
		if (!request || !request->hasOperationIdentity())
		{
			throw ArgumentOutOfRangeException(L"request");
		}

		if (_policy->getMode() == CpuInstructionTranslationMode::Identity)
		{
			return std::make_shared<CpuInstructionTranslationResult>(CpuInstructionTranslationResult::Success(request->VirtualAddress, getOwnerEpoch()));
		}

		if (!request->Scope.isValid() || request->Scope != *_policy->getScope())
		{
			return Fault(CpuInstructionTranslationFaultReason::OwnerScopeMismatch, request);
		}

		unsigned long long accessSize = request->AccessSize;
		if (request->VirtualAddress > std::numeric_limits<unsigned long long>::max() - (accessSize - 1))
		{
			return Fault(CpuInstructionTranslationFaultReason::AddressOverflow, request);
		}

		unsigned long long requestEnd = request->VirtualAddress + accessSize - 1;
		if (_policy->getMode() == CpuInstructionTranslationMode::TwoStageBoundedRegions)
		{
			return TranslateTwoStage(request, requestEnd);
		}

		for (auto region : _policy->getRegions())
		{
			unsigned long long regionEnd = region->VirtualBase + region->Length - 1;
			if (request->VirtualAddress < region->VirtualBase || requestEnd > regionEnd)
			{
				continue;
			}

			bool temp_var166;
			switch (request->AccessKind)
			{
				case CpuInstructionTranslationAccessKind::InstructionFetch:
					temp_var166 = region->Executable;
					break;
				case CpuInstructionTranslationAccessKind::ScalarLoad:
					temp_var166 = region->Readable;
					break;
				case CpuInstructionTranslationAccessKind::ScalarStore:
					temp_var166 = region->Writable;
					break;
				default:
					temp_var166 = false;
					break;
			}
			bool allowed = temp_var166;
			if (!allowed)
			{
				return Fault(CpuInstructionTranslationFaultReason::AccessDenied, request);
			}

			unsigned long long offset = request->VirtualAddress - region->VirtualBase;
			return std::make_shared<CpuInstructionTranslationResult>(CpuInstructionTranslationResult::Success(region->PhysicalBase + offset, getOwnerEpoch()));
		}

		return Fault(CpuInstructionTranslationFaultReason::UnmappedAddress, request);
	}

	std::shared_ptr<CpuInstructionTranslationResult> CpuInstructionTranslationOwner::TranslateTwoStage(std::shared_ptr<CpuInstructionTranslationRequest> request, unsigned long long requestEnd)
	{
		for (auto region : _policy->getStageOneRegions())
		{
			unsigned long long regionEnd = region->VirtualBase + region->Length - 1;
			if (request->VirtualAddress < region->VirtualBase || requestEnd > regionEnd)
			{
				continue;
			}

			if (!Allows(region, request->AccessKind))
			{
				return Fault(CpuInstructionTranslationFaultReason::AccessDenied, request);
			}

			unsigned long long guestPhysicalAddress = region->GuestPhysicalBase + (request->VirtualAddress - region->VirtualBase);
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: Core.MemoryDomainRuntime.CpuSecondStageTranslationBinding? binding = _policy.SecondStageBinding;
			std::shared_ptr<Core::MemoryDomainRuntime::CpuSecondStageTranslationBinding> binding = _policy->getSecondStageBinding();
			std::shared_ptr<Core::CpuSecondStageTranslationSnapshot> snapshot;
			if (binding == nullptr || !binding->getIssuer()->TryCaptureCanonicalCpuSecondStageTranslation(binding, request->Scope.DomainId, snapshot))
			{
				if (binding == nullptr)
				{
					throw InvalidOperationException(L"Two-stage policy lacks its MemoryDomainRuntime-issued binding.");
				}
				std::shared_ptr<Core::MemoryDomainTranslationControl> staleControl = binding->getSourceOwner()->getTranslationControl();
	// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
	// ORIGINAL LINE: snapshot = new Core.CpuSecondStageTranslationSnapshot( binding.SourceOwner, staleControl.SecondStageRoot, staleControl.DomainTag, binding.RuntimeAddressSpaceIdentity, binding.BindingGeneration, binding.AddressSpaceGeneration, binding.BindingGeneration, binding.SourceOwner.AddressSpace!);
				snapshot = std::make_shared<Core::CpuSecondStageTranslationSnapshot>(binding->getSourceOwner(), staleControl->SecondStageRoot, staleControl->DomainTag, binding->getRuntimeAddressSpaceIdentity(), binding->getBindingGeneration(), binding->getAddressSpaceGeneration(), binding->getBindingGeneration(), binding->getSourceOwner()->getAddressSpace());
				return SecondStageFault(CpuInstructionTranslationFaultReason::SecondStageSourceStale, request, guestPhysicalAddress, snapshot, CpuSecondStageNeutralAuxiliary::Encode(request->AccessKind, request->AccessSize, CpuSecondStageFaultKind::SourceStale, 1));
			}

			std::shared_ptr<CpuSecondStageTranslationResult> translated = CpuSecondStageTranslationMechanism::Translate(snapshot, guestPhysicalAddress, request->AccessKind, request->AccessSize, _physicalMemory);

			if (!binding->getIssuer()->IsCurrentCanonicalCpuSecondStageTranslation(binding, snapshot))
			{
				return SecondStageFault(CpuInstructionTranslationFaultReason::SecondStageSourceStale, request, guestPhysicalAddress, snapshot, CpuSecondStageNeutralAuxiliary::Encode(request->AccessKind, request->AccessSize, CpuSecondStageFaultKind::SourceStale, 0));
			}

			if (!translated->Succeeded)
			{
	// C# TO C++ CONVERTER TASK: The following 'switch expression' was not converted by C# to C++ Converter:
	//					CpuInstructionTranslationFaultReason reason = translated.FaultKind switch
	//					{
	//						CpuSecondStageFaultKind.Misconfiguration => CpuInstructionTranslationFaultReason.SecondStageMisconfiguration,
	//						_ => CpuInstructionTranslationFaultReason.SecondStageViolation,
	//					};
				const CpuInstructionTranslationFaultReason reason = translated->FaultKind == CpuSecondStageFaultKind::Misconfiguration
					? CpuInstructionTranslationFaultReason::SecondStageMisconfiguration
					: CpuInstructionTranslationFaultReason::SecondStageViolation;
				return SecondStageFault(reason, request, guestPhysicalAddress, snapshot, translated->NeutralAuxiliary);
			}

			return std::make_shared<CpuInstructionTranslationResult>(CpuInstructionTranslationResult{true, translated->CpuPhysicalAddress, getOwnerEpoch(), std::nullopt, true, guestPhysicalAddress});
		}

		return Fault(CpuInstructionTranslationFaultReason::UnmappedAddress, request);
	}

	std::shared_ptr<CpuInstructionTranslationResult> CpuInstructionTranslationOwner::SecondStageFault(CpuInstructionTranslationFaultReason reason, std::shared_ptr<CpuInstructionTranslationRequest> request, unsigned long long guestPhysicalAddress, std::shared_ptr<Core::CpuSecondStageTranslationSnapshot> snapshot, unsigned long long auxiliary)
	{
		unsigned long long qualification = (static_cast<unsigned long long>(static_cast<unsigned char>(reason)) << 56) | (static_cast<unsigned long long>(static_cast<unsigned char>(request->AccessKind)) << 48) | (static_cast<unsigned long long>(request->AccessSize) << 32);
		std::shared_ptr<CpuInstructionTranslationFault> fault = std::make_shared<CpuInstructionTranslationFault>(CpuInstructionTranslationFault{reason, *request, getOwnerEpoch(), qualification, auxiliary, CpuInstructionTranslationFaultStage::SecondStage, true, guestPhysicalAddress, snapshot->MemoryDomainOwnerEpoch, snapshot->AddressSpaceGeneration, snapshot->AddressSpaceIdentity});
		return std::make_shared<CpuInstructionTranslationResult>(CpuInstructionTranslationResult::Failed(*fault));
	}

	bool CpuInstructionTranslationOwner::Allows(std::shared_ptr<CpuInstructionStageOneRegion> region, CpuInstructionTranslationAccessKind accessKind)
	{
		bool temp_var168;
		switch (accessKind)
		{
			case CpuInstructionTranslationAccessKind::InstructionFetch:
				temp_var168 = region->Executable;
				break;
			case CpuInstructionTranslationAccessKind::ScalarLoad:
				temp_var168 = region->Readable;
				break;
			case CpuInstructionTranslationAccessKind::ScalarStore:
				temp_var168 = region->Writable;
				break;
			default:
				temp_var168 = false;
				break;
		}
		return temp_var168;
	}

	std::shared_ptr<CpuInstructionTranslationResult> CpuInstructionTranslationOwner::Fault(CpuInstructionTranslationFaultReason reason, std::shared_ptr<CpuInstructionTranslationRequest> request)
	{
		unsigned long long qualification = (static_cast<unsigned long long>(static_cast<unsigned char>(reason)) << 56) | (static_cast<unsigned long long>(static_cast<unsigned char>(request->AccessKind)) << 48) | (static_cast<unsigned long long>(request->AccessSize) << 32);
		std::shared_ptr<CpuInstructionTranslationFault> fault = std::make_shared<CpuInstructionTranslationFault>(CpuInstructionTranslationFault{reason, *request, getOwnerEpoch(), qualification, getOwnerEpoch()});
		return std::make_shared<CpuInstructionTranslationResult>(CpuInstructionTranslationResult::Failed(*fault));
	}

	unsigned long long CpuInstructionTranslationOwner::CreateMemoryOperationIdentity(unsigned long long attemptId, unsigned long long eventId, CpuInstructionTranslationAccessKind accessKind, unsigned long long virtualAddress)
	{
		unsigned long long value = attemptId ^ RotateLeft(eventId, 17) ^ (static_cast<unsigned long long>(static_cast<unsigned char>(accessKind)) << 56) ^ virtualAddress;
		return MixNonZero(value, 0x4350554D454D4F50ULL);
	}

	unsigned long long CpuInstructionTranslationOwner::MixNonZero(unsigned long long left, unsigned long long right)
	{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: ulong value = unchecked((left * 0x9E3779B185EBCA87UL) ^ right);
		unsigned long long value = (left * 0x9E3779B185EBCA87ULL) ^ right;
		value ^= value >> 29;
		value *= 0xD6E8FEB86659FD93ULL;
		return value == 0 ? 1ULL : value;
	}

	unsigned long long CpuInstructionTranslationOwner::RotateLeft(unsigned long long value, int count)
	{
		return (value << count) | (value >> (64 - count));
	}
}
