#include "ArchitecturalCompletionCommitOwner.h"
#include "NeutralArchitecturalCompletionFacts.h"
#include "../../../Architecture/Common/YAKSys_Hybrid_CPU.Processor.h"
#include "../../../Decoder/Sha256Digest.h"
#include <bcrypt.h>
#include <windows.h>
#include <algorithm>
#include <array>
#include <cwctype>
#include <iomanip>
#include <sstream>
#include <stdexcept>

#pragma comment(lib, "bcrypt.lib")

namespace {
	std::string ToUtf8(const std::wstring &value)
	{
		if (value.empty()) return {};
		const int size = WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, value.data(), static_cast<int>(value.size()), nullptr, 0, nullptr, nullptr);
		if (size <= 0) throw std::runtime_error("Could not encode completion seal input as UTF-8.");
		std::string result(static_cast<std::size_t>(size), '\0');
		if (WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, value.data(), static_cast<int>(value.size()), result.data(), size, nullptr, nullptr) != size)
			throw std::runtime_error("Could not encode completion seal input as UTF-8.");
		return result;
	}

	std::wstring HmacSha256Hex(const std::vector<unsigned char> &key, const std::wstring &value)
	{
		BCRYPT_ALG_HANDLE algorithm = nullptr;
		BCRYPT_HASH_HANDLE hash = nullptr;
		std::vector<unsigned char> object;
		NTSTATUS status = BCryptOpenAlgorithmProvider(&algorithm, BCRYPT_SHA256_ALGORITHM, nullptr, BCRYPT_ALG_HANDLE_HMAC_FLAG);
		if (status < 0) throw std::runtime_error("Could not initialize completion receipt HMAC.");
		try
		{
			DWORD objectSize = 0, returned = 0;
			status = BCryptGetProperty(algorithm, BCRYPT_OBJECT_LENGTH, reinterpret_cast<PUCHAR>(&objectSize), sizeof(objectSize), &returned, 0);
			if (status < 0) throw std::runtime_error("Could not query completion receipt HMAC.");
			object.resize(objectSize);
			status = BCryptCreateHash(algorithm, &hash, object.data(), objectSize,
				const_cast<PUCHAR>(key.data()), static_cast<ULONG>(key.size()), 0);
			if (status < 0) throw std::runtime_error("Could not create completion receipt HMAC.");
			const std::string bytes = ToUtf8(value);
			if (!bytes.empty())
			{
				status = BCryptHashData(hash, reinterpret_cast<PUCHAR>(const_cast<char *>(bytes.data())), static_cast<ULONG>(bytes.size()), 0);
				if (status < 0) throw std::runtime_error("Could not hash completion receipt input.");
			}
			std::array<unsigned char, 32> digest{};
			status = BCryptFinishHash(hash, digest.data(), static_cast<ULONG>(digest.size()), 0);
			if (status < 0) throw std::runtime_error("Could not finalize completion receipt HMAC.");
			constexpr wchar_t digits[] = L"0123456789abcdef";
			std::wstring result;
			result.reserve(digest.size() * 2);
			for (unsigned char byte : digest) { result.push_back(digits[byte >> 4]); result.push_back(digits[byte & 0x0f]); }
			BCryptDestroyHash(hash);
			BCryptCloseAlgorithmProvider(algorithm, 0);
			return result;
		}
		catch (...)
		{
			if (hash) BCryptDestroyHash(hash);
			BCryptCloseAlgorithmProvider(algorithm, 0);
			throw;
		}
	}
}

namespace YAKSys_Hybrid_CPU::Core
{

	ArchitecturalCompletionCommitOwner::CanonicalRetireWindowCommitLease::CanonicalRetireWindowCommitLease(std::shared_ptr<ArchitecturalCompletionCommitOwner> issuer, std::shared_ptr<ArchitecturalCompletionCommitOwner::ProducerRegistration> producer, std::vector<std::shared_ptr<ArchitecturalCompletionCandidate>> &candidates)
		: Producer(std::move(producer)), Candidates(candidates), _issuer(std::move(issuer)), _owningThread(std::this_thread::get_id()) {}

	std::shared_ptr<ArchitecturalCompletionCommitOwner::ProducerRegistration> ArchitecturalCompletionCommitOwner::CanonicalRetireWindowCommitLease::getProducer() const
	{
		return Producer;
	}

	std::vector<std::shared_ptr<ArchitecturalCompletionCandidate>> ArchitecturalCompletionCommitOwner::CanonicalRetireWindowCommitLease::getCandidates() const
	{
		return Candidates;
	}

	bool ArchitecturalCompletionCommitOwner::CanonicalRetireWindowCommitLease::IsOwnedByCurrentThread() const noexcept
	{
		return _owningThread == std::this_thread::get_id();
	}

	std::vector<std::shared_ptr<ArchitecturalCompletionCommitResult>> ArchitecturalCompletionCommitOwner::CanonicalRetireWindowCommitLease::Complete()
	{
		if (_released)
		{
			throw InvalidOperationException(L"Canonical completion commit lease was already released.");
		}
		try
		{
			auto results = _issuer->CompletePreparedRetireWindow(shared_from_this());
			_released = true;
			return results;
		}
		catch (...)
		{
			_released = true;
			throw;
		}
	}

	void ArchitecturalCompletionCommitOwner::CanonicalRetireWindowCommitLease::dispose()
	{
		if (_released)
		{
			return;
		}
		_issuer->AbortPreparedRetireWindow(shared_from_this());
		_released = true;
	}

	ArchitecturalCompletionCommitOwner::ProducerRegistration::ProducerRegistration(std::shared_ptr<ArchitecturalCompletionCommitOwner> issuer, unsigned long long ownerIdentity, unsigned long long ownerEpoch, std::shared_ptr<ArchitecturalCompletionProducerPolicy> policy)
		: OwnerIdentity(ownerIdentity), OwnerEpoch(ownerEpoch), Policy(std::move(policy)), _issuer(std::move(issuer)) {}

	unsigned long long ArchitecturalCompletionCommitOwner::ProducerRegistration::getOwnerIdentity() const
	{
		return OwnerIdentity;
	}

	unsigned long long ArchitecturalCompletionCommitOwner::ProducerRegistration::getOwnerEpoch() const
	{
		return OwnerEpoch;
	}

	std::shared_ptr<ArchitecturalCompletionProducerPolicy> ArchitecturalCompletionCommitOwner::ProducerRegistration::getPolicy() const
	{
		return Policy;
	}

	bool ArchitecturalCompletionCommitOwner::ProducerRegistration::IsIssuedBy(std::shared_ptr<ArchitecturalCompletionCommitOwner> owner)
	{
		return _issuer == owner;
	}

	ArchitecturalCompletionCommitOwner::ArchitecturalCompletionCommitReceipt::ArchitecturalCompletionCommitReceipt(std::shared_ptr<ArchitecturalCompletionCommitOwner> issuer, std::shared_ptr<ArchitecturalCompletionReceiptBinding> binding, const std::wstring &issuerSeal)
		: Issuer(std::move(issuer)), IssuerSeal(issuerSeal), Binding(std::move(binding)) {}

	std::shared_ptr<ArchitecturalCompletionCommitOwner> ArchitecturalCompletionCommitOwner::ArchitecturalCompletionCommitReceipt::getIssuer() const
	{
		return Issuer;
	}

	std::wstring ArchitecturalCompletionCommitOwner::ArchitecturalCompletionCommitReceipt::getIssuerSeal() const
	{
		return IssuerSeal;
	}

	std::shared_ptr<ArchitecturalCompletionReceiptBinding> ArchitecturalCompletionCommitOwner::ArchitecturalCompletionCommitReceipt::getBinding() const
	{
		return Binding;
	}

	bool ArchitecturalCompletionCommitOwner::ArchitecturalCompletionCommitReceipt::IsIssuedBy(std::shared_ptr<ArchitecturalCompletionCommitOwner> owner)
	{
		return getIssuer() == owner;
	}

	bool ArchitecturalCompletionCommitOwner::ArchitecturalCompletionCommitReceipt::SealEquals(const std::wstring &seal)
	{
		const std::wstring current = getIssuerSeal();
		if (!IsWellFormedSeal(current) || !IsWellFormedSeal(seal)) return false;
		unsigned int difference = 0;
		for (std::size_t i = 0; i < current.size(); ++i) difference |= static_cast<unsigned int>(current[i] ^ seal[i]);
		return difference == 0;
	}

	bool ArchitecturalCompletionCommitOwner::ArchitecturalCompletionCommitReceipt::IsWellFormedSeal(const std::wstring &seal)
	{
		if (seal.length() != 64)
		{
			return false;
		}
		for (wchar_t character : seal)
		{
			if (!((character >= L'0' && character <= L'9') || (character >= L'a' && character <= L'f') || (character >= L'A' && character <= L'F')))
			{
				return false;
			}
		}
		return true;
	}

// C# TO C++ CONVERTER TASK: Throw expressions are not converted by C# to C++ Converter:
// ORIGINAL LINE: internal ArchitecturalCompletionCommitOwner(VirtualizationRestoreGenerationOwner restoreOwner, DomainCompletionObservationOwner observationOwner)
	ArchitecturalCompletionCommitOwner::ArchitecturalCompletionCommitOwner(std::shared_ptr<VirtualizationRestoreGenerationOwner> restoreOwner, std::shared_ptr<DomainCompletionObservationOwner> observationOwner) : _restoreOwner((restoreOwner != nullptr) ? restoreOwner : throw std::invalid_argument("restoreOwner")), _observationOwner((observationOwner != nullptr) ? observationOwner : throw std::invalid_argument("observationOwner"))
	{
	}

	std::shared_ptr<DomainCompletionObservationOwner> ArchitecturalCompletionCommitOwner::getObservationOwner() const
	{
		{
		std::scoped_lock<std::recursive_mutex> lock(_gate);
		const_cast<ArchitecturalCompletionCommitOwner *>(this)->EnsureObservationIssuerUnderLock();
			return _observationOwner;
		}
	}

	unsigned long long ArchitecturalCompletionCommitOwner::getCurrentRestoreGeneration() const
	{
		return _restoreOwner->getCurrentGeneration();
	}

	int ArchitecturalCompletionCommitOwner::getLiveReceiptCount() const
	{
		{
		std::scoped_lock<std::recursive_mutex> lock(_gate);
			return _liveReceipts.size();
		}
	}

	std::shared_ptr<ArchitecturalCompletionCommitOwner::ProducerRegistration> ArchitecturalCompletionCommitOwner::RegisterProducer(std::shared_ptr<ArchitecturalCompletionProducerPolicy> policy)
	{
		if (!policy) throw std::invalid_argument("policy");
		if (std::all_of(policy->OwnerName.begin(), policy->OwnerName.end(), [](wchar_t character) { return std::iswspace(character) != 0; }))
			throw std::invalid_argument("policy.OwnerName");
		if (policy->CompletionClass == NeutralArchitecturalCompletionClass::None)
		{
			throw ArgumentOutOfRangeException(L"policy");
		}

		{
			std::scoped_lock<std::recursive_mutex> lock(_gate);
			unsigned long long identity = AllocateNonZero(_nextProducerIdentity);
			std::shared_ptr<ProducerRegistration> registration = std::make_shared<ProducerRegistration>(shared_from_this(), identity, 1, policy);
			_registrations.emplace(identity, registration);
			return registration;
		}
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal ArchitecturalCompletionCommitResult CommitAtCanonicalRetireBoundary(ProducerRegistration? producer, in ArchitecturalCompletionCandidate candidate)
	std::shared_ptr<ArchitecturalCompletionCommitResult> ArchitecturalCompletionCommitOwner::CommitAtCanonicalRetireBoundary(std::shared_ptr<ArchitecturalCompletionCommitOwner::ProducerRegistration> producer, std::shared_ptr<ArchitecturalCompletionCandidate> candidate)
	{
		{
			std::scoped_lock<std::recursive_mutex> lock(_gate);
			std::optional<std::unordered_set<std::shared_ptr<CompletionIdentityKey>>> pendingKeys;
			std::shared_ptr<ArchitecturalCompletionCommitResult> validation = ValidateCandidateUnderLock(producer, candidate, pendingKeys);
	// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
	// ORIGINAL LINE: return validation.Decision == ArchitecturalCompletionCommitDecision.Committed ? CommitValidatedCandidateUnderLock(producer!, candidate) : validation;
			return validation->Decision == ArchitecturalCompletionCommitDecision::Committed ? CommitValidatedCandidateUnderLock(producer, candidate) : validation;
		}
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal ArchitecturalCompletionCommitResult CommitAtCanonicalPreciseFaultBoundary(ProducerRegistration? producer, in ArchitecturalCompletionCandidate candidate)
	std::shared_ptr<ArchitecturalCompletionCommitResult> ArchitecturalCompletionCommitOwner::CommitAtCanonicalPreciseFaultBoundary(std::shared_ptr<ArchitecturalCompletionCommitOwner::ProducerRegistration> producer, std::shared_ptr<ArchitecturalCompletionCandidate> candidate)
	{
		{
			std::scoped_lock<std::recursive_mutex> lock(_gate);
			std::optional<std::unordered_set<std::shared_ptr<CompletionIdentityKey>>> pendingKeys;
			std::shared_ptr<ArchitecturalCompletionCommitResult> validation = ValidateCandidateUnderLock(producer, candidate, pendingKeys);
	// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
	// ORIGINAL LINE: return validation.Decision == ArchitecturalCompletionCommitDecision.Committed ? CommitValidatedCandidateUnderLock(producer!, candidate) : validation;
			return validation->Decision == ArchitecturalCompletionCommitDecision::Committed ? CommitValidatedCandidateUnderLock(producer, candidate) : validation;
		}
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal ArchitecturalCompletionBatchPreparationResult PrepareCanonicalRetireWindow(ProducerRegistration? producer, ReadOnlySpan<ArchitecturalCompletionCandidate> candidates)
	std::shared_ptr<ArchitecturalCompletionBatchPreparationResult> ArchitecturalCompletionCommitOwner::PrepareCanonicalRetireWindow(std::shared_ptr<ArchitecturalCompletionCommitOwner::ProducerRegistration> producer, std::span<const std::shared_ptr<ArchitecturalCompletionCandidate>> candidates)
	{
		if (candidates.empty())
		{
			return std::make_shared<ArchitecturalCompletionBatchPreparationResult>(ArchitecturalCompletionBatchPreparationResult{ArchitecturalCompletionCommitDecision::DeniedMissingIdentity, nullptr, L"A canonical completion commit lease requires at least one mandatory candidate."});
		}

		_gate.lock();
		try
		{
			if (_activeRetireWindowLease != nullptr)
			{
				throw InvalidOperationException(L"A canonical completion retire window is already prepared.");
			}

			std::optional<std::unordered_set<std::shared_ptr<CompletionIdentityKey>>> pendingKeys(std::in_place);
			std::vector<std::shared_ptr<ArchitecturalCompletionCandidate>> copy(candidates.begin(), candidates.end());
			for (int index = 0; index < copy.size(); index++)
			{
				std::shared_ptr<ArchitecturalCompletionCommitResult> validation = ValidateCandidateUnderLock(producer, copy[index], pendingKeys);
				if (validation->Decision != ArchitecturalCompletionCommitDecision::Committed)
				{
					auto rejected = std::make_shared<ArchitecturalCompletionBatchPreparationResult>(ArchitecturalCompletionBatchPreparationResult{validation->Decision, nullptr, validation->Reason});
					_gate.unlock();
					return rejected;
				}
			}

	// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
	// ORIGINAL LINE: CanonicalRetireWindowCommitLease lease = new CanonicalRetireWindowCommitLease(this, producer!, copy);
			std::shared_ptr<CanonicalRetireWindowCommitLease> lease = std::make_shared<CanonicalRetireWindowCommitLease>(shared_from_this(), producer, copy);
			auto prepared = std::make_shared<ArchitecturalCompletionBatchPreparationResult>(ArchitecturalCompletionBatchPreparationResult{ArchitecturalCompletionCommitDecision::Committed, lease, L"All mandatory completion candidates were prevalidated before retire-visible late effects."});
			_activeRetireWindowLease = lease;
			return prepared;
		}
		catch (...)
		{
			_gate.unlock();
			throw;
		}
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal bool ValidateLiveReceipt(ArchitecturalCompletionCommitReceipt? receipt, in ArchitecturalCompletionReceiptBinding expected)
	bool ArchitecturalCompletionCommitOwner::ValidateLiveReceipt(std::shared_ptr<ArchitecturalCompletionCommitReceipt> receipt, std::shared_ptr<ArchitecturalCompletionReceiptBinding> expected)
	{
		{
			std::scoped_lock<std::recursive_mutex> lock(_gate);
			return ValidateLiveReceiptUnderLock(receipt, expected);
		}
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal bool TryConsumeReceipt(ArchitecturalCompletionCommitReceipt? receipt, in ArchitecturalCompletionReceiptBinding expected)
	bool ArchitecturalCompletionCommitOwner::TryConsumeReceipt(std::shared_ptr<ArchitecturalCompletionCommitReceipt> receipt, std::shared_ptr<ArchitecturalCompletionReceiptBinding> expected)
	{
		{
			std::scoped_lock<std::recursive_mutex> lock(_gate);
			if (!ValidateLiveReceiptUnderLock(receipt, expected))
			{
				return false;
			}
			return _liveReceipts.erase(expected->CommitSequence);
		}
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal bool TryGetLatestLiveReceipt(out ArchitecturalCompletionCommitReceipt? receipt)
	bool ArchitecturalCompletionCommitOwner::TryGetLatestLiveReceipt(std::shared_ptr<ArchitecturalCompletionCommitReceipt> &receipt)
	{
		{
			std::scoped_lock<std::recursive_mutex> lock(_gate);
			receipt = _latestReceipt;
			return receipt != nullptr && _liveReceipts.contains(receipt->getBinding()->CommitSequence);
		}
	}

	void ArchitecturalCompletionCommitOwner::InvalidateAfterRestore()
	{
		{
			std::scoped_lock<std::recursive_mutex> lock(_gate);
			EnsureNoPreparedRetireWindow();
			_restoreOwner->AdvanceAfterRestore();
			_observationOwner->ClearAfterRestore();
			_liveReceipts.clear();
			_committedCompletionKeys.clear();
			_latestReceipt = nullptr;
		}
	}

	void ArchitecturalCompletionCommitOwner::ClearObservation(std::shared_ptr<CompletionObservationScope> scope)
	{
		{
			std::scoped_lock<std::recursive_mutex> lock(_gate);
			EnsureNoPreparedRetireWindow();
			InvalidateLiveScopeUnderLock(scope);
			_observationOwner->Clear(scope);
		}
	}

	void ArchitecturalCompletionCommitOwner::RebindObservation(std::shared_ptr<CompletionObservationScope> previousScope)
	{
		{
			std::scoped_lock<std::recursive_mutex> lock(_gate);
			EnsureNoPreparedRetireWindow();
			InvalidateLiveScopeUnderLock(previousScope);
			_observationOwner->Rebind(previousScope);
		}
	}

	void ArchitecturalCompletionCommitOwner::InvalidateAfterMemoryDomainRebind(std::shared_ptr<CompletionObservationScope> previousScope)
	{
		RebindObservation(previousScope);
	}

	std::shared_ptr<DomainCompletionObservationOwner> ArchitecturalCompletionCommitOwner::ReplaceObservationOwner()
	{
		{
			std::scoped_lock<std::recursive_mutex> lock(_gate);
			EnsureNoPreparedRetireWindow();
			EnsureObservationIssuerUnderLock();
			_liveReceipts.clear();
			_committedCompletionKeys.clear();
			_latestReceipt = nullptr;
			_observationOwner = _observationOwner->ReplaceOwner(shared_from_this());
			_observationInstaller = _observationOwner->RegisterCommitIssuer(shared_from_this());
			return _observationOwner;
		}
	}

	std::shared_ptr<DomainCompletionObservationOwner> ArchitecturalCompletionCommitOwner::ReplaceObservationOwnerAfterArchitecturalStateReplacement()
	{
		{
			std::scoped_lock<std::recursive_mutex> lock(_gate);
			EnsureNoPreparedRetireWindow();
			EnsureObservationIssuerUnderLock();
			_restoreOwner->AdvanceAfterRestore();
			_liveReceipts.clear();
			_committedCompletionKeys.clear();
			_latestReceipt = nullptr;
			_observationOwner = _observationOwner->ReplaceOwner(shared_from_this());
			_observationInstaller = _observationOwner->RegisterCommitIssuer(shared_from_this());
			return _observationOwner;
		}
	}

	std::vector<std::shared_ptr<ArchitecturalCompletionCommitResult>> ArchitecturalCompletionCommitOwner::CompletePreparedRetireWindow(std::shared_ptr<ArchitecturalCompletionCommitOwner::CanonicalRetireWindowCommitLease> lease)
	{
		EnsureActiveLeaseOnOwningThread(lease);
		try
		{
			const auto candidates = lease->getCandidates();
			std::vector<std::shared_ptr<ArchitecturalCompletionCommitResult>> results(candidates.size());
			for (std::size_t index = 0; index < candidates.size(); index++)
			{
				results[index] = CommitValidatedCandidateUnderLock(lease->getProducer(), candidates[index]);
			}
			_activeRetireWindowLease = nullptr;
			_gate.unlock();
			return results;
		}
		catch (...)
		{
			_activeRetireWindowLease = nullptr;
			_gate.unlock();
			throw;
		}
	}

	void ArchitecturalCompletionCommitOwner::AbortPreparedRetireWindow(std::shared_ptr<ArchitecturalCompletionCommitOwner::CanonicalRetireWindowCommitLease> lease)
	{
		EnsureActiveLeaseOnOwningThread(lease);
		_activeRetireWindowLease = nullptr;
		_gate.unlock();
	}

	void ArchitecturalCompletionCommitOwner::EnsureActiveLeaseOnOwningThread(std::shared_ptr<ArchitecturalCompletionCommitOwner::CanonicalRetireWindowCommitLease> lease)
	{
		if (!lease || !lease->IsOwnedByCurrentThread())
		{
			throw InvalidOperationException(L"Canonical completion retire-window lease must complete or abort on its preparing thread.");
		}
		std::unique_lock<std::recursive_mutex> lock(_gate, std::try_to_lock);
		if (!lock.owns_lock() || _activeRetireWindowLease != lease)
			throw InvalidOperationException(L"Canonical completion retire-window lease must complete or abort on its preparing thread.");
	}

	void ArchitecturalCompletionCommitOwner::EnsureNoPreparedRetireWindow()
	{
		if (_activeRetireWindowLease != nullptr)
		{
			throw InvalidOperationException(L"Completion lifecycle mutation cannot interleave with a prepared canonical retire window.");
		}
	}

	void ArchitecturalCompletionCommitOwner::EnsureObservationIssuerUnderLock()
	{
		if (!_observationInstaller)
			_observationInstaller = _observationOwner->RegisterCommitIssuer(shared_from_this());
	}

	void ArchitecturalCompletionCommitOwner::InvalidateLiveScopeUnderLock(std::shared_ptr<CompletionObservationScope> scope)
	{
		if (!scope || !scope->IsValid)
		{
			throw ArgumentOutOfRangeException(L"scope");
		}

		for (auto iterator = _liveReceipts.begin(); iterator != _liveReceipts.end();)
		{
			if (iterator->second && iterator->second->Receipt && BindingMatchesScope(iterator->second->Receipt->getBinding(), scope))
				iterator = _liveReceipts.erase(iterator);
			else
				++iterator;
		}
		for (auto iterator = _committedCompletionKeys.begin(); iterator != _committedCompletionKeys.end();)
		{
			if (*iterator && KeyMatchesScope(*iterator, scope)) iterator = _committedCompletionKeys.erase(iterator);
			else ++iterator;
		}
		if (_latestReceipt != nullptr && BindingMatchesScope(_latestReceipt->getBinding(), scope))
		{
			_latestReceipt = nullptr;
		}
	}

	bool ArchitecturalCompletionCommitOwner::BindingMatchesScope(std::shared_ptr<ArchitecturalCompletionReceiptBinding> binding, std::shared_ptr<CompletionObservationScope> scope)
	{
		return binding && scope && binding->DomainId == scope->DomainId && binding->ContextId == scope->ContextId && binding->VirtualThreadId == scope->VirtualThreadId;
	}

	bool ArchitecturalCompletionCommitOwner::KeyMatchesScope(std::shared_ptr<ArchitecturalCompletionCommitOwner::CompletionIdentityKey> key, std::shared_ptr<CompletionObservationScope> scope)
	{
		return key && scope && key->DomainId == scope->DomainId && key->ContextId == scope->ContextId && key->VirtualThreadId == scope->VirtualThreadId;
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private bool ValidateLiveReceiptUnderLock(ArchitecturalCompletionCommitReceipt? receipt, in ArchitecturalCompletionReceiptBinding expected)
	bool ArchitecturalCompletionCommitOwner::ValidateLiveReceiptUnderLock(std::shared_ptr<ArchitecturalCompletionCommitReceipt> receipt, std::shared_ptr<ArchitecturalCompletionReceiptBinding> expected)
	{
		if (!receipt || !expected || !receipt->IsIssuedBy(shared_from_this()) || receipt->getBinding() != expected || expected->RestoreGeneration != _restoreOwner->getCurrentGeneration()) return false;
		auto iterator = _liveReceipts.find(expected->CommitSequence);
		if (iterator == _liveReceipts.end() || !iterator->second || iterator->second->Receipt != receipt) return false;

		std::wstring expectedSeal = ComputeSeal(expected);
		const std::wstring &liveSeal = iterator->second->Seal;
		if (liveSeal.size() != expectedSeal.size()) return false;
		unsigned int difference = 0;
		for (std::size_t index = 0; index < liveSeal.size(); ++index) difference |= static_cast<unsigned int>(liveSeal[index] ^ expectedSeal[index]);
		return receipt->SealEquals(expectedSeal) && difference == 0;
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private ArchitecturalCompletionCommitResult ValidateCandidateUnderLock(ProducerRegistration? producer, in ArchitecturalCompletionCandidate candidate, System.Nullable<HashSet<CompletionIdentityKey>> pendingKeys)
	std::shared_ptr<ArchitecturalCompletionCommitResult> ArchitecturalCompletionCommitOwner::ValidateCandidateUnderLock(std::shared_ptr<ArchitecturalCompletionCommitOwner::ProducerRegistration> producer, std::shared_ptr<ArchitecturalCompletionCandidate> candidate, std::optional<std::unordered_set<std::shared_ptr<CompletionIdentityKey>>> &pendingKeys)
	{
		if (!producer || !candidate) return Denied(ArchitecturalCompletionCommitDecision::DeniedUnregisteredProducer, L"Completion commit requires the exact live producer registration.");
		auto registration = _registrations.find(producer->getOwnerIdentity());
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: if (producer == null || !producer.IsIssuedBy(this) || !_registrations.TryGetValue(producer.OwnerIdentity, out ProducerRegistration? live) || !ReferenceEquals(live, producer) || live.OwnerEpoch != producer.OwnerEpoch)
		if (!producer->IsIssuedBy(shared_from_this()) || registration == _registrations.end() || registration->second != producer || registration->second->getOwnerEpoch() != producer->getOwnerEpoch())
		{
			return Denied(ArchitecturalCompletionCommitDecision::DeniedUnregisteredProducer, L"Completion commit requires the exact live producer registration.");
		}

		if (!HasCompleteIdentity(candidate, producer->getPolicy()))
		{
			return Denied(ArchitecturalCompletionCommitDecision::DeniedMissingIdentity, L"Completion commit requires non-zero domain/context/VT attempt and event identity.");
		}

		if (!PolicyAllows(*producer->getPolicy(), candidate->Facts))
		{
			return Denied(ArchitecturalCompletionCommitDecision::DeniedProducerPolicy, L"Completion facts do not match the exact registered producer policy.");
		}

		std::shared_ptr<CompletionIdentityKey> key = CreateIdentityKey(producer, candidate);
		const auto hasEqualKey = [&key](const auto &keys) {
			return std::any_of(keys.begin(), keys.end(), [&key](const auto &existing) { return existing && *existing == *key; });
		};
		if (hasEqualKey(_committedCompletionKeys) || (pendingKeys && hasEqualKey(*pendingKeys)))
		{
			return Denied(ArchitecturalCompletionCommitDecision::DeniedDuplicateOrReplay, L"Completion identity was already committed or duplicated in the retire window.");
		}
		if (pendingKeys) pendingKeys->insert(std::move(key));

		return std::make_shared<ArchitecturalCompletionCommitResult>(ArchitecturalCompletionCommitResult{ArchitecturalCompletionCommitDecision::Committed, nullptr, L"Completion candidate passed exact producer, identity, policy, and replay prevalidation."});
	}

	std::shared_ptr<ArchitecturalCompletionCommitResult> ArchitecturalCompletionCommitOwner::CommitValidatedCandidateUnderLock(std::shared_ptr<ArchitecturalCompletionCommitOwner::ProducerRegistration> producer, std::shared_ptr<ArchitecturalCompletionCandidate> candidate)
	{
		std::shared_ptr<CompletionIdentityKey> key = CreateIdentityKey(producer, candidate);
		if (std::any_of(_committedCompletionKeys.begin(), _committedCompletionKeys.end(), [&key](const auto &existing) { return existing && *existing == *key; }))
		{
			throw InvalidOperationException(L"A prevalidated mandatory completion identity became non-unique before publication.");
		}
		_committedCompletionKeys.insert(key);

		std::wstring completionDigest = ComputeCompletionDigest(candidate->Facts);
		unsigned long long canonicalOrderSequence = AllocateNonZero(_nextCanonicalOrderSequence);
		unsigned long long commitSequence = AllocateNonZero(_nextCommitSequence);
		if (_latestReceipt != nullptr && canonicalOrderSequence <= _latestReceipt->getBinding()->CanonicalOrderSequence)
		{
			throw InvalidOperationException(L"Canonical completion order failed to advance after successful prevalidation.");
		}

		std::shared_ptr<ArchitecturalCompletionReceiptBinding> binding = std::make_shared<ArchitecturalCompletionReceiptBinding>(AllocateNonZero(_nextCompletionIdentity), producer->getOwnerIdentity(), producer->getOwnerEpoch(), candidate->DomainId, candidate->ContextId, candidate->VirtualThreadId, candidate->AttemptId, candidate->EventId, candidate->TranslationProvenance, candidate->Facts.CompletionClass, completionDigest, canonicalOrderSequence, commitSequence, _restoreOwner->getCurrentGeneration());
		std::wstring seal = ComputeSeal(binding);
		std::shared_ptr<ArchitecturalCompletionCommitReceipt> receipt = std::make_shared<ArchitecturalCompletionCommitReceipt>(shared_from_this(), binding, seal);
		EnsureObservationIssuerUnderLock();
		_observationOwner->InstallCommittedCompletion(_observationInstaller, shared_from_this(), binding, std::make_shared<NeutralArchitecturalCompletionFacts>(candidate->Facts));
		_liveReceipts.emplace(binding->CommitSequence, std::make_shared<LiveReceipt>(receipt, seal));
		_latestReceipt = receipt;
		return std::make_shared<ArchitecturalCompletionCommitResult>(ArchitecturalCompletionCommitResult{ArchitecturalCompletionCommitDecision::Committed, receipt, L"Completion committed at the canonical architectural completion boundary."});
	}

	std::shared_ptr<ArchitecturalCompletionCommitOwner::CompletionIdentityKey> ArchitecturalCompletionCommitOwner::CreateIdentityKey(std::shared_ptr<ArchitecturalCompletionCommitOwner::ProducerRegistration> producer, std::shared_ptr<ArchitecturalCompletionCandidate> candidate)
	{
		return std::make_shared<CompletionIdentityKey>(CompletionIdentityKey{producer->getOwnerIdentity(), producer->getOwnerEpoch(), candidate->DomainId, candidate->ContextId, candidate->VirtualThreadId, candidate->AttemptId, candidate->EventId, candidate->TranslationProvenance.MemoryOperationId});
	}

	bool ArchitecturalCompletionCommitOwner::HasCompleteIdentity(std::shared_ptr<ArchitecturalCompletionCandidate> candidate, std::shared_ptr<ArchitecturalCompletionProducerPolicy> policy)
	{
		return candidate && policy && candidate->DomainId != 0 && candidate->ContextId > 0 && (candidate->VirtualThreadId >= 0 && candidate->VirtualThreadId < Processor::CPU_Core::SmtWays) && candidate->AttemptId != 0 && candidate->EventId != 0 && (!policy->RequiresTranslationProvenance || candidate->TranslationProvenance.hasStageOneIdentity()) && (!policy->RequiresSecondStageProvenance || candidate->TranslationProvenance.hasSecondStageIdentity()) && candidate->Facts.CompletionClass != NeutralArchitecturalCompletionClass::None;
	}

	bool ArchitecturalCompletionCommitOwner::PolicyAllows(const ArchitecturalCompletionProducerPolicy &policy, const NeutralArchitecturalCompletionFacts &facts)
	{
		if (facts.CompletionClass != policy.CompletionClass || (policy.RequiresReason && !facts.Reason.IsPresent) || (!policy.AllowsQualification && facts.Qualification.IsPresent))
		{
			return false;
		}

		if (facts.FaultAddress.IsPresent)
		{
			if (facts.FaultAddress.Semantic == NeutralFaultAddressSemantic::None || facts.FaultAddress.Semantic != policy.AllowedAddressSemantic)
			{
				return false;
			}
		}
		else if (facts.FaultAddress.Semantic != NeutralFaultAddressSemantic::None)
		{
			return false;
		}

		if (facts.FaultAuxiliary.IsPresent)
		{
			if (facts.FaultAuxiliary.Semantic == NeutralFaultAuxiliarySemantic::None || facts.FaultAuxiliary.Semantic != policy.AllowedAuxiliarySemantic)
			{
				return false;
			}
		}
		else if (facts.FaultAuxiliary.Semantic != NeutralFaultAuxiliarySemantic::None)
		{
			return false;
		}

		return true;
	}

	std::wstring ArchitecturalCompletionCommitOwner::ComputeCompletionDigest(const NeutralArchitecturalCompletionFacts &facts)
	{
		const std::wstring canonical = std::to_wstring(static_cast<unsigned char>(facts.CompletionClass)) + L"|" +
			(facts.Reason.IsPresent ? L"1" : L"0") + L"|" + std::to_wstring(facts.Reason.Value) + L"|" +
			(facts.Qualification.IsPresent ? L"1" : L"0") + L"|" + std::to_wstring(facts.Qualification.Value) + L"|" +
			(facts.FaultAddress.IsPresent ? L"1" : L"0") + L"|" + std::to_wstring(facts.FaultAddress.Value) + L"|" +
			std::to_wstring(static_cast<unsigned char>(facts.FaultAddress.Semantic)) + L"|" +
			(facts.FaultAuxiliary.IsPresent ? L"1" : L"0") + L"|" + std::to_wstring(facts.FaultAuxiliary.Value) + L"|" +
			std::to_wstring(static_cast<unsigned char>(facts.FaultAuxiliary.Semantic));
		const std::string bytes = ToUtf8(canonical);
		return Decoder::Sha256Hex(std::span<const unsigned char>(reinterpret_cast<const unsigned char *>(bytes.data()), bytes.size()));
	}

	std::wstring ArchitecturalCompletionCommitOwner::ComputeSeal(std::shared_ptr<ArchitecturalCompletionReceiptBinding> binding)
	{
		const auto &provenance = binding->TranslationProvenance;
		const std::wstring canonical = std::to_wstring(binding->CompletionIdentity) + L"|" + std::to_wstring(binding->ProducerOwnerIdentity) + L"|" +
			std::to_wstring(binding->ProducerOwnerEpoch) + L"|" + std::to_wstring(binding->DomainId) + L"|" + std::to_wstring(binding->ContextId) + L"|" +
			std::to_wstring(binding->VirtualThreadId) + L"|" + std::to_wstring(binding->AttemptId) + L"|" + std::to_wstring(binding->EventId) + L"|" +
			(provenance.IsPresent ? L"1" : L"0") + L"|" + std::to_wstring(provenance.MemoryOperationId) + L"|" +
			std::to_wstring(provenance.TranslationOwnerEpoch) + L"|" + std::to_wstring(provenance.MemoryDomainOwnerEpoch) + L"|" +
			std::to_wstring(provenance.AddressSpaceGeneration) + L"|" + std::to_wstring(provenance.AddressSpaceIdentity) + L"|" +
			std::to_wstring(static_cast<unsigned char>(binding->CompletionClass)) + L"|" + binding->CompletionDigest + L"|" +
			std::to_wstring(binding->CanonicalOrderSequence) + L"|" + std::to_wstring(binding->CommitSequence) + L"|" + std::to_wstring(binding->RestoreGeneration);
		return HmacSha256Hex(_sealKey, canonical);
	}

	unsigned long long ArchitecturalCompletionCommitOwner::AllocateNonZero(unsigned long long &next)
	{
		unsigned long long value = next;
		AdvanceNonZero(next);
		return value == 0 ? AllocateNonZero(next) : value;
	}

	void ArchitecturalCompletionCommitOwner::AdvanceNonZero(unsigned long long &value)
	{
//		unchecked
			value++;
			if (value == 0)
			{
				value = 1;
			}
// C# TO C++ CONVERTER TASK: End of the original C# 'unchecked' block.
	}

	std::shared_ptr<ArchitecturalCompletionCommitResult> ArchitecturalCompletionCommitOwner::Denied(ArchitecturalCompletionCommitDecision decision, const std::wstring &reason)
	{
		return std::make_shared<ArchitecturalCompletionCommitResult>(ArchitecturalCompletionCommitResult{decision, nullptr, reason});
	}
}
