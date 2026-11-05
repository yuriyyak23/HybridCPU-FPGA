#include "MatrixTileStreamTransferAbi.h"
#include "MatrixTileRetirePublicationAbi.h"
#include "MatrixTileResourceContour.h"
#include "../../../../../Pipeline/Scheduling/SlotLegality/SlotClassDefinitions.h"
#include "MatrixTileMemoryShapeAndFaultAbi.h"
#include <algorithm>
#include <limits>

using namespace YAKSys_Hybrid_CPU::Core;
using namespace YAKSys_Hybrid_CPU::Memory;

namespace YAKSys_Hybrid_CPU::CloseToHSL::Core::ISA::Instructions::NonVmx::Lanes00_03Vector::MatrixTile
{

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER TASK: Throw expressions are not converted by C# to C++ Converter:
// ORIGINAL LINE: internal MatrixTileStreamTransferSession(uint coreId, int ownerThreadId, uint opcode, MatrixTileProjectedOperationKind operationKind, MatrixTileStreamTransferDirection direction, ushort expectedRows, StreamRegisterFile srf, System.Func<ulong, int, byte[]?>? memoryReader)
	MatrixTileStreamTransferSession::MatrixTileStreamTransferSession(unsigned int coreId, int ownerThreadId, unsigned int opcode, MatrixTileProjectedOperationKind operationKind, MatrixTileStreamTransferDirection direction, unsigned short expectedRows, std::shared_ptr<StreamRegisterFile> srf, std::function<std::optional<std::vector<unsigned char>>(unsigned long long, int)> memoryReader) : _coreId(coreId), _ownerThreadId(ownerThreadId), _opcode(opcode), _operationKind(operationKind), _direction(direction), _expectedRows(expectedRows), _srf((srf != nullptr) ? srf : throw std::invalid_argument("srf")), _memoryReader(memoryReader)
	{
		if (ownerThreadId < 0 || ownerThreadId >= Processor::CPU_Core::SmtWays)
		{
			throw ArgumentOutOfRangeException(L"ownerThreadId");
		}

// C# TO C++ CONVERTER TASK: The following 'is' operator pattern is not converted by C# to C++ Converter:
		if (!MatrixTileResourceContour::IsMatrixTileMemoryOpcode(opcode) || (operationKind != MatrixTileProjectedOperationKind::Load && operationKind != MatrixTileProjectedOperationKind::Store))
		{
			throw InvalidOperationException(L"MatrixTile stream transfer requires an MTILE_LOAD or MTILE_STORE identity.");
		}

	}

	std::optional<std::vector<unsigned char>> MatrixTileStreamTransferSession::ReadIngress(unsigned long long address, int length)
	{
		if (_operationKind != MatrixTileProjectedOperationKind::Load || _direction != MatrixTileStreamTransferDirection::MemoryIngress || !_memoryReader || length <= 0)
		{
			_transportFailed = true;
			return std::nullopt;
		}

		std::vector<unsigned char> row(length);
		if (!TryStageWindows(_nextIngressRow, address, row, true))
		{
			_transportFailed = true;
			return std::nullopt;
		}

		_nextIngressRow++;
		return row;
	}

	std::shared_ptr<MatrixTileTileImage> MatrixTileStreamTransferSession::StageEgress(std::shared_ptr<MatrixTileTileImage> sourceSnapshot, std::optional<std::shared_ptr<MatrixTileMemoryShapeValidationResult>> &memoryValidation)
	{
		if (_operationKind != MatrixTileProjectedOperationKind::Store || _direction != MatrixTileStreamTransferDirection::TileEgress || !sourceSnapshot || !sourceSnapshot->IsCanonicalPacked() || !memoryValidation || !memoryValidation.value() || !memoryValidation.value()->isMemoryShapeAbiAccepted())
		{
			_transportFailed = true;
			return sourceSnapshot;
		}

		const MatrixTileCanonicalDescriptorAbi &descriptor = sourceSnapshot->Descriptor;
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'checked' in this context:
// ORIGINAL LINE: int rowByteCount = checked(descriptor.Columns * descriptor.ElementSizeBytes);
		const unsigned long long rowByteCountWide = static_cast<unsigned long long>(descriptor.Columns) * descriptor.ElementSizeBytes;
		if (rowByteCountWide == 0 || rowByteCountWide > static_cast<unsigned long long>(std::numeric_limits<int>::max()))
		{
			_transportFailed = true;
			return sourceSnapshot;
		}
		const int rowByteCount = static_cast<int>(rowByteCountWide);
		std::vector<unsigned char> staged(sourceSnapshot->Data.size());
		for (unsigned short row = 0; row < descriptor.Rows; row++)
		{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'checked' in this context:
// ORIGINAL LINE: int packedOffset = checked(row * rowByteCount);
			const unsigned long long packedOffsetWide = static_cast<unsigned long long>(row) * static_cast<unsigned int>(rowByteCount);
			if (packedOffsetWide > staged.size() || static_cast<unsigned long long>(rowByteCount) > staged.size() - static_cast<std::size_t>(packedOffsetWide))
			{
				_transportFailed = true;
				return sourceSnapshot;
			}
			const auto packedOffset = static_cast<std::size_t>(packedOffsetWide);
			std::vector<unsigned char> rowData(sourceSnapshot->Data.begin() + packedOffset, sourceSnapshot->Data.begin() + packedOffset + rowByteCount);
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'checked' in this context:
// ORIGINAL LINE: ulong address = checked(memoryValidation.Value.FirstByteAddress + ((ulong)row * descriptor.StrideBytes));
			const unsigned long long firstAddress = memoryValidation.value()->FirstByteAddress;
			if (descriptor.StrideBytes != 0 && static_cast<unsigned long long>(row) > (std::numeric_limits<unsigned long long>::max() - firstAddress) / descriptor.StrideBytes)
			{
				_transportFailed = true;
				return sourceSnapshot;
			}
			const unsigned long long address = firstAddress + (static_cast<unsigned long long>(row) * descriptor.StrideBytes);
			if (!TryStageWindows(row, address, rowData, false))
			{
				_transportFailed = true;
				return nullptr;
			}

			std::copy(rowData.begin(), rowData.end(), staged.begin() + packedOffset);
		}

		return MatrixTileTileImage::Create(sourceSnapshot->TileId, sourceSnapshot->Descriptor, std::span<const unsigned char>(staged));
	}

	std::shared_ptr<MatrixTileStreamTransferRecord> MatrixTileStreamTransferSession::Complete(bool captureHasFault)
	{
		std::vector<std::shared_ptr<MatrixTileStreamWindowIdentity>> windows = _windows;
		bool completed = !captureHasFault && !_transportFailed && CountCompletedRows(windows) == _expectedRows;
		unsigned long long fingerprint = ComputeFingerprint(_coreId, _ownerThreadId, _opcode, _operationKind, _direction, _expectedRows, windows, _totalBytes, completed);

		return std::make_shared<MatrixTileStreamTransferRecord>(_coreId, _ownerThreadId, _opcode, _operationKind, MatrixTileRuntimeResourceClass::MatrixTileMemory, SlotClass::MatrixTileStreamClass, MatrixTileResourceContour::TileStreamLaneId, MatrixTileResourceContour::StreamEngineChannel, _direction, _expectedRows, windows, _totalBytes, fingerprint, completed);
	}

	bool MatrixTileStreamTransferSession::TryStageWindows(unsigned short row, unsigned long long rowAddress, std::vector<unsigned char> &rowData, bool readFromMemory)
	{
		int copied = 0;
		unsigned short window = 0;
		while (copied < rowData.size())
		{
			int chunkLength = std::min(MaxSrfWindowBytes, static_cast<int>(rowData.size()) - copied);
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'checked' in this context:
// ORIGINAL LINE: ulong chunkAddress = checked(rowAddress + (ulong)copied);
			unsigned long long chunkAddress = rowAddress + static_cast<unsigned long long>(copied);
			std::vector<unsigned char> source;
			if (readFromMemory)
			{
				std::optional<std::vector<unsigned char>> memoryRead = _memoryReader
					? _memoryReader(chunkAddress, chunkLength)
					: std::nullopt;
				if (!memoryRead || memoryRead->size() != static_cast<std::size_t>(chunkLength))
				{
					return false;
				}
				source = std::move(*memoryRead);
			}
			else
			{
				auto rowSpan = std::span<const unsigned char>(rowData).subspan(static_cast<std::size_t>(copied), static_cast<std::size_t>(chunkLength));
				source.assign(rowSpan.begin(), rowSpan.end());
			}

// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'checked' in this context:
// ORIGINAL LINE: int registerIndex = _srf.AllocateRegister(chunkAddress, elementSize: 1, checked((uint)chunkLength));
			int registerIndex = _srf->AllocateRegister(chunkAddress, 1, static_cast<unsigned int>(chunkLength));
			if (registerIndex < 0 || !_srf->LoadRegister(registerIndex, std::span<const unsigned char>(source)))
			{
				return false;
			}

			std::span<unsigned char> stagedChunk = std::span<unsigned char>(rowData).subspan(static_cast<std::size_t>(copied), static_cast<std::size_t>(chunkLength));
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'checked' in this context:
// ORIGINAL LINE: if (!_srf.TryReadPrefetchedChunk(chunkAddress, elementSize: 1, checked((uint)chunkLength), stagedChunk))
			if (!_srf->TryReadPrefetchedChunk(chunkAddress, 1, static_cast<unsigned int>(chunkLength), stagedChunk))
			{
				return false;
			}

// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'checked' in this context:
// ORIGINAL LINE: _windows.Add(new MatrixTileStreamWindowIdentity(row, window, chunkAddress, checked((uint)chunkLength), ComputeDataFingerprint(stagedChunk)));
			_windows.push_back(std::make_shared<MatrixTileStreamWindowIdentity>(row, window, chunkAddress, static_cast<unsigned int>(chunkLength), ComputeDataFingerprint(stagedChunk)));
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'checked' in this context:
// ORIGINAL LINE: _totalBytes += checked((uint)chunkLength);
			_totalBytes += static_cast<unsigned int>(chunkLength);
			copied += chunkLength;
			window++;
		}

		return true;
	}

	int MatrixTileStreamTransferSession::CountCompletedRows(std::vector<std::shared_ptr<MatrixTileStreamWindowIdentity>> &windows)
	{
		if (windows.empty())
		{
			return 0;
		}

		int rows = 1;
		unsigned short lastRow = windows[0]->Row;
		for (int index = 1; index < windows.size(); index++)
		{
			if (windows[index]->Row != lastRow)
			{
				rows++;
				lastRow = windows[index]->Row;
			}
		}

		return rows;
	}

	unsigned long long MatrixTileStreamTransferSession::ComputeDataFingerprint(std::span<const unsigned char> bytes)
	{
		unsigned long long hash = 14695981039346656037ULL;
		for (std::size_t index = 0; index < bytes.size(); index++)
		{
			hash ^= bytes[index];
			hash *= 1099511628211ULL;
		}

		return hash == 0 ? 1ULL : hash;
	}

	unsigned long long MatrixTileStreamTransferSession::ComputeFingerprint(unsigned int coreId, int ownerThreadId, unsigned int opcode, MatrixTileProjectedOperationKind operationKind, MatrixTileStreamTransferDirection direction, unsigned short expectedRows, std::vector<std::shared_ptr<MatrixTileStreamWindowIdentity>> &windows, unsigned long long totalBytes, bool completed)
	{
		unsigned long long hash = 14695981039346656037ULL;
		Add(hash, coreId);
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: Add(ref hash, unchecked((uint)ownerThreadId));
		Add(hash, static_cast<unsigned int>(ownerThreadId));
		Add(hash, opcode);
		Add(hash, static_cast<unsigned char>(operationKind));
		Add(hash, static_cast<unsigned char>(direction));
		Add(hash, expectedRows);
		Add(hash, totalBytes);
		Add(hash, completed ? static_cast<unsigned char>(1) : static_cast<unsigned char>(0));
		for (int index = 0; index < windows.size(); index++)
		{
			std::shared_ptr<MatrixTileStreamWindowIdentity> window = windows[index];
			Add(hash, window->Row);
			Add(hash, window->Window);
			Add(hash, window->Address);
			Add(hash, window->ByteCount);
			Add(hash, window->DataFingerprint);
		}

		return hash == 0 ? 1ULL : hash;
	}

	void MatrixTileStreamTransferSession::Add(unsigned long long &hash, unsigned char value)
	{
		hash ^= value;
		hash *= 1099511628211ULL;
	}

	void MatrixTileStreamTransferSession::Add(unsigned long long &hash, unsigned short value)
	{
		Add(hash, static_cast<unsigned char>(value));
		Add(hash, static_cast<unsigned char>(value >> 8));
	}

	void MatrixTileStreamTransferSession::Add(unsigned long long &hash, unsigned int value)
	{
		for (int shift = 0; shift < 32; shift += 8)
		{
			Add(hash, static_cast<unsigned char>(value >> shift));
		}
	}

	void MatrixTileStreamTransferSession::Add(unsigned long long &hash, unsigned long long value)
	{
		for (int shift = 0; shift < 64; shift += 8)
		{
			Add(hash, static_cast<unsigned char>(value >> shift));
		}
	}

	std::shared_ptr<MatrixTileStreamTransferSession> MatrixTileStreamTransferAbi::BeginLoad(std::shared_ptr<Processor::CPU_Core> &core, int ownerThreadId, unsigned int opcode, unsigned short expectedRows)
	{
		std::shared_ptr<StreamRegisterFile> srf = core->GetMatrixTileStreamRegisterFile();
		return std::make_shared<MatrixTileStreamTransferSession>(core->getCoreID(), ownerThreadId, opcode, MatrixTileProjectedOperationKind::Load, MatrixTileStreamTransferDirection::MemoryIngress, expectedRows, srf, core->CaptureMatrixTileMemoryReader());
	}

	std::shared_ptr<MatrixTileStreamTransferSession> MatrixTileStreamTransferAbi::BeginStore(std::shared_ptr<Processor::CPU_Core> &core, int ownerThreadId, unsigned int opcode, unsigned short expectedRows)
	{
		return std::make_shared<MatrixTileStreamTransferSession>(core->getCoreID(), ownerThreadId, opcode, MatrixTileProjectedOperationKind::Store, MatrixTileStreamTransferDirection::TileEgress, expectedRows, core->GetMatrixTileStreamRegisterFile(), nullptr);
	}

	void MatrixTileStreamTransferAbi::ValidateCapture(std::shared_ptr<MatrixTileExecutionCaptureRecord> capture)
	{
		std::shared_ptr<MatrixTileStreamTransferRecord> transfer = std::make_shared<MatrixTileStreamTransferRecord>(capture->StreamTransfer);
		bool memoryOperation = capture->OperationKind == MatrixTileProjectedOperationKind::Load || capture->OperationKind == MatrixTileProjectedOperationKind::Store;
		if (!memoryOperation)
		{
			if (!transfer->IsEmpty)
			{
				throw MatrixTileRetireValidationException(std::format(L"{0} compute capture cannot carry MatrixTile stream ownership.", capture->Mnemonic));
			}

			return;
		}

		bool faultedBeforeTransport = capture->HasFault && transfer->IsEmpty && capture->MemoryFaultKind != MatrixTileMemoryFaultKind::PartialMemoryFault;
		if (faultedBeforeTransport)
		{
			return;
		}

		if (!transfer->IsTypedTransport || transfer->OwnerThreadId != capture->CaptureIdentity.OwnerThreadId || transfer->CoreId != capture->CaptureIdentity.CoreId || transfer->Opcode != capture->CaptureIdentity.Opcode || transfer->OperationKind != capture->OperationKind)
		{
			throw MatrixTileRetireValidationException(std::format(L"{0} retire rejected missing or mismatched typed MatrixTile stream transfer identity.", capture->Mnemonic));
		}

		MatrixTileStreamTransferDirection expectedDirection = capture->OperationKind == MatrixTileProjectedOperationKind::Load ? MatrixTileStreamTransferDirection::MemoryIngress : MatrixTileStreamTransferDirection::TileEgress;
		if (transfer->Direction != expectedDirection || (!capture->HasFault && !transfer->Completed) || transfer->PublishesArchitecturalTileState || transfer->MutatesMemoryBeforeRetire || transfer->UsesDmaStreamComputeAuthority || transfer->UsesGenericStreamExecutionAuthority || transfer->UsesHostOwnedArchitecturalEvidence)
		{
			throw MatrixTileRetireValidationException(std::format(L"{0} retire rejected an invalid MatrixTile stream transport envelope.", capture->Mnemonic));
		}
	}
}
