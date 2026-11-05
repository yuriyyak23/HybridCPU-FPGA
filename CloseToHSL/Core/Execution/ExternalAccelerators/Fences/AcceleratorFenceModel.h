#pragma once

#include "../Commit/AcceleratorCommitModel.h"
#include "../Conflicts/ExternalAcceleratorConflictManager.h"
#include "../Memory/AcceleratorMemoryModel.h"
#include "../Tokens/AcceleratorToken.h"
#include "../Tokens/AcceleratorTokenStore.h"
#include "../../../Architecture/Common/YAKSys_Hybrid_CPU.Processor.h"
#include <string>
#include <vector>
#include <functional>
#include <optional>
#include <memory>

using namespace HybridCPU::ExternalRuntime::Contracts;
using namespace YAKSys_Hybrid_CPU;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Auth;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Commit;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Conflicts;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Memory;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Tokens;

namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Fences
{

	enum class AcceleratorFenceActiveTokenDisposition : unsigned char
	{
		Reject = 0,
		Cancel = 1,
		Fault = 2
	};

	class AcceleratorFenceScope final : public std::enable_shared_from_this<AcceleratorFenceScope>
	{
	private:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly IReadOnlyList<AcceleratorTokenHandle> TokenHandles;
		const std::vector<std::shared_ptr<AcceleratorTokenHandle>> TokenHandles;
		const bool CommitCompletedTokens;
		const bool CommitConflictPlaceholderAccepted;
		const AcceleratorFenceActiveTokenDisposition ActiveTokenDisposition;
		const std::shared_ptr<AcceleratorCancelPolicy> ActiveCancelPolicy;

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private AcceleratorFenceScope(IReadOnlyList<AcceleratorTokenHandle> tokenHandles, bool commitCompletedTokens, bool commitConflictPlaceholderAccepted, AcceleratorFenceActiveTokenDisposition activeTokenDisposition, AcceleratorCancelPolicy activeCancelPolicy)
		AcceleratorFenceScope(const std::vector<std::shared_ptr<AcceleratorTokenHandle>> &tokenHandles, bool commitCompletedTokens, bool commitConflictPlaceholderAccepted, AcceleratorFenceActiveTokenDisposition activeTokenDisposition, std::shared_ptr<AcceleratorCancelPolicy> activeCancelPolicy);

	public:
		const std::vector<std::shared_ptr<AcceleratorTokenHandle>> getTokenHandles() const;

		bool getCommitCompletedTokens() const;

		bool getCommitConflictPlaceholderAccepted() const;

		AcceleratorFenceActiveTokenDisposition getActiveTokenDisposition() const;

		std::shared_ptr<AcceleratorCancelPolicy> getActiveCancelPolicy() const;

		static std::shared_ptr<AcceleratorFenceScope> ForToken(std::shared_ptr<AcceleratorTokenHandle> tokenHandle, bool commitCompletedTokens = false, bool commitConflictPlaceholderAccepted = true, AcceleratorFenceActiveTokenDisposition activeTokenDisposition = AcceleratorFenceActiveTokenDisposition::Reject, std::optional<std::shared_ptr<AcceleratorCancelPolicy>> activeCancelPolicy = std::nullopt);

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: public static AcceleratorFenceScope ForTokens(IReadOnlyList<AcceleratorTokenHandle> tokenHandles, bool commitCompletedTokens = false, bool commitConflictPlaceholderAccepted = true, AcceleratorFenceActiveTokenDisposition activeTokenDisposition = AcceleratorFenceActiveTokenDisposition.Reject, System.Nullable<AcceleratorCancelPolicy> activeCancelPolicy = null)
		static std::shared_ptr<AcceleratorFenceScope> ForTokens(const std::vector<std::shared_ptr<AcceleratorTokenHandle>> &tokenHandles, bool commitCompletedTokens = false, bool commitConflictPlaceholderAccepted = true, AcceleratorFenceActiveTokenDisposition activeTokenDisposition = AcceleratorFenceActiveTokenDisposition::Reject, std::optional<std::shared_ptr<AcceleratorCancelPolicy>> activeCancelPolicy = std::nullopt);

	private:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private static IReadOnlyList<AcceleratorTokenHandle> FreezeHandles(IReadOnlyList<AcceleratorTokenHandle> tokenHandles)
		static const std::vector<std::shared_ptr<AcceleratorTokenHandle>> FreezeHandles(const std::vector<std::shared_ptr<AcceleratorTokenHandle>> &tokenHandles);
	};

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public sealed record AcceleratorFenceResult
//	{
//		private AcceleratorFenceResult(IReadOnlyList<AcceleratorTokenLookupResult> tokenResults, IReadOnlyList<AcceleratorCommitResult> commitResults, string message)
//		{
//			TokenResults = FreezeTokenResults(tokenResults);
//			CommitResults = FreezeCommitResults(commitResults);
//			Message = message;
//		}
//
//		public IReadOnlyList<AcceleratorTokenLookupResult> TokenResults
//		{
//			get;
//		}
//
//		public IReadOnlyList<AcceleratorCommitResult> CommitResults
//		{
//			get;
//		}
//
//		public string Message
//		{
//			get;
//		}
//
//		public int RejectedCount
//		{
//			get
//			{
//				return CountRejected(TokenResults) + CountRejected(CommitResults);
//			}
//		}
//
//		public int CommittedCount
//		{
//			get
//			{
//				return CountCommitted(CommitResults);
//			}
//		}
//
//		public int CanceledCount
//		{
//			get
//			{
//				return CountState(AcceleratorTokenState.Canceled);
//			}
//		}
//
//		public int FaultedCount
//		{
//			get
//			{
//				return CountState(AcceleratorTokenState.Faulted);
//			}
//		}
//
//		public int ObservedCount
//		{
//			get
//			{
//				return TokenResults.Count;
//			}
//		}
//
//		public bool Succeeded
//		{
//			get
//			{
//				return RejectedCount == 0;
//			}
//		}
//
//		public bool IsRejected
//		{
//			get
//			{
//				return !Succeeded;
//			}
//		}
//
//		public bool CanPublishArchitecturalMemory
//		{
//			get
//			{
//				return CommittedCount != 0;
//			}
//		}
//
//		public bool UserVisiblePublicationAllowed
//		{
//			get
//			{
//				return CommittedCount != 0;
//			}
//		}
//
//		public AcceleratorTokenFaultCode FaultCode
//		{
//			get
//			{
//				for (int index = 0; index < TokenResults.Count; index++)
//				{
//					if (TokenResults[index].IsRejected)
//					{
//						return TokenResults[index].FaultCode;
//					}
//				}
//
//				for (int index = 0; index < CommitResults.Count; index++)
//				{
//					if (CommitResults[index].IsRejected)
//					{
//						return CommitResults[index].FaultCode;
//					}
//				}
//
//				return AcceleratorTokenFaultCode.None;
//			}
//		}
//
//		public ulong PackedFenceStatus
//		{
//			get
//			{
//				ulong packed = (ulong)(byte)(Succeeded ? 1 : 0);
//				packed |= (ulong)(byte)FaultCode << 8;
//				packed |= (ulong)(ushort)Math.Min(ObservedCount, ushort.MaxValue) << 16;
//				packed |= (ulong)(ushort)Math.Min(CommittedCount, ushort.MaxValue) << 32;
//				packed |= (ulong)(ushort)Math.Min(RejectedCount, ushort.MaxValue) << 48;
//				return packed;
//			}
//		}
//
//		public static AcceleratorFenceResult Complete(IReadOnlyList<AcceleratorTokenLookupResult> tokenResults, IReadOnlyList<AcceleratorCommitResult> commitResults, string message)
//		{
//			return new(tokenResults, commitResults, message);
//		}
//
//		private int CountState(AcceleratorTokenState state)
//		{
//			int count = 0;
//			for (int index = 0; index < TokenResults.Count; index++)
//			{
//				if (TokenResults[index].StatusWord.State == state)
//				{
//					count++;
//				}
//			}
//
//			return count;
//		}
//
//		private static int CountRejected(IReadOnlyList<AcceleratorTokenLookupResult> results)
//		{
//			int count = 0;
//			for (int index = 0; index < results.Count; index++)
//			{
//				if (results[index].IsRejected)
//				{
//					count++;
//				}
//			}
//
//			return count;
//		}
//
//		private static int CountRejected(IReadOnlyList<AcceleratorCommitResult> results)
//		{
//			int count = 0;
//			for (int index = 0; index < results.Count; index++)
//			{
//				if (results[index].IsRejected)
//				{
//					count++;
//				}
//			}
//
//			return count;
//		}
//
//		private static int CountCommitted(IReadOnlyList<AcceleratorCommitResult> results)
//		{
//			int count = 0;
//			for (int index = 0; index < results.Count; index++)
//			{
//				if (results[index].Succeeded)
//				{
//					count++;
//				}
//			}
//
//			return count;
//		}
//
//		private static IReadOnlyList<AcceleratorTokenLookupResult> FreezeTokenResults(IReadOnlyList<AcceleratorTokenLookupResult> results)
//		{
//			if (results == nullptr || results.Count == 0)
//			{
//				return Array.Empty<AcceleratorTokenLookupResult>();
//			}
//
//			AcceleratorTokenLookupResult[] copy = new AcceleratorTokenLookupResult[results.Count];
//			for (int index = 0; index < results.Count; index++)
//			{
//				copy[index] = results[index];
//			}
//
//			return Array.AsReadOnly(copy);
//		}
//
//		private static IReadOnlyList<AcceleratorCommitResult> FreezeCommitResults(IReadOnlyList<AcceleratorCommitResult> results)
//		{
//			if (results == nullptr || results.Count == 0)
//			{
//				return Array.Empty<AcceleratorCommitResult>();
//			}
//
//			AcceleratorCommitResult[] copy = new AcceleratorCommitResult[results.Count];
//			for (int index = 0; index < results.Count; index++)
//			{
//				copy[index] = results[index];
//			}
//
//			return Array.AsReadOnly(copy);
//		}
//	}

		class AcceleratorFenceResult final {
	public:
		const std::vector<std::shared_ptr<AcceleratorTokenLookupResult>> TokenResults;
		const std::vector<std::shared_ptr<AcceleratorCommitResult>> CommitResults;
		const std::wstring Message;
		const int RejectedCount, CommittedCount, CanceledCount, FaultedCount, ObservedCount;
		const bool Succeeded, IsRejected, CanPublishArchitecturalMemory, UserVisiblePublicationAllowed;
		const AcceleratorTokenFaultCode FaultCode;
		const unsigned long long PackedFenceStatus;
		static std::shared_ptr<AcceleratorFenceResult> Complete(const std::vector<std::shared_ptr<AcceleratorTokenLookupResult>>& tokens, const std::vector<std::shared_ptr<AcceleratorCommitResult>>& commits, const std::wstring& message) {
			int rejected=0,committed=0,canceled=0,faulted=0; AcceleratorTokenFaultCode fault=AcceleratorTokenFaultCode::None;
			for(const auto& r:tokens){if(r->IsRejected){++rejected;if(fault==AcceleratorTokenFaultCode::None)fault=r->FaultCode;} if(r->StatusWord.State==AcceleratorTokenState::Canceled)++canceled; if(r->StatusWord.State==AcceleratorTokenState::Faulted)++faulted;}
			for(const auto& r:commits){if(r->IsRejected){++rejected;if(fault==AcceleratorTokenFaultCode::None)fault=r->getFaultCode();} if(r->Succeeded)++committed;}
			return std::shared_ptr<AcceleratorFenceResult>(new AcceleratorFenceResult(tokens,commits,message,rejected,committed,canceled,faulted,fault));
		}
	private:
		AcceleratorFenceResult(const std::vector<std::shared_ptr<AcceleratorTokenLookupResult>>& t,const std::vector<std::shared_ptr<AcceleratorCommitResult>>& c,const std::wstring& m,int rejected,int committed,int canceled,int faulted,AcceleratorTokenFaultCode fault) : TokenResults(t),CommitResults(c),Message(m),RejectedCount(rejected),CommittedCount(committed),CanceledCount(canceled),FaultedCount(faulted),ObservedCount(static_cast<int>(t.size())),Succeeded(rejected==0),IsRejected(rejected!=0),CanPublishArchitecturalMemory(committed!=0),UserVisiblePublicationAllowed(committed!=0),FaultCode(fault),PackedFenceStatus((Succeeded?1ull:0ull)|(static_cast<unsigned long long>(static_cast<unsigned char>(fault))<<8)|(static_cast<unsigned long long>(std::min(t.size(),static_cast<size_t>(65535)))<<16)|(static_cast<unsigned long long>(std::min(static_cast<size_t>(committed),static_cast<size_t>(65535)))<<32)|(static_cast<unsigned long long>(std::min(static_cast<size_t>(rejected),static_cast<size_t>(65535)))<<48)) {}
	};
	class AcceleratorFenceCoordinator final : public std::enable_shared_from_this<AcceleratorFenceCoordinator>
	{
		// Runtime-side fence coordinator for Phase 08. ACCEL_FENCE reaches this
		// path only through the lane7 SystemSingleton runtime, never through a
		// backend/fallback direct publication shortcut.
	public:
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public AcceleratorFenceResult TryFence(AcceleratorTokenStore tokenStore, AcceleratorFenceScope scope, System.Nullable<AcceleratorGuardEvidence> currentGuardEvidence, IAcceleratorStagingBuffer? stagingBuffer = null, System.Nullable<Processor.MainMemoryArea> mainMemory = null, System.Nullable<AcceleratorCommitCoordinator> commitCoordinator = null, System.Nullable<AcceleratorCommitInvalidationPlan> invalidationPlan = null, System.Nullable<ExternalAcceleratorConflictManager> conflictManager = null, System.Nullable<System.Func<AcceleratorToken, System.Nullable<ExternalOperationPublicationEvidence>>> externalPublicationEvidenceFactory = null)
		std::shared_ptr<AcceleratorFenceResult> TryFence(std::shared_ptr<AcceleratorTokenStore> tokenStore, std::shared_ptr<AcceleratorFenceScope> scope, std::optional<std::shared_ptr<AcceleratorGuardEvidence>> &currentGuardEvidence, std::shared_ptr<IAcceleratorStagingBuffer> stagingBuffer = nullptr, std::shared_ptr<Processor::MainMemoryArea> mainMemory = nullptr, std::shared_ptr<AcceleratorCommitCoordinator> commitCoordinator = nullptr, std::shared_ptr<AcceleratorCommitInvalidationPlan> invalidationPlan = nullptr, std::shared_ptr<ExternalAcceleratorConflictManager> conflictManager = nullptr, std::function<std::optional<std::shared_ptr<ExternalOperationPublicationEvidence>>(std::shared_ptr<AcceleratorToken>)> externalPublicationEvidenceFactory = nullptr);

	private:
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private static AcceleratorTokenLookupResult ResolveActiveToken(AcceleratorTokenStore tokenStore, AcceleratorToken token, AcceleratorTokenHandle handle, AcceleratorTokenLookupResult lookup, AcceleratorFenceScope scope, System.Nullable<AcceleratorGuardEvidence> currentGuardEvidence, ExternalAcceleratorConflictManager? conflictManager)
		static std::shared_ptr<AcceleratorTokenLookupResult> ResolveActiveToken(std::shared_ptr<AcceleratorTokenStore> tokenStore, std::shared_ptr<AcceleratorToken> token, std::shared_ptr<AcceleratorTokenHandle> handle, std::shared_ptr<AcceleratorTokenLookupResult> lookup, std::shared_ptr<AcceleratorFenceScope> scope, std::optional<std::shared_ptr<AcceleratorGuardEvidence>> &currentGuardEvidence, std::shared_ptr<ExternalAcceleratorConflictManager> conflictManager);

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private static int CountRejected(IReadOnlyList<AcceleratorTokenLookupResult> results)
		static int CountRejected(const std::vector<std::shared_ptr<AcceleratorTokenLookupResult>> &results);

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private static int CountRejected(IReadOnlyList<AcceleratorCommitResult> results)
		static int CountRejected(const std::vector<std::shared_ptr<AcceleratorCommitResult>> &results);
	};

}



