#include "YAKSys_Hybrid_CPU.Execution.StreamEngine.h"
#include <array>
#include <span>
#include <stdexcept>
#include <type_traits>
#include <codecvt>
#include <locale>
#include "../../../../Memory/Subsystem/YAKSys_Hybrid_CPU.Memory.MemorySubsystem.h"
#include "../../../../../NonRTL/Arch/YAKSys_Hybrid_CPU.Arch.OpcodeRegistry.h"
#include "../../../../../NonRTL/Arch/OpcodeInfo.Types.h"
#include "../../../Virtualization/Compatibility/Adapters/IO/YAKSys_Hybrid_CPU.Memory.IOMMU.h"
#include "../BurstIO/StreamEngine.BurstIO.h"
#include "../../../../../Arch/DataType.h"
#include "../../ExecutionFaultContract.h"
#include "../../Vector/ALU/YAKSys_Hybrid_CPU.Execution.VectorALU.h"
#include "../../../Architecture/Registers/Physical/PhysicalRegisterFile.h"
#include "../AddressGeneration/StreamEngine.AddressGen.h"
#include "../../../../Memory/MMU/IOMMU.h"
#include "../BurstIO/BurstPlanner.h"

using namespace YAKSys_Hybrid_CPU::Arch;
using namespace YAKSys_Hybrid_CPU::Memory;
using namespace YAKSys_Hybrid_CPU::Core::Registers::Retire;
using namespace YAKSys_Hybrid_CPU::Core;
using namespace HybridCPU_ISE::Arch;
using namespace HybridCPU_ISE::CloseToHSL::Memory::MMU;

namespace YAKSys_Hybrid_CPU::Execution
{
	namespace
	{
		std::string ToNarrow(const std::wstring &value)
		{
			return std::wstring_convert<std::codecvt_utf8<wchar_t>>().to_bytes(value);
		}

		std::wstring ToWide(const std::string &value)
		{
			return std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(value);
		}
	}
	namespace
	{
		template <typename T>
		T ReadLittleEndian(std::span<const unsigned char> bytes)
		{
			static_assert(std::is_unsigned_v<T>);
			if (bytes.size() < sizeof(T))
				throw std::out_of_range("Descriptor field is shorter than its integer width.");

			T value = 0;
			for (std::size_t i = 0; i < sizeof(T); ++i)
				value |= static_cast<T>(bytes[i]) << (i * 8);
			return value;
		}

		template <typename T>
		std::span<T> CheckedSubspan(std::span<T> source, std::size_t offset, std::size_t count)
		{
			if (offset > source.size() || count > source.size() - offset)
				throw std::out_of_range("Stream buffer slice is outside the available span.");
			return source.subspan(offset, count);
		}
	}

	bool StreamEngine::IsBinaryOp(unsigned int opCode)
	{
	// C# TO C++ CONVERTER TASK: The following 'switch expression' was not converted by C# to C++ Converter:
	//			return opCode switch
	//			{
	//				// Binary operations
	//				(uint)Processor.CPU_Core.InstructionsEnum.VADD => true,
	//				(uint)Processor.CPU_Core.InstructionsEnum.VSUB => true,
	//				(uint)Processor.CPU_Core.InstructionsEnum.VMUL => true,
	//				(uint)Processor.CPU_Core.InstructionsEnum.VDIV => true,
	//				(uint)Processor.CPU_Core.InstructionsEnum.VMOD => true,
	//				(uint)Processor.CPU_Core.InstructionsEnum.VXOR => true,
	//				(uint)Processor.CPU_Core.InstructionsEnum.VOR => true,
	//				(uint)Processor.CPU_Core.InstructionsEnum.VAND => true,
	//
	//				// Shift operations (binary)
	//				(uint)Processor.CPU_Core.InstructionsEnum.VSLL => true,
	//				(uint)Processor.CPU_Core.InstructionsEnum.VSRL => true,
	//				(uint)Processor.CPU_Core.InstructionsEnum.VSRA => true,
	//
	//				// Min/Max operations (binary)
	//				(uint)Processor.CPU_Core.InstructionsEnum.VMIN => true,
	//				(uint)Processor.CPU_Core.InstructionsEnum.VMAX => true,
	//				(uint)Processor.CPU_Core.InstructionsEnum.VMINU => true,
	//				(uint)Processor.CPU_Core.InstructionsEnum.VMAXU => true,
	//
	//				// Unary operations
	//				(uint)Processor.CPU_Core.InstructionsEnum.VSQRT => false,
	//				(uint)Processor.CPU_Core.InstructionsEnum.VNOT => false,
	//				(uint)Processor.CPU_Core.InstructionsEnum.VREVERSE => false,
	//				(uint)Processor.CPU_Core.InstructionsEnum.VPOPCNT => false,
	//				(uint)Processor.CPU_Core.InstructionsEnum.VCLZ => false,
	//				(uint)Processor.CPU_Core.InstructionsEnum.VCTZ => false,
	//				(uint)Processor.CPU_Core.InstructionsEnum.VBREV8 => false,
	//
	//				// Fail-safe default: only explicitly listed in-place two-source contours
	//				// are allowed to reuse the generic binary execution path.
	//				_ => false
	//			};
		switch (opCode)
		{
		case static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VADD):
		case static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VSUB):
		case static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VMUL):
		case static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VDIV):
		case static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VMOD):
		case static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VXOR):
		case static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VOR):
		case static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VAND):
		case static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VSLL):
		case static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VSRL):
		case static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VSRA):
		case static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VMIN):
		case static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VMAX):
		case static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VMINU):
		case static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VMAXU):
			return true;
		default:
			return false;
		}
	}

	inline bool StreamEngine::IsGenericInPlaceVectorComputeOp(unsigned int opCode)
	{
		std::optional<OpcodeInfo> info = OpcodeRegistry::GetInfo(opCode);
		if (!info || (static_cast<int>(info.value().Flags) & static_cast<int>(InstructionFlags::Vector)) == 0)
		{
			return false;
		}

		if (info.value().Category != OpcodeCategory::Vector &&
			info.value().Category != OpcodeCategory::BitManip)
		{
			return false;
		}

		if (OpcodeRegistry::IsWidenNarrowConvertOp(opCode))
		{
			return false;
		}

		if (OpcodeRegistry::IsScanOp(opCode))
		{
			return false;
		}

		if (opCode == static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VLOAD) ||
			opCode == static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VSTORE) ||
			opCode == static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VGATHER) ||
			opCode == static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VSCATTER))
		{
			return false;
		}

		return !IsComparisonOp(opCode) && !IsMaskManipOp(opCode) && !IsFMAOp(opCode) && !IsReductionOp(opCode) && !IsScanOp(opCode) && !IsDotProductOp(opCode) && !IsPredicativeMovementOp(opCode) && !IsPermutationOp(opCode) && !IsSlideOp(opCode);
	}

	inline bool StreamEngine::IsComparisonOp(unsigned int opCode)
	{
		// Table-driven — replaces VCMPEQ–VCMPGE range check (B9).
		return OpcodeRegistry::IsComparisonOp(opCode);
	}

	inline bool StreamEngine::IsMaskManipOp(unsigned int opCode)
	{
		// Table-driven — replaces VMAND–VPOPC range check (B9).
		return OpcodeRegistry::IsMaskManipOp(opCode);
	}

	inline TriOpDesc StreamEngine::ReadTriOpDesc(unsigned long long descriptorAddr)
	{
		std::array<unsigned char, 20> descStorage{};
		std::span<unsigned char> descBuf(descStorage);
		BurstIO::BurstRead(descriptorAddr, descBuf, 1, 20, 20);

		TriOpDesc desc;
		desc.SrcA = ReadLittleEndian<unsigned long long>(CheckedSubspan(descBuf, 0, 8));
		desc.SrcB = ReadLittleEndian<unsigned long long>(CheckedSubspan(descBuf, 8, 8));
		desc.StrideA = ReadLittleEndian<unsigned short>(CheckedSubspan(descBuf, 16, 2));
		desc.StrideB = ReadLittleEndian<unsigned short>(CheckedSubspan(descBuf, 18, 2));
		return desc;
	}

	inline bool StreamEngine::IsFMAOp(unsigned int opCode)
	{
		// Table-driven — replaces VFMADD–VFNMSUB range check (B9).
		return OpcodeRegistry::IsFmaOp(opCode);
	}

	inline bool StreamEngine::IsReductionOp(unsigned int opCode)
	{
		// Table-driven — replaces VREDSUM–VREDXOR range check (B9).
		return OpcodeRegistry::IsReductionOp(opCode);
	}

	inline bool StreamEngine::IsScanOp(unsigned int opCode)
	{
		return OpcodeRegistry::IsScanOp(opCode);
	}

	inline bool StreamEngine::IsDotProductOp(unsigned int opCode)
	{
		return opCode == static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VDOT) || opCode == static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VDOTU) || opCode == static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VDOTF) || opCode == static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VDOT_FP8) || opCode == static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VDOT_WIDE);
	}

	inline bool StreamEngine::IsPredicativeMovementOp(unsigned int opCode)
	{
		return opCode == static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VCOMPRESS) || opCode == static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VEXPAND);
	}

	inline bool StreamEngine::IsPermutationOp(unsigned int opCode)
	{
		return opCode == static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VPERMUTE) || opCode == static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VRGATHER);
	}

	inline bool StreamEngine::IsSlideOp(unsigned int opCode)
	{
		return opCode == static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VSLIDEUP) || opCode == static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VSLIDEDOWN);
	}

	void StreamEngine::Execute(std::shared_ptr<Processor::CPU_Core> &core, VLIW_Instruction const inst, int ownerThreadId)
	{
		StreamExecutionRequest request = StreamExecutionRequest::CreateValidatedCompatIngress(inst);
		Execute(core, request, ownerThreadId);
	}

	void StreamEngine::Execute(std::shared_ptr<Processor::CPU_Core> &core, StreamExecutionRequest const request, int ownerThreadId)
	{
		unsigned long long streamLength = request.getStreamLength();
		if (streamLength == 0)
		{
			throw InvalidOperationException(std::format("Stream opcode 0x{0:X} reached StreamEngine.Execute(...) with StreamLength == 0 without an authoritative mainline execute/retire contour. ", request.getOpCode()) + "Mainline vector callers must not rely on an inner StreamEngine no-op for zero-length requests.");
		}

		unsigned int opCode = request.getOpCode();
		DataTypeEnum dataType = request.getDataTypeValue();
		unsigned char predIndex = request.getPredicateMask();

		int executionVtId = ResolveExecutionVtId(core, ownerThreadId);
		ThrowIfUnsupportedRawTransferVectorContour(request, opCode);
		ThrowIfUnsupportedRawSlideOneContour(request, opCode);
		ThrowIfUnsupportedRawWideDotContour(request, opCode);
		ThrowIfUnsupportedRawPerm2Contour(request, opCode);
		ThrowIfUnsupportedRawTransposeContour(request, opCode);
		ThrowIfUnsupportedNonRepresentableVectorAddressing(request, opCode);
		ThrowIfUnsupportedRawDescriptorlessFmaContour(request, opCode);
		ThrowIfUnsupportedRawWidenNarrowConvertContour(request, opCode);
		ThrowIfUnsupportedRawScanContour(request, opCode);
		ThrowIfUnsupportedRawSaturatingPolicyContour(request, opCode);

		// --- Scalar register path (StreamLength = 1, register IDs in Word1) ---
		if (streamLength == 1 && request.isScalar())
		{
			// HLS Note: Scalar operations take EXECUTE_LATENCY_MIN cycles (1 cycle)
			ThrowIfUnsupportedScalarRegisterStreamOpcode(opCode);
			ExecuteScalarRegister(core, request, opCode, dataType, predIndex, executionVtId);
			return;
		}

		if (IsMaskManipOp(opCode))
		{
			ExecuteMaskManipulation(core, request, opCode, executionVtId);
			return;
		}

		int elemSize = ResolveRequiredElementSizeOrThrow(opCode, dataType);

		// --- Vector Memory-to-Memory path ---
		// HLS Note: Vector operations take variable cycles based on:
		// - Number of elements (streamLength)
		// - Memory access latency (MEMORY_LATENCY per burst)
		// - ALU operation latency (EXECUTE_LATENCY_MIN per element)
		ExecuteVectorMemory(core, request, opCode, dataType, predIndex, elemSize, streamLength, executionVtId);
	}

	void StreamEngine::CaptureRetireWindowPublications(std::shared_ptr<Processor::CPU_Core> &core, VLIW_Instruction const inst, Processor::CPU_Core::RetireWindowBatch &retireBatch, int ownerThreadId)
	{
		StreamExecutionRequest request = StreamExecutionRequest::CreateValidatedCompatIngress(inst);
		CaptureRetireWindowPublications(core, request, retireBatch, ownerThreadId);
	}

	void StreamEngine::CaptureRetireWindowPublications(std::shared_ptr<Processor::CPU_Core> &core, StreamExecutionRequest const request, Processor::CPU_Core::RetireWindowBatch &retireBatch, int ownerThreadId)
	{
		CaptureRetireWindowPublicationsCore(core, request, retireBatch, ownerThreadId, L"direct stream retire-window publication");
	}

	inline void StreamEngine::CaptureRetireWindowPublicationsCore(std::shared_ptr<Processor::CPU_Core> &core, StreamExecutionRequest const request, Processor::CPU_Core::RetireWindowBatch &retireBatch, int ownerThreadId, const std::wstring &executionContour)
	{
		unsigned long long streamLength = request.getStreamLength();
		if (streamLength == 0)
		{
			throw InvalidOperationException(std::format("Stream opcode 0x{0:X} reached {1} with StreamLength == 0 without an authoritative retire/apply contour. ", request.getOpCode(), ToNarrow(executionContour)) + "Direct callers must not treat zero-length direct stream requests as implicit success/no-op; use the canonical mainline path or materialize an explicit retire record before reopening this contour.");
		}

		unsigned int opCode = request.getOpCode();
		int executionVtId = ResolveExecutionVtId(core, ownerThreadId);
		ThrowIfUnsupportedDirectRetireVectorAddressing(request, opCode, executionContour);
		ThrowIfUnsupportedRawSaturatingPolicyContour(request, opCode);

		if (opCode == static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VPOPC))
		{
			CaptureVpopcRetireWindowPublication(core, request, retireBatch, executionVtId);
			return;
		}

		if (!request.isScalar() && (IsComparisonOp(opCode) || (IsMaskManipOp(opCode) && opCode != static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VPOPC))))
		{
			CapturePredicateStateRetireWindowPublication(core, request, retireBatch, opCode, request.getDataTypeValue(), request.getPredicateMask(), executionVtId);
			return;
		}

		if (streamLength == 1 && request.isScalar())
		{
			ThrowIfUnsupportedScalarRegisterStreamOpcode(opCode);
			CaptureScalarRegisterRetireWindowPublication(core, request, retireBatch, opCode, request.getPredicateMask(), executionVtId);
			return;
		}

		throw InvalidOperationException(std::format("{0} is only authoritative for scalar-writing or predicate-writing stream instructions that materialize explicit retire/apply follow-through.", ToNarrow(executionContour)));
	}

	inline void StreamEngine::ExecuteScalarRegister(std::shared_ptr<Processor::CPU_Core> &core, StreamExecutionRequest const request, unsigned int opCode, DataTypeEnum dataType, unsigned char predIndex, int executionVtId)
	{
		(void)dataType;
		ThrowIfUnsupportedScalarRegisterStreamOpcode(opCode);

		RetireRecord retireRecord;
		if (TryResolveScalarRegisterRetireRecord(core, request, opCode, predIndex, executionVtId, retireRecord))
		{
			PublishScalarRetireRegisterWrite(core, retireRecord);
		}
	}

	inline void StreamEngine::ExecuteVectorMemory(std::shared_ptr<Processor::CPU_Core> &core, StreamExecutionRequest const request, unsigned int opCode, DataTypeEnum dataType, unsigned char predIndex, int elemSize, unsigned long long streamLength, int executionVtId)
	{
		// Check for indexed addressing mode (gather/scatter)
		if (request.getIndexed())
		{
			ExecuteIndexed(core, request, opCode, dataType, predIndex, elemSize, streamLength);
			return;
		}

		// Check for 2D addressing mode
		if (request.getIs2D())
		{
			Execute2D(core, request, opCode, dataType, predIndex, elemSize, streamLength);
			return;
		}

		if (IsPredicativeMovementOp(opCode))
		{
			ExecutePredicativeMovement1D(core, request, opCode, dataType, predIndex, elemSize, streamLength);
			return;
		}

		// Default: 1D strided addressing
		// Use double-buffered path when pipeline mode is enabled and op is
		// a simple binary/unary (not FMA, reduction, comparison, mask, etc.)
		if (core->GetPipelineControl().Enabled && !IsFMAOp(opCode) && !IsReductionOp(opCode) && !IsComparisonOp(opCode) && !IsMaskManipOp(opCode) && !IsDotProductOp(opCode) && !IsPredicativeMovementOp(opCode) && !IsPermutationOp(opCode) && !IsSlideOp(opCode) && !OpcodeRegistry::IsWidenNarrowConvertOp(opCode))
		{
			Execute1D_DoubleBuffered(core, request, opCode, dataType, predIndex, elemSize, streamLength);
			return;
		}

		Execute1D(core, request, opCode, dataType, predIndex, elemSize, streamLength, executionVtId);
	}

	inline int StreamEngine::ResolveRequiredElementSizeOrThrow(unsigned int opCode, DataTypeEnum dataType)
	{
		if (!DataTypeUtils::IsValid(dataType))
		{
			throw ExecutionFaultContract::CreateUnsupportedVectorElementTypeException(ToWide(std::format("Stream/vector opcode 0x{0:X} reached StreamEngine.Execute(...) with unsupported element DataType '{1}' (raw 0x{2:02X}) on an element-sized execution contour. ", opCode, static_cast<unsigned int>(dataType), static_cast<unsigned char>(dataType)) + "Element-sized vector/stream execution must fail closed instead of collapsing into hidden success/no-op for an invalid data type."));
		}

		int elemSize = DataTypeUtils::SizeOf(dataType);
		if (elemSize != 0)
		{
			return elemSize;
		}

		if (IsDotProductOp(opCode) && (dataType == DataTypeEnum::FLOAT8_E4M3 || dataType == DataTypeEnum::FLOAT8_E5M2))
		{
			// Dedicated FP8 dot-product contours already widen through VectorALU.ApplyDotProduct(...).
			// Keep the raw StreamEngine gate narrow so only the authoritative dot-product family
			// reuses 1-byte element sizing instead of reopening broad generic FP8 compat execution.
			return 1;
		}

		throw ExecutionFaultContract::CreateUnsupportedVectorElementTypeException(ToWide(std::format("Stream/vector opcode 0x{0:X} reached StreamEngine.Execute(...) with unsupported element DataType '{1}' (raw 0x{2:02X}) on an element-sized execution contour. ", opCode, static_cast<unsigned int>(dataType), static_cast<unsigned char>(dataType)) + "Element-sized vector/stream execution must fail closed instead of collapsing into hidden success/no-op for an invalid data type."));
	}

	inline void StreamEngine::ExecuteMaskManipulation(std::shared_ptr<Processor::CPU_Core> &core, StreamExecutionRequest const request, unsigned int opCode, int executionVtId)
	{
		unsigned char srcPred1 = static_cast<unsigned char>(request.getImmediate() & 0x0F);
		unsigned char destPred = ResolveMaskManipulationDestinationPredicateRegister(request);
		unsigned long long mask1 = core->GetPredicateRegister(srcPred1);

		if (opCode == static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VPOPC))
		{
			unsigned long long vl = std::min<unsigned long long>(request.getStreamLength(), Processor::CPU_Core::RVV_Config::VLMAX);
			unsigned long long popCount = VectorALU::MaskPopCount(mask1, vl);
			if (destPred < YAKSys_Hybrid_CPU::Core::Registers::RenameMap::ArchRegs)
			{
				PublishScalarRetireRegisterWrite(core, executionVtId, destPred, popCount);
			}

			return;
		}

		core->SetPredicateRegister(destPred, ResolveMaskManipulationResultMask(core, request, opCode));
	}

	inline void StreamEngine::ThrowIfUnsupportedNonRepresentableVectorAddressing(StreamExecutionRequest const request, unsigned int opCode)
	{
		if (!request.getIndexed() && !request.getIs2D())
		{
			return;
		}

		std::wstring addressingContour = DescribeNonRepresentableVectorAddressingContour(request);
		if (IsMaskManipOp(opCode))
		{
			throw InvalidOperationException(std::format("Stream/vector opcode 0x{0:X} reached StreamEngine.Execute(...) with unsupported {1} predicate-mask addressing. ", opCode, ToNarrow(addressingContour)) + "Raw predicate-state execution does not materialize any addressed vector surface and must fail closed instead of hidden compat success/no-op.");
		}

		if (IsComparisonOp(opCode))
		{
			throw InvalidOperationException(std::format("Stream/vector opcode 0x{0:X} reached StreamEngine.Execute(...) with unsupported {1} vector-comparison addressing. ", opCode, ToNarrow(addressingContour)) + "Raw comparison fallback only follows through 1D predicate-publication truth and must fail closed instead of hidden compat success.");
		}

		if (IsFMAOp(opCode))
		{
			throw InvalidOperationException(std::format("Stream/vector opcode 0x{0:X} reached StreamEngine.Execute(...) with unsupported {1} vector-FMA addressing. ", opCode, ToNarrow(addressingContour)) + "Raw execution must fail closed instead of collapsing indexed/2D tri-operand requests into generic compute fallback while no authoritative mainline follow-through exists.");
		}

		if (IsDotProductOp(opCode))
		{
			throw InvalidOperationException(std::format("Stream/vector opcode 0x{0:X} reached StreamEngine.Execute(...) with unsupported {1} vector-dot-product addressing. ", opCode, ToNarrow(addressingContour)) + "Raw dot-product execution only follows through 1D scalar-result truth and must fail closed instead of hidden compat success.");
		}

		if (IsPredicativeMovementOp(opCode))
		{
			throw InvalidOperationException(std::format("Stream/vector opcode 0x{0:X} reached StreamEngine.Execute(...) with unsupported {1} vector-predicative-movement addressing. ", opCode, ToNarrow(addressingContour)) + "Raw predicative-movement execution only follows through 1D single-surface truth and must fail closed instead of hidden compat success.");
		}

		if (IsReductionOp(opCode))
		{
			throw InvalidOperationException(std::format("Stream/vector opcode 0x{0:X} reached StreamEngine.Execute(...) with unsupported {1} vector-reduction addressing. ", opCode, ToNarrow(addressingContour)) + "Raw reduction execution only follows through 1D scalar-footprint truth and must fail closed instead of hidden compat success.");
		}

		if (IsScanOp(opCode))
		{
			throw InvalidOperationException(std::format("Stream/vector opcode 0x{0:X} reached StreamEngine.Execute(...) with unsupported {1} vector-scan addressing. ", opCode, ToNarrow(addressingContour)) + "Phase 05A scan execution only follows through packed 1D staged-writeback truth and must fail closed instead of hidden compat success.");
		}

		if (IsPermutationOp(opCode))
		{
			throw InvalidOperationException(std::format("Stream/vector opcode 0x{0:X} reached StreamEngine.Execute(...) with unsupported {1} vector-permutation addressing. ", opCode, ToNarrow(addressingContour)) + "Raw permutation execution only follows through 1D two-surface truth and must fail closed instead of hidden compat success.");
		}

		if (IsSlideOp(opCode))
		{
			throw InvalidOperationException(std::format("Stream/vector opcode 0x{0:X} reached StreamEngine.Execute(...) with unsupported {1} vector-slide addressing. ", opCode, ToNarrow(addressingContour)) + "Raw slide execution only follows through 1D single-surface truth and must fail closed instead of hidden compat success.");
		}

		if (IsGenericInPlaceVectorComputeOp(opCode))
		{
			std::wstring computeContour = IsBinaryOp(opCode) ? L"vector-binary" : L"vector-unary";
			throw InvalidOperationException(std::format("Stream/vector opcode 0x{0:X} reached StreamEngine.Execute(...) with unsupported {1} {2} addressing. ", opCode, ToNarrow(addressingContour), ToNarrow(computeContour)) + "Raw in-place compute execution only follows through 1D memory-shape truth and must fail closed instead of hidden compat success.");
		}
	}

	inline void StreamEngine::ThrowIfUnsupportedRawTransferVectorContour(StreamExecutionRequest const request, unsigned int opCode)
	{
		if (request.isScalar())
		{
			return;
		}

		if (opCode != static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VLOAD) &&
			opCode != static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VSTORE))
		{
			return;
		}

		throw InvalidOperationException(std::format("Stream/vector opcode 0x{0:X} reached StreamEngine.Execute(...) on the legacy raw VLOAD/VSTORE contour. ", opCode) + "Dedicated transfer opcodes must fail closed on this raw stream engine surface instead of collapsing into generic compute-style memory traffic or hidden compat success.");
	}

	inline void StreamEngine::ThrowIfUnsupportedDirectRetireVectorAddressing(StreamExecutionRequest const request, unsigned int opCode, const std::wstring &executionContour)
	{
		if (!request.getIndexed() && !request.getIs2D())
		{
			return;
		}

		std::wstring addressingContour = DescribeNonRepresentableVectorAddressingContour(request);
		if (opCode == static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VPOPC))
		{
			throw InvalidOperationException(std::format("Stream opcode 0x{0:X} reached {1} with unsupported {2} vector-mask-popcount addressing. ", opCode, ToNarrow(executionContour), ToNarrow(addressingContour)) + "Direct VPOPC helper flow only publishes predicate-to-scalar retire truth and must fail closed instead of reopening an addressing-tag compat surface.");
		}

		if (IsComparisonOp(opCode))
		{
			throw InvalidOperationException(std::format("Stream opcode 0x{0:X} reached {1} with unsupported {2} vector-comparison addressing. ", opCode, ToNarrow(executionContour), ToNarrow(addressingContour)) + "Direct helper follow-through for `VCMP*` only publishes 1D predicate-state truth and must fail closed instead of reopening a non-representable compat surface.");
		}

		if (IsMaskManipOp(opCode))
		{
			throw InvalidOperationException(std::format("Stream opcode 0x{0:X} reached {1} with unsupported {2} vector-mask addressing. ", opCode, ToNarrow(executionContour), ToNarrow(addressingContour)) + "Direct helper follow-through for `VM*` only publishes predicate-state truth and must fail closed instead of reopening an addressing-tag compat surface.");
		}
	}

	inline void StreamEngine::ThrowIfUnsupportedRawDescriptorlessFmaContour(StreamExecutionRequest const request, unsigned int opCode)
	{
		if (!IsFMAOp(opCode) || request.getIndexed() || request.getIs2D() || request.getSrc2Pointer() != 0)
		{
			return;
		}

		throw InvalidOperationException(std::format("Stream/vector opcode 0x{0:X} reached StreamEngine.Execute(...) on a descriptor-less raw tri-operand FMA contour. ", opCode) + "The authoritative mainline FMA carrier already fails closed when the third source descriptor is missing, so raw StreamEngine callers must reject this legacy Immediate-address fallback instead of publishing hidden success on a false 1D contour.");
	}

	inline void StreamEngine::ThrowIfUnsupportedRawWidenNarrowConvertContour(StreamExecutionRequest const request, unsigned int opCode)
	{
		if (!OpcodeRegistry::IsWidenNarrowConvertOp(opCode))
		{
			return;
		}

		throw InvalidOperationException(std::format("Stream/vector opcode 0x{0:X} reached raw StreamEngine.Execute(...) on a widen/narrow/convert contour. ", opCode) + "Phase 05 VZEXT is executable only through the typed VectorZeroExtendMicroOp staging/writeback path; raw stream fallback must fail closed instead of publishing early memory writes.");
	}

	inline void StreamEngine::ThrowIfUnsupportedRawScanContour(StreamExecutionRequest const request, unsigned int opCode)
	{
		if (!OpcodeRegistry::IsScanOp(opCode))
		{
			return;
		}

		throw InvalidOperationException(std::format("Stream/vector opcode 0x{0:X} reached raw StreamEngine.Execute(...) on a vector scan contour. ", opCode) + "Phase 05A VSCAN.SUM is executable only through the typed VectorScanSumMicroOp staging/writeback path; raw stream fallback must fail closed instead of publishing early memory writes.");
	}

	inline void StreamEngine::ThrowIfUnsupportedRawSlideOneContour(StreamExecutionRequest const request, unsigned int opCode)
	{
		if (opCode != static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VSLIDE1UP) && opCode != static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VSLIDE1DOWN))
		{
			return;
		}

		std::wstring mnemonic = opCode == static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VSLIDE1DOWN) ? L"VSLIDE1DOWN" : L"VSLIDE1UP";
		std::wstring microOpName = opCode == static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VSLIDE1DOWN) ? L"VectorSlideOneDownMicroOp" : L"VectorSlideOneUpMicroOp";
		throw InvalidOperationException(std::format("Stream/vector opcode 0x{0:X} reached raw StreamEngine.Execute(...) on a Phase 09 slide-one contour. ", opCode) + std::format("{0} is executable only through the typed {1} staging/writeback path; raw stream fallback must fail closed instead of publishing early memory writes.", ToNarrow(mnemonic), ToNarrow(microOpName)));
	}

	inline void StreamEngine::ThrowIfUnsupportedRawWideDotContour(StreamExecutionRequest const request, unsigned int opCode)
	{
		if (opCode != static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VDOT_WIDE))
		{
			return;
		}

		throw InvalidOperationException(std::format("Stream/vector opcode 0x{0:X} reached raw StreamEngine.Execute(...) on a Phase 09 wide-dot contour. ", opCode) + "VDOT.WIDE is executable only through the typed VectorDotWideMicroOp staging/writeback path; raw stream fallback must fail closed instead of publishing early memory writes.");
	}

	inline void StreamEngine::ThrowIfUnsupportedRawPerm2Contour(StreamExecutionRequest const request, unsigned int opCode)
	{
		if (opCode != static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VPERM2))
		{
			return;
		}

		throw InvalidOperationException(std::format("Stream/vector opcode 0x{0:X} reached raw StreamEngine.Execute(...) on a Phase 09 permute2 contour. ", opCode) + "VPERM2 is executable only through the typed VectorPermute2MicroOp staging/writeback path; raw stream fallback must fail closed instead of publishing early memory writes.");
	}

	inline void StreamEngine::ThrowIfUnsupportedRawTransposeContour(StreamExecutionRequest const request, unsigned int opCode)
	{
		if (opCode != static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VTRANSPOSE))
		{
			return;
		}

		throw InvalidOperationException(std::format("Stream/vector opcode 0x{0:X} reached raw StreamEngine.Execute(...) on a Phase 09 transpose contour. ", opCode) + "VTRANSPOSE is executable only through the typed VectorTransposeMicroOp staging/writeback path; raw stream fallback must fail closed instead of publishing early memory writes.");
	}

	inline void StreamEngine::ThrowIfUnsupportedRawSaturatingPolicyContour(StreamExecutionRequest const request, unsigned int opCode)
	{
		if (!request.getSaturating())
		{
			return;
		}

		throw InvalidOperationException(std::format("Stream/vector opcode 0x{0:X} reached raw StreamEngine.Execute(...) with the saturating policy bit set. ", opCode) + "Phase 05B VADD.SAT is executable only through the typed VectorSaturatingAddMicroOp staging/writeback path; raw stream fallback must fail closed instead of silently publishing non-saturating writes.");
	}

	inline void StreamEngine::ThrowIfUnavailableRawScratchContour(unsigned int opCode, const std::wstring &executeContour, bool requiresScratchB, int scratchALength, int scratchBLength, int scratchDstLength, int scratchIndexLength)
	{
		bool hasPrimaryScratch = scratchALength != 0 && scratchDstLength != 0;
		bool hasSecondaryScratch = !requiresScratchB || scratchBLength != 0;
		bool hasIndexScratch = scratchIndexLength < 0 || scratchIndexLength != 0;
		if (hasPrimaryScratch && hasSecondaryScratch && hasIndexScratch)
		{
			return;
		}

		std::wstring scratchSurface = scratchIndexLength >= 0 ? L"preallocated data/index scratch surfaces" : L"preallocated data scratch surfaces";
		throw InvalidOperationException(std::format("Stream/vector opcode 0x{0:X} reached {1} without initialized scratch buffers. ", opCode, ToNarrow(executeContour)) + std::format("Raw execution must fail closed instead of publishing hidden success/no-op or opaque slice faults when the {0} are unavailable.", ToNarrow(scratchSurface)));
	}

	inline void StreamEngine::ThrowIfUnsupportedZeroRowLength2DContour(unsigned int opCode, unsigned long long streamLength, unsigned int rowLength, const std::wstring &executeContour)
	{
		if (streamLength == 0 || rowLength != 0)
		{
			return;
		}

		throw InvalidOperationException(std::format("Stream/vector opcode 0x{0:X} reached {1} with rowLength == 0 on a non-zero 2D request. ", opCode, ToNarrow(executeContour)) + "2D vector/stream execution must fail closed instead of collapsing into hidden success/no-op when row geometry is non-representable.");
	}

	inline std::wstring StreamEngine::DescribeNonRepresentableVectorAddressingContour(StreamExecutionRequest const request)
	{
		return request.getIndexed() && request.getIs2D() ? L"indexed+2D" : request.getIndexed() ? L"indexed" : L"2D";
	}

	void StreamEngine::ExecutePredicativeMovement1D(std::shared_ptr<Processor::CPU_Core> &core, StreamExecutionRequest const request, unsigned int opCode, DataTypeEnum dataType, unsigned char predIndex, int elemSize, unsigned long long streamLength)
	{
		unsigned long long ptrDestSrc1 = request.getDestSrc1Pointer();
		unsigned short stride = request.getStride();
		if (stride == 0)
		{
			stride = static_cast<unsigned short>(elemSize);
		}

		std::span<unsigned char> scratchA = core->GetScratchA();
		std::span<unsigned char> scratchDst = core->GetScratchDst();

		ThrowIfUnavailableRawScratchContour(opCode, L"raw 1D stream/vector execution", false, scratchA.size(), 0, scratchDst.size(), -1);
		ThrowIfUnsupportedRawPredicativeMovementBufferFootprint(opCode, streamLength, elemSize, scratchA.size(), scratchDst.size());

// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'checked' in this context:
// ORIGINAL LINE: int byteCount = checked((int)(streamLength * (ulong)elemSize));
		int byteCount = static_cast<int>(streamLength * static_cast<unsigned long long>(elemSize));
		std::span<unsigned char> bufSrc = CheckedSubspan(scratchA, 0, byteCount);
		std::span<unsigned char> bufDst = CheckedSubspan(scratchDst, 0, byteCount);

		BurstIO::BurstRead(ptrDestSrc1, bufSrc, streamLength, elemSize, stride);

		// Raw 1D predicative movement now follows the authoritative single-surface carrier:
		// seed destination before compaction/expansion so untouched lanes stay truthful.
		std::copy(bufSrc.begin(), bufSrc.end(), bufDst.begin());

		if (opCode == static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VCOMPRESS))
		{
			VectorALU::ApplyCompress(dataType, bufSrc, bufDst, elemSize, streamLength, predIndex, core);
		}
		else
		{
			VectorALU::ApplyExpand(dataType, bufSrc, bufDst, elemSize, streamLength, predIndex, request.getTailAgnostic(), request.getMaskAgnostic(), core);
		}

		BurstIO::BurstWrite(ptrDestSrc1, bufDst, streamLength, elemSize, stride);
	}

	void StreamEngine::ThrowIfUnsupportedRawPredicativeMovementBufferFootprint(unsigned int opCode, unsigned long long streamLength, int elemSize, int scratchALength, int scratchDstLength)
	{
		unsigned long long maxRepresentableElements = static_cast<unsigned long long>(std::min(scratchALength / elemSize, scratchDstLength / elemSize));

		if (streamLength <= maxRepresentableElements)
		{
			return;
		}

		throw InvalidOperationException(std::format("Stream/vector opcode 0x{0:X} reached raw 1D stream/vector execution with StreamLength {1} on the predicative-movement contour, but the truthful in-place VCOMPRESS/VEXPAND follow-through currently materializes the full source/destination surface through scratch-backed execution. ", opCode, streamLength) + std::format("Raw execution only represents up to {0} element(s) at the current element width and must fail closed instead of publishing partial compaction/expansion truth.", maxRepresentableElements));
	}

	void StreamEngine::Execute1D(std::shared_ptr<Processor::CPU_Core> &core, StreamExecutionRequest const request, unsigned int opCode, DataTypeEnum dataType, unsigned char predIndex, int elemSize, unsigned long long streamLength, int executionVtId)
	{
		unsigned long long ptrDestSrc1 = request.getDestSrc1Pointer();
		unsigned long long ptrSrc2 = request.getSrc2Pointer();
		unsigned short stride = request.getStride();

		// Default stride: packed (element size)
		if (stride == 0)
		{
			stride = static_cast<unsigned short>(elemSize);
		}

		// Hardware vector length (VLMAX) - from RVV_Config
		constexpr unsigned long long VLMAX = 32;

		// Get scratch buffers from core
		std::span<unsigned char> scratchA = core->GetScratchA();
		std::span<unsigned char> scratchB = core->GetScratchB();
		std::span<unsigned char> scratchDst = core->GetScratchDst();

		ThrowIfUnavailableRawScratchContour(opCode, L"raw 1D stream/vector execution", true, scratchA.size(), scratchB.size(), scratchDst.size(), -1);

		unsigned long long remaining = streamLength;
		bool isComparisonOp = IsComparisonOp(opCode);
		unsigned char comparisonDestPredReg = 0;
		unsigned long long accumulatedComparisonResultMask = 0;
		if (isComparisonOp)
		{
			comparisonDestPredReg = ResolveComparisonDestinationPredicateRegister(request);
		}

		// Strip-mining loop: process VLMAX elements per iteration
		while (remaining > 0)
		{
			// Compute vector length for this iteration
			unsigned long long vl = remaining < VLMAX ? remaining : VLMAX;

			// Clamp vl to fit within scratch buffer capacity
			unsigned long long maxVL = static_cast<unsigned long long>(scratchA.size() / elemSize);
			if (vl > maxVL)
			{
				vl = maxVL;
			}

			// Handle mask manipulation instructions (operate on predicate registers)
			if (IsMaskManipOp(opCode))
			{
				// Mask operations work on predicate registers directly, no memory access
				// Source predicate registers encoded in instruction fields
				unsigned char srcPred1 = static_cast<unsigned char>(request.getImmediate() & 0x0F); // Bits 0-3: src1 predicate reg
				unsigned char destPred = ResolveMaskManipulationDestinationPredicateRegister(request); // Bits 8-11: dest predicate reg

				unsigned long long mask1 = core->GetPredicateRegister(srcPred1);

				// Population count (VPOPC_M) - special case, writes to scalar register
				if (opCode == static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VPOPC))
				{
					unsigned long long popCount = VectorALU::MaskPopCount(mask1, vl);
					// Write result to scalar register (destPred used as register index)
					if (destPred < YAKSys_Hybrid_CPU::Core::Registers::RenameMap::ArchRegs)
					{
						PublishScalarRetireRegisterWrite(core, executionVtId, destPred, popCount);
					}
					break; // VPOPC doesn't iterate, just counts once
				}

				// Write result mask to destination predicate register
				core->SetPredicateRegister(destPred, ResolveMaskManipulationResultMask(core, request, opCode));

				// Mask operations don't iterate (operate on 64-bit mask directly)
				break;
			}
			// Handle comparison instructions separately (generate predicate mask)
			else if (IsComparisonOp(opCode))
			{
				// Read both source operands
				unsigned long long bytesToRead = vl * static_cast<unsigned long long>(elemSize);
				std::span<unsigned char> bufA = CheckedSubspan(scratchA, 0, static_cast<int>(bytesToRead));
				std::span<unsigned char> bufB = CheckedSubspan(scratchB, 0, static_cast<int>(bytesToRead));

				// Burst read source operands
				BurstIO::BurstRead(ptrDestSrc1, bufA, vl, elemSize, stride);
				BurstIO::BurstRead(ptrSrc2, bufB, vl, elemSize, stride);

				// Execute comparison and generate mask
				unsigned long long resultMask = VectorALU::ApplyComparison(opCode, dataType, bufA, bufB, elemSize, vl, predIndex, core);

				AccumulateComparisonPredicateChunk(accumulatedComparisonResultMask, resultMask, streamLength - remaining);
			}
			// Handle dot-product instructions (ML/DSP optimized)
			else if (IsDotProductOp(opCode))
			{
				// The opcode registry intentionally tags VDOT* as reduction-family ops for
				// structural/resource taxonomy, but the raw execute contour must still reach
				// the dedicated two-source dot-product datapath instead of generic unary
				// reduction semantics.
				// Read both source operands
				unsigned long long bytesToRead = vl * static_cast<unsigned long long>(elemSize);
				std::span<unsigned char> bufA = CheckedSubspan(scratchA, 0, static_cast<int>(bytesToRead));
				std::span<unsigned char> bufB = CheckedSubspan(scratchB, 0, static_cast<int>(bytesToRead));
				int resultElemSize = AddressGen::IsWideningOpcode(opCode) && (dataType == DataTypeEnum::FLOAT8_E4M3 || dataType == DataTypeEnum::FLOAT8_E5M2) ? sizeof(float) : elemSize;
				std::span<unsigned char> bufDst = CheckedSubspan(scratchDst, 0, resultElemSize); // Only 1 scalar result

				// Burst read operands
				BurstIO::BurstRead(ptrDestSrc1, bufA, vl, elemSize, stride);
				BurstIO::BurstRead(ptrSrc2, bufB, vl, elemSize, stride);

				// Execute dot-product operation (reduce to scalar)
				VectorALU::ApplyDotProduct(opCode, dataType, bufA, bufB, bufDst, elemSize, vl, predIndex, core);

				// Write scalar result back to first element of destination
				BurstIO::BurstWrite(ptrDestSrc1, bufDst, 1, resultElemSize, stride);

				// Dot-product only processes one chunk
				break;
			}
			// Handle reduction instructions (collapse vector to scalar)
			else if (IsReductionOp(opCode))
			{
				// Read source operand
				unsigned long long bytesToRead = vl * static_cast<unsigned long long>(elemSize);
				std::span<unsigned char> bufSrc = CheckedSubspan(scratchA, 0, static_cast<int>(bytesToRead));
				std::span<unsigned char> bufDst = CheckedSubspan(scratchDst, 0, elemSize); // Only 1 element result

				// Burst read operand
				BurstIO::BurstRead(ptrDestSrc1, bufSrc, vl, elemSize, stride);

				// Execute reduction operation (all elements -> single result)
				VectorALU::ApplyReduction(opCode, dataType, bufSrc, bufDst, elemSize, vl, predIndex, core);

				// Write scalar result back to first element of destination
				BurstIO::BurstWrite(ptrDestSrc1, bufDst, 1, elemSize, stride);

				// For reduction, only process one chunk (no strip-mining continuation needed)
				break;
			}
			// Handle predicative movement instructions (compress/expand)
			else if (IsPredicativeMovementOp(opCode))
			{
				unsigned long long bytesToRead = vl * static_cast<unsigned long long>(elemSize);
				std::span<unsigned char> bufSrc = CheckedSubspan(scratchA, 0, static_cast<int>(bytesToRead));
				std::span<unsigned char> bufDst = CheckedSubspan(scratchDst, 0, static_cast<int>(bytesToRead));

				// Burst read source
				BurstIO::BurstRead(ptrDestSrc1, bufSrc, vl, elemSize, stride);

				// Execute compress or expand operation
				if (opCode == static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VCOMPRESS))
				{
					VectorALU::ApplyCompress(dataType, bufSrc, bufDst, elemSize, vl, predIndex, core);
				}
				else // VEXPAND
				{
					VectorALU::ApplyExpand(dataType, bufSrc, bufDst, elemSize, vl, predIndex, request.getTailAgnostic(), request.getMaskAgnostic(), core);
				}

				// Burst write result
				BurstIO::BurstWrite(ptrDestSrc1, bufDst, vl, elemSize, stride);
			}
			// Handle permutation/gather instructions (indexed reordering)
			else if (IsPermutationOp(opCode))
			{
				unsigned long long bytesToRead = vl * static_cast<unsigned long long>(elemSize);
				std::span<unsigned char> bufSrc = CheckedSubspan(scratchA, 0, static_cast<int>(bytesToRead));
				std::span<unsigned char> bufIndices = CheckedSubspan(scratchB, 0, static_cast<int>(bytesToRead));
				std::span<unsigned char> bufDst = CheckedSubspan(scratchDst, 0, static_cast<int>(bytesToRead));

				// Burst read source and index vectors
				BurstIO::BurstRead(ptrDestSrc1, bufSrc, vl, elemSize, stride);
				BurstIO::BurstRead(ptrSrc2, bufIndices, vl, elemSize, stride); // Index vector
				// Raw permutation must preserve masked-off destination lanes the same way
				// the authoritative dedicated carrier does, so seed dst before ApplyPermute mutates it.
				BurstIO::BurstRead(ptrDestSrc1, bufDst, vl, elemSize, stride);

				// Execute permute/gather operation
				VectorALU::ApplyPermute(dataType, bufSrc, bufIndices, bufDst, elemSize, vl, predIndex, request.getTailAgnostic(), request.getMaskAgnostic(), core);

				// Burst write result
				BurstIO::BurstWrite(ptrDestSrc1, bufDst, vl, elemSize, stride);
			}
			// Handle slide instructions (element shifting)
			else if (IsSlideOp(opCode))
			{
				unsigned long long bytesToRead = vl * static_cast<unsigned long long>(elemSize);
				std::span<unsigned char> bufSrc = CheckedSubspan(scratchA, 0, static_cast<int>(bytesToRead));
				std::span<unsigned char> bufDst = CheckedSubspan(scratchDst, 0, static_cast<int>(bytesToRead));

				// Burst read source
				BurstIO::BurstRead(ptrDestSrc1, bufSrc, vl, elemSize, stride);
				// Raw slide must preserve undisturbed destination lanes and pre-offset gap
				// lanes the same way as the authoritative dedicated carrier.
				BurstIO::BurstRead(ptrDestSrc1, bufDst, vl, elemSize, stride);

				// Execute slide operation (offset is in immediate field)
				unsigned short slideOffset = request.getImmediate();
				if (opCode == static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VSLIDEUP))
				{
					VectorALU::ApplySlideUp(dataType, bufSrc, bufDst, elemSize, vl, slideOffset, predIndex, request.getTailAgnostic(), request.getMaskAgnostic(), core);
				}
				else // VSLIDEDOWN
				{
					VectorALU::ApplySlideDown(dataType, bufSrc, bufDst, elemSize, vl, slideOffset, predIndex, request.getTailAgnostic(), request.getMaskAgnostic(), core);
				}

				// Burst write result
				BurstIO::BurstWrite(ptrDestSrc1, bufDst, vl, elemSize, stride);
			}
			// Handle FMA instructions (ternary operation: vd = vd * vs1 + vs2)
			// FMA with TriOpDesc: Src2Pointer → descriptor address, descriptor holds SrcA, SrcB, StrideA, StrideB
			// Src2Pointer references a TriOpDesc descriptor holding SrcA, SrcB, StrideA, StrideB.
			else if (IsFMAOp(opCode))
			{
				if (ptrSrc2 == 0)
				{
					throw InvalidOperationException(std::wstring(L"StreamEngine.Execute(...) rejected descriptor-less raw VFM* execution on the descriptor-backed tri-operand contour. ") + L"Current FMA encoding requires Src2Pointer to reference a TriOpDesc, so raw execution must fail closed instead of reviving the legacy descriptor-less fallback path.");
				}
				if (ptrSrc2 != 0)
				{
					// Descriptor mode: read TriOpDesc from memory once
					TriOpDesc desc = ReadTriOpDesc(ptrSrc2);

					unsigned short strideA = desc.StrideA != 0 ? desc.StrideA : static_cast<unsigned short>(elemSize);
					unsigned short strideB = desc.StrideB != 0 ? desc.StrideB : static_cast<unsigned short>(elemSize);

					unsigned long long offsetDst = 0;
					unsigned long long offsetA = 0;
					unsigned long long offsetB = 0;
					unsigned long long fmaRemaining = remaining;

					while (fmaRemaining > 0)
					{
						unsigned long long fmaVl = fmaRemaining < VLMAX ? fmaRemaining : VLMAX;
						unsigned long long bytesToRead = fmaVl * static_cast<unsigned long long>(elemSize);
						std::span<unsigned char> bufDst = CheckedSubspan(scratchDst, 0, static_cast<int>(bytesToRead));
						std::span<unsigned char> bufVs1 = CheckedSubspan(scratchA, 0, static_cast<int>(bytesToRead));
						std::span<unsigned char> bufVs2 = CheckedSubspan(scratchB, 0, static_cast<int>(bytesToRead));

						BurstIO::BurstRead(ptrDestSrc1 + offsetDst, bufDst, fmaVl, elemSize, stride);
						BurstIO::BurstRead(desc.SrcA + offsetA, bufVs1, fmaVl, elemSize, strideA);
						BurstIO::BurstRead(desc.SrcB + offsetB, bufVs2, fmaVl, elemSize, strideB);

						VectorALU::ApplyFMA(opCode, dataType, bufDst, bufVs1, bufVs2, elemSize, fmaVl, predIndex, request.getTailAgnostic(), request.getMaskAgnostic(), core);

						BurstIO::BurstWrite(ptrDestSrc1 + offsetDst, bufDst, fmaVl, elemSize, stride);

						offsetDst += fmaVl * static_cast<unsigned long long>(stride);
						offsetA += fmaVl * static_cast<unsigned long long>(strideA);
						offsetB += fmaVl * static_cast<unsigned long long>(strideB);
						fmaRemaining -= fmaVl;
					}
				}
				else
				{
					// Legacy fallback: Src2Pointer = 0, use Immediate as vs2 address
					unsigned long long bytesToRead = vl * static_cast<unsigned long long>(elemSize);
					std::span<unsigned char> bufDst = CheckedSubspan(scratchDst, 0, static_cast<int>(bytesToRead));
					std::span<unsigned char> bufVs1 = CheckedSubspan(scratchA, 0, static_cast<int>(bytesToRead));
					std::span<unsigned char> bufVs2 = CheckedSubspan(scratchB, 0, static_cast<int>(bytesToRead));

					BurstIO::BurstRead(ptrDestSrc1, bufDst, vl, elemSize, stride);
					// In legacy mode, no second source pointer — use Immediate as address
					unsigned long long ptrVs2 = request.getImmediate();
					BurstIO::BurstRead(ptrVs2, bufVs1, vl, elemSize, stride);
					BurstIO::BurstRead(ptrVs2, bufVs2, vl, elemSize, stride);

					VectorALU::ApplyFMA(opCode, dataType, bufDst, bufVs1, bufVs2, elemSize, vl, predIndex, request.getTailAgnostic(), request.getMaskAgnostic(), core);

					BurstIO::BurstWrite(ptrDestSrc1, bufDst, vl, elemSize, stride);
				}

				// FMA handles its own strip-mining; skip outer loop advancement
				break;
			}
			// Determine if operation is binary or unary
			else if (IsBinaryOp(opCode))
			{
				// Read both operands
				unsigned long long bytesToRead = vl * static_cast<unsigned long long>(elemSize);
				std::span<unsigned char> bufA = CheckedSubspan(scratchA, 0, static_cast<int>(bytesToRead));
				std::span<unsigned char> bufB = CheckedSubspan(scratchB, 0, static_cast<int>(bytesToRead));
				std::span<unsigned char> bufDst = CheckedSubspan(scratchDst, 0, static_cast<int>(bytesToRead));

				// Burst read operand A (dest/src1)
				BurstIO::BurstRead(ptrDestSrc1, bufA, vl, elemSize, stride);

				// Burst read operand B (src2)
				BurstIO::BurstRead(ptrSrc2, bufB, vl, elemSize, stride);

				// Execute ALU operation
				VectorALU::ApplyBinary(opCode, dataType, bufA, bufB, bufDst, elemSize, vl, predIndex, request.getTailAgnostic(), request.getMaskAgnostic(), core);

				// Burst write result (destructive: overwrites dest/src1 location)
				BurstIO::BurstWrite(ptrDestSrc1, bufDst, vl, elemSize, stride);
			}
			else
			{
				// Unary operation: read only one operand
				unsigned long long bytesToRead = vl * static_cast<unsigned long long>(elemSize);
				std::span<unsigned char> bufA = CheckedSubspan(scratchA, 0, static_cast<int>(bytesToRead));
				std::span<unsigned char> bufDst = CheckedSubspan(scratchDst, 0, static_cast<int>(bytesToRead));

				// Burst read operand
				BurstIO::BurstRead(ptrDestSrc1, bufA, vl, elemSize, stride);

				// Execute ALU operation
				VectorALU::ApplyUnary(opCode, dataType, bufA, bufDst, elemSize, vl, predIndex, request.getTailAgnostic(), request.getMaskAgnostic(), core);

				// Burst write result
				BurstIO::BurstWrite(ptrDestSrc1, bufDst, vl, elemSize, stride);
			}

			// Update pointers and remaining count
			unsigned long long strideBytes = static_cast<unsigned long long>(stride) * vl;
			ptrDestSrc1 += strideBytes;
			ptrSrc2 += strideBytes;
			remaining -= vl;

			// Prefetch next chunk into StreamRegisterFile (req.md §2, §3)
			if (remaining > 0)
			{
				unsigned long long nextVl = remaining < VLMAX ? remaining : VLMAX;
				PrefetchToStreamRegister(ptrDestSrc1, static_cast<unsigned char>(elemSize), static_cast<unsigned int>(nextVl));
				if (ptrSrc2 != 0)
				{
					PrefetchToStreamRegister(ptrSrc2, static_cast<unsigned char>(elemSize), static_cast<unsigned int>(nextVl));
				}
			}
		}

		if (isComparisonOp)
		{
			core->SetPredicateRegister(comparisonDestPredReg, accumulatedComparisonResultMask);
		}
	}

	inline void StreamEngine::PublishScalarRetireRegisterWrite(std::shared_ptr<Processor::CPU_Core> &core, int executionVtId, unsigned short archRegId, unsigned long long value)
	{
		core->AppendGeneratedExecutingLaneRetireRecord(RetireRecord::RegisterWrite(executionVtId, archRegId, value));
	}

	inline void StreamEngine::PublishScalarRetireRegisterWrite(std::shared_ptr<Processor::CPU_Core> &core, RetireRecord const retireRecord)
	{
		core->AppendGeneratedExecutingLaneRetireRecord(retireRecord);
	}

	inline bool StreamEngine::TryResolveScalarRegisterRetireRecord(std::shared_ptr<Processor::CPU_Core> &core, StreamExecutionRequest const request, unsigned int opCode, unsigned char predIndex, int executionVtId, RetireRecord &retireRecord)
	{
		retireRecord = YAKSys_Hybrid_CPU::Core::Registers::Retire::RetireRecord();

		unsigned char destinationRegister;
		unsigned char sourceRegister1;
		unsigned char sourceRegister2;
		ResolveRequiredScalarRegisterOperandsOrThrow(request, opCode, destinationRegister, sourceRegister1, sourceRegister2);

		if (!core->LaneActive(predIndex, 0))
		{
			return false;
		}

		retireRecord = RetireRecord::RegisterWrite(executionVtId, destinationRegister, ResolveScalarRegisterResult(core, request, opCode, executionVtId, sourceRegister1, sourceRegister2));
		return true;
	}

	inline void StreamEngine::CaptureScalarRegisterRetireWindowPublication(std::shared_ptr<Processor::CPU_Core> &core, StreamExecutionRequest const request, Processor::CPU_Core::RetireWindowBatch &retireBatch, unsigned int opCode, unsigned char predIndex, int executionVtId)
	{
		ThrowIfUnsupportedScalarRegisterStreamOpcode(opCode);

		RetireRecord retireRecord;
		if (TryResolveScalarRegisterRetireRecord(core, request, opCode, predIndex, executionVtId, retireRecord))
		{
			retireBatch.AppendRetireRecord(retireRecord);
		}
	}

	inline unsigned long long StreamEngine::ResolveScalarRegisterResult(std::shared_ptr<Processor::CPU_Core> &core, StreamExecutionRequest const request, unsigned int opCode, int executionVtId, unsigned char sourceRegister1, unsigned char sourceRegister2)
	{
		unsigned long long val1 = core->ReadArch(executionVtId, sourceRegister1);
		unsigned long long val2 = core->ReadArch(executionVtId, sourceRegister2);

	// C# TO C++ CONVERTER TASK: The following 'switch expression' was not converted by C# to C++ Converter:
	//			return opCode switch
	//			{
	//				(uint)Processor.CPU_Core.InstructionsEnum.ADD => val1 + val2,
	//
	//				(uint)Processor.CPU_Core.InstructionsEnum.SUB => val1 - val2,
	//
	//				(uint)Processor.CPU_Core.InstructionsEnum.MUL => val1 * val2,
	//
	//				(uint)Processor.CPU_Core.InstructionsEnum.DIV => val2 != 0 ? val1 / val2 : 0,
	//
	//				(uint)Processor.CPU_Core.InstructionsEnum.XOR => val1 ^ val2,
	//
	//				(uint)Processor.CPU_Core.InstructionsEnum.OR => val1 | val2,
	//
	//				(uint)Processor.CPU_Core.InstructionsEnum.AND => val1 & val2,
	//
	//				(uint)Processor.CPU_Core.InstructionsEnum.SLL => val1 << (int)(request.Immediate & 0x3F),
	//
	//				(uint)Processor.CPU_Core.InstructionsEnum.SRL => val1 >> (int)(request.Immediate & 0x3F),
	//
	//				_ => throw CreateUnsupportedScalarRegisterStreamOpcodeException(opCode)
	//			};
		switch (opCode)
		{
		case static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::ADD): return val1 + val2;
		case static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::SUB): return val1 - val2;
		case static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::MUL): return val1 * val2;
		case static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::DIV): return val2 != 0 ? val1 / val2 : 0;
		case static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::XOR): return val1 ^ val2;
		case static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::OR): return val1 | val2;
		case static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::AND): return val1 & val2;
		case static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::SLL): return val1 << static_cast<unsigned int>(request.getImmediate() & 0x3F);
		case static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::SRL): return val1 >> static_cast<unsigned int>(request.getImmediate() & 0x3F);
		default: throw CreateUnsupportedScalarRegisterStreamOpcodeException(opCode);
		}
	}

	inline void StreamEngine::ResolveRequiredScalarRegisterOperandsOrThrow(StreamExecutionRequest const request, unsigned int opCode, unsigned char &destinationRegister, unsigned char &sourceRegister1, unsigned char &sourceRegister2)
	{
		if (request.TryUnpackArchRegs(destinationRegister, sourceRegister1, sourceRegister2) && destinationRegister != StreamExecutionRequest::NoArchReg && sourceRegister1 != StreamExecutionRequest::NoArchReg && sourceRegister2 != StreamExecutionRequest::NoArchReg)
		{
			return;
		}

		throw InvalidOperationException(std::format("Stream opcode 0x{0:X} reached the scalar direct stream helper contour with non-canonical architectural register fields. ", opCode) + "This direct-retire contour is authoritative only for fully representable rd/rs1/rs2 arch-register triples; direct callers must reject malformed register encodings instead of observing silent no-op or synthetic zero-valued helper success.");
	}

	inline void StreamEngine::ThrowIfUnsupportedScalarRegisterStreamOpcode(unsigned int opCode)
	{
		if (!IsSupportedScalarRegisterStreamOpcode(opCode))
		{
			throw CreateUnsupportedScalarRegisterStreamOpcodeException(opCode);
		}
	}

	inline bool StreamEngine::IsSupportedScalarRegisterStreamOpcode(unsigned int opCode)
	{
	// C# TO C++ CONVERTER TASK: The following 'switch expression' was not converted by C# to C++ Converter:
	//			return opCode switch
	//			{
	//				(uint)Processor.CPU_Core.InstructionsEnum.ADD => true,
	//				(uint)Processor.CPU_Core.InstructionsEnum.SUB => true,
	//				(uint)Processor.CPU_Core.InstructionsEnum.MUL => true,
	//				(uint)Processor.CPU_Core.InstructionsEnum.DIV => true,
	//
	//				(uint)Processor.CPU_Core.InstructionsEnum.XOR => true,
	//				(uint)Processor.CPU_Core.InstructionsEnum.OR => true,
	//				(uint)Processor.CPU_Core.InstructionsEnum.AND => true,
	//				(uint)Processor.CPU_Core.InstructionsEnum.SLL => true,
	//				(uint)Processor.CPU_Core.InstructionsEnum.SRL => true,
	//				_ => false
	//			};
		switch (opCode)
		{
		case static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::ADD):
		case static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::SUB):
		case static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::MUL):
		case static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::DIV):
		case static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::XOR):
		case static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::OR):
		case static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::AND):
		case static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::SLL):
		case static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::SRL):
			return true;
		default:
			return false;
		}
	}

	inline std::shared_ptr<InvalidOperationException> StreamEngine::CreateUnsupportedScalarRegisterStreamOpcodeException(unsigned int opCode)
	{
		return std::make_shared<InvalidOperationException>(std::format("Stream opcode 0x{0:X} reached the scalar direct stream helper contour without an authoritative scalar retire/apply contract. ", opCode) + "This contour is limited to scalar ALU register-write helpers; vector compute opcodes must not synthesize scalar retire truth from streamLength == 1 / IsScalar encoding, and dedicated scalar-result VPOPC remains on its explicit direct compat path.");
	}

	inline void StreamEngine::CaptureVpopcRetireWindowPublication(std::shared_ptr<Processor::CPU_Core> &core, StreamExecutionRequest const request, Processor::CPU_Core::RetireWindowBatch &retireBatch, int executionVtId)
	{
		RetireRecord retireRecord;
		if (TryResolveVpopcRetireRecord(core, request, executionVtId, retireRecord))
		{
			retireBatch.AppendRetireRecord(retireRecord);
		}
	}

	inline bool StreamEngine::TryResolveVpopcRetireRecord(std::shared_ptr<Processor::CPU_Core> &core, StreamExecutionRequest const request, int executionVtId, RetireRecord &retireRecord)
	{
		retireRecord = YAKSys_Hybrid_CPU::Core::Registers::Retire::RetireRecord();

		unsigned char srcPred = static_cast<unsigned char>(request.getImmediate() & 0x0F);
		unsigned char destReg = static_cast<unsigned char>((request.getImmediate() >> 8) & 0x0F);
		if (destReg >= YAKSys_Hybrid_CPU::Core::Registers::RenameMap::ArchRegs)
		{
			return false;
		}

		constexpr unsigned long long VLMAX = 32;
		unsigned long long vl = request.getStreamLength() < VLMAX ? request.getStreamLength() : VLMAX;
		unsigned long long popCount = VectorALU::MaskPopCount(core->GetPredicateRegister(srcPred), vl);
		retireRecord = RetireRecord::RegisterWrite(executionVtId, destReg, popCount);
		return true;
	}

	inline void StreamEngine::CapturePredicateStateRetireWindowPublication(std::shared_ptr<Processor::CPU_Core> &core, StreamExecutionRequest const request, Processor::CPU_Core::RetireWindowBatch &retireBatch, unsigned int opCode, DataTypeEnum dataType, unsigned char predIndex, int executionVtId)
	{
		if (IsComparisonOp(opCode))
		{
			int elemSize = ResolveRequiredElementSizeOrThrow(opCode, dataType);
			retireBatch.CaptureRetireWindowPredicateState(ResolveComparisonDestinationPredicateRegister(request), ResolveComparisonPredicateMask(core, request, opCode, dataType, predIndex, elemSize));
			retireBatch.CaptureRetireWindowVectorStreamDirty(executionVtId);
			return;
		}

		if (IsMaskManipOp(opCode) && opCode != static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VPOPC))
		{
			retireBatch.CaptureRetireWindowPredicateState(ResolveMaskManipulationDestinationPredicateRegister(request), ResolveMaskManipulationResultMask(core, request, opCode));
			retireBatch.CaptureRetireWindowVectorStreamDirty(executionVtId);
			return;
		}

		throw InvalidOperationException(std::format("Stream opcode 0x{0:X} reached direct stream retire-window predicate-state publication without an authoritative predicate-publication contour.", opCode));
	}

	inline unsigned long long StreamEngine::ResolveComparisonPredicateMask(std::shared_ptr<Processor::CPU_Core> &core, StreamExecutionRequest const request, unsigned int opCode, DataTypeEnum dataType, unsigned char predIndex, int elemSize)
	{
		unsigned long long ptrDestSrc1 = request.getDestSrc1Pointer();
		unsigned long long ptrSrc2 = request.getSrc2Pointer();
		unsigned short stride = request.getStride();
		if (stride == 0)
		{
			stride = static_cast<unsigned short>(elemSize);
		}

		constexpr unsigned long long VLMAX = 32;
		std::span<unsigned char> scratchA = core->GetScratchA();
		std::span<unsigned char> scratchB = core->GetScratchB();
		std::span<unsigned char> scratchDst = core->GetScratchDst();

		ThrowIfUnavailableRawScratchContour(opCode, L"direct stream compat retire resolution", true, scratchA.size(), scratchB.size(), scratchDst.size(), -1);

		unsigned long long remaining = request.getStreamLength();
		unsigned long long accumulatedResultMask = 0;

		while (remaining > 0)
		{
			unsigned long long vl = remaining < VLMAX ? remaining : VLMAX;
			unsigned long long maxVL = static_cast<unsigned long long>(scratchA.size() / elemSize);
			if (vl > maxVL)
			{
				vl = maxVL;
			}

			unsigned long long bytesToRead = vl * static_cast<unsigned long long>(elemSize);
			std::span<unsigned char> bufA = CheckedSubspan(scratchA, 0, static_cast<int>(bytesToRead));
			std::span<unsigned char> bufB = CheckedSubspan(scratchB, 0, static_cast<int>(bytesToRead));

			BurstIO::BurstRead(ptrDestSrc1, bufA, vl, elemSize, stride);
			BurstIO::BurstRead(ptrSrc2, bufB, vl, elemSize, stride);

			unsigned long long resultMask = VectorALU::ApplyComparison(opCode, dataType, bufA, bufB, elemSize, vl, predIndex, core);

			AccumulateComparisonPredicateChunk(accumulatedResultMask, resultMask, request.getStreamLength() - remaining);

			unsigned long long strideBytes = static_cast<unsigned long long>(stride) * vl;
			ptrDestSrc1 += strideBytes;
			ptrSrc2 += strideBytes;
			remaining -= vl;
		}

		return accumulatedResultMask;
	}

	inline unsigned char StreamEngine::ResolveComparisonDestinationPredicateRegister(StreamExecutionRequest const request)
	{
		return static_cast<unsigned char>(request.getImmediate() & 0x0F);
	}

	inline unsigned char StreamEngine::ResolveMaskManipulationDestinationPredicateRegister(StreamExecutionRequest const request)
	{
		return static_cast<unsigned char>((request.getImmediate() >> 8) & 0x0F);
	}

	inline unsigned long long StreamEngine::ResolveMaskManipulationResultMask(std::shared_ptr<Processor::CPU_Core> &core, StreamExecutionRequest const request, unsigned int opCode)
	{
		unsigned char srcPred1 = static_cast<unsigned char>(request.getImmediate() & 0x0F);
		unsigned char srcPred2 = static_cast<unsigned char>((request.getImmediate() >> 4) & 0x0F);
		unsigned long long mask1 = core->GetPredicateRegister(srcPred1);

	// C# TO C++ CONVERTER TASK: The following 'switch expression' was not converted by C# to C++ Converter:
	//			return opCode switch
	//			{
	//				(uint)Processor.CPU_Core.InstructionsEnum.VMAND => VectorALU.ApplyMaskBinary(
	//						opCode,
	//						mask1,
	//						core.GetPredicateRegister(srcPred2)),
	//				(uint)Processor.CPU_Core.InstructionsEnum.VMOR =>
	//					VectorALU.ApplyMaskBinary(
	//						opCode,
	//						mask1,
	//						core.GetPredicateRegister(srcPred2)),
	//				(uint)Processor.CPU_Core.InstructionsEnum.VMXOR =>
	//					VectorALU.ApplyMaskBinary(
	//						opCode,
	//						mask1,
	//						core.GetPredicateRegister(srcPred2)),
	//				(uint)Processor.CPU_Core.InstructionsEnum.VMNOT =>
	//					VectorALU.ApplyMaskUnary(opCode, mask1),
	//				(uint)Processor.CPU_Core.InstructionsEnum.VMSBF =>
	//					ResolveMaskSetBeforeFirstResult(mask1, in request),
	//				_ => throw new InvalidOperationException(
	//					$"Stream opcode 0x{opCode:X} reached predicate-mask resolution without an authoritative `VM*` follow-through contour.")
	//			};
		switch (opCode)
		{
		case static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VMAND):
		case static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VMOR):
		case static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VMXOR):
			return VectorALU::ApplyMaskBinary(opCode, mask1, core->GetPredicateRegister(srcPred2));
		case static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VMNOT):
			return VectorALU::ApplyMaskUnary(opCode, mask1);
		case static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VMSBF):
			return ResolveMaskSetBeforeFirstResult(mask1, request);
		default:
			throw InvalidOperationException(std::format("Stream opcode 0x{0:X} reached predicate-mask resolution without an authoritative VM* follow-through contour.", opCode));
		}
	}

	inline unsigned long long StreamEngine::ResolveMaskSetBeforeFirstResult(unsigned long long mask, StreamExecutionRequest const request)
	{
		unsigned char reservedSrcPred2 = static_cast<unsigned char>((request.getImmediate() >> 4) & 0x0F);
		if (reservedSrcPred2 != 0)
		{
			throw InvalidOperationException(std::wstring(L"Stream predicate-mask execution rejected VMSBF with a non-zero reserved src2 predicate nibble. ") + L"The Phase 04 contour is the canonical unary mask-prefix ABI only.");
		}

		unsigned long long vl = std::min<unsigned long long>(request.getStreamLength(), Processor::CPU_Core::RVV_Config::VLMAX);
		return VectorALU::ApplyMaskSetBeforeFirst(mask, vl);
	}

	inline void StreamEngine::AccumulateComparisonPredicateChunk(unsigned long long &accumulatedResultMask, unsigned long long chunkResultMask, unsigned long long elementsProcessed)
	{
		int shift = static_cast<int>(std::min(elementsProcessed, 64ULL));
		if (shift >= 64)
		{
			return;
		}

		unsigned long long shiftedMask = shift == 0 ? chunkResultMask : chunkResultMask << shift;
		accumulatedResultMask |= shiftedMask;
	}

	int StreamEngine::ResolveExecutionVtId(std::shared_ptr<Processor::CPU_Core> &core, int ownerThreadId)
	{
		if (static_cast<unsigned int>(ownerThreadId) < static_cast<unsigned int>(Processor::CPU_Core::SmtWays))
		{
			return ownerThreadId;
		}

		return core->ReadActiveVirtualThreadId();
	}

	inline unsigned long long StreamEngine::ResolvePreferredStreamChunkShape(unsigned long long remaining, int elementSize, int scratchByteCapacity, unsigned short stride)
	{
		if (remaining == 0 || elementSize <= 0)
		{
			return 0;
		}

		unsigned long long chunkElements = std::min(remaining, 32ULL);
		unsigned long long scratchBudget = static_cast<unsigned long long>(std::max(1, scratchByteCapacity / elementSize));
		chunkElements = std::min(chunkElements, scratchBudget);

		if (stride == elementSize)
		{
			unsigned int srfBudget = ResolveSrfResidentChunkBudget(elementSize, chunkElements);
			if (srfBudget != 0)
			{
				chunkElements = std::min(chunkElements, static_cast<unsigned long long>(srfBudget));
			}
		}

		return std::max(1ULL, chunkElements);
	}

	inline unsigned long long StreamEngine::ResolvePreferredStreamChunkShape(unsigned long long remaining, int elementSize, int scratchByteCapacity, unsigned int rowLength, unsigned short rowStride, unsigned short colStride, unsigned long long elementsProcessed)
	{
		unsigned long long chunkElements = ResolvePreferredStreamChunkShape(remaining, elementSize, scratchByteCapacity, colStride);
		if (chunkElements == 0 || rowLength == 0 || colStride != elementSize)
		{
			return chunkElements;
		}

		unsigned long long fullyContiguousRowBytes = static_cast<unsigned long long>(rowLength) * static_cast<unsigned long long>(elementSize);
		if (static_cast<unsigned long long>(rowStride) != fullyContiguousRowBytes)
		{
			unsigned long long rowOffset = elementsProcessed % rowLength;
			unsigned long long rowRemaining = rowLength - rowOffset;
			chunkElements = std::min(chunkElements, rowRemaining);
		}

		return std::max(1ULL, chunkElements);
	}

	inline unsigned long long StreamEngine::ResolvePreferredStreamChunkShape(unsigned long long remaining, int elementSize, int scratchByteCapacity, int indexElementSize, int scratchIndexByteCapacity, unsigned short indexStride)
	{
		unsigned long long chunkElements = ResolvePreferredStreamChunkShape(remaining, elementSize, scratchByteCapacity, static_cast<unsigned short>(elementSize));
		if (chunkElements == 0 || indexElementSize <= 0)
		{
			return chunkElements;
		}

		unsigned long long indexScratchBudget = static_cast<unsigned long long>(std::max(1, scratchIndexByteCapacity / indexElementSize));
		chunkElements = std::min(chunkElements, indexScratchBudget);

		if (indexStride == indexElementSize)
		{
			unsigned int indexSrfBudget = ResolveSrfResidentChunkBudget(indexElementSize, chunkElements);
			if (indexSrfBudget != 0)
			{
				chunkElements = std::min(chunkElements, static_cast<unsigned long long>(indexSrfBudget));
			}
		}

		return std::max(1ULL, chunkElements);
	}

	inline void StreamEngine::TryApplyIngressWarmPolicy(unsigned long long destPtr, unsigned long long src2Ptr, int elementSize, unsigned long long chunkElements, unsigned short stride, bool isBinary)
	{
		if (chunkElements == 0 || elementSize <= 0 || stride != elementSize || chunkElements > std::numeric_limits<unsigned int>::max())
		{
			return;
		}

		unsigned int exactChunkElements = static_cast<unsigned int>(chunkElements);
		PrefetchToStreamRegister(destPtr, static_cast<unsigned char>(elementSize), exactChunkElements);
		if (isBinary)
		{
			PrefetchToStreamRegister(src2Ptr, static_cast<unsigned char>(elementSize), exactChunkElements);
		}
	}

	inline void StreamEngine::TryApplyIngressWarmPolicy(unsigned long long baseAddr, int elementSize, unsigned long long chunkElements, unsigned int rowLength, unsigned short rowStride, unsigned short colStride, unsigned long long startOffset)
	{
		if (chunkElements == 0)
		{
			return;
		}

		TryWarmPlanned2DChunk(baseAddr, elementSize, chunkElements, rowLength, rowStride, colStride, startOffset);
	}

	inline void StreamEngine::TryApplyIngressWarmPolicy(unsigned long long destBase, unsigned long long indexBase, int elementSize, int indexElementSize, unsigned short indexStride, unsigned long long startElement, unsigned long long chunkElements)
	{
		if (chunkElements == 0)
		{
			return;
		}

		TryWarmIndexedIngressChunk(destBase, indexBase, elementSize, indexElementSize, indexStride, startElement, chunkElements);
	}

	inline void StreamEngine::Execute1D_DoubleBuffered(std::shared_ptr<Processor::CPU_Core> &core, StreamExecutionRequest const request, unsigned int opCode, DataTypeEnum dataType, unsigned char predIndex, int elemSize, unsigned long long streamLength)
	{
		unsigned long long ptrDestSrc1 = request.getDestSrc1Pointer();
		unsigned long long ptrSrc2 = request.getSrc2Pointer();
		unsigned short stride = request.getStride();
		if (stride == 0)
		{
			stride = static_cast<unsigned short>(elemSize);
		}

		// Latency model constants (matching FSM timing, Appendix A.3)
		constexpr unsigned long long MEMORY_LATENCY_PER_BURST = 4; // cycles per burst read/write
		constexpr unsigned long long COMPUTE_LATENCY_BASE = 1; // 1 cycle for simple ALU

		// Local accumulators — flushed to core at the end
		unsigned long long accReadCycles = 0;
		unsigned long long accWriteCycles = 0;
		unsigned long long accComputeCycles = 0;
		unsigned long long accOverlappedCycles = 0;

		bool isBinary = IsBinaryOp(opCode);

		unsigned long long remaining = streamLength;

		// ---------- Iteration 0: initial load into active buffer ----------
		std::span<unsigned char> bufA = core->GetActiveScratchA();
		std::span<unsigned char> bufB = core->GetActiveScratchB();
		std::span<unsigned char> bufDst = core->GetActiveScratchDst();

		ThrowIfUnavailableRawScratchContour(opCode, L"double-buffered raw 1D stream/vector execution", isBinary, bufA.size(), bufB.size(), bufDst.size(), -1);

		unsigned long long vl = ResolvePreferredStreamChunkShape(remaining, elemSize, bufA.size(), stride);
		unsigned long long bytes = vl * static_cast<unsigned long long>(elemSize);

		BurstIO::BurstRead(ptrDestSrc1, CheckedSubspan(bufA, 0, static_cast<int>(bytes)), vl, elemSize, stride);
		accReadCycles += MEMORY_LATENCY_PER_BURST;

		if (isBinary)
		{
			BurstIO::BurstRead(ptrSrc2, CheckedSubspan(bufB, 0, static_cast<int>(bytes)), vl, elemSize, stride);
			accReadCycles += MEMORY_LATENCY_PER_BURST;
		}

		unsigned long long curPtrDst = ptrDestSrc1;
		unsigned long long curPtrSrc2 = ptrSrc2;

		while (remaining > 0)
		{
			unsigned long long curVl = ResolvePreferredStreamChunkShape(remaining, elemSize, bufA.size(), stride);
			unsigned long long curBytes = curVl * static_cast<unsigned long long>(elemSize);

			// ── Compute on current active buffer ──
			std::span<unsigned char> cA = CheckedSubspan(bufA, 0, static_cast<int>(curBytes));
			std::span<unsigned char> cDst = CheckedSubspan(bufDst, 0, static_cast<int>(curBytes));

			if (isBinary)
			{
				std::span<unsigned char> cB = CheckedSubspan(bufB, 0, static_cast<int>(curBytes));
				VectorALU::ApplyBinary(opCode, dataType, cA, cB, cDst, elemSize, curVl, predIndex, request.getTailAgnostic(), request.getMaskAgnostic(), core);
			}
			else
			{
				VectorALU::ApplyUnary(opCode, dataType, cA, cDst, elemSize, curVl, predIndex, request.getTailAgnostic(), request.getMaskAgnostic(), core);
			}
			accComputeCycles += COMPUTE_LATENCY_BASE;

			// ── Prefetch next chunk into INACTIVE buffer (if remaining) ──
			unsigned long long nextPtrDst = curPtrDst + static_cast<unsigned long long>(stride) * curVl;
			unsigned long long nextPtrSrc2 = curPtrSrc2 + static_cast<unsigned long long>(stride) * curVl;
			unsigned long long nextRemaining = remaining - curVl;
			unsigned long long prefetchLoadCycles = 0;

			if (nextRemaining > 0)
			{
				core->ToggleDoubleBuffer();
				std::span<unsigned char> nextA = core->GetActiveScratchA();
				std::span<unsigned char> nextB = core->GetActiveScratchB();

				unsigned long long nextVl = ResolvePreferredStreamChunkShape(nextRemaining, elemSize, nextA.size(), stride);
				unsigned long long nextBytes = nextVl * static_cast<unsigned long long>(elemSize);

				TryApplyIngressWarmPolicy(nextPtrDst, nextPtrSrc2, elemSize, nextVl, stride, isBinary);
				BurstIO::BurstRead(nextPtrDst, CheckedSubspan(nextA, 0, static_cast<int>(nextBytes)), nextVl, elemSize, stride);
				prefetchLoadCycles += MEMORY_LATENCY_PER_BURST;

				if (isBinary)
				{
					BurstIO::BurstRead(nextPtrSrc2, CheckedSubspan(nextB, 0, static_cast<int>(nextBytes)), nextVl, elemSize, stride);
					prefetchLoadCycles += MEMORY_LATENCY_PER_BURST;
				}

				accReadCycles += prefetchLoadCycles;

				// Overlap model: prefetch happens in parallel with compute of current chunk
				accOverlappedCycles += std::min(prefetchLoadCycles, COMPUTE_LATENCY_BASE);
			}

			// ── Store result from current active buffer ──
			BurstIO::BurstWrite(curPtrDst, cDst, curVl, elemSize, stride);
			accWriteCycles += MEMORY_LATENCY_PER_BURST;

			// Advance
			curPtrDst = nextPtrDst;
			curPtrSrc2 = nextPtrSrc2;
			remaining = nextRemaining;

			// Swap active/inactive
			if (remaining > 0)
			{
				bufA = core->GetActiveScratchA();
				bufB = core->GetActiveScratchB();
				bufDst = core->GetActiveScratchDst();
			}
		}

		// Flush accumulated counters to core
		core->AccumulateOverlapCounters(accReadCycles, accWriteCycles, accComputeCycles, accOverlappedCycles);
	}

	inline void StreamEngine::Execute2D(std::shared_ptr<Processor::CPU_Core> &core, StreamExecutionRequest const request, unsigned int opCode, DataTypeEnum dataType, unsigned char predIndex, int elemSize, unsigned long long streamLength)
	{
		unsigned long long ptrDestSrc1 = request.getDestSrc1Pointer();
		unsigned long long ptrSrc2 = request.getSrc2Pointer();
		unsigned short colStride = request.getStride();
		unsigned short rowStride = request.getRowStride();
		unsigned int rowLength = request.getImmediate(); // Number of elements per row

		ThrowIfUnsupportedZeroRowLength2DContour(opCode, streamLength, rowLength, L"raw 2D stream/vector execution");

		// Default strides
		if (colStride == 0)
		{
			colStride = static_cast<unsigned short>(elemSize);
		}
		if (rowStride == 0)
		{
			rowStride = static_cast<unsigned short>(colStride * rowLength);
		}

		// Get scratch buffers from core
		std::span<unsigned char> scratchA = core->GetScratchA();
		std::span<unsigned char> scratchB = core->GetScratchB();
		std::span<unsigned char> scratchDst = core->GetScratchDst();
		bool isBinary = IsBinaryOp(opCode);

		ThrowIfUnavailableRawScratchContour(opCode, L"raw 2D stream/vector execution", isBinary, scratchA.size(), scratchB.size(), scratchDst.size(), -1);

		unsigned long long remaining = streamLength;
		unsigned long long elementsProcessed = 0;

		// Strip-mining loop: process VLMAX elements per iteration
		while (remaining > 0)
		{
			unsigned long long vl = ResolvePreferredStreamChunkShape(remaining, elemSize, scratchA.size(), rowLength, rowStride, colStride, elementsProcessed);

			if (isBinary)
			{
				// Read both operands using 2D addressing
				unsigned long long bytesToRead = vl * static_cast<unsigned long long>(elemSize);
				std::span<unsigned char> bufA = CheckedSubspan(scratchA, 0, static_cast<int>(bytesToRead));
				std::span<unsigned char> bufB = CheckedSubspan(scratchB, 0, static_cast<int>(bytesToRead));
				std::span<unsigned char> bufDst = CheckedSubspan(scratchDst, 0, static_cast<int>(bytesToRead));

				// Burst read with 2D pattern (dest/src1)
				BurstIO::BurstRead2D(ptrDestSrc1, bufA, vl, elemSize, rowLength, rowStride, colStride, elementsProcessed);

				// Burst read with 2D pattern (src2)
				BurstIO::BurstRead2D(ptrSrc2, bufB, vl, elemSize, rowLength, rowStride, colStride, elementsProcessed);

				// Execute ALU operation
				VectorALU::ApplyBinary(opCode, dataType, bufA, bufB, bufDst, elemSize, vl, predIndex, request.getTailAgnostic(), request.getMaskAgnostic(), core);

				// Burst write result with 2D pattern
				BurstIO::BurstWrite2D(ptrDestSrc1, bufDst, vl, elemSize, rowLength, rowStride, colStride, elementsProcessed);
			}
			else
			{
				// Unary operation: read only one operand
				unsigned long long bytesToRead = vl * static_cast<unsigned long long>(elemSize);
				std::span<unsigned char> bufA = CheckedSubspan(scratchA, 0, static_cast<int>(bytesToRead));
				std::span<unsigned char> bufDst = CheckedSubspan(scratchDst, 0, static_cast<int>(bytesToRead));

				// Burst read with 2D pattern
				BurstIO::BurstRead2D(ptrDestSrc1, bufA, vl, elemSize, rowLength, rowStride, colStride, elementsProcessed);

				// Execute ALU operation
				VectorALU::ApplyUnary(opCode, dataType, bufA, bufDst, elemSize, vl, predIndex, request.getTailAgnostic(), request.getMaskAgnostic(), core);

				// Burst write result with 2D pattern
				BurstIO::BurstWrite2D(ptrDestSrc1, bufDst, vl, elemSize, rowLength, rowStride, colStride, elementsProcessed);
			}

			unsigned long long nextRemaining = remaining - vl;
			if (nextRemaining > 0)
			{
				unsigned long long nextVl = ResolvePreferredStreamChunkShape(nextRemaining, elemSize, scratchA.size(), rowLength, rowStride, colStride, elementsProcessed + vl);
				unsigned long long nextOffset = elementsProcessed + vl;
				TryApplyIngressWarmPolicy(ptrDestSrc1, elemSize, nextVl, rowLength, rowStride, colStride, nextOffset);

				if (isBinary)
				{
					TryApplyIngressWarmPolicy(ptrSrc2, elemSize, nextVl, rowLength, rowStride, colStride, nextOffset);
				}
			}

			// Update remaining count
			elementsProcessed += vl;
			remaining -= vl;
		}
	}

	inline void StreamEngine::ExecuteIndexed(std::shared_ptr<Processor::CPU_Core> &core, StreamExecutionRequest const request, unsigned int opCode, DataTypeEnum dataType, unsigned char predIndex, int elemSize, unsigned long long streamLength)
	{
		if (opCode == static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VGATHER) ||
			opCode == static_cast<unsigned int>(Processor::CPU_Core::InstructionsEnum::VSCATTER))
		{
			throw InvalidOperationException(std::format("Stream/vector opcode 0x{0:X} reached indexed StreamEngine.Execute(...) on a non-representable raw gather/scatter contour. ", opCode) + "Legacy indexed fallback must fail closed for VGATHER/VSCATTER instead of publishing hidden success on a surface that still has no authoritative mainline MicroOp.");
		}

		// Word1: destination/source1 base address (for output)
		// Word2: descriptor address (points to Indexed2SrcDesc)
		unsigned long long ptrDestSrc1 = request.getDestSrc1Pointer();
		unsigned long long descriptorAddr = request.getSrc2Pointer();

		// Read descriptor from memory
		std::array<unsigned char, 32> descStorage{}; // Indexed2SrcDesc is ~20 bytes, allocate 32 for safety
		std::span<unsigned char> descBuffer(descStorage);
		if (!YAKSys_Hybrid_CPU::Memory::IOMMU::ReadBurst(0, descriptorAddr, descBuffer))
		{
			throw InvalidOperationException(std::format("Stream/vector opcode 0x{0:X} reached indexed StreamEngine.Execute(...) contour with unreadable descriptor at 0x{1:X}. ", opCode, descriptorAddr) + "Indexed mainline execution must fail closed instead of collapsing into hidden success/no-op when descriptor fetch cannot be materialized.");
		}

		// Parse descriptor (manual parsing for HLS compatibility)
		unsigned long long src2Base = ReadLittleEndian<unsigned long long>(CheckedSubspan(descBuffer, 0, 8));
		unsigned long long indexBase = ReadLittleEndian<unsigned long long>(CheckedSubspan(descBuffer, 8, 8));
		unsigned short indexStride = ReadLittleEndian<unsigned short>(CheckedSubspan(descBuffer, 16, 2));
		unsigned char indexType = descBuffer[18]; // 0=uint32, 1=uint64
		unsigned char indexIsByteOffset = descBuffer[19]; // 0=element index, 1=byte offset

		int indexElemSize = (indexType == 0) ? 4 : 8; // uint32 or uint64
		if (indexStride == 0)
		{
			indexStride = static_cast<unsigned short>(indexElemSize);
		}

		// Get scratch buffers from core
		std::span<unsigned char> scratchA = core->GetScratchA();
		std::span<unsigned char> scratchB = core->GetScratchB();
		std::span<unsigned char> scratchDst = core->GetScratchDst();
		std::span<unsigned char> scratchIndex = core->GetScratchIndex(); // Additional buffer for indices
		bool isBinary = IsBinaryOp(opCode);

		ThrowIfUnavailableRawScratchContour(opCode, L"raw indexed stream/vector execution", isBinary, scratchA.size(), scratchB.size(), scratchDst.size(), scratchIndex.size());

		unsigned long long remaining = streamLength;
		unsigned long long elementsProcessed = 0;

		// Strip-mining loop: process VLMAX elements per iteration
		while (remaining > 0)
		{
			unsigned long long vl = ResolvePreferredStreamChunkShape(remaining, elemSize, scratchA.size(), indexElemSize, scratchIndex.size(), indexStride);

			if (isBinary)
			{
				// Read operands using indexed addressing
				unsigned long long bytesToRead = vl * static_cast<unsigned long long>(elemSize);
				std::span<unsigned char> bufA = CheckedSubspan(scratchA, 0, static_cast<int>(bytesToRead));
				std::span<unsigned char> bufB = CheckedSubspan(scratchB, 0, static_cast<int>(bytesToRead));
				std::span<unsigned char> bufDst = CheckedSubspan(scratchDst, 0, static_cast<int>(bytesToRead));

				// Read destination/src1 (sequential access)
				BurstIO::BurstRead(ptrDestSrc1 + (elementsProcessed * static_cast<unsigned long long>(elemSize)), bufA, vl, elemSize, static_cast<unsigned short>(elemSize));

				// Read indices
				unsigned long long indexBytes = vl * static_cast<unsigned long long>(indexElemSize);
				std::span<unsigned char> indexBuf = CheckedSubspan(scratchIndex, 0, static_cast<int>(indexBytes));
				BurstIO::BurstRead(indexBase + (elementsProcessed * static_cast<unsigned long long>(indexStride)), indexBuf, vl, indexElemSize, indexStride);

				// Gather from src2 using indices
				BurstIO::BurstGather(src2Base, bufB, vl, elemSize, indexBuf, indexElemSize, indexIsByteOffset);

				// Execute ALU operation
				VectorALU::ApplyBinary(opCode, dataType, bufA, bufB, bufDst, elemSize, vl, predIndex, request.getTailAgnostic(), request.getMaskAgnostic(), core);

				// Scatter result back to destination (sequential write)
				BurstIO::BurstWrite(ptrDestSrc1 + (elementsProcessed * static_cast<unsigned long long>(elemSize)), bufDst, vl, elemSize, static_cast<unsigned short>(elemSize));
			}
			else
			{
				// Unary operation: read only one operand
				unsigned long long bytesToRead = vl * static_cast<unsigned long long>(elemSize);
				std::span<unsigned char> bufA = CheckedSubspan(scratchA, 0, static_cast<int>(bytesToRead));
				std::span<unsigned char> bufDst = CheckedSubspan(scratchDst, 0, static_cast<int>(bytesToRead));

				// Read operand (sequential access)
				BurstIO::BurstRead(ptrDestSrc1 + (elementsProcessed * static_cast<unsigned long long>(elemSize)), bufA, vl, elemSize, static_cast<unsigned short>(elemSize));

				// Execute ALU operation
				VectorALU::ApplyUnary(opCode, dataType, bufA, bufDst, elemSize, vl, predIndex, request.getTailAgnostic(), request.getMaskAgnostic(), core);

				// Write result (sequential write)
				BurstIO::BurstWrite(ptrDestSrc1 + (elementsProcessed * static_cast<unsigned long long>(elemSize)), bufDst, vl, elemSize, static_cast<unsigned short>(elemSize));
			}

			unsigned long long nextRemaining = remaining - vl;
			if (nextRemaining > 0)
			{
				unsigned long long nextVl = ResolvePreferredStreamChunkShape(nextRemaining, elemSize, scratchA.size(), indexElemSize, scratchIndex.size(), indexStride);
				unsigned long long nextElement = elementsProcessed + vl;
				TryApplyIngressWarmPolicy(ptrDestSrc1, indexBase, elemSize, indexElemSize, indexStride, nextElement, nextVl);
			}

			// Update remaining count
			elementsProcessed += vl;
			remaining -= vl;
		}
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: [MethodImpl(MethodImplOptions.AggressiveInlining)] private static MemorySubsystem? ResolveActiveMemorySubsystem(MemorySubsystem? memSub)
	inline std::shared_ptr<MemorySubsystem> StreamEngine::ResolveActiveMemorySubsystem(std::shared_ptr<MemorySubsystem> memSub)
	{
		return (memSub != nullptr) ? memSub : Processor::Memory;
	}

	unsigned int StreamEngine::ResolveSrfResidentChunkBudget(int elementSize, unsigned long long requestedElementCount)
	{
		if (elementSize <= 0 || requestedElementCount == 0)
		{
			return 0;
		}

		unsigned long long maxResidentElements = static_cast<unsigned long long>(MaxStreamRegisterBytes) / static_cast<unsigned long long>(elementSize);
		if (maxResidentElements == 0)
		{
			return 0;
		}

		unsigned long long boundedElementCount = std::min(requestedElementCount, maxResidentElements);
		return static_cast<unsigned int>(std::min(boundedElementCount, static_cast<unsigned long long>(std::numeric_limits<unsigned int>::max())));
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: [MethodImpl(MethodImplOptions.AggressiveInlining)] private static void PrefetchToStreamRegister(ulong sourceAddr, byte elemSize, uint elemCount, YAKSys_Hybrid_CPU.Memory.MemorySubsystem? memSub = null)
	inline void StreamEngine::PrefetchToStreamRegister(unsigned long long sourceAddr, unsigned char elemSize, unsigned int elemCount, std::shared_ptr<YAKSys_Hybrid_CPU::Memory::MemorySubsystem> memSub)
	{
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: MemorySubsystem? activeMemorySubsystem = ResolveActiveMemorySubsystem(memSub);
		std::shared_ptr<MemorySubsystem> activeMemorySubsystem = ResolveActiveMemorySubsystem(memSub);
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: StreamRegisterFile? srfNullable = activeMemorySubsystem == null ? null : activeMemorySubsystem.StreamRegisters;
		std::shared_ptr<StreamRegisterFile> srfNullable = activeMemorySubsystem == nullptr ? nullptr : activeMemorySubsystem->getStreamRegisters();
		if (srfNullable == nullptr || elemCount == 0 || elemSize == 0)
		{
			return;
		}

		unsigned int residentChunkBudget = ResolveSrfResidentChunkBudget(elemSize, elemCount);
		if (residentChunkBudget == 0 || residentChunkBudget != elemCount)
		{
			return;
		}

		std::shared_ptr<StreamRegisterFile> srf = srfNullable;
		int regIdx = srf->AllocateRegister(sourceAddr, elemSize, residentChunkBudget);
		if (regIdx < 0)
		{
			return;
		}

		TryWarmNextStreamChunk(srf, regIdx, sourceAddr, elemSize, residentChunkBudget, false, activeMemorySubsystem);
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: [MethodImpl(MethodImplOptions.AggressiveInlining)] private static bool TryPrefetchToAssistStreamRegister(ulong sourceAddr, byte elemSize, uint elemCount, YAKSys_Hybrid_CPU.Core.AssistStreamRegisterPartitionPolicy partitionPolicy, YAKSys_Hybrid_CPU.Memory.MemorySubsystem? memSub = null)
	inline bool StreamEngine::TryPrefetchToAssistStreamRegister(unsigned long long sourceAddr, unsigned char elemSize, unsigned int elemCount, YAKSys_Hybrid_CPU::Core::AssistStreamRegisterPartitionPolicy partitionPolicy, std::shared_ptr<YAKSys_Hybrid_CPU::Memory::MemorySubsystem> memSub)
	{
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: MemorySubsystem? activeMemorySubsystem = ResolveActiveMemorySubsystem(memSub);
		std::shared_ptr<MemorySubsystem> activeMemorySubsystem = ResolveActiveMemorySubsystem(memSub);
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: StreamRegisterFile? srfNullable = activeMemorySubsystem == null ? null : activeMemorySubsystem.StreamRegisters;
		std::shared_ptr<StreamRegisterFile> srfNullable = activeMemorySubsystem == nullptr ? nullptr : activeMemorySubsystem->getStreamRegisters();
		if (srfNullable == nullptr || elemCount == 0 || elemSize == 0)
		{
			return false;
		}

		unsigned int residentChunkBudget = ResolveSrfResidentChunkBudget(elemSize, elemCount);
		if (residentChunkBudget == 0 || residentChunkBudget != elemCount)
		{
			return false;
		}

		std::shared_ptr<StreamRegisterFile> srf = srfNullable;
		int regIdx;
		YAKSys_Hybrid_CPU::Core::AssistStreamRegisterRejectKind rejectKind;
		if (!srf->TryAllocateAssistRegister(sourceAddr, elemSize, residentChunkBudget, partitionPolicy, regIdx, rejectKind))
		{
			srf->RecordAssistScheduleReject(rejectKind);
			return false;
		}

		return TryWarmNextStreamChunk(srf, regIdx, sourceAddr, elemSize, residentChunkBudget, true, activeMemorySubsystem);
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: [MethodImpl(MethodImplOptions.AggressiveInlining)] private static bool TryWarmNextStreamChunk(StreamRegisterFile srf, int registerIndex, ulong sourceAddr, byte elemSize, uint elemCount, bool assistOwned, YAKSys_Hybrid_CPU.Memory.MemorySubsystem? memSub = null)
	inline bool StreamEngine::TryWarmNextStreamChunk(std::shared_ptr<StreamRegisterFile> srf, int registerIndex, unsigned long long sourceAddr, unsigned char elemSize, unsigned int elemCount, bool assistOwned, std::shared_ptr<YAKSys_Hybrid_CPU::Memory::MemorySubsystem> memSub)
	{
		if (registerIndex < 0 || elemSize == 0 || elemCount == 0)
		{
			return false;
		}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: MemorySubsystem? activeMemorySubsystem = ResolveActiveMemorySubsystem(memSub);
		std::shared_ptr<MemorySubsystem> activeMemorySubsystem = ResolveActiveMemorySubsystem(memSub);

		srf->RecordWarmAttempt(assistOwned);
		if (srf->GetRegisterState(registerIndex) == StreamRegisterFile::RegisterState::Valid)
		{
			srf->RecordWarmReuse(assistOwned);
			return true;
		}

		if (assistOwned)
		{
			srf->MarkAssistLoading(registerIndex);
		}
		else
		{
			srf->MarkLoading(registerIndex);
		}

		unsigned int byteCount = static_cast<unsigned int>(std::min(static_cast<unsigned long long>(elemSize) * elemCount, static_cast<unsigned long long>(MaxStreamRegisterBytes)));
		if (byteCount == 0)
		{
			srf->InvalidateRegister(registerIndex);
			return false;
		}

		if (!IOMMU::TryWarmTranslationForAssistRange(0, sourceAddr, byteCount, IOMMUAccessPermissions::Read))
		{
			srf->RecordWarmTranslationReject();
			srf->InvalidateRegister(registerIndex);
			return false;
		}

		std::vector<unsigned char> ingress(byteCount);
		std::span<unsigned char> ingressSpan(ingress);
		if (!BurstIO::TryReadThroughActiveBackend(sourceAddr, ingressSpan.first(static_cast<int>(byteCount)), activeMemorySubsystem) || !srf->LoadRegister(registerIndex, ingressSpan.first(static_cast<int>(byteCount))))
		{
			srf->RecordWarmBackendReject();
			srf->InvalidateRegister(registerIndex);
			return false;
		}

		srf->RecordWarmSuccess(assistOwned);
		return true;
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: [MethodImpl(MethodImplOptions.AggressiveInlining)] private static void TryWarmPlanned2DChunk(ulong baseAddr, int elementSize, ulong elementCount, uint rowLength, ushort rowStride, ushort colStride, ulong startOffset, YAKSys_Hybrid_CPU.Memory.MemorySubsystem? memSub = null)
	inline void StreamEngine::TryWarmPlanned2DChunk(unsigned long long baseAddr, int elementSize, unsigned long long elementCount, unsigned int rowLength, unsigned short rowStride, unsigned short colStride, unsigned long long startOffset, std::shared_ptr<YAKSys_Hybrid_CPU::Memory::MemorySubsystem> memSub)
	{
		if (elementCount == 0 || elementSize <= 0 || rowLength == 0 || colStride != elementSize)
		{
			return;
		}

		for (auto segment : BurstPlanner::Plan2D(baseAddr + (startOffset * colStride), elementCount, elementSize, rowLength, rowStride, colStride))
		{
			if (segment.Length <= 0 || segment.Length % elementSize != 0)
			{
				continue;
			}

			unsigned int segmentElements = static_cast<unsigned int>(segment.Length / elementSize);
			if (segmentElements == 0)
			{
				continue;
			}

			PrefetchToStreamRegister(segment.Address, static_cast<unsigned char>(elementSize), segmentElements, memSub);
		}
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: [MethodImpl(MethodImplOptions.AggressiveInlining)] private static void TryWarmIndexedIngressChunk(ulong destBase, ulong indexBase, int elementSize, int indexElementSize, ushort indexStride, ulong startElement, ulong elementCount, YAKSys_Hybrid_CPU.Memory.MemorySubsystem? memSub = null)
	inline void StreamEngine::TryWarmIndexedIngressChunk(unsigned long long destBase, unsigned long long indexBase, int elementSize, int indexElementSize, unsigned short indexStride, unsigned long long startElement, unsigned long long elementCount, std::shared_ptr<YAKSys_Hybrid_CPU::Memory::MemorySubsystem> memSub)
	{
		if (elementCount == 0 || elementCount > std::numeric_limits<unsigned int>::max() || elementSize <= 0)
		{
			return;
		}

		unsigned int chunkElements = static_cast<unsigned int>(elementCount);
		PrefetchToStreamRegister(destBase + (startElement * static_cast<unsigned long long>(elementSize)), static_cast<unsigned char>(elementSize), chunkElements, memSub);

		if (indexElementSize > 0 && indexStride == indexElementSize)
		{
			PrefetchToStreamRegister(indexBase + (startElement * static_cast<unsigned long long>(indexStride)), static_cast<unsigned char>(indexElementSize), chunkElements, memSub);
		}
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: [MethodImpl(MethodImplOptions.AggressiveInlining)] public static bool ScheduleLane6AssistPrefetch(ulong sourceAddr, byte elemSize, uint elemCount, YAKSys_Hybrid_CPU.Core.AssistStreamRegisterPartitionPolicy partitionPolicy, YAKSys_Hybrid_CPU.Memory.MemorySubsystem? memSub = null)
	bool StreamEngine::ScheduleLane6AssistPrefetch(unsigned long long sourceAddr, unsigned char elemSize, unsigned int elemCount, YAKSys_Hybrid_CPU::Core::AssistStreamRegisterPartitionPolicy partitionPolicy, std::shared_ptr<YAKSys_Hybrid_CPU::Memory::MemorySubsystem> memSub)
	{
		if (elemCount == 0 || elemSize == 0)
		{
			return false;
		}

		return TryPrefetchToAssistStreamRegister(sourceAddr, elemSize, elemCount, partitionPolicy, memSub);
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: [MethodImpl(MethodImplOptions.AggressiveInlining)] public static bool ScheduleAssistPrefetch(ulong sourceAddr, byte elemSize, uint elemCount, YAKSys_Hybrid_CPU.Core.AssistStreamRegisterPartitionPolicy partitionPolicy, YAKSys_Hybrid_CPU.Memory.MemorySubsystem? memSub = null)
	inline bool StreamEngine::ScheduleAssistPrefetch(unsigned long long sourceAddr, unsigned char elemSize, unsigned int elemCount, YAKSys_Hybrid_CPU::Core::AssistStreamRegisterPartitionPolicy partitionPolicy, std::shared_ptr<YAKSys_Hybrid_CPU::Memory::MemorySubsystem> memSub)
	{
		return ScheduleLane6AssistPrefetch(sourceAddr, elemSize, elemCount, partitionPolicy, memSub);
	}
}

