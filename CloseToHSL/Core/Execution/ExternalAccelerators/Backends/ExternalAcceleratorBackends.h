#pragma once

#include "../../../../../ExternalRuntime/Contracts/ExternalOperationContracts.h"
#include "../Queues/AcceleratorCommandQueue.h"
#include "../Memory/AcceleratorMemoryModel.h"
#include "../Tokens/AcceleratorTokenStore.h"
#include "../Tokens/AcceleratorToken.h"
#include "../Telemetry/AcceleratorTelemetry.h"
#include "../Tokens/AcceleratorTokenStatusWord.h"
#include <string>
#include <vector>
#include <limits>
#include <optional>
#include <format>
#include <memory>
#include <span>
#include <utility>

using namespace YAKSys_Hybrid_CPU::Core;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Auth;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Descriptors;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Memory;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Queues;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Telemetry;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Tokens;

namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Backends
{

	class IAcceleratorBackendClock
	{
	public:
		virtual unsigned long long getTickCount() const = 0;

		virtual unsigned long long Advance() = 0;
	};

	class AcceleratorBackendResult;
	class IExternalAcceleratorBackend
	{
	public:
		virtual std::shared_ptr<AcceleratorBackendResult> TrySubmit(std::shared_ptr<AcceleratorQueueAdmissionRequest> request, std::shared_ptr<IAcceleratorCommandQueue> queue, std::optional<std::shared_ptr<AcceleratorGuardEvidence>> &currentGuardEvidence) = 0;

		virtual std::shared_ptr<AcceleratorBackendResult> Tick(std::shared_ptr<IAcceleratorCommandQueue> queue, std::shared_ptr<IAcceleratorMemoryPortal> memoryPortal, std::shared_ptr<IAcceleratorStagingBuffer> stagingBuffer, std::optional<std::shared_ptr<AcceleratorGuardEvidence>> &currentGuardEvidence) = 0;

		virtual std::shared_ptr<AcceleratorBackendResult> TryCancel(std::shared_ptr<AcceleratorTokenStore> tokenStore, std::shared_ptr<AcceleratorTokenHandle> handle, std::optional<std::shared_ptr<AcceleratorGuardEvidence>> &currentGuardEvidence) = 0;
	};

	/// <summary>
	/// Internal opt-in seam for a backend which requires provider lifecycle proof
	/// before the CPU commit coordinator may publish its staged output.
	/// </summary>
	class IExternalAcceleratorPublicationEvidenceProvider
	{
	public:
		virtual std::optional<std::shared_ptr<HybridCPU::ExternalRuntime::Contracts::ExternalOperationPublicationEvidence>> GetPublicationEvidence(std::shared_ptr<AcceleratorToken> token) = 0;
	};

	/// <summary>Internal marker for opt-in backends whose provider lifecycle is advanced by L7 Poll.</summary>
	class IExternalAcceleratorBackendPollsExternalOperation
	{
	};

	enum class NullExternalAcceleratorBackendPolicy : unsigned char
	{
		RejectSubmit = 0,
		FaultToken = 1
	};

	enum class AcceleratorBackendResultKind : unsigned char
	{
		Submitted = 0,
		Rejected = 1,
		Faulted = 2,
		DeviceCompleted = 3,
		Canceled = 4,
		Pending = 5
	};

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public sealed record AcceleratorBackendResult
//	{
//		private AcceleratorBackendResult(AcceleratorBackendResultKind kind, System.Nullable<AcceleratorToken> token, AcceleratorTokenFaultCode faultCode, System.Nullable<AcceleratorQueueAdmissionResult> queueResult, System.Nullable<AcceleratorTokenLookupResult> tokenLookupResult, ulong bytesRead, ulong bytesStaged, int stagedWriteCount, bool directWriteViolationDetected, ulong backendTick, string message)
//		{
//			Kind = kind;
//			Token = token;
//			FaultCode = faultCode;
//			QueueResult = queueResult;
//			TokenLookupResult = tokenLookupResult;
//			BytesRead = bytesRead;
//			BytesStaged = bytesStaged;
//			StagedWriteCount = stagedWriteCount;
//			DirectWriteViolationDetected = directWriteViolationDetected;
//			BackendTick = backendTick;
//			Message = message;
//		}
//
//		public AcceleratorBackendResultKind Kind
//		{
//			get;
//		}
//
//		public bool IsAccepted
//		{
//			get
//			{
//				return Kind is AcceleratorBackendResultKind.Submitted or AcceleratorBackendResultKind.DeviceCompleted or AcceleratorBackendResultKind.Canceled;
//			}
//		}
//
//		public bool IsRejected
//		{
//			get
//			{
//				return Kind == AcceleratorBackendResultKind.Rejected;
//			}
//		}
//
//		public bool IsFaulted
//		{
//			get
//			{
//				return Kind == AcceleratorBackendResultKind.Faulted;
//			}
//		}
//
//		public System.Nullable<AcceleratorToken> Token
//		{
//			get;
//		}
//
//		public AcceleratorTokenFaultCode FaultCode
//		{
//			get;
//		}
//
//		public System.Nullable<AcceleratorQueueAdmissionResult> QueueResult
//		{
//			get;
//		}
//
//		public System.Nullable<AcceleratorTokenLookupResult> TokenLookupResult
//		{
//			get;
//		}
//
//		public ulong BytesRead
//		{
//			get;
//		}
//
//		public ulong BytesStaged
//		{
//			get;
//		}
//
//		public int StagedWriteCount
//		{
//			get;
//		}
//
//		public bool DirectWriteViolationDetected
//		{
//			get;
//		}
//
//		public ulong BackendTick
//		{
//			get;
//		}
//
//		public string Message
//		{
//			get;
//		}
//
//		public bool CanPublishArchitecturalMemory
//		{
//			get
//			{
//				return false;
//			}
//		}
//
//		public bool CanPublishException
//		{
//			get
//			{
//				return false;
//			}
//		}
//
//		public bool UserVisiblePublicationAllowed
//		{
//			get
//			{
//				return false;
//			}
//		}
//
//		public static AcceleratorBackendResult Submitted(AcceleratorQueueAdmissionResult queueResult, ulong backendTick)
//		{
//			ArgumentNullException.ThrowIfNull(queueResult);
//			return new AcceleratorBackendResult(AcceleratorBackendResultKind.Submitted, (queueResult.Command == nullptr ? nullptr : queueResult.Command.Token), AcceleratorTokenFaultCode.None, queueResult, tokenLookupResult: nullptr, bytesRead: 0, bytesStaged: 0, stagedWriteCount: 0, directWriteViolationDetected: false, backendTick, queueResult.Message);
//		}
//
//		public static AcceleratorBackendResult DeviceCompleted(AcceleratorToken token, ulong bytesRead, ulong bytesStaged, int stagedWriteCount, ulong backendTick, string message)
//		{
//			ArgumentNullException.ThrowIfNull(token);
//			return new AcceleratorBackendResult(AcceleratorBackendResultKind.DeviceCompleted, token, AcceleratorTokenFaultCode.None, queueResult: nullptr, tokenLookupResult: nullptr, bytesRead, bytesStaged, stagedWriteCount, directWriteViolationDetected: false, backendTick, message);
//		}
//
//		public static AcceleratorBackendResult Canceled(AcceleratorTokenLookupResult lookupResult, ulong backendTick)
//		{
//			ArgumentNullException.ThrowIfNull(lookupResult);
//			return new AcceleratorBackendResult(AcceleratorBackendResultKind.Canceled, lookupResult.Token, AcceleratorTokenFaultCode.None, queueResult: nullptr, lookupResult, bytesRead: 0, bytesStaged: 0, stagedWriteCount: 0, directWriteViolationDetected: false, backendTick, lookupResult.Message);
//		}
//
//		/// <summary>Non-terminal provider observation. It does not imply completion or visibility.</summary>
//		public static AcceleratorBackendResult Pending(AcceleratorToken token, ulong backendTick, string message)
//		{
//			ArgumentNullException.ThrowIfNull(token);
//			return new AcceleratorBackendResult(AcceleratorBackendResultKind.Pending, token, AcceleratorTokenFaultCode.None, queueResult: nullptr, tokenLookupResult: nullptr, bytesRead: 0, bytesStaged: 0, stagedWriteCount: 0, directWriteViolationDetected: false, backendTick, message);
//		}
//
//		public static AcceleratorBackendResult Rejected(AcceleratorTokenFaultCode faultCode, string message, ulong backendTick, System.Nullable<AcceleratorToken> token = nullptr, System.Nullable<AcceleratorQueueAdmissionResult> queueResult = nullptr, System.Nullable<AcceleratorTokenLookupResult> tokenLookupResult = nullptr)
//		{
//			if (faultCode == AcceleratorTokenFaultCode.None)
//			{
//				throw new ArgumentException("Rejected L7-SDC backend operations require a fault code.", nameof(faultCode));
//			}
//
//			return new AcceleratorBackendResult(AcceleratorBackendResultKind.Rejected, token, faultCode, queueResult, tokenLookupResult, bytesRead: 0, bytesStaged: 0, stagedWriteCount: 0, directWriteViolationDetected: false, backendTick, message);
//		}
//
//		public static AcceleratorBackendResult Faulted(AcceleratorToken token, AcceleratorTokenFaultCode faultCode, string message, ulong backendTick, bool directWriteViolationDetected = false)
//		{
//			ArgumentNullException.ThrowIfNull(token);
//			if (faultCode == AcceleratorTokenFaultCode.None)
//			{
//				throw new ArgumentException("Faulted L7-SDC backend operations require a fault code.", nameof(faultCode));
//			}
//
//			return new AcceleratorBackendResult(AcceleratorBackendResultKind.Faulted, token, faultCode, queueResult: nullptr, tokenLookupResult: nullptr, bytesRead: 0, bytesStaged: 0, stagedWriteCount: 0, directWriteViolationDetected, backendTick, message);
//		}
//	}

	// Restored from the commented C# record declaration above. This result is a
	// telemetry/value packet only; it never grants architectural publication.
	class AcceleratorBackendResult final
	{
	public:
		const AcceleratorBackendResultKind Kind;
		const bool IsAccepted;
		const bool IsRejected;
		const bool IsFaulted;
		const std::shared_ptr<AcceleratorToken> Token;
		const AcceleratorTokenFaultCode FaultCode;
		const std::shared_ptr<AcceleratorQueueAdmissionResult> QueueResult;
		const std::shared_ptr<AcceleratorTokenLookupResult> TokenLookupResult;
		const unsigned long long BytesRead;
		const unsigned long long BytesStaged;
		const int StagedWriteCount;
		const bool DirectWriteViolationDetected;
		const unsigned long long BackendTick;
		const std::wstring Message;
		const bool CanPublishArchitecturalMemory = false;
		const bool CanPublishException = false;
		const bool UserVisiblePublicationAllowed = false;

		AcceleratorBackendResult(AcceleratorBackendResultKind kind, std::shared_ptr<AcceleratorToken> token,
			AcceleratorTokenFaultCode faultCode, std::shared_ptr<AcceleratorQueueAdmissionResult> queueResult,
			std::shared_ptr<AcceleratorTokenLookupResult> tokenLookupResult, unsigned long long bytesRead,
			unsigned long long bytesStaged, int stagedWriteCount, bool directWriteViolationDetected,
			unsigned long long backendTick, const std::wstring& message)
			: Kind(kind), IsAccepted(kind == AcceleratorBackendResultKind::Submitted || kind == AcceleratorBackendResultKind::DeviceCompleted || kind == AcceleratorBackendResultKind::Canceled), IsRejected(kind == AcceleratorBackendResultKind::Rejected), IsFaulted(kind == AcceleratorBackendResultKind::Faulted), Token(std::move(token)), FaultCode(faultCode), QueueResult(std::move(queueResult)),
			  TokenLookupResult(std::move(tokenLookupResult)), BytesRead(bytesRead), BytesStaged(bytesStaged),
			  StagedWriteCount(stagedWriteCount), DirectWriteViolationDetected(directWriteViolationDetected),
			  BackendTick(backendTick), Message(message) {}

		bool getIsAccepted() const { return Kind == AcceleratorBackendResultKind::Submitted || Kind == AcceleratorBackendResultKind::DeviceCompleted || Kind == AcceleratorBackendResultKind::Canceled; }
		bool getIsRejected() const { return Kind == AcceleratorBackendResultKind::Rejected; }
		bool getIsFaulted() const { return Kind == AcceleratorBackendResultKind::Faulted; }

		static std::shared_ptr<AcceleratorBackendResult> Submitted(std::shared_ptr<AcceleratorQueueAdmissionResult> queueResult, unsigned long long backendTick)
		{
			if (!queueResult) throw std::invalid_argument("queueResult");
			std::shared_ptr<AcceleratorToken> token = queueResult->Command == nullptr ? nullptr : queueResult->Command->Token;
			return std::make_shared<AcceleratorBackendResult>(AcceleratorBackendResultKind::Submitted, token, AcceleratorTokenFaultCode::None, queueResult, nullptr, 0, 0, 0, false, backendTick, queueResult->Message);
		}

		static std::shared_ptr<AcceleratorBackendResult> DeviceCompleted(std::shared_ptr<AcceleratorToken> token, unsigned long long bytesRead, unsigned long long bytesStaged, int stagedWriteCount, unsigned long long backendTick, const std::wstring& message)
		{
			if (!token) throw std::invalid_argument("token");
			return std::make_shared<AcceleratorBackendResult>(AcceleratorBackendResultKind::DeviceCompleted, std::move(token), AcceleratorTokenFaultCode::None, nullptr, nullptr, bytesRead, bytesStaged, stagedWriteCount, false, backendTick, message);
		}

		static std::shared_ptr<AcceleratorBackendResult> Canceled(std::shared_ptr<AcceleratorTokenLookupResult> lookupResult, unsigned long long backendTick)
		{
			if (!lookupResult) throw std::invalid_argument("lookupResult");
			return std::make_shared<AcceleratorBackendResult>(AcceleratorBackendResultKind::Canceled, lookupResult->Token, AcceleratorTokenFaultCode::None, nullptr, lookupResult, 0, 0, 0, false, backendTick, lookupResult->Message);
		}

		static std::shared_ptr<AcceleratorBackendResult> Pending(std::shared_ptr<AcceleratorToken> token, unsigned long long backendTick, const std::wstring& message)
		{
			if (!token) throw std::invalid_argument("token");
			return std::make_shared<AcceleratorBackendResult>(AcceleratorBackendResultKind::Pending, std::move(token), AcceleratorTokenFaultCode::None, nullptr, nullptr, 0, 0, 0, false, backendTick, message);
		}

		static std::shared_ptr<AcceleratorBackendResult> Rejected(AcceleratorTokenFaultCode faultCode, const std::wstring& message, unsigned long long backendTick, std::shared_ptr<AcceleratorToken> token = nullptr, std::shared_ptr<AcceleratorQueueAdmissionResult> queueResult = nullptr, std::shared_ptr<AcceleratorTokenLookupResult> tokenLookupResult = nullptr)
		{
			if (faultCode == AcceleratorTokenFaultCode::None) throw std::invalid_argument("Rejected L7-SDC backend operations require a fault code.");
			return std::make_shared<AcceleratorBackendResult>(AcceleratorBackendResultKind::Rejected, std::move(token), faultCode, std::move(queueResult), std::move(tokenLookupResult), 0, 0, 0, false, backendTick, message);
		}

		static std::shared_ptr<AcceleratorBackendResult> Faulted(std::shared_ptr<AcceleratorToken> token, AcceleratorTokenFaultCode faultCode, const std::wstring& message, unsigned long long backendTick, bool directWriteViolationDetected = false)
		{
			if (!token) throw std::invalid_argument("token");
			if (faultCode == AcceleratorTokenFaultCode::None) throw std::invalid_argument("Faulted L7-SDC backend operations require a fault code.");
			return std::make_shared<AcceleratorBackendResult>(AcceleratorBackendResultKind::Faulted, std::move(token), faultCode, nullptr, nullptr, 0, 0, 0, directWriteViolationDetected, backendTick, message);
		}
	};

	class ManualAcceleratorBackendClock final : public IAcceleratorBackendClock
	{
	private:
		unsigned long long TickCount = 0;

	public:
		unsigned long long getTickCount() const override;
	private:
		void setTickCount(unsigned long long value);

	public:
		unsigned long long Advance() override;
	};

	class NullExternalAcceleratorBackend final : public IExternalAcceleratorBackend
	{
	private:
		const std::shared_ptr<IAcceleratorBackendClock> _clock;
		const NullExternalAcceleratorBackendPolicy _policy;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private readonly AcceleratorTelemetry? _telemetry;
		const std::shared_ptr<AcceleratorTelemetry> _telemetry;
		const std::shared_ptr<ExternalAcceleratorFeatureSwitch> _featureSwitch;

	public:
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public NullExternalAcceleratorBackend(NullExternalAcceleratorBackendPolicy policy = NullExternalAcceleratorBackendPolicy.RejectSubmit, IAcceleratorBackendClock? clock = null, System.Nullable<AcceleratorTelemetry> telemetry = null, System.Nullable<ExternalAcceleratorFeatureSwitch> featureSwitch = null)
		NullExternalAcceleratorBackend(NullExternalAcceleratorBackendPolicy policy = NullExternalAcceleratorBackendPolicy::RejectSubmit, std::shared_ptr<IAcceleratorBackendClock> clock = nullptr, std::shared_ptr<AcceleratorTelemetry> telemetry = nullptr, std::optional<std::shared_ptr<ExternalAcceleratorFeatureSwitch>> featureSwitch = std::nullopt);

		std::shared_ptr<AcceleratorBackendResult> TrySubmit(std::shared_ptr<AcceleratorQueueAdmissionRequest> request, std::shared_ptr<IAcceleratorCommandQueue> queue, std::optional<std::shared_ptr<AcceleratorGuardEvidence>> &currentGuardEvidence) override;

		std::shared_ptr<AcceleratorBackendResult> Tick(std::shared_ptr<IAcceleratorCommandQueue> queue, std::shared_ptr<IAcceleratorMemoryPortal> memoryPortal, std::shared_ptr<IAcceleratorStagingBuffer> stagingBuffer, std::optional<std::shared_ptr<AcceleratorGuardEvidence>> &currentGuardEvidence) override;

		std::shared_ptr<AcceleratorBackendResult> TryCancel(std::shared_ptr<AcceleratorTokenStore> tokenStore, std::shared_ptr<AcceleratorTokenHandle> handle, std::optional<std::shared_ptr<AcceleratorGuardEvidence>> &currentGuardEvidence) override;

	private:
		std::shared_ptr<AcceleratorBackendResult> RecordResult(std::shared_ptr<AcceleratorBackendResult> result);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private static bool TryValidateFaultPolicyRequest(AcceleratorQueueAdmissionRequest request, AcceleratorGuardEvidence currentGuardEvidence, out AcceleratorToken? token, out AcceleratorTokenFaultCode faultCode, out string message)
		static bool TryValidateFaultPolicyRequest(std::shared_ptr<AcceleratorQueueAdmissionRequest> request, std::shared_ptr<AcceleratorGuardEvidence> currentGuardEvidence, std::shared_ptr<AcceleratorToken> &token, AcceleratorTokenFaultCode &faultCode, std::wstring &message);
	};

	class FakeExternalAcceleratorBackend final : public IExternalAcceleratorBackend
	{
	private:
		const std::shared_ptr<IAcceleratorBackendClock> _clock;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private readonly AcceleratorTelemetry? _telemetry;
		const std::shared_ptr<AcceleratorTelemetry> _telemetry;
		const std::shared_ptr<ExternalAcceleratorFeatureSwitch> _featureSwitch;

	public:
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public FakeExternalAcceleratorBackend(IAcceleratorBackendClock? clock = null, System.Nullable<AcceleratorTelemetry> telemetry = null, System.Nullable<ExternalAcceleratorFeatureSwitch> featureSwitch = null)
		FakeExternalAcceleratorBackend(std::shared_ptr<IAcceleratorBackendClock> clock = nullptr, std::shared_ptr<AcceleratorTelemetry> telemetry = nullptr, std::optional<std::shared_ptr<ExternalAcceleratorFeatureSwitch>> featureSwitch = std::nullopt);

		bool isTestOnly() const;

		std::shared_ptr<AcceleratorBackendResult> TrySubmit(std::shared_ptr<AcceleratorQueueAdmissionRequest> request, std::shared_ptr<IAcceleratorCommandQueue> queue, std::optional<std::shared_ptr<AcceleratorGuardEvidence>> &currentGuardEvidence) override;

		std::shared_ptr<AcceleratorBackendResult> Tick(std::shared_ptr<IAcceleratorCommandQueue> queue, std::shared_ptr<IAcceleratorMemoryPortal> memoryPortal, std::shared_ptr<IAcceleratorStagingBuffer> stagingBuffer, std::optional<std::shared_ptr<AcceleratorGuardEvidence>> &currentGuardEvidence) override;

		std::shared_ptr<AcceleratorBackendResult> TryCancel(std::shared_ptr<AcceleratorTokenStore> tokenStore, std::shared_ptr<AcceleratorTokenHandle> handle, std::optional<std::shared_ptr<AcceleratorGuardEvidence>> &currentGuardEvidence) override;

	private:
		std::shared_ptr<AcceleratorBackendResult> RecordResult(std::shared_ptr<AcceleratorBackendResult> result);

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private static byte[] BuildDeterministicStagedBytes(AcceleratorToken token, IReadOnlyList<AcceleratorMemoryRead> reads, int length)
		static std::vector<unsigned char> BuildDeterministicStagedBytes(std::shared_ptr<AcceleratorToken> token, const std::vector<std::shared_ptr<AcceleratorMemoryRead>> &reads, int length);
	};

	class DirectWriteViolationBackend final : public IExternalAcceleratorBackend
	{
	private:
		const std::shared_ptr<IAcceleratorBackendClock> _clock;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private readonly AcceleratorTelemetry? _telemetry;
		const std::shared_ptr<AcceleratorTelemetry> _telemetry;

	public:
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public DirectWriteViolationBackend(IAcceleratorBackendClock? clock = null, System.Nullable<AcceleratorTelemetry> telemetry = null)
		DirectWriteViolationBackend(std::shared_ptr<IAcceleratorBackendClock> clock = nullptr, std::shared_ptr<AcceleratorTelemetry> telemetry = nullptr);

		bool isTestOnly() const;

		std::shared_ptr<AcceleratorBackendResult> TrySubmit(std::shared_ptr<AcceleratorQueueAdmissionRequest> request, std::shared_ptr<IAcceleratorCommandQueue> queue, std::optional<std::shared_ptr<AcceleratorGuardEvidence>> &currentGuardEvidence) override;

		std::shared_ptr<AcceleratorBackendResult> Tick(std::shared_ptr<IAcceleratorCommandQueue> queue, std::shared_ptr<IAcceleratorMemoryPortal> memoryPortal, std::shared_ptr<IAcceleratorStagingBuffer> stagingBuffer, std::optional<std::shared_ptr<AcceleratorGuardEvidence>> &currentGuardEvidence) override;

		std::shared_ptr<AcceleratorBackendResult> TryCancel(std::shared_ptr<AcceleratorTokenStore> tokenStore, std::shared_ptr<AcceleratorTokenHandle> handle, std::optional<std::shared_ptr<AcceleratorGuardEvidence>> &currentGuardEvidence) override;

		std::shared_ptr<AcceleratorBackendResult> AttemptDirectWriteForTest(std::shared_ptr<AcceleratorQueuedCommand> command, std::optional<std::shared_ptr<AcceleratorGuardEvidence>> &currentGuardEvidence, unsigned long long address, std::span<const unsigned char> attemptedData);

	private:
		std::shared_ptr<AcceleratorBackendResult> RecordResult(std::shared_ptr<AcceleratorBackendResult> result);
	};

}


