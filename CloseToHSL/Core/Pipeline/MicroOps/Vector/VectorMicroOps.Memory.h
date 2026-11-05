#pragma once

#include "../../../Execution/StreamEngine/AddressGeneration/StreamEngine.AddressGen.h"
#include "VectorMicroOps.h"
#include "../../../../Memory/Timing/MemoryCycleController.h"
#include "../../../Architecture/Common/YAKSys_Hybrid_CPU.Processor.h"
namespace YAKSys_Hybrid_CPU::Memory { class MemorySubsystem; class MemoryRequestToken; }
#include "../../../Architecture/Registers/Retire/RetireRecord.h"
#include <string>
#include <vector>
#include <limits>
#include <stdexcept>
#include <any>
#include <optional>
#include <format>
#include "tangible_exception_helper.h"
#include <memory>

// C# TO C++ CONVERTER NOTE: Forward class declarations:
namespace YAKSys_Hybrid_CPU::Core { class Indexed2SrcDescriptor; }
namespace YAKSys_Hybrid_CPU::Core { class StagedScatterWrite; }

using namespace HybridCPU_ISE::CloseToHSL::Memory::Timing;
using namespace YAKSys_Hybrid_CPU::Arch;
using namespace YAKSys_Hybrid_CPU::Core::Registers::Retire;
using namespace YAKSys_Hybrid_CPU::Execution;
using namespace YAKSys_Hybrid_CPU::Memory;


namespace YAKSys_Hybrid_CPU::Core
{
	class LoadSegmentMicroOp : public VectorMicroOp
	{
	private:
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private MemoryCycleController? _requestController;
		std::shared_ptr<MemoryCycleController> _requestController;
		std::optional<std::shared_ptr<MemoryRequestId>> _controllerRequestId;
		bool _controllerAdmissionBackpressured = false;
		std::shared_ptr<std::vector<unsigned char>> _loadBuffer;

		// RF-07.2f/RF-10.6: exact accepted-request evidence for the false wait.
		// Controller/address/size qualification prevents unrelated vector work
		// from borrowing this retry disposition.
	public:
		bool getOwnsPendingMemoryCompletion() const;

		bool getHasControllerAdmissionBackpressure() const;

		LoadSegmentMicroOp();

		void InitializeMetadata() override;

		bool Execute(std::shared_ptr<Processor::CPU_Core> &core) override;

		/// <summary>
		/// Get loaded buffer for consumption by downstream MicroOps
		/// </summary>
		std::shared_ptr<std::vector<unsigned char>> GetLoadedBuffer();

		bool CancelPendingControllerRequest();

	private:
		void ClearControllerRequestState(bool resetExecutionState);

	protected:
		std::shared_ptr<LoadSegmentMicroOp> shared_from_this()
		{
			return std::static_pointer_cast<LoadSegmentMicroOp>(VectorMicroOp::shared_from_this());
		}
	};

	/// <summary>
	/// Load 2D micro-operation for 2D vector loads.
	/// Uses BurstPlanner.Plan2D for complex addressing patterns.
	/// </summary>
	class Load2DMicroOp : public VectorMicroOp
	{
	private:
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private YAKSys_Hybrid_CPU.Memory.MemorySubsystem.MemoryRequestToken? _requestToken;
		std::shared_ptr<YAKSys_Hybrid_CPU::Memory::MemoryRequestToken> _requestToken;
		std::shared_ptr<std::vector<unsigned char>> _loadBuffer;

	public:
		Load2DMicroOp();

		void InitializeMetadata() override;

		bool Execute(std::shared_ptr<Processor::CPU_Core> &core) override;

		std::shared_ptr<std::vector<unsigned char>> GetLoadedBuffer();

	protected:
		std::shared_ptr<Load2DMicroOp> shared_from_this()
		{
			return std::static_pointer_cast<Load2DMicroOp>(VectorMicroOp::shared_from_this());
		}
	};

	/// <summary>
	/// VGATHER indexed-memory read micro-operation.
	/// Reads Indexed2SrcDesc from Src2Pointer, stages gathered elements during execute,
	/// and publishes the destination memory update only through WB/retire follow-through.
	/// </summary>
	class GatherMicroOp : public VectorMicroOp
	{
	private:
		static constexpr int Indexed2SrcDescriptorSize = 32;
		static constexpr int Indexed2SrcReservedOffset = 20;
		static constexpr int Uint32IndexType = 0;
		static constexpr int Uint64IndexType = 1;

		std::shared_ptr<std::vector<unsigned char>> _stagedDestinationBuffer;
		std::shared_ptr<std::vector<unsigned long long>> _indices;
		class Indexed2SrcDescriptor
		{
		private:
			const unsigned long long DescriptorAddress;
			const unsigned long long SourceBase;
			const unsigned long long IndexBase;
			const unsigned short IndexStride;
			const unsigned char IndexType;
			const bool IndexIsByteOffset;

		public:
			Indexed2SrcDescriptor(unsigned long long descriptorAddress, unsigned long long sourceBase, unsigned long long indexBase, unsigned short indexStride, unsigned char indexType, bool indexIsByteOffset);

			unsigned long long getDescriptorAddress() const;

			unsigned long long getSourceBase() const;

			unsigned long long getIndexBase() const;

			unsigned short getIndexStride() const;

			unsigned char getIndexType() const;

			bool getIndexIsByteOffset() const;

			int getIndexElementSize() const;

			unsigned long long getEffectiveIndexStride() const;

			Indexed2SrcDescriptor() = default;
		};

	private:
		std::optional<Indexed2SrcDescriptor> _descriptor;
		bool _descriptorResolved = false;

	public:
		GatherMicroOp();

		void InitializeMetadata() override;

		bool Execute(std::shared_ptr<Processor::CPU_Core> &core) override;

		void EmitWriteBackRetireRecords(std::shared_ptr<Processor::CPU_Core> &core, std::span<RetireRecord> retireRecords, int &retireRecordCount) override;

	private:
		void PublishStagedGatherResultAtWriteBack(std::shared_ptr<Processor::CPU_Core> &core);

		void ThrowIfUnsupportedPublicationContour();

		void ThrowIfUnsupportedRuntimeContour();

		Indexed2SrcDescriptor ResolveDescriptorFromPublishedMemory(const std::wstring &surface);

		static Indexed2SrcDescriptor ReadIndexedDescriptorFromPublishedMemory(unsigned long long descriptorAddress, const std::wstring &surface);

		static Indexed2SrcDescriptor ReadIndexedDescriptor(std::shared_ptr<Processor::CPU_Core> &core, unsigned long long descriptorAddress, const std::wstring &surface);

		static Indexed2SrcDescriptor ParseIndexedDescriptor(std::vector<unsigned char> &descriptorBytes, unsigned long long descriptorAddress, const std::wstring &surface, bool publication);

		[[noreturn]] static void CreateDescriptorException(const std::wstring &surface, const std::wstring &message, bool publication);

		static std::vector<unsigned long long> ReadPublishedIndexVector(Indexed2SrcDescriptor descriptor, unsigned long long elementCount, const std::wstring &surface);

		static unsigned long long ReadIndexElement(std::shared_ptr<Processor::CPU_Core> &core, Indexed2SrcDescriptor descriptor, unsigned long long element, const std::wstring &surface);
		static std::vector<MemoryRange> BuildReadRanges(Indexed2SrcDescriptor descriptor, const std::vector<unsigned long long> &indices, int elemSize, unsigned long long elementCount);
		MicroOp::MemoryRange BuildDestinationRange(int elemSize, unsigned long long elementCount);

// C# TO C++ CONVERTER TASK: Methods returning tuples are not converted by C# to C++ Converter:
//		private static (ulong Address, ulong Length)[] BuildReadRanges(Indexed2SrcDescriptor descriptor, IReadOnlyList<ulong> indices, int elemSize, ulong elementCount)
//		{
//			List<(ulong Address, ulong Length)> ranges = new List<(ulong Address, ulong Length)>(3) { (descriptor.DescriptorAddress, Indexed2SrcDescriptorSize), BuildIndexRange(descriptor, elementCount) };
//			ranges.Add(BuildSourceRange(descriptor, indices, elemSize));
//			return ranges.ToArray();
//		}

// C# TO C++ CONVERTER TASK: Methods returning tuples are not converted by C# to C++ Converter:
//		private static (ulong Address, ulong Length) BuildIndexRange(Indexed2SrcDescriptor descriptor, ulong elementCount)
//		{
//			ulong length = checked(((elementCount - 1) * descriptor.EffectiveIndexStride) + (ulong)descriptor.IndexElementSize);
//			return (descriptor.IndexBase, length);
//		}

// C# TO C++ CONVERTER TASK: Methods returning tuples are not converted by C# to C++ Converter:
//		private static (ulong Address, ulong Length) BuildSourceRange(Indexed2SrcDescriptor descriptor, IReadOnlyList<ulong> indices, int elemSize)
//		{
//			ulong min = ulong.MaxValue;
//			ulong maxExclusive = 0;
//			for (int i = 0; i < indices.Count; i++)
//			{
//				ulong sourceAddress = ResolveSourceAddress(descriptor, indices[i], elemSize, "GatherMicroOp.InitializeMetadata()");
//				min = Math.Min(min, sourceAddress);
//				maxExclusive = Math.Max(maxExclusive, checked(sourceAddress + (ulong)elemSize));
//			}
//
//			return (min, checked(maxExclusive - min));
//		}

// C# TO C++ CONVERTER TASK: Methods returning tuples are not converted by C# to C++ Converter:
//		private (ulong Address, ulong Length) BuildDestinationRange(int elemSize, ulong elementCount)
//		{
//			ulong length = checked(((elementCount - 1) * GetDestinationStride(elemSize)) + (ulong)elemSize);
//			return (Instruction.DestSrc1Pointer, length);
//		}

		static unsigned long long GetIndexAddress(Indexed2SrcDescriptor descriptor, unsigned long long element, const std::wstring &surface);

		unsigned long long GetDestinationAddress(unsigned long long element, int elemSize);

		unsigned long long GetDestinationStride(int elemSize);

		static unsigned long long ResolveSourceAddress(Indexed2SrcDescriptor descriptor, unsigned long long index, int elemSize, const std::wstring &surface);

		bool IsLaneActiveForGather(std::shared_ptr<Processor::CPU_Core> &core, unsigned long long element);

	public:
		std::shared_ptr<std::vector<unsigned char>> GetLoadedBuffer();
		std::shared_ptr<std::vector<unsigned long long>> GetIndices();

	private:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to the C# readonly struct:
// ORIGINAL LINE: private readonly struct Indexed2SrcDescriptor


	protected:
		std::shared_ptr<GatherMicroOp> shared_from_this()
		{
			return std::static_pointer_cast<GatherMicroOp>(VectorMicroOp::shared_from_this());
		}
	};

	/// <summary>
	/// Store segment micro-operation for 1D vector stores.
	/// Writes segment back to memory through EnqueueWrite.
	/// </summary>
	class StoreSegmentMicroOp : public VectorMicroOp
	{
	private:
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private YAKSys_Hybrid_CPU.Memory.MemorySubsystem.MemoryRequestToken? _requestToken;
		std::shared_ptr<YAKSys_Hybrid_CPU::Memory::MemoryRequestToken> _requestToken;
		std::shared_ptr<std::vector<unsigned char>> _storeBuffer;

	public:
		StoreSegmentMicroOp();

		/// <summary>
		/// Set the buffer to be stored
		/// </summary>
		void SetStoreBuffer(std::vector<unsigned char> &buffer);

		void InitializeMetadata() override;

		bool Execute(std::shared_ptr<Processor::CPU_Core> &core) override;

	protected:
		std::shared_ptr<StoreSegmentMicroOp> shared_from_this()
		{
			return std::static_pointer_cast<StoreSegmentMicroOp>(VectorMicroOp::shared_from_this());
		}
	};

	/// <summary>
	/// Store 2D micro-operation for 2D pattern stores.
	/// </summary>
	class Store2DMicroOp : public VectorMicroOp
	{
	private:
		std::shared_ptr<std::vector<unsigned char>> _storeBuffer;

	public:
		Store2DMicroOp();

		void SetStoreBuffer(std::vector<unsigned char> &buffer);

		void InitializeMetadata() override;

		bool Execute(std::shared_ptr<Processor::CPU_Core> &core) override;

	protected:
		std::shared_ptr<Store2DMicroOp> shared_from_this()
		{
			return std::static_pointer_cast<Store2DMicroOp>(VectorMicroOp::shared_from_this());
		}
	};

	/// <summary>
	/// VSCATTER indexed-memory write micro-operation.
	/// Reads source elements from Word1, reads Indexed2SrcDesc from Word2, stages
	/// indexed destination writes during execute, and mutates memory only at WB/retire.
	/// </summary>
	class StoreScatterMicroOp : public VectorMicroOp
	{
	public:
		class StagedScatterWrite;
	private:
		static constexpr int Indexed2SrcDescriptorSize = 32;
		static constexpr int Indexed2SrcReservedOffset = 20;
		static constexpr int Uint32IndexType = 0;
		static constexpr int Uint64IndexType = 1;

		std::shared_ptr<std::vector<StagedScatterWrite>> _stagedWrites;
		int _stagedWriteCount = 0;
		std::shared_ptr<std::vector<unsigned long long>> _indices;
		class Indexed2SrcDescriptor
		{
		private:
			const unsigned long long DescriptorAddress;
			const unsigned long long MemoryBase;
			const unsigned long long IndexBase;
			const unsigned short IndexStride;
			const unsigned char IndexType;
			const bool IndexIsByteOffset;

		public:
			Indexed2SrcDescriptor(unsigned long long descriptorAddress, unsigned long long memoryBase, unsigned long long indexBase, unsigned short indexStride, unsigned char indexType, bool indexIsByteOffset);

			unsigned long long getDescriptorAddress() const;

			unsigned long long getMemoryBase() const;

			unsigned long long getIndexBase() const;

			unsigned short getIndexStride() const;

			unsigned char getIndexType() const;

			bool getIndexIsByteOffset() const;

			int getIndexElementSize() const;

			unsigned long long getEffectiveIndexStride() const;

			Indexed2SrcDescriptor() = default;
		};

	private:
		std::optional<Indexed2SrcDescriptor> _descriptor;
		bool _descriptorResolved = false;

	public:
		StoreScatterMicroOp();

		void SetStoreBuffer(std::vector<unsigned char> &buffer, std::vector<unsigned long long> &indices);

		void InitializeMetadata() override;

		bool Execute(std::shared_ptr<Processor::CPU_Core> &core) override;

		void EmitWriteBackRetireRecords(std::shared_ptr<Processor::CPU_Core> &core, std::span<RetireRecord> retireRecords, int &retireRecordCount) override;

	private:
		void PublishStagedScatterWritesAtWriteBack(std::shared_ptr<Processor::CPU_Core> &core);

		void ThrowIfUnsupportedPublicationContour();

		void ThrowIfUnsupportedRuntimeContour();

		Indexed2SrcDescriptor ResolveDescriptorFromPublishedMemory(const std::wstring &surface);

		static Indexed2SrcDescriptor ReadIndexedDescriptorFromPublishedMemory(unsigned long long descriptorAddress, const std::wstring &surface);

		static Indexed2SrcDescriptor ReadIndexedDescriptor(std::shared_ptr<Processor::CPU_Core> &core, unsigned long long descriptorAddress, const std::wstring &surface);

		static Indexed2SrcDescriptor ParseIndexedDescriptor(std::vector<unsigned char> &descriptorBytes, unsigned long long descriptorAddress, const std::wstring &surface, bool publication);

		[[noreturn]] static void CreateDescriptorException(const std::wstring &surface, const std::wstring &message, bool publication);

		static std::vector<unsigned long long> ReadPublishedIndexVector(Indexed2SrcDescriptor descriptor, unsigned long long elementCount, const std::wstring &surface);

		static unsigned long long ReadIndexElement(std::shared_ptr<Processor::CPU_Core> &core, Indexed2SrcDescriptor descriptor, unsigned long long element, const std::wstring &surface);
		std::vector<MemoryRange> BuildReadRanges(Indexed2SrcDescriptor descriptor, int elemSize, unsigned long long elementCount);
		static std::vector<MemoryRange> BuildWriteRanges(Indexed2SrcDescriptor descriptor, const std::vector<unsigned long long> &indices, int elemSize);

// C# TO C++ CONVERTER TASK: Methods returning tuples are not converted by C# to C++ Converter:
//		private (ulong Address, ulong Length)[] BuildReadRanges(Indexed2SrcDescriptor descriptor, int elemSize, ulong elementCount)
//		{
//			return [(descriptor.DescriptorAddress, Indexed2SrcDescriptorSize), BuildIndexRange(descriptor, elementCount), BuildSourceDataRange(elemSize, elementCount)];
//		}

// C# TO C++ CONVERTER TASK: Methods returning tuples are not converted by C# to C++ Converter:
//		private static (ulong Address, ulong Length)[] BuildWriteRanges(Indexed2SrcDescriptor descriptor, IReadOnlyList<ulong> indices, int elemSize)
//		{
//			(ulong Address, ulong Length)[] ranges = new(ulong Address, ulong Length)[indices.Count];
//			for (int i = 0; i < indices.Count; i++)
//			{
//				ranges[i] = (ResolveTargetAddress(descriptor, indices[i], elemSize, "StoreScatterMicroOp.InitializeMetadata()"), (ulong)elemSize);
//			}
//
//			return ranges;
//		}

// C# TO C++ CONVERTER TASK: Methods returning tuples are not converted by C# to C++ Converter:
//		private static (ulong Address, ulong Length) BuildIndexRange(Indexed2SrcDescriptor descriptor, ulong elementCount)
//		{
//			ulong length = checked(((elementCount - 1) * descriptor.EffectiveIndexStride) + (ulong)descriptor.IndexElementSize);
//			return (descriptor.IndexBase, length);
//		}

// C# TO C++ CONVERTER TASK: Methods returning tuples are not converted by C# to C++ Converter:
//		private (ulong Address, ulong Length) BuildSourceDataRange(int elemSize, ulong elementCount)
//		{
//			ulong length = checked(((elementCount - 1) * GetSourceDataStride(elemSize)) + (ulong)elemSize);
//			return (Instruction.DestSrc1Pointer, length);
//		}

		static unsigned long long GetIndexAddress(Indexed2SrcDescriptor descriptor, unsigned long long element, const std::wstring &surface);

		unsigned long long GetSourceDataAddress(unsigned long long element, int elemSize);

		unsigned long long GetSourceDataStride(int elemSize);

		static unsigned long long ResolveTargetAddress(Indexed2SrcDescriptor descriptor, unsigned long long index, int elemSize, const std::wstring &surface);

		bool IsLaneActiveForScatter(std::shared_ptr<Processor::CPU_Core> &core, unsigned long long element);

	public:
		std::shared_ptr<std::vector<unsigned long long>> GetIndices();

		std::vector<StoreScatterMicroOp::StagedScatterWrite> GetStagedWrites();

	public:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to the C# readonly struct:
// ORIGINAL LINE: public readonly struct StagedScatterWrite
		class StagedScatterWrite
		{
		private:
			const unsigned long long Element;
			const unsigned long long Address;
			const std::vector<unsigned char> Data;

		public:
			StagedScatterWrite(unsigned long long element, unsigned long long address, std::vector<unsigned char> &data);

			unsigned long long getElement() const;

			unsigned long long getAddress() const;

			std::vector<unsigned char> getData() const;

			StagedScatterWrite() = default;
		};

	private:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to the C# readonly struct:
// ORIGINAL LINE: private readonly struct Indexed2SrcDescriptor


	protected:
		std::shared_ptr<StoreScatterMicroOp> shared_from_this()
		{
			return std::static_pointer_cast<StoreScatterMicroOp>(VectorMicroOp::shared_from_this());
		}
	};

	/// <summary>
	/// Vector configuration micro-operation.
	/// Sets VL (vector length) and VTYPE (encodes SEW, LMUL, tail/mask policy).
	///
	/// This is a privileged-like operation that cannot be stolen.
}
