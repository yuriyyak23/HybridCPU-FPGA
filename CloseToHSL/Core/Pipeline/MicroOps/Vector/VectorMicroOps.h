#pragma once

#include "../../../Execution/StreamEngine/AddressGeneration/StreamEngine.AddressGen.h"
#include "../Types/MicroOp.h"
#include "../../../../../NonRTL/Arch/Compat/HybridCPU_ISE.Arch.VLIW_Instruction.h"

#include "../../../Architecture/Common/YAKSys_Hybrid_CPU.Processor.h"
#include <string>
#include <vector>
#include <cmath>
#include <format>
#include <memory>

using namespace HybridCPU_ISE::Arch;

using namespace YAKSys_Hybrid_CPU::Arch;
using namespace YAKSys_Hybrid_CPU::Core::Registers;
using namespace YAKSys_Hybrid_CPU::Execution;

namespace YAKSys_Hybrid_CPU::Core
{
	/// <summary>
	/// Abstract base class for vector micro-operations.
	/// Provides common functionality for strip-mining, burst I/O, and async memory tracking.
	///
	/// Design goals:
	/// - Decouple vector operation logic from monolithic StreamEngine
	/// - Encapsulate strip-mining FSM (chunk processing up to VLMAX)
	/// - Integrate async memory subsystem via MemoryRequestToken
	/// - Maintain same semantics as original StreamEngine
	/// </summary>
	class VectorMicroOp : public MicroOp
	{
	private:
		VLIW_Instruction Instruction;

		/// <summary>
		/// Original VLIW instruction containing all addressing/type info
		/// </summary>
	public:
		VLIW_Instruction getInstruction() const;
		void setInstruction(VLIW_Instruction value);

		/// <summary>
		/// Execution state for multi-cycle strip-mining
		/// </summary>
	protected:
		enum class ExecutionState
		{
			NotStarted, // Initial state
			LoadingOperands, // Reading operands from memory
			Computing, // Executing operation in VectorALU
			StoringResults, // Writing results to memory
			Complete // Operation finished
		};

	protected:
		ExecutionState _state = ExecutionState::NotStarted;
		unsigned long long _elementsProcessed = 0;
		unsigned long long _totalElements = 0;

	public:
		CanonicalDecodePublicationMode getCanonicalDecodePublication() const override;

		VectorMicroOp();

		/// <summary>
		/// Initialize FSP metadata for vector operations.
		/// Memory ranges are computed based on addressing mode and stream length.
		/// Phase: Safety Tags & Certificates - Mandatory safety mask computation.
		/// </summary>
		virtual void InitializeMetadata();

		/// <summary>
		/// Re-synchronises the producer-side admission snapshot after a derived vector
		/// micro-op refines memory-range facts beyond the base vector defaults.
		/// </summary>
	protected:
		void RefreshVectorAdmissionMetadata(bool readsMemory, bool writesMemory);

		/// <summary>
		/// Get element size from data type
		/// </summary>
		int GetElementSize();

		/// <summary>
		/// Enforces fail-closed behavior for element-sized mainline vector contours.
		/// Dedicated vector carriers must not silently complete when decode materialized
		/// an unsupported element type onto an otherwise authoritative execution path.
		/// </summary>
		void ThrowIfUnsupportedElementDataType(const std::wstring &executionSurface);

		/// <summary>
		/// Enforces fail-closed behavior during authoritative vector carrier publication.
		/// Materialization must not publish zero-footprint or empty memory facts for an
		/// unsupported element DataType and defer the failure until a later execute stage.
		/// </summary>
		void ThrowIfUnsupportedElementDataTypeForMetadata(const std::wstring &metadataSurface);

		/// <summary>
		/// Enforces fail-closed behavior for zero-length mainline vector compute contours.
		/// Dedicated vector carriers must not silently complete when decode materialized
		/// an authoritative vector compute path with StreamLength == 0.
		/// </summary>
		void ThrowIfZeroLengthVectorComputeContour(const std::wstring &executionSurface);

		/// <summary>
		/// Get current VL from RVV config (clamped to VLMAX)
		/// </summary>
		unsigned long long GetVL(std::shared_ptr<Processor::CPU_Core> &core);

		/// <summary>
		/// Calculate chunk size for current strip-mine iteration
		/// </summary>
		unsigned long long GetChunkSize(unsigned long long remaining);

	public:
		std::wstring GetDescription() override;

	protected:
		std::shared_ptr<VectorMicroOp> shared_from_this()
		{
			return std::static_pointer_cast<VectorMicroOp>(MicroOp::shared_from_this());
		}
	};

	/// <summary>
	/// Vector binary operation micro-operation (two operands > one result)
	/// Examples: VADD, VSUB, VMUL, VDIV, VAND, VOR, VXOR, VMIN, VMAX, shifts
	///
	/// Execution pipeline:
	/// 1. Load operand A from memory
	/// 2. Load operand B from memory
	/// 3. Compute result via VectorALU
	/// 4. Store result to memory
	/// </summary>
}
