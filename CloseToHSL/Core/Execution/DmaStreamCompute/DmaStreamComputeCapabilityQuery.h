#pragma once

#include "DmaStreamComputeOwnerBinding.h"
#include "DmaStreamComputeReplayEvidence.h"
#include <memory>
#include <string>
#include <stdexcept>
#include <utility>

namespace YAKSys_Hybrid_CPU::Core::Execution::DmaStreamCompute
{
// C# TO C++ CONVERTER NOTE: The following .NET attribute has no direct equivalent in C++:
// ORIGINAL LINE: [Flags] public enum DmaStreamComputeCapabilityFlags : ulong
	enum class DmaStreamComputeCapabilityFlags : int
	{
		None = 0,
		Lane6DmaStreamBackend = 1ULL << 0,
		CurrentDsc1Production = 1ULL << 1,
		DscStatusQuery = 1ULL << 2,
		Dsc2ParserFootprintFoundation = 1ULL << 3,
		DscQueryCaps = 1ULL << 4
	};
	constexpr DmaStreamComputeCapabilityFlags operator|(DmaStreamComputeCapabilityFlags left, DmaStreamComputeCapabilityFlags right) noexcept
	{
		return static_cast<DmaStreamComputeCapabilityFlags>(static_cast<int>(left) | static_cast<int>(right));
	}

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public sealed record DmaStreamComputeCapabilityQueryResult
//	{
//		private DmaStreamComputeCapabilityQueryResult(DmaStreamComputeOwnerBinding ownerBinding, DmaStreamComputeCapabilityFlags capabilityFlags, string message)
//		{
//			OwnerBinding = ownerBinding;
//			CapabilityFlags = capabilityFlags;
//			Message = message;
//		}
//
//		public DmaStreamComputeOwnerBinding OwnerBinding
//		{
//			get;
//		}
//
//		public DmaStreamComputeCapabilityFlags CapabilityFlags
//		{
//			get;
//		}
//
//		public ulong EncodedCapabilityWord
//		{
//			get
//			{
//				return (ulong)CapabilityFlags;
//			}
//		}
//
//		public string Message
//		{
//			get;
//		}
//
//		public bool IsAccepted
//		{
//			get
//			{
//				return true;
//			}
//		}
//
//		public bool CanIssueToken
//		{
//			get
//			{
//				return false;
//			}
//		}
//
//		public bool CanPublishMemory
//		{
//			get
//			{
//				return false;
//			}
//		}
//
//		public bool CanProductionLower
//		{
//			get
//			{
//				return false;
//			}
//		}
//
//		public static DmaStreamComputeCapabilityQueryResult Capture(DmaStreamComputeOwnerBinding ownerBinding)
//		{
//			ArgumentNullException.ThrowIfNull(ownerBinding);
//			return new DmaStreamComputeCapabilityQueryResult(ownerBinding, DmaStreamComputeCapabilityFlags.Lane6DmaStreamBackend | DmaStreamComputeCapabilityFlags.CurrentDsc1Production | DmaStreamComputeCapabilityFlags.DscStatusQuery | DmaStreamComputeCapabilityFlags.Dsc2ParserFootprintFoundation | DmaStreamComputeCapabilityFlags.DscQueryCaps, "DSC_QUERY_CAPS captured the current lane6 DMA/stream capability word; execution authority, token issue, memory publication, and compiler lowering are unchanged.");
//		}
//	}

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct DmaStreamComputeCapabilityQueryReplayEvidence
//	{
//		public DmaStreamComputeCapabilityQueryReplayEvidence(uint opcode, DmaStreamComputeOwnerBinding ownerBinding, ulong encodedCapabilityWord, bool ownerBindingRevalidated, DmaStreamComputeLanePlacementEvidence lanePlacementEvidence, ulong evidenceHash)
//		{
//			Opcode = opcode;
//			OwnerBinding = ownerBinding;
//			EncodedCapabilityWord = encodedCapabilityWord;
//			OwnerBindingRevalidated = ownerBindingRevalidated;
//			LanePlacementEvidence = lanePlacementEvidence;
//			EvidenceHash = evidenceHash;
//		}
//
//		public uint Opcode
//		{
//			get;
//		}
//
//		public DmaStreamComputeOwnerBinding OwnerBinding
//		{
//			get;
//		}
//
//		public ulong EncodedCapabilityWord
//		{
//			get;
//		}
//
//		public bool OwnerBindingRevalidated
//		{
//			get;
//		}
//
//		public DmaStreamComputeLanePlacementEvidence LanePlacementEvidence
//		{
//			get;
//		}
//
//		public ulong EvidenceHash
//		{
//			get;
//		}
//
//		public bool IsComplete
//		{
//			get
//			{
//				return Opcode == Processor.CPU_Core.IsaOpcodeValues.DSC_QUERY_CAPS && EncodedCapabilityWord != 0 && OwnerBindingRevalidated && LanePlacementEvidence.IsComplete && EvidenceHash != 0;
//			}
//		}
//
//		public static DmaStreamComputeCapabilityQueryReplayEvidence Capture(DmaStreamComputeOwnerBinding ownerBinding, DmaStreamComputeCapabilityQueryResult result, DmaStreamComputeLanePlacementEvidence lanePlacementEvidence)
//		{
//			ArgumentNullException.ThrowIfNull(ownerBinding);
//			ArgumentNullException.ThrowIfNull(result);
//
//			bool ownerBindingRevalidated = result.OwnerBinding.Equals(ownerBinding) && ownerBinding.DeviceId == DmaStreamComputeDescriptor.CanonicalLane6DeviceId;
//			ulong encodedCapabilityWord = result.EncodedCapabilityWord;
//
//			ulong hash = 14695981039346656037UL;
//			Add(Processor.CPU_Core.IsaOpcodeValues.DSC_QUERY_CAPS);
//			Add(encodedCapabilityWord);
//			Add(ownerBinding.OwnerVirtualThreadId);
//			Add(ownerBinding.OwnerContextId);
//			Add(ownerBinding.OwnerCoreId);
//			Add(ownerBinding.OwnerPodId);
//			Add(ownerBinding.OwnerDomainTag);
//			Add(ownerBinding.DeviceId);
//			Add(ownerBindingRevalidated ? 1UL : 0UL);
//			Add(lanePlacementEvidence.EvidenceHash);
//
//			return new DmaStreamComputeCapabilityQueryReplayEvidence(Processor.CPU_Core.IsaOpcodeValues.DSC_QUERY_CAPS, ownerBinding, encodedCapabilityWord, ownerBindingRevalidated, lanePlacementEvidence, hash == 0 ? 0xD5C0_07A0UL : hash);
//
//			void Add(ulong value)
//			{
//				unchecked
//				{
//					hash ^= value;
//					hash *= 1099511628211UL;
//				}
//			}
//		}
//	}
	struct DmaStreamComputeCapabilityQueryResult final
	{
		std::shared_ptr<DmaStreamComputeOwnerBinding> OwnerBinding;
		DmaStreamComputeCapabilityFlags CapabilityFlags{};
		std::wstring Message;
		unsigned long long EncodedCapabilityWord = 0;
		bool IsAccepted = true;
		bool CanIssueToken = false;
		bool CanPublishMemory = false;
		bool CanProductionLower = false;

		DmaStreamComputeCapabilityQueryResult(std::shared_ptr<DmaStreamComputeOwnerBinding> ownerBinding,
			DmaStreamComputeCapabilityFlags capabilityFlags, std::wstring message)
			: OwnerBinding(std::move(ownerBinding)), CapabilityFlags(capabilityFlags), Message(std::move(message)),
			  EncodedCapabilityWord(static_cast<unsigned long long>(capabilityFlags)) {}

		static std::shared_ptr<DmaStreamComputeCapabilityQueryResult> Capture(std::shared_ptr<DmaStreamComputeOwnerBinding> ownerBinding)
		{
			if (!ownerBinding) throw std::invalid_argument("ownerBinding");
			constexpr auto flags = DmaStreamComputeCapabilityFlags::Lane6DmaStreamBackend
				| DmaStreamComputeCapabilityFlags::CurrentDsc1Production
				| DmaStreamComputeCapabilityFlags::DscStatusQuery
				| DmaStreamComputeCapabilityFlags::Dsc2ParserFootprintFoundation
				| DmaStreamComputeCapabilityFlags::DscQueryCaps;
			return std::make_shared<DmaStreamComputeCapabilityQueryResult>(std::move(ownerBinding), flags,
				L"DSC_QUERY_CAPS captured the current lane6 DMA/stream capability word; execution authority, token issue, memory publication, and compiler lowering are unchanged.");
		}
	};

	struct DmaStreamComputeCapabilityQueryReplayEvidence final
	{
		unsigned int Opcode = 0;
		std::shared_ptr<DmaStreamComputeOwnerBinding> OwnerBinding;
		unsigned long long EncodedCapabilityWord = 0;
		bool OwnerBindingRevalidated = false;
		DmaStreamComputeLanePlacementEvidence LanePlacementEvidence{};
		unsigned long long EvidenceHash = 0;

		bool IsComplete() const noexcept
		{
			return Opcode == Processor::CPU_Core::IsaOpcodeValues::DSC_QUERY_CAPS
				&& EncodedCapabilityWord != 0 && OwnerBindingRevalidated
				&& LanePlacementEvidence.IsComplete() && EvidenceHash != 0;
		}

		static std::shared_ptr<DmaStreamComputeCapabilityQueryReplayEvidence> Capture(
			std::shared_ptr<DmaStreamComputeOwnerBinding> ownerBinding,
			std::shared_ptr<DmaStreamComputeCapabilityQueryResult> result,
			DmaStreamComputeLanePlacementEvidence lanePlacementEvidence)
		{
			if (!ownerBinding) throw std::invalid_argument("ownerBinding");
			if (!result) throw std::invalid_argument("result");
			auto evidence = std::make_shared<DmaStreamComputeCapabilityQueryReplayEvidence>();
			evidence->Opcode = Processor::CPU_Core::IsaOpcodeValues::DSC_QUERY_CAPS;
			evidence->OwnerBinding = std::move(ownerBinding);
			evidence->EncodedCapabilityWord = result->EncodedCapabilityWord;
			evidence->OwnerBindingRevalidated = result->OwnerBinding
				&& result->OwnerBinding->OwnerVirtualThreadId == evidence->OwnerBinding->OwnerVirtualThreadId
				&& result->OwnerBinding->OwnerContextId == evidence->OwnerBinding->OwnerContextId
				&& result->OwnerBinding->OwnerCoreId == evidence->OwnerBinding->OwnerCoreId
				&& result->OwnerBinding->OwnerPodId == evidence->OwnerBinding->OwnerPodId
				&& result->OwnerBinding->OwnerDomainTag == evidence->OwnerBinding->OwnerDomainTag
				&& result->OwnerBinding->DeviceId == DmaStreamComputeDescriptor::CanonicalLane6DeviceId;
			evidence->LanePlacementEvidence = lanePlacementEvidence;

			unsigned long long hash = 14695981039346656037ULL;
			auto add = [&hash](unsigned long long value) { hash ^= value; hash *= 1099511628211ULL; };
			add(evidence->Opcode);
			add(evidence->EncodedCapabilityWord);
			add(evidence->OwnerBinding->OwnerVirtualThreadId);
			add(evidence->OwnerBinding->OwnerContextId);
			add(evidence->OwnerBinding->OwnerCoreId);
			add(evidence->OwnerBinding->OwnerPodId);
			add(evidence->OwnerBinding->OwnerDomainTag);
			add(evidence->OwnerBinding->DeviceId);
			add(evidence->OwnerBindingRevalidated ? 1ULL : 0ULL);
			add(evidence->LanePlacementEvidence.EvidenceHash);
			evidence->EvidenceHash = hash == 0 ? 0xD5C007A0ULL : hash;
			return evidence;
		}
	};
}
