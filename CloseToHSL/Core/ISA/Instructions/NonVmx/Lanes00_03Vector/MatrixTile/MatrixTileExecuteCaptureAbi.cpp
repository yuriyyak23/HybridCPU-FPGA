#include "MatrixTileExecuteCaptureAbi.h"
#include <algorithm>
#include "../../../../../Architecture/Common/YAKSys_Hybrid_CPU.Processor.h"
#include "MatrixTileMemoryShapeAndFaultAbi.h"
#include "MatrixTileMaccArithmeticAbi.h"
#include "MatrixTileAccumulatorAndTransposePolicyAbi.h"
#include "MatrixTileLayoutPolicyAbi.h"
#include <limits>

namespace YAKSys_Hybrid_CPU::CloseToHSL::Core::ISA::Instructions::NonVmx::Lanes00_03Vector::MatrixTile
{
	std::shared_ptr<MatrixTileTileImage> MatrixTileTileImage::Create(unsigned short tileId, const MatrixTileCanonicalDescriptorAbi &descriptor, std::span<const unsigned char> data)
	{
		if (data.size() > static_cast<std::size_t>(std::numeric_limits<int>::max()))
		{
			throw InvalidOperationException(L"MatrixTile packed image exceeds the supported byte length.");
		}
		auto descriptorCopy = std::make_shared<MatrixTileCanonicalDescriptorAbi>(descriptor);
		MatrixTileExecuteCaptureAbi::ThrowIfInvalidPackedTileImage(descriptorCopy, static_cast<int>(data.size()));
		auto image = std::make_shared<MatrixTileTileImage>();
		image->TileId = tileId;
		image->Descriptor = descriptor;
		image->Data.assign(data.begin(), data.end());
		return image;
	}

	std::shared_ptr<MatrixTileTileImage> MatrixTileTileImage::Create(unsigned short tileId, const std::shared_ptr<MatrixTileCanonicalDescriptorAbi> &descriptor, std::span<const unsigned char> data)
	{
		if (!descriptor)
		{
			throw InvalidOperationException(L"MatrixTile packed image requires a canonical tile descriptor.");
		}
		return Create(tileId, *descriptor, data);
	}

	int MatrixTileArchitecturalTileRegisterFile::getTileCount() const
	{
		return _tiles.size();
	}

	void MatrixTileArchitecturalTileRegisterFile::WriteTileForRuntimeSeed(int ownerThreadId, unsigned short tileId, std::shared_ptr<MatrixTileCanonicalDescriptorAbi> descriptor, std::span<const unsigned char> packedData)
	{
		std::shared_ptr<MatrixTileTileImage> image = MatrixTileTileImage::Create(tileId, descriptor, packedData);
		_tiles[std::make_pair(ValidateOwnerThreadId(ownerThreadId), tileId)] = image;
	}

	bool MatrixTileArchitecturalTileRegisterFile::TryCaptureSnapshot(int ownerThreadId, unsigned short tileId, std::shared_ptr<MatrixTileCanonicalDescriptorAbi> expectedDescriptor, std::shared_ptr<MatrixTileTileImage> &snapshot)
	{
		snapshot = nullptr;
		std::shared_ptr<MatrixTileTileImage> image;
		auto _tiles_iterator = _tiles.find(std::make_pair(ValidateOwnerThreadId(ownerThreadId), tileId));
		if (_tiles_iterator == _tiles.end())
		{
			return false;
		}
		else
		{
			image = _tiles_iterator->second;
		}

		if (!expectedDescriptor || image->Descriptor != *expectedDescriptor || !image->IsCanonicalPacked())
		{
			return false;
		}

		snapshot = std::make_shared<MatrixTileTileImage>(image->DeepClone());
		return true;
	}

	bool MatrixTileArchitecturalTileRegisterFile::TryCaptureAnySnapshot(int ownerThreadId, unsigned short tileId, std::shared_ptr<MatrixTileTileImage> &snapshot)
	{
		snapshot = nullptr;
		std::shared_ptr<MatrixTileTileImage> image;
		auto _tiles_iterator = _tiles.find(std::make_pair(ValidateOwnerThreadId(ownerThreadId), tileId));
		if (_tiles_iterator == _tiles.end() || !_tiles_iterator->second->IsCanonicalPacked())
		{
			return false;
		}
		else
		{
			image = _tiles_iterator->second;
		}

		snapshot = std::make_shared<MatrixTileTileImage>(image->DeepClone());
		return true;
	}

	bool MatrixTileArchitecturalTileRegisterFile::RemoveTile(int ownerThreadId, unsigned short tileId)
	{
		return _tiles.erase(std::make_pair(ValidateOwnerThreadId(ownerThreadId), tileId));
	}

	bool MatrixTileArchitecturalTileRegisterFile::ContainsTile(int ownerThreadId, unsigned short tileId)
	{
		return _tiles.contains(std::make_pair(ValidateOwnerThreadId(ownerThreadId), tileId));
	}

	int MatrixTileArchitecturalTileRegisterFile::ValidateOwnerThreadId(int ownerThreadId)
	{
		if (ownerThreadId < 0 || ownerThreadId >= Processor::CPU_Core::SmtWays)
		{
			throw ArgumentOutOfRangeException(L"ownerThreadId", ownerThreadId, std::format(L"MTILE architectural owner must be in the range [0, {0}].", Processor::CPU_Core::SmtWays - 1));
		}

		return ownerThreadId;
	}

	int MatrixTileExecuteCaptureAbi::GetPackedByteLength(std::shared_ptr<MatrixTileCanonicalDescriptorAbi> descriptor)
	{
//		checked
		if (!descriptor || !descriptor->isCanonical())
		{
			return 0;
		}
		const unsigned long long length = static_cast<unsigned long long>(descriptor->Rows) * descriptor->Columns * descriptor->ElementSizeBytes;
		if (length > static_cast<unsigned long long>(std::numeric_limits<int>::max()))
		{
			throw std::overflow_error("MatrixTile packed byte length exceeds Int32 range.");
		}
		return static_cast<int>(length);
// C# TO C++ CONVERTER TASK: End of the original C# 'checked' block.
	}

	void MatrixTileExecuteCaptureAbi::ThrowIfInvalidPackedTileImage(std::shared_ptr<MatrixTileCanonicalDescriptorAbi> descriptor, int byteLength)
	{
		if (!descriptor || !descriptor->isCanonical())
		{
			throw InvalidOperationException(L"MatrixTile packed image requires a canonical tile descriptor.");
		}

		int expectedLength = GetPackedByteLength(descriptor);
		if (byteLength != expectedLength)
		{
			throw InvalidOperationException(std::format(L"MatrixTile packed image length {0} does not match descriptor footprint {1}.", byteLength, expectedLength));
		}
	}

	std::shared_ptr<MatrixTileExecutionCaptureRecord> MatrixTileExecuteCaptureAbi::CaptureLoad(const std::wstring &mnemonic, MatrixTileProjectedOperationKind operationKind, unsigned short sourceTileId, unsigned short secondaryTileId, unsigned short destinationTileId, std::shared_ptr<MatrixTileCanonicalDescriptorAbi> tileDescriptor, std::shared_ptr<MatrixTileCanonicalDescriptorAbi> secondaryTileDescriptor, std::shared_ptr<MatrixTileCanonicalDescriptorAbi> resultTileDescriptor, std::optional<std::shared_ptr<MatrixTileMemoryShapeContract>> &memoryContract, std::optional<std::shared_ptr<MatrixTileMemoryShapeValidationResult>> &memoryValidation, std::function<std::optional<std::vector<unsigned char>>(unsigned long long, int)> readMemoryExact)
	{
		if (!memoryContract || !memoryContract.value())
		{
			return Fault(mnemonic, operationKind, sourceTileId, secondaryTileId, destinationTileId, tileDescriptor, secondaryTileDescriptor, resultTileDescriptor, MatrixTileExecutionFaultKind::MissingMemoryContract, L"MTILE_LOAD execution capture requires a Phase03 memory contract.");
		}

		if (!memoryValidation || !memoryValidation.value())
		{
			return Fault(mnemonic, operationKind, sourceTileId, secondaryTileId, destinationTileId, tileDescriptor, secondaryTileDescriptor, resultTileDescriptor, MatrixTileExecutionFaultKind::MissingMemoryValidation, L"MTILE_LOAD execution capture requires preserved Phase03 memory validation.");
		}

		if (!memoryValidation.value() || !memoryValidation.value()->isMemoryShapeAbiAccepted())
		{
			return MemoryFault(mnemonic, operationKind, sourceTileId, secondaryTileId, destinationTileId, tileDescriptor, secondaryTileDescriptor, resultTileDescriptor, memoryValidation.value(), L"MTILE_LOAD execution capture rejected an invalid memory shape.");
		}

		std::shared_ptr<MatrixTileMemoryShapeContract> contract = memoryContract.value();
		std::shared_ptr<MatrixTileMemoryShapeValidationResult> validation = memoryValidation.value();
		std::vector<unsigned char> packed(GetPackedByteLength(contract->Descriptor));
		std::shared_ptr<MatrixTileMemoryFaultPoint> faultPoint;
		if (!TryCopyMemoryRowsToPackedTile(contract, validation, readMemoryExact, packed, faultPoint))
		{
			return MemoryFault(mnemonic, operationKind, sourceTileId, secondaryTileId, destinationTileId, tileDescriptor, secondaryTileDescriptor, resultTileDescriptor, MatrixTileMemoryShapeValidationResult::Fault(MatrixTileMemoryFaultKind::PartialMemoryFault, faultPoint ? *faultPoint : MatrixTileMemoryFaultPoint{}, true), L"MTILE_LOAD execution capture failed to read a complete tile row.");
		}

		std::shared_ptr<MatrixTileTileImage> resultImage = MatrixTileTileImage::Create(destinationTileId, resultTileDescriptor, packed);
		return Success(MatrixTileExecutionCaptureKind::TileLoad, operationKind, mnemonic, sourceTileId, secondaryTileId, destinationTileId, tileDescriptor, secondaryTileDescriptor, resultTileDescriptor, nullptr, nullptr, nullptr, resultImage, memoryValidation, std::nullopt, {});
	}

	std::shared_ptr<MatrixTileExecutionCaptureRecord> MatrixTileExecuteCaptureAbi::CaptureStore(const std::wstring &mnemonic, MatrixTileProjectedOperationKind operationKind, unsigned short sourceTileId, unsigned short secondaryTileId, unsigned short destinationTileId, std::shared_ptr<MatrixTileCanonicalDescriptorAbi> tileDescriptor, std::shared_ptr<MatrixTileCanonicalDescriptorAbi> secondaryTileDescriptor, std::shared_ptr<MatrixTileCanonicalDescriptorAbi> resultTileDescriptor, std::optional<std::shared_ptr<MatrixTileMemoryShapeContract>> &memoryContract, std::optional<std::shared_ptr<MatrixTileMemoryShapeValidationResult>> &memoryValidation, std::shared_ptr<MatrixTileTileImage> sourceSnapshot)
	{
		if (!sourceSnapshot || !sourceSnapshot->IsCanonicalPacked())
		{
			return Fault(mnemonic, operationKind, sourceTileId, secondaryTileId, destinationTileId, tileDescriptor, secondaryTileDescriptor, resultTileDescriptor, MatrixTileExecutionFaultKind::MissingTileStateSnapshot, L"MTILE_STORE execution capture requires a tile-state source snapshot.");
		}

		if (!memoryContract || !memoryContract.value())
		{
			return Fault(mnemonic, operationKind, sourceTileId, secondaryTileId, destinationTileId, tileDescriptor, secondaryTileDescriptor, resultTileDescriptor, MatrixTileExecutionFaultKind::MissingMemoryContract, L"MTILE_STORE execution capture requires a Phase03 memory contract.");
		}

		if (!memoryValidation || !memoryValidation.value())
		{
			return Fault(mnemonic, operationKind, sourceTileId, secondaryTileId, destinationTileId, tileDescriptor, secondaryTileDescriptor, resultTileDescriptor, MatrixTileExecutionFaultKind::MissingMemoryValidation, L"MTILE_STORE execution capture requires preserved Phase03 memory validation.");
		}

		if (!memoryValidation.value() || !memoryValidation.value()->isMemoryShapeAbiAccepted())
		{
			return MemoryFault(mnemonic, operationKind, sourceTileId, secondaryTileId, destinationTileId, tileDescriptor, secondaryTileDescriptor, resultTileDescriptor, memoryValidation.value(), L"MTILE_STORE execution capture rejected an invalid memory shape.");
		}

		std::vector<std::shared_ptr<MatrixTileCapturedMemoryWrite>> writes = CreatePendingStoreWrites(memoryContract.value(), sourceSnapshot);

		return Success(MatrixTileExecutionCaptureKind::TileStore, operationKind, mnemonic, sourceTileId, secondaryTileId, destinationTileId, tileDescriptor, secondaryTileDescriptor, resultTileDescriptor, sourceSnapshot, nullptr, nullptr, nullptr, memoryValidation, std::nullopt, writes);
	}

	std::shared_ptr<MatrixTileExecutionCaptureRecord> MatrixTileExecuteCaptureAbi::CaptureMacc(const std::wstring &mnemonic, MatrixTileProjectedOperationKind operationKind, unsigned short sourceTileId, unsigned short secondaryTileId, unsigned short destinationTileId, std::shared_ptr<MatrixTileCanonicalDescriptorAbi> tileDescriptor, std::shared_ptr<MatrixTileCanonicalDescriptorAbi> secondaryTileDescriptor, std::shared_ptr<MatrixTileCanonicalDescriptorAbi> resultTileDescriptor, std::optional<std::shared_ptr<MatrixTileSemanticValidationResult>> &semanticValidation, std::optional<std::shared_ptr<MatrixTileMaccSemanticContract>> &maccContract, std::shared_ptr<MatrixTileTileImage> leftSnapshot, std::shared_ptr<MatrixTileTileImage> rightSnapshot, std::shared_ptr<MatrixTileTileImage> accumulatorSnapshot)
	{
		if (!semanticValidation || !semanticValidation.value())
		{
			return Fault(mnemonic, operationKind, sourceTileId, secondaryTileId, destinationTileId, tileDescriptor, secondaryTileDescriptor, resultTileDescriptor, MatrixTileExecutionFaultKind::MissingSemanticValidation, L"MTILE_MACC execution capture requires preserved Phase04 semantic validation.");
		}

		if (!semanticValidation.value() || !semanticValidation.value()->isSemanticAbiAccepted() || !maccContract || !maccContract.value())
		{
			return SemanticFault(mnemonic, operationKind, sourceTileId, secondaryTileId, destinationTileId, tileDescriptor, secondaryTileDescriptor, resultTileDescriptor, semanticValidation.value(), L"MTILE_MACC execution capture rejected an invalid semantic contract.");
		}

		if (!leftSnapshot || !rightSnapshot || !leftSnapshot->IsCanonicalPacked() || !rightSnapshot->IsCanonicalPacked())
		{
			return Fault(mnemonic, operationKind, sourceTileId, secondaryTileId, destinationTileId, tileDescriptor, secondaryTileDescriptor, resultTileDescriptor, MatrixTileExecutionFaultKind::MissingTileStateSnapshot, L"MTILE_MACC execution capture requires left and right tile-state snapshots.");
		}

		if (!accumulatorSnapshot || !accumulatorSnapshot->IsCanonicalPacked())
		{
			return Fault(mnemonic, operationKind, sourceTileId, secondaryTileId, destinationTileId, tileDescriptor, secondaryTileDescriptor, resultTileDescriptor, MatrixTileExecutionFaultKind::MissingAccumulatorSnapshot, L"MTILE_MACC execution capture requires an accumulator tile snapshot.");
		}

		std::shared_ptr<MatrixTileTileImage> resultImage;
		MatrixTileMaccArithmeticFaultKind arithmeticFault;
		if (!MatrixTileMaccArithmeticAbi::TryCompute(maccContract.value(), leftSnapshot, rightSnapshot, accumulatorSnapshot, resultImage, arithmeticFault))
		{
			return Fault(mnemonic, operationKind, sourceTileId, secondaryTileId, destinationTileId, tileDescriptor, secondaryTileDescriptor, resultTileDescriptor, arithmeticFault == MatrixTileMaccArithmeticFaultKind::ArithmeticOverflow ? MatrixTileExecutionFaultKind::ArithmeticOverflow : MatrixTileExecutionFaultKind::ArithmeticPolicyViolation, std::format(L"MTILE_MACC execution capture failed formal arithmetic policy: {0}.", static_cast<int>(arithmeticFault)));
		}

		return Success(MatrixTileExecutionCaptureKind::Macc, operationKind, mnemonic, sourceTileId, secondaryTileId, destinationTileId, tileDescriptor, secondaryTileDescriptor, resultTileDescriptor, leftSnapshot, rightSnapshot, accumulatorSnapshot, resultImage, std::nullopt, semanticValidation, {});
	}

	std::shared_ptr<MatrixTileExecutionCaptureRecord> MatrixTileExecuteCaptureAbi::CaptureTranspose(const std::wstring &mnemonic, MatrixTileProjectedOperationKind operationKind, unsigned short sourceTileId, unsigned short secondaryTileId, unsigned short destinationTileId, std::shared_ptr<MatrixTileCanonicalDescriptorAbi> tileDescriptor, std::shared_ptr<MatrixTileCanonicalDescriptorAbi> secondaryTileDescriptor, std::shared_ptr<MatrixTileCanonicalDescriptorAbi> resultTileDescriptor, std::optional<std::shared_ptr<MatrixTileSemanticValidationResult>> &semanticValidation, std::optional<std::shared_ptr<MatrixTileTransposeSemanticContract>> &transposeContract, std::shared_ptr<MatrixTileTileImage> sourceSnapshot)
	{
		if (!semanticValidation || !semanticValidation.value())
		{
			return Fault(mnemonic, operationKind, sourceTileId, secondaryTileId, destinationTileId, tileDescriptor, secondaryTileDescriptor, resultTileDescriptor, MatrixTileExecutionFaultKind::MissingSemanticValidation, L"MTRANSPOSE execution capture requires preserved Phase04 semantic validation.");
		}

		if (!semanticValidation.value() || !semanticValidation.value()->isSemanticAbiAccepted() || !transposeContract || !transposeContract.value())
		{
			return SemanticFault(mnemonic, operationKind, sourceTileId, secondaryTileId, destinationTileId, tileDescriptor, secondaryTileDescriptor, resultTileDescriptor, semanticValidation.value(), L"MTRANSPOSE execution capture rejected an invalid semantic contract.");
		}

		if (!sourceSnapshot || !sourceSnapshot->IsCanonicalPacked())
		{
			return Fault(mnemonic, operationKind, sourceTileId, secondaryTileId, destinationTileId, tileDescriptor, secondaryTileDescriptor, resultTileDescriptor, MatrixTileExecutionFaultKind::MissingTileStateSnapshot, L"MTRANSPOSE execution capture requires a source tile-state snapshot.");
		}

		std::shared_ptr<MatrixTileTileImage> resultImage = Transpose(sourceSnapshot, destinationTileId,
			std::make_shared<MatrixTileCanonicalDescriptorAbi>(transposeContract.value()->Destination), transposeContract.value()->LayoutPolicy);

		return Success(MatrixTileExecutionCaptureKind::Transpose, operationKind, mnemonic, sourceTileId, secondaryTileId, destinationTileId, tileDescriptor, secondaryTileDescriptor, resultTileDescriptor, sourceSnapshot, nullptr, nullptr, resultImage, std::nullopt, semanticValidation, {});
	}

	std::shared_ptr<MatrixTileExecutionCaptureRecord> MatrixTileExecuteCaptureAbi::Success(MatrixTileExecutionCaptureKind captureKind, MatrixTileProjectedOperationKind operationKind, const std::wstring &mnemonic, unsigned short sourceTileId, unsigned short secondaryTileId, unsigned short destinationTileId, std::shared_ptr<MatrixTileCanonicalDescriptorAbi> tileDescriptor, std::shared_ptr<MatrixTileCanonicalDescriptorAbi> secondaryTileDescriptor, std::shared_ptr<MatrixTileCanonicalDescriptorAbi> resultTileDescriptor, std::shared_ptr<MatrixTileTileImage> sourceSnapshot, std::shared_ptr<MatrixTileTileImage> secondarySnapshot, std::shared_ptr<MatrixTileTileImage> accumulatorSnapshot, std::shared_ptr<MatrixTileTileImage> resultImage, const std::optional<std::shared_ptr<MatrixTileMemoryShapeValidationResult>> &memoryValidation, const std::optional<std::shared_ptr<MatrixTileSemanticValidationResult>> &semanticValidation, const std::vector<std::shared_ptr<MatrixTileCapturedMemoryWrite>> &pendingStoreWrites)
	{
		const auto cloneImage = [](const std::shared_ptr<MatrixTileTileImage> &image)
		{
			return image ? std::make_shared<MatrixTileTileImage>(image->DeepClone()) : nullptr;
		};
		return std::make_shared<MatrixTileExecutionCaptureRecord>(captureKind, operationKind, mnemonic, sourceTileId, secondaryTileId, destinationTileId, tileDescriptor, secondaryTileDescriptor, resultTileDescriptor, cloneImage(sourceSnapshot), cloneImage(secondarySnapshot), cloneImage(accumulatorSnapshot), cloneImage(resultImage), memoryValidation, semanticValidation, pendingStoreWrites, MatrixTileExecutionFaultKind::None, MatrixTileMemoryFaultKind::None, MatrixTileSemanticFaultKind::None, MatrixTileMemoryFaultPoint{}, false, L"", false, true, true, true, false);
	}

	std::shared_ptr<MatrixTileExecutionCaptureRecord> MatrixTileExecuteCaptureAbi::Fault(const std::wstring &mnemonic, MatrixTileProjectedOperationKind operationKind, unsigned short sourceTileId, unsigned short secondaryTileId, unsigned short destinationTileId, std::shared_ptr<MatrixTileCanonicalDescriptorAbi> tileDescriptor, std::shared_ptr<MatrixTileCanonicalDescriptorAbi> secondaryTileDescriptor, std::shared_ptr<MatrixTileCanonicalDescriptorAbi> resultTileDescriptor, MatrixTileExecutionFaultKind faultKind, const std::wstring &message)
	{
		return std::make_shared<MatrixTileExecutionCaptureRecord>(MatrixTileExecutionCaptureKind::Fault, operationKind, mnemonic, sourceTileId, secondaryTileId, destinationTileId, tileDescriptor, secondaryTileDescriptor, resultTileDescriptor, nullptr, nullptr, nullptr, nullptr, std::nullopt, std::nullopt, std::vector<std::shared_ptr<MatrixTileCapturedMemoryWrite>>{}, faultKind, MatrixTileMemoryFaultKind::None, MatrixTileSemanticFaultKind::None, MatrixTileMemoryFaultPoint{}, false, message, true, true, true, true, false);
	}

	std::shared_ptr<MatrixTileExecutionCaptureRecord> MatrixTileExecuteCaptureAbi::MemoryFault(const std::wstring &mnemonic, MatrixTileProjectedOperationKind operationKind, unsigned short sourceTileId, unsigned short secondaryTileId, unsigned short destinationTileId, std::shared_ptr<MatrixTileCanonicalDescriptorAbi> tileDescriptor, std::shared_ptr<MatrixTileCanonicalDescriptorAbi> secondaryTileDescriptor, std::shared_ptr<MatrixTileCanonicalDescriptorAbi> resultTileDescriptor, std::shared_ptr<MatrixTileMemoryShapeValidationResult> memoryValidation, const std::wstring &message)
	{
		auto capture = Fault(mnemonic, operationKind, sourceTileId, secondaryTileId, destinationTileId, tileDescriptor, secondaryTileDescriptor, resultTileDescriptor, MatrixTileExecutionFaultKind::InvalidMemoryShape, message);
		if (memoryValidation)
		{
			capture->MemoryValidation = memoryValidation;
			capture->MemoryFaultKind = memoryValidation->FaultKind;
			capture->FaultPoint = memoryValidation->FaultPoint;
			capture->HasFaultPoint = memoryValidation->HasFaultPoint;
		}
		return capture;
	}

	std::shared_ptr<MatrixTileExecutionCaptureRecord> MatrixTileExecuteCaptureAbi::SemanticFault(const std::wstring &mnemonic, MatrixTileProjectedOperationKind operationKind, unsigned short sourceTileId, unsigned short secondaryTileId, unsigned short destinationTileId, std::shared_ptr<MatrixTileCanonicalDescriptorAbi> tileDescriptor, std::shared_ptr<MatrixTileCanonicalDescriptorAbi> secondaryTileDescriptor, std::shared_ptr<MatrixTileCanonicalDescriptorAbi> resultTileDescriptor, std::shared_ptr<MatrixTileSemanticValidationResult> semanticValidation, const std::wstring &message)
	{
		auto capture = Fault(mnemonic, operationKind, sourceTileId, secondaryTileId, destinationTileId, tileDescriptor, secondaryTileDescriptor, resultTileDescriptor, MatrixTileExecutionFaultKind::InvalidSemanticContract, message);
		if (semanticValidation)
		{
			capture->SemanticValidation = semanticValidation;
			capture->SemanticFaultKind = semanticValidation->FaultKind;
		}
		return capture;
	}

	bool MatrixTileExecuteCaptureAbi::TryCopyMemoryRowsToPackedTile(std::shared_ptr<MatrixTileMemoryShapeContract> contract, std::shared_ptr<MatrixTileMemoryShapeValidationResult> validation, std::function<std::optional<std::vector<unsigned char>>(unsigned long long, int)> readMemoryExact, std::vector<unsigned char> &packed, std::shared_ptr<MatrixTileMemoryFaultPoint> &faultPoint)
	{
		faultPoint = nullptr;
		if (!contract || !contract->Descriptor || !validation || !readMemoryExact)
		{
			return false;
		}
		std::shared_ptr<MatrixTileCanonicalDescriptorAbi> descriptor = contract->Descriptor;
		if (validation->RowByteCount > static_cast<unsigned int>(std::numeric_limits<int>::max()))
		{
			throw std::overflow_error("MatrixTile row byte count exceeds Int32 range.");
		}
		int rowBytes = static_cast<int>(validation->RowByteCount);
		for (unsigned short row = 0; row < descriptor->Rows; row++)
		{
			const unsigned long long rowOffset = static_cast<unsigned long long>(row) * descriptor->StrideBytes;
			if (rowOffset > std::numeric_limits<unsigned long long>::max() - contract->BaseAddress)
			{
				throw std::overflow_error("MatrixTile row address overflow.");
			}
			const unsigned long long address = contract->BaseAddress + rowOffset;
			auto rowBytesBuffer = readMemoryExact(address, rowBytes);
			if (!rowBytesBuffer || rowBytesBuffer->size() != static_cast<size_t>(rowBytes))
			{
				faultPoint = MatrixTileMemoryShapeAndFaultAbi::CreatePreciseFaultPoint(contract, row, 0, 0);
				return false;
			}

			std::copy(rowBytesBuffer->begin(), rowBytesBuffer->end(), packed.begin() + row * rowBytes);
		}

		return true;
	}

	std::vector<std::shared_ptr<MatrixTileCapturedMemoryWrite>> MatrixTileExecuteCaptureAbi::CreatePendingStoreWrites(std::shared_ptr<MatrixTileMemoryShapeContract> contract, std::shared_ptr<MatrixTileTileImage> sourceSnapshot)
	{
		if (!contract || !contract->Descriptor || !sourceSnapshot)
		{
			throw std::invalid_argument("MatrixTile store capture requires a contract and source snapshot.");
		}
		if (!sourceSnapshot->IsCanonicalPacked())
		{
			throw std::invalid_argument("MatrixTile store capture requires an exact canonical packed source snapshot.");
		}
		std::shared_ptr<MatrixTileCanonicalDescriptorAbi> descriptor = contract->Descriptor;
		const unsigned long long rowByteCount = static_cast<unsigned long long>(descriptor->Columns) * descriptor->ElementSizeBytes;
		if (rowByteCount > static_cast<unsigned long long>(std::numeric_limits<int>::max()))
		{
			throw std::overflow_error("MatrixTile store row byte count exceeds Int32 range.");
		}
		const int rowBytes = static_cast<int>(rowByteCount);
		std::vector<std::shared_ptr<MatrixTileCapturedMemoryWrite>> writes(descriptor->Rows);
		for (unsigned short row = 0; row < descriptor->Rows; row++)
		{
			std::vector<unsigned char> rowData(rowBytes);
			const std::size_t sourceOffset = static_cast<std::size_t>(row) * static_cast<std::size_t>(rowBytes);
			if (sourceOffset > sourceSnapshot->Data.size() || static_cast<std::size_t>(rowBytes) > sourceSnapshot->Data.size() - sourceOffset)
			{
				throw std::out_of_range("MatrixTile store row exceeds the canonical packed source snapshot.");
			}
			std::copy_n(sourceSnapshot->Data.begin() + sourceOffset, rowBytes, rowData.begin());
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'checked' in this context:
// ORIGINAL LINE: ulong address = checked(contract.BaseAddress + (ulong)row * descriptor.StrideBytes);
			const unsigned long long rowOffset = static_cast<unsigned long long>(row) * descriptor->StrideBytes;
			if (rowOffset > std::numeric_limits<unsigned long long>::max() - contract->BaseAddress)
			{
				throw std::overflow_error("MatrixTile store row address overflow.");
			}
			const unsigned long long address = contract->BaseAddress + rowOffset;
			writes[row] = std::make_shared<MatrixTileCapturedMemoryWrite>(row, address, rowData);
		}

		return writes;
	}

	std::shared_ptr<MatrixTileTileImage> MatrixTileExecuteCaptureAbi::Transpose(std::shared_ptr<MatrixTileTileImage> source, unsigned short destinationTileId, std::shared_ptr<MatrixTileCanonicalDescriptorAbi> destinationDescriptor, std::shared_ptr<MatrixTileLayoutPolicy> layoutPolicy)
	{
		if (!source || !destinationDescriptor || !layoutPolicy)
		{
			throw std::invalid_argument("MatrixTile transpose requires a source, destination descriptor, and layout policy.");
		}
		if (!source->IsCanonicalPacked())
		{
			throw std::invalid_argument("MatrixTile transpose requires an exact canonical packed source snapshot.");
		}
		std::shared_ptr<MatrixTileCanonicalDescriptorAbi> sourceDescriptor = std::make_shared<MatrixTileCanonicalDescriptorAbi>(source->Descriptor);
		int elementSize = sourceDescriptor->ElementSizeBytes;
		std::vector<unsigned char> destination(GetPackedByteLength(destinationDescriptor));

		for (unsigned short row = 0; row < sourceDescriptor->Rows; row++)
		{
			for (unsigned short column = 0; column < sourceDescriptor->Columns; column++)
			{
				int sourceOffset = MatrixTileLayoutPolicyAbi::GetPackedOffset(sourceDescriptor, row, column, layoutPolicy->SourceAddressing);
				int destinationOffset = MatrixTileLayoutPolicyAbi::GetPackedOffset(destinationDescriptor, column, row, layoutPolicy->DestinationAddressing);
				if (sourceOffset < 0 || destinationOffset < 0 || elementSize <= 0
					|| static_cast<std::size_t>(sourceOffset) > source->Data.size()
					|| static_cast<std::size_t>(elementSize) > source->Data.size() - static_cast<std::size_t>(sourceOffset)
					|| static_cast<std::size_t>(destinationOffset) > destination.size()
					|| static_cast<std::size_t>(elementSize) > destination.size() - static_cast<std::size_t>(destinationOffset))
				{
					throw std::out_of_range("MatrixTile transpose element exceeds its packed source or destination.");
				}
				std::copy_n(source->Data.begin() + sourceOffset, elementSize, destination.begin() + destinationOffset);
			}
		}

		return MatrixTileTileImage::Create(destinationTileId, destinationDescriptor, destination);
	}
}
