#pragma once

namespace YAKSys_Hybrid_CPU::Core::Decoder { class CanonicalScalarLoadAddressPlan; }
#include "ExecutionPlacement.h"
#include "ExecutionBindingIdentity.h"
#include "OperationAttemptIssuer.h"
#include "LegacyAdmissionProjection.h"
#include "FreezeRegisters.h"
#include "MemoryCapability.h"
namespace YAKSys_Hybrid_CPU::Core::Decoder { class VliwOperationId; class AdmissionRecord; }
#include "CanonicalDecodedContracts.h"
#include "SemanticInstructionKey.h"
#include "../../../NonRTL/Arch/InstructionClass.h"
#include "../../../NonRTL/Arch/SerializationClass.h"
#include "../Pipeline/Safety/ResourceBitset.h"
#include <string>
#include <vector>
#include <stdexcept>
#include "tangible_exception_helper.h"
#include <memory>
#include "../../../tangible_string_helper.h"


using namespace YAKSys_Hybrid_CPU::Arch;



namespace YAKSys_Hybrid_CPU::Core::Decoder
{

	/// <summary>
	/// Immutable typed identity for a generated materializer binding.
	/// </summary>
// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct MaterializerId
//	{
//		public MaterializerId(string value)
//		{
//			Value = RequireIdentity(value, nameof(value));
//		}
//
//		public string Value
//		{
//			get;
//		}
//
//		public override string ToString()
//		{
//			return Value;
//		}
//
//		private static string RequireIdentity(string value, string parameterName)
//		{
//			ArgumentException.ThrowIfNullOrWhiteSpace(value, parameterName);
//			return value.Trim();
//		}
//	}

	/// <summary>
	/// Immutable typed identity for a runtime execution provider binding.
	/// </summary>
// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct RuntimeExecutionProviderId
//	{
//		public RuntimeExecutionProviderId(string value)
//		{
//			Value = RequireIdentity(value, nameof(value));
//		}
//
//		public string Value
//		{
//			get;
//		}
//
//		public override string ToString()
//		{
//			return Value;
//		}
//
//		private static string RequireIdentity(string value, string parameterName)
//		{
//			ArgumentException.ThrowIfNullOrWhiteSpace(value, parameterName);
//			return value.Trim();
//		}
//	}

	/// <summary>
	/// Immutable typed identity for a latency/completion model. RF-06 does not own
	/// observed completion or mutable latency state.
	/// </summary>
// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct LatencyModelId
//	{
//		public LatencyModelId(string value)
//		{
//			Value = RequireIdentity(value, nameof(value));
//		}
//
//		public string Value
//		{
//			get;
//		}
//
//		public override string ToString()
//		{
//			return Value;
//		}
//
//		private static string RequireIdentity(string value, string parameterName)
//		{
//			ArgumentException.ThrowIfNullOrWhiteSpace(value, parameterName);
//			return value.Trim();
//		}
//	}

	/// <summary>
	/// The one generated static binding handed from decode to materialization.
	/// It contains no provider instance and no live execution state.
	/// </summary>
// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public sealed record GeneratedStaticBinding(uint Opcode, MaterializerId MaterializerId, RuntimeExecutionProviderId RuntimeExecutionProviderId, LatencyModelId LatencyModelId, string CatalogVersion, string CatalogSha256, string DescriptorFingerprint)
//	{
//		public static GeneratedStaticBinding FromDescriptor(in GeneratedIsaDescriptor descriptor)
//		{
//			string catalogVersion = GeneratedIsaCatalog.CatalogVersion;
//			string catalogSha256 = GeneratedIsaCatalog.CatalogSha256;
//			string descriptorFingerprint = ComputeDescriptorFingerprint(in descriptor);
//
//			return new GeneratedStaticBinding(descriptor.Opcode, new MaterializerId(descriptor.MaterializerId), new RuntimeExecutionProviderId(descriptor.ProviderId), new LatencyModelId(descriptor.LatencyModelId), catalogVersion, catalogSha256, descriptorFingerprint);
//		}
//
//		public static bool TryFromOpcode(uint opcode, out GeneratedStaticBinding binding)
//		{
//			if (GeneratedIsaCatalog.TryGetDescriptor(opcode, out GeneratedIsaDescriptor descriptor))
//			{
//				binding = FromDescriptor(in descriptor);
//				return true;
//			}
//
//// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
//// ORIGINAL LINE: binding = default!;
//			binding = default;
//			return false;
//		}
//
//		private static string ComputeDescriptorFingerprint(in GeneratedIsaDescriptor descriptor)
//		{
//			byte[] payload = JsonSerializer.SerializeToUtf8Bytes(descriptor);
//			return Convert.ToHexString(SHA256.HashData(payload)).ToLowerInvariant();
//		}
//	}

	/// <summary>Frozen physical placement capability. It is not a selected lane.</summary>
// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct ExecutionPlacement
//	{
//		private ExecutionPlacement(SlotClass requiredSlotClass, SlotPinningKind pinningKind, byte pinnedLaneId, ulong domainTag)
//		{
//			RequiredSlotClass = requiredSlotClass;
//			PinningKind = pinningKind;
//			PinnedLaneId = pinnedLaneId;
//			DomainTag = domainTag;
//		}
//
//		public SlotClass RequiredSlotClass
//		{
//			get;
//		}
//		public SlotPinningKind PinningKind
//		{
//			get;
//		}
//		public byte PinnedLaneId
//		{
//			get;
//		}
//		public ulong DomainTag
//		{
//			get;
//		}
//
//		/// <summary>
//		/// Constructs a valid hard-pinned placement from the checked physical-lane
//		/// representation. The retained placement carrier remains a raw byte.
//		/// </summary>
//		public static ExecutionPlacement CreateHardPinned(SlotClass requiredSlotClass, LaneId laneId, ulong domainTag = 0)
//		{
//			return new(requiredSlotClass, SlotPinningKind.HardPinned, laneId.ToRawValue(), domainTag);
//		}
//
//		/// <summary>
//		/// Projects a hard-pinned raw placement to a checked physical lane. A
//		/// flexible placement has no pinned lane; this does not decide placement
//		/// legality or scheduler admission.
//		/// </summary>
//		public bool TryGetHardPinnedLaneId(out LaneId laneId)
//		{
//			if (PinningKind == SlotPinningKind.HardPinned && LaneId.TryCreate(PinnedLaneId, out laneId))
//			{
//				return true;
//			}
//
//			laneId = default;
//			return false;
//		}
//
//		public static ExecutionPlacement Create(SlotClass requiredSlotClass, SlotPinningKind pinningKind, byte pinnedLaneId = 0, ulong domainTag = 0)
//		{
//			if (pinningKind == SlotPinningKind.HardPinned && pinnedLaneId >= BundleMetadata.BundleSlotCount)
//			{
//				throw new ArgumentOutOfRangeException(nameof(pinnedLaneId), "A hard-pinned lane must be in the physical eight-lane bundle.");
//			}
//
//			return new ExecutionPlacement(requiredSlotClass, pinningKind, pinningKind == SlotPinningKind.HardPinned ? pinnedLaneId : (byte)0, domainTag);
//		}
//	}



	/// <summary>
	/// Typed identity of a scheduler-visible memory bank. The current hardware
	/// contract exposes sixteen banks; unresolved legacy values (for example -1)
	/// are deliberately not representable here.
	/// </summary>
// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct MemoryBankId
//	{
//		public const int BankCount = 16;
//
//		public MemoryBankId(int value)
//		{
//			if ((uint)value >= BankCount)
//			{
//				throw new ArgumentOutOfRangeException(nameof(value), value, string.Format("A memory bank identity must be in the range [0, {0}].", BankCount - 1));
//			}
//
//			Value = value;
//		}
//
//		public int Value
//		{
//			get;
//		}
//
//		public override string ToString()
//		{
//			return Value.ToString();
//		}
//	}

	/// <summary>
	/// Static memory direction carried by a capability. This is not a request
	/// state or a completion state; it describes only the access contract.
	/// </summary>
// C# TO C++ CONVERTER NOTE: The following .NET attribute has no direct equivalent in C++:
// ORIGINAL LINE: [Flags] public enum MemoryAccessDirection : byte


// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct FrozenMemoryRange
//	{
//		public FrozenMemoryRange(ulong address, ulong length)
//		{
//			if (length == 0)
//			{
//				throw new ArgumentOutOfRangeException(nameof(length), "A frozen memory range must have non-zero length.");
//			}
//
//			if (length - 1 > ulong.MaxValue - address)
//			{
//				throw new ArgumentOutOfRangeException(nameof(length), "A frozen memory range must remain within the address space.");
//			}
//
//			Address = address;
//			Length = length;
//		}
//
//		public ulong Address
//		{
//			get;
//		}
//		public ulong Length
//		{
//			get;
//		}
//		public ulong LastAddress
//		{
//			get
//			{
//				return Address + Length - 1;
//			}
//		}
//	}

	/// <summary>
	/// Coherent memory capability. Mutable timing, MSHR and allocation state are
	/// deliberately absent.
	/// </summary>
// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public sealed record MemoryCapability
//	{
//		private MemoryCapability(MemoryCapabilityKind kind, MemoryAccessDirection direction, System.Nullable<MemoryBankId> bank, ImmutableArray<FrozenMemoryRange> footprint)
//		{
//			Kind = kind;
//			Direction = direction;
//			Bank = bank;
//			Footprint = footprint;
//		}
//
//		public MemoryCapabilityKind Kind
//		{
//			get;
//		}
//		public MemoryAccessDirection Direction
//		{
//			get;
//		}
//		public System.Nullable<MemoryBankId> Bank
//		{
//			get;
//		}
//		public System.Nullable<MemoryBankId> BankId
//		{
//			get
//			{
//				return Bank;
//			}
//		}
//		public ImmutableArray<FrozenMemoryRange> Footprint
//		{
//			get;
//		}
//
//		public static MemoryCapability None
//		{
//			get;
//		} = new(MemoryCapabilityKind.NoMemory, MemoryAccessDirection.None, bank: nullptr, ImmutableArray<FrozenMemoryRange>.Empty);
//
//		public static MemoryCapability Create(MemoryCapabilityKind kind, System.Nullable<IEnumerable<FrozenMemoryRange>> footprint = nullptr, System.Nullable<MemoryBankId> bank = nullptr, System.Nullable<MemoryAccessDirection> direction = nullptr)
//		{
//// C# TO C++ CONVERTER TASK: The following 'switch expression' was not converted by C# to C++ Converter:
////			MemoryAccessDirection expectedDirection = kind switch
////			{
////				MemoryCapabilityKind.NoMemory => MemoryAccessDirection.None,
////				MemoryCapabilityKind.Load => MemoryAccessDirection.Read,
////				MemoryCapabilityKind.Store => MemoryAccessDirection.Write,
////				MemoryCapabilityKind.Atomic => MemoryAccessDirection.ReadWrite,
////				_ => throw new ArgumentOutOfRangeException(nameof(kind), kind, "Unknown memory capability kind."),
////			};
//
//			if (kind == MemoryCapabilityKind.NoMemory && footprint != nullptr && footprint.Any())
//			{
//				throw new ArgumentException("NoMemory cannot carry a memory footprint.", nameof(footprint));
//			}
//
//			if (kind == MemoryCapabilityKind.NoMemory && bank.HasValue)
//			{
//				throw new ArgumentException("NoMemory cannot carry a memory bank identity.", nameof(bank));
//			}
//
//			if (kind == MemoryCapabilityKind.NoMemory && direction.HasValue && direction.Value != MemoryAccessDirection.None)
//			{
//				throw new ArgumentException("NoMemory cannot carry a read or write direction.", nameof(direction));
//			}
//
//			if (kind != MemoryCapabilityKind.NoMemory && footprint == nullptr)
//			{
//				throw new ArgumentNullException(nameof(footprint), "A memory capability must declare its frozen footprint.");
//			}
//
//			if (kind != MemoryCapabilityKind.NoMemory && !bank.HasValue)
//			{
//				throw new ArgumentNullException(nameof(bank), "A memory capability must declare its typed bank identity.");
//			}
//
//			if (direction.HasValue && direction.Value != expectedDirection)
//			{
//				throw new ArgumentException(string.Format("Memory capability kind {0} requires direction {1}.", kind, expectedDirection), nameof(direction));
//			}
//
//			ImmutableArray<FrozenMemoryRange> frozen = footprint == nullptr ? ImmutableArray<FrozenMemoryRange>.Empty : NormalizeFootprint(footprint);
//			if (kind != MemoryCapabilityKind.NoMemory && frozen.IsDefaultOrEmpty)
//			{
//				throw new ArgumentException("A memory capability must carry at least one frozen range.", nameof(footprint));
//			}
//
//			return new MemoryCapability(kind, expectedDirection, bank, frozen);
//		}
//
//		private static ImmutableArray<FrozenMemoryRange> NormalizeFootprint(IEnumerable<FrozenMemoryRange> footprint)
//		{
//			FrozenMemoryRange[] ordered = footprint.OrderBy(range => range.Address).ThenBy(range => range.Length).ToArray();
//
//			for (int index = 1; index < ordered.Length; index++)
//			{
//				FrozenMemoryRange previous = ordered[index - 1];
//				FrozenMemoryRange current = ordered[index];
//				if (current.Address <= previous.LastAddress)
//				{
//					throw new ArgumentException("A frozen memory capability footprint cannot contain overlapping ranges.", nameof(footprint));
//				}
//			}
//
//			return ImmutableArray.Create(ordered);
//		}
//	}

	/// <summary>
	/// Immutable companion for a canonical unresolved scalar-load shape. This is
	/// deliberately distinct from <see cref="MemoryCapability"/>: it has no
	/// effective address, bank, footprint or live request/completion state.
	/// </summary>
	class StaticMemoryAccessPlan final : public std::enable_shared_from_this<StaticMemoryAccessPlan>
	{
	private:
		const std::shared_ptr<CanonicalScalarLoadAddressPlan> ScalarLoadPlan;

		StaticMemoryAccessPlan(std::shared_ptr<CanonicalScalarLoadAddressPlan> scalarLoadPlan);

	public:
		std::shared_ptr<CanonicalScalarLoadAddressPlan> getScalarLoadPlan() const;
		std::shared_ptr<GeneratedStaticBinding> getGeneratedBinding() const;
		MemoryAccessDirection getDirection() const;

		static std::shared_ptr<StaticMemoryAccessPlan> UnresolvedScalarLoad(std::shared_ptr<CanonicalScalarLoadAddressPlan> scalarLoadPlan);
	};

	/// <summary>Resolved provider identity carried by the immutable contract.</summary>
// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public sealed record RuntimeExecutionProviderBinding
//	{
//		public RuntimeExecutionProviderBinding(RuntimeExecutionProviderId id, string payloadSchema)
//		{
//			Id = id;
//			PayloadSchema = RequireSchema(payloadSchema);
//		}
//
//		public RuntimeExecutionProviderId Id
//		{
//			get;
//		}
//		public string PayloadSchema
//		{
//			get;
//		}
//
//		private static string RequireSchema(string value)
//		{
//			ArgumentException.ThrowIfNullOrWhiteSpace(value);
//			return value.Trim();
//		}
//	}

	/// <summary>
	/// Immutable materialization/capability envelope. RF-07 owns all live outcome
	/// and completion semantics, so this type intentionally has no mutable runtime state.
	/// </summary>
	class ExecutionContract final : public std::enable_shared_from_this<ExecutionContract>
	{
	private:
		const std::shared_ptr<GeneratedStaticBinding> GeneratedBinding;
		const std::shared_ptr<RuntimeExecutionProviderBinding> RuntimeProvider;
		const YAKSys_Hybrid_CPU::Arch::InstructionClass InstructionClass;
		const YAKSys_Hybrid_CPU::Arch::SerializationClass SerializationClass;
		const std::shared_ptr<ExecutionPlacement> Placement;
		const std::wstring StaticEffectContract;
		const std::shared_ptr<MemoryCapability> Memory;
		const std::shared_ptr<StaticMemoryAccessPlan> StaticMemoryPlan;
		const std::vector<int> ReadRegisters;
		const std::vector<int> WriteRegisters;
		const ResourceBitset ResourceMask;
		const bool IsStealable;
		const bool IsRetireVisible;
		const bool IsAssist;

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private ExecutionContract(GeneratedStaticBinding generatedBinding, RuntimeExecutionProviderBinding runtimeProvider, InstructionClass instructionClass, SerializationClass serializationClass, ExecutionPlacement placement, string staticEffectContract, MemoryCapability memory, StaticMemoryAccessPlan? staticMemoryPlan, std::vector<int> readRegisters, std::vector<int> writeRegisters, ResourceBitset resourceMask, bool isStealable, bool isRetireVisible, bool isAssist)
		ExecutionContract(std::shared_ptr<GeneratedStaticBinding> generatedBinding, std::shared_ptr<RuntimeExecutionProviderBinding> runtimeProvider, YAKSys_Hybrid_CPU::Arch::InstructionClass instructionClass, YAKSys_Hybrid_CPU::Arch::SerializationClass serializationClass, std::shared_ptr<ExecutionPlacement> placement, const std::wstring &staticEffectContract, std::shared_ptr<MemoryCapability> memory, std::shared_ptr<StaticMemoryAccessPlan> staticMemoryPlan, std::vector<int> readRegisters, std::vector<int> writeRegisters, ResourceBitset resourceMask, bool isStealable, bool isRetireVisible, bool isAssist);

	public:
		std::shared_ptr<GeneratedStaticBinding> getGeneratedBinding() const;
		std::shared_ptr<RuntimeExecutionProviderBinding> getRuntimeProvider() const;
		YAKSys_Hybrid_CPU::Arch::InstructionClass getInstructionClass() const;
		YAKSys_Hybrid_CPU::Arch::SerializationClass getSerializationClass() const;
		std::shared_ptr<ExecutionPlacement> getPlacement() const;
		std::wstring getStaticEffectContract() const;
		std::shared_ptr<MemoryCapability> getMemory() const;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public StaticMemoryAccessPlan? StaticMemoryPlan {get;}
		std::shared_ptr<StaticMemoryAccessPlan> getStaticMemoryPlan() const;
		std::vector<int> getReadRegisters() const;
		std::vector<int> getWriteRegisters() const;
		ResourceBitset getResourceMask() const;
		bool isStealable() const;
		bool isRetireVisible() const;
		bool isAssist() const;

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public static ExecutionContract Create(GeneratedStaticBinding generatedBinding, RuntimeExecutionProviderBinding runtimeProvider, InstructionClass instructionClass, SerializationClass serializationClass, ExecutionPlacement placement, string staticEffectContract, MemoryCapability memory, IEnumerable<int>? readRegisters = null, IEnumerable<int>? writeRegisters = null, ResourceBitset resourceMask = default, bool isStealable = true, bool isRetireVisible = true, bool isAssist = false, System.Nullable<StaticMemoryAccessPlan> staticMemoryPlan = null)
		static std::shared_ptr<ExecutionContract> Create(std::shared_ptr<GeneratedStaticBinding> generatedBinding, std::shared_ptr<RuntimeExecutionProviderBinding> runtimeProvider, YAKSys_Hybrid_CPU::Arch::InstructionClass instructionClass, YAKSys_Hybrid_CPU::Arch::SerializationClass serializationClass, std::shared_ptr<ExecutionPlacement> placement, const std::wstring &staticEffectContract, std::shared_ptr<MemoryCapability> memory, std::vector<int> readRegisters = {}, std::vector<int> writeRegisters = {}, ResourceBitset resourceMask = YAKSys_Hybrid_CPU::Core::ResourceBitset(), bool isStealable = true, bool isRetireVisible = true, bool isAssist = false, std::shared_ptr<StaticMemoryAccessPlan> staticMemoryPlan = nullptr);

	private:
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private static std::vector<int> FreezeRegisters(IEnumerable<int>? registers, string parameterName)
		static std::vector<int> FreezeRegisters(const std::vector<int> &registers, const std::wstring &parameterName);

		static bool IsScalarLoad(unsigned int opcode);

		static bool IsMatrixTile(unsigned int opcode);

		static bool IsAcceleratorCommand(unsigned int opcode);
	};

	/// <summary>Source/fetch provenance, separate from working retirement identity.</summary>
// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public sealed record SourceOperationProvenance
//	{
//		[JsonConstructor] public SourceOperationProvenance(SemanticInstructionKey semanticKey, int sourceVirtualThreadId, ulong sourceBundleSerial, int sourceSlotIndex, ulong fetchEpoch) : this(semanticKey, sourceVirtualThreadId, sourceBundleSerial, ValidateRawSourceSlotIndex(sourceSlotIndex), fetchEpoch)
//		{
//		}
//
//		public SourceOperationProvenance(SemanticInstructionKey semanticKey, int sourceVirtualThreadId, ulong sourceBundleSerial, SlotId sourceSlotId, ulong fetchEpoch)
//		{
//			if (sourceVirtualThreadId < 0)
//			{
//				throw new ArgumentOutOfRangeException(nameof(sourceVirtualThreadId));
//			}
//
//			SemanticKey = semanticKey;
//			SourceVirtualThreadId = sourceVirtualThreadId;
//			SourceBundleSerial = sourceBundleSerial;
//			SourceSlotId = sourceSlotId;
//			FetchEpoch = fetchEpoch;
//		}
//
//		public SemanticInstructionKey SemanticKey
//		{
//			get;
//		}
//		public int SourceVirtualThreadId
//		{
//			get;
//		}
//		public ulong SourceBundleSerial
//		{
//			get;
//		}
//		[JsonIgnore] public SlotId SourceSlotId
//		{
//			get;
//		}
//		public int SourceSlotIndex
//		{
//			get
//			{
//				return SourceSlotId;
//			}
//		}
//		public ulong FetchEpoch
//		{
//			get;
//		}
//
//		private static SlotId ValidateRawSourceSlotIndex(int sourceSlotIndex)
//		{
//			if ((uint)sourceSlotIndex >= BundleMetadata.BundleSlotCount)
//			{
//				throw new ArgumentOutOfRangeException(nameof(sourceSlotIndex));
//			}
//
//			return SlotId.Create(sourceSlotIndex);
//		}
//	}

	class SourceOperationProvenance final
	{
	public:
		SemanticInstructionKey SemanticKey;
		int SourceVirtualThreadId;
		unsigned long long SourceBundleSerial;
		SlotId SourceSlotId;
		int SourceSlotIndex;
		unsigned long long FetchEpoch;
		SourceOperationProvenance(SemanticInstructionKey key, int vt, unsigned long long bundle, SlotId slot, unsigned long long epoch)
			: SemanticKey(std::move(key)), SourceVirtualThreadId(vt), SourceBundleSerial(bundle), SourceSlotId(slot), SourceSlotIndex(slot.getValue()), FetchEpoch(epoch)
		{ if (vt < 0) throw std::out_of_range("sourceVirtualThreadId"); }
	};

	class AdmissionRecord final
	{
	public:
		std::shared_ptr<SourceOperationProvenance> SourceProvenance;
		std::shared_ptr<ExecutionContract> Contract;
		int VirtualThreadId = 0, OwnerContextId = 0;
		unsigned long long DomainTag = 0;
		std::vector<int> ReadRegisters, WriteRegisters;
		bool IsStealable = true, IsRetireVisible = true;
		ResourceBitset ResourceMask;
		static std::shared_ptr<AdmissionRecord> Create(std::shared_ptr<SourceOperationProvenance> provenance, std::shared_ptr<ExecutionContract> contract, int vt, int owner, unsigned long long domainTag)
		{
			if (!provenance || !contract || vt < 0) throw std::invalid_argument("invalid admission record");
			auto result = std::make_shared<AdmissionRecord>();
			result->SourceProvenance = std::move(provenance); result->Contract = std::move(contract);
			result->VirtualThreadId = vt; result->OwnerContextId = owner; result->DomainTag = domainTag;
			result->ReadRegisters = result->Contract->getReadRegisters(); result->WriteRegisters = result->Contract->getWriteRegisters();
			result->IsStealable = result->Contract->isStealable(); result->IsRetireVisible = result->Contract->isRetireVisible(); result->ResourceMask = result->Contract->getResourceMask();
			return result;
		}
	};

	/// <summary>Frozen scheduler-facing owner/domain and hazard facts.</summary>
// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public sealed record AdmissionRecord
//	{
//		private AdmissionRecord(SourceOperationProvenance sourceProvenance, ExecutionContract executionContract, int virtualThreadId, int ownerContextId, ulong domainTag, std::vector<int> readRegisters, std::vector<int> writeRegisters, bool isStealable, bool isRetireVisible, ResourceBitset resourceMask)
//		{
//			SourceProvenance = sourceProvenance;
//			ExecutionContract = executionContract;
//			VirtualThreadId = virtualThreadId;
//			OwnerContextId = ownerContextId;
//			DomainTag = domainTag;
//			ReadRegisters = readRegisters;
//			WriteRegisters = writeRegisters;
//			IsStealable = isStealable;
//			IsRetireVisible = isRetireVisible;
//			ResourceMask = resourceMask;
//		}
//
//		public SourceOperationProvenance SourceProvenance
//		{
//			get;
//		}
//		public ExecutionContract ExecutionContract
//		{
//			get;
//		}
//		public int VirtualThreadId
//		{
//			get;
//		}
//		public int OwnerContextId
//		{
//			get;
//		}
//		public ulong DomainTag
//		{
//			get;
//		}
//		public std::vector<int> ReadRegisters
//		{
//			get;
//		}
//		public std::vector<int> WriteRegisters
//		{
//			get;
//		}
//		public bool IsStealable
//		{
//			get;
//		}
//		public bool IsRetireVisible
//		{
//			get;
//		}
//		public ResourceBitset ResourceMask
//		{
//			get;
//		}
//
//		public static AdmissionRecord Create(SourceOperationProvenance sourceProvenance, ExecutionContract executionContract, int virtualThreadId, int ownerContextId, ulong domainTag, System.Nullable<IEnumerable<int>> readRegisters = nullptr, System.Nullable<IEnumerable<int>> writeRegisters = nullptr, System.Nullable<bool> isStealable = nullptr, System.Nullable<bool> isRetireVisible = nullptr, System.Nullable<ResourceBitset> resourceMask = nullptr)
//		{
//			ArgumentNullException.ThrowIfNull(sourceProvenance);
//			ArgumentNullException.ThrowIfNull(executionContract);
//			if (virtualThreadId < 0)
//			{
//				throw new ArgumentOutOfRangeException(nameof(virtualThreadId));
//			}
//
//			return new AdmissionRecord(sourceProvenance, executionContract, virtualThreadId, ownerContextId, domainTag, readRegisters == nullptr ? executionContract.ReadRegisters : readRegisters.ToImmutableArray(), writeRegisters == nullptr ? executionContract.WriteRegisters : writeRegisters.ToImmutableArray(), isStealable ?? executionContract.IsStealable, isRetireVisible ?? executionContract.IsRetireVisible, resourceMask ?? executionContract.ResourceMask);
//		}
//
//		/// <summary>
//		/// Compares only scheduler policy facts. Source provenance and semantic cache
//		/// identity are intentionally excluded from this equivalence relation.
//		/// </summary>
//		public bool HasEquivalentAdmissionFacts(AdmissionRecord other)
//		{
//			ArgumentNullException.ThrowIfNull(other);
//			return ExecutionContract == other.ExecutionContract && VirtualThreadId == other.VirtualThreadId && OwnerContextId == other.OwnerContextId && DomainTag == other.DomainTag && ReadRegisters.SequenceEqual(other.ReadRegisters) && WriteRegisters.SequenceEqual(other.WriteRegisters) && IsStealable == other.IsStealable && IsRetireVisible == other.IsRetireVisible && ResourceMask == other.ResourceMask;
//		}
//	}

	/// <summary>Frozen projection facts used to detect stale legacy admission carriers.</summary>
// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public sealed record LegacyAdmissionProjection(int VirtualThreadId, int OwnerContextId, ulong DomainTag, bool IsStealable, std::vector<int> ReadRegisters, std::vector<int> WriteRegisters, ExecutionPlacement Placement, ResourceBitset ResourceMask);

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct AdmissionProjectionFingerprint
//	{
//		public AdmissionProjectionFingerprint(string value)
//		{
//			ArgumentException.ThrowIfNullOrWhiteSpace(value);
//			Value = value.Trim();
//		}
//
//		public string Value
//		{
//			get;
//		}
//	}



	/// <summary>
	/// Identity of one successful Stage-B materialization/issue attempt.
	/// Construction is private and reachable only through ScheduledOperation.
	/// </summary>
class VliwOperationId final
{
private:
	VliwOperationId(int virtualThreadId, unsigned long long workingBundleSequence, int workingSlotIndex, unsigned long long operationAttempt)
		: VirtualThreadId(virtualThreadId), WorkingBundleSequence(workingBundleSequence), WorkingSlotIndex(workingSlotIndex), OperationAttempt(operationAttempt) {}
	static std::shared_ptr<VliwOperationId> Issue(int virtualThreadId, unsigned long long workingBundleSequence, int workingSlotIndex, unsigned long long operationAttempt)
	{
		return std::shared_ptr<VliwOperationId>(new VliwOperationId(virtualThreadId, workingBundleSequence, workingSlotIndex, operationAttempt));
	}
	friend class ScheduledOperation;
public:
	const int VirtualThreadId;
	const unsigned long long WorkingBundleSequence;
	const int WorkingSlotIndex;
	const unsigned long long OperationAttempt;
};

	/// <summary>Non-cached issuer used only after successful Stage-B validation.</summary>


	/// <summary>
	/// Issued operation created only after a physical lane has been selected.
	/// It carries the already-resolved provider binding through the contract.
	/// </summary>
	class ScheduledOperation final : public std::enable_shared_from_this<ScheduledOperation>
	{
	private:
		const std::shared_ptr<VliwOperationId> OperationId;
		const std::shared_ptr<AdmissionRecord> Admission;
		const int PhysicalLane;

		ScheduledOperation(std::shared_ptr<VliwOperationId> operationId, std::shared_ptr<AdmissionRecord> admission, int physicalLane);

	public:
		std::shared_ptr<VliwOperationId> getOperationId() const;
		std::shared_ptr<AdmissionRecord> getAdmission() const;
		int getPhysicalLane() const;
		std::shared_ptr<RuntimeExecutionProviderBinding> getRuntimeProvider() const;

		static std::shared_ptr<ScheduledOperation> CreateAfterStageB(std::shared_ptr<AdmissionRecord> admission, unsigned long long workingBundleSequence, int workingSlotIndex, int physicalLane, std::shared_ptr<OperationAttemptIssuer> attemptIssuer);
	};

}
