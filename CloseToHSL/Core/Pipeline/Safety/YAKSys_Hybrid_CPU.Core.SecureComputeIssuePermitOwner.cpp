#include "YAKSys_Hybrid_CPU.Core.SecureComputeIssuePermitOwner.h"

#include <algorithm>
#include <memory>
#include <mutex>
#include <utility>

using namespace HybridCPU::ExternalRuntime::Contracts;

namespace YAKSys_Hybrid_CPU::Core
{
	std::shared_ptr<SecureComputeIssuePermit> SecureComputeIssuePermitOwner::Register(
		std::shared_ptr<SecureComputeMachineCertificate> certificate)
	{
		if (!certificate) return nullptr;
		std::scoped_lock<std::recursive_mutex> lock(gate);
		const auto issuerOwner = issuer.lock();
		if (!issuerOwner || !issuerOwner->RegisterSecureComputeCertificatePermit(certificate)) return nullptr;

		auto permit = std::make_shared<SecureComputeIssuePermit>(std::move(certificate));
		live.insert(permit);
		return permit;
	}

	std::shared_ptr<SecureComputeMachinePermitConsumption> SecureComputeIssuePermitOwner::Consume(
		std::shared_ptr<SecureComputeIssuePermit> permit,
		std::shared_ptr<ExternalSecureRegisterLocalRequest> request,
		std::shared_ptr<SecureComputePlacedIntent> placed,
		std::shared_ptr<ExternalSecureExecutionReservation> reservation)
	{
		if (!permit || !request || !placed || !reservation) return nullptr;
		std::scoped_lock<std::recursive_mutex> lock(gate);
		const auto issuerOwner = issuer.lock();
		if (!issuerOwner || !live.contains(permit)) return nullptr;

		const auto certificate = permit->getCertificate();
		if (!certificate || !issuerOwner->IsCurrentSecureComputeCertificate(certificate) ||
			certificate->getPlaced() != placed || certificate->getReservation() != reservation ||
			placed->getRequest() != request || reservation->getRequest() != request)
		{
			return nullptr;
		}

		live.erase(permit);
		consumed.insert(permit);
		return std::make_shared<SecureComputeMachinePermitConsumption>(
			std::make_shared<Guid>(issuerOwner->getSecureRegisterLocalScope()),
			certificate->getGeneration(), std::move(placed), std::move(reservation));
	}

	void SecureComputeIssuePermitOwner::PruneInvalidated()
	{
		std::scoped_lock<std::recursive_mutex> lock(gate);
		const auto issuerOwner = issuer.lock();
		if (!issuerOwner)
		{
			live.clear();
			consumed.clear();
			memoryLive.clear();
			memoryConsumed.clear();
			return;
		}

		std::erase_if(live, [&issuerOwner](const auto& permit)
		{
			return !permit || !permit->getCertificate() ||
				!issuerOwner->IsCurrentSecureComputeCertificate(permit->getCertificate());
		});
		std::erase_if(consumed, [&issuerOwner](const auto& permit)
		{
			return !permit || !permit->getCertificate() ||
				!issuerOwner->IsCurrentSecureComputeCertificate(permit->getCertificate());
		});
		std::erase_if(memoryLive, [&issuerOwner](const auto& permit)
		{
			return !permit || !permit->getCertificate() ||
				!issuerOwner->IsCurrentMemoryReadCertificate(permit->getCertificate());
		});
		std::erase_if(memoryConsumed, [&issuerOwner](const auto& permit)
		{
			return !permit || !permit->getCertificate() ||
				!issuerOwner->IsCurrentMemoryReadCertificate(permit->getCertificate());
		});
	}

	bool SecureComputeIssuePermitOwner::HasConsumed(std::shared_ptr<SecureComputeIssuePermit> permit)
	{
		if (!permit) return false;
		std::scoped_lock<std::recursive_mutex> lock(gate);
		const auto issuerOwner = issuer.lock();
		const auto certificate = permit->getCertificate();
		return issuerOwner && certificate && consumed.contains(permit) &&
			issuerOwner->IsCurrentSecureComputeCertificate(certificate);
	}

	std::shared_ptr<SecureComputeMemoryReadIssuePermit> SecureComputeIssuePermitOwner::Register(
		std::shared_ptr<SecureComputeMemoryReadCertificate> certificate)
	{
		if (!certificate) return nullptr;
		std::scoped_lock<std::recursive_mutex> lock(gate);
		const auto issuerOwner = issuer.lock();
		if (!issuerOwner || !issuerOwner->RegisterMemoryReadPermit(certificate)) return nullptr;

		auto permit = std::make_shared<SecureComputeMemoryReadIssuePermit>(std::move(certificate));
		memoryLive.insert(permit);
		return permit;
	}

	bool SecureComputeIssuePermitOwner::Consume(std::shared_ptr<SecureComputeMemoryReadIssuePermit> permit)
	{
		if (!permit) return false;
		std::scoped_lock<std::recursive_mutex> lock(gate);
		const auto issuerOwner = issuer.lock();
		const auto certificate = permit->getCertificate();
		if (!issuerOwner || !certificate || !memoryLive.contains(permit) ||
			!issuerOwner->IsCurrentMemoryReadCertificate(certificate))
		{
			return false;
		}

		memoryLive.erase(permit);
		memoryConsumed.insert(permit);
		return true;
	}

	bool SecureComputeIssuePermitOwner::HasConsumed(std::shared_ptr<SecureComputeMemoryReadIssuePermit> permit)
	{
		if (!permit) return false;
		std::scoped_lock<std::recursive_mutex> lock(gate);
		const auto issuerOwner = issuer.lock();
		const auto certificate = permit->getCertificate();
		return issuerOwner && certificate && memoryConsumed.contains(permit) &&
			issuerOwner->IsCurrentMemoryReadCertificate(certificate);
	}

	void SecureComputeIssuePermitOwner::PruneMemoryRead()
	{
		std::scoped_lock<std::recursive_mutex> lock(gate);
		const auto issuerOwner = issuer.lock();
		if (!issuerOwner)
		{
			memoryLive.clear();
			memoryConsumed.clear();
			return;
		}

		std::erase_if(memoryLive, [&issuerOwner](const auto& permit)
		{
			return !permit || !permit->getCertificate() ||
				!issuerOwner->IsCurrentMemoryReadCertificate(permit->getCertificate());
		});
		std::erase_if(memoryConsumed, [&issuerOwner](const auto& permit)
		{
			return !permit || !permit->getCertificate() ||
				!issuerOwner->IsCurrentMemoryReadCertificate(permit->getCertificate());
		});
	}
}