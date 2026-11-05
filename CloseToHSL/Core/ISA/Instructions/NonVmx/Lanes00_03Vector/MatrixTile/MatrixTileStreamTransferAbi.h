#pragma once

#include "MatrixTileIrProjectionAndMaterializer.h"
#include "MatrixTileResourceContour.h"
#include "../../../../../../Memory/Registers/YAKSys_Hybrid_CPU.Memory.StreamRegisterFile.h"
#include "../../../../../Architecture/Common/YAKSys_Hybrid_CPU.Processor.h"
#include <string>
#include <vector>
#include <cmath>
#include <stdexcept>
#include <functional>
#include <optional>
#include <format>
#include "tangible_exception_helper.h"
#include <memory>
#include <utility>
#include <span>

using namespace YAKSys_Hybrid_CPU::Core;
using namespace YAKSys_Hybrid_CPU::Memory;

namespace YAKSys_Hybrid_CPU::CloseToHSL::Core::ISA::Instructions::NonVmx::Lanes00_03Vector::MatrixTile
{
	struct MatrixTileExecutionCaptureRecord;

	enum class MatrixTileStreamTransferDirection : unsigned char
	{
		None = 0,
		MemoryIngress = 1,
		TileEgress = 2
	};

	struct MatrixTileStreamWindowIdentity final
	{
		unsigned short Row;
		unsigned short Window;
		unsigned long long Address;
		unsigned int ByteCount;
		unsigned long long DataFingerprint;
		MatrixTileStreamWindowIdentity(unsigned short row, unsigned short window, unsigned long long address, unsigned int byteCount, unsigned long long dataFingerprint)
			: Row(row), Window(window), Address(address), ByteCount(byteCount), DataFingerprint(dataFingerprint) {}
	};

	struct MatrixTileStreamTransferRecord final
	{
		unsigned int CoreId = 0;
		int OwnerThreadId = -1;
		unsigned int Opcode = 0;
		MatrixTileProjectedOperationKind OperationKind = MatrixTileProjectedOperationKind::Unspecified;
		MatrixTileRuntimeResourceClass ResourceClass = MatrixTileRuntimeResourceClass::None;
		YAKSys_Hybrid_CPU::Core::SlotClass SlotClass = YAKSys_Hybrid_CPU::Core::SlotClass::Unclassified;
		unsigned char PhysicalLaneId = 0;
		unsigned char StreamEngineChannel = 0;
		MatrixTileStreamTransferDirection Direction = MatrixTileStreamTransferDirection::None;
		unsigned short ExpectedRows = 0;
		std::vector<std::shared_ptr<MatrixTileStreamWindowIdentity>> Windows;
		unsigned long long TotalBytes = 0;
		unsigned long long TransferFingerprint = 0;
		bool Completed;
		bool IsEmpty;
		bool IsTypedTransport;
		bool PublishesArchitecturalTileState = false;
		bool MutatesMemoryBeforeRetire = false;
		bool UsesDmaStreamComputeAuthority = false;
		bool UsesGenericStreamExecutionAuthority = false;
		bool UsesHostOwnedArchitecturalEvidence = false;
		MatrixTileStreamTransferRecord()
			: IsEmpty(true), IsTypedTransport(false) {}

		MatrixTileStreamTransferRecord(unsigned int coreId, int ownerThreadId, unsigned int opcode,
			MatrixTileProjectedOperationKind operationKind, MatrixTileRuntimeResourceClass resourceClass,
			YAKSys_Hybrid_CPU::Core::SlotClass slotClass, unsigned char physicalLaneId,
			unsigned char streamEngineChannel, MatrixTileStreamTransferDirection direction,
			unsigned short expectedRows, std::vector<std::shared_ptr<MatrixTileStreamWindowIdentity>> windows,
			unsigned long long totalBytes, unsigned long long transferFingerprint, bool completed)
			: CoreId(coreId), OwnerThreadId(ownerThreadId), Opcode(opcode), OperationKind(operationKind),
			  ResourceClass(resourceClass), SlotClass(slotClass), PhysicalLaneId(physicalLaneId),
			  StreamEngineChannel(streamEngineChannel), Direction(direction), ExpectedRows(expectedRows),
			  Windows(std::move(windows)), TotalBytes(totalBytes), TransferFingerprint(transferFingerprint),
			  Completed(completed), IsEmpty(resourceClass == MatrixTileRuntimeResourceClass::None && Windows.empty() && transferFingerprint == 0),
			  IsTypedTransport(resourceClass == MatrixTileRuntimeResourceClass::MatrixTileMemory &&
				  slotClass == YAKSys_Hybrid_CPU::Core::SlotClass::MatrixTileStreamClass &&
				  physicalLaneId == MatrixTileResourceContour::TileStreamLaneId &&
				  streamEngineChannel == MatrixTileResourceContour::StreamEngineChannel &&
				  direction != MatrixTileStreamTransferDirection::None && transferFingerprint != 0)
		{
		}
		MatrixTileStreamTransferRecord DeepClone() const
		{
			auto windows = Windows;
			for (auto &window : windows) if (window) window = std::make_shared<MatrixTileStreamWindowIdentity>(*window);
			return MatrixTileStreamTransferRecord(CoreId, OwnerThreadId, Opcode, OperationKind, ResourceClass,
				SlotClass, PhysicalLaneId, StreamEngineChannel, Direction, ExpectedRows, std::move(windows),
				TotalBytes, TransferFingerprint, Completed);
		}
	};

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct MatrixTileStreamWindowIdentity(ushort Row, ushort Window, ulong Address, uint ByteCount, ulong DataFingerprint);

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct MatrixTileStreamTransferRecord(uint CoreId, int OwnerThreadId, uint Opcode, MatrixTileProjectedOperationKind OperationKind, MatrixTileRuntimeResourceClass ResourceClass, SlotClass SlotClass, byte PhysicalLaneId, byte StreamEngineChannel, MatrixTileStreamTransferDirection Direction, ushort ExpectedRows, MatrixTileStreamWindowIdentity[] Windows, ulong TotalBytes, ulong TransferFingerprint, bool Completed)
//	{
//		public bool IsEmpty
//		{
//			get
//			{
//				return ResourceClass == MatrixTileRuntimeResourceClass.None && (Windows == nullptr || Windows.Length == 0) && TransferFingerprint == 0;
//			}
//		}
//
//		public bool IsTypedTransport
//		{
//			get
//			{
//				return ResourceClass == MatrixTileRuntimeResourceClass.MatrixTileMemory && SlotClass == SlotClass.MatrixTileStreamClass && PhysicalLaneId == MatrixTileResourceContour.TileStreamLaneId && StreamEngineChannel == MatrixTileResourceContour.StreamEngineChannel && Direction != MatrixTileStreamTransferDirection.None && TransferFingerprint != 0;
//			}
//		}
//
//		public bool PublishesArchitecturalTileState
//		{
//			get
//			{
//				return false;
//			}
//		}
//
//		public bool MutatesMemoryBeforeRetire
//		{
//			get
//			{
//				return false;
//			}
//		}
//
//		public bool UsesDmaStreamComputeAuthority
//		{
//			get
//			{
//				return false;
//			}
//		}
//
//		public bool UsesGenericStreamExecutionAuthority
//		{
//			get
//			{
//				return false;
//			}
//		}
//
//		public bool UsesHostOwnedArchitecturalEvidence
//		{
//			get
//			{
//				return false;
//			}
//		}
//
//		public MatrixTileStreamTransferRecord DeepClone()
//		{
//			return this with { Windows = Windows == nullptr ? Array.Empty<MatrixTileStreamWindowIdentity>() : (MatrixTileStreamWindowIdentity[])Windows.Clone() };
//		}
//	}

	class MatrixTileStreamTransferSession final : public std::enable_shared_from_this<MatrixTileStreamTransferSession>
	{
	private:
		static constexpr int MaxSrfWindowBytes = 256;

		const unsigned int _coreId;
		const int _ownerThreadId;
		const unsigned int _opcode;
		const MatrixTileProjectedOperationKind _operationKind;
		const MatrixTileStreamTransferDirection _direction;
		const unsigned short _expectedRows;
		const std::shared_ptr<StreamRegisterFile> _srf;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private readonly System.Func<ulong, int, byte[]?>? _memoryReader;
		const std::function<std::optional<std::vector<unsigned char>>(unsigned long long, int)> _memoryReader;
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly List<MatrixTileStreamWindowIdentity> _windows = new();
		std::vector<std::shared_ptr<MatrixTileStreamWindowIdentity>> _windows = std::vector<std::shared_ptr<MatrixTileStreamWindowIdentity>>();
		unsigned short _nextIngressRow = 0;
		unsigned long long _totalBytes = 0;
		bool _transportFailed = false;

	public:
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER TASK: Throw expressions are not converted by C# to C++ Converter:
// ORIGINAL LINE: internal MatrixTileStreamTransferSession(uint coreId, int ownerThreadId, uint opcode, MatrixTileProjectedOperationKind operationKind, MatrixTileStreamTransferDirection direction, ushort expectedRows, StreamRegisterFile srf, System.Func<ulong, int, byte[]?>? memoryReader)
		MatrixTileStreamTransferSession(unsigned int coreId, int ownerThreadId, unsigned int opcode, MatrixTileProjectedOperationKind operationKind, MatrixTileStreamTransferDirection direction, unsigned short expectedRows, std::shared_ptr<StreamRegisterFile> srf, std::function<std::optional<std::vector<unsigned char>>(unsigned long long, int)> memoryReader);

		std::optional<std::vector<unsigned char>> ReadIngress(unsigned long long address, int length);

		std::shared_ptr<MatrixTileTileImage> StageEgress(std::shared_ptr<MatrixTileTileImage> sourceSnapshot, std::optional<std::shared_ptr<MatrixTileMemoryShapeValidationResult>> &memoryValidation);

		std::shared_ptr<MatrixTileStreamTransferRecord> Complete(bool captureHasFault);

	private:
		bool TryStageWindows(unsigned short row, unsigned long long rowAddress, std::vector<unsigned char> &rowData, bool readFromMemory);

		static int CountCompletedRows(std::vector<std::shared_ptr<MatrixTileStreamWindowIdentity>> &windows);

		static unsigned long long ComputeDataFingerprint(std::span<const unsigned char> bytes);

		static unsigned long long ComputeFingerprint(unsigned int coreId, int ownerThreadId, unsigned int opcode, MatrixTileProjectedOperationKind operationKind, MatrixTileStreamTransferDirection direction, unsigned short expectedRows, std::vector<std::shared_ptr<MatrixTileStreamWindowIdentity>> &windows, unsigned long long totalBytes, bool completed);

		static void Add(unsigned long long &hash, unsigned char value);

		static void Add(unsigned long long &hash, unsigned short value);

		static void Add(unsigned long long &hash, unsigned int value);

		static void Add(unsigned long long &hash, unsigned long long value);
	};

	class MatrixTileStreamTransferAbi final
	{
	public:
		static inline const std::wstring TransferDecision = L"TypedMatrixTileStreamEngineSrfTransport";
		static inline const std::wstring AuthorityDecision = L"TransportOnlyRetireRemainsArchitecturalAuthority";
		static inline const std::wstring DscBoundaryDecision = L"NoDmaStreamComputeDescriptorTokenOrQueueAuthority";
		static inline const std::wstring StoreBoundaryDecision = L"NoMemoryMutationBeforeRetireCommit";

		static std::shared_ptr<MatrixTileStreamTransferSession> BeginLoad(std::shared_ptr<Processor::CPU_Core> &core, int ownerThreadId, unsigned int opcode, unsigned short expectedRows);

		static std::shared_ptr<MatrixTileStreamTransferSession> BeginStore(std::shared_ptr<Processor::CPU_Core> &core, int ownerThreadId, unsigned int opcode, unsigned short expectedRows);

		static void ValidateCapture(std::shared_ptr<MatrixTileExecutionCaptureRecord> capture);
	};

}
